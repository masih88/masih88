// Calculators: Reinraum & Pharma / Cleanroom & pharma
#include <array>
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/cleanroom.hpp"

namespace elc::catalog {

using namespace elc::cleanroom;

namespace {

std::vector<Option> particleSizeOptions() {
    return {{"0.1", "≥ 0,1 µm"}, {"0.2", "≥ 0,2 µm"}, {"0.3", "≥ 0,3 µm"},
            {"0.5", "≥ 0,5 µm"}, {"1", "≥ 1 µm"},    {"5", "≥ 5 µm"}};
}

Calculator isoClass() {
    Calculator c;
    c.id = "iso14644_class";
    c.category = "cleanroom";
    c.title = {"Reinraumklasse ISO 14644-1", "Cleanroom class ISO 14644-1"};
    c.summary = {"Grenzwert je Klasse ↔ Klasse aus Messwert", "Class limit ↔ class from measurement"};
    c.description = {
        "C_n = 10^N · (0,1 / D)^2,08   [Partikel/m³]   (ISO 14644-1:2015, Formel A.1)\n"
        "N = log₁₀(C_n) + 2,08 · log₁₀(D / 0,1)\n"
        "Tabellenwerte sind auf ganze Zahlen mit max. 3 signifikanten Stellen gerundet. Zwischenklassen in 0,1-Schritten "
        "zulässig. Für ≥ 5 µm bei ISO 5 und darunter gilt das Makropartikel-Verfahren (M-Deskriptor).",
        "C_n = 10^N · (0.1 / D)^2.08   [particles/m³]   (ISO 14644-1:2015, formula A.1)\n"
        "N = log₁₀(C_n) + 2.08 · log₁₀(D / 0.1)\n"
        "Table values are rounded to whole numbers with at most 3 significant figures. Intermediate classes in 0.1 steps "
        "are allowed. For ≥ 5 µm at ISO 5 and cleaner use the macroparticle descriptor (M)."};
    c.inputs = {
        Num::choice("dir", {"Gegeben", "Given"},
                    {{"class", {"ISO-Klasse → Grenzwert", "ISO class → limit"}},
                     {"conc", {"Messwert → ISO-Klasse", "Measured value → ISO class"}}},
                    "class"),
        Num::number("N", {"ISO-Klasse N", "ISO class N"}, "", 7).range(1, 9).showIf("dir", {"class"}),
        Num::number("C", {"Gemessene Konzentration", "Measured concentration"}, "1/m³", 250000).positive().showIf("dir", {"conc"}),
        Num::choice("D", {"Partikelgröße", "Particle size"}, particleSizeOptions(), "0.5"),
    };
    c.outputs = {Out::number("limit", {"Max. Konzentration (Tabelle)", "Max. concentration (table)"}, "1/m³", 0).primary(),
                 Out::number("limitFt3", {"Max. Konzentration", "Max. concentration"}, "1/ft³", 1),
                 Out::number("limitL", {"Max. Konzentration", "Max. concentration"}, "1/l", 3),
                 Out::number("Nexact", {"Klassenzahl (exakt)", "Class number (exact)"}, "", 2),
                 Out::text("iso", {"ISO-Klasse", "ISO class"})};
    c.keywords = {"reinraum", "iso 14644", "partikel", "cleanroom", "klasse", "gmp", "exyte"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double D = parseDouble(in.choice("D"));
        if (in.choice("dir") == "conc") {
            const double N = classFromConcentration(in.num("C"), D);
            const double cls = std::ceil(N * 10.0 - 1e-9) / 10.0;  // next 0.1 step
            r.set("Nexact", N);
            r.setText("iso", cat(Text{"ISO ", "ISO "}, formatNumber(std::max(cls, 1.0), 1),
                                 Text{" (bei ", " (at "}, formatNumber(D, 1), Text{" µm)", " µm)"}));
            if (N > 9) r.warn({"Über ISO 9 – kein Reinraum im Sinne der Norm.", "Above ISO 9 – not a cleanroom per the standard."});
            if (N < 1) r.info({"Unter ISO 1.", "Below ISO 1."});
            return r;
        }
        const double N = in.num("N");
        const bool whole = std::fabs(N - std::round(N)) < 1e-9;
        const double limit = whole ? classLimit(N, D) : std::round(classLimitExact(N, D));
        r.set("limit", limit);
        r.set("limitFt3", limit * 0.028316846592);
        r.set("limitL", limit / 1000.0);
        r.set("Nexact", N);
        r.setText("iso", cat(Text{"ISO ", "ISO "}, formatNumber(N, 1)));
        if (whole && !classLimitTabulated(static_cast<int>(std::lround(N)), D)) {
            r.warn({"Für diese Klasse/Partikelgröße enthält ISO 14644-1 Tabelle 1 keinen Wert (nicht anwendbar).",
                    "ISO 14644-1 Table 1 gives no value for this class/size (not applicable)."});
        }
        return r;
    };
    return c;
}

Calculator sampling() {
    Calculator c;
    c.id = "iso14644_sampling";
    c.category = "cleanroom";
    c.title = {"Messstellen & Probenvolumen", "Sampling locations & volume"};
    c.summary = {"Klassifizierungsmessung nach ISO 14644-1", "Classification sampling per ISO 14644-1"};
    c.description = {
        "Mindestanzahl Messstellen N_L nach ISO 14644-1:2015 Tabelle A.1 (Fläche in m²), über 1000 m²: N_L = 27 · A / 1000.\n"
        "Mindest-Probenvolumen je Messstelle: V_s = 20 / C_n,m · 1000 [l], C_n,m = Grenzwert der größten betrachteten "
        "Partikelgröße. Mindestens 2 l und mindestens 1 min Messzeit.",
        "Minimum sampling locations N_L per ISO 14644-1:2015 Table A.1 (area in m²); above 1000 m²: N_L = 27 · A / 1000.\n"
        "Minimum single sample volume: V_s = 20 / C_n,m · 1000 [l], C_n,m = class limit of the largest considered "
        "particle size. At least 2 l and at least 1 min sampling time."};
    c.inputs = {
        Num::number("A", {"Reinraumfläche", "Cleanroom area"}, "m²", 120).positive(),
        Num::number("N", {"ISO-Klasse", "ISO class"}, "", 7).range(1, 9),
        Num::choice("D", {"Größte betrachtete Partikelgröße", "Largest considered particle size"}, particleSizeOptions(), "5"),
        Num::number("flow", {"Volumenstrom Partikelzähler", "Particle counter flow rate"}, "l/min", 28.3).positive()
            .withHint({"1 cfm = 28,3 l/min, 50 l/min, 100 l/min", "1 cfm = 28.3 l/min, 50 l/min, 100 l/min"}),
    };
    c.outputs = {Out::number("NL", {"Mindestanzahl Messstellen", "Minimum sampling locations"}, "", 0).primary(),
                 Out::number("Vs", {"Mindest-Probenvolumen je Stelle", "Minimum sample volume per location"}, "l", 1),
                 Out::number("t", {"Mindest-Messzeit je Stelle", "Minimum sampling time per location"}, "min", 1),
                 Out::number("ttotal", {"Messzeit gesamt (ohne Umsetzen)", "Total sampling time (excl. moving)"}, "min", 0)};
    c.keywords = {"messstellen", "probenahme", "partikelzähler", "sampling", "iso 14644", "qualifizierung"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const int NL = samplingLocations(in.num("A"));
        const double limit = classLimit(in.num("N"), parseDouble(in.choice("D")));
        const double Vs = sampleVolumeLitres(limit);
        const double t = std::max(1.0, Vs / in.num("flow"));
        r.set("NL", NL);
        r.set("Vs", Vs);
        r.set("t", t);
        r.set("ttotal", NL * t);
        if (limit < 20) {
            r.warn({"Sehr kleiner Grenzwert – ggf. sequenzielles Probenahmeverfahren (ISO 14644-1 Anhang D).",
                    "Very low limit – consider sequential sampling (ISO 14644-1 Annex D)."});
        }
        return r;
    };
    return c;
}

Calculator airChange() {
    Calculator c;
    c.id = "air_change";
    c.category = "cleanroom";
    c.title = {"Luftwechsel & Erholzeit", "Air change & recovery time"};
    c.summary = {"Luftwechselrate, Volumenstrom, 100:1-Erholzeit", "ACH, airflow, 100:1 recovery time"};
    c.description = {
        "n = V̇ / V_Raum [1/h]    Erholzeit bei idealer Mischung: t = ln(k) / n\n"
        "100:1 (Erholungstest ISO 14644-3): t = 4,61 / n [h] = 276 / n [min]. Reale Räume haben eine geringere "
        "Lüftungseffektivität ε – gemessene Werte sind meist länger. EU-GMP Annex 1: Erholzeit Klasse B/C „at rest“ "
        "in der Regel ≤ 15–20 min.",
        "n = V̇ / V_room [1/h]    Recovery with ideal mixing: t = ln(k) / n\n"
        "100:1 (recovery test ISO 14644-3): t = 4.61 / n [h] = 276 / n [min]. Real rooms have a lower ventilation "
        "effectiveness ε – measured values are usually longer. EU GMP Annex 1: recovery to \"at rest\" typically ≤ 15–20 min."};
    c.inputs = {
        Num::number("A", {"Raumfläche", "Room area"}, "m²", 50).positive(),
        Num::number("H", {"Raumhöhe", "Room height"}, "m", 3).positive(),
        Num::choice("dir", {"Gegeben", "Given"},
                    {{"flow", {"Zuluft-Volumenstrom", "Supply airflow"}}, {"ach", {"Luftwechselrate", "Air change rate"}}}, "flow"),
        Num::number("Q", {"Zuluft-Volumenstrom", "Supply airflow"}, "m³/h", 3000).positive().showIf("dir", {"flow"}),
        Num::number("n", {"Luftwechselrate", "Air change rate"}, "1/h", 20).positive().showIf("dir", {"ach"}),
        Num::number("eps", {"Lüftungseffektivität ε", "Ventilation effectiveness ε"}, "", 1.0).rangeExclusiveMin(0, 2),
    };
    c.outputs = {Out::number("n", {"Luftwechselrate", "Air change rate"}, "1/h", 1).primary(),
                 Out::number("Q", {"Zuluft-Volumenstrom", "Supply airflow"}, "m³/h", 0),
                 Out::number("V", {"Raumvolumen", "Room volume"}, "m³", 1),
                 Out::number("t100", {"Erholzeit 100:1", "Recovery time 100:1"}, "min", 1).primary(),
                 Out::number("t10", {"Erholzeit 10:1", "Recovery time 10:1"}, "min", 1)};
    c.keywords = {"luftwechsel", "erholzeit", "recovery", "ach", "reinraum", "lüftung"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double V = in.num("A") * in.num("H");
        const double n = in.choice("dir") == "ach" ? in.num("n") : in.num("Q") / V;
        const double nEff = n * in.num("eps");
        r.set("V", V);
        r.set("n", n);
        r.set("Q", n * V);
        r.set("t100", recoveryTimeHours(nEff, 100) * 60.0);
        r.set("t10", recoveryTimeHours(nEff, 10) * 60.0);
        if (recoveryTimeHours(nEff, 100) * 60.0 > 20.0) {
            r.warn({"Erholzeit 100:1 über 20 min – für GMP-Bereiche Luftwechsel prüfen.",
                    "100:1 recovery above 20 min – check air change for GMP areas."});
        }
        return r;
    };
    return c;
}

Calculator ffuCeiling() {
    Calculator c;
    c.id = "ffu_ceiling";
    c.category = "cleanroom";
    c.title = {"FFU-Decke (Filter-Fan-Units)", "FFU ceiling (fan filter units)"};
    c.summary = {"Anzahl FFU, Luftmenge, Leistung, Energiekosten", "FFU count, airflow, power, energy cost"};
    c.description = {
        "n_FFU = ⌈A_Raum · Belegung / A_FFU⌉    V̇ = n_FFU · A_FFU · v · 3600\n"
        "Luftwechsel = V̇ / (A · H)    P_el = n_FFU · P_FFU (geht als Wärme in den Raum!)\n"
        "Richtwert turbulenzarme Verdrängungsströmung (Klasse A/ISO 5): v ≈ 0,36–0,54 m/s (früherer EU-GMP-Leitwert 0,45 m/s ± 20 %).",
        "n_FFU = ⌈A_room · coverage / A_FFU⌉    V̇ = n_FFU · A_FFU · v · 3600\n"
        "Air change = V̇ / (A · H)    P_el = n_FFU · P_FFU (all of it ends up as heat in the room!)\n"
        "Guide value unidirectional flow (grade A/ISO 5): v ≈ 0.36–0.54 m/s (former EU GMP guidance 0.45 m/s ± 20 %)."};
    c.inputs = {
        Num::number("A", {"Reinraumfläche", "Cleanroom area"}, "m²", 100).positive(),
        Num::number("H", {"Raumhöhe", "Room height"}, "m", 3).positive(),
        Num::number("cov", {"Deckenbelegung", "Ceiling coverage"}, "%", 25).rangeExclusiveMin(0, 100),
        Num::choice("size", {"FFU-Größe (Nennmaß)", "FFU size (nominal)"},
                    {{"1200x600", "1200 × 600 mm"}, {"1200x1200", "1200 × 1200 mm"}, {"600x600", "600 × 600 mm"},
                     {"1500x1200", "1500 × 1200 mm"}},
                    "1200x600"),
        Num::number("v", {"Austrittsgeschwindigkeit", "Face velocity"}, "m/s", 0.45).positive(),
        Num::number("P", {"Leistung je FFU", "Power per FFU"}, "W", 120).nonNegative()
            .withHint({"EC-FFU 1200×600 bei 0,45 m/s typ. 80–180 W (Datenblatt)", "EC FFU 1200×600 at 0.45 m/s typ. 80–180 W (datasheet)"}),
        Num::number("h", {"Betriebsstunden pro Jahr", "Operating hours per year"}, "h", 8760).range(0, 8784),
        Num::number("price", {"Strompreis pro kWh", "Energy price per kWh"}, "€ / CHF", 0.25).nonNegative(),
    };
    c.outputs = {Out::number("n", {"Anzahl FFU", "Number of FFUs"}, "", 0).primary(),
                 Out::number("Q", {"Luftvolumenstrom gesamt", "Total airflow"}, "m³/h", 0),
                 Out::number("ach", {"Luftwechselrate", "Air change rate"}, "1/h", 0).primary(),
                 Out::number("Pel", {"Elektrische Leistung / Wärmelast", "Electrical power / heat load"}, "kW", 2),
                 Out::number("E", {"Energie pro Jahr", "Energy per year"}, "MWh", 1),
                 Out::number("cost", {"Energiekosten pro Jahr", "Energy cost per year"}, "€", 0)};
    c.keywords = {"ffu", "filter fan unit", "laminar flow", "reinraumdecke", "fan filter", "exyte"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const auto& s = in.choice("size");
        const double aFfu = s == "1200x1200" ? 1.44 : (s == "600x600" ? 0.36 : (s == "1500x1200" ? 1.8 : 0.72));
        const double n = std::ceil(in.num("A") * in.num("cov") / 100.0 / aFfu - 1e-9);
        const double Q = n * aFfu * in.num("v") * 3600.0;
        const double Pel = n * in.num("P") / 1000.0;
        r.set("n", n);
        r.set("Q", Q);
        r.set("ach", Q / (in.num("A") * in.num("H")));
        r.set("Pel", Pel);
        r.set("E", Pel * in.num("h") / 1000.0);
        r.set("cost", Pel * in.num("h") * in.num("price"), ctx.norm == NormProfile::CH ? "CHF" : "€");
        if (in.num("v") < 0.36 || in.num("v") > 0.54) {
            r.info({"Geschwindigkeit außerhalb 0,36–0,54 m/s – für Verdrängungsströmung begründen.",
                    "Velocity outside 0.36–0.54 m/s – justify for unidirectional flow."});
        }
        return r;
    };
    return c;
}

Calculator overflow() {
    Calculator c;
    c.id = "pressure_cascade";
    c.category = "cleanroom";
    c.title = {"Überströmung / Druckkaskade", "Overflow / pressure cascade"};
    c.summary = {"Leckluft durch Türspalte bei Δp", "Leakage air through door gaps at Δp"};
    c.description = {
        "Q = C_d · A · √(2 · Δp / ρ)    (Blendengleichung, C_d ≈ 0,61 für scharfkantige Spalte)\n"
        "A = Spaltlänge · Spaltbreite + weitere Öffnungen. Das Ergebnis ist die Überströmluft, die die Lüftung "
        "zusätzlich liefern muss, um Δp zu halten.\n"
        "EU-GMP Annex 1: Druckdifferenz zwischen angrenzenden Räumen unterschiedlicher Klassen typ. 10–15 Pa (Leitwert).",
        "Q = C_d · A · √(2 · Δp / ρ)    (orifice equation, C_d ≈ 0.61 for sharp-edged gaps)\n"
        "A = gap length · gap width + other openings. The result is the overflow air the HVAC must supply in addition "
        "to keep Δp.\nEU GMP Annex 1: pressure differential between adjacent rooms of different grades typ. 10–15 Pa (guidance)."};
    c.inputs = {
        Num::number("L", {"Spaltlänge gesamt (z. B. Türumfang)", "Total gap length (e.g. door perimeter)"}, "m", 6.2).nonNegative(),
        Num::number("s", {"Spaltbreite", "Gap width"}, "mm", 3).nonNegative(),
        Num::number("Ax", {"Weitere Öffnungen", "Other openings"}, "cm²", 0).nonNegative(),
        Num::number("dp", {"Druckdifferenz Δp", "Pressure differential Δp"}, "Pa", 15).positive(),
        Num::number("cd", {"Durchflussbeiwert C_d", "Discharge coefficient C_d"}, "", 0.61).rangeExclusiveMin(0, 1),
    };
    c.outputs = {Out::number("A", {"Leckfläche", "Leakage area"}, "cm²", 1),
                 Out::number("Q", {"Überströmluft", "Overflow airflow"}, "m³/h", 0).primary(),
                 Out::number("v", {"Geschwindigkeit im Spalt", "Velocity in gap"}, "m/s", 2)};
    c.keywords = {"druckkaskade", "überströmung", "leckluft", "türspalt", "pressure", "overflow", "druckhaltung"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double A = in.num("L") * in.num("s") / 1000.0 + in.num("Ax") / 1e4;
        if (A <= 0) throw CalcError({"Keine Öffnungsfläche angegeben.", "No opening area given."});
        const double q = leakageFlow(A, in.num("dp"), in.num("cd"));
        r.set("A", A * 1e4);
        r.set("Q", q * 3600.0);
        r.set("v", q / A);
        if (in.num("dp") < 10 || in.num("dp") > 15) {
            r.info({"Δp außerhalb des GMP-Leitwerts 10–15 Pa.", "Δp outside GMP guidance of 10–15 Pa."});
        }
        return r;
    };
    return c;
}

// ---- tables ---------------------------------------------------------------------------------

Table isoClassTable() {
    Table t;
    t.id = "iso14644_table";
    t.category = "cleanroom";
    t.title = {"ISO 14644-1: Klassengrenzwerte", "ISO 14644-1: class limits"};
    t.summary = {"Max. Partikel/m³ ≥ Partikelgröße", "Max. particles/m³ ≥ particle size"};
    t.description = {"ISO 14644-1:2015 Tabelle 1, berechnet mit C_n = 10^N · (0,1/D)^2,08 und gerundet. „–“ = nicht anwendbar.",
                     "ISO 14644-1:2015 Table 1, calculated with C_n = 10^N · (0.1/D)^2.08 and rounded. \"–\" = not applicable."};
    t.columns = {{"Klasse", "Class"}, "0,1 µm", "0,2 µm", "0,3 µm", "0,5 µm", "1 µm", "5 µm"};
    for (int n = 1; n <= 9; ++n) {
        std::vector<Text> row{Text{"ISO " + std::to_string(n)}};
        for (const double d : {0.1, 0.2, 0.3, 0.5, 1.0, 5.0}) {
            if (!classLimitTabulated(n, d)) {
                row.emplace_back("–");
                continue;
            }
            // thousands separator for readability: 3 520 000
            std::string s = std::to_string(std::llround(classLimit(n, d)));
            for (int pos = static_cast<int>(s.size()) - 3; pos > 0; pos -= 3) s.insert(static_cast<std::size_t>(pos), " ");
            row.emplace_back(s);
        }
        t.rows.push_back(std::move(row));
    }
    return t;
}

Table gmpParticles() {
    Table t;
    t.id = "gmp_annex1_particles";
    t.category = "cleanroom";
    t.title = {"EU-GMP Annex 1: Partikelgrenzwerte", "EU GMP Annex 1: particle limits"};
    t.summary = {"Klassen A–D, Ruhe- und Betriebszustand", "Grades A–D, at rest and in operation"};
    t.description = {
        "EU-GMP-Leitfaden Anhang 1 (2022), Tabelle 1/5 – max. zulässige Partikel/m³. n. v. = nicht vorgegeben "
        "(Klasse A/B: ≥ 5 µm im Ruhezustand nur über Trend). ISO-Entsprechung: A ≈ ISO 5, B ≈ ISO 5 (Ruhe) / ISO 7 (Betrieb), "
        "C ≈ ISO 7 / ISO 8, D ≈ ISO 8 (Ruhe). Gegen die gültige Fassung prüfen.",
        "EU GMP Guide Annex 1 (2022), Table 1/5 – max. permitted particles/m³. n/s = not specified. "
        "ISO equivalent: A ≈ ISO 5, B ≈ ISO 5 (at rest) / ISO 7 (in operation), C ≈ ISO 7 / ISO 8, D ≈ ISO 8 (at rest). "
        "Check against the version in force."};
    t.columns = {{"Klasse", "Grade"}, {"Ruhe ≥ 0,5 µm", "At rest ≥ 0.5 µm"}, {"Ruhe ≥ 5 µm", "At rest ≥ 5 µm"},
                 {"Betrieb ≥ 0,5 µm", "In operation ≥ 0.5 µm"}, {"Betrieb ≥ 5 µm", "In operation ≥ 5 µm"}};
    const Text ns{"n. v.", "n/s"};
    t.rows = {{"A", "3 520", ns, "3 520", "29"},
              {"B", "3 520", ns, "352 000", "2 930"},
              {"C", "352 000", "2 930", "3 520 000", "29 300"},
              {"D", "3 520 000", "29 300", ns, ns}};
    return t;
}

Table gmpMicrobial() {
    Table t;
    t.id = "gmp_annex1_microbial";
    t.category = "cleanroom";
    t.title = {"EU-GMP Annex 1: Keimgrenzwerte", "EU GMP Annex 1: microbial limits"};
    t.summary = {"KBE in Luft, auf Sedimentations- und Kontaktplatten", "CFU in air, settle and contact plates"};
    t.description = {"EU-GMP-Leitfaden Anhang 1 (2022), Tabelle 6 – Aktionsgrenzen im Betrieb. Klasse A: kein Wachstum erwartet.",
                     "EU GMP Guide Annex 1 (2022), Table 6 – action limits in operation. Grade A: no growth expected."};
    t.columns = {{"Klasse", "Grade"}, {"Luft KBE/m³", "Air CFU/m³"}, {"Sedimentation KBE/4 h", "Settle plates CFU/4 h"},
                 {"Kontakt KBE/Platte", "Contact CFU/plate"}, {"Handschuh KBE/Hand", "Glove CFU/glove"}};
    const Text none{"kein Wachstum", "no growth"};
    t.rows = {{"A", none, none, none, none}, {"B", "10", "5", "5", "5"}, {"C", "100", "50", "25", "–"}, {"D", "200", "100", "50", "–"}};
    return t;
}

}  // namespace

void addCleanroom(std::vector<Calculator>& out) {
    out.push_back(isoClass());
    out.push_back(sampling());
    out.push_back(airChange());
    out.push_back(ffuCeiling());
    out.push_back(overflow());
}

void addCleanroomTables(std::vector<Table>& out) {
    out.push_back(isoClassTable());
    out.push_back(gmpParticles());
    out.push_back(gmpMicrobial());
}

}  // namespace elc::catalog
