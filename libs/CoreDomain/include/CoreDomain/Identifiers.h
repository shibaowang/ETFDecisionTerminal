#pragma once

#include "CoreDomain/Result.h"

#include <cstdint>
#include <string>

namespace etfdt::core {

class Uuid final {
public:
    Uuid() = default;
    explicit Uuid(std::string value);

    [[nodiscard]] const std::string& raw() const noexcept;
    [[nodiscard]] bool isValidBasicFormat() const noexcept;

private:
    std::string value_;
};

class AccountId final {
public:
    constexpr AccountId() noexcept = default;

    [[nodiscard]] static constexpr AccountId fromRaw(std::int64_t value) noexcept
    {
        return AccountId(value);
    }

    [[nodiscard]] constexpr std::int64_t raw() const noexcept
    {
        return value_;
    }

    [[nodiscard]] constexpr bool isValid() const noexcept
    {
        return value_ > 0;
    }

private:
    explicit constexpr AccountId(std::int64_t value) noexcept
        : value_(value)
    {
    }

    std::int64_t value_ = 0;
};

class PortfolioId final {
public:
    constexpr PortfolioId() noexcept = default;

    [[nodiscard]] static constexpr PortfolioId fromRaw(std::int64_t value) noexcept
    {
        return PortfolioId(value);
    }

    [[nodiscard]] constexpr std::int64_t raw() const noexcept
    {
        return value_;
    }

    [[nodiscard]] constexpr bool isValid() const noexcept
    {
        return value_ > 0;
    }

private:
    explicit constexpr PortfolioId(std::int64_t value) noexcept
        : value_(value)
    {
    }

    std::int64_t value_ = 0;
};

class StrategyCode final {
public:
    StrategyCode() = default;

    [[nodiscard]] static Result<StrategyCode> fromString(const std::string& value);
    [[nodiscard]] const std::string& raw() const noexcept;
    [[nodiscard]] bool isValid() const noexcept;

private:
    explicit StrategyCode(std::string value);

    std::string value_;
};

class InstrumentCode final {
public:
    InstrumentCode() = default;

    [[nodiscard]] static Result<InstrumentCode> fromString(const std::string& value);
    [[nodiscard]] const std::string& raw() const noexcept;
    [[nodiscard]] bool isValid() const noexcept;

private:
    explicit InstrumentCode(std::string value);

    std::string value_;
};

}  // namespace etfdt::core
