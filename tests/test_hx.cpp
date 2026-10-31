// h,x-diagram (Mollier): moist-air formulas, state changes, chart and picking; PUE balance.
#include <algorithm>
#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/air.hpp"
#include "elc/formulas/datacenter.hpp"
#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;
using namespace elc::air;

namespace {
constexpr double kP0 = 1013.25;
}

TEST_CASE("Standard atmosphere pressure") {
    CHECK(pressureAtAltitude(0) == Approx(1013.25));
    CHECK(pressureAtAltitude(1000) == Approx(898.75).margin(0.3));  // ISA table
    CHECK(pressureAtAltitude(300) < pressureAtAltitude(0));
}

TEST_CASE("Saturation, dew point and wet bulb against table values") {
    // Recknagel / Mollier chart: x_s(20 °C, 1013 hPa) ≈ 14.7 g/kg
    CHECK(saturationRatio(20, kP0) * 1000 == Approx(14.7).margin(0.1));
    const double x = ratioFromRh(20, 50, kP0);
    CHECK(rhFromRatio(20, x, kP0) == Approx(50));
    CHECK(dewPointFromRatio(x, kP0) == Approx(dewPoint(20, 50)).margin(1e-6));
    CHECK(dewPoint(20, 50) == Approx(9.26).margin(0.05));
    // psychrometric chart: 20 °C / 50 % → wet bulb ≈ 13.7 °C
    CHECK(wetBulb(20, x, kP0) == Approx(13.7).margin(0.3));
    CHECK(wetBulb(20, saturationRatio(20, kP0), kP0) == Approx(20).margin(1e-3));
}

TEST_CASE("Mollier coordinates and enthalpy inverse") {
    CHECK(mollierY(25, 0) == Approx(25));
    CHECK(mollierY(25, 0.010) == Approx(25 * (1.006 + 0.0186) / 1.006));
    CHECK(mollierT(mollierY(-12, 0.002), 0.002) == Approx(-12));
    CHECK(temperatureFromHX(enthalpy(25, 0.010), 0.010) == Approx(25));
}

TEST_CASE("Heating and cooling with dehumidification") {
    const AirState s{32, ratioFromRh(32, 40, kP0)};
    const double td = dewPointFromRatio(s.x, kP0);
    CHECK(td == Approx(dewPoint(32, 40)).margin(1e-6));

    const auto heated = coolOrHeat(s, 40, kP0);
    CHECK(heated.x == Approx(s.x));
    CHECK(heated.t == Approx(40));

    const auto dry = coolOrHeat(s, td + 2, kP0);  // above the dew point: x stays
    CHECK(dry.x == Approx(s.x));

    const auto wet = coolOrHeat(s, 14, kP0);  // below the dew point: saturated at 14 °C
    CHECK(wet.t == Approx(14));
    CHECK(wet.x == Approx(saturationRatio(14, kP0)));
    CHECK(rhFromRatio(wet.t, wet.x, kP0) == Approx(100));
}

TEST_CASE("Humidification and mixing") {
    const AirState dry{22, ratioFromRh(22, 20, kP0)};
    const auto steam = steamHumidify(dry, 50, kP0);
    CHECK(steam.t == Approx(22));
    CHECK(rhFromRatio(steam.t, steam.x, kP0) == Approx(50).margin(1e-6));

    const AirState warm{30, ratioFromRh(30, 30, kP0)};
    const auto evap = adiabaticHumidify(warm, 90, kP0);
    CHECK(enthalpy(evap.t, evap.x) == Approx(enthalpy(warm.t, warm.x)).margin(1e-6));
    CHECK(rhFromRatio(evap.t, evap.x, kP0) == Approx(90).margin(1e-4));
    CHECK(evap.t < warm.t);
    CHECK(evap.t > wetBulb(warm.t, warm.x, kP0));

    const AirState outdoor{-10, ratioFromRh(-10, 80, kP0)};
    const AirState room{22, ratioFromRh(22, 40, kP0)};
    const auto m = mix(outdoor, room, 0.7);
    CHECK(m.x == Approx(0.3 * outdoor.x + 0.7 * room.x));
    CHECK(enthalpy(m.t, m.x) == Approx(0.3 * enthalpy(outdoor.t, outdoor.x) + 0.7 * enthalpy(room.t, room.x)));
}

TEST_CASE("h,x calculator: summer cooling with dehumidification") {
    const auto* c = findCalculator("hx_diagram");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.set("alt", 0);
    in.set("t1", 32);
    in.set("rh1", 40);
    in.setChoice("proc", "cool");
    in.set("t2", 14);
    in.set("V", 10000);
    const auto r = evaluate(*c, in, {});
    REQUIRE_FALSE(r.hasError());
    CHECK(r.values.at("x1") == Approx(ratioFromRh(32, 40, kP0) * 1000));
    CHECK(r.values.at("td1") == Approx(dewPoint(32, 40)).margin(1e-6));
    CHECK(r.values.at("x2") == Approx(saturationRatio(14, kP0) * 1000));
    CHECK(r.values.at("rh2") == Approx(100));
    CHECK(r.values.at("Q") < 0);       // cooling
    CHECK(r.values.at("water") < 0);   // condensate
    const double rho = density(32, saturationPressure(32) * 0.4, kP0);
    const double m = 10000 * rho / 3600;
    CHECK(r.values.at("Q") == Approx(m * (enthalpy(14, saturationRatio(14, kP0)) - enthalpy(32, ratioFromRh(32, 40, kP0)))));
}

TEST_CASE("h,x chart: grid, state points and picking") {
    const auto* c = findCalculator("hx_diagram");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.set("alt", 0);
    in.set("t1", 26);
    in.set("rh1", 50);
    in.setChoice("proc", "heat");
    in.set("t2", 35);
    const auto chart = evaluateChart(*c, in, {});
    REQUIRE(chart);
    CHECK(chart->x.min == Approx(0));
    CHECK(chart->y.min < 0);

    const auto sat = std::find_if(chart->series.begin(), chart->series.end(), [](const ChartSeries& s) { return s.style == "sat"; });
    REQUIRE(sat != chart->series.end());
    REQUIRE(sat->pts.size() > 10);
    for (std::size_t i = 1; i < sat->pts.size(); ++i) CHECK(sat->pts[i][0] > sat->pts[i - 1][0]);

    const auto p1 = std::find_if(chart->markers.begin(), chart->markers.end(), [](const ChartMarker& m) { return m.label == "1"; });
    REQUIRE(p1 != chart->markers.end());
    const double x1 = ratioFromRh(26, 50, kP0);
    CHECK(p1->x == Approx(x1 * 1000));
    CHECK(p1->y == Approx(mollierY(26, x1)));
    CHECK(std::any_of(chart->markers.begin(), chart->markers.end(), [](const ChartMarker& m) { return m.label == "2"; }));

    // tapping on state point 1 gives its temperature and humidity back
    const auto picked = pickChart(*c, in, p1->x, p1->y, {});
    REQUIRE(picked.size() >= 2);
    for (const auto& v : picked) {
        if (v.id == "t1") CHECK(v.number == Approx(26).margin(1e-6));
        if (v.id == "rh1") CHECK(v.number == Approx(50).margin(1e-6));
    }
    // above the saturation curve the pick is clamped to 100 %
    const auto fog = pickChart(*c, in, 25, 10, {});
    for (const auto& v : fog) {
        if (v.id == "rh1") CHECK(v.number == Approx(100));
    }
}

TEST_CASE("Moist air calculator also draws its state in the chart") {
    const auto* c = findCalculator("humid_air");
    REQUIRE(c);
    const auto chart = evaluateChart(*c, defaultInputs(*c), {});
    REQUIRE(chart);
    CHECK_FALSE(chart->markers.empty());
}

TEST_CASE("PUE balance from the consumer breakdown (hand calculation)") {
    datacenter::PueInputs in;
    in.itPower = 400;
    in.upsEfficiency = 0.95;
    in.distributionLoss = 0.02;
    in.otherPower = 10;
    in.eer = 4;
    in.freeCooling = 0.4;
    in.auxShare = 0.08;
    const auto b = datacenter::pueBalance(in);
    CHECK(b.upsLoss == Approx(21.0526).epsilon(1e-5));
    CHECK(b.distLoss == Approx(8.42105).epsilon(1e-5));
    CHECK(b.coolingLoad == Approx(431.0526).epsilon(1e-6));
    CHECK(b.coolingPower == Approx(99.1421).epsilon(1e-5));
    CHECK(b.total == Approx(538.6158).epsilon(1e-6));
    CHECK(b.pue == Approx(1.34654).epsilon(1e-5));
    CHECK(b.pPueCooling == Approx(1.24786).epsilon(1e-5));
    CHECK(b.pPuePower == Approx(1.07368).epsilon(1e-5));
}

TEST_CASE("PUE balance calculator with EnEfG check") {
    const auto* c = findCalculator("dc_pue_balance");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.set("Pit", 400);
    in.set("eta", 95);
    in.set("dist", 2);
    in.set("Pother", 10);
    in.set("eer", 4);
    in.set("free", 40);
    in.set("aux", 8);
    in.set("reuse", 0);
    const auto r = evaluate(*c, in, {});
    REQUIRE_FALSE(r.hasError());
    CHECK(r.values.at("pue") == Approx(1.34654).epsilon(1e-5));
    CHECK(r.values.at("Etot") == Approx(538.6158 * 8.76).epsilon(1e-6));
    CHECK(r.values.at("erf") == Approx(0));

    in.setChoice("start", "new");  // new build: PUE ≤ 1.2 → not met
    const auto rNew = evaluate(*c, in, {});
    CHECK(std::any_of(rNew.messages.begin(), rNew.messages.end(), [](const Message& m) { return m.severity == Severity::Warning; }));
}

TEST_CASE("PUE from metered annual energy") {
    const auto* c = findCalculator("dc_pue");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.setChoice("basis", "energy");
    in.set("Etot", 4200);
    in.set("Eit", 3000);
    const auto r = evaluate(*c, in, {});
    REQUIRE_FALSE(r.hasError());
    CHECK(r.values.at("pue") == Approx(1.4));
    CHECK(r.values.at("Eyear") == Approx(4200));
}
