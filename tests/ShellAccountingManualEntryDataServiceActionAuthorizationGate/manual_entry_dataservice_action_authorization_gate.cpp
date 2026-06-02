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

void requireAllTokens(const std::string& text, const std::vector<std::string>& tokens, const std::string& context)
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

std::string doc156(const Harness& h)
{
    return readFile(h.root / "docs" / "156_shell_accounting_manual_entry_dataservice_action_authorization_gate.md");
}

std::string doc157(const Harness& h)
{
    return readFile(h.root / "docs" / "157_shell_accounting_manual_entry_dataservice_action_authorization_test_plan.md");
}

std::vector<fs::path> shellRuntimeFiles(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "ServiceRuntime" / "src");
    auto headers = filesUnder(h.root / "libs" / "ServiceRuntime" / "include");
    auto registrar = filesUnder(h.root / "libs" / "DataServiceApi");
    files.insert(files.end(), headers.begin(), headers.end());
    files.insert(files.end(), registrar.begin(), registrar.end());
    return files;
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
        h.root / "libs" / "AccountingEngine" / "include",
        h.root / "libs" / "AccountingEngine" / "src",
        h.root / "libs" / "StrategyEngine",
        h.root / "libs" / "MarketEngine",
    };
    for (const auto& root : roots) {
        auto rootFiles = filesUnder(root);
        files.insert(files.end(), rootFiles.begin(), rootFiles.end());
    }
    return files;
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

void testDocs(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "156_shell_accounting_manual_entry_dataservice_action_authorization_gate.md"),
        "docs/156 must exist");
    require(fs::exists(h.root / "docs" / "157_shell_accounting_manual_entry_dataservice_action_authorization_test_plan.md"),
        "docs/157 must exist");
    requireAllTokens(doc156(h), {
        "TASK-179",
        "DataService action authorization gate-only",
        "TASK-180",
        "allows only DataService action scaffold registration",
        "still does not authorize write implementation",
        "does not write a database",
        "does not modify schema",
    }, "docs/156");
    requireAllTokens(doc157(h), {
        "TASK-179",
        "TASK-180",
        "Test Matrix",
        "Required Probes",
        "Scaffold-Only DataService Runtime Action",
        "No Production Write Path",
    }, "docs/157");
}

void testDocsIndex(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsReadme = readFile(h.root / "docs" / "README.md");
    requireContains(readme, "docs/156_shell_accounting_manual_entry_dataservice_action_authorization_gate.md", "README");
    requireContains(readme, "docs/157_shell_accounting_manual_entry_dataservice_action_authorization_test_plan.md", "README");
    requireContains(docsReadme, "156_shell_accounting_manual_entry_dataservice_action_authorization_gate.md", "docs/README");
    requireContains(docsReadme, "157_shell_accounting_manual_entry_dataservice_action_authorization_test_plan.md", "docs/README");
}

void testPromptTemplate(const Harness& h)
{
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    requireAllTokens(prompt, {
        "TASK-179",
        "docs/156",
        "docs/157",
        "TASK-180 evolves that rule",
        "disabled DataService action scaffold registration",
        "still does not authorize write implementation",
    }, "docs/12");
}

void testActionBoundaryDocs(const Harness& h)
{
    requireAllTokens(doc156(h), {
        "DataService is the only future write boundary",
        "Future QML / Presenter flows may only call through the controller / adapter / client boundary",
        "Future actions must validate first",
        "TASK-178 DTO / validation scaffold",
        "TASK-179 does not add these action names",
    }, "action boundary docs");
}

void testFutureTaskSplit(const Harness& h)
{
    requireAllTokens(doc156(h), {
        "DataService manual entry action scaffold",
        "DataService manual entry action implementation",
        "DataAccess manual transaction write repository",
        "DataAccess cash movement write repository",
        "AccountingEngine replay from authorized manual facts",
        "production QML / Presenter / Controller UI",
    }, "future task split");
}

void testBrokerPausePolicy(const Harness& h)
{
    requireAllTokens(doc156(h), {
        "Broker sandbox new capability development remains paused",
        "does not delete, weaken, skip, or bypass existing broker",
        "no-network",
        "no-credentials",
        "no-order-placement",
    }, "broker pause policy");
}

void testDataServiceActionsUnmodified(const Harness& h)
{
    const auto text = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
    requireContains(text, "handleAccountingManualEntryTransactionCreate", "DataServiceActions TASK-180 scaffold");
    requireContains(text, "handleAccountingManualEntryCashMovementCreate", "DataServiceActions TASK-180 scaffold");
    requireContains(text, "manualEntryValidationRejectedResponse", "DataServiceActions validation-first rejection wiring");
    requireContains(text, "validateManualTransactionEntry", "DataServiceActions TASK-182 validation wiring");
    requireContains(text, "validateManualCashMovement", "DataServiceActions TASK-182 validation wiring");
    requireContains(text, "ShellAccountingManualTransactionRepository repository(connection)", "DataServiceActions TASK-198 manual transaction wiring");
    requireContains(text, "ShellAccountingManualCashMovementRepository repository(connection)", "DataServiceActions TASK-198 manual cash movement wiring");
    requireContains(text, "persistManualTransaction", "DataServiceActions TASK-198 manual transaction write");
    requireContains(text, "persistManualCashMovement", "DataServiceActions TASK-198 manual cash movement write");
    require(!contains(text, "insertManualTransaction"), "DataServiceActions must not implement manual transaction write");
    require(!contains(text, "insertCashMovement"), "DataServiceActions must not implement manual cash write");
}

void testNoManualActionName(const Harness& h)
{
    const auto header = readFile(h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h");
    requireContains(header, "\"accounting.\"", "manual entry scaffold action prefix");
    requireContains(header, "\"manual_transaction.create\"", "manual transaction scaffold action name");
    requireContains(header, "\"manual_cash_movement.create\"", "manual cash movement scaffold action name");
}

void testNoDispatcherHandler(const Harness& h)
{
    const auto registrar = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp");
    requireContains(registrar, "kActionAccountingManualTransactionCreate", "manual transaction scaffold dispatcher handler");
    requireContains(registrar, "kActionAccountingManualCashMovementCreate", "manual cash movement scaffold dispatcher handler");
    requireContains(registrar, "handleAccountingManualEntryTransactionCreate", "manual transaction scaffold dispatcher handler");
    requireContains(registrar, "handleAccountingManualEntryCashMovementCreate", "manual cash movement scaffold dispatcher handler");
}

void testNoServiceRuntimeRoute(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "ServiceRuntime"), {
        "manual.entry.route",
        "manualTransactionRoute",
        "manualCashMovementRoute",
    }, "manual entry ServiceRuntime route");
}

void testValidationScaffoldPure(const Harness& h)
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

void testNoDataAccessWriteRepository(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "DataAccess" / "include");
    auto src = filesUnder(h.root / "libs" / "DataAccess" / "src");
    files.insert(files.end(), src.begin(), src.end());
    // TASK-196 authorizes ShellAccountingManualCashMovementRepository as a
    // DataAccess-only direct-test repository; DataService action write wiring
    // remains forbidden by this gate.
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

void testNoSqliteWrite(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "insertManualTransaction",
        "insertCashMovement",
        "INSERT INTO manual_transaction",
        "INSERT INTO manual_cash_movement",
    }, "manual entry SQLite write");
}

void testNoTradeLogWrite(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "insertManualTradeLog",
        "appendManualTradeLog",
        "manualTradeLogWrite",
    }, "manual entry trade_log write");
}

void testNoCashFactWrite(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "insertManualCashFact",
        "appendManualCashFact",
        "manualCashLedgerWrite",
        "cashFactsWritten\":true",
        "cashLedgerWritten\":true",
    }, "manual entry cash facts write");
}

void testNoAuditLedgerWrite(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "manualEntryAuditWrite",
        "manualEntryLedgerWrite",
        "appendManualEntryAudit",
    }, "manual entry audit/ledger write");
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
            "TASK-179 must not add unauthorized migration " + file.string());
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

void testAccountingEngineReplayUnmodified(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "AccountingEngine" / "include");
    auto src = filesUnder(h.root / "libs" / "AccountingEngine" / "src");
    files.insert(files.end(), src.begin(), src.end());
    requireNoTokens(files, {
        "ManualTransaction",
        "ManualCashMovement",
        "replayManualTransaction",
        "manualCashMovementReplay",
        "manual_transaction",
    }, "AccountingEngine manual replay");
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

void testNoTradeDraftSuggestion(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "manualTradeDraft",
        "createManualTradeDraft",
        "manualTradingSuggestion",
        "manualSuggestionEngine",
    }, "manual TradeDraft or suggestion");
}

void testNoBrokerSdk(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "BrokerSdk",
        "XtQuant",
        "QMT",
        "TORA",
        "CTP",
        "InteractiveBrokers",
        "IBKR",
        "FutuOpenD",
    }, "broker SDK");
}

void testNoNetworkEndpoint(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "brokerEndpoint",
        "brokerHost",
        "brokerUrl",
        "https://broker",
        "wss://broker",
        "QNetworkAccessManager",
    }, "broker network endpoint");
}

void testNoCredentialsSecretValues(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "brokerSecret",
        "brokerToken",
        "brokerPassword",
        "secretKey",
        "accessToken",
        "apiSecret",
        "privateKeyPem",
    }, "broker credentials or secret values");
}

void testNoRealBrokerOrderId(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "realBrokerOrderId",
        "externalOrderId",
        "broker_order_id_storage",
        "persistBrokerOrderId",
    }, "real broker order id");
}

void testNoRealOrderPlacement(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "placeRealOrder",
        "submitRealOrder",
        "sendBrokerOrder",
        "orderPlacement",
        "placeOrder(",
        "executeOrder(",
    }, "real order placement");
}

void testNoAutomaticTrading(const Harness& h)
{
    requireNoTokens(productionFiles(h), {
        "automaticTrading\":true",
        "autoTrading\":true",
        "auto_trade",
        "strategyExecute(",
        "executeStrategy",
    }, "automatic trading");
}

void testExistingBrokerGatesRetained(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "CMakeLists.txt");
    requireAllTokens(cmake, {
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

void testTask178ValidationStillValid(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "ShellAccountingManualTransactionCashMovementValidationScaffold" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_validation_scaffold_buy_valid", "TASK-178 CMake");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_validation_scaffold_no_sqlite_access", "TASK-178 CMake");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_validation_scaffold_dataserviceactions_unmodified", "TASK-178 CMake");
}

void testTask177GateStillValid(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "ShellAccountingManualTransactionCashMovementMvpAuthorizationGate" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization_no_dataserviceactions_manual_entry_action", "TASK-177 CMake");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization_no_real_order_placement", "TASK-177 CMake");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization_existing_broker_gates_retained", "TASK-177 CMake");
}

void runCase(const Harness& h, const std::string& name)
{
    if (name == "docs") return testDocs(h);
    if (name == "docs_index") return testDocsIndex(h);
    if (name == "prompt_template") return testPromptTemplate(h);
    if (name == "action_boundary_docs") return testActionBoundaryDocs(h);
    if (name == "future_task_split") return testFutureTaskSplit(h);
    if (name == "broker_pause_policy") return testBrokerPausePolicy(h);
    if (name == "dataserviceactions_unmodified") return testDataServiceActionsUnmodified(h);
    if (name == "no_manual_action_name") return testNoManualActionName(h);
    if (name == "no_dispatcher_handler") return testNoDispatcherHandler(h);
    if (name == "no_service_runtime_route") return testNoServiceRuntimeRoute(h);
    if (name == "validation_scaffold_pure") return testValidationScaffoldPure(h);
    if (name == "no_dataaccess_write_repository") return testNoDataAccessWriteRepository(h);
    if (name == "no_sqlite_write") return testNoSqliteWrite(h);
    if (name == "no_trade_log_write") return testNoTradeLogWrite(h);
    if (name == "no_cash_fact_write") return testNoCashFactWrite(h);
    if (name == "no_audit_ledger_write") return testNoAuditLedgerWrite(h);
    if (name == "schema_unmodified") return testSchemaUnmodified(h);
    if (name == "no_new_migration") return testNoNewMigration(h);
    if (name == "production_qml_unmodified") return testProductionQmlUnmodified(h);
    if (name == "startup_unmodified") return testStartupUnmodified(h);
    if (name == "accountingengine_replay_unmodified") return testAccountingEngineReplayUnmodified(h);
    if (name == "strategy_market_unmodified") return testStrategyMarketUnmodified(h);
    if (name == "no_tradedraft_suggestion") return testNoTradeDraftSuggestion(h);
    if (name == "no_broker_sdk") return testNoBrokerSdk(h);
    if (name == "no_network_endpoint") return testNoNetworkEndpoint(h);
    if (name == "no_credentials_secret_values") return testNoCredentialsSecretValues(h);
    if (name == "no_real_broker_order_id") return testNoRealBrokerOrderId(h);
    if (name == "no_real_order_placement") return testNoRealOrderPlacement(h);
    if (name == "no_automatic_trading") return testNoAutomaticTrading(h);
    if (name == "existing_broker_gates_retained") return testExistingBrokerGatesRetained(h);
    if (name == "no_gate_weakening_or_skip") return testNoGateWeakeningOrSkip(h);
    if (name == "task178_validation_still_valid") return testTask178ValidationStillValid(h);
    if (name == "task177_gate_still_valid") return testTask177GateStillValid(h);
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

