#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "Protocol/ErrorCode.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "Transport/LocalSocketName.h"

#include <QCoreApplication>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

using etfdt::data_access::AccountRecord;
using etfdt::data_access::DatabaseError;
using etfdt::data_access::InstrumentRecord;
using etfdt::data_access::OtcChannelRecord;
using etfdt::data_access::PortfolioRecord;
using etfdt::data_access::SchemaVersionRecord;
using etfdt::data_access::StrategyRecord;

void printHelp()
{
    std::cout
        << "ETFDataService database utility\n"
        << "\n"
        << "Usage:\n"
        << "  ETFDataService --init-db --db <path> --migration <path>\n"
        << "  ETFDataService --check-db --db <path>\n"
        << "  ETFDataService --db-summary --db <path>\n"
        << "  ETFDataService --list-accounts --db <path>\n"
        << "  ETFDataService --list-portfolios --db <path>\n"
        << "  ETFDataService --list-instruments --db <path>\n"
        << "  ETFDataService --list-strategies --db <path>\n"
        << "  ETFDataService --list-otc --db <path> --strategy-code <code>\n"
        << "  ETFDataService --serve-readonly --db <path> --socket-name <name>\n"
        << "  ETFDataService --serve-daily-use --db <path> --socket-name <name>\n"
        << "  ETFDataService --serve-dev-audit --db <path> --socket-name <name>\n"
        << "  ETFDataService --help\n"
        << "\n"
        << "This entry point initializes, checks, reads SQLite, or starts the "
        << "development preview Local Socket services. The dev audit service only "
        << "writes audit_log and does not write business facts.\n";
}

std::string optionValue(const std::vector<std::string>& args, const std::string& option)
{
    for (std::size_t i = 0; i + 1 < args.size(); ++i) {
        if (args[i] == option) {
            return args[i + 1];
        }
    }
    return {};
}

bool hasOption(const std::vector<std::string>& args, const std::string& option)
{
    for (const auto& arg : args) {
        if (arg == option) {
            return true;
        }
    }
    return false;
}

void printStatus(const etfdt::data_access::DatabaseStatus& status)
{
    std::cout << "databasePath: " << status.databasePath << '\n';
    std::cout << "opened: " << (status.opened ? "true" : "false") << '\n';
    std::cout << "walEnabled: " << (status.walEnabled ? "true" : "false") << '\n';
    std::cout << "foreignKeysEnabled: " << (status.foreignKeysEnabled ? "true" : "false")
              << '\n';
    std::cout << "schemaVersionPresent: "
              << (status.schemaVersionPresent ? "true" : "false") << '\n';
    std::cout << "initialMigrationApplied: "
              << (status.initialMigrationApplied ? "true" : "false") << '\n';
    std::cout << "latestVersion: " << status.latestVersion << '\n';
    std::cout << "message: " << status.message << '\n';
}

int printDatabaseError(const DatabaseError& error)
{
    std::cerr << etfdt::protocol::toString(error.errorCode) << ": " << error.message << '\n';
    return 1;
}

int printQueryError(const std::string& message)
{
    std::cerr << etfdt::protocol::toString(etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR)
              << ": " << message << '\n';
    return 1;
}

etfdt::data_access::DatabaseResult<bool> openHealthyReadOnlyDatabase(
    const std::filesystem::path& dbPath,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath;
    config.createIfMissing = false;

    auto openResult = connection.open(config);
    if (!openResult) {
        return openResult;
    }

    auto health = connection.healthCheck();
    if (!health) {
        return etfdt::data_access::DatabaseResult<bool>::failure(
            health.error().errorCode,
            health.error().message);
    }

    return etfdt::data_access::DatabaseResult<bool>::success(true);
}

void printAccount(const AccountRecord& account)
{
    std::cout << "id: " << account.id << ", uid: " << account.uid.raw()
              << ", name: " << account.name
              << ", accountType: " << etfdt::core::toString(account.accountType)
              << ", brokerName: " << account.brokerName
              << ", baseCurrency: " << account.baseCurrency
              << ", isActive: " << (account.isActive ? "true" : "false")
              << ", initialCashCents: " << account.initialCashCents.raw() << '\n';
}

void printPortfolio(const PortfolioRecord& portfolio)
{
    std::cout << "id: " << portfolio.id << ", uid: " << portfolio.uid.raw()
              << ", name: " << portfolio.name
              << ", basePositionRatioPpm: " << portfolio.basePositionRatioPpm.raw()
              << ", isActive: " << (portfolio.isActive ? "true" : "false") << '\n';
}

void printInstrument(const InstrumentRecord& instrument)
{
    std::cout << "id: " << instrument.id << ", uid: " << instrument.uid.raw()
              << ", code: " << instrument.code.raw()
              << ", name: " << instrument.name
              << ", instrumentType: " << etfdt::core::toString(instrument.instrumentType)
              << ", marketCode: " << etfdt::core::toString(instrument.marketCode)
              << ", currency: " << instrument.currency
              << ", enabled: " << (instrument.enabled ? "true" : "false") << '\n';
}

void printStrategy(const StrategyRecord& strategy)
{
    std::cout << "id: " << strategy.id << ", uid: " << strategy.uid.raw()
              << ", strategyCode: " << strategy.strategyCode.raw()
              << ", instrumentId: " << strategy.instrumentId
              << ", name: " << strategy.name
              << ", enabled: " << (strategy.enabled ? "true" : "false")
              << ", externalPremiumThresholdPpm: "
              << strategy.externalPremiumThresholdPpm.raw()
              << ", takeProfitPremiumThresholdPpm: "
              << strategy.takeProfitPremiumThresholdPpm.raw()
              << ", returnTakeProfitRatioPpm: " << strategy.returnTakeProfitRatioPpm.raw()
              << ", addPremiumLimitPpm: " << strategy.addPremiumLimitPpm.raw()
              << ", adjustmentFactor1e6: " << strategy.adjustmentFactor1e6 << '\n';
}

void printOtcChannel(const OtcChannelRecord& channel)
{
    std::cout << "id: " << channel.id << ", uid: " << channel.uid.raw()
              << ", strategyCode: " << channel.strategyCode.raw()
              << ", actualCode: " << channel.actualCode.raw()
              << ", fundClass: " << channel.fundClass
              << ", enabled: " << (channel.enabled ? "true" : "false")
              << ", dailyLimitCents: " << channel.dailyLimitCents.raw()
              << ", priority: " << channel.priority
              << ", minBuyAmountCents: " << channel.minBuyAmountCents.raw() << '\n';
}

void printSchemaVersion(const SchemaVersionRecord& version)
{
    std::cout << "id: " << version.id << ", uid: " << version.uid.raw()
              << ", version: " << version.version
              << ", migrationName: " << version.migrationName
              << ", checksum: " << version.checksum
              << ", appliedAtUtc: " << version.appliedAtUtc << '\n';
}

int initDatabase(const std::filesystem::path& dbPath, const std::filesystem::path& migrationPath)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath;
    config.migrationFilePath = migrationPath;
    config.createIfMissing = true;

    etfdt::data_access::SQLiteConnection connection;
    auto openResult = connection.open(config);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    if (!migrationResult) {
        return printDatabaseError(migrationResult.error());
    }

    auto health = connection.healthCheck();
    if (!health) {
        return printDatabaseError(health.error());
    }

    printStatus(health.value());
    return 0;
}

int checkDatabase(const std::filesystem::path& dbPath)
{
    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    auto health = connection.healthCheck();
    if (!health) {
        return printDatabaseError(health.error());
    }

    printStatus(health.value());
    return 0;
}

int printDatabaseSummary(const std::filesystem::path& dbPath)
{
    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    etfdt::data_access::SchemaVersionRepository schemaVersions(connection);
    etfdt::data_access::AccountRepository accounts(connection);
    etfdt::data_access::PortfolioRepository portfolios(connection);
    etfdt::data_access::InstrumentRepository instruments(connection);
    etfdt::data_access::StrategyRepository strategies(connection);
    etfdt::data_access::OtcChannelRepository otcChannels(connection);

    auto versions = schemaVersions.listVersions();
    if (!versions) {
        return printDatabaseError(versions.error());
    }
    auto activeAccount = accounts.getActiveAccount();
    if (!activeAccount) {
        return printDatabaseError(activeAccount.error());
    }
    auto activePortfolio = portfolios.getActivePortfolio();
    if (!activePortfolio) {
        return printDatabaseError(activePortfolio.error());
    }
    auto enabledInstruments = instruments.listEnabledInstruments();
    if (!enabledInstruments) {
        return printDatabaseError(enabledInstruments.error());
    }
    auto enabledStrategies = strategies.listEnabledStrategies();
    if (!enabledStrategies) {
        return printDatabaseError(enabledStrategies.error());
    }
    auto otcCount = otcChannels.countAll();
    if (!otcCount) {
        return printDatabaseError(otcCount.error());
    }

    std::cout << "schemaVersionLatest: ";
    if (versions.value().empty()) {
        std::cout << "none\n";
    }
    else {
        const auto& latest = versions.value().back();
        std::cout << latest.version << " " << latest.migrationName << " "
                  << latest.appliedAtUtc << '\n';
    }
    std::cout << "activeAccount: " << activeAccount.value().id << " "
              << activeAccount.value().name << '\n';
    std::cout << "activePortfolio: " << activePortfolio.value().id << " "
              << activePortfolio.value().name << '\n';
    std::cout << "enabledInstrumentCount: " << enabledInstruments.value().size() << '\n';
    std::cout << "enabledStrategyCount: " << enabledStrategies.value().size() << '\n';
    std::cout << "otcChannelCount: " << otcCount.value() << '\n';
    return 0;
}

int listAccounts(const std::filesystem::path& dbPath)
{
    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    etfdt::data_access::AccountRepository repository(connection);
    auto records = repository.listAccounts();
    if (!records) {
        return printDatabaseError(records.error());
    }

    std::cout << "accounts: " << records.value().size() << '\n';
    for (const auto& record : records.value()) {
        printAccount(record);
    }
    return 0;
}

int listPortfolios(const std::filesystem::path& dbPath)
{
    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    etfdt::data_access::PortfolioRepository repository(connection);
    auto records = repository.listPortfolios();
    if (!records) {
        return printDatabaseError(records.error());
    }

    std::cout << "portfolios: " << records.value().size() << '\n';
    for (const auto& record : records.value()) {
        printPortfolio(record);
    }
    return 0;
}

int listInstruments(const std::filesystem::path& dbPath)
{
    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    etfdt::data_access::InstrumentRepository repository(connection);
    auto records = repository.listEnabledInstruments();
    if (!records) {
        return printDatabaseError(records.error());
    }

    std::cout << "enabledInstruments: " << records.value().size() << '\n';
    for (const auto& record : records.value()) {
        printInstrument(record);
    }
    return 0;
}

int listStrategies(const std::filesystem::path& dbPath)
{
    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    etfdt::data_access::StrategyRepository repository(connection);
    auto records = repository.listEnabledStrategies();
    if (!records) {
        return printDatabaseError(records.error());
    }

    std::cout << "enabledStrategies: " << records.value().size() << '\n';
    for (const auto& record : records.value()) {
        printStrategy(record);
    }
    return 0;
}

int listOtcChannels(const std::filesystem::path& dbPath, const std::string& strategyCode)
{
    if (strategyCode.empty()) {
        return printQueryError("--strategy-code <code> is required for --list-otc");
    }

    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    etfdt::data_access::OtcChannelRepository repository(connection);
    auto records = repository.listAllByStrategyCode(strategyCode);
    if (!records) {
        return printDatabaseError(records.error());
    }
    if (records.value().empty()) {
        return printQueryError("No OTC channels found for strategy code: " + strategyCode);
    }

    std::cout << "otcChannels: " << records.value().size() << '\n';
    for (const auto& record : records.value()) {
        printOtcChannel(record);
    }
    return 0;
}

int serveReadOnly(
    int argc,
    char* argv[],
    const std::filesystem::path& dbPath,
    const std::string& socketName)
{
    if (socketName.empty()) {
        std::cerr << "--socket-name <name> is required for --serve-readonly\n";
        return 1;
    }
    const auto normalizedSocketName = etfdt::transport::normalizeLocalSocketName(socketName);

    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    QCoreApplication app(argc, argv);
    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    auto listenResult = host.listen(normalizedSocketName);
    if (!listenResult) {
        std::cerr << "Failed to start read-only service host: "
                  << listenResult.error().message << '\n';
        return 1;
    }

    std::cout << "ETFDataService read-only service listening on " << normalizedSocketName << '\n';
    std::cout << "Development preview: only system.* and data.* read-only actions are enabled.\n";
    const int exitCode = app.exec();
    host.close();
    return exitCode;
}

int serveDailyUse(
    int argc,
    char* argv[],
    const std::filesystem::path& dbPath,
    const std::string& socketName)
{
    if (socketName.empty()) {
        std::cerr << "--socket-name <name> is required for --serve-daily-use\n";
        return 1;
    }
    const auto normalizedSocketName = etfdt::transport::normalizeLocalSocketName(socketName);

    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    QCoreApplication app(argc, argv);
    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceDailyUseActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    auto listenResult = host.listen(normalizedSocketName);
    if (!listenResult) {
        std::cerr << "Failed to start daily-use service host: "
                  << listenResult.error().message << '\n';
        return 1;
    }

    std::cout << "ETFDataService daily-use service listening on "
              << normalizedSocketName << '\n';
    std::cout << "Daily-use mode: read-only dashboard actions and accepted "
              << "Excel/VBA import persistence are enabled; broker, order, "
              << "and automaticTrading actions are not registered.\n";
    const int exitCode = app.exec();
    host.close();
    return exitCode;
}

int serveDevAudit(
    int argc,
    char* argv[],
    const std::filesystem::path& dbPath,
    const std::string& socketName)
{
    if (socketName.empty()) {
        std::cerr << "--socket-name <name> is required for --serve-dev-audit\n";
        return 1;
    }
    const auto normalizedSocketName = etfdt::transport::normalizeLocalSocketName(socketName);

    etfdt::data_access::SQLiteConnection connection;
    auto openResult = openHealthyReadOnlyDatabase(dbPath, connection);
    if (!openResult) {
        return printDatabaseError(openResult.error());
    }

    QCoreApplication app(argc, argv);
    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    etfdt::data_service_api::registerDataServiceWriteActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    auto listenResult = host.listen(normalizedSocketName);
    if (!listenResult) {
        std::cerr << "Failed to start dev audit service host: "
                  << listenResult.error().message << '\n';
        return 1;
    }

    std::cout << "ETFDataService dev audit service listening on " << normalizedSocketName << '\n';
    std::cout << "Development preview: enabled write actions are data.audit.append and "
              << "accounting.snapshot.write; snapshot write is allowlisted to derived "
              << "snapshot tables only.\n";
    const int exitCode = app.exec();
    host.close();
    return exitCode;
}

}  // namespace

int main(int argc, char* argv[])
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (args.empty() || hasOption(args, "--help")) {
        printHelp();
        return 0;
    }

    const std::string dbPath = optionValue(args, "--db");
    if (dbPath.empty()) {
        std::cerr << "--db <path> is required\n";
        return 1;
    }

    if (hasOption(args, "--init-db")) {
        const std::string migrationPath = optionValue(args, "--migration");
        if (migrationPath.empty()) {
            std::cerr << "--migration <path> is required for --init-db\n";
            return 1;
        }
        return initDatabase(dbPath, migrationPath);
    }

    if (hasOption(args, "--check-db")) {
        return checkDatabase(dbPath);
    }

    if (hasOption(args, "--db-summary")) {
        return printDatabaseSummary(dbPath);
    }

    if (hasOption(args, "--list-accounts")) {
        return listAccounts(dbPath);
    }

    if (hasOption(args, "--list-portfolios")) {
        return listPortfolios(dbPath);
    }

    if (hasOption(args, "--list-instruments")) {
        return listInstruments(dbPath);
    }

    if (hasOption(args, "--list-strategies")) {
        return listStrategies(dbPath);
    }

    if (hasOption(args, "--list-otc")) {
        return listOtcChannels(dbPath, optionValue(args, "--strategy-code"));
    }

    if (hasOption(args, "--serve-readonly")) {
        return serveReadOnly(argc, argv, dbPath, optionValue(args, "--socket-name"));
    }

    if (hasOption(args, "--serve-daily-use")) {
        return serveDailyUse(argc, argv, dbPath, optionValue(args, "--socket-name"));
    }

    if (hasOption(args, "--serve-dev-audit")) {
        return serveDevAudit(argc, argv, dbPath, optionValue(args, "--socket-name"));
    }

    printHelp();
    return 1;
}
