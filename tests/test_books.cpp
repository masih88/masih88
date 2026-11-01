// Formulas and calculators added from the technical books (v1.5).
// Reference values are the worked examples printed in the books (page in the comment).
#include <array>
#include <cmath>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/acoustics.hpp"
#include "elc/formulas/building.hpp"
#include "elc/formulas/control.hpp"
#include "elc/formulas/heating.hpp"
#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;

namespace {

Result run(const char* id, std::initializer_list<std::pair<const char*, double>> nums,
           std::initializer_list<std::pair<const char*, const char*>> choices = {}, Context ctx = {}) {
    const auto* c = findCalculator(id);
    REQUIRE(c != nullptr);
    Inputs in = defaultInputs(*c);
    for (const auto& [k, v] : nums) in.set(k, v);
    for (const auto& [k, v] : choices) in.setChoice(k, v);
    return evaluate(*c, in, ctx);
}

const std::string& text(const Result& r, const char* id) {
    const auto it = r.texts.find(id);
    REQUIRE(it != r.texts.end());
    return it->second.de;
}

bool hasSeverity(const Result& r, Severity s) {
    for (const auto& m : r.messages)
        if (m.severity == s) return true;
    return false;
}

}  // namespace

// ---------------------------------------------------------------- control (Siemens Regeltechnik, S. 82–83)

TEST_CASE("difficulty ratio S = Tu/Tg and rating", "[books][control]") {
    CHECK(control::difficultyRatio(5, 100) == Approx(0.05));
    CHECK(control::classify(0.05) == control::Difficulty::Easy);
    CHECK(control::classify(0.2) == control::Difficulty::Medium);
    CHECK(control::classify(0.35) == control::Difficulty::Hard);
}

TEST_CASE("Ziegler/Nichols rules (Fig. 6-15)", "[books][control]") {
    using control::Controller;
    using control::Rule;
    // Ks = 1, Tt = 10 s, T = 100 s
    const auto p = control::tune(Rule::ZieglerNichols, Controller::P, 1, 10, 100);
    REQUIRE(p);
    CHECK(p->kp == Approx(10));
    const auto pi = control::tune(Rule::ZieglerNichols, Controller::PI, 1, 10, 100);
    CHECK(pi->kp == Approx(9));
    CHECK(pi->tn == Approx(33));
    const auto pd = control::tune(Rule::ZieglerNichols, Controller::PD, 1, 10, 100);
    CHECK(pd->kp == Approx(12));
    CHECK(pd->tv == Approx(2.5));
    const auto pid = control::tune(Rule::ZieglerNichols, Controller::PID, 2, 10, 100);
    CHECK(pid->kp == Approx(6));
    CHECK(pid->tn == Approx(20));
    CHECK(pid->tv == Approx(4.2));
}

TEST_CASE("Chien/Hrones/Reswick rules (Fig. 6-18)", "[books][control]") {
    using control::Controller;
    using control::Rule;
    // V0 = Ks · Kp; Ks = 2, Tu = 10 s, Tg = 100 s → Tg/Tu = 10
    const auto a = control::tune(Rule::ChrAperiodicDisturbance, Controller::PI, 2, 10, 100);
    REQUIRE(a);
    CHECK(a->kp == Approx(3));   // V0 = 0,6 · 10 = 6
    CHECK(a->tn == Approx(40));  // 4 · Tu
    const auto b = control::tune(Rule::ChrAperiodicSetpoint, Controller::PID, 2, 10, 100);
    CHECK(b->kp == Approx(3));    // 0,6 · 10 / 2
    CHECK(b->tn == Approx(100));  // Tg
    CHECK(b->tv == Approx(5));    // 0,5 · Tu
    const auto c = control::tune(Rule::Chr20Disturbance, Controller::PID, 1, 10, 100);
    CHECK(c->kp == Approx(12));
    CHECK(c->tn == Approx(20));
    CHECK(c->tv == Approx(4.2));
    const auto d = control::tune(Rule::Chr20Setpoint, Controller::PID, 1, 10, 100);
    CHECK(d->kp == Approx(9.5));
    CHECK(d->tn == Approx(135));
    CHECK(d->tv == Approx(4.7));
    CHECK_FALSE(control::tune(Rule::Chr20Setpoint, Controller::PD, 1, 10, 100));
}

TEST_CASE("controller_tuning calculator", "[books][control]") {
    const auto r = run("controller_tuning", {{"ks", 1}, {"tu", 10}, {"tg", 100}}, {{"rule", "zn"}, {"type", "pid"}});
    CHECK(r.values.at("kp") == Approx(12));
    CHECK(r.values.at("tn") == Approx(20));
    CHECK(r.values.at("S") == Approx(0.1));
}

// ---------------------------------------------------------------- heating (Siemens Heizung, S. 19–20)

TEST_CASE("heating curve steepness and parallel shift", "[books][heating]") {
    const double s = heating::steepness(67, 33);
    CHECK(s == Approx(1.7));
    CHECK(heating::flowShift(-5, s) == Approx(-13.5));
    CHECK(heating::linearFlow(67, 33, -5) == Approx(67));
    CHECK(heating::linearFlow(67, 33, 15) == Approx(33));
    CHECK(heating::linearFlow(67, 33, 5) == Approx(50));
}

TEST_CASE("radiator model passes through the design point", "[books][heating]") {
    const heating::Design d{20, -10, 70, 50, 1.3};
    CHECK(heating::flowTemperature(d, -10, 20) == Approx(70));
    CHECK(heating::flowTemperature(d, 20, 20) == Approx(20));
    const double mid = heating::flowTemperature(d, 5, 20);
    CHECK(mid > 40);
    CHECK(mid < 50);
}

TEST_CASE("heating_curve calculator reproduces the book example", "[books][heating]") {
    const auto r = run("heating_curve", {{"tvm5", 67}, {"tvp15", 33}, {"dtr", -5}, {"ta", -5}}, {{"mode", "points"}});
    CHECK(r.values.at("s") == Approx(1.7));
    CHECK(r.values.at("dtv") == Approx(-13.5));
    CHECK(r.values.at("tvm5s") == Approx(53.5));
    CHECK(r.values.at("tvp15s") == Approx(19.5));
}

// ---------------------------------------------------------------- fans/pumps (Siemens HLK-Einführung, S. 180)

TEST_CASE("proportionality laws", "[books][fans]") {
    const auto p = fans::affinity({4000, 350, 1.0}, 2.0);
    CHECK(p.flow == Approx(8000));
    CHECK(p.pressure == Approx(1400));
    CHECK(p.power == Approx(8.0));
    const auto r = run("fan_affinity", {{"V1", 4000}, {"p1", 350}, {"P1", 1}, {"n1", 1000}, {"n2", 2000}}, {{"mode", "speed"}});
    CHECK(r.values.at("V2o") == Approx(8000));
    CHECK(r.values.at("p2") == Approx(1400));
}

// ---------------------------------------------------------------- valves (QM Holzheizwerke FAQ 37)

TEST_CASE("minimum controllable flow and heat output", "[books][valves]") {
    const double v = valves::minFlowRatio(0.5, 50);
    CHECK(v == Approx(0.0283).margin(0.0005));  // ≈ 3 %
    CHECK(valves::heatRatio(v, 1.0) == Approx(v));
    CHECK(valves::heatRatio(v, 0.7) == Approx(0.040).margin(0.001));  // ≈ 4 %
}

// ---------------------------------------------------------------- heat pump (Siemens Kältetechnik, S. 112–115)

TEST_CASE("Carnot COP", "[books][heatpump]") {
    CHECK(heatpump::carnotHeating(35, 0) == Approx(308.15 / 35));
    CHECK(heatpump::carnotCooling(0, 35) == Approx(273.15 / 35));
    const auto r = run("heat_pump", {{"tsink", 35}, {"tsource", 2}, {"cop", 3.0}}, {{"type", "air"}});
    CHECK(hasSeverity(r, Severity::Warning));  // below Keymark minimum 3.1 (A2/W35)
}

// ---------------------------------------------------------------- BAC factors (Siemens EN ISO 52120, S. 124–125)

TEST_CASE("BAC efficiency factors", "[books][bac]") {
    using bac::BuildingType;
    using bac::Energy;
    CHECK(bac::factor(BuildingType::Office, Energy::Thermal, 'D') == Approx(1.51));
    CHECK(bac::factor(BuildingType::Office, Energy::Thermal, 'A') == Approx(0.70));
    CHECK(bac::factor(BuildingType::Retail, Energy::Thermal, 'A') == Approx(0.6));
    CHECK(bac::factor(BuildingType::Residential, Energy::Electric, 'A') == Approx(0.92));
    CHECK(bac::factor(BuildingType::Other, Energy::Electric, 'B') == Approx(1.0));
    const auto r = run("bac_efficiency", {{"eth", 100000}, {"eel", 0}}, {{"type", "office"}, {"from", "D"}, {"to", "B"}});
    CHECK(r.values.at("eth2") == Approx(100000 * 0.80 / 1.51));
    CHECK(r.values.at("sth") == Approx(100 * (1 - 0.80 / 1.51)));
}

// ---------------------------------------------------------------- escape routes (ASR A2.3:2022)

TEST_CASE("escape route widths ASR A2.3", "[books][escape]") {
    auto w = escape::minWidth(5);
    REQUIRE(w);
    CHECK(w->door == Approx(0.80));
    CHECK(w->route == Approx(0.90));
    w = escape::minWidth(20);
    CHECK(w->door == Approx(0.90));
    CHECK(w->route == Approx(1.00));
    w = escape::minWidth(250);  // linear interpolation above 200 persons
    CHECK(w->door == Approx(1.35));
    CHECK(w->route == Approx(1.50));
    w = escape::minWidth(400);
    CHECK(w->route == Approx(2.40));
    CHECK_FALSE(escape::minWidth(401));
    CHECK(escape::maxLength(escape::Room::FireNoExtinguisher) == Approx(25));
    CHECK(escape::maxLength(escape::Room::Explosive) == Approx(10));
}

// ---------------------------------------------------------------- lift shaft (MBO § 39, VKF)

TEST_CASE("lift shaft smoke vent opening", "[books][shaft]") {
    CHECK(shaft::openingDE(2.5) == Approx(0.10));
    CHECK(shaft::openingDE(6.0) == Approx(0.15));
    CHECK(shaft::openingCH(2.5) == Approx(0.125));
    CHECK(shaft::openingCH(4.0) == Approx(0.16));
}

// ---------------------------------------------------------------- KNX (KNX Grundlagenwissen 2013, S. 8–9)

TEST_CASE("KNX topology", "[books][knx]") {
    const auto t = knx::plan(150, 64);
    CHECK(t.lines == 3);
    CHECK(t.areas == 1);
    CHECK(t.segmentsPerLine == 1);
    const auto big = knx::plan(1000, 255);
    CHECK(big.lines == 4);
    CHECK(big.segmentsPerLine == 4);
    CHECK(knx::plan(5000, 64).areas == 6);
}

// ---------------------------------------------------------------- acoustics (BAuA Fb 1030, S. 19–20)

TEST_CASE("Sabine and DIN 18041 targets", "[books][acoustics]") {
    CHECK(acoustics::sabine(200, 40) == Approx(0.815));
    CHECK(acoustics::din18041Target(acoustics::Use::Teaching, 150) == Approx(0.53).margin(0.005));
    CHECK(acoustics::din18041Target(acoustics::Use::Teaching, 400) == Approx(0.66).margin(0.005));
    CHECK(acoustics::din18041Target(acoustics::Use::TeachingInclusive, 200) == Approx(0.26 * std::log10(200.0) - 0.14));
    const std::array<double, 2> two{80, 80};
    CHECK(acoustics::levelSum(two) == Approx(83.01).margin(0.01));
    CHECK(acoustics::distanceLevel(90, 1, 10) == Approx(70));
}

// ---------------------------------------------------------------- AMEV address key (AMEV GA 2005, Anhang 2.1)

TEST_CASE("AMEV data point address", "[books][amev]") {
    const auto r = run("amev_address", {{"site", 31021}, {"bldg", 8}, {"plantNo", 1}, {"make", 1}, {"partNo", 1}},
                       {{"trade", "H"}, {"plant", "HKR"}, {"part", "PUU"}, {"kind", "SM"}, {"desc", "17"}});
    CHECK(text(r, "addr") == "31021-008-H-HKR-01-01-PUU-01-SM-17");
}

// ---------------------------------------------------------------- checks of existing calculators against the books

TEST_CASE("valve_kv shows the minimum controllable output (FAQ 37)", "[books][valves]") {
    // authority 0.5 → choose Δp_MV = Δp_V100; rangeability 50, heat exchanger a = 0.7 → ≈ 3 % flow, ≈ 4 % heat
    const auto r = run("valve_kv", {{"Q", 2.5}, {"dp", 10}, {"dpmv", 0}, {"sv", 50}, {"awt", 0.7}}, {{"mode", "flow"}});
    const double a = r.values.at("a");
    CHECK(r.values.at("vmin") == Approx(100 * valves::minFlowRatio(a, 50)));
    CHECK(r.values.at("qmin") == Approx(100 * valves::heatRatio(valves::minFlowRatio(a, 50), 0.7)));
}

TEST_CASE("transformer: steady short-circuit current without voltage factor (Haag Band 05)", "[books][trafo]") {
    // 400 kVA, 10/0,4 kV, uz = 4 % → I_r = 577 A, I_kD = 14,43 kA
    const auto r = run("transformer", {{"S", 400}, {"U1", 10000}, {"U2", 400}, {"uk", 4}}, {{"sys", "ac3"}});
    CHECK(r.values.at("I2") == Approx(577.35).margin(0.1));
    CHECK(r.values.at("IkD") == Approx(14.43).margin(0.01));
}
