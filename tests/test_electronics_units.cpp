#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/electronics.hpp"
#include "elc/formulas/units.hpp"

using Catch::Approx;
using namespace elc;

TEST_CASE("E-series sizes and nesting") {
    using electronics::ESeries;
    CHECK(electronics::eSeriesBase(ESeries::E6).size() == 6);
    CHECK(electronics::eSeriesBase(ESeries::E12).size() == 12);
    CHECK(electronics::eSeriesBase(ESeries::E24).size() == 24);
    const auto e48 = electronics::eSeriesBase(ESeries::E48);
    const auto e96 = electronics::eSeriesBase(ESeries::E96);
    REQUIRE(e48.size() == 48);
    REQUIRE(e96.size() == 96);
    for (std::size_t i = 0; i < e48.size(); ++i) CHECK(e48[i] == e96[2 * i]);
}

TEST_CASE("E-series match") {
    const auto m = electronics::eSeriesMatch(electronics::ESeries::E12, 5000);
    CHECK(m.lower == Approx(4700));
    CHECK(m.upper == Approx(5600));
    CHECK(m.nearest == Approx(4700));
    const auto exact = electronics::eSeriesMatch(electronics::ESeries::E24, 4700);
    CHECK(exact.lower == Approx(4700));
    CHECK(exact.upper == Approx(4700));
    const auto e96 = electronics::eSeriesMatch(electronics::ESeries::E96, 10.1e3);
    CHECK(e96.nearest == Approx(10.2e3));
}

TEST_CASE("RC and LC") {
    CHECK(electronics::cutoffFrequency(10e3, 10e-6) == Approx(1.59155).epsilon(1e-5));
    CHECK(electronics::chargingVoltage(24, 0.1, 0.1) == Approx(24 * (1 - std::exp(-1.0))));
    CHECK(electronics::resonanceFrequency(1e-3, 1e-6) == Approx(5032.92).epsilon(1e-5));
    CHECK(electronics::inductiveReactance(50, 0.1) == Approx(31.4159).epsilon(1e-5));
    CHECK(electronics::capacitiveReactance(50, 100e-6) == Approx(31.831).epsilon(1e-4));
}

TEST_CASE("Unit conversion") {
    const auto power = units::powerUnits();
    const auto& kW = power[1];
    const auto& PS = power[3];
    const auto& TR = power[7];
    CHECK(units::convert(1, kW, PS) == Approx(1.35962).epsilon(1e-5));
    CHECK(units::convert(1, TR, kW) == Approx(3.51685).epsilon(1e-5));
    const auto energy = units::energyUnits();
    CHECK(units::convert(1, energy[4], energy[3]) == Approx(1000));  // kWh → Wh
    const auto pressure = units::pressureUnits();
    CHECK(units::convert(1, pressure[4], pressure[2]) == Approx(100));  // bar → kPa
    const auto flow = units::flowUnits();
    CHECK(units::convert(3.6, flow[0], flow[2]) == Approx(1.0));  // 3.6 m³/h → 1 l/s
}

TEST_CASE("Temperature") {
    CHECK(units::celsiusToFahrenheit(100) == Approx(212));
    CHECK(units::fahrenheitToCelsius(-40) == Approx(-40));
    CHECK(units::celsiusToKelvin(0) == Approx(273.15));
}

TEST_CASE("AWG") {
    CHECK(units::awgDiameter(12) == Approx(2.0525).epsilon(1e-4));
    CHECK(units::awgArea(12) == Approx(3.3088).epsilon(1e-3));
    CHECK(units::awgArea(36) == Approx(0.0127).epsilon(1e-2));
    CHECK(units::awgFromArea(units::awgArea(18)) == Approx(18));
}
