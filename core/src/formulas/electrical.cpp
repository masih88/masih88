#include "elc/formulas/electrical.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <complex>
#include <limits>
#include <numbers>

#include "elc/formulas/electronics.hpp"

namespace elc::electrical {

namespace {

constexpr double kInfinity = std::numeric_limits<double>::infinity();

double sinOf(double cosPhi) { return std::sqrt(std::max(0.0, 1.0 - cosPhi * cosPhi)); }

std::string upper(std::string_view s) {
    std::string out;
    for (const char ch : s) {
        if (ch == ' ') continue;
        out += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return out;
}

bool allDigits(std::string_view s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), [](char ch) { return ch >= '0' && ch <= '9'; });
}

/// "4.7", ".47", "47." → number (digits and at most one '.'), nullopt otherwise
std::optional<double> plainNumber(std::string_view s) {
    if (s.empty()) return std::nullopt;
    double v = 0.0;
    double scale = 0.0;  // 0 = before the decimal point
    int dots = 0;
    int digits = 0;
    for (const char ch : s) {
        if (ch == '.') {
            if (++dots > 1) return std::nullopt;
            scale = 0.1;
            continue;
        }
        if (ch < '0' || ch > '9') return std::nullopt;
        ++digits;
        const int d = ch - '0';
        if (scale == 0.0) {
            v = v * 10.0 + d;
        } else {
            v += d * scale;
            scale *= 0.1;
        }
    }
    if (digits == 0) return std::nullopt;
    return v;
}

/// Replaces the single decimal marker `marker` (e.g. 'R') by '.' and parses; nullopt if absent or repeated
std::optional<double> markerNumber(const std::string& s, char marker) {
    if (std::count(s.begin(), s.end(), marker) != 1) return std::nullopt;
    std::string t = s;
    std::replace(t.begin(), t.end(), marker, '.');
    if (t.find('.') != t.rfind('.')) return std::nullopt;
    return plainNumber(t);
}

double eiaMultiplier(char letter) {
    switch (letter) {
        case 'Z': return 0.001;
        case 'Y':
        case 'R': return 0.01;
        case 'X':
        case 'S': return 0.1;
        case 'A': return 1.0;
        case 'B':
        case 'H': return 10.0;
        case 'C': return 100.0;
        case 'D': return 1e3;
        case 'E': return 1e4;
        case 'F': return 1e5;
        default: return 0.0;
    }
}

constexpr std::array<VoltageCode, 22> kVoltageCodes{{
    {"0G", 4},   {"0L", 5.5}, {"0J", 6.3}, {"1A", 10},  {"1C", 16},  {"1D", 20},  {"1E", 25},  {"1V", 35},
    {"1H", 50},  {"1J", 63},  {"1K", 80},  {"2A", 100}, {"2B", 125}, {"2C", 160}, {"2D", 200}, {"2E", 250},
    {"2F", 315}, {"2V", 350}, {"2G", 400}, {"2W", 450}, {"2J", 630}, {"3A", 1000},
}};

constexpr std::array<Gauge, 57> kSwg{{
    {"7/0", 0.500},  {"6/0", 0.464},  {"5/0", 0.432},  {"4/0", 0.400},  {"3/0", 0.372},  {"2/0", 0.348},
    {"1/0", 0.324},  {"1", 0.300},    {"2", 0.276},    {"3", 0.252},    {"4", 0.232},    {"5", 0.212},
    {"6", 0.192},    {"7", 0.176},    {"8", 0.160},    {"9", 0.144},    {"10", 0.128},   {"11", 0.116},
    {"12", 0.104},   {"13", 0.092},   {"14", 0.080},   {"15", 0.072},   {"16", 0.064},   {"17", 0.056},
    {"18", 0.048},   {"19", 0.040},   {"20", 0.036},   {"21", 0.032},   {"22", 0.028},   {"23", 0.024},
    {"24", 0.022},   {"25", 0.020},   {"26", 0.018},   {"27", 0.0164},  {"28", 0.0148},  {"29", 0.0136},
    {"30", 0.0124},  {"31", 0.0116},  {"32", 0.0108},  {"33", 0.0100},  {"34", 0.0092},  {"35", 0.0084},
    {"36", 0.0076},  {"37", 0.0068},  {"38", 0.0060},  {"39", 0.0052},  {"40", 0.0048},  {"41", 0.0044},
    {"42", 0.0040},  {"43", 0.0036},  {"44", 0.0032},  {"45", 0.0028},  {"46", 0.0024},  {"47", 0.0020},
    {"48", 0.0016},  {"49", 0.0012},  {"50", 0.0010},
}};

// ---- NEC Table 430.250 (three-phase, induction motors) and 430.248 (single-phase) -------------
constexpr double kNo = std::numeric_limits<double>::quiet_NaN();
constexpr std::array<double, 27> kHp3{0.5, 0.75, 1, 1.5, 2, 3, 5, 7.5, 10, 15, 20, 25, 30, 40,
                                      50, 60, 75, 100, 125, 150, 200, 250, 300, 350, 400, 450, 500};
constexpr std::array<int, 7> kV3{115, 200, 208, 230, 460, 575, 2300};
constexpr std::array<std::array<double, 7>, 27> kFlc3{{
    {4.4, 2.5, 2.4, 2.2, 1.1, 0.9, kNo},       {6.4, 3.7, 3.5, 3.2, 1.6, 1.3, kNo},
    {8.4, 4.8, 4.6, 4.2, 2.1, 1.7, kNo},       {12.0, 6.9, 6.6, 6.0, 3.0, 2.4, kNo},
    {13.6, 7.8, 7.5, 6.8, 3.4, 2.7, kNo},      {kNo, 11.0, 10.6, 9.6, 4.8, 3.9, kNo},
    {kNo, 17.5, 16.7, 15.2, 7.6, 6.1, kNo},    {kNo, 25.3, 24.2, 22, 11, 9, kNo},
    {kNo, 32.2, 30.8, 28, 14, 11, kNo},        {kNo, 48.3, 46.2, 42, 21, 17, kNo},
    {kNo, 62.1, 59.4, 54, 27, 22, kNo},        {kNo, 78.2, 74.8, 68, 34, 27, kNo},
    {kNo, 92, 88, 80, 40, 32, kNo},            {kNo, 120, 114, 104, 52, 41, kNo},
    {kNo, 150, 143, 130, 65, 52, kNo},         {kNo, 177, 169, 154, 77, 62, 16},
    {kNo, 221, 211, 192, 96, 77, 20},          {kNo, 285, 273, 248, 124, 99, 26},
    {kNo, 359, 343, 312, 156, 125, 31},        {kNo, 414, 396, 360, 180, 144, 37},
    {kNo, 552, 528, 480, 240, 192, 49},        {kNo, kNo, kNo, kNo, 302, 242, 60},
    {kNo, kNo, kNo, kNo, 361, 289, 72},        {kNo, kNo, kNo, kNo, 414, 336, 83},
    {kNo, kNo, kNo, kNo, 477, 382, 95},        {kNo, kNo, kNo, kNo, 515, 412, 103},
    {kNo, kNo, kNo, kNo, 590, 472, 118},
}};
constexpr std::array<double, 12> kHp1{1.0 / 6.0, 0.25, 1.0 / 3.0, 0.5, 0.75, 1, 1.5, 2, 3, 5, 7.5, 10};
constexpr std::array<int, 4> kV1{115, 200, 208, 230};
constexpr std::array<std::array<double, 4>, 12> kFlc1{{
    {4.4, 2.5, 2.4, 2.2},   {5.8, 3.3, 3.2, 2.9},   {7.2, 4.1, 4.0, 3.6},   {9.8, 5.6, 5.4, 4.9},
    {13.8, 7.9, 7.6, 6.9},  {16, 9.2, 8.8, 8.0},    {20, 11.5, 11.0, 10},   {24, 13.8, 13.2, 12},
    {34, 19.6, 18.7, 17},   {56, 32.2, 30.8, 28},   {80, 46.0, 44.0, 40},   {100, 57.5, 55.0, 50},
}};

// ---- busbars (Striebel & John, Kap. 10 Sammelschienensysteme; DIN 43671 conditions) ----------
constexpr std::array<Busbar, 12> kBusbars{{
    {12, 5, 59, 0.525, 250, 0},      {12, 10, 119, 1.059, 360, 0},   {20, 5, 99, 0.881, 320, 0},
    {20, 10, 199, 1.771, 500, 850},  {25, 5, 124, 1.104, 390, 0},    {30, 5, 149, 1.331, 440, 0},
    {30, 10, 299, 2.661, 630, 1250}, {40, 10, 399, 3.551, 850, 1500}, {50, 10, 499, 4.441, 1000, 0},
    {60, 10, 599, 5.331, 1250, 0},   {80, 10, 799, 7.111, 1450, 0},  {100, 10, 999, 8.891, 1700, 0},
}};

// ---- mineral-insulated cables, 70 °C sheath, method C ---------------------------------------
struct MiRow {
    MiDuty duty;
    MiConfig cfg;
    double area;
    double amps;
};
constexpr std::array<MiRow, 61> kMi{{
    {MiDuty::Light500V, MiConfig::TwoCore, 1.0, 18.5},       {MiDuty::Light500V, MiConfig::TwoCore, 1.5, 23},
    {MiDuty::Light500V, MiConfig::TwoCore, 2.5, 31},         {MiDuty::Light500V, MiConfig::TwoCore, 4.0, 40},
    {MiDuty::Light500V, MiConfig::ThreeCore3ph, 1.0, 15},    {MiDuty::Light500V, MiConfig::ThreeCore3ph, 1.5, 19},
    {MiDuty::Light500V, MiConfig::ThreeCore3ph, 2.5, 26},    {MiDuty::Light500V, MiConfig::FourCore3ph, 1.0, 15},
    {MiDuty::Light500V, MiConfig::FourCore3ph, 1.5, 19},     {MiDuty::Light500V, MiConfig::FourCore3ph, 2.5, 26},
    {MiDuty::Light500V, MiConfig::SevenCore, 1.5, 13},       {MiDuty::Light500V, MiConfig::SevenCore, 2.5, 17.5},
    {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 6, 57},     {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 10, 77},
    {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 16, 102},   {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 25, 133},
    {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 35, 163},   {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 50, 202},
    {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 70, 247},   {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 95, 296},
    {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 120, 340},  {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 150, 388},
    {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 185, 440},  {MiDuty::Heavy750V, MiConfig::SingleCore1ph, 240, 514},
    {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 6, 52},     {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 10, 70},
    {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 16, 92},    {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 25, 120},
    {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 35, 147},   {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 50, 181},
    {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 70, 221},   {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 95, 264},
    {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 120, 303},  {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 150, 346},
    {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 185, 392},  {MiDuty::Heavy750V, MiConfig::SingleCore3ph, 240, 457},
    {MiDuty::Heavy750V, MiConfig::TwoCore, 1.5, 25},         {MiDuty::Heavy750V, MiConfig::TwoCore, 2.5, 34},
    {MiDuty::Heavy750V, MiConfig::TwoCore, 4, 45},           {MiDuty::Heavy750V, MiConfig::TwoCore, 6, 57},
    {MiDuty::Heavy750V, MiConfig::TwoCore, 10, 77},          {MiDuty::Heavy750V, MiConfig::TwoCore, 16, 102},
    {MiDuty::Heavy750V, MiConfig::TwoCore, 25, 133},         {MiDuty::Heavy750V, MiConfig::ThreeCore3ph, 1.5, 21},
    {MiDuty::Heavy750V, MiConfig::ThreeCore3ph, 2.5, 28},    {MiDuty::Heavy750V, MiConfig::ThreeCore3ph, 4, 37},
    {MiDuty::Heavy750V, MiConfig::ThreeCore3ph, 6, 48},      {MiDuty::Heavy750V, MiConfig::ThreeCore3ph, 10, 65},
    {MiDuty::Heavy750V, MiConfig::ThreeCore3ph, 16, 86},     {MiDuty::Heavy750V, MiConfig::ThreeCore3ph, 25, 112},
    {MiDuty::Heavy750V, MiConfig::FourCore3ph, 1.5, 21},     {MiDuty::Heavy750V, MiConfig::FourCore3ph, 2.5, 28},
    {MiDuty::Heavy750V, MiConfig::FourCore3ph, 4, 37},       {MiDuty::Heavy750V, MiConfig::FourCore3ph, 6, 47},
    {MiDuty::Heavy750V, MiConfig::FourCore3ph, 10, 64},      {MiDuty::Heavy750V, MiConfig::FourCore3ph, 16, 85},
    {MiDuty::Heavy750V, MiConfig::FourCore3ph, 25, 110},     {MiDuty::Heavy750V, MiConfig::SevenCore, 1.5, 14.5},
    {MiDuty::Heavy750V, MiConfig::SevenCore, 2.5, 19.5},     {MiDuty::Heavy750V, MiConfig::TwelveCore, 2.5, 16},
    {MiDuty::Heavy750V, MiConfig::NineteenCore, 1.5, 10},
}};

}  // namespace

// ---- power, voltage, impedance --------------------------------------------------------------

double voltageFromPower(System sys, double P, double I, double cosPhi) {
    const double cp = sys == System::DC ? 1.0 : cosPhi;
    return P / (powerFactorK(sys) * I * cp);
}

double powerFactor(System sys, double P, double U, double I) { return P / (powerFactorK(sys) * U * I); }

Impedance impedanceFromUI(System sys, double U, double I, double cosPhi) {
    const double Z = U / (powerFactorK(sys) * I);
    return {Z, Z * cosPhi, Z * sinOf(cosPhi)};
}

double operatingCurrent(System sys, double U, double P, double ku, double ks, double cosPhi, double eta) {
    const double cp = sys == System::DC ? 1.0 : cosPhi;
    return P * ku * ks / (powerFactorK(sys) * U * cp * eta);
}

// ---- cables ---------------------------------------------------------------------------------

double cableTemperature(double Ib, double Iz, double ambient, double maxTemp) {
    const double x = Ib / Iz;
    return ambient + (maxTemp - ambient) * x * x;
}

double neutralCurrentFundamental(double I1, double I2, double I3) {
    const double q = I1 * I1 + I2 * I2 + I3 * I3 - I1 * I2 - I2 * I3 - I3 * I1;
    return std::sqrt(std::max(0.0, q));
}

double neutralCurrent(double I1, double I2, double I3, double h3Percent) {
    const double n1 = neutralCurrentFundamental(I1, I2, I3);
    const double n3 = h3Percent / 100.0 * (I1 + I2 + I3);
    return std::hypot(n1, n3);
}

HarmonicSizing harmonicSizing(double h3Percent) {
    if (h3Percent <= 15.0) return {false, 1.0};
    if (h3Percent <= 33.0) return {false, 0.86};
    if (h3Percent <= 45.0) return {true, 0.86};
    return {true, 1.0};
}

double distributedMoment(int n, double I, double L1, double d) {
    const double nn = n;
    return I * (nn * L1 + d * nn * (nn - 1.0) / 2.0);
}

double distributedVoltageDrop(System sys, double moment, double A, double rho, double cosPhi, double xPerM) {
    const double cp = sys == System::DC ? 1.0 : cosPhi;
    const double x = sys == System::DC ? 0.0 : xPerM;
    return dropFactorK(sys) * moment * (rho / A * cp + x * sinOf(cp));
}

double distributedRequiredArea(System sys, double moment, double rho, double cosPhi, double xPerM, double dUmax) {
    const double cp = sys == System::DC ? 1.0 : cosPhi;
    const double x = sys == System::DC ? 0.0 : xPerM;
    const double denom = dUmax / (dropFactorK(sys) * moment) - x * sinOf(cp);
    if (denom <= 0.0) return kInfinity;
    return rho * cp / denom;
}

double capacitanceFor(double Q, double U, double f) { return Q / (2.0 * std::numbers::pi * f * U * U); }

// ---- earthing, short circuit ----------------------------------------------------------------

MinShortCircuit minShortCircuit(double c, double U0, double ZupLoop, double xrUp, double L, double Sph, double Spe,
                                double Sn, cables::Material mat, double theta, double xPerM) {
    const double rho = cables::resistivity(mat, theta);
    MinShortCircuit r{};
    r.rLine = rho * L / Sph;
    r.rPe = rho * L / Spe;
    r.rN = rho * L / Sn;
    const double rUp = ZupLoop / std::sqrt(1.0 + xrUp * xrUp);
    const double xUp = rUp * xrUp;
    const double x2 = 2.0 * xPerM * L;
    r.ik1 = c * U0 / std::abs(std::complex<double>(rUp + r.rLine + r.rPe, xUp + x2));
    r.ik1N = c * U0 / std::abs(std::complex<double>(rUp + r.rLine + r.rN, xUp + x2));
    r.ik2 = c * kSqrt3 * U0 / std::abs(std::complex<double>(rUp + 2.0 * r.rLine, xUp + x2));
    return r;
}

// ---- transformers, antennas, CCTV -----------------------------------------------------------

double turnsPerVolt(double f, double B, double areaM2) { return 1.0 / (4.44 * f * B * areaM2); }

double wavelength(double f) { return 299792458.0 / f; }

double cctvBitsPerPixel(std::string_view codec) {
    if (codec == "h265") return 0.04;
    if (codec == "mjpeg") return 0.75;
    return 0.08;
}

// ---- electronics ----------------------------------------------------------------------------

ZenerDesign zenerDesign(double UeMin, double UeMax, double Uz, double IzMin, double ILmin, double ILmax) {
    ZenerDesign z{};
    z.Rv = (UeMin - Uz) / (IzMin + ILmax);
    z.IzMax = (UeMax - Uz) / z.Rv - ILmin;
    z.PzMax = Uz * z.IzMax;
    z.PR = (UeMax - Uz) * (UeMax - Uz) / z.Rv;
    return z;
}

std::array<double, 3> deltaToStar(double R12, double R23, double R31) {
    const double s = R12 + R23 + R31;
    return {R12 * R31 / s, R12 * R23 / s, R23 * R31 / s};
}

std::array<double, 3> starToDelta(double R1, double R2, double R3) {
    const double p = R1 * R2 + R2 * R3 + R3 * R1;
    return {p / R3, p / R1, p / R2};
}

std::span<const double> eia96Values() { return electronics::eSeriesBase(electronics::ESeries::E96); }

std::optional<SmdCode> smdResistor(std::string_view code) {
    const std::string s = upper(code);
    if (s.empty() || s.size() > 4) return std::nullopt;
    if (std::all_of(s.begin(), s.end(), [](char ch) { return ch == '0'; })) return SmdCode{0.0, "0"};
    if (s.find('R') != std::string::npos && !(s.size() == 3 && allDigits(s.substr(0, 2)))) {
        if (const auto v = markerNumber(s, 'R')) return SmdCode{*v, "R"};
        return std::nullopt;
    }
    if (s.size() == 3 && allDigits(s.substr(0, 2)) && !allDigits(s.substr(2))) {
        const int idx = (s[0] - '0') * 10 + (s[1] - '0');
        const double mult = eiaMultiplier(s[2]);
        if (idx < 1 || idx > 96 || mult == 0.0) return std::nullopt;
        return SmdCode{eia96Values()[static_cast<std::size_t>(idx - 1)] * mult, "EIA-96"};
    }
    if (!allDigits(s) || s.size() < 3) return std::nullopt;
    const double mantissa = *plainNumber(s.substr(0, s.size() - 1));
    const int exponent = s.back() - '0';
    return SmdCode{mantissa * std::pow(10.0, exponent), s.size() == 3 ? "3" : "4"};
}

std::span<const VoltageCode> capacitorVoltageCodes() { return kVoltageCodes; }

std::optional<CapacitorCode> capacitorCode(std::string_view code) {
    std::string s;
    for (std::size_t i = 0; i < code.size(); ++i) {
        const unsigned char ch = static_cast<unsigned char>(code[i]);
        if (ch == ' ') continue;
        if (ch == 0xC2 && i + 1 < code.size() && static_cast<unsigned char>(code[i + 1]) == 0xB5) {  // "µ"
            s += 'u';
            ++i;
            continue;
        }
        if (ch == ',') {
            s += '.';
            continue;
        }
        s += static_cast<char>(ch);
    }
    if (s.empty()) return std::nullopt;
    CapacitorCode r{};
    // voltage prefix "2A", "1H", …
    if (s.size() > 2) {
        const std::string prefix = upper(s.substr(0, 2));
        for (const auto& vc : kVoltageCodes) {
            if (prefix == vc.code) {
                r.voltage = vc.volts;
                s = s.substr(2);
                break;
            }
        }
    }
    // tolerance letter at the end
    if (s.size() > 1 && std::isdigit(static_cast<unsigned char>(s[s.size() - 2]))) {
        const char t = static_cast<char>(std::toupper(static_cast<unsigned char>(s.back())));
        bool used = true;
        switch (t) {
            case 'B': r.tolerancePF = 0.1; r.toleranceText = "±0,1 pF"; break;
            case 'C': r.tolerancePF = 0.25; r.toleranceText = "±0,25 pF"; break;
            case 'D': r.tolerancePF = 0.5; r.toleranceText = "±0,5 pF"; break;
            case 'F': r.tolerancePercent = 1; r.toleranceText = "±1 %"; break;
            case 'G': r.tolerancePercent = 2; r.toleranceText = "±2 %"; break;
            case 'J': r.tolerancePercent = 5; r.toleranceText = "±5 %"; break;
            case 'K': r.tolerancePercent = 10; r.toleranceText = "±10 %"; break;
            case 'M': r.tolerancePercent = 20; r.toleranceText = "±20 %"; break;
            case 'Z': r.toleranceText = "+80 / −20 %"; break;
            default: used = false;
        }
        if (used) s.pop_back();
    }
    if (s.empty()) return std::nullopt;
    const std::string su = upper(s);
    for (const auto& [letter, unit] : {std::pair{'P', 1.0}, std::pair{'N', 1e3}, std::pair{'U', 1e6}}) {
        if (su.find(letter) != std::string::npos) {
            const auto v = markerNumber(su, letter);
            if (!v) return std::nullopt;
            r.pF = *v * unit;
            return r;
        }
    }
    if (su.find('R') != std::string::npos) {
        const auto v = markerNumber(su, 'R');
        if (!v) return std::nullopt;
        r.pF = *v;
        return r;
    }
    if (su.find('.') != std::string::npos) {  // decimal number = µF (film and electrolytic capacitors)
        const auto v = plainNumber(su);
        if (!v) return std::nullopt;
        r.pF = *v * 1e6;
        return r;
    }
    if (!allDigits(su)) return std::nullopt;
    if (su.size() == 3) {
        const double mantissa = (su[0] - '0') * 10.0 + (su[1] - '0');
        const int m = su[2] - '0';
        const double factor = m == 8 ? 0.01 : m == 9 ? 0.1 : std::pow(10.0, m);
        r.pF = mantissa * factor;
        return r;
    }
    r.pF = *plainNumber(su);
    return r;
}

std::optional<double> inductorCode(std::string_view code) {
    std::string s = upper(code);
    if (s.size() > 1 && std::isdigit(static_cast<unsigned char>(s[s.size() - 2])) &&
        (s.back() == 'J' || s.back() == 'K' || s.back() == 'M')) {
        s.pop_back();  // tolerance letter
    }
    if (s.empty()) return std::nullopt;
    if (s.find('N') != std::string::npos) {
        const auto v = markerNumber(s, 'N');
        if (!v) return std::nullopt;
        return *v / 1000.0;  // nH → µH
    }
    if (s.find('R') != std::string::npos) return markerNumber(s, 'R');
    if (!allDigits(s)) return std::nullopt;
    if (s.size() == 3) return ((s[0] - '0') * 10.0 + (s[1] - '0')) * std::pow(10.0, s[2] - '0');
    return plainNumber(s);
}

std::optional<BandCode> bandsFor(double value, int significant) {
    if (!(value > 0.0) || !std::isfinite(value) || significant < 2 || significant > 3) return std::nullopt;
    int e = static_cast<int>(std::floor(std::log10(value) + 1e-12));
    int exp = e - significant + 1;
    double mant = std::round(value / std::pow(10.0, exp));
    if (mant >= std::pow(10.0, significant)) {  // 9.96 → 10.0
        ++exp;
        mant = std::round(mant / 10.0);
    }
    if (exp < -2 || exp > 9) return std::nullopt;
    BandCode b;
    b.exponent = exp;
    b.value = mant * std::pow(10.0, exp);
    long long m = std::llround(mant);
    b.digits.assign(static_cast<std::size_t>(significant), 0);
    for (int i = significant - 1; i >= 0; --i) {
        b.digits[static_cast<std::size_t>(i)] = static_cast<int>(m % 10);
        m /= 10;
    }
    return b;
}

std::span<const Gauge> swgTable() { return kSwg; }

// ---- NEC ------------------------------------------------------------------------------------

std::span<const double> necHorsepowers(int phases) {
    if (phases == 1) return kHp1;
    return kHp3;
}

std::span<const int> necVoltages(int phases) {
    if (phases == 1) return kV1;
    return kV3;
}

std::optional<double> necFullLoadCurrent(int phases, double hp, int volts) {
    const auto hps = necHorsepowers(phases);
    const auto vs = necVoltages(phases);
    for (std::size_t i = 0; i < hps.size(); ++i) {
        if (std::fabs(hps[i] - hp) > 1e-4 * hp) continue;
        for (std::size_t j = 0; j < vs.size(); ++j) {
            if (vs[j] != volts) continue;
            const double v = phases == 1 ? kFlc1[i][j] : kFlc3[i][j];
            if (std::isnan(v)) return std::nullopt;
            return v;
        }
    }
    return std::nullopt;
}

// ---- busbars and MI cables ------------------------------------------------------------------

std::span<const Busbar> busbars() { return kBusbars; }

std::optional<double> miAmpacity(MiDuty duty, MiConfig cfg, double area) {
    for (const auto& row : kMi) {
        if (row.duty == duty && row.cfg == cfg && std::fabs(row.area - area) < 1e-9) return row.amps;
    }
    return std::nullopt;
}

std::vector<double> miAreas(MiDuty duty, MiConfig cfg) {
    std::vector<double> out;
    for (const auto& row : kMi) {
        if (row.duty == duty && row.cfg == cfg) out.push_back(row.area);
    }
    return out;
}

}  // namespace elc::electrical
