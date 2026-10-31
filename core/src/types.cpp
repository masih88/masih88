#include "elc/types.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>

namespace elc {

InputDef InputDef::number(std::string id, Text label, std::string unit, double def) {
    InputDef d;
    d.id = std::move(id);
    d.label = std::move(label);
    d.unit = std::move(unit);
    d.kind = Kind::Number;
    d.defaultValue = def;
    return d;
}

InputDef InputDef::choice(std::string id, Text label, std::vector<Option> options, std::string def) {
    InputDef d;
    d.id = std::move(id);
    d.label = std::move(label);
    d.kind = Kind::Choice;
    d.options = std::move(options);
    d.defaultChoice = def.empty() && !d.options.empty() ? d.options.front().value : std::move(def);
    return d;
}

InputDef InputDef::text(std::string id, Text label, std::string def) {
    InputDef d;
    d.id = std::move(id);
    d.label = std::move(label);
    d.kind = Kind::Text;
    d.defaultChoice = std::move(def);
    d.optional = true;
    return d;
}

OutputDef OutputDef::number(std::string id, Text label, std::string unit, int decimals) {
    OutputDef o;
    o.id = std::move(id);
    o.label = std::move(label);
    o.unit = std::move(unit);
    o.decimals = decimals;
    return o;
}

OutputDef OutputDef::text(std::string id, Text label) {
    OutputDef o;
    o.id = std::move(id);
    o.label = std::move(label);
    o.decimals = 0;
    o.isText = true;
    return o;
}

double Inputs::num(std::string_view id) const {
    const auto it = numbers_.find(id);
    return it == numbers_.end() ? kNaN : it->second;
}

bool Inputs::has(std::string_view id) const { return std::isfinite(num(id)); }

std::optional<double> Inputs::maybe(std::string_view id) const {
    if (!has(id)) return std::nullopt;
    return num(id);
}

const std::string& Inputs::choice(std::string_view id) const {
    static const std::string empty;
    const auto it = choices_.find(id);
    return it == choices_.end() ? empty : it->second;
}

bool Result::hasError() const {
    return std::any_of(messages.begin(), messages.end(),
                       [](const Message& m) { return m.severity == Severity::Error; });
}

bool isVisible(const InputDef& def, const Inputs& in) {
    if (!def.visibleIf) return true;
    const auto& current = in.choice(def.visibleIf->inputId);
    const auto& allowed = def.visibleIf->values;
    return std::find(allowed.begin(), allowed.end(), current) != allowed.end();
}

Inputs defaultInputs(const Calculator& calc) {
    Inputs in;
    for (const auto& d : calc.inputs) {
        if (d.kind == InputDef::Kind::Choice || d.kind == InputDef::Kind::Text) {
            in.setChoice(d.id, d.defaultChoice);
        } else {
            in.set(d.id, d.defaultValue);
        }
    }
    return in;
}

// Locale-independent on purpose: Qt calls setlocale() on Unix/Android, which would break
// printf/strtod with a decimal comma. iostreams imbued with the classic locale are safe.
std::string formatNumber(double v, int decimals, Lang lang) {
    if (!std::isfinite(v)) return v > 0 ? "∞" : (v < 0 ? "-∞" : "–");
    std::ostringstream os;
    os.imbue(std::locale::classic());
    const double av = std::fabs(v);
    std::string out;
    if (av != 0.0 && (av >= 1e10 || av < 1e-3)) {
        os << std::setprecision(4) << v;
        out = os.str();
    } else {
        os << std::fixed << std::setprecision(std::max(0, decimals)) << v;
        out = os.str();
        // trim trailing zeros for compact text
        if (out.find('.') != std::string::npos) {
            while (!out.empty() && out.back() == '0') out.pop_back();
            if (!out.empty() && out.back() == '.') out.pop_back();
        }
        if (out == "-0") out = "0";
    }
    if (lang == Lang::De) std::replace(out.begin(), out.end(), '.', ',');
    return out;
}

std::optional<double> parseNumber(std::string_view text) {
    std::string s(text);
    std::replace(s.begin(), s.end(), ',', '.');
    s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c) != 0; }), s.end());
    if (s.empty()) return std::nullopt;
    std::istringstream is(s);
    is.imbue(std::locale::classic());
    double v = 0.0;
    is >> v;
    if (is.fail() || !is.eof() || !std::isfinite(v)) return std::nullopt;
    return v;
}

Text formatNumber(double v, int decimals) {
    return Text{formatNumber(v, decimals, Lang::De), formatNumber(v, decimals, Lang::En)};
}

namespace {

Text rangeMessage(const InputDef& d) {
    const auto lo = formatNumber(d.min, 4);
    const auto hi = formatNumber(d.max, 4);
    const bool hasLo = std::isfinite(d.min);
    const bool hasHi = std::isfinite(d.max);
    if (hasLo && hasHi) {
        return {"„" + d.label.de + "“ muss zwischen " + lo.de + " und " + hi.de + " liegen.",
                "\"" + d.label.en + "\" must be between " + lo.en + " and " + hi.en + "."};
    }
    if (hasLo) {
        const char* opDe = d.minExclusive ? " muss größer als " : " muss mindestens ";
        const char* opEn = d.minExclusive ? " must be greater than " : " must be at least ";
        return {"„" + d.label.de + "“" + opDe + lo.de + " sein.", "\"" + d.label.en + "\"" + opEn + lo.en + "."};
    }
    return {"„" + d.label.de + "“ darf höchstens " + hi.de + " sein.",
            "\"" + d.label.en + "\" must be at most " + hi.en + "."};
}

}  // namespace

Result normalizeAndValidate(const std::vector<InputDef>& defs, Inputs& in) {
    Result validation;
    // 1st pass: choices (visibility of numeric inputs depends on them)
    for (const auto& d : defs) {
        if (d.kind != InputDef::Kind::Choice) continue;
        const auto& v = in.choice(d.id);
        const bool known = std::any_of(d.options.begin(), d.options.end(),
                                       [&](const Option& o) { return o.value == v; });
        if (!known) in.setChoice(d.id, d.defaultChoice);
    }
    // 2nd pass: visible numbers
    for (const auto& d : defs) {
        if (d.kind != InputDef::Kind::Number || !isVisible(d, in)) continue;
        const double v = in.num(d.id);
        if (!std::isfinite(v)) {
            if (!d.optional) {
                validation.error({"Bitte „" + d.label.de + "“ eingeben.", "Please enter \"" + d.label.en + "\"."});
            }
            continue;
        }
        const bool tooLow = v < d.min || (d.minExclusive && v <= d.min);
        if (tooLow || v > d.max) validation.error(rangeMessage(d));
    }
    return validation;
}

Result guardedCompute(const std::function<Result()>& fn) {
    try {
        return fn();
    } catch (const CalcError& e) {
        Result r;
        r.error(e.text());
        return r;
    } catch (const std::exception& e) {
        Result r;
        r.error({std::string("Berechnung nicht möglich: ") + e.what(), std::string("Calculation failed: ") + e.what()});
        return r;
    }
}

Result evaluate(const Calculator& calc, const Inputs& raw, const Context& ctx) {
    Inputs in = raw;
    Result validation = normalizeAndValidate(calc.inputs, in);
    if (validation.hasError()) return validation;

    try {
        return calc.compute(in, ctx);
    } catch (const CalcError& e) {
        Result r;
        r.error(e.text());
        return r;
    } catch (const std::exception& e) {
        Result r;
        r.error({std::string("Berechnung nicht möglich: ") + e.what(), std::string("Calculation failed: ") + e.what()});
        return r;
    }
}

}  // namespace elc
