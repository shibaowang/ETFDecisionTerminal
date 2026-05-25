#pragma once

#include "Protocol/Protocol.h"

#include <optional>
#include <string>
#include <utility>

namespace etfdt::shell_services {

struct ShellDataError final {
    std::optional<etfdt::protocol::ErrorCode> errorCode;
    std::string message;
};

template <typename T>
class ShellDataResult final {
public:
    static ShellDataResult success(T value)
    {
        return ShellDataResult(std::move(value));
    }

    static ShellDataResult failure(
        std::optional<etfdt::protocol::ErrorCode> errorCode,
        std::string message)
    {
        return ShellDataResult(ShellDataError{errorCode, std::move(message)});
    }

    [[nodiscard]] bool hasValue() const noexcept
    {
        return hasValue_;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return hasValue();
    }

    [[nodiscard]] const T& value() const
    {
        return value_;
    }

    [[nodiscard]] T& value()
    {
        return value_;
    }

    [[nodiscard]] const ShellDataError& error() const noexcept
    {
        return error_;
    }

private:
    explicit ShellDataResult(T value)
        : hasValue_(true)
        , value_(std::move(value))
    {
    }

    explicit ShellDataResult(ShellDataError error)
        : hasValue_(false)
        , error_(std::move(error))
    {
    }

    bool hasValue_ = false;
    T value_{};
    ShellDataError error_;
};

struct ShellDataConnectionState final {
    bool disconnected = true;
    bool connecting = false;
    bool connected = false;
    bool unhealthy = false;
    std::string lastError;
    std::string socketName;
};

struct ShellDataResponse final {
    bool success = false;
    std::string payloadJson;
    std::optional<etfdt::protocol::ErrorCode> errorCode;
    std::string errorMessage;
};

struct ShellReadOnlyDataSnapshot final {
    bool healthy = false;
    std::string summaryJson;
    int accountCount = 0;
    int portfolioCount = 0;
    int instrumentCount = 0;
    int strategyCount = 0;
    std::string lastError;
};

}  // namespace etfdt::shell_services
