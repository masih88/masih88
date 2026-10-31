#include <array>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/basics.hpp"
#include "elc/types.hpp"

using Catch::Approx;
using namespace elc;

TEST_CASE("Power triangle three-phase 400 V / 16 A / cos φ 0.9") {
    const auto p = basics::powerFromVoltageCurrent(System::AC3, 400, 16, 0.9);
    CHECK(p.S == Approx(11085.13).epsilon(1e-5));
    CHECK(p.P == Approx(9976.61).epsilon(1e-5));
    CHECK(p.Q == Approx(4831.94).epsilon(1e-4));
    CHECK(p.phiDeg == Approx(25.842).epsilon(1e-4));
}

TEST_CASE("DC power ignores cos φ") {
    const auto p = basics::powerFromVoltageCurrent(System::DC, 24, 2, 0.5);
    CHECK(p.P == Approx(48));
    CHECK(p.Q == Approx(0).margin(1e-12));
}

TEST_CASE("Current from active power") {
    CHECK(basics::currentFromActivePower(System::AC3, 11000, 400, 0.85) == Approx(18.680).epsilon(1e-4));
    CHECK(basics::currentFromActivePower(System::AC1, 2300, 230, 1.0) == Approx(10.0));
}

TEST_CASE("Ohm's law from any two values") {
    const auto v = basics::solveOhm(230.0, std::nullopt, std::nullopt, 1000.0);
    REQUIRE(v.has_value());
    CHECK(v->I == Approx(4.34783).epsilon(1e-5));
    CHECK(v->R == Approx(52.9));

    const auto rp = basics::solveOhm(std::nullopt, std::nullopt, 100.0, 4.0);
    REQUIRE(rp.has_value());
    CHECK(rp->U == Approx(20.0));
    CHECK(rp->I == Approx(0.2));

    CHECK_FALSE(basics::solveOhm(1.0, 2.0, 3.0, std::nullopt).has_value());
    CHECK_FALSE(basics::solveOhm(1.0, std::nullopt, std::nullopt, std::nullopt).has_value());
    CHECK_FALSE(basics::solveOhm(10.0, 0.0, std::nullopt, std::nullopt).has_value());
}

TEST_CASE("Series and parallel") {
    const std::array<double, 2> r{100.0, 100.0};
    CHECK(basics::directSum(r) == Approx(200));
    CHECK(basics::reciprocalSum(r) == Approx(50));
    const std::array<double, 3> r3{10.0, 20.0, 30.0};
    CHECK(basics::reciprocalSum(r3) == Approx(5.4545).epsilon(1e-4));
}

TEST_CASE("Voltage divider unloaded and loaded") {
    const auto d = basics::voltageDivider(24, 10e3, 4.7e3, std::nullopt);
    CHECK(d.UoutUnloaded == Approx(7.6735).epsilon(1e-4));
    CHECK(d.UoutLoaded == Approx(d.UoutUnloaded));
    const auto l = basics::voltageDivider(24, 10e3, 4.7e3, 4.7e3);
    CHECK(l.UoutLoaded == Approx(4.5668).epsilon(1e-4));
}

TEST_CASE("Number formatting and parsing are locale independent") {
    CHECK(formatNumber(2.5, 2, Lang::De) == "2,5");
    CHECK(formatNumber(2.5, 2, Lang::En) == "2.5");
    CHECK(formatNumber(1234.567, 1, Lang::En) == "1234.6");
    CHECK(formatNumber(-0.0001, 2, Lang::En) == "-0.0001");
    CHECK(formatNumber(0.0, 2, Lang::En) == "0");
    CHECK(parseNumber("2,5").value() == Approx(2.5));
    CHECK(parseNumber(" 1e3 ").value() == Approx(1000));
    CHECK_FALSE(parseNumber("").has_value());
    CHECK_FALSE(parseNumber("abc").has_value());
    CHECK_FALSE(parseNumber("2.5x").has_value());
}
