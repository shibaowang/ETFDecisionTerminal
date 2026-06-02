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

std::string dataServiceActions(const Harness& h)
{
    return readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string registrar(const Harness& h)
{
    return readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp");
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

DbFixture makeDb(const Harness& h, const std::string& label)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task198_dataservice_write_" + label + "_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task198.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open sqlite");
    require(fixture.connection->applyMigrationFile(h.root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    require(fixture.connection->applyMigrationFile(
                h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
                .hasValue(),
        "apply migration 002");

    auto account = fixture.connection->executeStatement(
        "INSERT INTO account(uid,name,account_type,base_currency,is_active,initial_cash_cents,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-198-account-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-198 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-198 fixture account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    require(account.hasValue(), "insert account");
    fixture.accountId = fixture.connection->lastInsertRowId();

    auto portfolio = fixture.connection->executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-198-portfolio-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-198 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("manual entry DataService write tests"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    require(portfolio.hasValue(), "insert portfolio");
    fixture.portfolioId = fixture.connection->lastInsertRowId();

    auto instrument = fixture.connection->executeStatement(
        "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-198-instrument-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("159509"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-198 ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-198 fixture instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-02T12:00:00Z")});
    require(instrument.hasValue(), "insert instrument");
    fixture.instrumentId = fixture.connection->lastInsertRowId();
    return fixture;
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-198-manual-entry";
    request.traceId = "task-198-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-06-02T12:00:00Z";
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
    const std::string& idempotencyKey = "task-198-transaction-idempotency")
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
            << "\"occurredAt\":\"2026-06-02T12:34:56Z\","
            << "\"sourceMemo\":\"manual DataService write\","
            << "\"requestId\":\"task-198-transaction-request\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\""
            << "}";
    return payload.str();
}

std::string cashPayload(
    const DbFixture& fixture,
    const std::string& idempotencyKey = "task-198-cash-idempotency")
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\","
            << "\"movementType\":\"Deposit\","
            << "\"amountMinor\":12345,"
            << "\"currency\":\"CNY\","
            << "\"occurredAt\":\"2026-06-02T12:34:56Z\","
            << "\"sourceMemo\":\"manual cash DataService write\","
            << "\"sourceReference\":\"bank-ref-198\","
            << "\"requestId\":\"task-198-cash-request\","
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
        "\"realOrderPlacement\":false",
        "\"automaticTrading\":false",
    }, "safe response payload");
}

void testDocsAndIndexes(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md"),
        "docs/192 exists");
    require(fs::exists(h.root / "docs" / "193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md"),
        "docs/193 exists");
    requireContains(readFile(h.root / "README.md"),
        "docs/192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md",
        "README");
    requireContains(readFile(h.root / "README.md"),
        "docs/193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md",
        "README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "12_codex_prompt_template.md"), "TASK-198", "docs/12");
}

void testStaticBoundaries(const Harness& h)
{
    const auto actions = dataServiceActions(h);
    const auto registrarText = registrar(h);
    requireContains(actions, "ShellAccountingManualTransactionRepository", "DataServiceActions");
    requireContains(actions, "ShellAccountingManualCashMovementRepository", "DataServiceActions");
    requireContains(actions, "persistManualTransaction", "DataServiceActions");
    requireContains(actions, "persistManualCashMovement", "DataServiceActions");
    requireContains(registrarText, "kActionAccountingManualTransactionCreate", "registrar");
    requireContains(registrarText, "kActionAccountingManualCashMovementCreate", "registrar");

    const auto transaction = functionBody(actions, "handleAccountingManualEntryTransactionCreate");
    const auto cash = functionBody(actions, "handleAccountingManualEntryCashMovementCreate");
    for (const auto& body : {transaction, cash}) {
        requireContains(body, "validateManual", "manual entry handler");
        requireContains(body, "Repository repository(connection)", "manual entry handler");
        requireNotContains(body, "executeStatement", "manual entry handler");
        requireNotContains(body, "executeSql", "manual entry handler");
        requireNotContains(body, "queryRows", "manual entry handler");
        for (const auto& token : {"INSERT", "UPDATE", "DELETE", "REPLACE"}) {
            requireNotContains(body, token, "manual entry handler");
        }
    }
    requireNotContains(functionBody(actions, "manualTransactionWriteResponse"), "\"auditWritten\\\":true",
        "manual transaction response");
    requireNotContains(functionBody(actions, "manualCashMovementWriteResponse"), "\"auditWritten\\\":true",
        "manual cash response");
    requireNotContains(actions, "runShellAccountingReadOnlyReplay(context.request.payloadJson)", "DataServiceActions");
}

void testManualTransactionWrite(const Harness& h)
{
    auto fixture = makeDb(h, "transaction");
    const auto response = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture));
    require(response.success, "manual transaction DataService write succeeds");
    requireAll(response.payloadJson, {
        "\"writeImplemented\":true",
        "\"writeEnabled\":true",
        "\"validationAccepted\":true",
        "\"databaseWritten\":true",
        "\"repositoryWrite\":true",
        "\"tradeExecutionGroupWritten\":true",
        "\"tradeLogWritten\":true",
        "\"cashAdjustmentWritten\":false",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
        "\"status\":\"OK\"",
        "\"tradeLogUid\":\"task-192-manual-transaction-log-task-198-transaction-idempotency\"",
    }, "manual transaction write response");
    requireSafePayload(response.payloadJson);
    require(countRows(*fixture.connection, "trade_log") == 1, "manual transaction writes one trade_log");
    require(countRows(*fixture.connection, "trade_execution_group") == 1,
        "manual transaction writes one execution group");
    require(countRows(*fixture.connection, "cash_adjustment") == 0,
        "manual transaction does not write cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 0, "manual transaction does not write audit_log");
}

void testManualCashMovementWrite(const Harness& h)
{
    auto fixture = makeDb(h, "cash");
    const auto response = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture));
    require(response.success, "manual cash movement DataService write succeeds");
    requireAll(response.payloadJson, {
        "\"writeImplemented\":true",
        "\"writeEnabled\":true",
        "\"validationAccepted\":true",
        "\"databaseWritten\":true",
        "\"repositoryWrite\":true",
        "\"tradeLogWritten\":true",
        "\"cashAdjustmentWritten\":true",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
        "\"status\":\"OK\"",
        "\"tradeLogUid\":\"task-196-manual-cash-trade-log-task-198-cash-idempotency\"",
        "\"cashAdjustmentUid\":\"task-196-manual-cash-adjustment-task-198-cash-idempotency\"",
    }, "manual cash movement write response");
    requireSafePayload(response.payloadJson);
    require(countRows(*fixture.connection, "trade_log") == 1, "manual cash writes one trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "manual cash writes one cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 0, "manual cash does not write audit_log");
}

void testInvalidPayloadNoWrite(const Harness& h)
{
    auto fixture = makeDb(h, "invalid");
    const auto response = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        R"({"accountId":"","portfolioId":"","securityCode":"","tradeSide":"Hold","quantityUnits":0,"priceAmountMinor":0,"grossAmountMinor":0,"feeAmountMinor":-1,"taxAmountMinor":-1,"occurredAt":"","sourceMemo":"invalid"})");
    require(!response.success, "invalid payload rejected");
    requireAll(response.payloadJson, {
        "\"validationAccepted\":false",
        "\"repositoryWrite\":false",
        "\"databaseWritten\":false",
        "\"tradeLogWritten\":false",
        "\"cashAdjustmentWritten\":false",
        "TRADE_SIDE_INVALID",
    }, "invalid response");
    require(countRows(*fixture.connection, "trade_log") == 0, "invalid payload writes no trade_log");
    require(countRows(*fixture.connection, "trade_execution_group") == 0, "invalid payload writes no group");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "invalid payload writes no cash_adjustment");

    const auto nonObject = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        "[]");
    require(!nonObject.success, "non-object payload rejected");
    requireContains(nonObject.payloadJson, "PAYLOAD_JSON_OBJECT_REQUIRED", "non-object payload");
    require(countRows(*fixture.connection, "trade_log") == 0, "non-object writes no trade_log");

    const auto sensitive = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        std::string("{")
            + "\"accountId\":\"" + std::to_string(fixture.accountId) + "\","
            + "\"portfolioId\":\"" + std::to_string(fixture.portfolioId) + "\","
            + "\"movementType\":\"Deposit\","
            + "\"amountMinor\":12345,"
            + "\"occurredAt\":\"2026-06-02T12:34:56Z\","
            + "\"sourceMemo\":\"contains secret token\""
            + "}");
    require(!sensitive.success, "sensitive memo rejected");
    requireContains(sensitive.payloadJson, "SENSITIVE_MEMO_TOKEN", "sensitive memo response");
    require(countRows(*fixture.connection, "trade_log") == 0, "sensitive memo writes no trade_log");
}

void testIdempotencyAndRepositoryFailure(const Harness& h)
{
    auto fixture = makeDb(h, "idempotency");
    const auto first = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture, "task-198-idempotent-cash"));
    const auto second = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture, "task-198-idempotent-cash"));
    require(first.success && second.success, "duplicate cash movement succeeds idempotently");
    requireContains(second.payloadJson, "\"idempotent\":true", "duplicate response");
    requireContains(second.payloadJson, "\"duplicate\":true", "duplicate response");
    require(countRows(*fixture.connection, "trade_log") == 1, "duplicate writes no extra trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "duplicate writes no extra cash_adjustment");

    const auto failure = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        std::string("{")
            + "\"accountId\":\"999999\","
            + "\"portfolioId\":\"" + std::to_string(fixture.portfolioId) + "\","
            + "\"instrumentId\":\"" + std::to_string(fixture.instrumentId) + "\","
            + "\"securityCode\":\"159509\","
            + "\"tradeSide\":\"Buy\","
            + "\"quantityUnits\":1000000,"
            + "\"priceAmountMinor\":20100,"
            + "\"grossAmountMinor\":20100,"
            + "\"feeAmountMinor\":100,"
            + "\"taxAmountMinor\":20,"
            + "\"occurredAt\":\"2026-06-02T12:34:56Z\","
            + "\"sourceMemo\":\"missing account safe failure\","
            + "\"requestId\":\"task-198-failure\","
            + "\"idempotencyKey\":\"task-198-failure-key\""
            + "}");
    require(!failure.success, "repository failure maps to DataService failure");
    requireContains(failure.payloadJson, "\"status\":\"REFERENCE_VALIDATION_FAILED\"", "failure response");
    requireContains(failure.payloadJson, "REFERENCE_ROW_MISSING", "failure response");
    requireSafePayload(failure.payloadJson);
}

void testNoUnauthorizedPathChanges(const Harness& h)
{
    require(fs::exists(h.root / "migrations" / "001_initial_schema.sql"), "migration 001 retained");
    require(fs::exists(h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql"),
        "migration 002 retained");
    require(fs::exists(h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp"),
        "TASK-192 repository retained");
    require(fs::exists(h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualCashMovementRepository.cpp"),
        "TASK-196 repository retained");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntryDataServiceWriteWiringImplementation",
        "tests/CMakeLists");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingBrokerOrderImplementation",
        "broker gates retained");
}

void runAll(const Harness& h)
{
    testDocsAndIndexes(h);
    testStaticBoundaries(h);
    testManualTransactionWrite(h);
    testManualCashMovementWrite(h);
    testInvalidPayloadNoWrite(h);
    testIdempotencyAndRepositoryFailure(h);
    testNoUnauthorizedPathChanges(h);
    require(g_checks >= 55, "TASK-198 test must execute at least 55 checks");
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
