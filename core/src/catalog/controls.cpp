// Automation calculators and tables taken from the user's technical books (v1.5):
// controller tuning (Siemens Regeltechnik), KNX topology (KNX Association), AMEV data point address key (AMEV GA 2005),
// h,x altitude table (Siemens h,x-Diagramm) and the five safety rules / protective distances (Haag, Band 09).
#include <array>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/air.hpp"
#include "elc/formulas/building.hpp"
#include "elc/formulas/control.hpp"

namespace elc::catalog {

namespace {

// ------------------------------------------------------------------ controller tuning

Calculator controllerTuning() {
    Calculator c;
    c.id = "controller_tuning";
    c.category = "automation";
    c.title = {"Reglereinstellung (Ziegler/Nichols, CHR)", "Controller tuning (Ziegler/Nichols, CHR)"};
    c.summary = {"Kp, Tn, Tv aus Streckenkennwerten Ks, Tu, Tg – Schwierigkeitsgrad S", "Kp, Tn, Tv from process data Ks, Tu, Tg – difficulty S"};
    c.description = {
        "Schwierigkeitsgrad S = Tu / Tg (bzw. Tt / T): < 0,1 leicht, ≈ 0,2 mittel, > 0,3 schwierig zu regeln.\n"
        "Ziegler/Nichols (1942, max. 20 % Überschwingen; Einspeicherstrecke mit Totzeit Tt, Zeitkonstante T):\n"
        "P: Kp = T/(Ks·Tt) · PI: 0,9·T/(Ks·Tt), Tn = 3,3·Tt · PD: 1,2·T/(Ks·Tt), Tv = 0,25·Tt · PID: 1,2·T/(Ks·Tt), "
        "Tn = 2·Tt, Tv = 0,42·Tt.\n"
        "Chien/Hrones/Reswick (1952, Mehrspeicherstrecke, V0 = Ks·Kp):\n"
        "aperiodisch – Störung: P 0,3; PI 0,6 / Tn 4·Tu; PID 0,95 / Tn 2,4·Tu / Tv 0,42·Tu – Führung: P 0,3; "
        "PI 0,35 / Tn 1,2·Tg; PID 0,6 / Tn Tg / Tv 0,5·Tu;\n"
        "20 % Überschwingen – Störung: P 0,7; PI 0,7 / Tn 2,3·Tu; PID 1,2 / Tn 2·Tu / Tv 0,42·Tu – Führung: P 0,7; "
        "PI 0,6 / Tn Tg; PID 0,95 / Tn 1,35·Tg / Tv 0,47·Tu (V0-Faktoren jeweils · Tg/Tu).\n"
        "Quelle: Siemens „Regeltechnik – Grundlagen“ (08/2022), Kap. 3.3 und 6.5, S. 33–34 und 82–83 (Fig. 6-15, 6-18). "
        "Startwerte – an der Anlage nachoptimieren.",
        "Difficulty S = Tu / Tg (or Tt / T): < 0.1 easy, ≈ 0.2 medium, > 0.3 hard to control.\n"
        "Ziegler/Nichols (1942, max. 20 % overshoot; single-capacity process with dead time Tt, time constant T):\n"
        "P: Kp = T/(Ks·Tt) · PI: 0.9·T/(Ks·Tt), Tn = 3.3·Tt · PD: 1.2·T/(Ks·Tt), Tv = 0.25·Tt · PID: 1.2·T/(Ks·Tt), "
        "Tn = 2·Tt, Tv = 0.42·Tt.\n"
        "Chien/Hrones/Reswick (1952, multi-capacity process, V0 = Ks·Kp):\n"
        "aperiodic – disturbance: P 0.3; PI 0.6 / Tn 4·Tu; PID 0.95 / Tn 2.4·Tu / Tv 0.42·Tu – set point: P 0.3; "
        "PI 0.35 / Tn 1.2·Tg; PID 0.6 / Tn Tg / Tv 0.5·Tu;\n"
        "20 % overshoot – disturbance: P 0.7; PI 0.7 / Tn 2.3·Tu; PID 1.2 / Tn 2·Tu / Tv 0.42·Tu – set point: P 0.7; "
        "PI 0.6 / Tn Tg; PID 0.95 / Tn 1.35·Tg / Tv 0.47·Tu (V0 factors each · Tg/Tu).\n"
        "Source: Siemens “Regeltechnik – Grundlagen” (08/2022), ch. 3.3 and 6.5, p. 33–34 and 82–83 (Fig. 6-15, 6-18). "
        "Starting values – fine-tune on site."};
    c.inputs = {
        Num::number("ks", {"Streckenverstärkung Ks", "Process gain Ks"}, "", 1).positive()
            .withHint({"Änderung Regelgröße / Änderung Stellgröße (bezogen)", "Change of controlled variable / change of output (normalised)"}),
        Num::number("tu", {"Verzugszeit Tu bzw. Totzeit Tt", "Delay time Tu or dead time Tt"}, "s", 30).positive(),
        Num::number("tg", {"Ausgleichszeit Tg bzw. Zeitkonstante T", "Balance time Tg or time constant T"}, "s", 300).positive(),
        Num::choice("type", {"Reglertyp", "Controller type"},
                    {{"p", "P"}, {"pi", "PI"}, {"pd", "PD"}, {"pid", "PID"}}, "pi"),
        Num::choice("rule", {"Einstellregel", "Tuning rule"},
                    {{"zn", {"Ziegler/Nichols (max. 20 % Überschwingen)", "Ziegler/Nichols (max. 20 % overshoot)"}},
                     {"chr_ad", {"CHR aperiodisch – Störverhalten", "CHR aperiodic – disturbance"}},
                     {"chr_as", {"CHR aperiodisch – Führungsverhalten", "CHR aperiodic – set point"}},
                     {"chr_2d", {"CHR 20 % Überschwingen – Störverhalten", "CHR 20 % overshoot – disturbance"}},
                     {"chr_2s", {"CHR 20 % Überschwingen – Führungsverhalten", "CHR 20 % overshoot – set point"}}},
                    "chr_ad"),
    };
    c.outputs = {Out::number("S", {"Schwierigkeitsgrad S", "Difficulty S"}, "", 3),
                 Out::text("diff", {"Regelbarkeit", "Controllability"}),
                 Out::number("kp", {"Proportionalbeiwert Kp", "Proportional gain Kp"}, "", 3).primary(),
                 Out::number("xp", {"Proportionalbereich Xp = 100 %/Kp", "Proportional band Xp = 100 %/Kp"}, "%", 1),
                 Out::number("tn", {"Nachstellzeit Tn", "Integral time Tn"}, "s", 1).primary(),
                 Out::number("tv", {"Vorhaltezeit Tv", "Derivative time Tv"}, "s", 1)};
    c.keywords = {"regler", "pid", "ziegler", "nichols", "chien", "hrones", "reswick", "kp", "tn", "tv", "tu", "tg",
                  "einstellregel", "tuning", "regelstrecke", "schwierigkeitsgrad"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        using control::Controller;
        using control::Rule;
        const double S = control::difficultyRatio(in.num("tu"), in.num("tg"));
        r.set("S", S);
        switch (control::classify(S)) {
        case control::Difficulty::Easy: r.setText("diff", {"leicht regelbar (S < 0,1)", "easy (S < 0.1)"}); break;
        case control::Difficulty::Medium: r.setText("diff", {"mittel (S ≈ 0,1 … 0,3)", "medium (S ≈ 0.1 … 0.3)"}); break;
        case control::Difficulty::Hard: r.setText("diff", {"schwierig (S > 0,3)", "hard (S > 0.3)"}); break;
        }
        const auto& t = in.choice("type");
        const auto type = t == "p" ? Controller::P : t == "pd" ? Controller::PD : t == "pid" ? Controller::PID : Controller::PI;
        const auto& k = in.choice("rule");
        const auto rule = k == "zn"       ? Rule::ZieglerNichols
                          : k == "chr_as" ? Rule::ChrAperiodicSetpoint
                          : k == "chr_2d" ? Rule::Chr20Disturbance
                          : k == "chr_2s" ? Rule::Chr20Setpoint
                                          : Rule::ChrAperiodicDisturbance;
        const auto res = control::tune(rule, type, in.num("ks"), in.num("tu"), in.num("tg"));
        if (!res) throw CalcError({"Für PD-Regler nennt die CHR-Tabelle keine Werte – Ziegler/Nichols wählen.",
                                   "The CHR table has no PD values – choose Ziegler/Nichols."});
        r.set("kp", res->kp);
        r.set("xp", 100.0 / res->kp);
        if (!std::isnan(res->tn)) r.set("tn", res->tn);
        if (!std::isnan(res->tv)) r.set("tv", res->tv);
        if (S > 0.3) {
            r.warn({"Schwierige Strecke: Totzeit verkleinern (Fühler näher am Stellglied) oder Kaskadenregelung prüfen.",
                    "Hard process: reduce dead time (sensor closer to the actuator) or consider cascade control."});
        }
        if (rule == Rule::ZieglerNichols) {
            r.info({"Ziegler/Nichols gilt für die Einspeicherstrecke mit Totzeit (Tt, T); bei Mehrspeicherstrecken CHR verwenden.",
                    "Ziegler/Nichols applies to single-capacity processes with dead time (Tt, T); use CHR for multi-capacity processes."});
        }
        r.info({"Einstellwerte sind Startwerte – an der Anlage nachoptimieren.", "Settings are starting values – fine-tune on site."});
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ KNX topology

Calculator knxTopology() {
    Calculator c;
    c.id = "knx_topology";
    c.category = "automation";
    c.title = {"KNX-Topologie & Linienplanung", "KNX topology & line planning"};
    c.summary = {"Linien, Bereiche, Segmente, Leitungslängen und Netzteillast (KNX TP)", "Lines, areas, segments, cable lengths and PSU load (KNX TP)"};
    c.description = {
        "KNX TP: bis 64 Teilnehmer je Liniensegment mit eigener Spannungsversorgung; mit Linienverstärkern bis 4 Segmente "
        "= 255 Teilnehmer je Linie; bis 15 Linien je Bereich (Linienkoppler), bis 15 Bereiche (Bereichskoppler); Koppler "
        "zählen auf der Haupt-/Bereichslinie mit.\n"
        "Leitungslängen je Segment: Spannungsversorgung – Teilnehmer ≤ 350 m, Teilnehmer – Teilnehmer ≤ 700 m, Gesamtlänge ≤ 1000 m.\n"
        "Praxis: neue Linie ab ca. 64 Teilnehmern und Reserve einplanen.\n"
        "Quelle: KNX Association „Grundlagenwissen zum KNX Standard“ (2013), S. 8–9. Stromaufnahme je Gerät laut Hersteller.",
        "KNX TP: up to 64 devices per line segment with its own power supply; with line repeaters up to 4 segments = 255 "
        "devices per line; up to 15 lines per area (line couplers), up to 15 areas (area couplers); couplers count on the "
        "main/backbone line.\n"
        "Cable lengths per segment: power supply – device ≤ 350 m, device – device ≤ 700 m, total ≤ 1000 m.\n"
        "Practice: start a new line from about 64 devices and plan spare capacity.\n"
        "Source: KNX Association “KNX basic knowledge” (2013), p. 8–9. Current per device from the manufacturer."};
    c.inputs = {
        Num::number("n", {"Anzahl Busteilnehmer", "Number of bus devices"}, "", 150).positive(),
        Num::number("reserve", {"Reserve", "Spare capacity"}, "%", 20).range(0, 100),
        Num::choice("per", {"Teilnehmer je Linie (Planung)", "Devices per line (planning)"},
                    {{"64", {"64 – ohne Linienverstärker", "64 – no line repeater"}},
                     {"128", {"128 – 1 Linienverstärker", "128 – 1 line repeater"}},
                     {"192", {"192 – 2 Linienverstärker", "192 – 2 line repeaters"}},
                     {"255", {"255 – 3 Linienverstärker", "255 – 3 line repeaters"}}},
                    "64"),
        Num::number("lenTot", {"Leitungslänge je Segment (gesamt)", "Cable length per segment (total)"}, "m", kNaN)
            .positive().opt().startsSection({"Prüfung Segment", "Segment check"}),
        Num::number("lenPsu", {"Größter Abstand Netzteil – Teilnehmer", "Largest distance PSU – device"}, "m", kNaN).positive().opt(),
        Num::number("lenDev", {"Größter Abstand Teilnehmer – Teilnehmer", "Largest distance device – device"}, "m", kNaN).positive().opt(),
        Num::number("iDev", {"Stromaufnahme je Teilnehmer", "Current per device"}, "mA", 10).positive()
            .withHint({"Herstellerangabe; viele Geräte 5 … 12 mA", "Manufacturer data; many devices 5 … 12 mA"}),
        Num::choice("psu", {"Spannungsversorgung", "Power supply"},
                    {{"160", "160 mA"}, {"320", "320 mA"}, {"640", "640 mA"}, {"1280", "1280 mA"}}, "640"),
    };
    c.outputs = {Out::number("lines", {"Linien", "Lines"}, "", 0).primary(),
                 Out::number("areas", {"Bereiche", "Areas"}, "", 0).primary(),
                 Out::number("perLine", {"Teilnehmer je Linie (max.)", "Devices per line (max.)"}, "", 0),
                 Out::number("segs", {"Segmente je Linie", "Segments per line"}, "", 0),
                 Out::number("psus", {"Spannungsversorgungen (Linien)", "Power supplies (lines)"}, "", 0),
                 Out::number("load", {"Netzteilauslastung je Segment", "PSU load per segment"}, "%", 0)};
    c.keywords = {"knx", "eib", "linie", "bereich", "linienkoppler", "linienverstärker", "topologie", "busteilnehmer",
                  "spannungsversorgung", "physikalische adresse"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const int need = static_cast<int>(std::ceil(in.num("n") * (1.0 + in.num("reserve") / 100.0)));
        const int per = static_cast<int>(parseNumber(in.choice("per")).value_or(64.0));
        const auto t = knx::plan(need, per);
        r.set("lines", t.lines);
        r.set("areas", t.areas);
        r.set("perLine", t.devicesPerLine);
        r.set("segs", t.segmentsPerLine);
        r.set("psus", t.lines * t.segmentsPerLine);
        const int perSeg = std::min(knx::kDevicesPerSegment, t.devicesPerLine);
        const double psu = parseNumber(in.choice("psu")).value_or(640.0);
        const double load = 100.0 * perSeg * in.num("iDev") / psu;
        r.set("load", load);
        if (t.areas > knx::kAreas) r.error({"Mehr als 15 Bereiche – KNX IP als Backbone und Projekt teilen.", "More than 15 areas – use KNX IP backbone and split the project."});
        if (t.lines > 1) r.info({"Linien über Linienkoppler an die Hauptlinie (Koppler zählen dort als Teilnehmer).",
                                 "Lines connect to the main line via line couplers (couplers count as devices there)."});
        if (load > 100) r.error({"Netzteil überlastet – größeres Netzteil oder weniger Teilnehmer je Segment.", "Power supply overloaded – larger PSU or fewer devices per segment."});
        else if (load > 80) r.warn({"Netzteil über 80 % ausgelastet.", "Power supply above 80 % load."});
        if (const auto v = in.maybe("lenTot"); v && *v > knx::kSegmentLength) r.error({"Gesamtleitungslänge > 1000 m je Segment.", "Total cable length > 1000 m per segment."});
        if (const auto v = in.maybe("lenPsu"); v && *v > knx::kPsuToDevice) r.error({"Abstand Netzteil – Teilnehmer > 350 m.", "Distance PSU – device > 350 m."});
        if (const auto v = in.maybe("lenDev"); v && *v > knx::kDeviceToDevice) r.error({"Abstand Teilnehmer – Teilnehmer > 700 m.", "Distance device – device > 700 m."});
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ AMEV data point address key (GA 2005, Anhang 2.1)

struct Code {
    std::string_view code;
    std::string_view text;
};

constexpr std::array<Code, 11> kTrades{{
    {"A", "Aufzugs- und Fördertechnik"},
    {"E", "Elektrotechnische Anlagen"},
    {"G", "Gasanlagen"},
    {"H", "Heizungstechnische Anlagen"},
    {"I", "Informationstechnische und Sicherheitstechnische Anlagen"},
    {"K", "Kältetechnische Anlagen"},
    {"L", "Lufttechnische Anlagen"},
    {"N", "Nutzungsspezifische Anlagen"},
    {"S", "Sanitärtechnische Anlagen"},
    {"Y", "Gewerkeübergreifende Anlagen"},
    {"Z", "Energiemanagement/Verbrauchserfassung"},
}};
constexpr std::array<Code, 82> kPlants{{
    {"AA_", "Abluftanlage"},
    {"AFF", "Antennen-, Funk-, Fernsehanlage"},
    {"ANA", "Anlage Nachtauskühlung (Fensterlüftung)"},
    {"AUF", "Hebeanlage"},
    {"AUL", "Aufzug Lasten"},
    {"AUP", "Aufzug Personen"},
    {"AV_", "Allgemeine Stromversorgung"},
    {"AWA", "Abwasseranlage"},
    {"AWB", "Abwasserbehandlungsanlage"},
    {"AWH", "Abwasserhebeanlage"},
    {"BEA", "Beleuchtungsanlage außen"},
    {"BEI", "Beleuchtungsanlage innen"},
    {"BES", "Beleuchtung als Sicherheitsbeleuchtung"},
    {"BMA", "Brandmeldeanlage"},
    {"BNR", "Behindertennotruf"},
    {"BUS", "Übertragungsnetz, Komponenten"},
    {"CO_", "CO-Warnanlage"},
    {"DEA", "Druckerhöhungsanlage"},
    {"DLA", "Druckluftanlage"},
    {"DRA", "Druckregelanlage"},
    {"DSA", "Dosieranlage"},
    {"DV_", "Datenverarbeitung"},
    {"EFA", "Entfeuchtungsanlage"},
    {"EHA", "Enthärtungsanlage"},
    {"EIN", "Einspeisung (öffentliches Versorgungsnetz)"},
    {"ELA", "Elektroakustische Anlage"},
    {"EMA", "Einbruchmeldeanlage (auch Überfall)"},
    {"EOA", "Energieoptimierungsanlage"},
    {"ERD", "Erdungs- und Blitzschutzanlage"},
    {"ESA", "Entsalzungsanlage"},
    {"FIA", "Filteranlage"},
    {"FLA", "Feuerlöschanlage (Sprinkleranlage, Gaslöschanlage)"},
    {"FTS", "Fluchttürsysteme, -steueranlage, Rettungswegtechnik"},
    {"GAL", "Gaslagerungsanlage"},
    {"GAR", "Gasregelstrecke"},
    {"GAW", "Gaswarnanlage"},
    {"GHV", "Gebäudehauptverteilung"},
    {"GUV", "Gebäudeunterverteilung"},
    {"HAS", "Hausanschlussstation (HAST), Übergabe-, Trafostation"},
    {"HER", "Be-Heizung (Einläufe, Rinnen, Rohre)"},
    {"HKR", "Heizkreis"},
    {"ISP", "Informationsschwerpunkt"},
    {"KEA", "Kälteerzeugungsanlage"},
    {"KTA", "Küchentechnische Anlage"},
    {"KVA", "Kälteverteilanlage"},
    {"LFA", "Leichtflüssigkeitsabscheider"},
    {"LWA", "Leckwarnanlage"},
    {"MLA", "Medizin-/Labortechnische Anlage"},
    {"MMA", "Multimediaanlage"},
    {"MSA", "Mittelspannungsanlage"},
    {"MSS", "Mittelspannungsschaltanlage"},
    {"MZE", "Mess-/Zählwerterfassung, Wetterstation"},
    {"NEA", "Netzersatzanlage"},
    {"NEU", "Neutralisationsanlage"},
    {"NSS", "Niederspannungsschaltanlage"},
    {"PVA", "Photovoltaikanlage"},
    {"RBS", "Raumbedienstation"},
    {"RKA", "Rückkühlanlage"},
    {"RWA", "Rauchwärmeabzugsanlage"},
    {"SIG", "Signalanlage"},
    {"SON", "Sonnenschutz- und Verdunklungsanlage"},
    {"SRA", "Schrankenanlage"},
    {"SV_", "Sicherheitsstromversorgung"},
    {"TAA", "Tankstellen-, Tankanlage"},
    {"TEL", "Telekommunikationsanlage"},
    {"TKA", "Teilklimaanlage"},
    {"TTA", "Tür-, Toranlage"},
    {"USV", "unterbrechungsfreie Stromversorgung"},
    {"VID", "Video-, Überwachungsanlage"},
    {"VKA", "Vollklimaanlage"},
    {"WAA", "Wasseranlage"},
    {"WAU", "Wasseraufbereitungsanlage (a. Trinkwasser)"},
    {"WEA", "Wärmeerzeugungsanlage"},
    {"WED", "Wassererwärmer dezentral"},
    {"WEZ", "Wassererwärmungsanlage zentral"},
    {"WPU", "Wärmepumpe"},
    {"WRG", "Wärmerückgewinnungsanlage"},
    {"WVN", "Wärmeverteilnetz"},
    {"ZAA", "Zu- und Abluftanlage"},
    {"ZA_", "Zuluftanlage"},
    {"ZDA", "Zeitdienstanlage"},
    {"ZKA", "Zugangskontrollanlage"},
}};
constexpr std::array<Code, 128> kParts{{
    {"AB_", "Abluft"},
    {"AKT", "Aktor allgemein"},
    {"AST", "Automatisierungsstation (allgemein)"},
    {"ATR", "Antrieb (Tür, Tor, Schranke, Fenster, Jalousie)"},
    {"AUK", "Außenluftklappe"},
    {"AUM", "Aufzugsmaschine"},
    {"AUT", "Aufzugstür"},
    {"AU_", "Außenluft"},
    {"BAT", "Batterie/Akkumulatoren"},
    {"BEF", "Befeuchter"},
    {"BRE", "Brenner"},
    {"BSK", "Brandschutzklappe"},
    {"DAE", "Dacheinlauf, Dachrinne (Heizung)"},
    {"DE_", "Druckerhöhung"},
    {"DH_", "Druckhaltung"},
    {"DR_", "Druckreduzierung"},
    {"EG_", "Endgerät allgemein (auch Datenendgerät)"},
    {"EH_", "Erhitzer"},
    {"EIN", "Spannungsversorgung"},
    {"ELS", "Endlagenschalter"},
    {"ENK", "Entrauchungsklappe"},
    {"ERR", "Einzelraumregler"},
    {"FBH", "Fußbodenheizung"},
    {"FIA", "Filter Abluft"},
    {"FIG", "Filter Gas"},
    {"FIL", "Filter Luft"},
    {"FIR", "Rückspülfilter"},
    {"FIU", "Filter Außenluft"},
    {"FIW", "Filter Wasser"},
    {"FIZ", "Filter Zuluft"},
    {"FI_", "Filter allgemein"},
    {"FOK", "Fortluftklappe"},
    {"FO_", "Fortluft"},
    {"FU_", "Frequenzumrichter"},
    {"GEN", "Generator"},
    {"HKE", "Heizkessel"},
    {"INT", "intern (anlagenintern)"},
    {"ISO", "Isolationsüberwachung"},
    {"JAK", "Jalousieklappe"},
    {"KGK", "Küchenkühlgerät"},
    {"KGW", "Küchenwärmegerät"},
    {"KLK", "Kühlwasserkreislauf"},
    {"KLP", "Primärkreislauf"},
    {"KLS", "Sekundärkreislauf"},
    {"KLW", "Kaltwasserkreislauf"},
    {"KM_", "Kältemaschine/Kälteerzeuger"},
    {"KOM", "Kompressor, Verdichter"},
    {"KON", "Kondensator, Verflüssiger"},
    {"KUD", "Kühl-/Lüftungsdecke"},
    {"KUL", "Kühler, Verdampfer"},
    {"KUR", "Kühlraum/Kühlzelle/Tiefkühltruhe"},
    {"LBF", "Luftbefeuchter"},
    {"LEA", "Leuchte allgemein"},
    {"LER", "Leuchte Rettungsweg (Rettungsweg-Kennzeichenleuchte)"},
    {"LES", "Leuchte Sicherheitsbeleuchtung"},
    {"LE_", "Luft-Erhitzer"},
    {"LKE", "Luft-Kühler und -Erhitzer"},
    {"LK_", "Luft-Kühler"},
    {"LNE", "Luft-Nacherhitzer"},
    {"LNK", "Luft-Nachkühler"},
    {"LSF", "Fehlerstromschutzschalter"},
    {"LSR", "Leser"},
    {"LS_", "Leistungsschalter (allgemein)"},
    {"LTS", "Lasttrennschalter"},
    {"LVE", "Luft-Vorerhitzer"},
    {"LVK", "Luft-Vorkühler"},
    {"LZ_", "Leitzentrale"},
    {"MBM", "Bewegungs-/Präsenzmelder"},
    {"MBR", "Brandmelder"},
    {"MEB", "Einbruchmelder (auch Überfallmelder)"},
    {"MES", "Messeinrichtung (physikalischer Größen)"},
    {"MGM", "Gasmelder"},
    {"MI_", "Mischluft"},
    {"MOT", "Motor (allgemein)"},
    {"MRM", "Rauchmelder"},
    {"M__", "Melder allgemein"},
    {"PUE", "Erhitzerpumpe"},
    {"PUK", "Kühlwasserpumpe"},
    {"PUL", "Ladepumpe"},
    {"PUP", "Pumpe Primärkreislauf"},
    {"PUU", "Umwälzpumpe"},
    {"PUW", "Kaltwasserpumpe"},
    {"PUZ", "Zirkulationspumpe"},
    {"PU_", "Pumpe allgemein"},
    {"REG", "Regler/Controller"},
    {"RL_", "Rücklauf"},
    {"R__", "Raum(-luft)"},
    {"SCH", "Schalter"},
    {"SEN", "Sensor allgemein"},
    {"SIE", "Sicherheitseinrichtung"},
    {"SIG", "Signalisierung (optisch, akustisch)"},
    {"SIL", "Sicherungslasttrennschalter"},
    {"SI_", "Sicherung, Auslöser"},
    {"SPU", "Spannungsüberwachung (Phasenüberwachung)"},
    {"SP_", "Speicher"},
    {"TAN", "Tank/Behälter"},
    {"TL_", "Terminal/Tableau"},
    {"TRA", "Transformator"},
    {"TRO", "Trockner"},
    {"TTF", "Tür-, Tor-, Fensterkontakt"},
    {"UMK", "Umluftklappe"},
    {"UM_", "Umluft"},
    {"USS", "Überspannungsschutz, Phasenwächter"},
    {"VB_", "Ventil Bypass"},
    {"VD_", "Ventil Durchgang"},
    {"VE_", "Ventil Erhitzer"},
    {"VHK", "Ventil Heizkreis"},
    {"VK_", "Ventil Kühler"},
    {"VL_", "Vorlauf"},
    {"VNE", "Ventil Nacherhitzer"},
    {"VNK", "Ventil Nachkühler"},
    {"VRA", "Abluftventilator"},
    {"VRU", "Umluftventilator"},
    {"VRZ", "Zuluftventilator"},
    {"VR_", "Ventilator"},
    {"VSR", "Volumenstromregler"},
    {"VST", "Verstärker"},
    {"VVE", "Ventil Vorerhitzer"},
    {"VVK", "Ventil Vorkühler"},
    {"VWR", "Ventil Wärmerückgewinnung"},
    {"VWT", "Ventil Wärmetauscher"},
    {"V__", "Ventil"},
    {"WE_", "Wärmeerzeuger"},
    {"WRG", "Wärmerückgewinnung"},
    {"WT_", "Wärmetauscher"},
    {"ZTR", "Zentrale allgemein"},
    {"ZU_", "Zuluft"},
    {"Z__", "Zähler (Verbrauchszähler/Maximumzähler)"},
}};
constexpr std::array<Code, 14> kKinds{{
    {"AM", "Alarm-/Gefahrenmeldung"},
    {"BM", "Betriebsmeldung"},
    {"MW", "Messwert"},
    {"PA", "Parameter, Sollwert (-verstellung)"},
    {"SB", "Schaltbefehl"},
    {"SM", "Störmeldung"},
    {"ST", "Stellbefehl"},
    {"VA", "Virtueller Datenpunkt analog"},
    {"VD", "Virtueller Datenpunkt digital"},
    {"VS", "Virtueller Datenpunkt »Szene«"},
    {"VZ", "Virtueller Datenpunkt »Zeitprogramm«"},
    {"WM", "Wartungsmeldung"},
    {"ZP", "Zeitprogramm"},
    {"ZW", "Zählwert"},
}};
constexpr std::array<Code, 99> kDescs{{
    {"00", "Sonderfall"},
    {"01", "Aus, Zu"},
    {"02", "Ein, Auf, Stufe 1"},
    {"03", "Stufe 2"},
    {"04", "Stufe 3"},
    {"05", "Stufe 4"},
    {"06", "Zentral Aus, Zentral Zu"},
    {"07", "Zentral Ein, Zentral Auf, Hauptschaltbefehl"},
    {"08", "Anforderung"},
    {"09", "Freigabe"},
    {"10", "Betrieb"},
    {"11", "Wartung"},
    {"12", "Warnung"},
    {"13", "Alarm, Gefahr, Notruf"},
    {"14", "Not-Aus"},
    {"15", "Störung"},
    {"16", "Sammelstörung"},
    {"17", "Störung allg. elektrisch"},
    {"18", "Netzausfall"},
    {"19", "Ausfall Hilfsenergie"},
    {"20", "Schaltzustandsfehler"},
    {"21", "SV-Betrieb"},
    {"22", "Störung allg. mechanisch"},
    {"23", "Keilriemen"},
    {"24", "Stellantrieb"},
    {"25", "Auslösung (Melder)"},
    {"26", "Scharfschaltung"},
    {"27", "Quittierung"},
    {"28", "Hand/Auto(-Umschaltung)"},
    {"29", "Anlagenschalter"},
    {"30", "Reparaturschalter"},
    {"31", "Endlage »Zu«"},
    {"32", "Endlage »Auf«"},
    {"33", "Regelantrieb"},
    {"34", "Regelantrieb (Rückmeldung)"},
    {"35", "Sollwert (berechnet)"},
    {"36", "Sollwertanhebung, Sollwertsteller"},
    {"37", "Hysterese"},
    {"38", "Krümmung (Heizkurve)"},
    {"39", "Steigung (Heizkurve)"},
    {"40", "Sicherheitstemperaturwächter (STW)"},
    {"41", "Druckwächter"},
    {"42", "Durchflusswächter"},
    {"43", "Frostschutzwächter"},
    {"44", "Feuchte-/Kondensations-Leckagewächter"},
    {"45", "Max-Wächter"},
    {"46", "Sicherheitstemperaturbegrenzer (STB)"},
    {"47", "Sicherheitsdruckbegrenzer (SDB)"},
    {"48", "Durchflussbegrenzer"},
    {"49", "Temperatur"},
    {"50", "Temperatur min./unten"},
    {"51", "Temperatur max./oben"},
    {"52", "Temperatur Mittelwert/Mitte"},
    {"53", "Vorlauftemperatur"},
    {"54", "Rücklauftemperatur"},
    {"55", "Luftqualität"},
    {"56", "Enthalpie"},
    {"57", "Feuchte"},
    {"58", "Druck"},
    {"59", "Druck max., Hochdruck"},
    {"60", "Druck min., Niederdruck"},
    {"61", "Differenzdruck"},
    {"62", "Volumenstrom"},
    {"63", "Durchfluss/Menge"},
    {"64", "Durchfluss max./Menge max."},
    {"65", "Durchfluss min./Menge min."},
    {"66", "pH-Wert"},
    {"67", "Drehzahl (Geschwindigkeit)"},
    {"68", "Schlupf"},
    {"69", "Wassermangel"},
    {"70", "Füllstand"},
    {"71", "Füllstand min."},
    {"72", "Füllstand max."},
    {"73", "Wind"},
    {"74", "Windrichtung"},
    {"75", "Niederschlag, Eis"},
    {"76", "Helligkeit innen/außen"},
    {"77", "Spannung"},
    {"78", "Spannung min. (auch Unterspannung)"},
    {"79", "Spannung max."},
    {"80", "Strom"},
    {"81", "Strom min."},
    {"82", "Strom max."},
    {"83", "Leistung"},
    {"84", "Leistung max. (1/4 h Leistung)"},
    {"85", "Blindleistungswert cos ϕ"},
    {"86", "Zeit (Laufzeit, Betriebsstunden)"},
    {"87", "elektrische Arbeit, elektrische Arbeit HT"},
    {"88", "elektrische Arbeit NT"},
    {"89", "Wärmemenge"},
    {"90", "Gasmenge"},
    {"91", "Ölmenge"},
    {"92", "Dampfmenge"},
    {"93", "Kaltwasser"},
    {"94", "Warmwasser"},
    {"95", "Fahrtenanzahl"},
    {"96", "Haltestelle"},
    {"97", "Feuerwehrsteuerung, Sonder-(fahrt)steuerung"},
    {"98", "Szenenaufruf"},
}};

template <std::size_t N>
std::vector<Option> codeOptions(const std::array<Code, N>& list) {
    std::vector<Option> o;
    o.reserve(N);
    for (const auto& c : list) {
        const std::string label = std::string(c.code) + " – " + std::string(c.text);
        o.push_back({std::string(c.code), {label, label}});
    }
    return o;
}

template <std::size_t N>
std::string_view codeText(const std::array<Code, N>& list, const std::string& code) {
    for (const auto& c : list)
        if (c.code == code) return c.text;
    return {};
}

std::string pad(double v, int width) {
    std::string s = std::to_string(static_cast<long long>(std::llround(v)));
    while (static_cast<int>(s.size()) < width) s.insert(s.begin(), '0');
    return s;
}

const char* kAmevSource = "Quelle: AMEV „Gebäudeautomation 2005“ (Nr. 87), Anhang 2.1/2.2, S. 78–85. Bezeichnungen wie im Original "
                          "(nur deutsch); projektspezifisch anpassbar, neuere AMEV-Empfehlungen (z. B. BACnet) prüfen.";
const char* kAmevSourceEn = "Source: AMEV “Gebäudeautomation 2005” (no. 87), appendix 2.1/2.2, p. 78–85. Terms as in the original "
                            "(German only); adaptable per project, check newer AMEV recommendations (e.g. BACnet).";

Calculator amevAddress() {
    Calculator c;
    c.id = "amev_address";
    c.category = "automation";
    c.title = {"AMEV-Datenpunktadresse", "AMEV data point address"};
    c.summary = {"Benutzeradresse nach AMEV GA 2005 aus Gewerk, Anlage, Anlagenteil, Datenpunkt", "User address per AMEV GA 2005 from trade, plant, component, data point"};
    c.description = {std::string("Aufbau (25 Stellen + Klartext): Liegenschaft (5) – Gebäude (3) – Gewerk (1) – Anlage (3) – lfd. Nr. "
                                 "(2) – Regelfabrikat (2) – Anlagenteil (3) – lfd. Nr. (2) – Datenpunktart (2) – Datenpunktbeschreibung (2).\n"
                                 "Beispiel: 31021-008-H-HKR-01-01-PUU-01-SM-17 = Heizkreis 01, Umwälzpumpe 01, Störmeldung "
                                 "„Störung allg. elektrisch“.\n") + kAmevSource,
                     std::string("Structure (25 characters + plain text): site (5) – building (3) – trade (1) – plant (3) – no. (2) – "
                                 "controller make (2) – component (3) – no. (2) – point type (2) – point description (2).\n"
                                 "Example: 31021-008-H-HKR-01-01-PUU-01-SM-17 = heating circuit 01, circulation pump 01, fault "
                                 "message “general electrical fault”.\n") + kAmevSourceEn};
    c.inputs = {
        Num::number("site", {"Liegenschaftsnummer", "Site number"}, "", 31021).range(0, 99999),
        Num::number("bldg", {"Gebäudenummer (999 = Außenanlagen)", "Building no. (999 = outdoor)"}, "", 8).range(0, 999),
        Num::choice("trade", {"Gewerk", "Trade"}, codeOptions(kTrades), "H"),
        Num::choice("plant", {"Anlage", "Plant"}, codeOptions(kPlants), "HKR"),
        Num::number("plantNo", {"Lfd. Nr. der Anlage", "Plant no."}, "", 1).range(0, 99),
        Num::number("make", {"Regelfabrikat (Nr.)", "Controller make (no.)"}, "", 1).range(0, 99),
        Num::choice("part", {"Anlagenteil", "Component"}, codeOptions(kParts), "PUU"),
        Num::number("partNo", {"Lfd. Nr. des Anlagenteils", "Component no."}, "", 1).range(0, 99),
        Num::choice("kind", {"Datenpunktart", "Point type"}, codeOptions(kKinds), "SM"),
        Num::choice("desc", {"Datenpunktbeschreibung", "Point description"}, codeOptions(kDescs), "17"),
    };
    c.outputs = {Out::text("addr", {"Datenpunktadresse", "Data point address"}).primary().abbrev({"Adresse", "Address"}),
                 Out::text("compact", {"Ohne Trennzeichen (GLT)", "Without separators (BMS)"}),
                 Out::text("plain", {"Klartext (Vorschlag, max. 30 Zeichen)", "Plain text (suggestion, max. 30 characters)"})};
    c.keywords = {"amev", "datenpunkt", "adressierung", "anlagenkennzeichnung", "aks", "benutzeradresse", "gebäudeautomation 2005",
                  "glt", "kennzeichnung"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const std::string addr = pad(in.num("site"), 5) + "-" + pad(in.num("bldg"), 3) + "-" + in.choice("trade") + "-" +
                                 in.choice("plant") + "-" + pad(in.num("plantNo"), 2) + "-" + pad(in.num("make"), 2) + "-" +
                                 in.choice("part") + "-" + pad(in.num("partNo"), 2) + "-" + in.choice("kind") + "-" +
                                 in.choice("desc");
        std::string compact;
        for (const char ch : addr)
            if (ch != '-') compact += ch;
        r.setText("addr", addr);
        r.setText("compact", compact);
        std::string plain = std::string(codeText(kParts, in.choice("part"))) + " " + pad(in.num("partNo"), 2) + " " +
                            std::string(codeText(kDescs, in.choice("desc")));
        // cut at 30 characters (UTF-8 aware: never split a multi-byte sequence)
        std::size_t chars = 0, pos = 0;
        while (pos < plain.size() && chars < 30) {
            const auto b = static_cast<unsigned char>(plain[pos]);
            pos += b < 0x80 ? 1 : (b >> 5) == 0x6 ? 2 : (b >> 4) == 0xE ? 3 : 4;
            ++chars;
        }
        if (pos < plain.size()) {
            plain.resize(pos);
            if (const auto sp = plain.find_last_of(' '); sp != std::string::npos && sp > 15) plain.resize(sp);
        }
        r.setText("plain", plain);
        r.info({"Fehlende Buchstaben dürfen durch Unterstriche ersetzt werden (z. B. PU_).", "Missing letters may be replaced by underscores (e.g. PU_)."});
        return r;
    };
    return c;
}

template <std::size_t N>
Table codeTable(const char* id, Text title, Text summary, const std::array<Code, N>& list) {
    Table t;
    t.id = id;
    t.category = "automation";
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = {kAmevSource, kAmevSourceEn};
    t.columns = {{"Kürzel", "Code"}, {"Bedeutung", "Meaning"}};
    for (const auto& c : list) t.rows.push_back({std::string(c.code), std::string(c.text)});
    return t;
}

// ------------------------------------------------------------------ h,x altitude table (Siemens h,x)

Table hxAltitude() {
    Table t;
    t.id = "hx_altitude";
    t.category = "automation";
    t.title = {"h,x-Diagramm: Höhenkorrektur", "h,x chart: altitude correction"};
    t.summary = {"Luftdruck und Umrechnungsfaktor k für φ und ρ", "Air pressure and conversion factor k for φ and ρ"};
    t.description = {"Buchwerte: Siemens „h,x-Diagramm – Aufbau und Anwendung“ (08/2022), S. 59. Spalte ISA: Normatmosphäre, wie sie "
                     "der h,x-Rechner verwendet. Bei 800 m nennt das Buch k = 0,901; aus 921/1013 mbar ergibt sich 0,909 – "
                     "vermutlich ein Druckfehler. Bei 1500 m liegt der Buchwert rund 0,5 % unter der Normatmosphäre.",
                     "Book values: Siemens “h,x-Diagramm – Aufbau und Anwendung” (08/2022), p. 59. Column ISA: standard atmosphere "
                     "as used by the h,x calculator. At 800 m the book gives k = 0.901; 921/1013 mbar gives 0.909 – probably a "
                     "misprint. At 1500 m the book value is about 0.5 % below the standard atmosphere."};
    t.columns = {{"Höhe ü. M.", "Altitude"}, {"p (Buch)", "p (book)"}, {"k (Buch)", "k (book)"}, {"p ISA", "p ISA"}, {"k ISA", "k ISA"}};
    struct Row {
        double h, p, k;
    };
    constexpr std::array<Row, 8> rows{{{0, 1013, 1.0}, {200, 989, 0.976}, {400, 966, 0.953}, {600, 943, 0.931}, {800, 921, 0.901},
                                       {1000, 899, 0.887}, {1500, 842, 0.831}, {2000, 795, 0.785}}};
    for (const auto& row : rows) {
        const double p = air::pressureAtAltitude(row.h);  // hPa = mbar
        t.rows.push_back({numText(row.h, 0, "m"), numText(row.p, 0, "mbar"), formatNumber(row.k, 3), numText(p, 0, "mbar"),
                          formatNumber(p / 1013.25, 3)});
    }
    return t;
}

// ------------------------------------------------------------------ five safety rules (Haag Band 09, DIN VDE 0105-100)

Table safetyRules() {
    Table t;
    t.id = "safety_rules";
    t.category = "tables";
    t.title = {"5 Sicherheitsregeln & Schutzabstände", "5 safety rules & safety distances"};
    t.summary = {"Arbeiten an elektrischen Anlagen (DIN VDE 0105-100)", "Work on electrical installations (DIN VDE 0105-100)"};
    t.description = {"Quelle: Haag „Grundlagen der Stromversorgung“, Band 09 Arbeitssicherheit (2007), S. 7–9 (DIN VDE 0105-100 "
                     "Tab. 101–103, Ausschnitt). Gegen die gültige DIN VDE 0105-100 und DGUV-Vorschrift 3 prüfen.",
                     "Source: Haag “Grundlagen der Stromversorgung”, vol. 09 occupational safety (2007), p. 7–9 (DIN VDE 0105-100 "
                     "tab. 101–103, extract). Check against DIN VDE 0105-100 and DGUV regulation 3 in force."};
    t.columns = {{"Thema", "Topic"}, {"Wert / Regel", "Value / rule"}};
    t.rows = {
        {{"Regel 1", "Rule 1"}, {"Freischalten", "Disconnect completely"}},
        {{"Regel 2", "Rule 2"}, {"Gegen Wiedereinschalten sichern", "Secure against reconnection"}},
        {{"Regel 3", "Rule 3"}, {"Spannungsfreiheit feststellen (allpolig)", "Verify absence of voltage (all poles)"}},
        {{"Regel 4", "Rule 4"}, {"Erden und kurzschließen", "Earth and short-circuit"}},
        {{"Regel 5", "Rule 5"}, {"Benachbarte, unter Spannung stehende Teile abdecken oder abschranken", "Cover or fence off adjacent live parts"}},
        {{"Schutzabstand Elektrofachkraft ≤ 1 kV", "Safety distance skilled person ≤ 1 kV"}, "0,5 m"},
        {{"Schutzabstand Elektrofachkraft 1 … 30 kV", "Safety distance skilled person 1 … 30 kV"}, "1,5 m"},
        {{"Schutzabstand Elektrofachkraft 30 … 110 kV", "Safety distance skilled person 30 … 110 kV"}, "2,0 m"},
        {{"Schutzabstand Elektrofachkraft 110 … 220 kV", "Safety distance skilled person 110 … 220 kV"}, "3,0 m"},
        {{"Schutzabstand Elektrofachkraft 220 … 380 kV", "Safety distance skilled person 220 … 380 kV"}, "4,0 m"},
        {{"Bauarbeiten/Laien ≤ 1 kV", "Construction/lay persons ≤ 1 kV"}, "1,0 m"},
        {{"Bauarbeiten/Laien 1 … 110 kV", "Construction/lay persons 1 … 110 kV"}, "3,0 m"},
        {{"Bauarbeiten/Laien 110 … 220 kV", "Construction/lay persons 110 … 220 kV"}, "4,0 m"},
        {{"Bauarbeiten/Laien 220 … 380 kV", "Construction/lay persons 220 … 380 kV"}, "5,0 m"},
        {{"Gefahrenzone ≤ 1 kV", "Danger zone ≤ 1 kV"}, {"Abdecken (keine Berührung)", "Cover (no contact)"}},
        {{"Gefahrenzone 10 kV innen / 20 kV / 110 kV", "Danger zone 10 kV indoor / 20 kV / 110 kV"}, "120 / 220 / 1100 mm"},
    };
    return t;
}

}  // namespace

void addControls(std::vector<Calculator>& out) {
    out.push_back(controllerTuning());
    out.push_back(knxTopology());
    out.push_back(amevAddress());
}

void addControlTables(std::vector<Table>& out) {
    out.push_back(hxAltitude());
    out.push_back(codeTable("amev_trades", {"AMEV: Gewerke", "AMEV: trades"}, {"Stelle 9 der Datenpunktadresse", "Position 9 of the address"}, kTrades));
    out.push_back(codeTable("amev_plants", {"AMEV: Anlagen", "AMEV: plants"}, {"Stellen 10–12", "Positions 10–12"}, kPlants));
    out.push_back(codeTable("amev_parts", {"AMEV: Anlagenteile", "AMEV: components"}, {"Stellen 17–19", "Positions 17–19"}, kParts));
    out.push_back(codeTable("amev_point_types", {"AMEV: Datenpunktarten", "AMEV: point types"}, {"Stellen 22–23", "Positions 22–23"}, kKinds));
    out.push_back(codeTable("amev_point_desc", {"AMEV: Datenpunktbeschreibung", "AMEV: point descriptions"}, {"Stellen 24–25", "Positions 24–25"}, kDescs));
    out.push_back(safetyRules());
}

}  // namespace elc::catalog
