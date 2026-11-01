// Hydraulic basic circuits guide (v1.6): selection help, schematic drawings and texts.
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "elc/registry.hpp"

using namespace elc;

namespace {

const char* const kCircuits[] = {"throttle", "throttle_picv", "inject2", "inject_picv",
                                 "mixing",   "mixing_fixed",  "diverting", "inject3"};

Inputs inputsFor(std::initializer_list<std::pair<const char*, const char*>> choices) {
    const auto* c = findCalculator("hydraulic_circuits");
    REQUIRE(c != nullptr);
    Inputs in = defaultInputs(*c);
    for (const auto& [k, v] : choices) in.setChoice(k, v);
    return in;
}

std::string recommended(std::initializer_list<std::pair<const char*, const char*>> choices) {
    const auto* c = findCalculator("hydraulic_circuits");
    auto in = inputsFor(choices);
    in.setChoice("mode", "help");
    const auto r = evaluate(*c, in, Context{});
    REQUIRE_FALSE(r.hasError());
    const auto it = r.texts.find("name");
    REQUIRE(it != r.texts.end());
    return it->second.en;
}

}  // namespace

TEST_CASE("every circuit has texts in both languages and a diagram inside its bounds", "[hydraulics]") {
    const auto* c = findCalculator("hydraulic_circuits");
    REQUIRE(c != nullptr);
    REQUIRE(c->chart);
    for (const char* id : kCircuits) {
        INFO(id);
        auto in = inputsFor({{"mode", "view"}, {"circuit", id}});
        const auto r = evaluate(*c, in, Context{});
        REQUIRE_FALSE(r.hasError());
        for (const char* out : {"name", "type", "dist", "vltemp", "flowc", "flowg", "pumps", "rlt", "valve", "func", "props", "use"}) {
            INFO(out);
            const auto it = r.texts.find(out);
            REQUIRE(it != r.texts.end());
            CHECK_FALSE(it->second.de.empty());
            CHECK_FALSE(it->second.en.empty());
        }
        const auto ch = evaluateChart(*c, in, Context{});
        REQUIRE(ch);
        CHECK(ch->diagram);
        CHECK(ch->aspect > 0.5);
        CHECK(ch->series.size() > 10);
        for (const auto& s : ch->series)
            for (const auto& p : s.pts) {
                CHECK(p[0] >= ch->x.min);
                CHECK(p[0] <= ch->x.max);
                CHECK(p[1] >= ch->y.min);
                CHECK(p[1] <= ch->y.max);
            }
        bool hasValve = false;
        for (const auto& m : ch->markers) hasValve = hasValve || m.style == "motor";
        CHECK(hasValve);
    }
}

TEST_CASE("selection help follows the rules of the books", "[hydraulics]") {
    // Luftkühler mit Entfeuchtung → Durchflussregelung (Drossel)
    CHECK(recommended({{"dehum", "yes"}, {"dist", "pressure"}}) == "Throttling circuit");
    // Lufterwärmer mit Einfriergefahr am drucklosen Verteiler → Beimischschaltung
    CHECK(recommended({{"frost", "yes"}, {"dist", "unpressurised"}}) == "Mixing circuit");
    // … am druckbehafteten Verteiler mit tiefer Rücklauftemperatur → Einspritz mit Durchgangsventil
    CHECK(recommended({{"frost", "yes"}, {"dist", "pressure"}, {"lowrl", "yes"}}) == "Injection circuit with two-way valve");
    // … ohne Anforderung an den Rücklauf → Einspritz mit Dreiwegventil
    CHECK(recommended({{"frost", "yes"}, {"dist", "pressure"}, {"lowrl", "no"}}) == "Injection circuit with three-way valve");
    // Fußbodenheizung (VL deutlich tiefer) am drucklosen Verteiler → feste Vormischung
    CHECK(recommended({{"control", "mix"}, {"dist", "unpressurised"}, {"lowvl", "yes"}}) == "Mixing circuit with fixed premix");
    // mengenvariabel, konstanter Erzeugerdurchfluss gewünscht, Rücklauf egal → Umlenkschaltung
    CHECK(recommended({{"control", "flow"}, {"dist", "pressure"}, {"lowrl", "no"}, {"constgen", "yes"}}) == "Diverting circuit");
    // mengenvariabel mit tiefem Rücklauf → Drossel
    CHECK(recommended({{"control", "flow"}, {"dist", "pressure"}, {"lowrl", "yes"}}) == "Throttling circuit");
}

TEST_CASE("selection table lists all circuits", "[hydraulics]") {
    const auto* t = findTable("hydraulic_selection");
    REQUIRE(t != nullptr);
    CHECK(t->rows.size() == 8);
    for (const auto& row : t->rows) CHECK(row.size() == t->columns.size());
}
