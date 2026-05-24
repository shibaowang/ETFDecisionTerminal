#include "CoreDomain/FixedPoint.h"

#include <cctype>
#include <limits>
#include <string_view>

namespace etfdt::core {
namespace {

std::string trimCopy(std::string_view value)
{
    auto first = value.begin();
    auto last = value.end();

    while (first != last
           && std::isspace(static_cast<unsigned char>(*first)) != 0) {
        ++first;
    }

    while (first != last
           && std::isspace(static_cast<unsigned char>(*(last - 1))) != 0) {
        --last;
    }

    return std::string(first, last);
}

Result<std::int64_t> parseFixedDecimal(
    std::string_view input,
    int fractionDigits,
    std::string_view typeName)
{
    const std::string text = trimCopy(input);
    if (text.empty()) {
        return Result<std::int64_t>::failure(std::string(typeName) + " cannot be empty");
    }

    std::size_t index = 0;
    bool negative = false;
    if (text[index] == '+' || text[index] == '-') {
        negative = text[index] == '-';
        ++index;
        if (index == text.size()) {
            return Result<std::int64_t>::failure(std::string(typeName) + " has no digits");
        }
    }

    bool sawDigit = false;
    bool sawDot = false;
    int parsedFractionDigits = 0;
    std::uint64_t magnitude = 0;
    const std::uint64_t maxMagnitude = negative
        ? static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1ULL
        : static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());

    for (; index < text.size(); ++index) {
        const char ch = text[index];
        if (ch == '.') {
            if (sawDot) {
                return Result<std::int64_t>::failure(
                    std::string(typeName) + " contains multiple decimal points");
            }
            sawDot = true;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(ch)) == 0) {
            return Result<std::int64_t>::failure(
                std::string(typeName) + " contains invalid character");
        }

        sawDigit = true;

        if (sawDot) {
            if (parsedFractionDigits >= fractionDigits) {
                return Result<std::int64_t>::failure(
                    std::string(typeName) + " has too many decimal places");
            }
            ++parsedFractionDigits;
        }

        const int digit = ch - '0';
        const auto unsignedDigit = static_cast<std::uint64_t>(digit);
        if (magnitude > (maxMagnitude - unsignedDigit) / 10) {
            return Result<std::int64_t>::failure(std::string(typeName) + " is out of range");
        }
        magnitude = magnitude * 10 + unsignedDigit;
    }

    if (!sawDigit) {
        return Result<std::int64_t>::failure(std::string(typeName) + " has no digits");
    }

    while (parsedFractionDigits < fractionDigits) {
        if (magnitude > maxMagnitude / 10) {
            return Result<std::int64_t>::failure(std::string(typeName) + " is out of range");
        }
        magnitude *= 10;
        ++parsedFractionDigits;
    }

    if (negative) {
        if (magnitude == maxMagnitude) {
            return Result<std::int64_t>::success(std::numeric_limits<std::int64_t>::min());
        }
        return Result<std::int64_t>::success(-static_cast<std::int64_t>(magnitude));
    }

    return Result<std::int64_t>::success(static_cast<std::int64_t>(magnitude));
}

std::uint64_t absoluteMagnitude(std::int64_t value)
{
    if (value >= 0) {
        return static_cast<std::uint64_t>(value);
    }
    return static_cast<std::uint64_t>(-(value + 1)) + 1U;
}

std::string formatFixedDecimal(std::int64_t value, int fractionDigits, bool trimTrailingZeros)
{
    const std::uint64_t scale = [&] {
        std::uint64_t result = 1;
        for (int i = 0; i < fractionDigits; ++i) {
            result *= 10;
        }
        return result;
    }();

    const std::uint64_t magnitude = absoluteMagnitude(value);
    const std::uint64_t integerPart = magnitude / scale;
    std::uint64_t fractionPart = magnitude % scale;

    std::string result;
    if (value < 0) {
        result.push_back('-');
    }

    result += std::to_string(integerPart);

    if (fractionDigits == 0) {
        return result;
    }

    std::string fraction = std::to_string(fractionPart);
    if (fraction.size() < static_cast<std::size_t>(fractionDigits)) {
        fraction.insert(fraction.begin(), static_cast<std::size_t>(fractionDigits) - fraction.size(), '0');
    }

    if (trimTrailingZeros) {
        while (!fraction.empty() && fraction.back() == '0') {
            fraction.pop_back();
        }
    }

    if (!fraction.empty() || !trimTrailingZeros) {
        result.push_back('.');
        result += fraction;
    }

    return result;
}

}  // namespace

Result<MoneyCents> MoneyCents::fromYuanString(const std::string& text)
{
    auto parsed = parseFixedDecimal(text, 2, "MoneyCents");
    if (!parsed) {
        return Result<MoneyCents>::failure(parsed.error());
    }
    return Result<MoneyCents>::success(MoneyCents::fromRaw(parsed.value()));
}

std::string MoneyCents::toYuanString() const
{
    return formatFixedDecimal(value_, 2, false);
}

MoneyCents& MoneyCents::operator+=(MoneyCents other) noexcept
{
    value_ += other.value_;
    return *this;
}

MoneyCents& MoneyCents::operator-=(MoneyCents other) noexcept
{
    value_ -= other.value_;
    return *this;
}

Result<Price1e6> Price1e6::fromDecimalString(const std::string& text)
{
    auto parsed = parseFixedDecimal(text, 6, "Price1e6");
    if (!parsed) {
        return Result<Price1e6>::failure(parsed.error());
    }
    return Result<Price1e6>::success(Price1e6::fromRaw(parsed.value()));
}

std::string Price1e6::toDecimalString() const
{
    return formatFixedDecimal(value_, 6, true);
}

Result<Quantity1e6> Quantity1e6::fromDecimalString(const std::string& text)
{
    auto parsed = parseFixedDecimal(text, 6, "Quantity1e6");
    if (!parsed) {
        return Result<Quantity1e6>::failure(parsed.error());
    }
    return Result<Quantity1e6>::success(Quantity1e6::fromRaw(parsed.value()));
}

std::string Quantity1e6::toDecimalString() const
{
    return formatFixedDecimal(value_, 6, true);
}

Result<RatioPpm> RatioPpm::fromPercentString(const std::string& text)
{
    const std::string trimmed = trimCopy(text);
    if (trimmed.empty() || trimmed.back() != '%') {
        return Result<RatioPpm>::failure("RatioPpm percent string must end with %");
    }

    auto parsed = parseFixedDecimal(
        std::string_view(trimmed.data(), trimmed.size() - 1),
        4,
        "RatioPpm");
    if (!parsed) {
        return Result<RatioPpm>::failure(parsed.error());
    }
    return Result<RatioPpm>::success(RatioPpm::fromRaw(parsed.value()));
}

std::string RatioPpm::toPercentString() const
{
    return formatFixedDecimal(value_, 4, true) + "%";
}

}  // namespace etfdt::core
