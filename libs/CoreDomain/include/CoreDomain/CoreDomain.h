#pragma once

#include <cstdint>
#include <string_view>

namespace etfdt::core {

struct MoneyCents final {
    std::int64_t value = 0;
};

struct Price1e6 final {
    std::int64_t value = 0;
};

struct Quantity1e6 final {
    std::int64_t value = 0;
};

constexpr std::string_view kTradeLogTable = "trade_log";

}  // namespace etfdt::core
