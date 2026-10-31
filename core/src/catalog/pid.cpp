// P&ID identification letters: tag finder for sensors and switches, letter tables.
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/instruments.hpp"

namespace elc::catalog {

namespace {

using instruments::Function;

const Text kPidSource{
    "Kennbuchstaben nach ANSI/ISA-5.1-2009 und DIN EN 62424 (Nachfolgerin von DIN 19227-1); GA-Kürzel nach VDI 3814 Blatt 4.1 "
    "Tab. 3/4 (Beispiele), Klassen nach IEC 81346-2 wie in VDI 3814-4.1. Projektspezifische Festlegungen haben Vorrang.",
    "Letters per ANSI/ISA-5.1-2009 and EN 62424 (successor of DIN 19227-1); BACS codes per VDI 3814 Part 4.1 Tab. 3/4 "
    "(examples), classes per IEC 81346-2 as listed in VDI 3814-4.1. Project conventions take precedence."};

std::vector<Option> functionOptions() {
    return {{"auto", {"automatisch (typisch für das Gerät)", "automatic (typical for the device)"}},
            {"element", {"Fühler / Messelement", "Sensor / primary element"}},
            {"transmitter", {"Messumformer (Signal an GLT/PLS)", "Transmitter (signal to BMS/DCS)"}},
            {"indicator", {"Anzeige vor Ort (Thermometer, Manometer)", "Local indicator (thermometer, gauge)"}},
            {"controller", {"Regler mit Anzeige", "Indicating controller"}},
            {"switchH", {"Wächter / Schalter Max", "Switch high"}},
            {"switchL", {"Wächter / Schalter Min", "Switch low"}},
            {"alarmH", {"Alarm hoch", "Alarm high"}},
            {"alarmL", {"Alarm tief", "Alarm low"}},
            {"safety", {"Sicherheitsabschaltung / Begrenzer", "Safety trip / limiter"}},
            {"totalizer", {"Zähler / Summierung", "Meter / totalizer"}}};
}

Function functionOf(const std::string& v) {
    if (v == "element") return Function::Element;
    if (v == "transmitter") return Function::Transmitter;
    if (v == "indicator") return Function::Indicator;
    if (v == "controller") return Function::Controller;
    if (v == "switchH") return Function::SwitchHigh;
    if (v == "switchL") return Function::SwitchLow;
    if (v == "alarmH") return Function::AlarmHigh;
    if (v == "alarmL") return Function::AlarmLow;
    if (v == "safety") return Function::Safety;
    if (v == "totalizer") return Function::Totalizer;
    return Function::Auto;
}

bool binary(Function f) { return f != Function::Element && f != Function::Transmitter && f != Function::Indicator && f != Function::Controller && f != Function::Totalizer; }

Text signalText(const instruments::Entry& e, Function f) {
    if (f == Function::Indicator) return {"– (nur Anzeige vor Ort)", "– (local display only)"};
    if (f == Function::Totalizer) return {"Zählimpuls (BI) oder Bus (M-Bus, Modbus)", "Pulse (BI) or bus (M-Bus, Modbus)"};
    if (binary(f)) {
        return {"BI – potentialfreier Kontakt; Störung/Gefahr als Öffner (VDI 3814-3.1)",
                "BI – volt-free contact; fault/danger as NC contact (VDI 3814-3.1)"};
    }
    if (e.isa == "A") return {"AI 4–20 mA (bzw. Relais je Alarmstufe)", "AI 4–20 mA (or relay per alarm level)"};
    if (e.isa == "T" && f == Function::Element) return {"AI passiv (Pt1000, Ni1000, NTC)", "AI passive (Pt1000, Ni1000, NTC)"};
    return {"AI (0–10 V oder 4–20 mA) bzw. Bus", "AI (0–10 V or 4–20 mA) or bus"};
}

Text symbolText(const std::string& loc) {
    if (loc == "central") {
        return {"ISA: Kreis im Quadrat mit Querstrich (Funktion im Leitsystem/GLT, Bildschirm); SPS/DDC-Funktion: Raute im Quadrat. "
                "EN 62424: Oval mit Strich (Leitwarte).",
                "ISA: circle in square with line (shared display/control in DCS/BMS); PLC/DDC function: diamond in square. "
                "EN 62424: oval with line (control room)."};
    }
    if (loc == "panel") {
        return {"ISA: Kreis mit doppeltem Querstrich (Hilfsstandort, z. B. Schaltschrank vor Ort, zugänglich).",
                "ISA: circle with double line (auxiliary location, e.g. local panel, accessible)."};
    }
    return {"ISA: Kreis ohne Querstrich (im Feld montiert). EN 62424: Oval ohne Strich (vor Ort).",
            "ISA: circle without line (field mounted). EN 62424: oval without line (local)."};
}

Calculator pidTags() {
    Calculator c;
    c.id = "pid_tags";
    c.category = "process";
    c.title = {"R&I-Kennbuchstaben-Finder", "P&ID tag finder"};
    c.summary = {"Name eingeben (z. B. Temperatur Zuluft, Gas Methan, Leckage) → ISA, EN 62424, VDI 3814",
                 "Enter a name (e.g. supply air temperature, methane, leakage) → ISA, EN 62424, VDI 3814"};
    c.description = cat(
        Text{"Erster Buchstabe = Messgröße (T Temperatur, P Druck, F Durchfluss, L Stand, M Feuchte, A/Q Analyse …), "
             "Folgebuchstaben = Funktion: E Element, T Transmitter, I Anzeige, C Regelung, S Schalter, A Alarm, H/L hoch/tief, "
             "Q Summe. Tag = Buchstaben + Kreisnummer, z. B. TT-101.\n"
             "Für die GA zusätzlich Bauteil-Kürzel und Benutzeradresse nach VDI 3814-4.1 (Kürzel + Nr. _ Funktion _ Nr.).\n",
             "First letter = measured variable (T temperature, P pressure, F flow, L level, M moisture, A/Q analysis …), "
             "succeeding letters = function: E element, T transmitter, I indicate, C control, S switch, A alarm, H/L high/low, "
             "Q totalize. Tag = letters + loop number, e.g. TT-101.\n"
             "For BACS also the component code and user address per VDI 3814-4.1 (code + no. _ function _ no.).\n"},
        kPidSource);
    c.inputs = {
        Num::text("name", {"Messstelle / Gerät (Name)", "Measuring point / device (name)"}, "Temperatur Zuluft")
            .withHint({"z. B. Feuchte, Gas Methan, Leckage, Druckwächter min", "e.g. humidity, methane, leakage, pressure switch low"}),
        Num::choice("func", {"Funktion", "Function"}, functionOptions(), "auto"),
        Num::choice("loc", {"Ort / Darstellung", "Location / symbol"},
                    {{"field", {"Feld (vor Ort montiert)", "Field mounted"}},
                     {"central", {"Leitsystem / GLT (Bildschirm)", "DCS / BMS (screen)"}},
                     {"panel", {"Schaltschrank / Bedienpult vor Ort", "Local panel"}}},
                    "field"),
        Num::text("loop", {"Kreisnummer", "Loop number"}, "101"),
    };
    c.outputs = {
        Out::text("match", {"Erkannt", "Recognised"}),
        Out::text("isa", {"Kennbuchstaben ISA-5.1", "Letters ISA-5.1"}).primary().abbrev({"ISA", "ISA"}),
        Out::text("tag", {"Beispiel-Tag (ISA)", "Example tag (ISA)"}).primary().abbrev({"Tag", "Tag"}),
        Out::text("meaning", {"Bedeutung", "Meaning"}),
        Out::text("en", {"Kennbuchstaben EN 62424 / DIN 19227", "Letters EN 62424 / DIN 19227"}),
        Out::text("vdi", {"GA-Kürzel VDI 3814-4.1", "BACS code VDI 3814-4.1"}),
        Out::text("bas", {"Benutzeradresse (Bauteil_Funktion)", "User address (component_function)"}),
        Out::text("iec", {"Klasse IEC 81346-2", "Class IEC 81346-2"}),
        Out::text("signal", {"Signal", "Signal"}),
        Out::text("symbol", {"PI-Symbol", "P&ID symbol"}),
        Out::text("alts", {"Weitere Treffer", "Other matches"}),
    };
    c.keywords = {"r&i", "ri", "pid", "p&id", "kennbuchstaben", "kennzeichnung", "tag", "isa", "62424", "19227", "fühler", "wächter",
                  "sensor", "messstelle", "abkürzung", "kürzel", "symbol"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& text = in.text("name");
        const auto matches = instruments::find(text);
        if (matches.empty()) {
            throw CalcError({"Kein passender Sensor/Wächter gefunden – anderes Wort versuchen (z. B. Druck, Feuchte, Gas, Zähler).",
                             "No matching sensor/switch found – try another word (e.g. pressure, humidity, gas, meter)."});
        }
        const auto& e = *matches.front().entry;
        const Function f = instruments::resolve(e, functionOf(in.choice("func")), text);
        const std::string isa = instruments::isaCode(e, f);
        std::string loop = in.text("loop");
        if (loop.empty()) loop = "101";
        const std::string ann = e.annotation.empty() ? std::string() : " (" + e.annotation + ")";
        r.setText("match", Text{e.name.de + " · " + e.domain.de, e.name.en + " · " + e.domain.en});
        r.setText("isa", Text{isa});
        r.setText("tag", Text{isa + "-" + loop + ann});
        r.setText("meaning", instruments::isaMeaning(e, f));
        r.setText("en", Text{instruments::enCode(e, f) + ann});
        const bool single = !e.vdi.empty() && e.vdi.find(',') == std::string::npos;
        if (e.vdi.empty()) {
            r.setText("vdi", {"– (kein Kürzel in VDI 3814-4.1 Tab. 3, projektspezifisch festlegen)",
                              "– (no code in VDI 3814-4.1 Tab. 3, define per project)"});
        } else {
            r.setText("vdi", Text{e.vdi + (single ? "" : " (je nach Ort)"), e.vdi + (single ? "" : " (depending on location)")});
        }
        const std::string fnCode = binary(f) ? (e.vdiFn.empty() ? "SM_" : e.vdiFn) : (f == Function::Totalizer ? "ZW_" : "MW_");
        if (single && fnCode.find('/') == std::string::npos) {
            r.setText("bas", Text{e.vdi + "01_" + fnCode + "01"});
        } else {
            r.setText("bas", Text{"<Kürzel>01_" + fnCode + "01", "<code>01_" + fnCode + "01"});
        }
        r.setText("iec", Text{e.iec.empty() ? std::string("–") : e.iec});
        r.setText("signal", signalText(e, f));
        r.setText("symbol", symbolText(in.choice("loc")));
        if (matches.size() > 1) {
            Text alts{"", ""};
            for (std::size_t i = 1; i < matches.size() && i < 6; ++i) {
                alts.de += (i > 1 ? ", " : "") + matches[i].entry->name.de;
                alts.en += (i > 1 ? ", " : "") + matches[i].entry->name.en;
            }
            r.setText("alts", alts);
        }
        if (!e.note.de.empty()) r.info(e.note);
        return r;
    };
    return c;
}

Table makeTable(const char* id, Text title, Text summary, Text description, std::vector<Text> columns, std::vector<std::vector<Text>> rows) {
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

}  // namespace

void addPid(std::vector<Calculator>& out) { out.push_back(pidTags()); }

void addPidTables(std::vector<Table>& out) {
    {
        std::vector<std::vector<Text>> rows;
        for (const auto& e : instruments::all()) {
            const std::string ann = e.annotation.empty() ? std::string() : " (" + e.annotation + ")";
            rows.push_back({e.name, e.domain, Text{instruments::isaCode(e, e.fn) + ann}, Text{instruments::enCode(e, e.fn)},
                            Text{e.vdi.empty() ? std::string("–") : e.vdi}, Text{e.iec.empty() ? std::string("–") : e.iec}});
        }
        out.push_back(makeTable("pid_instruments", {"Sensoren & Wächter – Kennbuchstaben", "Sensors & switches – letter codes"},
                                {"HLKS, MSR, Prozess: ISA-5.1, EN 62424, VDI 3814, IEC 81346", "HVAC, I&C, process: ISA-5.1, EN 62424, VDI 3814, IEC 81346"},
                                cat(Text{"Typische Funktion je Gerät; andere Funktion (Fühler, Anzeige, Regler, Max/Min) im Kennbuchstaben-Finder wählen.\n",
                                         "Typical function per device; choose another function (element, indicator, controller, high/low) in the tag finder.\n"},
                                    kPidSource),
                                {{"Messstelle / Gerät", "Measuring point / device"}, {"Bereich", "Area"}, {"ISA-5.1", "ISA-5.1"},
                                 {"EN 62424", "EN 62424"}, {"VDI 3814", "VDI 3814"}, {"IEC 81346", "IEC 81346"}},
                                std::move(rows)));
    }
    out.push_back(makeTable(
        "pid_letters_isa", {"Kennbuchstaben ISA-5.1", "Identification letters ISA-5.1"},
        {"Erstbuchstabe, Ergänzung, Folgebuchstaben A–Z", "First letter, modifier, succeeding letters A–Z"},
        Text{"ANSI/ISA-5.1-2009 Tab. 4.1 (sinngemäß). „frei“ = vom Anwender festzulegen. Beispiel: TIC-101 = Temperatur, Anzeige, Regelung, Kreis 101.",
             "ANSI/ISA-5.1-2009 Tab. 4.1 (paraphrased). “user” = user's choice. Example: TIC-101 = temperature, indicate, control, loop 101."},
        {{"Buchstabe", "Letter"}, {"Messgröße (1. Buchstabe)", "Measured variable (1st letter)"}, {"Ergänzung", "Modifier"},
         {"Anzeige-/passive Funktion", "Readout/passive function"}, {"Ausgabefunktion", "Output function"}, {"Funktionszusatz", "Function modifier"}},
        {{"A", {"Analyse", "Analysis"}, "–", {"Alarm", "Alarm"}, "–", "–"},
         {"B", {"Brenner, Verbrennung", "Burner, combustion"}, "–", {"frei", "user"}, {"frei", "user"}, {"frei", "user"}},
         {"C", {"frei (oft Leitfähigkeit)", "user (often conductivity)"}, "–", "–", {"Regelung", "Control"}, {"Schließen", "Close"}},
         {"D", {"frei (oft Dichte)", "user (often density)"}, {"Differenz", "Differential"}, "–", "–", {"Abweichung", "Deviation"}},
         {"E", {"Spannung", "Voltage"}, "–", {"Messelement, Sensor", "Sensor, primary element"}, "–", "–"},
         {"F", {"Durchfluss", "Flow"}, {"Verhältnis", "Ratio"}, "–", "–", "–"},
         {"G", {"frei", "user"}, "–", {"Schauglas, Anzeiger", "Glass, gauge"}, "–", "–"},
         {"H", {"Hand", "Hand"}, "–", "–", "–", {"Hoch", "High"}},
         {"I", {"Strom", "Current"}, "–", {"Anzeige", "Indicate"}, "–", "–"},
         {"J", {"Leistung", "Power"}, "–", {"Abfrage", "Scan"}, "–", "–"},
         {"K", {"Zeit, Zeitplan", "Time, schedule"}, {"Änderungsgeschwindigkeit", "Rate of change"}, "–", {"Bedienstation", "Control station"}, "–"},
         {"L", {"Stand, Niveau", "Level"}, "–", {"Leuchte", "Light"}, "–", {"Tief", "Low"}},
         {"M", {"frei (oft Feuchte)", "user (often moisture)"}, "–", "–", "–", {"Mittel", "Middle"}},
         {"N", {"frei", "user"}, "–", {"frei", "user"}, {"frei", "user"}, {"frei", "user"}},
         {"O", {"frei", "user"}, "–", {"Blende, Drossel", "Orifice, restriction"}, "–", {"Öffnen", "Open"}},
         {"P", {"Druck", "Pressure"}, "–", {"Messanschluss", "Test point"}, "–", "–"},
         {"Q", {"Menge", "Quantity"}, {"Integral, Summe", "Integrate, totalize"}, {"Integral, Summe", "Integrate, totalize"}, "–", "–"},
         {"R", {"Strahlung", "Radiation"}, "–", {"Registrierung", "Record"}, "–", {"Lauf", "Run"}},
         {"S", {"Drehzahl, Frequenz", "Speed, frequency"}, {"Sicherheit", "Safety"}, "–", {"Schalter", "Switch"}, {"Stopp", "Stop"}},
         {"T", {"Temperatur", "Temperature"}, "–", "–", {"Transmitter", "Transmit"}, "–"},
         {"U", {"Mehrere Größen", "Multivariable"}, "–", {"Mehrfachfunktion", "Multifunction"}, {"Mehrfachfunktion", "Multifunction"}, "–"},
         {"V", {"Schwingung", "Vibration"}, "–", "–", {"Ventil, Klappe", "Valve, damper"}, "–"},
         {"W", {"Gewicht, Kraft", "Weight, force"}, "–", {"Tauchhülse, Sonde", "Well, probe"}, "–", "–"},
         {"X", {"nicht klassifiziert", "Unclassified"}, {"X-Achse", "X axis"}, {"nicht klassifiziert", "Unclassified"}, {"nicht klassifiziert", "Unclassified"}, "–"},
         {"Y", {"Ereignis, Zustand, Anwesenheit", "Event, state, presence"}, {"Y-Achse", "Y axis"}, "–", {"Hilfsgerät (Relais, Umformer)", "Auxiliary device"}, "–"},
         {"Z", {"Position, Abmessung", "Position, dimension"}, {"Z-Achse", "Z axis"}, "–", {"Antrieb, Stellglied", "Driver, actuator"}, "–"}}));
    out.push_back(makeTable(
        "pid_letters_en62424", {"Kennbuchstaben EN 62424 / DIN 19227", "Identification letters EN 62424 / DIN 19227"},
        {"PLT-Kategorie und Verarbeitungsfunktionen", "PCE category and processing functions"},
        Text{"Erstbuchstabe = Messgröße, Ergänzung D/F/Q, Folgebuchstaben in der Reihenfolge I R C S Z A O, Grenzwerte H/L (früher +/−). "
             "Beispiel: TIC = Temperatur, Anzeige, Regelung; PSZ = Druck, Schaltung, Schutzauslösung. DIN 19227-1 / DIN EN 62424, "
             "gegen gültige Ausgabe prüfen.",
             "First letter = variable, modifiers D/F/Q, succeeding letters in the order I R C S Z A O, limits H/L (formerly +/−). "
             "Example: TIC = temperature, indicate, control; PSZ = pressure, switching, safety trip. DIN 19227-1 / EN 62424, "
             "check the edition in force."},
        {{"Buchstabe", "Letter"}, {"Als Erstbuchstabe", "As first letter"}, {"Als Ergänzung/Folgebuchstabe", "As modifier/succeeding letter"}},
        {{"A", "–", {"Störungsmeldung (Alarm)", "Fault message (alarm)"}},
         {"C", "–", {"selbsttätige Regelung", "Automatic control"}},
         {"D", {"Dichte", "Density"}, {"Differenz", "Difference"}},
         {"E", {"elektrische Größen", "Electrical variables"}, "–"},
         {"F", {"Durchfluss", "Flow"}, {"Verhältnis", "Ratio"}},
         {"G", {"Abstand, Länge, Stellung, Amplitude", "Distance, length, position, amplitude"}, "–"},
         {"H", {"Handeingabe", "Manual input"}, {"oberer Grenzwert", "Upper limit"}},
         {"I", "–", {"Anzeige", "Indication"}},
         {"K", {"Zeit", "Time"}, "–"},
         {"L", {"Stand (auch Trennschicht)", "Level (also interface)"}, {"unterer Grenzwert", "Lower limit"}},
         {"M", {"Feuchte", "Moisture"}, "–"},
         {"O", "–", {"Sichtzeichen, Ja/Nein-Anzeige", "Visual signal, yes/no indication"}},
         {"P", {"Druck", "Pressure"}, "–"},
         {"Q", {"Stoffeigenschaft, Analyse", "Material property, analysis"}, {"Integral, Summe", "Integral, total"}},
         {"R", {"Strahlungsgrößen", "Radiation"}, {"Registrierung", "Recording"}},
         {"S", {"Geschwindigkeit, Drehzahl, Frequenz", "Speed, frequency"}, {"Schaltung, Ablaufsteuerung", "Switching, sequence control"}},
         {"T", {"Temperatur", "Temperature"}, {"Messumformer (Gerätebuchstabe)", "Transmitter (device letter)"}},
         {"U", {"zusammengesetzte Größen", "Combined variables"}, {"zusammengefasste Antriebsfunktionen", "Combined drive functions"}},
         {"V", {"Viskosität", "Viscosity"}, {"Stellgerät (Gerätebuchstabe)", "Final control element (device letter)"}},
         {"W", {"Gewichtskraft, Masse", "Weight, mass"}, "–"},
         {"X", {"sonstige Größen", "Other variables"}, "–"},
         {"Y", "–", {"Rechenfunktion (Gerätebuchstabe)", "Computing function (device letter)"}},
         {"Z", "–", {"Noteingriff, Schutz durch Auslösung", "Emergency action, protective trip"}}}));
}

}  // namespace elc::catalog
