// Calculators: Motoren & Antriebe / Motors & drives
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/motors.hpp"

namespace elc::catalog {

namespace {

Calculator motorCurrent() {
    Calculator c;
    c.id = "motor_current";
    c.category = "motors";
    c.title = {"Motor-Bemessungsstrom", "Motor rated current"};
    c.summary = {"I_N aus P_N, U, cos φ und η", "I_N from P_N, U, cos φ and η"};
    c.description = {"I_N = P_N / (k · U · cos φ · η)    k = √3 (3~) bzw. 1 (1~)\n"
                     "P_N = abgegebene mechanische Leistung laut Typenschild.\nP_el = P_N / η",
                     "I_N = P_N / (k · U · cos φ · η)    k = √3 (3~) or 1 (1~)\n"
                     "P_N = rated mechanical output from the nameplate.\nP_el = P_N / η"};
    c.inputs = {
        Num::choice("sys", {"Netz", "System"}, systemOptions(false), "ac3"),
        Num::number("P", {"Bemessungsleistung P_N", "Rated output P_N"}, "kW", 7.5).positive(),
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
        cosPhiInput(0.85),
        Num::number("eta", {"Wirkungsgrad η", "Efficiency η"}, "%", 90).rangeExclusiveMin(0, 100),
    };
    c.outputs = {Out::number("I", {"Bemessungsstrom I_N", "Rated current I_N"}, "A", 2).primary(),
                 Out::number("Pel", {"Aufgenommene Wirkleistung P_el", "Active input power P_el"}, "kW", 2),
                 Out::number("S", {"Scheinleistung S", "Apparent power S"}, "kVA", 2),
                 Out::number("Q", {"Blindleistung Q", "Reactive power Q"}, "kvar", 2)};
    c.keywords = {"motor", "nennstrom", "bemessungsstrom", "rated current"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const double eta = in.num("eta") / 100.0;
        const double cosPhi = in.num("cosphi");
        const double I = motors::ratedCurrent(sys, in.num("P") * 1e3, in.num("U"), cosPhi, eta);
        const double Pel = in.num("P") / eta;
        const double S = Pel / cosPhi;
        r.set("I", I);
        r.set("Pel", Pel);
        r.set("S", S);
        r.set("Q", S * std::sin(std::acos(cosPhi)));
        return r;
    };
    return c;
}

Calculator torque() {
    Calculator c;
    c.id = "torque";
    c.category = "motors";
    c.title = {"Drehmoment · Leistung · Drehzahl", "Torque · power · speed"};
    c.summary = {"M = 9549 · P / n – zwei Werte eingeben", "M = 9549 · P / n – enter any two"};
    c.description = {"M [Nm] = 9549,3 · P [kW] / n [1/min]    (P = 2π · n · M / 60)\nGenau zwei Werte eingeben.",
                     "M [Nm] = 9549.3 · P [kW] / n [1/min]    (P = 2π · n · M / 60)\nEnter exactly two values."};
    c.inputs = {
        Num::number("P", {"Leistung P", "Power P"}, "kW", 7.5).opt().positive(),
        Num::number("n", {"Drehzahl n", "Speed n"}, "1/min", 1450).opt().positive(),
        Num::number("M", {"Drehmoment M", "Torque M"}, "Nm", kNaN).opt().positive(),
    };
    c.outputs = {Out::number("P", {"Leistung P", "Power P"}, "kW", 3), Out::number("n", {"Drehzahl n", "Speed n"}, "1/min", 1),
                 Out::number("M", {"Drehmoment M", "Torque M"}, "Nm", 2).primary()};
    c.keywords = {"drehmoment", "drehzahl", "torque", "speed", "nm"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto t = motors::solveTorque(in.maybe("P"), in.maybe("n"), in.maybe("M"));
        if (!t) {
            r.error({"Bitte genau zwei Werte eingeben.", "Please enter exactly two values."});
            return r;
        }
        r.set("P", t->P_kW);
        r.set("n", t->n_rpm);
        r.set("M", t->M_Nm);
        return r;
    };
    return c;
}

Calculator motorSpeed() {
    Calculator c;
    c.id = "motor_speed";
    c.category = "motors";
    c.title = {"Drehzahl & Schlupf", "Speed & slip"};
    c.summary = {"Synchrondrehzahl, Schlupf, Läuferfrequenz", "Synchronous speed, slip, rotor frequency"};
    c.description = {"n_s = 120 · f / Polzahl = 60 · f / p    s = (n_s − n) / n_s    f_2 = s · f",
                     "n_s = 120 · f / poles = 60 · f / p    s = (n_s − n) / n_s    f_2 = s · f"};
    std::vector<Option> poles;
    for (int p : {2, 4, 6, 8, 10, 12}) {
        const auto s = std::to_string(p);
        poles.push_back({s, {s + "-polig (p = " + std::to_string(p / 2) + ")", s + "-pole (p = " + std::to_string(p / 2) + ")"}});
    }
    c.inputs = {
        Num::number("f", {"Frequenz f", "Frequency f"}, "Hz", 50).positive(),
        Num::choice("poles", {"Polzahl", "Number of poles"}, poles, "4"),
        Num::number("n", {"Bemessungsdrehzahl n", "Rated speed n"}, "1/min", 1450).opt().nonNegative(),
    };
    c.outputs = {Out::number("ns", {"Synchrondrehzahl n_s", "Synchronous speed n_s"}, "1/min", 0).primary(),
                 Out::number("s", {"Schlupf s", "Slip s"}, "%", 2),
                 Out::number("f2", {"Läuferfrequenz f_2", "Rotor frequency f_2"}, "Hz", 2)};
    c.keywords = {"drehzahl", "schlupf", "polpaar", "slip", "rpm", "frequenzumrichter"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double ns = motors::synchronousSpeed(in.num("f"), parseDouble(in.choice("poles")));
        r.set("ns", ns);
        if (const auto n = in.maybe("n")) {
            const double s = motors::slip(ns, *n);
            r.set("s", s * 100.0);
            r.set("f2", s * in.num("f"));
            if (s < 0) r.info({"Negativer Schlupf: Generatorbetrieb.", "Negative slip: generator operation."});
        }
        return r;
    };
    return c;
}

Calculator efficiency() {
    Calculator c;
    c.id = "motor_efficiency";
    c.category = "motors";
    c.title = {"Wirkungsgrad aus Messung", "Efficiency from measurement"};
    c.summary = {"η aus U, I, cos φ und Abgabeleistung", "η from U, I, cos φ and output power"};
    c.description = {"P_el = k · U · I · cos φ    η = P_mech / P_el    P_V = P_el − P_mech",
                     "P_el = k · U · I · cos φ    η = P_mech / P_el    P_loss = P_el − P_mech"};
    c.inputs = {
        Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac3"),
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
        Num::number("I", {"Strom I", "Current I"}, "A", 15).positive(),
        cosPhiInput(0.85),
        Num::number("Pmech", {"Abgegebene Leistung P_mech", "Output power P_mech"}, "kW", 7.5).positive(),
    };
    c.outputs = {Out::number("Pel", {"Aufgenommene Leistung P_el", "Input power P_el"}, "kW", 3),
                 Out::number("eta", {"Wirkungsgrad η", "Efficiency η"}, "%", 1).primary(),
                 Out::number("Pv", {"Verluste P_V", "Losses P_loss"}, "kW", 3)};
    c.keywords = {"wirkungsgrad", "efficiency", "verluste", "losses", "ie3"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const double cosPhi = in.has("cosphi") ? in.num("cosphi") : 1.0;
        const double Pel = powerFactorK(sys) * in.num("U") * in.num("I") * (sys == System::DC ? 1.0 : cosPhi) / 1e3;
        const double eta = in.num("Pmech") / Pel;
        r.set("Pel", Pel);
        r.set("eta", eta * 100.0);
        r.set("Pv", Pel - in.num("Pmech"));
        if (eta > 1.0) r.error({"η > 100 % – Eingaben prüfen.", "η > 100 % – check inputs."});
        return r;
    };
    return c;
}

Calculator starDelta() {
    Calculator c;
    c.id = "star_delta";
    c.category = "motors";
    c.title = {"Stern-Dreieck-Anlauf", "Star-delta starting"};
    c.summary = {"Anlaufstrom, Anlaufmoment, Motorschutz-Einstellung", "Starting current, torque, overload setting"};
    c.description = {
        "Im Stern: I_A,Y = I_A,Δ / 3    M_A,Y = M_A,Δ / 3\n"
        "Motorschutzrelais im Motorstrang (nach dem Netzschütz): Einstellung I_N / √3 ≈ 0,58 · I_N.\n"
        "Voraussetzung: Motor für Dreieck an Netzspannung (z. B. 400 V Δ / 690 V Y).",
        "In star: I_A,Y = I_A,Δ / 3    M_A,Y = M_A,Δ / 3\n"
        "Overload relay in the motor phase (after main contactor): setting I_N / √3 ≈ 0.58 · I_N.\n"
        "Requires a motor rated for delta at mains voltage (e.g. 400 V Δ / 690 V Y)."};
    c.inputs = {
        Num::number("In", {"Bemessungsstrom I_N", "Rated current I_N"}, "A", 15).positive(),
        Num::number("IaIn", {"Anlaufstromverhältnis I_A/I_N", "Starting current ratio I_A/I_N"}, "", 6.5).positive(),
        Num::number("MaMn", {"Anzugsmomentverhältnis M_A/M_N", "Starting torque ratio M_A/M_N"}, "", 2.2).positive(),
        Num::number("ML", {"Lastmoment beim Anlauf M_L/M_N", "Load torque at start M_L/M_N"}, "", kNaN)
            .opt()
            .nonNegative(),
    };
    c.outputs = {Out::number("Idol", {"Anlaufstrom direkt (Δ)", "Starting current DOL (Δ)"}, "A", 1),
                 Out::number("Istar", {"Anlaufstrom Stern (Y)", "Starting current star (Y)"}, "A", 1).primary(),
                 Out::number("Mstar", {"Anzugsmoment Stern M_A,Y / M_N", "Starting torque star M_A,Y / M_N"}, "", 2),
                 Out::number("relay", {"Einstellung Motorschutzrelais", "Overload relay setting"}, "A", 2)};
    c.keywords = {"stern dreieck", "anlauf", "star delta", "y-d", "motorschutz"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto s = motors::starDelta(in.num("In"), in.num("IaIn"), in.num("MaMn"));
        r.set("Idol", s.IstartDirect);
        r.set("Istar", s.IstartStar);
        r.set("Mstar", s.torqueStarRatio);
        r.set("relay", s.relaySetting);
        if (const auto ml = in.maybe("ML")) {
            if (s.torqueStarRatio <= *ml) {
                r.error({"Anzugsmoment im Stern reicht für das Lastmoment nicht aus.",
                         "Star starting torque is too low for the load torque."});
            } else {
                r.ok({"Anzugsmoment im Stern > Lastmoment.", "Star starting torque > load torque."});
            }
        }
        return r;
    };
    return c;
}

Calculator steinmetz() {
    Calculator c;
    c.id = "steinmetz";
    c.category = "motors";
    c.title = {"Drehstrommotor am 1~-Netz (Steinmetz)", "3~ motor on single phase (Steinmetz)"};
    c.summary = {"Betriebs- und Anlaufkondensator", "Run and start capacitor"};
    c.description = {
        "Faustformel: C_B ≈ 70 µF pro kW bei 230 V / 50 Hz (skaliert mit (230/U)² · 50/f)\n"
        "C_A ≈ 2…3 · C_B (nur während des Anlaufs). Leistung ca. 70…80 % von P_N.\n"
        "Motor in Dreieck für 230 V schalten (230/400-V-Motor). Kondensator-Nennspannung ≥ 1,25 · U.",
        "Rule of thumb: C_run ≈ 70 µF per kW at 230 V / 50 Hz (scaled by (230/U)² · 50/f)\n"
        "C_start ≈ 2…3 · C_run (during start only). Output approx. 70…80 % of P_N.\n"
        "Connect motor in delta for 230 V (230/400 V motor). Capacitor rated voltage ≥ 1.25 · U."};
    c.inputs = {
        Num::number("P", {"Motorleistung P_N", "Motor output P_N"}, "kW", 0.75).positive(),
        Num::number("U", {"Netzspannung 1~", "Single-phase voltage"}, "V", 230).positive(),
        Num::number("f", {"Frequenz f", "Frequency f"}, "Hz", 50).positive(),
    };
    c.outputs = {Out::number("Cb", {"Betriebskondensator C_B", "Run capacitor C_run"}, "µF", 1).primary(),
                 Out::text("Ca", {"Anlaufkondensator C_A", "Start capacitor C_start"}),
                 Out::number("P", {"Verfügbare Leistung ca.", "Available output approx."}, "kW", 2)};
    c.keywords = {"steinmetz", "kondensator", "einphasig", "capacitor", "single phase"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto s = motors::steinmetz(in.num("P"), in.num("U"), in.num("f"));
        r.set("Cb", s.Crun_uF);
        r.setText("Ca", cat(numText(s.CstartMin_uF, 0), Text{" … ", " … "}, numText(s.CstartMax_uF, 0, "µF")));
        r.set("P", s.Pavail_kW);
        if (in.num("P") > 2.2) {
            r.warn({"Steinmetzschaltung nur für kleine Motoren (≈ bis 2 kW) sinnvoll.",
                    "Steinmetz connection is only practical for small motors (≈ up to 2 kW)."});
        }
        return r;
    };
    return c;
}

}  // namespace

void addMotors(std::vector<Calculator>& out) {
    out.push_back(motorCurrent());
    out.push_back(torque());
    out.push_back(motorSpeed());
    out.push_back(efficiency());
    out.push_back(starDelta());
    out.push_back(steinmetz());
}

}  // namespace elc::catalog
