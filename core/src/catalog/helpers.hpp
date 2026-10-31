#pragma once
// Small helpers to keep the calculator definitions short and readable.

#include <string>
#include <vector>

#include "elc/formulas/cables.hpp"
#include "elc/formulas/common.hpp"
#include "elc/types.hpp"

namespace elc::catalog {

using Num = InputDef;  // InputDef::number(...)
using Out = OutputDef;

void addBasics(std::vector<Calculator>& out);
void addCables(std::vector<Calculator>& out);
void addMotors(std::vector<Calculator>& out);
void addIndustry(std::vector<Calculator>& out);
void addAutomation(std::vector<Calculator>& out);
void addElectronics(std::vector<Calculator>& out);
void addConverters(std::vector<Calculator>& out);
void addTables(std::vector<Table>& out);

inline std::vector<Option> systemOptions(bool withDc = true) {
    std::vector<Option> o;
    if (withDc) o.push_back({"dc", {"Gleichstrom (DC)", "Direct current (DC)"}});
    o.push_back({"ac1", {"Wechselstrom 1~", "Single-phase AC"}});
    o.push_back({"ac3", {"Drehstrom 3~", "Three-phase AC"}});
    return o;
}

inline System parseSystem(const std::string& v) {
    if (v == "dc") return System::DC;
    if (v == "ac3") return System::AC3;
    return System::AC1;
}

inline std::vector<Option> materialOptions() {
    return {{"cu", {"Kupfer (Cu)", "Copper (Cu)"}}, {"al", {"Aluminium (Al)", "Aluminium (Al)"}}};
}

inline cables::Material parseMaterial(const std::string& v) {
    return v == "al" ? cables::Material::Al : cables::Material::Cu;
}

/// Choice options "1.5", "2.5", … for the given cross-sections
template <FloatSeries R>
std::vector<Option> areaOptions(const R& areas) {
    std::vector<Option> o;
    for (const double a : areas) {
        const auto t = formatNumber(a, 2);
        o.push_back({formatNumber(a, 2, Lang::En), {t.de + " mm²", t.en + " mm²"}});
    }
    return o;
}

inline double parseDouble(const std::string& s) { return parseNumber(s).value_or(kNaN); }

/// "16,3 %" style text for both languages
inline Text numText(double v, int decimals, const std::string& unit = {}) {
    auto t = formatNumber(v, decimals);
    if (!unit.empty()) {
        t.de += " " + unit;
        t.en += " " + unit;
    }
    return t;
}

/// Concatenate translated fragments: cat({"a", "b"}, numText(...), ...)
template <typename... T>
Text cat(const T&... parts) {
    Text t{"", ""};
    ((t.de += Text(parts).de, t.en += Text(parts).en), ...);
    return t;
}

inline const InputDef cosPhiInput(double def = 0.9) {
    return InputDef::number("cosphi", {"Leistungsfaktor cos φ", "Power factor cos φ"}, "", def)
        .rangeExclusiveMin(0.0, 1.0)
        .showIf("sys", {"ac1", "ac3"});
}

}  // namespace elc::catalog
