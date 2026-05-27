#include "AccountingNoWriteTestUtils.h"

#include "DataAccess/MigrationRunner.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
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
        / ("etfdt_accounting_no_write_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

bool containsTable(const std::vector<std::string>& tables, const std::string& table)
{
    return std::find(tables.begin(), tables.end(), table) != tables.end();
}

void openMigratedDatabase(
    const std::filesystem::path& migrationPath,
    const std::filesystem::path& dbPath,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath;
    config.migrationFilePath = migrationPath;
    config.enableWal = true;
    config.foreignKeys = true;

    auto openResult = connection.open(config);
    expectTrue(openResult.hasValue(), "SQLiteConnection open succeeds");
    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationResult.hasValue(), "Initial migration succeeds");
}

}  // namespace

int main(int argc, char* argv[])
{
    const auto migrationPath = optionValue(argc, argv, "--migration");
    expectTrue(!migrationPath.empty(), "--migration option is required");
    if (migrationPath.empty()) {
        return 1;
    }

    const auto protectedTables =
        etfdt::tests::accounting_no_write::protectedAccountingTables();
    expectTrue(containsTable(protectedTables, "trade_log"), "protected tables include trade_log");
    expectTrue(containsTable(protectedTables, "trade_draft"), "protected tables include trade_draft");
    expectTrue(containsTable(protectedTables, "cash_snapshot"), "protected tables include cash_snapshot");
    expectTrue(
        containsTable(protectedTables, "position_snapshot"),
        "protected tables include position_snapshot");
    expectTrue(
        containsTable(protectedTables, "portfolio_summary"),
        "protected tables include portfolio_summary");
    expectTrue(containsTable(protectedTables, "audit_log"), "protected tables include audit_log");

    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "accounting_no_write.sqlite";

    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    const auto counts = etfdt::tests::accounting_no_write::captureProtectedTableRowCounts(connection);
    expectTrue(
        counts.size() == protectedTables.size(),
        "capture returns one row-count entry per protected table");

    bool sawExistingTable = false;
    for (const auto& count : counts) {
        if (count.exists) {
            sawExistingTable = true;
            expectTrue(count.rowCount >= 0, "existing protected table has non-negative row count");
        }
    }
    expectTrue(sawExistingTable, "capture handles existing protected tables");

    auto missingTable = connection.queryTableExists("missing_task_086_table");
    expectTrue(missingTable.hasValue(), "missing table existence query succeeds");
    expectTrue(!missingTable.value(), "missing table is reported absent without schema mutation");

    const auto noopResult =
        etfdt::tests::accounting_no_write::assertNoWritesAround(connection, []() {});
    expectTrue(noopResult.unchanged, "no-op callback keeps protected table row counts unchanged");

    const auto selectOnlyResult =
        etfdt::tests::accounting_no_write::assertNoWritesAround(connection, [&connection]() {
            auto count = connection.querySingleInt("SELECT COUNT(*) FROM trade_log;");
            expectTrue(count.hasValue(), "SELECT-only callback succeeds");
        });
    expectTrue(
        selectOnlyResult.unchanged,
        "SELECT-only callback keeps protected table row counts unchanged");

    auto artificialBefore = counts;
    auto artificialAfter = counts;
    for (auto& count : artificialAfter) {
        if (count.exists) {
            ++count.rowCount;
            break;
        }
    }
    auto changedResult = etfdt::tests::accounting_no_write::compareProtectedTableRowCounts(
        artificialBefore,
        artificialAfter);
    expectTrue(!changedResult.unchanged, "comparison reports row count changes");
    expectTrue(!changedResult.messages.empty(), "comparison reports failure message");

    auto schemaStillExists = connection.queryTableExists("missing_task_086_table");
    expectTrue(schemaStillExists.hasValue(), "schema mutation check succeeds");
    expectTrue(!schemaStillExists.value(), "harness does not create missing tables");

    connection.close();
    std::filesystem::remove_all(tempDir);

    return gFailures == 0 ? 0 : 1;
}
