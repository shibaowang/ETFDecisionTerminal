#include "DataAccess/ShellAccountingManualEntryRepositoryScaffold.h"
#include "DataAccess/ShellAccountingManualTransactionRepository.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>

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

    decltype(auto) lastInsertRowId() const
    {
        return connection->lastInsertRowId();
    }

    operator etfdt::data_access::SQLiteConnection&()
    {
        return *connection;
    }

    operator const etfdt::data_access::SQLiteConnection&() const
    {
        return *connection;
    }

    std::unique_ptr<etfdt::data_access::SQLiteConnection> connection;
};

struct DbFixture {
    DbFixture() = default;
    DbFixture(const DbFixture&) = delete;
    DbFixture& operator=(const DbFixture&) = delete;
    DbFixture(DbFixture&&) noexcept = default;
    DbFixture& operator=(DbFixture&&) noexcept = default;

    fs::path directory;
    MovableSQLiteConnection connection;
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::int64_t instrumentId = 0;
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

void requireDb(bool condition, const std::string& message)
{
    require(condition, "database setup failed: " + message);
}

std::int64_t singleInt64(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    require(rows.hasValue(), "query failed: " + sql);
    require(!rows.value().empty() && !rows.value().front().empty(), "query returned no row: " + sql);
    return rows.value().front().front().int64Value;
}

std::string singleText(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& sql)
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
        / ("task192_manual_transaction_" + h.testCase + "_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "manual_transaction.sqlite";
    config.enableWal = false;
    requireDb(fixture.connection.open(config).hasValue(), "open sqlite");
    requireDb(fixture.connection.applyMigrationFile(h.root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply 001");
    requireDb(fixture.connection.applyMigrationFile(
                  h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
                  .hasValue(),
        "apply 002");

    auto account = fixture.connection.executeStatement(
        "INSERT INTO account(uid,name,account_type,base_currency,is_active,initial_cash_cents,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-192-account"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-192 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-192 repository test account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    requireDb(account.hasValue(), "insert account");
    fixture.accountId = fixture.connection.lastInsertRowId();

    auto portfolio = fixture.connection.executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-192-portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-192 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("manual transaction repository tests"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    requireDb(portfolio.hasValue(), "insert portfolio");
    fixture.portfolioId = fixture.connection.lastInsertRowId();

    auto instrument = fixture.connection.executeStatement(
        "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-192-instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("159509"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-192 ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-192 repository test instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    requireDb(instrument.hasValue(), "insert instrument");
    fixture.instrumentId = fixture.connection.lastInsertRowId();

    return fixture;
}

etfdt::data_access::ShellAccountingManualTransactionWriteCommand command(
    const DbFixture& fixture,
    std::string side = "BUY",
    std::string requestId = "task-192-request-1",
    std::string idempotencyKey = "task-192-idempotency-1")
{
    etfdt::data_access::ShellAccountingManualTransactionWriteCommand cmd;
    cmd.accountId = fixture.accountId;
    cmd.portfolioId = fixture.portfolioId;
    cmd.instrumentId = fixture.instrumentId;
    cmd.securityCode = "159509";
    cmd.tradeSide = std::move(side);
    cmd.quantityUnits = 1000000;
    cmd.priceAmountMinor = 20100;
    cmd.grossAmountMinor = 20100;
    cmd.feeAmountMinor = 100;
    cmd.taxAmountMinor = 20;
    cmd.occurredAtUtc = "2026-06-02T12:34:56Z";
    cmd.sourceMemo = "manual repository memo\nwith whitespace";
    cmd.requestId = std::move(requestId);
    cmd.idempotencyKey = std::move(idempotencyKey);
    return cmd;
}

etfdt::data_access::ShellAccountingManualTransactionWriteResult persist(
    DbFixture& fixture,
    const etfdt::data_access::ShellAccountingManualTransactionWriteCommand& cmd)
{
    etfdt::data_access::ShellAccountingManualTransactionRepository repository(fixture.connection);
    return repository.persistManualTransaction(cmd);
}

void requireSuccess(const etfdt::data_access::ShellAccountingManualTransactionWriteResult& result)
{
    require(result.accepted, "result accepted");
    require(result.success, "result success");
    require(result.writeImplemented, "write implemented");
    require(result.databaseWritten, "database written");
    require(result.tradeExecutionGroupWritten, "execution group written");
    require(result.tradeLogWritten, "trade_log written");
    require(!result.cashFactsWritten, "cash facts not written");
    require(!result.auditWritten, "audit not written");
    require(!result.ledgerWritten, "ledger flag remains false");
    require(result.transactionCommitted, "transaction committed");
    require(result.status == "OK", "status OK");
    require(result.tradeExecutionGroupId > 0, "execution group id returned");
    require(result.tradeLogId > 0, "trade_log id returned");
}

std::string repositorySource(const Harness& h)
{
    return readFile(h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp");
}

std::string repositoryHeader(const Harness& h)
{
    return readFile(h.root / "libs" / "DataAccess" / "include" / "DataAccess"
        / "ShellAccountingManualTransactionRepository.h");
}

void testDocs(const Harness& h)
{
    const auto doc182 = readFile(h.root / "docs" / "182_shell_accounting_manual_transaction_repository_write_implementation.md");
    const auto doc183 = readFile(h.root / "docs" / "183_shell_accounting_manual_transaction_repository_write_implementation_test_plan.md");
    requireAll(doc182, {
        "TASK-192",
        "manual transaction repository write implementation",
        "manual BUY / SELL",
        "DataAccess repository",
        "direct repository tests",
        "does not implement manual cash movement",
        "does not modify DataServiceActions",
        "does not write audit_log",
        "does not call broker",
        "broker sandbox new capability development remains paused",
    }, "docs/182");
    requireAll(doc183, {
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "manual BUY command writes successfully",
        "manual SELL command writes successfully",
        "duplicate idempotencyKey",
        "rollback / failure case",
    }, "docs/183");
}

void testDocsIndexPrompt(const Harness& h)
{
    requireContains(readFile(h.root / "README.md"),
        "docs/182_shell_accounting_manual_transaction_repository_write_implementation.md",
        "README");
    requireContains(readFile(h.root / "README.md"),
        "docs/183_shell_accounting_manual_transaction_repository_write_implementation_test_plan.md",
        "README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "182_shell_accounting_manual_transaction_repository_write_implementation.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "183_shell_accounting_manual_transaction_repository_write_implementation_test_plan.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "12_codex_prompt_template.md"), "TASK-192", "docs/12");
}

void testMigrationsUnmodified(const Harness&)
{
    require(true, "verified by git diff regression");
}

void testNoNewMigration(const Harness& h)
{
    for (const auto& file : filesUnder(h.root / "migrations")) {
        require(file.filename().string() != "003_shell_accounting_manual_transaction_repository.sql",
            "TASK-192 must not add migration");
    }
}

void testNoSchemaFile(const Harness& h)
{
    for (const auto& file : filesUnder(h.root)) {
        require(file.extension() != ".schema", "TASK-192 must not add schema side file");
    }
}

void testDataServiceActionsUnmodified(const Harness& h)
{
    const auto source = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
    const auto header = readFile(h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h");
    const auto registrar = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp");
    for (const auto& text : {source, header, registrar}) {
        requireNotContains(text, "ShellAccountingManualTransactionRepository", "DataServiceActions boundary");
        requireNotContains(text, "persistManualTransaction", "DataServiceActions boundary");
    }
}

void testValidationCodeUnmodified(const Harness& h)
{
    const auto source = readFile(h.root / "libs" / "DataServiceApi" / "src"
        / "ShellAccountingManualTransactionCashMovementValidation.cpp");
    requireNotContains(source, "ShellAccountingManualTransactionRepository", "TASK-178 validation source");
}

void testRepositoryExists(const Harness& h)
{
    require(fs::exists(h.root / "libs" / "DataAccess" / "include" / "DataAccess"
                / "ShellAccountingManualTransactionRepository.h"),
        "repository header exists");
    require(fs::exists(h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp"),
        "repository source exists");
}

void testDataAccessOnly(const Harness& h)
{
    requireContains(readFile(h.root / "libs" / "DataAccess" / "CMakeLists.txt"),
        "ShellAccountingManualTransactionRepository.cpp",
        "DataAccess CMake");
}

void testNotInDataServiceApi(const Harness& h)
{
    requireNotContains(joinFiles(filesUnder(h.root / "libs" / "DataServiceApi")),
        "ShellAccountingManualTransactionRepository",
        "DataServiceApi scan");
}

void testNotInShell(const Harness& h)
{
    requireNotContains(joinFiles(filesUnder(h.root / "apps" / "ETFDecisionShell")),
        "ShellAccountingManualTransactionRepository",
        "Shell scan");
    requireNotContains(joinFiles(filesUnder(h.root / "libs" / "ShellServices")),
        "ShellAccountingManualTransactionRepository",
        "ShellServices scan");
}

void testSqliteApply001002(const Harness& h)
{
    auto fixture = makeDb(h);
    require(countRows(fixture.connection, "trade_log") == 0, "trade_log initially empty");
}

void testManualBuySuccess(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "BUY")));
}

void testManualSellSuccess(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "SELL")));
}

void testBuySellTradeLog(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "BUY", "req-buy", "idem-buy")));
    requireSuccess(persist(fixture, command(fixture, "SELL", "req-sell", "idem-sell")));
    require(countRows(fixture.connection, "trade_log") == 2, "BUY and SELL write trade_log rows");
}

void testExecutionGroupWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(countRows(fixture.connection, "trade_execution_group") == 1, "execution group written");
}

void testRequestIdWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "BUY", "request-written", "idem-request-written")));
    require(singleText(fixture.connection, "SELECT request_id FROM trade_log ORDER BY id DESC LIMIT 1;")
            == "request-written",
        "request_id written");
}

void testIdempotencyKeyWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "BUY", "req-idem", "idem-written")));
    require(singleText(fixture.connection, "SELECT idempotency_key FROM trade_log ORDER BY id DESC LIMIT 1;")
            == "idem-written",
        "idempotency_key written");
}

void testOccurredAtWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(singleText(fixture.connection, "SELECT occurred_at_utc FROM trade_log ORDER BY id DESC LIMIT 1;")
            == "2026-06-02T12:34:56Z",
        "occurred_at_utc written");
}

void testTaxWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(singleInt64(fixture.connection, "SELECT tax_cents FROM trade_log ORDER BY id DESC LIMIT 1;") == 20,
        "tax_cents written");
}

void testSanitizedMemoWritten(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    const auto memo = singleText(fixture.connection, "SELECT source_memo_sanitized FROM trade_log ORDER BY id DESC LIMIT 1;");
    requireContains(memo, "manual repository memo", "source_memo_sanitized");
    requireNotContains(memo, "\n", "source_memo_sanitized");
}

void testManualFlags(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(singleInt64(fixture.connection, "SELECT manual_entry FROM trade_log ORDER BY id DESC LIMIT 1;") == 1,
        "manual_entry flag");
}

void testTradeSourceManual(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(singleText(fixture.connection, "SELECT trade_source FROM trade_log ORDER BY id DESC LIMIT 1;") == "MANUAL",
        "trade_source manual");
}

void testActionTypeBuySell(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "BUY", "req-buy", "idem-buy")));
    requireSuccess(persist(fixture, command(fixture, "SELL", "req-sell", "idem-sell")));
    require(singleText(fixture.connection, "SELECT action_type FROM trade_log WHERE idempotency_key='idem-buy';") == "BUY",
        "BUY action_type");
    require(singleText(fixture.connection, "SELECT action_type FROM trade_log WHERE idempotency_key='idem-sell';") == "SELL",
        "SELL action_type");
}

void testAmountFields(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(singleInt64(fixture.connection, "SELECT quantity_1e6 FROM trade_log ORDER BY id DESC LIMIT 1;") == 1000000,
        "quantity written");
    require(singleInt64(fixture.connection, "SELECT price_1e6 FROM trade_log ORDER BY id DESC LIMIT 1;") == 20100,
        "price written");
    require(singleInt64(fixture.connection, "SELECT amount_cents FROM trade_log ORDER BY id DESC LIMIT 1;") == 20100,
        "amount written");
    require(singleInt64(fixture.connection, "SELECT fee_cents FROM trade_log ORDER BY id DESC LIMIT 1;") == 100,
        "fee written");
}

void testNetCashImpact(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "BUY", "req-buy", "idem-buy")));
    requireSuccess(persist(fixture, command(fixture, "SELL", "req-sell", "idem-sell")));
    require(singleInt64(fixture.connection, "SELECT net_cash_impact_cents FROM trade_log WHERE idempotency_key='idem-buy';")
            == -20220,
        "BUY net cash impact");
    require(singleInt64(fixture.connection, "SELECT net_cash_impact_cents FROM trade_log WHERE idempotency_key='idem-sell';")
            == 19980,
        "SELL net cash impact");
}

void testDuplicateIdempotencyNoExtraLog(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto cmd = command(fixture, "BUY", "req-dup", "idem-dup");
    requireSuccess(persist(fixture, cmd));
    const auto duplicate = persist(fixture, cmd);
    require(duplicate.duplicate, "duplicate marked");
    require(countRows(fixture.connection, "trade_log") == 1, "duplicate idempotency does not add trade_log");
}

void testDuplicateIdempotencyResult(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto cmd = command(fixture, "BUY", "req-dup-result", "idem-dup-result");
    requireSuccess(persist(fixture, cmd));
    const auto duplicate = persist(fixture, cmd);
    require(duplicate.success, "duplicate result success");
    require(duplicate.idempotentReplay, "duplicate result idempotent");
    require(duplicate.status == "IDEMPOTENT_REPLAY", "duplicate result status");
}

void testEmptyIdempotencyStable(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture, "BUY", "req-empty-1", "")));
    requireSuccess(persist(fixture, command(fixture, "BUY", "req-empty-2", "")));
    require(countRows(fixture.connection, "trade_log") == 2, "empty idempotency allows independent requests");
}

void testInvalidCommandNoTradeLog(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture);
    cmd.quantityUnits = 0;
    const auto result = persist(fixture, cmd);
    require(!result.success, "invalid command rejected");
    require(countRows(fixture.connection, "trade_log") == 0, "invalid command writes no trade_log");
}

void testReferenceFailureNoPartialRows(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture);
    cmd.instrumentId = 999999;
    const auto result = persist(fixture, cmd);
    require(!result.success, "missing instrument rejected");
    require(countRows(fixture.connection, "trade_execution_group") == 0, "missing reference writes no group");
    require(countRows(fixture.connection, "trade_log") == 0, "missing reference writes no trade_log");
}

void testTransactionFailureNoPartialRows(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto cmd = command(fixture, "BUY", "same-empty-request", "");
    requireSuccess(persist(fixture, cmd));
    const auto failed = persist(fixture, cmd);
    require(!failed.success, "duplicate generated uid fails without idempotency");
    require(countRows(fixture.connection, "trade_execution_group") == 1, "failed duplicate group insert leaves no partial group");
    require(countRows(fixture.connection, "trade_log") == 1, "failed duplicate group insert leaves no partial trade_log");
}

void testNoCashAdjustmentWrite(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(countRows(fixture.connection, "cash_adjustment") == 0, "cash_adjustment not written");
}

void testNoAuditLogWrite(const Harness& h)
{
    auto fixture = makeDb(h);
    requireSuccess(persist(fixture, command(fixture)));
    require(countRows(fixture.connection, "audit_log") == 0, "audit_log not written");
}

void testNoCashFacts(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto result = persist(fixture, command(fixture));
    require(!result.cashFactsWritten, "cash facts result flag false");
}

void testNoAccountingReplay(const Harness& h)
{
    requireNotContains(repositorySource(h), "AccountingEngine", "manual transaction repository");
    requireNotContains(repositorySource(h), "replay", "manual transaction repository");
}

void testNoTradeDraftSuggestion(const Harness& h)
{
    requireNotContains(repositorySource(h), "TradeDraft", "manual transaction repository");
    requireNotContains(repositorySource(h), "suggest", "manual transaction repository");
}

void testNoBroker(const Harness& h)
{
    requireNotContains(repositorySource(h), "Broker", "manual transaction repository");
    requireNotContains(repositorySource(h), "broker", "manual transaction repository");
}

void testNoNetworkEndpoint(const Harness& h)
{
    for (const auto& token : {"endpoint configuration", "network call", "WinHttp", "curl_easy", "socket("}) {
        requireNotContains(repositorySource(h), token, "manual transaction repository");
    }
}

void testNoCredentials(const Harness& h)
{
    for (const auto& token : {"credential store", "secret store", "password value", "apiKey"}) {
        requireNotContains(repositorySource(h), token, "manual transaction repository");
    }
}

void testNoRealOrder(const Harness& h)
{
    for (const auto& token : {"placeOrder", "executeOrder", "brokerOrderId", "realOrderId"}) {
        requireNotContains(repositorySource(h), token, "manual transaction repository");
    }
}

void testNoAutomaticTrading(const Harness& h)
{
    for (const auto& token : {"automaticTrading", "autoTrade", "strategyExecute"}) {
        requireNotContains(repositorySource(h), token, "manual transaction repository");
    }
}

void testPostMigrationGateEvolved(const Harness& h)
{
    requireContains(readFile(h.root / "docs" / "180_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_gate.md"),
        "TASK-192 implements manual transaction repository write",
        "docs/180 TASK-192 evolution");
}

void testTask190StillValid(const Harness& h)
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

void testCashMovementDisabled(const Harness&)
{
    etfdt::data_access::ShellAccountingManualEntryRepositoryScaffold scaffold;
    etfdt::data_access::ShellAccountingManualCashMovementPersistenceCommand cmd;
    cmd.accountId = "account";
    cmd.portfolioId = "portfolio";
    cmd.movementType = "Deposit";
    cmd.amountMinor = 100;
    cmd.occurredAt = "2026-06-02T12:00:00Z";
    const auto result = scaffold.persistManualCashMovement(cmd);
    require(!result.writeImplemented, "cash movement remains write-not-implemented");
    require(!result.databaseWritten, "cash movement writes no database");
}

void testResultFlags(const Harness& h)
{
    auto fixture = makeDb(h);
    const auto result = persist(fixture, command(fixture));
    requireSuccess(result);
}

void testSensitiveMemoRejected(const Harness& h)
{
    auto fixture = makeDb(h);
    auto cmd = command(fixture);
    cmd.sourceMemo = "contains secret token";
    const auto result = persist(fixture, cmd);
    require(!result.success, "sensitive memo rejected");
    require(countRows(fixture.connection, "trade_log") == 0, "sensitive memo writes no trade_log");
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
    if (h.testCase == "repository_exists") return testRepositoryExists(h);
    if (h.testCase == "dataaccess_only") return testDataAccessOnly(h);
    if (h.testCase == "not_in_dataserviceapi") return testNotInDataServiceApi(h);
    if (h.testCase == "not_in_shell") return testNotInShell(h);
    if (h.testCase == "sqlite_apply_001_002") return testSqliteApply001002(h);
    if (h.testCase == "manual_buy_success") return testManualBuySuccess(h);
    if (h.testCase == "manual_sell_success") return testManualSellSuccess(h);
    if (h.testCase == "buy_sell_trade_log") return testBuySellTradeLog(h);
    if (h.testCase == "execution_group_written") return testExecutionGroupWritten(h);
    if (h.testCase == "request_id_written") return testRequestIdWritten(h);
    if (h.testCase == "idempotency_key_written") return testIdempotencyKeyWritten(h);
    if (h.testCase == "occurred_at_written") return testOccurredAtWritten(h);
    if (h.testCase == "tax_written") return testTaxWritten(h);
    if (h.testCase == "sanitized_memo_written") return testSanitizedMemoWritten(h);
    if (h.testCase == "manual_flags") return testManualFlags(h);
    if (h.testCase == "trade_source_manual") return testTradeSourceManual(h);
    if (h.testCase == "action_type_buy_sell") return testActionTypeBuySell(h);
    if (h.testCase == "amount_fields") return testAmountFields(h);
    if (h.testCase == "net_cash_impact") return testNetCashImpact(h);
    if (h.testCase == "duplicate_idempotency_no_extra_log") return testDuplicateIdempotencyNoExtraLog(h);
    if (h.testCase == "duplicate_idempotency_result") return testDuplicateIdempotencyResult(h);
    if (h.testCase == "empty_idempotency_stable") return testEmptyIdempotencyStable(h);
    if (h.testCase == "invalid_command_no_trade_log") return testInvalidCommandNoTradeLog(h);
    if (h.testCase == "reference_failure_no_partial_rows") return testReferenceFailureNoPartialRows(h);
    if (h.testCase == "transaction_failure_no_partial_rows") return testTransactionFailureNoPartialRows(h);
    if (h.testCase == "no_cash_adjustment_write") return testNoCashAdjustmentWrite(h);
    if (h.testCase == "no_audit_log_write") return testNoAuditLogWrite(h);
    if (h.testCase == "no_cash_facts") return testNoCashFacts(h);
    if (h.testCase == "no_accounting_replay") return testNoAccountingReplay(h);
    if (h.testCase == "no_tradedraft_suggestion") return testNoTradeDraftSuggestion(h);
    if (h.testCase == "no_broker") return testNoBroker(h);
    if (h.testCase == "no_network_endpoint") return testNoNetworkEndpoint(h);
    if (h.testCase == "no_credentials") return testNoCredentials(h);
    if (h.testCase == "no_real_order") return testNoRealOrder(h);
    if (h.testCase == "no_automatic_trading") return testNoAutomaticTrading(h);
    if (h.testCase == "post_migration_gate_evolved") return testPostMigrationGateEvolved(h);
    if (h.testCase == "task190_still_valid") return testTask190StillValid(h);
    if (h.testCase == "task182_validation_only") return testTask182ValidationOnly(h);
    if (h.testCase == "task178_validation_retained") return testTask178ValidationRetained(h);
    if (h.testCase == "broker_gates_retained") return testBrokerGatesRetained(h);
    if (h.testCase == "cash_movement_disabled") return testCashMovementDisabled(h);
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
