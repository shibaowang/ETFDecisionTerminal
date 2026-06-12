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

bool exists(const Harness& harness, const std::string& relativePath)
{
    return fs::exists(harness.root / relativePath);
}

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

bool contains(const std::string& haystack, const std::string& needle)
{
    return haystack.find(needle) != std::string::npos;
}

std::vector<fs::path> filesUnder(const fs::path& root)
{
    std::vector<fs::path> files;
    if (!fs::exists(root)) {
        return files;
    }

    for (const auto& entry : fs::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        files.push_back(entry.path());
    }
    return files;
}

std::vector<fs::path> productionScanRoots(const Harness& harness)
{
    return {
        harness.root / "apps" / "ETFDecisionShell" / "qml",
        harness.root / "apps" / "ETFDecisionShell" / "src",
        harness.root / "libs" / "ShellServices",
        harness.root / "libs" / "ShellCore",
        harness.root / "libs" / "DataServiceApi" / "include",
        harness.root / "libs" / "DataServiceApi" / "src",
        harness.root / "libs" / "DataAccess" / "include",
        harness.root / "libs" / "DataAccess" / "src",
        harness.root / "libs" / "AccountingEngine" / "include",
        harness.root / "libs" / "AccountingEngine" / "src",
        harness.root / "libs" / "StrategyEngine",
        harness.root / "libs" / "MarketEngine",
    };
}

std::vector<fs::path> productionFiles(const Harness& harness)
{
    std::vector<fs::path> files;
    for (const auto& root : productionScanRoots(harness)) {
        auto rootFiles = filesUnder(root);
        files.insert(files.end(), rootFiles.begin(), rootFiles.end());
    }
    return files;
}

std::vector<fs::path> withoutEpic289LivePublicMarketProvider(std::vector<fs::path> files, const Harness& harness)
{
    files.erase(std::remove_if(files.begin(), files.end(), [&](const fs::path& file) {
                    const auto relative = file.lexically_relative(harness.root).generic_string();
                    return relative == "libs/MarketEngine/include/MarketEngine/LivePublicMarketDataProvider.h"
                        || relative == "libs/MarketEngine/src/LivePublicMarketDataProvider.cpp";
                }),
        files.end());
    return files;
}

bool fileContainsAny(const fs::path& file, const std::vector<std::string>& tokens)
{
    const auto text = readFile(file);
    return std::any_of(tokens.begin(), tokens.end(), [&](const std::string& token) {
        return contains(text, token);
    });
}

void requireNoTokensInFiles(
    const std::vector<fs::path>& files,
    const std::vector<std::string>& tokens,
    const std::string& context)
{
    for (const auto& file : files) {
        const auto text = readFile(file);
        for (const auto& token : tokens) {
            if (contains(text, token)) {
                throw std::runtime_error(context + " found token `" + token + "` in " + file.string());
            }
        }
    }
}

void requireAllTokens(const std::string& text, const std::vector<std::string>& tokens, const std::string& context)
{
    for (const auto& token : tokens) {
        require(contains(text, token), context + " missing `" + token + "`");
    }
}

std::string doc152(const Harness& harness)
{
    return readFile(harness.root / "docs" / "152_shell_accounting_manual_transaction_cash_movement_mvp_authorization_gate.md");
}

std::string doc153(const Harness& harness)
{
    return readFile(harness.root / "docs" / "153_shell_accounting_manual_transaction_cash_movement_mvp_authorization_test_plan.md");
}

void testDocs(const Harness& harness)
{
    require(exists(harness, "docs/152_shell_accounting_manual_transaction_cash_movement_mvp_authorization_gate.md"),
        "docs/152 must exist");
    require(exists(harness, "docs/153_shell_accounting_manual_transaction_cash_movement_mvp_authorization_test_plan.md"),
        "docs/153 must exist");

    const auto gate = doc152(harness);
    const auto plan = doc153(harness);

    requireAllTokens(gate, {
        "TASK-177",
        "Manual buy and sell records",
        "Deposit and withdraw records",
        "TradeLog, cash facts, and position replay stability",
        "Daily holdings, cash, and PnL page usability",
        "Index retracement golden-tower and sniper suggestions",
        "On-exchange and off-exchange buy and sell suggestions",
        "Local backup, export, verification, and package readiness",
        "Broker sandbox new capability development is paused",
        "does not implement manual transaction entry",
        "does not implement manual transaction entry",
    }, "docs/152");

    requireAllTokens(plan, {
        "TASK-177",
        "Test Matrix",
        "Required Probes",
        "No Implementation Yet",
        "No Broker / Network / Credential Expansion",
        "Existing Safety Gates",
    }, "docs/153");
}

void testDocsIndex(const Harness& harness)
{
    const auto readme = readFile(harness.root / "README.md");
    const auto docsReadme = readFile(harness.root / "docs" / "README.md");

    require(contains(readme, "docs/152_shell_accounting_manual_transaction_cash_movement_mvp_authorization_gate.md"),
        "README must link docs/152");
    require(contains(readme, "docs/153_shell_accounting_manual_transaction_cash_movement_mvp_authorization_test_plan.md"),
        "README must link docs/153");
    require(contains(docsReadme, "152_shell_accounting_manual_transaction_cash_movement_mvp_authorization_gate.md"),
        "docs/README must link docs/152");
    require(contains(docsReadme, "153_shell_accounting_manual_transaction_cash_movement_mvp_authorization_test_plan.md"),
        "docs/README must link docs/153");
}

void testPromptTemplate(const Harness& harness)
{
    const auto prompt = readFile(harness.root / "docs" / "12_codex_prompt_template.md");
    requireAllTokens(prompt, {
        "TASK-177",
        "docs/152",
        "docs/153",
        "manual buy/sell",
        "deposit/withdraw",
        "broker sandbox new capability development is paused",
        "Do not add production QML",
        "Do not add broker SDK",
    }, "docs/12");
}

void testPriorityShift(const Harness& harness)
{
    const auto text = doc152(harness);
    requireAllTokens(text, {
        "priority now switches",
        "Manual buy and sell records",
        "Deposit and withdraw records",
        "TradeLog, cash facts, and position replay stability",
        "Daily holdings, cash, and PnL page usability",
        "golden-tower",
        "sniper suggestions",
        "On-exchange and off-exchange",
        "Local backup, export, verification, and package readiness",
    }, "MVP priority shift");
}

void testBrokerPausePolicy(const Harness& harness)
{
    const auto text = doc152(harness);
    requireAllTokens(text, {
        "Broker sandbox work is paused",
        "does not delete, weaken, skip, or bypass existing broker safety gates",
        "no broker SDK",
        "no network",
        "no credentials",
        "no real order id",
        "no order placement",
    }, "broker pause policy");
}

void testMigrationNotModified(const Harness& harness)
{
    const auto migration = readFile(harness.root / "migrations" / "001_initial_schema.sql");
    require(!contains(migration, "manual_transaction"), "initial schema must not add manual_transaction");
    require(!contains(migration, "cash_movement"), "initial schema must not add cash_movement");
    require(!contains(migration, "manual_cash_movement"), "initial schema must not add manual_cash_movement");
}

void testNoNewMigration(const Harness& harness)
{
    for (const auto& file : filesUnder(harness.root / "migrations")) {
        const auto filename = file.filename().string();
        require(
            filename == "001_initial_schema.sql"
                || filename == "002_shell_accounting_manual_entry_schema.sql",
            "TASK-177 must not add unauthorized migration " + file.string());
    }
}

void testNoProductionQmlManualUi(const Harness& harness)
{
    requireNoTokensInFiles(filesUnder(harness.root / "apps" / "ETFDecisionShell" / "qml"), {
        "manualBuy",
        "manualSell",
        "depositButton",
        "withdrawButton",
        "recordDeposit",
        "recordWithdraw",
        "Manual Transaction",
        "Cash Movement",
    }, "production QML manual UI");
}

void testNoStartupRegistration(const Harness& harness)
{
    requireNoTokensInFiles(filesUnder(harness.root / "apps" / "ETFDecisionShell" / "src"), {
        "ManualTransaction",
        "ManualCashMovement",
        "registerManualTransaction",
        "registerCashMovement",
    }, "production startup manual registration");
}

void testNoDataServiceActionsManualEntryAction(const Harness& harness)
{
    const auto source = readFile(harness.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
    require(!contains(source, "accounting.manual_transaction"), "DataServiceActions must not use raw manual transaction action string");
    require(!contains(source, "accounting.cash_movement"), "DataServiceActions must not use deprecated cash movement action string");
    require(contains(source, "manualEntryValidationRejectedResponse"), "DataServiceActions keeps validation-first rejection path");
    require(contains(source, "validateManualTransactionEntry"), "DataServiceActions may contain TASK-182 manual transaction validation wiring");
    require(contains(source, "validateManualCashMovement"), "DataServiceActions may contain TASK-182 manual cash validation wiring");
    require(contains(source, "ShellAccountingManualTransactionRepository repository(connection)"),
        "DataServiceActions may contain TASK-198 manual transaction repository wiring");
    require(contains(source, "ShellAccountingManualCashMovementRepository repository(connection)"),
        "DataServiceActions may contain TASK-198 manual cash movement repository wiring");
    require(contains(source, "persistManualTransaction"), "DataServiceActions may call TASK-192 repository through TASK-198 wiring");
    require(contains(source, "persistManualCashMovement"), "DataServiceActions may call TASK-196 repository through TASK-198 wiring");
    require(!contains(source, "insertManualTransaction"), "DataServiceActions must not implement manual transaction write");
    require(!contains(source, "insertCashMovement"), "DataServiceActions must not implement manual cash write");
    require(!contains(source, "recordManualBuy"), "DataServiceActions must not expose manual buy write shortcut");
    require(!contains(source, "recordManualSell"), "DataServiceActions must not expose manual sell write shortcut");
    require(!contains(source, "recordDeposit"), "DataServiceActions must not expose deposit write shortcut");
    require(!contains(source, "recordWithdraw"), "DataServiceActions must not expose withdrawal write shortcut");
}

void testNoDataAccessManualWriteRepository(const Harness& harness)
{
    std::vector<fs::path> files;
    auto includeFiles = filesUnder(harness.root / "libs" / "DataAccess" / "include");
    auto sourceFiles = filesUnder(harness.root / "libs" / "DataAccess" / "src");
    files.insert(files.end(), includeFiles.begin(), includeFiles.end());
    files.insert(files.end(), sourceFiles.begin(), sourceFiles.end());
    // TASK-196 authorizes ShellAccountingManualCashMovementRepository as a
    // DataAccess-only direct-test repository. This legacy gate keeps blocking
    // older write escape-hatch names and runtime manual cash shortcuts.
    requireNoTokensInFiles(files, {
        "insertManualTransaction",
        "insertCashMovement",
        "manual_transaction",
        "cash_movement",
    }, "DataAccess manual write repository");
}

void testNoAccountingEngineManualReplay(const Harness& harness)
{
    std::vector<fs::path> files;
    auto includeFiles = filesUnder(harness.root / "libs" / "AccountingEngine" / "include");
    auto sourceFiles = filesUnder(harness.root / "libs" / "AccountingEngine" / "src");
    files.insert(files.end(), includeFiles.begin(), includeFiles.end());
    files.insert(files.end(), sourceFiles.begin(), sourceFiles.end());
    requireNoTokensInFiles(files, {
        "ManualTransaction",
        "ManualCashMovement",
        "replayManualTransaction",
        "cashMovementReplay",
        "manual_transaction",
        "cash_movement",
    }, "AccountingEngine manual replay");
}

void testNoStrategyMarketImpl(const Harness& harness)
{
    std::vector<fs::path> files;
    auto strategyFiles = filesUnder(harness.root / "libs" / "StrategyEngine");
    auto marketFiles = filesUnder(harness.root / "libs" / "MarketEngine");
    files.insert(files.end(), strategyFiles.begin(), strategyFiles.end());
    files.insert(files.end(), marketFiles.begin(), marketFiles.end());
    requireNoTokensInFiles(files, {
        "goldenTowerSuggestion",
        "sniperSuggestion",
        "tradeSuggestion",
    }, "StrategyEngine/MarketEngine manual MVP implementation");
}

void testNoBrokerSdk(const Harness& harness)
{
    requireNoTokensInFiles(productionFiles(harness), {
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

void testNoNetworkEndpoint(const Harness& harness)
{
    requireNoTokensInFiles(withoutEpic289LivePublicMarketProvider(productionFiles(harness), harness), {
        "brokerEndpoint",
        "brokerHost",
        "brokerUrl",
        "brokerPortUrl",
        "https://broker",
        "wss://broker",
        "QNetworkAccessManager",
    }, "broker network endpoint");
}

void testNoCredentialsSecretValues(const Harness& harness)
{
    requireNoTokensInFiles(productionFiles(harness), {
        "brokerSecret",
        "brokerToken",
        "brokerPassword",
        "secretKey",
        "accessToken",
        "apiSecret",
        "privateKeyPem",
    }, "broker credentials or secret values");
}

void testNoRealBrokerOrderId(const Harness& harness)
{
    requireNoTokensInFiles(productionFiles(harness), {
        "realBrokerOrderId",
        "externalOrderId",
        "broker_order_id_storage",
        "persistBrokerOrderId",
    }, "real broker order id");
}

void testNoRealOrderPlacement(const Harness& harness)
{
    requireNoTokensInFiles(productionFiles(harness), {
        "placeRealOrder",
        "submitRealOrder",
        "sendBrokerOrder",
        "orderPlacement",
        "placeOrder(",
        "executeOrder(",
    }, "real order placement");
}

void testNoAutomaticTrading(const Harness& harness)
{
    requireNoTokensInFiles(productionFiles(harness), {
        "automaticTrading\":true",
        "autoTrading\":true",
        "auto_trade",
        "strategyExecute(",
        "executeStrategy",
    }, "automatic trading");
}

void testNoTradeDraftOrSuggestionImpl(const Harness& harness)
{
    requireNoTokensInFiles(productionFiles(harness), {
        "manualTradeDraft",
        "createManualTradeDraft",
        "goldenTowerSuggestion",
        "sniperSuggestion",
        "offExchangeSuggestion",
        "onExchangeSuggestion",
    }, "manual TradeDraft or suggestion implementation");
}

void testExistingBrokerGatesRetained(const Harness& harness)
{
    const auto cmake = readFile(harness.root / "tests" / "CMakeLists.txt");
    requireAllTokens(cmake, {
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderAuthorizationGate",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
        "ShellAccountingBrokerSandboxRuntimeEnablementStateDisabledWiring",
    }, "tests/CMake broker gates");
    require(exists(harness, "tests/ShellAccountingBrokerSandboxRuntimeEnablementStateDisabledWiring/CMakeLists.txt"),
        "TASK-176 CMake must remain");
}

void testNoGateWeakeningOrSkip(const Harness& harness)
{
    const std::vector<fs::path> gateFiles = {
        harness.root / "tests" / "ShellAccountingBrokerAdapterDisabledWiring" / "CMakeLists.txt",
        harness.root / "tests" / "ShellAccountingBrokerOrderImplementation" / "CMakeLists.txt",
        harness.root / "tests" / "ShellAccountingRealBrokerOrderAuthorizationGate" / "CMakeLists.txt",
        harness.root / "tests" / "ShellAccountingRealBrokerOrderImplementationGate" / "CMakeLists.txt",
        harness.root / "tests" / "ShellAccountingBrokerSandboxRuntimeEnablementStateDisabledWiring" / "CMakeLists.txt",
    };

    for (const auto& file : gateFiles) {
        const auto text = readFile(file);
        require(!contains(text, "DISABLED"), "existing broker gate must not be disabled in " + file.string());
        require(!contains(text, "WILL_FAIL"), "existing broker gate must not be weakened in " + file.string());
        require(!contains(text, "SKIP_REGULAR_EXPRESSION"), "existing broker gate must not be skipped in " + file.string());
    }
}

void testPolicyKeywordsDocsTestsOnly(const Harness& harness)
{
    const auto docs = doc152(harness) + doc153(harness);
    require(contains(docs, "manual transaction"), "docs may contain manual transaction policy text");
    require(contains(docs, "broker SDK"), "docs may contain broker SDK policy text");
    requireNoTokensInFiles(productionFiles(harness), {
        "realBrokerOrderId",
    }, "policy keywords must not be production implementation");
}

void testTask176BaselineRetained(const Harness& harness)
{
    const auto doc150 = readFile(harness.root / "docs" / "150_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring.md");
    const auto doc151 = readFile(harness.root / "docs" / "151_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring_test_plan.md");
    requireAllTokens(doc150, {
        "TASK-176",
        "brokerPortMode",
        "brokerPortDisabled",
        "brokerPortDryRunOnly",
        "remains `true`",
    }, "TASK-176 docs/150 baseline");
    requireAllTokens(doc151, {
        "TASK-176",
        "`brokerPortDryRunOnly` remains `true`",
        "Broker disabled, broker order, and real broker gates remain valid",
    }, "TASK-176 docs/151 baseline");
}

void testDataServiceOnlyFutureWritePolicy(const Harness& harness)
{
    requireAllTokens(doc152(harness), {
        "Use DataService-only boundaries",
        "DataService actions must be allowlisted",
        "DataAccess repositories must be explicit write repositories",
        "Persist facts only through an explicitly authorized repository",
    }, "DataService-only future write policy");
}

void testQmlPresenterNoDirectDbPolicy(const Harness& harness)
{
    requireAllTokens(doc152(harness), {
        "Production QML must not directly access SQLite",
        "DataAccess",
        "DataServiceClient",
        "UI and Presenter code must not call replay directly",
    }, "QML/Presenter no direct DB policy");
}

void testManualTradeScopeDocumented(const Harness& harness)
{
    requireAllTokens(doc152(harness), {
        "Future manual buy/sell implementation",
        "account, portfolio, instrument, side, quantity, price, fees",
        "Reject invalid side",
        "Keep TradeLog/cash facts/position replay stable",
    }, "manual trade scope");
}

void testCashMovementScopeDocumented(const Harness& harness)
{
    requireAllTokens(doc152(harness), {
        "Future deposit/withdraw implementation",
        "currency, amount, movement type, timestamp, and reason",
        "Reject zero or negative amount",
        "Persist cash movement facts",
    }, "cash movement scope");
}

void testFutureTaskSplitDocumented(const Harness& harness)
{
    requireAllTokens(doc152(harness), {
        "each implementation must be separately authorized",
        "Future suggestion tasks",
        "Future manual record and cash movement features must be independently disableable",
        "Explicit user authorization for each implementation scope",
    }, "future task split");
}

void testRollbackPolicy(const Harness& harness)
{
    requireAllTokens(doc152(harness), {
        "Rollback / Disable Policy",
        "Disabling them must keep read-only accounting",
        "Fallbacks must not fake facts",
        "bypass DataService",
    }, "rollback policy");
}

void runCase(const Harness& harness, const std::string& caseName)
{
    if (caseName == "docs") return testDocs(harness);
    if (caseName == "docs_index") return testDocsIndex(harness);
    if (caseName == "prompt_template") return testPromptTemplate(harness);
    if (caseName == "priority_shift") return testPriorityShift(harness);
    if (caseName == "broker_pause_policy") return testBrokerPausePolicy(harness);
    if (caseName == "migration_not_modified") return testMigrationNotModified(harness);
    if (caseName == "no_new_migration") return testNoNewMigration(harness);
    if (caseName == "no_production_qml_manual_ui") return testNoProductionQmlManualUi(harness);
    if (caseName == "no_startup_registration") return testNoStartupRegistration(harness);
    if (caseName == "no_dataserviceactions_manual_entry_action") return testNoDataServiceActionsManualEntryAction(harness);
    if (caseName == "no_dataaccess_manual_write_repository") return testNoDataAccessManualWriteRepository(harness);
    if (caseName == "no_accountingengine_manual_replay") return testNoAccountingEngineManualReplay(harness);
    if (caseName == "no_strategy_market_impl") return testNoStrategyMarketImpl(harness);
    if (caseName == "no_broker_sdk") return testNoBrokerSdk(harness);
    if (caseName == "no_network_endpoint") return testNoNetworkEndpoint(harness);
    if (caseName == "no_credentials_secret_values") return testNoCredentialsSecretValues(harness);
    if (caseName == "no_real_broker_order_id") return testNoRealBrokerOrderId(harness);
    if (caseName == "no_real_order_placement") return testNoRealOrderPlacement(harness);
    if (caseName == "no_automatic_trading") return testNoAutomaticTrading(harness);
    if (caseName == "no_tradedraft_or_suggestion_impl") return testNoTradeDraftOrSuggestionImpl(harness);
    if (caseName == "existing_broker_gates_retained") return testExistingBrokerGatesRetained(harness);
    if (caseName == "no_gate_weakening_or_skip") return testNoGateWeakeningOrSkip(harness);
    if (caseName == "policy_keywords_docs_tests_only") return testPolicyKeywordsDocsTestsOnly(harness);
    if (caseName == "task176_baseline_retained") return testTask176BaselineRetained(harness);
    if (caseName == "dataservice_only_future_write_policy") return testDataServiceOnlyFutureWritePolicy(harness);
    if (caseName == "qml_presenter_no_direct_db_policy") return testQmlPresenterNoDirectDbPolicy(harness);
    if (caseName == "manual_trade_scope_documented") return testManualTradeScopeDocumented(harness);
    if (caseName == "cash_movement_scope_documented") return testCashMovementScopeDocumented(harness);
    if (caseName == "future_task_split_documented") return testFutureTaskSplitDocumented(harness);
    if (caseName == "rollback_policy") return testRollbackPolicy(harness);

    throw std::runtime_error("Unknown case: " + caseName);
}

} // namespace

int main(int argc, char** argv)
{
    try {
        Harness harness;
        std::string caseName;

        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--source-root" && i + 1 < argc) {
                harness.root = fs::path(argv[++i]);
            } else if (arg == "--case" && i + 1 < argc) {
                caseName = argv[++i];
            }
        }

        require(!harness.root.empty(), "--source-root is required");
        require(!caseName.empty(), "--case is required");

        runCase(harness, caseName);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
