#include "elc/instruments.hpp"

#include <algorithm>
#include <cctype>
#include <map>

namespace elc::instruments {

namespace {

using F = Function;

const Text kHlks{"HLKS", "HVAC"};
const Text kMsr{"MSR", "I&C"};
const Text kProc{"Prozess", "Process"};
const Text kElt{"Elektro", "Electrical"};

struct Spec {
    const char* ann = "";
    Text note;
    const char* isaFixed = "";
    const char* enFixed = "";
};

Entry make(const char* id, Text name, const char* words, const char* isa, const char* en, F fn, const char* vdi, const char* vdiFn,
           const char* iec, const Text& domain, Spec s = {}) {
    Entry e;
    e.id = id;
    e.name = std::move(name);
    e.words = words;
    e.isa = isa;
    e.en = en;
    e.annotation = s.ann;
    e.fn = fn;
    e.vdi = vdi;
    e.vdiFn = vdiFn;
    e.iec = iec;
    e.domain = domain;
    e.note = std::move(s.note);
    e.isaFixed = s.isaFixed;
    e.enFixed = s.enFixed;
    return e;
}

const Text kGasNote{"Alarmstufen typ. ASH (Voralarm) und ASHH (Hauptalarm), Messbereich meist % UEG.",
                    "Alarm levels typically ASH (pre-alarm) and ASHH (main alarm), range usually % LEL."};

std::vector<Entry> build() {
    std::vector<Entry> v;
    // ---- temperature (VDI 3814-4.1 Tab. 3: 481 GA-Sensorik – Temperatur)
    v.push_back(make("t", {"Temperatur", "Temperature"}, "temperatur temp temperature temperaturfuehler temperaturmessung", "T", "T",
                     F::Transmitter, "TZU, TAB, TAU, TRU, TVL, TRL", "", "BT", kMsr,
                     {"", {"VDI-Kürzel je nach Ort, z. B. „Temperatur Zuluft“ eingeben.", "VDI code depends on location, e.g. enter “supply air temperature”."}}));
    v.push_back(make("t_sup", {"Zulufttemperatur", "Supply air temperature"}, "zuluft zulufttemperatur temperatur supply air", "T", "T",
                     F::Transmitter, "TZU", "", "BT", kHlks));
    v.push_back(make("t_ext", {"Ablufttemperatur", "Extract air temperature"}, "abluft ablufttemperatur temperatur extract", "T", "T",
                     F::Transmitter, "TAB", "", "BT", kHlks));
    v.push_back(make("t_out", {"Außentemperatur", "Outdoor temperature"}, "aussen aussentemperatur aussenluft temperatur outdoor outside",
                     "T", "T", F::Transmitter, "TAU", "", "BT", kHlks));
    v.push_back(make("t_room", {"Raumtemperatur", "Room temperature"}, "raum raumtemperatur raumfuehler temperatur room", "T", "T",
                     F::Transmitter, "TRU", "", "BT", kHlks));
    v.push_back(make("t_exh", {"Fortlufttemperatur", "Exhaust air temperature"}, "fortluft fortlufttemperatur temperatur exhaust", "T", "T",
                     F::Transmitter, "TFO", "", "BT", kHlks));
    v.push_back(make("t_mix", {"Mischlufttemperatur", "Mixed air temperature"}, "mischluft mischlufttemperatur temperatur mixed", "T", "T",
                     F::Transmitter, "TMI", "", "BT", kHlks));
    v.push_back(make("t_flow", {"Vorlauftemperatur", "Flow temperature"}, "vorlauf vorlauftemperatur temperatur flow", "T", "T",
                     F::Transmitter, "TVL", "", "BT", kHlks));
    v.push_back(make("t_ret", {"Rücklauftemperatur", "Return temperature"}, "ruecklauf ruecklauftemperatur temperatur return", "T", "T",
                     F::Transmitter, "TRL", "", "BT", kHlks));
    v.push_back(make("t_dhw", {"Warmwassertemperatur", "Hot water temperature"}, "warmwasser trinkwarmwasser twe temperatur dhw hot water",
                     "T", "T", F::Transmitter, "TWW", "", "BT", kHlks));
    v.push_back(make("t_sto", {"Speichertemperatur", "Storage tank temperature"}, "speicher speichertemperatur puffer pufferspeicher temperatur tank",
                     "T", "T", F::Transmitter, "TSP", "", "BT", kHlks));
    v.push_back(make("t_flue", {"Abgastemperatur", "Flue gas temperature"}, "abgas abgastemperatur temperatur flue", "T", "T",
                     F::Transmitter, "TAG", "", "BT", kHlks));
    v.push_back(make("t_coil", {"Temperatur nach Kühler", "Temperature after cooling coil"}, "kuehler nach kuehlregister temperatur cooling coil",
                     "T", "T", F::Transmitter, "TNK", "", "BT", kHlks));
    v.push_back(make("frost", {"Frostschutzwächter", "Frost protection thermostat"},
                     "frost frostschutz frostschutzwaechter frostschutzthermostat frostwaechter thermostat", "T", "T", F::SwitchLow, "FSW",
                     "GM_", "BT", kHlks,
                     {"", {"Öffnerkontakt, schaltet bei Unterschreiten (Kapillare über das ganze Register).",
                           "NC contact, trips below the set point (capillary across the whole coil)."}}));
    v.push_back(make("stb", {"Sicherheitstemperaturbegrenzer (STB)", "Safety temperature limiter (STB)"},
                     "stb sicherheitstemperaturbegrenzer begrenzer sicherheit safety limiter", "T", "T", F::Safety, "STB", "GM_", "BT", kHlks,
                     {"", {"Verriegelnd, Entriegelung nur von Hand.", "Latching, manual reset only."}}));
    v.push_back(make("stw", {"Sicherheitstemperaturwächter (STW)", "Safety temperature monitor (STW)"},
                     "stw sicherheitstemperaturwaechter sicherheit waechter", "T", "T", F::Safety, "STW", "GM_", "BT", kHlks,
                     {"", {"Selbsttätig rückstellend.", "Self-resetting."}}));
    v.push_back(make("thermostat", {"Thermostat / Temperaturwächter", "Thermostat / temperature switch"},
                     "thermostat temperaturwaechter temperaturschalter temperature switch", "T", "T", F::SwitchHigh, "THM", "SM_", "BT", kHlks));
    v.push_back(make("t_dew", {"Taupunkttemperatur", "Dew point temperature"}, "taupunkt taupunkttemperatur dew point", "T", "T",
                     F::Transmitter, "TTP", "", "BT", kHlks));
    v.push_back(make("dew_sw", {"Taupunktwächter", "Dew point switch"}, "taupunktwaechter taupunktschalter betauung kuehldecke dew switch",
                     "M", "M", F::SwitchHigh, "TPW", "SM_", "BT", kHlks,
                     {"", {"Betauung an Kühldecken/-segeln: Kühlwasserventil schließen.", "Condensation at chilled ceilings: close the chilled water valve."}}));
    v.push_back(make("cond", {"Kondensationswächter", "Condensation monitor"}, "kondensation kondensationswaechter kondensat schwitzwasser condensation",
                     "M", "M", F::SwitchHigh, "MKW", "SM_", "BM", kHlks));
    // ---- humidity
    const Text mNote{"ISA-5.1: M ist frei wählbar und üblich für Feuchte (Moisture).", "ISA-5.1: M is user's choice, common for moisture/humidity."};
    v.push_back(make("m", {"Feuchte (relativ)", "Relative humidity"}, "feuchte luftfeuchte relativ rh humidity feuchtigkeit feuchtefuehler", "M",
                     "M", F::Transmitter, "MZU, MAB, MAU, MRU", "", "BM", kHlks, {"", mNote}));
    v.push_back(make("m_sup", {"Zuluftfeuchte", "Supply air humidity"}, "zuluft zuluftfeuchte feuchte supply humidity", "M", "M",
                     F::Transmitter, "MZU", "", "BM", kHlks, {"", mNote}));
    v.push_back(make("m_ext", {"Abluftfeuchte", "Extract air humidity"}, "abluft abluftfeuchte feuchte extract humidity", "M", "M",
                     F::Transmitter, "MAB", "", "BM", kHlks, {"", mNote}));
    v.push_back(make("m_out", {"Außenfeuchte", "Outdoor humidity"}, "aussen aussenfeuchte aussenluft feuchte outdoor humidity", "M", "M",
                     F::Transmitter, "MAU", "", "BM", kHlks, {"", mNote}));
    v.push_back(make("m_room", {"Raumfeuchte", "Room humidity"}, "raum raumfeuchte feuchte room humidity", "M", "M", F::Transmitter, "MRU",
                     "", "BM", kHlks, {"", mNote}));
    v.push_back(make("m_abs", {"Absolute Feuchte (Wassergehalt)", "Absolute humidity (humidity ratio)"},
                     "absolute absolut feuchte wassergehalt humidity ratio", "M", "M", F::Transmitter, "XZU, XAB, XAU, XRU", "", "BX", kHlks,
                     {"", mNote}));
    v.push_back(make("hygrostat", {"Hygrostat / Feuchtewächter", "Hygrostat / humidity switch"},
                     "hygrostat feuchtewaechter feuchteschalter humidistat", "M", "M", F::SwitchHigh, "", "SM_", "BM", kHlks, {"", mNote}));
    // ---- pressure
    v.push_back(make("p", {"Druck", "Pressure"}, "druck pressure drucksensor drucktransmitter druckmessung", "P", "P", F::Transmitter,
                     "PZU, PAB, PVI", "", "BP", kMsr));
    v.push_back(make("pd", {"Differenzdruck", "Differential pressure"}, "differenzdruck druckdifferenz dp delta differential", "PD", "PD",
                     F::Transmitter, "PDI", "", "BP", kMsr));
    v.push_back(make("p_room", {"Raumdruck (Reinraum)", "Room pressure (cleanroom)"}, "raumdruck reinraum ueberdruck druckkaskade room cleanroom",
                     "PD", "PD", F::Transmitter, "PRU", "", "BP", kHlks,
                     {"", {"Raumdruck gegen Referenz = Differenzdruck.", "Room pressure against reference = differential pressure."}}));
    v.push_back(make("p_duct", {"Zuluftdruck (Kanaldruck)", "Supply duct pressure"}, "zuluftdruck kanaldruck druck duct pressure", "P", "P",
                     F::Transmitter, "PZU", "", "BP", kHlks));
    v.push_back(make("p_water", {"Wasserdruck / Anlagendruck", "Water / system pressure"},
                     "wasserdruck anlagendruck systemdruck heizungsdruck druck water system pressure", "P", "P", F::Transmitter, "PVI", "",
                     "BP", kHlks));
    v.push_back(make("p_sw", {"Druckwächter / Pressostat", "Pressure switch"}, "druckwaechter pressostat druckschalter pressure switch", "P",
                     "P", F::SwitchHigh, "DRW", "SM_", "BP", kMsr));
    v.push_back(make("sdb", {"Sicherheitsdruckbegrenzer (SDB)", "Safety pressure limiter"}, "sdb sicherheitsdruckbegrenzer begrenzer sicherheit",
                     "P", "P", F::Safety, "SDB", "GM_", "BP", kHlks));
    v.push_back(make("filter", {"Filterüberwachung (Differenzdruckwächter)", "Filter monitoring (Δp switch)"},
                     "filter filterwaechter filterueberwachung filterverschmutzung differenzdruckwaechter dirty", "PD", "PD", F::SwitchHigh,
                     "LFI", "WM_", "BP", kHlks,
                     {"", {"VDI-Kürzel LFI = Luftfilter (Bauteil), Meldung als Wartungsmeldung WM_.", "VDI code LFI = air filter (component), signal as maintenance message WM_."}}));
    // ---- flow
    v.push_back(make("f_air", {"Volumenstrom Luft", "Air flow rate"}, "volumenstrom luftmenge luftvolumenstrom luft airflow", "F", "F",
                     F::Transmitter, "FZU, FAB, FAU, FFO", "", "BS", kHlks));
    v.push_back(make("f_water", {"Durchfluss Wasser", "Water flow"}, "durchfluss durchflussmesser wasserdurchfluss flowmeter mid magnetisch induktiv",
                     "F", "F", F::Transmitter, "FKW, FWW", "", "BS", kMsr));
    v.push_back(make("f_sw_air", {"Strömungswächter (Luft)", "Air flow switch"},
                     "stroemungswaechter luftstroemungswaechter keilriemenwaechter keilriemen air flow switch", "F", "F", F::SwitchLow, "SSW",
                     "BM_", "BS", kHlks));
    v.push_back(make("f_sw_water", {"Wasserströmungswächter", "Water flow switch"},
                     "wasserstroemungswaechter paddelschalter durchflusswaechter water flow switch", "F", "F", F::SwitchLow, "WSW", "BM_", "BL",
                     kHlks));
    // ---- meters
    v.push_back(make("q_water", {"Wasserzähler", "Water meter"}, "wasserzaehler wasseruhr zaehler water meter", "F", "F", F::Totalizer, "WAZ",
                     "ZW_", "BG", kHlks));
    v.push_back(make("q_gas", {"Gaszähler", "Gas meter"}, "gaszaehler gasuhr erdgaszaehler zaehler gas meter", "F", "F", F::Totalizer, "EGZ",
                     "ZW_", "BF", kHlks));
    v.push_back(make("q_heat", {"Wärmemengenzähler", "Heat meter"}, "waermemengenzaehler wmz waermezaehler zaehler heat meter energie", "J", "",
                     F::Totalizer, "WMZ", "ZW_", "BG", kHlks,
                     {"", {"Energie = Integral der Leistung (ISA: J + Q).", "Energy = integral of power (ISA: J + Q)."}}));
    v.push_back(make("q_cold", {"Kältemengenzähler", "Cooling energy meter"}, "kaeltemengenzaehler kmz kaeltezaehler zaehler cooling meter", "J",
                     "", F::Totalizer, "KMZ", "ZW_", "BG", kHlks));
    v.push_back(make("q_el", {"Elektrozähler", "Electricity meter"}, "elektrozaehler stromzaehler energiezaehler zaehler electricity meter kwh",
                     "J", "E", F::Totalizer, "ELZ", "ZW_", "BC", kElt));
    // ---- level / leakage
    v.push_back(make("l", {"Füllstand", "Level"}, "fuellstand niveau level pegel fuellstandsmessung behaelter tank", "L", "L", F::Transmitter,
                     "LSP", "", "BL", kMsr));
    v.push_back(make("l_high", {"Niveauschalter max (Überlauf)", "High level switch (overflow)"},
                     "ueberlauf niveauschalter schwimmerschalter maximal max level switch high overflow", "L", "L", F::SwitchHigh, "UBL",
                     "GM_", "BL", kMsr));
    v.push_back(make("l_low", {"Niveauschalter min (Trockenlauf)", "Low level switch (dry running)"},
                     "trockenlauf trockenlaufschutz mindestniveau niveauschalter minimal min low level", "L", "L", F::SwitchLow, "", "SM_",
                     "BL", kMsr));
    v.push_back(make("wms", {"Wassermangelsicherung", "Low water cut-off"}, "wassermangel wassermangelsicherung low water", "L", "L",
                     F::SwitchLow, "WMS", "GM_", "BL", kHlks));
    v.push_back(make("leak", {"Leckageüberwachung", "Leak detection"},
                     "leckage leckagewaechter leckageueberwachung leckagesensor wassermelder bodensonde wasseralarm leak leakage", "L", "L",
                     F::SwitchHigh, "LCK", "GM_", "BM", kHlks,
                     {"", {"Bodensonde/Sensorkabel z. B. in Auffangwanne; ISA alternativ YS (Ereignis).",
                           "Floor probe/sensor cable e.g. in a drip tray; ISA alternatively YS (event)."}}));
    // ---- gas and analysis
    v.push_back(make("co2", {"CO₂ (Luftqualität)", "CO₂ (air quality)"}, "co2 kohlendioxid carbon dioxide luftqualitaet", "A", "Q", F::Transmitter,
                     "COM", "", "BQ", kHlks, {"CO₂", {}}));
    v.push_back(make("voc", {"Luftqualität (VOC/Mischgas)", "Air quality (VOC/mixed gas)"}, "luftqualitaet voc mischgas iaq air quality", "A",
                     "Q", F::Transmitter, "LQF", "", "BQ", kHlks, {"VOC", {}}));
    v.push_back(make("ch4", {"Gaswarnung Methan (Erdgas)", "Gas detection methane (natural gas)"}, "methan ch4 erdgas natural gas", "A", "Q",
                     F::Transmitter, "", "GM_", "BQ", kProc, {"CH₄", kGasNote}));
    v.push_back(make("h2", {"Gaswarnung Wasserstoff", "Gas detection hydrogen"}, "wasserstoff h2 hydrogen batterieraum elektrolyse gas", "A", "Q",
                     F::Transmitter, "", "GM_", "BQ", kProc, {"H₂", kGasNote}));
    v.push_back(make("co", {"Gaswarnung Kohlenmonoxid", "Gas detection carbon monoxide"}, "kohlenmonoxid co carbon monoxide tiefgarage garage gas",
                     "A", "Q", F::Transmitter, "", "GM_", "BQ", kHlks, {"CO", kGasNote}));
    v.push_back(make("nh3", {"Gaswarnung Ammoniak", "Gas detection ammonia"}, "ammoniak nh3 ammonia gas", "A", "Q", F::Transmitter, "", "GM_",
                     "BQ", kProc, {"NH₃", kGasNote}));
    v.push_back(make("refr", {"Kältemittelwarnung", "Refrigerant leak detection"},
                     "kaeltemittel kaeltemittelwarnung kaeltemittelleckage refrigerant r32 r290 r744 r410a r1234ze gas", "A", "Q",
                     F::Transmitter, "", "GM_", "BQ", kHlks, {"Rxxx", {"Nach EN 378 je nach Kältemittel und Füllmenge.", "Per EN 378 depending on refrigerant and charge."}}));
    v.push_back(make("o2", {"Sauerstoffüberwachung (O₂-Mangel)", "Oxygen monitoring (O₂ deficiency)"},
                     "sauerstoff o2 oxygen sauerstoffmangel inertgas stickstoff n2", "A", "Q", F::Transmitter, "", "GM_", "BQ", kProc,
                     {"O₂", {"Alarm bei Unterschreitung (ASL/ASLL).", "Alarm on low concentration (ASL/ASLL)."}}));
    v.push_back(make("gas", {"Gaswarnanlage (allgemein)", "Gas detection (general)"}, "gas gaswarnanlage gasmelder gaswarner gaswarnung detector",
                     "A", "Q", F::Transmitter, "", "GM_", "BQ", kProc,
                     {"Gas", {"Gasart als Zusatz angeben, z. B. AT-101 (CH₄).", "State the gas as annotation, e.g. AT-101 (CH₄)."}}));
    v.push_back(make("smoke", {"Rauchmelder (Kanal)", "Smoke detector (duct)"}, "rauch rauchmelder kanalrauchmelder smoke brand", "A", "Q",
                     F::SwitchHigh, "RME", "GM_", "BR", kHlks,
                     {"Rauch", {"ISA ohne eigenen Buchstaben – üblich AS bzw. XS mit Zusatz „Rauch“.", "No ISA letter of its own – usually AS or XS annotated “smoke”."}}));
    v.push_back(make("flame", {"Flammenwächter", "Flame detector"}, "flamme flammenwaechter flammenueberwachung brenner ionisation flame burner", "B",
                     "", F::SwitchLow, "", "GM_", "", kProc));
    v.push_back(make("ph", {"pH-Wert", "pH value"}, "ph phwert saeure lauge", "A", "Q", F::Transmitter, "", "", "BQ", kProc, {"pH", {}}));
    v.push_back(make("cond_el", {"Leitfähigkeit", "Conductivity"}, "leitfaehigkeit conductivity reinstwasser wfi purified water", "A", "Q",
                     F::Transmitter, "", "", "BQ", kProc, {"µS/cm", {}}));
    // ---- speed, vibration, position, weight
    v.push_back(make("speed", {"Drehzahl", "Speed"}, "drehzahl speed frequenz frequency rpm umdrehung", "S", "S", F::Transmitter, "SZU, SAB, SPP",
                     "", "BS", kMsr));
    v.push_back(make("vib", {"Schwingung / Vibration", "Vibration"}, "schwingung vibration lagerschwingung", "V", "G", F::Transmitter, "", "", "",
                     kProc, {"", {"EN 62424 / DIN 19227: G = Amplitude, V = Viskosität.", "EN 62424 / DIN 19227: G = amplitude, V = viscosity."}}));
    v.push_back(make("pos", {"Stellungsrückmeldung (Ventil/Klappe)", "Position feedback (valve/damper)"},
                     "stellung stellungsrueckmeldung position klappenstellung ventilstellung rueckmeldung", "Z", "G", F::Transmitter, "", "RW_",
                     "BG", kMsr, {"", {"VDI-Funktion RW_ (Rückführwert).", "VDI function RW_ (feedback value)."}}));
    v.push_back(make("limit", {"Endlagenschalter (AUF/ZU)", "Limit switch (open/closed)"}, "endschalter endlage endlagenschalter auf zu limit switch",
                     "Z", "G", F::SwitchHigh, "", "RMA / RMZ", "BG", kMsr, {"", {}, "ZSO / ZSC", "GSH / GSL"}));
    v.push_back(make("weight", {"Gewicht / Wägezelle", "Weight / load cell"}, "gewicht waage waegezelle masse weight load cell", "W", "W",
                     F::Transmitter, "", "", "", kProc));
    // ---- electrical
    v.push_back(make("current", {"Strom (elektrisch)", "Electric current"}, "strom stromwandler stromaufnahme ampere current", "I", "E",
                     F::Transmitter, "", "", "", kElt));
    v.push_back(make("voltage", {"Spannung", "Voltage"}, "spannung netzspannung voltage", "E", "E", F::Transmitter, "", "", "", kElt));
    v.push_back(make("power", {"Wirkleistung (elektrisch)", "Electric power"}, "leistung wirkleistung leistungsmessung power", "J", "E",
                     F::Transmitter, "", "", "", kElt));
    // ---- weather, light, presence
    v.push_back(make("lux", {"Beleuchtungsstärke / Helligkeit", "Illuminance / brightness"}, "helligkeit beleuchtungsstaerke lux licht illuminance brightness",
                     "R", "R", F::Transmitter, "EAU, ERU", "", "", kHlks));
    v.push_back(make("wind", {"Windgeschwindigkeit", "Wind speed"}, "wind windgeschwindigkeit windmesser anemometer", "S", "S", F::Transmitter,
                     "WDG", "", "BF", kHlks, {"Wind", {}}));
    v.push_back(make("rain", {"Regenmelder", "Rain sensor"}, "regen regenmelder niederschlag rain", "M", "M", F::SwitchHigh, "RGM", "STA", "BM",
                     kHlks));
    v.push_back(make("sun", {"Sonneneinstrahlung (Globalstrahlung)", "Solar irradiance"}, "sonne sonneneinstrahlung globalstrahlung strahlung pyranometer irradiance",
                     "R", "R", F::Transmitter, "SES", "", "BR", kHlks));
    v.push_back(make("presence", {"Präsenzmelder / Bewegungsmelder", "Presence / motion detector"},
                     "praesenz praesenzmelder bewegungsmelder bewegung belegung presence motion pir", "Y", "X", F::SwitchHigh, "", "BEL", "", kHlks,
                     {"", {"ISA Y = Ereignis, Zustand, Anwesenheit.", "ISA Y = event, state, presence."}, "YS", "XS"}));
    // ---- manual
    v.push_back(make("estop", {"Not-Aus-Taster", "Emergency stop"}, "notaus not aus nothalt emergency stop", "H", "H", F::SwitchHigh, "NOT", "GM_",
                     "SF", kMsr, {"", {}, "HS", "HS"}));
    v.push_back(make("plant_sw", {"Anlagenschalter (Hand-0-Auto)", "Plant switch (hand-off-auto)"}, "anlagenschalter hand auto handschalter wahlschalter hoa",
                     "H", "H", F::SwitchHigh, "ANL", "AS_", "SF", kHlks, {"", {}, "HS", "HS"}));
    v.push_back(make("repair", {"Reparaturschalter", "Maintenance (repair) switch"}, "reparaturschalter revisionsschalter wartungsschalter repair switch",
                     "H", "H", F::SwitchHigh, "", "WM_", "SF", kHlks, {"", {}, "HS", "HS"}));
    return v;
}

std::vector<std::string> tokens(std::string_view text) {
    std::vector<std::string> out;
    std::string cur;
    for (const char c : normalize(text)) {
        if (std::isalnum(static_cast<unsigned char>(c)) != 0) {
            cur += c;
        } else if (!cur.empty()) {
            out.push_back(cur);
            cur.clear();
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

bool contains(const std::vector<std::string>& toks, std::initializer_list<const char*> words) {
    return std::any_of(toks.begin(), toks.end(), [&](const std::string& t) {
        return std::any_of(words.begin(), words.end(), [&](const char* w) { return t == w; });
    });
}

bool isSwitch(Function f) { return f == F::SwitchHigh || f == F::SwitchLow || f == F::Safety || f == F::AlarmHigh || f == F::AlarmLow; }

}  // namespace

std::string normalize(std::string_view text) {
    static const std::map<std::string, std::string> repl{{"ä", "ae"}, {"ö", "oe"}, {"ü", "ue"}, {"Ä", "ae"}, {"Ö", "oe"}, {"Ü", "ue"},
                                                         {"ß", "ss"}, {"₂", "2"},  {"₃", "3"},  {"₄", "4"},  {"µ", "u"},  {"Δ", "delta"}};
    std::string out;
    for (std::size_t i = 0; i < text.size();) {
        bool done = false;
        for (const auto& [from, to] : repl) {
            if (text.compare(i, from.size(), from) == 0) {
                out += to;
                i += from.size();
                done = true;
                break;
            }
        }
        if (done) continue;
        out += static_cast<char>(std::tolower(static_cast<unsigned char>(text[i])));
        ++i;
    }
    return out;
}

const std::vector<Entry>& all() {
    static const std::vector<Entry> list = build();
    return list;
}

std::vector<Match> find(std::string_view text) {
    static const std::vector<std::string> stop{"der", "die", "das", "und", "fuer", "im", "in", "an", "am", "the", "of", "for", "mit", "von"};
    std::vector<std::string> query;
    for (auto& t : tokens(text)) {
        if (t.size() >= 2 && std::find(stop.begin(), stop.end(), t) == stop.end()) query.push_back(t);
    }
    std::vector<Match> out;
    if (query.empty()) return out;
    for (const auto& e : all()) {
        const auto words = tokens(e.name.de + " " + e.name.en + " " + e.words + " " + e.vdi + " " + e.annotation);
        int score = 0;
        for (const auto& q : query) {
            int best = 0;
            for (const auto& w : words) {
                if (q == w) {
                    best = 3;
                    break;
                }
                if ((q.size() >= 4 && w.rfind(q, 0) == 0) || (w.size() >= 4 && q.rfind(w, 0) == 0)) {
                    best = std::max(best, 2);
                } else if (q.size() >= 5 && w.find(q) != std::string::npos) {
                    best = std::max(best, 1);
                }
            }
            score += best;
        }
        if (score > 0) out.push_back({&e, score});
    }
    std::stable_sort(out.begin(), out.end(), [](const Match& a, const Match& b) { return a.score > b.score; });
    return out;
}

Function resolve(const Entry& e, Function requested, std::string_view text) {
    if (requested != F::Auto) return requested;
    const auto toks = tokens(text);
    if (isSwitch(e.fn)) {
        if (contains(toks, {"min", "minimal", "tief", "low", "unten"})) return F::SwitchLow;
        if (contains(toks, {"max", "maximal", "hoch", "high", "oben"})) return F::SwitchHigh;
        return e.fn;
    }
    if (e.fn == F::Totalizer) return e.fn;
    if (contains(toks, {"regler", "regelung", "controller", "regelkreis"})) return F::Controller;
    if (contains(toks, {"anzeige", "thermometer", "manometer", "indicator", "gauge"})) return F::Indicator;
    if (contains(toks, {"fuehler", "element", "pt100", "pt1000", "ni1000", "messelement"})) return F::Element;
    if (contains(toks, {"transmitter", "messumformer"})) return F::Transmitter;
    if (contains(toks, {"waechter", "schalter", "switch"})) return contains(toks, {"min", "tief", "low"}) ? F::SwitchLow : F::SwitchHigh;
    return e.fn;
}

std::string isaCode(const Entry& e, Function f) {
    if (f == F::Auto) f = e.fn;
    if (!e.isaFixed.empty() && f == e.fn) return e.isaFixed;
    const std::string& v = e.isa;
    switch (f) {
        case F::Element: return v + "E";
        case F::Transmitter: return v + "T";
        case F::Indicator: return v + "I";
        case F::Controller: return v + "IC";
        case F::SwitchHigh: return v + "SH";
        case F::SwitchLow: return v + "SL";
        case F::AlarmHigh: return v + "AH";
        case F::AlarmLow: return v + "AL";
        case F::Safety: return v + "SHH";
        case F::Totalizer: return v + "QI";
        case F::Auto: break;
    }
    return v;
}

std::string enCode(const Entry& e, Function f) {
    if (f == F::Auto) f = e.fn;
    if (!e.enFixed.empty() && f == e.fn) return e.enFixed;
    if (e.en.empty()) return "–";
    const std::string& v = e.en;
    switch (f) {
        case F::Element:
        case F::Transmitter:
        case F::Indicator: return v + "I";
        case F::Controller: return v + "IC";
        case F::SwitchHigh: return v + "SH";
        case F::SwitchLow: return v + "SL";
        case F::AlarmHigh: return v + "AH";
        case F::AlarmLow: return v + "AL";
        case F::Safety: return v + "ZH";
        case F::Totalizer: return v + "QI";
        case F::Auto: break;
    }
    return v;
}

Text isaMeaning(const Entry& e, Function f) {
    if (f == F::Auto) f = e.fn;
    static const std::map<char, Text> first{
        {'A', {"Analyse", "analysis"}},          {'B', {"Brenner/Flamme", "burner/flame"}},   {'E', {"Spannung", "voltage"}},
        {'F', {"Durchfluss", "flow"}},           {'H', {"Hand", "hand"}},                     {'I', {"Strom", "current"}},
        {'J', {"Leistung", "power"}},            {'L', {"Stand/Niveau", "level"}},            {'M', {"Feuchte (frei wählbar)", "moisture (user's choice)"}},
        {'P', {"Druck", "pressure"}},            {'R', {"Strahlung", "radiation"}},           {'S', {"Drehzahl/Geschwindigkeit", "speed"}},
        {'T', {"Temperatur", "temperature"}},    {'V', {"Schwingung", "vibration"}},          {'W', {"Gewicht/Kraft", "weight/force"}},
        {'Y', {"Ereignis/Zustand", "event/state"}}, {'Z', {"Position/Stellung", "position"}}};
    Text t;
    if (const auto it = first.find(e.isa.empty() ? ' ' : e.isa.front()); it != first.end()) {
        t = Text{std::string(1, e.isa.front()) + " = " + it->second.de, std::string(1, e.isa.front()) + " = " + it->second.en};
    }
    if (e.isa.size() > 1 && e.isa[1] == 'D') {
        t.de += ", D = Differenz";
        t.en += ", D = differential";
    }
    if (!e.isaFixed.empty() && f == e.fn) {
        t.de += " · Sonderkennung " + e.isaFixed;
        t.en += " · special code " + e.isaFixed;
        return t;
    }
    Text fn;
    switch (f) {
        case F::Element: fn = {"E = Messelement (Fühler)", "E = primary element (sensor)"}; break;
        case F::Transmitter: fn = {"T = Transmitter (Messumformer)", "T = transmitter"}; break;
        case F::Indicator: fn = {"I = Anzeige", "I = indicator"}; break;
        case F::Controller: fn = {"I = Anzeige, C = Regelung", "I = indicate, C = control"}; break;
        case F::SwitchHigh: fn = {"S = Schalter, H = oberer Grenzwert", "S = switch, H = high"}; break;
        case F::SwitchLow: fn = {"S = Schalter, L = unterer Grenzwert", "S = switch, L = low"}; break;
        case F::AlarmHigh: fn = {"A = Alarm, H = oberer Grenzwert", "A = alarm, H = high"}; break;
        case F::AlarmLow: fn = {"A = Alarm, L = unterer Grenzwert", "A = alarm, L = low"}; break;
        case F::Safety: fn = {"S = Schalter, HH = Max-Max (Sicherheitsabschaltung)", "S = switch, HH = high-high (safety trip)"}; break;
        case F::Totalizer: fn = {"Q = Integral/Summe, I = Anzeige", "Q = integrate/totalize, I = indicate"}; break;
        case F::Auto: break;
    }
    t.de += " · " + fn.de;
    t.en += " · " + fn.en;
    return t;
}

}  // namespace elc::instruments
