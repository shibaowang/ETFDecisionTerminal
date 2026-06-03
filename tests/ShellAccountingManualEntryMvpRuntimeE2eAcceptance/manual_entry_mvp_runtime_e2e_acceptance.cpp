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

DbFixture makeDb(const Harness& h, const std::string& label)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task204_manual_entry_mvp_e2e_" + label + "_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task204.sqlite";
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
        {etfdt::data_access::SqlStatementParameter::textValue("task-204-account-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-204 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic TASK-204 fixture account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(account.hasValue(), "insert synthetic account fixture");
    fixture.accountId = fixture.connection->lastInsertRowId();

    auto portfolio = fixture.connection->executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-204-portfolio-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-204 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic manual entry MVP E2E fixture"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(portfolio.hasValue(), "insert synthetic portfolio fixture");
    fixture.portfolioId = fixture.connection->lastInsertRowId();

    auto instrument = fixture.connection->executeStatement(
        "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-204-instrument-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("159509"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-204 ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic TASK-204 fixture instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(instrument.hasValue(), "insert synthetic instrument fixture");
    fixture.instrumentId = fixture.connection->lastInsertRowId();

    return fixture;
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-204-manual-entry-mvp-e2e";
    request.traceId = "task-204-trace";
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
    const std::string& idempotencyKey = "task-204-transaction-idempotency")
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
            << "\"sourceMemo\":\"task 204 synthetic manual transaction\","
            << "\"requestId\":\"task-204-transaction-request\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\""
            << "}";
    return payload.str();
}

std::string cashPayload(
    const DbFixture& fixture,
    const std::string& idempotencyKey = "task-204-cash-idempotency")
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\","
            << "\"movementType\":\"Deposit\","
            << "\"amountMinor\":12345,"
            << "\"currency\":\"CNY\","
            << "\"occurredAt\":\"2026-06-03T12:34:56Z\","
            << "\"sourceMemo\":\"task 204 synthetic cash deposit\","
            << "\"sourceReference\":\"bank-ref-task-204\","
            << "\"requestId\":\"task-204-cash-request\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\""
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
        requireNotContains(payload, token, "DataService response payload");
    }
    requireAll(payload, {
        "\"rawSqlExposed\":false",
        "\"rawPayloadExposed\":false",
        "\"credentialsExposed\":false",
        "\"endpointExposed\":false",
        "\"internalStackExposed\":false",
        "\"brokerSdkCalled\":false",
        "\"networkAccessed\":false",
        "\"credentialsAccessed\":false",
        "\"endpointAccessed\":false",
        "\"realOrderPlacement\":false",
        "\"automaticTrading\":false",
    }, "safe response payload");
}

void testDocsAndRegistration(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md"),
        "docs/204 exists");
    require(fs::exists(h.root / "docs" / "205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md"),
        "docs/205 exists");
    for (const auto& path : {
             h.root / "README.md",
             h.root / "docs" / "README.md",
             h.root / "docs" / "12_codex_prompt_template.md",
         }) {
        requireContains(readFile(path), "TASK-204", path.string());
    }
    requireContains(readFile(h.root / "README.md"),
        "docs/204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md",
        "README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md",
        "docs/README");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryMvpRuntimeE2eAcceptance",
        "tests/CMakeLists");
    requireContains(readFile(h.root / "docs" / "202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md"),
        "TASK-204",
        "docs/202");
    requireContains(readFile(h.root / "docs" / "203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md"),
        "TASK-204",
        "docs/203");
}

void testManualTransactionRuntimeE2e(const Harness& h)
{
    auto fixture = makeDb(h, "transaction");
    const auto response = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture));
    require(response.success, "manual transaction runtime E2E write succeeds");
    requireAll(response.payloadJson, {
        "\"databaseWritten\":true",
        "\"tradeExecutionGroupWritten\":true",
        "\"tradeLogWritten\":true",
        "\"cashAdjustmentWritten\":false",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
        "\"status\":\"OK\"",
    }, "manual transaction response flags");
    requireSafePayload(response.payloadJson);
    require(countRows(*fixture.connection, "trade_execution_group") == 1,
        "manual transaction writes one trade_execution_group");
    require(countRows(*fixture.connection, "trade_log") == 1,
        "manual transaction writes one trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 0,
        "manual transaction does not write cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 0,
        "manual transaction does not write audit_log");

    const auto duplicate = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture));
    require(duplicate.success, "manual transaction duplicate succeeds idempotently");
    requireContains(duplicate.payloadJson, "\"idempotent\":true", "manual transaction duplicate response");
    requireContains(duplicate.payloadJson, "\"duplicate\":true", "manual transaction duplicate response");
    require(countRows(*fixture.connection, "trade_execution_group") == 1,
        "manual transaction duplicate writes no extra trade_execution_group");
    require(countRows(*fixture.connection, "trade_log") == 1,
        "manual transaction duplicate writes no extra trade_log");

    const auto invalid = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        R"({"accountId":"","portfolioId":"","securityCode":"","tradeSide":"Hold","quantityUnits":0,"priceAmountMinor":0,"grossAmountMinor":0,"feeAmountMinor":-1,"taxAmountMinor":-1,"occurredAt":"","sourceMemo":"invalid"})");
    require(!invalid.success, "manual transaction invalid payload rejected");
    requireAll(invalid.payloadJson, {
        "\"validationAccepted\":false",
        "\"databaseWritten\":false",
        "\"tradeLogWritten\":false",
        "\"cashAdjustmentWritten\":false",
        "TRADE_SIDE_INVALID",
    }, "manual transaction invalid response");
    requireSafePayload(invalid.payloadJson);
    require(countRows(*fixture.connection, "trade_execution_group") == 1,
        "manual transaction invalid payload writes no extra trade_execution_group");
    require(countRows(*fixture.connection, "trade_log") == 1,
        "manual transaction invalid payload writes no extra trade_log");
}

void testManualCashMovementRuntimeE2e(const Harness& h)
{
    auto fixture = makeDb(h, "cash");
    const auto response = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture));
    require(response.success, "manual cash movement runtime E2E write succeeds");
    requireAll(response.payloadJson, {
        "\"databaseWritten\":true",
        "\"tradeLogWritten\":true",
        "\"cashAdjustmentWritten\":true",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
        "\"status\":\"OK\"",
    }, "manual cash movement response flags");
    requireSafePayload(response.payloadJson);
    require(countRows(*fixture.connection, "trade_log") == 1,
        "manual cash movement writes one trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1,
        "manual cash movement writes one cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 0,
        "manual cash movement does not write audit_log");
    require(singleInt64(*fixture.connection,
                "SELECT trade_log_id FROM cash_adjustment WHERE uid LIKE 'task-196-manual-cash-adjustment-%';")
            == singleInt64(*fixture.connection,
                "SELECT id FROM trade_log WHERE uid LIKE 'task-196-manual-cash-trade-log-%';"),
        "cash_adjustment.trade_log_id references trade_log.id");

    const auto duplicate = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture));
    require(duplicate.success, "manual cash movement duplicate succeeds idempotently");
    requireContains(duplicate.payloadJson, "\"idempotent\":true", "manual cash movement duplicate response");
    requireContains(duplicate.payloadJson, "\"duplicate\":true", "manual cash movement duplicate response");
    require(countRows(*fixture.connection, "trade_log") == 1,
        "manual cash movement duplicate writes no extra trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1,
        "manual cash movement duplicate writes no extra cash_adjustment");

    const auto invalid = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        R"({"accountId":"","portfolioId":"","movementType":"Transfer","amountMinor":0,"occurredAt":"","sourceMemo":"invalid"})");
    require(!invalid.success, "manual cash movement invalid payload rejected");
    requireAll(invalid.payloadJson, {
        "\"validationAccepted\":false",
        "\"databaseWritten\":false",
        "\"tradeLogWritten\":false",
        "\"cashAdjustmentWritten\":false",
        "CASH_MOVEMENT_TYPE_INVALID",
    }, "manual cash movement invalid response");
    requireSafePayload(invalid.payloadJson);
    require(countRows(*fixture.connection, "trade_log") == 1,
        "manual cash movement invalid payload writes no extra trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1,
        "manual cash movement invalid payload writes no extra cash_adjustment");
}

void testPostWriteReadbackAndUiBoundary(const Harness& h)
{
    const auto qml = readFile(h.root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml");
    const auto presenterH = readFile(h.root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h");
    const auto presenterCpp = readFile(h.root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp");
    const auto controllerCpp = readFile(h.root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp");
    const auto actions = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");

    requireAll(qml + presenterH + presenterCpp, {
        "submitManualTransaction",
        "submitManualCashMovement",
        "shellAccountingSubmitManualTransactionButton",
        "shellAccountingSubmitManualCashMovementButton",
        "shellAccountingPostWriteRefreshStatus",
        "shellAccountingPostWriteRefreshIssue",
        "shellAccountingPostWriteRefreshSummary",
        "lastPostWriteRefreshStatus",
        "lastPostWriteRefreshIssue",
        "lastPostWriteRefreshSummary",
    }, "QML / Presenter manual entry boundary");
    requireAll(controllerCpp + presenterCpp, {
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
    }, "post-write readback DataService action allowlist");
    for (const auto& token : {
             "DataServiceClient",
             "SQLite",
             "DataAccess",
             "INSERT INTO",
             "UPDATE ",
             "DELETE FROM",
             "REPLACE INTO",
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
             "AccountingReplay",
             "StrategyEngine",
             "BrokerOrderPort",
         }) {
        requireNotContains(presenterH + presenterCpp, token, "Presenter");
    }

    const auto transactionHandler = functionBody(actions, "handleAccountingManualEntryTransactionCreate");
    const auto cashHandler = functionBody(actions, "handleAccountingManualEntryCashMovementCreate");
    for (const auto& body : {transactionHandler, cashHandler}) {
        requireContains(body, "validateManual", "manual entry handler");
        requireContains(body, "Repository repository(connection)", "manual entry handler");
        requireNotContains(body, "executeStatement", "manual entry handler");
        requireNotContains(body, "queryRows", "manual entry handler");
        requireNotContains(body, "INSERT", "manual entry handler");
        requireNotContains(body, "UPDATE", "manual entry handler");
        requireNotContains(body, "DELETE", "manual entry handler");
        requireNotContains(body, "REPLACE", "manual entry handler");
    }
    requireNotContains(actions, "runShellAccountingReadOnlyReplay(context.request.payloadJson)", "DataServiceActions");
}

void testNoProductionDriftAndBrokerBoundary(const Harness& h)
{
    for (const auto& path : {
             h.root / "migrations" / "001_initial_schema.sql",
             h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql",
             h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp",
             h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualCashMovementRepository.cpp",
             h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp",
             h.root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml",
         }) {
        require(fs::exists(path), "required retained file exists: " + path.string());
    }
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate",
        "TASK-203 gate retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingBrokerAdapterDisabledWiring",
        "broker disabled gate retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingBrokerOrderImplementation",
        "broker order gate retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingRealBrokerOrderImplementationGate",
        "real broker gate retained");
}

void runAll(const Harness& h)
{
    testDocsAndRegistration(h);
    testManualTransactionRuntimeE2e(h);
    testManualCashMovementRuntimeE2e(h);
    testPostWriteReadbackAndUiBoundary(h);
    testNoProductionDriftAndBrokerBoundary(h);
    require(g_checks >= 120, "TASK-204 runtime E2E acceptance must execute at least 120 checks");
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
