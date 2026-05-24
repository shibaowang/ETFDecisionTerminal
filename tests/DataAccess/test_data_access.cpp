#include "DataAccess/DataAccess.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectEqual(int actual, int expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectEqual(std::string_view actual, std::string_view expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

std::filesystem::path optionValue(
    int argc,
    char* argv[],
    const std::string& option)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == option) {
            return argv[i + 1];
        }
    }
    return {};
}

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_dataaccess_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

std::string toLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool containsInitialMigration(const std::vector<std::string>& versions)
{
    return std::any_of(versions.begin(), versions.end(), [](const std::string& value) {
        return value.find("001_initial_schema") != std::string::npos;
    });
}

void testDatabaseInitialization(const std::filesystem::path& migrationPath)
{
    using etfdt::data_access::DatabaseConfig;
    using etfdt::data_access::MigrationRunner;
    using etfdt::data_access::SQLiteConnection;
    using etfdt::data_access::requiredCoreTables;
    using etfdt::protocol::ErrorCode;

    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "test.db";

    SQLiteConnection connection;
    DatabaseConfig config;
    config.databasePath = dbPath;
    config.migrationFilePath = migrationPath;
    config.enableWal = true;
    config.foreignKeys = true;
    config.busyTimeoutMs = 2500;

    auto openResult = connection.open(config);
    expectTrue(openResult.hasValue(), "SQLiteConnection open succeeds");
    expectTrue(connection.isOpen(), "SQLiteConnection is open");

    auto migrationResult = MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationResult.hasValue(), "Initial migration succeeds");

    auto migrationAgain = MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationAgain.hasValue(), "Initial migration is idempotent");

    auto health = connection.healthCheck();
    expectTrue(health.hasValue(), "healthCheck succeeds");
    if (health.hasValue()) {
        expectTrue(health.value().opened, "healthCheck opened");
        expectTrue(health.value().walEnabled, "healthCheck WAL enabled");
        expectTrue(health.value().foreignKeysEnabled, "healthCheck foreign keys enabled");
        expectTrue(health.value().schemaVersionPresent, "healthCheck schema_version present");
        expectTrue(
            health.value().initialMigrationApplied,
            "healthCheck initial migration applied");
        expectEqual(health.value().latestVersion, 1, "healthCheck latest version");
    }

    auto journalMode = connection.querySingleString("PRAGMA journal_mode;");
    expectTrue(journalMode.hasValue(), "journal_mode query succeeds");
    if (journalMode.hasValue()) {
        expectEqual(toLower(journalMode.value()), "wal", "WAL is enabled");
    }

    auto foreignKeys = connection.querySingleInt("PRAGMA foreign_keys;");
    expectTrue(foreignKeys.hasValue(), "foreign_keys query succeeds");
    if (foreignKeys.hasValue()) {
        expectEqual(foreignKeys.value(), 1, "foreign_keys is ON");
    }

    auto versions = connection.getSchemaVersions();
    expectTrue(versions.hasValue(), "schema_version query succeeds");
    if (versions.hasValue()) {
        expectTrue(
            containsInitialMigration(versions.value()),
            "schema_version contains 001_initial_schema");
    }

    for (const auto& table : requiredCoreTables()) {
        auto exists = connection.queryTableExists(table);
        expectTrue(exists.hasValue(), "table exists query succeeds for " + table);
        if (exists.hasValue()) {
            expectTrue(exists.value(), "core table exists: " + table);
        }
    }

    auto missingMigration = MigrationRunner::runInitialMigration(
        connection,
        tempDir / "missing_001_initial_schema.sql");
    expectTrue(!missingMigration.hasValue(), "missing migration fails");
    if (!missingMigration.hasValue()) {
        expectTrue(
            missingMigration.error().errorCode == ErrorCode::E2002_MIGRATION_FAILED,
            "missing migration returns E2002");
    }

    connection.close();
    std::filesystem::remove_all(tempDir);
}

}  // namespace

int main(int argc, char* argv[])
{
    const auto migrationPath = optionValue(argc, argv, "--migration");
    if (migrationPath.empty()) {
        std::cerr << "--migration <path> is required\n";
        return 1;
    }

    testDatabaseInitialization(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " DataAccess test failure(s)" << '\n';
        return 1;
    }

    std::cout << "DataAccess tests passed." << '\n';
    return 0;
}
