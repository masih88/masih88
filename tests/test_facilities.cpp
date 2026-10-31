// Cleanroom, data centre, PV and moist-air formulas with reference values.
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/air.hpp"
#include "elc/formulas/cleanroom.hpp"
#include "elc/formulas/pv.hpp"
#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;

TEST_CASE("ISO 14644-1:2015 Table 1 values") {
    using cleanroom::classLimit;
    CHECK(classLimit(1, 0.1) == 10);
    CHECK(classLimit(1, 0.2) == 2);
    CHECK(classLimit(2, 0.3) == 10);
    CHECK(classLimit(2, 0.5) == 4);
    CHECK(classLimit(3, 0.2) == 237);
    CHECK(classLimit(3, 1.0) == 8);
    CHECK(classLimit(4, 0.3) == 1020);
    CHECK(classLimit(5, 0.5) == 3520);
    CHECK(classLimit(5, 1.0) == 832);
    CHECK(classLimit(6, 5.0) == 293);
    CHECK(classLimit(7, 0.5) == 352000);
    CHECK(classLimit(8, 1.0) == 832000);
    CHECK(classLimit(9, 5.0) == 293000);
    CHECK(classLimit(9, 0.5) == 35200000);
}

TEST_CASE("ISO class from measured concentration") {
    CHECK(cleanroom::classFromConcentration(3520, 0.5) == Approx(5.0).margin(0.001));
    CHECK(cleanroom::classFromConcentration(352000, 0.5) == Approx(7.0).margin(0.001));
}

TEST_CASE("Tabulated cells follow ISO 14644-1 Table 1") {
    CHECK(cleanroom::classLimitTabulated(1, 0.2));
    CHECK_FALSE(cleanroom::classLimitTabulated(1, 0.3));
    CHECK(cleanroom::classLimitTabulated(3, 1.0));
    CHECK_FALSE(cleanroom::classLimitTabulated(5, 5.0));
    CHECK(cleanroom::classLimitTabulated(6, 5.0));
    CHECK_FALSE(cleanroom::classLimitTabulated(7, 0.3));
}

TEST_CASE("Sampling locations Table A.1 and sample volume") {
    CHECK(cleanroom::samplingLocations(1.5) == 1);
    CHECK(cleanroom::samplingLocations(10) == 5);
    CHECK(cleanroom::samplingLocations(11) == 6);
    CHECK(cleanroom::samplingLocations(120) == 19);
    CHECK(cleanroom::samplingLocations(1000) == 27);
    CHECK(cleanroom::samplingLocations(2500) == 68);
    // ISO 7, ≥ 5 µm: 20 / 2930 · 1000 = 6.83 l
    CHECK(cleanroom::sampleVolumeLitres(2930) == Approx(6.826).epsilon(1e-3));
    CHECK(cleanroom::sampleVolumeLitres(352000) == Approx(2.0));
}

TEST_CASE("Recovery and leakage") {
    CHECK(cleanroom::recoveryTimeHours(20) * 60 == Approx(13.82).epsilon(1e-3));
    // 0.0186 m² gap at 15 Pa: 0.61 · 0.0186 · √(25) = 0.05673 m³/s
    CHECK(cleanroom::leakageFlow(0.0186, 15) == Approx(0.05673).epsilon(1e-3));
}

TEST_CASE("PV temperature correction and string limits") {
    CHECK(pv::atTemperature(37.5, -0.27, -10) == Approx(41.04375));
    const pv::StringInput in{37.5, 31.3, -0.27, -0.34, -10, 70, 1000, 200, 800};
    const auto lim = pv::stringLimits(in);
    CHECK(lim.vocMin == Approx(41.044).epsilon(1e-4));
    CHECK(lim.vmpMax == Approx(26.511).epsilon(1e-4));
    CHECK(lim.maxByVoltage == 24);
    CHECK(lim.maxByMpp == 22);  // 800 V / (31.3 · 1.119) = 22.8
    CHECK(lim.minByMpp == 8);
    CHECK_FALSE(pv::stringFuseRequired(2, 13.9, 25));
    CHECK(pv::stringFuseRequired(3, 13.9, 25));
}

TEST_CASE("Moist air at 20 °C / 50 %") {
    CHECK(air::saturationPressure(20) == Approx(23.33).epsilon(2e-3));
    CHECK(air::dewPoint(20, 50) == Approx(9.26).margin(0.05));
    CHECK(air::relativeHumidity(20, air::dewPoint(20, 50)) == Approx(50));
    const double x = air::humidityRatio(air::saturationPressure(20) * 0.5, 1013.25);
    CHECK(x * 1000 == Approx(7.24).epsilon(5e-3));
    CHECK(air::enthalpy(20, x) == Approx(38.5).epsilon(5e-3));
    CHECK(air::density(20, 11.66, 1013.25) == Approx(1.199).epsilon(2e-3));
}

TEST_CASE("CO₂ balance") {
    CHECK(air::co2Airflow(1, 20, 1000, 400) == Approx(33.33).epsilon(1e-3));
    CHECK(air::co2Concentration(1, 20, 33.333, 400) == Approx(1000).epsilon(1e-3));
}

namespace {
double run(const char* id, const char* out, std::initializer_list<std::pair<const char*, double>> values = {}) {
    const auto* c = findCalculator(id);
    REQUIRE(c);
    auto in = defaultInputs(*c);
    for (const auto& [k, v] : values) in.set(k, v);
    const auto r = evaluate(*c, in, {});
    const auto it = r.values.find(out);
    REQUIRE(it != r.values.end());
    return it->second;
}
}  // namespace

TEST_CASE("Data centre calculators") {
    CHECK(run("dc_pue", "pue") == Approx(650.0 / 450.0));
    // 20 racks · 8 kW = 160 kW → 2 units of 100 kW + 1 redundant
    CHECK(run("dc_rack_cooling", "units") == 3);
    CHECK(run("dc_rack_cooling", "qr") == Approx(8 * 3600 / (1.2 * 1.005 * 12)));
    // (150 + 10) · 1.2 = 192 kW → ⌈192 / 40⌉ = 5 modules, N+1 = 6
    CHECK(run("dc_ups", "n") == 5);
    CHECK(run("dc_ups", "ntot") == 6);
    CHECK(run("dc_ups", "E") == Approx(192.0 * 10 / 60 / 0.96));
    CHECK(run("dc_availability", "h", {{"A", 99.982}}) == Approx(1.5768).epsilon(1e-3));
    CHECK(run("dc_availability", "Atot", {{"A", 99.0}, {"k", 2}}) == Approx(99.99));
}

TEST_CASE("Cleanroom and PV calculators end to end") {
    CHECK(run("iso14644_class", "limit") == 352000);
    CHECK(run("iso14644_sampling", "NL") == 19);
    CHECK(run("ffu_ceiling", "n") == 35);  // 100 m² · 25 % / 0.72 m² = 34.7 → 35
    CHECK(run("pv_yield", "E") == Approx(9500));
    CHECK(run("fresh_air", "q") == Approx((10 * 7 + 80 * 0.7) * 3.6));
}
