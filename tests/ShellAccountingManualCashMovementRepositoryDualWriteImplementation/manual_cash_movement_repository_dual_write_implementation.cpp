#include "DataAccess/ShellAccountingManualCashMovementRepository.h"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace fs = std::filesystem;

struct Harness {
    fs::path root;
    std::string testCase;
};

struct MovableSQLiteConnection {
    MovableSQLiteConnection()
        : connection(std::make_unique<etfdt::data_access::SQLiteConnection>())
    {
    }

    MovableSQLiteConnection(const MovableSQLiteConnection&) = delete;
    MovableSQLiteConnection& operator=(const MovableSQLiteConnection&) = delete;
    MovableSQLiteConnection(MovableSQLiteConnection&&) noexcept = default;
    MovableSQLiteConnection& operator=(MovableSQLiteConnection&&) noexcept = default;

    template <typename Config>
    decltype(auto) open(const Config& config)
    {
        return connection->open(config);
    }

    decltype(auto) applyMigrationFile(const fs::path& path)
    {
        return connection->applyMigrationFile(path);
    }

    decltype(auto) executeStatement(
        const std::string& sql,
        const std::vector<etfdt::data_access::SqlStatementParameter>& parameters = {})
    {
        return connection->executeStatement(sql, parameters);
    }

    decltype(auto) queryRows(
        const std::string& sql,
        const std::vector<std::string>& parameters = {})
    {
        return connection->queryRows(sql, parameters);
    }

    decltype(auto) lastInsertRowId() const
    {
        return connection->lastInsertRowId();
    }

    operator etfdt::data_access::SQLiteConnection&()
    {
        return *connection;
    }

    std::unique_ptr<etfdt::data_access::SQLiteConnection> connection;
};

struct DbFixture {
    fs::path directory;
    MovableSQLiteConnection connection;
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
};

std::string readFile(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Unable to read " + path.string());
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

bool contains(const std::string& text, const std::string& token)
{
    return text.find(token) != std::string::npos;
}

void requireContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(contains(text, token), context + " missing `" + token + "`");
}

void requireNotContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(!contains(text, token), context + " unexpectedly contains `" + token + "`");
}

void requireAll(const std::string& text, const std::vector<std::string>& tokens, const std::string& context)
{
    for (const auto& token : tokens) {
        requireContains(text, token, context);
    }
}

std::vector<fs::path> filesUnder(const fs::path& root)
{
    std::vector<fs::path> files;
    if (!fs::exists(root)) {
        return files;
    }
    for (const auto& entry : fs::recursive_directory_iterator(root)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }
    return files;
}

std::string joinFiles(const std::vector<fs::path>& files)
{
    std::string text;
    for (const auto& file : files) {
        text += "\n";
        text += readFile(file);
    }
    return text;
}

std::int64_t singleInt64(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    require(rows.hasValue(), "query failed: " + sql);
    require(!rows.value().empty() && !rows.value().front().empty(), "query returned no row: " + sql);
    return rows.value().front().front().int64Value;
}

std::string singleText(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    require(rows.hasValue(), "query failed: " + sql);
    require(!rows.value().empty() && !rows.value().front().empty(), "query returned no row: " + sql);
    return rows.value().front().front().text;
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    return static_cast<int>(singleInt64(connection, "SELECT COUNT(*) FROM " + table + ";"));
}

DbFixture makeDb(const Harness& h)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task196_manual_cash_movement_" + h.testCase + "_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "manual_cash_movement.sqlite";
    config.enableWal = false;
    require(fixture.connection.open(config).hasValue(), "open sqlite");
    require(fixture.connection.applyMigrationFile(h.root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply 001");
    require(fixture.connection.applyMigrationFile(
                  h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
                  .hasValue(),
        "apply 002");

    auto account = fixture.connection.executeStatement(
        "INSERT INTO account(uid,name,account_type,base_currency,is_active,initial_cash_cents,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-196-account"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-196 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-196 repository test account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    require(account.hasValue(), "insert account");
    fixture.accountId = fixture.connection.lastInsertRowId();

    auto portfolio = fixture.connection.executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-196-portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-196 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("manual cash movement repository tests"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    require(portfolio.hasValue(), "insert portfolio");
    fixture.portfolioId = fixture.connection.lastInsertRowId();

    return fixture;
}

etfdt::data_access::ShellAccountingManualCashMovementWriteCommand command(
    const DbFixture& fixture,
    std::string movementType = "Deposit",
    std::string requestId = "task-196-request-1",
    std::string idempotencyKey = "task-196-idempotency-1")
{
    etfdt::data_access::ShellAccountingManualCashMovementWriteCommand cmd;
    cmd.accountId = fixture.accountId;
    cmd.portfolioId = fixture.portfolioId;
    cmd.movementType = std::move(movementType);
    cmd.amountMinor = 12345;
    cmd.currencyCode = "CNY";
    cmd.occurredAtUtc = "2026-06-02T12:34:56Z";
    cmd.sourceMemo = "manual cash movement memo\nwith whitespace";
    cmd.sourceReference = "bank-reference-196";
    cmd.requestId = std::move(requestId);
    cmd.idempotencyKey = std::move(idempotencyKey);
    return cmd;
}

etfdt::data_access::ShellAccountingManualCashMovementWriteResult persist(
    DbFixture& fixture,
    const etfdt::data_access::ShellAccountingManualCashMovementWriteCommand& cmd)
{
    etfdt::data_access::ShellAccountingManualCashMovementRepository repository(fixture.connection);
    return repository.persistManualCashMovement(cmd);
}

void requireSuccess(const etfdt::data_access::ShellAccountingManualCashMovementWriteResult& result)
{
    require(result.accepted, "result accepted");
    require(result.success, "result success");
    require(result.writeImplemented, "write implemented");
    require(result.databaseWritten, "database written");
    require(result.tradeLogWritten, "trade_log written");
    require(result.cashAdjustmentWritten, "cash_adjustment written");
    require(!result.auditWritten, "audit not written");
    require(!result.ledgerWritten, "ledger flag remains false");
    require(result.transactionCommitted, "transaction committed");
    require(result.status == "OK", "status OK");
    require(result.tradeLogId > 0, "trade_log id returned");
    require(result.cashAdjustmentId > 0, "cash_adjustment id returned");
}

std::string repositorySource(const Harness& h)
{
    return readFile(h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualCashMovementRepository.cpp");
}

std::string repositoryHeader(const Harness& h)
{
    return readFile(h.root / "libs" / "DataAccess" / "include" / "DataAccess"
        / "ShellAccountingManualCashMovementRepository.h");
}

std::string gitDiff(const Harness& h, const std::vector<std::string>& paths)
{
    std::ostringstream command;
    command << "git -C \"" << h.root.string() << "\" diff --name-only main";
    for (const auto& path : paths) {
        command << " -- \"" << path << "\"";
    }
    FILE* pipe = _popen(command.str().c_str(), "r");
    require(pipe != nullptr, "open git diff pipe");
    char buffer[512];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    const int rc = _pclose(pipe);
    require(rc == 0, "git diff command failed");
    return output;
}

void testDocs(const Harness& h)
{
    const auto doc188 = readFile(h.root / "docs" / "188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md");
    const auto doc189 = readFile(h.root / "docs" / "189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md");
    requireAll(doc188, {
        "TASK-196",
        "manual cash movement repository dual-write implementation",
        "trade_log + cash_adjustment",
        "DataAccess-only",
        "direct repository tests",
        "does not modify DataServiceActions",
        "does not write audit_log",
        "does not write ledger",
        "does not call AccountingEngine replay",
        "does not call broker",
        "Broker sandbox new capability development remains paused",
    }, "docs/188");
    requireAll(doc189, {
        "TASK-196",
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "Deposit command writes successfully",
        "Withdrawal command writes successfully",
        "duplicate idempotencyKey",
        "rollback / failure case",
    }, "docs/189");
}

void testDocsIndexPrompt(const Harness& h)
{
    requireContains(readFile(h.root / "README.md"),
        "docs/188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md",
        "README");
    requireContains(readFile(h.root / "README.md"),
        "docs/189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md",
        "README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "12_codex_prompt_template.md"), "TASK-196", "docs/12");
}

void testMigrationsUnmodified(const Harness& h)
{
    require(gitDiff(h, {"migrations"}).empty(), "TASK-196 must not modify migrations");
}

void testNoNewMigration(const Harness& h)
{
    for (const auto& file : filesUnder(h.root / "migrations")) {
        require(file.filename().string() != "003_shell_accounting_manual_cash_movement_repository.sql",
            "TASK-196 must not add migration");
    }
}

void testNoSchemaFile(const Harness& h)
{
    for (const auto& file : filesUnder(h.root)) {
        require(file.extension() != ".schema", "TASK-196 must not add schema side file");
    }
}

void testDataServiceActionsUnmodified(const Harness& h)
{
    require(gitDiff(h, {
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    }).empty(), "DataServiceActions and registrar must not change");
}

void testValidationCodeUnmodified(const Harness& h)
{
    require(gitDiff(h, {
        "libs/DataServiceApi/include/DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h",
        "libs/DataServiceApi/src/ShellAccountingManualTransactionCashMovementValidation.cpp",
    }).empty(), "TASK-178 validation production code must not change");
}

void testTask192RepositoryUnmodified(const Harness& h)
{
    require(gitDiff(h, {
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
    }).empty(), "TASK-192 manual transaction repository must not change");
}

void testRepositoryExists(const Harness& h)
{
    require(fs::exists(h.root / "libs" / "DataAccess" / "include" / "DataAccess"
                / "ShellAccountingManualCashMovementRepository.h"),
        "repository header exists");
    require(fs::exists(h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualCashMovementRepository.cpp"),
        "repository source exists");
}

void testDataAccessOnly(const Harness& h)
{
    requireContains(readFile(h.root / "libs" / "DataAccess" / "CMakeLists.txt"),
        "ShellAccountingManualCashMovementRepository.cpp",
        "DataAccess CMake");
}

void testNotInDataServiceApi(const Harness& h)
{
    requireNotContains(joinFiles(filesUnder(h.root / "libs" / "DataServiceApi")),
        "ShellAccountingManualCashMovementRepository",
        "DataServiceApi scan");
}

void testNotInShell(const Harness& h)
{
    requireNotContains(joinFiles(filesUnder(h.root / "apps" / "ETFDecisionShell")),
        "ShellAccountingManualCashMovementRepository",
        "Shell scan");
    requireNotContains(joinFiles(filesUnder(h.root / "libs" / "ShellServices")),
        "ShellAccountingManualCashMovementRepository",
        "ShellServices scan");
}

void testSqliteApply001002(const Harness& h)
{
    auto fixture = makeDb(h);
    require(countRows(fixture.connection, "trade_log") == 0, "trade_log initially empty");
    require(countRows(fixture.connection, "cash_adjustment") == 0, "cash_adjustment initially empty");
}

void testDepositSuccess(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit")));
}

void testWithdrawalSuccess(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Withdrawal")));
}

void testDepositWithdrawalTradeLog(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "req-deposit", "idem-deposit")));
    requireSuccess(persist(fixture, command(fixture, "Withdrawal", "req-withdraw", "idem-withdraw")));
    require(countRows(fixture.connection, "trade_log") == 2, "Deposit and Withdrawal write trade_log");
}

void testDepositWithdrawalCashAdjustment(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "req-deposit", "idem-deposit")));
    requireSuccess(persist(fixture, command(fixture, "Withdrawal", "req-withdraw", "idem-withdraw")));
    require(countRows(fixture.connection, "cash_adjustment") == 2, "Deposit and Withdrawal write cash_adjustment");
}

void testCashAdjustmentTradeLogIdLink(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto result = persist(fixture, command(fixture));
    requireSuccess(result);
    require(singleInt64(fixture.connection, "SELECT trade_log_id FROM cash_adjustment ORDER BY id DESC LIMIT 1;")
            == result.tradeLogId,
        "cash_adjustment.trade_log_id points to trade_log.id");
}

void testTradeLogCashAdjustmentUidLink(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto result = persist(fixture, command(fixture));
    requireSuccess(result);
    require(singleText(fixture.connection, "SELECT cash_adjustment_uid FROM trade_log ORDER BY id DESC LIMIT 1;")
            == result.cashAdjustmentUid,
        "trade_log.cash_adjustment_uid matches cash_adjustment uid");
}

void testCashAdjustmentTradeLogUidLink(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto result = persist(fixture, command(fixture));
    requireSuccess(result);
    require(singleText(fixture.connection, "SELECT trade_log_uid FROM cash_adjustment ORDER BY id DESC LIMIT 1;")
            == result.tradeLogUid,
        "cash_adjustment.trade_log_uid matches trade_log uid");
}

void testTradeLogRequestIdWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "request-written", "idem-request-written")));
    require(singleText(fixture.connection, "SELECT request_id FROM trade_log ORDER BY id DESC LIMIT 1;")
            == "request-written",
        "trade_log request_id written");
}

void testTradeLogIdempotencyKeyWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "req-idem", "idem-written")));
    require(singleText(fixture.connection, "SELECT idempotency_key FROM trade_log ORDER BY id DESC LIMIT 1;")
            == "idem-written",
        "trade_log idempotency_key written");
}

void testTradeLogOccurredAtWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(singleText(fixture.connection, "SELECT occurred_at_utc FROM trade_log ORDER BY id DESC LIMIT 1;")
            == "2026-06-02T12:34:56Z",
        "trade_log occurred_at_utc written");
}

void testTradeLogSourceMemoSanitized(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    const auto memo = singleText(fixture.connection, "SELECT source_memo_sanitized FROM trade_log ORDER BY id DESC LIMIT 1;");
    requireContains(memo, "manual cash movement memo", "trade_log memo");
    requireNotContains(memo, "\n", "trade_log memo");
}

void testCashAdjustmentRequestIdWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "cash-request-written", "cash-idem-request")));
    require(singleText(fixture.connection, "SELECT request_id FROM cash_adjustment ORDER BY id DESC LIMIT 1;")
            == "cash-request-written",
        "cash_adjustment request_id written");
}

void testCashAdjustmentIdempotencyKeyWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "cash-req-idem", "cash-idem-written")));
    require(singleText(fixture.connection, "SELECT idempotency_key FROM cash_adjustment ORDER BY id DESC LIMIT 1;")
            == "cash-idem-written",
        "cash_adjustment idempotency_key written");
}

void testCashAdjustmentOccurredAtWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(singleText(fixture.connection, "SELECT occurred_at_utc FROM cash_adjustment ORDER BY id DESC LIMIT 1;")
            == "2026-06-02T12:34:56Z",
        "cash_adjustment occurred_at_utc written");
}

void testCashAdjustmentSourceMemoSanitized(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    const auto memo = singleText(fixture.connection, "SELECT source_memo_sanitized FROM cash_adjustment ORDER BY id DESC LIMIT 1;");
    requireContains(memo, "manual cash movement memo", "cash_adjustment memo");
    requireNotContains(memo, "\n", "cash_adjustment memo");
}

void testAmountSignPolicy(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "req-deposit", "idem-deposit")));
    requireSuccess(persist(fixture, command(fixture, "Withdrawal", "req-withdraw", "idem-withdraw")));
    require(singleInt64(fixture.connection, "SELECT amount_cents FROM cash_adjustment WHERE idempotency_key='idem-deposit';")
            == 12345,
        "Deposit cash_adjustment amount positive");
    require(singleInt64(fixture.connection, "SELECT amount_cents FROM cash_adjustment WHERE idempotency_key='idem-withdraw';")
            == -12345,
        "Withdrawal cash_adjustment amount negative");
}

void testTradeLogNetCashImpact(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "req-deposit", "idem-deposit")));
    requireSuccess(persist(fixture, command(fixture, "Withdrawal", "req-withdraw", "idem-withdraw")));
    require(singleInt64(fixture.connection, "SELECT net_cash_impact_cents FROM trade_log WHERE idempotency_key='idem-deposit';")
            == 12345,
        "Deposit net cash impact positive");
    require(singleInt64(fixture.connection, "SELECT net_cash_impact_cents FROM trade_log WHERE idempotency_key='idem-withdraw';")
            == -12345,
        "Withdrawal net cash impact negative");
}

void testCashAdjustmentAmountType(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "req-deposit", "idem-deposit")));
    requireSuccess(persist(fixture, command(fixture, "Withdrawal", "req-withdraw", "idem-withdraw")));
    require(singleText(fixture.connection, "SELECT adjustment_type FROM cash_adjustment WHERE idempotency_key='idem-deposit';")
            == "CASH_IN",
        "Deposit adjustment type CASH_IN");
    require(singleText(fixture.connection, "SELECT adjustment_type FROM cash_adjustment WHERE idempotency_key='idem-withdraw';")
            == "CASH_OUT",
        "Withdrawal adjustment type CASH_OUT");
}

void testAccountPortfolioFields(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto result = persist(fixture, command(fixture));
    requireSuccess(result);
    require(singleInt64(fixture.connection, "SELECT account_id FROM trade_log ORDER BY id DESC LIMIT 1;")
            == fixture.accountId,
        "trade_log account");
    require(singleInt64(fixture.connection, "SELECT portfolio_id FROM cash_adjustment ORDER BY id DESC LIMIT 1;")
            == fixture.portfolioId,
        "cash_adjustment portfolio");
}

void testDuplicateIdempotencyNoExtraTradeLog(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto cmd = command(fixture, "Deposit", "req-dup", "idem-dup");
    requireSuccess(persist(fixture, cmd));
    const auto duplicate = persist(fixture, cmd);
    require(duplicate.duplicate, "duplicate marked");
    require(countRows(fixture.connection, "trade_log") == 1, "duplicate idempotency does not add trade_log");
}

void testDuplicateIdempotencyNoExtraCashAdjustment(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto cmd = command(fixture, "Deposit", "req-dup", "idem-dup");
    requireSuccess(persist(fixture, cmd));
    const auto duplicate = persist(fixture, cmd);
    require(duplicate.duplicate, "duplicate marked");
    require(countRows(fixture.connection, "cash_adjustment") == 1, "duplicate idempotency does not add cash_adjustment");
}

void testDuplicateIdempotencyResult(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto cmd = command(fixture, "Deposit", "req-dup-result", "idem-dup-result");
    requireSuccess(persist(fixture, cmd));
    const auto duplicate = persist(fixture, cmd);
    require(duplicate.success, "duplicate result success");
    require(duplicate.idempotentReplay, "duplicate result idempotent");
    require(duplicate.status == "IDEMPOTENT_REPLAY", "duplicate result status");
}

void testEmptyIdempotencyStable(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "Deposit", "req-empty-1", "")));
    requireSuccess(persist(fixture, command(fixture, "Deposit", "req-empty-2", "")));
    require(countRows(fixture.connection, "trade_log") == 2, "empty idempotency allows independent requests");
    require(countRows(fixture.connection, "cash_adjustment") == 2, "empty idempotency allows independent cash rows");
}

void testInvalidCommandNoTradeLog(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture);
    cmd.amountMinor = 0;
    const auto result = persist(fixture, cmd);
    require(!result.success, "invalid command rejected");
    require(countRows(fixture.connection, "trade_log") == 0, "invalid command writes no trade_log");
}

void testInvalidCommandNoCashAdjustment(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture);
    cmd.amountMinor = -1;
    const auto result = persist(fixture, cmd);
    require(!result.success, "invalid command rejected");
    require(countRows(fixture.connection, "cash_adjustment") == 0, "invalid command writes no cash_adjustment");
}

void testInvalidMovementTypeFailClosed(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture, "Adjustment");
    const auto result = persist(fixture, cmd);
    require(!result.success, "Adjustment rejected");
    require(result.status == "VALIDATION_REJECTED", "Adjustment fail closed");
}

void testAdjustmentUnauthorizedFailClosed(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture, "CashAdjustment");
    const auto result = persist(fixture, cmd);
    require(!result.success, "unauthorized adjustment rejected");
    require(countRows(fixture.connection, "trade_log") == 0, "unauthorized adjustment writes no trade_log");
}

void testReferenceFailureNoPartialRows(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture);
    cmd.portfolioId = 999999;
    const auto result = persist(fixture, cmd);
    require(!result.success, "missing portfolio rejected");
    require(countRows(fixture.connection, "trade_log") == 0, "missing reference writes no trade_log");
    require(countRows(fixture.connection, "cash_adjustment") == 0, "missing reference writes no cash_adjustment");
}

void insertCashAdjustmentUidConflict(DbFixture& fixture, const std::string& conflictingUid)
{
    auto log = fixture.connection.executeStatement(
        "INSERT INTO trade_log(uid,account_id,portfolio_id,action_type,trade_source,trade_intent_type,"
        "amount_cents,net_cash_impact_cents,manual_entry,created_at_utc,idempotency_key) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-196-conflict-log"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(fixture.accountId),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(fixture.portfolioId),
         etfdt::data_access::SqlStatementParameter::textValue("CASH_IN"),
         etfdt::data_access::SqlStatementParameter::textValue("MANUAL"),
         etfdt::data_access::SqlStatementParameter::textValue("CASH_EVENT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z"),
         etfdt::data_access::SqlStatementParameter::textValue("other-idempotency")});
    require(log.hasValue(), "insert conflict trade_log");
    const auto tradeLogId = fixture.connection.lastInsertRowId();
    auto cash = fixture.connection.executeStatement(
        "INSERT INTO cash_adjustment(uid,trade_log_id,account_id,portfolio_id,adjustment_type,amount_cents,reason,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue(conflictingUid),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(tradeLogId),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(fixture.accountId),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(fixture.portfolioId),
         etfdt::data_access::SqlStatementParameter::textValue("CASH_IN"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("conflict fixture"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    require(cash.hasValue(), "insert conflict cash_adjustment");
}

void testCashAdjustmentInsertFailureRollsBackTradeLog(const Harness& h)
{
    auto fixture = makeDb(h);
    insertCashAdjustmentUidConflict(fixture, "task-196-manual-cash-adjustment-idem-conflict");
    auto cmd = command(fixture, "Deposit", "req-conflict", "idem-conflict");
    const auto result = persist(fixture, cmd);
    require(!result.success, "cash adjustment unique conflict rejected");
    require(countRows(fixture.connection, "trade_log") == 1, "new trade_log rolled back after cash_adjustment failure");
    require(countRows(fixture.connection, "cash_adjustment") == 1, "no new cash_adjustment after conflict");
}

void testRollbackFailureNoPartialFacts(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture);
    cmd.currencyCode = "USD";
    const auto result = persist(fixture, cmd);
    require(!result.success, "currency mismatch rejected");
    require(countRows(fixture.connection, "trade_log") == 0, "currency mismatch writes no trade_log");
    require(countRows(fixture.connection, "cash_adjustment") == 0, "currency mismatch writes no cash_adjustment");
}

void testNoAuditLogWrite(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(countRows(fixture.connection, "audit_log") == 0, "audit_log not written");
}

void testNoLedgerWrite(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto result = persist(fixture, command(fixture));
    require(!result.ledgerWritten, "ledger flag remains false");
}

void testNoAccountingReplay(const Harness& h)
{
    requireNotContains(repositorySource(h), "AccountingEngine", "manual cash movement repository");
    requireNotContains(repositorySource(h), "replay", "manual cash movement repository");
}

void testNoTradeDraftSuggestion(const Harness& h)
{
    requireNotContains(repositorySource(h), "TradeDraft", "manual cash movement repository");
    requireNotContains(repositorySource(h), "suggest", "manual cash movement repository");
}

void testNoBroker(const Harness& h)
{
    requireNotContains(repositorySource(h), "Broker", "manual cash movement repository");
    requireNotContains(repositorySource(h), "broker", "manual cash movement repository");
}

void testNoNetworkEndpoint(const Harness& h)
{
    for (const auto& token : {"endpoint configuration", "network call", "WinHttp", "curl_easy", "socket("}) {
        requireNotContains(repositorySource(h), token, "manual cash movement repository");
    }
}

void testNoCredentials(const Harness& h)
{
    for (const auto& token : {"credential store", "secret store", "password value", "apiKey"}) {
        requireNotContains(repositorySource(h), token, "manual cash movement repository");
    }
}

void testNoRealOrder(const Harness& h)
{
    for (const auto& token : {"placeOrder", "executeOrder", "brokerOrderId", "realOrderId"}) {
        requireNotContains(repositorySource(h), token, "manual cash movement repository");
    }
}

void testNoAutomaticTrading(const Harness& h)
{
    for (const auto& token : {"automaticTrading", "autoTrade", "strategyExecute"}) {
        requireNotContains(repositorySource(h), token, "manual cash movement repository");
    }
}

void testTask195ContractRetained(const Harness& h)
{
    requireContains(readFile(h.root / "docs" / "186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md"),
        "TASK-196 implements DataAccess-only manual cash movement repository dual-write",
        "docs/186 TASK-196 evolution");
}

void testTask194GateEvolved(const Harness& h)
{
    requireContains(readFile(h.root / "docs" / "184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md"),
        "TASK-196 implements DataAccess-only manual cash movement repository dual-write",
        "docs/184 TASK-196 evolution");
}

void testTask192SemanticsRetained(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualTransactionRepositoryWriteImplementation" / "CMakeLists.txt"),
        "shell_accounting_manual_transaction_repository_write_implementation_manual_buy_success",
        "TASK-192 tests retained");
}

void testTask190MigrationRetained(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualEntrySchemaMigrationImplementation" / "CMakeLists.txt"),
        "shell_accounting_manual_entry_schema_migration_implementation_gate",
        "TASK-190 tests retained");
}

void testTask182ValidationOnly(const Harness& h)
{
    requireContains(readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp"),
        "\"writeImplemented\\\":false",
        "TASK-182 DataService validation-only response");
}

void testTask178ValidationRetained(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualTransactionCashMovementValidationScaffold" / "CMakeLists.txt"),
        "shell_accounting_manual_transaction_cash_movement_validation_scaffold_buy_valid",
        "TASK-178 tests retained");
}

void testBrokerGatesRetained(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "CMakeLists.txt");
    requireAll(cmake, {
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
    }, "broker gates retained");
}

void testResultFlags(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
}

void testSensitiveMemoRejected(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture);
    cmd.sourceMemo = "contains secret token";
    const auto result = persist(fixture, cmd);
    require(!result.success, "sensitive memo rejected");
    require(countRows(fixture.connection, "trade_log") == 0, "sensitive memo writes no trade_log");
    require(countRows(fixture.connection, "cash_adjustment") == 0, "sensitive memo writes no cash_adjustment");
}

void runCase(const Harness& h)
{
    if (h.testCase == "docs") return testDocs(h);
    if (h.testCase == "docs_index_prompt") return testDocsIndexPrompt(h);
    if (h.testCase == "migrations_unmodified") return testMigrationsUnmodified(h);
    if (h.testCase == "no_new_migration") return testNoNewMigration(h);
    if (h.testCase == "no_schema_file") return testNoSchemaFile(h);
    if (h.testCase == "dataserviceactions_unmodified") return testDataServiceActionsUnmodified(h);
    if (h.testCase == "validation_code_unmodified") return testValidationCodeUnmodified(h);
    if (h.testCase == "task192_repository_unmodified") return testTask192RepositoryUnmodified(h);
    if (h.testCase == "repository_exists") return testRepositoryExists(h);
    if (h.testCase == "dataaccess_only") return testDataAccessOnly(h);
    if (h.testCase == "not_in_dataserviceapi") return testNotInDataServiceApi(h);
    if (h.testCase == "not_in_shell") return testNotInShell(h);
    if (h.testCase == "sqlite_apply_001_002") return testSqliteApply001002(h);
    if (h.testCase == "deposit_success") return testDepositSuccess(h);
    if (h.testCase == "withdrawal_success") return testWithdrawalSuccess(h);
    if (h.testCase == "deposit_withdrawal_trade_log") return testDepositWithdrawalTradeLog(h);
    if (h.testCase == "deposit_withdrawal_cash_adjustment") return testDepositWithdrawalCashAdjustment(h);
    if (h.testCase == "cash_adjustment_trade_log_id_link") return testCashAdjustmentTradeLogIdLink(h);
    if (h.testCase == "trade_log_cash_adjustment_uid_link") return testTradeLogCashAdjustmentUidLink(h);
    if (h.testCase == "cash_adjustment_trade_log_uid_link") return testCashAdjustmentTradeLogUidLink(h);
    if (h.testCase == "trade_log_request_id_written") return testTradeLogRequestIdWritten(h);
    if (h.testCase == "trade_log_idempotency_key_written") return testTradeLogIdempotencyKeyWritten(h);
    if (h.testCase == "trade_log_occurred_at_written") return testTradeLogOccurredAtWritten(h);
    if (h.testCase == "trade_log_source_memo_sanitized") return testTradeLogSourceMemoSanitized(h);
    if (h.testCase == "cash_adjustment_request_id_written") return testCashAdjustmentRequestIdWritten(h);
    if (h.testCase == "cash_adjustment_idempotency_key_written") return testCashAdjustmentIdempotencyKeyWritten(h);
    if (h.testCase == "cash_adjustment_occurred_at_written") return testCashAdjustmentOccurredAtWritten(h);
    if (h.testCase == "cash_adjustment_source_memo_sanitized") return testCashAdjustmentSourceMemoSanitized(h);
    if (h.testCase == "amount_sign_policy") return testAmountSignPolicy(h);
    if (h.testCase == "trade_log_net_cash_impact") return testTradeLogNetCashImpact(h);
    if (h.testCase == "cash_adjustment_amount_type") return testCashAdjustmentAmountType(h);
    if (h.testCase == "account_portfolio_fields") return testAccountPortfolioFields(h);
    if (h.testCase == "duplicate_idempotency_no_extra_trade_log") return testDuplicateIdempotencyNoExtraTradeLog(h);
    if (h.testCase == "duplicate_idempotency_no_extra_cash_adjustment") return testDuplicateIdempotencyNoExtraCashAdjustment(h);
    if (h.testCase == "duplicate_idempotency_result") return testDuplicateIdempotencyResult(h);
    if (h.testCase == "empty_idempotency_stable") return testEmptyIdempotencyStable(h);
    if (h.testCase == "invalid_command_no_trade_log") return testInvalidCommandNoTradeLog(h);
    if (h.testCase == "invalid_command_no_cash_adjustment") return testInvalidCommandNoCashAdjustment(h);
    if (h.testCase == "invalid_movement_type_fail_closed") return testInvalidMovementTypeFailClosed(h);
    if (h.testCase == "adjustment_unauthorized_fail_closed") return testAdjustmentUnauthorizedFailClosed(h);
    if (h.testCase == "reference_failure_no_partial_rows") return testReferenceFailureNoPartialRows(h);
    if (h.testCase == "cash_adjustment_insert_failure_rolls_back_trade_log") return testCashAdjustmentInsertFailureRollsBackTradeLog(h);
    if (h.testCase == "rollback_failure_no_partial_facts") return testRollbackFailureNoPartialFacts(h);
    if (h.testCase == "no_audit_log_write") return testNoAuditLogWrite(h);
    if (h.testCase == "no_ledger_write") return testNoLedgerWrite(h);
    if (h.testCase == "no_accounting_replay") return testNoAccountingReplay(h);
    if (h.testCase == "no_tradedraft_suggestion") return testNoTradeDraftSuggestion(h);
    if (h.testCase == "no_broker") return testNoBroker(h);
    if (h.testCase == "no_network_endpoint") return testNoNetworkEndpoint(h);
    if (h.testCase == "no_credentials") return testNoCredentials(h);
    if (h.testCase == "no_real_order") return testNoRealOrder(h);
    if (h.testCase == "no_automatic_trading") return testNoAutomaticTrading(h);
    if (h.testCase == "task195_contract_retained") return testTask195ContractRetained(h);
    if (h.testCase == "task194_gate_evolved") return testTask194GateEvolved(h);
    if (h.testCase == "task192_semantics_retained") return testTask192SemanticsRetained(h);
    if (h.testCase == "task190_migration_retained") return testTask190MigrationRetained(h);
    if (h.testCase == "task182_validation_only") return testTask182ValidationOnly(h);
    if (h.testCase == "task178_validation_retained") return testTask178ValidationRetained(h);
    if (h.testCase == "broker_gates_retained") return testBrokerGatesRetained(h);
    if (h.testCase == "result_flags") return testResultFlags(h);
    if (h.testCase == "sensitive_memo_rejected") return testSensitiveMemoRejected(h);
    throw std::runtime_error("Unknown case: " + h.testCase);
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        Harness harness;
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--source-root" && i + 1 < argc) {
                harness.root = argv[++i];
            } else if (arg == "--case" && i + 1 < argc) {
                harness.testCase = argv[++i];
            }
        }
        require(!harness.root.empty(), "--source-root is required");
        require(!harness.testCase.empty(), "--case is required");
        runCase(harness);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
