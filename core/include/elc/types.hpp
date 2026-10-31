#pragma once
// Data model shared by the calculation core, the CLI and the Qt app.
// A calculator is pure data (inputs, outputs, texts) plus one compute function,
// so the UI can render every calculator with one generic page.

#include <functional>
#include <limits>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "elc/chart.hpp"
#include "elc/constants.hpp"
#include "elc/text.hpp"

namespace elc {

enum class NormProfile { DE, CH };

struct Option {
    std::string value;
    Text label;
};

/// Show an input only while another (choice) input has one of the given values.
struct VisibleIf {
    std::string inputId;
    std::vector<std::string> values;
};

struct InputDef {
    enum class Kind { Number, Choice, Text };

    std::string id;
    Text label;
    std::string unit;
    Kind kind = Kind::Number;
    double defaultValue = kNaN;  // NaN = field starts empty
    bool optional = false;
    double min = -kInf;
    double max = kInf;
    bool minExclusive = false;
    std::vector<Option> options;
    std::string defaultChoice;
    std::optional<VisibleIf> visibleIf;
    Text hint;
    std::vector<std::string> aliases;  // extra column names recognised when importing tables (e.g. Excel headers)
    Text section;                      // non-empty: this input starts a new group in the form

    static InputDef number(std::string id, Text label, std::string unit, double def);
    static InputDef choice(std::string id, Text label, std::vector<Option> options, std::string def = {});
    /// Free text (names, codes, rooms); stored like a choice value, never validated
    static InputDef text(std::string id, Text label, std::string def = {});

    InputDef& alias(std::vector<std::string> names) {
        aliases.insert(aliases.end(), names.begin(), names.end());
        return *this;
    }

    InputDef& positive() { min = 0.0; minExclusive = true; return *this; }
    InputDef& nonNegative() { min = 0.0; minExclusive = false; return *this; }
    InputDef& range(double lo, double hi) { min = lo; max = hi; minExclusive = false; return *this; }
    InputDef& rangeExclusiveMin(double lo, double hi) { min = lo; max = hi; minExclusive = true; return *this; }
    InputDef& opt() { optional = true; return *this; }
    InputDef& showIf(std::string inputId, std::vector<std::string> values) {
        visibleIf = VisibleIf{std::move(inputId), std::move(values)};
        return *this;
    }
    InputDef& withHint(Text h) { hint = std::move(h); return *this; }
    InputDef& startsSection(Text title) { section = std::move(title); return *this; }
};

struct OutputDef {
    std::string id;
    Text label;
    std::string unit;
    int decimals = 2;
    bool isText = false;
    bool key = false;  // highlighted in the UI and shown in the result bar
    Text shortLabel;   // compact label for list rows (falls back to label)

    static OutputDef number(std::string id, Text label, std::string unit, int decimals = 2);
    static OutputDef text(std::string id, Text label);

    OutputDef& primary() { key = true; return *this; }
    OutputDef& abbrev(Text s) { shortLabel = std::move(s); return *this; }
};

/// Input values keyed by input id. Missing numbers read as NaN, missing choices as "".
class Inputs {
public:
    void set(std::string id, double value) { numbers_[std::move(id)] = value; }
    void setChoice(std::string id, std::string value) { choices_[std::move(id)] = std::move(value); }

    [[nodiscard]] double num(std::string_view id) const;
    [[nodiscard]] bool has(std::string_view id) const;
    [[nodiscard]] const std::string& choice(std::string_view id) const;
    [[nodiscard]] const std::string& text(std::string_view id) const { return choice(id); }
    [[nodiscard]] std::optional<double> maybe(std::string_view id) const;

private:
    std::map<std::string, double, std::less<>> numbers_;
    std::map<std::string, std::string, std::less<>> choices_;
};

enum class Severity { Info, Ok, Warning, Error };

struct Message {
    Severity severity;
    Text text;
};

struct Result {
    std::map<std::string, double, std::less<>> values;
    std::map<std::string, Text, std::less<>> texts;
    std::map<std::string, std::string, std::less<>> units;  // overrides OutputDef::unit
    std::vector<Message> messages;

    void set(std::string id, double v) { values[std::move(id)] = v; }
    void set(std::string id, double v, std::string unit) {
        units[id] = std::move(unit);
        values[std::move(id)] = v;
    }
    void setText(std::string id, Text t) { texts[std::move(id)] = std::move(t); }
    void info(Text t) { messages.push_back({Severity::Info, std::move(t)}); }
    void ok(Text t) { messages.push_back({Severity::Ok, std::move(t)}); }
    void warn(Text t) { messages.push_back({Severity::Warning, std::move(t)}); }
    void error(Text t) { messages.push_back({Severity::Error, std::move(t)}); }
    [[nodiscard]] bool hasError() const;
};

struct Context {
    NormProfile norm = NormProfile::DE;
};

using ComputeFn = std::function<Result(const Inputs&, const Context&)>;

/// Optional diagram of a calculator, built from its (normalised) inputs and the computed result
using ChartFn = std::function<Chart(const Inputs&, const Result&, const Context&)>;
/// Optional: tapping at (x, y) in the chart changes these inputs (empty = nothing to set)
using PickFn = std::function<std::vector<PickValue>(const Inputs&, double x, double y, const Context&)>;

struct Calculator {
    std::string id;
    std::string category;
    Text title;
    Text summary;      // one line for lists
    Text description;  // formulas, norm references, assumptions
    std::vector<InputDef> inputs;
    std::vector<OutputDef> outputs;
    ComputeFn compute;
    std::vector<std::string> keywords;  // extra search terms
    ChartFn chart;
    PickFn pick;
};

struct Table {
    std::string id;
    std::string category;
    Text title;
    Text summary;
    Text description;
    std::vector<Text> columns;
    std::vector<std::vector<Text>> rows;
};

struct Category {
    std::string id;
    Text title;
    std::string badge;  // short symbol for the UI tile
};

/// Raised inside compute functions for a physically impossible or unsupported case.
class CalcError : public std::runtime_error {
public:
    explicit CalcError(Text text) : std::runtime_error(text.en), text_(std::move(text)) {}
    [[nodiscard]] const Text& text() const noexcept { return text_; }

private:
    Text text_;
};

/// True if the input is currently shown (its VisibleIf condition holds).
[[nodiscard]] bool isVisible(const InputDef& def, const Inputs& in);

/// Inputs pre-filled with the calculator's default values.
[[nodiscard]] Inputs defaultInputs(const Calculator& calc);

/// Applies choice defaults, validates visible numeric inputs, runs compute and turns
/// exceptions into error messages. Never throws for bad user input.
[[nodiscard]] Result evaluate(const Calculator& calc, const Inputs& in, const Context& ctx);

/// Chart of a calculator for the given inputs (nullopt if the calculator has none)
[[nodiscard]] std::optional<Chart> evaluateChart(const Calculator& calc, const Inputs& in, const Context& ctx);
/// Inputs to change when the user taps at (x, y) in the chart
[[nodiscard]] std::vector<PickValue> pickChart(const Calculator& calc, const Inputs& in, double x, double y, const Context& ctx);

/// Replaces unknown choice values by defaults and validates visible numeric inputs (error messages only).
[[nodiscard]] Result normalizeAndValidate(const std::vector<InputDef>& defs, Inputs& in);
/// Runs fn and turns CalcError/std::exception into an error Result.
[[nodiscard]] Result guardedCompute(const std::function<Result()>& fn);

/// Compact number formatting for messages, with decimal comma for German.
[[nodiscard]] std::string formatNumber(double v, int decimals, Lang lang);
[[nodiscard]] Text formatNumber(double v, int decimals);

/// Parses "2,5" or "2.5" independent of the C locale; nullopt for empty/invalid text.
[[nodiscard]] std::optional<double> parseNumber(std::string_view text);

}  // namespace elc
