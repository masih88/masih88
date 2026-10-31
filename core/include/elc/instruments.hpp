#pragma once
// Sensors and switches for HVAC, building automation (MSR) and process automation with their
// identification letters: ANSI/ISA-5.1, EN 62424 (DIN 19227), VDI 3814-4.1 component codes, IEC 81346-2 class.

#include <string>
#include <string_view>
#include <vector>

#include "elc/text.hpp"

namespace elc::instruments {

enum class Function {
    Auto,         // from the entry (and words like "min", "Regler" in the search text)
    Element,      // sensor / primary element, e.g. Pt1000 (ISA E)
    Transmitter,  // transmitter with signal to the control system (ISA T)
    Indicator,    // local indicator, gauge, thermometer (ISA I)
    Controller,   // indicating controller (ISA IC)
    SwitchHigh,   // switch at high limit, e.g. pressure switch max (ISA SH)
    SwitchLow,    // switch at low limit, e.g. frost thermostat (ISA SL)
    AlarmHigh,    // alarm high (ISA AH)
    AlarmLow,     // alarm low (ISA AL)
    Safety,       // safety limiter / trip, e.g. STB (ISA SHH, EN 62424 Z)
    Totalizer,    // meter / totalizer (ISA FQI, JQI)
};

struct Entry {
    std::string id;
    Text name;
    std::string words;       // search words (lower case, ae/oe/ue/ss)
    std::string isa;         // ISA-5.1 measured variable incl. modifier, e.g. "T", "PD", "A"
    std::string en;          // EN 62424 / DIN 19227 measured variable incl. modifier; empty = no letter
    std::string annotation;  // added to the tag, e.g. "CH₄"
    Function fn;             // typical function of this device
    std::string vdi;         // VDI 3814-4.1 Tab. 3 component code(s), e.g. "TZU" or "TZU, TAB"
    std::string vdiFn;       // VDI 3814-4.1 Tab. 4 function code for binary signals, e.g. "GM_"
    std::string iec;         // IEC 81346-2 class (as in VDI 3814-4.1 Tab. 3), e.g. "BT"
    Text domain;
    Text note;
    std::string isaFixed;    // complete ISA letters for the typical function (special cases like "ZSO / ZSC")
    std::string enFixed;
};

[[nodiscard]] const std::vector<Entry>& all();

struct Match {
    const Entry* entry;
    int score;
};

/// Entries matching a free text like "Temperatur Zuluft", "Gas Methan", "Leckage" – best first, empty if none
[[nodiscard]] std::vector<Match> find(std::string_view text);

/// Function to use: the requested one, or (Auto) derived from words in the text and the entry's typical function
[[nodiscard]] Function resolve(const Entry& e, Function requested, std::string_view text);

/// ISA-5.1 letters, e.g. "TT", "PDT", "TSL", "FQI"
[[nodiscard]] std::string isaCode(const Entry& e, Function f);
/// EN 62424 / DIN 19227 letters, e.g. "TI", "TZH", "QI"; "–" if the variable has no letter there
[[nodiscard]] std::string enCode(const Entry& e, Function f);
/// Plain-language explanation of the ISA letters
[[nodiscard]] Text isaMeaning(const Entry& e, Function f);
/// Search text normalisation (lower case, umlauts → ae/oe/ue, subscripts → digits)
[[nodiscard]] std::string normalize(std::string_view text);

}  // namespace elc::instruments
