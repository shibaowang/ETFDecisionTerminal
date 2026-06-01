#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace fs = std::filesystem;

struct Harness {
    fs::path root;
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

void requireNoTokens(
    const std::vector<fs::path>& files,
    const std::vector<std::string>& tokens,
    const std::string& context)
{
    for (const auto& file : files) {
        const auto text = readFile(file);
        for (const auto& token : tokens) {
            if (contains(text, token)) {
                throw std::runtime_error(context + " found `" + token + "` in " + file.string());
            }
        }
    }
}

std::string dataServiceActions(const Harness& h)
{
    return readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string dataServiceActionsHeader(const Harness& h)
{
    return readFile(
        h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h");
}

std::string registrar(const Harness& h)
{
    return readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp");
}

std::string validationHeader(const Harness& h)
{
    return readFile(
        h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi"
        / "ShellAccountingManualTransactionCashMovementValidation.h");
}

std::string validationSource(const Harness& h)
{
    return readFile(
        h.root / "libs" / "DataServiceApi" / "src"
        / "ShellAccountingManualTransactionCashMovementValidation.cpp");
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

std::string manualTransactionHandler(const Harness& h)
{
    return functionBody(dataServiceActions(h), "handleAccountingManualEntryTransactionCreate");
}

std::string manualCashHandler(const Harness& h)
{
    return functionBody(dataServiceActions(h), "handleAccountingManualEntryCashMovementCreate");
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-183-manual-entry-persistence";
    request.traceId = "task-183-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-06-01T12:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

etfdt::protocol::ProtocolResponse dispatchManualAction(const std::string& action, const std::string& payloadJson)
{
    etfdt::data_access::SQLiteConnection connection;
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    return dispatcher.dispatch(envelope(action, payloadJson));
}

std::string validManualTransactionPayload()
{
    return std::string("{")
        + "\"accountId\":\"acct-1\","
        + "\"portfolioId\":\"pf-1\","
        + "\"securityCode\":\"ETF-001\","
        + "\"tradeSide\":\"Buy\","
        + "\"quantityUnits\":100,"
        + "\"priceAmountMinor\":1234,"
        + "\"grossAmountMinor\":123400,"
        + "\"feeAmountMinor\":10,"
        + "\"taxAmountMinor\":0,"
        + "\"occurredAt\":\"2026-06-01T12:00:00Z\","
        + "\"sourceMemo\":\"task 183 validation only\""
        + "}";
}

std::string validManualCashMovementPayload()
{
    return std::string("{")
        + "\"accountId\":\"acct-1\","
        + "\"portfolioId\":\"pf-1\","
        + "\"movementType\":\"Deposit\","
        + "\"amountMinor\":50000,"
        + "\"occurredAt\":\"2026-06-01T12:00:00Z\","
        + "\"sourceMemo\":\"task 183 validation only\""
        + "}";
}

void requireWriteNotImplementedResponse(const etfdt::protocol::ProtocolResponse& response)
{
    require(!response.success, "TASK-182 validation-only response must fail closed");
    require(response.errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "valid payload must remain unavailable until persistence is separately authorized");
    requireAll(response.payloadJson, {
        "\"validationOnly\":true",
        "\"validationAccepted\":true",
        "\"writeImplemented\":false",
        "\"writeEnabled\":false",
        "\"databaseWritten\":false",
        "\"tradeLogWritten\":false",
        "\"cashFactsWritten\":false",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
        "\"status\":\"VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED\"",
    }, "TASK-182 validation-only response");
}

void requireNoPersistentIds(const std::string& payload)
{
    for (const auto& token : {"tradeLogId", "cashFactId", "auditLogId", "ledgerId", "persistentId"}) {
        requireNotContains(payload, token, "validation-only response");
    }
}

void testGate(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "164_shell_accounting_manual_entry_persistence_authorization_gate.md"),
        "docs/164 must exist");
    require(fs::exists(h.root / "docs" / "165_shell_accounting_manual_entry_persistence_authorization_test_plan.md"),
        "docs/165 must exist");
    const auto doc = readFile(h.root / "docs" / "164_shell_accounting_manual_entry_persistence_authorization_gate.md");
    requireAll(doc, {
        "TASK-183",
        "gate-only",
        "does not implement persistence",
        "DataService-only",
        "validation-first",
        "Repository Boundary Policy",
        "Transaction / Rollback Policy",
        "Idempotency / Duplicate Policy",
        "Audit Policy",
        "No Broker / Network / Trading Policy",
    }, "docs/164");
}

void testDocs(const Harness& h)
{
    const auto doc = readFile(h.root / "docs" / "164_shell_accounting_manual_entry_persistence_authorization_gate.md");
    const auto plan = readFile(h.root / "docs" / "165_shell_accounting_manual_entry_persistence_authorization_test_plan.md");
    requireAll(doc, {
        "DataServiceActions.cpp",
        "DataServiceActions.h",
        "DataServiceActionRegistrar.cpp",
        "writeImplemented=false",
        "manual transaction persistence is not broker order placement",
        "manual cash movement persistence is not a broker transfer",
        "migrations/001_initial_schema.sql",
        "Repository scaffold / DTO mapping gate",
    }, "docs/164 required policy");
    requireAll(plan, {
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "No DataAccess write repository",
        "No broker SDK",
        "No automatic trading",
    }, "docs/165 required plan");
}

void testDocsIndexPrompt(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsIndex = readFile(h.root / "docs" / "README.md");
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    requireContains(readme, "docs/164_shell_accounting_manual_entry_persistence_authorization_gate.md", "README");
    requireContains(readme, "docs/165_shell_accounting_manual_entry_persistence_authorization_test_plan.md", "README");
    requireContains(docsIndex, "164_shell_accounting_manual_entry_persistence_authorization_gate.md", "docs/README");
    requireContains(docsIndex, "165_shell_accounting_manual_entry_persistence_authorization_test_plan.md", "docs/README");
    requireContains(prompt, "TASK-183", "docs/12");
    requireContains(prompt, "writeImplemented=false", "docs/12 TASK-183 rule");
}

void testDataServiceActionsCppUnmodified(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireNotContains(source, "TASK-183", "DataServiceActions.cpp");
    requireAll(source, {
        "validateManualTransactionEntry",
        "validateManualCashMovement",
        "manualEntryValidationOnlyResponse",
        "VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED",
    }, "DataServiceActions.cpp TASK-182 validation-only wiring");
}

void testDataServiceActionsHUnmodified(const Harness& h)
{
    const auto header = dataServiceActionsHeader(h);
    requireNotContains(header, "TASK-183", "DataServiceActions.h");
    requireNotContains(header, "Persistence", "DataServiceActions.h");
    requireNotContains(header, "Repository", "DataServiceActions.h");
}

void testRegistrarUnmodified(const Harness& h)
{
    const auto source = registrar(h);
    requireNotContains(source, "TASK-183", "DataServiceActionRegistrar.cpp");
    requireNotContains(source, "Persistence", "DataServiceActionRegistrar.cpp");
}

void testValidationCodeUnmodified(const Harness& h)
{
    const auto header = validationHeader(h);
    const auto source = validationSource(h);
    requireNotContains(header, "TASK-183", "TASK-178 validation header");
    requireNotContains(source, "TASK-183", "TASK-178 validation source");
    requireContains(header, "ShellAccountingManualTransactionEntry", "TASK-178 validation header");
    requireContains(source, "validateManualCashMovement", "TASK-178 validation source");
}

void testTransactionWriteNotImplemented(const Harness&)
{
    const auto response = dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload());
    requireWriteNotImplementedResponse(response);
}

void testCashWriteNotImplemented(const Harness&)
{
    const auto response = dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload());
    requireWriteNotImplementedResponse(response);
}

void testNoPersistentIds(const Harness&)
{
    const auto tradeResponse = dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload());
    const auto cashResponse = dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload());
    requireNoPersistentIds(tradeResponse.payloadJson);
    requireNoPersistentIds(cashResponse.payloadJson);
}

void testNoDataAccessWriteRepository(const Harness& h)
{
    const auto dataAccessCMake = readFile(h.root / "libs" / "DataAccess" / "CMakeLists.txt");
    requireNotContains(dataAccessCMake, "ManualTransactionRepository", "DataAccess CMake");
    requireNotContains(dataAccessCMake, "ManualCashMovementRepository", "DataAccess CMake");
    requireNotContains(dataAccessCMake, "ManualEntryPersistence", "DataAccess CMake");
}

void testNoManualPersistenceRepository(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "DataAccess"), {
        "ManualEntryPersistenceRepository",
        "ManualTransactionWriteRepository",
        "ManualCashMovementWriteRepository",
        "persistManualTransaction",
        "persistManualCashMovement",
    }, "DataAccess manual persistence repository scan");
}

void testNoSql(const Harness& h)
{
    const auto transaction = manualTransactionHandler(h);
    const auto cash = manualCashHandler(h);
    for (const auto& token : {"INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "CREATE ", "DROP ", "ALTER "}) {
        requireNotContains(transaction, token, "manual transaction handler SQL scan");
        requireNotContains(cash, token, "manual cash handler SQL scan");
    }
}

void testNoSqliteWrite(const Harness& h)
{
    const auto transaction = manualTransactionHandler(h);
    const auto cash = manualCashHandler(h);
    for (const auto& token : {"executeSql", "executeNonQuery", "sqlite3_exec", "beginTransaction", "commit"}) {
        requireNotContains(transaction, token, "manual transaction handler SQLite scan");
        requireNotContains(cash, token, "manual cash handler SQLite scan");
    }
}

void testNoTradeLogWrite(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    requireNotContains(source, "trade_log", "manual handlers");
    requireNotContains(source, "tradeLogWritten\":true", "manual handlers");
}

void testNoCashFactWrite(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    requireNotContains(source, "cash_fact", "manual handlers");
    requireNotContains(source, "cash_ledger", "manual handlers");
    requireNotContains(source, "cashFactsWritten\":true", "manual handlers");
}

void testNoAuditLedgerWrite(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    requireNotContains(source, "audit_log", "manual handlers");
    requireNotContains(source, "ledgerWritten\":true", "manual handlers");
    requireNotContains(source, "auditWritten\":true", "manual handlers");
}

void testSchemaUnmodified(const Harness& h)
{
    const auto schema = readFile(h.root / "migrations" / "001_initial_schema.sql");
    requireNotContains(schema, "TASK-183", "initial schema");
    requireNotContains(schema, "manual_entry_persistence", "initial schema");
}

void testNoNewMigration(const Harness& h)
{
    for (const auto& file : filesUnder(h.root / "migrations")) {
        requireNotContains(file.filename().string(), "183", "migration filename");
        requireNotContains(file.filename().string(), "manual_entry_persistence", "migration filename");
    }
}

void testProductionQmlUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "qml"), {
        "manualTransaction",
        "manualCashMovement",
        "ManualEntryPersistence",
        "persistManual",
    }, "production QML manual persistence scan");
}

void testStartupUnmodified(const Harness& h)
{
    const auto startup = readFile(h.root / "apps" / "ETFDecisionShell" / "src" / "main.cpp");
    requireNotContains(startup, "manualTransaction", "production startup");
    requireNotContains(startup, "ManualEntryPersistence", "production startup");
}

void testPresenterControllerUnmodified(const Harness& h)
{
    const auto shellServices = filesUnder(h.root / "libs" / "ShellServices");
    const auto shellCore = filesUnder(h.root / "libs" / "ShellCore");
    requireNoTokens(shellServices, {"ManualEntryPersistence", "persistManualTransaction", "persistManualCashMovement"},
        "ShellServices manual persistence scan");
    requireNoTokens(shellCore, {"ManualEntryPersistence", "persistManualTransaction", "persistManualCashMovement"},
        "ShellCore manual persistence scan");
}

void testAccountingEngineReplayUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "AccountingEngine"), {
        "ManualEntryPersistence",
        "persistManualTransaction",
        "persistManualCashMovement",
    }, "AccountingEngine manual persistence scan");
}

void testStrategyMarketUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "StrategyEngine"), {"ManualEntryPersistence", "manualCashMovement"},
        "StrategyEngine manual persistence scan");
    requireNoTokens(filesUnder(h.root / "libs" / "MarketEngine"), {"ManualEntryPersistence", "manualCashMovement"},
        "MarketEngine manual persistence scan");
}

void testNoTradeDraftSuggestion(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    requireNotContains(source, "TradeDraft", "manual handlers");
    requireNotContains(source, "Suggestion", "manual handlers");
    requireNotContains(source, "recommend", "manual handlers");
}

void testNoBrokerSdk(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    requireNotContains(source, "Broker", "manual handlers");
    requireNotContains(source, "broker", "manual handlers");
}

void testNoNetworkEndpoint(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    for (const auto& token : {"http://", "https://", "endpoint", "network", "socket"}) {
        requireNotContains(source, token, "manual handlers network scan");
    }
}

void testNoCredentials(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    for (const auto& token : {"credential", "secret", "token", "password", "apiKey"}) {
        requireNotContains(source, token, "manual handlers credential scan");
    }
}

void testNoRealOrderId(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    requireNotContains(source, "brokerOrderId", "manual handlers");
    requireNotContains(source, "realOrderId", "manual handlers");
}

void testNoRealOrderPlacement(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    for (const auto& token : {"placeOrder", "executeOrder", "submitOrder", "orderPlacement"}) {
        requireNotContains(source, token, "manual handlers order scan");
    }
}

void testNoAutomaticTrading(const Harness& h)
{
    const auto source = manualTransactionHandler(h) + manualCashHandler(h);
    requireNotContains(source, "automaticTrading", "manual handlers");
    requireNotContains(source, "autoTrade", "manual handlers");
}

void testTask182StillValid(const Harness& h)
{
    const auto cmake = readFile(
        h.root / "tests" / "ShellAccountingManualEntryDataServiceActionValidationWiring" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_entry_dataservice_action_validation_wiring", "TASK-182 CMake");
    testTransactionWriteNotImplemented(h);
    testCashWriteNotImplemented(h);
}

void testTask181StillValid(const Harness& h)
{
    const auto cmake = readFile(
        h.root / "tests" / "ShellAccountingManualEntryDataServiceActionImplementationAuthorizationGate" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_entry_dataservice_action_implementation_authorization", "TASK-181 CMake");
}

void testTask180StillValid(const Harness& h)
{
    const auto cmake = readFile(
        h.root / "tests" / "ShellAccountingManualEntryDataServiceActionScaffold" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_entry_dataservice_action_scaffold", "TASK-180 CMake");
}

void testTask178StillPure(const Harness& h)
{
    const auto source = validationSource(h);
    requireNotContains(source, "SQLite", "TASK-178 validation source");
    requireNotContains(source, "Repository", "TASK-178 validation source");
    requireNotContains(source, "trade_log", "TASK-178 validation source");
}

void testTask177StillValid(const Harness& h)
{
    const auto cmake = readFile(
        h.root / "tests" / "ShellAccountingManualTransactionCashMovementMvpAuthorizationGate" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization", "TASK-177 CMake");
}

void testBrokerGatesRetained(const Harness& h)
{
    const auto testsCmake = readFile(h.root / "tests" / "CMakeLists.txt");
    requireAll(testsCmake, {
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementation",
    }, "broker gate retention");
}

void testFutureSplitDocumented(const Harness& h)
{
    const auto doc = readFile(h.root / "docs" / "164_shell_accounting_manual_entry_persistence_authorization_gate.md");
    requireAll(doc, {
        "Repository scaffold / DTO mapping gate",
        "Repository implementation",
        "Manual transaction write implementation",
        "Manual cash movement write implementation",
        "Replay / read model integration",
        "QML / UI exposure",
    }, "future task split");
}

Harness parseArgs(int argc, char** argv, std::string& testCase)
{
    Harness harness;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--source-root" && i + 1 < argc) {
            harness.root = fs::path(argv[++i]);
        } else if (arg == "--case" && i + 1 < argc) {
            testCase = argv[++i];
        }
    }
    require(!harness.root.empty(), "--source-root is required");
    require(!testCase.empty(), "--case is required");
    return harness;
}

} // namespace

int main(int argc, char** argv)
{
    try {
        std::string testCase;
        const auto harness = parseArgs(argc, argv, testCase);

        if (testCase == "gate") testGate(harness);
        else if (testCase == "docs") testDocs(harness);
        else if (testCase == "docs_index_prompt") testDocsIndexPrompt(harness);
        else if (testCase == "dataserviceactions_cpp_unmodified") testDataServiceActionsCppUnmodified(harness);
        else if (testCase == "dataserviceactions_h_unmodified") testDataServiceActionsHUnmodified(harness);
        else if (testCase == "registrar_unmodified") testRegistrarUnmodified(harness);
        else if (testCase == "validation_code_unmodified") testValidationCodeUnmodified(harness);
        else if (testCase == "transaction_write_not_implemented") testTransactionWriteNotImplemented(harness);
        else if (testCase == "cash_write_not_implemented") testCashWriteNotImplemented(harness);
        else if (testCase == "no_persistent_ids") testNoPersistentIds(harness);
        else if (testCase == "no_dataaccess_write_repository") testNoDataAccessWriteRepository(harness);
        else if (testCase == "no_manual_persistence_repository") testNoManualPersistenceRepository(harness);
        else if (testCase == "no_sql") testNoSql(harness);
        else if (testCase == "no_sqlite_write") testNoSqliteWrite(harness);
        else if (testCase == "no_trade_log_write") testNoTradeLogWrite(harness);
        else if (testCase == "no_cash_fact_write") testNoCashFactWrite(harness);
        else if (testCase == "no_audit_ledger_write") testNoAuditLedgerWrite(harness);
        else if (testCase == "schema_unmodified") testSchemaUnmodified(harness);
        else if (testCase == "no_new_migration") testNoNewMigration(harness);
        else if (testCase == "production_qml_unmodified") testProductionQmlUnmodified(harness);
        else if (testCase == "startup_unmodified") testStartupUnmodified(harness);
        else if (testCase == "presenter_controller_unmodified") testPresenterControllerUnmodified(harness);
        else if (testCase == "accountingengine_replay_unmodified") testAccountingEngineReplayUnmodified(harness);
        else if (testCase == "strategy_market_unmodified") testStrategyMarketUnmodified(harness);
        else if (testCase == "no_tradedraft_suggestion") testNoTradeDraftSuggestion(harness);
        else if (testCase == "no_broker_sdk") testNoBrokerSdk(harness);
        else if (testCase == "no_network_endpoint") testNoNetworkEndpoint(harness);
        else if (testCase == "no_credentials") testNoCredentials(harness);
        else if (testCase == "no_real_order_id") testNoRealOrderId(harness);
        else if (testCase == "no_real_order_placement") testNoRealOrderPlacement(harness);
        else if (testCase == "no_automatic_trading") testNoAutomaticTrading(harness);
        else if (testCase == "task182_still_valid") testTask182StillValid(harness);
        else if (testCase == "task181_still_valid") testTask181StillValid(harness);
        else if (testCase == "task180_still_valid") testTask180StillValid(harness);
        else if (testCase == "task178_still_pure") testTask178StillPure(harness);
        else if (testCase == "task177_still_valid") testTask177StillValid(harness);
        else if (testCase == "broker_gates_retained") testBrokerGatesRetained(harness);
        else if (testCase == "future_split_documented") testFutureSplitDocumented(harness);
        else throw std::runtime_error("Unknown test case: " + testCase);

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
