#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "DataServiceApi/ShellAccountingBrokerRuntimeModeSource.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::filesystem::path argPath(int argc, char** argv, const std::string& flag)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == flag) {
            return argv[i + 1];
        }
    }
    return {};
}

std::string argValue(int argc, char** argv, const std::string& flag)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == flag) {
            return argv[i + 1];
        }
    }
    return {};
}

std::string readTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

bool hasExtension(const std::filesystem::path& path, const std::vector<std::string>& extensions)
{
    const auto extension = path.extension().string();
    for (const auto& allowed : extensions) {
        if (extension == allowed) {
            return true;
        }
    }
    return false;
}

std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions)
{
    std::vector<std::filesystem::path> result;
    if (!std::filesystem::exists(root)) {
        return result;
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file() && hasExtension(entry.path(), extensions)) {
            result.push_back(entry.path());
        }
    }
    return result;
}

std::string joinFiles(const std::vector<std::filesystem::path>& files)
{
    std::string result;
    for (const auto& file : files) {
        result += "\n// FILE: " + file.generic_string() + "\n";
        result += readTextFile(file);
    }
    return result;
}

bool expect(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << message << '\n';
        return false;
    }
    return true;
}

bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) == std::string::npos) {
            std::cerr << "missing token `" << token << "`\n";
            return false;
        }
    }
    return true;
}

bool containsNoTokens(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) != std::string::npos) {
            std::cerr << "unexpected token `" << token << "`\n";
            return false;
        }
    }
    return true;
}

std::string runtimeHeaderText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
                        "ShellAccountingBrokerRuntimeModeSource.h");
}

std::string runtimeSourceText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" /
                        "ShellAccountingBrokerRuntimeModeSource.cpp");
}

std::string dataServiceActionsText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string orderPortSelectorText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
                        "ShellAccountingBrokerOrderPortModeSelector.h") +
           "\n" +
           readTextFile(root / "libs" / "DataServiceApi" / "src" /
                        "ShellAccountingBrokerOrderPortModeSelector.cpp");
}

std::string shellPathText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp", ".qml", ".js"});
    const auto shellServices = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), shellServices.begin(), shellServices.end());
    files.insert(files.end(), shellCore.begin(), shellCore.end());
    return joinFiles(files);
}

std::string docsText(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" /
                        "136_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_gate.md") +
           "\n" +
           readTextFile(root / "docs" /
                        "137_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_test_plan.md");
}

std::string productionBrokerSourceText(const std::filesystem::path& root)
{
    return runtimeHeaderText(root) + "\n" + runtimeSourceText(root) + "\n" +
           dataServiceActionsText(root) + "\n" + orderPortSelectorText(root);
}

std::vector<std::string> runtimeSourceSelectorImplementationTokens()
{
    return {"ShellAccountingBrokerRuntimeModeSourceSelector",
            "SandboxShellAccountingBrokerRuntimeModeSourceSelector",
            "defaultShellAccountingBrokerRuntimeModeSourceSelector",
            "selectShellAccountingBrokerRuntimeModeSource",
            "runtimeModeSourceSelector",
            "brokerRuntimeModeSourceSelector"};
}

std::vector<std::string> payloadModeTokens()
{
    return {"extractJsonStringField(context.request.payloadJson, \"brokerMode\"",
            "extractJsonStringField(context.request.payloadJson, \"brokerPortMode\"",
            "extractJsonStringField(context.request.payloadJson, \"sandboxMode\"",
            "extractJsonStringField(context.request.payloadJson, \"runtimeMode\""};
}

std::vector<std::string> externalModeTokens()
{
    return {"getenv(", "qgetenv(", "QSettings", "commandLine", "readModeFile",
            "loadModeFile", "modeConfig", "modeSetting", "secretManager",
            "credentialStore", "databaseBrokerMode", "runtimeModeFile"};
}

std::vector<std::string> forbiddenSensitiveSourceTokens()
{
    return {"readCredential", "loadCredential", "brokerCredential", "brokerToken", "apiSecret",
            "secretKey", "privateKey", "certificatePath", "brokerEndpoint", "brokerUrl",
            "endpointUrl", "orderApiPath", "accountData", "orderData"};
}

std::vector<std::string> brokerSdkTokens()
{
    return {"BrokerSdk", "BrokerSDK", "#include <broker", "#include \"Broker", "ctp_api",
            "xtp_api", "ibapi", "twsapi", "sandboxBrokerAdapter", "paperBrokerAdapter",
            "realBrokerAdapter"};
}

std::vector<std::string> networkTokens()
{
    return {"QNetworkAccessManager", "QTcpSocket", "QUdpSocket", "QWebSocket", "curl_easy",
            "WinHttp", "winsock", "brokerEndpoint", "brokerUrl", "orderEndpoint",
            "submitOrderHttp", "sendBrokerRequest", "brokerSocket", "endpointUrl",
            "orderApiPath"};
}

std::vector<std::string> credentialValueTokens()
{
    return {"BROKER_API_KEY=", "BROKER_SECRET=", "BROKER_PASSWORD=", "-----BEGIN PRIVATE KEY-----",
            "apiKeyValue", "secretValue", "passwordValue"};
}

std::vector<std::string> realOrderIdTokens()
{
    return {"realBrokerOrderId", "acceptedBrokerOrderId", "persistBrokerOrderId",
            "externalBrokerOrderId", "liveBrokerOrderId"};
}

std::vector<std::string> orderPlacementTokens()
{
    return {"placeRealOrder(", "submitRealOrder(", "sendRealOrder(", "placeOrder(",
            "submitOrder(", "realOrderPlacement\":true", "brokerOrderSubmitted\":true"};
}

std::vector<std::string> writeTokens()
{
    return {"INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "CREATE ", "DROP ", "ALTER ",
            "sqlite3_exec", "QSqlQuery", "audit_log", "trade_log", "trade_execution_group",
            "trade_draft"};
}

std::vector<std::string> reconciliationTokens()
{
    return {"BrokerOrderReconciliation", "reconcileBrokerOrder", "pollBrokerOrderStatus",
            "cancelBrokerOrder", "correctBrokerOrder", "BrokerOrderCancellation",
            "BrokerOrderCorrection"};
}

std::vector<std::string> strategyTokens()
{
    return {"StrategyEngine::execute", "strategyExecute(", "executeStrategy(", "autoTrading",
            "automaticTradingEnabled", "AutomaticTrading", "auto_trade",
            "backgroundBrokerSubmission"};
}

bool runCase(const std::filesystem::path& root, const std::string& caseName)
{
    const auto header = runtimeHeaderText(root);
    const auto source = runtimeSourceText(root);
    const auto actions = dataServiceActionsText(root);
    const auto shell = shellPathText(root);
    const auto productionBroker = productionBrokerSourceText(root);
    const auto docs = docsText(root);

    if (caseName == "gate") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "136_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_gate.md"),
                      "docs/136 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "137_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_test_plan.md"),
                      "docs/137 exists") &&
               containsAllTokens(docs, {"TASK-169", "authorization gate only",
                                        "does not implement that selector",
                                        "does not enable sandbox runtime"});
    }

    if (caseName == "docs_index") {
        const auto readme = readTextFile(root / "README.md");
        const auto docsIndex = readTextFile(root / "docs" / "README.md");
        const auto prompt = readTextFile(root / "docs" / "12_codex_prompt_template.md");
        return containsAllTokens(readme, {"docs/136_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_gate.md",
                                          "docs/137_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_test_plan.md"}) &&
               containsAllTokens(docsIndex, {"136_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_gate.md",
                                             "137_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_test_plan.md"}) &&
               containsAllTokens(prompt, {"TASK-169", "docs/136", "docs/137"});
    }

    if (caseName == "no_selector_implementation") {
        return containsNoTokens(productionBroker, runtimeSourceSelectorImplementationTokens());
    }

    if (caseName == "runtime_source_unchanged") {
        return containsAllTokens(header + "\n" + source,
                                 {"DisabledShellAccountingBrokerRuntimeModeSource",
                                  "SandboxShellAccountingBrokerRuntimeModeSourceScaffold",
                                  "defaultShellAccountingBrokerRuntimeModeSource"}) &&
               containsNoTokens(header + "\n" + source,
                                runtimeSourceSelectorImplementationTokens());
    }

    if (caseName == "dataserviceactions_not_wired") {
        return containsAllTokens(actions, {"defaultShellAccountingBrokerRuntimeModeSource()",
                                          "shellAccountingBrokerOrderPortForMode(brokerPortMode)"}) &&
               containsNoTokens(actions, {"SandboxShellAccountingBrokerRuntimeModeSourceScaffold",
                                         "ShellAccountingBrokerOrderPortMode::Sandbox",
                                         "RuntimeModeSourceSelector",
                                         "shellAccountingBrokerOrderPortForMode(\"sandbox\")"});
    }

    if (caseName == "default_source_disabled") {
        return expect(etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                          .brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Disabled,
                      "default runtime mode source remains disabled");
    }

    if (caseName == "sandbox_runtime_disabled") {
        return containsAllTokens(docs, {"sandbox runtime remains disabled", "default runtime source remains disabled-only"}) &&
               containsNoTokens(actions, {"brokerPortMode = \"sandbox\"", "shellAccountingBrokerOrderPortForMode(\"sandbox\")"});
    }

    if (caseName == "scaffold_direct_test_only") {
        return containsAllTokens(header, {"SandboxShellAccountingBrokerRuntimeModeSourceScaffold"}) &&
               containsNoTokens(actions + "\n" + shell, {"SandboxShellAccountingBrokerRuntimeModeSourceScaffold"});
    }

    if (caseName == "direct_scaffold_returns_sandbox") {
        etfdt::dataservice::SandboxShellAccountingBrokerRuntimeModeSourceScaffold sourceScaffold;
        return expect(sourceScaffold.brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Sandbox,
                      "direct scaffold returns sandbox mode");
    }

    if (caseName == "direct_sandbox_scaffold_unavailable") {
        etfdt::dataservice::SandboxShellAccountingBrokerRuntimeModeSourceScaffold sourceScaffold;
        etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
        auto response = etfdt::dataservice::shellAccountingBrokerOrderPortForMode(
                            sourceScaffold.brokerOrderPortMode())
                            .submitOrderEnvelope(request);
        return expect(!response.success, "sandbox direct response does not succeed") &&
               expect(response.disabled, "sandbox direct response disabled") &&
               expect(response.dryRunOnly, "sandbox direct response dry-run only") &&
               expect(response.brokerMode == "sandbox", "sandbox direct broker mode") &&
               expect(response.errorCode == "BROKER_SANDBOX_NOT_CONFIGURED",
                      "sandbox direct scaffold unavailable code");
    }

    if (caseName == "fail_closed_policy_documented") {
        return containsAllTokens(docs, {"unknown, blank, unsupported, paper, and real modes",
                                       "fail closed", "disabled or explicit unavailable/error"});
    }

    if (caseName == "no_payload_qml_config_env_cli_file_db_secret_source") {
        return containsNoTokens(actions + "\n" + header + "\n" + source, payloadModeTokens()) &&
               containsNoTokens(actions + "\n" + header + "\n" + source, externalModeTokens()) &&
               containsNoTokens(shell, {"brokerRuntimeMode", "sandboxRuntimeMode",
                                       "RuntimeModeSourceSelector"});
    }

    if (caseName == "no_credentials_endpoint_account_order_data") {
        return containsNoTokens(header + "\n" + source + "\n" + actions, forbiddenSensitiveSourceTokens());
    }

    if (caseName == "paper_real_not_implemented") {
        return expect(etfdt::dataservice::shellAccountingBrokerOrderPortModeFromString("paper") ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Unsupported,
                      "paper remains unsupported") &&
               expect(etfdt::dataservice::shellAccountingBrokerOrderPortModeFromString("real") ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Unsupported,
                      "real remains unsupported") &&
               containsNoTokens(header + "\n" + source,
                                {"PaperShellAccountingBrokerRuntimeModeSource",
                                 "RealShellAccountingBrokerRuntimeModeSource"});
    }

    if (caseName == "no_broker_sdk") {
        return containsNoTokens(productionBroker, brokerSdkTokens());
    }

    if (caseName == "no_network_endpoint") {
        return containsNoTokens(productionBroker, networkTokens());
    }

    if (caseName == "no_credentials_secret_values") {
        return containsNoTokens(productionBroker, credentialValueTokens()) &&
               containsAllTokens(docs, {"no-credentials", "Real credentials"});
    }

    if (caseName == "no_real_order_id") {
        return containsNoTokens(productionBroker, realOrderIdTokens());
    }

    if (caseName == "no_order_placement") {
        return containsNoTokens(productionBroker, orderPlacementTokens());
    }

    if (caseName == "no_db_audit_ledger_write") {
        return containsNoTokens(header + "\n" + source + "\n" + orderPortSelectorText(root), writeTokens()) &&
               containsAllTokens(docs, {"no database write", "no audit write", "no ledger write"});
    }

    if (caseName == "no_reconciliation_cancellation_correction") {
        return containsNoTokens(productionBroker, reconciliationTokens());
    }

    if (caseName == "no_strategy_auto_trading") {
        return containsNoTokens(productionBroker, strategyTokens());
    }

    if (caseName == "schema_not_modified") {
        const auto migration = readTextFile(root / "migrations" / "001_initial_schema.sql");
        return containsAllTokens(migration, {"trade_log", "trade_draft", "audit_log"});
    }

    if (caseName == "task168_scaffold_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs134 = readTextFile(root / "docs" /
                                          "134_shell_accounting_broker_sandbox_runtime_mode_source_scaffold.md");
        const auto docs135 = readTextFile(root / "docs" /
                                          "135_shell_accounting_broker_sandbox_runtime_mode_source_scaffold_test_plan.md");
        return containsAllTokens(cmake, {"ShellAccountingBrokerSandboxRuntimeModeSourceScaffold"}) &&
               containsAllTokens(docs134 + "\n" + docs135,
                                 {"TASK-169", "direct-test-only", "default runtime source remains disabled-only"});
    }

    if (caseName == "task167_gate_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs132 = readTextFile(root / "docs" /
                                          "132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md");
        return containsAllTokens(cmake, {"ShellAccountingBrokerSandboxRuntimeModeSourceAuthorizationGate"}) &&
               containsAllTokens(docs132, {"TASK-168", "sandbox runtime remains disabled"});
    }

    if (caseName == "task166_disabled_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs130 = readTextFile(root / "docs" /
                                          "130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md");
        return containsAllTokens(cmake, {"ShellAccountingBrokerRuntimeModeSourceDisabledScaffold"}) &&
               containsAllTokens(docs130, {"TASK-168", "disabled-only"});
    }

    if (caseName == "docs_tests_policy_keywords_not_production") {
        return containsAllTokens(docs, {"runtime mode source selector", "credentials", "endpoint",
                                       "broker SDK", "order placement"}) &&
               containsNoTokens(productionBroker, runtimeSourceSelectorImplementationTokens()) &&
               containsNoTokens(shell, {"runtimeModeSourceSelector", "brokerRuntimeModeSourceSelector"});
    }

    if (caseName == "rollback_policy") {
        return containsAllTokens(docs, {"Rollback", "TASK-166 disabled-only runtime source",
                                       "TASK-168 direct-test-only scaffold", "fail closed"});
    }

    std::cerr << "unknown case `" << caseName << "`\n";
    return false;
}

}  // namespace

int main(int argc, char** argv)
{
    const auto rootArg = argPath(argc, argv, "--source-root");
    const auto root = rootArg.empty() ? std::filesystem::current_path() : rootArg;
    const auto caseName = argValue(argc, argv, "--case");
    return runCase(root, caseName) ? 0 : 1;
}
