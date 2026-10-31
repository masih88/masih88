#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/automation.hpp"

using Catch::Approx;
using namespace elc::automation;

TEST_CASE("Linear scaling 4–20 mA") {
    CHECK(scale(12, 4, 20, 0, 100) == Approx(50));
    CHECK(scale(50, 0, 100, 4, 20) == Approx(12));
    CHECK(scale(13824, 0, 27648, -20, 80) == Approx(30));
}

TEST_CASE("Pt100 IEC 60751 reference values") {
    CHECK(rtdResistance(Rtd::Pt100, 0) == Approx(100.0));
    CHECK(rtdResistance(Rtd::Pt100, 100) == Approx(138.5055).epsilon(1e-6));
    CHECK(rtdResistance(Rtd::Pt100, -100) == Approx(60.2558).epsilon(1e-5));
    CHECK(rtdResistance(Rtd::Pt1000, 20) == Approx(1077.935).epsilon(1e-6));
    CHECK(rtdResistance(Rtd::Pt100, 850) == Approx(390.481).epsilon(1e-5));
}

TEST_CASE("RTD inversion") {
    CHECK(rtdTemperature(Rtd::Pt100, 138.5055) == Approx(100.0).margin(1e-3));
    CHECK(rtdTemperature(Rtd::Pt100, 60.2558) == Approx(-100.0).margin(1e-3));
    CHECK(rtdTemperature(Rtd::Pt1000, 1000.0) == Approx(0.0).margin(1e-6));
    for (const auto type : {Rtd::Pt100, Rtd::Pt500, Rtd::Pt1000, Rtd::Ni1000, Rtd::Ni1000TK5000}) {
        for (double T : {-40.0, 0.0, 21.5, 80.0, 140.0}) {
            CHECK(rtdTemperature(type, rtdResistance(type, T)) == Approx(T).margin(1e-6));
        }
    }
}

TEST_CASE("Ni1000 characteristics") {
    CHECK(rtdResistance(Rtd::Ni1000, 0) == Approx(1000));
    CHECK(rtdResistance(Rtd::Ni1000, 100) == Approx(1617.785).epsilon(1e-6));
    CHECK(rtdResistance(Rtd::Ni1000TK5000, 100) == Approx(1500.0).epsilon(1e-4));
}

TEST_CASE("RTD sensitivity matches numeric derivative") {
    for (const auto type : {Rtd::Pt100, Rtd::Ni1000, Rtd::Ni1000TK5000}) {
        for (double T : {-30.0, 20.0, 100.0}) {
            const double h = 1e-4;
            const double num = (rtdResistance(type, T + h) - rtdResistance(type, T - h)) / (2 * h);
            CHECK(rtdSensitivity(type, T) == Approx(num).epsilon(1e-6));
        }
    }
    CHECK(rtdSensitivity(Rtd::Pt100, 0) == Approx(0.39083));
}

TEST_CASE("NTC beta model") {
    CHECK(ntcResistance(10000, 3950, 25) == Approx(10000));
    // 10 kΩ · exp(3950 · (1/273.15 − 1/298.15)) = 33 620.6 Ω (pure beta model)
    CHECK(ntcResistance(10000, 3950, 0) == Approx(33620.6).epsilon(1e-5));
    CHECK(ntcTemperature(10000, 3950, ntcResistance(10000, 3950, 50)) == Approx(50).margin(1e-9));
}

TEST_CASE("Valve kv and pressure drop") {
    CHECK(kvValue(2.5, 10, 1000) == Approx(7.9057).epsilon(1e-4));
    CHECK(kvValue(1.0, 100, 1000) == Approx(1.0));
    CHECK(pressureDrop(2.5, 6.3, 1000) == Approx(15.747).epsilon(1e-4));
}

TEST_CASE("Heat flow water: V = P / (1.163 · ΔT)") {
    CHECK(volumeFlowFromPower(10, 20, 1000, 4.187) == Approx(10.0 / (1.163 * 20)).epsilon(1e-3));
    CHECK(powerFromVolumeFlow(volumeFlowFromPower(10, 20, 1000, 4.187), 20, 1000, 4.187) == Approx(10));
}
