#pragma once

#include <algorithm>
#include <string>
#include <utility>

namespace etfdt::transport {

constexpr const char* kDefaultLocalSocketName = "ETFDataServiceDailyUse";

[[nodiscard]] inline bool isAllowedLocalSocketNameChar(char ch)
{
    // Keep QLocalServer / QLocalSocket names to [A-Za-z0-9_.-] across Windows and Unix.
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')
        || (ch >= '0' && ch <= '9') || ch == '_' || ch == '.' || ch == '-';
}

[[nodiscard]] inline std::string trimLocalSocketNameSeparators(std::string value)
{
    while (!value.empty() && (value.front() == '.' || value.front() == '-')) {
        value.erase(value.begin());
    }
    while (!value.empty() && (value.back() == '.' || value.back() == '-')) {
        value.pop_back();
    }
    return value;
}

[[nodiscard]] inline std::string normalizeLocalSocketName(
    const std::string& input,
    const std::string& fallback = kDefaultLocalSocketName)
{
    std::string normalized;
    normalized.reserve(input.size());
    bool previousSeparator = false;

    for (const char ch : input) {
        if (isAllowedLocalSocketNameChar(ch)) {
            normalized.push_back(ch);
            previousSeparator = false;
            continue;
        }
        if (!previousSeparator) {
            normalized.push_back('-');
            previousSeparator = true;
        }
    }

    normalized = trimLocalSocketNameSeparators(std::move(normalized));
    if (normalized.empty()) {
        return fallback.empty() ? kDefaultLocalSocketName : fallback;
    }
    return normalized;
}

[[nodiscard]] inline bool isNormalizedLocalSocketName(const std::string& input)
{
    return !input.empty()
        && std::all_of(input.begin(), input.end(), [](char ch) {
               return isAllowedLocalSocketNameChar(ch);
           });
}

}  // namespace etfdt::transport
