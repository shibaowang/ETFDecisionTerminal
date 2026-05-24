#include "CoreDomain/Identifiers.h"

#include <cctype>
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

}  // namespace

Uuid::Uuid(std::string value)
    : value_(trimCopy(value))
{
}

const std::string& Uuid::raw() const noexcept
{
    return value_;
}

bool Uuid::isValidBasicFormat() const noexcept
{
    return !value_.empty();
}

StrategyCode::StrategyCode(std::string value)
    : value_(std::move(value))
{
}

Result<StrategyCode> StrategyCode::fromString(const std::string& value)
{
    std::string trimmed = trimCopy(value);
    if (trimmed.empty()) {
        return Result<StrategyCode>::failure("StrategyCode cannot be empty");
    }
    return Result<StrategyCode>::success(StrategyCode(std::move(trimmed)));
}

const std::string& StrategyCode::raw() const noexcept
{
    return value_;
}

bool StrategyCode::isValid() const noexcept
{
    return !value_.empty();
}

InstrumentCode::InstrumentCode(std::string value)
    : value_(std::move(value))
{
}

Result<InstrumentCode> InstrumentCode::fromString(const std::string& value)
{
    std::string trimmed = trimCopy(value);
    if (trimmed.empty()) {
        return Result<InstrumentCode>::failure("InstrumentCode cannot be empty");
    }
    return Result<InstrumentCode>::success(InstrumentCode(std::move(trimmed)));
}

const std::string& InstrumentCode::raw() const noexcept
{
    return value_;
}

bool InstrumentCode::isValid() const noexcept
{
    return !value_.empty();
}

}  // namespace etfdt::core
