// Calculators and tables (category process): explosion protection.
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/explosion.hpp"

namespace elc::catalog {

namespace {

struct Substance {
    const char* id;
    Text name;
    double ait;  // ignition temperature °C
    const char* group;
    Text note;
};

// Guide values from manufacturer documents (Schmersal Ex poster, Festo explosion protection handout) and GESTIS.
// The safety data sheet / IEC 60079-20-1 of the actual substance is decisive.
const std::vector<Substance>& substances() {
    static const std::vector<Substance> s{
        {"methane", {"Methan (Erdgas)", "Methane (natural gas)"}, 595, "IIA", {}},
        {"hydrogen", {"Wasserstoff", "Hydrogen"}, 560, "IIC", {}},
        {"ammonia", {"Ammoniak", "Ammonia"}, 630, "IIA", {}},
        {"propane", {"Propan", "Propane"}, 470, "IIA", {}},
        {"acetone", {"Aceton", "Acetone"}, 535, "IIA", {}},
        {"ethylene", {"Ethylen", "Ethylene"}, 425, "IIB", {}},
        {"methanol", {"Methanol", "Methanol"}, 440, "IIA", {}},
        {"ethanol", {"Ethanol", "Ethanol"}, 400, "IIB", {"Gruppe je nach Quelle IIA oder IIB – IIB angesetzt", "Group IIA or IIB depending on source – IIB assumed"}},
        {"isopropanol", {"2-Propanol (Isopropanol)", "2-Propanol (isopropanol)"}, 425, "IIA", {}},
        {"butane", {"n-Butan", "n-Butane"}, 365, "IIA", {}},
        {"acetylene", {"Acetylen", "Acetylene"}, 305, "IIC", {}},
        {"cyclohexane", {"Cyclohexan", "Cyclohexane"}, 259, "IIA", {}},
        {"diesel", {"Dieselkraftstoff", "Diesel fuel"}, 220, "IIA", {"Zündtemperatur ≥ 220 °C (EN 590)", "Ignition temperature ≥ 220 °C (EN 590)"}},
        {"h2s", {"Schwefelwasserstoff", "Hydrogen sulphide"}, 270, "IIB", {}},
        {"ether", {"Diethylether", "Diethyl ether"}, 170, "IIB", {}},
        {"acetaldehyde", {"Acetaldehyd", "Acetaldehyde"}, 155, "IIA", {}},
        {"cs2", {"Schwefelkohlenstoff", "Carbon disulphide"}, 95, "IIC", {}},
    };
    return s;
}

const Substance* findSubstance(const std::string& id) {
    for (const auto& s : substances()) {
        if (id == s.id) return &s;
    }
    return nullptr;
}

struct Zone {
    const char* id;
    bool dust;
    const char* category;  // ATEX 2014/34/EU
    const char* epl;       // IEC 60079-0
    const char* typical;   // typical type of protection for the marking example
    Text occurrence;
    Text protection;
};

const std::vector<Zone>& zones() {
    static const std::vector<Zone> z{
        {"0", false, "1G", "Ga", "ia", {"ständig, über lange Zeiträume oder häufig", "continuously, for long periods or frequently"},
         {"ia, ma (oder zwei unabhängige Gb-Zündschutzarten)", "ia, ma (or two independent Gb types)"}},
        {"1", false, "2G", "Gb", "db", {"gelegentlich im Normalbetrieb", "occasionally in normal operation"},
         {"db, eb, ib, mb, pxb, pyb, ob, qb – und alle Ga-Geräte", "db, eb, ib, mb, pxb, pyb, ob, qb – and all Ga equipment"}},
        {"2", false, "3G", "Gc", "ec", {"im Normalbetrieb nicht, wenn doch nur kurzzeitig", "not in normal operation, if so only briefly"},
         {"ec (früher nA), ic, mc, pzc, oc, nR – und alle Ga/Gb-Geräte", "ec (formerly nA), ic, mc, pzc, oc, nR – and all Ga/Gb equipment"}},
        {"20", true, "1D", "Da", "ta", {"Staubwolke ständig, langzeitig oder häufig", "dust cloud continuously, long-term or frequently"},
         {"ta, ia, ma", "ta, ia, ma"}},
        {"21", true, "2D", "Db", "tb", {"Staubwolke gelegentlich im Normalbetrieb", "dust cloud occasionally in normal operation"},
         {"tb, ib, mb, pb – und alle Da-Geräte", "tb, ib, mb, pb – and all Da equipment"}},
        {"22", true, "3D", "Dc", "tc", {"Staubwolke normal nicht, wenn doch nur kurzzeitig", "dust cloud not in normal operation, if so only briefly"},
         {"tc, ic, mc, pc – und alle Da/Db-Geräte", "tc, ic, mc, pc – and all Da/Db equipment"}},
    };
    return z;
}

const Zone& findZone(const std::string& id) {
    for (const auto& z : zones()) {
        if (id == z.id) return z;
    }
    return zones()[1];
}

std::string tName(int cls) { return "T" + std::to_string(cls); }

Calculator exSelection() {
    Calculator c;
    c.id = "ex_selection";
    c.category = "process";
    c.title = {"Explosionsschutz: Temperaturklasse & Kennzeichnung", "Explosion protection: temperature class & marking"};
    c.summary = {"Zone → Kategorie/EPL, Stoff → T-Klasse & Gruppe, Staub → T_max, Geräteprüfung",
                 "Zone → category/EPL, substance → T class & group, dust → T_max, equipment check"};
    c.description = {
        "Gas/Dampf: Die maximale Oberflächentemperatur des Geräts muss unter der Zündtemperatur liegen.\n"
        "Temperaturklassen (IEC 60079-0): T1 450 °C · T2 300 °C · T3 200 °C · T4 135 °C · T5 100 °C · T6 85 °C.\n"
        "Explosionsgruppe (IEC 60079-20-1): IIA ⊂ IIB ⊂ IIC – ein IIC-Gerät darf auch für IIB/IIA eingesetzt werden.\n"
        "Staub (IEC 60079-14): T_max ≤ 2/3 · T_Wolke und T_max ≤ T_5mm − 75 K (Schicht bis 5 mm).\n"
        "Zone 0/1/2 → Kategorie 1G/2G/3G bzw. EPL Ga/Gb/Gc; Zone 20/21/22 → 1D/2D/3D bzw. Da/Db/Dc (ATEX 2014/34/EU, "
        "1999/92/EG). Stoffwerte sind Richtwerte (Herstellerunterlagen Schmersal/Festo, GESTIS) – maßgeblich sind "
        "Sicherheitsdatenblatt, IEC 60079-20-1 und das Explosionsschutzdokument.",
        "Gas/vapour: the maximum surface temperature of the equipment must stay below the ignition temperature.\n"
        "Temperature classes (IEC 60079-0): T1 450 °C · T2 300 °C · T3 200 °C · T4 135 °C · T5 100 °C · T6 85 °C.\n"
        "Gas group (IEC 60079-20-1): IIA ⊂ IIB ⊂ IIC – IIC equipment may also be used for IIB/IIA.\n"
        "Dust (IEC 60079-14): T_max ≤ 2/3 · T_cloud and T_max ≤ T_5mm − 75 K (layers up to 5 mm).\n"
        "Zone 0/1/2 → category 1G/2G/3G or EPL Ga/Gb/Gc; zone 20/21/22 → 1D/2D/3D or Da/Db/Dc (ATEX 2014/34/EU, "
        "1999/92/EC). Substance data are guide values (Schmersal/Festo documents, GESTIS) – the safety data sheet, "
        "IEC 60079-20-1 and the explosion protection document are decisive."};
    std::vector<Option> subs;
    for (const auto& s : substances()) {
        subs.push_back({s.id, {s.name.de + " – " + formatNumber(s.ait, 0, Lang::De) + " °C, " + s.group,
                               s.name.en + " – " + formatNumber(s.ait, 0, Lang::En) + " °C, " + s.group}});
    }
    subs.push_back({"custom", {"Eigener Stoff (Zündtemperatur eingeben)", "Own substance (enter ignition temperature)"}});
    const std::vector<std::string> gasZones{"0", "1", "2"};
    const std::vector<std::string> dustZones{"20", "21", "22"};
    c.inputs = {
        Num::choice("zone", {"Zone", "Zone"},
                    {{"0", {"Zone 0 (Gas)", "Zone 0 (gas)"}}, {"1", {"Zone 1 (Gas)", "Zone 1 (gas)"}}, {"2", {"Zone 2 (Gas)", "Zone 2 (gas)"}},
                     {"20", {"Zone 20 (Staub)", "Zone 20 (dust)"}}, {"21", {"Zone 21 (Staub)", "Zone 21 (dust)"}},
                     {"22", {"Zone 22 (Staub)", "Zone 22 (dust)"}}},
                    "1"),
        Num::choice("sub", {"Stoff (Gas/Dampf)", "Substance (gas/vapour)"}, subs, "methane").showIf("zone", gasZones),
        Num::number("ait", {"Zündtemperatur (eigener Stoff)", "Ignition temperature (own substance)"}, "°C", 300).range(0, 1000)
            .showIf("sub", {"custom"}),
        Num::choice("grp", {"Explosionsgruppe (eigener Stoff)", "Gas group (own substance)"}, {{"IIA", "IIA"}, {"IIB", "IIB"}, {"IIC", "IIC"}},
                    "IIA")
            .showIf("sub", {"custom"}),
        Num::choice("dgroup", {"Staubgruppe", "Dust group"},
                    {{"IIIA", {"IIIA – brennbare Flusen", "IIIA – combustible flyings"}},
                     {"IIIB", {"IIIB – nicht leitfähiger Staub", "IIIB – non-conductive dust"}},
                     {"IIIC", {"IIIC – leitfähiger Staub", "IIIC – conductive dust"}}},
                    "IIIB")
            .showIf("zone", dustZones),
        Num::number("tcl", {"Zündtemperatur Staubwolke", "Ignition temperature dust cloud"}, "°C", 450).range(50, 1200).showIf("zone", dustZones),
        Num::number("t5", {"Glimmtemperatur 5-mm-Schicht", "Ignition temperature 5 mm layer"}, "°C", 300).range(50, 1200)
            .showIf("zone", dustZones),
        Num::number("layer", {"Staubschichtdicke", "Dust layer thickness"}, "mm", 5).range(0, 500).showIf("zone", dustZones),
        Num::choice("devT", {"Gerät: Temperaturklasse", "Equipment: temperature class"},
                    {{"none", {"– nicht prüfen –", "– do not check –"}}, {"T1", "T1"}, {"T2", "T2"}, {"T3", "T3"}, {"T4", "T4"}, {"T5", "T5"}, {"T6", "T6"}},
                    "none")
            .showIf("zone", gasZones)
            .startsSection({"Vorhandenes Gerät prüfen (optional)", "Check existing equipment (optional)"}),
        Num::choice("devG", {"Gerät: Explosionsgruppe", "Equipment: gas group"},
                    {{"none", {"– nicht prüfen –", "– do not check –"}}, {"IIA", "IIA"}, {"IIB", "IIB"}, {"IIC", "IIC"}}, "none")
            .showIf("zone", gasZones),
        Num::number("devTmax", {"Gerät: max. Oberflächentemperatur", "Equipment: max. surface temperature"}, "°C", kNaN).opt().range(0, 1000)
            .showIf("zone", dustZones),
    };
    c.outputs = {
        Out::text("cat", {"Gerätekategorie (EPL)", "Equipment category (EPL)"}).primary().abbrev({"Kategorie", "Category"}),
        Out::text("occ", {"Auftreten explosionsfähiger Atmosphäre", "Occurrence of explosive atmosphere"}),
        Out::number("ait", {"Zündtemperatur", "Ignition temperature"}, "°C", 0),
        Out::text("tcls", {"Erforderliche Temperaturklasse", "Required temperature class"}).primary().abbrev({"T-Klasse", "T class"}),
        Out::number("tmax", {"Zulässige Oberflächentemperatur", "Permissible surface temperature"}, "°C", 0),
        Out::text("group", {"Erforderliche Gruppe", "Required group"}),
        Out::text("mark", {"Kennzeichnung (Beispiel)", "Marking (example)"}),
        Out::text("prot", {"Geeignete Zündschutzarten", "Suitable types of protection"}),
    };
    c.keywords = {"ex", "atex", "explosionsschutz", "temperaturklasse", "zone", "zündtemperatur", "explosionsgruppe", "epl",
                  "staub", "gas", "t4", "iic", "hazardous area", "temperature class"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& z = findZone(in.choice("zone"));
        r.setText("cat", Text{std::string(z.category) + " (EPL " + z.epl + ")"});
        r.setText("occ", z.occurrence);
        r.setText("prot", z.protection);
        if (!z.dust) {
            double ait = in.num("ait");
            std::string group = in.choice("grp");
            if (const auto* s = findSubstance(in.choice("sub"))) {
                ait = s->ait;
                group = s->group;
                if (!s->note.de.empty()) r.info(s->note);
            }
            const int cls = ex::temperatureClass(ait);
            if (cls == 0) {
                throw CalcError({"Zündtemperatur ≤ 85 °C: keine Temperaturklasse ausreichend – Sonderlösung nötig.",
                                 "Ignition temperature ≤ 85 °C: no temperature class sufficient – special solution needed."});
            }
            r.set("ait", ait);
            r.setText("tcls", Text{tName(cls)});
            r.set("tmax", ex::classLimit(cls));
            r.setText("group", Text{group});
            r.setText("mark", Text{std::string("II ") + z.category + " Ex " + z.typical + " " + group + " " + tName(cls) + " " + z.epl});
            const auto& devT = in.choice("devT");
            if (devT != "none") {
                const int dev = devT.back() - '0';
                if (dev >= cls) {
                    r.ok({"Gerät " + devT + " erfüllt " + tName(cls) + ".", "Equipment " + devT + " meets " + tName(cls) + "."});
                } else {
                    r.error({"Gerät " + devT + " ist zu heiß – mindestens " + tName(cls) + " erforderlich.",
                             "Equipment " + devT + " is too hot – at least " + tName(cls) + " required."});
                }
            }
            const auto& devG = in.choice("devG");
            if (devG != "none") {
                if (ex::groupCovers(devG, group)) {
                    r.ok({"Gerätegruppe " + devG + " deckt " + group + " ab.", "Equipment group " + devG + " covers " + group + "."});
                } else {
                    r.error({"Gerätegruppe " + devG + " reicht nicht – " + group + " oder höher erforderlich.",
                             "Equipment group " + devG + " is not sufficient – " + group + " or higher required."});
                }
            }
        } else {
            const double tmax = std::floor(ex::dustMaxSurface(in.num("tcl"), in.num("t5")));
            const auto& group = in.choice("dgroup");
            r.set("tmax", tmax);
            r.setText("tcls", Text{"T" + formatNumber(tmax, 0, Lang::De) + " °C", "T" + formatNumber(tmax, 0, Lang::En) + " °C"});
            r.setText("group", Text{group});
            r.setText("mark", Text{std::string("II ") + z.category + " Ex " + z.typical + " " + group + " T" +
                                   formatNumber(tmax, 0, Lang::En) + "°C " + z.epl});
            if (in.num("layer") > 5.0) {
                r.warn({"Schicht > 5 mm: zulässige Temperatur nach IEC 60079-14 weiter abmindern; ab 50 mm Laborprüfung.",
                        "Layer > 5 mm: reduce the permissible temperature per IEC 60079-14; above 50 mm laboratory test."});
            }
            if (const auto dev = in.maybe("devTmax")) {
                if (*dev <= tmax) {
                    r.ok({"Gerät mit " + formatNumber(*dev, 0, Lang::De) + " °C zulässig.", "Equipment with " + formatNumber(*dev, 0, Lang::En) + " °C permitted."});
                } else {
                    r.error({"Gerät zu heiß: max. " + formatNumber(tmax, 0, Lang::De) + " °C zulässig.",
                             "Equipment too hot: max. " + formatNumber(tmax, 0, Lang::En) + " °C permitted."});
                }
            }
        }
        return r;
    };
    return c;
}

Table make(const char* id, Text title, Text summary, Text description, std::vector<Text> columns, std::vector<std::vector<Text>> rows) {
    Table t;
    t.id = id;
    t.category = "process";
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = std::move(description);
    t.columns = std::move(columns);
    t.rows = std::move(rows);
    return t;
}

const Text kExSource{"Quellen: IEC/EN 60079-0, -14, -20-1, ATEX 2014/34/EU und 1999/92/EG, Herstellerunterlagen (Schmersal, Festo). "
                     "Gegen gültige Ausgabe und Explosionsschutzdokument prüfen.",
                     "Sources: IEC/EN 60079-0, -14, -20-1, ATEX 2014/34/EU and 1999/92/EC, manufacturer documents (Schmersal, Festo). "
                     "Check against the edition in force and the explosion protection document."};

}  // namespace

void addProcess(std::vector<Calculator>& out) { out.push_back(exSelection()); }

void addProcessTables(std::vector<Table>& out) {
    out.push_back(make("ex_temperature_classes", {"Ex-Temperaturklassen T1–T6", "Ex temperature classes T1–T6"},
                       {"Max. Oberflächentemperatur und Zündtemperatur", "Max. surface temperature and ignition temperature"}, kExSource,
                       {{"Klasse", "Class"}, {"Max. Oberflächentemperatur", "Max. surface temperature"}, {"Zündtemperatur des Stoffs", "Ignition temperature of substance"},
                        {"Beispiele", "Examples"}},
                       {{"T1", "450 °C", "> 450 °C", {"Methan, Wasserstoff, Ammoniak, Propan, Aceton", "Methane, hydrogen, ammonia, propane, acetone"}},
                        {"T2", "300 °C", "> 300 … 450 °C", {"Ethylen, Ethanol, Methanol, n-Butan, Acetylen", "Ethylene, ethanol, methanol, n-butane, acetylene"}},
                        {"T3", "200 °C", "> 200 … 300 °C", {"Diesel, Cyclohexan, Schwefelwasserstoff", "Diesel, cyclohexane, hydrogen sulphide"}},
                        {"T4", "135 °C", "> 135 … 200 °C", {"Diethylether, Acetaldehyd", "Diethyl ether, acetaldehyde"}},
                        {"T5", "100 °C", "> 100 … 135 °C", {"(selten)", "(rare)"}},
                        {"T6", "85 °C", "> 85 … 100 °C", {"Schwefelkohlenstoff", "Carbon disulphide"}}}));
    out.push_back(make("ex_groups", {"Ex-Gruppen (Gas und Staub)", "Ex groups (gas and dust)"},
                       {"I, IIA/IIB/IIC, IIIA/IIIB/IIIC mit MESG", "I, IIA/IIB/IIC, IIIA/IIIB/IIIC with MESG"}, kExSource,
                       {{"Gruppe", "Group"}, {"Bereich", "Area"}, {"MESG (Grenzspaltweite)", "MESG"}, {"MIC-Verhältnis", "MIC ratio"}, {"Beispiel", "Example"}},
                       {{"I", {"Bergbau (Grubengas)", "Mining (firedamp)"}, "–", "–", {"Methan", "Methane"}},
                        {"IIA", {"Gas – übrige Bereiche", "Gas – other areas"}, "> 0,9 mm", "> 0,8", {"Propan", "Propane"}},
                        {"IIB", {"Gas", "Gas"}, "0,5 … 0,9 mm", "0,45 … 0,8", {"Ethylen", "Ethylene"}},
                        {"IIC", {"Gas", "Gas"}, "< 0,5 mm", "< 0,45", {"Wasserstoff, Acetylen", "Hydrogen, acetylene"}},
                        {"IIIA", {"Staub", "Dust"}, "–", "–", {"brennbare Flusen", "combustible flyings"}},
                        {"IIIB", {"Staub", "Dust"}, "–", "–", {"nicht leitfähiger Staub (Holz, Mehl)", "non-conductive dust (wood, flour)"}},
                        {"IIIC", {"Staub", "Dust"}, "–", "–", {"leitfähiger Staub (Metall, Kohlenstoff)", "conductive dust (metal, carbon)"}}}));
    {
        std::vector<std::vector<Text>> rows;
        for (const auto& z : zones()) {
            rows.push_back({std::string("Zone ") + z.id, z.dust ? Text{"Staub", "Dust"} : Text{"Gas/Dampf", "Gas/vapour"}, z.occurrence,
                            Text{z.category}, Text{z.epl}, z.protection});
        }
        out.push_back(make("ex_zones", {"Ex-Zonen, Kategorien, EPL", "Ex zones, categories, EPL"},
                           {"Zone 0/1/2, 20/21/22 → Gerätekategorie und Zündschutzarten", "Zone 0/1/2, 20/21/22 → category and types of protection"},
                           kExSource,
                           {{"Zone", "Zone"}, {"Atmosphäre", "Atmosphere"}, {"Auftreten", "Occurrence"}, {"Kategorie", "Category"}, {"EPL", "EPL"},
                            {"Zündschutzarten", "Types of protection"}},
                           std::move(rows)));
    }
    out.push_back(make("ex_protection", {"Zündschutzarten", "Types of protection"}, {"d, e, i, p, m, o, q, n, t, op", "d, e, i, p, m, o, q, n, t, op"},
                       kExSource,
                       {{"Kürzel", "Code"}, {"Zündschutzart", "Type of protection"}, {"Prinzip", "Principle"}, {"Schutzniveau → Zone", "Level → zone"}, {"Norm", "Standard"}},
                       {{"d", {"Druckfeste Kapselung", "Flameproof enclosure"}, {"Explosion bleibt im Gehäuse", "Explosion contained in enclosure"}, "db → 1, dc → 2", "IEC 60079-1"},
                        {"e", {"Erhöhte Sicherheit", "Increased safety"}, {"keine Funken/heißen Stellen", "no sparks/hot spots"}, "eb → 1, ec → 2", "IEC 60079-7"},
                        {"i", {"Eigensicherheit", "Intrinsic safety"}, {"Energie begrenzt", "energy limited"}, "ia → 0, ib → 1, ic → 2", "IEC 60079-11"},
                        {"p", {"Überdruckkapselung", "Pressurized enclosure"}, {"Schutzgas mit Überdruck", "purge gas overpressure"}, "pxb/pyb → 1, pzc → 2", "IEC 60079-2"},
                        {"m", {"Vergusskapselung", "Encapsulation"}, {"in Masse eingegossen", "embedded in compound"}, "ma → 0, mb → 1, mc → 2", "IEC 60079-18"},
                        {"o", {"Flüssigkeitskapselung", "Liquid immersion"}, {"unter Schutzflüssigkeit", "under protective liquid"}, "ob → 1, oc → 2", "IEC 60079-6"},
                        {"q", {"Sandkapselung", "Powder filling"}, {"mit Sand gefüllt", "filled with sand"}, "qb → 1", "IEC 60079-5"},
                        {"nR", {"Schwadenhemmendes Gehäuse", "Restricted breathing"}, {"dichtes Gehäuse", "tight enclosure"}, "nR → 2", "IEC 60079-15"},
                        {"t", {"Schutz durch Gehäuse (Staub)", "Protection by enclosure (dust)"}, {"staubdicht, Temperatur begrenzt", "dust-tight, limited temperature"},
                         "ta → 20, tb → 21, tc → 22", "IEC 60079-31"},
                        {"op", {"Optische Strahlung", "Optical radiation"}, {"is/pr/sh", "is/pr/sh"}, "Ga/Gb/Gc", "IEC 60079-28"}}));
    {
        std::vector<std::vector<Text>> rows;
        for (const auto& s : substances()) {
            const int cls = ex::temperatureClass(s.ait);
            rows.push_back({s.name, Text{(std::string(s.id) == "diesel" ? "≥ " : "") + formatNumber(s.ait, 0, Lang::De) + " °C",
                                         (std::string(s.id) == "diesel" ? "≥ " : "") + formatNumber(s.ait, 0, Lang::En) + " °C"},
                            Text{tName(cls)}, Text{s.group}});
        }
        out.push_back(make("ex_substances", {"Zündtemperaturen von Gasen und Dämpfen", "Ignition temperatures of gases and vapours"},
                           {"Richtwerte mit Temperaturklasse und Explosionsgruppe", "Guide values with temperature class and group"},
                           Text{"Richtwerte (Schmersal-Ex-Poster, Festo-Handout Explosionsschutz, GESTIS); je nach Quelle abweichend. Maßgeblich "
                                "sind Sicherheitsdatenblatt und IEC 60079-20-1.",
                                "Guide values (Schmersal Ex poster, Festo explosion protection handout, GESTIS); they differ between sources. "
                                "The safety data sheet and IEC 60079-20-1 are decisive."},
                           {{"Stoff", "Substance"}, {"Zündtemperatur", "Ignition temperature"}, {"Temperaturklasse", "Temperature class"},
                            {"Explosionsgruppe", "Gas group"}},
                           std::move(rows)));
    }
}

}  // namespace elc::catalog
