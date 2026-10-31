// Calculators: Photovoltaik & Speicher / PV & storage
#include <algorithm>
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/cables.hpp"
#include "elc/formulas/pv.hpp"

namespace elc::catalog {

namespace {

Calculator pvString() {
    Calculator c;
    c.id = "pv_string";
    c.category = "pv";
    c.title = {"PV-String-Auslegung", "PV string design"};
    c.summary = {"Module pro String für den Wechselrichter, Strangsicherung", "Modules per string for the inverter, string fuses"};
    c.description = {
        "X(T) = X_STC · (1 + β · (T − 25 °C))   für U_oc und U_mpp (β negativ, Datenblatt)\n"
        "n_max = min(⌊U_DC,max / U_oc(T_min)⌋, ⌊U_MPP,max / U_mpp(T_min)⌋)    n_min = ⌈U_MPP,min / U_mpp(T_max)⌉\n"
        "T_min = tiefste Zelltemperatur (DE typ. −10 °C, Höhenlagen/CH bis −20 °C), T_max = Zelltemperatur im Sommer (typ. 70 °C).\n"
        "Strangsicherung nötig, wenn (N_p − 1) · 1,25 · I_sc > Rückstrombelastbarkeit des Moduls (IEC 62548 / VDE 0100-712); "
        "Bemessung dann 1,5 · I_sc < I_n ≤ 2,4 · I_sc und ≤ Rückstromwert.",
        "X(T) = X_STC · (1 + β · (T − 25 °C))   for V_oc and V_mpp (β negative, datasheet)\n"
        "n_max = min(⌊V_DC,max / V_oc(T_min)⌋, ⌊V_MPP,max / V_mpp(T_min)⌋)    n_min = ⌈V_MPP,min / V_mpp(T_max)⌉\n"
        "T_min = lowest cell temperature (DE typ. −10 °C, alpine/CH down to −20 °C), T_max = summer cell temperature (typ. 70 °C).\n"
        "String fuses required if (N_p − 1) · 1.25 · I_sc > module reverse current rating (IEC 62548); "
        "rating then 1.5 · I_sc < I_n ≤ 2.4 · I_sc and ≤ reverse current rating."};
    c.inputs = {
        Num::number("voc", {"Modul U_oc (STC)", "Module V_oc (STC)"}, "V", 37.5).positive(),
        Num::number("vmp", {"Modul U_mpp (STC)", "Module V_mpp (STC)"}, "V", 31.3).positive(),
        Num::number("isc", {"Modul I_sc (STC)", "Module I_sc (STC)"}, "A", 13.9).positive(),
        Num::number("pmod", {"Modulleistung", "Module power"}, "Wp", 430).positive(),
        Num::number("bvoc", {"Temp.-Koeffizient U_oc", "Temp. coefficient V_oc"}, "%/K", -0.27).range(-1, 0),
        Num::number("bvmp", {"Temp.-Koeffizient U_mpp (≈ P_max)", "Temp. coefficient V_mpp (≈ P_max)"}, "%/K", -0.34).range(-1, 0),
        Num::number("tmin", {"Min. Zelltemperatur", "Min. cell temperature"}, "°C", -10).range(-40, 25),
        Num::number("tmax", {"Max. Zelltemperatur", "Max. cell temperature"}, "°C", 70).range(25, 100),
        Num::number("udc", {"WR max. DC-Eingangsspannung", "Inverter max. DC input voltage"}, "V", 1000).positive(),
        Num::number("mppmin", {"WR MPP-Bereich min.", "Inverter MPP range min."}, "V", 200).positive(),
        Num::number("mppmax", {"WR MPP-Bereich max.", "Inverter MPP range max."}, "V", 800).positive(),
        Num::number("imax", {"WR max. I_SC je MPPT", "Inverter max. I_SC per MPPT"}, "A", 25).positive(),
        Num::number("np", {"Parallele Strings je MPPT", "Parallel strings per MPPT"}, "", 1).range(1, 50),
        Num::number("ir", {"Rückstrombelastbarkeit Modul", "Module reverse current rating"}, "A", 25).positive(),
        Num::number("n", {"Geplante Module pro String", "Planned modules per string"}, "", 18).opt().range(1, 100),
    };
    c.outputs = {Out::number("vocMin", {"U_oc je Modul bei T_min", "V_oc per module at T_min"}, "V", 2),
                 Out::number("vmpMax", {"U_mpp je Modul bei T_max", "V_mpp per module at T_max"}, "V", 2),
                 Out::text("range", {"Module pro String", "Modules per string"}).primary(),
                 Out::number("uoc", {"String-U_oc bei T_min (geplant)", "String V_oc at T_min (planned)"}, "V", 0),
                 Out::number("umpp", {"String-U_mpp bei T_max (geplant)", "String V_mpp at T_max (planned)"}, "V", 0),
                 Out::number("kwp", {"Leistung je String (geplant)", "Power per string (planned)"}, "kWp", 2),
                 Out::number("i", {"Strom je MPPT (1,25 · I_sc · N_p)", "Current per MPPT (1.25 · I_sc · N_p)"}, "A", 1),
                 Out::text("fuse", {"Strangsicherung", "String fuse"})};
    c.keywords = {"pv", "photovoltaik", "string", "wechselrichter", "inverter", "solar", "modul", "voc"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const pv::StringInput si{in.num("voc"), in.num("vmp"), in.num("bvoc"), in.num("bvmp"), in.num("tmin"), in.num("tmax"),
                                 in.num("udc"), in.num("mppmin"), in.num("mppmax")};
        const auto lim = pv::stringLimits(si);
        const int nMax = std::min(lim.maxByVoltage, lim.maxByMpp);
        const int nMin = lim.minByMpp;
        r.set("vocMin", lim.vocMin);
        r.set("vmpMax", lim.vmpMax);
        if (nMin > nMax) {
            r.setText("range", {"keine gültige Anzahl", "no valid count"});
            r.error({"MPP-Bereich und Spannungsgrenze passen nicht zum Modul – anderen Wechselrichter wählen.",
                     "MPP range and voltage limit do not fit the module – choose another inverter."});
        } else {
            r.setText("range", cat(numText(nMin, 0), Text{" … ", " … "}, numText(nMax, 0)));
        }
        const int np = static_cast<int>(std::lround(in.num("np")));
        const double i = 1.25 * in.num("isc") * np;
        r.set("i", i);
        if (i > in.num("imax")) {
            r.error({"Max. Kurzschlussstrom des Wechselrichter-Eingangs überschritten – weniger parallele Strings je MPPT.",
                     "Inverter input short-circuit current exceeded – fewer parallel strings per MPPT."});
        }
        if (pv::stringFuseRequired(np, in.num("isc"), in.num("ir"))) {
            const double lo = 1.5 * in.num("isc");
            const double hi = std::min(2.4 * in.num("isc"), in.num("ir"));
            r.setText("fuse", cat(Text{"erforderlich, ", "required, "}, numText(lo, 1), Text{" A < I_n ≤ ", " A < I_n ≤ "}, numText(hi, 1, "A")));
        } else {
            r.setText("fuse", {"nicht erforderlich", "not required"});
        }
        if (const auto n = in.maybe("n")) {
            const double nn = std::round(*n);
            r.set("uoc", nn * lim.vocMin);
            r.set("umpp", nn * lim.vmpMax);
            r.set("kwp", nn * in.num("pmod") / 1000.0);
            if (nn > nMax) {
                r.error({"Zu viele Module: U_oc bei Kälte oder MPP-Obergrenze überschritten.",
                         "Too many modules: V_oc at low temperature or MPP upper limit exceeded."});
            } else if (nn < nMin) {
                r.warn({"Zu wenige Module: U_mpp bei Hitze unter MPP-Untergrenze – Ertragsverlust.",
                        "Too few modules: V_mpp in hot conditions below MPP minimum – yield loss."});
            } else {
                r.ok({"Geplante Stringlänge passt zum Wechselrichter.", "Planned string length fits the inverter."});
            }
        }
        return r;
    };
    return c;
}

Calculator pvYield() {
    Calculator c;
    c.id = "pv_yield";
    c.category = "pv";
    c.title = {"PV-Ertrag & Wirtschaftlichkeit", "PV yield & economics"};
    c.summary = {"Jahresertrag, Eigenverbrauch, Ersparnis, Amortisation", "Annual yield, self-consumption, savings, payback"};
    c.description = {
        "E = P_peak · spez. Ertrag    Ersparnis = E · EV-Anteil · Strompreis    Einspeisung = E · (1 − EV-Anteil) · Vergütung\n"
        "Amortisation (statisch) = Investition / jährlicher Nutzen – ohne Zinsen, Degradation, Wartung.\n"
        "Spezifischer Ertrag z. B. aus PVGIS: DE typ. 850–1100 kWh/kWp, CH-Mittelland ca. 950–1100 kWh/kWp, Alpen höher.",
        "E = P_peak · specific yield    Saving = E · self-consumption · energy price    Feed-in = E · (1 − SC) · tariff\n"
        "Payback (simple) = investment / annual benefit – without interest, degradation, maintenance.\n"
        "Specific yield e.g. from PVGIS: Germany typ. 850–1100 kWh/kWp, Swiss plateau approx. 950–1100 kWh/kWp, Alps higher."};
    c.inputs = {
        Num::number("kwp", {"Anlagenleistung", "System size"}, "kWp", 10).positive(),
        Num::number("spec", {"Spezifischer Ertrag", "Specific yield"}, "kWh/kWp", 950).positive(),
        Num::number("sc", {"Eigenverbrauchsanteil", "Self-consumption share"}, "%", 30).range(0, 100),
        Num::number("price", {"Strompreis Bezug", "Grid energy price"}, "€ / CHF", 0.30).nonNegative(),
        Num::number("feed", {"Einspeisevergütung", "Feed-in tariff"}, "€ / CHF", 0.08).nonNegative(),
        Num::number("inv", {"Investition", "Investment"}, "€ / CHF", 15000).opt().nonNegative(),
        Num::number("co2", {"CO₂-Faktor Netzstrom", "Grid CO₂ factor"}, "g/kWh", 380).nonNegative(),
    };
    c.outputs = {Out::number("E", {"Jahresertrag", "Annual yield"}, "kWh", 0).primary(),
                 Out::number("Esc", {"Davon Eigenverbrauch", "Thereof self-consumed"}, "kWh", 0),
                 Out::number("save", {"Ersparnis Eigenverbrauch", "Self-consumption saving"}, "€", 0),
                 Out::number("rev", {"Einspeiseerlös", "Feed-in revenue"}, "€", 0),
                 Out::number("total", {"Nutzen pro Jahr", "Benefit per year"}, "€", 0),
                 Out::number("payback", {"Amortisation (statisch)", "Payback (simple)"}, "a", 1).primary(),
                 Out::number("co2t", {"CO₂-Einsparung pro Jahr", "CO₂ saving per year"}, "t", 2)};
    c.keywords = {"pv", "ertrag", "photovoltaik", "amortisation", "eigenverbrauch", "solar", "yield"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const std::string cur = ctx.norm == NormProfile::CH ? "CHF" : "€";
        const double E = in.num("kwp") * in.num("spec");
        const double esc = E * in.num("sc") / 100.0;
        const double save = esc * in.num("price");
        const double rev = (E - esc) * in.num("feed");
        r.set("E", E);
        r.set("Esc", esc);
        r.set("save", save, cur);
        r.set("rev", rev, cur);
        r.set("total", save + rev, cur);
        r.set("co2t", E * in.num("co2") / 1e6);
        if (const auto inv = in.maybe("inv"); inv && save + rev > 0) {
            r.set("payback", *inv / (save + rev));
        }
        return r;
    };
    return c;
}

Calculator pvDcCable() {
    Calculator c;
    c.id = "pv_dc_cable";
    c.category = "pv";
    c.title = {"PV-DC-Leitung: Verluste", "PV DC cable: losses"};
    c.summary = {"Leistungsverlust und Mindestquerschnitt String-Leitung", "Power loss and minimum cross-section of string cable"};
    c.description = {
        "P_V = 2 · L · I² · ρ / A    Verlust % = P_V / (U_mpp · I) · 100    A_min = 2 · L · I · ρ / (v · U_mpp)\n"
        "Üblich: DC-Verlust ≤ 1 %. Leitertemperatur 70 °C (Dach). Belastbarkeit der Solarleitung (H1Z2Z2-K, EN 50618) "
        "nach Datenblatt prüfen.",
        "P_loss = 2 · L · I² · ρ / A    Loss % = P_loss / (V_mpp · I) · 100    A_min = 2 · L · I · ρ / (v · V_mpp)\n"
        "Common target: DC loss ≤ 1 %. Conductor temperature 70 °C (roof). Check ampacity of the solar cable "
        "(H1Z2Z2-K, EN 50618) in the datasheet."};
    c.inputs = {
        Num::number("U", {"String-Spannung U_mpp", "String voltage V_mpp"}, "V", 560).positive(),
        Num::number("I", {"String-Strom I_mpp", "String current I_mpp"}, "A", 13.2).positive(),
        Num::number("L", {"Leitungslänge (einfach)", "Cable length (one way)"}, "m", 40).positive(),
        Num::choice("A", {"Querschnitt", "Cross-section"}, {{"2.5", "2,5 mm²"}, {"4", "4 mm²"}, {"6", "6 mm²"}, {"10", "10 mm²"}, {"16", "16 mm²"}}, "4"),
        Num::number("vmax", {"Zul. Verlust", "Permitted loss"}, "%", 1).positive(),
    };
    c.outputs = {Out::number("loss", {"Leistungsverlust", "Power loss"}, "W", 1),
                 Out::number("pct", {"Verlust", "Loss"}, "%", 2).primary(),
                 Out::number("Amin", {"Mindestquerschnitt für Zielwert", "Minimum cross-section for target"}, "mm²", 2),
                 Out::number("Eloss", {"Energieverlust pro Jahr (≈ 1000 Vollaststunden)", "Energy loss per year (≈ 1000 full-load hours)"}, "kWh", 0)};
    c.keywords = {"pv", "dc", "solarkabel", "leitungsverlust", "h1z2z2", "string"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double rho = cables::resistivity(cables::Material::Cu, 70.0);
        const double A = parseDouble(in.choice("A"));
        const double I = in.num("I");
        const double loss = 2.0 * in.num("L") * I * I * rho / A;
        const double pct = loss / (in.num("U") * I) * 100.0;
        r.set("loss", loss);
        r.set("pct", pct);
        r.set("Amin", 2.0 * in.num("L") * I * rho / (in.num("vmax") / 100.0 * in.num("U")));
        r.set("Eloss", loss * 1000.0 / 1000.0);
        if (pct <= in.num("vmax")) {
            r.ok({"Verlust im Zielwert.", "Loss within target."});
        } else {
            r.warn({"Verlust über Zielwert – größeren Querschnitt wählen.", "Loss above target – use a larger cross-section."});
        }
        return r;
    };
    return c;
}

Calculator storage() {
    Calculator c;
    c.id = "pv_storage";
    c.category = "pv";
    c.title = {"Batteriespeicher dimensionieren", "Battery storage sizing"};
    c.summary = {"Kapazität aus Nachtverbrauch und Faustformeln", "Capacity from night consumption and rules of thumb"};
    c.description = {
        "C_nenn = E_Nacht / (DoD · η_Entladung)\n"
        "Faustformeln (z. B. HTW Berlin, Eigenheim): ca. 1,0–1,5 kWh Speicher je 1000 kWh Jahresverbrauch und "
        "nicht mehr als ca. 1,5 kWh je kWp. Größere Speicher erhöhen die Autarkie nur noch wenig.",
        "C_nominal = E_night / (DoD · η_discharge)\n"
        "Rules of thumb (e.g. HTW Berlin, homes): approx. 1.0–1.5 kWh storage per 1000 kWh annual consumption and "
        "no more than approx. 1.5 kWh per kWp. Larger batteries add little autarky."};
    c.inputs = {
        Num::number("Enight", {"Verbrauch abends/nachts", "Evening/night consumption"}, "kWh/d", 6).positive(),
        Num::number("dod", {"Nutzbare Entladetiefe (DoD)", "Usable depth of discharge"}, "%", 90).rangeExclusiveMin(0, 100),
        Num::number("eta", {"Wirkungsgrad Entladung", "Discharge efficiency"}, "%", 95).rangeExclusiveMin(0, 100),
        Num::number("Eyear", {"Jahresverbrauch", "Annual consumption"}, "kWh", 4500).positive(),
        Num::number("kwp", {"PV-Leistung", "PV size"}, "kWp", 10).positive(),
    };
    c.outputs = {Out::number("C", {"Nennkapazität für Nachtbedarf", "Nominal capacity for night demand"}, "kWh", 1).primary(),
                 Out::text("rule1", {"Faustformel Verbrauch", "Rule of thumb consumption"}),
                 Out::number("rule2", {"Obergrenze nach PV-Leistung", "Upper limit by PV size"}, "kWh", 1)};
    c.keywords = {"speicher", "batterie", "akku", "pv", "autarkie", "storage"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double C = in.num("Enight") / (in.num("dod") / 100.0 * in.num("eta") / 100.0);
        const double lo = in.num("Eyear") / 1000.0;
        const double hi = 1.5 * in.num("Eyear") / 1000.0;
        r.set("C", C);
        r.setText("rule1", cat(numText(lo, 1), Text{" … ", " … "}, numText(hi, 1, "kWh")));
        r.set("rule2", 1.5 * in.num("kwp"));
        if (C > hi * 1.3) {
            r.info({"Nachtbedarf übersteigt die Faustformel deutlich – Wirtschaftlichkeit prüfen.",
                    "Night demand clearly exceeds the rule of thumb – check economics."});
        }
        return r;
    };
    return c;
}

}  // namespace

void addPv(std::vector<Calculator>& out) {
    out.push_back(pvString());
    out.push_back(pvYield());
    out.push_back(pvDcCable());
    out.push_back(storage());
}

}  // namespace elc::catalog
