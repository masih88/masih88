// Project lists: consumer list (checked against the user's Excel sheet) and data point list.
#include <algorithm>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "elc/lists.hpp"
#include "elc/registry.hpp"

using Catch::Approx;
using namespace elc;

namespace {

const ListDef& consumers() {
    const auto* l = findList("consumer_list");
    REQUIRE(l);
    return *l;
}

double val(const Result& r, const char* id) {
    const auto it = r.values.find(id);
    REQUIRE(it != r.values.end());
    return it->second;
}

Inputs excelSettings(const ListDef& l) {
    auto s = defaultSettings(l);
    s.setChoice("grow", "nein");  // like the Excel sheet: ampacity only
    return s;
}

}  // namespace

TEST_CASE("Consumer row like Excel row 6: 230 V, 0.5 kW, MSS, E, f1 0.91, f2 0.8") {
    const auto& l = consumers();
    auto row = defaultRow(l);
    row.setChoice("U", "230");
    row.set("P", 0.5);
    row.set("cosphi", 0.9);
    row.setChoice("dev", "MSS");
    row.setChoice("method", "E");
    row.set("f1", 0.91);
    row.set("f2", 0.8);
    row.set("L", 95);
    const auto r = evaluateRow(l, row, excelSettings(l), {});
    CHECK(val(r, "S") == Approx(0.555556).epsilon(1e-5));
    CHECK(val(r, "IB") == Approx(2.41546).epsilon(1e-5));  // Excel Y6
    CHECK(val(r, "F") == Approx(0.728));                    // Excel AL6
    CHECK(val(r, "IMW") == Approx(3.31794).epsilon(1e-5));  // Excel AM6
    CHECK(val(r, "A") == Approx(1.5));                      // Excel AZ6
    CHECK(val(r, "IZ") == Approx(16.016));                  // Excel BB6
    CHECK(r.texts.at("check").de == "i. O.");
}

TEST_CASE("Consumer row like Excel row 9: 400 V, 15 kW → 4 mm²") {
    const auto& l = consumers();
    auto row = defaultRow(l);
    row.setChoice("U", "400");
    row.set("P", 15);
    row.set("cosphi", 0.9);
    row.setChoice("dev", "MSS");
    row.setChoice("method", "E");
    row.set("f1", 0.91);
    row.set("f2", 0.82);
    row.set("L", 80);
    const auto r = evaluateRow(l, row, excelSettings(l), {});
    // Excel uses 1.73 instead of √3: 24.0848 A; exact √3 gives 24.056 A
    CHECK(val(r, "IB") == Approx(24.056).epsilon(1e-4));
    CHECK(val(r, "A") == Approx(4));
    CHECK(val(r, "IZ") == Approx(25.3708).epsilon(1e-4));  // Excel BB9
    // ΔU with R' = ρ(70 °C)/A (the sheet's table holds ≈ 20 °C values → 3.46 %)
    CHECK(val(r, "dUpct") == Approx(3.907).epsilon(2e-3));
    CHECK(r.hasError() == false);
}

TEST_CASE("Automatic cross-section increase for voltage drop") {
    const auto& l = consumers();
    auto row = defaultRow(l);
    row.set("P", 15);
    row.set("cosphi", 0.9);
    row.set("f1", 0.91);
    row.set("f2", 0.82);
    row.set("L", 80);
    const auto r = evaluateRow(l, row, defaultSettings(l), {});  // limit 2.5 %
    CHECK(val(r, "Aamp") == Approx(4));
    CHECK(val(r, "A") == Approx(10));
    CHECK(val(r, "dUpct") <= 2.5);
}

TEST_CASE("MCB and gG fuse rows check I_2 and pick the next rating") {
    const auto& l = consumers();
    auto mcb = defaultRow(l);
    mcb.setChoice("U", "230");
    mcb.set("P", 3);
    mcb.set("cosphi", 1);
    mcb.setChoice("dev", "LS-C");
    mcb.set("L", 10);
    const auto r1 = evaluateRow(l, mcb, defaultSettings(l), {});
    CHECK(val(r1, "In") == Approx(16));
    CHECK(val(r1, "IMW") == Approx(16));
    CHECK(val(r1, "A") == Approx(1.5));
    CHECK_FALSE(r1.hasError());

    auto fuse = defaultRow(l);
    fuse.set("P", 30);
    fuse.set("cosphi", 0.85);
    fuse.setChoice("dev", "gG");
    fuse.set("L", 10);
    const auto r2 = evaluateRow(l, fuse, defaultSettings(l), {});
    CHECK(val(r2, "IB") == Approx(50.94).epsilon(1e-3));
    CHECK(val(r2, "In") == Approx(63));
    CHECK(val(r2, "A") == Approx(16));
    CHECK(r2.texts.at("devinfo").de.find("gG 63 A") != std::string::npos);
}

TEST_CASE("Totals per supply exclude redundant consumers") {
    const auto& l = consumers();
    std::vector<Inputs> rows;
    auto a = defaultRow(l);
    a.set("P", 9);
    a.set("cosphi", 0.9);
    a.setChoice("supply", "NN");
    rows.push_back(a);
    auto b = a;
    b.setChoice("supply", "USV");
    b.setChoice("U", "230");
    b.set("P", 0.9);
    b.set("n", 2);
    rows.push_back(b);
    auto c = a;
    c.setChoice("red", "ja");
    rows.push_back(c);
    const auto res = evaluateList(l, rows, defaultSettings(l), {});
    CHECK(val(res.totals, "S_NN") == Approx(10));
    CHECK(val(res.totals, "S_USV") == Approx(2));
    CHECK(val(res.totals, "S") == Approx(12));
    CHECK(val(res.totals, "P") == Approx(10.8));
    CHECK(val(res.totals, "I") == Approx(12000 / (1.7320508 * 400)).epsilon(1e-4));
}

TEST_CASE("CSV export and re-import round trip") {
    const auto& l = consumers();
    std::vector<Inputs> rows;
    auto a = defaultRow(l);
    a.setChoice("name", "Pumpe Heizung");
    a.set("P", 2.2);
    a.setChoice("U", "24dc");
    rows.push_back(a);
    rows.push_back(defaultRow(l));
    const auto csv = exportCsv(l, rows, defaultSettings(l), {}, Lang::De);
    CHECK(csv.find("Anlage/Gerät") != std::string::npos);
    CHECK(csv.find("Summen") != std::string::npos);
    const auto back = importTable(l, csv);
    REQUIRE(back.size() == 2);
    CHECK(back[0].text("name") == "Pumpe Heizung");
    CHECK(back[0].num("P") == Approx(2.2));
    CHECK(back[0].choice("U") == "24dc");
}

TEST_CASE("Import of a table copied from the Excel sheet (tab separated)") {
    const auto& l = consumers();
    const std::string tsv =
        "Verbraucherdaten\t\t\n"
        "Nummer\tGewerk\tAnlage/Gerät\tAnzahl\tAnschluss ELT/MSR\tNEA/USV/NN\tNennspannung / V\tNennleistung ASV / kW\t"
        "cos phi / -\tGleichzeitigkeitsfaktor\tRedundant [Ja/Nein]\tTyp Schutzeinrichtung\tVerlegeart: C (Rohr), E (Trasse)\t"
        "f1 Faktor Temperatur\tf2 Häufung\tLänge Leitung / m\n"
        "1\tRLT\tVentilator Zuluft\t1\tMSR\tNEA\t230\t0,5\t0,9\t1\tnein\tMSS\tE\t0,91\t0,8\t95\n"
        "4\tTKW\tRadialschmiedeanlage\t1\tMSR\tNEA\t400\t15\t0,9\t1\tnein\tMSS\tE\t0,91\t0,82\t80\n"
        "5\tSAN\tHebeanlage\t1\tELT\tUSV\t24 DC\t0,3\t0,9\t1\tja\tSelMod\tC\t0,91\t0,82\t95\n"
        "6\t\t\t1\t\t\t400\t\t\t1\tnein\tMSS\tE\t0,91\t0,82\t\n";  // empty template row → skipped
    const auto rows = importTable(l, tsv);
    REQUIRE(rows.size() == 3);
    CHECK(rows[0].text("name") == "Ventilator Zuluft");
    CHECK(rows[0].choice("supply") == "NEA");
    CHECK(rows[0].num("f2") == Approx(0.8));
    CHECK(rows[1].choice("U") == "400");
    CHECK(rows[2].choice("U") == "24dc");
    CHECK(rows[2].choice("red") == "ja");
    CHECK(rows[2].choice("dev") == "SelMod");
    CHECK(rows[2].choice("method") == "C");
    auto cfg = defaultSettings(l);
    cfg.setChoice("grow", "nein");
    const auto r = evaluateList(l, rows, cfg, {});
    CHECK(val(r.rows[0], "IZ") == Approx(16.016));
}

TEST_CASE("Import keeps multi-line Excel headers in quotes") {
    const auto& l = consumers();
    const std::string tsv =
        "Anlage/Gerät\t\"Nennleistung ASV / kW\"\t\"Länge Leitung\n/ m\"\n"
        "Pumpe\t2,2\t40\n";
    const auto rows = importTable(l, tsv);
    REQUIRE(rows.size() == 1);
    CHECK(rows[0].num("P") == Approx(2.2));
    CHECK(rows[0].num("L") == Approx(40));
}

TEST_CASE("Data point list counts, modules and BAS addresses") {
    const auto* l = findList("ga_points");
    REQUIRE(l);
    std::vector<Inputs> rows;
    auto fans = defaultRow(*l);  // fan with VFD: SB BO, BM BI, SM BI, ST AO, WM BI
    fans.set("qty", 2);
    rows.push_back(fans);
    auto temp = defaultRow(*l);
    temp.setChoice("tpl", "temp");
    temp.setChoice("comp", "TZU");
    rows.push_back(temp);
    const auto res = evaluateList(*l, rows, defaultSettings(*l), {});
    CHECK(val(res.rows[0], "BI") == 6);
    CHECK(val(res.rows[0], "BO") == 2);
    CHECK(val(res.rows[0], "AO") == 2);
    CHECK(res.rows[0].texts.at("example").de == "K_HG_01_L_VKA01_ZUV01_SB_01");
    CHECK(val(res.totals, "phys") == 11);
    CHECK(val(res.totals, "physRes") == 14);  // ⌈11 · 1.2⌉
    CHECK(val(res.totals, "modBI") == 1);
    const auto csv = exportCsv(*l, rows, defaultSettings(*l), {}, Lang::De, true);
    CHECK(csv.find("K_HG_01_L_VKA01_ZUV02_WM_01") != std::string::npos);
    CHECK(csv.find("K_HG_01_L_VKA01_TZU01_MW_01") != std::string::npos);
    // 2 fans · 5 points + 1 sensor + header = 12 lines
    CHECK(std::count(csv.begin(), csv.end(), '\n') == 12);
}

TEST_CASE("Example projects load completely and compute without errors") {
    for (const auto& l : lists()) {
        INFO(l.id);
        const auto rows = exampleRows(l);
        const auto expected = static_cast<std::size_t>(std::count(l.example.begin(), l.example.end(), '\n')) - 1;
        REQUIRE(rows.size() == expected);
        const auto res = evaluateList(l, rows, defaultSettings(l), {});
        for (std::size_t i = 0; i < rows.size(); ++i) {
            INFO(rows[i].text(l.rowInputs.front().id));
            CHECK_FALSE(res.rows[i].hasError());
        }
    }
    const auto& c = consumers();
    const auto rows = exampleRows(c);
    CHECK(rows[7].choice("dev") == "LS-B");
    CHECK(rows[8].choice("U") == "24dc");
    CHECK(rows[6].choice("red") == "ja");
}

TEST_CASE("Tab separated export for pasting into a spreadsheet") {
    const auto& c = consumers();
    auto row = defaultRow(c);
    row.setChoice("name", "Pumpe; Heizung");
    const auto tsv = exportCsv(c, {row}, defaultSettings(c), {}, Lang::En, false, '\t');
    CHECK(tsv.find("Plant/device\t") == 0);
    CHECK(tsv.find("\tPumpe; Heizung\t") == std::string::npos);  // first column, no quoting needed for ';'
    CHECK(tsv.find("Pumpe; Heizung\t") != std::string::npos);
    CHECK(importTable(c, tsv).front().text("name") == "Pumpe; Heizung");
}

TEST_CASE("List definitions are complete") {
    for (const auto& l : lists()) {
        INFO(l.id);
        CHECK_FALSE(l.title.de.empty());
        CHECK_FALSE(l.title.en.empty());
        CHECK_FALSE(l.rowInputs.empty());
        CHECK(l.rowInputs.front().kind == InputDef::Kind::Text);
        const auto res = evaluateList(l, {defaultRow(l)}, defaultSettings(l), {});
        CHECK_FALSE(res.rows.front().hasError());
        CHECK_FALSE(res.totals.values.empty());
    }
}
