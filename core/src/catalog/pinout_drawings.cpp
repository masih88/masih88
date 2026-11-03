// Pictures for the pinout tables (v1.10): connector faces or contact rows with the pins coloured by their function
// (supply, ground/screen, signal, unused) or by the real wire colours. The colours are taken from the table rows, so
// picture and table cannot disagree. Faces are drawn true to side only where the view is well documented (RJ plugs,
// D-sub, M12, DIN/MIDI, USB-C, OBD II, SIM, pin headers); otherwise the contact order is shown schematically and the
// note says so.
#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <numbers>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/sketch.hpp"

namespace elc::catalog {

namespace {

enum class Cls { Power, Ground, Signal, Free, Unknown };

const char* fillOf(Cls c) {
    switch (c) {
        case Cls::Power: return "fill:#E53935";
        case Cls::Ground: return "fill:#455A64";
        case Cls::Signal: return "fill:#1E88E5";
        case Cls::Free: return "fill:#ECEFF1";
        case Cls::Unknown: return "fill:#FF00FF";
    }
    return "fill:#FF00FF";
}

std::string lower(std::string s) {
    for (auto& ch : s) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return s;
}

std::string trim(const std::string& s) {
    const auto a = s.find_first_not_of(' ');
    if (a == std::string::npos) return {};
    const auto b = s.find_last_not_of(' ');
    return s.substr(a, b - a + 1);
}

/// true if `word` occurs in `t` as a whole word (ASCII letters/digits as word characters)
bool hasWord(const std::string& t, const std::string& word) {
    for (std::size_t p = t.find(word); p != std::string::npos; p = t.find(word, p + 1)) {
        const bool left = p == 0 || !std::isalnum(static_cast<unsigned char>(t[p - 1]));
        const std::size_t e = p + word.size();
        const bool right = e >= t.size() || !std::isalnum(static_cast<unsigned char>(t[e]));
        if (left && right) return true;
    }
    return false;
}

Cls classify(const std::string& text) {
    const std::string t = lower(trim(text));
    const auto has = [&](const char* k) { return t.find(k) != std::string::npos; };
    if (t.empty() || t == "–" || t == "– | –" || hasWord(t, "nc") || has("frei") || has("reserv") || has("nicht belegt"))
        return Cls::Free;
    if (hasWord(t, "gnd") || has("masse") || has("schirm") || has("shield") || hasWord(t, "vss") || hasWord(t, "vss1") ||
        hasWord(t, "vss2") || has("0 v)") || has("return"))
        return Cls::Ground;
    if (has("+5") || has("+12") || has("+3,3") || hasWord(t, "vcc") || hasWord(t, "vdd") || hasWord(t, "vbus") || has("pwr") ||
        has("l+") || has("versorgung") || has("dauerplus") || has("zündungsplus") || has("24 v") || t.rfind("5 v", 0) == 0 ||
        t.rfind("3,3 v", 0) == 0)
        return Cls::Power;
    return Cls::Signal;
}

/// Pin token match: "3", "1, 2", "2 / 3", "7–10", "A1–A3", "B1 / B2"
bool pinMatches(const std::string& cell, const std::string& id) {
    std::vector<std::string> tokens;
    std::string cur;
    for (std::size_t i = 0; i < cell.size(); ++i) {
        if (cell[i] == ',' || cell[i] == '/') {
            tokens.push_back(trim(cur));
            cur.clear();
        } else {
            cur += cell[i];
        }
    }
    tokens.push_back(trim(cur));
    const std::string dash = "–";
    const auto split = [](const std::string& s) {  // "A12" → {"A", 12}
        std::size_t k = 0;
        while (k < s.size() && !std::isdigit(static_cast<unsigned char>(s[k]))) ++k;
        int n = -1;
        if (k < s.size()) n = static_cast<int>(parseNumber(s.substr(k)).value_or(-1));
        return std::pair<std::string, int>{s.substr(0, k), n};
    };
    for (const auto& tok : tokens) {
        if (tok == id) return true;
        const auto d = tok.find(dash);
        if (d == std::string::npos) continue;
        const auto [p1, a] = split(trim(tok.substr(0, d)));
        const auto [p2, b] = split(trim(tok.substr(d + dash.size())));
        const auto [pi, n] = split(id);
        const std::string pfx2 = p2.empty() ? p1 : p2;
        if (a >= 0 && b >= 0 && n >= 0 && pi == p1 && pi == pfx2 && n >= a && n <= b) return true;
    }
    return false;
}

struct Lookup {
    std::size_t pinCol = 0;
    std::vector<std::size_t> textCols;  // empty = all other columns
    int filterCol = -1;
    std::string filter;
};

/// Text of the row that describes pin `id` (all describing cells joined); several "Pin" columns (pin, text, pin, text)
/// are searched pairwise.
std::optional<std::string> pinText(const Table& t, const std::string& id, const Lookup& lk) {
    std::vector<std::size_t> pinCols;
    for (std::size_t c = 0; c < t.columns.size(); ++c) {
        if (t.columns[c].de.rfind("Pin", 0) == 0) pinCols.push_back(c);
    }
    const bool pairs = pinCols.size() > 1 && lk.textCols.empty() && lk.filterCol < 0;
    for (const auto& row : t.rows) {
        if (lk.filterCol >= 0 && row[static_cast<std::size_t>(lk.filterCol)].de != lk.filter) continue;
        if (pairs) {
            for (const auto pc : pinCols) {
                if (pc + 1 < row.size() && pinMatches(row[pc].de, id)) return row[pc + 1].de;
            }
            continue;
        }
        if (!pinMatches(row[lk.pinCol].de, id)) continue;
        std::string s;
        for (std::size_t c = 0; c < row.size(); ++c) {
            const bool use = lk.textCols.empty() ? (c != lk.pinCol && static_cast<int>(c) != lk.filterCol)
                                                 : std::find(lk.textCols.begin(), lk.textCols.end(), c) != lk.textCols.end();
            if (use) s += (s.empty() ? "" : " | ") + row[c].de;
        }
        return s;
    }
    return std::nullopt;
}

/// Wire/plug colour by German colour name (first match wins, "weiß-x" = white with x stripe)
struct WireColour {
    std::string base;
    std::string stripe;  // empty = plain
};
std::optional<std::string> hexOf(const std::string& name) {
    static const std::vector<std::pair<const char*, const char*>> m{
        {"schiefergrau", "#708090"}, {"orange", "#FB8C00"}, {"grün", "#43A047"}, {"blau", "#1E88E5"}, {"braun", "#8D6E63"},
        {"rot", "#E53935"},          {"schwarz", "#212121"}, {"gelb", "#FDD835"}, {"grau", "#9E9E9E"}, {"violett", "#8E24AA"},
        {"rosa", "#F48FB1"},         {"aqua", "#26C6DA"},    {"weiß", "#FFFFFF"}};
    const std::string l = lower(name);
    std::size_t best = std::string::npos;
    std::optional<std::string> hex;
    for (const auto& [k, v] : m) {  // the colour named first in the text wins ("weiß (oder schwarz)" = white)
        const auto pos = l.find(k);
        if (pos != std::string::npos && pos < best) {
            best = pos;
            hex = v;
        }
    }
    return hex;
}
WireColour wireColour(const std::string& name) {
    const std::string l = lower(name);
    const std::string white = "weiß-";
    if (l.rfind(white, 0) == 0) return {"#FFFFFF", hexOf(l.substr(white.size())).value_or("#FF00FF")};
    return {hexOf(l).value_or("#FF00FF"), {}};
}

const Text kNoteSchematic{"Kontaktfolge schematisch (nicht seitenrichtig) – Pin-Nummern am Isolierkörper bzw. im Datenblatt prüfen.",
                          "Contact order shown schematically (not true to side) – check pin numbers on the insulator or in the datasheet."};

/// Drawing context: sketch plus pin classification from the table
struct D {
    const Table& t;
    Lookup lk;
    Sketch s;
    std::set<Cls> used;

    Cls cls(const std::string& id) {
        const auto txt = pinText(t, id, lk);
        const Cls c = txt ? classify(*txt) : Cls::Unknown;
        used.insert(c);
        return c;
    }
    void pad(const std::string& id, double x0, double y0, double x1, double y1) {
        const Cls c = cls(id);
        s.rect(fillOf(c), x0, y0, x1, y1);
        if (c == Cls::Unknown) s.label((x0 + x1) / 2, (y0 + y1) / 2, {"?", "?"});
    }
    void dot(const std::string& id, double x, double y, double r) {
        const Cls c = cls(id);
        s.circle(fillOf(c), x, y, r);
        if (c == Cls::Unknown) s.label(x, y, {"?", "?"});
    }
    void num(const std::string& n, double x, double y, const char* style = "c") { s.label(x, y, {n, n}, style); }
    void wirePad(const WireColour& w, double x0, double y0, double x1, double y1) {
        s.rect("fill:" + w.base, x0, y0, x1, y1);
        if (!w.stripe.empty()) {
            const double c = (x0 + x1) / 2, h = (x1 - x0) * 0.22;
            s.rect("fill:" + w.stripe, c - h, y0, c + h, y1);
        }
    }
    /// Legend of the used pin classes in one row at height y
    void legend(double y) {
        struct Item {
            Cls c;
            Text t;
        };
        const std::vector<Item> items{{Cls::Power, {"Versorgung", "Supply"}},
                                      {Cls::Ground, {"Masse/Schirm", "Ground/screen"}},
                                      {Cls::Signal, {"Signal/Daten", "Signal/data"}},
                                      {Cls::Free, {"frei", "unused"}}};
        double x = 4;
        for (const auto& it : items) {
            if (!used.contains(it.c)) continue;
            s.rect(fillOf(it.c), x, y - 1.6, x + 3.2, y + 1.6);
            s.label(x + 4.6, y, it.t, "l");
            x += 25;
        }
    }
};

std::string str(int n) { return std::to_string(n); }

// ------------------------------------------------------------------ RJ modular plugs

/// Modular plug seen from the contact side (lying on the latch, cable towards the viewer): pin 1 on the left.
/// colours(i) returns the fill of contact i (1-based); extra labels above the numbers optional.
Chart modularPlug(D& d, int positions, const std::function<WireColour(int)>& colours, const Text& note,
                  const std::vector<std::string>& extra = {}) {
    const double pitch = 6.4;
    const double width = positions * pitch + 6.0;
    const double x0 = 50.0 - width / 2;
    const double h = extra.empty() ? 64.0 : 70.0;
    d.s.frame(100, h, h / 100.0);
    d.s.rect("fill:#90A4AE", 41, 3, 59, 16);                      // cable
    d.s.rect("fill:#E3E8EE", x0, 16, x0 + width, 56);            // plug body
    d.s.rect("fill:#CFD8DC", x0 + 2, 18, x0 + width - 2, 38);    // strain relief / body
    for (int i = 1; i <= positions; ++i) {
        const double cx = x0 + 3.0 + pitch * (i - 0.5);
        d.wirePad(colours(i), cx - 1.8, 41, cx + 1.8, 54);
        d.num(str(i), cx, 59.5);
        if (!extra.empty()) d.num(extra[static_cast<std::size_t>(i - 1)], cx, 65.5, "cb");
    }
    d.s.ch.note = note;
    return d.s.ch;
}

Chart rj45(const Table& t, bool poe) {
    D d{t, {}, {}, {}};
    const std::size_t colourCol = poe ? 1 : 2;  // T568B colour
    const auto colours = [&](int i) {
        for (const auto& row : t.rows) {
            if (row[0].de == str(i)) return wireColour(row[colourCol].de);
        }
        return WireColour{"#FF00FF", {}};
    };
    if (poe) {
        return modularPlug(d, 8, colours,
                           {"RJ45-Stecker von oben: Rastnase unten, Kontakte zum Betrachter, Kabel unten – Pin 1 links. Aderfarben T568B; "
                            "A = Speisung Modus A (Datenpaare 1/2, 3/6), B = Modus B (Paare 4/5, 7/8).",
                            "RJ45 plug from the top: latch down, contacts towards the viewer, cable at the bottom – pin 1 on the left. "
                            "Wire colours T568B; A = power mode A (data pairs 1/2, 3/6), B = mode B (pairs 4/5, 7/8)."},
                           {"A", "A", "A", "B", "B", "A", "B", "B"});
    }
    return modularPlug(d, 8, colours,
                       {"RJ45-Stecker von oben: Rastnase unten, Kontakte zum Betrachter, Kabel unten – Pin 1 links. Aderfarben "
                        "T568B (T568A: grünes und oranges Paar getauscht).",
                        "RJ45 plug from the top: latch down, contacts towards the viewer, cable at the bottom – pin 1 on the left. "
                        "Wire colours T568B (T568A: green and orange pair swapped)."});
}

Chart rjTelephone(const Table& t) {
    D d{t, {}, {}, {}};
    // lines as in the table: 3/4 line 1, 2/5 line 2, 1/6 line 3
    const auto colours = [](int i) {
        if (i == 3 || i == 4) return WireColour{"#1E88E5", {}};
        if (i == 2 || i == 5) return WireColour{"#FB8C00", {}};
        return WireColour{"#43A047", {}};
    };
    Chart ch = modularPlug(d, 6, colours,
                           {"6-poliger Stecker (RJ11/14/25) von oben, Rastnase unten – Pin 1 links. Blau = Leitung 1 (3/4), orange = "
                            "Leitung 2 (2/5), grün = Leitung 3 (1/6). RJ9 (4P) und RJ48C (8P) werden ebenso von links gezählt.",
                            "6-position plug (RJ11/14/25) from the top, latch down – pin 1 on the left. Blue = line 1 (3/4), orange = "
                            "line 2 (2/5), green = line 3 (1/6). RJ9 (4P) and RJ48C (8P) are counted from the left as well."});
    return ch;
}

// ------------------------------------------------------------------ D-sub

/// D-sub shell (trapezoid, wide side up) around the given rows
void dsubShell(Sketch& s, double xl, double xr, double yb, double yt) {
    const double inset = (yt - yb) * 0.22;
    s.closed("fill:#E3E8EE", {{xl, yt}, {xr, yt}, {xr - inset, yb}, {xl + inset, yb}});
}

Chart dsub9(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 64, 0.64);
    dsubShell(d.s, 14, 86, 14, 52);
    for (int i = 0; i < 5; ++i) {  // male face: 1–5 top, left to right
        const double x = 26 + 12 * i;
        d.dot(str(i + 1), x, 40, 3.2);
        d.num(str(i + 1), x, 47.5);
    }
    for (int i = 0; i < 4; ++i) {
        const double x = 32 + 12 * i;
        d.dot(str(i + 6), x, 26, 3.2);
        d.num(str(i + 6), x, 18.5);
    }
    d.legend(5);
    d.s.ch.note = {"D-Sub 9 Stecker (Stifte, am PC/DTE), Ansicht auf die Stifte: Pin 1 oben links. Die Buchse ist spiegelbildlich "
                   "(Pin 1 oben rechts).",
                   "D-sub 9 male (pins, on the PC/DTE), view onto the pins: pin 1 top left. The female is mirrored (pin 1 top right)."};
    return d.s.ch;
}

Chart vga(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 66, 0.66);
    dsubShell(d.s, 16, 84, 12, 54);
    // female face (graphics card): rows 5…1, 10…6, 15…11 from left to right
    for (int i = 0; i < 5; ++i) {
        const double x = 28 + 11 * i;
        const int top = 5 - i, mid = 10 - i, bot = 15 - i;
        d.dot(str(top), x, 44, 2.6);
        d.num(str(top), x, 49.8);
        d.dot(str(mid), x + 5.5, 33, 2.6);
        d.num(str(mid), x + 5.5 + 4.6, 33, "l");
        d.dot(str(bot), x, 22, 2.6);
        d.num(str(bot), x, 16.2);
    }
    d.legend(5);
    d.s.ch.note = {"VGA-Buchse (DE-15, an der Grafikkarte), Ansicht auf die Steckseite: Pin 1 oben rechts. Der Stecker am Kabel ist "
                   "spiegelbildlich (Pin 1 oben links).",
                   "VGA female (DE-15, on the graphics card), view onto the mating side: pin 1 top right. The cable plug is mirrored "
                   "(pin 1 top left)."};
    return d.s.ch;
}

// ------------------------------------------------------------------ circular connectors

/// M12 A-coded face, `male` = view onto the pins (counter-clockwise numbering), else view onto the socket (clockwise)
void m12Face(D& d, double cx, double cy, bool male, const std::function<WireColour(int)>& colour, bool five) {
    d.s.circle("fill:#E3E8EE", cx, cy, 17);
    d.s.circle("ink", cx, cy, 13.5);
    d.s.rect("fill:#90A4AE", cx - 2, cy + 13.5, cx + 2, cy + 17);  // coding key at the top
    const double r = 8.0, a = std::numbers::pi / 4;
    // positions: upper right, upper left, lower left, lower right
    const double px[4] = {cx + r * std::cos(a), cx - r * std::cos(a), cx - r * std::cos(a), cx + r * std::cos(a)};
    const double py[4] = {cy + r * std::sin(a), cy + r * std::sin(a), cy - r * std::sin(a), cy - r * std::sin(a)};
    // male: 1 upper right, 2 upper left, 3 lower left, 4 lower right; female mirrored
    const int maleOrder[4] = {1, 2, 3, 4};
    const int femaleOrder[4] = {2, 1, 4, 3};
    for (int k = 0; k < 4; ++k) {
        const int pin = male ? maleOrder[k] : femaleOrder[k];
        const auto w = colour(pin);
        d.s.circle("fill:" + w.base, px[k], py[k], 2.4);
        const double lx = cx + (px[k] - cx) * 1.55, ly = cy + (py[k] - cy) * 1.55;
        d.num(str(pin), lx, ly, "cb");
    }
    if (five) {
        d.s.circle("fill:" + colour(5).base, cx, cy, 2.4);
        d.num("5", cx + 4.2, cy, "lb");
    }
}

Chart m12(const Table& t, bool ioLink) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 54, 0.54);
    // wire colours per IEC 60947-5-2: 1 brown, 2 white, 3 blue, 4 black, 5 grey
    const auto colour = [](int pin) {
        static const char* c[6] = {"#FF00FF", "#8D6E63", "#FFFFFF", "#1E88E5", "#212121", "#9E9E9E"};
        return WireColour{c[pin < 1 || pin > 5 ? 0 : pin], {}};
    };
    m12Face(d, 27, 28, true, colour, true);
    m12Face(d, 73, 28, false, colour, true);
    d.s.label(27, 5, {"Stecker (Stifte), z. B. Sensor", "Male (pins), e.g. sensor"});
    d.s.label(73, 5, {"Buchse, z. B. Kabeldose", "Female, e.g. cable socket"});
    d.s.ch.note = ioLink ? Text{"M12 A-codiert, Ansicht auf die Steckseite, Kodierung oben. Aderfarben nach IEC 60947-5-2: 1 braun, 2 weiß, "
                                "3 blau, 4 schwarz (C/Q), 5 grau. 4-polig ohne Pin 5.",
                                "M12 A-coded, view onto the mating side, coding key at the top. Wire colours per IEC 60947-5-2: 1 brown, "
                                "2 white, 3 blue, 4 black (C/Q), 5 grey. 4-pin without pin 5."}
                         : Text{"M12 A-codiert, Ansicht auf die Steckseite, Kodierung oben: Stecker gegen den Uhrzeigersinn, Buchse im "
                                "Uhrzeigersinn nummeriert. Farben = Aderfarben (IEC 60947-5-2). 4-polig ohne Pin 5.",
                                "M12 A-coded, view onto the mating side, coding key at the top: male numbered counter-clockwise, female "
                                "clockwise. Colours = wire colours (IEC 60947-5-2). 4-pin without pin 5."};
    return d.s.ch;
}

Chart midi(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 58, 0.58);
    const double cx = 50, cy = 26;
    d.s.circle("fill:#E3E8EE", cx, cy, 21);
    d.s.circle("ink", cx, cy, 17);
    d.s.rect("fill:#90A4AE", cx - 3, cy - 21, cx + 3, cy - 16);  // key at the bottom
    // female socket, facing the holes: 1-4-2-5-3 from left to right along the upper half circle (180°)
    const int order[5] = {1, 4, 2, 5, 3};
    for (int k = 0; k < 5; ++k) {
        const double ang = std::numbers::pi - k * std::numbers::pi / 4;
        const double x = cx + 11 * std::cos(ang), y = cy + 11 * std::sin(ang);
        d.dot(str(order[k]), x, y, 2.4);
        d.num(str(order[k]), cx + 25 * std::cos(ang), cy + 25 * std::sin(ang), "cb");
    }
    d.legend(-3);
    d.s.ch.y.min = -6;
    d.s.ch.aspect = 64.0 / 100.0;
    d.s.ch.note = {"DIN-Buchse 5-polig 180° (MIDI IN/OUT am Gerät), Ansicht auf die Löcher: von links 1–4–2–5–3, Nut unten. "
                   "Am Stecker ist die Folge spiegelbildlich (Wikipedia „DIN connector“).",
                   "5-pin 180° DIN socket (MIDI IN/OUT on the device), facing the holes: 1–4–2–5–3 from the left, key at the bottom. "
                   "Mirrored on the plug (Wikipedia “DIN connector”)."};
    return d.s.ch;
}

// ------------------------------------------------------------------ rows and grids of contacts

/// Rows of rectangular contacts in numeric order (schematic) inside a shell; row `staggerRow` is shifted by half a
/// pitch (interleaved rows as in HDMI, DisplayPort, SCART), −1 = none
Chart contactRows(D& d, const std::vector<std::vector<std::string>>& rows, double pitch, const Text& note, bool numbersAll = true,
                  int staggerRow = -1) {
    const bool stagger = staggerRow >= 0;
    std::size_t n = 0;
    for (const auto& r : rows) n = std::max(n, r.size());
    const double width = static_cast<double>(n) * pitch + 8 + (stagger ? pitch / 2 : 0);
    const double x0 = 50 - width / 2;
    const double rowH = rows.size() > 2 ? 15 : 12;
    const double yb = 12, yt = yb + rowH * static_cast<double>(rows.size());
    d.s.frame(100, yt + 7, (yt + 7) / 100.0);
    d.s.rect("fill:#E3E8EE", x0, yb, x0 + width, yt);
    for (std::size_t ri = 0; ri < rows.size(); ++ri) {
        const double yc = yt - rowH * (static_cast<double>(ri) + 0.5);
        const double shift = static_cast<int>(ri) == staggerRow ? pitch / 2 : 0;
        for (std::size_t i = 0; i < rows[ri].size(); ++i) {
            const auto& id = rows[ri][i];
            const double cx = x0 + 4 + shift + pitch * (static_cast<double>(i) + 0.5);
            d.pad(id, cx - pitch * 0.32, yc - 3.2, cx + pitch * 0.32, yc + 3.2);
            const bool show = numbersAll || i == 0 || i + 1 == rows[ri].size() || (std::stoi(id) % 5 == 0);
            if (!show) continue;
            const bool last = ri + 1 == rows.size();
            const double ly = ri == 0 ? yt + 3.5 : last ? yb - 3.5 : yc - rowH * 0.42;
            d.num(id, cx, ly);
        }
    }
    d.legend(2.5);
    d.s.ch.note = note;
    return d.s.ch;
}

std::vector<std::string> seq(int from, int to, int step = 1, const std::string& prefix = "") {
    std::vector<std::string> v;
    for (int i = from; step > 0 ? i <= to : i >= to; i += step) v.push_back(prefix + str(i));
    return v;
}

Chart usbC(const Table& t) {
    D d{t, {}, {}, {}};
    const double pitch = 6.6;
    d.s.frame(100, 52, 0.52);
    d.s.rect("fill:#E3E8EE", 6, 12, 94, 44);   // shell
    d.s.rect("fill:#CFD8DC", 9, 23, 91, 33);  // tongue
    for (int i = 1; i <= 12; ++i) {
        const double x = 50 + (i - 6.5) * pitch;
        d.pad("A" + str(i), x - 2, 29, x + 2, 32.5);
        d.num("A" + str(i), x, 38.5);
        const int b = 13 - i;  // bottom row B12 … B1 from left to right
        d.pad("B" + str(b), x - 2, 23.5, x + 2, 27);
        d.num("B" + str(b), x, 17.5);
    }
    d.legend(5);
    d.s.ch.note = {"USB-C-Buchse, Ansicht auf die Steckseite: oben A1…A12, unten B12…B1 (USB Type-C Specification). Der Stecker ist "
                   "drehbar – die Belegung ist punktsymmetrisch.",
                   "USB-C receptacle, view onto the mating side: top A1…A12, bottom B12…B1 (USB Type-C specification). The plug is "
                   "reversible – the assignment is point-symmetric."};
    return d.s.ch;
}

Chart obd2(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 54, 0.54);
    d.s.closed("fill:#E3E8EE", {{8, 42}, {92, 42}, {86, 14}, {14, 14}});
    for (int i = 0; i < 8; ++i) {
        const double x = 18.5 + 9 * i;
        const std::string top = str(i + 1), bot = str(i + 9);
        const bool tUsed = pinText(t, top, {}).has_value(), bUsed = pinText(t, bot, {}).has_value();
        if (tUsed) d.pad(top, x - 2.6, 31, x + 2.6, 37); else { d.s.rect(fillOf(Cls::Free), x - 2.6, 31, x + 2.6, 37); d.used.insert(Cls::Free); }
        if (bUsed) d.pad(bot, x - 2.6, 19, x + 2.6, 25); else { d.s.rect(fillOf(Cls::Free), x - 2.6, 19, x + 2.6, 25); d.used.insert(Cls::Free); }
        d.num(top, x, 46.5);
        d.num(bot, x, 9.5);
    }
    d.legend(3);
    d.s.ch.note = {"Diagnosebuchse im Fahrzeug (SAE J1962), Ansicht auf die Steckseite: oben 1–8, unten 9–16 von links. Nicht "
                   "genormte Pins sind herstellerspezifisch (hier „frei“).",
                   "Diagnostic socket in the vehicle (SAE J1962), view onto the mating side: top 1–8, bottom 9–16 from the left. Pins "
                   "not standardised are manufacturer-specific (shown as unused)."};
    return d.s.ch;
}

Chart sim(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 60, 0.6);
    d.s.rect("fill:#F5E6A8", 16, 8, 84, 54);  // contact plate
    for (int i = 0; i < 4; ++i) {
        const double y = 46 - 10.5 * i;
        d.pad("C" + str(i + 1), 31, y - 3.6, 46, y + 3.6);
        d.num("C" + str(i + 1), 28.5, y, "rb");
        d.pad("C" + str(i + 5), 54, y - 3.6, 69, y + 3.6);
        d.num("C" + str(i + 5), 71.5, y, "lb");
    }
    d.legend(1.5);
    d.s.ch.note = {"Kontaktfeld nach ISO/IEC 7816-2, Blick auf die Kontakte: links C1–C4, rechts C5–C8 (C1 VCC gegenüber C5 GND). "
                   "Gleiche Lage bei Mini-, Micro- und Nano-SIM.",
                   "Contact field per ISO/IEC 7816-2, view onto the contacts: left C1–C4, right C5–C8 (C1 VCC opposite C5 GND). Same "
                   "layout for mini, micro and nano SIM."};
    return d.s.ch;
}

Chart raspberryPi(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 34, 0.34);
    d.s.rect("fill:#2E7D32", 2, 9, 98, 27);  // board edge strip
    const double pitch = 4.7;
    for (int i = 0; i < 20; ++i) {
        const double x = 5.3 + pitch * i;
        const int odd = 2 * i + 1, even = 2 * i + 2;
        d.pad(str(even), x - 1.6, 20, x + 1.6, 23.2);  // outer row (board edge): even pins
        if (odd == 1) {
            d.s.rect("line:#FDD835", x - 2.1, 12.3, x + 2.1, 16.9);  // square pad marks pin 1
        }
        d.pad(str(odd), x - 1.6, 13, x + 1.6, 16.2);
        if (i % 2 == 0 || i == 19) {
            d.num(str(even), x, 30);
            d.num(str(odd), x, 5.5);
        }
    }
    d.legend(-3.5);
    d.s.ch.y.min = -6;
    d.s.ch.aspect = 40.0 / 100.0;
    d.s.ch.note = {"Draufsicht, Stiftleiste am oberen Platinenrand: innere Reihe ungerade (Pin 1 links, quadratisches Lötauge, gelb "
                   "markiert), äußere Reihe gerade Pins.",
                   "Top view, header at the upper board edge: inner row odd (pin 1 on the left, square pad, marked yellow), outer row "
                   "even pins."};
    return d.s.ch;
}

Chart lcd1602(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 40, 0.4);
    d.s.rect("fill:#2E7D32", 2, 16, 98, 30);
    static const char* names[16] = {"VSS", "VDD", "V0", "RS", "RW", "E", "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "A", "K"};
    for (int i = 0; i < 16; ++i) {
        const double x = 5.6 + 5.9 * i;
        d.dot(str(i + 1), x, 24, 1.8);
        d.num(str(i + 1), x, 34.5);
        d.num(names[i], x, 11.5);
    }
    d.legend(3.5);
    d.s.ch.note = {"Anschlussleiste des Moduls (Pin 1 = VSS, auf der Platine markiert). A/K = Hintergrundbeleuchtung.",
                   "Header of the module (pin 1 = VSS, marked on the PCB). A/K = backlight."};
    return d.s.ch;
}

// ------------------------------------------------------------------ schematic pictures

Chart usb2(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 50, 0.5);
    const char* name[4] = {"VBUS +5 V", "D−", "D+", "GND"};
    for (int i = 0; i < 4; ++i) {
        const double y = 42 - 9 * i;
        WireColour w = wireColour(t.rows[static_cast<std::size_t>(i)][2].de);
        d.s.rect("fill:" + w.base, 30, y - 1.6, 92, y + 1.6);
        d.num(str(i + 1), 24, y, "rb");
        d.s.label(6, y, {name[i], name[i]}, "l");
    }
    d.s.label(62, 6, {"Micro-B: 4 = ID, 5 = GND", "Micro-B: 4 = ID, 5 = GND"});
    d.s.ch.note = {"Adern im USB-2.0-Kabel mit Pin-Nummer (Typ A/B). Micro-/Mini-B haben zusätzlich ID (Pin 4) und GND auf Pin 5.",
                   "Cores in the USB 2.0 cable with pin number (type A/B). Micro/mini-B add ID (pin 4) and GND on pin 5."};
    return d.s.ch;
}

Chart knx(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 46, 0.46);
    d.s.rect("fill:#43A047", 4, 6, 30, 40);  // green KNX cable sheath
    for (std::size_t i = 0; i < t.rows.size() && i < 4; ++i) {
        const double y = 34 - 9 * static_cast<double>(i);
        const auto w = wireColour(t.rows[i][0].de);
        d.s.rect("fill:" + w.base, 30, y - 1.5, 50, y + 1.5);
        d.s.label(52, y, t.rows[i][1], "l");
    }
    d.s.ch.note = {"KNX-Busleitung: rot = Bus +, schwarz = Bus −; gelb/weiß Reserve (z. B. zweite Linie oder Zusatzspannung). "
                   "Busklemme rot/dunkelgrau steckbar.",
                   "KNX bus cable: red = bus +, black = bus −; yellow/white spare (e.g. second line or auxiliary supply). Pluggable "
                   "red/dark-grey bus terminal."};
    return d.s.ch;
}

Chart rs485(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 50, 0.5);
    const double y1 = 36, y0 = 28, yc = 18;
    d.s.line("line:#1E88E5", {{22, y1}, {94, y1}});
    d.s.line("line:#FB8C00", {{22, y0}, {94, y0}});
    d.s.line("line:#455A64", {{22, yc}, {94, yc}});
    d.s.label(3, y1, {"D1 (B)", "D1 (B)"}, "l");
    d.s.label(3, y0, {"D0 (A)", "D0 (A)"}, "l");
    d.s.label(3, yc, {"Common", "Common"}, "l");
    for (const double x : {26.0, 90.0}) {  // termination at both ends
        d.s.bar("bgfill", {x, y1}, {x, y0}, 2.6);
        d.s.label(x, 23, {"120 Ω", "120 Ω"});
    }
    for (int k = 0; k < 4; ++k) {  // devices in a line (daisy chain)
        const double x = 38 + 13 * k;
        d.s.rect("fill:#E3E8EE", x - 4.5, 41, x + 4.5, 49);
        d.s.label(x, 45, {str(k + 1), str(k + 1)}, "cb");
        d.s.line("ink", {{x - 2.5, 41}, {x - 2.5, y1}});
        d.s.line("ink", {{x, 41}, {x, y0}});
        d.s.line("ink", {{x + 2.5, 41}, {x + 2.5, yc}});
    }
    d.s.ch.note = {"RS-485 als Linie (keine Stichleitungen), 120-Ω-Abschluss an beiden Enden, Common mitführen. Polarität: im "
                   "Ruhezustand D1 > D0.",
                   "RS-485 as a daisy chain (no stubs), 120 Ω termination at both ends, run the common. Polarity: in idle state D1 > D0."};
    return d.s.ch;
}

Chart xlr(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 46, 0.46);
    struct W {
        double y;
        const char* c;
        Text l;
    };
    const std::vector<W> w{{36, "#E53935", {"Pin 2 – heiß (+) / DMX Daten −", "pin 2 – hot (+) / DMX data −"}},
                           {26, "#1E88E5", {"Pin 3 – kalt (−) / DMX Daten +", "pin 3 – cold (−) / DMX data +"}},
                           {16, "#455A64", {"Pin 1 – Masse/Schirm", "pin 1 – ground/screen"}}};
    d.s.rect("fill:#90A4AE", 4, 10, 20, 42);
    for (const auto& x : w) {
        d.s.rect(std::string("fill:") + x.c, 20, x.y - 1.6, 38, x.y + 1.6);
        d.s.label(40, x.y, x.l, "l");
    }
    d.s.ch.note = {"Symmetrische Leitung (verdrilltes Paar mit Schirm): Pin 2 heiß, Pin 3 kalt, Pin 1 Schirm (AES14). Pin-Nummern sind "
                   "am XLR-Einsatz eingeprägt.",
                   "Balanced line (twisted pair with screen): pin 2 hot, pin 3 cold, pin 1 screen (AES14). Pin numbers are moulded on "
                   "the XLR insert."};
    return d.s.ch;
}

Chart jack(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 40, 0.4);
    // TRRS plug side view: sleeve, ring 2, ring 1, tip from left to right, separated by insulating rings
    d.s.rect("fill:#90A4AE", 2, 14, 22, 26);  // handle
    struct Seg {
        double x0, x1;
        Text name;
    };
    const std::vector<Seg> segs{{22, 46, {"Schaft (Sleeve)", "Sleeve"}}, {48, 60, {"Ring 2", "Ring 2"}}, {62, 74, {"Ring 1", "Ring 1"}}};
    for (const auto& s : segs) {
        d.s.rect("fill:#CFD8DC", s.x0, 16, s.x1, 24);
        d.s.label((s.x0 + s.x1) / 2, 30, s.name);
    }
    for (const double x : {46.0, 60.0, 74.0}) d.s.rect("fill:#212121", x, 16.5, x + 2, 23.5);
    d.s.closed("fill:#CFD8DC", {{76, 16}, {86, 16}, {92, 20}, {86, 24}, {76, 24}});
    d.s.label(84, 30, {"Spitze (Tip)", "Tip"});
    d.s.ch.note = {"Klinkenstecker TRRS (Seitenansicht). Bei TRS fehlt Ring 2, bei TS auch Ring 1. Belegung je Variante siehe Tabelle.",
                   "TRRS jack plug (side view). TRS has no ring 2, TS no ring 1 either. Assignment per variant see table."};
    return d.s.ch;
}

Chart rca(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 34, 0.34);
    const double pitch = 92.0 / static_cast<double>(t.rows.size());
    static const std::vector<Text> shortNames{{"Video", "Video"}, {"L", "L"},           {"R / Pr", "R / Pr"}, {"Y", "Y"},
                                              {"Pb", "Pb"},       {"S/PDIF", "S/PDIF"}, {"Sub", "Sub"}};
    for (std::size_t i = 0; i < t.rows.size(); ++i) {
        const double x = 4 + pitch * (static_cast<double>(i) + 0.5);
        const auto w = wireColour(t.rows[i][0].de);
        d.s.circle("fill:" + w.base, x, 20, 5.2);
        d.s.circle("fill:#CFD8DC", x, 20, 1.8);
        d.s.label(x, 8.5, i < shortNames.size() ? shortNames[i] : t.rows[i][1]);
    }
    d.s.ch.note = {"Farben der Cinch-Stecker (Ansicht von vorn).", "Colours of the RCA plugs (front view)."};
    return d.s.ch;
}

Chart fibre(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 30, 0.3);
    const double pitch = 92.0 / static_cast<double>(t.rows.size());
    for (std::size_t i = 0; i < t.rows.size(); ++i) {
        const double x = 4 + pitch * (static_cast<double>(i) + 0.5);
        const auto w = wireColour(t.rows[i][1].de);
        d.s.rect("fill:" + w.base, x - pitch * 0.38, 9, x + pitch * 0.38, 22);
        d.num(t.rows[i][0].de, x, 26);
    }
    d.s.ch.note = {"Faserfarben 1–12 nach TIA-598 (ab Faser 13 Wiederholung mit Ring oder Strich).",
                   "Fibre colours 1–12 per TIA-598 (from fibre 13 repeated with a ring or dash)."};
    return d.s.ch;
}

Chart led(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 56, 0.56);
    // 5 mm LED: body with dome and flat side (cathode, right), long leg = anode (left)
    std::vector<Sketch::P> body{{20, 30}, {20, 44}};
    for (int k = 0; k <= 12; ++k) {
        const double a = std::numbers::pi - k * std::numbers::pi / 12;
        body.push_back({30 + 10 * std::cos(a), 44 + 10 * std::sin(a)});
    }
    body.push_back({38, 30});
    d.s.closed("fill:#EF9A9A", body);
    d.s.rect("fill:#EF9A9A", 18, 27, 38, 30);  // collar, flat at the right = cathode
    d.s.line("line:#9E9E9E", {{26, 27}, {26, 4}});   // anode, long
    d.s.line("line:#9E9E9E", {{34, 27}, {34, 12}});  // cathode, short
    d.s.label(24, 4, {"Anode (+)", "anode (+)"}, "r");
    d.s.label(36, 12, {"Kathode (−)", "cathode (−)"}, "l");
    d.s.label(40, 30, {"flache Seite", "flat side"}, "l");
    // diode symbol
    const double y = 34;
    d.s.line("ink", {{58, y}, {70, y}});
    d.s.closed("inkfill", {{70, y + 6}, {70, y - 6}, {80, y}});
    d.s.line("ink", {{80, y + 6}, {80, y - 6}});
    d.s.line("ink", {{80, y}, {94, y}});
    d.s.line("ink", {{76, y + 8}, {80, y + 12}});
    d.s.line("ink", {{80, y + 8}, {84, y + 12}});
    d.s.label(60, y + 5, {"A +", "A +"}, "cb");
    d.s.label(92, y + 5, {"K −", "K −"}, "cb");
    d.s.ch.note = {"Bedrahtete LED: langes Bein und runde Seite = Anode, kurzes Bein und abgeflachte Seite = Kathode.",
                   "Through-hole LED: long leg and round side = anode, short leg and flat side = cathode."};
    return d.s.ch;
}

Chart molexSata(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 62, 0.62);
    // Molex 4-pin: wire colours from the table
    d.s.label(4, 56, {"Molex 4-polig", "Molex 4-pin"}, "lb");
    for (int i = 1; i <= 4; ++i) {
        const double x = 46 + 11 * (i - 1);
        std::string colourName = "–";
        for (const auto& row : t.rows) {
            if (row[0].de == "Molex 4-polig" && row[1].de == str(i)) colourName = row[2].de;
        }
        const auto open = colourName.find('('), close = colourName.find(')');
        const auto w = wireColour(open != std::string::npos ? colourName.substr(open + 1, close - open - 1) : colourName);
        d.s.circle("fill:" + w.base, x, 56, 3.2);
        d.num(str(i), x, 50);
    }
    D data{t, {1, {2}, 0, "SATA-Daten"}, {}, {}};
    D power{t, {1, {2}, 0, "SATA-Strom"}, {}, {}};
    d.s.label(4, 38, {"SATA-Daten 7-polig", "SATA data 7-pin"}, "lb");
    for (int i = 1; i <= 7; ++i) {
        const double x = 46 + 6.5 * (i - 1);
        const Cls c = data.cls(str(i));
        d.used.insert(c);
        d.s.rect(fillOf(c), x - 2, 35, x + 2, 41);
        d.num(str(i), x, 31);
    }
    d.s.label(4, 20, {"SATA-Strom 15-polig", "SATA power 15-pin"}, "lb");
    for (int i = 1; i <= 15; ++i) {
        const double x = 41 + 3.8 * (i - 1);
        const Cls c = power.cls(str(i));
        d.used.insert(c);
        d.s.rect(fillOf(c), x - 1.3, 17, x + 1.3, 23);
        if (i == 1 || i % 5 == 0) d.num(str(i), x, 13);
    }
    d.legend(4);
    d.s.ch.note = {"Molex: Farben = Adern (gelb +12 V, schwarz Masse, rot +5 V). SATA: Kontaktfolge schematisch; Masse-Kontakte "
                   "sind voreilend.",
                   "Molex: colours = wires (yellow +12 V, black ground, red +5 V). SATA: contact order schematic; ground contacts "
                   "mate first."};
    return d.s.ch;
}

Chart iso10487(const Table& t) {
    D d{t, {}, {}, {}};
    d.s.frame(100, 52, 0.52);
    for (int blk = 0; blk < 2; ++blk) {
        const std::string p = blk == 0 ? "A" : "B";
        const double x0 = blk == 0 ? 8 : 54;
        d.s.rect("fill:#E3E8EE", x0, 12, x0 + 38, 40);
        d.s.label(x0 + 19, 46, {blk == 0 ? "A – Strom" : "B – Lautsprecher", blk == 0 ? "A – power" : "B – speakers"}, "cb");
        for (int i = 0; i < 4; ++i) {
            const double x = x0 + 6 + 8.7 * i;
            const std::string odd = p + str(2 * i + 1), even = p + str(2 * i + 2);
            d.pad(odd, x - 2.4, 29, x + 2.4, 35);
            d.num(odd, x, 25.5);
            d.pad(even, x - 2.4, 15, x + 2.4, 21);
            d.num(even, x, 9.5);
        }
    }
    d.legend(2);
    d.s.ch.note = kNoteSchematic;
    return d.s.ch;
}

}  // namespace

std::optional<Chart> tableDrawing(const Table& t) {
    const auto& id = t.id;
    if (id == "pin_rj45") return rj45(t, false);
    if (id == "pin_poe") return rj45(t, true);
    if (id == "pin_rj_telephone") return rjTelephone(t);
    if (id == "pin_rs232") return dsub9(t);
    if (id == "pin_vga") return vga(t);
    if (id == "pin_m12") return m12(t, false);
    if (id == "pin_io_link") return m12(t, true);
    if (id == "pin_midi") return midi(t);
    if (id == "pin_thunderbolt") return usbC(t);
    if (id == "pin_obd2") return obd2(t);
    if (id == "pin_sim_card") return sim(t);
    if (id == "pin_raspberry_pi") return raspberryPi(t);
    if (id == "pin_lcd1602") return lcd1602(t);
    if (id == "pin_usb") return usb2(t);
    if (id == "pin_knx") return knx(t);
    if (id == "pin_rs485") return rs485(t);
    if (id == "pin_xlr") return xlr(t);
    if (id == "pin_jack") return jack(t);
    if (id == "rca_colours") return rca(t);
    if (id == "fiber_colours") return fibre(t);
    if (id == "pin_led") return led(t);
    if (id == "pin_molex_sata") return molexSata(t);
    if (id == "pin_iso10487") return iso10487(t);
    // schematic contact rows
    D d{t, {}, {}, {}};
    if (id == "pin_hdmi") return contactRows(d, {seq(1, 19, 2), seq(2, 18, 2)}, 8.4, kNoteSchematic, true, 1);
    if (id == "pin_displayport") return contactRows(d, {seq(1, 19, 2), seq(2, 20, 2)}, 8.4, kNoteSchematic, true, 1);
    if (id == "pin_scart") return contactRows(d, {seq(2, 20, 2), seq(1, 21, 2)}, 7.6, kNoteSchematic, true, 0);
    if (id == "pin_dvi") {
        auto r3 = seq(17, 24);
        return contactRows(d, {seq(1, 8), seq(9, 16), r3, {"C1", "C2", "C3", "C4", "C5"}}, 10.0, kNoteSchematic);
    }
    if (id == "pin_firewire") {
        D f{t, {0, {2}, -1, {}}, {}, {}};
        return contactRows(f, {seq(1, 6)}, 11.0, kNoteSchematic);
    }
    if (id == "pin_lightning") return contactRows(d, {seq(1, 8)}, 10.0, kNoteSchematic);
    if (id == "pin_apple_dock") return contactRows(d, {seq(1, 30)}, 3.0, kNoteSchematic, false);
    if (id == "pin_ps2") return contactRows(d, {seq(1, 6)}, 11.0, kNoteSchematic);
    if (id == "pin_sd_card") {
        D sd{t, {0, {2}, -1, {}}, {}, {}};
        return contactRows(sd, {{"9", "1", "2", "3", "4", "5", "6", "7", "8"}}, 9.0,
                           {"SD-Karte, Kontaktfolge: Pin 9 liegt neben Pin 1 (nachträglich ergänzt). microSD: 8 Kontakte, eigene Nummerierung "
                            "(siehe Tabelle).",
                            "SD card, contact order: pin 9 sits next to pin 1 (added later). microSD: 8 contacts, own numbering (see table)."});
    }
    return std::nullopt;
}

}  // namespace elc::catalog
