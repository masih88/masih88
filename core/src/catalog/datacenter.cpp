// Calculators: Rechenzentrum / Data centre
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"

namespace elc::catalog {

namespace {

std::vector<Option> redundancyOptions() {
    return {{"N", "N"}, {"N+1", "N+1"}, {"N+2", "N+2"}, {"2N", "2N"}};
}

/// Units required for a load including redundancy
double unitsWithRedundancy(double base, const std::string& red) {
    if (red == "N+1") return base + 1;
    if (red == "N+2") return base + 2;
    if (red == "2N") return 2 * base;
    return base;
}

Calculator pue() {
    Calculator c;
    c.id = "dc_pue";
    c.category = "datacenter";
    c.title = {"PUE / Energieeffizienz", "PUE / energy efficiency"};
    c.summary = {"PUE, DCiE, Overhead, Jahresenergie, EnEfG", "PUE, DCiE, overhead, annual energy, EnEfG"};
    c.description = {
        "PUE = P_gesamt / P_IT (ISO/IEC 30134-2, Jahresmittel)    DCiE = 1 / PUE · 100 %\n"
        "Overhead = P_gesamt − P_IT (Kühlung, USV-Verluste, Licht …)\n"
        "DE – Energieeffizienzgesetz (EnEfG 2023) §11: RZ mit Betriebsbeginn ab 1.7.2026 PUE ≤ 1,2; bestehende RZ "
        "PUE ≤ 1,5 ab 1.7.2027 und ≤ 1,3 ab 1.7.2030. Gültige Fassung prüfen.",
        "PUE = P_total / P_IT (ISO/IEC 30134-2, annual average)    DCiE = 1 / PUE · 100 %\n"
        "Overhead = P_total − P_IT (cooling, UPS losses, lighting …)\n"
        "Germany – Energy Efficiency Act (EnEfG 2023) §11: DCs starting operation from 1 July 2026 PUE ≤ 1.2; existing DCs "
        "PUE ≤ 1.5 from 1 July 2027 and ≤ 1.3 from 1 July 2030. Check the version in force."};
    c.inputs = {
        Num::number("Ptot", {"Gesamtleistung RZ", "Total facility power"}, "kW", 650).positive(),
        Num::number("Pit", {"IT-Leistung", "IT power"}, "kW", 450).positive(),
        Num::number("price", {"Strompreis pro kWh", "Energy price per kWh"}, "€ / CHF", 0.20).nonNegative(),
    };
    c.outputs = {Out::number("pue", {"PUE", "PUE"}, "", 2).primary(),
                 Out::number("dcie", {"DCiE", "DCiE"}, "%", 1),
                 Out::number("over", {"Overhead-Leistung", "Overhead power"}, "kW", 1),
                 Out::number("Eyear", {"Energie pro Jahr gesamt", "Total energy per year"}, "MWh", 0),
                 Out::number("Cover", {"Kosten Overhead pro Jahr", "Overhead cost per year"}, "€", 0),
                 Out::number("save", {"Einsparung bei PUE 1,3", "Saving at PUE 1.3"}, "MWh/a", 0)};
    c.keywords = {"pue", "rechenzentrum", "data center", "dcie", "enefg", "effizienz"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const double Ptot = in.num("Ptot");
        const double Pit = in.num("Pit");
        if (Pit > Ptot) throw CalcError({"IT-Leistung größer als Gesamtleistung.", "IT power exceeds total power."});
        const double p = Ptot / Pit;
        r.set("pue", p);
        r.set("dcie", 100.0 / p);
        r.set("over", Ptot - Pit);
        r.set("Eyear", Ptot * 8760.0 / 1000.0);
        r.set("Cover", (Ptot - Pit) * 8760.0 * in.num("price"), ctx.norm == NormProfile::CH ? "CHF" : "€");
        r.set("save", std::max(0.0, (p - 1.3) * Pit * 8760.0 / 1000.0));
        if (p <= 1.2) {
            r.ok({"Sehr effizient (PUE ≤ 1,2).", "Very efficient (PUE ≤ 1.2)."});
        } else if (p <= 1.5) {
            r.ok({"Gut (PUE ≤ 1,5).", "Good (PUE ≤ 1.5)."});
        } else {
            r.warn({"PUE > 1,5 – Kühlung/Freie Kühlung, Temperaturen und USV-Wirkungsgrad prüfen.",
                    "PUE > 1.5 – check cooling/free cooling, temperatures and UPS efficiency."});
        }
        if (ctx.norm == NormProfile::DE && p > 1.3) {
            r.info({"EnEfG: Bestands-RZ müssen ab 1.7.2030 PUE ≤ 1,3 erreichen.", "EnEfG: existing DCs must reach PUE ≤ 1.3 from 1 July 2030."});
        }
        return r;
    };
    return c;
}

Calculator rackCooling() {
    Calculator c;
    c.id = "dc_rack_cooling";
    c.category = "datacenter";
    c.title = {"Rack-Kühlung & Redundanz", "Rack cooling & redundancy"};
    c.summary = {"Luftmenge je Rack, Kühlgeräte N+1/2N", "Airflow per rack, cooling units N+1/2N"};
    c.description = {
        "Die IT-Leistung wird vollständig zu Wärme.  V̇ = P / (ρ · c · ΔT)   (Luft: ρ = 1,2 kg/m³, c = 1,005 kJ/(kg·K))\n"
        "ΔT = Temperaturdifferenz Server-Ausblas − Ansaugung (typ. 10–15 K). Anzahl Umluftkühlgeräte: N = ⌈Last / Leistung je Gerät⌉, "
        "zuzüglich Redundanz. Empfohlene Zulufttemperatur nach ASHRAE TC 9.9: 18–27 °C.",
        "All IT power becomes heat.  V̇ = P / (ρ · c · ΔT)   (air: ρ = 1.2 kg/m³, c = 1.005 kJ/(kg·K))\n"
        "ΔT = server exhaust − inlet temperature (typ. 10–15 K). Number of cooling units: N = ⌈load / unit capacity⌉ plus "
        "redundancy. Recommended inlet temperature per ASHRAE TC 9.9: 18–27 °C."};
    c.inputs = {
        Num::number("racks", {"Anzahl Racks", "Number of racks"}, "", 20).range(1, 100000),
        Num::number("kw", {"Leistung je Rack", "Power per rack"}, "kW", 8).positive(),
        Num::number("dT", {"ΔT über Server", "ΔT across servers"}, "K", 12).positive(),
        Num::number("unit", {"Kühlleistung je Gerät (CRAH/CRAC)", "Capacity per cooling unit (CRAH/CRAC)"}, "kW", 100).positive(),
        Num::choice("red", {"Redundanz", "Redundancy"}, redundancyOptions(), "N+1"),
    };
    c.outputs = {Out::number("P", {"IT-Wärmelast gesamt", "Total IT heat load"}, "kW", 0).primary(),
                 Out::number("qr", {"Luftmenge je Rack", "Airflow per rack"}, "m³/h", 0),
                 Out::number("qrcfm", {"Luftmenge je Rack", "Airflow per rack"}, "CFM", 0),
                 Out::number("q", {"Luftmenge gesamt", "Total airflow"}, "m³/h", 0),
                 Out::number("units", {"Kühlgeräte inkl. Redundanz", "Cooling units incl. redundancy"}, "", 0).primary(),
                 Out::number("inst", {"Installierte Kühlleistung", "Installed cooling capacity"}, "kW", 0)};
    c.keywords = {"rack", "kühlung", "crah", "crac", "rechenzentrum", "luftmenge", "n+1", "cooling"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double rackKw = in.num("kw");
        const double P = std::round(in.num("racks")) * rackKw;
        const double perRack = rackKw * 3600.0 / (1.2 * 1.005 * in.num("dT"));
        const double base = std::ceil(P / in.num("unit") - 1e-9);
        const double units = unitsWithRedundancy(base, in.choice("red"));
        r.set("P", P);
        r.set("qr", perRack);
        r.set("qrcfm", perRack * 0.588578);
        r.set("q", perRack * std::round(in.num("racks")));
        r.set("units", units);
        r.set("inst", units * in.num("unit"));
        if (rackKw > 20) {
            r.info({"Über ca. 20 kW/Rack: Einhausung, In-Row- oder Flüssigkühlung (Rücktür-WT, Direct Liquid Cooling) prüfen.",
                    "Above approx. 20 kW/rack: consider containment, in-row or liquid cooling (rear-door HX, direct liquid cooling)."});
        }
        return r;
    };
    return c;
}

Calculator upsSizing() {
    Calculator c;
    c.id = "dc_ups";
    c.category = "datacenter";
    c.title = {"USV-Dimensionierung", "UPS sizing"};
    c.summary = {"Module mit Redundanz, Auslastung, Batterie-Autonomie", "Modules with redundancy, loading, battery autonomy"};
    c.description = {
        "P_Auslegung = (P_IT + P_sonstige) · (1 + Reserve)    S = P / cos φ\n"
        "Module N = ⌈P / (P_Modul · max. Auslastung)⌉, dazu Redundanz (N+1, 2N = zwei getrennte Systeme).\n"
        "Batterie: E = P · t / η_WR    I_DC = P / (η_WR · U_DC). Für kurze Autonomien Batterien nach Leistungstabelle "
        "(W/Zelle) des Herstellers wählen – die Ah-Nennkapazität ist dann deutlich höher als E / U.",
        "P_design = (P_IT + P_other) · (1 + reserve)    S = P / cos φ\n"
        "Modules N = ⌈P / (P_module · max. loading)⌉ plus redundancy (N+1; 2N = two separate systems).\n"
        "Battery: E = P · t / η_inv    I_DC = P / (η_inv · U_DC). For short autonomy select batteries from the manufacturer's "
        "constant-power table (W/cell) – the nominal Ah rating will be much higher than E / U."};
    c.inputs = {
        Num::number("Pit", {"IT-Last", "IT load"}, "kW", 150).positive(),
        Num::number("Poth", {"Sonstige gesicherte Last", "Other protected load"}, "kW", 10).nonNegative(),
        Num::number("res", {"Wachstumsreserve", "Growth reserve"}, "%", 20).range(0, 300),
        Num::number("pf", {"Leistungsfaktor der Last", "Load power factor"}, "", 0.95).rangeExclusiveMin(0, 1),
        Num::number("mod", {"Modulleistung (Wirkleistung)", "Module rating (active power)"}, "kW", 50).positive(),
        Num::number("load", {"Max. Auslastung je Modul", "Max. loading per module"}, "%", 80).rangeExclusiveMin(0, 100),
        Num::choice("red", {"Redundanz", "Redundancy"}, redundancyOptions(), "N+1"),
        Num::number("t", {"Autonomiezeit", "Autonomy time"}, "min", 10).positive(),
        Num::number("Udc", {"Batterie-Nennspannung", "Battery nominal voltage"}, "V", 480).positive(),
        Num::number("eta", {"Wirkungsgrad Wechselrichter", "Inverter efficiency"}, "%", 96).rangeExclusiveMin(0, 100),
    };
    c.outputs = {Out::number("P", {"Auslegungsleistung", "Design power"}, "kW", 1),
                 Out::number("S", {"Scheinleistung", "Apparent power"}, "kVA", 1),
                 Out::number("n", {"Module ohne Redundanz (N)", "Modules without redundancy (N)"}, "", 0),
                 Out::number("ntot", {"Module gesamt inkl. Redundanz", "Total modules incl. redundancy"}, "", 0).primary(),
                 Out::number("util", {"Auslastung je Modul (Normalbetrieb)", "Loading per module (normal)"}, "%", 0),
                 Out::number("E", {"Batterieenergie (Entnahme)", "Battery energy (discharge)"}, "kWh", 1).primary(),
                 Out::number("Idc", {"Batterie-Entladestrom", "Battery discharge current"}, "A", 0)};
    c.keywords = {"usv", "ups", "batterie", "autonomie", "rechenzentrum", "redundanz"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double P = (in.num("Pit") + in.num("Poth")) * (1.0 + in.num("res") / 100.0);
        const double mod = in.num("mod");
        const double n = std::ceil(P / (mod * in.num("load") / 100.0) - 1e-9);
        const auto& red = in.choice("red");
        const double ntot = unitsWithRedundancy(n, red);
        const double eta = in.num("eta") / 100.0;
        r.set("P", P);
        r.set("S", P / in.num("pf"));
        r.set("n", n);
        r.set("ntot", ntot);
        r.set("util", P / (ntot * mod) * 100.0);
        r.set("E", P * in.num("t") / 60.0 / eta);
        r.set("Idc", P * 1000.0 / (eta * in.num("Udc")));
        if (red == "2N") r.info({"2N: zwei getrennte USV-Systeme mit je N Modulen (A- und B-Versorgung).",
                                 "2N: two separate UPS systems with N modules each (A and B feed)."});
        return r;
    };
    return c;
}

Calculator availability() {
    Calculator c;
    c.id = "dc_availability";
    c.category = "datacenter";
    c.title = {"Verfügbarkeit & Ausfallzeit", "Availability & downtime"};
    c.summary = {"Prozent ↔ Ausfallzeit pro Jahr, Tier-Vergleich", "Percent ↔ downtime per year, tier comparison"};
    c.description = {
        "Ausfallzeit = (1 − A) · 8760 h    Serie: A = A1 · A2    Parallel (redundant): A = 1 − (1 − A1)·(1 − A2)\n"
        "Häufig zitierte Richtwerte: Tier I 99,671 %, Tier II 99,741 %, Tier III 99,982 %, Tier IV 99,995 % "
        "(keine offiziellen Uptime-Institute-Kriterien).",
        "Downtime = (1 − A) · 8760 h    Series: A = A1 · A2    Parallel (redundant): A = 1 − (1 − A1)·(1 − A2)\n"
        "Frequently quoted guide values: Tier I 99.671 %, Tier II 99.741 %, Tier III 99.982 %, Tier IV 99.995 % "
        "(not official Uptime Institute criteria)."};
    c.inputs = {
        Num::number("A", {"Verfügbarkeit einer Komponente/Strecke", "Availability of one component/path"}, "%", 99.9).rangeExclusiveMin(0, 100),
        Num::number("k", {"Anzahl paralleler (redundanter) Strecken", "Number of parallel (redundant) paths"}, "", 1).range(1, 10),
    };
    c.outputs = {Out::number("Atot", {"Gesamtverfügbarkeit", "Total availability"}, "%", 5).primary(),
                 Out::number("h", {"Ausfallzeit pro Jahr", "Downtime per year"}, "h", 2).primary(),
                 Out::number("min", {"Ausfallzeit pro Jahr", "Downtime per year"}, "min", 1),
                 Out::number("minMonth", {"Ausfallzeit pro Monat", "Downtime per month"}, "min", 1)};
    c.keywords = {"verfügbarkeit", "tier", "availability", "redundanz", "ausfall", "en 50600"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double a = in.num("A") / 100.0;
        const double k = std::round(in.num("k"));
        const double tot = 1.0 - std::pow(1.0 - a, k);
        const double hours = (1.0 - tot) * 8760.0;
        r.set("Atot", tot * 100.0);
        r.set("h", hours);
        r.set("min", hours * 60.0);
        r.set("minMonth", hours * 60.0 / 12.0);
        r.info({"Annahme: unabhängige Ausfälle ohne gemeinsame Fehlerursache (Common Cause).",
                "Assumption: independent failures without common cause."});
        return r;
    };
    return c;
}

Table ashrae() {
    Table t;
    t.id = "ashrae_tc99";
    t.category = "datacenter";
    t.title = {"ASHRAE TC 9.9: Klimaklassen IT", "ASHRAE TC 9.9: IT environmental classes"};
    t.summary = {"Zulufttemperatur und Feuchte am Server-Eintritt", "Inlet temperature and humidity at server intake"};
    t.description = {"ASHRAE Thermal Guidelines for Data Processing Environments (4./5. Auflage). Werte am Lufteintritt der IT-Geräte; "
                     "gültige Ausgabe und Herstellerangaben prüfen.",
                     "ASHRAE Thermal Guidelines for Data Processing Environments (4th/5th ed.). Values at the IT equipment air inlet; "
                     "check the edition in force and vendor data."};
    t.columns = {{"Bereich", "Range"}, {"Temperatur", "Temperature"}, {"Feuchte", "Humidity"}, {"Max. Taupunkt", "Max. dew point"}};
    t.rows = {{{"Empfohlen (A1–A4)", "Recommended (A1–A4)"}, "18–27 °C", {"TP −9 °C … 15 °C, ≤ 60 % r. F.", "DP −9 °C … 15 °C, ≤ 60 % RH"}, "15 °C"},
              {{"Zulässig A1", "Allowable A1"}, "15–32 °C", {"8–80 % r. F.", "8–80 % RH"}, "17 °C"},
              {{"Zulässig A2", "Allowable A2"}, "10–35 °C", {"8–80 % r. F.", "8–80 % RH"}, "21 °C"},
              {{"Zulässig A3", "Allowable A3"}, "5–40 °C", {"8–85 % r. F.", "8–85 % RH"}, "24 °C"},
              {{"Zulässig A4", "Allowable A4"}, "5–45 °C", {"8–90 % r. F.", "8–90 % RH"}, "24 °C"}};
    return t;
}

Table availabilityClasses() {
    Table t;
    t.id = "dc_availability_classes";
    t.category = "datacenter";
    t.title = {"Verfügbarkeitsklassen EN 50600 / Tier", "Availability classes EN 50600 / tier"};
    t.summary = {"Redundanz und Wartbarkeit der Infrastruktur", "Redundancy and maintainability of infrastructure"};
    t.description = {"EN 50600-1 Verfügbarkeitsklassen 1–4; Tier I–IV nach Uptime Institute zum Vergleich (nicht identisch).",
                     "EN 50600-1 availability classes 1–4; Uptime Institute Tier I–IV for comparison (not identical)."};
    t.columns = {{"EN 50600", "EN 50600"}, {"Vergleich", "Compare"}, {"Aufbau", "Design"}, {"Eigenschaft", "Property"}};
    t.rows = {{{"Klasse 1", "Class 1"}, "Tier I", {"ein Versorgungspfad, keine Redundanz", "single path, no redundancy"},
               {"geringe Verfügbarkeit", "low availability"}},
              {{"Klasse 2", "Class 2"}, "Tier II", {"ein Pfad, redundante Komponenten (N+1)", "single path, redundant components (N+1)"},
               {"mittlere Verfügbarkeit", "medium availability"}},
              {{"Klasse 3", "Class 3"}, "Tier III", {"mehrere Pfade, ein aktiver", "multiple paths, one active"},
               {"Wartung im laufenden Betrieb", "concurrently maintainable"}},
              {{"Klasse 4", "Class 4"}, "Tier IV", {"mehrere aktive Pfade (2N)", "multiple active paths (2N)"},
               {"fehlertolerant", "fault tolerant"}}};
    return t;
}

}  // namespace

void addDatacenter(std::vector<Calculator>& out) {
    out.push_back(pue());
    out.push_back(rackCooling());
    out.push_back(upsSizing());
    out.push_back(availability());
}

void addDatacenterTables(std::vector<Table>& out) {
    out.push_back(ashrae());
    out.push_back(availabilityClasses());
}

}  // namespace elc::catalog
