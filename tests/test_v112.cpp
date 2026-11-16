// Version 1.12: design planning (HOAI stage 3) after Vielhauer/Geyer/Hürst/Adelmann, "Entwurfsplanung der Technischen
// Gebäudeausrüstung" (Springer Vieweg 2025) – book examples Abb. 11.10 (ISP supply) and Abb. 11.12 (cabinet area) –
// plus HVAC symbols and line colours (EN 16798-3, DIN 2403, DIN 1988).
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

bool tableHas(const Table& t, const std::string& s) {
    return std::any_of(t.rows.begin(), t.rows.end(),
                       [&](const auto& row) { return std::any_of(row.begin(), row.end(), [&](const Text& c) { return contains(c, s); }); });
}

}  // namespace

TEST_CASE("Design planning: HOAI basic services and quality premises", "[planning]") {
    const auto* s = findTable("lph3_services");
    REQUIRE(s);
    CHECK(s->category == "planning");
    CHECK(s->rows.size() == 8);  // HOAI 2021 Anlage 15, Lph 3: a) … h)
    CHECK(contains(s->rows.front().front(), "a"));
    CHECK(tableHas(*s, "DIN 276"));
    const auto* p = findTable("lph3_premises");
    REQUIRE(p);
    CHECK(p->rows.size() == 8);  // book chapter 4.6
    for (const auto* id : {"lph3_formal", "lph3_documents", "ga_lph3_documents", "ga_lph3_interfaces", "ga_lph3_rules"}) {
        INFO(id);
        const auto* t = findTable(id);
        REQUIRE(t);
        CHECK(t->category == "planning");
        CHECK(t->rows.size() >= 5);
        CHECK(contains(t->description, "Vielhauer"));  // source named
    }
    const auto* d = findTable("lph3_documents");
    for (const auto* kg : {"410", "420", "430", "434", "440", "450", "480"}) {
        INFO(kg);
        CHECK(tableHas(*d, kg));
    }
    CHECK(findTable("ga_lph3_documents")->rows.size() >= 12);  // book 11.2.1 … 11.2.14
}

TEST_CASE("GA cost calculation KG 480 and cost control", "[planning]") {
    const auto* c = findCalculator("ga_cost_480");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.set("dpPhys", 400);
    in.set("dpCom", 100);
    in.set("price", 440);
    in.set("estimate", 200000);
    auto r = evaluate(*c, in, {});
    CHECK(r.values.at("cost") == Approx(220000));
    CHECK(r.values.at("deviation") == Approx(10));
    CHECK_FALSE(hasWarning(r));
    in.set("price", 700);  // above the AMEV range 300 … 580 € per hardware data point
    CHECK(hasWarning(evaluate(*c, in, {})));
}

TEST_CASE("HVAC symbols and line colours with pictures", "[symbols]") {
    for (const auto* id : {"symbols_air", "symbols_heating", "symbols_sanitary", "symbols_cooling"}) {
        INFO(id);
        const auto* t = findTable(id);
        REQUIRE(t);
        CHECK(t->category == "hvac");
        CHECK(t->rows.size() >= 12);
        REQUIRE(t->drawing.has_value());
        CHECK(t->drawing->diagram);
        CHECK(t->drawing->aspect > 0);
        CHECK_FALSE(t->drawing->note.de.empty());
        // every symbol of the table is labelled with its number in the picture
        for (std::size_t i = 1; i <= t->rows.size(); ++i) {
            const std::string n = std::to_string(i);
            const bool found = std::any_of(t->drawing->labels.begin(), t->drawing->labels.end(),
                                           [&](const ChartLabel& l) { return l.text.de.rfind(n + " ", 0) == 0 || l.text.de == n; });
            INFO(n);
            CHECK(found);
        }
    }
    const auto* lc = findTable("line_colors");
    REQUIRE(lc);
    CHECK(lc->category == "hvac");
    REQUIRE(lc->drawing.has_value());
    CHECK(tableHas(*lc, "ODA"));   // outdoor air
    CHECK(tableHas(*lc, "grün"));
    CHECK(tableHas(*lc, "PWH-C"));  // circulation violet
    CHECK(tableHas(*lc, "DIN 2403"));
    CHECK(tableHas(*lc, "Sauerstoff"));
}
