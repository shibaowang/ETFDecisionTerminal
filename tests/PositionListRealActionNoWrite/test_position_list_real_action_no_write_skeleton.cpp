#include "PositionListRealActionNoWriteSkeleton.h"

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

void expectEqual(std::string_view actual, std::string_view expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
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
        / ("etfdt_position_list_real_no_write_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
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
    expectTrue(openResult.hasValue(), "test database opens");
    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationResult.hasValue(), "test migration succeeds");
}

bool containsTable(const std::vector<std::string>& tables, const std::string& tableName)
{
    return std::find(tables.begin(), tables.end(), tableName) != tables.end();
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
    expectTrue(containsTable(protectedTables, "trade_execution_group"), "protected tables include trade_execution_group");
    expectTrue(containsTable(protectedTables, "trade_draft"), "protected tables include trade_draft");
    expectTrue(containsTable(protectedTables, "cash_snapshot"), "protected tables include cash_snapshot");
    expectTrue(containsTable(protectedTables, "position_snapshot"), "protected tables include position_snapshot");
    expectTrue(containsTable(protectedTables, "portfolio_summary"), "protected tables include portfolio_summary");
    expectTrue(containsTable(protectedTables, "audit_log"), "protected tables include audit_log");

    const auto scenarios =
        etfdt::tests::position_list_no_write::positionListRealActionNoWriteScenarios();
    expectTrue(!scenarios.empty(), "position.list real action no-write skeleton scenarios exist");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "valid_request_guard_path"),
        "skeleton includes valid request guard path");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "invalid_request_protocol_error_path"),
        "skeleton includes invalid request protocol error path");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "replay_unavailable_path"),
        "skeleton includes replay unavailable path");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "facts_query_unavailable_path"),
        "skeleton includes facts query unavailable path");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "mapping_failure_path"),
        "skeleton includes mapping failure path");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "missing_market_price_path"),
        "skeleton includes missing market price path");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "multi_currency_unsupported_path"),
        "skeleton includes multi-currency unsupported path");

    const auto validScenarioIt = std::find_if(
        scenarios.begin(),
        scenarios.end(),
        [](const auto& scenario) {
            return scenario.scenarioName == "valid_request_guard_path";
        });
    expectTrue(validScenarioIt != scenarios.end(), "valid request guard scenario is available");

    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "position_list_real_no_write.sqlite";

    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    if (validScenarioIt != scenarios.end()) {
        etfdt::tests::position_list_no_write::FakePositionListActionResult callbackResult;
        const auto noWriteResult =
            etfdt::tests::accounting_no_write::assertNoWritesAround(connection, [&]() {
                callbackResult =
                    etfdt::tests::position_list_no_write::runFakePositionListRealActionCallbackForTest(
                        *validScenarioIt);
            });

        expectTrue(noWriteResult.unchanged, "valid_request_guard_path keeps protected row counts unchanged");
        expectEqual(callbackResult.scenarioName, "valid_request_guard_path", "valid scenario name is returned");
        expectEqual(callbackResult.domainStatus, "POSITION_LIST_NOT_AVAILABLE", "valid guard path remains unavailable");
        expectTrue(!callbackResult.implemented, "position.list guard skeleton remains implemented=false");
        expectTrue(callbackResult.readOnly, "position.list guard skeleton remains readOnly=true");
        expectTrue(!callbackResult.writeEnabled, "position.list guard skeleton remains writeEnabled=false");
    }

    connection.close();
    std::filesystem::remove_all(tempDir);

    return gFailures == 0 ? 0 : 1;
}
