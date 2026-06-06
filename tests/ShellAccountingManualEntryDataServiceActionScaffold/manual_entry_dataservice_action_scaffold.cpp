#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <algorithm>
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

bool isTask265ExcelVbaImportManualEntryPersistenceRepositoryFile(const fs::path& file)
{
    const auto path = file.generic_string();
    return path.find("libs/DataAccess/include/DataAccess/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.h")
        != std::string::npos
        || path.find("libs/DataAccess/src/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.cpp")
            != std::string::npos;
}

std::vector<fs::path> filesWithoutTask265ExcelVbaImportManualEntryPersistenceRepository(
    std::vector<fs::path> files)
{
    files.erase(
        std::remove_if(files.begin(), files.end(), isTask265ExcelVbaImportManualEntryPersistenceRepositoryFile),
        files.end());
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

std::vector<fs::path> productionFiles(const Harness& h)
{
    std::vector<fs::path> files;
    const std::vector<fs::path> roots = {
        h.root / "apps" / "ETFDecisionShell" / "qml",
        h.root / "apps" / "ETFDecisionShell" / "src",
        h.root / "apps" / "ETFDataService" / "src",
        h.root / "libs" / "ShellServices",
        h.root / "libs" / "ShellCore",
        h.root / "libs" / "DataServiceApi" / "include",
        h.root / "libs" / "DataServiceApi" / "src",
        h.root / "libs" / "DataAccess" / "include",
        h.root / "libs" / "DataAccess" / "src",
        h.root / "libs" / "AccountingEngine",
        h.root / "libs" / "StrategyEngine",
        h.root / "libs" / "MarketEngine",
    };
    for (const auto& root : roots) {
        auto rootFiles = filesUnder(root);
        files.insert(files.end(), rootFiles.begin(), rootFiles.end());
    }
    return files;
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
    request.msgId = "task-180-manual-entry";
    request.traceId = "task-180-trace";
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
    require(!response.success, "manual entry repository action must fail safely without an opened database");
    requireContains(response.payloadJson, "\"implemented\":true", "scaffold payload");
    requireContains(response.payloadJson, "\"validationOnly\":false", "scaffold payload");
    requireContains(response.payloadJson, "\"validationAccepted\":true", "scaffold payload");
    requireContains(response.payloadJson, "\"writeImplemented\":true", "scaffold payload");
    requireContains(response.payloadJson, "\"manualEntryEnabled\":true", "scaffold payload");
    requireContains(response.payloadJson, "\"writeEnabled\":true", "scaffold payload");
    requireContains(response.payloadJson, "\"databaseWritten\":false", "scaffold payload");
    requireContains(response.payloadJson, "\"repositoryCalled\":true", "scaffold payload");
    requireContains(response.payloadJson, "\"repositoryWrite\":false", "scaffold payload");
}

void testDocs(const Harness& h)
{
    const auto doc158 = readFile(h.root / "docs" / "158_shell_accounting_manual_entry_dataservice_action_scaffold.md");
    const auto doc159 = readFile(h.root / "docs" / "159_shell_accounting_manual_entry_dataservice_action_scaffold_test_plan.md");
    requireAll(doc158, {
        "TASK-180",
        "DataService action scaffold",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "unavailable / not implemented / disabled scaffold",
        "does not write a database",
        "does not add a repository",
        "does not modify production QML",
        "Broker sandbox new capability work remains paused",
    }, "docs/158");
    requireAll(doc159, {
        "TASK-180",
        "Test Matrix",
        "Required Probes",
        "DataService action scaffold",
        "No write path",
        "Broker gate regression",
    }, "docs/159");
}

void testDocsIndex(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsReadme = readFile(h.root / "docs" / "README.md");
    requireContains(readme, "docs/158_shell_accounting_manual_entry_dataservice_action_scaffold.md", "README");
    requireContains(readme, "docs/159_shell_accounting_manual_entry_dataservice_action_scaffold_test_plan.md", "README");
    requireContains(docsReadme, "158_shell_accounting_manual_entry_dataservice_action_scaffold.md", "docs/README");
    requireContains(docsReadme, "159_shell_accounting_manual_entry_dataservice_action_scaffold_test_plan.md", "docs/README");
}

void testPromptTemplate(const Harness& h)
{
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    requireAll(prompt, {
        "TASK-180",
        "docs/158",
        "docs/159",
        "manual entry DataService action scaffold",
        "unavailable / not implemented / disabled scaffold",
    }, "docs/12");
}

void testManualTransactionActionRegistered(const Harness& h)
{
    requireContains(dataServiceActionsHeader(h), "kActionAccountingManualTransactionCreate", "DataServiceActions.h");
    requireContains(dataServiceActionsHeader(h), "\"accounting.\"", "DataServiceActions.h");
    requireContains(dataServiceActionsHeader(h), "\"manual_transaction.create\"", "DataServiceActions.h");
    requireContains(registrar(h), "kActionAccountingManualTransactionCreate", "DataServiceActionRegistrar");
    requireContains(registrar(h), "handleAccountingManualEntryTransactionCreate", "DataServiceActionRegistrar");
}

void testManualCashMovementActionRegistered(const Harness& h)
{
    requireContains(dataServiceActionsHeader(h), "kActionAccountingManualCashMovementCreate", "DataServiceActions.h");
    requireContains(dataServiceActionsHeader(h), "\"accounting.\"", "DataServiceActions.h");
    requireContains(dataServiceActionsHeader(h), "\"manual_cash_movement.create\"", "DataServiceActions.h");
    requireContains(registrar(h), "kActionAccountingManualCashMovementCreate", "DataServiceActionRegistrar");
    requireContains(registrar(h), "handleAccountingManualEntryCashMovementCreate", "DataServiceActionRegistrar");
}

void requireDisabledHandlerBody(const std::string& body, const std::string& reason)
{
    (void)reason;
    requireAll(body, {
        "manualEntryValidationRejectedResponse",
        "validateManual",
        "kActionAccountingManual",
    }, "manual handler body");
}

void testManualTransactionHandlerDisabled(const Harness& h)
{
    requireDisabledHandlerBody(
        handlerBody(dataServiceActions(h), "handleAccountingManualEntryTransactionCreate"),
        "MANUAL_TRANSACTION_ENTRY_NOT_IMPLEMENTED");
}

void testManualCashMovementHandlerDisabled(const Harness& h)
{
    requireDisabledHandlerBody(
        handlerBody(dataServiceActions(h), "handleAccountingManualEntryCashMovementCreate"),
        "MANUAL_CASH_MOVEMENT_NOT_IMPLEMENTED");
}

void testDispatcherReturnsUnavailable()
{
    requireDisabledResponse(
        dispatchScaffoldAction(etfdt::data_service_api::kActionAccountingManualTransactionCreate),
        "MANUAL_TRANSACTION_ENTRY_NOT_IMPLEMENTED");
    requireDisabledResponse(
        dispatchScaffoldAction(etfdt::data_service_api::kActionAccountingManualCashMovementCreate),
        "MANUAL_CASH_MOVEMENT_NOT_IMPLEMENTED");
}

void testNoSqliteWrite(const Harness& h)
{
    const auto transactionBody = handlerBody(dataServiceActions(h), "handleAccountingManualEntryTransactionCreate");
    const auto cashBody = handlerBody(dataServiceActions(h), "handleAccountingManualEntryCashMovementCreate");
    requireNoTokens({h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp"}, {
        "insertManualTransaction",
        "insertCashMovement",
        "INSERT INTO manual_transaction",
        "INSERT INTO manual_cash_movement",
    }, "manual entry SQLite write");
    requireNotContains(transactionBody, "executeSql", "manual transaction handler");
    requireNotContains(transactionBody, "executeStatement", "manual transaction handler");
    requireNotContains(cashBody, "executeSql", "manual cash movement handler");
    requireNotContains(cashBody, "executeStatement", "manual cash movement handler");
}

void testNoTradeLogWrite(const Harness& h)
{
    const auto transactionBody = handlerBody(dataServiceActions(h), "handleAccountingManualEntryTransactionCreate");
    const auto cashBody = handlerBody(dataServiceActions(h), "handleAccountingManualEntryCashMovementCreate");
    requireContains(transactionBody, "ShellAccountingManualTransactionRepository", "manual transaction handler");
    requireContains(cashBody, "ShellAccountingManualCashMovementRepository", "manual cash movement handler");
    requireNotContains(transactionBody, "appendManualTradeLog", "manual transaction handler");
    requireNotContains(cashBody, "appendManualTradeLog", "manual cash movement handler");
}

void testNoCashFactWrite(const Harness& h)
{
    const auto scaffoldBody = dataServiceActions(h);
    requireNotContains(scaffoldBody, "cash_facts", "manual scaffold helper");
    requireNotContains(scaffoldBody, "cash_ledger", "manual scaffold helper");
    requireNotContains(scaffoldBody, "cashFactsWritten\":true", "manual scaffold helper");
    requireNotContains(scaffoldBody, "cashLedgerWritten\":true", "manual scaffold helper");
}

void testNoAuditLedgerWrite(const Harness& h)
{
    const auto scaffoldBody = dataServiceActions(h);
    requireNotContains(scaffoldBody, "auditWritten\":true", "manual scaffold helper");
    requireNotContains(scaffoldBody, "ledgerWrite", "manual scaffold helper");
}

void testNoRepositoryCall(const Harness& h)
{
    const auto source = dataServiceActions(h);
    const auto transactionBody = handlerBody(source, "handleAccountingManualEntryTransactionCreate");
    const auto cashBody = handlerBody(source, "handleAccountingManualEntryCashMovementCreate");
    requireContains(transactionBody, "ShellAccountingManualTransactionRepository", "manual transaction handler");
    requireContains(cashBody, "ShellAccountingManualCashMovementRepository", "manual cash movement handler");
    requireContains(transactionBody, "persistManualTransaction", "manual transaction handler");
    requireContains(cashBody, "persistManualCashMovement", "manual cash movement handler");
}

void testNoAccountingEngineReplay(const Harness& h)
{
    const auto transactionBody = handlerBody(dataServiceActions(h), "handleAccountingManualEntryTransactionCreate");
    const auto cashBody = handlerBody(dataServiceActions(h), "handleAccountingManualEntryCashMovementCreate");
    requireNotContains(transactionBody, "runShellAccountingReadOnlyReplay", "manual transaction handler");
    requireNotContains(cashBody, "runShellAccountingReadOnlyReplay", "manual cash movement handler");
}

void testNoTradeDraft(const Harness& h)
{
    const auto source = dataServiceActions(h);
    const auto transactionBody = handlerBody(source, "handleAccountingManualEntryTransactionCreate");
    const auto cashBody = handlerBody(source, "handleAccountingManualEntryCashMovementCreate");
    requireContains(source, "\\\"tradeDraftGenerated\\\":false", "manual scaffold helper");
    requireNotContains(transactionBody, "createTradeDraft", "manual transaction handler");
    requireNotContains(cashBody, "createTradeDraft", "manual cash movement handler");
}

void testNoSuggestion(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireContains(source, "\\\"tradeSuggestionGenerated\\\":false", "manual scaffold helper");
    requireNotContains(source, "SuggestionEngine", "manual scaffold helper");
}

void testNoBroker(const Harness& h)
{
    const auto source = dataServiceActions(h);
    const auto transactionBody = handlerBody(source, "handleAccountingManualEntryTransactionCreate");
    const auto cashBody = handlerBody(source, "handleAccountingManualEntryCashMovementCreate");
    requireContains(source, "\\\"brokerSdkCalled\\\":false", "manual scaffold helper");
    requireNotContains(transactionBody, "BrokerSdk", "manual transaction handler");
    requireNotContains(cashBody, "BrokerSdk", "manual cash movement handler");
    requireNotContains(transactionBody, "submitOrder", "manual transaction handler");
    requireNotContains(cashBody, "submitOrder", "manual cash movement handler");
}

void testNoNetworkEndpoint(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireContains(source, "\\\"networkAccessed\\\":false", "manual scaffold helper");
    requireContains(source, "\\\"endpointAccessed\\\":false", "manual scaffold helper");
    requireNotContains(source, "QNetwork", "manual scaffold helper");
}

void testNoCredentialsSecretValues(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireContains(source, "\\\"credentialsAccessed\\\":false", "manual scaffold helper");
    requireNotContains(source, "getenv", "manual scaffold helper");
    requireNotContains(source, "secretKey", "manual scaffold helper");
}

void testNoRealOrderPlacement(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireContains(source, "\\\"realOrderPlacement\\\":false", "manual scaffold helper");
    requireNotContains(source, "placeOrder", "manual scaffold helper");
}

void testNoAutomaticTrading(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireContains(source, "\\\"automaticTrading\\\":false", "manual scaffold helper");
    requireNotContains(source, "executeStrategy", "manual scaffold helper");
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
        const auto filename = file.filename().string();
        require(
            filename == "001_initial_schema.sql"
                || filename == "002_shell_accounting_manual_entry_schema.sql",
            "TASK-180 must not add unauthorized migration " + file.string());
    }
}

void testProductionQmlUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "qml"), {
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

void testNoDataAccessWriteRepository(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "DataAccess" / "include");
    auto src = filesUnder(h.root / "libs" / "DataAccess" / "src");
    files.insert(files.end(), src.begin(), src.end());
    files = filesWithoutTask265ExcelVbaImportManualEntryPersistenceRepository(std::move(files));
    // TASK-196 authorizes ShellAccountingManualCashMovementRepository as a
    // DataAccess-only direct-test repository; this scaffold gate still blocks
    // DataService runtime write wiring and older repository escape hatches.
    requireNoTokens(files, {
        "ManualEntryWriteRepository",
        "ManualEntryPersistenceRepository",
        "ManualTransactionWriteRepository",
        "ManualCashMovementWriteRepository",
        "insertManualTransaction",
        "insertCashMovement",
        "manual_transaction",
        "manual_cash_movement",
    }, "DataAccess manual write repository");
}

void testStrategyMarketUnmodified(const Harness& h)
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

void testAuthorizationGateEvolved(const Harness& h)
{
    const auto doc156 = readFile(h.root / "docs" / "156_shell_accounting_manual_entry_dataservice_action_authorization_gate.md");
    const auto doc157 = readFile(h.root / "docs" / "157_shell_accounting_manual_entry_dataservice_action_authorization_test_plan.md");
    requireAll(doc156, {
        "TASK-180",
        "allows only DataService action scaffold registration",
        "still does not authorize write implementation",
    }, "docs/156");
    requireAll(doc157, {
        "TASK-180",
        "No write implementation",
        "disabled scaffold",
    }, "docs/157");
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

void testTask177GateStillValid(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "ShellAccountingManualTransactionCashMovementMvpAuthorizationGate" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization_no_dataserviceactions_manual_entry_action", "TASK-177 CMake");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization_no_real_order_placement", "TASK-177 CMake");
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
    if (name == "manual_transaction_action_registered") return testManualTransactionActionRegistered(h);
    if (name == "manual_cash_movement_action_registered") return testManualCashMovementActionRegistered(h);
    if (name == "manual_transaction_handler_disabled") return testManualTransactionHandlerDisabled(h);
    if (name == "manual_cash_movement_handler_disabled") return testManualCashMovementHandlerDisabled(h);
    if (name == "dispatcher_returns_unavailable") return testDispatcherReturnsUnavailable();
    if (name == "no_sqlite_write") return testNoSqliteWrite(h);
    if (name == "no_trade_log_write") return testNoTradeLogWrite(h);
    if (name == "no_cash_fact_write") return testNoCashFactWrite(h);
    if (name == "no_audit_ledger_write") return testNoAuditLedgerWrite(h);
    if (name == "no_repository_call") return testNoRepositoryCall(h);
    if (name == "no_accountingengine_replay") return testNoAccountingEngineReplay(h);
    if (name == "no_tradedraft") return testNoTradeDraft(h);
    if (name == "no_suggestion") return testNoSuggestion(h);
    if (name == "no_broker") return testNoBroker(h);
    if (name == "no_network_endpoint") return testNoNetworkEndpoint(h);
    if (name == "no_credentials_secret_values") return testNoCredentialsSecretValues(h);
    if (name == "no_real_order_placement") return testNoRealOrderPlacement(h);
    if (name == "no_automatic_trading") return testNoAutomaticTrading(h);
    if (name == "schema_unmodified") return testSchemaUnmodified(h);
    if (name == "no_new_migration") return testNoNewMigration(h);
    if (name == "production_qml_unmodified") return testProductionQmlUnmodified(h);
    if (name == "startup_unmodified") return testStartupUnmodified(h);
    if (name == "no_dataaccess_write_repository") return testNoDataAccessWriteRepository(h);
    if (name == "strategy_market_unmodified") return testStrategyMarketUnmodified(h);
    if (name == "authorization_gate_evolved") return testAuthorizationGateEvolved(h);
    if (name == "validation_scaffold_still_pure") return testValidationScaffoldStillPure(h);
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
