// VDI 3814 Blatt 4.1:2019-01 – identifier code lists (extracts) for the user addressing system (BAS).
// Codes are project-specific adaptable; the tables here follow the guideline's examples.
#include <string>
#include <vector>

#include "catalog/helpers.hpp"

namespace elc::catalog {

namespace {

const Text kSourceNote{"Auszug aus VDI 3814 Blatt 4.1 (2019-01). Kürzel können projektspezifisch ergänzt und angepasst werden.",
                       "Extract from VDI 3814 Part 4.1 (2019-01). Codes can be supplemented and adapted per project."};

Table trades() {
    Table t;
    t.id = "vdi3814_trades";
    t.category = "automation";
    t.title = {"VDI 3814: Gewerkekennzeichnung", "VDI 3814: trade identifiers"};
    t.summary = {"BAS kurz/lang nach DIN 276-1 (Tabelle 1)", "BAS short/long per DIN 276-1 (Table 1)"};
    t.description = kSourceNote;
    t.columns = {{"Kostengruppe/Gewerk", "Cost group/trade"}, "DIN 276-1", {"BAS kurz", "BAS short"}, {"BAS lang", "BAS long"}, "IEC 81346"};
    t.rows = {
        {{"Türen, Tore, Fenster, Sonnenschutz", "Doors, gates, windows, sun protection"}, "330", "Z", "TTF", "BF/BD"},
        {{"Abwasser-, Wasser-, Gasanlagen", "Sewage, water, gas"}, "410", "W", "GWA", "TS/TT/TG"},
        {{"Wärmeversorgungsanlagen", "Heat supply"}, "420", "H", "HZG", "TH"},
        {{"Lufttechnische Anlagen", "Ventilation"}, "430", "L", "RLT", "TL"},
        {{"Kälteanlagen", "Refrigeration"}, "434", "K", "KAE", "TK"},
        {{"Starkstromanlagen", "Power installations"}, "440", "E", "ELT", "TE"},
        {{"Fernmelde- und IT-Anlagen", "Telecommunication and IT"}, "450", "N", "FIA", "TF/TD"},
        {{"Gefahrenmelde- und Alarmanlagen", "Hazard detection and alarm"}, "456", "G", "GMA", "TF"},
        {{"Förderanlagen", "Conveyors"}, "460", "F", "FTA", "TJ"},
        {{"Küchentechnische Anlagen", "Kitchen equipment"}, "471", "C", "KTA", "TQ"},
        {{"Wäscherei- und Reinigungsanlagen", "Laundry and cleaning"}, "472", "D", "WRA", "TN"},
        {{"Medienversorgungsanlagen", "Media supply"}, "473", "O", "MVA", "TM"},
        {{"Medizin- und labortechnische Anlagen", "Medical and laboratory"}, "474", "M", "MED", "TN"},
        {{"Feuerlöschanlagen", "Fire extinguishing"}, "475", "S", "FLA", "TP"},
        {{"Badetechnische Anlagen", "Bathing facilities"}, "476", "B", "BTA", "TN"},
        {{"Prozesswärme-, -kälte- und -luftanlagen", "Process heat, cooling, air"}, "477", "P", "PRO", "TN"},
        {{"Entsorgungsanlagen", "Waste disposal"}, "478", "X", "ENT", "TU"},
        {{"Gebäudeautomation allgemein", "BACS general"}, "480", "A", "GAS", "TC"},
        {{"Automations-/Informationsschwerpunkt", "Automation/information focal point"}, "482", "I", "ASP/ISP", "TC"},
        {{"Raumautomation", "Room automation"}, "484", "R", "RAU", "TC"},
        {{"Technische Anlagen in Außenanlagen", "Outdoor technical equipment"}, "540", "Y", "TAA", "BG"},
    };
    return t;
}

Table plants() {
    Table t;
    t.id = "vdi3814_plants";
    t.category = "automation";
    t.title = {"VDI 3814: Anlagenkürzel", "VDI 3814: plant codes"};
    t.summary = {"Anlagenkurzbezeichnung nach DIN 276-1 (Tabelle 2, Auszug)", "Plant abbreviations per DIN 276-1 (Table 2, extract)"};
    t.description = kSourceNote;
    t.columns = {{"KG", "CG"}, {"Anlage", "Plant"}, {"Kürzel", "Code"}};
    struct R {
        const char* kg;
        const char* de;
        const char* en;
        const char* code;
    };
    static const R rows[] = {
        {"300", "Fassade", "Façade", "FAS"}, {"300", "Natürlicher Rauchabzug", "Natural smoke vent", "NRA"},
        {"300", "Rauch- und Wärmeabzugsanlage", "Smoke and heat exhaust", "RWA"}, {"300", "Sonnenschutzanlage", "Sun protection", "SON"},
        {"411", "Hebeanlage", "Lifting plant", "HBA"}, {"411", "Neutralisationsanlage", "Neutralisation plant", "NEU"},
        {"412", "Druckerhöhungsanlage", "Booster system", "DEA"}, {"412", "Enthärtungsanlage", "Softening plant", "EHA"},
        {"412", "Trinkwasser", "Drinking water", "TWA"}, {"412", "Trinkwassererwärmung", "Domestic hot water", "TWE"},
        {"412", "Umkehrosmoseanlage", "Reverse osmosis", "UKO"}, {"412", "Vollentsalzungsanlage", "Demineralisation", "VEA"},
        {"421", "BHKW", "CHP", "BHK"}, {"421", "Fernwärmeübergabe", "District heating transfer", "FWÜ"},
        {"421", "Geothermieanlage", "Geothermal system", "GTH"}, {"421", "Kessel", "Boiler", "KES"},
        {"421", "Reindampferzeugung", "Clean steam generation", "RDE"}, {"421", "Solaranlage", "Solar thermal", "SOL"},
        {"421", "Wärmepumpe", "Heat pump", "WPU"}, {"422", "Druckhaltung", "Pressure maintenance", "DHG"},
        {"422", "Heizkreis", "Heating circuit", "HZK"}, {"422", "Pufferspeicher", "Buffer tank", "PFS"},
        {"422", "Wärmeübertrager", "Heat exchanger", "WTR"}, {"423", "Betonkernaktivierung", "Concrete core activation", "BKA"},
        {"423", "Fußbodenheizung", "Underfloor heating", "FBH"}, {"423", "Heiz-/Kühldecke", "Heated/chilled ceiling", "HKD"},
        {"423", "Statische Heizung", "Static heating", "STH"}, {"430", "Abluftanlage", "Exhaust air system", "ABA"},
        {"430", "Kreislaufverbundsystem", "Run-around coil system", "KVS"}, {"430", "Maschinelle Rauchabzugsanlage", "Mechanical smoke extraction", "MRA"},
        {"430", "Rauchdruckanlage", "Smoke pressurisation", "RDA"}, {"430", "Raumlufttechnik allgemein", "Ventilation general", "RLT"},
        {"430", "Teilklimaanlage", "Partial air conditioning", "TKA"}, {"430", "Umluftanlage", "Recirculation system", "ULA"},
        {"430", "Vollklimaanlage", "Full air conditioning", "VKA"}, {"430", "Wärmerückgewinnung", "Heat recovery", "WRG"},
        {"430", "Zuluftanlage", "Supply air system", "ZUA"}, {"430", "Entrauchungsanlage", "Smoke extraction", "ERA"},
        {"434", "Fernkälteübergabe", "District cooling transfer", "FKÜ"}, {"434", "Kältekreis", "Cooling circuit", "KKR"},
        {"434", "Kälteverteilnetz", "Cooling distribution", "KVN"}, {"434", "Kaltwassererzeugung", "Chilled water generation", "KWE"},
        {"434", "Kältespeicher", "Cold storage", "KSP"}, {"434", "Kühldecke", "Chilled ceiling", "KÜD"},
        {"434", "Rückkühlwerk", "Re-cooling plant", "RKW"}, {"441", "Schaltanlagen", "Switchgear", "STA"},
        {"441", "Transformator", "Transformer", "TRF"}, {"441", "Kompensationsanlage", "Compensation system", "KOA"},
        {"442", "Batterieanlage", "Battery unit", "BAT"}, {"442", "Ersatzstromanlage", "Standby power", "ESA"},
        {"442", "Fotovoltaikanlage", "Photovoltaic system", "PVA"}, {"442", "USV", "UPS", "USV"},
        {"443", "Allgemeine Stromversorgung", "General power supply", "ASV"}, {"443", "Sicherheitsstromversorgung", "Safety power supply", "SSV"},
        {"443", "Beleuchtungsanlage", "Lighting", "BEL"}, {"443", "Sicherheitsbeleuchtung", "Emergency lighting", "SIB"},
    };
    for (const auto& r : rows) t.rows.push_back({r.kg, {r.de, r.en}, r.code});
    return t;
}

Table devices() {
    Table t;
    t.id = "vdi3814_devices";
    t.category = "automation";
    t.title = {"VDI 3814: Geräte- und Sensorkürzel", "VDI 3814: device and sensor codes"};
    t.summary = {"Komponentenbezeichnung (Tabelle 3, Auszug)", "Component designation (Table 3, extract)"};
    t.description = kSourceNote;
    t.columns = {{"Gruppe", "Group"}, {"Gerät/Komponente", "Device/component"}, {"Kürzel", "Code"}};
    struct R {
        const char* group;
        const char* de;
        const char* en;
        const char* code;
    };
    static const R rows[] = {
        {"430", "Abluftventilator", "Exhaust fan", "ABV"}, {"430", "Zuluftventilator", "Supply fan", "ZUV"},
        {"430", "Fortluftventilator", "Discharge air fan", "FOV"}, {"430", "Umluftventilator", "Recirculation fan", "UMV"},
        {"430", "Befeuchter", "Humidifier", "BEF"}, {"430", "Brandschutzklappe", "Fire damper", "BSK"},
        {"430", "Entrauchungsklappe", "Smoke extraction damper", "ERK"}, {"430", "Erhitzer allgemein", "Heater general", "ERH"},
        {"430", "Wasserlufterhitzer", "Water air heater", "WLH"}, {"430", "Wasservorerhitzer", "Water pre-heater", "WVH"},
        {"430", "Wassernacherhitzer", "Water re-heater", "WNH"}, {"430", "Elektrolufterhitzer", "Electric air heater", "ELH"},
        {"430", "Luftfilter", "Air filter", "LFI"}, {"430", "Lüftungsklappe", "Ventilation damper", "LKL"},
        {"430", "Pumpe", "Pump", "PPE"}, {"430", "Variabler Volumenstromregler", "VAV controller", "VVR"},
        {"430", "Wärmerückgewinnung", "Heat recovery", "WRG"}, {"430", "Umluftkühlgerät", "Recirculating cooler", "ULK"},
        {"434", "Kältemaschine", "Chiller", "KMA"}, {"434", "Rückkühlwerk", "Re-cooling plant", "RKW"},
        {"440", "Leistungsschalter", "Circuit breaker", "LSS"}, {"440", "Netzanalysegerät", "Power analyser", "NAG"},
        {"440", "Notstromaggregat", "Standby generator", "NSG"}, {"440", "Transformator", "Transformer", "TRF"},
        {"480", "Anlagenschalter", "Plant switch", "ANL"}, {"480", "Automationsstationseinrichtung", "Automation station", "ASE"},
        {"480", "Datenschnittstelleneinheit", "Data interface unit", "DSE"}, {"480", "Feldbusmodul", "Fieldbus module", "FBM"},
        {"481 T", "Außentemperatur", "Outdoor temperature", "TAU"}, {"481 T", "Zulufttemperatur", "Supply air temperature", "TZU"},
        {"481 T", "Ablufttemperatur", "Extract air temperature", "TAB"}, {"481 T", "Fortlufttemperatur", "Exhaust air temperature", "TFO"},
        {"481 T", "Mischlufttemperatur", "Mixed air temperature", "TMI"}, {"481 T", "Raumtemperatur", "Room temperature", "TRU"},
        {"481 T", "Vorlauftemperatur", "Flow temperature", "TVL"}, {"481 T", "Rücklauftemperatur", "Return temperature", "TRL"},
        {"481 T", "Frostschutzwächter", "Frost protection thermostat", "FSW"}, {"481 T", "Sicherheitstemperaturbegrenzer", "Safety temperature limiter", "STB"},
        {"481 T", "Temperatur nach WRG", "Temperature after heat recovery", "TNW"}, {"481 T", "Temperatur Warmwasser", "Hot water temperature", "TWW"},
        {"481 p", "Differenzdruck", "Differential pressure", "PDI"}, {"481 p", "Zuluftdruck", "Supply air pressure", "PZU"},
        {"481 p", "Raumdruck", "Room pressure", "PRU"}, {"481 p", "Druckwächter", "Pressure switch", "DRW"},
        {"481 Q", "CO₂-Sensor", "CO₂ sensor", "COM"}, {"481 Q", "Luftqualität Raum", "Room air quality", "QRU"},
        {"481 φ", "Raumfeuchte relativ", "Room relative humidity", "MRU"}, {"481 φ", "Zuluftfeuchte relativ", "Supply air relative humidity", "MZU"},
        {"481 x", "Zuluftfeuchte absolut", "Supply air absolute humidity", "XZU"}, {"481 φ", "Kondensationswächter", "Condensation monitor", "MKW"},
        {"481 Z", "Elektrozähler", "Electricity meter", "ELZ"}, {"481 Z", "Wärmemengenzähler", "Heat meter", "WMZ"},
        {"481 Z", "Kältemengenzähler", "Cooling meter", "KMZ"}, {"481 Z", "Wasserzähler", "Water meter", "WAZ"},
        {"481 S", "Leckageüberwachung", "Leak detection", "LCK"}, {"481 S", "Strömungswächter", "Flow switch", "SSW"},
        {"481 S", "Not-Aus-Taster", "Emergency stop", "NOT"}, {"481 V̇", "Volumenstrom Zuluft", "Supply airflow", "FZU"},
        {"481 V̇", "Volumenstrom Abluft", "Extract airflow", "FAB"}, {"481 A", "Motorische Klappe", "Motorised damper", "KLA"},
        {"481 A", "Motorventil", "Motorised valve", "VEN"}, {"481 A", "Heizkörperventil-Antrieb", "Radiator valve drive", "HKV"},
        {"485", "Präsenzmelder", "Presence detector", "PAE"}, {"485", "Raumbediengerät", "Room control unit", "RBG"},
        {"485", "Raumautomationseinrichtung", "Room automation device", "RAE"},
    };
    for (const auto& r : rows) t.rows.push_back({r.group, {r.de, r.en}, r.code});
    return t;
}

Table functions() {
    Table t;
    t.id = "vdi3814_functions";
    t.category = "automation";
    t.title = {"VDI 3814: Funktionskennungen", "VDI 3814: function identifiers"};
    t.summary = {"Funktionskennung im BAS (Tabelle 4)", "Function identifier in the BAS (Table 4)"};
    t.description = cat(kSourceNote, Text{"\nZusatz-Erläuterung möglich, z. B. SWH01_ECO, ABV01_WM_01_REP. Spalte „Typ. E/A“ = "
                                          "übliche Zuordnung aus der Planungspraxis, nicht Teil der Richtlinie.",
                                          "\nAdditional explanation possible, e.g. SWH01_ECO, ABV01_WM_01_REP. Column \"Typ. I/O\" = "
                                          "common planning practice, not part of the guideline."});
    t.columns = {{"Funktionskennung", "Function identifier"}, {"Kürzel", "Code"}, {"Typ. E/A", "Typ. I/O"}};
    struct R {
        const char* de;
        const char* en;
        const char* code;
        const char* io;
    };
    static const R rows[] = {
        {"Anforderung", "Requirement", "AF_", "BI/COM"}, {"Anlagenschalter", "Plant switch", "AS_", "BI"},
        {"Belegtzustand Raum", "Room occupancy", "BEL", "BI/COM"}, {"Betriebsbereit", "Ready", "BB_", "BI"},
        {"Betriebsmeldung", "Operating message", "BM_", "BI"}, {"Betriebsstunden", "Operating hours", "BZ_", "virt."},
        {"Einschaltfehler", "Switch-on error", "EF_", "virt."}, {"Entriegelung", "Unlocking", "ENT", "BO"},
        {"Freigabe", "Enable", "FR_", "BO"}, {"Gefahrmeldung", "Danger alarm", "GM_", "BI"},
        {"Grenzwert", "Limit value", "GW_", "virt."}, {"Grenzwert MAX", "Max. limit", "GWX", "virt."},
        {"Grenzwert MIN", "Min. limit", "GWI", "virt."}, {"LVB-Handmeldung", "Local override message", "HD_", "BI"},
        {"Heizkennlinie Steilheit", "Heating curve slope", "HST", "Par."}, {"Hysterese", "Hysteresis", "HYS", "Par."},
        {"Kalender", "Calendar", "KAL", "virt."}, {"Laufzeitfehler", "Runtime error", "LFZ", "virt."},
        {"Loop-Objekt (Regler)", "Loop object (controller)", "LP_", "virt."}, {"Messwert", "Measured value", "MW_", "AI"},
        {"Mittelwert", "Mean value", "MWT", "virt."}, {"Nachlaufzeit", "Follow-up time", "NL_", "Par."},
        {"Nachstellzeit", "Reset time", "TN_", "Par."}, {"Programm", "Program", "PR_", "virt."},
        {"Proportionalband", "Proportional band", "XP_", "Par."}, {"Rückmeldung AUF", "Feedback OPEN", "RMA", "BI"},
        {"Rückmeldung ZU", "Feedback CLOSED", "RMZ", "BI"}, {"Rückführwert", "Feedback value", "RW_", "AI"},
        {"Rückmeldung", "Feedback", "RM_", "BI"}, {"Ausführkontrolle", "Execution control", "RF_", "virt."},
        {"Schaltbefehl", "Switching command", "SB_", "BO"}, {"Scheduler", "Scheduler", "SCH", "virt."},
        {"Sollwert", "Setpoint", "SW_", "AO/virt."}, {"Sollwert Heizen", "Heating setpoint", "SWH", "virt."},
        {"Sollwert Kühlen", "Cooling setpoint", "SWK", "virt."}, {"Sollwert Nacht", "Night setpoint", "SWN", "virt."},
        {"Sollwert Tag", "Day setpoint", "SWT", "virt."}, {"Sollwertversteller", "Setpoint adjuster", "SWV", "AI/COM"},
        {"Status", "Status", "STA", "virt."}, {"Stellsignal", "Control signal", "ST_", "AO"},
        {"Störung", "Fault", "SM_", "BI"}, {"Taster", "Button", "TS_", "BI"},
        {"Totzone", "Dead zone", "TZ_", "Par."}, {"Trend-Log-Objekt", "Trend log object", "TR_", "virt."},
        {"Vorhaltezeit", "Derivative time", "TV_", "Par."}, {"Wartungsmeldung", "Maintenance message", "WM_", "BI"},
        {"Wirkungsgrad", "Efficiency", "WG_", "virt."}, {"Zählwert", "Count value", "ZW_", "BI/COM"},
        {"Zeitprogramm", "Time program", "ZP_", "virt."},
    };
    for (const auto& r : rows) t.rows.push_back({{r.de, r.en}, r.code, r.io});
    return t;
}

}  // namespace

void addVdiTables(std::vector<Table>& out) {
    out.push_back(trades());
    out.push_back(plants());
    out.push_back(devices());
    out.push_back(functions());
}

}  // namespace elc::catalog
