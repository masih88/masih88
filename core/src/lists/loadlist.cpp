// Project list: ELT consumer list with cable sizing.
// Calculation chain follows the user's Excel tool "Leistungsdaten_Kabelauslegung"
// (I_B → protective device → reduction factors → cross-section per DIN VDE 0298-4 → I_Z →
// check I_B ≤ I_N ≤ I_Z → voltage drop), columns after VDI 3814 Blatt 4.1 Tabelle 6.
// Improvements over the sheet: √3 instead of 1.73, R' from ρ(θ) (the sheet's "70 °C" column
// holds ≈ 20 °C values), I_2 ≤ 1.45·I_Z for LS/gG, optional cross-section increase for ΔU.
#include <array>
#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/cables.hpp"
#include "elc/lists.hpp"

namespace elc::catalog {

using namespace elc::cables;

namespace {

struct Supply {
    double U;        // V
    double kI;       // I = S / (kI · U)
    double kDrop;    // ΔU = kDrop · I · L · (R' cos φ + X' sin φ)
    int loaded;      // loaded conductors for the ampacity table
    bool dc;
};

Supply supplyFor(const std::string& v) {
    if (v == "230") return {230, 1.0, 2.0, 2, false};
    if (v == "24ac") return {24, 1.0, 2.0, 2, false};
    if (v == "24dc") return {24, 1.0, 2.0, 2, true};
    return {400, kSqrt3, kSqrt3, 3, false};
}

/// Reactance per length of multi-core cables [Ω/km] (values of the Excel lookup "NormWiderstände")
double reactancePerKm(double area) {
    static const std::map<double, double> x{{1.5, 0.115}, {2.5, 0.110}, {4, 0.107}, {6, 0.100},  {10, 0.094},
                                            {16, 0.090},  {25, 0.086},  {35, 0.083}, {50, 0.083}, {70, 0.082},
                                            {95, 0.082},  {120, 0.080}, {150, 0.080}, {185, 0.080}, {240, 0.079},
                                            {300, 0.079}};
    const auto it = x.lower_bound(area - 1e-9);
    return it == x.end() ? 0.079 : it->second;
}

std::string nhSize(double In) {
    if (In <= 63) return "D02 / NH00";
    if (In <= 160) return "NH00";
    if (In <= 250) return "NH1";
    if (In <= 400) return "NH2";
    return "NH3";
}

std::vector<Option> methodOpts() {
    return {{"A1", "A1"}, {"A2", "A2"}, {"B1", "B1"}, {"B2", "B2"},
            {"C", {"C – direkt auf Wand", "C – clipped direct"}}, {"D1", {"D1 – Erdreich", "D1 – ground"}},
            {"E", {"E – frei in Luft / Trasse", "E – free air / tray"}}};
}

Method parseM(const std::string& v) {
    if (v == "A1") return Method::A1;
    if (v == "A2") return Method::A2;
    if (v == "B1") return Method::B1;
    if (v == "B2") return Method::B2;
    if (v == "C") return Method::C;
    if (v == "D1") return Method::D1;
    return Method::E;
}

Arrangement parseArr(const std::string& v) {
    if (v == "wall") return Arrangement::SingleLayerWall;
    if (v == "tray") return Arrangement::PerforatedTray;
    if (v == "ladder") return Arrangement::Ladder;
    return Arrangement::Bunched;
}

std::vector<Option> areaChoice() {
    std::vector<Option> o{{"auto", {"automatisch", "automatic"}}};
    for (const auto& a : areaOptions(kAmpacityAreas)) o.push_back(a);
    return o;
}

Result computeConsumer(const Inputs& in, const Inputs& cfg, const Context&) {
    Result r;
    const auto sup = supplyFor(in.choice("U"));
    const double cosPhi = sup.dc ? 1.0 : in.num("cosphi");
    const double n = std::round(in.num("n"));
    const bool redundant = in.choice("red") == "ja";

    // power and current (per circuit, like the sheet: "pro Stromkreis, ohne Anzahl")
    const double S = in.num("P") / cosPhi;  // kVA
    const double Stot = redundant ? 0.0 : S * in.num("g") * n;
    const double IB = S * 1000.0 / (sup.kI * sup.U);
    r.set("S", S);
    r.set("Stot", Stot);
    r.set("IB", IB);

    // protective device
    const auto& dev = in.choice("dev");
    const bool adjustable = dev == "MSS" || dev == "SelMod";
    double In = IB;
    double I2factor = 1.0;
    if (!adjustable) {
        const Device d = dev == "gG" ? Device::FuseGG : (dev == "LS-B" ? Device::MCB_B : Device::MCB_C);
        const auto rating = nextAtLeast(ratedCurrents(d), IB);
        if (!rating) throw CalcError({"Betriebsstrom über der größten Normstufe.", "Design current above largest rating."});
        In = *rating;
        I2factor = conventionalTripFactor(d, In);
        r.setText("devinfo", dev == "gG" ? cat(Text{"gG "}, numText(In, 0, "A"), Text{" (" + nhSize(In) + ")"})
                                         : cat(Text{dev == "LS-B" ? "LS B" : "LS C"}, Text{" "}, numText(In, 0, "A")));
    } else {
        r.setText("devinfo", cat(Text{dev + " "}, Text{"einstellbar auf ", "set to "}, numText(IB, 2, "A")));
    }
    r.set("In", In);
    const double basis = adjustable ? IB : In;
    r.setText("basis", adjustable ? Text{"I_B"} : Text{"I_N"});

    // reduction factors
    const auto ins = in.choice("ins") == "xlpe" ? Insulation::XLPE : Insulation::PVC;
    const auto method = parseM(in.choice("method"));
    const double f1 = in.has("f1") ? in.num("f1") : ambientFactor(ins, method, in.num("Ta"));
    const double f2 = in.has("f2") ? in.num("f2")
                                   : groupingFactor(parseArr(in.choice("arr")), static_cast<int>(std::lround(in.num("nc"))));
    const double F = f1 * f2 * in.num("f3") * in.num("f4");
    if (F <= 0) throw CalcError({"Gesamtfaktor ist 0 – Temperatur prüfen.", "Total factor is 0 – check temperature."});
    r.set("F", F);
    const double IMW = basis / F;
    r.set("IMW", IMW);

    // cross-section by ampacity (DIN VDE 0298-4 Tab. 3/4 resp. IEC 60364-5-52)
    std::optional<double> Aamp;
    for (const double a : kAmpacityAreas) {
        if (baseAmpacity(ins, method, sup.loaded, a).value() >= IMW) {
            Aamp = a;
            break;
        }
    }
    if (!Aamp) throw CalcError({"Kein Querschnitt bis 300 mm² ausreichend – Parallelkabel.", "No cross-section up to 300 mm² – use parallel cables."});
    r.set("Aamp", *Aamp);

    // voltage drop helper
    const double rho = resistivity(Material::Cu, cfg.num("temp"));
    const double L = in.num("L");
    const auto dropPct = [&](double A) {
        const double R = rho / A;                              // Ω/m
        const double X = sup.dc ? 0.0 : reactancePerKm(A) / 1000.0;
        const double sinPhi = std::sqrt(std::max(0.0, 1.0 - cosPhi * cosPhi));
        return sup.kDrop * IB * L * (R * cosPhi + X * sinPhi) / sup.U * 100.0;
    };
    const double limit = cfg.num("dUmax");

    double A = *Aamp;
    if (in.choice("Aset") != "auto") {
        A = parseDouble(in.choice("Aset"));
    } else if (cfg.choice("grow") == "ja") {
        for (const double a : kAmpacityAreas) {
            if (a < *Aamp) continue;
            A = a;  // ends at 300 mm² if the limit cannot be met
            if (dropPct(a) <= limit) break;
        }
    }
    r.set("A", A);
    const double IR = baseAmpacity(ins, method, sup.loaded, A).value_or(kNaN);
    const double IZ = IR * F;
    r.set("IR", IR);
    r.set("IZ", IZ);

    const double dUp = dropPct(A);
    r.set("dU", dUp / 100.0 * sup.U);
    r.set("dUpct", dUp);

    // checks
    const bool ok1 = IB <= In + 1e-9 && In <= IZ + 1e-9;
    const bool ok2 = adjustable || I2factor * In <= 1.45 * IZ + 1e-9;
    r.setText("check", ok1 && ok2 ? Text{"i. O.", "OK"} : Text{"n. i. O.", "not OK"});
    if (!ok1) {
        r.error({"I_B ≤ I_N ≤ I_Z nicht erfüllt – Querschnitt erhöhen.", "I_B ≤ I_N ≤ I_Z not met – increase cross-section."});
    } else if (!ok2) {
        r.error({"I_2 > 1,45 · I_Z – Querschnitt erhöhen.", "I_2 > 1.45 · I_Z – increase cross-section."});
    }
    if (dUp > limit) {
        r.warn(cat(Text{"Spannungsfall ", "Voltage drop "}, numText(dUp, 2, "%"), Text{" > ", " > "}, numText(limit, 1, "%")));
    }
    if (A > *Aamp + 1e-9 && in.choice("Aset") == "auto") {
        r.info({"Querschnitt wegen Spannungsfall vergrößert.", "Cross-section increased for voltage drop."});
    }
    if (redundant) r.info({"Redundant: nicht in der Leistungsbilanz.", "Redundant: not in the power balance."});
    return r;
}

Result summarizeConsumers(const std::vector<Inputs>& rows, const std::vector<Result>& results, const Inputs&,
                          const Context&) {
    Result t;
    std::map<std::string, double> P, S;
    int nok = 0;
    double dUmax = 0;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const auto& in = rows[i];
        const auto& r = results[i];
        if (r.hasError()) ++nok;
        if (const auto it = r.values.find("dUpct"); it != r.values.end()) dUmax = std::max(dUmax, it->second);
        if (in.choice("red") == "ja") continue;
        const double p = in.num("P") * in.num("g") * std::round(in.num("n"));
        const auto sIt = r.values.find("Stot");
        const double s = sIt == r.values.end() ? 0.0 : sIt->second;
        for (const auto& key : {std::string("all"), "sup_" + in.choice("supply"), "conn_" + in.choice("conn"), "u_" + in.choice("U")}) {
            P[key] += p;
            S[key] += s;
        }
    }
    t.set("P", P["all"]);
    t.set("S", S["all"]);
    const double I = S["all"] * 1000.0 / (kSqrt3 * 400.0);
    t.set("I", I);
    if (const auto f = nextAtLeast(ratedCurrents(Device::FuseGG), I)) t.set("fuse", *f);
    t.set("P_NN", P["sup_NN"]);
    t.set("S_NN", S["sup_NN"]);
    t.set("P_NEA", P["sup_NEA"]);
    t.set("S_NEA", S["sup_NEA"]);
    t.set("P_USV", P["sup_USV"]);
    t.set("S_USV", S["sup_USV"]);
    t.set("S_ELT", S["conn_ELT"]);
    t.set("S_MSR", S["conn_MSR"]);
    t.set("S_400", S["u_400"]);
    t.set("S_230", S["u_230"]);
    t.set("S_24", S["u_24ac"] + S["u_24dc"]);
    t.set("rows", static_cast<double>(rows.size()));
    t.set("nok", nok);
    t.set("dUmax", dUmax);
    if (nok > 0) {
        t.error(cat(numText(nok, 0), Text{" Verbraucher nicht i. O.", " consumers not OK"}));
    } else if (!rows.empty()) {
        t.ok({"Alle Stromkreise i. O.", "All circuits OK"});
    }
    t.info({"Summen mit Gleichzeitigkeitsfaktor und Anzahl, ohne redundante Verbraucher. Gesamtstrom bezogen auf 400 V, "
            "symmetrische Aufteilung der 1~-Verbraucher (VDI 3814 Blatt 4.1, Tab. 6).",
            "Totals include simultaneity factor and quantity, without redundant consumers. Total current at 400 V, "
            "single-phase loads assumed balanced (VDI 3814 Part 4.1, Tab. 6)."});
    return t;
}

}  // namespace

ListDef consumerList() {
    ListDef l;
    l.id = "consumer_list";
    l.category = "projects";
    l.title = {"Verbraucherliste & Kabelauslegung", "Consumer list & cable sizing"};
    l.summary = {"Leistungsbilanz NN/NEA/USV, Schutzorgan, Querschnitt, ΔU je Verbraucher",
                 "Power balance NN/NEA/USV, protection, cross-section, ΔU per consumer"};
    l.description = {
        "Rechenkette je Verbraucher (wie Excel „Leistungsdaten_Kabelauslegung“):\n"
        "S = P / cos φ   I_B = S / (k · U), k = √3 (400 V) bzw. 1\n"
        "Schutzorgan: MSS/SelMod einstellbar (I_N = I_B, Auslegung auf I_B), LS/gG nächste Normstufe (Auslegung auf I_N)\n"
        "F_ges = f1 · f2 · f3 · f4   I_r,min = I_B bzw. I_N / F_ges → Querschnitt nach DIN VDE 0298-4 / IEC 60364-5-52\n"
        "I_Z = I_r · F_ges   Prüfung I_B ≤ I_N ≤ I_Z (LS/gG zusätzlich I_2 ≤ 1,45 · I_Z)\n"
        "ΔU = k · I_B · L · (R' cos φ + X' sin φ), k = √3 bzw. 2, R' = ρ(θ)/A, X' nach Querschnitt\n"
        "Spalten nach VDI 3814 Blatt 4.1 Tab. 6 (ELT-Verbraucherliste). f1/f2 leer = aus θ_a und Häufung berechnet.",
        "Chain per consumer (as in the Excel tool \"Leistungsdaten_Kabelauslegung\"):\n"
        "S = P / cos φ   I_B = S / (k · U), k = √3 (400 V) or 1\n"
        "Protection: MSS/SelMod adjustable (I_N = I_B, sized on I_B), MCB/gG next rating (sized on I_N)\n"
        "F_tot = f1 · f2 · f3 · f4   I_r,min = I_B or I_N / F_tot → cross-section per DIN VDE 0298-4 / IEC 60364-5-52\n"
        "I_Z = I_r · F_tot   check I_B ≤ I_N ≤ I_Z (MCB/gG also I_2 ≤ 1.45 · I_Z)\n"
        "ΔU = k · I_B · L · (R' cos φ + X' sin φ), k = √3 or 2, R' = ρ(θ)/A, X' by cross-section\n"
        "Columns per VDI 3814 Part 4.1 Tab. 6 (ELT consumer list). f1/f2 empty = calculated from θ_a and grouping."};
    l.settingsInputs = {
        Num::number("dUmax", {"Zul. Spannungsfall", "Permitted voltage drop"}, "%", 2.5).positive()
            .withHint({"Excel-Vorlage 2,5 %; VDE 0100-520 Anh. G 3/5 %; NIN 4 %", "Excel template 2.5 %; IEC 60364-5-52 Annex G 3/5 %"}),
        Num::number("temp", {"Leitertemperatur für ΔU", "Conductor temperature for ΔU"}, "°C", 70).range(20, 90),
        Num::choice("grow", {"Querschnitt für ΔU automatisch erhöhen", "Increase cross-section for ΔU automatically"},
                    {{"ja", {"ja", "yes"}}, {"nein", {"nein (wie Excel)", "no (as Excel)"}}}, "ja"),
    };
    l.rowInputs = {
        Num::text("name", {"Anlage/Gerät", "Plant/device"}, "Verbraucher").startsSection({"Kennzeichnung", "Identification"}).alias({"Anlage/Gerät", "Klartext", "Bezeichnung"}),
        Num::text("bas", {"BAS / Kennzeichnung", "BAS / identifier"}).alias({"BAS", "AKS"}),
        Num::text("trade", {"Gewerk", "Trade"}, "RLT").alias({"Gewerk"}),
        Num::text("system", {"System/Schaltschrank", "System/panel"}).alias({"System/Schaltschrank"}),
        Num::text("room", {"Aufstellort/Raum", "Location/room"}).alias({"Aufstellort/Raum", "Raum/Achsen", "Raum"}),
        Num::number("n", {"Anzahl", "Quantity"}, "", 1).range(1, 10000).startsSection({"Leistung & Versorgung", "Load & supply"}).alias({"Anzahl"}),
        Num::choice("conn", {"Anschluss ELT/MSR", "Connection ELT/MSR"}, {{"MSR", "MSR"}, {"ELT", "ELT"}}, "MSR")
            .alias({"Anschluss ELT/MSR"}),
        Num::choice("supply", {"Versorgung", "Supply"},
                    {{"NN", {"NN – Netz (AV)", "NN – mains (normal)"}}, {"NEA", {"NEA – Ersatzstrom (SV)", "NEA – standby generator"}},
                     {"USV", {"USV", "UPS"}}},
                    "NN")
            .alias({"NEA/USV/NN"}),
        Num::choice("U", {"Nennspannung", "Rated voltage"},
                    {{"400", "400 V 3~"}, {"230", "230 V 1~"}, {"24ac", "24 V AC"}, {"24dc", "24 V DC"}}, "400")
            .alias({"Nennspannung / V", "Nennspannung in V"}),
        Num::number("P", {"Nennleistung", "Rated power"}, "kW", 1.1).nonNegative().alias({"Nennleistung ASV / kW", "Nennleistung in kW"}),
        Num::number("cosphi", {"cos φ", "cos φ"}, "", 0.9).rangeExclusiveMin(0, 1).alias({"cos phi / -", "cos (φ)", "cos phi"}),
        Num::number("g", {"Gleichzeitigkeitsfaktor", "Simultaneity factor"}, "", 1).range(0, 1).alias({"Gleichzeitigkeitsfaktor"}),
        Num::choice("red", {"Redundant (Standby)", "Redundant (standby)"}, {{"nein", {"nein", "no"}}, {"ja", {"ja", "yes"}}}, "nein")
            .alias({"Redundant [Ja/Nein]"}),
        Num::choice("dev", {"Schutzeinrichtung", "Protective device"},
                    {{"MSS", {"MSS – Motorschutzschalter (einstellbar)", "MSS – motor protection (adjustable)"}},
                     {"SelMod", {"SelMod – elektronisch (einstellbar)", "SelMod – electronic (adjustable)"}},
                     {"LS-B", {"LS-Schalter B", "MCB type B"}},
                     {"LS-C", {"LS-Schalter C", "MCB type C"}},
                     {"gG", {"gG – Sicherung NH/D", "gG fuse NH/D"}}},
                    "MSS")
            .alias({"Typ Schutzeinrichtung"}),
        Num::choice("method", {"Verlegeart", "Installation method"}, methodOpts(), "E").startsSection({"Leitung & Verlegung", "Cable & installation"}).alias({"Verlegeart: C (Rohr), E (Trasse)", "Verlegeart"}),
        Num::choice("ins", {"Isolierung", "Insulation"}, {{"pvc", "PVC 70 °C"}, {"xlpe", {"VPE 90 °C", "XLPE 90 °C"}}}, "pvc"),
        Num::number("Ta", {"Umgebungstemperatur", "Ambient temperature"}, "°C", 30).range(-30, 85),
        Num::number("f1", {"f1 Temperatur (manuell)", "f1 temperature (manual)"}, "", kNaN).opt().rangeExclusiveMin(0, 1.5)
            .alias({"f1 Faktor Temperatur"}),
        Num::choice("arr", {"Häufung – Anordnung", "Grouping – arrangement"},
                    {{"bunched", {"gebündelt / Rohr", "bunched / conduit"}}, {"wall", {"einlagig Wand", "single layer wall"}},
                     {"tray", {"einlagig gelochte Rinne", "single layer perforated tray"}}, {"ladder", {"Kabelleiter", "ladder"}}},
                    "tray"),
        Num::number("nc", {"Anzahl Stromkreise in Häufung", "Grouped circuits"}, "", 1).range(1, 50),
        Num::number("f2", {"f2 Häufung (manuell)", "f2 grouping (manual)"}, "", kNaN).opt().rangeExclusiveMin(0, 1)
            .alias({"f2 Häufung"}),
        Num::number("f3", {"f3 Oberschwingungen", "f3 harmonics"}, "", 1).rangeExclusiveMin(0, 1).alias({"f3 Oberschwingungen"}),
        Num::number("f4", {"f4 weiterer Faktor", "f4 additional factor"}, "", 1).rangeExclusiveMin(0, 1).alias({"f4 Anz. bel. Adern"}),
        Num::number("L", {"Leitungslänge", "Cable length"}, "m", 30).positive().alias({"Länge Leitung / m"}),
        Num::choice("Aset", {"Querschnitt vorgeben", "Fixed cross-section"}, areaChoice(), "auto")
            .alias({"Selbst vorgegener Querschnitt 0 = Mindestquerschnitt verwenden / mm²", "Querschnitt vorgegeben"}),
    };
    l.rowOutputs = {Out::number("S", {"Anschlussleistung S", "Apparent power S"}, "kVA", 2),
                    Out::number("Stot", {"S mit Gzf, Anzahl", "S with simultaneity, qty"}, "kVA", 2),
                    Out::number("IB", {"Betriebsstrom I_B", "Design current I_B"}, "A", 2).primary().abbrev({"I_B", "I_B"}),
                    Out::number("In", {"Schutzeinrichtung I_N", "Protective device I_N"}, "A", 2),
                    Out::text("devinfo", {"Schutzorgan", "Device"}),
                    Out::text("basis", {"Auslegung auf", "Sized on"}),
                    Out::number("F", {"Gesamtfaktor F_ges", "Total factor F"}, "", 3),
                    Out::number("IMW", {"Mindest-Belastbarkeit I_r,min", "Min. rating I_r,min"}, "A", 2),
                    Out::number("Aamp", {"Querschnitt nach Belastbarkeit", "Area by ampacity"}, "mm²", 1),
                    Out::number("A", {"Gewählter Querschnitt", "Selected cross-section"}, "mm²", 1).primary().abbrev({"Querschnitt", "Size"}),
                    Out::number("IR", {"Tabellenwert I_r", "Table rating I_r"}, "A", 1),
                    Out::number("IZ", {"Strombelastbarkeit I_Z", "Current capacity I_Z"}, "A", 2),
                    Out::text("check", {"I_B ≤ I_N ≤ I_Z", "I_B ≤ I_N ≤ I_Z"}),
                    Out::number("dU", {"Spannungsfall ΔU", "Voltage drop ΔU"}, "V", 2),
                    Out::number("dUpct", {"Spannungsfall ΔU", "Voltage drop ΔU"}, "%", 2).primary().abbrev({"ΔU", "ΔU"})};
    l.totalOutputs = {Out::number("P", {"Wirkleistung gesamt", "Total active power"}, "kW", 2).primary(),
                      Out::number("S", {"Scheinleistung gesamt", "Total apparent power"}, "kVA", 2).primary(),
                      Out::number("I", {"Gesamtstrom (400 V)", "Total current (400 V)"}, "A", 1),
                      Out::number("fuse", {"Vorsicherung gG (Vorschlag)", "Upstream gG fuse (proposal)"}, "A", 0),
                      Out::number("S_NN", {"S Netz (NN/AV)", "S mains (NN)"}, "kVA", 2),
                      Out::number("S_NEA", {"S Ersatzstrom (NEA/SV)", "S standby (NEA)"}, "kVA", 2),
                      Out::number("S_USV", {"S USV", "S UPS"}, "kVA", 2),
                      Out::number("S_ELT", {"S Anschluss ELT", "S connection ELT"}, "kVA", 2),
                      Out::number("S_MSR", {"S Anschluss MSR", "S connection MSR"}, "kVA", 2),
                      Out::number("S_400", {"S 400 V", "S 400 V"}, "kVA", 2),
                      Out::number("S_230", {"S 230 V", "S 230 V"}, "kVA", 2),
                      Out::number("S_24", {"S 24 V", "S 24 V"}, "kVA", 3),
                      Out::number("rows", {"Anzahl Verbraucherzeilen", "Consumer rows"}, "", 0),
                      Out::number("nok", {"Davon nicht i. O.", "Thereof not OK"}, "", 0),
                      Out::number("dUmax", {"Größter Spannungsfall", "Largest voltage drop"}, "%", 2)};
    l.computeRow = computeConsumer;
    l.summarize = summarizeConsumers;
    l.keywords = {"verbraucherliste", "leistungsbilanz", "kabelliste", "kabelauslegung", "nshv", "usv", "nea", "excel"};
    l.example =
        "name\ttrade\tsystem\troom\tn\tconn\tsupply\tU\tP\tcosphi\tg\tred\tdev\tmethod\tnc\tL\n"
        "Ventilator Zuluft\tRLT\tISP-RLT01\tTechnikzentrale\t1\tMSR\tNEA\t230\t0.5\t0.9\t1\tnein\tMSS\tE\t3\t95\n"
        "Ventilator Abluft\tRLT\tISP-RLT01\tTechnikzentrale\t1\tMSR\tNEA\t230\t0.5\t0.9\t1\tnein\tMSS\tE\t3\t95\n"
        "Pumpe Heizung\tHZG\tISP-HZG01\tHeizzentrale\t1\tMSR\tNEA\t400\t3\t0.9\t1\tnein\tMSS\tE\t3\t60\n"
        "Radialschmiedeanlage\tTKW\tUV-Halle1\tHalle 1\t1\tELT\tNEA\t400\t15\t0.9\t1\tnein\tMSS\tE\t3\t80\n"
        "Hebeanlage Abwasser\tSAN\tISP-SAN01\tUG\t1\tMSR\tUSV\t230\t0.3\t0.9\t1\tnein\tMSS\tE\t2\t95\n"
        "Kältemaschine 1\tKLT\tNSHV\tDach\t1\tELT\tNN\t400\t45\t0.88\t1\tnein\tgG\tE\t2\t60\n"
        "Kältemaschine 2 (Reserve)\tKLT\tNSHV\tDach\t1\tELT\tNN\t400\t45\t0.88\t1\tja\tgG\tE\t2\t65\n"
        "Steckdosen Büro\tELT\tUV-1.OG\t1.OG\t6\tELT\tNN\t230\t3.68\t1\t0.3\tnein\tLS-B\tC\t1\t25\n"
        "Brandschutzklappen 24 V\tRLT\tISP-RLT01\tTechnikzentrale\t1\tMSR\tUSV\t24dc\t0.12\t1\t1\tnein\tLS-B\tE\t1\t40\n";
    return l;
}

}  // namespace elc::catalog
