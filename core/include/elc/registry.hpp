#pragma once
// Access to all calculators, tables and categories of the app.

#include <string_view>
#include <utility>
#include <vector>

#include "elc/types.hpp"

namespace elc {

[[nodiscard]] const std::vector<Category>& categories();
[[nodiscard]] const std::vector<Calculator>& calculators();
[[nodiscard]] const std::vector<Table>& tables();

/// Groups shown on a category page (Category::groups, or all items of the category in one untitled group)
[[nodiscard]] std::vector<CategoryGroup> categoryGroups(std::string_view categoryId);

[[nodiscard]] const Calculator* findCalculator(std::string_view id);
[[nodiscard]] const Table* findTable(std::string_view id);

/// Current id of an item that was merged into another one (v1.9 clean-up), e.g. "motor_current" → "power";
/// unchanged ids are returned as they are. Used to keep stored favourites working.
[[nodiscard]] std::string_view currentItemId(std::string_view id);
/// Areas that replace a removed category id (v1.9: "process" → "pid", "atex"); empty if the id is unknown.
[[nodiscard]] std::vector<std::string_view> currentAreaIds(std::string_view id);

/// App version shown in the UI and CLI
[[nodiscard]] std::string_view version();

}  // namespace elc
