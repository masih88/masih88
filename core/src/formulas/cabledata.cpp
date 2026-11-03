#include "elc/formulas/cabledata.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>

#include "elc/types.hpp"

namespace elc::cabledata {

namespace {

// clang-format off
const std::vector<CableSpec> kData{
    // NYM-J / NYM-O – Faber Kabel data sheet NYM-J/NYM-O (DA approx. values); 2-core = NYM-O
    {Family::NYM, 1, 2.5, 5.8, 70},   {Family::NYM, 1, 4, 6.4, 80},     {Family::NYM, 1, 6, 6.8, 105},   {Family::NYM, 1, 10, 8.0, 155},
    {Family::NYM, 1, 16, 9.1, 230},   {Family::NYM, 1, 25, 12.3, 330},
    {Family::NYM, 2, 1.5, 7.8, 115},  {Family::NYM, 2, 2.5, 8.9, 157},
    {Family::NYM, 3, 1.5, 8.2, 135},  {Family::NYM, 3, 2.5, 9.4, 190},  {Family::NYM, 3, 4, 10.8, 265},  {Family::NYM, 3, 6, 12.2, 315},
    {Family::NYM, 3, 10, 14.7, 465},
    {Family::NYM, 4, 1.5, 8.8, 160},  {Family::NYM, 4, 2.5, 10.2, 230}, {Family::NYM, 4, 4, 12.1, 330},  {Family::NYM, 4, 6, 13.3, 460},
    {Family::NYM, 4, 10, 16.1, 690},  {Family::NYM, 4, 16, 19.0, 1090}, {Family::NYM, 4, 25, 23.4, 1640}, {Family::NYM, 4, 35, 25.7, 2090},
    {Family::NYM, 5, 1.5, 9.5, 190},  {Family::NYM, 5, 2.5, 11.0, 270}, {Family::NYM, 5, 4, 13.2, 410},  {Family::NYM, 5, 6, 14.5, 540},
    {Family::NYM, 5, 10, 17.7, 850},  {Family::NYM, 5, 16, 21.2, 1350}, {Family::NYM, 5, 25, 25.7, 1990},
    {Family::NYM, 7, 1.5, 10.5, 235}, {Family::NYM, 7, 2.5, 12.6, 350}, {Family::NYM, 8, 1.5, 12.5, 237}, {Family::NYM, 10, 1.5, 14.3, 330},
    {Family::NYM, 12, 1.5, 14.4, 400}, {Family::NYM, 12, 2.5, 15.4, 660}, {Family::NYM, 16, 1.5, 15.8, 457},
    // NYY-J – Waskönig+Walter data sheet NYY-J (07.05.2019, diameters rounded to mm)
    {Family::NYY, 1, 10, 10, 168},    {Family::NYY, 1, 16, 11, 230},    {Family::NYY, 1, 25, 13, 349},    {Family::NYY, 1, 35, 14, 448},
    {Family::NYY, 1, 50, 15, 584},    {Family::NYY, 1, 70, 17, 796},    {Family::NYY, 1, 95, 19, 1067},   {Family::NYY, 1, 120, 20, 1296},
    {Family::NYY, 1, 150, 22, 1592},  {Family::NYY, 1, 185, 24, 1954},  {Family::NYY, 1, 240, 27, 2525},  {Family::NYY, 1, 300, 30, 3150},
    {Family::NYY, 3, 1.5, 11, 178},   {Family::NYY, 3, 2.5, 12, 223},   {Family::NYY, 3, 4, 14, 313},     {Family::NYY, 3, 6, 15, 390},
    {Family::NYY, 3, 10, 17, 539},    {Family::NYY, 3, 16, 19, 749},
    {Family::NYY, 4, 1.5, 12, 208},   {Family::NYY, 4, 2.5, 13, 261},   {Family::NYY, 4, 4, 15, 371},     {Family::NYY, 4, 6, 16, 469},
    {Family::NYY, 4, 10, 19, 656},    {Family::NYY, 4, 16, 20, 929},    {Family::NYY, 4, 25, 25, 1452},   {Family::NYY, 4, 35, 25, 1697},
    {Family::NYY, 4, 50, 30, 2357},   {Family::NYY, 4, 70, 33, 3142},   {Family::NYY, 4, 95, 37, 4301},   {Family::NYY, 4, 120, 43, 5439},
    {Family::NYY, 4, 150, 44, 6483},  {Family::NYY, 4, 185, 51, 8244},  {Family::NYY, 4, 240, 56, 10501},
    {Family::NYY, 5, 1.5, 13, 238},   {Family::NYY, 5, 2.5, 14, 303},   {Family::NYY, 5, 4, 16, 433},     {Family::NYY, 5, 6, 18, 551},
    {Family::NYY, 5, 10, 20, 779},    {Family::NYY, 5, 16, 22, 1122},   {Family::NYY, 5, 25, 28, 1758},   {Family::NYY, 5, 35, 31, 2314},
    {Family::NYY, 5, 50, 35, 3129},   {Family::NYY, 5, 70, 40, 4314},   {Family::NYY, 5, 95, 47, 5905},   {Family::NYY, 5, 120, 50, 7166},
    {Family::NYY, 7, 1.5, 14, 289},   {Family::NYY, 7, 2.5, 15, 374},   {Family::NYY, 10, 1.5, 16, 394},  {Family::NYY, 12, 1.5, 17, 455},
    {Family::NYY, 12, 2.5, 19, 599},  {Family::NYY, 19, 1.5, 20, 627},  {Family::NYY, 24, 1.5, 22, 792},
    // J-Y(St)Y Lg – Lapp product information J-Y(ST)Y...LG (pairs x conductor diameter)
    {Family::JYSTY, 1, 0.6, 5.0, 30},   {Family::JYSTY, 2, 0.6, 5.5, 40},   {Family::JYSTY, 4, 0.6, 6.7, 60},   {Family::JYSTY, 10, 0.6, 9.0, 110},
    {Family::JYSTY, 1, 0.8, 6.0, 40},   {Family::JYSTY, 2, 0.8, 7.0, 60},   {Family::JYSTY, 3, 0.8, 8.5, 80},   {Family::JYSTY, 4, 0.8, 9.0, 100},
    {Family::JYSTY, 5, 0.8, 9.5, 120},  {Family::JYSTY, 6, 0.8, 10.5, 140}, {Family::JYSTY, 8, 0.8, 11.5, 170}, {Family::JYSTY, 10, 0.8, 13.0, 220},
    {Family::JYSTY, 12, 0.8, 14.0, 250}, {Family::JYSTY, 16, 0.8, 15.5, 320}, {Family::JYSTY, 20, 0.8, 16.5, 380}, {Family::JYSTY, 24, 0.8, 19.0, 460},
    {Family::JYSTY, 30, 0.8, 20.0, 560}, {Family::JYSTY, 40, 0.8, 22.5, 710}, {Family::JYSTY, 50, 0.8, 25.5, 900},
    // UNITRONIC LiYCY – Lapp data sheet (01.09.2020)
    {Family::LiYCY, 2, 0.25, 4.5, 32},  {Family::LiYCY, 3, 0.25, 4.7, 37},  {Family::LiYCY, 4, 0.25, 5.0, 41.3}, {Family::LiYCY, 6, 0.25, 6.0, 58},
    {Family::LiYCY, 8, 0.25, 7.1, 73},  {Family::LiYCY, 10, 0.25, 7.5, 82}, {Family::LiYCY, 12, 0.25, 7.7, 98},
    {Family::LiYCY, 2, 0.5, 5.6, 47},   {Family::LiYCY, 3, 0.5, 5.9, 55},   {Family::LiYCY, 4, 0.5, 6.3, 70},   {Family::LiYCY, 5, 0.5, 7.0, 90},
    {Family::LiYCY, 7, 0.5, 7.6, 112},  {Family::LiYCY, 10, 0.5, 9.3, 139}, {Family::LiYCY, 12, 0.5, 9.6, 177},
    {Family::LiYCY, 2, 0.75, 6.0, 53},  {Family::LiYCY, 3, 0.75, 6.3, 65},  {Family::LiYCY, 4, 0.75, 7.0, 79},  {Family::LiYCY, 5, 0.75, 7.6, 109},
    {Family::LiYCY, 7, 0.75, 8.2, 156}, {Family::LiYCY, 12, 0.75, 10.8, 218},
    {Family::LiYCY, 2, 1.0, 6.3, 72},   {Family::LiYCY, 3, 1.0, 6.8, 90},   {Family::LiYCY, 4, 1.0, 7.3, 109},  {Family::LiYCY, 7, 1.0, 8.6, 171},
    {Family::LiYCY, 2, 1.5, 7.1, 90},   {Family::LiYCY, 3, 1.5, 7.5, 115},  {Family::LiYCY, 4, 1.5, 8.1, 129},  {Family::LiYCY, 5, 1.5, 8.8, 176},
    {Family::LiYCY, 7, 1.5, 9.5, 220},
    // Cat 7 S/FTP 4x2xAWG23, LSZH – DIGITUS DK-1743-VH data sheet (Dca version)
    {Family::Cat7, 4, 0, 7.2, 51},
};
// clang-format on

std::string normalise(std::string_view text) {
    std::string s;
    for (std::size_t i = 0; i < text.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(text[i]);
        if (c == 0xC3 && i + 1 < text.size() && static_cast<unsigned char>(text[i + 1]) == 0x97) {  // "×"
            s += 'x';
            ++i;
        } else if (c == ' ') {
            continue;
        } else if (c == '*') {
            s += 'x';
        } else if (c == ',') {
            s += '.';
        } else {
            s += static_cast<char>(std::tolower(c));
        }
    }
    // "5g2.5" (G = with green-yellow core) → "5x2.5"
    for (std::size_t i = 1; i + 1 < s.size(); ++i) {
        if (s[i] == 'g' && std::isdigit(static_cast<unsigned char>(s[i - 1])) && std::isdigit(static_cast<unsigned char>(s[i + 1]))) s[i] = 'x';
    }
    return s;
}

std::vector<double> numbersAfter(const std::string& s, std::size_t from) {
    std::vector<double> out;
    std::size_t i = from;
    while (i < s.size() && !std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
    while (i < s.size()) {
        std::size_t j = i;
        while (j < s.size() && (std::isdigit(static_cast<unsigned char>(s[j])) || s[j] == '.')) ++j;
        if (j == i) break;
        out.push_back(parseNumber(s.substr(i, j - i)).value_or(-1));
        if (j >= s.size() || s[j] != 'x') break;
        i = j + 1;
    }
    return out;
}

std::string fmt(double v) {
    std::string s = formatNumber(v, 2, Lang::De);
    return s;
}

}  // namespace

std::string CableSpec::name() const {
    switch (family) {
        case Family::NYM: return (cores == 2 ? "NYM-O " : "NYM-J ") + std::to_string(cores) + "x" + fmt(size);
        case Family::NYY: return "NYY-J " + std::to_string(cores) + "x" + fmt(size);
        case Family::JYSTY: return "J-Y(St)Y " + std::to_string(cores) + "x2x" + fmt(size);
        case Family::LiYCY: return "LiYCY " + std::to_string(cores) + "x" + fmt(size);
        case Family::Cat7: return "Cat 7 S/FTP 4x2xAWG23";
    }
    return {};
}

const std::vector<CableSpec>& all() { return kData; }

std::optional<CableSpec> lookup(std::string_view text) {
    const std::string s = normalise(text);
    Family fam{};
    std::size_t start = 0;
    if (s.rfind("nym", 0) == 0) {
        fam = Family::NYM;
        start = 3;
    } else if (s.rfind("nyy", 0) == 0) {
        fam = Family::NYY;
        start = 3;
    } else if (s.find("y(st)y") != std::string::npos || s.find("ysty") != std::string::npos) {
        fam = Family::JYSTY;
        const auto p = s.find("y(st)y");
        start = p != std::string::npos ? p + 6 : s.find("ysty") + 4;
    } else if (s.find("liycy") != std::string::npos) {
        fam = Family::LiYCY;
        start = s.find("liycy") + 5;
    } else if (s.rfind("cat7", 0) == 0 || s.rfind("cat.7", 0) == 0) {
        return kData.back();
    } else {
        return std::nullopt;
    }
    // numbersAfter skips the type suffix ("-j", "-o") before the dimensions
    const auto nums = numbersAfter(s, start);
    if (fam == Family::JYSTY) {
        if (nums.size() < 3) return std::nullopt;
        for (const auto& c : kData) {
            if (c.family == fam && c.cores == static_cast<int>(nums[0]) && std::fabs(c.size - nums[2]) < 1e-6) return c;
        }
        return std::nullopt;
    }
    if (nums.size() < 2) return std::nullopt;
    for (const auto& c : kData) {
        if (c.family == fam && c.cores == static_cast<int>(nums[0]) && std::fabs(c.size - nums[1]) < 1e-6) return c;
    }
    return std::nullopt;
}

Text source(Family f) {
    switch (f) {
        case Family::NYM: return {"Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte)", "Faber Kabel, data sheet NYM-J/NYM-O (approx. values)"};
        case Family::NYY: return {"Waskönig+Walter, Datenblatt NYY-J (2019)", "Waskönig+Walter, data sheet NYY-J (2019)"};
        case Family::JYSTY: return {"Lapp, Produktinformation J-Y(ST)Y…LG", "Lapp, product information J-Y(ST)Y…LG"};
        case Family::LiYCY: return {"Lapp, Datenblatt UNITRONIC LiYCY (2020)", "Lapp, data sheet UNITRONIC LiYCY (2020)"};
        case Family::Cat7: return {"DIGITUS DK-1743-VH (Cat 7, Dca)", "DIGITUS DK-1743-VH (Cat 7, Dca)"};
    }
    return {"", ""};
}

}  // namespace elc::cabledata
