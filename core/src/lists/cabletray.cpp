// Project list: cable tray – fill, width, weight and grouping (v1.10).
// Method after OBO Bettermann "KTS-Guide: Systemauswahl": space per cable = d², about 30 % space reserve, usable
// cross-section width × height, permissible cable load per DIN VDE 0639 with 0.028 N/(m·mm²), support spacing ≤ 1.5 m;
// cable data from manufacturer data sheets (cabledata.cpp).
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/cabledata.hpp"
#include "elc/formulas/cables.hpp"
#include "elc/lists.hpp"

namespace elc::catalog {

namespace {

constexpr double kVde0639 = 0.028;  // N/(m·mm²), heaviest cable in VDE 0639 (NYY-J 4x95), OBO KTS guide
constexpr std::array<double, 9> kWidths{50, 75, 100, 150, 200, 300, 400, 500, 600};

struct RowCable {
    double d = kNaN;
    double g = kNaN;  // kg/km
    bool power = true;
    std::string name;
};

RowCable cableOf(const Inputs& in) {
    RowCable c;
    if (const auto spec = cabledata::lookup(in.text("cable"))) {
        c.d = spec->d;
        c.g = spec->kgPerKm;
        c.power = spec->power();
        c.name = spec->name();
    }
    if (const auto d = in.maybe("d")) c.d = *d;
    if (const auto g = in.maybe("g")) c.g = *g;
    const auto& kind = in.choice("kind");
    if (kind == "power") c.power = true;
    if (kind == "data") c.power = false;
    return c;
}

Result trayRow(const Inputs& in, const Inputs&, const Context&) {
    Result r;
    const auto c = cableOf(in);
    if (!(c.d > 0)) {
        throw CalcError({"Kabeltyp nicht in der Datenbank – Außendurchmesser d (und Gewicht) eingeben.",
                         "Cable type not in the database – enter outer diameter d (and weight)."});
    }
    const double n = std::round(in.num("n"));
    r.set("dd", c.d);
    r.set("A", n * c.d * c.d);
    r.set("sumd", n * c.d);
    if (c.g > 0) {
        r.set("q", n * c.g / 1000.0);
    } else {
        r.warn({"Gewicht unbekannt – Zeile ohne Last gerechnet.", "Weight unknown – row computed without load."});
    }
    r.setText("kindOut", c.power ? Text{"Energie", "power"} : Text{"Daten/Steuerung", "data/control"});
    if (!c.name.empty()) r.setText("type", {c.name, c.name});
    return r;
}

Result traySummary(const std::vector<Inputs>& rows, const std::vector<Result>& results, const Inputs& set, const Context&) {
    Result t;
    double A = 0, q = 0, widthPower = 0;
    int nPower = 0, nData = 0;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const auto& r = results[i];
        if (r.hasError()) continue;
        A += r.values.at("A");
        if (const auto it = r.values.find("q"); it != r.values.end()) q += it->second;
        const int n = static_cast<int>(std::round(rows[i].num("n")));
        if (cableOf(rows[i]).power) {
            nPower += n;
            widthPower += r.values.at("sumd");
        } else {
            nData += n;
        }
    }
    const double H = parseDouble(set.choice("h"));
    const double reserve = set.num("reserve") / 100.0;
    const double Areq = A * (1.0 + reserve);
    double B = kWidths.back();
    int trays = 1;
    bool found = false;
    for (const double w : kWidths) {
        if (w * H >= Areq) {
            B = w;
            found = true;
            break;
        }
    }
    if (!found) trays = static_cast<int>(std::ceil(Areq / (kWidths.back() * H)));
    const double Atray = B * H * trays;
    t.set("A", A);
    t.set("Areq", Areq);
    t.set("B", B);
    t.set("trays", trays);
    t.set("fill", A / Atray * 100.0);
    t.set("fillRes", Areq / Atray * 100.0);
    t.set("q", q);
    const double F = q * 9.81 / 1000.0 / trays;  // kN/m per tray
    t.set("F", F);
    t.set("Fmax", kVde0639 * B * H / 1000.0);
    t.set("nPower", nPower);
    t.set("nData", nData);
    t.set("widthPower", widthPower);
    if (!found) {
        t.warn(cat(Text{"Mehr als eine Trasse nötig: ", "More than one tray needed: "}, numText(trays, 0), Text{" × 600 mm.", " × 600 mm."}));
    }
    // grouping of the power cables: single layer if they fit side by side, otherwise bunched
    if (nPower > 0) {
        const auto arr = set.choice("type") == "ladder" ? cables::Arrangement::Ladder : cables::Arrangement::PerforatedTray;
        const bool single = widthPower <= B * trays;
        t.set("f", cables::groupingFactor(single ? arr : cables::Arrangement::Bunched, nPower));
        if (!single) {
            t.info({"Energiekabel passen nicht nebeneinander – Häufungsfaktor für gebündelte Verlegung (mehrlagig).",
                    "Power cables do not fit side by side – grouping factor for bunched (multi-layer) installation."});
        }
    }
    if (const auto qp = set.maybe("qperm")) {
        if (F > *qp) {
            t.warn({"Kabellast über der zulässigen Last laut Belastungsdiagramm – Stützabstand verkleinern oder stärkere Rinne.",
                    "Cable load above the permissible load of the load diagram – reduce the support spacing or use a stronger tray."});
        } else {
            t.ok({"Kabellast ≤ zulässige Last laut Belastungsdiagramm.", "Cable load ≤ permissible load of the load diagram."});
        }
    }
    if (F > kVde0639 * B * H / 1000.0) {
        t.warn({"Last über dem Bemessungswert nach VDE 0639 (0,028 N/(m·mm²) × Nutzquerschnitt) – Herstellerangaben prüfen.",
                "Load above the design value per VDE 0639 (0.028 N/(m·mm²) × usable cross-section) – check manufacturer data."});
    }
    if (nPower > 0 && nData > 0 && set.choice("sep") == "no") {
        t.warn({"Energie- und Datenkabel gemeinsam: Trennsteg oder Abstand nach EN 50174-2 vorsehen.",
                "Power and data cables together: use a separator or distance per EN 50174-2."});
    }
    if (set.num("span") > 1.5) {
        t.info({"Stützabstand > 1,5 m: Tragfähigkeit und Durchbiegung im Belastungsdiagramm des Herstellers prüfen.",
                "Support spacing > 1.5 m: check load capacity and deflection in the manufacturer's load diagram."});
    }
    return t;
}

}  // namespace

ListDef cableTrayList() {
    ListDef l;
    l.id = "cable_tray";
    l.category = "projects";
    l.title = {"Kabeltrasse: Belegung, Breite und Last", "Cable tray: fill, width and load"};
    l.summary = {"Kabel eintragen → Trassenbreite mit Reserve, Füllgrad, Gewicht, Häufung", "Enter cables → tray width with reserve, fill, weight, grouping"};
    l.description = {
        "Platzbedarf je Kabel d² (Quadrat um den Kabelquerschnitt), Summe × (1 + Reserve); Richtwert Reserve ≈ 30 %.\n"
        "Trassenbreite = kleinste Normbreite (50 … 600 mm) mit B · H ≥ Bedarf; Füllgrad = Σ d² / (B · H).\n"
        "Kabellast q = Σ n · m' (kg/m), F = q · 9,81 N/kg; Bemessungslast nach VDE 0639: 0,028 N/(m·mm²) × B · H.\n"
        "Zulässige Last je Stützabstand aus dem Belastungsdiagramm des Herstellers eintragen (Stützabstand möglichst ≤ 1,5 m).\n"
        "Häufungsfaktor der Energiekabel nach DIN VDE 0298-4 / IEC 60364-5-52: einlagig auf gelochter Rinne bzw. Leiter, sonst gebündelt.\n"
        "Kabeldaten (d, Gewicht) aus Datenblättern: NYM (Faber), NYY-J (Waskönig+Walter), J-Y(St)Y und LiYCY (Lapp), Cat 7 "
        "(DIGITUS) – andere Typen mit d und Gewicht eintragen. Quelle Methode: OBO Bettermann KTS-Guide „Systemauswahl“.",
        "Space per cable d² (square around the cable), sum × (1 + reserve); guide value reserve ≈ 30 %.\n"
        "Tray width = smallest standard width (50 … 600 mm) with B · H ≥ demand; fill = Σ d² / (B · H).\n"
        "Cable load q = Σ n · m' (kg/m), F = q · 9.81 N/kg; design load per VDE 0639: 0.028 N/(m·mm²) × B · H.\n"
        "Enter the permissible load for the support spacing from the manufacturer's load diagram (spacing preferably ≤ 1.5 m).\n"
        "Grouping factor of the power cables per IEC 60364-5-52: single layer on perforated tray or ladder, otherwise bunched.\n"
        "Cable data (d, weight) from data sheets: NYM (Faber), NYY-J (Waskönig+Walter), J-Y(St)Y and LiYCY (Lapp), Cat 7 "
        "(DIGITUS) – enter d and weight for other types. Method: OBO Bettermann KTS guide “system selection”."};
    l.settingsInputs = {
        Num::choice("type", {"Trassenart", "Tray type"},
                    {{"tray", {"Kabelrinne / Gitterrinne (gelocht)", "Cable tray / wire mesh (perforated)"}},
                     {"ladder", {"Kabelleiter", "Cable ladder"}}},
                    "tray"),
        Num::choice("h", {"Seitenhöhe (Nutzhöhe)", "Side height (usable height)"},
                    {{"35", "35 mm"}, {"60", "60 mm"}, {"85", "85 mm"}, {"110", "110 mm"}}, "60"),
        Num::number("reserve", {"Platzreserve", "Space reserve"}, "%", 30).range(0, 200),
        Num::number("span", {"Stützabstand", "Support spacing"}, "m", 1.5).positive(),
        Num::number("qperm", {"Zulässige Last laut Belastungsdiagramm", "Permissible load from load diagram"}, "kN/m", kNaN).opt().positive()
            .withHint({"Herstellerwert für Rinne und Stützabstand, z. B. 0,9 kN/m", "Manufacturer value for tray and spacing, e.g. 0.9 kN/m"}),
        Num::choice("sep", {"Trennsteg Energie/Daten", "Separator power/data"}, {{"yes", {"ja", "yes"}}, {"no", {"nein", "no"}}}, "yes"),
    };
    l.rowInputs = {
        Num::text("name", {"Bezeichnung", "Designation"}, "Kabel").alias({"Bezeichnung", "Stromkreis", "Kabel-Nr."}),
        Num::text("cable", {"Kabeltyp", "Cable type"}, "NYM-J 5x2,5").alias({"Kabeltyp", "Typ", "Kabel"})
            .withHint({"z. B. NYM-J 5x2,5, NYY-J 4x95, J-Y(St)Y 2x2x0,8, LiYCY 4x0,75, Cat 7",
                       "e.g. NYM-J 5x2.5, NYY-J 4x95, J-Y(St)Y 2x2x0.8, LiYCY 4x0.75, Cat 7"}),
        Num::number("n", {"Anzahl", "Quantity"}, "", 1).range(1, 10000).alias({"Anzahl", "Stück"}),
        Num::number("d", {"Außendurchmesser (falls abweichend)", "Outer diameter (if different)"}, "mm", kNaN).opt().positive()
            .alias({"Durchmesser", "d"}),
        Num::number("g", {"Gewicht (falls abweichend)", "Weight (if different)"}, "kg/km", kNaN).opt().positive().alias({"Gewicht"}),
        Num::choice("kind", {"Art", "Kind"},
                    {{"auto", {"automatisch aus Typ", "automatic from type"}}, {"power", {"Energie", "power"}}, {"data", {"Daten/Steuerung", "data/control"}}},
                    "auto"),
    };
    l.rowOutputs = {Out::number("dd", {"d", "d"}, "mm", 1), Out::number("A", {"Platzbedarf n · d²", "Space n · d²"}, "mm²", 0),
                    Out::number("q", {"Last", "Load"}, "kg/m", 2), Out::number("sumd", {"Breite nebeneinander", "Width side by side"}, "mm", 0),
                    Out::text("kindOut", {"Art", "Kind"}), Out::text("type", {"Erkannt als", "Recognised as"})};
    l.totalOutputs = {Out::number("B", {"Trassenbreite (Normbreite)", "Tray width (standard)"}, "mm", 0).primary(),
                      Out::number("trays", {"Anzahl Trassen", "Number of trays"}, "", 0),
                      Out::number("fill", {"Füllgrad ohne Reserve", "Fill without reserve"}, "%", 1).primary(),
                      Out::number("fillRes", {"Belegung mit Reserve", "Occupancy with reserve"}, "%", 1),
                      Out::number("A", {"Platzbedarf Σ n · d²", "Space Σ n · d²"}, "mm²", 0),
                      Out::number("Areq", {"Bedarf mit Reserve", "Demand with reserve"}, "mm²", 0),
                      Out::number("q", {"Kabelgewicht", "Cable weight"}, "kg/m", 1),
                      Out::number("F", {"Kabellast je Trasse", "Cable load per tray"}, "kN/m", 3).primary(),
                      Out::number("Fmax", {"Bemessungslast VDE 0639", "Design load VDE 0639"}, "kN/m", 3),
                      Out::number("nPower", {"Energiekabel", "Power cables"}, "", 0),
                      Out::number("nData", {"Daten-/Steuerkabel", "Data/control cables"}, "", 0),
                      Out::number("widthPower", {"Breite Energiekabel nebeneinander", "Width of power cables side by side"}, "mm", 0),
                      Out::number("f", {"Häufungsfaktor Energiekabel", "Grouping factor power cables"}, "", 2)};
    l.computeRow = trayRow;
    l.summarize = traySummary;
    l.keywords = {"kabeltrasse", "kabelrinne", "trasse", "füllgrad", "belegung", "kabelleiter", "gitterrinne", "cable tray", "tray fill"};
    l.example =
        "name\tcable\tn\n"
        "Zuleitung UV-RLT\tNYY-J 5x16\t1\n"
        "Ventilatoren\tNYM-J 5x2,5\t4\n"
        "Pumpen\tNYM-J 5x1,5\t6\n"
        "Steckdosen\tNYM-J 3x2,5\t8\n"
        "Fühler\tJ-Y(St)Y 2x2x0,8\t20\n"
        "Bus KNX/Modbus\tJ-Y(St)Y 4x2x0,8\t2\n"
        "Netzwerk\tCat 7\t6\n"
        "Stellsignale\tLiYCY 4x0,75\t10\n";
    return l;
}

}  // namespace elc::catalog
