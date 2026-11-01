// Building services & energy: calculators and tables taken from the user's technical books (v1.5).
// Every description names the book (publisher, edition, page) and asks to check the edition in force.
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/acoustics.hpp"
#include "elc/formulas/building.hpp"
#include "elc/formulas/heating.hpp"

namespace elc::catalog {

namespace {

// ------------------------------------------------------------------ GA efficiency class (EN ISO 52120)

std::vector<Option> buildingTypeOptions() {
    return {{"office", {"Büro", "Office"}},
            {"lecture", {"Hörsaal", "Lecture hall"}},
            {"school", {"Bildungseinrichtung (Schule)", "Education (school)"}},
            {"hospital", {"Krankenhaus", "Hospital"}},
            {"hotel", {"Hotel", "Hotel"}},
            {"restaurant", {"Restaurant", "Restaurant"}},
            {"retail", {"Groß- und Einzelhandel", "Wholesale and retail"}},
            {"other", {"Sport, Lager, Industrie u. a.", "Sports, storage, industry etc."}},
            {"residential", {"Wohngebäude", "Residential"}}};
}

bac::BuildingType parseBuildingType(const std::string& v) {
    using T = bac::BuildingType;
    if (v == "lecture") return T::LectureHall;
    if (v == "school") return T::School;
    if (v == "hospital") return T::Hospital;
    if (v == "hotel") return T::Hotel;
    if (v == "restaurant") return T::Restaurant;
    if (v == "retail") return T::Retail;
    if (v == "other") return T::Other;
    if (v == "residential") return T::Residential;
    return T::Office;
}

std::vector<Option> bacClassOptions() {
    return {{"D", {"D – nicht energieeffizient", "D – non energy efficient"}},
            {"C", {"C – Standard (Referenz)", "C – standard (reference)"}},
            {"B", {"B – erhöhte Energieeffizienz", "B – advanced"}},
            {"A", {"A – hohe Energieeffizienz", "A – high energy performance"}}};
}

const char* kBacSource =
    "Quelle: Siemens „Gebäudeautomation – Einfluss auf Energie-Effizienz von Gebäuden, Anwendung der EN ISO 52120“ "
    "(09/2022), Kap. 4.4, S. 124–125. Gegen die gültige Ausgabe der EN ISO 52120-1 prüfen.";
const char* kBacSourceEn =
    "Source: Siemens “Building automation – impact on energy efficiency, application of EN ISO 52120” "
    "(09/2022), ch. 4.4, p. 124–125. Check against the edition of EN ISO 52120-1 in force.";

Calculator bacEfficiency() {
    Calculator c;
    c.id = "bac_efficiency";
    c.category = "building";
    c.title = {"GA-Effizienzklasse (EN ISO 52120)", "BAC efficiency class (EN ISO 52120)"};
    c.summary = {"Einsparung durch bessere Gebäudeautomation – BAC-Faktoren je Gebäudetyp",
                 "Savings from better building automation – BAC factors per building type"};
    c.description = {std::string("Energiebedarf neu = Energiebedarf alt · f_BAC,neu / f_BAC,alt;  Einsparung = 1 − f_neu / f_alt\n"
                                 "f_BAC,th: Heizen und Kühlen; f_BAC,el: Beleuchtung und Hilfsenergie (ohne sonstige Geräte).\n"
                                 "Klasse C = 1 (Standard-Referenz), D schlechter, B und A besser.\n") + kBacSource,
                     std::string("Energy new = energy old · f_BAC,new / f_BAC,old;  saving = 1 − f_new / f_old\n"
                                 "f_BAC,th: heating and cooling; f_BAC,el: lighting and auxiliary energy (no other appliances).\n"
                                 "Class C = 1 (standard reference), D worse, B and A better.\n") + kBacSourceEn};
    c.inputs = {
        Num::choice("type", {"Gebäudetyp", "Building type"}, buildingTypeOptions(), "office"),
        Num::choice("from", {"GA-Klasse heute", "BAC class today"}, bacClassOptions(), "D"),
        Num::choice("to", {"GA-Klasse geplant", "BAC class planned"}, bacClassOptions(), "B"),
        Num::number("eth", {"Thermischer Energiebedarf heute", "Thermal energy today"}, "kWh/a", 200000)
            .nonNegative()
            .startsSection({"Verbrauch & Preise", "Consumption & prices"}),
        Num::number("eel", {"Elektrischer Energiebedarf heute", "Electric energy today"}, "kWh/a", 50000).nonNegative(),
        Num::number("pth", {"Preis Wärme/Kälte", "Price heat/cooling"}, kCurrencyUnit, 0.12).nonNegative().withHint({"je kWh", "per kWh"}),
        Num::number("pel", {"Preis Strom", "Price electricity"}, kCurrencyUnit, 0.30).nonNegative().withHint({"je kWh", "per kWh"}),
    };
    c.outputs = {Out::number("fth1", {"f_BAC,th heute", "f_BAC,th today"}, "", 2),
                 Out::number("fth2", {"f_BAC,th geplant", "f_BAC,th planned"}, "", 2),
                 Out::number("sth", {"Einsparung thermisch", "Saving thermal"}, "%", 1).primary(),
                 Out::number("eth2", {"Thermischer Bedarf geplant", "Thermal energy planned"}, "kWh/a", 0),
                 Out::number("fel1", {"f_BAC,el heute", "f_BAC,el today"}, "", 2),
                 Out::number("fel2", {"f_BAC,el geplant", "f_BAC,el planned"}, "", 2),
                 Out::number("sel", {"Einsparung elektrisch", "Saving electric"}, "%", 1),
                 Out::number("eel2", {"Elektrischer Bedarf geplant", "Electric energy planned"}, "kWh/a", 0),
                 Out::number("cost", {"Kosteneinsparung pro Jahr", "Cost saving per year"}, "€", 0).primary()};
    c.keywords = {"en 15232", "en iso 52120", "bac", "ga-faktor", "effizienzklasse", "gebäudeautomation", "einsparung",
                  "energieeffizienz", "geg", "building automation"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto type = parseBuildingType(in.choice("type"));
        const char from = in.choice("from").empty() ? 'D' : in.choice("from")[0];
        const char to = in.choice("to").empty() ? 'B' : in.choice("to")[0];
        const double fth1 = bac::factor(type, bac::Energy::Thermal, from);
        const double fth2 = bac::factor(type, bac::Energy::Thermal, to);
        const double fel1 = bac::factor(type, bac::Energy::Electric, from);
        const double fel2 = bac::factor(type, bac::Energy::Electric, to);
        const double eth2 = in.num("eth") * fth2 / fth1;
        const double eel2 = in.num("eel") * fel2 / fel1;
        r.set("fth1", fth1);
        r.set("fth2", fth2);
        r.set("sth", 100.0 * (1.0 - fth2 / fth1));
        r.set("eth2", eth2);
        r.set("fel1", fel1);
        r.set("fel2", fel2);
        r.set("sel", 100.0 * (1.0 - fel2 / fel1));
        r.set("eel2", eel2);
        r.set("cost", (in.num("eth") - eth2) * in.num("pth") + (in.num("eel") - eel2) * in.num("pel"));
        if (to > from) {
            r.warn({"Die geplante Klasse ist schlechter als die heutige.", "The planned class is worse than today's."});
        } else if (to == from) {
            r.info({"Gleiche Klasse – keine Einsparung.", "Same class – no saving."});
        } else {
            r.ok({"Einsparung durch höhere GA-Effizienzklasse.", "Saving from a higher BAC class."});
        }
        if (to == 'A' && (type == bac::BuildingType::LectureHall || type == bac::BuildingType::Retail)) {
            r.info({"Klasse A bei Hörsaal/Handel hängt stark vom Heiz-/Kühlbedarf der Lüftung ab (Fußnote a).",
                    "Class A for lecture halls/retail depends strongly on the ventilation heating/cooling demand (note a)."});
        }
        if (type == bac::BuildingType::Other) {
            r.info({"Für Sport, Lager, Industrie gibt die Norm f = 1 für alle Klassen an.",
                    "For sports, storage and industry the standard gives f = 1 for all classes."});
        }
        return r;
    };
    return c;
}

Table bacFactors() {
    Table t;
    t.id = "bac_factors";
    t.category = "building";
    t.title = {"GA-Effizienzfaktoren f_BAC (EN ISO 52120)", "BAC efficiency factors f_BAC (EN ISO 52120)"};
    t.summary = {"Thermisch und elektrisch je Gebäudetyp, Klassen D–A", "Thermal and electric per building type, classes D–A"};
    t.description = {std::string("Klasse C = 1 (Referenz). a) Wert hängt stark vom Heiz-/Kühlbedarf der Lüftung ab.\n") + kBacSource,
                     std::string("Class C = 1 (reference). a) value depends strongly on the ventilation heating/cooling demand.\n") +
                         kBacSourceEn};
    t.columns = {{"Gebäudetyp", "Building type"}, {"thermisch D / C / B / A", "thermal D / C / B / A"},
                 {"elektrisch D / C / B / A", "electric D / C / B / A"}};
    const auto opts = buildingTypeOptions();
    for (const auto& o : opts) {
        const auto type = parseBuildingType(o.value);
        auto cell = [&](bac::Energy e) {
            Text s{"", ""};
            for (const char k : {'D', 'C', 'B', 'A'}) {
                const auto v = formatNumber(bac::factor(type, e, k), 2);
                const bool note = e == bac::Energy::Thermal && k == 'A' &&
                                  (type == bac::BuildingType::LectureHall || type == bac::BuildingType::Retail);
                s.de += (s.de.empty() ? "" : " / ") + v.de + (note ? " a)" : "");
                s.en += (s.en.empty() ? "" : " / ") + v.en + (note ? " a)" : "");
            }
            return s;
        };
        t.rows.push_back({o.label, cell(bac::Energy::Thermal), cell(bac::Energy::Electric)});
    }
    return t;
}

// ------------------------------------------------------------------ heating curve (Siemens Heizung)

Calculator heatingCurve() {
    Calculator c;
    c.id = "heating_curve";
    c.category = "building";
    c.title = {"Heizkennlinie: Steilheit & Parallelverschiebung", "Heating curve: slope & parallel shift"};
    c.summary = {"Vorlauftemperatur nach Außentemperatur, Nachtabsenkung", "Flow temperature vs. outdoor temperature, night setback"};
    c.description = {
        "Steilheit s = (T_V(−5 °C) − T_V(+15 °C)) / 20 K, üblicher Einstellbereich 0,25 … 4,0.\n"
        "Raumsollwert-Korrektur/Absenkung: ΔT_V = ΔT_R · (1 + s) – Parallelverschiebung der Kennlinie.\n"
        "Stützwerte: Gerade durch die Punkte bei −5 °C und +15 °C (lineares Raster wie am Regler).\n"
        "Auslegung: T_V = T_i + (T_m,a − T_i)·q^(1/n) + (T_V,a − T_R,a)·q/2, q = (T_i − T_a)/(T_i − T_a,a), "
        "Heizkörperexponent n ≈ 1,3 (Fußbodenheizung ≈ 1,1).\n"
        "Quelle: Siemens „Regeln und Steuern von Heizungsanlagen“ (03/2023), Kap. 3.1, S. 18–20 (Beispiel 67/33 °C, s = 1,7, "
        "−5 K → −13,5 K).",
        "Slope s = (T_F(−5 °C) − T_F(+15 °C)) / 20 K, usual setting range 0.25 … 4.0.\n"
        "Room set-point correction/setback: ΔT_F = ΔT_R · (1 + s) – parallel shift of the curve.\n"
        "Support points: straight line through the points at −5 °C and +15 °C (linear grid as on the controller).\n"
        "Design: T_F = T_i + (T_m,d − T_i)·q^(1/n) + (T_F,d − T_R,d)·q/2, q = (T_i − T_a)/(T_i − T_a,d), "
        "radiator exponent n ≈ 1.3 (floor heating ≈ 1.1).\n"
        "Source: Siemens “Regeln und Steuern von Heizungsanlagen” (03/2023), ch. 3.1, p. 18–20 (example 67/33 °C, s = 1.7, "
        "−5 K → −13.5 K)."};
    c.inputs = {
        Num::choice("mode", {"Kennlinie aus", "Curve from"},
                    {{"points", {"Stützwerten bei −5 °C und +15 °C", "Support points at −5 °C and +15 °C"}},
                     {"design", {"Auslegung der Heizflächen", "Heating surface design"}}},
                    "points"),
        Num::number("tvm5", {"Vorlauf bei −5 °C", "Flow at −5 °C"}, "°C", 67).showIf("mode", {"points"}),
        Num::number("tvp15", {"Vorlauf bei +15 °C", "Flow at +15 °C"}, "°C", 33).showIf("mode", {"points"}),
        Num::number("tia", {"Raumtemperatur (Auslegung)", "Room temperature (design)"}, "°C", 20).showIf("mode", {"design"}),
        Num::number("taa", {"Norm-Außentemperatur", "Design outdoor temperature"}, "°C", -12).showIf("mode", {"design"}),
        Num::number("tva", {"Vorlauf bei Auslegung", "Flow at design"}, "°C", 70).showIf("mode", {"design"}),
        Num::number("tra", {"Rücklauf bei Auslegung", "Return at design"}, "°C", 55).showIf("mode", {"design"}),
        Num::number("n", {"Heizkörperexponent n", "Radiator exponent n"}, "", 1.3).range(1.0, 2.0).showIf("mode", {"design"}),
        Num::number("dtr", {"Raumsollwert-Korrektur / Absenkung", "Room set-point correction / setback"}, "K", -5)
            .range(-15, 10)
            .startsSection({"Verschiebung & Betriebspunkt", "Shift & operating point"}),
        Num::number("ta", {"Aktuelle Außentemperatur", "Current outdoor temperature"}, "°C", 0).range(-30, 25),
    };
    c.outputs = {Out::number("s", {"Steilheit s", "Slope s"}, "", 2).primary(),
                 Out::number("tvm5c", {"Vorlauf bei −5 °C", "Flow at −5 °C"}, "°C", 1),
                 Out::number("tvp15c", {"Vorlauf bei +15 °C", "Flow at +15 °C"}, "°C", 1),
                 Out::number("dtv", {"Verschiebung Vorlauf ΔT_V", "Flow shift ΔT_F"}, "K", 1),
                 Out::number("tvm5s", {"Verschoben: Vorlauf bei −5 °C", "Shifted: flow at −5 °C"}, "°C", 1),
                 Out::number("tvp15s", {"Verschoben: Vorlauf bei +15 °C", "Shifted: flow at +15 °C"}, "°C", 1),
                 Out::number("tv", {"Vorlauf-Sollwert bei T_a", "Flow set point at T_a"}, "°C", 1).primary(),
                 Out::number("tvs", {"Vorlauf-Sollwert verschoben", "Flow set point shifted"}, "°C", 1).primary()};
    c.keywords = {"heizkurve", "heizkennlinie", "steilheit", "nachtabsenkung", "vorlauftemperatur", "witterungsgeführt",
                  "heating curve", "parallelverschiebung", "außentemperatur"};
    auto curve = [](const Inputs& in) {
        // returns a function T_V(T_a) for the unshifted curve
        if (in.choice("mode") == "design") {
            const heating::Design d{in.num("tia"), in.num("taa"), in.num("tva"), in.num("tra"), in.num("n")};
            return std::function<double(double)>([d](double ta) { return heating::flowTemperature(d, ta, d.ti); });
        }
        const double a = in.num("tvm5");
        const double b = in.num("tvp15");
        return std::function<double(double)>([a, b](double ta) { return heating::linearFlow(a, b, ta); });
    };
    c.compute = [curve](const Inputs& in, const Context&) {
        Result r;
        if (in.choice("mode") == "design") {
            if (in.num("tva") <= in.num("tra")) throw CalcError({"Vorlauf muss über dem Rücklauf liegen.", "Flow must exceed return."});
            if (in.num("tra") <= in.num("tia")) throw CalcError({"Rücklauf muss über der Raumtemperatur liegen.", "Return must exceed room temperature."});
            if (in.num("taa") >= in.num("tia")) throw CalcError({"Norm-Außentemperatur muss unter der Raumtemperatur liegen.", "Design outdoor temperature must be below room temperature."});
        }
        const auto f = curve(in);
        const double a = f(-5.0);
        const double b = f(15.0);
        const double s = heating::steepness(a, b);
        const double dtv = heating::flowShift(in.num("dtr"), s);
        r.set("s", s);
        r.set("tvm5c", a);
        r.set("tvp15c", b);
        r.set("dtv", dtv);
        r.set("tvm5s", a + dtv);
        r.set("tvp15s", b + dtv);
        r.set("tv", f(in.num("ta")));
        r.set("tvs", f(in.num("ta")) + dtv);
        if (s < 0.25 || s > 4.0) {
            r.warn({"Steilheit außerhalb des üblichen Einstellbereichs 0,25 … 4,0.", "Slope outside the usual setting range 0.25 … 4.0."});
        } else {
            r.ok({"Steilheit im üblichen Einstellbereich 0,25 … 4,0.", "Slope within the usual setting range 0.25 … 4.0."});
        }
        if (in.choice("mode") == "points") {
            r.info({"Stützwerte-Modus: Kennlinie linear zwischen den Stützpunkten. Reale Kennlinien verlaufen bei tiefen "
                    "Außentemperaturen flacher (Modus „Auslegung“).",
                    "Support-point mode: curve linear between the points. Real curves get flatter at low outdoor temperatures "
                    "(“design” mode)."});
        }
        return r;
    };
    c.chart = [curve](const Inputs& in, const Result& r, const Context&) {
        Chart ch;
        const auto f = curve(in);
        const double dtv = r.values.count("dtv") ? r.values.at("dtv") : 0.0;
        ChartSeries base{"sat", {}};
        ChartSeries shifted{"process", {}};
        double ymax = 0;
        for (double ta = 20.0; ta >= -20.0; ta -= 1.0) {
            const double v = f(ta);
            base.pts.push_back({ta, v});
            shifted.pts.push_back({ta, v + dtv});
            ymax = std::max({ymax, v, v + dtv});
        }
        const double top = std::ceil((ymax + 5.0) / 10.0) * 10.0;
        ch.x = {{"Außentemperatur T_a in °C", "Outdoor temperature T_a in °C"}, -20, 20, 5};
        ch.y = {{"Vorlauf T_V in °C", "Flow T_F in °C"}, 10, std::max(top, 40.0), 10};
        for (double ta = -20; ta <= 20; ta += 5) ch.series.push_back({"t", {{ta, 10}, {ta, ch.y.max}}});
        ch.series.push_back(std::move(base));
        ch.series.push_back(std::move(shifted));
        ch.labels.push_back({20, f(20.0), {"Kennlinie", "curve"}, "sat"});
        ch.labels.push_back({20, f(20.0) + dtv, {"verschoben", "shifted"}, "rh"});
        ch.markers.push_back({-5, f(-5.0), "−5", "aux"});
        ch.markers.push_back({15, f(15.0), "+15", "aux"});
        ch.markers.push_back({in.num("ta"), f(in.num("ta")) + dtv, "T_a", "state"});
        ch.note = {"Antippen setzt die aktuelle Außentemperatur.", "Tap to set the current outdoor temperature."};
        return ch;
    };
    c.pick = [](const Inputs&, double x, double, const Context&) {
        return std::vector<PickValue>{{"ta", std::round(std::clamp(x, -30.0, 25.0)), {}}};
    };
    return c;
}

// ------------------------------------------------------------------ heat pump (Siemens Kältetechnik)

Calculator heatPump() {
    Calculator c;
    c.id = "heat_pump";
    c.category = "building";
    c.title = {"Wärmepumpe: COP, Carnot & JAZ", "Heat pump: COP, Carnot & SPF"};
    c.summary = {"Leistungszahl aus Temperaturen, Mindest-COP nach EN 14511, Jahresarbeitszahl",
                 "COP from temperatures, minimum COP per EN 14511, seasonal performance factor"};
    c.description = {
        "COP_Carnot = T_Senke / (T_Senke − T_Quelle) (Temperaturen in K); realer COP ≈ Gütegrad · COP_Carnot.\n"
        "COP = Φ_c / P_el (Momentanwert für einen Betriebspunkt);  JAZ = Wärme / Strom über ein Jahr (kWh/kWh).\n"
        "Mindest-COP für das Keymark-Gütesiegel (EN 14511): Luft/Wasser > 3,1 (A2/W35), Sole/Wasser > 4,3 (B0/W35), "
        "Wasser/Wasser > 5,1 (W10/W35). Typische JAZ (Fraunhofer 2011, AZ 2): Luft/Wasser 2,9, Sole/Wasser 3,9.\n"
        "Quelle: Siemens „Kältetechnik“ (08/2022), Kap. 7.6, S. 112–115. Gütegrad ist eine Annahme – Herstellerdaten verwenden.",
        "COP_Carnot = T_sink / (T_sink − T_source) (temperatures in K); real COP ≈ efficiency grade · COP_Carnot.\n"
        "COP = Φ_c / P_el (instantaneous value for one operating point);  SPF = heat / electricity over a year (kWh/kWh).\n"
        "Minimum COP for the Keymark label (EN 14511): air/water > 3.1 (A2/W35), brine/water > 4.3 (B0/W35), "
        "water/water > 5.1 (W10/W35). Typical SPF (Fraunhofer 2011, AZ 2): air/water 2.9, brine/water 3.9.\n"
        "Source: Siemens “Kältetechnik” (08/2022), ch. 7.6, p. 112–115. The efficiency grade is an assumption – use vendor data."};
    c.inputs = {
        Num::choice("type", {"Wärmequelle", "Heat source"},
                    {{"air", {"Luft/Wasser", "Air/water"}}, {"brine", {"Sole/Wasser", "Brine/water"}},
                     {"water", {"Wasser/Wasser", "Water/water"}}},
                    "air"),
        Num::number("tsource", {"Quellentemperatur", "Source temperature"}, "°C", 2).range(-30, 40),
        Num::number("tsink", {"Vorlauftemperatur (Senke)", "Flow temperature (sink)"}, "°C", 35).range(20, 80),
        Num::number("eta", {"Gütegrad (Annahme)", "Efficiency grade (assumption)"}, "", 0.5)
            .rangeExclusiveMin(0, 1)
            .withHint({"Typisch 0,4 … 0,6 – Herstellerdaten bevorzugen", "Typically 0.4 … 0.6 – prefer vendor data"}),
        Num::number("cop", {"COP laut Datenblatt (Prüfpunkt)", "COP from data sheet (test point)"}, "", kNaN).positive().opt(),
        Num::number("qheat", {"Erzeugte Wärme pro Jahr", "Heat produced per year"}, "kWh/a", kNaN)
            .positive()
            .opt()
            .startsSection({"Jahresarbeitszahl aus Zählern", "SPF from meters"}),
        Num::number("wel", {"Stromverbrauch pro Jahr", "Electricity per year"}, "kWh/a", kNaN).positive().opt(),
    };
    c.outputs = {Out::number("copc", {"COP Carnot (ideal)", "COP Carnot (ideal)"}, "", 2),
                 Out::number("copest", {"COP geschätzt (Gütegrad)", "COP estimated (efficiency grade)"}, "", 2).primary(),
                 Out::number("copmin", {"Mindest-COP Keymark (Prüfpunkt)", "Minimum COP Keymark (test point)"}, "", 1),
                 Out::number("jaz", {"Jahresarbeitszahl JAZ", "Seasonal performance factor SPF"}, "", 2).primary(),
                 Out::number("jaztyp", {"Typische JAZ (Praxis)", "Typical SPF (field)"}, "", 1)};
    c.keywords = {"wärmepumpe", "cop", "jaz", "jahresarbeitszahl", "scop", "carnot", "en 14511", "keymark", "heat pump", "eer"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double ts = in.num("tsource");
        const double tk = in.num("tsink");
        if (tk <= ts) throw CalcError({"Vorlauftemperatur muss über der Quellentemperatur liegen.", "Flow temperature must exceed source temperature."});
        const double copc = heatpump::carnotHeating(tk, ts);
        r.set("copc", copc);
        r.set("copest", copc * in.num("eta"));
        const auto& type = in.choice("type");
        const double copmin = type == "brine" ? 4.3 : (type == "water" ? 5.1 : 3.1);
        r.set("copmin", copmin);
        if (type != "water") r.set("jaztyp", type == "brine" ? 3.9 : 2.9);
        if (const auto cop = in.maybe("cop")) {
            const char* point = type == "brine" ? "B0/W35" : (type == "water" ? "W10/W35" : "A2/W35");
            if (*cop > copmin) {
                r.ok(cat(Text{"COP über dem Keymark-Mindestwert (Prüfpunkt ", "COP above the Keymark minimum (test point "}, point, ")."));
            } else {
                r.warn(cat(Text{"COP nicht über dem Keymark-Mindestwert ", "COP not above the Keymark minimum "}, numText(copmin, 1),
                           Text{" (Prüfpunkt ", " (test point "}, point, ")."));
            }
            if (*cop > copc) r.error({"COP über dem Carnot-Wert – Temperaturen prüfen.", "COP above the Carnot value – check temperatures."});
        }
        const auto q = in.maybe("qheat");
        const auto w = in.maybe("wel");
        if (q && w) {
            const double jaz = *q / *w;
            r.set("jaz", jaz);
            if (type != "water") {
                const double typ = type == "brine" ? 3.9 : 2.9;
                if (jaz < typ) {
                    r.warn({"JAZ unter dem typischen Praxiswert – Systemtemperaturen und Hilfsenergie prüfen.",
                            "SPF below the typical field value – check system temperatures and auxiliary energy."});
                }
            }
        } else {
            r.info({"Für die JAZ Wärme- und Stromzähler über ein Jahr eingeben.", "Enter heat and electricity meter readings over a year for the SPF."});
        }
        r.info({"Je kleiner die Differenz zwischen Quelle und Vorlauf, desto höher der COP (Flächenheizung vor Heizkörpern).",
                "The smaller the difference between source and flow, the higher the COP (surface heating before radiators)."});
        return r;
    };
    return c;
}

Table heatPumpValues() {
    Table t;
    t.id = "heat_pump_values";
    t.category = "building";
    t.title = {"Wärmepumpen: Mindest-COP & typische JAZ", "Heat pumps: minimum COP & typical SPF"};
    t.summary = {"Keymark (EN 14511) und Praxiswerte", "Keymark (EN 14511) and field values"};
    t.description = {"Quelle: Siemens „Kältetechnik“ (08/2022), S. 113–115 (JAZ: Studie Fraunhofer-Institut 2011, AZ 2). "
                     "Prüfpunkt: A = Außenluft, B = Sole, W = Wasser; Zahl = Eintrittstemperatur Quelle bzw. Vorlauf in °C. "
                     "Gegen gültige Ausgabe prüfen.",
                     "Source: Siemens “Kältetechnik” (08/2022), p. 113–115 (SPF: Fraunhofer study 2011, AZ 2). "
                     "Test point: A = air, B = brine, W = water; number = source inlet or flow temperature in °C. "
                     "Check the edition in force."};
    t.columns = {{"Wärmepumpe", "Heat pump"}, {"Prüfpunkt", "Test point"}, {"Mindest-COP", "Minimum COP"}, {"Typische JAZ", "Typical SPF"}};
    t.rows = {{{"Luft/Wasser", "Air/water"}, "A2/W35", "> 3,1", "2,9"},
              {{"Sole/Wasser", "Brine/water"}, "B0/W35", "> 4,3", "3,9"},
              {{"Wasser/Wasser", "Water/water"}, "W10/W35", "> 5,1", "–"}};
    return t;
}

// ------------------------------------------------------------------ CO₂ (Siemens Lüftung)

Table co2Levels() {
    Table t;
    t.id = "co2_levels";
    t.category = "building";
    t.title = {"CO₂-Konzentrationen & Richtwerte", "CO₂ concentrations & guide values"};
    t.summary = {"Außenluft, Pettenkofer, Grenzwerte", "Outdoor air, Pettenkofer, limits"};
    t.description = {"Quelle: Siemens „Regeln und Steuern von Lüftungsanlagen“ (10/2022), Fig. 3-1, S. 28. "
                     "Die Werte für DIN 1946 und MAK sind Buchangaben – gültige Regeln (z. B. ASR A3.6, UBA, TRGS 900) prüfen.",
                     "Source: Siemens “Regeln und Steuern von Lüftungsanlagen” (10/2022), Fig. 3-1, p. 28. "
                     "DIN 1946 and MAK values as given in the book – check current rules (e.g. ASR A3.6, UBA, TRGS 900)."};
    t.columns = {{"CO₂ in ppm", "CO₂ in ppm"}, {"Bedeutung", "Meaning"}};
    t.rows = {{"400 – 450", {"Normale Außenluft (städtisch oft 550 … 700)", "Normal outdoor air (urban often 550 … 700)"}},
              {"1000", {"20 % der eintretenden Personen unzufrieden (Pettenkofer)", "20 % of people entering dissatisfied (Pettenkofer)"}},
              {"1500", {"Grenzwert nach DIN 1946 (laut Buch)", "Limit per DIN 1946 (as in the book)"}},
              {"2000", {"Empfindliche Personen klagen über Kopfweh", "Sensitive people complain of headache"}},
              {"4000", {"Maximalwerte im Klassenzimmer nach Unterricht", "Peak values in classrooms after lessons"}},
              {"5000", {"MAK-Wert (Arbeitsplatzgrenzwert)", "Occupational exposure limit"}},
              {"100000", {"Kerze erlischt, Bewusstseinsverlust", "Candle goes out, loss of consciousness"}}};
    return t;
}

// ------------------------------------------------------------------ air handling unit (Trox, EN 13053, EN 16798-3)

Text velocityClass(double v) {
    constexpr std::array<double, 5> lim{1.6, 1.8, 2.0, 2.2, 2.5};
    for (std::size_t i = 0; i < lim.size(); ++i)
        if (v <= lim[i] + 1e-9) return "V" + std::to_string(i + 1);
    return {"V6 oder höher", "V6 or higher"};
}

Text sfpClass(double sfp) {
    constexpr std::array<double, 6> lim{500, 750, 1250, 2000, 3000, 4500};
    for (std::size_t i = 0; i < lim.size(); ++i)
        if (sfp <= lim[i]) return "SFP " + std::to_string(i + 1);
    return "SFP 7";
}

Calculator ahuSizing() {
    Calculator c;
    c.id = "ahu_sizing";
    c.category = "building";
    c.title = {"RLT-Gerät & Luftkanal", "Air handling unit & duct"};
    c.summary = {"Geschwindigkeitsklasse (EN 13053), SFP-Klasse, Kanaldurchmesser", "Velocity class (EN 13053), SFP class, duct diameter"};
    c.description = {
        "Anströmgeschwindigkeit v = V̇ / (3600 · B · H) im freien Gerätequerschnitt.\n"
        "Geschwindigkeitsklassen EN 13053: V1 ≤ 1,6 · V2 ≤ 1,8 · V3 ≤ 2,0 · V4 ≤ 2,2 · V5 ≤ 2,5 m/s (darüber V6 …).\n"
        "SFP = P_el / V̇ in W/(m³/s); Klassen EN 13779 / EN 16798-3: SFP 1 ≤ 500, 2 ≤ 750, 3 ≤ 1250, 4 ≤ 2000, "
        "5 ≤ 3000, 6 ≤ 4500, 7 > 4500.\n"
        "Rundkanal d = √(4 · V̇ / (3600 · π · v)); Niedergeschwindigkeitsanlagen < 10 m/s im Kanal.\n"
        "Quellen: TROX „Planungshandbuch RLT-Geräte“ (2018), S. 6; Siemens „Einführung in die HLK- und Gebäudetechnik“ "
        "(08/2022), Kap. 6. Gegen gültige Ausgaben prüfen.",
        "Face velocity v = V̇ / (3600 · W · H) in the free unit cross-section.\n"
        "Velocity classes EN 13053: V1 ≤ 1.6 · V2 ≤ 1.8 · V3 ≤ 2.0 · V4 ≤ 2.2 · V5 ≤ 2.5 m/s (above: V6 …).\n"
        "SFP = P_el / V̇ in W/(m³/s); classes EN 13779 / EN 16798-3: SFP 1 ≤ 500, 2 ≤ 750, 3 ≤ 1250, 4 ≤ 2000, "
        "5 ≤ 3000, 6 ≤ 4500, 7 > 4500.\n"
        "Round duct d = √(4 · V̇ / (3600 · π · v)); low-velocity systems < 10 m/s in the duct.\n"
        "Sources: TROX “Planungshandbuch RLT-Geräte” (2018), p. 6; Siemens “Einführung in die HLK- und Gebäudetechnik” "
        "(08/2022), ch. 6. Check the editions in force."};
    c.inputs = {
        Num::number("V", {"Volumenstrom V̇", "Air flow V̇"}, "m³/h", 10000).positive(),
        Num::number("B", {"Gerätequerschnitt innen: Breite", "Unit inner cross-section: width"}, "m", 1.5).positive(),
        Num::number("H", {"Gerätequerschnitt innen: Höhe", "Unit inner cross-section: height"}, "m", 1.2).positive(),
        Num::number("P", {"Elektr. Leistung Ventilator(en)", "Fan electric power"}, "kW", 3.5).positive().opt(),
        Num::number("vd", {"Geschwindigkeit im Kanal", "Duct velocity"}, "m/s", 6).positive()
            .startsSection({"Luftkanal", "Duct"}),
    };
    c.outputs = {Out::number("v", {"Anströmgeschwindigkeit", "Face velocity"}, "m/s", 2),
                 Out::text("vcls", {"Geschwindigkeitsklasse", "Velocity class"}).primary().abbrev({"V-Klasse", "V class"}),
                 Out::number("sfp", {"Spezifische Ventilatorleistung SFP", "Specific fan power SFP"}, "W/(m³/s)", 0),
                 Out::text("sfpcls", {"SFP-Klasse", "SFP class"}).primary(),
                 Out::number("d", {"Rundkanal-Durchmesser", "Round duct diameter"}, "mm", 0),
                 Out::number("A", {"Kanalquerschnitt", "Duct cross-section"}, "m²", 3)};
    c.keywords = {"rlt", "lüftungsgerät", "ahu", "sfp", "en 13053", "en 16798", "luftgeschwindigkeit", "kanal", "trox",
                  "geschwindigkeitsklasse", "duct"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double q = in.num("V") / 3600.0;  // m³/s
        const double v = q / (in.num("B") * in.num("H"));
        r.set("v", v);
        r.setText("vcls", velocityClass(v));
        if (v > 2.5) {
            r.warn({"Anströmgeschwindigkeit > 2,5 m/s – hohe Druckverluste, Gerät größer wählen.",
                    "Face velocity > 2.5 m/s – high pressure losses, choose a larger unit."});
        } else if (v <= 2.0) {
            r.ok({"Energetisch günstige Geschwindigkeitsklasse.", "Energy-efficient velocity class."});
        }
        if (const auto p = in.maybe("P")) {
            const double sfp = *p * 1000.0 / q;
            r.set("sfp", sfp);
            r.setText("sfpcls", sfpClass(sfp));
        }
        const double A = q / in.num("vd");
        r.set("A", A);
        r.set("d", std::sqrt(4.0 * A / kPi) * 1000.0);
        if (in.num("vd") >= 10) {
            r.info({"Kanalgeschwindigkeit ≥ 10 m/s: Hochgeschwindigkeitsanlage (Geräusche, Druckverlust).",
                    "Duct velocity ≥ 10 m/s: high-velocity system (noise, pressure loss)."});
        }
        return r;
    };
    return c;
}

Table ahuClasses() {
    Table t;
    t.id = "ahu_classes";
    t.category = "building";
    t.title = {"RLT: Geschwindigkeits- und SFP-Klassen", "AHU: velocity and SFP classes"};
    t.summary = {"EN 13053 und EN 13779 / EN 16798-3", "EN 13053 and EN 13779 / EN 16798-3"};
    t.description = {"Quellen: TROX „Planungshandbuch RLT-Geräte“ (2018), S. 6 (V2–V5); SFP-Klassen nach EN 13779:2007 "
                     "(übernommen in EN 16798-3). Gegen gültige Ausgabe prüfen.",
                     "Sources: TROX “Planungshandbuch RLT-Geräte” (2018), p. 6 (V2–V5); SFP classes per EN 13779:2007 "
                     "(adopted in EN 16798-3). Check the edition in force."};
    t.columns = {{"Klasse", "Class"}, {"Grenze", "Limit"}};
    t.rows = {{"V1", "≤ 1,6 m/s"},
              {"V2", "1,6 – 1,8 m/s"},
              {"V3", "1,8 – 2,0 m/s"},
              {"V4", "2,0 – 2,2 m/s"},
              {"V5", "2,2 – 2,5 m/s"},
              {"SFP 1", "≤ 500 W/(m³/s)"},
              {"SFP 2", "500 – 750 W/(m³/s)"},
              {"SFP 3", "750 – 1250 W/(m³/s)"},
              {"SFP 4", "1250 – 2000 W/(m³/s)"},
              {"SFP 5", "2000 – 3000 W/(m³/s)"},
              {"SFP 6", "3000 – 4500 W/(m³/s)"},
              {"SFP 7", "> 4500 W/(m³/s)"}};
    return t;
}

// ------------------------------------------------------------------ room acoustics (BAuA Fb 1030, DIN 18041)

Calculator reverberation() {
    Calculator c;
    c.id = "reverberation";
    c.category = "building";
    c.title = {"Nachhallzeit (Sabine) & DIN 18041", "Reverberation time (Sabine) & DIN 18041"};
    c.summary = {"Ist- und Soll-Nachhallzeit, nötige Absorberfläche", "Actual and target reverberation time, absorber area needed"};
    c.description = {
        "Sabine: T = 0,163 s/m · V / A,   A = α · S (äquivalente Absorptionsfläche)\n"
        "Soll-Nachhallzeit DIN 18041 (besetzter Raum, mittlere Frequenzen, Toleranz ±20 %):\n"
        "A1 Musik T = 0,45·lg V + 0,07 · A2 Sprache/Vortrag T = 0,37·lg V − 0,14 · A3 Unterricht/Kommunikation "
        "T = 0,32·lg V − 0,17 · A4 Unterricht inklusiv T = 0,26·lg V − 0,14 (A4 seit DIN 18041:2016).\n"
        "Quelle: BAuA-Forschungsbericht Fb 1030 „Lärm in Bildungsstätten“ (2004), Kap. 3, S. 19–20 (Tab. 3.1: 150 m³ → 0,53 s); "
        "im Buch S. 50 sind die Vorzeichen der Konstanten verdruckt. Gegen DIN 18041:2016 prüfen.",
        "Sabine: T = 0.163 s/m · V / A,   A = α · S (equivalent absorption area)\n"
        "Target reverberation time DIN 18041 (occupied room, mid frequencies, tolerance ±20 %):\n"
        "A1 music T = 0.45·lg V + 0.07 · A2 speech/lecture T = 0.37·lg V − 0.14 · A3 teaching/communication "
        "T = 0.32·lg V − 0.17 · A4 inclusive teaching T = 0.26·lg V − 0.14 (A4 since DIN 18041:2016).\n"
        "Source: BAuA research report Fb 1030 “Lärm in Bildungsstätten” (2004), ch. 3, p. 19–20 (Tab. 3.1: 150 m³ → 0.53 s); "
        "on p. 50 the signs of the constants are misprinted. Check against DIN 18041:2016."};
    c.inputs = {
        Num::number("L", {"Raumlänge", "Room length"}, "m", 9).positive(),
        Num::number("B", {"Raumbreite", "Room width"}, "m", 7.5).positive(),
        Num::number("H", {"Raumhöhe", "Room height"}, "m", 3).positive(),
        Num::choice("use", {"Nutzung (DIN 18041)", "Use (DIN 18041)"},
                    {{"music", {"A1 Musik", "A1 music"}}, {"speech", {"A2 Sprache/Vortrag", "A2 speech/lecture"}},
                     {"teach", {"A3 Unterricht/Kommunikation", "A3 teaching/communication"}},
                     {"incl", {"A4 Unterricht inklusiv", "A4 inclusive teaching"}}},
                    "teach"),
        Num::number("alpha", {"Mittlerer Absorptionsgrad Raumflächen", "Mean absorption coefficient of surfaces"}, "", 0.08)
            .range(0.01, 1)
            .startsSection({"Absorption", "Absorption"})
            .withHint({"Putz, Beton, Glas, Linoleum ≈ 0,02 … 0,1 (ohne Absorber)", "Plaster, concrete, glass ≈ 0.02 … 0.1 (no absorbers)"}),
        Num::number("aabs", {"Absorptionsgrad Absorber/Akustikdecke", "Absorption coefficient of absorbers"}, "", 0.9).range(0.05, 1),
        Num::number("sabs", {"Vorhandene Absorberfläche", "Existing absorber area"}, "m²", 0).nonNegative(),
    };
    c.outputs = {Out::number("Vol", {"Raumvolumen", "Room volume"}, "m³", 1),
                 Out::number("T", {"Nachhallzeit Ist", "Reverberation time actual"}, "s", 2).primary(),
                 Out::number("Ts", {"Nachhallzeit Soll", "Reverberation time target"}, "s", 2).primary(),
                 Out::number("Tmax", {"Toleranz obere Grenze (+20 %)", "Tolerance upper limit (+20 %)"}, "s", 2),
                 Out::number("A", {"Äquivalente Absorptionsfläche Ist", "Equivalent absorption area actual"}, "m²", 1),
                 Out::number("As", {"Nötige Absorptionsfläche", "Absorption area needed"}, "m²", 1),
                 Out::number("sneed", {"Zusätzliche Absorberfläche", "Additional absorber area"}, "m²", 1).primary()};
    c.keywords = {"nachhall", "nachhallzeit", "sabine", "din 18041", "raumakustik", "lärm", "klassenraum", "schule",
                  "absorber", "akustikdecke", "reverberation"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double L = in.num("L"), B = in.num("B"), H = in.num("H");
        const double V = L * B * H;
        const double S = 2.0 * (L * B + L * H + B * H);
        const double sabs = in.num("sabs");
        if (sabs > S) throw CalcError({"Absorberfläche größer als die Raumoberfläche.", "Absorber area larger than the room surface."});
        const double alpha = in.num("alpha");
        const double aabs = in.num("aabs");
        const double A = alpha * (S - sabs) + aabs * sabs;
        const auto& u = in.choice("use");
        const auto use = u == "music" ? acoustics::Use::Music
                         : u == "speech" ? acoustics::Use::Speech
                         : u == "incl"   ? acoustics::Use::TeachingInclusive
                                         : acoustics::Use::Teaching;
        const double T = acoustics::sabine(V, A);
        const double Ts = acoustics::din18041Target(use, V);
        const double As = acoustics::absorptionFor(V, Ts);
        r.set("Vol", V);
        r.set("T", T);
        r.set("Ts", Ts);
        r.set("Tmax", 1.2 * Ts);
        r.set("A", A);
        r.set("As", As);
        double need = 0;
        if (As > A) {
            if (aabs <= alpha) throw CalcError({"Absorber muss stärker absorbieren als die Raumflächen.", "Absorbers must absorb more than the surfaces."});
            need = (As - A) / (aabs - alpha);
        }
        r.set("sneed", need);
        if (T > 1.2 * Ts) {
            r.warn({"Nachhallzeit zu lang – Absorberfläche ergänzen.", "Reverberation time too long – add absorbers."});
        } else if (T < 0.8 * Ts) {
            r.info({"Nachhallzeit unter dem Toleranzbereich (Raum sehr „trocken“).", "Reverberation time below the tolerance range (very “dry” room)."});
        } else {
            r.ok({"Nachhallzeit im Toleranzbereich ±20 %.", "Reverberation time within the ±20 % tolerance."});
        }
        if (need > L * B) {
            r.warn({"Nötige Absorberfläche größer als die Deckenfläche – auch Wandflächen belegen.",
                    "Absorber area needed exceeds the ceiling area – also use wall areas."});
        }
        if (V < 30 || V > 5000) {
            r.warn({"Raumvolumen außerhalb des üblichen Gültigkeitsbereichs der Formeln – DIN 18041 prüfen.",
                    "Room volume outside the usual validity range of the formulas – check DIN 18041."});
        }
        r.info({"Personen und Mobiliar sind nicht berücksichtigt; der Soll-Wert gilt für den besetzten Raum.",
                "Persons and furniture are not included; the target applies to the occupied room."});
        return r;
    };
    return c;
}

Calculator soundLevels() {
    Calculator c;
    c.id = "sound_levels";
    c.category = "building";
    c.title = {"Schallpegel addieren & Entfernung", "Add sound levels & distance"};
    c.summary = {"Energetische Summe mehrerer Quellen, Abnahme mit der Entfernung", "Energetic sum of sources, decrease with distance"};
    c.description = {"L_Σ = 10 · lg Σ 10^(L_i/10)   (zwei gleiche Quellen: +3 dB)\n"
                     "Punktquelle im Freifeld: L₂ = L₁ − 20 · lg(r₂/r₁)   (Verdopplung der Entfernung: −6 dB)\n"
                     "Grundlagen der Akustik; vgl. BAuA Fb 1030 „Lärm in Bildungsstätten“ (2004).",
                     "L_Σ = 10 · lg Σ 10^(L_i/10)   (two equal sources: +3 dB)\n"
                     "Point source in the free field: L₂ = L₁ − 20 · lg(r₂/r₁)   (double distance: −6 dB)\n"
                     "Acoustics basics; cf. BAuA Fb 1030 “Lärm in Bildungsstätten” (2004)."};
    c.inputs = {
        Num::number("L1", {"Pegel Quelle 1", "Level source 1"}, "dB", 80),
        Num::number("L2", {"Pegel Quelle 2", "Level source 2"}, "dB", 80).opt(),
        Num::number("L3", {"Pegel Quelle 3", "Level source 3"}, "dB", kNaN).opt(),
        Num::number("L4", {"Pegel Quelle 4", "Level source 4"}, "dB", kNaN).opt(),
        Num::number("r1", {"Abstand der Messung", "Measuring distance"}, "m", 1).positive().startsSection({"Entfernung", "Distance"}),
        Num::number("r2", {"Neuer Abstand", "New distance"}, "m", 10).positive(),
    };
    c.outputs = {Out::number("Ls", {"Summenpegel", "Total level"}, "dB", 1).primary(),
                 Out::number("Lr", {"Summenpegel im neuen Abstand", "Total level at new distance"}, "dB", 1).primary()};
    c.keywords = {"schallpegel", "dezibel", "db", "pegeladdition", "lärm", "schall", "sound level", "entfernung"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        std::vector<double> ls;
        for (const char* id : {"L1", "L2", "L3", "L4"})
            if (const auto v = in.maybe(id)) ls.push_back(*v);
        if (ls.empty()) throw CalcError({"Mindestens einen Pegel eingeben.", "Enter at least one level."});
        const double sum = acoustics::levelSum(ls);
        r.set("Ls", sum);
        r.set("Lr", acoustics::distanceLevel(sum, in.num("r1"), in.num("r2")));
        r.info({"Freifeld-Näherung; in Räumen begrenzt der Nachhall die Pegelabnahme.", "Free-field approximation; indoors reverberation limits the decrease."});
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ escape routes (ASR A2.3)

Calculator escapeRoute() {
    Calculator c;
    c.id = "escape_route";
    c.category = "building";
    c.title = {"Fluchtweg: Breite & Länge (ASR A2.3)", "Escape route: width & length (ASR A2.3)"};
    c.summary = {"Mindestbreiten nach Personenzahl, maximale Fluchtweglänge", "Minimum widths by number of persons, maximum route length"};
    c.description = {
        "ASR A2.3 (Fassung 03/2022, geändert 2024), Abschn. 5: Tab. 1 Mindestbreiten (Spalte B Türen/Durchgänge, Spalte C "
        "Hauptfluchtweg): bis 5 P. 0,80/0,90 · bis 20 P. 0,90/1,00 · bis 50 P. 0,90/1,20 · bis 100 P. 1,00/1,20 · "
        "bis 200 P. 1,05/1,20 · bis 300 P. 1,65/1,80 · bis 400 P. 2,25/2,40 m; über 200 Personen linear interpolierbar. "
        "Bei Neubau für bis 5 Personen auch an Türen 0,90 m empfohlen (Barrierefreiheit).\n"
        "Hauptfluchtweglänge (Luftlinie): 35 m (normal oder erhöhte Brandgefährdung mit Löschanlage), 25 m (erhöhte "
        "Brandgefährdung ohne Löschanlage), 10 m (explosionsgefährliche Stoffe); tatsächliche Laufweglänge ≤ 1,5-fach.\n"
        "Das Buch (ASR A2.3:2007) nennt noch eine Spalte: 0,875 / 1,00 / 1,20 / 1,80 / 2,40 m und 20 m für giftstoff- und "
        "explosionsgefährdete Räume. Bauordnungsrecht kann längere Rettungswege zulassen.",
        "ASR A2.3 (edition 03/2022, amended 2024), section 5: Tab. 1 minimum widths (column B doors/passages, column C main "
        "escape route): up to 5 p. 0.80/0.90 · 20 p. 0.90/1.00 · 50 p. 0.90/1.20 · 100 p. 1.00/1.20 · 200 p. 1.05/1.20 · "
        "300 p. 1.65/1.80 · 400 p. 2.25/2.40 m; above 200 persons linear interpolation allowed. For new buildings 0.90 m "
        "is recommended for doors up to 5 persons (accessibility).\n"
        "Main escape route length (straight line): 35 m (normal, or increased fire hazard with extinguishing system), 25 m "
        "(increased fire hazard without), 10 m (explosive substances); actual walking distance ≤ 1.5 times.\n"
        "The book (ASR A2.3:2007) still has one column: 0.875 / 1.00 / 1.20 / 1.80 / 2.40 m and 20 m for toxic and explosive "
        "areas. Building law may allow longer rescue routes."};
    c.inputs = {
        Num::number("n", {"Personen im Einzugsgebiet", "Persons in the catchment area"}, "", 30).positive(),
        Num::choice("room", {"Raumart", "Room type"},
                    {{"normal", {"Ohne oder normale Brandgefährdung", "No or normal fire hazard"}},
                     {"fireext", {"Erhöhte Brandgefährdung, mit Löschanlage", "Increased fire hazard, with extinguishing system"}},
                     {"fire", {"Erhöhte Brandgefährdung, ohne Löschanlage", "Increased fire hazard, no extinguishing system"}},
                     {"explosive", {"Explosionsgefährliche Stoffe", "Explosive substances"}}},
                    "normal"),
        Num::number("len", {"Fluchtweglänge (Luftlinie)", "Escape route length (straight line)"}, "m", kNaN).positive().opt(),
        Num::number("walk", {"Tatsächliche Laufweglänge", "Actual walking distance"}, "m", kNaN).positive().opt(),
        Num::number("wact", {"Vorhandene lichte Breite Flur", "Existing clear corridor width"}, "m", kNaN).positive().opt(),
    };
    c.outputs = {Out::number("wroute", {"Mindestbreite Hauptfluchtweg", "Minimum width main escape route"}, "m", 2).primary(),
                 Out::number("wdoor", {"Mindestbreite Türen/Durchgänge", "Minimum width doors/passages"}, "m", 2).primary(),
                 Out::number("lmax", {"Max. Fluchtweglänge", "Max. escape route length"}, "m", 0),
                 Out::number("wmax", {"Max. Laufweglänge", "Max. walking distance"}, "m", 1)};
    c.keywords = {"fluchtweg", "rettungsweg", "notausgang", "asr a2.3", "flurbreite", "türbreite", "arbeitsstätte",
                  "escape route", "fluchtweglänge"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const auto w = escape::minWidth(in.num("n"));
        if (!w) throw CalcError({"Über 400 Personen enthält die Tabelle keine Werte – Einzelfallbetrachtung.",
                                 "Above 400 persons the table has no values – individual assessment."});
        r.set("wroute", w->route);
        r.set("wdoor", w->door);
        const auto& k = in.choice("room");
        const auto room = k == "fireext" ? escape::Room::FireWithExtinguisher
                          : k == "fire"  ? escape::Room::FireNoExtinguisher
                          : k == "explosive" ? escape::Room::Explosive
                                             : escape::Room::Normal;
        const double lmax = escape::maxLength(room);
        r.set("lmax", lmax);
        r.set("wmax", 1.5 * lmax);
        if (const auto l = in.maybe("len")) {
            if (*l > lmax) r.error({"Fluchtweglänge überschritten.", "Escape route too long."});
            else r.ok({"Fluchtweglänge eingehalten.", "Escape route length OK."});
        }
        if (const auto wk = in.maybe("walk")) {
            if (*wk > 1.5 * lmax) r.error({"Laufweglänge > 1,5-fache Fluchtweglänge.", "Walking distance > 1.5 × route length."});
        }
        if (const auto wa = in.maybe("wact")) {
            if (*wa + 1e-9 < w->route) r.error({"Vorhandene Breite zu gering.", "Existing width too small."});
            else r.ok({"Vorhandene Breite ausreichend.", "Existing width sufficient."});
        }
        if (in.num("n") <= 5) {
            r.info({"Neubau/Umbau: auch an Türen 0,90 m empfohlen (barrierefrei).", "New build/conversion: 0.90 m also recommended at doors (accessible)."});
        }
        if (ctx.norm == NormProfile::CH) {
            r.info({"ASR A2.3 gilt in Deutschland; in der Schweiz gelten die VKF-Brandschutzrichtlinien und die ArGV 4.",
                    "ASR A2.3 applies in Germany; in Switzerland the VKF fire protection guidelines and ArGV 4 apply."});
        }
        return r;
    };
    return c;
}

Table escapeWidths() {
    Table t;
    t.id = "escape_widths";
    t.category = "building";
    t.title = {"Fluchtwegbreiten ASR A2.3", "Escape route widths ASR A2.3"};
    t.summary = {"Mindestbreiten 2022 und Vergleich zur Fassung 2007", "Minimum widths 2022 compared with the 2007 edition"};
    t.description = {"ASR A2.3, Fassung 03/2022 (GMBl. 2022 S. 227, geändert 2024), Abschn. 5 Tab. 1. Die Fassung 2007 steht im "
                     "Fachbuch (Ordner Brandschutz & Arbeitsstätten). Gültige Fassung bei der BAuA prüfen.",
                     "ASR A2.3, edition 03/2022 (GMBl. 2022 p. 227, amended 2024), section 5 Tab. 1. The 2007 edition is in the "
                     "book (folder fire protection & workplaces). Check the edition in force at BAuA."};
    t.columns = {{"Personen", "Persons"}, {"Türen/Durchgänge 2022", "Doors/passages 2022"}, {"Hauptfluchtweg 2022", "Main route 2022"},
                 {"Fluchtweg 2007", "Escape route 2007"}};
    t.rows = {{{"bis 5", "up to 5"}, "0,80 m (Neubau 0,90)", "0,90 m", "0,875 m"},
              {{"bis 20", "up to 20"}, "0,90 m", "1,00 m", "1,00 m"},
              {{"bis 50", "up to 50"}, "0,90 m", "1,20 m", "1,20 m"},
              {{"bis 100", "up to 100"}, "1,00 m", "1,20 m", "1,20 m"},
              {{"bis 200", "up to 200"}, "1,05 m", "1,20 m", "1,20 m"},
              {{"bis 300", "up to 300"}, "1,65 m", "1,80 m", "1,80 m"},
              {{"bis 400", "up to 400"}, "2,25 m", "2,40 m", "2,40 m"}};
    return t;
}

// ------------------------------------------------------------------ lift shaft smoke vent

Calculator liftShaft() {
    Calculator c;
    c.id = "lift_shaft";
    c.category = "building";
    c.title = {"Aufzugsschacht: Rauchabzug & Lüftung", "Lift shaft: smoke vent & ventilation"};
    c.summary = {"Freier Querschnitt der Schachtöffnung nach MBO bzw. VKF", "Free area of the shaft opening per MBO or VKF"};
    c.description = {
        "DE – MBO § 39 Abs. 3: Öffnung zur Rauchableitung ≥ 2,5 % der Fahrschachtgrundfläche, mindestens 0,10 m² "
        "(Landesbauordnung prüfen). Lüftung nach DIN EN 81-1/-2 (2010): Empfehlung 1 % der Schachtgrundfläche am oberen Ende.\n"
        "CH – VKF-Brandschutzrichtlinie (Stand Blatt 2007): lichter Querschnitt 5 % des Schachtquerschnitts, max. 1600 cm²; "
        "Maschinen-/Rollenraum separat entlüftet mindestens 500 cm².\n"
        "Quellen: ZVEI/VDMA-Leitfaden „Rauchableitung und Lüftung von Aufzugsschächten“ (07/2012), S. 3; IG-BSK-Arbeitsblatt "
        "D_AE_001 (03/2007). EN 81-1/-2 sind durch EN 81-20/-50 ersetzt; gültige Fassungen prüfen.",
        "DE – MBO § 39 (3): smoke vent opening ≥ 2.5 % of the shaft floor area, at least 0.10 m² (check the state building code). "
        "Ventilation per DIN EN 81-1/-2 (2010): recommended 1 % of the shaft floor area at the top.\n"
        "CH – VKF fire protection guideline (sheet 2007): clear area 5 % of the shaft cross-section, max. 1600 cm²; "
        "machine/pulley room vented separately at least 500 cm².\n"
        "Sources: ZVEI/VDMA guide on smoke venting and ventilation of lift shafts (07/2012), p. 3; IG-BSK sheet D_AE_001 "
        "(03/2007). EN 81-1/-2 are replaced by EN 81-20/-50; check the editions in force."};
    c.inputs = {
        Num::number("W", {"Schachtbreite (innen)", "Shaft width (inside)"}, "m", 2.0).positive(),
        Num::number("D", {"Schachttiefe (innen)", "Shaft depth (inside)"}, "m", 2.2).positive(),
    };
    c.outputs = {Out::number("A", {"Schachtgrundfläche", "Shaft floor area"}, "m²", 2),
                 Out::number("Ao", {"Freier Querschnitt Rauchabzug", "Free area smoke vent"}, "m²", 3).primary(),
                 Out::number("Aocm", {"Freier Querschnitt Rauchabzug", "Free area smoke vent"}, "cm²", 0),
                 Out::number("Avent", {"Lüftung (Empfehlung 1 %)", "Ventilation (recommended 1 %)"}, "m²", 3)};
    c.keywords = {"aufzug", "fahrschacht", "aufzugsschacht", "rauchabzug", "entrauchung", "mbo", "vkf", "lift", "rwa"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const double A = in.num("W") * in.num("D");
        const bool ch = ctx.norm == NormProfile::CH;
        const double Ao = ch ? shaft::openingCH(A) : shaft::openingDE(A);
        r.set("A", A);
        r.set("Ao", Ao);
        r.set("Aocm", Ao * 1e4);
        r.set("Avent", 0.01 * A);
        if (ch) {
            r.info({"Profil CH: 5 % des Schachtquerschnitts, höchstens 1600 cm² (VKF).", "Profile CH: 5 % of the shaft cross-section, at most 1600 cm² (VKF)."});
            r.info({"Maschinen-/Rollenraum separat entlüftet: mindestens 500 cm².", "Machine/pulley room vented separately: at least 500 cm²."});
        } else {
            r.info({"Profil DE: 2,5 % der Fahrschachtgrundfläche, mindestens 0,10 m² (MBO § 39).",
                    "Profile DE: 2.5 % of the shaft floor area, at least 0.10 m² (MBO § 39)."});
            if (0.025 * A < 0.10) r.info({"Mindestquerschnitt 0,10 m² maßgebend.", "Minimum area 0.10 m² governs."});
        }
        r.info({"Energiesparend: geschlossene Klappe mit Rauchmelder/Temperaturfühler statt ständig offener Öffnung.",
                "Energy saving: closed damper with smoke detector/temperature sensor instead of a permanently open vent."});
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ pumps and fans (proportionality laws)

Calculator fanAffinity() {
    Calculator c;
    c.id = "fan_affinity";
    c.category = "motors";
    c.title = {"Pumpe/Ventilator am FU: Proportionalitätsgesetze", "Pump/fan on VFD: affinity laws"};
    c.summary = {"V̇ ~ n, Δp ~ n², P ~ n³ – Leistung und Einsparung bei Drehzahlregelung",
                 "V̇ ~ n, Δp ~ n², P ~ n³ – power and saving with speed control"};
    c.description = {
        "V̇₂/V̇₁ = n₂/n₁ · Δp₂/Δp₁ = (n₂/n₁)² · P₂/P₁ = (n₂/n₁)³\n"
        "Gilt für quadratische Anlagenkennlinien mit gleichbleibender Dichte und Wirkungsgrad (Ventilatoren, geschlossene "
        "Kreisläufe). Mit statischem Anteil (Förderhöhe, Vordruck) nicht anwendbar.\n"
        "Pumpen und Ventilatoren arbeiten meist bei 50 … 90 % Drehzahl.\n"
        "Quellen: Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022), S. 180 (Beispiel 4000 → 8000 m³/h, "
        "350 → 1400 Pa); Danfoss „Wissenswertes über Frequenzumrichter“, Kap. 3.",
        "V̇₂/V̇₁ = n₂/n₁ · Δp₂/Δp₁ = (n₂/n₁)² · P₂/P₁ = (n₂/n₁)³\n"
        "Valid for quadratic system curves with constant density and efficiency (fans, closed circuits). Not applicable "
        "with a static part (head, inlet pressure).\n"
        "Pumps and fans usually run at 50 … 90 % speed.\n"
        "Sources: Siemens “Einführung in die HLK- und Gebäudetechnik” (08/2022), p. 180 (example 4000 → 8000 m³/h, "
        "350 → 1400 Pa); Danfoss “Facts worth knowing about frequency converters”, ch. 3."};
    c.inputs = {
        Num::number("V1", {"Volumenstrom V̇₁", "Flow V̇₁"}, "m³/h", 4000).positive(),
        Num::number("p1", {"Druckerhöhung Δp₁", "Pressure rise Δp₁"}, "Pa", 350).positive(),
        Num::number("P1", {"Leistungsaufnahme P₁", "Power input P₁"}, "kW", 1.0).positive(),
        Num::number("n1", {"Drehzahl n₁", "Speed n₁"}, "1/min", 1450).positive(),
        Num::choice("mode", {"Neuer Betriebspunkt aus", "New operating point from"},
                    {{"speed", {"Drehzahl n₂", "Speed n₂"}}, {"flow", {"Volumenstrom V̇₂", "Flow V̇₂"}}}, "flow")
            .startsSection({"Neuer Betriebspunkt", "New operating point"}),
        Num::number("n2", {"Drehzahl n₂", "Speed n₂"}, "1/min", 1160).positive().showIf("mode", {"speed"}),
        Num::number("V2", {"Volumenstrom V̇₂", "Flow V̇₂"}, "m³/h", 3200).positive().showIf("mode", {"flow"}),
        Num::number("hours", {"Betriebsstunden im neuen Punkt", "Operating hours at new point"}, "h/a", 4000).nonNegative(),
        Num::number("price", {"Strompreis", "Electricity price"}, kCurrencyUnit, 0.30).nonNegative().withHint({"je kWh", "per kWh"}),
    };
    c.outputs = {Out::number("ratio", {"Drehzahlverhältnis n₂/n₁", "Speed ratio n₂/n₁"}, "%", 1),
                 Out::number("n2o", {"Drehzahl n₂", "Speed n₂"}, "1/min", 0),
                 Out::number("V2o", {"Volumenstrom V̇₂", "Flow V̇₂"}, "m³/h", 0),
                 Out::number("p2", {"Druckerhöhung Δp₂", "Pressure rise Δp₂"}, "Pa", 0),
                 Out::number("P2", {"Leistungsaufnahme P₂", "Power input P₂"}, "kW", 2).primary(),
                 Out::number("dE", {"Energieeinsparung", "Energy saving"}, "kWh/a", 0),
                 Out::number("cost", {"Kosteneinsparung", "Cost saving"}, "€", 0).primary()};
    c.keywords = {"proportionalitätsgesetz", "affinitätsgesetz", "frequenzumrichter", "fu", "pumpe", "ventilator", "drehzahl",
                  "n³", "energieeinsparung", "affinity", "vfd", "fan", "pump"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double ratio = in.choice("mode") == "speed" ? in.num("n2") / in.num("n1") : in.num("V2") / in.num("V1");
        const auto p = fans::affinity({in.num("V1"), in.num("p1"), in.num("P1")}, ratio);
        r.set("ratio", 100.0 * ratio);
        r.set("n2o", in.num("n1") * ratio);
        r.set("V2o", p.flow);
        r.set("p2", p.pressure);
        r.set("P2", p.power);
        const double dE = (in.num("P1") - p.power) * in.num("hours");
        r.set("dE", dE);
        r.set("cost", dE * in.num("price"));
        if (ratio > 1.0) {
            r.warn({"Drehzahl über Nenndrehzahl – Motor, FU und Laufrad auf Überlast prüfen.",
                    "Speed above rated speed – check motor, VFD and impeller for overload."});
        } else if (ratio < 0.3) {
            r.warn({"Sehr kleine Drehzahl – Motorkühlung und Mindestvolumenstrom prüfen.",
                    "Very low speed – check motor cooling and minimum flow."});
        }
        r.info({"Nur für Anlagen ohne statischen Druckanteil (quadratische Kennlinie).",
                "Only for systems without static pressure component (quadratic curve)."});
        return r;
    };
    c.chart = [](const Inputs&, const Result& r, const Context&) {
        Chart ch;
        ch.x = {{"Drehzahl in % von n₁", "Speed in % of n₁"}, 0, 120, 20};
        ch.y = {{"in % vom Betriebspunkt 1", "in % of operating point 1"}, 0, 150, 25};
        ChartSeries v{"sat", {}}, dp{"rh", {}}, pw{"process", {}};
        for (int k = 0; k <= 120; k += 2) {
            const double f = k / 100.0;
            v.pts.push_back({double(k), 100.0 * f});
            dp.pts.push_back({double(k), 100.0 * f * f});
            pw.pts.push_back({double(k), 100.0 * f * f * f});
        }
        ch.series = {std::move(v), std::move(dp), std::move(pw)};
        ch.labels.push_back({120, 120, "V̇ ~ n", "sat"});
        ch.labels.push_back({120, 144, "Δp ~ n²", "rh"});
        ch.labels.push_back({112, 142, "P ~ n³", "h"});
        const double ratio = r.values.count("ratio") ? r.values.at("ratio") : 100.0;
        ch.markers.push_back({100, 100, "1", "aux"});
        ch.markers.push_back({ratio, 100.0 * std::pow(ratio / 100.0, 3), "2", "state"});
        ch.note = {"V̇ linear, Δp quadratisch, P kubisch mit der Drehzahl.", "V̇ linear, Δp square, P cube of speed."};
        return ch;
    };
    return c;
}

}  // namespace

void addBuilding(std::vector<Calculator>& out) {
    out.push_back(bacEfficiency());
    out.push_back(heatingCurve());
    out.push_back(heatPump());
    out.push_back(ahuSizing());
    out.push_back(reverberation());
    out.push_back(soundLevels());
    out.push_back(escapeRoute());
    out.push_back(liftShaft());
    out.push_back(fanAffinity());
}

void addBuildingTables(std::vector<Table>& out) {
    out.push_back(bacFactors());
    out.push_back(heatPumpValues());
    out.push_back(co2Levels());
    out.push_back(ahuClasses());
    out.push_back(escapeWidths());
}

}  // namespace elc::catalog
