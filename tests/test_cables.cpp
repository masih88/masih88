#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/cables.hpp"

using Catch::Approx;
using namespace elc;
using namespace elc::cables;

TEST_CASE("Resistivity with temperature") {
    CHECK(resistivity(Material::Cu, 20) == Approx(0.017241));
    CHECK(resistivity(Material::Cu, 70) == Approx(0.020629).epsilon(1e-4));
    CHECK(resistivity(Material::Al, 20) == Approx(0.028264));
}

TEST_CASE("Voltage drop single-phase, resistive") {
    // 2 · 30 m · 16 A · 0.0225 / 2.5 mm² = 8.64 V
    CHECK(voltageDrop(System::AC1, 30, 16, 2.5, 0.0225, 1.0, 0.0) == Approx(8.64));
    // DC ignores cos φ and reactance
    CHECK(voltageDrop(System::DC, 30, 16, 2.5, 0.0225, 0.5, 1e-3) == Approx(8.64));
}

TEST_CASE("Voltage drop three-phase with reactance") {
    // √3 · 50 · 32 · (0.0225/6 · 0.85 + 0.08e-3 · 0.52678) = 8.9502 V
    CHECK(voltageDrop(System::AC3, 50, 32, 6, 0.0225, 0.85, 0.08e-3) == Approx(8.9502).epsilon(1e-4));
}

TEST_CASE("Required area and max length invert the voltage drop") {
    const double A = requiredArea(System::AC3, 80, 40, 0.0206, 0.9, 0.08e-3, 12.0);
    CHECK(voltageDrop(System::AC3, 80, 40, A, 0.0206, 0.9, 0.08e-3) == Approx(12.0));
    const double L = maxLength(System::DC, 2.0, 1.5, 0.0178, 1.0, 0.0, 1.2);
    CHECK(voltageDrop(System::DC, L, 2.0, 1.5, 0.0178, 1.0, 0.0) == Approx(1.2));
    CHECK(requiredArea(System::AC3, 1000, 100, 0.0206, 0.5, 1e-3, 1.0) == kInf);
}

TEST_CASE("Line losses") {
    // 3 · 32² · 0.0225 · 50 / 6 = 576 W
    CHECK(lineLoss(System::AC3, 50, 32, 6, 0.0225) == Approx(576));
}

TEST_CASE("Ampacity table spot checks (IEC 60364-5-52 / DIN VDE 0298-4)") {
    CHECK(baseAmpacity(Insulation::PVC, Method::C, 2, 2.5).value() == Approx(27));
    CHECK(baseAmpacity(Insulation::PVC, Method::B2, 3, 1.5).value() == Approx(15));
    CHECK(baseAmpacity(Insulation::PVC, Method::A2, 3, 16).value() == Approx(52));
    CHECK(baseAmpacity(Insulation::PVC, Method::E, 2, 10).value() == Approx(70));
    CHECK(baseAmpacity(Insulation::XLPE, Method::E, 3, 300).value() == Approx(621));
    CHECK(baseAmpacity(Insulation::XLPE, Method::C, 2, 1.5).value() == Approx(24));
    CHECK_FALSE(baseAmpacity(Insulation::PVC, Method::A1, 2, 0.75).has_value());
    CHECK_FALSE(baseAmpacity(Insulation::PVC, Method::A1, 4, 2.5).has_value());
}

TEST_CASE("Ampacity tables grow with cross-section and XLPE > PVC") {
    const Method methods[] = {Method::A1, Method::A2, Method::B1, Method::B2, Method::C, Method::D1, Method::E};
    for (const auto m : methods) {
        for (int loaded : {2, 3}) {
            double prevPvc = 0, prevXlpe = 0;
            for (const double a : kAmpacityAreas) {
                const double pvc = baseAmpacity(Insulation::PVC, m, loaded, a).value();
                const double xlpe = baseAmpacity(Insulation::XLPE, m, loaded, a).value();
                CHECK(pvc > prevPvc);
                CHECK(xlpe > prevXlpe);
                CHECK(xlpe > pvc);
                prevPvc = pvc;
                prevXlpe = xlpe;
            }
        }
    }
}

TEST_CASE("Ambient temperature factor reproduces Table B.52.14") {
    CHECK(ambientFactor(Insulation::PVC, Method::C, 30) == Approx(1.0));
    CHECK(ambientFactor(Insulation::PVC, Method::C, 40) == Approx(0.87).margin(0.005));
    CHECK(ambientFactor(Insulation::PVC, Method::C, 25) == Approx(1.06).margin(0.005));
    CHECK(ambientFactor(Insulation::XLPE, Method::C, 50) == Approx(0.82).margin(0.005));
    CHECK(ambientFactor(Insulation::PVC, Method::D1, 20) == Approx(1.0));
    CHECK(ambientFactor(Insulation::PVC, Method::C, 70) == Approx(0.0));
}

TEST_CASE("Grouping factors Table B.52.17") {
    CHECK(groupingFactor(Arrangement::Bunched, 1) == Approx(1.0));
    CHECK(groupingFactor(Arrangement::Bunched, 3) == Approx(0.70));
    CHECK(groupingFactor(Arrangement::Bunched, 10) == Approx(0.45));
    CHECK(groupingFactor(Arrangement::Bunched, 30) == Approx(0.38));
    CHECK(groupingFactor(Arrangement::SingleLayerWall, 12) == Approx(0.70));
    CHECK(groupingFactor(Arrangement::SingleLayerCeiling, 1) == Approx(0.95));
    CHECK(groupingFactor(Arrangement::PerforatedTray, 2) == Approx(0.88));
}

TEST_CASE("Protective devices") {
    CHECK(conventionalTripFactor(Device::MCB_B, 16) == Approx(1.45));
    CHECK(conventionalTripFactor(Device::FuseGG, 4) == Approx(2.1));
    CHECK(conventionalTripFactor(Device::FuseGG, 10) == Approx(1.9));
    CHECK(conventionalTripFactor(Device::FuseGG, 63) == Approx(1.6));
    CHECK(instantaneousMultiple(Device::MCB_C) == Approx(10));
    CHECK(ratedCurrents(Device::FuseGG).back() == Approx(630));
}

TEST_CASE("Max length for disconnection, TN, B16, 2.5 mm²") {
    // 0.8 · 230 · 2.5 / (0.0225 · 2 · 80) = 127.8 m
    CHECK(maxLengthDisconnection(230, 2.5, 2.5, Material::Cu, 80) == Approx(127.78).epsilon(1e-4));
}

TEST_CASE("k factors Table 43A") {
    CHECK(kFactor(Material::Cu, Insulation::PVC) == Approx(115));
    CHECK(kFactor(Material::Cu, Insulation::XLPE) == Approx(143));
    CHECK(kFactor(Material::Al, Insulation::PVC) == Approx(76));
    CHECK(kFactor(Material::Al, Insulation::XLPE) == Approx(94));
}
