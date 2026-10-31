#pragma once
// Project lists: many rows of the same kind (consumers, plant components) computed together,
// with totals, CSV export (Excel) and import from pasted tables.

#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "elc/types.hpp"

namespace elc {

using RowFn = std::function<Result(const Inputs& row, const Inputs& settings, const Context&)>;
using SummaryFn = std::function<Result(const std::vector<Inputs>& rows, const std::vector<Result>& results,
                                       const Inputs& settings, const Context&)>;
/// Optional detail export, e.g. one line per data point instead of per component
using ExpandFn = std::function<std::vector<std::vector<std::string>>(const std::vector<Inputs>& rows, const Inputs& settings,
                                                                     Lang lang)>;

struct ListDef {
    std::string id;
    std::string category;
    Text title;
    Text summary;
    Text description;
    std::vector<InputDef> settingsInputs;  // project-wide parameters
    std::vector<InputDef> rowInputs;       // first text input is used as row title
    std::vector<OutputDef> rowOutputs;
    std::vector<OutputDef> totalOutputs;
    RowFn computeRow;
    SummaryFn summarize;
    std::vector<Text> expandColumns;  // header of the detail export
    ExpandFn expand;
    Text expandTitle;
    std::vector<std::string> keywords;
    /// Example project as tab separated table (header = input ids), loaded via importTable
    std::string example;
};

[[nodiscard]] const std::vector<ListDef>& lists();
[[nodiscard]] const ListDef* findList(std::string_view id);

/// Inputs of one row/settings with defaults applied (like defaultInputs for calculators)
[[nodiscard]] Inputs defaultRow(const ListDef& def);
[[nodiscard]] Inputs defaultSettings(const ListDef& def);

/// Validates and computes one row (validation as for calculators)
[[nodiscard]] Result evaluateRow(const ListDef& def, const Inputs& row, const Inputs& settings, const Context& ctx);

struct ListResult {
    std::vector<Result> rows;
    Result totals;
};
[[nodiscard]] ListResult evaluateList(const ListDef& def, const std::vector<Inputs>& rows, const Inputs& settings,
                                      const Context& ctx);

/// Semicolon separated, decimal comma for German – opens directly in German Excel.
/// detail = true uses the expand export (e.g. data point list); sep = '\t' for pasting into a spreadsheet.
[[nodiscard]] std::string exportCsv(const ListDef& def, const std::vector<Inputs>& rows, const Inputs& settings,
                                    const Context& ctx, Lang lang, bool detail = false, char sep = ';');

/// Rows of the list's example project
[[nodiscard]] std::vector<Inputs> exampleRows(const ListDef& def);

/// Parses a pasted table (tab, semicolon or comma separated, first line = header).
/// Columns are matched by input id, label (DE/EN) or alias; choice values by value or label.
/// Unknown columns are ignored, missing ones get default values.
[[nodiscard]] std::vector<Inputs> importTable(const ListDef& def, std::string_view text);

}  // namespace elc
