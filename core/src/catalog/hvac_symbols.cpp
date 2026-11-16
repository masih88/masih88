// HVAC area (v1.12): graphical symbols of ventilation, heating, plumbing and refrigeration components and the line colours
// of building services media. Symbols are our own drawings modelled on the usual forms in DIN EN 12792 (ventilation),
// DIN EN ISO 10628-2 / DIN 2429-2 (pipework, pumps, valves), DIN EN 12828 (heating), DIN 1988-100 / DIN EN 806-1 and
// DIN 1986-100 (plumbing) and DIN EN 1861 (refrigeration). Line colours from DIN EN 16798-3 (air types), DIN 1988 /
// DIN EN 806 (drinking water), DIN 2403 via TRGS 201 annex 3 (pipeline identification) and common drawing practice.
#include <array>
#include <cmath>
#include <functional>
#include <numbers>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/sketch.hpp"

namespace elc::catalog {

namespace {

constexpr const char* kCat = "hvac";
using P = Sketch::P;

// ------------------------------------------------------------------ symbol primitives (centre x, y; size ≈ 12 units)

struct Sym : Sketch {
    void pipe(double x, double y, double half = 13) { line("ink", {{x - half, y}, {x + half, y}}); }
    void duct(double x, double y, double half = 14, double h = 5.5) {
        line("ink", {{x - half, y + h}, {x + half, y + h}});
        line("ink", {{x - half, y - h}, {x + half, y - h}});
    }
    void box(double x, double y, double w, double h) { rect("bgfill", x - w / 2, y - h / 2, x + w / 2, y + h / 2); }
    void tag(double x, double y, Text t) { label(x, y, std::move(t), "cb"); }
    void bowtie(double x, double y, const char* style, double h = 4.4, double w = 3.3) {
        closed(style, {{x - h, y - w}, {x - h, y + w}, {x, y}});
        closed(style, {{x + h, y - w}, {x + h, y + w}, {x, y}});
    }
    void pumpSym(double x, double y, double r = 5.2) {
        circle("bgfill", x, y, r);
        closed("ink", {{x - r * 0.5, y + r * 0.866}, {x - r * 0.5, y - r * 0.866}, {x + r, y}});
    }
    void zigzag(double x0, double x1, double y, double amp, int n) {
        std::vector<P> pts;
        for (int i = 0; i <= n; ++i) pts.push_back({x0 + (x1 - x0) * i / n, y + (i % 2 == 0 ? -amp : amp) * (i == 0 || i == n ? 0 : 1)});
        line("ink", pts);
    }
    void capsule(double x, double y, double w, double h) {
        // vertical vessel with rounded ends
        std::vector<P> pts;
        const double r = w / 2;
        for (int i = 0; i <= 12; ++i) {
            const double a = std::numbers::pi * i / 12;
            pts.push_back({x + r * std::cos(a), y + h / 2 - r + r * std::sin(a)});
        }
        for (int i = 0; i <= 12; ++i) {
            const double a = std::numbers::pi + std::numbers::pi * i / 12;
            pts.push_back({x + r * std::cos(a), y - h / 2 + r + r * std::sin(a)});
        }
        closed("bgfill", pts);
    }
    void hcapsule(double x, double y, double w, double h) {
        std::vector<P> pts;
        const double r = h / 2;
        for (int i = 0; i <= 12; ++i) {
            const double a = -std::numbers::pi / 2 + std::numbers::pi * i / 12;
            pts.push_back({x + w / 2 - r + r * std::cos(a), y + r * std::sin(a)});
        }
        for (int i = 0; i <= 12; ++i) {
            const double a = std::numbers::pi / 2 + std::numbers::pi * i / 12;
            pts.push_back({x - w / 2 + r + r * std::cos(a), y + r * std::sin(a)});
        }
        closed("bgfill", pts);
    }
    void arrow(P a, P b) {
        line("ink", {a, b});
        const double dx = b[0] - a[0], dy = b[1] - a[1], l = std::hypot(dx, dy);
        const double ux = dx / l, uy = dy / l;
        closed("inkfill", {b, {b[0] - 2.2 * ux + 1.1 * uy, b[1] - 2.2 * uy - 1.1 * ux}, {b[0] - 2.2 * ux - 1.1 * uy, b[1] - 2.2 * uy + 1.1 * ux}});
    }
    void motor(double x, double y) {
        line("ink", {{x, y}, {x, y + 7}});
        ch.markers.push_back({x, y + 9.5, "M", "motor"});
    }
    void dashedLine(P a, P b, int n = 5) {
        for (int i = 0; i < n; ++i) {
            const double t0 = static_cast<double>(i) / n, t1 = t0 + 0.55 / n;
            line("ink", {along(a, b, t0), along(a, b, t1)});
        }
    }
};

using Draw = std::function<void(Sym&, double, double)>;

struct Entry {
    Text name;      // full name for the table
    Text shortName; // label in the picture (≤ 18 characters)
    Text meaning;
    const char* norm;
    Draw draw;
};

/// Grid of symbols, 3 per row, each labelled "n Name"; table rows with the same numbers
Table symbolTable(std::string id, Text title, Text summary, Text description, const std::vector<Entry>& e) {
    Table t;
    t.id = std::move(id);
    t.category = kCat;
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = std::move(description);
    t.columns = {{"Nr.", "No."}, {"Symbol", "Symbol"}, {"Bedeutung / Einsatz", "Meaning / use"}, {"Angelehnt an", "Modelled on"}};
    Sym s;
    constexpr int cols = 3;
    constexpr double cw = 100.0 / cols, chh = 27;
    const int rows = (static_cast<int>(e.size()) + cols - 1) / cols;
    const double H = rows * chh + 2;
    s.frame(100, H, H / 100.0);
    for (std::size_t i = 0; i < e.size(); ++i) {
        const int r = static_cast<int>(i) / cols, c = static_cast<int>(i) % cols;
        const double cx = cw * c + cw / 2, top = H - 1 - r * chh;
        e[i].draw(s, cx, top - 11.5);
        const std::string n = std::to_string(i + 1);
        s.label(cx, top - chh + 3.2, {n + " " + e[i].shortName.de, n + " " + e[i].shortName.en}, "c");
        t.rows.push_back({n, e[i].name, e[i].meaning, e[i].norm});
    }
    s.ch.note = {"Eigene Zeichnung der üblichen Symbolformen – je Norm und Planungsbüro gibt es Abweichungen. Im Projekt die verwendeten "
                 "Symbole immer in der Legende angeben (Entwurfsplanung: Legende laienverständlich).",
                 "Own drawings of the usual symbol shapes – standards and offices differ in detail. Always state the symbols used in the "
                 "project legend (design stage: legend understandable for laypersons)."};
    t.drawing = s.ch;
    return t;
}

// ------------------------------------------------------------------ ventilation

Table airSymbols() {
    const char* n12792 = "DIN EN 12792";
    std::vector<Entry> e{
        {{"Ventilator", "Fan"}, {"Ventilator", "Fan"}, {"Zu- oder Abluftventilator, Spitze in Förderrichtung", "Supply or extract fan, tip in flow direction"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.circle("bgfill", x, y, 5.5);
             s.line("ink", {{x - 2.7, y + 4.8}, {x + 5.5, y}, {x - 2.7, y - 4.8}});
         }},
        {{"Luftfilter", "Air filter"}, {"Filter", "Filter"}, {"Filterstufe (ISO ePM1 … ISO Coarse), Druckverlust überwachen", "Filter stage (ISO ePM1 … coarse), monitor pressure drop"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.box(x, y, 9, 11);
             s.zigzag(x - 3.5, x + 3.5, y, 4.5, 4);
         }},
        {{"Lufterhitzer (Heizregister)", "Heating coil"}, {"Erhitzer", "Heater"}, {"Warmwasser- oder Elektroregister, Frostschutz", "Hot water or electric coil, frost protection"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.box(x, y, 9, 11);
             s.line("ink", {{x - 4.5, y - 5.5}, {x + 4.5, y + 5.5}});
             s.line("ink", {{x - 3.2, y + 2.8}, {x - 0.6, y + 2.8}});
             s.line("ink", {{x - 1.9, y + 1.5}, {x - 1.9, y + 4.1}});
         }},
        {{"Luftkühler (Kühlregister)", "Cooling coil"}, {"Kühler", "Cooler"}, {"Kaltwasser- oder Direktverdampfer, mit Tropfenabscheider", "Chilled water or DX coil, with droplet separator"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.box(x, y, 9, 11);
             s.line("ink", {{x - 4.5, y - 5.5}, {x + 4.5, y + 5.5}});
             s.line("ink", {{x - 3.2, y + 2.8}, {x - 0.6, y + 2.8}});
         }},
        {{"Befeuchter", "Humidifier"}, {"Befeuchter", "Humidifier"}, {"Dampf- oder Verdunstungsbefeuchter", "Steam or evaporative humidifier"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.box(x, y, 9, 11);
             for (const double dx : {-2.5, 0.0, 2.5}) {
                 s.line("ink", {{x + dx, y + 4.5}, {x + dx, y + 0.5}});
                 s.circle("inkfill", x + dx, y - 1.8, 0.6, 8);
             }
         }},
        {{"Plattenwärmeübertrager", "Plate heat exchanger"}, {"Platten-WRG", "Plate HR"}, {"Kreuz- oder Gegenstrom, Luftströme getrennt", "Cross or counter flow, air streams separated"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.box(x, y, 11, 11);
             s.line("ink", {{x - 5.5, y - 5.5}, {x + 5.5, y + 5.5}});
             s.line("ink", {{x - 5.5, y + 5.5}, {x + 5.5, y - 5.5}});
         }},
        {{"Rotationswärmeübertrager", "Rotary heat exchanger"}, {"Rotor-WRG", "Rotor HR"}, {"Rotor mit Feuchterückgewinnung, Drehzahl geregelt", "Rotor with moisture recovery, speed controlled"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.box(x, y, 11, 11);
             s.circle("ink", x, y, 4.2, 20);
             s.line("ink", {{x, y + 4.2}, {x, y - 4.2}});
         }},
        {{"Kreislaufverbundsystem (KVS)", "Run-around coil system"}, {"KVS", "Run-around"}, {"Zwei Register mit Pumpenkreis, Luftströme räumlich getrennt", "Two coils with pump loop, air streams apart"}, n12792,
         [](Sym& s, double x, double y) {
             s.box(x - 6, y + 2, 5, 8);
             s.box(x + 6, y + 2, 5, 8);
             s.line("ink", {{x - 8.5, y - 2}, {x + 8.5, y + 6}});
             s.line("ink", {{x - 6, y - 2}, {x - 6, y - 6}, {x + 6, y - 6}, {x + 6, y - 2}});
             s.line("ink", {{x - 3.5, y - 2}, {x + 3.5, y + 6}});
             s.pumpSym(x, y - 6, 2.6);
         }},
        {{"Schalldämpfer", "Silencer"}, {"Schalldämpfer", "Silencer"}, {"Kulissen- oder Rohrschalldämpfer", "Splitter or circular silencer"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.box(x, y, 13, 11);
             for (const double dx : {-3.5, 0.0, 3.5}) s.rect("inkfill", x + dx - 0.9, y - 4, x + dx + 0.9, y + 4);
         }},
        {{"Jalousieklappe / Regelklappe", "Damper"}, {"Klappe", "Damper"}, {"Absperr- oder Regelklappe, mit Antrieb (M)", "Shut-off or control damper, with actuator (M)"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.line("ink", {{x - 4, y - 4.5}, {x + 4, y + 4.5}});
             s.circle("inkfill", x, y, 0.9, 10);
         }},
        {{"Brandschutzklappe (BSK)", "Fire damper"}, {"Brandschutzkl.", "Fire damper"}, {"Schließt im Brandfall, Auslösung thermisch oder über Rauchmelder", "Closes in case of fire, thermal or smoke-detector release"},
         "DIN EN 12792, DIN EN 15650",
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.line("ink", {{x - 4, y - 4.5}, {x + 4, y + 4.5}});
             s.closed("inkfill", {{x - 2, y + 5.5}, {x + 2, y + 5.5}, {x, y + 2.2}});
             s.line("ink", {{x, y + 5.5}, {x, y + 8}});
             s.rect("inkfill", x - 1.6, y + 8, x + 1.6, y + 9.6);
         }},
        {{"Volumenstromregler (VSR)", "VAV / CAV controller"}, {"VSR", "VAV box"}, {"Konstant- oder variabler Volumenstrom, mit Messung", "Constant or variable flow, with measurement"}, n12792,
         [](Sym& s, double x, double y) {
             s.duct(x, y);
             s.line("ink", {{x - 4, y - 4.5}, {x + 4, y + 4.5}});
             s.arrow({x - 6, y - 4}, {x + 6.5, y + 4.2});
         }},
        {{"Wetterschutzgitter", "Weather louvre"}, {"Wetterschutzgitter", "Louvre"}, {"Außenluftansaugung / Fortluftausblas", "Outdoor air intake / exhaust outlet"}, n12792,
         [](Sym& s, double x, double y) {
             s.line("ink", {{x + 1, y + 5.5}, {x + 13, y + 5.5}});
             s.line("ink", {{x + 1, y - 5.5}, {x + 13, y - 5.5}});
             s.box(x - 1, y, 4, 13);
             for (const double dy : {-4.0, -1.0, 2.0}) s.line("ink", {{x - 3, y + dy}, {x + 1, y + dy + 2.5}});
         }},
        {{"Zuluftdurchlass (Deckendurchlass)", "Supply air diffuser"}, {"Zuluftdurchlass", "Supply diffuser"}, {"Drall- oder Schlitzauslass, Luftstrom in den Raum", "Swirl or slot diffuser, air into the room"}, n12792,
         [](Sym& s, double x, double y) {
             s.box(x, y, 12, 12);
             s.box(x, y, 5, 5);
             s.arrow({x + 2.5, y + 2.5}, {x + 6.5, y + 6.5});
             s.arrow({x - 2.5, y - 2.5}, {x - 6.5, y - 6.5});
         }},
        {{"Abluftdurchlass (Gitter, Ventil)", "Extract air grille"}, {"Abluftdurchlass", "Extract grille"}, {"Abluftgitter oder Tellerventil, Luftstrom aus dem Raum", "Extract grille or disc valve, air out of the room"}, n12792,
         [](Sym& s, double x, double y) {
             s.box(x, y, 12, 12);
             s.circle("ink", x, y, 2.6, 16);
             s.arrow({x + 6.5, y + 6.5}, {x + 3, y + 3});
             s.arrow({x - 6.5, y - 6.5}, {x - 3, y - 3});
         }},
    };
    return symbolTable("symbols_air", {"Symbole Lüftung (RLT)", "Symbols ventilation (AHU)"},
                       {"Ventilator, Filter, Register, WRG, Klappen, BSK, VSR, Durchlässe – mit Bild", "Fan, filter, coils, heat recovery, dampers, fire damper, VAV, diffusers – with picture"},
                       {"Bildzeichen für Lüftungsschemata und Grundrisse in der üblichen Form, angelehnt an DIN EN 12792 (Lüftung von Gebäuden – "
                        "Symbole und Begriffe) und DIN EN ISO 10628-2. Luftarten und ihre Farben siehe Tabelle „Leitungsfarben“. Gegen "
                        "gültige Normausgabe und Bürostandard prüfen.",
                        "Graphical symbols for ventilation schematics and plans in the usual form, modelled on EN 12792 (ventilation – "
                        "symbols and terms) and EN ISO 10628-2. Air types and their colours see table \"Line colours\". Check against the "
                        "edition in force and the office standard."},
                       e);
}

// ------------------------------------------------------------------ heating

Table heatingSymbols() {
    const char* iso = "DIN EN ISO 10628-2, DIN 2429-2";
    std::vector<Entry> e{
        {{"Wärmeerzeuger (Kessel)", "Heat generator (boiler)"}, {"Kessel", "Boiler"}, {"Gas-, Öl- oder Biomassekessel mit Brenner", "Gas, oil or biomass boiler with burner"}, "DIN EN 12828",
         [](Sym& s, double x, double y) {
             s.box(x, y, 14, 13);
             s.line("ink", {{x - 4, y - 4.5}, {x - 2.5, y - 1}, {x - 1, y - 3.5}, {x + 0.5, y + 0.5}, {x + 2, y - 3}, {x + 3.5, y - 1}, {x + 4.5, y - 4.5}});
         }},
        {{"Wärmepumpe", "Heat pump"}, {"Wärmepumpe", "Heat pump"}, {"Luft-, Sole- oder Wasser-Wärmepumpe", "Air, brine or water heat pump"}, "DIN EN 12828",
         [](Sym& s, double x, double y) {
             s.box(x, y, 14, 13);
             s.line("ink", {{x - 7, y - 6.5}, {x + 7, y + 6.5}});
             s.arrow({x + 2, y - 4}, {x + 2, y + 1.5});
             s.arrow({x - 2, y - 1.5}, {x - 2, y + 4});
         }},
        {{"Umwälzpumpe", "Circulating pump"}, {"Pumpe", "Pump"}, {"Spitze in Förderrichtung", "Tip in flow direction"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.pumpSym(x, y);
         }},
        {{"Pumpe drehzahlgeregelt", "Variable-speed pump"}, {"Pumpe geregelt", "VS pump"}, {"Hocheffizienzpumpe mit Drehzahlregelung (Pfeil = verstellbar)", "High-efficiency pump with speed control (arrow = adjustable)"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.pumpSym(x, y);
             s.arrow({x - 7, y - 7}, {x + 7.5, y + 7.5});
         }},
        {{"Absperrventil", "Stop valve"}, {"Absperrventil", "Stop valve"}, {"Durchgangsventil, offen dargestellt", "Globe valve, shown open"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.bowtie(x, y, "bgfill");
         }},
        {{"Kugelhahn / Absperrklappe", "Ball valve / butterfly valve"}, {"Kugelhahn", "Ball valve"}, {"Schnellschluss, voller Durchgang", "Quick shut-off, full bore"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.bowtie(x, y, "bgfill");
             s.circle("inkfill", x, y, 1.6, 12);
         }},
        {{"Rückflussverhinderer", "Non-return valve"}, {"Rückflussverh.", "Check valve"}, {"Durchfluss nur in Pfeilrichtung", "Flow only in arrow direction"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.closed("bgfill", {{x - 4.4, y - 3.3}, {x - 4.4, y + 3.3}, {x + 1.5, y}});
             s.line("ink", {{x + 2.2, y - 4}, {x + 2.2, y + 4}});
         }},
        {{"Zweiwege-Regelventil mit Antrieb", "Two-way control valve with actuator"}, {"Regelventil 2-W.", "2-way valve"}, {"Durchflussregelung, Antrieb M", "Flow control, actuator M"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.bowtie(x, y, "inkfill");
             s.motor(x, y);
         }},
        {{"Dreiwege-Regelventil mit Antrieb", "Three-way control valve with actuator"}, {"Regelventil 3-W.", "3-way valve"}, {"Mischen oder Verteilen, Antrieb M", "Mixing or diverting, actuator M"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.line("ink", {{x, y}, {x, y - 9}});
             s.bowtie(x, y, "inkfill");
             s.closed("inkfill", {{x - 3.3, y - 4.4}, {x + 3.3, y - 4.4}, {x, y}});
             s.motor(x, y);
         }},
        {{"Strangregulierventil", "Balancing valve"}, {"Strangregulier.", "Balancing v."}, {"Hydraulischer Abgleich, mit Messanschlüssen", "Hydronic balancing, with test points"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.bowtie(x, y, "bgfill");
             s.line("ink", {{x, y}, {x, y + 6}});
             s.line("ink", {{x - 2.5, y + 6}, {x + 2.5, y + 6}});
         }},
        {{"Sicherheitsventil", "Safety valve"}, {"Sicherheitsvent.", "Safety valve"}, {"Begrenzt den Anlagendruck, Abblaseleitung sichtbar führen", "Limits the system pressure, visible discharge"}, "DIN EN 12828",
         [](Sym& s, double x, double y) {
             s.line("ink", {{x, y - 8}, {x, y - 3}});
             s.closed("bgfill", {{x - 3.3, y - 7.4}, {x + 3.3, y - 7.4}, {x, y - 2.5}});
             s.closed("bgfill", {{x - 0.5, y - 2.5}, {x + 5.4, y + 0.8}, {x + 5.4, y - 5.8}});
             s.line("ink", {{x + 5.4, y - 2.5}, {x + 11, y - 2.5}});
             s.line("ink", {{x, y - 2.5}, {x - 1.5, y}, {x + 1.5, y + 1.5}, {x - 1.5, y + 3}, {x + 1.5, y + 4.5}, {x, y + 6}});
         }},
        {{"Ausdehnungsgefäß (MAG)", "Expansion vessel"}, {"MAG", "Exp. vessel"}, {"Membran-Druckausdehnungsgefäß, Vordruck einstellen", "Membrane expansion vessel, set the pre-pressure"}, "DIN EN 12828",
         [](Sym& s, double x, double y) {
             s.line("ink", {{x, y - 9.5}, {x, y - 6}});
             s.capsule(x, y + 0.5, 9, 13);
             s.line("ink", {{x - 4.5, y + 0.5}, {x + 4.5, y + 0.5}});
         }},
        {{"Heizkörper", "Radiator"}, {"Heizkörper", "Radiator"}, {"Raumheizfläche mit Thermostatventil", "Room heating surface with thermostatic valve"}, "DIN EN 12828",
         [](Sym& s, double x, double y) {
             s.box(x, y, 15, 9);
             for (int i = -2; i <= 2; ++i) s.line("ink", {{x + 2.6 * i, y - 4.5}, {x + 2.6 * i, y + 4.5}});
         }},
        {{"Plattenwärmeübertrager", "Plate heat exchanger"}, {"Wärmeübertrager", "Heat exchanger"}, {"Systemtrennung, z. B. Fernwärme oder Glykolkreis", "System separation, e.g. district heat or glycol loop"}, iso,
         [](Sym& s, double x, double y) {
             s.box(x, y, 10, 14);
             s.zigzag(x - 5, x + 5, y, 4, 4);
             for (const double dx : {-5.0, 5.0}) {
                 s.line("ink", {{x + dx, y + 5}, {x + dx * 1.8, y + 5}});
                 s.line("ink", {{x + dx, y - 5}, {x + dx * 1.8, y - 5}});
             }
         }},
        {{"Schmutzfänger", "Strainer"}, {"Schmutzfänger", "Strainer"}, {"Schützt Pumpen, Ventile und Zähler", "Protects pumps, valves and meters"}, iso,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.closed("bgfill", {{x - 5.5, y}, {x, y + 5.5}, {x + 5.5, y}, {x, y - 5.5}});
             s.dashedLine({x - 2.8, y - 2.8}, {x + 2.8, y + 2.8}, 3);
         }},
        {{"Pufferspeicher", "Buffer tank"}, {"Pufferspeicher", "Buffer tank"}, {"Hydraulische Entkopplung, Laufzeitverlängerung", "Hydraulic decoupling, longer run times"}, "DIN EN 12828",
         [](Sym& s, double x, double y) {
             s.capsule(x, y, 9, 18);
             s.line("ink", {{x + 4.5, y + 5}, {x + 11, y + 5}});
             s.line("ink", {{x + 4.5, y - 5}, {x + 11, y - 5}});
         }},
    };
    return symbolTable("symbols_heating", {"Symbole Heizung und Hydraulik", "Symbols heating and hydronics"},
                       {"Kessel, Wärmepumpe, Pumpen, Ventile, Sicherheitsventil, MAG, Heizkörper, Speicher – mit Bild", "Boiler, heat pump, pumps, valves, safety valve, expansion vessel, radiator, tank – with picture"},
                       {"Bildzeichen für Heizungs- und Kaltwasserschemata in der üblichen Form, angelehnt an DIN EN ISO 10628-2 und DIN 2429-2 "
                        "(Pumpen, Armaturen) sowie DIN EN 12828 (Sicherheitstechnik von Heizungsanlagen). Hydraulische Grundschaltungen "
                        "siehe Gruppe „Hydraulik & Ventile“. Gegen gültige Normausgabe und Bürostandard prüfen.",
                        "Graphical symbols for heating and chilled water schematics in the usual form, modelled on EN ISO 10628-2 and DIN "
                        "2429-2 (pumps, valves) and EN 12828 (safety of heating systems). Hydronic circuits see group \"Hydronics & "
                        "valves\". Check against the edition in force and the office standard."},
                       e);
}

// ------------------------------------------------------------------ plumbing

Table sanitarySymbols() {
    const char* tw = "DIN 1988-100, DIN EN 806-1";
    const char* ab = "DIN 1986-100";
    std::vector<Entry> e{
        {{"Wasserzähler", "Water meter"}, {"Wasserzähler", "Water meter"}, {"Haupt- oder Unterzähler, Ein- und Auslauf gerade", "Main or sub-meter, straight inlet and outlet"}, tw,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.circle("bgfill", x, y, 5.2);
             s.tag(x, y, "m³");
         }},
        {{"Systemtrenner (BA)", "Backflow preventer (BA)"}, {"Systemtrenner BA", "BA preventer"}, {"Schutz des Trinkwassers bis Kategorie 4 (DIN EN 1717)", "Protects drinking water up to category 4 (EN 1717)"}, "DIN EN 1717",
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.box(x, y, 12, 9);
             s.tag(x, y, "BA");
         }},
        {{"Druckminderer", "Pressure reducer"}, {"Druckminderer", "Pressure red."}, {"Begrenzt den Ruhedruck hinter dem Hausanschluss", "Limits the static pressure after the service connection"}, tw,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.bowtie(x, y, "bgfill");
             s.line("ink", {{x, y}, {x, y + 4}});
             s.box(x, y + 6.5, 6, 5);
             s.arrow({x - 2, y + 4.8}, {x + 2.2, y + 8.3});
         }},
        {{"Trinkwasserfilter", "Drinking water filter"}, {"Filter", "Filter"}, {"Rückspülfilter nach dem Zähler", "Backwash filter after the meter"}, tw,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.closed("bgfill", {{x - 5.5, y}, {x, y + 5.5}, {x + 5.5, y}, {x, y - 5.5}});
             s.dashedLine({x - 2.8, y - 2.8}, {x + 2.8, y + 2.8}, 3);
         }},
        {{"Absperrventil", "Stop valve"}, {"Absperrventil", "Stop valve"}, {"Absperrung je Strang und Stockwerk", "Isolation per riser and floor"}, tw,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.bowtie(x, y, "bgfill");
         }},
        {{"Rückflussverhinderer", "Non-return valve"}, {"Rückflussverh.", "Check valve"}, {"Verhindert Rückfließen (Kategorie 2)", "Prevents backflow (category 2)"}, tw,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.closed("bgfill", {{x - 4.4, y - 3.3}, {x - 4.4, y + 3.3}, {x + 1.5, y}});
             s.line("ink", {{x + 2.2, y - 4}, {x + 2.2, y + 4}});
         }},
        {{"Trinkwassererwärmer (Speicher)", "Water heater (cylinder)"}, {"TW-Erwärmer", "Water heater"}, {"Speicher mit Heizwendel, ≥ 60 °C am Austritt", "Cylinder with coil, ≥ 60 °C at the outlet"}, tw,
         [](Sym& s, double x, double y) {
             s.capsule(x, y, 10, 18);
             s.zigzag(x - 2.8, x + 2.8, y - 3, 2.2, 4);
             s.line("ink", {{x - 3, y - 3}, {x - 9, y - 3}});
             s.line("ink", {{x + 3, y - 3}, {x + 9, y - 3}});
         }},
        {{"Zirkulationspumpe", "Circulation pump"}, {"Zirkulationspumpe", "Circ. pump"}, {"Hält das Warmwassernetz auf Temperatur (≥ 55 °C)", "Keeps the hot water network hot (≥ 55 °C)"}, tw,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.pumpSym(x, y);
         }},
        {{"Waschtisch", "Washbasin"}, {"Waschtisch", "Washbasin"}, {"Grundrisssymbol mit Becken", "Plan symbol with bowl"}, ab,
         [](Sym& s, double x, double y) {
             s.box(x, y, 16, 10);
             std::vector<P> bowl;
             for (int i = 0; i <= 14; ++i) {
                 const double a = std::numbers::pi * i / 14;
                 bowl.push_back({x - 5 * std::cos(a), y + 3 - 6.5 * std::sin(a)});
             }
             s.line("ink", bowl);
             s.line("ink", {{x - 5, y + 3}, {x + 5, y + 3}});
         }},
        {{"WC", "WC"}, {"WC", "WC"}, {"Grundrisssymbol mit Spülkasten bzw. Vorwandelement", "Plan symbol with cistern or frame"}, ab,
         [](Sym& s, double x, double y) {
             s.box(x, y + 6, 13, 4);
             std::vector<P> o;
             for (int i = 0; i <= 24; ++i) {
                 const double a = 2 * std::numbers::pi * i / 24;
                 o.push_back({x + 4.2 * std::cos(a), y - 1.5 + 5.5 * std::sin(a)});
             }
             s.closed("bgfill", o);
         }},
        {{"Dusche", "Shower"}, {"Dusche", "Shower"}, {"Duschtasse oder bodengleich mit Ablauf", "Tray or level-access with drain"}, ab,
         [](Sym& s, double x, double y) {
             s.box(x, y, 13, 13);
             s.line("ink", {{x - 6.5, y - 6.5}, {x + 6.5, y + 6.5}});
             s.line("ink", {{x - 6.5, y + 6.5}, {x + 6.5, y - 6.5}});
             s.circle("bgfill", x, y, 1.6, 12);
         }},
        {{"Bodenablauf", "Floor drain"}, {"Bodenablauf", "Floor drain"}, {"Mit Geruchsverschluss, ggf. Rückstauverschluss", "With trap, backflow stop if needed"}, ab,
         [](Sym& s, double x, double y) {
             s.box(x, y, 10, 10);
             s.circle("ink", x, y, 3.4, 18);
             s.line("ink", {{x - 3.4, y}, {x + 3.4, y}});
             s.line("ink", {{x, y - 3.4}, {x, y + 3.4}});
         }},
        {{"Fallleitung (Schmutzwasser)", "Stack (foul water)"}, {"Fallleitung", "Stack"}, {"Grundriss: Kreis mit Strich, Pfeil = Fließrichtung", "Plan: circle with stroke, arrow = flow direction"}, ab,
         [](Sym& s, double x, double y) {
             s.circle("bgfill", x, y, 4.5);
             s.line("ink", {{x - 3.2, y - 3.2}, {x + 3.2, y + 3.2}});
             s.arrow({x + 5, y + 6}, {x + 5, y - 6});
         }},
        {{"Hebeanlage", "Lifting plant"}, {"Hebeanlage", "Lifting plant"}, {"Fördert Abwasser unter der Rückstauebene", "Lifts waste water below the backflow level"}, "DIN EN 12050",
         [](Sym& s, double x, double y) {
             s.box(x, y, 16, 12);
             s.pumpSym(x + 2, y - 1, 3.6);
             s.line("ink", {{x + 2, y + 2.6}, {x + 2, y + 9}, {x + 11, y + 9}});
         }},
        {{"Rückstauverschluss", "Backflow stop"}, {"Rückstauverschl.", "Backflow stop"}, {"Schützt Abläufe unter der Rückstauebene", "Protects drains below the backflow level"}, "DIN EN 13564",
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.box(x, y, 10, 9);
             s.line("ink", {{x - 2.5, y + 4.5}, {x + 2.5, y - 3}});
             s.circle("inkfill", x - 2.5, y + 4.5, 0.8, 8);
         }},
    };
    return symbolTable("symbols_sanitary", {"Symbole Sanitär", "Symbols plumbing"},
                       {"Zähler, Systemtrenner, Druckminderer, Erwärmer, Objekte, Abläufe, Hebeanlage – mit Bild", "Meter, backflow preventer, reducer, heater, fixtures, drains, lifting plant – with picture"},
                       {"Bildzeichen für Trinkwasser- und Entwässerungspläne in der üblichen Form, angelehnt an DIN 1988-100 / DIN EN 806-1 "
                        "(Trinkwasser), DIN EN 1717 (Schutz vor Verschmutzung), DIN 1986-100 (Entwässerung) sowie DIN EN 12050 und "
                        "DIN EN 13564. Gegen gültige Normausgabe und Bürostandard prüfen.",
                        "Graphical symbols for drinking water and drainage plans in the usual form, modelled on DIN 1988-100 / EN 806-1, "
                        "EN 1717 (backflow protection), DIN 1986-100 (drainage) and EN 12050 / EN 13564. Check against the edition in "
                        "force and the office standard."},
                       e);
}

// ------------------------------------------------------------------ refrigeration

Table coolingSymbols() {
    const char* n1861 = "DIN EN 1861";
    std::vector<Entry> e{
        {{"Verdichter", "Compressor"}, {"Verdichter", "Compressor"}, {"Hub-, Scroll- oder Schraubenverdichter", "Reciprocating, scroll or screw compressor"}, n1861,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.circle("bgfill", x, y, 5.5);
             s.line("ink", {{x - 4.4, y + 3.3}, {x + 4.4, y + 1.4}});
             s.line("ink", {{x - 4.4, y - 3.3}, {x + 4.4, y - 1.4}});
         }},
        {{"Verflüssiger, luftgekühlt", "Air-cooled condenser"}, {"Verflüssiger", "Condenser"}, {"Gibt die Wärme an die Außenluft ab", "Rejects the heat to outdoor air"}, n1861,
         [](Sym& s, double x, double y) {
             s.box(x, y - 2, 16, 8);
             s.zigzag(x - 7, x + 7, y - 2, 2.6, 6);
             s.circle("bgfill", x, y + 6, 3);
             s.line("ink", {{x - 2.1, y + 3.9}, {x + 2.1, y + 8.1}});
         }},
        {{"Verdampfer", "Evaporator"}, {"Verdampfer", "Evaporator"}, {"Nimmt Wärme aus Wasser oder Luft auf", "Absorbs heat from water or air"}, n1861,
         [](Sym& s, double x, double y) {
             s.box(x, y, 16, 9);
             s.zigzag(x - 7, x + 7, y, 2.8, 6);
             s.line("ink", {{x - 8, y}, {x - 12, y}});
             s.line("ink", {{x + 8, y}, {x + 12, y}});
         }},
        {{"Expansionsventil, thermostatisch", "Thermostatic expansion valve"}, {"Expansionsventil", "Expansion v."}, {"Regelt die Überhitzung, Fühler am Saugrohr", "Controls superheat, bulb on the suction line"}, n1861,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.bowtie(x, y, "bgfill");
             s.arrow({x - 5, y - 5}, {x + 5.5, y + 5.5});
             s.line("ink", {{x, y + 3}, {x, y + 8}, {x + 9, y + 8}});
             s.circle("inkfill", x + 10, y + 8, 1.2, 10);
         }},
        {{"Magnetventil", "Solenoid valve"}, {"Magnetventil", "Solenoid valve"}, {"Absperrung der Flüssigkeitsleitung bei Stillstand", "Shuts the liquid line at standstill"}, n1861,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.bowtie(x, y, "bgfill");
             s.line("ink", {{x, y}, {x, y + 4}});
             s.box(x, y + 6.5, 6, 5);
             s.line("ink", {{x - 3, y + 4}, {x + 3, y + 9}});
         }},
        {{"Kältemittelsammler", "Liquid receiver"}, {"Sammler", "Receiver"}, {"Nimmt Kältemittel bei Lastwechsel auf", "Holds refrigerant during load changes"}, n1861,
         [](Sym& s, double x, double y) {
             s.pipe(x, y, 13);
             s.hcapsule(x, y, 18, 8);
         }},
        {{"Filtertrockner", "Filter drier"}, {"Filtertrockner", "Filter drier"}, {"Bindet Feuchte und Schmutz im Kältemittel", "Removes moisture and dirt from the refrigerant"}, n1861,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.box(x, y, 12, 7);
             for (const double dx : {-3.5, 0.0, 3.5}) s.circle("inkfill", x + dx, y, 0.8, 8);
         }},
        {{"Schauglas", "Sight glass"}, {"Schauglas", "Sight glass"}, {"Zeigt Füllstand und Feuchte (Farbumschlag)", "Shows charge and moisture (colour change)"}, n1861,
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.circle("bgfill", x, y, 4.6);
             s.circle("ink", x, y, 2.2, 16);
         }},
        {{"Rückkühler / Trockenkühler", "Dry cooler"}, {"Rückkühler", "Dry cooler"}, {"Rückkühlung des Verflüssigerkreises, Freie Kühlung", "Re-cooling of the condenser loop, free cooling"}, n1861,
         [](Sym& s, double x, double y) {
             s.box(x, y - 3, 18, 7);
             s.zigzag(x - 8, x + 8, y - 3, 2.2, 8);
             for (const double dx : {-4.5, 4.5}) {
                 s.circle("bgfill", x + dx, y + 4.5, 3);
                 s.line("ink", {{x + dx - 2.1, y + 2.4}, {x + dx + 2.1, y + 6.6}});
             }
         }},
        {{"Kaltwassersatz (Kältemaschine)", "Chiller"}, {"Kältemaschine", "Chiller"}, {"Erzeugt Kaltwasser, z. B. 6/12 °C oder 16/19 °C", "Produces chilled water, e.g. 6/12 °C or 16/19 °C"}, n1861,
         [](Sym& s, double x, double y) {
             s.box(x, y, 14, 13);
             s.tag(x, y, "KM");
             for (const double dy : {-3.5, 3.5}) {
                 s.line("ink", {{x - 7, y + dy}, {x - 11, y + dy}});
                 s.line("ink", {{x + 7, y + dy}, {x + 11, y + dy}});
             }
         }},
        {{"Kälte-Pufferspeicher", "Chilled water buffer"}, {"Pufferspeicher", "Buffer tank"}, {"Taktbegrenzung, Mindestvolumen nach Hersteller", "Limits cycling, minimum volume per manufacturer"}, n1861,
         [](Sym& s, double x, double y) {
             s.capsule(x, y, 9, 18);
             s.line("ink", {{x - 4.5, y + 5}, {x - 11, y + 5}});
             s.line("ink", {{x - 4.5, y - 5}, {x - 11, y - 5}});
         }},
        {{"Kaltwasserpumpe", "Chilled water pump"}, {"Pumpe", "Pump"}, {"Primär- oder Sekundärkreis", "Primary or secondary circuit"}, "DIN EN ISO 10628-2",
         [](Sym& s, double x, double y) {
             s.pipe(x, y);
             s.pumpSym(x, y);
         }},
        {{"Druckwächter (Hoch-/Niederdruck)", "Pressure switch (high/low)"}, {"Druckwächter", "Pressure sw."}, {"Sicherheitsabschaltung des Verdichters", "Safety cut-out of the compressor"}, "DIN EN 378",
         [](Sym& s, double x, double y) {
             s.pipe(x, y - 5);
             s.line("ink", {{x, y - 5}, {x, y + 0.5}});
             s.circle("bgfill", x, y + 4.5, 4.2);
             s.tag(x, y + 4.5, "PS");
         }},
        {{"Umluftkühler (Gebläsekonvektor)", "Fan coil unit"}, {"Umluftkühler", "Fan coil"}, {"Raumkühlung mit Ventilator und Kühlregister", "Room cooling with fan and cooling coil"}, n1861,
         [](Sym& s, double x, double y) {
             s.box(x - 3, y, 10, 11);
             s.line("ink", {{x - 8, y - 5.5}, {x + 2, y + 5.5}});
             s.line("ink", {{x - 6.5, y + 3}, {x - 4, y + 3}});
             s.circle("bgfill", x + 7, y, 3.4);
             s.line("ink", {{x + 5.6, y + 2.8}, {x + 10.4, y}, {x + 5.6, y - 2.8}});
         }},
    };
    return symbolTable("symbols_cooling", {"Symbole Kälte", "Symbols refrigeration"},
                       {"Verdichter, Verflüssiger, Verdampfer, Expansions- und Magnetventil, Rückkühler, Kältemaschine – mit Bild",
                        "Compressor, condenser, evaporator, expansion and solenoid valve, dry cooler, chiller – with picture"},
                       {"Bildzeichen für Kälteanlagen in der üblichen Form, angelehnt an DIN EN 1861 (Kälteanlagen – Systemfließbilder, "
                        "Rohrleitungs- und Instrumentenfließbilder), DIN EN ISO 10628-2 und DIN EN 378. Gegen gültige Normausgabe und "
                        "Bürostandard prüfen.",
                        "Graphical symbols for refrigerating systems in the usual form, modelled on EN 1861 (system flow diagrams and P&I "
                        "diagrams), EN ISO 10628-2 and EN 378. Check against the edition in force and the office standard."},
                       e);
}

// ------------------------------------------------------------------ line colours

struct Swatch {
    const char* hex;
    Text label;
};

Table lineColors() {
    Table t;
    t.id = "line_colors";
    t.category = kCat;
    t.title = {"Leitungsfarben (Luftarten, Trinkwasser, Heizung, DIN 2403)", "Line colours (air types, drinking water, heating, DIN 2403)"};
    t.summary = {"Farben für Pläne und Rohrkennzeichnung – mit Farbbild", "Colours for drawings and pipe marking – with colour picture"};
    t.description = {
        "Luftarten nach DIN EN 16798-3 (Nichtwohngebäude; für Wohnungslüftung DIN EN 12792 mit Abweichungen: Zuluft je Aufbereitung "
        "grün, rot, blau oder violett, Umluft gelb). Trinkwasser-Kurzzeichen nach DIN 1988-100 / DIN EN 806-1 mit den üblichen "
        "Kennfarben. Rohrleitungskennzeichnung nach DIN 2403 (Gruppenfarbe, Zusatzfarbe) wie in TRGS 201 Anhang 3 wiedergegeben. "
        "Heizung und Kälte sind für Pläne nicht genormt: Vorlauf rot / Rücklauf blau ist verbreitete Praxis (z. B. CAD-Layerstruktur "
        "Universität Hamburg 2006); Kälte im Projekt festlegen. Verwendete Farben immer in der Planlegende angeben. Gegen gültige "
        "Normausgabe und Bürostandard prüfen.",
        "Air types per EN 16798-3 (non-residential; for dwellings EN 12792 with differences: supply air green, red, blue or violet by "
        "treatment, recirculation yellow). Drinking water codes per DIN 1988-100 / EN 806-1 with the usual identification colours. "
        "Pipeline identification per DIN 2403 (group colour, additional colour) as given in TRGS 201 annex 3. Heating and cooling are "
        "not standardised for drawings: supply red / return blue is common practice (e.g. CAD layer structure University of Hamburg "
        "2006); fix cooling colours per project. Always state the colours in the drawing legend. Check against the edition in force."};
    t.columns = {{"Bereich", "Area"}, {"Medium", "Medium"}, {"Kürzel", "Code"}, {"Farbe", "Colour"}, {"Grundlage", "Basis"}};
    const Text air{"Luft", "Air"};
    const Text tw{"Trinkwasser", "Drinking water"};
    const Text din{"Rohrkennzeichnung", "Pipe marking"};
    const char* e16798 = "DIN EN 16798-3";
    const char* d2403 = "DIN 2403";
    t.rows = {
        {air, {"Außenluft", "Outdoor air"}, "ODA", {"grün", "green"}, e16798},
        {air, {"Zuluft", "Supply air"}, "SUP", {"blau (DIN EN 12792: grün/rot/blau/violett je Aufbereitung)", "blue (EN 12792: green/red/blue/violet by treatment)"}, e16798},
        {air, {"Raumluft", "Indoor air"}, "IDA", {"grau", "grey"}, e16798},
        {air, {"Überströmluft", "Transferred air"}, "TRA", {"grau", "grey"}, e16798},
        {air, {"Abluft", "Extract air"}, "ETA", {"gelb", "yellow"}, e16798},
        {air, {"Umluft", "Recirculation air"}, "RCA", {"orange (DIN EN 12792: gelb)", "orange (EN 12792: yellow)"}, e16798},
        {air, {"Fortluft", "Exhaust air"}, "EHA", {"braun", "brown"}, e16798},
        {air, {"Sekundärluft", "Secondary air"}, "SEC", {"orange", "orange"}, e16798},
        {air, {"Mischluft", "Mixed air"}, "MIA", {"Farben der Anteile bzw. orange", "colours of the parts or orange"}, e16798},
        {air, {"Leckluft", "Leakage air"}, "LEA", {"grau", "grey"}, e16798},
        {tw, {"Trinkwasser kalt", "Cold drinking water"}, "PWC", {"grün", "green"}, "DIN 1988-100"},
        {tw, {"Trinkwasser warm", "Hot drinking water"}, "PWH", {"rot", "red"}, "DIN 1988-100"},
        {tw, {"Zirkulation", "Circulation"}, "PWH-C", {"violett", "violet"}, "DIN 1988-100"},
        {{"Heizung", "Heating"}, {"Vorlauf", "Supply"}, {"VL", "F"}, {"rot", "red"}, {"Planpraxis, nicht genormt", "drawing practice, not standardised"}},
        {{"Heizung", "Heating"}, {"Rücklauf", "Return"}, {"RL", "R"}, {"blau", "blue"}, {"Planpraxis, nicht genormt", "drawing practice, not standardised"}},
        {{"Kälte", "Cooling"}, {"Kaltwasser Vorlauf/Rücklauf", "Chilled water supply/return"}, "KVL/KRL",
         {"im Projekt festlegen (z. B. dunkelgrün/hellblau)", "fix per project (e.g. dark green/light blue)"}, {"Planpraxis, nicht genormt", "drawing practice, not standardised"}},
        {din, {"Wasser", "Water"}, {"Gruppe 1", "Group 1"}, {"grün", "green"}, d2403},
        {din, {"Wasserdampf", "Steam"}, {"Gruppe 2", "Group 2"}, {"rot", "red"}, d2403},
        {din, {"Luft", "Air"}, {"Gruppe 3", "Group 3"}, {"grau", "grey"}, d2403},
        {din, {"Brennbare Gase", "Combustible gases"}, {"Gruppe 4", "Group 4"}, {"gelb, Zusatzfarbe rot", "yellow, additional red"}, d2403},
        {din, {"Nicht brennbare Gase", "Non-combustible gases"}, {"Gruppe 5", "Group 5"}, {"gelb, Zusatzfarbe schwarz", "yellow, additional black"}, d2403},
        {din, {"Säuren", "Acids"}, {"Gruppe 6", "Group 6"}, {"orange", "orange"}, d2403},
        {din, {"Laugen", "Alkalis"}, {"Gruppe 7", "Group 7"}, {"violett", "violet"}, d2403},
        {din, {"Brennbare Flüssigkeiten und Feststoffe", "Combustible liquids and solids"}, {"Gruppe 8", "Group 8"}, {"braun, Zusatzfarbe rot", "brown, additional red"}, d2403},
        {din, {"Nicht brennbare Flüssigkeiten und Feststoffe", "Non-combustible liquids and solids"}, {"Gruppe 9", "Group 9"}, {"braun, Zusatzfarbe schwarz", "brown, additional black"}, d2403},
        {din, {"Sauerstoff", "Oxygen"}, {"Gruppe 0", "Group 0"}, {"blau", "blue"}, d2403},
    };
    // picture: colour swatches in two columns
    const std::vector<std::pair<Text, std::vector<Swatch>>> left{
        {{"Luftarten (DIN EN 16798-3)", "Air types (EN 16798-3)"},
         {{"#2E7D32", {"ODA Außenluft", "ODA outdoor"}}, {"#1565C0", {"SUP Zuluft", "SUP supply"}}, {"#FBC02D", {"ETA Abluft", "ETA extract"}},
          {"#6D4C41", {"EHA Fortluft", "EHA exhaust"}}, {"#EF6C00", {"RCA Umluft", "RCA recirc."}}, {"#9E9E9E", {"IDA/TRA Raumluft", "IDA/TRA indoor"}}}},
        {{"Trinkwasser (DIN 1988)", "Drinking water (DIN 1988)"},
         {{"#2E7D32", {"PWC kalt", "PWC cold"}}, {"#C62828", {"PWH warm", "PWH hot"}}, {"#7B1FA2", {"PWH-C Zirkulation", "PWH-C circulation"}}}},
        {{"Heizung (Planpraxis)", "Heating (practice)"}, {{"#C62828", {"Vorlauf", "Supply"}}, {"#1565C0", {"Rücklauf", "Return"}}}},
    };
    const std::vector<std::pair<Text, std::vector<Swatch>>> right{
        {{"Rohrkennzeichnung (DIN 2403)", "Pipe marking (DIN 2403)"},
         {{"#2E7D32", {"1 Wasser", "1 water"}}, {"#C62828", {"2 Wasserdampf", "2 steam"}}, {"#9E9E9E", {"3 Luft", "3 air"}},
          {"#FBC02D", {"4/5 Gase", "4/5 gases"}}, {"#EF6C00", {"6 Säuren", "6 acids"}}, {"#7B1FA2", {"7 Laugen", "7 alkalis"}},
          {"#6D4C41", {"8/9 Flüssigkeiten", "8/9 liquids"}}, {"#1565C0", {"0 Sauerstoff", "0 oxygen"}}}},
    };
    Sketch s;
    const double H = 80;
    s.frame(100, H, H / 100.0);
    auto column = [&](const std::vector<std::pair<Text, std::vector<Swatch>>>& groups, double x0) {
        double y = H - 4;
        for (const auto& [head, items] : groups) {
            s.label(x0, y, head, "lb");
            y -= 5.6;
            for (const auto& it : items) {
                s.rect(std::string("fill:") + it.hex, x0, y - 1.3, x0 + 9, y + 1.3);
                s.label(x0 + 11, y, it.label, "l");
                y -= 4.6;
            }
            y -= 2.5;
        }
    };
    column(left, 2);
    column(right, 52);
    // Gas: yellow with red additional colour; show the additional colour band next to the group 4/5 swatch
    s.ch.note = {"Farbbalken = Kennfarbe des Mediums. Bei DIN 2403 zeigt die Zusatzfarbe (rot = brennbar, schwarz = nicht brennbar) die "
                 "Gefahr an – auf Schildern und Bändern neben der Gruppenfarbe.",
                 "Colour bar = identification colour of the medium. With DIN 2403 the additional colour (red = combustible, black = "
                 "non-combustible) marks the hazard – on labels and bands next to the group colour."};
    t.drawing = s.ch;
    return t;
}

}  // namespace

void addHvacSymbolTables(std::vector<Table>& out) {
    out.push_back(airSymbols());
    out.push_back(heatingSymbols());
    out.push_back(sanitarySymbols());
    out.push_back(coolingSymbols());
    out.push_back(lineColors());
}

}  // namespace elc::catalog
