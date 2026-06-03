#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <chrono>
#include <cstdint>
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

int g_checks = 0;

struct Harness {
    fs::path root;
};

struct DbFixture {
    fs::path directory;
    std::unique_ptr<etfdt::data_access::SQLiteConnection> connection =
        std::make_unique<etfdt::data_access::SQLiteConnection>();
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::int64_t instrumentId = 0;
};

struct RowCounts {
    int tradeExecutionGroup = 0;
    int tradeLog = 0;
    int cashAdjustment = 0;
    int auditLog = 0;
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
    ++g_checks;
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

std::int64_t singleInt64(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    require(rows.hasValue(), "query failed: " + sql);
    require(!rows.value().empty() && !rows.value().front().empty(), "query returned no row: " + sql);
    return rows.value().front().front().int64Value;
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    return static_cast<int>(singleInt64(connection, "SELECT COUNT(*) FROM " + table + ";"));
}

RowCounts rowCounts(etfdt::data_access::SQLiteConnection& connection)
{
    return RowCounts{
        countRows(connection, "trade_execution_group"),
        countRows(connection, "trade_log"),
        countRows(connection, "cash_adjustment"),
        countRows(connection, "audit_log"),
    };
}

void requireSameCounts(const RowCounts& before, const RowCounts& after, const std::string& context)
{
    require(before.tradeExecutionGroup == after.tradeExecutionGroup, context + " changed trade_execution_group");
    require(before.tradeLog == after.tradeLog, context + " changed trade_log");
    require(before.cashAdjustment == after.cashAdjustment, context + " changed cash_adjustment");
    require(before.auditLog == after.auditLog, context + " changed audit_log");
}

DbFixture makeDb(const Harness& h)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory =
        fs::temp_directory_path() / ("task209_manual_entry_daily_use_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task209.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open temporary sqlite");
    require(fixture.connection->applyMigrationFile(h.root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    require(fixture.connection->applyMigrationFile(
                h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
                .hasValue(),
        "apply migration 002");

    auto account = fixture.connection->executeStatement(
        "INSERT INTO account(uid,name,account_type,base_currency,is_active,initial_cash_cents,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-209-account"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-209 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic TASK-209 fixture account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(account.hasValue(), "insert synthetic account fixture");
    fixture.accountId = fixture.connection->lastInsertRowId();

    auto portfolio = fixture.connection->executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-209-portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-209 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic daily-use readback fixture"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(portfolio.hasValue(), "insert synthetic portfolio fixture");
    fixture.portfolioId = fixture.connection->lastInsertRowId();

    auto instrument = fixture.connection->executeStatement(
        "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-209-instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("159509"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-209 ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic TASK-209 fixture instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(instrument.hasValue(), "insert synthetic instrument fixture");
    fixture.instrumentId = fixture.connection->lastInsertRowId();

    return fixture;
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-209-manual-entry-daily-use";
    request.traceId = "task-209-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-06-03T12:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

etfdt::protocol::ProtocolResponse dispatch(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& action,
    const std::string& payloadJson)
{
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    return dispatcher.dispatch(envelope(action, payloadJson));
}

std::string transactionPayload(const DbFixture& fixture)
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\","
            << "\"instrumentId\":\"" << fixture.instrumentId << "\","
            << "\"securityCode\":\"159509\","
            << "\"tradeSide\":\"Buy\","
            << "\"quantityUnits\":1000000,"
            << "\"priceAmountMinor\":20100,"
            << "\"grossAmountMinor\":20100,"
            << "\"feeAmountMinor\":100,"
            << "\"taxAmountMinor\":20,"
            << "\"occurredAt\":\"2026-06-03T12:34:56Z\","
            << "\"sourceMemo\":\"task 209 synthetic manual BUY\","
            << "\"requestId\":\"task-209-transaction-request\","
            << "\"idempotencyKey\":\"task-209-buy\""
            << "}";
    return payload.str();
}

std::string cashPayload(const DbFixture& fixture)
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\","
            << "\"movementType\":\"Deposit\","
            << "\"amountMinor\":50000,"
            << "\"currency\":\"CNY\","
            << "\"occurredAt\":\"2026-06-03T12:35:56Z\","
            << "\"sourceMemo\":\"task 209 synthetic cash deposit\","
            << "\"sourceReference\":\"bank-ref-task-209\","
            << "\"requestId\":\"task-209-cash-request\","
            << "\"idempotencyKey\":\"task-209-deposit\""
            << "}";
    return payload.str();
}

std::string readPayload(const DbFixture& fixture)
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\""
            << "}";
    return payload.str();
}

void requireSafePayload(const std::string& payload)
{
    for (const auto& token : {
             "raw SQL",
             "SELECT ",
             "INSERT INTO",
             "UPDATE ",
             "DELETE FROM",
             "REPLACE INTO",
             "credential=",
             "credentials=",
             "password",
             "api_key",
             "endpoint=",
             "brokerOrderId",
             "realOrderId",
             "stack trace",
             "broker payload",
             "full trade payload",
         }) {
        requireNotContains(payload, token, "daily-use response payload");
    }
    requireAll(payload, {
        "\"rawSqlExposed\":false",
        "\"rawPayloadExposed\":false",
        "\"credentialsExposed\":false",
        "\"endpointExposed\":false",
    }, "safe daily-use response payload");
}

void writeBuyAndDeposit(DbFixture& fixture)
{
    const auto buy = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture));
    require(buy.success, "manual BUY setup succeeds");
    requireAll(buy.payloadJson, {
        "\"validationAccepted\":true",
        "\"databaseWritten\":true",
        "\"tradeExecutionGroupWritten\":true",
        "\"tradeLogWritten\":true",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
    }, "manual BUY setup response");
    requireSafePayload(buy.payloadJson);

    const auto deposit = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture));
    require(deposit.success, "manual Deposit setup succeeds");
    requireAll(deposit.payloadJson, {
        "\"validationAccepted\":true",
        "\"databaseWritten\":true",
        "\"tradeLogWritten\":true",
        "\"cashAdjustmentWritten\":true",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
    }, "manual Deposit setup response");
    requireSafePayload(deposit.payloadJson);

    require(countRows(*fixture.connection, "trade_execution_group") == 1, "setup writes one execution group");
    require(countRows(*fixture.connection, "trade_log") == 2, "setup writes two trade_log rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "setup writes one cash_adjustment row");
    require(countRows(*fixture.connection, "audit_log") == 0, "setup writes no audit_log rows");
}

void testDailyUseReadbackRuntime(const Harness& h)
{
    auto fixture = makeDb(h);
    writeBuyAndDeposit(fixture);
    const auto beforeReadback = rowCounts(*fixture.connection);

    const auto position = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPositionList,
        readPayload(fixture));
    require(position.success, "position.list daily-use readback succeeds");
    requireAll(position.payloadJson, {
        "\"action\":\"position.list\"",
        "\"status\":\"PARTIAL\"",
        "\"dataQualityStatus\":\"PARTIAL\"",
        "\"manualEntryReadback\":true",
        "\"realizedPnlComputed\":false",
        "\"unrealizedPnlComputed\":false",
        "\"accountId\":\"" + std::to_string(fixture.accountId) + "\"",
        "\"portfolioId\":\"" + std::to_string(fixture.portfolioId) + "\"",
        "\"instrumentCode\":\"159509\"",
        "\"quantityText\":\"1\"",
        "\"costAmountText\":\"202.00\"",
        "\"marketValueText\":\"UNAVAILABLE\"",
        "\"unrealizedPnlText\":\"UNAVAILABLE\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "position.list daily-use payload");
    requireSafePayload(position.payloadJson);
    requireNotContains(position.payloadJson, "\"marketValueText\":\"0.00\"", "position.list daily-use payload");
    requireNotContains(position.payloadJson, "\"unrealizedPnlText\":\"0.00\"", "position.list daily-use payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "position.list daily-use readback");

    const auto cash = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionCashSummary,
        readPayload(fixture));
    require(cash.success, "cash.summary daily-use readback succeeds");
    requireAll(cash.payloadJson, {
        "\"action\":\"cash.summary\"",
        "\"status\":\"OK\"",
        "\"dataQualityStatus\":\"OK\"",
        "\"manualEntryReadback\":true",
        "\"accountId\":\"" + std::to_string(fixture.accountId) + "\"",
        "\"portfolioId\":\"" + std::to_string(fixture.portfolioId) + "\"",
        "\"cashBalanceText\":\"297.80\"",
        "\"principalBaseText\":\"500.00\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
    }, "cash.summary daily-use payload");
    requireSafePayload(cash.payloadJson);
    requireNotContains(cash.payloadJson, "\"cashBalanceText\":\"0.00\"", "cash.summary daily-use payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "cash.summary daily-use readback");

    const auto portfolio = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPortfolioPnlSummary,
        readPayload(fixture));
    require(portfolio.success, "portfolio.pnl.summary daily-use readback succeeds");
    requireAll(portfolio.payloadJson, {
        "\"action\":\"portfolio.pnl.summary\"",
        "\"status\":\"PARTIAL\"",
        "\"dataQualityStatus\":\"PARTIAL\"",
        "\"manualEntryReadback\":true",
        "\"realizedPnlComputed\":false",
        "\"unrealizedPnlComputed\":false",
        "\"portfolioId\":\"" + std::to_string(fixture.portfolioId) + "\"",
        "\"cashBalanceText\":\"297.80\"",
        "\"principalBaseText\":\"500.00\"",
        "\"holdingCostText\":\"202.00\"",
        "\"totalAssetsText\":\"UNAVAILABLE\"",
        "\"totalMarketValueText\":\"UNAVAILABLE\"",
        "\"totalPnlText\":\"UNAVAILABLE\"",
        "\"totalReturnText\":\"UNAVAILABLE\"",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "portfolio.pnl.summary daily-use payload");
    requireSafePayload(portfolio.payloadJson);
    requireNotContains(portfolio.payloadJson, "\"totalPnlText\":\"0.00\"", "portfolio.pnl.summary daily-use payload");
    requireNotContains(portfolio.payloadJson, "\"totalMarketValueText\":\"0.00\"", "portfolio.pnl.summary daily-use payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "portfolio.pnl.summary daily-use readback");
}

void testDocsAndIndexes(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md"),
        "docs/214 exists");
    require(fs::exists(
                h.root / "docs" / "215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md"),
        "docs/215 exists");
    requireContains(readFile(h.root / "README.md"),
        "docs/214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md",
        "README");
    requireContains(readFile(h.root / "README.md"),
        "docs/215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md",
        "README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "12_codex_prompt_template.md"), "TASK-209", "docs/12");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance",
        "tests/CMakeLists");
}

void testStaticBoundaries(const Harness& h)
{
    const auto qml = readFile(h.root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml");
    const auto presenter = readFile(h.root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp");
    const auto controller =
        readFile(h.root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp");
    const auto actions = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");

    for (const auto& token : {
             "SQLite",
             "DataAccess",
             "DataServiceClient",
             "INSERT INTO",
             "UPDATE ",
             "DELETE FROM",
             "REPLACE INTO",
             "calculatePosition",
             "calculatePnl",
             "brokerOrder",
             "placeOrder",
             "realOrderId",
             "automaticTrading",
         }) {
        requireNotContains(qml, token, "production QML");
    }
    for (const auto& token : {
             "DataAccess/ShellAccountingManual",
             "SQLite",
             "INSERT INTO",
             "UPDATE ",
             "DELETE FROM",
             "REPLACE INTO",
             "BrokerOrderPort",
             "StrategyEngine",
         }) {
        requireNotContains(presenter + controller, token, "Presenter / Controller");
    }
    requireAll(actions, {
        "loadManualEntryReadbackPositions",
        "loadManualEntryReadbackCashSummaries",
        "manualEntryPortfolioSummariesFromReadback",
        "manualEntryReadbackIssuesJson",
    }, "DataServiceActions retained readback mapping");
    requireNotContains(actions, "runShellAccountingReadOnlyReplay(context.request.payloadJson)", "DataServiceActions");
}

void testRetainedRegressionGates(const Harness& h)
{
    const auto testsCmake = readFile(h.root / "tests" / "CMakeLists.txt");
    for (const auto& token : {
             "ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate",
             "ShellAccountingManualEntryReadbackMappingImplementation",
             "ShellAccountingManualEntryReadbackMappingAuthorizationGate",
             "ShellAccountingManualEntryReadbackReplayAdequacyReviewGate",
             "ShellAccountingManualEntryMvpRuntimeE2eAcceptance",
             "ShellAccountingManualEntryPostWriteReadbackRefreshImplementation",
             "ShellAccountingManualEntryQmlPresenterImplementation",
             "ShellAccountingManualEntryDataServiceWriteWiringImplementation",
             "ShellAccountingManualCashMovementRepositoryDualWriteImplementation",
             "ShellAccountingManualTransactionRepositoryWriteImplementation",
             "ShellAccountingBrokerAdapterDisabledWiring",
             "ShellAccountingBrokerOrderImplementation",
             "ShellAccountingRealBrokerOrderAuthorizationGate",
             "ShellAccountingRealBrokerOrderImplementationGate",
         }) {
        requireContains(testsCmake, token, "retained regression gate");
    }

    for (const auto& path : {
             h.root / "migrations" / "001_initial_schema.sql",
             h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql",
             h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp",
             h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualCashMovementRepository.cpp",
             h.root / "apps" / "ETFDecisionShell" / "src" / "main.cpp",
         }) {
        require(fs::exists(path), "required retained file exists: " + path.string());
    }
}

void runAll(const Harness& h)
{
    testDocsAndIndexes(h);
    testDailyUseReadbackRuntime(h);
    testStaticBoundaries(h);
    testRetainedRegressionGates(h);
    require(g_checks >= 95, "TASK-209 daily-use runtime acceptance must execute at least 95 checks");
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        Harness h;
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--source-root" && i + 1 < argc) {
                h.root = argv[++i];
            }
        }
        if (h.root.empty()) {
            throw std::runtime_error("Usage: --source-root <path>");
        }
        runAll(h);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
