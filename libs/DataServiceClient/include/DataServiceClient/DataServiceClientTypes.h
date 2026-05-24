#pragma once

#include "Protocol/Protocol.h"

#include <optional>
#include <string>
#include <utility>

namespace etfdt::data_service_client {

struct DataServiceClientError final {
    std::optional<etfdt::protocol::ErrorCode> errorCode;
    std::string message;
};

template <typename T>
class DataServiceClientResult final {
public:
    static DataServiceClientResult success(T value)
    {
        return DataServiceClientResult(std::move(value));
    }

    static DataServiceClientResult failure(
        std::optional<etfdt::protocol::ErrorCode> errorCode,
        std::string message)
    {
        return DataServiceClientResult(DataServiceClientError{errorCode, std::move(message)});
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

    [[nodiscard]] const DataServiceClientError& error() const noexcept
    {
        return error_;
    }

private:
    explicit DataServiceClientResult(T value)
        : hasValue_(true)
        , value_(std::move(value))
    {
    }

    explicit DataServiceClientResult(DataServiceClientError error)
        : hasValue_(false)
        , error_(std::move(error))
    {
    }

    bool hasValue_ = false;
    T value_{};
    DataServiceClientError error_;
};

struct AuditAppendRequest final {
    std::string entityType;
    std::string entityId;
    std::string action;
    std::string reason;
    std::string operatorName = "local-user";
    std::string oldValueJson = "{}";
    std::string newValueJson = "{}";
};

}  // namespace etfdt::data_service_client
