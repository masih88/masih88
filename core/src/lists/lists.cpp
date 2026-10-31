// Generic project-list machinery: evaluation, CSV export, table import.
#include "elc/lists.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <optional>
#include <sstream>

#include "elc/registry.hpp"

namespace elc {

namespace {

Inputs defaults(const std::vector<InputDef>& defs) {
    Inputs in;
    for (const auto& d : defs) {
        if (d.kind == InputDef::Kind::Number) {
            in.set(d.id, d.defaultValue);
        } else {
            in.setChoice(d.id, d.defaultChoice);
        }
    }
    return in;
}

std::string csvField(std::string s, char sep) {
    if (s.find_first_of(std::string{sep, '"', '\n', '\r'}) == std::string::npos) return s;
    std::string out = "\"";
    for (const char c : s) {
        if (c == '"') out += '"';
        out += c;
    }
    return out + "\"";
}

std::string optionLabel(const InputDef& d, const std::string& value, Lang lang) {
    for (const auto& o : d.options) {
        if (o.value == value) return o.label.in(lang);
    }
    return value;
}

std::string inputText(const InputDef& d, const Inputs& in, Lang lang) {
    switch (d.kind) {
        case InputDef::Kind::Number: return in.has(d.id) ? formatNumber(in.num(d.id), 6, lang) : std::string{};
        case InputDef::Kind::Choice: return optionLabel(d, in.choice(d.id), lang);
        case InputDef::Kind::Text: return in.text(d.id);
    }
    return {};
}

std::string statusText(const Result& r, Lang lang) {
    bool warn = false;
    for (const auto& m : r.messages) {
        if (m.severity == Severity::Error) return lang == Lang::De ? "n. i. O." : "not OK";
        if (m.severity == Severity::Warning) warn = true;
    }
    return warn ? (lang == Lang::De ? "Hinweis" : "Warning") : (lang == Lang::De ? "i. O." : "OK");
}

std::string outputText(const OutputDef& o, const Result& r, Lang lang) {
    if (o.isText) {
        const auto it = r.texts.find(o.id);
        return it == r.texts.end() ? std::string{} : it->second.in(lang);
    }
    const auto it = r.values.find(o.id);
    return it == r.values.end() ? std::string{} : formatNumber(it->second, o.decimals, lang);
}

std::string unitOf(const OutputDef& o, const Result& r) {
    const auto u = r.units.find(o.id);
    return u == r.units.end() ? o.unit : u->second;
}

// ---- import helpers -------------------------------------------------------------------------

std::string normalize(std::string_view s) {
    std::string out;
    bool space = false;
    for (const unsigned char c : s) {
        if (std::isspace(c) || c == '\n') {
            space = !out.empty();
            continue;
        }
        if (space) out += ' ';
        space = false;
        out += static_cast<char>(std::tolower(c));
    }
    return out;
}

std::vector<std::vector<std::string>> parseDelimited(std::string_view text) {
    // delimiter from the first line
    const auto firstEnd = text.find('\n');
    const std::string_view first = text.substr(0, firstEnd);
    char delim = ',';
    if (first.find('\t') != std::string_view::npos) {
        delim = '\t';
    } else if (first.find(';') != std::string_view::npos) {
        delim = ';';
    }
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> row;
    std::string field;
    bool quoted = false;
    for (std::size_t i = 0; i < text.size(); ++i) {
        const char c = text[i];
        if (quoted) {
            if (c == '"') {
                if (i + 1 < text.size() && text[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    quoted = false;
                }
            } else {
                field += c;
            }
            continue;
        }
        if (c == '"' && field.empty()) {
            quoted = true;
        } else if (c == delim) {
            row.push_back(field);
            field.clear();
        } else if (c == '\n' || c == '\r') {
            if (c == '\r' && i + 1 < text.size() && text[i + 1] == '\n') ++i;
            row.push_back(field);
            field.clear();
            rows.push_back(std::move(row));
            row.clear();
        } else {
            field += c;
        }
    }
    if (!field.empty() || !row.empty()) {
        row.push_back(field);
        rows.push_back(std::move(row));
    }
    return rows;
}

const InputDef* matchColumn(const ListDef& def, const std::string& header) {
    const auto full = normalize(header);
    if (full.empty()) return nullptr;
    // our own export writes "Label [unit]": also try the header without the unit suffix
    std::string bare = full;
    if (const auto pos = bare.rfind(" ["); pos != std::string::npos && bare.back() == ']') bare.erase(pos);
    for (const auto& h : {full, bare}) {
        for (const auto& d : def.rowInputs) {
            if (normalize(d.id) == h || normalize(d.label.de) == h || normalize(d.label.en) == h) return &d;
            for (const auto& a : d.aliases) {
                if (normalize(a) == h) return &d;
            }
        }
    }
    return nullptr;
}

std::optional<std::string> matchOption(const InputDef& d, const std::string& cell) {
    const auto c = normalize(cell);
    if (c.empty()) return std::nullopt;
    std::string nospace;
    for (const char ch : c) {
        if (ch != ' ') nospace += ch;
    }
    // numeric cells: compare canonical number text ("2,5" → "2.5")
    if (const auto n = parseNumber(cell)) {
        const auto canon = formatNumber(*n, 6, Lang::En);
        for (const auto& o : d.options) {
            if (o.value == canon) return o.value;
        }
    }
    for (const auto& o : d.options) {
        if (normalize(o.value) == c || normalize(o.value) == nospace || normalize(o.label.de) == c || normalize(o.label.en) == c) {
            return o.value;
        }
    }
    for (const auto& o : d.options) {
        if (normalize(o.value).rfind(nospace, 0) == 0) return o.value;  // "24" → "24ac"
    }
    for (const auto& o : d.options) {
        if (normalize(o.label.de).find(c) != std::string::npos || normalize(o.label.en).find(c) != std::string::npos) {
            return o.value;
        }
    }
    return std::nullopt;
}

}  // namespace

Inputs defaultRow(const ListDef& def) { return defaults(def.rowInputs); }
Inputs defaultSettings(const ListDef& def) { return defaults(def.settingsInputs); }

Result evaluateRow(const ListDef& def, const Inputs& row, const Inputs& settings, const Context& ctx) {
    Inputs in = row;
    Inputs cfg = settings;
    Result validation = normalizeAndValidate(def.rowInputs, in);
    const Result cfgValidation = normalizeAndValidate(def.settingsInputs, cfg);
    for (const auto& m : cfgValidation.messages) validation.messages.push_back(m);
    if (validation.hasError()) return validation;
    return guardedCompute([&] { return def.computeRow(in, cfg, ctx); });
}

ListResult evaluateList(const ListDef& def, const std::vector<Inputs>& rows, const Inputs& settings, const Context& ctx) {
    ListResult lr;
    std::vector<Inputs> normalized;
    Inputs cfg = settings;
    (void)normalizeAndValidate(def.settingsInputs, cfg);
    for (const auto& row : rows) {
        Inputs in = row;
        (void)normalizeAndValidate(def.rowInputs, in);
        normalized.push_back(in);
        lr.rows.push_back(evaluateRow(def, row, settings, ctx));
    }
    lr.totals = guardedCompute([&] { return def.summarize(normalized, lr.rows, cfg, ctx); });
    return lr;
}

std::string exportCsv(const ListDef& def, const std::vector<Inputs>& rows, const Inputs& settings, const Context& ctx,
                      Lang lang, bool detail, char sep) {
    std::ostringstream os;
    const auto line = [&](const std::vector<std::string>& cells) {
        for (std::size_t i = 0; i < cells.size(); ++i) {
            if (i) os << sep;
            os << csvField(cells[i], sep);
        }
        os << "\r\n";
    };
    if (detail && def.expand) {
        Inputs cfg = settings;
        (void)normalizeAndValidate(def.settingsInputs, cfg);
        std::vector<Inputs> normalized;
        for (const auto& r : rows) {
            Inputs in = r;
            (void)normalizeAndValidate(def.rowInputs, in);
            normalized.push_back(in);
        }
        std::vector<std::string> head;
        for (const auto& c : def.expandColumns) head.push_back(c.in(lang));
        line(head);
        for (const auto& r : def.expand(normalized, cfg, lang)) line(r);
        return os.str();
    }

    const auto result = evaluateList(def, rows, settings, ctx);
    std::vector<std::string> head;
    for (const auto& d : def.rowInputs) head.push_back(d.label.in(lang) + (d.unit.empty() ? "" : " [" + d.unit + "]"));
    for (const auto& o : def.rowOutputs) head.push_back(o.label.in(lang) + (o.unit.empty() ? "" : " [" + o.unit + "]"));
    head.emplace_back(lang == Lang::De ? "Status" : "Status");
    line(head);
    for (std::size_t i = 0; i < rows.size(); ++i) {
        Inputs in = rows[i];
        (void)normalizeAndValidate(def.rowInputs, in);
        std::vector<std::string> cells;
        for (const auto& d : def.rowInputs) cells.push_back(inputText(d, in, lang));
        for (const auto& o : def.rowOutputs) cells.push_back(outputText(o, result.rows[i], lang));
        cells.push_back(statusText(result.rows[i], lang));
        line(cells);
    }
    os << "\r\n";
    line({lang == Lang::De ? "Summen" : "Totals"});
    for (const auto& o : def.totalOutputs) {
        const auto v = outputText(o, result.totals, lang);
        if (!v.empty()) line({o.label.in(lang), v, unitOf(o, result.totals)});
    }
    return os.str();
}

std::vector<Inputs> exampleRows(const ListDef& def) {
    if (def.example.empty()) return {defaultRow(def)};
    return importTable(def, def.example);
}

std::vector<Inputs> importTable(const ListDef& def, std::string_view text) {
    std::vector<Inputs> out;
    const auto table = parseDelimited(text);
    if (table.size() < 2) return out;

    // header may not be the first line (Excel copies often include title rows): take the line with most matches
    std::size_t headerRow = 0;
    std::size_t best = 0;
    for (std::size_t r = 0; r < std::min<std::size_t>(table.size(), 10); ++r) {
        std::size_t hits = 0;
        for (const auto& cell : table[r]) hits += matchColumn(def, cell) ? 1 : 0;
        if (hits > best) {
            best = hits;
            headerRow = r;
        }
    }
    if (best == 0) return out;
    std::vector<const InputDef*> columns;
    for (const auto& cell : table[headerRow]) columns.push_back(matchColumn(def, cell));
    // the first row input is the row's name: if the table has that column, rows without a name are skipped
    std::optional<std::size_t> keyColumn;
    for (std::size_t c = 0; c < columns.size(); ++c) {
        if (columns[c] && columns[c]->id == def.rowInputs.front().id) {
            keyColumn = c;
            break;
        }
    }

    for (std::size_t r = headerRow + 1; r < table.size(); ++r) {
        const auto& cells = table[r];
        const bool empty = std::all_of(cells.begin(), cells.end(), [](const std::string& c) { return normalize(c).empty(); });
        if (empty) break;  // end of data (e.g. totals block of our own export)
        if (keyColumn && (*keyColumn >= cells.size() || normalize(cells[*keyColumn]).empty())) {
            continue;  // prepared template rows without a name (common in Excel sheets)
        }
        Inputs in = defaultRow(def);
        bool any = false;
        for (std::size_t c = 0; c < cells.size() && c < columns.size(); ++c) {
            const InputDef* d = columns[c];
            if (!d) continue;
            const auto& cell = cells[c];
            switch (d->kind) {
                case InputDef::Kind::Number: {
                    const auto v = parseNumber(cell);
                    if (v) {
                        in.set(d->id, *v);
                        any = true;
                    } else if (normalize(cell).empty() && d->optional) {
                        in.set(d->id, kNaN);
                    }
                    break;
                }
                case InputDef::Kind::Choice:
                    if (const auto v = matchOption(*d, cell)) {
                        in.setChoice(d->id, *v);
                        any = true;
                    }
                    break;
                case InputDef::Kind::Text:
                    if (!normalize(cell).empty()) {
                        in.setChoice(d->id, cell);
                        any = true;
                    }
                    break;
            }
        }
        if (any) out.push_back(std::move(in));
    }
    return out;
}

}  // namespace elc
