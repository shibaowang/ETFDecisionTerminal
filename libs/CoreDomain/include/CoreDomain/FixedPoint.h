#pragma once

#include "CoreDomain/Result.h"

#include <cstdint>
#include <string>

namespace etfdt::core {

class MoneyCents final {
public:
    constexpr MoneyCents() noexcept = default;

    [[nodiscard]] static constexpr MoneyCents fromRaw(std::int64_t value) noexcept
    {
        return MoneyCents(value);
    }

    [[nodiscard]] static Result<MoneyCents> fromYuanString(const std::string& text);

    [[nodiscard]] constexpr std::int64_t raw() const noexcept
    {
        return value_;
    }

    [[nodiscard]] std::string toYuanString() const;

    MoneyCents& operator+=(MoneyCents other) noexcept;
    MoneyCents& operator-=(MoneyCents other) noexcept;

private:
    explicit constexpr MoneyCents(std::int64_t value) noexcept
        : value_(value)
    {
    }

    std::int64_t value_ = 0;
};

[[nodiscard]] constexpr MoneyCents operator+(MoneyCents lhs, MoneyCents rhs) noexcept
{
    return MoneyCents::fromRaw(lhs.raw() + rhs.raw());
}

[[nodiscard]] constexpr MoneyCents operator-(MoneyCents lhs, MoneyCents rhs) noexcept
{
    return MoneyCents::fromRaw(lhs.raw() - rhs.raw());
}

[[nodiscard]] constexpr bool operator==(MoneyCents lhs, MoneyCents rhs) noexcept
{
    return lhs.raw() == rhs.raw();
}

[[nodiscard]] constexpr bool operator!=(MoneyCents lhs, MoneyCents rhs) noexcept
{
    return !(lhs == rhs);
}

[[nodiscard]] constexpr bool operator<(MoneyCents lhs, MoneyCents rhs) noexcept
{
    return lhs.raw() < rhs.raw();
}

[[nodiscard]] constexpr bool operator<=(MoneyCents lhs, MoneyCents rhs) noexcept
{
    return lhs.raw() <= rhs.raw();
}

[[nodiscard]] constexpr bool operator>(MoneyCents lhs, MoneyCents rhs) noexcept
{
    return lhs.raw() > rhs.raw();
}

[[nodiscard]] constexpr bool operator>=(MoneyCents lhs, MoneyCents rhs) noexcept
{
    return lhs.raw() >= rhs.raw();
}

class Price1e6 final {
public:
    constexpr Price1e6() noexcept = default;

    [[nodiscard]] static constexpr Price1e6 fromRaw(std::int64_t value) noexcept
    {
        return Price1e6(value);
    }

    [[nodiscard]] static Result<Price1e6> fromDecimalString(const std::string& text);

    [[nodiscard]] constexpr std::int64_t raw() const noexcept
    {
        return value_;
    }

    [[nodiscard]] std::string toDecimalString() const;

private:
    explicit constexpr Price1e6(std::int64_t value) noexcept
        : value_(value)
    {
    }

    std::int64_t value_ = 0;
};

class Quantity1e6 final {
public:
    constexpr Quantity1e6() noexcept = default;

    [[nodiscard]] static constexpr Quantity1e6 fromRaw(std::int64_t value) noexcept
    {
        return Quantity1e6(value);
    }

    [[nodiscard]] static Result<Quantity1e6> fromDecimalString(const std::string& text);

    [[nodiscard]] constexpr std::int64_t raw() const noexcept
    {
        return value_;
    }

    [[nodiscard]] std::string toDecimalString() const;

private:
    explicit constexpr Quantity1e6(std::int64_t value) noexcept
        : value_(value)
    {
    }

    std::int64_t value_ = 0;
};

class RatioPpm final {
public:
    constexpr RatioPpm() noexcept = default;

    [[nodiscard]] static constexpr RatioPpm fromRaw(std::int64_t value) noexcept
    {
        return RatioPpm(value);
    }

    [[nodiscard]] static Result<RatioPpm> fromPercentString(const std::string& text);

    [[nodiscard]] constexpr std::int64_t raw() const noexcept
    {
        return value_;
    }

    [[nodiscard]] std::string toPercentString() const;

private:
    explicit constexpr RatioPpm(std::int64_t value) noexcept
        : value_(value)
    {
    }

    std::int64_t value_ = 0;
};

}  // namespace etfdt::core
