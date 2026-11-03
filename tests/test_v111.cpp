// Version 1.11: surge protection extended with the DEHN white papers WPX035 "Blitz- und Überspannungsschutz für
// explosionsgefährdete Bereiche" and WPX050 "Blitz- und Überspannungsschutzkonzept für Rechenzentren" – reference values
// from these documents (data sheet tables, Bild 11, Bild 15, Tabelle 1–3).
#include <algorithm>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;

namespace {

bool hasWarning(const Result& r) {
    return std::any_of(r.messages.begin(), r.messages.end(), [](const Message& m) { return m.severity == Severity::Warning; });
}

bool contains(const Text& t, const std::string& s) { return t.de.find(s) != std::string::npos; }

/// Row of a table whose first cell contains `key`
const std::vector<Text>* rowOf(const Table& t, const std::string& key) {
    for (const auto& row : t.rows) {
        if (!row.empty() && contains(row.front(), key)) return &row;
    }
    return nullptr;
}

bool rowHas(const std::vector<Text>& row, const std::string& s) {
    return std::any_of(row.begin(), row.end(), [&](const Text& t) { return contains(t, s); });
}

bool tableHas(const Table& t, const std::string& s) {
    return std::any_of(t.rows.begin(), t.rows.end(), [&](const auto& row) { return rowHas(row, s); });
}

}  // namespace

TEST_CASE("Ex i measuring circuit: SPD against U_o/I_o, zone and installation rules (DEHN WPX035)", "[surge][ex]") {
    const auto* c = findCalculator("spd_ex");
    REQUIRE(c);
    CHECK(c->category == "surge");
    // defaults = WPX035 Tabelle 2: transmitter in zone 1 (U_o 29.4 V, I_o 130 mA), BXT ML4 BD EX 24 (ia, U_i 30 V, I_i 500 mA)
    auto in = defaultInputs(*c);
    auto r = evaluate(*c, in, {});
    REQUIRE_FALSE(r.hasError());
    CHECK_FALSE(hasWarning(r));
    CHECK(contains(r.texts.at("cat"), "ib"));          // zone 1 → EPL Gb, ia or ib
    CHECK(r.values.at("UiMin") == Approx(29.4));
    CHECK(r.values.at("IiMin") == Approx(130));
    CHECK(contains(r.texts.at("capacity"), "10 kA"));  // 10 impulses of 10 kA (8/20 µs)
    CHECK(contains(r.texts.at("earth"), "4 mm²"));
    CHECK(contains(r.texts.at("lpz"), "0B"));
    // SPD voltage below the open-circuit voltage of the supply → not permitted
    in.set("Ui", 24);
    CHECK(evaluate(*c, in, {}).hasError());
    in.set("Ui", 30);
    in.set("Ii", 100);  // below I_o 130 mA
    CHECK(evaluate(*c, in, {}).hasError());
    in.set("Ii", 500);
    // zone 0 needs ia
    in.setChoice("zone", "0");
    in.setChoice("spdcat", "ib");
    CHECK(evaluate(*c, in, {}).hasError());
    in.setChoice("spdcat", "ia");
    CHECK_FALSE(evaluate(*c, in, {}).hasError());
    in.setChoice("zone", "2");
    in.setChoice("spdcat", "ic");
    CHECK_FALSE(evaluate(*c, in, {}).hasError());
    // unshielded line longer than 1 m between SPD and device
    in = defaultInputs(*c);
    in.set("len", 3);
    in.setChoice("shield", "none");
    CHECK(hasWarning(evaluate(*c, in, {})));
    // earthed transmitter (insulation < 500 V AC) → protection level core–PG below its insulation, e.g. ≤ 35 V
    in = defaultInputs(*c);
    in.setChoice("iso", "no");
    CHECK(contains(evaluate(*c, in, {}).texts.at("up"), "35 V"));
    const auto ch = evaluateChart(*c, in, {});
    REQUIRE(ch);
    CHECK(ch->diagram);
}

TEST_CASE("Induced voltage in an installation loop: U = M · Δi/Δt (DEHN WPX050 Bild 15)", "[surge]") {
    const auto* c = findCalculator("lemp_loop");
    REQUIRE(c);
    auto in = defaultInputs(*c);  // M2 = 4.8 µH, LPL II subsequent stroke 150 kA/µs
    auto r = evaluate(*c, in, {});
    CHECK(r.values.at("didt") == Approx(150));
    CHECK(r.values.at("U") == Approx(720));  // 4.8 · 150 = 720 kV
    in.setChoice("lpl", "1");
    in.setChoice("stroke", "first");
    r = evaluate(*c, in, {});
    CHECK(r.values.at("didt") == Approx(20));  // 200 kA / 10 µs (IEC 62305-1)
    CHECK(r.values.at("U") == Approx(96));
    in.set("custom", 100);
    CHECK(evaluate(*c, in, {}).values.at("U") == Approx(480));
    REQUIRE(evaluateChart(*c, in, {}));
}

TEST_CASE("I&C lines leaving the building: lightning current per core for LPL I (VdS 2833)", "[surge]") {
    const auto* c = findCalculator("spd_signal");
    REQUIRE(c);
    auto in = defaultInputs(*c);  // zone "out"
    in.set("pairs", 2);
    CHECK(evaluate(*c, in, {}).values.at("IimpCore") == Approx(2.5));
    in.set("pairs", 3);
    CHECK(evaluate(*c, in, {}).values.at("IimpCore") == Approx(1.0));
    in.set("pairs", 10);
    CHECK(evaluate(*c, in, {}).values.at("IimpCore") == Approx(0.5));
    in.setChoice("zone", "in");
    CHECK_FALSE(evaluate(*c, in, {}).values.contains("IimpCore"));
}

TEST_CASE("SPD component tables carry the DEHN data sheet values (WPX050, WPX035)", "[surge]") {
    const auto* p = findTable("spd_products_power");
    REQUIRE(p);
    const auto* venci = rowOf(*p, "DEHNvenCI");
    REQUIRE(venci);
    CHECK(rowHas(*venci, "961 205"));
    CHECK(rowHas(*venci, "25 kA"));
    CHECK(rowHas(*venci, "≤ 1,5 kV"));
    CHECK(rowHas(*rowOf(*p, "DEHNventil"), "100 kA"));
    CHECK(rowHas(*rowOf(*p, "DEHNguard"), "20 kA"));
    CHECK(rowHas(*rowOf(*p, "DEHNrail"), "3 kA"));
    CHECK(rowHas(*rowOf(*p, "DEHNcube"), "1000 V"));

    const auto* s = findTable("spd_products_signal");
    REQUIRE(s);
    CHECK(rowHas(*rowOf(*s, "BCO ML2 BE 24"), "33 V"));
    CHECK(rowHas(*rowOf(*s, "BCO ML2 BD 24"), "36 V"));
    CHECK(rowHas(*rowOf(*s, "BCO ML2 BD HF 5"), "100 MHz"));
    CHECK(rowHas(*rowOf(*s, "DPA CLE IP66"), "250 MHz"));
    CHECK(rowHas(*rowOf(*s, "BXT ML4 BD EX 24"), "30 V"));

    const auto* dc = findTable("spd_datacenter");
    REQUIRE(dc);
    CHECK(dc->rows.size() == 7);  // Bild 11, positions 1–7
    CHECK(rowHas(dc->rows.front(), "961 205"));
    CHECK(rowHas(dc->rows.front(), "954 405"));
    CHECK(rowHas(dc->rows[5], "900 920"));

    const auto* cap = findTable("spd_discharge_capacity");
    REQUIRE(cap);
    CHECK(tableHas(*cap, "100 kA"));  // 3+1 N–PE, LPL I
    CHECK(tableHas(*cap, "2,5 kA"));  // < 3 pairs

    const auto* ex = findTable("ex_lightning_protection");
    REQUIRE(ex);
    CHECK(tableHas(*ex, "10 × 10 m"));
    CHECK(tableHas(*ex, "4 mm²"));
    const auto* dcl = findTable("datacenter_lightning");
    REQUIRE(dcl);
    CHECK(tableHas(*dcl, "5 × 5 m"));

    // down conductor spacing per LPS class (WPX035 Tabelle 1)
    const auto* lp = findTable("lightning_parameters");
    REQUIRE(lp);
    CHECK(lp->columns.size() == 8);
    CHECK(rowHas(*rowOf(*lp, "III"), "15 m"));
}

TEST_CASE("Surge area has pictures for zones, connections, applications and the calculators", "[surge][pictures]") {
    for (const auto* id : {"lpz_zones", "spd_types", "spd_connection", "overvoltage_categories", "spd_datacenter", "ex_lightning_protection"}) {
        INFO(id);
        const auto* t = findTable(id);
        REQUIRE(t);
        REQUIRE(t->drawing.has_value());
        CHECK(t->drawing->diagram);
        CHECK(t->drawing->aspect > 0);
        CHECK_FALSE(t->drawing->series.empty());
        CHECK_FALSE(t->drawing->note.de.empty());
        CHECK_FALSE(t->drawing->note.en.empty());
    }
    for (const auto* id : {"spd_power", "spd_signal", "spd_ex", "lemp_loop", "spd_distance"}) {
        INFO(id);
        const auto* c = findCalculator(id);
        REQUIRE(c);
        const auto ch = evaluateChart(*c, defaultInputs(*c), {});
        REQUIRE(ch);
        CHECK(ch->diagram);
        CHECK_FALSE(ch->series.empty());
    }
    // the connection picture follows the earthing system: TT shows the N–PE spark gap of 3+1
    const auto* c = findCalculator("spd_power");
    auto in = defaultInputs(*c);
    in.setChoice("net", "tt");
    const auto tt = evaluateChart(*c, in, {});
    const bool gap = std::any_of(tt->labels.begin(), tt->labels.end(), [](const ChartLabel& l) { return l.text.de.find("3+1") != std::string::npos; });
    CHECK(gap);
}

TEST_CASE("New surge topics sit in the surge area", "[surge]") {
    for (const auto* id : {"spd_ex", "lemp_loop"}) {
        INFO(id);
        CHECK(findCalculator(id)->category == "surge");
    }
    for (const auto* id : {"spd_products_power", "spd_products_signal", "spd_datacenter", "datacenter_lightning", "ex_lightning_protection",
                           "spd_discharge_capacity"}) {
        INFO(id);
        CHECK(findTable(id)->category == "surge");
    }
}
