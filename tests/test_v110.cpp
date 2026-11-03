// Version 1.10: pinout pictures, cable tray, motor feeder and surge protection – reference values from the sources named
// in the calculators (Eaton Schaltungsbuch, DEHN, IEC 62305, OBO, Lapp/Faber/Waskönig data sheets).
#include <algorithm>
#include <cmath>
#include <set>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/lists.hpp"
#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;

TEST_CASE("Every pinout table in Kommunikation & Pinbelegungen has a picture whose pins are all found in the table", "[pinout]") {
    int pictures = 0;
    for (const auto& t : tables()) {
        if (t.category != "comms" || t.id == "cctv_resolutions") continue;
        INFO(t.id);
        REQUIRE(t.drawing.has_value());
        CHECK(t.drawing->diagram);
        CHECK(t.drawing->aspect > 0);
        CHECK_FALSE(t.drawing->series.empty());
        CHECK_FALSE(t.drawing->note.de.empty());
        CHECK_FALSE(t.drawing->note.en.empty());
        for (const auto& l : t.drawing->labels) {
            CHECK(l.text.de != "?");  // "?" marks a pin of the picture that is missing in the table
        }
        for (const auto& s : t.drawing->series) CHECK(s.style.find("#FF00FF") == std::string::npos);
        ++pictures;
    }
    CHECK(pictures >= 32);
    // tables without a picture keep none
    CHECK_FALSE(findTable("si_prefixes")->drawing.has_value());
}

TEST_CASE("Pinout pictures colour the pins by function from the table", "[pinout]") {
    const auto* rs232 = findTable("pin_rs232");
    REQUIRE(rs232);
    // pin 5 = GND → dark, the others signal blue
    int ground = 0, signal = 0;
    for (const auto& s : rs232->drawing->series) {
        if (s.style == "fill:#455A64") ++ground;
        if (s.style == "fill:#1E88E5") ++signal;
    }
    CHECK(ground == 1 + 1);  // pin 5 + legend square
    CHECK(signal == 8 + 1);
    // RJ45: T568B pin 1 white-orange = white pad with orange stripe
    const auto* rj = findTable("pin_rj45");
    REQUIRE(rj);
    const auto it = std::find_if(rj->drawing->series.begin(), rj->drawing->series.end(), [](const ChartSeries& s) { return s.style == "fill:#FB8C00"; });
    CHECK(it != rj->drawing->series.end());
}

#include "elc/formulas/cabledata.hpp"
#include "elc/formulas/cables.hpp"

TEST_CASE("Cable data: free text is recognised (data sheet values)", "[tray]") {
    const auto nym = cabledata::lookup("NYM-J 5x2,5");
    REQUIRE(nym);
    CHECK(nym->d == Approx(11.0));        // Faber NYM-J 5x2,5: DA 11 mm
    CHECK(nym->kgPerKm == Approx(270));   // 270 kg/km
    CHECK(nym->name() == "NYM-J 5x2,5");
    CHECK(nym->power());
    CHECK(cabledata::lookup("nym 5G2.5")->d == Approx(11.0));
    CHECK(cabledata::lookup("NYY-J 4×95")->d == Approx(37));            // Waskönig+Walter
    CHECK(cabledata::lookup("NYY-J 4×95")->kgPerKm == Approx(4301));
    CHECK(cabledata::lookup("J-Y(St)Y 2x2x0,8")->d == Approx(7.0));     // Lapp
    CHECK(cabledata::lookup("jysty 10x2x0.8")->kgPerKm == Approx(220));
    CHECK_FALSE(cabledata::lookup("J-Y(St)Y 2x2x0,8")->power());
    CHECK(cabledata::lookup("LiYCY 4x0,75")->d == Approx(7.0));
    CHECK(cabledata::lookup("Cat.7")->d == Approx(7.2));
    CHECK(cabledata::lookup("NYM-O 2x1,5")->name() == "NYM-O 2x1,5");
    CHECK_FALSE(cabledata::lookup("NHXH 3x1,5").has_value());
    CHECK_FALSE(cabledata::lookup("NYM-J 5x3").has_value());
    const auto* t = findTable("cable_dimensions");
    REQUIRE(t);
    CHECK(t->rows.size() == cabledata::all().size());
}

TEST_CASE("Cable tray example: width, fill and load by hand", "[tray]") {
    const auto* l = findList("cable_tray");
    REQUIRE(l);
    const auto rows = exampleRows(*l);
    REQUIRE(rows.size() == 8);
    const auto res = evaluateList(*l, rows, defaultSettings(*l), {});
    for (const auto& r : res.rows) CHECK_FALSE(r.hasError());
    const auto& v = res.totals.values;
    // Σ n·d² = 22² + 4·11² + 6·9,5² + 8·9,4² + 20·7² + 2·9² + 6·7,2² + 10·7² = 4159,42 mm²; +30 % = 5407,25 mm²
    CHECK(v.at("A") == Approx(4159.42).epsilon(1e-6));
    CHECK(v.at("Areq") == Approx(5407.246).epsilon(1e-6));
    CHECK(v.at("B") == Approx(100));  // 100 mm × 60 mm = 6000 mm² ≥ 5407 mm²
    CHECK(v.at("fill") == Approx(69.32).epsilon(1e-3));
    CHECK(v.at("q") == Approx(7.358).epsilon(1e-6));
    CHECK(v.at("F") == Approx(7.358 * 9.81 / 1000).epsilon(1e-6));
    CHECK(v.at("Fmax") == Approx(0.168));  // 0,028 N/(m·mm²) × 6000 mm²
    CHECK(v.at("nPower") == Approx(19));
    CHECK(v.at("nData") == Approx(38));
    CHECK(v.at("widthPower") == Approx(198.2));
    // power cables do not fit side by side on 100 mm → bunched
    CHECK(v.at("f") == Approx(cables::groupingFactor(cables::Arrangement::Bunched, 19)));
    // unknown cable without diameter is an error, with d and weight it works
    auto row = defaultRow(*l);
    row.setChoice("cable", "NHXH-J 3x1,5");
    CHECK(evaluateRow(*l, row, defaultSettings(*l), {}).hasError());
    row.set("d", 10.1);
    row.set("g", 160);
    const auto ok = evaluateRow(*l, row, defaultSettings(*l), {});
    CHECK_FALSE(ok.hasError());
    CHECK(ok.values.at("A") == Approx(102.01));
}

#include "elc/formulas/motors.hpp"

TEST_CASE("Motor guide values and MPCB ranges (Eaton Schaltungsbuch 06/11)", "[feeder]") {
    const auto m = motors::motorGuide(7.5);
    REQUIRE(m);
    CHECK(m->I400 == Approx(15.2));     // p. 10-44
    CHECK(m->fuseDol400 == Approx(32));
    CHECK(m->fuseYd400 == Approx(16));
    CHECK(m->cosPhi == Approx(0.82));
    CHECK(motors::motorGuide(22)->I400 == Approx(41));
    CHECK(motors::motorGuide(30)->fuseDol400 == Approx(100));
    CHECK(motors::motorGuide(8)->P_kW == Approx(11));  // next standard power
    CHECK_FALSE(motors::motorGuide(500).has_value());
    CHECK(motors::mpcbRange(15.2)->lo == Approx(10));
    CHECK(motors::mpcbRange(15.2)->hi == Approx(16));
    CHECK(motors::mpcbRange(9)->hi == Approx(10));
    CHECK(motors::mpcbRange(60)->lo == Approx(55));
    CHECK_FALSE(motors::mpcbRange(70).has_value());
    CHECK(motors::tripClassFor(3) == 10);
    CHECK(motors::tripClassFor(5) == 20);
    CHECK(motors::tripClassFor(8) == 30);
    CHECK(motors::tripClassFor(12) == 0);
    CHECK(findTable("motor_rated_currents")->rows.size() == motors::motorGuideTable().size());
}

TEST_CASE("Motor feeder: pump 7.5 kW direct-on-line with MPCB", "[feeder]") {
    const auto* c = findCalculator("motor_feeder");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    const auto r = evaluate(*c, in, {});
    REQUIRE_FALSE(r.hasError());
    CHECK(r.values.at("IN") == Approx(15.2));
    CHECK(r.values.at("IA") == Approx(91.2));  // 6 · I_N
    CHECK(r.values.at("Ir") == Approx(15.2));
    CHECK(r.values.at("fuse") == Approx(32));
    CHECK(r.texts.at("prot").de.find("10–16 A") != std::string::npos);
    CHECK(r.texts.at("tclass").de.find("CLASS 10") != std::string::npos);
    CHECK(r.texts.at("contactor").de.find("K1") != std::string::npos);
    // cable: method E, PVC, 3 loaded conductors, 30 °C, single circuit → 1.5 mm² (I_z ≥ 15.2 A)
    CHECK(r.values.at("A") == Approx(1.5));
    CHECK(r.values.at("Iz") == Approx(*cables::baseAmpacity(cables::Insulation::PVC, cables::Method::E, 3, 1.5)));
    const double dU = cables::voltageDrop(System::AC3, 30, 15.2, 1.5, cables::resistivity(cables::Material::Cu, 70), 0.82, 0.08e-3) / 400 * 100;
    CHECK(r.values.at("dU") == Approx(dU));
    CHECK(r.texts.at("rcd").de.find("Typ A") != std::string::npos);
}

TEST_CASE("Motor feeder: star-delta, drive and smoke extraction", "[feeder]") {
    const auto* c = findCalculator("motor_feeder");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.setChoice("app", "fan");
    in.setChoice("start", "yd");
    in.set("P", 15);
    in.set("ta", 12);
    auto r = evaluate(*c, in, {});
    REQUIRE_FALSE(r.hasError());
    CHECK(r.values.at("IN") == Approx(29.3));
    CHECK(r.values.at("Ir") == Approx(0.58 * 29.3));  // relay in the motor line (t_A ≤ 15 s)
    CHECK(r.values.at("fuse") == Approx(32));         // Y-Δ column
    CHECK(r.texts.at("tclass").de.find("Position A") != std::string::npos);
    CHECK(r.texts.at("contactor").de.find("≥ 17 A") != std::string::npos);  // K1/K3 0,58 · 29,3 A = 16,99 A
    CHECK(r.texts.at("contactor").de.find("9,67 A") != std::string::npos);  // K2 0,33 · 29,3 A
    in.set("ta", 25);
    CHECK(evaluate(*c, in, {}).values.at("Ir") == Approx(29.3));  // position B in the supply

    in = defaultInputs(*c);
    in.setChoice("start", "vfd");
    r = evaluate(*c, in, {});
    CHECK(r.values.at("In") == Approx(16));  // next MCB ≥ 15.2 A
    CHECK_FALSE(r.values.contains("fuse"));
    CHECK(r.texts.at("rcd").de.find("Typ B") != std::string::npos);
    CHECK(r.texts.at("cable").de.find("geschirmt") != std::string::npos);

    in = defaultInputs(*c);
    in.setChoice("app", "smoke");
    r = evaluate(*c, in, {});
    CHECK(r.texts.at("prot").de.find("nur melden") != std::string::npos);
    CHECK(r.texts.at("cable").de.find("Funktionserhalt") != std::string::npos);

    in = defaultInputs(*c);
    in.setChoice("data", "plate");
    in.set("IN", 12);
    in.set("cosphi", 0.8);
    r = evaluate(*c, in, {});
    CHECK(r.values.at("IN") == Approx(12));
    CHECK(r.values.at("fuse") == Approx(32));  // first table motor with I_N ≥ 12 A (7.5 kW)
    CHECK(r.texts.at("prot").de.find("8–12 A") != std::string::npos);  // smallest range containing 12 A
}

#include "elc/formulas/automation.hpp"

TEST_CASE("Surge protection 230/400 V: selection by zone and lightning current split", "[surge]") {
    const auto* c = findCalculator("spd_power");
    REQUIRE(c);
    REQUIRE(c->category == "surge");
    auto in = defaultInputs(*c);  // main distribution, LPS class III, TN-S 3~, one service
    auto r = evaluate(*c, in, {});
    REQUIRE_FALSE(r.hasError());
    CHECK(r.texts.at("type").de.find("Typ 1+2") != std::string::npos);
    CHECK(r.values.at("Iimp") == Approx(12.5));  // 0,5 · 100 kA / (1 · 4)
    CHECK(r.values.at("IimpTot") == Approx(50));
    CHECK(r.values.at("Uc") == Approx(253));
    CHECK(r.values.at("UcN") == Approx(230));
    CHECK(r.texts.at("scheme").de.find("4+0") != std::string::npos);
    // LPL I in TT: 25 kA per path, 100 kA N–PE (matches DEHNventil M TT 25/100)
    in.setChoice("lpl", "1");
    in.setChoice("net", "tt");
    r = evaluate(*c, in, {});
    CHECK(r.values.at("Iimp") == Approx(25));
    CHECK(r.values.at("IimpTot") == Approx(100));
    CHECK(r.texts.at("scheme").de.find("3+1") != std::string::npos);
    // no LPS, underground cable → type 2 with I_n ≥ 5 kA
    in = defaultInputs(*c);
    in.setChoice("lps", "none");
    r = evaluate(*c, in, {});
    CHECK(r.texts.at("type").de.find("Typ 2") != std::string::npos);
    CHECK(r.values.at("In") == Approx(5));
    CHECK_FALSE(r.values.contains("Iimp"));
    // sub-distribution: > 10 m → type 2, ≤ 10 m → none
    in = defaultInputs(*c);
    in.setChoice("loc", "sub");
    CHECK(evaluate(*c, in, {}).texts.at("type").de.find("Typ 2") != std::string::npos);
    in.set("dist", 5);
    CHECK(evaluate(*c, in, {}).texts.at("type").de.find("kein weiteres") != std::string::npos);
    // equipment > 10 m from the type 2 → type 3, U_p ≤ 1.5 kV
    in.setChoice("loc", "device");
    in.set("dist", 20);
    r = evaluate(*c, in, {});
    CHECK(r.texts.at("type").de.find("Typ 3") != std::string::npos);
    CHECK(r.values.at("Up") == Approx(1.5));
    // IT without neutral: U_C ≥ U
    in = defaultInputs(*c);
    in.setChoice("net", "it");
    CHECK(evaluate(*c, in, {}).values.at("Uc") == Approx(400));
    in.setChoice("net", "tnc");
    in.setChoice("ph", "1");
    CHECK(evaluate(*c, in, {}).hasError());
}

TEST_CASE("Surge protection for I&C: U_C class, zone and Pt100 error", "[surge]") {
    const auto* c = findCalculator("spd_signal");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    auto r = evaluate(*c, in, {});
    CHECK(r.values.at("Uc") == Approx(33));  // 24 V signal → DEHN 33 V class
    CHECK(r.texts.at("arrester").de.find("D1") != std::string::npos);
    CHECK(r.texts.at("dehn").de.find("BE 24") != std::string::npos);
    in.setChoice("zone", "in");
    CHECK(evaluate(*c, in, {}).texts.at("arrester").de.find("C2") != std::string::npos);
    in.setChoice("sig", "eth");
    in.set("U", 57);
    r = evaluate(*c, in, {});
    CHECK(r.values.at("Uc") == Approx(58));
    CHECK(r.texts.at("dehn").de.find("DEHNpatch") != std::string::npos);
    in.setChoice("sig", "rs485");
    in.set("U", 5);
    CHECK(evaluate(*c, in, {}).values.at("Uc") == Approx(6));
    // Pt100, 2-wire, 1.8 Ω per core, arresters in field and panel: ΔT = 2 · 1.8 Ω · 2 / (dR/dT)
    in.setChoice("sig", "pt");
    in.set("U", 5);
    r = evaluate(*c, in, {});
    const double sens = automation::rtdSensitivity(automation::Rtd::Pt100, 20);
    CHECK(r.values.at("dT") == Approx(2 * 1.8 * 2 / sens));
    CHECK(r.values.at("dT") > 18);  // ≈ 18.7 K – far too much for 2-wire
    in.setChoice("wires", "3");
    CHECK(evaluate(*c, in, {}).values.at("dT") == Approx(0));
    in.set("U", 230);
    CHECK(evaluate(*c, in, {}).hasError());
}

TEST_CASE("Effective protection level and protective distance (OVE E 09, IEC 62305-4)", "[surge]") {
    const auto* c = findCalculator("spd_distance");
    REQUIRE(c);
    auto in = defaultInputs(*c);  // U_p 1.5 kV + 0.5 kV, category II, 15 m
    auto r = evaluate(*c, in, {});
    CHECK(r.values.at("Upf") == Approx(2.0));
    CHECK(r.values.at("ratio") == Approx(80));
    CHECK(r.values.at("lpo") == Approx(20));  // (2.5 − 2.0) kV / 25 V/m
    CHECK(r.texts.at("verdict").de.find("l_po") != std::string::npos);
    in.set("l", 25);
    CHECK(evaluate(*c, in, {}).texts.at("verdict").de.find("länger") != std::string::npos);
    in.set("Up", 0.7);
    in.setChoice("du", "none");
    CHECK(evaluate(*c, in, {}).texts.at("verdict").de.find("unabhängig") != std::string::npos);
    in.set("Up", 2.5);
    in.setChoice("du", "flat");
    CHECK(evaluate(*c, in, {}).hasError());
    in.setChoice("du", "lead");
    in.set("Up", 1.5);
    in.set("a", 1.0);
    CHECK(evaluate(*c, in, {}).values.at("Upf") == Approx(2.5));
}

TEST_CASE("Area Überspannungsschutz holds the surge topics", "[surge]") {
    for (const auto* id : {"spd_power", "spd_signal", "spd_distance", "surge_risk"}) {
        INFO(id);
        CHECK(findCalculator(id)->category == "surge");
    }
    for (const auto* id : {"lpz_zones", "spd_types", "spd_connection", "overvoltage_categories", "lightning_parameters", "signal_spd_categories"}) {
        INFO(id);
        CHECK(findTable(id)->category == "surge");
    }
    CHECK(findCalculator("motor_feeder")->category == "motors");
    CHECK(findList("cable_tray")->category == "projects");
}
