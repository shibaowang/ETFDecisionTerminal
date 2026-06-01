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

void requireNoTokens(const std::vector<fs::path>& files, const std::vector<std::string>& tokens, const std::string& context)
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
    return readFile(h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h");
}

std::string registrar(const Harness& h)
{
    return readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp");
}

std::string handlerBody(const std::string& source, const std::string& functionName)
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

std::string manualScaffoldHelperBody(const Harness& h)
{
    return handlerBody(dataServiceActions(h), "manualEntryValidationOnlyResponse");
}

std::string manualTransactionHandlerBody(const Harness& h)
{
    return handlerBody(dataServiceActions(h), "handleAccountingManualEntryTransactionCreate");
}

std::string manualCashMovementHandlerBody(const Harness& h)
{
    return handlerBody(dataServiceActions(h), "handleAccountingManualEntryCashMovementCreate");
}

std::vector<fs::path> validationFiles(const Harness& h)
{
    return {
        h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
            "ShellAccountingManualTransactionCashMovementValidation.h",
        h.root / "libs" / "DataServiceApi" / "src" /
            "ShellAccountingManualTransactionCashMovementValidation.cpp",
    };
}

etfdt::protocol::MessageEnvelope envelope(std::string action)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-181-manual-entry";
    request.traceId = "task-181-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-06-01T12:00:00Z";
    if (request.action == etfdt::data_service_api::kActionAccountingManualTransactionCreate) {
        request.payloadJson =
            R"({"accountId":"acct-1","portfolioId":"pf-1","securityCode":"ETF-001","tradeSide":"Buy","quantityUnits":100,"priceAmountMinor":1234,"grossAmountMinor":123400,"feeAmountMinor":10,"taxAmountMinor":0,"occurredAt":"2026-06-01T12:00:00Z","sourceMemo":"validation only"})";
    } else {
        request.payloadJson =
            R"({"accountId":"acct-1","portfolioId":"pf-1","movementType":"Deposit","amountMinor":50000,"occurredAt":"2026-06-01T12:00:00Z","sourceMemo":"validation only"})";
    }
    return request;
}

etfdt::protocol::ProtocolResponse dispatchScaffoldAction(const std::string& action)
{
    etfdt::data_access::SQLiteConnection connection;
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    return dispatcher.dispatch(envelope(action));
}

void requireDisabledResponse(const etfdt::protocol::ProtocolResponse& response, const std::string& reason)
{
    (void)reason;
    require(!response.success, "manual entry implementation gate requires fail-closed validation-only response");
    require(response.errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "manual entry validation-only accepted response must return E9001_SERVICE_UNAVAILABLE");
    requireContains(response.payloadJson, "\"implemented\":false", "scaffold payload");
    requireContains(response.payloadJson, "\"validationOnly\":true", "scaffold payload");
    requireContains(response.payloadJson, "\"validationAccepted\":true", "scaffold payload");
    requireContains(response.payloadJson, "\"writeImplemented\":false", "scaffold payload");
    requireContains(response.payloadJson, "\"manualEntryEnabled\":false", "scaffold payload");
    requireContains(response.payloadJson, "\"writeEnabled\":false", "scaffold payload");
    requireContains(response.payloadJson, "\"databaseWritten\":false", "scaffold payload");
    requireContains(response.payloadJson, "\"repositoryCalled\":false", "scaffold payload");
    requireContains(response.payloadJson, "\"status\":\"VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED\"", "scaffold payload");
    requireContains(response.payloadJson, "MANUAL_ENTRY_VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED", "scaffold payload");
}

void testDocs(const Harness& h)
{
    const auto doc160 = readFile(h.root / "docs" / "160_shell_accounting_manual_entry_dataservice_action_implementation_authorization_gate.md");
    const auto doc161 = readFile(h.root / "docs" / "161_shell_accounting_manual_entry_dataservice_action_implementation_authorization_test_plan.md");
    requireAll(doc160, {
        "TASK-181",
        "gate-only",
        "TASK-182 completed the",
        "`DataServiceActions.cpp`",
        "`DataServiceActions.h`",
        "`DataServiceActionRegistrar.cpp`",
        "validation-only",
        "Payload parsing plus TASK-178 validation wiring",
        "must not write a database",
        "Manual transaction implementation is not broker order placement",
        "Manual cash movement implementation is not broker transfer",
        "Broker sandbox new capability work remains paused",
    }, "docs/160");
    requireAll(doc161, {
        "TASK-181",
        "Test Matrix",
        "Required Probes",
        "Production source evolution",
        "TASK-180 scaffold semantics evolved by TASK-182",
        "No persistence expansion",
        "No broker / network / credentials / automatic trading",
    }, "docs/161");
}

void testDocsIndex(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsReadme = readFile(h.root / "docs" / "README.md");
    requireContains(readme, "docs/160_shell_accounting_manual_entry_dataservice_action_implementation_authorization_gate.md", "README");
    requireContains(readme, "docs/161_shell_accounting_manual_entry_dataservice_action_implementation_authorization_test_plan.md", "README");
    requireContains(docsReadme, "160_shell_accounting_manual_entry_dataservice_action_implementation_authorization_gate.md", "docs/README");
    requireContains(docsReadme, "161_shell_accounting_manual_entry_dataservice_action_implementation_authorization_test_plan.md", "docs/README");
}

void testPromptTemplate(const Harness& h)
{
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    requireAll(prompt, {
        "TASK-181",
        "docs/160",
        "docs/161",
        "future implementation authorization gate only",
        "Do not modify",
        "validation-only unavailable",
        "write-not-implemented responses",
    }, "docs/12");
}

void testDataServiceActionsCppUnmodified(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireNotContains(source, "TASK-181", "DataServiceActions.cpp");
    requireContains(manualTransactionHandlerBody(h), "validateManualTransactionEntry", "manual transaction handler");
    requireContains(manualCashMovementHandlerBody(h), "validateManualCashMovement", "manual cash movement handler");
}

void testDataServiceActionsHUnmodified(const Harness& h)
{
    const auto header = dataServiceActionsHeader(h);
    requireNotContains(header, "TASK-181", "DataServiceActions.h");
    requireContains(header, "kActionAccountingManualTransactionCreate", "DataServiceActions.h");
    requireContains(header, "kActionAccountingManualCashMovementCreate", "DataServiceActions.h");
}

void testRegistrarUnmodified(const Harness& h)
{
    const auto source = registrar(h);
    requireNotContains(source, "TASK-181", "DataServiceActionRegistrar.cpp");
    requireContains(source, "handleAccountingManualEntryTransactionCreate", "DataServiceActionRegistrar.cpp");
    requireContains(source, "handleAccountingManualEntryCashMovementCreate", "DataServiceActionRegistrar.cpp");
}

void testTransactionScaffoldDisabled()
{
    requireDisabledResponse(
        dispatchScaffoldAction(etfdt::data_service_api::kActionAccountingManualTransactionCreate),
        "MANUAL_TRANSACTION_ENTRY_NOT_IMPLEMENTED");
}

void testCashScaffoldDisabled()
{
    requireDisabledResponse(
        dispatchScaffoldAction(etfdt::data_service_api::kActionAccountingManualCashMovementCreate),
        "MANUAL_CASH_MOVEMENT_NOT_IMPLEMENTED");
}

void testNoSuccessImplementation(const Harness& h)
{
    requireNotContains(manualScaffoldHelperBody(h), "success = true", "manual scaffold helper");
    requireNotContains(manualTransactionHandlerBody(h), "success = true", "manual transaction handler");
    requireNotContains(manualCashMovementHandlerBody(h), "success = true", "manual cash movement handler");
}

void testNoPayloadToWritePath(const Harness& h)
{
    requireContains(manualTransactionHandlerBody(h), "payloadJson", "manual transaction handler");
    requireContains(manualCashMovementHandlerBody(h), "payloadJson", "manual cash movement handler");
    requireNotContains(manualTransactionHandlerBody(h), "Repository", "manual transaction handler");
    requireNotContains(manualCashMovementHandlerBody(h), "Repository", "manual cash movement handler");
    requireNotContains(manualTransactionHandlerBody(h), "INSERT", "manual transaction handler");
    requireNotContains(manualCashMovementHandlerBody(h), "INSERT", "manual cash movement handler");
}

void testNoSqliteWrite(const Harness& h)
{
    requireNoTokens({h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp"}, {
        "insertManualTransaction",
        "insertCashMovement",
        "INSERT INTO manual_transaction",
        "INSERT INTO manual_cash_movement",
    }, "manual entry SQLite write");
    requireNotContains(manualScaffoldHelperBody(h), "executeSql", "manual scaffold helper");
}

void testNoTradeLogWrite(const Harness& h)
{
    requireNotContains(manualScaffoldHelperBody(h), "trade_log", "manual scaffold helper");
    requireNotContains(manualTransactionHandlerBody(h), "trade_log", "manual transaction handler");
}

void testNoCashFactWrite(const Harness& h)
{
    const auto helper = manualScaffoldHelperBody(h);
    requireNotContains(helper, "cash_facts", "manual scaffold helper");
    requireNotContains(helper, "cash_ledger", "manual scaffold helper");
}

void testNoAuditLedgerWrite(const Harness& h)
{
    const auto helper = manualScaffoldHelperBody(h);
    requireNotContains(helper, "audit_log", "manual scaffold helper");
    requireNotContains(helper, "ledgerWrite", "manual scaffold helper");
}

void testNoRepositoryCall(const Harness& h)
{
    requireNotContains(manualTransactionHandlerBody(h), "Repository", "manual transaction handler");
    requireNotContains(manualCashMovementHandlerBody(h), "Repository", "manual cash movement handler");
}

void testNoDataAccessWriteRepository(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "DataAccess" / "include");
    auto src = filesUnder(h.root / "libs" / "DataAccess" / "src");
    files.insert(files.end(), src.begin(), src.end());
    requireNoTokens(files, {
        "ManualTransactionRepository",
        "ManualCashMovementRepository",
        "ManualEntryRepository",
        "insertManualTransaction",
        "insertCashMovement",
        "manual_transaction",
        "manual_cash_movement",
    }, "DataAccess manual write repository");
}

void testNoAccountingEngineReplay(const Harness& h)
{
    requireNotContains(manualTransactionHandlerBody(h), "runShellAccountingReadOnlyReplay", "manual transaction handler");
    requireNotContains(manualCashMovementHandlerBody(h), "runShellAccountingReadOnlyReplay", "manual cash movement handler");
}

void testValidationScaffoldStillPure(const Harness& h)
{
    requireNoTokens(validationFiles(h), {
        "SQLite",
        "trade_log",
        "cash_facts",
        "audit_log",
        "ledger",
        "Repository",
        "DataServiceActions",
        "QNetwork",
        "BrokerSdk",
    }, "TASK-178 validation scaffold");
}

void testSchemaUnmodified(const Harness& h)
{
    const auto migration = readFile(h.root / "migrations" / "001_initial_schema.sql");
    require(!contains(migration, "manual_transaction"), "schema must not add manual_transaction");
    require(!contains(migration, "manual_cash_movement"), "schema must not add manual_cash_movement");
}

void testNoNewMigration(const Harness& h)
{
    for (const auto& file : filesUnder(h.root / "migrations")) {
        require(file.filename() == "001_initial_schema.sql", "TASK-181 must not add migration " + file.string());
    }
}

void testProductionQmlUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "qml"), {
        "manualTransaction",
        "manualCashMovement",
        "manualBuy",
        "manualSell",
        "recordDeposit",
        "recordWithdraw",
        "Manual Entry",
    }, "production QML manual entry UI");
}

void testStartupUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "src"), {
        "ManualEntry",
        "ManualTransaction",
        "ManualCashMovement",
        "registerManual",
    }, "production startup manual entry registration");
}

void testNoStrategyMarketChange(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "StrategyEngine");
    auto market = filesUnder(h.root / "libs" / "MarketEngine");
    files.insert(files.end(), market.begin(), market.end());
    requireNoTokens(files, {
        "manualTradeSuggestion",
        "manualTransactionSuggestion",
        "manualCashMovement",
        "goldenTowerSuggestion",
        "sniperSuggestion",
    }, "StrategyEngine/MarketEngine manual entry");
}

void testNoTradeDraft(const Harness& h)
{
    requireContains(manualScaffoldHelperBody(h), "\\\"tradeDraftGenerated\\\":false", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "createTradeDraft", "manual scaffold helper");
}

void testNoSuggestion(const Harness& h)
{
    requireContains(manualScaffoldHelperBody(h), "\\\"tradeSuggestionGenerated\\\":false", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "SuggestionEngine", "manual scaffold helper");
}

void testNoBroker(const Harness& h)
{
    requireContains(manualScaffoldHelperBody(h), "\\\"brokerSdkCalled\\\":false", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "BrokerSdk", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "submitOrder", "manual scaffold helper");
}

void testNoNetworkEndpoint(const Harness& h)
{
    requireContains(manualScaffoldHelperBody(h), "\\\"networkAccessed\\\":false", "manual scaffold helper");
    requireContains(manualScaffoldHelperBody(h), "\\\"endpointAccessed\\\":false", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "QNetwork", "manual scaffold helper");
}

void testNoCredentialsSecretValues(const Harness& h)
{
    requireContains(manualScaffoldHelperBody(h), "\\\"credentialsAccessed\\\":false", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "getenv", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "secretKey", "manual scaffold helper");
}

void testNoRealOrderPlacement(const Harness& h)
{
    requireContains(manualScaffoldHelperBody(h), "\\\"realOrderPlacement\\\":false", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "placeOrder", "manual scaffold helper");
}

void testNoAutomaticTrading(const Harness& h)
{
    requireContains(manualScaffoldHelperBody(h), "\\\"automaticTrading\\\":false", "manual scaffold helper");
    requireNotContains(manualScaffoldHelperBody(h), "executeStrategy", "manual scaffold helper");
}

void testTask180ScaffoldStillValid(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "ShellAccountingManualEntryDataServiceActionScaffold" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_entry_dataservice_action_scaffold_dispatcher_returns_unavailable", "TASK-180 CMake");
    requireContains(cmake, "shell_accounting_manual_entry_dataservice_action_scaffold_no_repository_call", "TASK-180 CMake");
}

void testTask179GateStillValid(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "ShellAccountingManualEntryDataServiceActionAuthorizationGate" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_entry_dataservice_action_authorization_no_sqlite_write", "TASK-179 CMake");
    requireContains(cmake, "shell_accounting_manual_entry_dataservice_action_authorization_existing_broker_gates_retained", "TASK-179 CMake");
}

void testTask178ValidationStillValid(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "ShellAccountingManualTransactionCashMovementValidationScaffold" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_validation_scaffold_no_sqlite_access", "TASK-178 CMake");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_validation_scaffold_existing_broker_gates_retained", "TASK-178 CMake");
}

void testTask177GateStillValid(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "ShellAccountingManualTransactionCashMovementMvpAuthorizationGate" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization_no_dataserviceactions_manual_entry_action", "TASK-177 CMake");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization_existing_broker_gates_retained", "TASK-177 CMake");
}

void testExistingBrokerGatesRetained(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "CMakeLists.txt");
    requireAll(cmake, {
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
        "ShellAccountingBrokerSandboxRuntimeEnablementStateDisabledWiring",
    }, "tests/CMake broker gates");
}

void testNoGateWeakeningOrSkip(const Harness& h)
{
    const std::vector<fs::path> gateFiles = {
        h.root / "tests" / "ShellAccountingBrokerAdapterDisabledWiring" / "CMakeLists.txt",
        h.root / "tests" / "ShellAccountingBrokerOrderImplementation" / "CMakeLists.txt",
        h.root / "tests" / "ShellAccountingRealBrokerOrderAuthorizationGate" / "CMakeLists.txt",
        h.root / "tests" / "ShellAccountingRealBrokerOrderImplementationGate" / "CMakeLists.txt",
        h.root / "tests" / "ShellAccountingBrokerSandboxRuntimeEnablementStateDisabledWiring" / "CMakeLists.txt",
    };
    for (const auto& file : gateFiles) {
        const auto text = readFile(file);
        require(!contains(text, "DISABLED"), "broker gate must not be disabled in " + file.string());
        require(!contains(text, "SKIP_REGULAR_EXPRESSION"), "broker gate must not be skipped in " + file.string());
        require(!contains(text, "WILL_FAIL"), "broker gate must not be weakened in " + file.string());
    }
}

void runCase(const Harness& h, const std::string& name)
{
    if (name == "docs") return testDocs(h);
    if (name == "docs_index") return testDocsIndex(h);
    if (name == "prompt_template") return testPromptTemplate(h);
    if (name == "dataserviceactions_cpp_unmodified") return testDataServiceActionsCppUnmodified(h);
    if (name == "dataserviceactions_h_unmodified") return testDataServiceActionsHUnmodified(h);
    if (name == "registrar_unmodified") return testRegistrarUnmodified(h);
    if (name == "transaction_scaffold_disabled") return testTransactionScaffoldDisabled();
    if (name == "cash_scaffold_disabled") return testCashScaffoldDisabled();
    if (name == "no_success_implementation") return testNoSuccessImplementation(h);
    if (name == "no_payload_to_write_path") return testNoPayloadToWritePath(h);
    if (name == "no_sqlite_write") return testNoSqliteWrite(h);
    if (name == "no_trade_log_write") return testNoTradeLogWrite(h);
    if (name == "no_cash_fact_write") return testNoCashFactWrite(h);
    if (name == "no_audit_ledger_write") return testNoAuditLedgerWrite(h);
    if (name == "no_repository_call") return testNoRepositoryCall(h);
    if (name == "no_dataaccess_write_repository") return testNoDataAccessWriteRepository(h);
    if (name == "no_accountingengine_replay") return testNoAccountingEngineReplay(h);
    if (name == "validation_scaffold_still_pure") return testValidationScaffoldStillPure(h);
    if (name == "schema_unmodified") return testSchemaUnmodified(h);
    if (name == "no_new_migration") return testNoNewMigration(h);
    if (name == "production_qml_unmodified") return testProductionQmlUnmodified(h);
    if (name == "startup_unmodified") return testStartupUnmodified(h);
    if (name == "no_strategy_market_change") return testNoStrategyMarketChange(h);
    if (name == "no_tradedraft") return testNoTradeDraft(h);
    if (name == "no_suggestion") return testNoSuggestion(h);
    if (name == "no_broker") return testNoBroker(h);
    if (name == "no_network_endpoint") return testNoNetworkEndpoint(h);
    if (name == "no_credentials_secret_values") return testNoCredentialsSecretValues(h);
    if (name == "no_real_order_placement") return testNoRealOrderPlacement(h);
    if (name == "no_automatic_trading") return testNoAutomaticTrading(h);
    if (name == "task180_scaffold_still_valid") return testTask180ScaffoldStillValid(h);
    if (name == "task179_gate_still_valid") return testTask179GateStillValid(h);
    if (name == "task178_validation_still_valid") return testTask178ValidationStillValid(h);
    if (name == "task177_gate_still_valid") return testTask177GateStillValid(h);
    if (name == "existing_broker_gates_retained") return testExistingBrokerGatesRetained(h);
    if (name == "no_gate_weakening_or_skip") return testNoGateWeakeningOrSkip(h);
    throw std::runtime_error("Unknown case: " + name);
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        Harness h;
        std::string caseName;
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--source-root" && i + 1 < argc) {
                h.root = argv[++i];
            } else if (arg == "--case" && i + 1 < argc) {
                caseName = argv[++i];
            }
        }
        require(!h.root.empty(), "--source-root is required");
        require(!caseName.empty(), "--case is required");
        runCase(h, caseName);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
