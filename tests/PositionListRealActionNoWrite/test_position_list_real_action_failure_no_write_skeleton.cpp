#include "PositionListRealActionNoWriteSkeleton.h"

#include "AccountingNoWriteTestUtils.h"
#include "DataAccess/MigrationRunner.h"

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
        / ("etfdt_position_list_failure_no_write_" + std::to_string(now));
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

}  // namespace

int main(int argc, char* argv[])
{
    const auto migrationPath = optionValue(argc, argv, "--migration");
    expectTrue(!migrationPath.empty(), "--migration option is required");
    if (migrationPath.empty()) {
        return 1;
    }

    const auto scenarios =
        etfdt::tests::position_list_no_write::positionListRealActionFailureNoWriteScenarios();
    expectTrue(!scenarios.empty(), "failure no-write skeleton scenarios exist");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "invalid_request_protocol_error_path"),
        "failure skeleton includes INVALID_REQUEST");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "replay_unavailable_path"),
        "failure skeleton includes REPLAY_NOT_AVAILABLE");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "facts_query_unavailable_path"),
        "failure skeleton includes DATA_SOURCE_MISSING");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "mapping_failure_path"),
        "failure skeleton includes POSITION_LIST_MAPPING_FAILED");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "missing_market_price_path"),
        "failure skeleton includes MARKET_PRICE_MISSING");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "multi_currency_unsupported_path"),
        "failure skeleton includes MULTI_CURRENCY_UNSUPPORTED");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "fx_rate_missing_path"),
        "failure skeleton includes FX_RATE_MISSING");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "sqlite_readonly_open_failed_path"),
        "failure skeleton includes SQLITE_READONLY_OPEN_FAILED");
    expectTrue(
        etfdt::tests::position_list_no_write::hasScenario(scenarios, "sqlite_query_failed_path"),
        "failure skeleton includes SQLITE_QUERY_FAILED");

    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "position_list_failure_no_write.sqlite";

    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    for (const auto& scenario : scenarios) {
        etfdt::tests::position_list_no_write::FakePositionListActionResult callbackResult;
        const auto noWriteResult =
            etfdt::tests::accounting_no_write::assertNoWritesAround(connection, [&]() {
                callbackResult =
                    etfdt::tests::position_list_no_write::runFakePositionListRealActionCallbackForTest(
                        scenario);
            });

        expectTrue(noWriteResult.unchanged, scenario.scenarioName + " keeps protected row counts unchanged");
        expectEqual(callbackResult.scenarioName, scenario.scenarioName, scenario.scenarioName + " returns scenario name");
        expectEqual(callbackResult.domainStatus, scenario.expectedDomainStatus, scenario.scenarioName + " returns expected status");
        expectTrue(callbackResult.implemented == scenario.expectedImplemented, scenario.scenarioName + " keeps expected implemented flag");
        expectTrue(callbackResult.readOnly == scenario.expectedReadOnly, scenario.scenarioName + " keeps expected readOnly flag");
        expectTrue(callbackResult.writeEnabled == scenario.expectedWriteEnabled, scenario.scenarioName + " keeps expected writeEnabled flag");
        expectTrue(scenario.expectedNoWrite, scenario.scenarioName + " is modeled as no-write");
    }

    connection.close();
    std::filesystem::remove_all(tempDir);

    return gFailures == 0 ? 0 : 1;
}
