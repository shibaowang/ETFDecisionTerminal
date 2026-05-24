#pragma once

#include <cstdint>

namespace etfdt::quantization {

constexpr std::int64_t kPartsPerMillion = 1'000'000;
constexpr std::int64_t kPriceScale = 1'000'000;
constexpr std::int64_t kQuantityScale = 1'000'000;

}  // namespace etfdt::quantization
