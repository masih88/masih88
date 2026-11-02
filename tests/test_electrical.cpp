// Area "Elektrotechnik" (v1.7): formulas with reference values (hand calculation, NIST, NEC, IEC tables).
#include <algorithm>
#include <cmath>
#include <set>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/electrical.hpp"
#include "elc/formulas/thermocouple.hpp"
#include "elc/formulas/units.hpp"
#include "elc/lists.hpp"
#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;
using namespace elc::electrical;

TEST_CASE("Voltage, power factor and impedance from measured values", "[electrical]") {
    CHECK(voltageFromPower(System::AC3, 10000, 16, 0.9) == Approx(400.94).epsilon(1e-4));
    CHECK(voltageFromPower(System::DC, 240, 10, 0.5) == Approx(24.0));
    CHECK(powerFactor(System::AC1, 2000, 230, 10) == Approx(0.869565).epsilon(1e-5));
    const auto z = impedanceFromUI(System::AC1, 230, 10, 0.8);
    CHECK(z.Z == Approx(23.0));
    CHECK(z.R == Approx(18.4));
    CHECK(z.X == Approx(13.8));
    CHECK(impedanceFromUI(System::AC3, 400, 10, 1.0).Z == Approx(23.094).epsilon(1e-4));
    // 10 kW installed, k_u 0.75, k_s 0.8 → 6 kW; 400 V, cos φ 0.85, η 0.9
    CHECK(operatingCurrent(System::AC3, 400, 10000, 0.75, 0.8, 0.85, 0.9) == Approx(11.320).epsilon(1e-4));
    CHECK(jouleHeat(10, 2, 60) == Approx(2400));
    CHECK(temperatureRise(4186, 1, 4186) == Approx(1.0));
}

TEST_CASE("Cable temperature, neutral current and harmonic sizing", "[electrical]") {
    CHECK(cableTemperature(20, 27, 30, 70) == Approx(51.95).epsilon(1e-3));
    CHECK(cableTemperature(27, 27, 30, 70) == Approx(70.0));
    CHECK(neutralCurrentFundamental(10, 10, 10) == Approx(0.0).margin(1e-9));
    CHECK(neutralCurrentFundamental(10, 0, 0) == Approx(10.0));
    CHECK(neutralCurrentFundamental(20, 10, 10) == Approx(10.0));
    CHECK(neutralCurrent(10, 10, 10, 20) == Approx(6.0));  // 3 · 20 % · 10 A
    // IEC 60364-5-52 Table E.52.1
    CHECK_FALSE(harmonicSizing(10).byNeutral);
    CHECK(harmonicSizing(10).factor == Approx(1.0));
    CHECK_FALSE(harmonicSizing(20).byNeutral);
    CHECK(harmonicSizing(20).factor == Approx(0.86));
    CHECK(harmonicSizing(40).byNeutral);
    CHECK(harmonicSizing(40).factor == Approx(0.86));
    CHECK(harmonicSizing(50).byNeutral);
    CHECK(harmonicSizing(50).factor == Approx(1.0));
}

TEST_CASE("Distributed loads: moment, voltage drop and cross-section", "[electrical]") {
    // 4 loads of 10 A at 10, 20, 30, 40 m
    const double m = distributedMoment(4, 10, 10, 10);
    CHECK(m == Approx(1000.0));
    CHECK(distributedVoltageDrop(System::AC1, m, 2.5, 0.0178, 1.0, 0.0) == Approx(14.24));
    CHECK(distributedRequiredArea(System::AC1, m, 0.0178, 1.0, 0.0, 14.24) == Approx(2.5));
    CHECK(std::isinf(distributedRequiredArea(System::AC3, m, 0.0178, 0.5, 1e-3, 0.1)));
}

TEST_CASE("Compensation of transformers and capacitors at other voltages", "[electrical]") {
    CHECK(transformerNoLoadQ(630, 1.5) == Approx(9.45));
    CHECK(transformerLoadQ(630, 6, 630) == Approx(37.8));
    CHECK(transformerLoadQ(630, 6, 315) == Approx(9.45));
    CHECK(capacitorPowerAt(25, 400, 50, 440, 50) == Approx(30.25));
    CHECK(capacitorPowerAt(25, 400, 50, 400, 60) == Approx(30.0));
    CHECK(capacitanceFor(1000, 230, 50) == Approx(60.17e-6).epsilon(1e-3));
}

TEST_CASE("Earthing, minimum short-circuit current and overvoltage risk", "[electrical]") {
    CHECK(maxEarthResistanceTT(50, 0.03) == Approx(1666.67).epsilon(1e-4));
    CHECK(maxLoopImpedanceTN(230, 80) == Approx(2.875));
    // 100 m 3 × 2.5 mm² Cu at 70 °C, no upstream impedance: ρ70 = 0.017241·(1 + 0.00393·50)
    const auto sc = minShortCircuit(0.95, 230, 0.0, 0.0, 100, 2.5, 2.5, 2.5, cables::Material::Cu, 70, 0.0);
    CHECK(sc.rLine == Approx(0.82516).epsilon(1e-4));
    CHECK(sc.ik1 == Approx(132.40).epsilon(1e-3));
    CHECK(sc.ik1N == Approx(132.40).epsilon(1e-3));
    CHECK(sc.ik2 == Approx(229.32).epsilon(1e-3));
    // 443.5: L_P = 2·0.2 + 0.3 + 0.4·0.5 = 0.9 km; rural f_env 85, N_g 2 → CRL 47 < 1000
    CHECK(surgeLP(0.2, 0.3, 0.5, 0.0) == Approx(0.9));
    CHECK(surgeCRL(85, 0.9, 2) == Approx(47.22).epsilon(1e-3));
}

TEST_CASE("Transformer windings, antenna and CCTV", "[electrical]") {
    CHECK(turnsPerVolt(50, 1.2, 10e-4) == Approx(3.7538).epsilon(1e-4));
    CHECK(wavelength(100e6) == Approx(2.99792458));
    CHECK(cctvBitsPerPixel("h265") < cctvBitsPerPixel("h264"));
    CHECK(cctvBitsPerPixel("mjpeg") > cctvBitsPerPixel("h264"));
}

TEST_CASE("Zener stabiliser and Δ-Y transformation", "[electrical]") {
    const auto z = zenerDesign(12, 15, 5.1, 0.005, 0.0, 0.020);
    CHECK(z.Rv == Approx(276.0));
    CHECK(z.IzMax == Approx(0.035870).epsilon(1e-4));
    CHECK(z.PzMax == Approx(0.18293).epsilon(1e-4));
    CHECK(z.PR == Approx(0.35511).epsilon(1e-4));
    const auto y = deltaToStar(10, 20, 30);
    CHECK(y[0] == Approx(5.0));
    CHECK(y[1] == Approx(10.0 / 3.0));
    CHECK(y[2] == Approx(10.0));
    const auto d = starToDelta(y[0], y[1], y[2]);
    CHECK(d[0] == Approx(10.0));
    CHECK(d[1] == Approx(20.0));
    CHECK(d[2] == Approx(30.0));
    const auto e = deltaToStar(30, 30, 30);
    CHECK(e[0] == Approx(10.0));
}

TEST_CASE("SMD resistor, capacitor and inductor codes", "[electrical]") {
    CHECK(smdResistor("472")->ohms == Approx(4700));
    CHECK(smdResistor("4702")->ohms == Approx(47000));
    CHECK(smdResistor("4R7")->ohms == Approx(4.7));
    CHECK(smdResistor("R47")->ohms == Approx(0.47));
    CHECK(smdResistor("000")->ohms == Approx(0.0));
    CHECK(smdResistor("01C")->ohms == Approx(10000));  // EIA-96: 01 = 100, C = ×100
    CHECK(smdResistor("68X")->ohms == Approx(49.9));   // 68 = 499, X = ×0.1
    CHECK(smdResistor("01c")->kind == "EIA-96");
    CHECK_FALSE(smdResistor("4Q7"));
    CHECK_FALSE(smdResistor(""));
    CHECK(eia96Values().size() == 96);

    const auto c1 = capacitorCode("104K");
    REQUIRE(c1);
    CHECK(c1->pF == Approx(100000));
    CHECK(c1->tolerancePercent.value() == Approx(10));
    const auto c2 = capacitorCode("2A104J");
    REQUIRE(c2);
    CHECK(c2->voltage.value() == Approx(100));
    CHECK(c2->tolerancePercent.value() == Approx(5));
    CHECK(capacitorCode("4n7")->pF == Approx(4700));
    CHECK(capacitorCode("1R5")->pF == Approx(1.5));
    CHECK(capacitorCode("0.1")->pF == Approx(100000));   // decimal number = µF
    CHECK(capacitorCode("47")->pF == Approx(47));        // two digits = pF
    CHECK(capacitorCode("229")->pF == Approx(2.2));      // multiplier digit 9 = ×0.1
    CHECK(capacitorCode("22u")->pF == Approx(22e6));
    CHECK(capacitorCode("1H104")->voltage.value() == Approx(50));
    CHECK_FALSE(capacitorCode("xyz"));

    CHECK(inductorCode("101").value() == Approx(100));
    CHECK(inductorCode("4R7").value() == Approx(4.7));
    CHECK(inductorCode("R10").value() == Approx(0.1));
    CHECK(inductorCode("47N").value() == Approx(0.047));
    CHECK(inductorCode("102").value() == Approx(1000));
}

TEST_CASE("Colour bands for a value", "[electrical]") {
    const auto b = bandsFor(4700, 2);
    REQUIRE(b);
    CHECK(b->digits == std::vector<int>{4, 7});
    CHECK(b->exponent == 2);
    const auto b5 = bandsFor(4700, 3);
    REQUIRE(b5);
    CHECK(b5->digits == std::vector<int>{4, 7, 0});
    CHECK(b5->exponent == 1);
    CHECK(bandsFor(0.47, 2)->exponent == -2);
    CHECK(bandsFor(1, 2)->digits == std::vector<int>{1, 0});
    CHECK(bandsFor(1, 2)->exponent == -1);
    CHECK(bandsFor(4750, 2)->value == Approx(4800));  // rounded to 2 significant digits
    CHECK_FALSE(bandsFor(0.05, 2));                   // would need ×0.001
    CHECK_FALSE(bandsFor(-1, 2));
}

TEST_CASE("Wire gauges", "[electrical]") {
    CHECK(swgTable().size() == 57);
    for (const auto& g : swgTable()) {
        if (g.name == "10") CHECK(g.inch * kMmPerInch == Approx(3.2512));
        if (g.name == "7/0") CHECK(g.inch == Approx(0.5));
        if (g.name == "50") CHECK(g.inch == Approx(0.001));
    }
    CHECK(units::awgArea(10) == Approx(5.261).epsilon(1e-3));
    CHECK(250 * kMm2PerKcmil == Approx(126.68).epsilon(1e-4));
}

TEST_CASE("NEC full-load currents (Tables 430.248 and 430.250)", "[electrical]") {
    CHECK(necFullLoadCurrent(3, 10, 460).value() == Approx(14));
    CHECK(necFullLoadCurrent(3, 30, 575).value() == Approx(32));
    CHECK(necFullLoadCurrent(3, 100, 208).value() == Approx(273));
    CHECK(necFullLoadCurrent(3, 500, 2300).value() == Approx(118));
    CHECK_FALSE(necFullLoadCurrent(3, 250, 230));
    CHECK_FALSE(necFullLoadCurrent(3, 3, 115));
    CHECK(necFullLoadCurrent(1, 1, 230).value() == Approx(8.0));
    CHECK(necFullLoadCurrent(1, 10, 115).value() == Approx(100));
    CHECK(necFullLoadCurrent(1, 1.0 / 6.0, 208).value() == Approx(2.4));
}

TEST_CASE("Busbar and mineral-insulated cable ratings", "[electrical]") {
    bool found = false;
    for (const auto& b : busbars()) {
        if (b.width == 30 && b.thickness == 10) {
            CHECK(b.i1 == Approx(630));
            CHECK(b.i2 == Approx(1250));
            found = true;
        }
    }
    CHECK(found);
    CHECK(miAmpacity(MiDuty::Heavy750V, MiConfig::ThreeCore3ph, 10).value() == Approx(65));
    CHECK(miAmpacity(MiDuty::Light500V, MiConfig::TwoCore, 2.5).value() == Approx(31));
    CHECK(miAmpacity(MiDuty::Heavy750V, MiConfig::SingleCore1ph, 240).value() == Approx(514));
    CHECK(miAmpacity(MiDuty::Heavy750V, MiConfig::SingleCore3ph, 240).value() == Approx(457));
    CHECK_FALSE(miAmpacity(MiDuty::Light500V, MiConfig::TwoCore, 10));
    CHECK(miAreas(MiDuty::Light500V, MiConfig::TwoCore).size() == 4);
}

TEST_CASE("Thermocouples per NIST ITS-90 (reference tables)", "[electrical]") {
    using namespace elc::thermocouple;
    CHECK(emf(Type::K, 100).value() == Approx(4.096).margin(0.001));
    CHECK(emf(Type::K, -100).value() == Approx(-3.554).margin(0.001));
    CHECK(emf(Type::K, 1000).value() == Approx(41.276).margin(0.001));
    CHECK(emf(Type::J, 100).value() == Approx(5.269).margin(0.001));
    CHECK(emf(Type::T, 100).value() == Approx(4.279).margin(0.001));
    CHECK(emf(Type::E, 100).value() == Approx(6.319).margin(0.001));
    CHECK(emf(Type::N, 100).value() == Approx(2.774).margin(0.001));
    CHECK(emf(Type::S, 1000).value() == Approx(9.587).margin(0.001));
    CHECK(emf(Type::R, 1000).value() == Approx(10.506).margin(0.001));
    CHECK(emf(Type::B, 1000).value() == Approx(4.834).margin(0.001));
    CHECK_FALSE(emf(Type::T, 500));
    CHECK(temperature(Type::K, 4.096).value() == Approx(100).margin(0.02));
    CHECK(temperature(Type::J, emf(Type::J, 345.6).value()).value() == Approx(345.6).margin(1e-4));
    CHECK_FALSE(temperature(Type::K, 80));
}

TEST_CASE("Area Elektrotechnik: groups reference existing items, each once, and cover the category", "[electrical]") {
    const auto groups = categoryGroups("electrical");
    REQUIRE(groups.size() >= 8);
    std::set<std::string> seen;
    for (const auto& g : groups) {
        INFO(g.title.de);
        CHECK_FALSE(g.title.de.empty());
        CHECK_FALSE(g.title.en.empty());
        CHECK_FALSE(g.items.empty());
        std::set<std::string> inGroup;  // an item may appear in a topic group and in "Ressourcen", but only once per group
        for (const auto& id : g.items) {
            INFO(id);
            CHECK((findCalculator(id) != nullptr || findTable(id) != nullptr || findList(id) != nullptr));
            CHECK(inGroup.insert(id).second);
            seen.insert(id);
        }
    }
    for (const auto& c : calculators()) {
        if (c.category == "electrical") {
            INFO(c.id);
            CHECK(seen.contains(c.id));
        }
    }
    for (const auto& t : tables()) {
        if (t.category == "electrical") {
            INFO(t.id);
            CHECK(seen.contains(t.id));
        }
    }
    // other categories keep their flat list
    const auto basics = categoryGroups("basics");
    REQUIRE(basics.size() == 1);
    CHECK(basics.front().title.de.empty());
    CHECK_FALSE(basics.front().items.empty());
}

TEST_CASE("Ressourcen: computed tables match the formulas", "[electrical]") {
    // voltage drop per A and m: 2.5 mm² Cu at 70 °C, 1~, cos φ = 1 → 2 · ρ70 / 2.5 · 1000 = 16.50 mV/(A·m)
    const auto* t = findTable("vdrop_per_length");
    REQUIRE(t != nullptr);
    CHECK(t->rows[1][0].en == "2.5");
    CHECK(t->rows[1][1].en == "16.5");
    // reactance of multi-core cables lies around the usual design value 0.08 mΩ/m
    const auto* x = findTable("cable_reactance_table");
    REQUIRE(x != nullptr);
    for (const auto& row : x->rows) {
        const double v = parseNumber(row[1].en).value();
        CHECK(v > 0.06);
        CHECK(v < 0.12);
    }
    // the Ressourcen group lists every item of the original resources tab
    const auto groups = categoryGroups("electrical");
    const auto it = std::find_if(groups.begin(), groups.end(), [](const CategoryGroup& g) { return g.title.de == "Ressourcen"; });
    REQUIRE(it != groups.end());
    CHECK(it->items.size() >= 27);
}

TEST_CASE("Tripping curve chart is logarithmic and monotonic", "[electrical]") {
    const auto* c = findCalculator("trip_curves");
    REQUIRE(c != nullptr);
    for (const char* type : {"B", "C", "D"}) {
        INFO(type);
        Inputs in = defaultInputs(*c);
        in.setChoice("type", type);
        const auto ch = evaluateChart(*c, in, Context{});
        REQUIRE(ch);
        CHECK(ch->xLog);
        CHECK(ch->yLog);
        REQUIRE(ch->series.size() == 2);
        for (const auto& s : ch->series) {
            for (std::size_t i = 1; i < s.pts.size(); ++i) {
                CHECK(s.pts[i][0] >= s.pts[i - 1][0] - 1e-9);  // current never decreases
                CHECK(s.pts[i][1] <= s.pts[i - 1][1] + 1e-9);  // time never increases
                CHECK(s.pts[i][1] > 0);
            }
        }
    }
}
