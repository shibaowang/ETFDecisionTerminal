#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"

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

std::string dataServiceActionsText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string selectorSourceText(const std::filesystem::path& root)
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
    return readTextFile(root / "docs" / "128_shell_accounting_broker_runtime_mode_source_authorization_gate.md") +
           "\n" +
           readTextFile(root / "docs" / "129_shell_accounting_broker_runtime_mode_source_authorization_test_plan.md");
}

std::vector<std::string> modeSourceTokens()
{
    return {"brokerMode", "brokerPortMode", "sandboxMode", "runtimeMode", "runtimeBrokerMode",
            "sandboxRuntimeMode"};
}

std::vector<std::string> externalSourceTokens()
{
    return {"getenv(", "qgetenv(", "QSettings", "commandLine", "readModeFile", "loadModeFile",
            "modeConfig", "modeSetting", "secretManager", "credentialStore", "databaseBrokerMode"};
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
            "submitOrderHttp", "sendBrokerRequest", "brokerSocket"};
}

std::vector<std::string> credentialOrEndpointTokens()
{
    return {"readCredential", "loadCredential", "brokerCredential", "brokerToken", "apiSecret",
            "secretKey", "privateKey", "certificatePath", "endpointUrl", "orderApiPath"};
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
    const auto actions = dataServiceActionsText(root);
    const auto selector = selectorSourceText(root);
    const auto shell = shellPathText(root);
    const auto docs = docsText(root);

    if (caseName == "docs") {
        return expect(std::filesystem::exists(
                          root / "docs" / "128_shell_accounting_broker_runtime_mode_source_authorization_gate.md"),
                      "docs/128 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "129_shell_accounting_broker_runtime_mode_source_authorization_test_plan.md"),
                      "docs/129 exists") &&
               containsAllTokens(docs, {"TASK-165", "runtime mode source authorization gate",
                                        "mode source not implemented", "sandbox runtime remains disabled",
                                        "CI no-network", "credentials isolation"});
    }

    if (caseName == "docs_index") {
        const auto readme = readTextFile(root / "README.md");
        const auto docsIndex = readTextFile(root / "docs" / "README.md");
        const auto prompt = readTextFile(root / "docs" / "12_codex_prompt_template.md");
        return containsAllTokens(readme, {"docs/128_shell_accounting_broker_runtime_mode_source_authorization_gate.md",
                                          "docs/129_shell_accounting_broker_runtime_mode_source_authorization_test_plan.md"}) &&
               containsAllTokens(docsIndex, {"128_shell_accounting_broker_runtime_mode_source_authorization_gate.md",
                                             "129_shell_accounting_broker_runtime_mode_source_authorization_test_plan.md"}) &&
               containsAllTokens(prompt, {"TASK-165", "docs/128", "docs/129"});
    }

    if (caseName == "dataserviceactions_default_mode_only") {
        return containsAllTokens(actions, {"ShellAccountingBrokerRuntimeModeSource.h",
                                          "defaultShellAccountingBrokerRuntimeModeSource()",
                                          ".brokerOrderPortMode()",
                                          "shellAccountingBrokerOrderPortForMode(brokerPortMode)"}) &&
               containsNoTokens(actions, {"shellAccountingBrokerOrderPortForMode(\"sandbox\")",
                                          "shellAccountingBrokerOrderPortForMode(\"paper\")",
                                          "shellAccountingBrokerOrderPortForMode(\"real\")"});
    }

    if (caseName == "no_payload_mode_source") {
        return containsNoTokens(actions, {"extractJsonStringField(context.request.payloadJson, \"brokerMode\"",
                                         "extractJsonStringField(context.request.payloadJson, \"brokerPortMode\"",
                                         "extractJsonStringField(context.request.payloadJson, \"sandboxMode\"",
                                         "extractJsonStringField(context.request.payloadJson, \"runtimeMode\""});
    }

    if (caseName == "no_qml_presenter_mode_source") {
        return containsNoTokens(shell, {"ShellAccountingBrokerOrderPortModeSelector",
                                       "shellAccountingBrokerOrderPortForMode",
                                       "defaultShellAccountingBrokerOrderPortMode", "brokerRuntimeMode",
                                       "sandboxRuntimeMode"});
    }

    if (caseName == "no_config_env_cli_file_db_secret_source") {
        return containsNoTokens(actions, externalSourceTokens());
    }

    if (caseName == "sandbox_runtime_not_enabled") {
        return containsNoTokens(actions, {"shellAccountingBrokerOrderPortForMode(\"sandbox\")",
                                         "brokerPortMode = \"sandbox\"", "sandboxRuntimeMode"}) &&
               containsAllTokens(docs, {"sandbox runtime remains disabled", "sandbox runtime still requires a separate task"});
    }

    if (caseName == "default_provider_disabled") {
        using namespace etfdt::dataservice;
        ShellAccountingBrokerOrderRequest request;
        auto response =
            shellAccountingBrokerOrderPortForMode(defaultShellAccountingBrokerOrderPortMode()).submitOrderEnvelope(request);
        return expect(defaultShellAccountingBrokerOrderPortMode() == ShellAccountingBrokerOrderPortMode::Disabled,
                      "default mode remains disabled") &&
               expect(response.disabled, "selected default provider is disabled") &&
               expect(!response.success, "selected default provider does not succeed") &&
               expect(response.brokerMode == "disabled", "selected default provider mode is disabled");
    }

    if (caseName == "no_broker_sdk") {
        return containsNoTokens(selector + "\n" + actions, brokerSdkTokens());
    }

    if (caseName == "no_network") {
        return containsNoTokens(selector + "\n" + actions, networkTokens());
    }

    if (caseName == "no_credentials_or_endpoint") {
        return containsNoTokens(selector + "\n" + actions, credentialOrEndpointTokens());
    }

    if (caseName == "no_real_broker_order_id") {
        return containsNoTokens(selector + "\n" + actions, realOrderIdTokens());
    }

    if (caseName == "no_order_placement") {
        return containsNoTokens(selector + "\n" + actions, orderPlacementTokens());
    }

    if (caseName == "no_database_audit_ledger_write") {
        return containsNoTokens(selector, writeTokens()) &&
               containsAllTokens(docs, {"No broker", "database write", "audit write", "ledger write"});
    }

    if (caseName == "no_reconciliation_cancellation_correction") {
        return containsNoTokens(selector + "\n" + actions, reconciliationTokens());
    }

    if (caseName == "no_strategy_or_auto_trading") {
        return containsNoTokens(selector + "\n" + actions, strategyTokens());
    }

    if (caseName == "task160_to_164_baseline") {
        const auto rootTests = readTextFile(root / "tests" / "CMakeLists.txt");
        return containsAllTokens(rootTests, {"ShellAccountingBrokerSandboxAdapterAuthorizationGate",
                                            "ShellAccountingBrokerSandboxAdapterScaffold",
                                            "ShellAccountingBrokerSandboxModeSelector",
                                            "ShellAccountingBrokerSandboxRuntimeSelectorAuthorizationGate",
                                            "ShellAccountingBrokerSandboxRuntimeSelectorDisabledWiring"}) &&
               containsAllTokens(docs, {"TASK-160 through TASK-164", "disabled-default selector wiring"});
    }

    if (caseName == "docs_tests_policy_keywords_not_production") {
        return containsAllTokens(docs, {"payload", "environment", "secret-store", "broker SDK",
                                       "order placement"}) &&
               containsNoTokens(actions, {"extractJsonStringField(context.request.payloadJson, \"brokerMode\"",
                                         "runtimeBrokerMode", "sandboxRuntimeMode"}) &&
               containsNoTokens(shell, modeSourceTokens());
    }

    if (caseName == "rollback_policy") {
        return containsAllTokens(docs, {"Rollback", "TASK-159 disabled/null provider",
                                       "fall back to disabled/null provider semantics"});
    }

    if (caseName == "future_authorization_required") {
        return containsAllTokens(docs, {"separately authorized", "Future runtime mode source",
                                       "Sandbox runtime", "separate authorization"});
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

