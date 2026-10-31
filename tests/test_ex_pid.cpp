// Currency (€ and CHF), explosion protection and P&ID letter codes.
#include <algorithm>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/formulas/explosion.hpp"
#include "elc/instruments.hpp"
#include "elc/lists.hpp"
#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;

namespace {

Context chf() {
    Context c;
    c.currency = Currency::CHF;
    c.chfPerEur = 0.92;
    return c;
}

Context eur() {
    Context c;
    c.currency = Currency::EUR;
    c.chfPerEur = 0.92;
    return c;
}

const std::string& text(const Result& r, const char* id) {
    const auto it = r.texts.find(id);
    REQUIRE(it != r.texts.end());
    return it->second.de;
}

}  // namespace

// ---------------------------------------------------------------- currency

TEST_CASE("Cost results carry the currency and the converted amount") {
    const auto* c = findCalculator("energy_cost");
    REQUIRE(c);
    const auto in = defaultInputs(*c);
    const auto rEur = evaluate(*c, in, eur());
    REQUIRE(rEur.units.at("Cyear") == "€");
    const double v = rEur.values.at("Cyear");
    REQUIRE(rEur.alt.count("Cyear") == 1);
    CHECK(rEur.alt.at("Cyear").en.find("CHF") != std::string::npos);
    CHECK(rEur.alt.at("Cyear").en == "≈ " + formatNumber(v * 0.92, v * 0.92 >= 100 ? 0 : 2, Lang::En) + " CHF");

    const auto rChf = evaluate(*c, in, chf());
    CHECK(rChf.units.at("Cyear") == "CHF");
    CHECK(rChf.alt.at("Cyear").en == "≈ " + formatNumber(v / 0.92, v / 0.92 >= 100 ? 0 : 2, Lang::En) + " €");
    CHECK(currencySymbol(chf()) == "CHF");
    CHECK(currencySymbol(eur()) == "€");
}

TEST_CASE("List totals are converted as well") {
    const auto* l = findList("ga_points");
    REQUIRE(l);
    auto cfg = defaultSettings(*l);
    cfg.set("price", 500);
    const auto res = evaluateList(*l, exampleRows(*l), cfg, chf());
    CHECK(res.totals.units.at("cost") == "CHF");
    CHECK(res.totals.alt.count("cost") == 1);
}

TEST_CASE("Currency converter € ↔ CHF") {
    const auto* c = findCalculator("conv_currency");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.set("amount", 100);
    in.setChoice("from", "EUR");
    in.set("rate", 0.92);
    auto r = evaluate(*c, in, eur());
    CHECK(r.values.at("out") == Approx(92));
    CHECK(r.units.at("out") == "CHF");
    in.setChoice("from", "CHF");
    r = evaluate(*c, in, eur());
    CHECK(r.values.at("out") == Approx(100 / 0.92));
    CHECK(r.units.at("out") == "€");
    in.set("rate", kNaN);  // empty: rate from the settings
    Context ctx = eur();
    ctx.chfPerEur = 0.95;
    r = evaluate(*c, in, ctx);
    CHECK(r.values.at("out") == Approx(100 / 0.95));
}

// ---------------------------------------------------------------- explosion protection

TEST_CASE("Temperature class from ignition temperature (IEC 60079-0 limits)") {
    using namespace elc::ex;
    CHECK(temperatureClass(595) == 1);  // methane
    CHECK(temperatureClass(450.5) == 1);
    CHECK(temperatureClass(450) == 2);
    CHECK(temperatureClass(305) == 2);  // acetylene
    CHECK(temperatureClass(300) == 3);
    CHECK(temperatureClass(170) == 4);  // diethyl ether
    CHECK(temperatureClass(120) == 5);
    CHECK(temperatureClass(95) == 6);   // carbon disulphide
    CHECK(temperatureClass(80) == 0);   // below T6: no class sufficient
    CHECK(classLimit(1) == 450);
    CHECK(classLimit(4) == 135);
    CHECK(classLimit(6) == 85);
    // dust (IEC 60079-14): T_max ≤ 2/3 · T_cloud and ≤ T_5mm − 75 K
    CHECK(dustMaxSurface(500, 300) == Approx(225));
    CHECK(dustMaxSurface(330, 400) == Approx(220));
    CHECK(groupCovers("IIC", "IIB"));
    CHECK(groupCovers("IIB", "IIB"));
    CHECK_FALSE(groupCovers("IIA", "IIC"));
    CHECK(groupCovers("IIIC", "IIIA"));
}

TEST_CASE("Ex selection: hydrogen in zone 1") {
    const auto* c = findCalculator("ex_selection");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.setChoice("zone", "1");
    in.setChoice("sub", "hydrogen");
    in.setChoice("devT", "T4");
    in.setChoice("devG", "IIB");
    const auto r = evaluate(*c, in, {});
    CHECK(text(r, "cat") == "2G (EPL Gb)");
    CHECK(text(r, "tcls") == "T1");
    CHECK(text(r, "group") == "IIC");
    CHECK(text(r, "mark").find("II 2G Ex") == 0);
    CHECK(text(r, "mark").find("IIC T1 Gb") != std::string::npos);
    CHECK(r.values.at("ait") == Approx(560));
    CHECK(r.hasError());  // IIB device not suitable for hydrogen
}

TEST_CASE("Ex selection: dust zone 21 with layer rule") {
    const auto* c = findCalculator("ex_selection");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.setChoice("zone", "21");
    in.setChoice("dgroup", "IIIB");
    in.set("tcl", 450);
    in.set("t5", 280);
    in.set("layer", 5);
    in.set("devTmax", 135);
    const auto r = evaluate(*c, in, {});
    CHECK(text(r, "cat") == "2D (EPL Db)");
    CHECK(r.values.at("tmax") == Approx(205));  // min(300, 205)
    CHECK(text(r, "mark") == "II 2D Ex tb IIIB T205°C Db");
    CHECK_FALSE(r.hasError());
}

// ---------------------------------------------------------------- P&ID letter codes

TEST_CASE("Instrument lookup by name") {
    const auto m = instruments::find("Temperatur Zuluft");
    REQUIRE_FALSE(m.empty());
    CHECK(m.front().entry->vdi == "TZU");
    CHECK(instruments::find("Gas Methan").front().entry->annotation == "CH₄");
    CHECK(instruments::find("Leckage").front().entry->vdi == "LCK");
    CHECK(instruments::find("Feuchte").front().entry->isa == "M");
    CHECK(instruments::find("frostschutzwaechter").front().entry->vdi == "FSW");
    CHECK(instruments::find("Druckwächter").front().entry->vdi == "DRW");
    CHECK(instruments::find("xyzzy").empty());
}

TEST_CASE("Letter codes per ISA-5.1 and EN 62424") {
    using instruments::Function;
    const auto& temp = *instruments::find("Temperatur").front().entry;
    CHECK(instruments::isaCode(temp, Function::Transmitter) == "TT");
    CHECK(instruments::isaCode(temp, Function::Element) == "TE");
    CHECK(instruments::isaCode(temp, Function::Controller) == "TIC");
    CHECK(instruments::isaCode(temp, Function::SwitchHigh) == "TSH");
    CHECK(instruments::enCode(temp, Function::Transmitter) == "TI");
    CHECK(instruments::enCode(temp, Function::Safety) == "TZH");

    const auto& dp = *instruments::find("Differenzdruck").front().entry;
    CHECK(instruments::isaCode(dp, Function::Transmitter) == "PDT");
    const auto& meter = *instruments::find("Wasserzähler").front().entry;
    CHECK(instruments::isaCode(meter, Function::Totalizer) == "FQI");
    const auto& methane = *instruments::find("Methan").front().entry;
    CHECK(instruments::isaCode(methane, Function::Transmitter) == "AT");
    CHECK(instruments::enCode(methane, Function::Transmitter) == "QI");
}

TEST_CASE("Tag finder calculator") {
    const auto* c = findCalculator("pid_tags");
    REQUIRE(c);
    auto in = defaultInputs(*c);
    in.setChoice("name", "Frostschutzwächter");
    in.setChoice("func", "auto");
    in.setChoice("loop", "101");
    auto r = evaluate(*c, in, {});
    REQUIRE_FALSE(r.hasError());
    CHECK(text(r, "isa") == "TSL");
    CHECK(text(r, "tag") == "TSL-101");
    CHECK(text(r, "vdi") == "FSW");
    CHECK(text(r, "bas") == "FSW01_GM_01");

    in.setChoice("name", "Gas Methan");
    r = evaluate(*c, in, {});
    CHECK(text(r, "isa") == "AT");
    CHECK(text(r, "tag") == "AT-101 (CH₄)");

    in.setChoice("name", "unbekanntes Ding");
    r = evaluate(*c, in, {});
    CHECK(r.hasError());
}

TEST_CASE("Instrument table lists every entry") {
    const auto* t = findTable("pid_instruments");
    REQUIRE(t);
    CHECK(t->rows.size() == instruments::all().size());
    CHECK(instruments::all().size() >= 60);
}
