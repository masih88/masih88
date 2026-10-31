// Project list: BACS I/O data point list (GA-Funktionsliste, Ein-/Ausgabefunktionen).
// I/O types after VDI 3814 Blatt 3.1 (AI, AO, BI, BO + communicative), user addresses (BAS)
// and identifiers after VDI 3814 Blatt 4.1 (Tab. 1 trades, Tab. 2 plants, Tab. 3 devices,
// Tab. 4 function identifiers). Component templates reflect common planning practice and
// can be adapted per project.
#include <array>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/lists.hpp"

namespace elc::catalog {

namespace {

enum class Io { AI, AO, BI, BO, COM };

const char* ioName(Io t) {
    switch (t) {
        case Io::AI: return "AI";
        case Io::AO: return "AO";
        case Io::BI: return "BI";
        case Io::BO: return "BO";
        case Io::COM: return "COM";
    }
    return "";
}

struct Point {
    const char* code;  // VDI 3814-4.1 Tab. 4 function identifier
    Io type;
    const char* hintDe;
    const char* hintEn;
};

struct Template {
    const char* id;
    Text label;
    const char* comp;  // default device code (VDI 3814-4.1 Tab. 3)
    Text compName;
    std::vector<Point> points;
};

// Function identifiers (VDI 3814 Blatt 4.1 Tabelle 4)
const std::map<std::string, Text>& functionNames() {
    static const std::map<std::string, Text> m{
        {"SB_", {"Schaltbefehl", "Switching command"}}, {"BM_", {"Betriebsmeldung", "Operating message"}},
        {"SM_", {"Störung", "Fault"}},                   {"ST_", {"Stellsignal", "Control signal"}},
        {"RW_", {"Rückführwert", "Feedback value"}},     {"RMA", {"Rückmeldung AUF", "Feedback OPEN"}},
        {"RMZ", {"Rückmeldung ZU", "Feedback CLOSED"}},  {"MW_", {"Messwert", "Measured value"}},
        {"WM_", {"Wartungsmeldung", "Maintenance message"}}, {"GM_", {"Gefahrmeldung", "Danger alarm"}},
        {"FR_", {"Freigabe", "Enable"}},                 {"SW_", {"Sollwert", "Setpoint"}},
        {"ZW_", {"Zählwert", "Count value"}},            {"AS_", {"Anlagenschalter", "Plant switch"}},
        {"HD_", {"LVB-Handmeldung", "Local override message"}}, {"SWV", {"Sollwertversteller", "Setpoint adjuster"}},
        {"BEL", {"Belegtzustand Raum", "Room occupancy"}}, {"STA", {"Status", "Status"}},
    };
    return m;
}

constexpr const char* kNc = "Öffnerkontakt, drahtbruchsicher nach VDI 3814-3.1";
constexpr const char* kNcEn = "NC contact, wire-break safe per VDI 3814-3.1";

const std::vector<Template>& templates() {
    static const std::vector<Template> t{
        {"fan_fu", {"Ventilator mit FU", "Fan with VFD"}, "ZUV", {"Zuluftventilator", "Supply fan"},
         {{"SB_", Io::BO, "Freigabe/Ein", "Enable/on"}, {"BM_", Io::BI, "", ""}, {"SM_", Io::BI, kNc, kNcEn},
          {"ST_", Io::AO, "Drehzahl 0–10 V", "Speed 0–10 V"}, {"WM_", Io::BI, "Reparaturschalter", "Repair switch"}}},
        {"fan_dol", {"Ventilator Direktanlauf", "Fan direct-on-line"}, "ABV", {"Abluftventilator", "Exhaust fan"},
         {{"SB_", Io::BO, "", ""}, {"BM_", Io::BI, "", ""}, {"SM_", Io::BI, kNc, kNcEn}, {"WM_", Io::BI, "Reparaturschalter", "Repair switch"}}},
        {"fan_bus", {"Ventilator EC/FU über Bus", "Fan EC/VFD via bus"}, "ZUV", {"Zuluftventilator", "Supply fan"},
         {{"SB_", Io::BO, "Freigabe hart verdrahtet", "Hard-wired enable"}, {"BM_", Io::COM, "Modbus/BACnet", "Modbus/BACnet"},
          {"SM_", Io::COM, "", ""}, {"ST_", Io::COM, "Drehzahl", "Speed"}, {"MW_", Io::COM, "Leistung/Strom", "Power/current"}}},
        {"pump", {"Pumpe (Ein/Aus)", "Pump (on/off)"}, "PPE", {"Pumpe", "Pump"},
         {{"SB_", Io::BO, "", ""}, {"BM_", Io::BI, "", ""}, {"SM_", Io::BI, kNc, kNcEn}}},
        {"pump_he", {"Hocheffizienzpumpe 0–10 V", "High-efficiency pump 0–10 V"}, "PPE", {"Pumpe", "Pump"},
         {{"SB_", Io::BO, "", ""}, {"SM_", Io::BI, kNc, kNcEn}, {"ST_", Io::AO, "Sollwert 0–10 V", "Setpoint 0–10 V"}}},
        {"valve_mod", {"Ventil stetig mit Rückmeldung", "Modulating valve with feedback"}, "VEN", {"Ventil", "Valve"},
         {{"ST_", Io::AO, "0(2)–10 V", "0(2)–10 V"}, {"RW_", Io::AI, "Stellungsrückmeldung", "Position feedback"}}},
        {"valve_mod_nf", {"Ventil stetig ohne Rückmeldung", "Modulating valve w/o feedback"}, "VEN", {"Ventil", "Valve"},
         {{"ST_", Io::AO, "0(2)–10 V", "0(2)–10 V"}}},
        {"valve_oc", {"Ventil/Klappe Auf/Zu mit Endlagen", "Valve/damper open/close with limit switches"}, "KLA", {"Klappe", "Damper"},
         {{"SB_", Io::BO, "AUF", "OPEN"}, {"RMA", Io::BI, "", ""}, {"RMZ", Io::BI, "", ""}}},
        {"damper_mod", {"Klappe stetig mit Rückmeldung", "Modulating damper with feedback"}, "KLA", {"Klappe", "Damper"},
         {{"ST_", Io::AO, "0–10 V", "0–10 V"}, {"RW_", Io::AI, "", ""}}},
        {"fire_damper", {"Brandschutzklappe mit Endlagen", "Fire damper with limit switches"}, "BSK", {"Brandschutzklappe", "Fire damper"},
         {{"SB_", Io::BO, "Ansteuerung 24/230 V", "Drive 24/230 V"}, {"RMA", Io::BI, "", ""}, {"RMZ", Io::BI, "", ""}}},
        {"temp", {"Temperaturfühler", "Temperature sensor"}, "TZU", {"Temperatur", "Temperature"},
         {{"MW_", Io::AI, "Pt1000/Ni1000/NTC", "Pt1000/Ni1000/NTC"}}},
        {"hum", {"Feuchtefühler (rel. F. + Temperatur)", "Humidity sensor (RH + temperature)"}, "MZU", {"Feuchte", "Humidity"},
         {{"MW_", Io::AI, "0–10 V / 4–20 mA", "0–10 V / 4–20 mA"}, {"MW_", Io::AI, "Temperatur", "Temperature"}}},
        {"press", {"Druck-/Differenzdrucksensor", "Pressure/differential pressure sensor"}, "PDI", {"Differenzdruck", "Differential pressure"},
         {{"MW_", Io::AI, "0–10 V / 4–20 mA", "0–10 V / 4–20 mA"}}},
        {"co2", {"CO₂-/Luftqualitätsfühler", "CO₂/air quality sensor"}, "COM", {"CO₂", "CO₂"}, {{"MW_", Io::AI, "0–10 V", "0–10 V"}}},
        {"flow", {"Volumenstrommessung", "Airflow measurement"}, "FZU", {"Volumenstrom", "Airflow"}, {{"MW_", Io::AI, "", ""}}},
        {"frost", {"Frostschutzwächter", "Frost protection thermostat"}, "FSW", {"Frostschutzwächter", "Frost thermostat"},
         {{"GM_", Io::BI, kNc, kNcEn}}},
        {"filter", {"Filterüberwachung (Δp-Wächter)", "Filter monitoring (Δp switch)"}, "LFI", {"Luftfilter", "Air filter"},
         {{"WM_", Io::BI, "", ""}}},
        {"filter_ai", {"Filterüberwachung stetig", "Filter monitoring analog"}, "LFI", {"Luftfilter", "Air filter"},
         {{"MW_", Io::AI, "Δp 0–10 V", "Δp 0–10 V"}}},
        {"flow_switch", {"Strömungswächter", "Flow switch"}, "SSW", {"Strömungswächter", "Flow switch"}, {{"BM_", Io::BI, "", ""}}},
        {"stb", {"Sicherheitstemperaturbegrenzer", "Safety temperature limiter"}, "STB", {"STB", "STB"},
         {{"GM_", Io::BI, kNc, kNcEn}}},
        {"plant_switch", {"Anlagenschalter Hand-0-Auto", "Plant switch hand-off-auto"}, "ANL", {"Anlagenschalter", "Plant switch"},
         {{"AS_", Io::BI, "Hand", "Hand"}, {"AS_", Io::BI, "Auto", "Auto"}}},
        {"chiller", {"Kältemaschine (Freigabe + Bus)", "Chiller (enable + bus)"}, "KMA", {"Kältemaschine", "Chiller"},
         {{"FR_", Io::BO, "", ""}, {"BM_", Io::BI, "", ""}, {"SM_", Io::BI, kNc, kNcEn}, {"SW_", Io::AO, "Sollwert", "Setpoint"},
          {"MW_", Io::COM, "Betriebsdaten über Bus", "Operating data via bus"}}},
        {"boiler", {"Wärmeerzeuger (Freigabe + Sollwert)", "Heat generator (enable + setpoint)"}, "HZK", {"Heizkessel", "Boiler"},
         {{"FR_", Io::BO, "", ""}, {"BM_", Io::BI, "", ""}, {"SM_", Io::BI, kNc, kNcEn}, {"SW_", Io::AO, "0–10 V", "0–10 V"}}},
        {"vav", {"Volumenstromregler über Bus", "VAV controller via bus"}, "VVR", {"Volumenstromregler", "VAV box"},
         {{"SW_", Io::COM, "", ""}, {"MW_", Io::COM, "Ist-Volumenstrom", "Actual airflow"}, {"RW_", Io::COM, "Klappenstellung", "Damper position"}}},
        {"heat_meter", {"Wärme-/Kältemengenzähler (M-Bus)", "Heat/cooling meter (M-Bus)"}, "WMZ", {"Wärmemengenzähler", "Heat meter"},
         {{"ZW_", Io::COM, "Energie", "Energy"}, {"MW_", Io::COM, "Leistung", "Power"}, {"MW_", Io::COM, "Volumenstrom", "Flow"}}},
        {"el_meter", {"Elektrozähler (Modbus)", "Electricity meter (Modbus)"}, "ELZ", {"Elektrozähler", "Electricity meter"},
         {{"ZW_", Io::COM, "Wirkenergie", "Active energy"}, {"MW_", Io::COM, "Wirkleistung", "Active power"}}},
        {"pulse_meter", {"Zähler mit Impulsausgang", "Meter with pulse output"}, "WAZ", {"Zähler", "Meter"},
         {{"ZW_", Io::BI, "Impulseingang", "Pulse input"}}},
        {"room_unit", {"Raumbediengerät (Bus)", "Room control unit (bus)"}, "RBG", {"Raumbediengerät", "Room unit"},
         {{"MW_", Io::COM, "Raumtemperatur", "Room temperature"}, {"SWV", Io::COM, "", ""}, {"BEL", Io::COM, "Präsenz", "Presence"}}},
        {"alarm_in", {"Störmeldung extern (Sammel)", "External fault (summary)"}, "SSK", {"Schaltschrank", "Panel"},
         {{"SM_", Io::BI, kNc, kNcEn}}},
    };
    return t;
}

const Template& findTemplate(const std::string& id) {
    for (const auto& t : templates()) {
        if (id == t.id) return t;
    }
    return templates().front();
}

struct PlantCode {
    const char* code;
    char trade;  // VDI 3814-4.1 Tab. 1 "BAS kurz"
    Text name;
};

// Plant abbreviations (VDI 3814 Blatt 4.1 Tabelle 2, Auszug)
const std::vector<PlantCode>& plantCodes() {
    static const std::vector<PlantCode> p{
        {"VKA", 'L', {"Vollklimaanlage", "Full air conditioning"}}, {"TKA", 'L', {"Teilklimaanlage", "Partial air conditioning"}},
        {"ZUA", 'L', {"Zuluftanlage", "Supply air system"}},       {"ABA", 'L', {"Abluftanlage", "Exhaust air system"}},
        {"ULA", 'L', {"Umluftanlage", "Recirculation system"}},    {"RLT", 'L', {"Raumlufttechnik allgemein", "Ventilation general"}},
        {"MRA", 'L', {"Maschinelle Rauchabzugsanlage", "Mechanical smoke extraction"}}, {"WRG", 'L', {"Wärmerückgewinnung", "Heat recovery"}},
        {"HZK", 'H', {"Heizkreis", "Heating circuit"}},            {"KES", 'H', {"Kessel", "Boiler"}},
        {"FWÜ", 'H', {"Fernwärmeübergabe", "District heating transfer"}}, {"WPU", 'H', {"Wärmepumpe", "Heat pump"}},
        {"WTR", 'H', {"Wärmeübertrager", "Heat exchanger"}},       {"PFS", 'H', {"Pufferspeicher", "Buffer tank"}},
        {"FBH", 'H', {"Fußbodenheizung", "Underfloor heating"}},   {"BKA", 'H', {"Betonkernaktivierung", "Concrete core activation"}},
        {"KWE", 'K', {"Kaltwassererzeugung", "Chilled water generation"}}, {"KKR", 'K', {"Kältekreis", "Cooling circuit"}},
        {"KVN", 'K', {"Kälteverteilnetz", "Cooling distribution"}}, {"RKW", 'K', {"Rückkühlwerk", "Re-cooling plant"}},
        {"KÜD", 'K', {"Kühldecke", "Chilled ceiling"}},            {"KSP", 'K', {"Kältespeicher", "Cold storage"}},
        {"TWE", 'W', {"Trinkwassererwärmung", "Domestic hot water"}}, {"DEA", 'W', {"Druckerhöhungsanlage", "Booster system"}},
        {"HBA", 'W', {"Hebeanlage", "Lifting plant"}},             {"ASV", 'E', {"Allgemeine Stromversorgung", "General power supply"}},
        {"SSV", 'E', {"Sicherheitsstromversorgung", "Safety power supply"}}, {"USV", 'E', {"USV", "UPS"}},
        {"PVA", 'E', {"Fotovoltaikanlage", "PV system"}},          {"BEL", 'E', {"Beleuchtung", "Lighting"}},
        {"SON", 'Z', {"Sonnenschutz", "Sun protection"}},          {"RWA", 'Z', {"Rauch- und Wärmeabzug", "Smoke and heat vent"}},
    };
    return p;
}

const PlantCode& findPlant(const std::string& code) {
    for (const auto& p : plantCodes()) {
        if (code == p.code) return p;
    }
    return plantCodes().front();
}

std::string two(double n) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "%02d", static_cast<int>(std::lround(n)));
    return buf;
}

std::string compCode(const Inputs& in, const Template& t) {
    const auto& c = in.text("comp");
    return c.empty() ? std::string(t.comp) : c;
}

/// Counts per I/O type of one row (including quantity)
std::array<double, 5> counts(const Inputs& in) {
    std::array<double, 5> c{};
    const auto& t = findTemplate(in.choice("tpl"));
    const double q = std::round(in.num("qty"));
    for (const auto& p : t.points) c[static_cast<std::size_t>(p.type)] += q;
    return c;
}

std::string basPrefix(const Inputs& cfg, const Inputs& in) {
    const auto& sep = cfg.text("sep");
    const auto& plant = findPlant(in.choice("plant"));
    std::string s;
    for (const auto* id : {"site", "bldg", "isp"}) {
        if (!cfg.text(id).empty()) s += cfg.text(id) + sep;
    }
    return s + std::string(1, plant.trade) + sep + plant.code + two(in.num("plantNo")) + sep;
}

std::string functionBlock(const std::string& code, int number) {
    return code + two(number);  // "SB_" → "SB_01", "RMA" → "RMA01" (VDI 3814-4.1 5.3.8/5.3.9)
}

Result computePoints(const Inputs& in, const Inputs& cfg, const Context&) {
    Result r;
    const auto c = counts(in);
    r.set("AI", c[0]);
    r.set("AO", c[1]);
    r.set("BI", c[2]);
    r.set("BO", c[3]);
    r.set("COM", c[4]);
    r.set("phys", c[0] + c[1] + c[2] + c[3]);
    const auto& t = findTemplate(in.choice("tpl"));
    const auto first = basPrefix(cfg, in) + compCode(in, t) + two(in.num("compNo")) + cfg.text("sep") +
                       functionBlock(t.points.front().code, 1);
    r.setText("example", Text{first});
    std::string list;
    for (const auto& p : t.points) list += (list.empty() ? "" : ", ") + std::string(p.code) + " " + ioName(p.type);
    r.setText("points", Text{list});
    return r;
}

Result summarizePoints(const std::vector<Inputs>& rows, const std::vector<Result>&, const Inputs& cfg, const Context& ctx) {
    Result t;
    std::array<double, 5> sum{};
    for (const auto& in : rows) {
        const auto c = counts(in);
        for (std::size_t i = 0; i < 5; ++i) sum[i] += c[i];
    }
    const double reserve = 1.0 + cfg.num("reserve") / 100.0;
    const double phys = sum[0] + sum[1] + sum[2] + sum[3];
    t.set("AI", sum[0]);
    t.set("AO", sum[1]);
    t.set("BI", sum[2]);
    t.set("BO", sum[3]);
    t.set("COM", sum[4]);
    t.set("phys", phys);
    t.set("physRes", std::ceil(phys * reserve - 1e-9));
    const std::array<const char*, 4> chId{"chAI", "chAO", "chBI", "chBO"};
    const std::array<const char*, 4> modId{"modAI", "modAO", "modBI", "modBO"};
    for (std::size_t i = 0; i < 4; ++i) {
        t.set(modId[i], std::ceil(sum[i] * reserve / cfg.num(chId[i]) - 1e-9));
    }
    const std::string cur = ctx.norm == NormProfile::CH ? "CHF" : "€";
    if (cfg.num("price") > 0 || cfg.num("priceCom") > 0) {
        t.set("cost", phys * cfg.num("price") + sum[4] * cfg.num("priceCom"), cur);
    }
    t.info({"Datenpunkte = Ein-/Ausgabefunktionen nach VDI 3814 Blatt 3.1; Module inkl. Reserve.",
            "Data points = input/output functions per VDI 3814 Part 3.1; modules incl. reserve."});
    return t;
}

std::vector<std::vector<std::string>> expandPoints(const std::vector<Inputs>& rows, const Inputs& cfg, Lang lang) {
    std::vector<std::vector<std::string>> out;
    int nr = 0;
    for (const auto& in : rows) {
        const auto& t = findTemplate(in.choice("tpl"));
        const auto& plant = findPlant(in.choice("plant"));
        const int qty = static_cast<int>(std::lround(in.num("qty")));
        const int start = static_cast<int>(std::lround(in.num("compNo")));
        const auto& name = in.text("name").empty() ? t.compName.in(lang) : in.text("name");
        for (int k = 0; k < qty; ++k) {
            const int compNo = start + k;
            const auto comp = compCode(in, t) + two(compNo);
            std::map<std::string, int> fnCounter;
            for (const auto& p : t.points) {
                const int fn = ++fnCounter[p.code];
                const auto bas = basPrefix(cfg, in) + comp + cfg.text("sep") + functionBlock(p.code, fn);
                const auto it = functionNames().find(p.code);
                const std::string fname = it == functionNames().end() ? p.code : it->second.in(lang);
                const std::string hint = lang == Lang::De ? p.hintDe : p.hintEn;
                out.push_back({std::to_string(++nr), bas, name + " " + two(compNo) + " – " + fname + (hint.empty() ? "" : " (" + hint + ")"),
                               ioName(p.type), fname, std::string(plant.code) + two(in.num("plantNo")), std::string(1, plant.trade), comp});
            }
        }
    }
    return out;
}

std::vector<Option> templateOptions() {
    std::vector<Option> o;
    for (const auto& t : templates()) o.push_back({t.id, t.label});
    return o;
}

std::vector<Option> plantOptions() {
    std::vector<Option> o;
    for (const auto& p : plantCodes()) {
        o.push_back({p.code, {std::string(p.code) + " – " + p.name.de, std::string(p.code) + " – " + p.name.en}});
    }
    return o;
}

}  // namespace

ListDef dataPointList() {
    ListDef l;
    l.id = "ga_points";
    l.category = "projects";
    l.title = {"GA-Datenpunktliste (VDI 3814)", "BACS data point list (VDI 3814)"};
    l.summary = {"AI/AO/BI/BO/COM zählen, Module, BAS-Adressen nach VDI 3814-4.1",
                 "Count AI/AO/BI/BO/COM, modules, BAS addresses per VDI 3814-4.1"};
    l.description = {
        "GA-Funktionsliste – Ein-/Ausgabefunktionen (VDI 3814 Blatt 3.1: Analoge/Binäre Eingabe AI/BI, Analoge/Binäre Ausgabe "
        "AO/BO, kommunikative Datenpunkte COM) aus Bauteil-Vorlagen.\n"
        "Benutzeradresse (BAS) nach VDI 3814 Blatt 4.1: Liegenschaft_Gebäude_ISP_Gewerk_Anlage+Nr_Bauteil+Nr_Funktion+Nr, "
        "z. B. K_HG_01_L_VKA01_ZUV01_SB_01. Gewerk (Tab. 1), Anlage (Tab. 2), Bauteil (Tab. 3), Funktionskennung (Tab. 4).\n"
        "Module = ⌈Anzahl · (1 + Reserve) / Kanäle je Modul⌉. Störungs-/Gefahrmeldungen als Öffnerkontakt (VDI 3814-3.1).\n"
        "Die Vorlagen sind Planungspraxis und projektspezifisch anzupassen (Bauteil-Kürzel überschreibbar).",
        "BACS function list – input/output functions (VDI 3814 Part 3.1: analog/binary input AI/BI, analog/binary output AO/BO, "
        "communicative data points COM) from component templates.\n"
        "User address (BAS) per VDI 3814 Part 4.1: site_building_ISP_trade_plant+no_component+no_function+no, "
        "e.g. K_HG_01_L_VKA01_ZUV01_SB_01. Trade (Tab. 1), plant (Tab. 2), component (Tab. 3), function identifier (Tab. 4).\n"
        "Modules = ⌈count · (1 + reserve) / channels per module⌉. Fault/danger messages as NC contacts (VDI 3814-3.1).\n"
        "Templates reflect planning practice and must be adapted per project (component code can be overridden)."};
    l.settingsInputs = {
        Num::text("site", {"Liegenschaft", "Site"}, "K").startsSection({"Benutzeradresse (BAS)", "User address (BAS)"}),
        Num::text("bldg", {"Gebäude/Bauteil", "Building"}, "HG"),
        Num::text("isp", {"Informationsschwerpunkt (ISP)", "Automation focal point (ISP)"}, "01"),
        Num::text("sep", {"Trennzeichen", "Separator"}, "_"),
        Num::number("reserve", {"Reserve", "Spare"}, "%", 20).range(0, 200).startsSection({"Hardware & Kosten", "Hardware & cost"}),
        Num::number("chAI", {"Kanäle je AI-Modul", "Channels per AI module"}, "", 8).range(1, 64),
        Num::number("chAO", {"Kanäle je AO-Modul", "Channels per AO module"}, "", 8).range(1, 64),
        Num::number("chBI", {"Kanäle je BI-Modul", "Channels per BI module"}, "", 16).range(1, 64),
        Num::number("chBO", {"Kanäle je BO-Modul", "Channels per BO module"}, "", 8).range(1, 64),
        Num::number("price", {"Preis je physikalischem Datenpunkt", "Price per physical data point"}, "€ / CHF", 0).nonNegative()
            .withHint({"0 = keine Kostenschätzung", "0 = no cost estimate"}),
        Num::number("priceCom", {"Preis je kommunikativem Datenpunkt", "Price per communicative data point"}, "€ / CHF", 0).nonNegative(),
    };
    l.rowInputs = {
        Num::text("name", {"Klartext Bauteil", "Component plain text"}, "Zuluftventilator").alias({"Klartext", "Bezeichnung"}),
        Num::choice("plant", {"Anlage (VDI 3814-4.1 Tab. 2)", "Plant (VDI 3814-4.1 Tab. 2)"}, plantOptions(), "VKA").alias({"Anlage"}),
        Num::number("plantNo", {"Anlagennummer", "Plant number"}, "", 1).range(0, 99),
        Num::choice("tpl", {"Bauteil-Vorlage", "Component template"}, templateOptions(), "fan_fu").alias({"Vorlage"}),
        Num::text("comp", {"Bauteil-Kürzel (leer = Vorlage)", "Component code (empty = template)"}).alias({"Bauteil"}),
        Num::number("compNo", {"Bauteilnummer (Start)", "Component number (start)"}, "", 1).range(0, 99),
        Num::number("qty", {"Anzahl", "Quantity"}, "", 1).range(1, 99).alias({"Anzahl"}),
    };
    l.rowOutputs = {Out::number("AI", {"AI", "AI"}, "", 0), Out::number("AO", {"AO", "AO"}, "", 0),
                    Out::number("BI", {"BI", "BI"}, "", 0), Out::number("BO", {"BO", "BO"}, "", 0),
                    Out::number("COM", {"COM (Bus)", "COM (bus)"}, "", 0),
                    Out::number("phys", {"Physikalische Datenpunkte", "Physical data points"}, "", 0).primary().abbrev({"Phys. DP", "Phys. DP"}),
                    Out::text("example", {"Erste Benutzeradresse", "First user address"}).primary().abbrev({"BAS", "BAS"}),
                    Out::text("points", {"Funktionen", "Functions"})};
    l.totalOutputs = {Out::number("phys", {"Physikalische Datenpunkte", "Physical data points"}, "", 0).primary(),
                      Out::number("COM", {"Kommunikative Datenpunkte", "Communicative data points"}, "", 0).primary(),
                      Out::number("AI", {"Analoge Eingabe AI", "Analog input AI"}, "", 0),
                      Out::number("AO", {"Analoge Ausgabe AO", "Analog output AO"}, "", 0),
                      Out::number("BI", {"Binäre Eingabe BI", "Binary input BI"}, "", 0),
                      Out::number("BO", {"Binäre Ausgabe BO", "Binary output BO"}, "", 0),
                      Out::number("physRes", {"Physikalisch inkl. Reserve", "Physical incl. spare"}, "", 0),
                      Out::number("modAI", {"AI-Module", "AI modules"}, "", 0),
                      Out::number("modAO", {"AO-Module", "AO modules"}, "", 0),
                      Out::number("modBI", {"BI-Module", "BI modules"}, "", 0),
                      Out::number("modBO", {"BO-Module", "BO modules"}, "", 0),
                      Out::number("cost", {"Kostenschätzung Datenpunkte", "Data point cost estimate"}, "€", 0)};
    l.computeRow = computePoints;
    l.summarize = summarizePoints;
    l.expandTitle = {"Datenpunktliste (BAS)", "Data point list (BAS)"};
    l.expandColumns = {{"Nr.", "No."}, {"Benutzeradresse (BAS)", "User address (BAS)"}, {"Klartext", "Plain text"},
                       {"Typ", "Type"}, {"Funktion", "Function"}, {"Anlage", "Plant"}, {"Gewerk", "Trade"}, {"Bauteil", "Component"}};
    l.expand = expandPoints;
    l.keywords = {"vdi 3814", "datenpunkt", "funktionsliste", "bas", "io-liste", "e/a", "ga", "adressierung", "kennzeichnung"};
    l.example =
        "name\tplant\tplantNo\ttpl\tcomp\tcompNo\tqty\n"
        "Anlagenschalter\tVKA\t1\tplant_switch\t\t1\t1\n"
        "Außenluftklappe\tVKA\t1\tvalve_oc\tKLA\t1\t1\n"
        "Filter Außenluft/Abluft\tVKA\t1\tfilter\tLFI\t1\t2\n"
        "Frostschutzwächter\tVKA\t1\tfrost\t\t1\t1\n"
        "Ventil Lufterhitzer\tVKA\t1\tvalve_mod\t\t1\t1\n"
        "Pumpe Lufterhitzer\tVKA\t1\tpump\t\t1\t1\n"
        "Zuluftventilator\tVKA\t1\tfan_fu\tZUV\t1\t1\n"
        "Abluftventilator\tVKA\t1\tfan_fu\tABV\t1\t1\n"
        "Außentemperatur\tVKA\t1\ttemp\tTAU\t1\t1\n"
        "Zulufttemperatur\tVKA\t1\ttemp\tTZU\t1\t1\n"
        "Ablufttemperatur\tVKA\t1\ttemp\tTAB\t1\t1\n"
        "Brandschutzklappen\tVKA\t1\tfire_damper\t\t1\t4\n"
        "Heizkreis Pumpe\tHZK\t1\tpump_he\t\t1\t1\n"
        "Heizkreis Ventil\tHZK\t1\tvalve_mod_nf\t\t1\t1\n"
        "Vorlauftemperatur\tHZK\t1\ttemp\tTVL\t1\t1\n"
        "Wärmemengenzähler\tHZK\t1\theat_meter\t\t1\t1\n";
    return l;
}

}  // namespace elc::catalog
