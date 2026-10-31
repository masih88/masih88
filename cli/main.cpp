// elcalc – command line front end for the calculation core.
//   elcalc list                         all calculators and tables
//   elcalc show <id>                    inputs, outputs and formulas
//   elcalc run <id> [key=value ...]     calculate (missing inputs use defaults)
//   elcalc table <id>                   print a reference table
//   elcalc markdown                     Markdown reference of everything (docs/BERECHNUNGEN.md)
// Options: --lang de|en  --norm de|ch
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "elc/registry.hpp"

namespace {

using namespace elc;

struct Options {
    Lang lang = Lang::De;
    NormProfile norm = NormProfile::DE;
    std::vector<std::string> args;
};

Options parse(std::span<char*> argv) {
    Options o;
    for (std::size_t i = 1; i < argv.size(); ++i) {
        const std::string_view a = argv[i];
        if (a == "--lang" && i + 1 < argv.size()) {
            o.lang = std::string_view(argv[++i]) == "en" ? Lang::En : Lang::De;
        } else if (a == "--norm" && i + 1 < argv.size()) {
            o.norm = std::string_view(argv[++i]) == "ch" ? NormProfile::CH : NormProfile::DE;
        } else {
            o.args.emplace_back(a);
        }
    }
    return o;
}

const char* severityTag(Severity s) {
    switch (s) {
        case Severity::Ok: return "[OK]  ";
        case Severity::Warning: return "[WARN]";
        case Severity::Error: return "[FEHLER/ERROR]";
        case Severity::Info: break;
    }
    return "[INFO]";
}

int list(const Options& o) {
    for (const auto& cat : categories()) {
        std::cout << "\n== " << cat.title.in(o.lang) << " ==\n";
        for (const auto& c : calculators()) {
            if (c.category == cat.id) std::cout << "  " << std::left << std::setw(28) << c.id << c.title.in(o.lang) << '\n';
        }
        for (const auto& t : tables()) {
            if (t.category == cat.id) std::cout << "  " << std::left << std::setw(28) << t.id << t.title.in(o.lang) << '\n';
        }
    }
    return 0;
}

int show(const Options& o, const std::string& id) {
    const auto* c = findCalculator(id);
    if (!c) {
        std::cerr << "Unbekannt / unknown: " << id << '\n';
        return 2;
    }
    std::cout << c->title.in(o.lang) << "\n\n" << c->description.in(o.lang) << "\n\nInputs:\n";
    for (const auto& in : c->inputs) {
        std::cout << "  " << std::left << std::setw(10) << in.id << in.label.in(o.lang);
        if (!in.unit.empty()) std::cout << " [" << in.unit << ']';
        if (in.kind == InputDef::Kind::Choice) {
            std::cout << "  {";
            for (std::size_t i = 0; i < in.options.size(); ++i) std::cout << (i ? ", " : "") << in.options[i].value;
            std::cout << "} default " << in.defaultChoice;
        } else if (std::isfinite(in.defaultValue)) {
            std::cout << "  default " << formatNumber(in.defaultValue, 6, Lang::En);
        }
        std::cout << '\n';
    }
    std::cout << "\nOutputs:\n";
    for (const auto& out : c->outputs) std::cout << "  " << std::left << std::setw(10) << out.id << out.label.in(o.lang) << '\n';
    return 0;
}

int run(const Options& o, const std::string& id) {
    const auto* c = findCalculator(id);
    if (!c) {
        std::cerr << "Unbekannt / unknown: " << id << '\n';
        return 2;
    }
    auto in = defaultInputs(*c);
    for (std::size_t i = 2; i < o.args.size(); ++i) {
        const auto& kv = o.args[i];
        const auto eq = kv.find('=');
        if (eq == std::string::npos) continue;
        const auto key = kv.substr(0, eq);
        const auto val = kv.substr(eq + 1);
        const auto def = std::find_if(c->inputs.begin(), c->inputs.end(), [&](const InputDef& d) { return d.id == key; });
        if (def == c->inputs.end()) {
            std::cerr << "Unbekannter Parameter / unknown input: " << key << '\n';
            return 2;
        }
        if (def->kind == InputDef::Kind::Choice) {
            in.setChoice(key, val);
        } else {
            in.set(key, parseNumber(val).value_or(kNaN));
        }
    }
    const auto r = evaluate(*c, in, Context{o.norm});
    std::cout << c->title.in(o.lang) << '\n';
    for (const auto& out : c->outputs) {
        std::string text;
        if (out.isText) {
            const auto it = r.texts.find(out.id);
            if (it == r.texts.end()) continue;
            text = it->second.in(o.lang);
        } else {
            const auto it = r.values.find(out.id);
            if (it == r.values.end()) continue;
            const auto unit = r.units.contains(out.id) ? r.units.at(out.id) : out.unit;
            text = formatNumber(it->second, out.decimals, o.lang) + (unit.empty() ? "" : " " + unit);
        }
        std::cout << "  " << std::left << std::setw(42) << out.label.in(o.lang) << text << '\n';
    }
    for (const auto& m : r.messages) std::cout << severityTag(m.severity) << ' ' << m.text.in(o.lang) << '\n';
    return r.hasError() ? 1 : 0;
}

int table(const Options& o, const std::string& id) {
    const auto* t = findTable(id);
    if (!t) {
        std::cerr << "Unbekannt / unknown: " << id << '\n';
        return 2;
    }
    std::cout << t->title.in(o.lang) << '\n';
    for (const auto& c : t->columns) std::cout << c.in(o.lang) << " | ";
    std::cout << '\n';
    for (const auto& row : t->rows) {
        for (const auto& cell : row) std::cout << cell.in(o.lang) << " | ";
        std::cout << '\n';
    }
    std::cout << '\n' << t->description.in(o.lang) << '\n';
    return 0;
}

std::string mdEscape(std::string s) {
    for (std::size_t p = s.find('|'); p != std::string::npos; p = s.find('|', p + 2)) s.replace(p, 1, "\\|");
    return s;
}

/// Markdown reference of all calculators and tables (docs/BERECHNUNGEN.md is generated with this).
int markdown(const Options& o) {
    const bool de = o.lang == Lang::De;
    std::cout << (de ? "# Berechnungen, Formeln & Tabellen\n\n" : "# Calculations, formulas & tables\n\n")
              << (de ? "> Automatisch erzeugt mit `elcalc markdown` aus dem Rechenkern (Version "
                     : "> Generated by `elcalc markdown` from the calculation core (version ")
              << version() << ").\n"
              << (de ? "> Alle Angaben ohne Gewähr – Normwerte vor Verwendung gegen die gültige Ausgabe prüfen.\n\n"
                     : "> No warranty – check standard values against the edition in force before use.\n\n");
    for (const auto& cat : categories()) {
        std::cout << "## " << cat.title.in(o.lang) << "\n\n";
        for (const auto& c : calculators()) {
            if (c.category != cat.id) continue;
            std::cout << "### " << c.title.in(o.lang) << "\n\n*" << c.summary.in(o.lang) << "* — `" << c.id << "`\n\n```text\n"
                      << c.description.in(o.lang) << "\n```\n\n"
                      << (de ? "**Eingaben:** " : "**Inputs:** ");
            for (std::size_t i = 0; i < c.inputs.size(); ++i) {
                const auto& in = c.inputs[i];
                std::cout << (i ? ", " : "") << in.label.in(o.lang) << (in.unit.empty() ? "" : " [" + in.unit + "]");
            }
            std::cout << "\n\n" << (de ? "**Ergebnisse:** " : "**Results:** ");
            for (std::size_t i = 0; i < c.outputs.size(); ++i) {
                const auto& out = c.outputs[i];
                std::cout << (i ? ", " : "") << out.label.in(o.lang) << (out.unit.empty() ? "" : " [" + out.unit + "]");
            }
            std::cout << "\n\n";
        }
        for (const auto& t : tables()) {
            if (t.category != cat.id) continue;
            std::cout << "### " << t.title.in(o.lang) << "\n\n";
            std::cout << '|';
            for (const auto& col : t.columns) std::cout << ' ' << mdEscape(col.in(o.lang)) << " |";
            std::cout << "\n|";
            for (std::size_t i = 0; i < t.columns.size(); ++i) std::cout << "---|";
            std::cout << '\n';
            for (const auto& row : t.rows) {
                std::cout << '|';
                for (const auto& cell : row) std::cout << ' ' << mdEscape(cell.in(o.lang)) << " |";
                std::cout << '\n';
            }
            std::cout << "\n*" << t.description.in(o.lang) << "*\n\n";
        }
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    const auto o = parse(std::span<char*>(argv, static_cast<std::size_t>(argc)));
    if (o.args.empty() || o.args[0] == "list") return list(o);
    if (o.args.size() >= 2 && o.args[0] == "show") return show(o, o.args[1]);
    if (o.args.size() >= 2 && o.args[0] == "run") return run(o, o.args[1]);
    if (o.args.size() >= 2 && o.args[0] == "table") return table(o, o.args[1]);
    if (o.args[0] == "markdown") return markdown(o);
    std::cout << "ElektroRechner " << version()
              << "\nUsage: elcalc list | show <id> | run <id> [key=value ...] | table <id> | markdown"
                 " [--lang de|en] [--norm de|ch]\n";
    return 0;
}
