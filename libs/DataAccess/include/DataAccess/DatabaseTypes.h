#pragma once

#include "Protocol/ErrorCode.h"

#include <filesystem>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace etfdt::data_access {

struct DatabaseConfig final {
    std::filesystem::path databasePath;
    std::filesystem::path migrationFilePath;
    bool enableWal = true;
    int busyTimeoutMs = 5000;
    bool foreignKeys = true;
    bool createIfMissing = true;
};

struct DatabaseStatus final {
    bool opened = false;
    bool walEnabled = false;
    bool foreignKeysEnabled = false;
    bool schemaVersionPresent = false;
    bool initialMigrationApplied = false;
    int latestVersion = 0;
    std::string databasePath;
    std::string message;
    std::vector<std::string> missingCoreTables;
};

struct DatabaseError final {
    etfdt::protocol::ErrorCode errorCode = etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR;
    std::string message;
};

struct SqlValue final {
    bool isNull = true;
    std::string text;
    std::int64_t int64Value = 0;
};

struct SqlStatementParameter final {
    bool isNull = true;
    std::string text;
    std::int64_t int64Value = 0;
    bool bindAsInteger = false;

    [[nodiscard]] static SqlStatementParameter nullValue()
    {
        return {};
    }

    [[nodiscard]] static SqlStatementParameter textValue(std::string value)
    {
        SqlStatementParameter parameter;
        parameter.isNull = false;
        parameter.text = std::move(value);
        return parameter;
    }

    [[nodiscard]] static SqlStatementParameter int64ValueOf(std::int64_t value)
    {
        SqlStatementParameter parameter;
        parameter.isNull = false;
        parameter.int64Value = value;
        parameter.bindAsInteger = true;
        return parameter;
    }
};

using SqlQueryRow = std::vector<SqlValue>;

template <typename T>
class DatabaseResult final {
public:
    static DatabaseResult success(T value)
    {
        return DatabaseResult(std::move(value));
    }

    static DatabaseResult failure(etfdt::protocol::ErrorCode code, std::string message)
    {
        return DatabaseResult(DatabaseError{code, std::move(message)});
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

    [[nodiscard]] const DatabaseError& error() const noexcept
    {
        return error_;
    }

private:
    explicit DatabaseResult(T value)
        : hasValue_(true)
        , value_(std::move(value))
    {
    }

    explicit DatabaseResult(DatabaseError error)
        : hasValue_(false)
        , error_(std::move(error))
    {
    }

    bool hasValue_ = false;
    T value_{};
    DatabaseError error_;
};

[[nodiscard]] const std::vector<std::string>& requiredCoreTables();

}  // namespace etfdt::data_access
