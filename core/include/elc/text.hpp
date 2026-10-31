#pragma once
// Bilingual text (German/English) used everywhere in the core.

#include <string>
#include <utility>

namespace elc {

enum class Lang { De, En };

/// Text in German and English. Constructing from one string uses it for both languages.
struct Text {
    std::string de;
    std::string en;

    Text() = default;
    Text(const char* both) : de(both), en(both) {}  // NOLINT(google-explicit-constructor)
    Text(std::string both) : de(both), en(std::move(both)) {}  // NOLINT(google-explicit-constructor)
    Text(std::string german, std::string english) : de(std::move(german)), en(std::move(english)) {}

    [[nodiscard]] const std::string& in(Lang lang) const noexcept { return lang == Lang::De ? de : en; }
};

}  // namespace elc
