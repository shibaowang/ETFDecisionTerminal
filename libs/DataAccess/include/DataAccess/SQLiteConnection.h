#pragma once

#include "DataAccess/DatabaseTypes.h"

#include <filesystem>
#include <sqlite3.h>
#include <string>
#include <vector>

namespace etfdt::data_access {

class SQLiteConnection final {
public:
    SQLiteConnection() = default;
    ~SQLiteConnection();

    SQLiteConnection(const SQLiteConnection&) = delete;
    SQLiteConnection& operator=(const SQLiteConnection&) = delete;
    SQLiteConnection(SQLiteConnection&&) = delete;
    SQLiteConnection& operator=(SQLiteConnection&&) = delete;

    [[nodiscard]] DatabaseResult<bool> open(const DatabaseConfig& config);
    void close() noexcept;

    [[nodiscard]] bool isOpen() const noexcept;
    [[nodiscard]] const DatabaseConfig& config() const noexcept;

    [[nodiscard]] DatabaseResult<bool> executeSql(const std::string& sql);
    [[nodiscard]] DatabaseResult<std::string> querySingleString(const std::string& sql);
    [[nodiscard]] DatabaseResult<int> querySingleInt(const std::string& sql);
    [[nodiscard]] DatabaseResult<bool> queryTableExists(const std::string& tableName);

    [[nodiscard]] DatabaseResult<bool> initializePragmas();
    [[nodiscard]] DatabaseResult<bool> enableWal();
    [[nodiscard]] DatabaseResult<bool> applyMigrationFile(const std::filesystem::path& migrationPath);
    [[nodiscard]] DatabaseResult<std::vector<std::string>> getSchemaVersions();
    [[nodiscard]] DatabaseResult<DatabaseStatus> healthCheck();

private:
    [[nodiscard]] std::string sqliteErrorMessage() const;
    [[nodiscard]] DatabaseResult<bool> requireOpen() const;
    [[nodiscard]] DatabaseResult<bool> prepareStatement(
        const std::string& sql,
        sqlite3_stmt** statement);

    sqlite3* db_ = nullptr;
    DatabaseConfig config_;
};

}  // namespace etfdt::data_access
