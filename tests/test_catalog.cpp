// Contract tests: every calculator/table definition is complete and every calculator
// computes with its default inputs without error.
#include <algorithm>
#include <cmath>
#include <set>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/lists.hpp"
#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;

namespace {

bool hasCategory(const std::string& id) {
    const auto& cats = categories();
    return std::any_of(cats.begin(), cats.end(), [&](const Category& c) { return c.id == id; });
}

bool complete(const Text& t) { return !t.de.empty() && !t.en.empty(); }

double value(const Result& r, const std::string& id) {
    const auto it = r.values.find(id);
    REQUIRE(it != r.values.end());
    return it->second;
}

bool hasSeverity(const Result& r, Severity s) {
    return std::any_of(r.messages.begin(), r.messages.end(), [&](const Message& m) { return m.severity == s; });
}

}  // namespace

TEST_CASE("Registry is populated") {
    CHECK(calculators().size() >= 45);
    CHECK(tables().size() >= 25);
    for (const auto& cat : categories()) {
        const bool any = std::any_of(calculators().begin(), calculators().end(), [&](const Calculator& c) { return c.category == cat.id; }) ||
                         std::any_of(tables().begin(), tables().end(), [&](const Table& t) { return t.category == cat.id; }) ||
                         std::any_of(lists().begin(), lists().end(), [&](const ListDef& l) { return l.category == cat.id; });
        INFO(cat.id);
        CHECK(any);
    }
}

TEST_CASE("Calculator definitions are complete and compute with defaults") {
    std::set<std::string> ids;
    for (const auto& c : calculators()) {
        INFO("calculator " << c.id);
        CHECK(ids.insert(c.id).second);
        CHECK(hasCategory(c.category));
        CHECK(complete(c.title));
        CHECK(complete(c.summary));
        CHECK(complete(c.description));
        CHECK_FALSE(c.outputs.empty());
        REQUIRE(c.compute);

        std::set<std::string> inputIds;
        for (const auto& in : c.inputs) {
            INFO("input " << in.id);
            CHECK(inputIds.insert(in.id).second);
            CHECK(complete(in.label));
            if (in.kind == InputDef::Kind::Choice) {
                REQUIRE_FALSE(in.options.empty());
                CHECK(std::any_of(in.options.begin(), in.options.end(), [&](const Option& o) { return o.value == in.defaultChoice; }));
                for (const auto& o : in.options) CHECK(complete(o.label));
            }
            if (in.visibleIf) {
                const auto it = std::find_if(c.inputs.begin(), c.inputs.end(), [&](const InputDef& d) { return d.id == in.visibleIf->inputId; });
                REQUIRE(it != c.inputs.end());
                CHECK(it->kind == InputDef::Kind::Choice);
                for (const auto& v : in.visibleIf->values) {
                    CHECK(std::any_of(it->options.begin(), it->options.end(), [&](const Option& o) { return o.value == v; }));
                }
            }
        }
        std::set<std::string> outIds;
        for (const auto& o : c.outputs) {
            CHECK(outIds.insert(o.id).second);
            CHECK(complete(o.label));
        }

        for (const auto norm : {NormProfile::DE, NormProfile::CH}) {
            const auto r = evaluate(c, defaultInputs(c), Context{norm});
            for (const auto& m : r.messages) {
                INFO("message " << m.text.de);
                CHECK(complete(m.text));
            }
            for (const auto& [id, v] : r.values) {
                INFO("output " << id);
                CHECK(outIds.contains(id));
                CHECK(std::isfinite(v));
            }
            for (const auto& [id, t] : r.texts) {
                CHECK(outIds.contains(id));
                CHECK(complete(t));
            }
            // defaults must produce a result (an error may only come from a limit check, never from missing data)
            CHECK_FALSE(r.values.empty());
        }
    }
}

TEST_CASE("Table definitions are rectangular and translated") {
    std::set<std::string> ids;
    for (const auto& t : tables()) {
        INFO("table " << t.id);
        CHECK(ids.insert(t.id).second);
        CHECK(hasCategory(t.category));
        CHECK(complete(t.title));
        CHECK(complete(t.description));
        REQUIRE_FALSE(t.columns.empty());
        CHECK_FALSE(t.rows.empty());
        for (const auto& row : t.rows) {
            CHECK(row.size() == t.columns.size());
            for (const auto& cell : row) CHECK(complete(cell));
        }
    }
}

TEST_CASE("Voltage drop end to end with DE/CH limit") {
    const auto* c = findCalculator("voltage_drop");
    REQUIRE(c);
    const auto in = defaultInputs(*c);  // 1~ 230 V, 16 A, 30 m, 2.5 mm² Cu, 70 °C
    const auto de = evaluate(*c, in, {NormProfile::DE});
    CHECK(value(de, "dU") == Approx(7.921).epsilon(1e-3));
    CHECK(value(de, "dUpct") == Approx(3.444).epsilon(1e-3));
    CHECK(value(de, "limit") == Approx(3.0));
    CHECK(hasSeverity(de, Severity::Error));
    const auto ch = evaluate(*c, in, {NormProfile::CH});
    CHECK(value(ch, "limit") == Approx(4.0));
    CHECK(hasSeverity(ch, Severity::Ok));
}

TEST_CASE("Cable sizing default case") {
    const auto* c = findCalculator("cable_sizing");
    REQUIRE(c);
    const auto r = evaluate(*c, defaultInputs(*c), {});
    CHECK(value(r, "A") == Approx(2.5));
    CHECK(value(r, "Iz") == Approx(24));
    CHECK(value(r, "In") == Approx(20));
    CHECK(value(r, "Avd") == Approx(2.153).epsilon(2e-3));
    CHECK(hasSeverity(r, Severity::Ok));
}

TEST_CASE("Cable sizing escalates for long lines") {
    const auto* c = findCalculator("cable_sizing");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.set("L", 200);
    const auto r = evaluate(*c, in, {});
    CHECK(value(r, "A") == Approx(16));
    CHECK(r.texts.at("decisive").en == "Voltage drop");
}

TEST_CASE("Validation reports missing and out-of-range inputs") {
    const auto* c = findCalculator("voltage_drop");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.set("L", kNaN);
    CHECK(evaluate(*c, in, {}).hasError());
    in.set("L", -5);
    CHECK(evaluate(*c, in, {}).hasError());
    // hidden inputs are not validated: DC hides cos φ
    auto dc = defaultInputs(*c);
    dc.setChoice("sys", "dc");
    dc.set("cosphi", kNaN);
    const auto r = evaluate(*c, dc, {});
    CHECK_FALSE(r.values.empty());  // computed (an error here would only be the 3 % limit check)
    CHECK(value(r, "dU") == Approx(7.921).epsilon(1e-3));
}

TEST_CASE("Ohm calculator needs exactly two values") {
    const auto* c = findCalculator("ohm");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.set("R", 10);
    CHECK(evaluate(*c, in, {}).hasError());
}

TEST_CASE("Analog scaling 12 mA → 50 % → S7 13824") {
    const auto* c = findCalculator("analog_scaling");
    REQUIRE(c);
    const auto r = evaluate(*c, defaultInputs(*c), {});
    CHECK(value(r, "pct") == Approx(50));
    CHECK(value(r, "eng") == Approx(50));
    CHECK(value(r, "raw") == Approx(13824));
    CHECK(r.units.at("sig") == "mA");
}

TEST_CASE("Colour code yellow-violet-red-gold = 4.7 kΩ ±5 %") {
    const auto* c = findCalculator("resistor_color");
    REQUIRE(c);
    const auto r = evaluate(*c, defaultInputs(*c), {});
    CHECK(value(r, "value") == Approx(4.7));
    CHECK(r.units.at("value") == "kΩ");
    CHECK(value(r, "tol") == Approx(5));
}
