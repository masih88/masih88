// Calculators: Leitungen & Schutz / Cables & protection
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/cables.hpp"

namespace elc::catalog {

using namespace elc::cables;

namespace {

std::vector<Option> insulationOptions() {
    return {{"pvc", {"PVC 70 °C (z. B. NYM, NYY, H07V)", "PVC 70 °C (e.g. NYM, NYY, H07V)"}},
            {"xlpe", {"VPE/XLPE 90 °C (z. B. N2XH, N2XY)", "XLPE/EPR 90 °C (e.g. N2XH, N2XY)"}}};
}

Insulation parseInsulation(const std::string& v) { return v == "xlpe" ? Insulation::XLPE : Insulation::PVC; }

std::vector<Option> methodOptions() {
    return {
        {"A1", {"A1 – Aderleitungen im Rohr in wärmegedämmter Wand", "A1 – Single cores in conduit in insulated wall"}},
        {"A2", {"A2 – Mehradrige Leitung im Rohr in wärmegedämmter Wand",
                "A2 – Multi-core cable in conduit in insulated wall"}},
        {"B1", {"B1 – Aderleitungen im Rohr/Kanal auf oder in der Wand",
                "B1 – Single cores in conduit/trunking on or in wall"}},
        {"B2", {"B2 – Mehradrige Leitung im Rohr/Kanal auf der Wand",
                "B2 – Multi-core cable in conduit/trunking on wall"}},
        {"C", {"C – Direkt auf/in der Wand (z. B. NYM auf Putz)", "C – Clipped direct on/in wall"}},
        {"D1", {"D1 – Mehradriges Kabel im Rohr im Erdreich", "D1 – Multi-core cable in duct in the ground"}},
        {"E", {"E – Mehradrig frei in Luft / gelochte Kabelrinne", "E – Multi-core in free air / perforated tray"}},
    };
}

Method parseMethod(const std::string& v) {
    if (v == "A1") return Method::A1;
    if (v == "A2") return Method::A2;
    if (v == "B1") return Method::B1;
    if (v == "B2") return Method::B2;
    if (v == "D1") return Method::D1;
    if (v == "E") return Method::E;
    return Method::C;
}

std::vector<Option> arrangementOptions() {
    return {
        {"bunched", {"Gebündelt, im Rohr oder Kanal", "Bunched, in conduit or trunking"}},
        {"wall", {"Einlagig auf Wand/Boden/ungelochter Rinne", "Single layer on wall/floor/unperforated tray"}},
        {"ceiling", {"Einlagig direkt unter der Decke", "Single layer directly under ceiling"}},
        {"tray", {"Einlagig auf gelochter Kabelrinne", "Single layer on perforated tray"}},
        {"ladder", {"Einlagig auf Kabelleiter/Konsolen", "Single layer on ladder/cleats"}},
    };
}

Arrangement parseArrangement(const std::string& v) {
    if (v == "wall") return Arrangement::SingleLayerWall;
    if (v == "ceiling") return Arrangement::SingleLayerCeiling;
    if (v == "tray") return Arrangement::PerforatedTray;
    if (v == "ladder") return Arrangement::Ladder;
    return Arrangement::Bunched;
}

std::vector<Option> deviceOptions(bool withManual = false) {
    std::vector<Option> o{{"B", {"LS-Schalter Typ B", "MCB type B"}},
                          {"C", {"LS-Schalter Typ C", "MCB type C"}},
                          {"D", {"LS-Schalter Typ D", "MCB type D"}}};
    if (withManual) {
        o.push_back({"manual", {"Sicherung / sonstige – Ia manuell", "Fuse / other – enter Ia"}});
    } else {
        o.push_back({"gG", {"Schmelzsicherung gG (NH/D)", "Fuse gG"}});
    }
    return o;
}

Device parseDevice(const std::string& v) {
    if (v == "B") return Device::MCB_B;
    if (v == "D") return Device::MCB_D;
    if (v == "gG") return Device::FuseGG;
    return Device::MCB_C;
}

const Text kAmpacitySource{
    "Strombelastbarkeit Kupfer nach IEC 60364-5-52 Anhang B (entspricht DIN VDE 0298-4 bzw. NIN 5.2.3), "
    "Bezug 30 °C Luft bzw. 20 °C Erdreich. Werte vor Verwendung gegen die gültige Normausgabe prüfen.",
    "Copper current-carrying capacity per IEC 60364-5-52 Annex B (equivalent to DIN VDE 0298-4 / NIN 5.2.3), "
    "reference 30 °C air or 20 °C ground. Check values against the edition in force before use."};

// ---- voltage drop limits ----------------------------------------------------------------

std::vector<Option> limitOptions() {
    return {
        {"auto", {"Nach Normprofil (DE 3 % / CH 4 %)", "By norm profile (DE 3 % / CH 4 %)"}},
        {"de_18015", {"DE · DIN 18015-1 Wohngebäude ab Zähler: 3 %", "DE · DIN 18015-1 residential from meter: 3 %"}},
        {"de_light", {"DE · VDE 0100-520 Anh. G öff. Netz, Beleuchtung: 3 %",
                      "DE · VDE 0100-520 Annex G public LV, lighting: 3 %"}},
        {"de_other", {"DE · VDE 0100-520 Anh. G öff. Netz, sonstige: 5 %",
                      "DE · VDE 0100-520 Annex G public LV, other: 5 %"}},
        {"de_priv_light", {"DE · VDE 0100-520 Anh. G eigene Trafostation, Beleuchtung: 6 %",
                           "DE · VDE 0100-520 Annex G private supply, lighting: 6 %"}},
        {"de_priv_other", {"DE · VDE 0100-520 Anh. G eigene Trafostation, sonstige: 8 %",
                           "DE · VDE 0100-520 Annex G private supply, other: 8 %"}},
        {"ch_nin", {"CH · NIN: Anschlussüberstromunterbrecher bis Verbraucher: 4 %",
                    "CH · NIN: service fuse to load: 4 %"}},
        {"custom", {"Eigener Grenzwert", "Custom limit"}},
    };
}

double limitPercent(const std::string& choice, double custom, NormProfile norm) {
    if (choice == "de_18015" || choice == "de_light") return 3.0;
    if (choice == "de_other") return 5.0;
    if (choice == "de_priv_light") return 6.0;
    if (choice == "de_priv_other") return 8.0;
    if (choice == "ch_nin") return 4.0;
    if (choice == "custom") return custom;
    return norm == NormProfile::CH ? 4.0 : 3.0;
}

Calculator voltageDropCalc() {
    Calculator c;
    c.id = "voltage_drop";
    c.category = "cables";
    c.title = {"Spannungsfall", "Voltage drop"};
    c.summary = {"ΔU in V und %, Verluste, Grenzwert DE/CH", "ΔU in V and %, losses, DE/CH limit"};
    c.description = {
        "ΔU = k · L · I · (ρ(θ)/A · cos φ + X' · sin φ)\n"
        "k = 2 bei DC/1~ (Hin- und Rückleiter), k = √3 bei 3~ (Außenleiterspannung)\n"
        "ΔU % = ΔU / U · 100    P_V = n · I² · ρ · L / A\n"
        "Typisch X' ≈ 0,08 mΩ/m für mehradrige Kabel. Leitertemperatur 70 °C (PVC) ist konservativ; "
        "IEC 60364-5-52 Anh. G nutzt ρ = 0,0225 Ω·mm²/m (Cu).\n"
        "Grenzwerte: DIN 18015-1, DIN VDE 0100-520 Anhang G, NIN – gegen gültige Ausgabe und TAB des Netzbetreibers prüfen.",
        "ΔU = k · L · I · (ρ(θ)/A · cos φ + X' · sin φ)\n"
        "k = 2 for DC/1~ (out and return), k = √3 for 3~ (line-to-line voltage)\n"
        "ΔU % = ΔU / U · 100    P_loss = n · I² · ρ · L / A\n"
        "Typically X' ≈ 0.08 mΩ/m for multi-core cables. 70 °C conductor temperature (PVC) is conservative; "
        "IEC 60364-5-52 Annex G uses ρ = 0.0225 Ω·mm²/m (Cu).\n"
        "Limits: DIN 18015-1, DIN VDE 0100-520 Annex G, NIN – check against the edition in force and local rules."};
    c.inputs = {
        Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac1"),
        Num::number("U", {"Nennspannung U", "Nominal voltage U"}, "V", 230).positive(),
        Num::number("I", {"Betriebsstrom I_B", "Load current I_B"}, "A", 16).positive(),
        Num::number("L", {"Leitungslänge L (einfach)", "Cable length L (one way)"}, "m", 30).positive(),
        Num::number("A", {"Querschnitt A", "Cross-section A"}, "mm²", 2.5).positive(),
        Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
        cosPhiInput(1.0),
        Num::number("T", {"Leitertemperatur θ", "Conductor temperature θ"}, "°C", 70).range(-40, 250),
        Num::number("x", {"Reaktanzbelag X'", "Reactance per length X'"}, "mΩ/m", 0.08)
            .nonNegative()
            .showIf("sys", {"ac1", "ac3"}),
        Num::choice("limit", {"Grenzwert", "Limit"}, limitOptions(), "auto"),
        Num::number("custom", {"Eigener Grenzwert ΔU max", "Custom limit ΔU max"}, "%", 3)
            .positive()
            .showIf("limit", {"custom"}),
    };
    c.outputs = {Out::number("dU", {"Spannungsfall ΔU", "Voltage drop ΔU"}, "V", 2),
                 Out::number("dUpct", {"Spannungsfall ΔU", "Voltage drop ΔU"}, "%", 2).primary(),
                 Out::number("Uend", {"Spannung am Verbraucher", "Voltage at load"}, "V", 1),
                 Out::number("Ploss", {"Leitungsverluste P_V", "Cable losses P_loss"}, "W", 1),
                 Out::number("limit", {"Grenzwert", "Limit"}, "%", 1),
                 Out::number("Lmax", {"Max. Länge beim Grenzwert", "Max. length at limit"}, "m", 1)};
    c.keywords = {"spannungsfall", "spannungsabfall", "delta u", "voltage drop", "leitungslänge"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const double rho = resistivity(parseMaterial(in.choice("mat")), in.num("T"));
        const double x = in.num("x") * 1e-3;
        const double cosPhi = in.has("cosphi") ? in.num("cosphi") : 1.0;
        const double U = in.num("U");
        const double dU = voltageDrop(sys, in.num("L"), in.num("I"), in.num("A"), rho, cosPhi, x);
        const double pct = dU / U * 100.0;
        const double limit = limitPercent(in.choice("limit"), in.num("custom"), ctx.norm);
        r.set("dU", dU);
        r.set("dUpct", pct);
        r.set("Uend", U - dU);
        r.set("Ploss", lineLoss(sys, in.num("L"), in.num("I"), in.num("A"), rho));
        r.set("limit", limit);
        r.set("Lmax", maxLength(sys, in.num("I"), in.num("A"), rho, cosPhi, x, limit / 100.0 * U));
        if (pct <= limit) {
            r.ok(cat(Text{"Spannungsfall ", "Voltage drop "}, numText(pct, 2, "%"), Text{" ≤ ", " ≤ "},
                     numText(limit, 1, "%"), Text{" – eingehalten.", " – OK."}));
        } else {
            r.error(cat(Text{"Spannungsfall ", "Voltage drop "}, numText(pct, 2, "%"), Text{" > ", " > "},
                        numText(limit, 1, "%"),
                        Text{" – Querschnitt erhöhen oder Länge verkürzen.", " – increase cross-section or shorten cable."}));
        }
        return r;
    };
    return c;
}

// ---- cable sizing -----------------------------------------------------------------------

Calculator cableSizing() {
    Calculator c;
    c.id = "cable_sizing";
    c.category = "cables";
    c.title = {"Leitungsdimensionierung", "Cable sizing"};
    c.summary = {"Querschnitt nach Strombelastbarkeit, Spannungsfall und Schutzorgan",
                 "Cross-section by ampacity, voltage drop and protective device"};
    c.description = cat(
        Text{"Gewählt wird der kleinste Kupfer-Querschnitt, der alle Bedingungen erfüllt:\n"
             "1) I_z = I_z0 · f_T · f_G ≥ I_B    2) ΔU ≤ ΔU_max    3) I_B ≤ I_n ≤ I_z und I_2 ≤ 1,45 · I_z\n"
             "f_T aus Umgebungstemperatur (Tab. B.52.14/15), f_G Häufung (Tab. B.52.17). "
             "Spannungsfall bei max. Leitertemperatur (70/90 °C), X' = 0,08 mΩ/m.\n",
             "The smallest copper cross-section meeting all conditions is selected:\n"
             "1) I_z = I_z0 · f_T · f_G ≥ I_B    2) ΔU ≤ ΔU_max    3) I_B ≤ I_n ≤ I_z and I_2 ≤ 1.45 · I_z\n"
             "f_T for ambient temperature (Tab. B.52.14/15), f_G grouping (Tab. B.52.17). "
             "Voltage drop at max. conductor temperature (70/90 °C), X' = 0.08 mΩ/m.\n"},
        kAmpacitySource);
    c.inputs = {
        Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac3"),
        Num::number("U", {"Nennspannung U", "Nominal voltage U"}, "V", 400).positive(),
        Num::number("IB", {"Betriebsstrom I_B", "Design current I_B"}, "A", 20).positive(),
        Num::number("L", {"Leitungslänge L (einfach)", "Cable length L (one way)"}, "m", 40).positive(),
        cosPhiInput(0.9),
        Num::number("dUmax", {"Zul. Spannungsfall ΔU max", "Permitted voltage drop ΔU max"}, "%", 3)
            .positive()
            .withHint({"DE typ. 3 %, CH 4 %", "DE typ. 3 %, CH 4 %"}),
        Num::choice("ins", {"Isolierung", "Insulation"}, insulationOptions(), "pvc"),
        Num::choice("method", {"Verlegeart", "Installation method"}, methodOptions(), "C"),
        Num::number("Ta", {"Umgebungstemperatur θ_a", "Ambient temperature θ_a"}, "°C", 30).range(-30, 85),
        Num::choice("arr", {"Häufung – Anordnung", "Grouping – arrangement"}, arrangementOptions(), "bunched"),
        Num::number("n", {"Anzahl Stromkreise in Häufung", "Number of grouped circuits"}, "", 1).range(1, 50),
        Num::choice("dev", {"Schutzorgan", "Protective device"}, deviceOptions(), "C"),
    };
    c.outputs = {Out::number("fT", {"Faktor Temperatur f_T", "Temperature factor f_T"}, "", 3),
                 Out::number("fG", {"Faktor Häufung f_G", "Grouping factor f_G"}, "", 2),
                 Out::number("IzReq", {"Erforderliches I_z0", "Required I_z0"}, "A", 1),
                 Out::number("Aamp", {"Min. Querschnitt (Belastbarkeit)", "Min. area (ampacity)"}, "mm²", 1),
                 Out::number("Avd", {"Min. Querschnitt (Spannungsfall)", "Min. area (voltage drop)"}, "mm²", 2),
                 Out::number("A", {"Gewählter Querschnitt", "Selected cross-section"}, "mm²", 1).primary(),
                 Out::number("Iz", {"Belastbarkeit I_z", "Current capacity I_z"}, "A", 1),
                 Out::number("In", {"Schutzorgan I_n", "Protective device I_n"}, "A", 0).primary(),
                 Out::number("dU", {"Spannungsfall", "Voltage drop"}, "%", 2),
                 Out::text("decisive", {"Maßgebend", "Decisive"})};
    c.keywords = {"querschnitt", "dimensionierung", "kabel", "leitung", "auslegung", "cable sizing", "cross section"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const auto ins = parseInsulation(in.choice("ins"));
        const auto method = parseMethod(in.choice("method"));
        const auto dev = parseDevice(in.choice("dev"));
        const int loaded = sys == System::AC3 ? 3 : 2;
        const int n = static_cast<int>(std::lround(in.num("n")));
        const double IB = in.num("IB");
        const double U = in.num("U");
        const double cosPhi = in.has("cosphi") ? in.num("cosphi") : 1.0;

        const double fT = ambientFactor(ins, method, in.num("Ta"));
        const double fG = groupingFactor(parseArrangement(in.choice("arr")), n);
        if (fT <= 0.0) {
            throw CalcError({"Umgebungstemperatur liegt über der zulässigen Leitertemperatur.",
                             "Ambient temperature exceeds the permitted conductor temperature."});
        }
        const double rho = resistivity(Material::Cu, maxConductorTemp(ins));
        const double x = 0.08e-3;
        const double dUmaxV = in.num("dUmax") / 100.0 * U;
        const double Avd = requiredArea(sys, in.num("L"), IB, rho, cosPhi, x, dUmaxV);

        r.set("fT", fT);
        r.set("fG", fG);
        r.set("IzReq", IB / (fT * fG));
        r.set("Avd", Avd);

        std::optional<double> Aamp;
        for (const double a : kAmpacityAreas) {
            if (baseAmpacity(ins, method, loaded, a).value() * fT * fG >= IB) {
                Aamp = a;
                break;
            }
        }
        if (Aamp) r.set("Aamp", *Aamp);

        for (const double a : kAmpacityAreas) {
            const double Iz = baseAmpacity(ins, method, loaded, a).value() * fT * fG;
            if (Iz < IB || a < Avd) continue;
            std::optional<double> In;
            for (const double rating : ratedCurrents(dev)) {
                if (rating >= IB && rating <= Iz && conventionalTripFactor(dev, rating) * rating <= 1.45 * Iz + 1e-9) {
                    In = rating;
                    break;
                }
            }
            if (!In) continue;
            const double dU = voltageDrop(sys, in.num("L"), IB, a, rho, cosPhi, x) / U * 100.0;
            r.set("A", a);
            r.set("Iz", Iz);
            r.set("In", *In);
            r.set("dU", dU);
            const auto aVd = nextAtLeast(kAmpacityAreas, Avd);
            const bool byAmp = Aamp && a == *Aamp;
            const bool byVd = aVd && a == *aVd;
            if (byAmp && byVd) {
                r.setText("decisive", {"Strombelastbarkeit und Spannungsfall", "Current capacity and voltage drop"});
            } else if (byAmp) {
                r.setText("decisive", {"Strombelastbarkeit", "Current capacity"});
            } else if (byVd) {
                r.setText("decisive", {"Spannungsfall", "Voltage drop"});
            } else {
                r.setText("decisive", {"Schutzorgan-Zuordnung", "Protective device coordination"});
            }
            r.ok(cat(Text{"Empfehlung: Cu ", "Recommendation: Cu "}, numText(a, 1, "mm²"),
                     Text{" mit Schutzorgan ", " with protective device "}, numText(*In, 0, "A"),
                     Text{" (I_B ≤ I_n ≤ I_z: ", " (I_B ≤ I_n ≤ I_z: "}, numText(IB, 1), Text{" ≤ ", " ≤ "},
                     numText(*In, 0), Text{" ≤ ", " ≤ "}, numText(Iz, 1), Text{" A).", " A)."}));
            r.info({"Abschaltbedingung (Schleifenimpedanz) und Kurzschlussschutz separat prüfen.",
                    "Check disconnection (loop impedance) and short-circuit protection separately."});
            return r;
        }
        r.error({"Kein Querschnitt bis 300 mm² erfüllt alle Bedingungen – Parallelleitungen, andere Verlegeart "
                 "oder größeren Spannungsfall prüfen.",
                 "No cross-section up to 300 mm² meets all conditions – consider parallel cables, another "
                 "installation method or a higher voltage drop."});
        return r;
    };
    return c;
}

Calculator ampacityCalc() {
    Calculator c;
    c.id = "ampacity";
    c.category = "cables";
    c.title = {"Strombelastbarkeit I_z", "Current-carrying capacity I_z"};
    c.summary = {"I_z nach Verlegeart, Temperatur und Häufung", "I_z by method, temperature and grouping"};
    c.description = cat(Text{"I_z = I_z0 · f_T · f_G\nf_T = √((θ_max − θ_a)/(θ_max − θ_ref))\n",
                             "I_z = I_z0 · f_T · f_G\nf_T = √((θ_max − θ_a)/(θ_max − θ_ref))\n"},
                        kAmpacitySource);
    c.inputs = {
        Num::choice("A", {"Querschnitt (Cu)", "Cross-section (Cu)"}, areaOptions(kAmpacityAreas), "2.5"),
        Num::choice("ins", {"Isolierung", "Insulation"}, insulationOptions(), "pvc"),
        Num::choice("method", {"Verlegeart", "Installation method"}, methodOptions(), "C"),
        Num::choice("loaded", {"Belastete Adern", "Loaded conductors"},
                    {{"2", {"2 (DC, 1~)", "2 (DC, 1~)"}}, {"3", {"3 (3~)", "3 (3~)"}}}, "3"),
        Num::number("Ta", {"Umgebungstemperatur θ_a", "Ambient temperature θ_a"}, "°C", 30).range(-30, 85),
        Num::choice("arr", {"Häufung – Anordnung", "Grouping – arrangement"}, arrangementOptions(), "bunched"),
        Num::number("n", {"Anzahl Stromkreise in Häufung", "Number of grouped circuits"}, "", 1).range(1, 50),
    };
    c.outputs = {Out::number("Iz0", {"Tabellenwert I_z0", "Table value I_z0"}, "A", 1),
                 Out::number("fT", {"Faktor Temperatur f_T", "Temperature factor f_T"}, "", 3),
                 Out::number("fG", {"Faktor Häufung f_G", "Grouping factor f_G"}, "", 2),
                 Out::number("Iz", {"Strombelastbarkeit I_z", "Current capacity I_z"}, "A", 1).primary()};
    c.keywords = {"strombelastbarkeit", "belastbarkeit", "verlegeart", "ampacity", "vde 0298", "häufung"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto ins = parseInsulation(in.choice("ins"));
        const auto method = parseMethod(in.choice("method"));
        const int loaded = in.choice("loaded") == "2" ? 2 : 3;
        const auto Iz0 = baseAmpacity(ins, method, loaded, parseDouble(in.choice("A")));
        if (!Iz0) throw CalcError({"Querschnitt nicht tabelliert.", "Cross-section not tabulated."});
        const double fT = ambientFactor(ins, method, in.num("Ta"));
        const double fG = groupingFactor(parseArrangement(in.choice("arr")), static_cast<int>(std::lround(in.num("n"))));
        r.set("Iz0", *Iz0);
        r.set("fT", fT);
        r.set("fG", fG);
        r.set("Iz", *Iz0 * fT * fG);
        if (in.num("n") > 20) r.warn({"Häufungsfaktoren nur bis 20 Stromkreise tabelliert.", "Grouping factors tabulated up to 20 circuits only."});
        return r;
    };
    return c;
}

Calculator overloadProtection() {
    Calculator c;
    c.id = "overload_protection";
    c.category = "cables";
    c.title = {"Überlastschutz (Koordination)", "Overload protection (coordination)"};
    c.summary = {"I_B ≤ I_n ≤ I_z und I_2 ≤ 1,45 · I_z", "I_B ≤ I_n ≤ I_z and I_2 ≤ 1.45 · I_z"};
    c.description = {
        "Bedingungen nach IEC 60364-4-43 / DIN VDE 0100-430:\n(1) I_B ≤ I_n ≤ I_z    (2) I_2 ≤ 1,45 · I_z\n"
        "I_2 = 1,45 · I_n für LS-Schalter (IEC 60898-1); gG: 2,1 (≤ 4 A), 1,9 (< 16 A), 1,6 (≥ 16 A) · I_n (IEC 60269-1).",
        "Conditions per IEC 60364-4-43:\n(1) I_B ≤ I_n ≤ I_z    (2) I_2 ≤ 1.45 · I_z\n"
        "I_2 = 1.45 · I_n for MCBs (IEC 60898-1); gG: 2.1 (≤ 4 A), 1.9 (< 16 A), 1.6 (≥ 16 A) · I_n (IEC 60269-1)."};
    c.inputs = {
        Num::number("IB", {"Betriebsstrom I_B", "Design current I_B"}, "A", 13).positive(),
        Num::number("Iz", {"Strombelastbarkeit I_z", "Current capacity I_z"}, "A", 20).positive(),
        Num::choice("dev", {"Schutzorgan", "Protective device"}, deviceOptions(), "B"),
    };
    c.outputs = {Out::number("In", {"Bemessungsstrom I_n", "Rated current I_n"}, "A", 0).primary(),
                 Out::number("I2", {"Auslösestrom I_2", "Conventional tripping current I_2"}, "A", 1),
                 Out::number("lim", {"Grenze 1,45 · I_z", "Limit 1.45 · I_z"}, "A", 1)};
    c.keywords = {"sicherung", "ls", "leitungsschutz", "überlast", "fuse", "mcb", "breaker"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto dev = parseDevice(in.choice("dev"));
        const double IB = in.num("IB");
        const double Iz = in.num("Iz");
        const auto In = nextAtLeast(ratedCurrents(dev), IB);
        if (!In) throw CalcError({"I_B größer als die größte Normstufe.", "I_B exceeds the largest standard rating."});
        const double I2 = conventionalTripFactor(dev, *In) * *In;
        r.set("In", *In);
        r.set("I2", I2);
        r.set("lim", 1.45 * Iz);
        if (*In > Iz) {
            r.error({"I_n > I_z – Leitung ist nicht geschützt. Querschnitt erhöhen.",
                     "I_n > I_z – cable not protected. Increase cross-section."});
        } else if (I2 > 1.45 * Iz + 1e-9) {
            r.error({"I_2 > 1,45 · I_z – Bedingung (2) verletzt.", "I_2 > 1.45 · I_z – condition (2) violated."});
        } else {
            r.ok({"Beide Bedingungen sind erfüllt.", "Both conditions are met."});
        }
        return r;
    };
    return c;
}

Calculator disconnection() {
    Calculator c;
    c.id = "disconnection";
    c.category = "cables";
    c.title = {"Abschaltbedingung / max. Länge (TN)", "Disconnection / max. length (TN)"};
    c.summary = {"Z_s,max und L_max für LS B/C/D", "Z_s,max and L_max for MCB B/C/D"};
    c.description = {
        "Abschaltbedingung TN (IEC 60364-4-41, 411.4.4): Z_s · I_a ≤ U_0\n"
        "Konventionelle Methode: L_max = 0,8 · U_0 · S_ph / (ρ · (1 + m) · I_a), m = S_ph / S_PE\n"
        "0,8: 80 % von U_0 am Anfang des Stromkreises; ρ = 0,0225 (Cu) bzw. 0,036 (Al) Ω·mm²/m.\n"
        "I_a = Sofortauslösung LS: B 5 · I_n, C 10 · I_n, D 20 · I_n. Bei Sicherungen I_a aus Kennlinie (0,4 s / 5 s).\n"
        "Ersetzt nicht die Messung der Schleifenimpedanz.",
        "Disconnection in TN systems (IEC 60364-4-41, 411.4.4): Z_s · I_a ≤ U_0\n"
        "Conventional method: L_max = 0.8 · U_0 · S_ph / (ρ · (1 + m) · I_a), m = S_ph / S_PE\n"
        "0.8: 80 % of U_0 at the origin of the circuit; ρ = 0.0225 (Cu) or 0.036 (Al) Ω·mm²/m.\n"
        "I_a = instantaneous trip of MCB: B 5 · I_n, C 10 · I_n, D 20 · I_n. For fuses take I_a from the curve.\n"
        "Does not replace measuring the loop impedance."};
    c.inputs = {
        Num::number("U0", {"Spannung gegen Erde U_0", "Voltage to earth U_0"}, "V", 230).positive(),
        Num::choice("dev", {"Schutzorgan", "Protective device"}, deviceOptions(true), "B"),
        Num::number("In", {"Bemessungsstrom I_n", "Rated current I_n"}, "A", 16).positive().showIf("dev", {"B", "C", "D"}),
        Num::number("Ia", {"Abschaltstrom I_a", "Disconnection current I_a"}, "A", 100).positive().showIf("dev", {"manual"}),
        Num::number("Sph", {"Querschnitt Außenleiter S_ph", "Phase cross-section S_ph"}, "mm²", 2.5).positive(),
        Num::number("Spe", {"Querschnitt Schutzleiter S_PE", "PE cross-section S_PE"}, "mm²", 2.5).positive(),
        Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
        Num::number("L", {"Geplante Länge", "Planned length"}, "m", kNaN).opt().positive(),
    };
    c.outputs = {Out::number("Ia", {"Abschaltstrom I_a", "Disconnection current I_a"}, "A", 0),
                 Out::number("Zs", {"Max. Schleifenimpedanz Z_s", "Max. loop impedance Z_s"}, "Ω", 3),
                 Out::number("Lmax", {"Max. Leitungslänge L_max", "Max. cable length L_max"}, "m", 1).primary()};
    c.keywords = {"schleifenimpedanz", "abschaltbedingung", "abschaltung", "tn", "loop impedance", "fehlerschutz"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& d = in.choice("dev");
        const double Ia = d == "manual" ? in.num("Ia") : instantaneousMultiple(parseDevice(d)) * in.num("In");
        const double U0 = in.num("U0");
        const double Lmax = maxLengthDisconnection(U0, in.num("Sph"), in.num("Spe"), parseMaterial(in.choice("mat")), Ia);
        r.set("Ia", Ia);
        r.set("Zs", U0 / Ia);
        r.set("Lmax", Lmax);
        if (const auto L = in.maybe("L")) {
            if (*L <= Lmax) {
                r.ok(cat(Text{"Länge ", "Length "}, numText(*L, 1, "m"), Text{" ≤ L_max – Abschaltbedingung erfüllt.",
                                                                              " ≤ L_max – disconnection condition met."}));
            } else {
                r.error(cat(Text{"Länge ", "Length "}, numText(*L, 1, "m"),
                            Text{" > L_max – Querschnitt erhöhen, RCD einsetzen oder Charakteristik B wählen.",
                                 " > L_max – increase cross-section, add RCD or use type B."}));
            }
        }
        return r;
    };
    return c;
}

Calculator maxLengthVdrop() {
    Calculator c;
    c.id = "max_length_vdrop";
    c.category = "cables";
    c.title = {"Max. Leitungslänge (Spannungsfall)", "Max. cable length (voltage drop)"};
    c.summary = {"z. B. 24-V-Feldgeräte, Stellantriebe", "e.g. 24 V field devices, actuators"};
    c.description = {"L_max = ΔU_max / (k · I · (ρ/A · cos φ + X' · sin φ))\nk = 2 bei DC/1~, √3 bei 3~.",
                     "L_max = ΔU_max / (k · I · (ρ/A · cos φ + X' · sin φ))\nk = 2 for DC/1~, √3 for 3~."};
    c.inputs = {
        Num::choice("sys", {"Netz", "System"}, systemOptions(), "dc"),
        Num::number("U", {"Nennspannung U", "Nominal voltage U"}, "V", 24).positive(),
        Num::number("I", {"Strom I", "Current I"}, "A", 2).positive(),
        Num::number("A", {"Querschnitt A", "Cross-section A"}, "mm²", 1.5).positive(),
        Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
        cosPhiInput(1.0),
        Num::number("T", {"Leitertemperatur θ", "Conductor temperature θ"}, "°C", 30).range(-40, 250),
        Num::number("dUmax", {"Zul. Spannungsfall ΔU max", "Permitted voltage drop ΔU max"}, "%", 5).positive(),
    };
    c.outputs = {Out::number("Lmax", {"Max. Leitungslänge (einfach)", "Max. cable length (one way)"}, "m", 1).primary(),
                 Out::number("dUV", {"Zul. Spannungsfall", "Permitted voltage drop"}, "V", 2),
                 Out::number("Umin", {"Min. Spannung am Gerät", "Min. voltage at device"}, "V", 2)};
    c.keywords = {"leitungslänge", "24v", "länge", "cable length", "feldgerät"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const double rho = resistivity(parseMaterial(in.choice("mat")), in.num("T"));
        const double dU = in.num("dUmax") / 100.0 * in.num("U");
        const double cosPhi = in.has("cosphi") ? in.num("cosphi") : 1.0;
        r.set("Lmax", maxLength(sys, in.num("I"), in.num("A"), rho, cosPhi, 0.08e-3, dU));
        r.set("dUV", dU);
        r.set("Umin", in.num("U") - dU);
        return r;
    };
    return c;
}

Calculator shortCircuitWithstand() {
    Calculator c;
    c.id = "short_circuit_withstand";
    c.category = "cables";
    c.title = {"Kurzschlussfestigkeit (I²t)", "Short-circuit withstand (I²t)"};
    c.summary = {"Zulässige Energie und Abschaltzeit einer Leitung", "Permissible energy and time of a cable"};
    c.description = {
        "Adiabatische Erwärmung (IEC 60364-4-43, 434.5.2): t = (k · S / I_k)²   bzw.   S ≥ √(I_k² · t) / k\n"
        "Zulässige Energie: (k · S)²   k nach Tab. 43A: Cu/PVC 115, Cu/VPE 143, Al/PVC 76, Al/VPE 94.\n"
        "Gültig für t ≤ 5 s. Für t < 0,1 s Durchlass-I²t des Schutzorgans (Herstellerangabe) vergleichen.",
        "Adiabatic heating (IEC 60364-4-43, 434.5.2): t = (k · S / I_k)²   or   S ≥ √(I_k² · t) / k\n"
        "Permissible energy: (k · S)²   k per Tab. 43A: Cu/PVC 115, Cu/XLPE 143, Al/PVC 76, Al/XLPE 94.\n"
        "Valid for t ≤ 5 s. For t < 0.1 s compare with the device's let-through I²t (manufacturer data)."};
    c.inputs = {
        Num::choice("type", {"Leiter / Isolierung", "Conductor / insulation"},
                    {{"cu_pvc", {"Cu / PVC (k = 115)", "Cu / PVC (k = 115)"}},
                     {"cu_xlpe", {"Cu / VPE (k = 143)", "Cu / XLPE (k = 143)"}},
                     {"al_pvc", {"Al / PVC (k = 76)", "Al / PVC (k = 76)"}},
                     {"al_xlpe", {"Al / VPE (k = 94)", "Al / XLPE (k = 94)"}}},
                    "cu_pvc"),
        Num::number("S", {"Querschnitt S", "Cross-section S"}, "mm²", 2.5).positive(),
        Num::number("Ik", {"Kurzschlussstrom I_k", "Short-circuit current I_k"}, "kA", 1).positive(),
        Num::number("t", {"Abschaltzeit t", "Disconnection time t"}, "s", 0.05).rangeExclusiveMin(0, 5),
    };
    c.outputs = {Out::number("k", {"Materialbeiwert k", "Material factor k"}, "A·√s/mm²", 0),
                 Out::number("I2t", {"Zulässige Energie (k·S)²", "Permissible energy (k·S)²"}, "A²s", 0),
                 Out::number("tmax", {"Max. Abschaltzeit bei I_k", "Max. disconnection time at I_k"}, "s", 3).primary(),
                 Out::number("Smin", {"Min. Querschnitt für I_k, t", "Min. cross-section for I_k, t"}, "mm²", 2)};
    c.keywords = {"kurzschluss", "i2t", "adiabatisch", "short circuit", "k-faktor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& t = in.choice("type");
        const auto mat = t.rfind("al", 0) == 0 ? Material::Al : Material::Cu;
        const auto ins = t.find("xlpe") != std::string::npos ? Insulation::XLPE : Insulation::PVC;
        const double k = kFactor(mat, ins);
        const double S = in.num("S");
        const double Ik = in.num("Ik") * 1e3;
        const double tmax = std::pow(k * S / Ik, 2);
        r.set("k", k);
        r.set("I2t", std::pow(k * S, 2));
        r.set("tmax", tmax);
        r.set("Smin", std::sqrt(Ik * Ik * in.num("t")) / k);
        if (in.num("t") <= tmax) {
            r.ok({"Leitung ist für I_k und t kurzschlussfest.", "Cable withstands I_k for time t."});
        } else {
            r.error({"Abschaltzeit zu lang – Querschnitt erhöhen oder schnelleres Schutzorgan.",
                     "Disconnection too slow – increase cross-section or use a faster device."});
        }
        return r;
    };
    return c;
}

Calculator conduitFill() {
    Calculator c;
    c.id = "conduit_fill";
    c.category = "cables";
    c.title = {"Rohr-/Kanalfüllung", "Conduit fill"};
    c.summary = {"Füllgrad eines Installationsrohres", "Fill ratio of a conduit"};
    c.description = {
        "Füllgrad = n · d² / D_i²  (Querschnittsflächen)\n"
        "Richtwerte (u. a. NEC Chapter 9): 1 Leitung 53 %, 2 Leitungen 31 %, ab 3 Leitungen 40 %. "
        "In DE/CH gilt: Leitungen müssen sich ohne Beschädigung einziehen lassen – Herstellerangaben beachten.",
        "Fill = n · d² / D_i²  (cross-sectional areas)\n"
        "Guide values (e.g. NEC Chapter 9): 1 cable 53 %, 2 cables 31 %, 3 or more 40 %. "
        "Cables must be installable without damage – follow manufacturer data."};
    c.inputs = {
        Num::number("D", {"Innendurchmesser Rohr D_i", "Conduit inner diameter D_i"}, "mm", 25).positive(),
        Num::number("d", {"Außendurchmesser Leitung d", "Cable outer diameter d"}, "mm", 8.5).positive(),
        Num::number("n", {"Anzahl Leitungen", "Number of cables"}, "", 2).range(1, 500),
    };
    c.outputs = {Out::number("fill", {"Füllgrad", "Fill ratio"}, "%", 1).primary(),
                 Out::number("limit", {"Richtwert", "Guide value"}, "%", 0),
                 Out::number("nmax", {"Max. Anzahl bei 40 %", "Max. number at 40 %"}, "", 0)};
    c.keywords = {"rohr", "füllung", "kanal", "conduit", "fill"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double D = in.num("D");
        const double d = in.num("d");
        const double n = std::round(in.num("n"));
        if (d >= D) throw CalcError({"Leitung passt nicht in das Rohr.", "Cable does not fit into the conduit."});
        const double fill = n * d * d / (D * D) * 100.0;
        const double limit = n <= 1 ? 53.0 : (n <= 2 ? 31.0 : 40.0);
        r.set("fill", fill);
        r.set("limit", limit);
        r.set("nmax", std::floor(0.40 * D * D / (d * d)));
        if (fill <= limit) {
            r.ok({"Füllgrad im Richtwert.", "Fill ratio within guide value."});
        } else {
            r.warn({"Füllgrad über Richtwert – größeres Rohr wählen.", "Fill ratio above guide value – use a larger conduit."});
        }
        return r;
    };
    return c;
}

}  // namespace

void addCables(std::vector<Calculator>& out) {
    out.push_back(cableSizing());
    out.push_back(voltageDropCalc());
    out.push_back(ampacityCalc());
    out.push_back(overloadProtection());
    out.push_back(disconnection());
    out.push_back(maxLengthVdrop());
    out.push_back(shortCircuitWithstand());
    out.push_back(conduitFill());
}

}  // namespace elc::catalog
