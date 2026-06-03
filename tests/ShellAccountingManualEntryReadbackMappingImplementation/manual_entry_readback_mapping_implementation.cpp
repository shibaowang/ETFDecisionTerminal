#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <chrono>
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

std::string functionBody(const std::string& source, const std::string& functionName)
{
    const auto start = source.find(functionName + "(");
    require(start != std::string::npos, functionName + " must exist");
    const auto brace = source.find('{', start);
    require(brace != std::string::npos, functionName + " body must start");
    int depth = 0;
    for (std::size_t i = brace; i < source.size(); ++i) {
        if (source[i] == '{') {
            ++depth;
        } else if (source[i] == '}') {
            --depth;
            if (depth == 0) {
                return source.substr(brace, i - brace + 1U);
            }
        }
    }
    throw std::runtime_error(functionName + " body must end");
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

DbFixture makeDb(const Harness& h, const std::string& label)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task207_manual_entry_readback_" + label + "_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task207.sqlite";
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
        {etfdt::data_access::SqlStatementParameter::textValue("task-207-account-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-207 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic TASK-207 fixture account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(account.hasValue(), "insert synthetic account fixture");
    fixture.accountId = fixture.connection->lastInsertRowId();

    auto portfolio = fixture.connection->executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-207-portfolio-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-207 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic manual entry readback fixture"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(portfolio.hasValue(), "insert synthetic portfolio fixture");
    fixture.portfolioId = fixture.connection->lastInsertRowId();

    auto instrument = fixture.connection->executeStatement(
        "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-207-instrument-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("159509"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-207 ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic TASK-207 fixture instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(instrument.hasValue(), "insert synthetic instrument fixture");
    fixture.instrumentId = fixture.connection->lastInsertRowId();

    return fixture;
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-207-manual-entry-readback";
    request.traceId = "task-207-trace";
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

std::string transactionPayload(
    const DbFixture& fixture,
    const std::string& side,
    std::int64_t quantity,
    std::int64_t gross,
    std::int64_t fee,
    std::int64_t tax,
    const std::string& idempotencyKey)
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\","
            << "\"instrumentId\":\"" << fixture.instrumentId << "\","
            << "\"securityCode\":\"159509\","
            << "\"tradeSide\":\"" << side << "\","
            << "\"quantityUnits\":" << quantity << ','
            << "\"priceAmountMinor\":20100,"
            << "\"grossAmountMinor\":" << gross << ','
            << "\"feeAmountMinor\":" << fee << ','
            << "\"taxAmountMinor\":" << tax << ','
            << "\"occurredAt\":\"2026-06-03T12:34:56Z\","
            << "\"sourceMemo\":\"task 207 synthetic manual transaction\","
            << "\"requestId\":\"task-207-transaction-request-" << idempotencyKey << "\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\""
            << "}";
    return payload.str();
}

std::string cashPayload(
    const DbFixture& fixture,
    const std::string& movementType,
    std::int64_t amount,
    const std::string& idempotencyKey)
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\","
            << "\"movementType\":\"" << movementType << "\","
            << "\"amountMinor\":" << amount << ','
            << "\"currency\":\"CNY\","
            << "\"occurredAt\":\"2026-06-03T12:34:56Z\","
            << "\"sourceMemo\":\"task 207 synthetic cash movement\","
            << "\"sourceReference\":\"bank-ref-task-207\","
            << "\"requestId\":\"task-207-cash-request-" << idempotencyKey << "\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\""
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
         }) {
        requireNotContains(payload, token, "DataService readback response payload");
    }
    requireAll(payload, {
        "\"rawSqlExposed\":false",
        "\"rawPayloadExposed\":false",
        "\"credentialsExposed\":false",
        "\"endpointExposed\":false",
        "\"writeEnabled\":false",
        "\"replayExecuted\":false",
        "\"snapshotRebuilt\":false",
        "\"accountingEngineCalled\":false",
        "\"tradeDraftGenerated\":false",
        "\"tradeSuggestionGenerated\":false",
        "\"strategyExecuted\":false",
        "\"brokerOrderSubmitted\":false",
    }, "safe readback response payload");
}

void testDocsAndIndexes(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "210_shell_accounting_manual_entry_readback_mapping_implementation.md"),
        "docs/210 exists");
    require(fs::exists(
                h.root / "docs" / "211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md"),
        "docs/211 exists");
    requireContains(readFile(h.root / "README.md"),
        "docs/210_shell_accounting_manual_entry_readback_mapping_implementation.md",
        "README");
    requireContains(readFile(h.root / "README.md"),
        "docs/211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md",
        "README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "210_shell_accounting_manual_entry_readback_mapping_implementation.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "12_codex_prompt_template.md"), "TASK-207", "docs/12");
}

void writeManualFacts(DbFixture& fixture)
{
    auto buy = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture, "Buy", 1000000, 20100, 100, 20, "task-207-buy"));
    require(buy.success, "manual BUY write succeeds");
    auto buyDuplicate = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture, "Buy", 1000000, 20100, 100, 20, "task-207-buy"));
    require(buyDuplicate.success, "duplicate manual BUY succeeds idempotently");
    requireContains(buyDuplicate.payloadJson, "\"duplicate\":true", "duplicate BUY response");

    auto sell = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture, "Sell", 250000, 5025, 50, 10, "task-207-sell"));
    require(sell.success, "manual SELL write succeeds");

    auto deposit = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture, "Deposit", 12345, "task-207-deposit"));
    require(deposit.success, "manual Deposit write succeeds");
    auto depositDuplicate = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture, "Deposit", 12345, "task-207-deposit"));
    require(depositDuplicate.success, "duplicate manual Deposit succeeds idempotently");
    requireContains(depositDuplicate.payloadJson, "\"duplicate\":true", "duplicate Deposit response");

    auto withdrawal = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture, "Withdrawal", 2345, "task-207-withdrawal"));
    require(withdrawal.success, "manual Withdrawal write succeeds");

    require(countRows(*fixture.connection, "trade_log") == 4, "duplicate writes did not double count trade_log");
    require(countRows(*fixture.connection, "trade_execution_group") == 2,
        "manual transaction duplicates did not double count execution groups");
    require(countRows(*fixture.connection, "cash_adjustment") == 2,
        "manual cash duplicates did not double count cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 0, "manual entry write path did not add audit_log");
}

void testReadbackRuntime(const Harness& h)
{
    auto fixture = makeDb(h, "runtime");
    writeManualFacts(fixture);
    const auto before = rowCounts(*fixture.connection);

    const auto position = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPositionList,
        readPayload(fixture));
    require(position.success, "position.list readback succeeds");
    requireAll(position.payloadJson, {
        "\"action\":\"position.list\"",
        "\"status\":\"PARTIAL\"",
        "\"dataQualityStatus\":\"PARTIAL\"",
        "\"manualEntryReadback\":true",
        "\"realizedPnlComputed\":false",
        "\"unrealizedPnlComputed\":false",
        "\"instrumentCode\":\"159509\"",
        "\"quantityText\":\"0.75\"",
        "\"costAmountText\":\"151.25\"",
        "\"marketValueText\":\"UNAVAILABLE\"",
        "\"unrealizedPnlText\":\"UNAVAILABLE\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "position.list manual readback");
    requireSafePayload(position.payloadJson);
    requireSameCounts(before, rowCounts(*fixture.connection), "position.list readback");

    const auto cash = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionCashSummary,
        readPayload(fixture));
    require(cash.success, "cash.summary readback succeeds");
    requireAll(cash.payloadJson, {
        "\"action\":\"cash.summary\"",
        "\"status\":\"OK\"",
        "\"manualEntryReadback\":true",
        "\"cashBalanceText\":\"-52.55\"",
        "\"principalBaseText\":\"100.00\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
    }, "cash.summary manual readback");
    requireSafePayload(cash.payloadJson);
    requireSameCounts(before, rowCounts(*fixture.connection), "cash.summary readback");

    const auto portfolio = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPortfolioPnlSummary,
        readPayload(fixture));
    require(portfolio.success, "portfolio.pnl.summary readback succeeds");
    requireAll(portfolio.payloadJson, {
        "\"action\":\"portfolio.pnl.summary\"",
        "\"status\":\"PARTIAL\"",
        "\"dataQualityStatus\":\"PARTIAL\"",
        "\"manualEntryReadback\":true",
        "\"realizedPnlComputed\":false",
        "\"unrealizedPnlComputed\":false",
        "\"cashBalanceText\":\"-52.55\"",
        "\"principalBaseText\":\"100.00\"",
        "\"holdingCostText\":\"151.25\"",
        "\"totalAssetsText\":\"UNAVAILABLE\"",
        "\"totalMarketValueText\":\"UNAVAILABLE\"",
        "\"totalPnlText\":\"UNAVAILABLE\"",
        "\"totalReturnText\":\"UNAVAILABLE\"",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "portfolio.pnl.summary manual readback");
    requireSafePayload(portfolio.payloadJson);
    requireSameCounts(before, rowCounts(*fixture.connection), "portfolio.pnl.summary readback");
}

void testStaticBoundaries(const Harness& h)
{
    const auto actions = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
    requireAll(actions, {
        "loadManualEntryReadbackPositions",
        "loadManualEntryReadbackCashSummaries",
        "manualEntryPortfolioSummariesFromReadback",
        "manualEntryReadbackIssuesJson",
        "MANUAL_ENTRY_READBACK_MAPPING",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "DataServiceActions readback implementation");

    for (const auto& functionName : {
             "loadManualEntryReadbackPositions",
             "loadManualEntryReadbackCashSummaries",
             "manualEntryPortfolioSummariesFromReadback",
             "handlePositionList",
             "handleCashSummary",
             "handlePortfolioPnlSummary",
         }) {
        const auto body = functionBody(actions, functionName);
        requireNotContains(body, "executeStatement", functionName);
        requireNotContains(body, "TransactionRunner", functionName);
        for (const auto& token : {"INSERT ", "UPDATE ", "DELETE ", "REPLACE "}) {
            requireNotContains(body, token, functionName);
        }
    }

    const auto combinedReadbackBody = functionBody(actions, "loadManualEntryReadbackPositions")
        + functionBody(actions, "loadManualEntryReadbackCashSummaries")
        + functionBody(actions, "manualEntryPortfolioSummariesFromReadback");
    requireNotContains(combinedReadbackBody, "brokerOrderId", "manual entry readback mapping");
    requireNotContains(combinedReadbackBody, "realOrderId", "manual entry readback mapping");
    requireNotContains(combinedReadbackBody, "automaticTrading\":true", "manual entry readback mapping");
    requireContains(readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp"),
        "kActionPositionList",
        "DataServiceActionRegistrar retained");
}

void testNoUnauthorizedPathChanges(const Harness& h)
{
    for (const auto& path : {
             h.root / "migrations" / "001_initial_schema.sql",
             h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql",
             h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp",
             h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualCashMovementRepository.cpp",
             h.root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml",
             h.root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp",
             h.root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
         }) {
        require(fs::exists(path), "required retained file exists: " + path.string());
    }
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryReadbackMappingImplementation",
        "TASK-207 test registered");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryReadbackMappingAuthorizationGate",
        "TASK-206 gate retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryReadbackReplayAdequacyReviewGate",
        "TASK-205 gate retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryMvpRuntimeE2eAcceptance",
        "TASK-204 runtime E2E retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryPostWriteReadbackRefreshImplementation",
        "TASK-202 post-write refresh retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryQmlPresenterImplementation",
        "TASK-200 QML Presenter retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryDataServiceWriteWiringImplementation",
        "TASK-198 write wiring retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualCashMovementRepositoryDualWriteImplementation",
        "TASK-196 repository retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualTransactionRepositoryWriteImplementation",
        "TASK-192 repository retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingBrokerAdapterDisabledWiring",
        "broker disabled gate retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingBrokerOrderImplementation",
        "broker order gate retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "real broker authorization gate retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingRealBrokerOrderImplementationGate",
        "real broker implementation gate retained");
}

void runAll(const Harness& h)
{
    testDocsAndIndexes(h);
    testReadbackRuntime(h);
    testStaticBoundaries(h);
    testNoUnauthorizedPathChanges(h);
    require(g_checks >= 90, "TASK-207 readback mapping implementation must execute at least 90 checks");
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
