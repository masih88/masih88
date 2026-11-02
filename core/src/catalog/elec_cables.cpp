// Area "Elektrotechnik" (v1.7): mineral-insulated cables, busbars, cable R/X/Z, temperature, losses,
// neutral current with harmonics, voltage drop and sizing for distributed loads.
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/electrical.hpp"

namespace elc::catalog {

using namespace elc::electrical;

namespace {

constexpr const char* kCat = "electrical";

std::string areaText(double a) { return formatNumber(a, 2, Lang::De); }

// ------------------------------------------------------------------ mineral-insulated cables

MiConfig parseMiConfig(const std::string& v) {
    if (v == "3c") return MiConfig::ThreeCore3ph;
    if (v == "4c") return MiConfig::FourCore3ph;
    if (v == "1c1") return MiConfig::SingleCore1ph;
    if (v == "1c3") return MiConfig::SingleCore3ph;
    if (v == "7c") return MiConfig::SevenCore;
    if (v == "12c") return MiConfig::TwelveCore;
    if (v == "19c") return MiConfig::NineteenCore;
    return MiConfig::TwoCore;
}

Calculator ampacityMi() {
    Calculator c;
    c.id = "ampacity_mi";
    c.category = kCat;
    c.title = {"Strombelastbarkeit von blanken und mineralisolierten Leitern", "Current rating of bare and mineral-insulated cables"};
    c.summary = {"MI-Kabel (Cu), Mantel 70 °C, Verlegeart C", "MI cables (Cu), sheath 70 °C, method C"};
    c.description = {
        "Mineralisolierte Kupferkabel (MICC) nach IEC 60702-1, Mantel PVC-umhüllt oder blank berührbar (Manteltemperatur 70 °C), "
        "direkt auf der Wand (Verlegeart C), Umgebung 30 °C. Werte nach BS 7671 Tab. 4G1A = IEC 60364-5-52 Tab. B.52.6 "
        "(Herstellerdatenblatt). 1~ = 2 belastete Leiter, 3~ = 3 belastete Leiter.\n"
        "Umgebungstemperatur: f = √((70 − θ_a)/(70 − 30)) als Näherung von Tab. B.52.14 – für blanke, nicht berührbare Kabel "
        "(Mantel 105 °C, Tab. B.52.7) und Häufung die Norm verwenden. Gegen die gültige Ausgabe prüfen.",
        "Mineral-insulated copper cables (MICC) per IEC 60702-1, sheath PVC covered or bare exposed to touch (sheath temperature "
        "70 °C), clipped direct (method C), 30 °C ambient. Values per BS 7671 Tab. 4G1A = IEC 60364-5-52 Tab. B.52.6 "
        "(manufacturer data sheet). 1~ = 2 loaded conductors, 3~ = 3 loaded conductors.\n"
        "Ambient: f = √((70 − θ_a)/(70 − 30)) as an approximation of Tab. B.52.14 – for bare cables not exposed to touch "
        "(sheath 105 °C, Tab. B.52.7) and grouping use the standard. Check against the edition in force."};
    std::vector<double> areas{1.0, 1.5, 2.5, 4, 6, 10, 16, 25, 35, 50, 70, 95, 120, 150, 185, 240};
    c.inputs = {
        Num::choice("duty", {"Ausführung", "Grade"},
                    {{"light", {"Leicht, 500 V (L)", "Light duty, 500 V (L)"}}, {"heavy", {"Schwer, 750 V (H)", "Heavy duty, 750 V (H)"}}}, "heavy"),
        Num::choice("cfg", {"Aderzahl / Belastung", "Cores / loading"},
                    {{"2c", {"2 Adern, 1~ oder DC", "2 cores, 1~ or DC"}},
                     {"3c", {"3 Adern, 3~", "3 cores, 3~"}},
                     {"4c", {"4 Adern, 3~", "4 cores, 3~"}},
                     {"1c1", {"Einleiter, 1~ (2 Kabel)", "Single core, 1~ (2 cables)"}},
                     {"1c3", {"Einleiter, 3~ (3 Kabel)", "Single core, 3~ (3 cables)"}},
                     {"7c", {"7 Adern (alle belastet)", "7 cores (all loaded)"}},
                     {"12c", {"12 Adern (alle belastet)", "12 cores (all loaded)"}},
                     {"19c", {"19 Adern (alle belastet)", "19 cores (all loaded)"}}},
                    "3c"),
        Num::choice("A", {"Querschnitt", "Cross-section"}, areaOptions(areas), "2.5"),
        Num::number("Ta", {"Umgebungstemperatur θ_a", "Ambient temperature θ_a"}, "°C", 30).range(-20, 65),
        Num::number("Ib", {"Betriebsstrom I_b (optional)", "Operating current I_b (optional)"}, "A", 20).positive().opt(),
    };
    c.outputs = {Out::number("Iz", {"Belastbarkeit I_z", "Current rating I_z"}, "A", 1).primary(),
                 Out::number("Iz0", {"Tabellenwert (30 °C)", "Table value (30 °C)"}, "A", 1),
                 Out::number("f", {"Faktor Umgebungstemperatur", "Ambient temperature factor"}, "", 3),
                 Out::text("ref", {"Kabelbezeichnung", "Cable reference"})};
    c.keywords = {"mineralisoliert", "micc", "pyro", "mi kabel", "feuerfest", "mineral insulated", "bare"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto duty = in.choice("duty") == "light" ? MiDuty::Light500V : MiDuty::Heavy750V;
        const auto cfg = parseMiConfig(in.choice("cfg"));
        const double A = parseDouble(in.choice("A"));
        const auto iz0 = miAmpacity(duty, cfg, A);
        if (!iz0) {
            Text list{"", ""};
            for (const double a : miAreas(duty, cfg)) {
                list.de += (list.de.empty() ? "" : ", ") + areaText(a);
                list.en += (list.en.empty() ? "" : ", ") + formatNumber(a, 2, Lang::En);
            }
            if (list.de.empty()) throw CalcError({"Diese Aderzahl gibt es in dieser Ausführung nicht.", "This core count does not exist in this grade."});
            throw CalcError(cat(Text{"Querschnitt nicht tabelliert. Verfügbar: ", "Cross-section not tabulated. Available: "}, list, " mm²"));
        }
        const double ta = in.num("Ta");
        const double f = std::sqrt(std::max(0.0, (70.0 - ta) / 40.0));
        r.set("Iz0", *iz0);
        r.set("f", f);
        r.set("Iz", *iz0 * f);
        const std::string cores = in.choice("cfg") == "1c1" || in.choice("cfg") == "1c3" ? "1" : in.choice("cfg").substr(0, in.choice("cfg").size() - 1);
        const std::string ref = cores + (duty == MiDuty::Light500V ? "L" : "H") + formatNumber(A, 1, Lang::En);
        r.setText("ref", {ref, ref});
        if (const auto ib = in.maybe("Ib")) {
            if (*ib <= *iz0 * f) r.ok({"I_b ≤ I_z", "I_b ≤ I_z"});
            else r.error({"I_b > I_z: größeren Querschnitt wählen.", "I_b > I_z: choose a larger cross-section."});
        }
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ busbars

Calculator busbarAmpacity() {
    Calculator c;
    c.id = "busbar_ampacity";
    c.category = kCat;
    c.title = {"Strombelastbarkeit von Sammelschienen", "Current rating of busbars"};
    c.summary = {"E-Cu-Flachschienen nach DIN 43671, 1 oder 2 Schienen je Leiter", "E-Cu flat busbars per DIN 43671, 1 or 2 bars per conductor"};
    c.description = {
        "Dauerströme für E-Cu-Schienen mit Rechteckquerschnitt in Innenanlagen bei 25 °C Lufttemperatur um den Leiter und "
        "max. 65 °C Schienentemperatur, Wechselstrom, Schienen hochkant (Striebel & John, Kap. 10 Sammelschienensysteme, nach "
        "DIN 43671).\nAndere Umgebungstemperatur als Näherung: I = I₂₅ · √((65 − θ_a)/(65 − 25)) (Wärmeabgabe ∝ Übertemperatur). "
        "Für Auslegung die Korrekturfaktoren der DIN 43671 bzw. Herstellerangaben verwenden (Anordnung, Anstrich, Höhe, Frequenz).",
        "Continuous currents for E-Cu bars with rectangular cross-section, indoors, 25 °C air around the conductor and max. "
        "65 °C bar temperature, AC, bars on edge (Striebel & John catalogue ch. 10, after DIN 43671).\n"
        "Other ambient temperature as an approximation: I = I₂₅ · √((65 − θ_a)/(65 − 25)) (heat dissipation ∝ temperature rise). "
        "For design use the correction factors of DIN 43671 or manufacturer data (arrangement, paint, altitude, frequency)."};
    std::vector<Option> sizes;
    for (const auto& b : busbars()) {
        const std::string id = formatNumber(b.width, 0, Lang::En) + "x" + formatNumber(b.thickness, 0, Lang::En);
        const std::string label = formatNumber(b.width, 0, Lang::En) + " × " + formatNumber(b.thickness, 0, Lang::En) + " mm";
        sizes.push_back({id, {label, label}});
    }
    c.inputs = {Num::choice("size", {"Schiene B × D", "Bar W × T"}, sizes, "30x10"),
                Num::choice("n", {"Schienen je Leiter", "Bars per conductor"}, {{"1", "1"}, {"2", "2"}}, "1"),
                Num::number("Ta", {"Umgebungstemperatur θ_a", "Ambient temperature θ_a"}, "°C", 35).range(-20, 60),
                Num::number("Ib", {"Betriebsstrom I_b (optional)", "Operating current I_b (optional)"}, "A", 500).positive().opt()};
    c.outputs = {Out::number("I", {"Dauerstrom bei θ_a", "Continuous current at θ_a"}, "A", 0).primary(),
                 Out::number("I25", {"Tabellenwert (25 °C)", "Table value (25 °C)"}, "A", 0),
                 Out::number("A", {"Querschnitt", "Cross-section"}, "mm²", 0),
                 Out::number("J", {"Stromdichte", "Current density"}, "A/mm²", 2),
                 Out::number("kg", {"Gewicht je Leiter", "Weight per conductor"}, "kg/m", 3)};
    c.keywords = {"sammelschiene", "kupferschiene", "stromschiene", "busbar", "din 43671"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const Busbar* bar = nullptr;
        for (const auto& b : busbars()) {
            const std::string id = formatNumber(b.width, 0, Lang::En) + "x" + formatNumber(b.thickness, 0, Lang::En);
            if (id == in.choice("size")) bar = &b;
        }
        if (bar == nullptr) bar = &busbars().front();
        const int n = in.choice("n") == "2" ? 2 : 1;
        const double i25 = n == 2 ? bar->i2 : bar->i1;
        if (i25 <= 0)
            throw CalcError({"Für 2 Schienen liegen nur Werte für 20 × 10, 30 × 10 und 40 × 10 mm vor.",
                             "For 2 bars values exist only for 20 × 10, 30 × 10 and 40 × 10 mm."});
        const double f = std::sqrt(std::max(0.0, (65.0 - in.num("Ta")) / 40.0));
        const double I = i25 * f;
        r.set("I25", i25);
        r.set("I", I);
        r.set("A", bar->area * n);
        r.set("J", I / (bar->area * n));
        r.set("kg", bar->kgPerM * n);
        if (const auto ib = in.maybe("Ib")) {
            if (*ib <= I) r.ok({"I_b ≤ Dauerstrom", "I_b ≤ continuous current"});
            else r.error({"I_b > Dauerstrom: größere Schiene oder mehrere Schienen wählen.", "I_b > continuous current: choose a larger bar or more bars."});
        }
        if (std::fabs(in.num("Ta") - 25.0) > 0.1)
            r.info({"Temperaturumrechnung ist eine Näherung – gegen DIN 43671 prüfen.", "Temperature correction is an approximation – check against DIN 43671."});
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ cable R, X, Z

Calculator cableRxz() {
    Calculator c;
    c.id = "cable_rxz";
    c.category = kCat;
    c.title = {"Kabelwiderstand, Reaktanz und Impedanz", "Cable resistance, reactance and impedance"};
    c.summary = {"R(θ), X und Z je Leiter und Schleife", "R(θ), X and Z per conductor and loop"};
    c.description = {
        "R₂₀ = ρ₂₀ · L / A    R(θ) = R₂₀ · (1 + α₂₀ · (θ − 20 °C))    X = X' · L    Z = √(R² + X²)\n"
        "Cu: ρ₂₀ = 0,017241 Ω·mm²/m, α = 0,00393 1/K; Al: 0,028264 Ω·mm²/m, 0,00403 1/K. X' ≈ 0,08 mΩ/m (mehradrige Kabel), "
        "ca. 0,1–0,15 mΩ/m für Einleiter mit Abstand. Schleife 1~/DC = Hin- und Rückleiter; 3~ = je Strang.\n"
        "Gleichstromwiderstand ohne Skineffekt (bei 50 Hz ab ca. 150 mm² merklich).",
        "R₂₀ = ρ₂₀ · L / A    R(θ) = R₂₀ · (1 + α₂₀ · (θ − 20 °C))    X = X' · L    Z = √(R² + X²)\n"
        "Cu: ρ₂₀ = 0.017241 Ω·mm²/m, α = 0.00393 1/K; Al: 0.028264 Ω·mm²/m, 0.00403 1/K. X' ≈ 0.08 mΩ/m (multi-core cables), "
        "approx. 0.1–0.15 mΩ/m for spaced single cores. Loop 1~/DC = go and return conductor; 3~ = per phase.\n"
        "DC resistance without skin effect (noticeable at 50 Hz from approx. 150 mm²)."};
    c.inputs = {Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac3"),
                Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
                Num::choice("A", {"Querschnitt", "Cross-section"}, areaOptions(cables::kStandardAreas), "16"),
                Num::number("L", {"Länge L (einfach)", "Length L (one way)"}, "m", 100).positive(),
                Num::number("theta", {"Leitertemperatur θ", "Conductor temperature θ"}, "°C", 70).range(-40, 250),
                Num::number("x", {"Reaktanzbelag X'", "Reactance per length X'"}, "mΩ/m", 0.08).nonNegative()};
    c.outputs = {Out::number("R20", {"R₂₀ je Leiter", "R₂₀ per conductor"}, "mΩ", 2),
                 Out::number("R", {"R(θ) je Leiter", "R(θ) per conductor"}, "mΩ", 2),
                 Out::number("X", {"X je Leiter", "X per conductor"}, "mΩ", 2),
                 Out::number("Z", {"Z je Leiter", "Z per conductor"}, "mΩ", 2).primary(),
                 Out::number("Zloop", {"Z Schleife (Hin + Rück)", "Z loop (go + return)"}, "mΩ", 2),
                 Out::number("Rkm", {"R(θ) je km", "R(θ) per km"}, "Ω/km", 4)};
    c.keywords = {"kabelwiderstand", "reaktanz", "impedanz", "leitungswiderstand", "cable impedance"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto mat = parseMaterial(in.choice("mat"));
        const double A = parseDouble(in.choice("A"));
        const double L = in.num("L");
        const auto sys = parseSystem(in.choice("sys"));
        const double R20 = cables::resistivity20(mat) * L / A;
        const double R = cables::resistivity(mat, in.num("theta")) * L / A;
        const double X = sys == System::DC ? 0.0 : in.num("x") * 1e-3 * L;
        r.set("R20", R20 * 1e3);
        r.set("R", R * 1e3);
        r.set("X", X * 1e3);
        r.set("Z", std::hypot(R, X) * 1e3);
        r.set("Zloop", 2.0 * std::hypot(R, X) * 1e3);
        r.set("Rkm", R / L * 1e3);
        return r;
    };
    return c;
}

Calculator cableTemperatureCalc() {
    Calculator c;
    c.id = "cable_temperature";
    c.category = kCat;
    c.title = {"Berechnung der Kabeltemperatur", "Cable temperature"};
    c.summary = {"Leitertemperatur aus Auslastung I_b/I_z", "Conductor temperature from loading I_b/I_z"};
    c.description = {"θ = θ_a + (θ_z − θ_a) · (I_b / I_z)²\nθ_z = zulässige Leitertemperatur (PVC 70 °C, VPE/EPR 90 °C), "
                     "I_z = Belastbarkeit bei der tatsächlichen Umgebungstemperatur θ_a und Häufung. Verluste ∝ I² "
                     "(IEC 60287, ohne Änderung von R mit der Temperatur).",
                     "θ = θ_a + (θ_z − θ_a) · (I_b / I_z)²\nθ_z = permissible conductor temperature (PVC 70 °C, XLPE/EPR 90 °C), "
                     "I_z = current rating at the actual ambient θ_a and grouping. Losses ∝ I² "
                     "(IEC 60287, ignoring the change of R with temperature)."};
    c.inputs = {Num::number("Ib", {"Betriebsstrom I_b", "Operating current I_b"}, "A", 20).nonNegative(),
                Num::number("Iz", {"Belastbarkeit I_z", "Current rating I_z"}, "A", 27).positive(),
                Num::number("Ta", {"Umgebungstemperatur θ_a", "Ambient temperature θ_a"}, "°C", 30).range(-30, 85),
                Num::choice("ins", {"Isolierung", "Insulation"},
                            {{"pvc", {"PVC (70 °C)", "PVC (70 °C)"}}, {"xlpe", {"VPE/EPR (90 °C)", "XLPE/EPR (90 °C)"}}}, "pvc")};
    c.outputs = {Out::number("theta", {"Leitertemperatur θ", "Conductor temperature θ"}, "°C", 1).primary(),
                 Out::number("dT", {"Übertemperatur", "Temperature rise"}, "K", 1),
                 Out::number("load", {"Auslastung I_b/I_z", "Loading I_b/I_z"}, "%", 1),
                 Out::number("rfactor", {"R(θ)/R₂₀ (Cu)", "R(θ)/R₂₀ (Cu)"}, "", 3)};
    c.keywords = {"kabeltemperatur", "leitertemperatur", "cable temperature"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double tmax = in.choice("ins") == "xlpe" ? 90.0 : 70.0;
        const double ta = in.num("Ta");
        if (ta >= tmax) throw CalcError({"Umgebungstemperatur ≥ zulässige Leitertemperatur.", "Ambient ≥ permissible conductor temperature."});
        const double th = cableTemperature(in.num("Ib"), in.num("Iz"), ta, tmax);
        r.set("theta", th);
        r.set("dT", th - ta);
        r.set("load", in.num("Ib") / in.num("Iz") * 100.0);
        r.set("rfactor", 1.0 + cables::alpha20(cables::Material::Cu) * (th - 20.0));
        if (in.num("Ib") > in.num("Iz")) r.error({"I_b > I_z: Leitertemperatur über dem zulässigen Wert.", "I_b > I_z: conductor temperature above the permissible value."});
        return r;
    };
    return c;
}

Calculator cableLosses() {
    Calculator c;
    c.id = "cable_losses";
    c.category = kCat;
    c.title = {"Verlustleistung in Kabeln", "Power loss in cables"};
    c.summary = {"P_V = n · I² · R(θ), Energie und Kosten pro Jahr", "P_loss = n · I² · R(θ), energy and cost per year"};
    c.description = {"P_V = n · I² · ρ(θ) · L / A    n = 2 (DC/1~), 3 (3~, symmetrisch)\nE = P_V · Betriebsstunden    "
                     "Kosten = E · Strompreis. θ = mittlere Leitertemperatur (siehe Kabeltemperatur), Neutralleiter ohne Strom.",
                     "P_loss = n · I² · ρ(θ) · L / A    n = 2 (DC/1~), 3 (3~, balanced)\nE = P_loss · operating hours    "
                     "Cost = E · energy price. θ = mean conductor temperature (see cable temperature), neutral without current."};
    c.inputs = {Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac3"),
                Num::number("I", {"Strom I", "Current I"}, "A", 50).positive(),
                Num::number("L", {"Länge L (einfach)", "Length L (one way)"}, "m", 80).positive(),
                Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
                Num::choice("A", {"Querschnitt", "Cross-section"}, areaOptions(cables::kStandardAreas), "16"),
                Num::number("theta", {"Leitertemperatur θ", "Conductor temperature θ"}, "°C", 50).range(-40, 90),
                Num::number("U", {"Spannung U (optional, für %)", "Voltage U (optional, for %)"}, "V", 400).positive().opt(),
                cosPhiInput(0.9),
                Num::number("h", {"Betriebsstunden pro Jahr", "Operating hours per year"}, "h", 4000).range(0, 8784),
                Num::number("price", {"Strompreis pro kWh", "Energy price per kWh"}, kCurrencyUnit, 0.25).nonNegative()};
    c.outputs = {Out::number("P", {"Verlustleistung P_V", "Power loss P_loss"}, "W", 1).primary(),
                 Out::number("Pm", {"Verluste je Meter Leitung", "Loss per metre of cable"}, "W/m", 2),
                 Out::number("pct", {"Anteil an der übertragenen Leistung", "Share of the transmitted power"}, "%", 2),
                 Out::number("E", {"Verlustenergie pro Jahr", "Energy loss per year"}, "kWh", 0),
                 Out::number("cost", {"Verlustkosten pro Jahr", "Loss cost per year"}, "€", 2)};
    c.keywords = {"verlustleistung", "kabelverluste", "leitungsverluste", "loss", "cable losses"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const double n = sys == System::AC3 ? 3.0 : 2.0;
        const double R = cables::resistivity(parseMaterial(in.choice("mat")), in.num("theta")) * in.num("L") / parseDouble(in.choice("A"));
        const double I = in.num("I");
        const double P = n * I * I * R;
        r.set("P", P);
        r.set("Pm", P / in.num("L"));
        if (const auto U = in.maybe("U")) {
            const double cp = sys == System::DC ? 1.0 : in.num("cosphi");
            r.set("pct", P / (powerFactorK(sys) * *U * I * cp) * 100.0);
        }
        const double E = P * in.num("h") / 1e3;
        r.set("E", E);
        r.set("cost", E * in.num("price"), currencySymbol(ctx));
        return r;
    };
    return c;
}

Calculator neutralCurrentCalc() {
    Calculator c;
    c.id = "neutral_current";
    c.category = kCat;
    c.title = {"Neutralleiterstrom", "Neutral current"};
    c.summary = {"Unsymmetrie und 3. Oberschwingung, Auslegung nach IEC 60364-5-52 Anhang E",
                 "Unbalance and 3rd harmonic, sizing per IEC 60364-5-52 Annex E"};
    c.description = {
        "Grundschwingung (gleiches cos φ, 120° versetzt): I_N1 = √(I₁² + I₂² + I₃² − I₁I₂ − I₂I₃ − I₃I₁)\n"
        "3. Oberschwingung (h₃ in % des Außenleiterstroms) addiert sich im N: I_N3 = h₃ · (I₁ + I₂ + I₃)    I_N = √(I_N1² + I_N3²)\n"
        "Auslegung (IEC 60364-5-52 Tab. E.52.1, 4-/5-adrige Kabel): h₃ ≤ 15 %: Außenleiterstrom, Faktor 1,0; "
        "15–33 %: Außenleiterstrom, 0,86; 33–45 %: Neutralleiterstrom, 0,86; > 45 %: Neutralleiterstrom, 1,0.",
        "Fundamental (same cos φ, 120° apart): I_N1 = √(I₁² + I₂² + I₃² − I₁I₂ − I₂I₃ − I₃I₁)\n"
        "3rd harmonic (h₃ in % of the line current) adds up in N: I_N3 = h₃ · (I₁ + I₂ + I₃)    I_N = √(I_N1² + I_N3²)\n"
        "Sizing (IEC 60364-5-52 Tab. E.52.1, 4-/5-core cables): h₃ ≤ 15 %: line current, factor 1.0; "
        "15–33 %: line current, 0.86; 33–45 %: neutral current, 0.86; > 45 %: neutral current, 1.0."};
    c.inputs = {Num::number("I1", {"Strom L1", "Current L1"}, "A", 32).nonNegative(),
                Num::number("I2", {"Strom L2", "Current L2"}, "A", 25).nonNegative(),
                Num::number("I3", {"Strom L3", "Current L3"}, "A", 20).nonNegative(),
                Num::number("h3", {"Anteil 3. Oberschwingung h₃", "3rd harmonic content h₃"}, "%", 0).range(0, 100)};
    c.outputs = {Out::number("IN", {"Neutralleiterstrom I_N", "Neutral current I_N"}, "A", 1).primary(),
                 Out::number("IN1", {"Anteil Unsymmetrie I_N1", "Unbalance part I_N1"}, "A", 1),
                 Out::number("IN3", {"Anteil 3. Oberschwingung I_N3", "3rd harmonic part I_N3"}, "A", 1),
                 Out::number("ratio", {"I_N / größter Außenleiterstrom", "I_N / highest line current"}, "%", 0),
                 Out::number("Idesign", {"Bemessungsstrom für die Kabelwahl", "Design current for cable selection"}, "A", 1),
                 Out::text("basis", {"Auslegung", "Sizing"})};
    c.keywords = {"neutralleiter", "n-leiter", "oberschwingung", "harmonische", "neutral", "harmonics"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double I1 = in.num("I1"), I2 = in.num("I2"), I3 = in.num("I3"), h3 = in.num("h3");
        const double n1 = neutralCurrentFundamental(I1, I2, I3);
        const double n3 = h3 / 100.0 * (I1 + I2 + I3);
        const double IN = neutralCurrent(I1, I2, I3, h3);
        const double imax = std::max({I1, I2, I3});
        r.set("IN", IN);
        r.set("IN1", n1);
        r.set("IN3", n3);
        if (imax > 0) r.set("ratio", IN / imax * 100.0);
        const auto s = harmonicSizing(h3);
        const double base = s.byNeutral ? IN : imax;
        r.set("Idesign", base / s.factor);
        r.setText("basis", s.byNeutral ? cat(Text{"Neutralleiterstrom, Faktor ", "neutral current, factor "}, numText(s.factor, 2))
                                       : cat(Text{"Außenleiterstrom, Faktor ", "line current, factor "}, numText(s.factor, 2)));
        if (IN > imax) r.warn({"I_N > Außenleiterstrom: Neutralleiter nicht reduzieren, ggf. N verstärken.",
                               "I_N > line current: do not reduce the neutral, possibly enlarge N."});
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ distributed loads

Calculator distributed(bool sizing) {
    Calculator c;
    c.id = sizing ? "sizing_distributed" : "vdrop_distributed";
    c.category = kCat;
    c.title = sizing ? Text{"Leiterdimensionierung bei verteilten Lasten", "Cable sizing for distributed loads"}
                     : Text{"Spannungsfall bei verteilten Lasten", "Voltage drop with distributed loads"};
    c.summary = sizing ? Text{"Mindestquerschnitt für n gleiche Lasten entlang einer Leitung", "Minimum cross-section for n equal loads along a line"}
                       : Text{"n gleiche Lasten entlang einer Leitung (z. B. Leuchtenband)", "n equal loads along a line (e.g. lighting row)"};
    c.description = {
        "Momentenmethode: ΔU = k · Σ(I · L_i) · (ρ/A · cos φ + X' · sin φ), L_i = L₁ + (i − 1) · d\n"
        "k = 2 (DC/1~), √3 (3~, symmetrisch verteilt); ρ bei 70 °C, X' = 0,08 mΩ/m. Σ(I·L) entspricht dem Gesamtstrom "
        "in der Ersatzlänge L₁ + (n − 1) · d / 2.\nQuerschnitt: A = ρ · cos φ / (ΔU_max/(k · Σ I·L) − X' · sin φ), "
        "danach nächster Normquerschnitt.",
        "Moment method: ΔU = k · Σ(I · L_i) · (ρ/A · cos φ + X' · sin φ), L_i = L₁ + (i − 1) · d\n"
        "k = 2 (DC/1~), √3 (3~, balanced); ρ at 70 °C, X' = 0.08 mΩ/m. Σ(I·L) equals the total current at the "
        "equivalent length L₁ + (n − 1) · d / 2.\nCross-section: A = ρ · cos φ / (ΔU_max/(k · Σ I·L) − X' · sin φ), "
        "then the next standard size."};
    c.inputs = {Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac1"),
                Num::number("U", {"Nennspannung U", "Nominal voltage U"}, "V", 230).positive(),
                Num::number("n", {"Anzahl Lasten n", "Number of loads n"}, "", 10).range(1, 1000),
                Num::number("I", {"Strom je Last", "Current per load"}, "A", 0.5).positive(),
                Num::number("L1", {"Abstand bis zur ersten Last L₁", "Distance to the first load L₁"}, "m", 15).nonNegative(),
                Num::number("d", {"Abstand zwischen den Lasten d", "Spacing between loads d"}, "m", 3).nonNegative(),
                Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
                cosPhiInput(0.95)};
    if (sizing) {
        c.inputs.push_back(Num::number("limit", {"Zulässiger Spannungsfall", "Permissible voltage drop"}, "%", 3).rangeExclusiveMin(0, 20));
    } else {
        c.inputs.push_back(Num::choice("A", {"Querschnitt", "Cross-section"}, areaOptions(cables::kStandardAreas), "1.5"));
    }
    if (sizing) {
        c.outputs = {Out::number("A", {"Gewählter Querschnitt", "Selected cross-section"}, "mm²", 1).primary(),
                     Out::number("Amin", {"Mindestquerschnitt (rechnerisch)", "Minimum cross-section (calculated)"}, "mm²", 2),
                     Out::number("dUpct", {"Spannungsfall mit gewähltem Querschnitt", "Voltage drop with selected cross-section"}, "%", 2),
                     Out::number("Itot", {"Gesamtstrom am Anfang", "Total current at the start"}, "A", 2),
                     Out::number("Leq", {"Ersatzlänge", "Equivalent length"}, "m", 1)};
    } else {
        c.outputs = {Out::number("dUpct", {"Spannungsfall an der letzten Last", "Voltage drop at the last load"}, "%", 2).primary(),
                     Out::number("dU", {"Spannungsfall ΔU", "Voltage drop ΔU"}, "V", 2),
                     Out::number("Ulast", {"Spannung an der letzten Last", "Voltage at the last load"}, "V", 1),
                     Out::number("Itot", {"Gesamtstrom am Anfang", "Total current at the start"}, "A", 2),
                     Out::number("Leq", {"Ersatzlänge", "Equivalent length"}, "m", 1),
                     Out::number("Lend", {"Länge bis zur letzten Last", "Length to the last load"}, "m", 1)};
    }
    c.keywords = {"verteilte last", "leuchtenband", "streckenlast", "spannungsfall", "distributed load", "voltage drop"};
    c.compute = [sizing](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const int n = static_cast<int>(std::lround(in.num("n")));
        const double I = in.num("I");
        const double L1 = in.num("L1");
        const double d = in.num("d");
        const double M = distributedMoment(n, I, L1, d);
        if (M <= 0) throw CalcError({"Leitungslänge 0 – kein Spannungsfall.", "Cable length 0 – no voltage drop."});
        const double rho = cables::resistivity(parseMaterial(in.choice("mat")), 70.0);
        const double cp = in.num("cosphi");
        const double x = 0.08e-3;
        const double U = in.num("U");
        r.set("Itot", n * I);
        r.set("Leq", M / (n * I));
        if (sizing) {
            const double dUmax = in.num("limit") / 100.0 * U;
            const double Amin = distributedRequiredArea(sys, M, rho, cp, x, dUmax);
            if (!std::isfinite(Amin))
                throw CalcError({"Grenzwert nicht erreichbar (Reaktanz allein zu groß) – Leitung aufteilen.",
                                 "Limit not reachable (reactance alone too large) – split the line."});
            const auto A = nextAtLeast(cables::kStandardAreas, std::max(Amin, 1.5));
            if (!A) throw CalcError({"Mehr als 300 mm² nötig – Leitung aufteilen.", "More than 300 mm² needed – split the line."});
            r.set("Amin", Amin);
            r.set("A", *A);
            r.set("dUpct", distributedVoltageDrop(sys, M, *A, rho, cp, x) / U * 100.0);
            r.info({"Mindestens 1,5 mm² gewählt; Belastbarkeit und Schutz gesondert prüfen.", "At least 1.5 mm² selected; check current rating and protection separately."});
        } else {
            const double A = parseDouble(in.choice("A"));
            const double dU = distributedVoltageDrop(sys, M, A, rho, cp, x);
            r.set("dU", dU);
            r.set("dUpct", dU / U * 100.0);
            r.set("Ulast", U - dU);
            r.set("Lend", L1 + (n - 1) * d);
            if (dU / U * 100.0 > 3.0) r.warn({"Spannungsfall > 3 % – größeren Querschnitt prüfen.", "Voltage drop > 3 % – consider a larger cross-section."});
        }
        return r;
    };
    return c;
}

}  // namespace

void addElectricalCables(std::vector<Calculator>& out) {
    out.push_back(ampacityMi());
    out.push_back(busbarAmpacity());
    out.push_back(cableRxz());
    out.push_back(cableTemperatureCalc());
    out.push_back(cableLosses());
    out.push_back(neutralCurrentCalc());
    out.push_back(distributed(false));
    out.push_back(distributed(true));
}

}  // namespace elc::catalog
