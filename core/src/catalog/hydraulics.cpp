// Hydraulic basic circuits ("Regelgruppen") as a guide: schematic, behaviour, applications, valve sizing,
// plus a selection help. Facts from Siemens "Einführung in die HLK- und Gebäudetechnik" (08/2022), ch. 5.4–5.5,
// p. 144–153, and the Belimo selection table of hydraulic basic circuits (03/2023). Drawings are our own.
#include <array>
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"

namespace elc::catalog {

namespace {

using P = std::array<double, 2>;

// ------------------------------------------------------------------ drawing helpers (data units, y up)

constexpr double kW = 100;   // drawing width
constexpr double kH = 132;   // drawing height
constexpr double kVl = 34;   // x of the flow (supply) riser
constexpr double kRl = 66;   // x of the return riser

struct Sketch {
    Chart ch;

    void line(const char* style, std::vector<P> pts) { ch.series.push_back({style, std::move(pts)}); }
    void closed(const char* style, std::vector<P> pts) {
        pts.push_back(pts.front());
        line(style, std::move(pts));
    }
    void circle(const char* style, double x, double y, double r) {
        std::vector<P> pts;
        for (int i = 0; i <= 28; ++i) {
            const double a = 2.0 * kPi * i / 28.0;
            pts.push_back({x + r * std::cos(a), y + r * std::sin(a)});
        }
        line(style, std::move(pts));
    }
    void label(double x, double y, Text t, const char* style = "c") { ch.labels.push_back({x, y, std::move(t), style}); }
    void box(double x0, double y0, double x1, double y1, Text t) {
        closed("bgfill", {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}});
        label(0.5 * (x0 + x1), 0.5 * (y0 + y1), std::move(t), "cb");
    }
    /// pump on a vertical pipe, flow upwards; vfd = speed controlled (diagonal arrow)
    void pump(double x, double y, bool vfd) {
        constexpr double r = 4.6;
        circle("bgfill", x, y, r);
        closed("ink", {{x - 3.98, y - 2.3}, {x + 3.98, y - 2.3}, {x, y + r}});
        if (vfd) {
            line("ink", {{x - 6.5, y - 6.5}, {x + 6.2, y + 6.2}});
            closed("inkfill", {{x + 6.8, y + 6.8}, {x + 3.4, y + 5.8}, {x + 5.8, y + 3.4}});
        }
    }
    /// two-way valve (two filled triangles meeting in the middle)
    void valve2(double x, double y, bool vertical) {
        constexpr double h = 4.6;
        constexpr double w = 3.4;
        if (vertical) {
            closed("inkfill", {{x - w, y + h}, {x + w, y + h}, {x, y}});
            closed("inkfill", {{x - w, y - h}, {x + w, y - h}, {x, y}});
        } else {
            closed("inkfill", {{x - h, y - w}, {x - h, y + w}, {x, y}});
            closed("inkfill", {{x + h, y - w}, {x + h, y + w}, {x, y}});
        }
    }
    /// three-way valve on a vertical pipe; the third port points left (dir −1) or right (+1)
    void valve3(double x, double y, int dir) {
        valve2(x, y, true);
        closed("inkfill", {{x + dir * 4.6, y - 3.4}, {x + dir * 4.6, y + 3.4}, {x, y}});
    }
    void motor(double x, double y, int dir) {
        line("ink", {{x, y}, {x + dir * 9.5, y}});
        ch.markers.push_back({x + dir * 9.5, y, "M", "motor"});
    }
    /// balancing (regulating) valve on a horizontal pipe
    void balancing(double x, double y) {
        constexpr double h = 4.0;
        constexpr double w = 2.9;
        closed("bgfill", {{x - h, y - w}, {x - h, y + w}, {x, y}});
        closed("bgfill", {{x + h, y - w}, {x + h, y + w}, {x, y}});
        line("ink", {{x, y}, {x, y + 5.5}});
        line("ink", {{x - 2.2, y + 5.5}, {x + 2.2, y + 5.5}});
    }
    /// non-return valve on a horizontal pipe
    void check(double x, double y) {
        closed("bgfill", {{x - 4, y - 3}, {x + 4, y - 3}, {x + 4, y + 3}, {x - 4, y + 3}});
        line("ink", {{x - 4, y - 3}, {x + 4, y + 3}});
    }
    void sensor(double x, double y, int dir) {
        line("ink", {{x, y}, {x + dir * 8.5, y}});
        ch.markers.push_back({x + dir * 8.5, y, "T", "sensor"});
    }
    /// supply and return risers between generator and consumer
    void risers() {
        line("vl", {{kVl, 16}, {kVl, 116}});
        line("rl", {{kRl, 116}, {kRl, 16}});
    }
    void boxes() {
        box(18, 4, 82, 16, {"Erzeuger", "Generator"});
        box(18, 116, 82, 128, {"Verbraucher", "Consumer"});
        label(kVl + 7, 21, "VL");
        label(kRl - 7, 21, "RL");
    }
};

void drawThrottle(Sketch& s, bool picv) {
    s.risers();
    s.pump(kVl, 42, true);
    s.valve2(kRl, 68, true);
    s.motor(kRl, 68, +1);
    if (picv) s.label(kRl + 16, 80, "PICV", "cb");
    s.boxes();
}

void drawInject2(Sketch& s, bool picv) {
    s.risers();
    s.line("rl", {{kVl, 72}, {kRl, 72}});
    s.pump(kVl, 40, true);
    s.pump(kVl, 96, false);
    s.check(50, 72);
    s.valve2(kRl, 44, true);
    s.motor(kRl, 44, +1);
    if (picv) s.label(kRl + 16, 56, "PICV", "cb");
    s.sensor(kVl, 107, -1);
    s.boxes();
}

void drawMixing(Sketch& s, bool fixed) {
    s.risers();
    const double node = fixed ? 56 : 64;
    s.line("rl", {{kVl, node}, {kRl, node}});
    if (fixed) {
        s.line("rl", {{kVl, 80}, {kRl, 80}});
        s.balancing(50, 80);
    }
    s.valve3(kVl, node, +1);
    s.motor(kVl, node, -1);
    s.pump(kVl, fixed ? 97 : 92, false);
    s.sensor(kVl, 108, -1);
    s.boxes();
}

void drawDiverting(Sketch& s) {
    s.risers();
    s.line("vl", {{kVl, 66}, {kRl, 66}});
    s.balancing(50, 66);
    s.pump(kVl, 38, false);
    s.valve3(kRl, 66, -1);
    s.motor(kRl, 66, +1);
    s.boxes();
}

void drawInject3(Sketch& s) {
    s.risers();
    s.line("vl", {{kVl, 46}, {kRl, 46}});
    s.line("rl", {{kVl, 78}, {kRl, 78}});
    s.balancing(50, 46);
    s.pump(kVl, 31, false);
    s.valve3(kRl, 46, -1);
    s.motor(kRl, 46, +1);
    s.pump(kVl, 96, false);
    s.sensor(kVl, 107, -1);
    s.boxes();
}

// ------------------------------------------------------------------ circuit data

struct Circuit {
    const char* code;
    Text name;
    Text type, dist, vltemp, flowc, flowg, pumps, rlt, valve, func, props, use, note;
};

const Text kFlowCtl{"Durchflussregelung (mengenvariabel)", "Flow control (variable flow)"};
const Text kMixCtl{"Mischregelung (mengenkonstant)", "Mixing control (constant flow)"};
const Text kPressure{"Druckbehaftet", "Pressurised"};
const Text kNoPressure{"Drucklos bzw. druckarm (hydraulische Weiche, Pufferspeicher)",
                       "Unpressurised or low pressure (hydraulic separator, buffer tank)"};
const Text kVlGen{"= Vorlauf des Erzeugers", "= generator flow temperature"};
const Text kVar{"Variabel", "Variable"};
const Text kConst{"Konstant", "Constant"};
const Text kDp{"Δp_V100 ≥ 3 kPa bei Vorlauftemperaturregelung, ≥ 10 kPa bei RLT-Anwendungen; Ventilautorität 0,3 … 0,7 "
               "(Vorlauftemperaturregelung ca. 0,3, RLT ca. 0,6). ",
               "Δp_V100 ≥ 3 kPa for flow temperature control, ≥ 10 kPa for air handling; valve authority 0.3 … 0.7 "
               "(flow temperature control approx. 0.3, air handling approx. 0.6). "};
const Text kBalance{"Bei mehreren Verbrauchern hydraulischer Abgleich mit Einstellventilen nötig (abgeglichen nur bei Volllast) – "
                    "druckunabhängige Ventile (PICV) sind bei Voll- und Teillast abgeglichen.",
                    "With several consumers hydraulic balancing with regulating valves is required (balanced at full load "
                    "only) – pressure-independent valves (PICV) stay balanced at full and part load."};
const Text kPicv{"Druckunabhängiges Regelventil (PICV): Der eingebaute Differenzdruckregler hält die Verhältnisse am Ventil "
                 "stabil – die Pumpe muss dafür den Mindestdifferenzdruck p_min liefern. ",
                 "Pressure-independent control valve (PICV): the built-in differential pressure regulator keeps conditions at "
                 "the valve stable – the pump must provide the minimum differential pressure p_min. "};

std::vector<Circuit> circuits() {
    const Text throttleUse{
        "Lufterwärmer ohne Einfriergefahr, Luftkühler mit Entfeuchtung, Warmwasserspeicher-Ladung, Fernwärmeanschluss (direkt "
        "oder mit Wärmeübertrager), Speicherladung/-entladung, Anlagen mit Brennwertkessel, Zonenregelung, Heizkörper. "
        "Nicht für Vorerhitzer mit Frostgefahr.",
        "Air heaters without risk of freezing, air coolers with dehumidification, DHW tank charging, district heating "
        "connection (direct or via heat exchanger), storage charging/discharging, condensing boiler plants, zone control, "
        "radiators. Not for pre-heaters at risk of frost."};
    const Text throttleProps{
        "Variabler Volumenstrom in der ganzen Anlage · tiefe Rücklauftemperatur bei Teillast (Heizen) · Totzeit beim Anfahren "
        "je nach Rohrlänge · bei geschlossenem Ventil kann die Pumpe überhitzen → drehzahlgeregelte Pumpe.",
        "Variable flow in the whole plant · low return temperature at part load (heating) · dead time on start-up depending "
        "on pipe length · pump may overheat with the valve closed → speed-controlled pump."};
    const Text inject2Use{
        "Wärmespeicher und Wärmepumpen, Brennwert- und Niedertemperaturkessel, direkte Fernwärmeanschlüsse, Radiator- und "
        "Fußbodenheizung, witterungsgeführte Vorlauftemperaturregelung, Lufterhitzer, Luftkühler ohne/mit Entfeuchtung.",
        "Thermal storage and heat pumps, condensing and low-temperature boilers, direct district heating connections, "
        "radiator and underfloor heating, weather-compensated flow temperature control, air heaters, air coolers with or "
        "without dehumidification."};
    const Text inject2Props{
        "Konstanter Volumenstrom mit variabler Temperatur beim Verbraucher · stark schwankender Volumenstrom und Druck im "
        "Erzeugerkreis (bei mehreren Gruppen beachten) · gleichmäßige Temperaturverteilung · geringe Einfriergefahr · bei "
        "geschlossenem Ventil kann die Erzeugerpumpe überhitzen → drehzahlgeregelt.",
        "Constant flow with variable temperature at the consumer · strongly varying flow and pressure in the generator "
        "circuit (consider with several groups) · even temperature across the consumer · low risk of freezing · generator "
        "pump may overheat with the valve closed → speed-controlled."};
    return {
        {"throttle",
         {"Drosselschaltung", "Throttling circuit"},
         kFlowCtl, kPressure, kVlGen, kVar, kVar,
         {"Erzeugerpumpe (drehzahlgeregelt empfohlen)", "Generator pump (speed control recommended)"},
         {"Tief bei Teillast (Heizen)", "Low at part load (heating)"},
         cat(kDp, Text{"Durchgangsventil meist im Rücklauf. ", "Two-way valve usually in the return. "}, kBalance),
         {"Das Durchgangsventil verändert den Volumenstrom durch Verbraucher und Erzeuger gemeinsam; die Druckverhältnisse "
          "ändern sich in der ganzen Anlage.",
          "The two-way valve changes the flow through consumer and generator together; pressures change throughout the "
          "plant."},
         throttleProps, throttleUse,
         {"Braucht einen druckbehafteten Verteiler (Erzeugerpumpe fördert über den Verbraucher).",
          "Needs a pressurised distributor (the generator pump drives the flow through the consumer)."}},
        {"throttle_picv",
         {"Drosselschaltung mit PICV", "Throttling circuit with PICV"},
         kFlowCtl, kPressure, kVlGen, kVar, kVar,
         {"Erzeugerpumpe (drehzahlgeregelt)", "Generator pump (speed controlled)"},
         {"Hoch bei Teillast (Kühlen), tief bei Teillast (Heizen)", "High at part load (cooling), low at part load (heating)"},
         cat(kPicv, Text{"Kein Abgleich mit Einstellventilen nötig.", "No balancing with regulating valves required."}),
         {"Wie die Drosselschaltung; das PICV regelt den Durchfluss unabhängig vom Differenzdruck der Anlage.",
          "Like the throttling circuit; the PICV controls the flow independently of the plant differential pressure."},
         throttleProps, throttleUse,
         {"Typisch: Luftkühler mit Entfeuchtung.", "Typical: air cooler with dehumidification."}},
        {"inject2",
         {"Einspritzschaltung mit Durchgangsventil", "Injection circuit with two-way valve"},
         kMixCtl, kPressure, kVar, kConst, kVar,
         {"Erzeuger- und Verbraucherpumpe", "Generator and consumer pump"},
         {"Relativ tief (Heizen)", "Relatively low (heating)"},
         cat(kDp, Text{"Rückschlagklappe im Bypass, Bypass gleiche DN wie Hauptleitung. Warmhaltung der Zubringerleitung: "
                       "kleines Ventil im Bypass, DN 15 meist ausreichend. ",
                       "Non-return valve in the bypass, bypass same DN as the main pipe. Keeping the supply line warm: small "
                       "valve in the bypass, DN 15 usually sufficient. "},
             kBalance),
         {"Die Erzeugerpumpe spritzt je nach Ventilstellung mehr oder weniger heißes Vorlaufwasser in den Verbraucherkreis "
          "ein; die Verbraucherpumpe fördert konstant über den Verbraucher.",
          "Depending on the valve position the generator pump injects more or less hot supply water into the consumer "
          "circuit; the consumer pump circulates a constant flow through the consumer."},
         inject2Props, inject2Use,
         {"Wirkt wie eine Beimischschaltung, jedoch mit Erzeugerpumpe (druckbehafteter Verteiler).",
          "Works like a mixing circuit, but with a generator pump (pressurised distributor)."}},
        {"inject_picv",
         {"Einspritzschaltung mit PICV", "Injection circuit with PICV"},
         kMixCtl, kPressure, kVar, kConst, kVar,
         {"Erzeuger- und Verbraucherpumpe", "Generator and consumer pump"},
         {"Relativ tief (Heizen)", "Relatively low (heating)"},
         cat(kPicv, Text{"Rückschlagklappe im Bypass, Bypass gleiche DN wie Hauptleitung.",
                         "Non-return valve in the bypass, bypass same DN as the main pipe."}),
         {"Wie die Einspritzschaltung mit Durchgangsventil; das PICV hält den eingespritzten Durchfluss unabhängig vom "
          "Netzdruck.",
          "Like the injection circuit with two-way valve; the PICV keeps the injected flow independent of the network "
          "pressure."},
         inject2Props, inject2Use,
         {"Typisch: Lufterwärmer.", "Typical: air heater."}},
        {"mixing",
         {"Beimischschaltung", "Mixing circuit"},
         kMixCtl, kNoPressure, kVar, kConst, kVar,
         {"Nur Verbraucherpumpe", "Consumer pump only"},
         {"Tief bei kleiner Last (Heizen), hoch bei kleiner Last (Kühlen)", "Low at low load (heating), high at low load (cooling)"},
         cat(kDp, Text{"Dreiweg-Mischventil im Vorlauf, Bypass gleiche DN wie Hauptleitung.",
                       "Three-way mixing valve in the supply, bypass same DN as the main pipe."}),
         {"Das Dreiwegventil mischt heißes Erzeugerwasser und abgekühltes Rücklaufwasser auf die gewünschte "
          "Vorlauftemperatur; die Verbraucherpumpe fördert konstant.",
          "The three-way valve mixes hot generator water and cooled return water to the required flow temperature; the "
          "consumer pump circulates a constant flow."},
         {"Konstanter Volumenstrom mit variabler Temperatur beim Verbraucher · variabler Volumenstrom im Erzeugerkreis · "
          "gleichmäßige Temperaturverteilung · geringe Einfriergefahr · nicht geeignet bei mehr als 20 m zwischen Bypass "
          "und Vorlauffühler (Totzeit).",
          "Constant flow with variable temperature at the consumer · variable flow in the generator circuit · even "
          "temperature distribution · low risk of freezing · not suitable with more than 20 m between bypass and flow "
          "sensor (dead time)."},
         {"Einfache Heizungsgruppen, witterungsgeführte Vorlauftemperaturregelung, Lufterwärmer mit Einfriergefahr, "
          "Luftkühler ohne Entfeuchtung. Nur bei geringem wasserseitigem Druckverlust des Erzeugers oder nach hydraulischer "
          "Weiche bzw. Pufferspeicher.",
          "Simple heating groups, weather-compensated flow temperature control, air heaters at risk of freezing, air "
          "coolers without dehumidification. Only with low water-side pressure drop of the generator or after a hydraulic "
          "separator or buffer tank."},
         {"Fließt im Auslegungsfall mehr als 25 % über Tor B (Bypass), Beimischschaltung mit fester Vormischung wählen.",
          "If more than 25 % flows through port B (bypass) at design, choose the mixing circuit with fixed premix."}},
        {"mixing_fixed",
         {"Beimischschaltung mit fester Vormischung", "Mixing circuit with fixed premix"},
         kMixCtl, kNoPressure,
         {"Variabel, deutlich unter dem Erzeuger-Vorlauf", "Variable, well below the generator flow temperature"},
         kConst, kVar,
         {"Nur Verbraucherpumpe", "Consumer pump only"},
         {"Allgemein tief", "Generally low"},
         cat(kDp, Text{"Feste Vormischung über Bypass mit Einstellventil einregulieren.",
                       "Set the fixed premix with the regulating valve in the bypass."}),
         {"Wie die Beimischschaltung; zusätzlich wird über den festen Bypass immer ein Anteil Rücklaufwasser beigemischt, "
          "damit das Dreiwegventil über den ganzen Hub arbeitet.",
          "Like the mixing circuit; in addition a fixed bypass always adds a share of return water so the three-way valve "
          "works over its full stroke."},
         {"Stellglied arbeitet über den gesamten Hub · konstanter Volumenstrom beim Verbraucher · variabler Volumenstrom im "
          "Erzeugerkreis · nicht geeignet bei mehr als 20 m zwischen Bypass und Fühler.",
          "Valve works over its full stroke · constant flow at the consumer · variable flow in the generator circuit · not "
          "suitable with more than 20 m between bypass and sensor."},
         {"Fußboden-, Wand- und Deckenheizung, Betonkerntemperierung, Heizkörper mit Hochtemperatur-Erzeuger (z. B. "
          "Holzkessel).",
          "Underfloor, wall and ceiling heating, concrete core activation, radiators with high-temperature generators "
          "(e.g. wood boiler)."},
         {"Auch „Bypassschaltung“ genannt.", "Also called “bypass circuit”."}},
        {"diverting",
         {"Umlenkschaltung", "Diverting circuit"},
         kFlowCtl, kPressure, kVlGen, kVar, kConst,
         {"Erzeugerpumpe", "Generator pump"},
         {"Heizen: mittel bis hoch · Kühlen: mittel bis tief", "Heating: medium to high · cooling: medium to low"},
         cat(Text{"Δp_V100 = Δp des Verbrauchers; Bypass mit Einstellventil auf den Druckverlust des Verbrauchers "
                  "einregulieren. Dreiwegventil im Rücklauf. ",
                  "Δp_V100 = Δp of the consumer; set the regulating valve in the bypass to the consumer's pressure drop. "
                  "Three-way valve in the return. "},
             kBalance),
         {"Der Vorlauf wird je nach Ventilstellung zwischen Verbraucher und Bypass aufgeteilt; die Leistung wird über den "
          "Durchfluss geregelt. Bei geschlossenem Ventil sind Vor- und Rücklauf annähernd gleich warm.",
          "Depending on the valve position the supply is split between consumer and bypass; the output is controlled by the "
          "flow. With the valve closed supply and return are almost equally warm."},
         {"Variabler Durchfluss im Verbraucher · konstanter Durchfluss und Druck im Erzeugerkreis · beim Anfahren steht die "
          "Vorlauftemperatur schnell am Verbraucher an.",
          "Variable flow in the consumer · constant flow and pressure in the generator circuit · on start-up the flow "
          "temperature reaches the consumer quickly."},
         {"Luftkühler mit Entfeuchtung, Lufterwärmer ohne Einfriergefahr bzw. ohne Außenluftanteil, Wärmerückgewinnung, "
          "Brauchwassererwärmung, Nachwärmer, Schwimmbeckenaufheizung.",
          "Air coolers with dehumidification, air heaters without risk of freezing or without outdoor air, heat recovery, "
          "domestic hot water heating, reheaters, pool heating."},
         {"Nicht für Fernwärme und Brennwerttechnik (hohe Rücklauftemperatur); in Heizungen kaum noch eingesetzt.",
          "Not for district heating and condensing technology (high return temperature); rarely used in heating today."}},
        {"inject3",
         {"Einspritzschaltung mit Dreiwegventil", "Injection circuit with three-way valve"},
         kMixCtl, kPressure, kVar, kConst, kConst,
         {"Erzeuger- und Verbraucherpumpe", "Generator and consumer pump"},
         {"Relativ hoch (bei 0 % Last = Erzeuger-Vorlauf)", "Relatively high (at 0 % load = generator flow temperature)"},
         Text{"Δp_V100 ≥ 3 kPa; Bypass im Verbraucherkreis gleiche DN wie Hauptleitung; primären Bypass einregulieren.",
              "Δp_V100 ≥ 3 kPa; consumer bypass same DN as the main pipe; balance the primary bypass."},
         {"Die Erzeugerpumpe spritzt über das Dreiwegventil heißes Wasser in den Verbraucherkreis ein; die Verbraucherpumpe "
          "saugt über den Bypass abgekühltes Rücklaufwasser an.",
          "The generator pump injects hot water into the consumer circuit through the three-way valve; the consumer pump "
          "draws cooled return water through the bypass."},
         {"Konstanter Durchfluss in Erzeuger- und Verbraucherkreis · variabler Durchfluss im Bypass · gleichmäßige "
          "Temperaturverteilung · geringe Einfriergefahr bei Lufterwärmern.",
          "Constant flow in generator and consumer circuit · variable flow in the bypass · even temperature distribution · "
          "low risk of freezing for air heaters."},
         {"Lufterwärmer mit Einfriergefahr (Außenluft), Luftkühler ohne geregelte Entfeuchtung, Radiator- und "
          "Fußbodenheizung, Warmwasserspeicher-Ladung.",
          "Air heaters at risk of freezing (outdoor air), air coolers without controlled dehumidification, radiator and "
          "underfloor heating, DHW tank charging."},
         {"Nicht für Fernwärme und Brennwerttechnik (hohe Rücklauftemperatur); wird kaum noch eingesetzt.",
          "Not for district heating and condensing technology (high return temperature); rarely used today."}},
    };
}

const Circuit& findCircuit(const std::vector<Circuit>& all, const std::string& code) {
    for (const auto& c : all)
        if (code == c.code) return c;
    return all.front();
}

Chart drawCircuit(const std::string& code) {
    Sketch s;
    if (code == "throttle") drawThrottle(s, false);
    else if (code == "throttle_picv") drawThrottle(s, true);
    else if (code == "inject2") drawInject2(s, false);
    else if (code == "inject_picv") drawInject2(s, true);
    else if (code == "mixing") drawMixing(s, false);
    else if (code == "mixing_fixed") drawMixing(s, true);
    else if (code == "diverting") drawDiverting(s);
    else drawInject3(s);
    s.ch.x = {"", 0, kW, 10};
    s.ch.y = {"", 0, kH, 10};
    s.ch.diagram = true;
    s.ch.aspect = 1.18;
    s.ch.note = {"Rot = Vorlauf (VL), blau = Rücklauf (RL), M = Stellantrieb, T = Temperaturfühler. Eigene Prinzipskizze.",
                 "Red = supply (VL), blue = return (RL), M = actuator, T = temperature sensor. Own schematic."};
    return s.ch;
}

/// Selection help → circuit code and reasoning
struct Choice {
    std::string code;
    Text why;
    std::string alt;
};

Choice select(const Inputs& in, Result& r) {
    const bool pressure = in.choice("dist") != "unpressurised";
    const bool frost = in.choice("frost") == "yes";
    const bool dehum = in.choice("dehum") == "yes";
    const bool lowrl = in.choice("lowrl") == "yes";
    const bool lowvl = in.choice("lowvl") == "yes";
    const bool constgen = in.choice("constgen") == "yes";
    const auto& control = in.choice("control");
    auto needPressure = [&] {
        if (!pressure) {
            r.warn({"Durchflussregelung braucht einen druckbehafteten Verteiler (Erzeugerpumpe) – sonst Mischregelung wählen.",
                    "Flow control needs a pressurised distributor (generator pump) – otherwise choose mixing control."});
        }
    };
    if (dehum) {
        needPressure();
        const bool div = !lowrl && constgen;
        return {div ? "diverting" : "throttle",
                {"Entfeuchtung braucht das kalte Wasser mit voller Vorlauftemperatur am Kühler → Durchflussregelung.",
                 "Dehumidification needs cold water at full supply temperature at the coil → flow control."},
                div ? "throttle" : "diverting"};
    }
    if (frost || control == "mix" || (control != "flow" && lowvl)) {
        Text why = frost ? Text{"Einfriergefahr: konstanter Durchfluss durch das Register → Mischregelung. ",
                                "Risk of freezing: constant flow through the coil → mixing control. "}
                         : Text{"Konstanter Durchfluss mit variabler Vorlauftemperatur → Mischregelung. ",
                                "Constant flow with variable flow temperature → mixing control. "};
        if (!pressure) {
            return {lowvl ? "mixing_fixed" : "mixing",
                    cat(why, Text{"Drucklos/druckarm → Beimischschaltung (nur Verbraucherpumpe).",
                                  "Unpressurised/low pressure → mixing circuit (consumer pump only)."}),
                    lowvl ? "mixing" : "mixing_fixed"};
        }
        const bool two = lowrl || lowvl;
        return {two ? "inject2" : "inject3",
                cat(why, two ? Text{"Druckbehaftet und tiefer Rücklauf gewünscht → Einspritzschaltung mit Durchgangsventil.",
                                    "Pressurised and low return wanted → injection circuit with two-way valve."}
                             : Text{"Druckbehaftet, konstanter Durchfluss auch im Erzeugerkreis → Einspritzschaltung mit "
                                    "Dreiwegventil.",
                                    "Pressurised, constant flow also in the generator circuit → injection circuit with "
                                    "three-way valve."}),
                two ? "inject3" : "inject2"};
    }
    if (control == "flow") {
        needPressure();
        const bool div = !lowrl && constgen;
        return {div ? "diverting" : "throttle",
                div ? Text{"Mengenvariabel beim Verbraucher, aber konstanter Durchfluss im Erzeugerkreis → Umlenkschaltung.",
                           "Variable flow at the consumer but constant flow in the generator circuit → diverting circuit."}
                    : Text{"Mengenvariabel mit tiefem Rücklauf → Drosselschaltung (drehzahlgeregelte Erzeugerpumpe).",
                           "Variable flow with low return → throttling circuit (speed-controlled generator pump)."},
                div ? "throttle" : "throttle_picv"};
    }
    if (!pressure) {
        return {"mixing",
                {"Drucklos/druckarm ohne besondere Anforderung → Beimischschaltung.",
                 "Unpressurised/low pressure without special requirement → mixing circuit."},
                "mixing_fixed"};
    }
    return {"inject2",
            {"Druckbehaftet ohne besondere Anforderung → Einspritzschaltung mit Durchgangsventil (tiefer Rücklauf, konstanter "
             "Durchfluss beim Verbraucher).",
             "Pressurised without special requirement → injection circuit with two-way valve (low return, constant flow at "
             "the consumer)."},
            "throttle"};
}

std::vector<Option> circuitOptions() {
    std::vector<Option> o;
    for (const auto& c : circuits()) o.push_back({c.code, c.name});
    return o;
}

std::vector<Option> yesNo() { return {{"no", {"Nein", "No"}}, {"yes", {"Ja", "Yes"}}}; }

const char* kSource =
    "Quellen: Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022), Kap. 5.4–5.5, S. 144–153; Belimo "
    "„Auswahltabelle von hydraulischen Grundschaltungen“ (03/2023). Prinzipskizzen ohne Absperr- und Sicherheitsarmaturen; "
    "Auslegung immer projektbezogen prüfen.";
const char* kSourceEn =
    "Sources: Siemens “Einführung in die HLK- und Gebäudetechnik” (08/2022), ch. 5.4–5.5, p. 144–153; Belimo selection "
    "table of hydraulic basic circuits (03/2023). Schematics without isolating and safety fittings; always check the "
    "design for the project.";

Calculator hydraulicCircuits() {
    Calculator c;
    c.id = "hydraulic_circuits";
    c.category = "building";
    c.title = {"Hydraulische Grundschaltungen (Regelgruppen)", "Hydraulic basic circuits (control groups)"};
    c.summary = {"Schema, Funktion, Einsatz und Ventilauslegung – mit Auswahlhilfe",
                 "Schematic, function, applications and valve sizing – with selection help"};
    c.description = {
        std::string("Leistung Q̇ ~ V̇ · ΔT: Durchflussregelung (mengenvariabel: Drossel-, Umlenkschaltung) ändert den Volumenstrom, "
                    "Mischregelung (mengenkonstant: Beimisch-, Einspritzschaltung) die Vorlauftemperatur.\n"
                    "Auswahl nach Verteiler (druckbehaftet: Drossel, Umlenk, Einspritz · drucklos: Beimisch), Frostgefahr "
                    "(mengenkonstant), Entfeuchtung (mengenvariabel) und gewünschter Rücklauftemperatur (Brennwert/Fernwärme: "
                    "Drossel oder Einspritz mit Durchgangsventil).\n") + kSource,
        std::string("Output Q̇ ~ V̇ · ΔT: flow control (variable flow: throttling, diverting) changes the flow rate, mixing control "
                    "(constant flow: mixing, injection) changes the flow temperature.\n"
                    "Choose by distributor (pressurised: throttling, diverting, injection · unpressurised: mixing), risk of "
                    "frost (constant flow), dehumidification (variable flow) and required return temperature (condensing/"
                    "district heating: throttling or injection with two-way valve).\n") + kSourceEn};
    c.inputs = {
        Num::choice("mode", {"Anzeigen", "Show"},
                    {{"view", {"Schaltung auswählen", "Choose circuit"}},
                     {"help", {"Auswahlhilfe nach Anforderungen", "Selection help by requirements"}}},
                    "view"),
        Num::choice("circuit", {"Schaltung", "Circuit"}, circuitOptions(), "mixing").showIf("mode", {"view"}),
        Num::choice("dist", {"Verteiler", "Distributor"},
                    {{"pressure", {"Druckbehaftet (Pumpe im Erzeugerkreis, Fernwärme)", "Pressurised (pump in generator circuit, district heating)"}},
                     {"unpressurised", {"Drucklos/druckarm (hydraulische Weiche, Puffer)", "Unpressurised (hydraulic separator, buffer)"}}},
                    "pressure")
            .showIf("mode", {"help"}),
        Num::choice("control", {"Leistung anpassen über", "Adjust output by"},
                    {{"auto", {"Weiß nicht – nach Anwendung", "Don't know – by application"}},
                     {"flow", {"Durchfluss (mengenvariabel)", "Flow rate (variable flow)"}},
                     {"mix", {"Vorlauftemperatur (mengenkonstant)", "Flow temperature (constant flow)"}}},
                    "auto")
            .showIf("mode", {"help"}),
        Num::choice("frost", {"Lufterwärmer mit Einfriergefahr", "Air heater at risk of freezing"}, yesNo(), "no").showIf("mode", {"help"}),
        Num::choice("dehum", {"Luftkühler mit Entfeuchtung", "Air cooler with dehumidification"}, yesNo(), "no").showIf("mode", {"help"}),
        Num::choice("lowrl", {"Tiefe Rücklauftemperatur nötig (Brennwert, Fernwärme, Wärmepumpe)", "Low return temperature needed (condensing, district heating, heat pump)"},
                    yesNo(), "yes")
            .showIf("mode", {"help"}),
        Num::choice("lowvl", {"Verbraucher-VL deutlich tiefer als Erzeuger-VL (Flächenheizung)", "Consumer flow well below generator flow (surface heating)"},
                    yesNo(), "no")
            .showIf("mode", {"help"}),
        Num::choice("constgen", {"Konstanter Durchfluss im Erzeugerkreis nötig", "Constant flow needed in generator circuit"}, yesNo(), "no")
            .showIf("mode", {"help"}),
    };
    c.outputs = {Out::text("name", {"Schaltung", "Circuit"}).primary(),
                 Out::text("why", {"Begründung", "Reasoning"}),
                 Out::text("alt", {"Alternative", "Alternative"}),
                 Out::text("type", {"Regelungsart", "Control type"}).primary().abbrev({"Art", "Type"}),
                 Out::text("dist", {"Verteiler", "Distributor"}),
                 Out::text("vltemp", {"Vorlauftemperatur am Verbraucher", "Flow temperature at consumer"}),
                 Out::text("flowc", {"Durchfluss Verbraucherkreis", "Flow in consumer circuit"}),
                 Out::text("flowg", {"Durchfluss Erzeugerkreis", "Flow in generator circuit"}),
                 Out::text("pumps", {"Pumpen", "Pumps"}),
                 Out::text("rlt", {"Rücklauftemperatur zum Erzeuger", "Return temperature to generator"}),
                 Out::text("func", {"Funktionsweise", "How it works"}),
                 Out::text("props", {"Eigenschaften", "Characteristics"}),
                 Out::text("use", {"Typische Anwendungen", "Typical applications"}),
                 Out::text("valve", {"Ventil- und Bypassauslegung", "Valve and bypass sizing"}),
                 Out::text("note", {"Hinweis", "Note"})};
    c.keywords = {"hydraulik", "grundschaltung", "regelgruppe", "beimischschaltung", "einspritzschaltung", "drosselschaltung",
                  "umlenkschaltung", "bypassschaltung", "mischventil", "dreiwegventil", "durchgangsventil", "picv",
                  "heizgruppe", "hydraulic circuit", "mixing circuit", "injection", "diverting", "belimo"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto all = circuits();
        std::string code = in.choice("circuit");
        if (in.choice("mode") == "help") {
            const auto ch = select(in, r);
            code = ch.code;
            r.setText("why", ch.why);
            r.setText("alt", findCircuit(all, ch.alt).name);
        }
        const auto& k = findCircuit(all, code);
        r.setText("name", k.name);
        r.setText("type", k.type);
        r.setText("dist", k.dist);
        r.setText("vltemp", k.vltemp);
        r.setText("flowc", k.flowc);
        r.setText("flowg", k.flowg);
        r.setText("pumps", k.pumps);
        r.setText("rlt", k.rlt);
        r.setText("func", k.func);
        r.setText("props", k.props);
        r.setText("use", k.use);
        r.setText("valve", k.valve);
        r.setText("note", k.note);
        r.info({"Prinzipschaltung – Absperr-, Entleer- und Sicherheitsarmaturen sowie Ausdehnung sind nicht dargestellt.",
                "Schematic only – isolating, drain and safety fittings and expansion are not shown."});
        return r;
    };
    c.chart = [](const Inputs& in, const Result& r, const Context&) {
        // in help mode the recommended circuit is drawn: find its code by name
        std::string code = in.choice("circuit");
        if (in.choice("mode") == "help") {
            const auto it = r.texts.find("name");
            for (const auto& k : circuits())
                if (it != r.texts.end() && k.name.de == it->second.de) code = k.code;
        }
        return drawCircuit(code);
    };
    return c;
}

Table hydraulicSelection() {
    Table t;
    t.id = "hydraulic_selection";
    t.category = "building";
    t.title = {"Auswahltabelle hydraulische Grundschaltungen", "Selection table hydraulic basic circuits"};
    t.summary = {"Verteiler, Durchfluss, Pumpen, Rücklauf, Anwendung je Schaltung", "Distributor, flow, pumps, return, application per circuit"};
    t.description = {kSource, kSourceEn};
    t.columns = {{"Schaltung", "Circuit"},          {"Regelung", "Control"},       {"Verteiler", "Distributor"},
                 {"VL am Verbraucher", "Consumer flow temp."}, {"Durchfluss Verbraucher / Erzeuger", "Flow consumer / generator"},
                 {"Pumpen", "Pumps"},               {"Rücklauf", "Return"},        {"Typisch für", "Typical for"}};
    auto shortDist = [](const Circuit& k) {
        return k.dist.de.rfind("Druckbehaftet", 0) == 0 ? Text{"druckbehaftet", "pressurised"} : Text{"drucklos", "unpressurised"};
    };
    const Text typical[] = {
        {"Zonen, Heizkörper, Luftkühler mit Entfeuchtung", "Zones, radiators, coolers with dehumidification"},
        {"Luftkühler mit Entfeuchtung", "Coolers with dehumidification"},
        {"Wärmepumpe, Brennwert, Fußbodenheizung", "Heat pump, condensing, underfloor heating"},
        {"Lufterwärmer", "Air heaters"},
        {"Heizgruppen, Lufterwärmer mit Frostgefahr", "Heating groups, air heaters at frost risk"},
        {"Fußboden-/Wandheizung, Betonkern", "Underfloor/wall heating, concrete core"},
        {"Nachwärmer, WRG, Kühler mit Entfeuchtung", "Reheaters, heat recovery, coolers with dehumidification"},
        {"Lufterwärmer mit Außenluft", "Air heaters with outdoor air"},
    };
    std::size_t i = 0;
    for (const auto& k : circuits()) {
        const bool flow = k.type.de.rfind("Durchfluss", 0) == 0;
        t.rows.push_back({k.name,
                          flow ? Text{"mengenvariabel", "variable flow"} : Text{"mengenkonstant", "constant flow"},
                          shortDist(k),
                          k.vltemp.de.rfind("=", 0) == 0 ? Text{"= Erzeuger-VL", "= generator"} : Text{"variabel", "variable"},
                          cat(k.flowc, " / ", k.flowg),
                          k.pumps,
                          k.rlt,
                          typical[i++]});
    }
    return t;
}

}  // namespace

void addHydraulics(std::vector<Calculator>& out) { out.push_back(hydraulicCircuits()); }
void addHydraulicTables(std::vector<Table>& out) { out.push_back(hydraulicSelection()); }

}  // namespace elc::catalog
