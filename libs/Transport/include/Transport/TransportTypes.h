#pragma once

#include <string>
#include <utility>

namespace etfdt::transport {

struct TransportError final {
    std::string message;
};

template <typename T>
class TransportResult final {
public:
    static TransportResult success(T value)
    {
        return TransportResult(std::move(value));
    }

    static TransportResult failure(std::string message)
    {
        return TransportResult(TransportError{std::move(message)});
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

    [[nodiscard]] const TransportError& error() const noexcept
    {
        return error_;
    }

private:
    explicit TransportResult(T value)
        : hasValue_(true)
        , value_(std::move(value))
    {
    }

    explicit TransportResult(TransportError error)
        : hasValue_(false)
        , error_(std::move(error))
    {
    }

    bool hasValue_ = false;
    T value_{};
    TransportError error_;
};

}  // namespace etfdt::transport
