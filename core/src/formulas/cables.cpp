#include "elc/formulas/cables.hpp"

#include <cmath>
#include <cstddef>

namespace elc::cables {

double resistivity(Material m, double tempC) { return resistivity20(m) * (1.0 + alpha20(m) * (tempC - 20.0)); }

namespace {

void normalise(System sys, double& cosPhi, double& xPerM) {
    if (sys == System::DC) {
        cosPhi = 1.0;
        xPerM = 0.0;
    }
}

double sinOf(double cosPhi) { return std::sqrt(std::max(0.0, 1.0 - cosPhi * cosPhi)); }

}  // namespace

double voltageDrop(System sys, double L, double I, double A, double rho, double cosPhi, double xPerM) {
    normalise(sys, cosPhi, xPerM);
    return dropFactorK(sys) * L * I * (rho / A * cosPhi + xPerM * sinOf(cosPhi));
}

double requiredArea(System sys, double L, double I, double rho, double cosPhi, double xPerM, double dUmax) {
    normalise(sys, cosPhi, xPerM);
    const double k = dropFactorK(sys) * L * I;
    const double reactivePart = k * xPerM * sinOf(cosPhi);
    const double room = dUmax - reactivePart;
    if (room <= 0.0) return kInf;
    return k * rho * cosPhi / room;
}

double maxLength(System sys, double I, double A, double rho, double cosPhi, double xPerM, double dUmax) {
    normalise(sys, cosPhi, xPerM);
    return dUmax / (dropFactorK(sys) * I * (rho / A * cosPhi + xPerM * sinOf(cosPhi)));
}

double lineLoss(System sys, double L, double I, double A, double rho) {
    const double conductors = sys == System::AC3 ? 3.0 : 2.0;
    return conductors * I * I * rho * L / A;
}

// -----------------------------------------------------------------------------------------
// Current-carrying capacity, copper, IEC 60364-5-52:2009 Annex B (= DIN VDE 0298-4 / NIN 5.2.3).
// Columns: A1, A2, B1, B2, C, D1, E. Rows: kAmpacityAreas (1.5 … 300 mm²).
// Please check against the edition in force before relying on the values.
// -----------------------------------------------------------------------------------------
namespace {

using Row = std::array<double, 7>;
using Grid = std::array<Row, 16>;

// PVC 70 °C, 2 loaded conductors (B.52.2, B.52.10 for E)
constexpr Grid kPvc2{{
    {14.5, 14, 17.5, 16.5, 19.5, 22, 22},
    {19.5, 18.5, 24, 23, 27, 29, 30},
    {26, 25, 32, 30, 36, 37, 40},
    {34, 32, 41, 38, 46, 46, 51},
    {46, 43, 57, 52, 63, 60, 70},
    {61, 57, 76, 69, 85, 78, 94},
    {80, 75, 101, 90, 112, 99, 119},
    {99, 92, 125, 111, 138, 119, 148},
    {119, 110, 151, 133, 168, 140, 180},
    {151, 139, 192, 168, 213, 173, 232},
    {182, 167, 232, 201, 258, 204, 282},
    {210, 192, 269, 232, 299, 231, 328},
    {240, 219, 300, 258, 344, 261, 379},
    {273, 248, 341, 294, 392, 292, 434},
    {321, 291, 400, 344, 461, 336, 514},
    {367, 334, 458, 394, 530, 379, 593},
}};

// PVC 70 °C, 3 loaded conductors (B.52.4, B.52.10 for E)
constexpr Grid kPvc3{{
    {13.5, 13, 15.5, 15, 17.5, 18, 18.5},
    {18, 17.5, 21, 20, 24, 24, 25},
    {24, 23, 28, 27, 32, 30, 34},
    {31, 29, 36, 34, 41, 38, 43},
    {42, 39, 50, 46, 57, 50, 60},
    {56, 52, 68, 62, 76, 64, 80},
    {73, 68, 89, 80, 96, 82, 101},
    {89, 83, 110, 99, 119, 98, 126},
    {108, 99, 134, 118, 144, 116, 153},
    {136, 125, 171, 149, 184, 143, 196},
    {164, 150, 207, 179, 223, 169, 238},
    {188, 172, 239, 206, 259, 192, 276},
    {216, 196, 262, 225, 299, 217, 319},
    {245, 223, 296, 255, 341, 243, 364},
    {286, 261, 346, 297, 403, 280, 430},
    {328, 298, 394, 339, 464, 316, 497},
}};

// XLPE/EPR 90 °C, 2 loaded conductors (B.52.3, B.52.11 for E)
constexpr Grid kXlpe2{{
    {19, 18.5, 23, 22, 24, 25, 26},
    {26, 25, 31, 30, 33, 33, 36},
    {35, 33, 42, 40, 45, 43, 49},
    {45, 42, 54, 51, 58, 53, 63},
    {61, 57, 75, 69, 80, 71, 86},
    {81, 76, 100, 91, 107, 91, 115},
    {106, 99, 133, 119, 138, 116, 149},
    {131, 121, 164, 146, 171, 139, 185},
    {158, 145, 198, 175, 209, 164, 225},
    {200, 183, 253, 221, 269, 203, 289},
    {241, 220, 306, 265, 328, 239, 352},
    {278, 253, 354, 305, 382, 271, 410},
    {318, 290, 393, 334, 441, 306, 473},
    {362, 329, 449, 384, 506, 343, 542},
    {424, 386, 528, 459, 599, 395, 641},
    {486, 442, 603, 532, 693, 446, 741},
}};

// XLPE/EPR 90 °C, 3 loaded conductors (B.52.5, B.52.11 for E)
constexpr Grid kXlpe3{{
    {17, 16.5, 20, 19.5, 22, 21, 23},
    {23, 22, 28, 26, 30, 28, 32},
    {31, 30, 37, 35, 40, 36, 42},
    {40, 38, 48, 44, 52, 44, 54},
    {54, 51, 66, 60, 71, 58, 75},
    {73, 68, 88, 80, 96, 75, 100},
    {95, 89, 117, 105, 119, 96, 127},
    {117, 109, 144, 128, 147, 115, 158},
    {141, 130, 175, 154, 179, 135, 192},
    {179, 164, 222, 194, 229, 167, 246},
    {216, 197, 269, 233, 278, 197, 298},
    {249, 227, 312, 268, 322, 223, 346},
    {285, 259, 342, 300, 371, 251, 399},
    {324, 295, 384, 340, 424, 281, 456},
    {380, 346, 450, 398, 500, 324, 538},
    {435, 396, 514, 455, 576, 365, 621},
}};

const Grid& gridFor(Insulation ins, int loaded) {
    if (ins == Insulation::PVC) return loaded == 2 ? kPvc2 : kPvc3;
    return loaded == 2 ? kXlpe2 : kXlpe3;
}

// Table B.52.17 – reduction factors for groups of circuits
constexpr std::array<int, 12> kBunchedCounts{1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 16, 20};
constexpr std::array<double, 12> kBunched{1.00, 0.80, 0.70, 0.65, 0.60, 0.57, 0.54, 0.52, 0.50, 0.45, 0.41, 0.38};
constexpr std::array<double, 9> kWall{1.00, 0.85, 0.79, 0.75, 0.73, 0.72, 0.72, 0.71, 0.70};
constexpr std::array<double, 9> kCeiling{0.95, 0.81, 0.72, 0.68, 0.66, 0.64, 0.63, 0.62, 0.61};
constexpr std::array<double, 9> kTray{1.00, 0.88, 0.82, 0.77, 0.75, 0.73, 0.73, 0.72, 0.72};
constexpr std::array<double, 9> kLadder{1.00, 0.87, 0.82, 0.80, 0.80, 0.79, 0.79, 0.78, 0.78};

constexpr std::array<double, 15> kMcbRatings{2, 4, 6, 10, 13, 16, 20, 25, 32, 40, 50, 63, 80, 100, 125};
constexpr std::array<double, 23> kGgRatings{2,  4,  6,  10,  13,  16,  20,  25,  32,  35,  40, 50,
                                            63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630};

}  // namespace

std::optional<double> baseAmpacity(Insulation ins, Method method, int loaded, double area) {
    if (loaded != 2 && loaded != 3) return std::nullopt;
    for (std::size_t i = 0; i < kAmpacityAreas.size(); ++i) {
        if (std::fabs(kAmpacityAreas[i] - area) < 1e-9) {
            return gridFor(ins, loaded)[i][static_cast<std::size_t>(method)];
        }
    }
    return std::nullopt;
}

double ambientFactor(Insulation ins, Method method, double ambientC) {
    const double tMax = maxConductorTemp(ins);
    const double tRef = method == Method::D1 ? 20.0 : 30.0;
    if (ambientC >= tMax) return 0.0;
    return std::sqrt((tMax - ambientC) / (tMax - tRef));
}

double groupingFactor(Arrangement arr, int circuits) {
    if (circuits <= 1) return arr == Arrangement::SingleLayerCeiling ? 0.95 : 1.0;
    if (arr == Arrangement::Bunched) {
        for (std::size_t i = 0; i < kBunchedCounts.size(); ++i) {
            if (circuits <= kBunchedCounts[i]) return kBunched[i];
        }
        return kBunched.back();
    }
    const auto idx = static_cast<std::size_t>(std::min(circuits, 9) - 1);  // no further reduction above 9
    switch (arr) {
        case Arrangement::SingleLayerWall: return kWall[idx];
        case Arrangement::SingleLayerCeiling: return kCeiling[idx];
        case Arrangement::PerforatedTray: return kTray[idx];
        case Arrangement::Ladder: return kLadder[idx];
        case Arrangement::Bunched: break;
    }
    return 1.0;
}

std::span<const double> ratedCurrents(Device d) {
    if (d == Device::FuseGG) return kGgRatings;
    return kMcbRatings;
}

double conventionalTripFactor(Device d, double In) {
    if (d != Device::FuseGG) return 1.45;
    if (In <= 4.0) return 2.1;
    if (In < 16.0) return 1.9;
    return 1.6;
}

double instantaneousMultiple(Device d) {
    switch (d) {
        case Device::MCB_B: return 5.0;
        case Device::MCB_C: return 10.0;
        case Device::MCB_D: return 20.0;
        case Device::FuseGG: break;
    }
    return kNaN;
}

double maxLengthDisconnection(double U0, double Sph, double Spe, Material mat, double Ia) {
    const double m = Sph / Spe;
    return 0.8 * U0 * Sph / (faultResistivity(mat) * (1.0 + m) * Ia);
}

double kFactor(Material m, Insulation ins) {
    if (m == Material::Cu) return ins == Insulation::PVC ? 115.0 : 143.0;
    return ins == Insulation::PVC ? 76.0 : 94.0;
}

}  // namespace elc::cables
