// Calculators: Rechenzentrum / Data centre
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/datacenter.hpp"

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

/// EnEfG §11 PUE limit text and check (law in force 2023 or cabinet draft 2026)
struct EnefgLimits {
    double pue2027;  // existing DCs from 1 July 2027 (new builds: the single limit)
    double pue2030;
};

EnefgLimits enefgLimits(bool newBuild, bool draft) {
    if (newBuild) return {1.2, 1.2};
    return draft ? EnefgLimits{1.6, 1.4} : EnefgLimits{1.5, 1.3};
}

const Text kEnefgSource{
    "EnEfG (2023) §11: Neubau mit Betriebsbeginn ab 1.7.2026 PUE ≤ 1,2; Bestand PUE ≤ 1,5 ab 1.7.2027 und ≤ 1,3 ab "
    "1.7.2030; Neubau ERF ≥ 10 % (ab 1.7.2026), 15 % (2027), 20 % (2028). Kabinettsentwurf EnEfG-Novelle vom 24.6.2026: "
    "Bestand 1,6 / 1,4, Neubau 1,2 (4 Jahre Frist), Geltung ab 500 kW IT-Leistung – im Bundestag noch nicht "
    "verabschiedet (Stand Juli 2026). Gültige Fassung prüfen.",
    "German EnEfG (2023) §11: new DCs starting operation from 1 July 2026 PUE ≤ 1.2; existing DCs PUE ≤ 1.5 from "
    "1 July 2027 and ≤ 1.3 from 1 July 2030; new DCs ERF ≥ 10 % (from July 2026), 15 % (2027), 20 % (2028). Cabinet draft "
    "of the amendment of 24 June 2026: existing 1.6 / 1.4, new 1.2 (4-year period), scope from 500 kW IT load – not yet "
    "passed (as of July 2026). Check the version in force."};

std::vector<Option> startOptions() {
    return {{"existing", {"Bestand (Betrieb vor 1.7.2026)", "Existing (operating before 1 July 2026)"}},
            {"new", {"Neubau (Betrieb ab 1.7.2026)", "New build (operating from 1 July 2026)"}}};
}

std::vector<Option> lawOptions() {
    return {{"2023", {"EnEfG 2023 (geltend)", "EnEfG 2023 (in force)"}},
            {"draft2026", {"Novelle 2026 (Kabinettsentwurf)", "2026 amendment (cabinet draft)"}}};
}

/// Adds the EnEfG messages for a PUE value (Germany only)
void checkEnefg(Result& r, double pue, const Inputs& in, const Context& ctx) {
    if (ctx.norm != NormProfile::DE) return;
    const bool newBuild = in.choice("start") == "new";
    const bool draft = in.choice("law") == "draft2026";
    const auto lim = enefgLimits(newBuild, draft);
    const auto f = [](double v) { return formatNumber(v, 2); };
    if (newBuild) {
        if (pue <= lim.pue2027 + 1e-9) {
            r.ok({"EnEfG Neubau: PUE ≤ " + f(lim.pue2027).de + " erfüllt.", "EnEfG new build: PUE ≤ " + f(lim.pue2027).en + " met."});
        } else {
            r.warn({"EnEfG Neubau: PUE ≤ " + f(lim.pue2027).de + " nicht erfüllt.", "EnEfG new build: PUE ≤ " + f(lim.pue2027).en + " not met."});
        }
        return;
    }
    if (pue <= lim.pue2030 + 1e-9) {
        r.ok({"EnEfG Bestand: erfüllt auch die Stufe ab 1.7.2030 (PUE ≤ " + f(lim.pue2030).de + ").",
              "EnEfG existing: also meets the stage from 1 July 2030 (PUE ≤ " + f(lim.pue2030).en + ")."});
    } else if (pue <= lim.pue2027 + 1e-9) {
        r.warn({"EnEfG Bestand: erfüllt ab 1.7.2027 (≤ " + f(lim.pue2027).de + "), aber nicht ab 1.7.2030 (≤ " + f(lim.pue2030).de + ").",
                "EnEfG existing: meets 1 July 2027 (≤ " + f(lim.pue2027).en + ") but not 1 July 2030 (≤ " + f(lim.pue2030).en + ")."});
    } else {
        r.warn({"EnEfG Bestand: PUE ≤ " + f(lim.pue2027).de + " ab 1.7.2027 nicht erfüllt.",
                "EnEfG existing: PUE ≤ " + f(lim.pue2027).en + " from 1 July 2027 not met."});
    }
}

void rateEfficiency(Result& r, double p) {
    if (p <= 1.2) {
        r.ok({"Sehr effizient (PUE ≤ 1,2).", "Very efficient (PUE ≤ 1.2)."});
    } else if (p <= 1.5) {
        r.ok({"Gut (PUE ≤ 1,5).", "Good (PUE ≤ 1.5)."});
    } else {
        r.warn({"PUE > 1,5 – Kühlung/freie Kühlung, Temperaturen und USV-Wirkungsgrad prüfen.",
                "PUE > 1.5 – check cooling/free cooling, temperatures and UPS efficiency."});
    }
}

Calculator pue() {
    Calculator c;
    c.id = "dc_pue";
    c.category = "datacenter";
    c.title = {"PUE / Energieeffizienz", "PUE / energy efficiency"};
    c.summary = {"PUE aus Leistung oder Jahresverbrauch, DCiE, Kosten, EnEfG", "PUE from power or annual energy, DCiE, cost, EnEfG"};
    c.description = cat(Text{
        "PUE = E_gesamt / E_IT (ISO/IEC 30134-2: Jahresenergie, gemessen am Übergabepunkt bzw. am Ausgang USV/PDU)\n"
        "Aus Leistung (Momentanwert): PUE = P_gesamt / P_IT, Jahresenergie = P · 8760 h    DCiE = 1 / PUE · 100 %\n"
        "Overhead = Gesamt − IT (Kühlung, USV-Verluste, Licht …)\n",
        "PUE = E_total / E_IT (ISO/IEC 30134-2: annual energy, metered at the utility point and at the UPS/PDU output)\n"
        "From power (instantaneous): PUE = P_total / P_IT, annual energy = P · 8760 h    DCiE = 1 / PUE · 100 %\n"
        "Overhead = total − IT (cooling, UPS losses, lighting …)\n"}, kEnefgSource);
    c.inputs = {
        Num::choice("basis", {"Grundlage", "Basis"},
                    {{"power", {"Leistung (kW, Mittelwert)", "Power (kW, average)"}},
                     {"energy", {"Jahresverbrauch (MWh, Zählerwerte)", "Annual energy (MWh, metered)"}}},
                    "power"),
        Num::number("Ptot", {"Gesamtleistung RZ", "Total facility power"}, "kW", 650).positive().showIf("basis", {"power"}),
        Num::number("Pit", {"IT-Leistung", "IT power"}, "kW", 450).positive().showIf("basis", {"power"}),
        Num::number("Etot", {"Gesamtverbrauch RZ pro Jahr", "Total facility energy per year"}, "MWh", 5000).positive()
            .showIf("basis", {"energy"}),
        Num::number("Eit", {"IT-Verbrauch pro Jahr", "IT energy per year"}, "MWh", 3600).positive().showIf("basis", {"energy"}),
        Num::number("price", {"Strompreis pro kWh", "Energy price per kWh"}, "€ / CHF", 0.20).nonNegative(),
        Num::choice("start", {"Rechenzentrum", "Data centre"}, startOptions(), "existing"),
        Num::choice("law", {"EnEfG-Fassung (nur DE)", "EnEfG version (Germany only)"}, lawOptions(), "2023"),
    };
    c.outputs = {Out::number("pue", {"PUE", "PUE"}, "", 2).primary(),
                 Out::number("dcie", {"DCiE", "DCiE"}, "%", 1),
                 Out::number("over", {"Overhead-Leistung (Mittel)", "Overhead power (average)"}, "kW", 1),
                 Out::number("Eyear", {"Energie pro Jahr gesamt", "Total energy per year"}, "MWh", 0),
                 Out::number("Cover", {"Kosten Overhead pro Jahr", "Overhead cost per year"}, "€", 0),
                 Out::number("save", {"Einsparung bei PUE 1,3", "Saving at PUE 1.3"}, "MWh/a", 0)};
    c.keywords = {"pue", "rechenzentrum", "data center", "dcie", "enefg", "effizienz", "verbrauch", "jahresverbrauch"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const bool energy = in.choice("basis") == "energy";
        const double tot = energy ? in.num("Etot") * 1000.0 / 8760.0 : in.num("Ptot");  // kW average
        const double it = energy ? in.num("Eit") * 1000.0 / 8760.0 : in.num("Pit");
        if (it > tot) throw CalcError({"IT-Anteil größer als Gesamtverbrauch.", "IT share exceeds the total."});
        const double p = tot / it;
        r.set("pue", p);
        r.set("dcie", 100.0 / p);
        r.set("over", tot - it);
        r.set("Eyear", tot * 8760.0 / 1000.0);
        r.set("Cover", (tot - it) * 8760.0 * in.num("price"), currencySymbol(ctx));
        r.set("save", std::max(0.0, (p - 1.3) * it * 8760.0 / 1000.0));
        rateEfficiency(r, p);
        if (!energy) {
            r.info({"Für den EnEfG-Nachweis zählt der Jahres-PUE aus Energiezählern („Jahresverbrauch“).",
                    "EnEfG compliance uses the annual PUE from energy meters (“Annual energy”)."});
        }
        checkEnefg(r, p, in, ctx);
        return r;
    };
    return c;
}

Calculator pueBalance() {
    Calculator c;
    c.id = "dc_pue_balance";
    c.category = "datacenter";
    c.title = {"PUE-Bilanz aus Verbrauchern", "PUE balance from consumers"};
    c.summary = {"PUE, pPUE, Jahresverbrauch, Kosten, CO₂, Abwärme – aus IT, USV, Kühlung",
                 "PUE, pPUE, annual energy, cost, CO₂, waste heat – from IT, UPS, cooling"};
    c.description = cat(Text{
        "USV-Verlust = P_IT · (1/η_USV − 1)    Verteilverluste = (P_IT + USV-Verlust) · v (Trafo, Leitungen vor der USV)\n"
        "Kühllast Q = P_IT + USV-Verlust + Sonstiges    P_Kühlung = Q · (1 − Anteil freie Kühlung) / EER + Q · Hilfsenergie\n"
        "P_gesamt = P_IT + USV- + Verteilverluste + P_Kühlung + Sonstiges    PUE = P_gesamt / P_IT\n"
        "pPUE Kühlung = (P_IT + P_Kühlung) / P_IT    pPUE Strom = (P_IT + Verluste) / P_IT (ISO/IEC 30134-2 Anh.)\n"
        "ERF = genutzte Abwärme / Gesamtenergie (ISO/IEC 30134-6). Jahreswerte mit 8760 h und mittleren Leistungen.\n"
        "EER und Anteil freie Kühlung als Jahresmittel (Klima, Kaltwassertemperatur) beim Planer/Hersteller erfragen.\n",
        "UPS loss = P_IT · (1/η_UPS − 1)    distribution loss = (P_IT + UPS loss) · v (transformer, cabling before the UPS)\n"
        "Cooling load Q = P_IT + UPS loss + other    P_cooling = Q · (1 − free cooling share) / EER + Q · auxiliaries\n"
        "P_total = P_IT + UPS + distribution losses + P_cooling + other    PUE = P_total / P_IT\n"
        "pPUE cooling = (P_IT + P_cooling) / P_IT    pPUE power = (P_IT + losses) / P_IT (ISO/IEC 30134-2 annex)\n"
        "ERF = reused heat / total energy (ISO/IEC 30134-6). Annual values with 8760 h and average powers.\n"
        "Take annual EER and free cooling share (climate, chilled water temperature) from the designer/manufacturer.\n"},
        kEnefgSource);
    c.inputs = {
        Num::number("Pit", {"IT-Leistung (Jahresmittel)", "IT power (annual average)"}, "kW", 400).positive()
            .withHint({"Mittelwert, nicht Anschlussleistung (typ. 40–70 % der installierten IT-Leistung)",
                       "Average, not rated power (typically 40–70 % of installed IT power)"}),
        Num::number("eta", {"USV-Wirkungsgrad", "UPS efficiency"}, "%", 95).range(50, 100)
            .startsSection({"Stromversorgung", "Power supply"}),
        Num::number("dist", {"Verluste Trafo/Verteilung", "Transformer/distribution losses"}, "%", 2).range(0, 20),
        Num::number("Pother", {"Beleuchtung, Sicherheit, Sonstiges", "Lighting, security, other"}, "kW", 10).nonNegative(),
        Num::number("eer", {"EER Kälteerzeugung (Jahresmittel)", "Chiller EER (annual average)"}, "", 4).range(0.5, 30)
            .startsSection({"Kühlung", "Cooling"}),
        Num::number("free", {"Anteil freie Kühlung", "Free cooling share"}, "%", 40).range(0, 100)
            .withHint({"Anteil der Kühlenergie ohne Verdichter (Klima Basel/Freiburg: oft 40–70 %)",
                       "Share of cooling energy without compressors"}),
        Num::number("aux", {"Hilfsenergie Ventilatoren/Pumpen/Rückkühler (bezogen auf Kühllast)",
                            "Fans/pumps/dry coolers (relative to cooling load)"}, "%", 8)
            .range(0, 50),
        Num::number("price", {"Strompreis pro kWh", "Energy price per kWh"}, "€ / CHF", 0.20).nonNegative()
            .startsSection({"Kosten, CO₂, Abwärme, EnEfG", "Cost, CO₂, waste heat, EnEfG"}),
        Num::number("ef", {"CO₂-Emissionsfaktor Strom", "CO₂ emission factor"}, "kg/kWh", 0.363).nonNegative()
            .withHint({"DE-Strommix 2024: 0,363 kg/kWh (UBA, Schätzung); sonst Wert des Versorgers, Ökostrom-Vertrag beachten",
                       "German mix 2024: 0.363 kg/kWh (UBA estimate); otherwise use the supplier's value"}),
        Num::number("reuse", {"Genutzte Abwärme (Mittel)", "Reused waste heat (average)"}, "kW", 0).nonNegative(),
        Num::choice("start", {"Rechenzentrum", "Data centre"}, startOptions(), "existing"),
        Num::choice("law", {"EnEfG-Fassung (nur DE)", "EnEfG version (Germany only)"}, lawOptions(), "2023"),
    };
    c.outputs = {
        Out::number("pue", {"PUE", "PUE"}, "", 2).primary(),
        Out::number("Ptot", {"Gesamtleistung RZ (Mittel)", "Total facility power (average)"}, "kW", 1).primary().abbrev({"P gesamt", "P total"}),
        Out::number("ppueC", {"pPUE Kühlung", "pPUE cooling"}, "", 3),
        Out::number("ppueP", {"pPUE Stromversorgung", "pPUE power supply"}, "", 3),
        Out::number("dcie", {"DCiE", "DCiE"}, "%", 1),
        Out::number("Pups", {"USV-Verluste", "UPS losses"}, "kW", 1),
        Out::number("Pdist", {"Verteilverluste", "Distribution losses"}, "kW", 1),
        Out::number("Q", {"Kühllast", "Cooling load"}, "kW", 1),
        Out::number("Pcool", {"Leistung Kühlung", "Cooling power"}, "kW", 1),
        Out::number("shareCool", {"Anteil Kühlung am Gesamtverbrauch", "Cooling share of total"}, "%", 1),
        Out::number("Etot", {"Jahresverbrauch gesamt", "Total annual energy"}, "MWh", 0),
        Out::number("Eit", {"Jahresverbrauch IT", "IT annual energy"}, "MWh", 0),
        Out::number("Cyear", {"Stromkosten pro Jahr", "Energy cost per year"}, "€", 0),
        Out::number("Cover", {"davon Overhead (nicht IT)", "of which overhead (non-IT)"}, "€", 0),
        Out::number("co2", {"CO₂ pro Jahr", "CO₂ per year"}, "t", 0),
        Out::number("heat", {"Abwärme verfügbar (Kühllast)", "Waste heat available (cooling load)"}, "MWh/a", 0),
        Out::number("erf", {"ERF (Energiewiederverwendung)", "ERF (energy reuse factor)"}, "%", 1),
    };
    c.keywords = {"pue", "ppue", "rechenzentrum", "data center", "verbrauch", "usv", "kühlung", "freie kühlung", "enefg",
                  "erf", "abwärme", "co2", "energiebilanz"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        datacenter::PueInputs pi;
        pi.itPower = in.num("Pit");
        pi.upsEfficiency = in.num("eta") / 100.0;
        pi.distributionLoss = in.num("dist") / 100.0;
        pi.otherPower = in.num("Pother");
        pi.eer = in.num("eer");
        pi.freeCooling = in.num("free") / 100.0;
        pi.auxShare = in.num("aux") / 100.0;
        const auto b = datacenter::pueBalance(pi);
        const std::string cur = currencySymbol(ctx);
        const double hours = 8760.0;
        r.set("pue", b.pue);
        r.set("Ptot", b.total);
        r.set("ppueC", b.pPueCooling);
        r.set("ppueP", b.pPuePower);
        r.set("dcie", 100.0 / b.pue);
        r.set("Pups", b.upsLoss);
        r.set("Pdist", b.distLoss);
        r.set("Q", b.coolingLoad);
        r.set("Pcool", b.coolingPower);
        r.set("shareCool", 100.0 * b.coolingPower / b.total);
        r.set("Etot", b.total * hours / 1000.0);
        r.set("Eit", pi.itPower * hours / 1000.0);
        r.set("Cyear", b.total * hours * in.num("price"), cur);
        r.set("Cover", (b.total - pi.itPower) * hours * in.num("price"), cur);
        r.set("co2", b.total * hours * in.num("ef") / 1000.0);
        r.set("heat", b.coolingLoad * hours / 1000.0);
        const double erf = in.num("reuse") / b.total;
        r.set("erf", 100.0 * erf);
        if (in.num("reuse") > b.coolingLoad) {
            r.warn({"Genutzte Abwärme größer als die Kühllast – Eingabe prüfen.", "Reused heat exceeds the cooling load – check input."});
        }
        rateEfficiency(r, b.pue);
        if (b.coolingPower > 0.3 * pi.itPower) {
            r.info({"Kühlung > 30 % der IT-Leistung: höhere Kaltwasser-/Zulufttemperaturen (ASHRAE A1) und mehr freie Kühlung prüfen.",
                    "Cooling > 30 % of IT power: check higher chilled water/supply temperatures (ASHRAE A1) and more free cooling."});
        }
        checkEnefg(r, b.pue, in, ctx);
        if (ctx.norm == NormProfile::DE && in.choice("start") == "new" && erf < 0.10) {
            r.info({"EnEfG Neubau: ERF ≥ 10 % ab 1.7.2026, 15 % ab 2027, 20 % ab 2028 (Ausnahmen beachten).",
                    "EnEfG new build: ERF ≥ 10 % from July 2026, 15 % from 2027, 20 % from 2028 (exemptions apply)."});
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
    out.push_back(pueBalance());
    out.push_back(rackCooling());
    out.push_back(upsSizing());
    out.push_back(availability());
}

void addDatacenterTables(std::vector<Table>& out) {
    out.push_back(ashrae());
    out.push_back(availabilityClasses());
}

}  // namespace elc::catalog
