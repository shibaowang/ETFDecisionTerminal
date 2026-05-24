#include "DataAccess/SQLiteConnection.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace etfdt::data_access {
namespace {

std::string toLowerCopy(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string readFileToString(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

std::string joinMissingTables(const std::vector<std::string>& missingTables)
{
    std::ostringstream stream;
    for (std::size_t i = 0; i < missingTables.size(); ++i) {
        if (i != 0) {
            stream << ", ";
        }
        stream << missingTables[i];
    }
    return stream.str();
}

bool hasNonWhitespaceTail(const char* tail)
{
    if (tail == nullptr) {
        return false;
    }

    while (*tail != '\0') {
        const auto ch = static_cast<unsigned char>(*tail);
        if (!std::isspace(ch) && *tail != ';') {
            return true;
        }
        ++tail;
    }

    return false;
}

}  // namespace

SQLiteConnection::~SQLiteConnection()
{
    close();
}

DatabaseResult<bool> SQLiteConnection::open(const DatabaseConfig& config)
{
    close();
    config_ = config;

    if (config_.databasePath.empty()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2001_DATABASE_OPEN_FAILED,
            "Database path is required");
    }

    const int flags = SQLITE_OPEN_READWRITE
        | (config_.createIfMissing ? SQLITE_OPEN_CREATE : 0);
    sqlite3* opened = nullptr;
    const int rc = sqlite3_open_v2(config_.databasePath.string().c_str(), &opened, flags, nullptr);
    if (rc != SQLITE_OK) {
        std::string message = "Failed to open database";
        if (opened != nullptr) {
            message += ": ";
            message += sqlite3_errmsg(opened);
            sqlite3_close(opened);
        }
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2001_DATABASE_OPEN_FAILED,
            message);
    }

    db_ = opened;

    auto pragmaResult = initializePragmas();
    if (!pragmaResult) {
        close();
        return pragmaResult;
    }

    if (config_.enableWal) {
        auto walResult = enableWal();
        if (!walResult) {
            close();
            return walResult;
        }
    }

    return DatabaseResult<bool>::success(true);
}

void SQLiteConnection::close() noexcept
{
    if (db_ != nullptr) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool SQLiteConnection::isOpen() const noexcept
{
    return db_ != nullptr;
}

const DatabaseConfig& SQLiteConnection::config() const noexcept
{
    return config_;
}

DatabaseResult<bool> SQLiteConnection::executeSql(const std::string& sql)
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }

    char* errorMessage = nullptr;
    const int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errorMessage);
    if (rc != SQLITE_OK) {
        std::string message = "SQLite execute failed";
        if (errorMessage != nullptr) {
            message += ": ";
            message += errorMessage;
            sqlite3_free(errorMessage);
        }
        else {
            message += ": ";
            message += sqliteErrorMessage();
        }
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            message);
    }

    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> SQLiteConnection::executeStatement(
    const std::string& sql,
    const std::vector<SqlStatementParameter>& parameters)
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }

    sqlite3_stmt* statement = nullptr;
    const char* tail = nullptr;
    const int prepareRc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &statement, &tail);
    if (prepareRc != SQLITE_OK) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite prepare failed: " + sqliteErrorMessage());
    }

    if (statement == nullptr) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite prepare produced no statement");
    }

    if (hasNonWhitespaceTail(tail)) {
        sqlite3_finalize(statement);
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "Write statement helper accepts exactly one SQL statement");
    }

    const int expectedParameterCount = sqlite3_bind_parameter_count(statement);
    if (expectedParameterCount != static_cast<int>(parameters.size())) {
        sqlite3_finalize(statement);
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite parameter count mismatch");
    }

    for (int i = 0; i < expectedParameterCount; ++i) {
        const auto& parameter = parameters[static_cast<std::size_t>(i)];
        int bindRc = SQLITE_OK;
        if (parameter.isNull) {
            bindRc = sqlite3_bind_null(statement, i + 1);
        }
        else if (parameter.bindAsInteger) {
            bindRc = sqlite3_bind_int64(statement, i + 1, parameter.int64Value);
        }
        else {
            bindRc = sqlite3_bind_text(
                statement,
                i + 1,
                parameter.text.c_str(),
                -1,
                SQLITE_TRANSIENT);
        }

        if (bindRc != SQLITE_OK) {
            const std::string message = "SQLite bind failed: " + sqliteErrorMessage();
            sqlite3_finalize(statement);
            return DatabaseResult<bool>::failure(
                etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
                message);
        }
    }

    const int stepRc = sqlite3_step(statement);
    if (stepRc != SQLITE_DONE) {
        const std::string message = "SQLite statement failed: " + sqliteErrorMessage();
        sqlite3_finalize(statement);
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            message);
    }

    sqlite3_finalize(statement);
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::string> SQLiteConnection::querySingleString(const std::string& sql)
{
    sqlite3_stmt* statement = nullptr;
    auto prepareResult = prepareStatement(sql, &statement);
    if (!prepareResult) {
        return DatabaseResult<std::string>::failure(
            prepareResult.error().errorCode,
            prepareResult.error().message);
    }

    const int rc = sqlite3_step(statement);
    if (rc == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(statement, 0);
        std::string result = value == nullptr ? "" : reinterpret_cast<const char*>(value);
        sqlite3_finalize(statement);
        return DatabaseResult<std::string>::success(result);
    }

    if (rc == SQLITE_DONE) {
        sqlite3_finalize(statement);
        return DatabaseResult<std::string>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite query returned no rows");
    }

    std::string message = "SQLite query failed: " + sqliteErrorMessage();
    sqlite3_finalize(statement);
    return DatabaseResult<std::string>::failure(
        etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
        message);
}

DatabaseResult<int> SQLiteConnection::querySingleInt(const std::string& sql)
{
    sqlite3_stmt* statement = nullptr;
    auto prepareResult = prepareStatement(sql, &statement);
    if (!prepareResult) {
        return DatabaseResult<int>::failure(
            prepareResult.error().errorCode,
            prepareResult.error().message);
    }

    const int rc = sqlite3_step(statement);
    if (rc == SQLITE_ROW) {
        const int result = sqlite3_column_int(statement, 0);
        sqlite3_finalize(statement);
        return DatabaseResult<int>::success(result);
    }

    if (rc == SQLITE_DONE) {
        sqlite3_finalize(statement);
        return DatabaseResult<int>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite query returned no rows");
    }

    std::string message = "SQLite query failed: " + sqliteErrorMessage();
    sqlite3_finalize(statement);
    return DatabaseResult<int>::failure(
        etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
        message);
}

DatabaseResult<std::vector<SqlQueryRow>> SQLiteConnection::queryRows(
    const std::string& sql,
    const std::vector<std::string>& parameters)
{
    auto openResult = requireOpen();
    if (!openResult) {
        return DatabaseResult<std::vector<SqlQueryRow>>::failure(
            openResult.error().errorCode,
            openResult.error().message);
    }

    sqlite3_stmt* statement = nullptr;
    const char* tail = nullptr;
    const int prepareRc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &statement, &tail);
    if (prepareRc != SQLITE_OK) {
        return DatabaseResult<std::vector<SqlQueryRow>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite prepare failed: " + sqliteErrorMessage());
    }

    if (statement == nullptr) {
        return DatabaseResult<std::vector<SqlQueryRow>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite prepare produced no statement");
    }

    if (hasNonWhitespaceTail(tail)) {
        sqlite3_finalize(statement);
        return DatabaseResult<std::vector<SqlQueryRow>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "Read-only query helper accepts exactly one SQL statement");
    }

    if (sqlite3_stmt_readonly(statement) == 0) {
        sqlite3_finalize(statement);
        return DatabaseResult<std::vector<SqlQueryRow>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "Read-only query helper rejected a mutating SQL statement");
    }

    const int expectedParameterCount = sqlite3_bind_parameter_count(statement);
    if (expectedParameterCount != static_cast<int>(parameters.size())) {
        sqlite3_finalize(statement);
        return DatabaseResult<std::vector<SqlQueryRow>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite parameter count mismatch");
    }

    for (int i = 0; i < expectedParameterCount; ++i) {
        const auto& parameter = parameters[static_cast<std::size_t>(i)];
        const int bindRc =
            sqlite3_bind_text(statement, i + 1, parameter.c_str(), -1, SQLITE_TRANSIENT);
        if (bindRc != SQLITE_OK) {
            const std::string message = "SQLite bind failed: " + sqliteErrorMessage();
            sqlite3_finalize(statement);
            return DatabaseResult<std::vector<SqlQueryRow>>::failure(
                etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
                message);
        }
    }

    std::vector<SqlQueryRow> rows;
    while (true) {
        const int stepRc = sqlite3_step(statement);
        if (stepRc == SQLITE_ROW) {
            SqlQueryRow row;
            const int columnCount = sqlite3_column_count(statement);
            row.reserve(static_cast<std::size_t>(columnCount));
            for (int column = 0; column < columnCount; ++column) {
                SqlValue value;
                value.isNull = sqlite3_column_type(statement, column) == SQLITE_NULL;
                value.int64Value = sqlite3_column_int64(statement, column);
                const unsigned char* text = sqlite3_column_text(statement, column);
                value.text = text == nullptr ? "" : reinterpret_cast<const char*>(text);
                row.push_back(std::move(value));
            }
            rows.push_back(std::move(row));
            continue;
        }

        if (stepRc == SQLITE_DONE) {
            sqlite3_finalize(statement);
            return DatabaseResult<std::vector<SqlQueryRow>>::success(std::move(rows));
        }

        const std::string message = "SQLite query failed: " + sqliteErrorMessage();
        sqlite3_finalize(statement);
        return DatabaseResult<std::vector<SqlQueryRow>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            message);
    }
}

DatabaseResult<bool> SQLiteConnection::queryTableExists(const std::string& tableName)
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }

    sqlite3_stmt* statement = nullptr;
    const char* sql = "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = ? LIMIT 1";
    auto prepareResult = prepareStatement(sql, &statement);
    if (!prepareResult) {
        return prepareResult;
    }

    sqlite3_bind_text(statement, 1, tableName.c_str(), -1, SQLITE_TRANSIENT);
    const int rc = sqlite3_step(statement);
    if (rc == SQLITE_ROW) {
        sqlite3_finalize(statement);
        return DatabaseResult<bool>::success(true);
    }
    if (rc == SQLITE_DONE) {
        sqlite3_finalize(statement);
        return DatabaseResult<bool>::success(false);
    }

    std::string message = "SQLite table check failed: " + sqliteErrorMessage();
    sqlite3_finalize(statement);
    return DatabaseResult<bool>::failure(
        etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
        message);
}

DatabaseResult<bool> SQLiteConnection::initializePragmas()
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }

    sqlite3_busy_timeout(db_, config_.busyTimeoutMs);

    if (config_.foreignKeys) {
        auto result = executeSql("PRAGMA foreign_keys = ON;");
        if (!result) {
            return result;
        }
    }

    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> SQLiteConnection::enableWal()
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }

    auto mode = querySingleString("PRAGMA journal_mode = WAL;");
    if (!mode) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            mode.error().message);
    }

    if (toLowerCopy(mode.value()) != "wal") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "Failed to enable WAL; journal_mode is " + mode.value());
    }

    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> SQLiteConnection::applyMigrationFile(
    const std::filesystem::path& migrationPath)
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }

    if (migrationPath.empty() || !std::filesystem::exists(migrationPath)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            "Migration file does not exist: " + migrationPath.string());
    }

    const std::string sql = readFileToString(migrationPath);
    if (sql.empty()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            "Migration file is empty: " + migrationPath.string());
    }

    auto result = executeSql(sql);
    if (!result) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            result.error().message);
    }

    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::vector<std::string>> SQLiteConnection::getSchemaVersions()
{
    auto exists = queryTableExists("schema_version");
    if (!exists) {
        return DatabaseResult<std::vector<std::string>>::failure(
            exists.error().errorCode,
            exists.error().message);
    }
    if (!exists.value()) {
        return DatabaseResult<std::vector<std::string>>::success({});
    }

    sqlite3_stmt* statement = nullptr;
    const char* sql = "SELECT migration_name FROM schema_version ORDER BY version, id";
    auto prepareResult = prepareStatement(sql, &statement);
    if (!prepareResult) {
        return DatabaseResult<std::vector<std::string>>::failure(
            prepareResult.error().errorCode,
            prepareResult.error().message);
    }

    std::vector<std::string> versions;
    while (true) {
        const int rc = sqlite3_step(statement);
        if (rc == SQLITE_ROW) {
            const unsigned char* value = sqlite3_column_text(statement, 0);
            versions.emplace_back(value == nullptr ? "" : reinterpret_cast<const char*>(value));
            continue;
        }
        if (rc == SQLITE_DONE) {
            sqlite3_finalize(statement);
            return DatabaseResult<std::vector<std::string>>::success(std::move(versions));
        }

        std::string message = "SQLite schema_version query failed: " + sqliteErrorMessage();
        sqlite3_finalize(statement);
        return DatabaseResult<std::vector<std::string>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            message);
    }
}

DatabaseResult<DatabaseStatus> SQLiteConnection::healthCheck()
{
    DatabaseStatus status;
    status.opened = isOpen();
    status.databasePath = config_.databasePath.string();

    if (!status.opened) {
        status.message = "Database is not open";
        return DatabaseResult<DatabaseStatus>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            status.message);
    }

    auto foreignKeys = querySingleInt("PRAGMA foreign_keys;");
    if (!foreignKeys) {
        return DatabaseResult<DatabaseStatus>::failure(
            foreignKeys.error().errorCode,
            foreignKeys.error().message);
    }
    status.foreignKeysEnabled = foreignKeys.value() == 1;

    auto journalMode = querySingleString("PRAGMA journal_mode;");
    if (!journalMode) {
        return DatabaseResult<DatabaseStatus>::failure(
            journalMode.error().errorCode,
            journalMode.error().message);
    }
    status.walEnabled = toLowerCopy(journalMode.value()) == "wal";

    auto schemaVersionExists = queryTableExists("schema_version");
    if (!schemaVersionExists) {
        return DatabaseResult<DatabaseStatus>::failure(
            schemaVersionExists.error().errorCode,
            schemaVersionExists.error().message);
    }
    status.schemaVersionPresent = schemaVersionExists.value();
    if (!status.schemaVersionPresent) {
        status.message = "schema_version table is missing";
        return DatabaseResult<DatabaseStatus>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            status.message);
    }

    auto latestVersion = querySingleInt("SELECT COALESCE(MAX(version), 0) FROM schema_version;");
    if (!latestVersion) {
        return DatabaseResult<DatabaseStatus>::failure(
            latestVersion.error().errorCode,
            latestVersion.error().message);
    }
    status.latestVersion = latestVersion.value();

    auto migrationCount = querySingleInt(
        "SELECT COUNT(*) FROM schema_version "
        "WHERE migration_name LIKE '%001_initial_schema%';");
    if (!migrationCount) {
        return DatabaseResult<DatabaseStatus>::failure(
            migrationCount.error().errorCode,
            migrationCount.error().message);
    }
    status.initialMigrationApplied = migrationCount.value() > 0;
    if (!status.initialMigrationApplied) {
        status.message = "001_initial_schema migration is missing";
        return DatabaseResult<DatabaseStatus>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            status.message);
    }

    for (const auto& table : requiredCoreTables()) {
        auto tableExists = queryTableExists(table);
        if (!tableExists) {
            return DatabaseResult<DatabaseStatus>::failure(
                tableExists.error().errorCode,
                tableExists.error().message);
        }
        if (!tableExists.value()) {
            status.missingCoreTables.push_back(table);
        }
    }

    if (!status.foreignKeysEnabled) {
        status.message = "foreign_keys PRAGMA is not enabled";
        return DatabaseResult<DatabaseStatus>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            status.message);
    }
    if (!status.walEnabled) {
        status.message = "WAL journal mode is not enabled";
        return DatabaseResult<DatabaseStatus>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            status.message);
    }
    if (!status.missingCoreTables.empty()) {
        status.message = "Missing core tables: " + joinMissingTables(status.missingCoreTables);
        return DatabaseResult<DatabaseStatus>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            status.message);
    }

    status.message = "Database health check passed";
    return DatabaseResult<DatabaseStatus>::success(std::move(status));
}

DatabaseResult<bool> SQLiteConnection::beginTransaction()
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }
    if (isInTransaction()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "Nested SQLite transactions are not supported");
    }
    auto result = executeSql("BEGIN IMMEDIATE TRANSACTION;");
    if (!result) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            result.error().message);
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> SQLiteConnection::commitTransaction()
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }
    if (!isInTransaction()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "No active SQLite transaction to commit");
    }
    auto result = executeSql("COMMIT;");
    if (!result) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            result.error().message);
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> SQLiteConnection::rollbackTransaction()
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }
    if (!isInTransaction()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "No active SQLite transaction to rollback");
    }
    auto result = executeSql("ROLLBACK;");
    if (!result) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            result.error().message);
    }
    return DatabaseResult<bool>::success(true);
}

bool SQLiteConnection::isInTransaction() const noexcept
{
    return db_ != nullptr && sqlite3_get_autocommit(db_) == 0;
}

std::int64_t SQLiteConnection::lastInsertRowId() const noexcept
{
    return db_ == nullptr ? 0 : static_cast<std::int64_t>(sqlite3_last_insert_rowid(db_));
}

std::string SQLiteConnection::sqliteErrorMessage() const
{
    if (db_ == nullptr) {
        return "database is not open";
    }
    return sqlite3_errmsg(db_);
}

DatabaseResult<bool> SQLiteConnection::requireOpen() const
{
    if (db_ == nullptr) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "Database is not open");
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> SQLiteConnection::prepareStatement(
    const std::string& sql,
    sqlite3_stmt** statement)
{
    auto openResult = requireOpen();
    if (!openResult) {
        return openResult;
    }

    const int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, statement, nullptr);
    if (rc != SQLITE_OK) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "SQLite prepare failed: " + sqliteErrorMessage());
    }

    return DatabaseResult<bool>::success(true);
}

}  // namespace etfdt::data_access
