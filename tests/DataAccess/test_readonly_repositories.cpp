#include "DataAccess/DataAccess.h"

#include <chrono>
#include <cstdint>
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

void expectEqual(std::int64_t actual, std::int64_t expected, std::string_view message)
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
        / ("etfdt_readonly_repositories_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

void testReadOnlyRepositories(const std::filesystem::path& migrationPath)
{
    using etfdt::data_access::AccountRepository;
    using etfdt::data_access::DatabaseConfig;
    using etfdt::data_access::InstrumentRepository;
    using etfdt::data_access::MigrationRunner;
    using etfdt::data_access::OtcChannelRepository;
    using etfdt::data_access::PortfolioRepository;
    using etfdt::data_access::SchemaVersionRepository;
    using etfdt::data_access::SQLiteConnection;
    using etfdt::data_access::StrategyRepository;

    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "readonly.db";

    SQLiteConnection connection;
    DatabaseConfig config;
    config.databasePath = dbPath;
    config.migrationFilePath = migrationPath;

    auto openResult = connection.open(config);
    expectTrue(openResult.hasValue(), "SQLiteConnection open succeeds");

    auto migrationResult = MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationResult.hasValue(), "Initial migration succeeds");

    auto health = connection.healthCheck();
    expectTrue(health.hasValue(), "healthCheck succeeds before repository reads");

    AccountRepository accounts(connection);
    auto accountList = accounts.listAccounts();
    expectTrue(accountList.hasValue(), "AccountRepository::listAccounts succeeds");
    if (accountList.hasValue()) {
        expectTrue(!accountList.value().empty(), "default account exists");
        expectEqual(accountList.value().front().initialCashCents.raw(), 0, "default account cash");
        expectTrue(
            accountList.value().front().accountType == etfdt::core::AccountType::Real,
            "default account type is REAL");
    }

    auto activeAccount = accounts.getActiveAccount();
    expectTrue(activeAccount.hasValue(), "AccountRepository::getActiveAccount succeeds");
    if (activeAccount.hasValue()) {
        expectTrue(activeAccount.value().isActive, "active account is active");
        auto byId = accounts.getById(activeAccount.value().id);
        expectTrue(byId.hasValue(), "AccountRepository::getById succeeds");
        if (byId.hasValue()) {
            expectEqual(byId.value().id, activeAccount.value().id, "account getById id");
        }
    }

    PortfolioRepository portfolios(connection);
    auto portfolioList = portfolios.listPortfolios();
    expectTrue(portfolioList.hasValue(), "PortfolioRepository::listPortfolios succeeds");
    if (portfolioList.hasValue()) {
        expectTrue(!portfolioList.value().empty(), "default portfolio exists");
        expectEqual(
            portfolioList.value().front().basePositionRatioPpm.raw(),
            200000,
            "default base position ratio");
    }

    auto activePortfolio = portfolios.getActivePortfolio();
    expectTrue(activePortfolio.hasValue(), "PortfolioRepository::getActivePortfolio succeeds");
    if (activePortfolio.hasValue()) {
        expectTrue(activePortfolio.value().isActive, "active portfolio is active");
        auto byId = portfolios.getById(activePortfolio.value().id);
        expectTrue(byId.hasValue(), "PortfolioRepository::getById succeeds");
        if (byId.hasValue()) {
            expectEqual(byId.value().id, activePortfolio.value().id, "portfolio getById id");
        }
    }

    InstrumentRepository instruments(connection);
    auto instrumentList = instruments.listEnabledInstruments();
    expectTrue(instrumentList.hasValue(), "InstrumentRepository::listEnabledInstruments succeeds");
    if (instrumentList.hasValue()) {
        expectTrue(!instrumentList.value().empty(), "enabled instrument exists");
    }

    auto cash = instruments.getByCode("CASH");
    expectTrue(cash.hasValue(), "InstrumentRepository::getByCode CASH succeeds");
    if (cash.hasValue()) {
        expectEqual(cash.value().code.raw(), "CASH", "CASH instrument code");
        expectTrue(
            cash.value().instrumentType == etfdt::core::InstrumentType::Cash,
            "CASH instrument type");
        expectTrue(cash.value().marketCode == etfdt::core::MarketCode::Cash, "CASH market code");
        auto byId = instruments.getById(cash.value().id);
        expectTrue(byId.hasValue(), "InstrumentRepository::getById succeeds");
    }

    StrategyRepository strategies(connection);
    auto strategyList = strategies.listEnabledStrategies();
    expectTrue(strategyList.hasValue(), "StrategyRepository::listEnabledStrategies succeeds");

    OtcChannelRepository otcChannels(connection);
    auto missingOtc = otcChannels.listAllByStrategyCode("159509");
    expectTrue(missingOtc.hasValue(), "OtcChannelRepository missing strategy returns empty list");
    if (missingOtc.hasValue()) {
        expectTrue(missingOtc.value().empty(), "missing strategy has no OTC channels");
    }

    SchemaVersionRepository schemaVersions(connection);
    auto versions = schemaVersions.listVersions();
    expectTrue(versions.hasValue(), "SchemaVersionRepository::listVersions succeeds");
    if (versions.hasValue()) {
        expectTrue(!versions.value().empty(), "schema_version has rows");
    }
    auto hasInitial = schemaVersions.hasMigration("001_initial_schema");
    expectTrue(hasInitial.hasValue(), "SchemaVersionRepository::hasMigration succeeds");
    if (hasInitial.hasValue()) {
        expectTrue(hasInitial.value(), "schema_version has 001_initial_schema");
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

    testReadOnlyRepositories(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " read-only repository test failure(s)" << '\n';
        return 1;
    }

    std::cout << "Read-only repository tests passed." << '\n';
    return 0;
}
