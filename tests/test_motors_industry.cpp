#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/motors.hpp"

using Catch::Approx;
using namespace elc;

TEST_CASE("Motor rated current 7.5 kW / 400 V / cos φ 0.85 / η 0.9") {
    CHECK(motors::ratedCurrent(System::AC3, 7500, 400, 0.85, 0.9) == Approx(14.152).epsilon(1e-4));
}

TEST_CASE("Torque from power and speed") {
    const auto t = motors::solveTorque(7.5, 1450.0, std::nullopt);
    REQUIRE(t.has_value());
    CHECK(t->M_Nm == Approx(49.394).epsilon(1e-4));
    const auto p = motors::solveTorque(std::nullopt, 1450.0, 49.394);
    REQUIRE(p.has_value());
    CHECK(p->P_kW == Approx(7.5).epsilon(1e-4));
    CHECK_FALSE(motors::solveTorque(1.0, 2.0, 3.0).has_value());
}

TEST_CASE("Synchronous speed and slip") {
    CHECK(motors::synchronousSpeed(50, 4) == Approx(1500));
    CHECK(motors::synchronousSpeed(60, 2) == Approx(3600));
    CHECK(motors::slip(1500, 1450) == Approx(1.0 / 30.0));
}

TEST_CASE("Star-delta starting") {
    const auto s = motors::starDelta(15, 6.5, 2.2);
    CHECK(s.IstartDirect == Approx(97.5));
    CHECK(s.IstartStar == Approx(32.5));
    CHECK(s.torqueStarRatio == Approx(0.7333).epsilon(1e-3));
    CHECK(s.relaySetting == Approx(8.660).epsilon(1e-3));
}

TEST_CASE("Steinmetz rule of thumb") {
    const auto s = motors::steinmetz(1.0, 230, 50);
    CHECK(s.Crun_uF == Approx(70));
    CHECK(s.CstartMax_uF == Approx(210));
}

TEST_CASE("Power factor correction 100 kW 0.75 → 0.95") {
    const double Qc = industry::compensationPower(100e3, 0.75, 0.95);
    CHECK(Qc == Approx(55319).epsilon(1e-3));
    const double C = industry::capacitance(Qc, 400, 50, industry::CapConnection::Delta);
    CHECK(C * 1e6 == Approx(366.8).epsilon(1e-3));
    const double Cy = industry::capacitance(Qc, 400, 50, industry::CapConnection::Star);
    CHECK(Cy == Approx(3 * C));
}

TEST_CASE("Transformer currents and short circuit at terminals") {
    CHECK(industry::transformerCurrent(630e3, 400, true) == Approx(909.33).epsilon(1e-4));
    industry::ShortCircuitInput in{630, 6, 1, 400, std::nullopt, 0.0, 95, 50, cables::Material::Cu, 0.08};
    in.L_m = 1e-9;  // practically at the terminals
    const auto r = industry::shortCircuit(in);
    // c · I_r · 100/uk = 1.05 · 909.33 / 0.06
    CHECK(r.Ik3Trafo_kA == Approx(15.913).epsilon(1e-3));
    CHECK(r.Ik3End_kA == Approx(r.Ik3Trafo_kA).epsilon(1e-6));
}

TEST_CASE("Short circuit decreases along the line and Ik1 < Ik3") {
    industry::ShortCircuitInput in{630, 6, 1, 400, 500.0, 100.0, 95, 50, cables::Material::Cu, 0.08};
    const auto r = industry::shortCircuit(in);
    CHECK(r.Ik3End_kA < r.Ik3Trafo_kA);
    CHECK(r.Ik1End_kA < r.Ik3End_kA);
    CHECK(r.Ik1End_kA > 1.0);
}

TEST_CASE("Enclosure effective surface") {
    CHECK(industry::enclosureSurface(2.0, 0.8, 0.6, false) == Approx(5.712));
    CHECK(industry::enclosureSurface(0.8, 0.6, 0.25, true) == Approx(1.4 * 0.6 * 1.05 + 1.8 * 0.25 * 0.8));
}
