#include "DataAccess/DataAccess.h"
#include "Protocol/ErrorCode.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

void printHelp()
{
    std::cout
        << "ETFDataService database utility\n"
        << "\n"
        << "Usage:\n"
        << "  ETFDataService --init-db --db <path> --migration <path>\n"
        << "  ETFDataService --check-db --db <path>\n"
        << "  ETFDataService --help\n"
        << "\n"
        << "This TASK-004 entry point initializes and checks SQLite only. "
        << "It does not start Local Socket services.\n";
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

int printDatabaseError(const etfdt::data_access::DatabaseError& error)
{
    std::cerr << etfdt::protocol::toString(error.errorCode) << ": " << error.message << '\n';
    return 1;
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
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath;
    config.createIfMissing = false;

    etfdt::data_access::SQLiteConnection connection;
    auto openResult = connection.open(config);
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

    printHelp();
    return 1;
}
