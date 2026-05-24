#include "DataAccess/DataAccess.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
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

void expectEqual(int actual, int expected, std::string_view message)
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
        / ("etfdt_transaction_audit_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

etfdt::data_access::AuditLogEntry auditEntry(std::string action)
{
    etfdt::data_access::AuditLogEntry entry;
    entry.entityType = "SYSTEM";
    entry.entityId = 1;
    entry.action = std::move(action);
    entry.oldValueJson = "{}";
    entry.newValueJson = R"({"changed":true})";
    entry.reason = "transaction audit boundary test";
    entry.operatorName = "unit-test";
    entry.createdAtUtc = "2026-05-24T15:00:00Z";
    return entry;
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + table + ";");
    expectTrue(count.hasValue(), "count query succeeds for " + table);
    return count.hasValue() ? count.value() : -1;
}

int countAuditAction(etfdt::data_access::SQLiteConnection& connection, const std::string& action)
{
    auto rows = connection.queryRows(
        "SELECT COUNT(*) FROM audit_log WHERE action = ?;",
        {action});
    expectTrue(rows.hasValue(), "audit_log action count query succeeds");
    if (!rows || rows.value().empty() || rows.value().front().empty()) {
        return -1;
    }
    return static_cast<int>(rows.value().front()[0].int64Value);
}

void testTransactionAndAudit(const std::filesystem::path& migrationPath)
{
    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "transaction_audit.db";

    etfdt::data_access::SQLiteConnection connection;
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath;
    config.migrationFilePath = migrationPath;

    auto openResult = connection.open(config);
    expectTrue(openResult.hasValue(), "SQLiteConnection open succeeds");
    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationResult.hasValue(), "Initial migration succeeds");

    etfdt::data_access::AuditLogRepository auditLogs(connection);

    {
        etfdt::data_access::Transaction transaction(connection);
        auto begin = transaction.begin();
        expectTrue(begin.hasValue(), "Transaction begin succeeds");
        auto inserted = auditLogs.insertAuditLog(auditEntry("COMMIT_TEST"));
        expectTrue(inserted.hasValue(), "audit_log insert succeeds inside transaction");
        auto commit = transaction.commit();
        expectTrue(commit.hasValue(), "Transaction commit succeeds");
    }
    expectEqual(countAuditAction(connection, "COMMIT_TEST"), 1, "committed audit_log exists");

    {
        etfdt::data_access::Transaction transaction(connection);
        auto begin = transaction.begin();
        expectTrue(begin.hasValue(), "Rollback transaction begin succeeds");
        auto inserted = auditLogs.insertAuditLog(auditEntry("ROLLBACK_TEST"));
        expectTrue(inserted.hasValue(), "audit_log insert before rollback succeeds");
        auto rollback = transaction.rollback();
        expectTrue(rollback.hasValue(), "Transaction rollback succeeds");
    }
    expectEqual(countAuditAction(connection, "ROLLBACK_TEST"), 0, "rolled back audit_log absent");

    {
        etfdt::data_access::Transaction transaction(connection);
        auto begin = transaction.begin();
        expectTrue(begin.hasValue(), "RAII transaction begin succeeds");
        auto inserted = auditLogs.insertAuditLog(auditEntry("RAII_ROLLBACK_TEST"));
        expectTrue(inserted.hasValue(), "audit_log insert before RAII rollback succeeds");
    }
    expectEqual(
        countAuditAction(connection, "RAII_ROLLBACK_TEST"),
        0,
        "uncommitted RAII transaction rolls back");

    etfdt::data_access::TransactionRunner runner(connection);
    auto runnerSuccess = runner.runInTransaction([&]() {
        auto inserted = auditLogs.insertAuditLog(auditEntry("RUNNER_COMMIT_TEST"));
        if (!inserted) {
            return etfdt::data_access::DatabaseResult<bool>::failure(
                inserted.error().errorCode,
                inserted.error().message);
        }
        return etfdt::data_access::DatabaseResult<bool>::success(true);
    });
    expectTrue(runnerSuccess.hasValue(), "TransactionRunner commits successful callback");
    expectEqual(
        countAuditAction(connection, "RUNNER_COMMIT_TEST"),
        1,
        "TransactionRunner committed audit_log exists");

    auto runnerFailure = runner.runInTransaction([&]() {
        auto inserted = auditLogs.insertAuditLog(auditEntry("RUNNER_ROLLBACK_TEST"));
        if (!inserted) {
            return etfdt::data_access::DatabaseResult<bool>::failure(
                inserted.error().errorCode,
                inserted.error().message);
        }
        return etfdt::data_access::DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "intentional rollback");
    });
    expectTrue(!runnerFailure.hasValue(), "TransactionRunner returns callback failure");
    expectEqual(
        countAuditAction(connection, "RUNNER_ROLLBACK_TEST"),
        0,
        "TransactionRunner failed callback rolls back audit_log");

    {
        etfdt::data_access::Transaction transaction(connection);
        expectTrue(transaction.begin().hasValue(), "Invalid input transaction begin succeeds");

        auto missingReason = auditEntry("INVALID_REASON");
        missingReason.reason = "";
        expectTrue(
            !auditLogs.insertAuditLog(missingReason).hasValue(),
            "audit_log insert rejects missing reason");

        auto missingEntityType = auditEntry("INVALID_ENTITY_TYPE");
        missingEntityType.entityType = "";
        expectTrue(
            !auditLogs.insertAuditLog(missingEntityType).hasValue(),
            "audit_log insert rejects missing entityType");

        auto invalidOldJson = auditEntry("INVALID_OLD_JSON");
        invalidOldJson.oldValueJson = "not-json";
        expectTrue(
            !auditLogs.insertAuditLog(invalidOldJson).hasValue(),
            "audit_log insert rejects invalid oldValueJson");

        auto invalidNewJson = auditEntry("INVALID_NEW_JSON");
        invalidNewJson.newValueJson = "not-json";
        expectTrue(
            !auditLogs.insertAuditLog(invalidNewJson).hasValue(),
            "audit_log insert rejects invalid newValueJson");

        expectTrue(transaction.rollback().hasValue(), "Invalid input transaction rolls back");
    }

    auto recent = auditLogs.listRecentAuditLogs(5);
    expectTrue(recent.hasValue(), "listRecentAuditLogs succeeds");
    if (recent) {
        expectTrue(!recent.value().empty(), "listRecentAuditLogs returns records");
    }

    expectEqual(countRows(connection, "trade_log"), 0, "trade_log remains empty");
    expectEqual(countRows(connection, "position_snapshot"), 0, "position_snapshot remains empty");
    expectEqual(countRows(connection, "cash_snapshot"), 0, "cash_snapshot remains empty");
    expectEqual(countRows(connection, "portfolio_summary"), 0, "portfolio_summary remains empty");

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

    testTransactionAndAudit(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " transaction audit test failure(s)" << '\n';
        return 1;
    }

    std::cout << "Transaction and audit tests passed." << '\n';
    return 0;
}
