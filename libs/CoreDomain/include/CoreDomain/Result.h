#pragma once

#include <stdexcept>
#include <string>
#include <utility>

namespace etfdt::core {

template <typename T>
class Result final {
public:
    static Result success(T value)
    {
        return Result(std::move(value));
    }

    static Result failure(std::string error)
    {
        return Result(std::move(error));
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
        if (!hasValue_) {
            throw std::logic_error("Result has no value");
        }
        return value_;
    }

    [[nodiscard]] T& value()
    {
        if (!hasValue_) {
            throw std::logic_error("Result has no value");
        }
        return value_;
    }

    [[nodiscard]] const std::string& error() const noexcept
    {
        return error_;
    }

private:
    explicit Result(T value)
        : hasValue_(true)
        , value_(std::move(value))
    {
    }

    explicit Result(std::string error)
        : hasValue_(false)
        , error_(std::move(error))
    {
    }

    bool hasValue_ = false;
    T value_{};
    std::string error_;
};

}  // namespace etfdt::core
