#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"

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

std::string docs124Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" /
                        "124_shell_accounting_broker_sandbox_runtime_selector_authorization_gate.md");
}

std::string docs125Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" /
                        "125_shell_accounting_broker_sandbox_runtime_selector_authorization_test_plan.md");
}

std::string selectorSourceText(const std::filesystem::path& root)
{
    return readTextFile(
               root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
               "ShellAccountingBrokerOrderPortModeSelector.h") +
           "\n" +
           readTextFile(
               root / "libs" / "DataServiceApi" / "src" /
               "ShellAccountingBrokerOrderPortModeSelector.cpp");
}

std::string dataServiceActionsText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string productionUiAndShellText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp", ".qml", ".js"});
    const auto shellServices = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), shellServices.begin(), shellServices.end());
    files.insert(files.end(), shellCore.begin(), shellCore.end());
    return joinFiles(files);
}

std::string task160To162DocsText(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "118_shell_accounting_broker_sandbox_adapter_authorization_gate.md") +
           "\n" +
           readTextFile(root / "docs" / "119_shell_accounting_broker_sandbox_adapter_authorization_test_plan.md") +
           "\n" +
           readTextFile(root / "docs" / "120_shell_accounting_broker_sandbox_adapter_scaffold.md") +
           "\n" +
           readTextFile(root / "docs" / "121_shell_accounting_broker_sandbox_adapter_scaffold_test_plan.md") +
           "\n" +
           readTextFile(root / "docs" / "122_shell_accounting_broker_sandbox_mode_selector.md") +
           "\n" +
           readTextFile(root / "docs" / "123_shell_accounting_broker_sandbox_mode_selector_test_plan.md");
}

std::vector<std::string> selectorRuntimeWiringTokens()
{
    return {"ShellAccountingBrokerOrderPortModeSelector", "shellAccountingBrokerOrderPortForMode",
            "defaultShellAccountingBrokerOrderPortMode", "broker sandbox runtime selector",
            "brokerRuntimeMode", "sandboxRuntimeMode"};
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

std::vector<std::string> credentialReadTokens()
{
    return {"getenv(", "qgetenv(", "QSettings", "credentialStore", "secretManager",
            "readCredential", "loadCredential", "brokerCredential"};
}

std::vector<std::string> credentialValueTokens()
{
    return {"BROKER_API_KEY=", "BROKER_SECRET=", "BROKER_PASSWORD=", "brokerPassword=",
            "brokerToken=", "apiSecret=", "secretKey=", "privateKey=",
            "-----BEGIN PRIVATE KEY-----", "AKIA", "ghp_", "xoxb-", "http://", "https://"};
}

std::vector<std::string> realOrderIdTokens()
{
    return {"realBrokerOrderId", "acceptedBrokerOrderId", "persistBrokerOrderId",
            "externalBrokerOrderId", "liveBrokerOrderId"};
}

std::vector<std::string> realOrderPlacementTokens()
{
    return {"placeRealOrder(", "submitRealOrder(", "sendRealOrder(", "placeOrder(",
            "submitOrder(", "realOrderPlacement", "brokerOrderSubmitted"};
}

std::vector<std::string> sqliteWriteTokens()
{
    return {"INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "CREATE ", "DROP ", "ALTER ",
            "sqlite3_exec", "QSqlQuery", "audit_log", "trade_log", "trade_execution_group",
            "trade_draft"};
}

std::vector<std::string> reconciliationCancellationTokens()
{
    return {"BrokerOrderReconciliation", "reconcileBrokerOrder", "pollBrokerOrderStatus",
            "cancelBrokerOrder", "correctBrokerOrder", "BrokerOrderCancellation",
            "BrokerOrderCorrection"};
}

std::vector<std::string> strategyAndAutoTradingTokens()
{
    return {"StrategyEngine::execute", "strategyExecute(", "executeStrategy(", "autoTrading",
            "automaticTradingEnabled", "AutomaticTrading", "auto_trade",
            "backgroundBrokerSubmission"};
}

bool runCase(const std::filesystem::path& root, const std::string& caseName)
{
    const auto docs124 = docs124Text(root);
    const auto docs125 = docs125Text(root);
    const auto docs = docs124 + "\n" + docs125;
    const auto selectorSource = selectorSourceText(root);
    const auto dataServiceActions = dataServiceActionsText(root);
    const auto productionUiAndShell = productionUiAndShellText(root);

    if (caseName == "gate") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "124_shell_accounting_broker_sandbox_runtime_selector_authorization_gate.md"),
                      "docs/124 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "125_shell_accounting_broker_sandbox_runtime_selector_authorization_test_plan.md"),
                      "docs/125 exists") &&
               containsAllTokens(docs, {"TASK-163", "runtime selector wiring requires separate authorization",
                                        "default must remain disabled", "CI no-network"});
    }

    if (caseName == "docs_index") {
        const auto readme = readTextFile(root / "README.md");
        const auto docsIndex = readTextFile(root / "docs" / "README.md");
        const auto prompt = readTextFile(root / "docs" / "12_codex_prompt_template.md");
        return containsAllTokens(readme, {"docs/124_shell_accounting_broker_sandbox_runtime_selector_authorization_gate.md",
                                          "docs/125_shell_accounting_broker_sandbox_runtime_selector_authorization_test_plan.md"}) &&
               containsAllTokens(docsIndex, {"124_shell_accounting_broker_sandbox_runtime_selector_authorization_gate.md",
                                             "125_shell_accounting_broker_sandbox_runtime_selector_authorization_test_plan.md"}) &&
               containsAllTokens(prompt, {"TASK-163", "docs/124", "docs/125"});
    }

    if (caseName == "no_dataserviceactions_runtime_wiring") {
        return containsNoTokens(dataServiceActions, selectorRuntimeWiringTokens());
    }

    if (caseName == "default_provider_disabled") {
        using namespace etfdt::dataservice;
        ShellAccountingBrokerOrderRequest request;
        auto response = defaultShellAccountingBrokerOrderPort().submitOrderEnvelope(request);
        return expect(defaultShellAccountingBrokerOrderPortMode() == ShellAccountingBrokerOrderPortMode::Disabled,
                      "selector default mode remains disabled") &&
               expect(response.disabled, "default provider response is disabled") &&
               expect(!response.success, "default provider does not succeed") &&
               expect(response.brokerMode == "disabled", "default provider broker mode is disabled");
    }

    if (caseName == "selector_not_in_startup") {
        const auto startup = readTextFile(root / "apps" / "ETFDecisionShell" / "src" / "main.cpp");
        return containsNoTokens(startup, selectorRuntimeWiringTokens());
    }

    if (caseName == "no_qml_presenter_exposure") {
        return containsNoTokens(productionUiAndShell, selectorRuntimeWiringTokens()) &&
               containsNoTokens(productionUiAndShell, {"brokerRuntimeMode", "brokerCredential", "brokerEndpoint"});
    }

    if (caseName == "no_broker_sdk") {
        return containsNoTokens(selectorSource, brokerSdkTokens());
    }

    if (caseName == "no_network") {
        return containsNoTokens(selectorSource, networkTokens());
    }

    if (caseName == "no_credentials_read") {
        return containsNoTokens(selectorSource, credentialReadTokens());
    }

    if (caseName == "no_endpoint_or_secret_values") {
        return containsNoTokens(selectorSource + "\n" + docs, credentialValueTokens());
    }

    if (caseName == "no_real_broker_order_id") {
        return containsNoTokens(selectorSource, realOrderIdTokens());
    }

    if (caseName == "no_order_placement") {
        return containsNoTokens(selectorSource, realOrderPlacementTokens());
    }

    if (caseName == "no_database_audit_ledger_write") {
        return containsNoTokens(selectorSource, sqliteWriteTokens());
    }

    if (caseName == "no_reconciliation_cancellation_correction") {
        return containsNoTokens(selectorSource, reconciliationCancellationTokens());
    }

    if (caseName == "no_strategy_or_auto_trading") {
        return containsNoTokens(selectorSource, strategyAndAutoTradingTokens());
    }

    if (caseName == "task160_161_162_regression") {
        const auto regressionDocs = task160To162DocsText(root);
        return containsAllTokens(regressionDocs, {"TASK-160", "TASK-161", "TASK-162",
                                                 "disabled/null", "TASK-161 scaffold",
                                                 "unknown mode fail closed"});
    }

    if (caseName == "docs_tests_policy_keywords_not_production") {
        return containsAllTokens(docs, {"credentials injection", "broker SDK", "order placement"}) &&
               containsNoTokens(dataServiceActions, selectorRuntimeWiringTokens()) &&
               containsNoTokens(productionUiAndShell, selectorRuntimeWiringTokens());
    }

    if (caseName == "future_runtime_wiring_requires_new_task") {
        return containsAllTokens(docs, {"runtime selector wiring requires separate authorization",
                                        "Future runtime selector wiring must be implemented in a separate task",
                                        "User explicitly authorizes runtime selector wiring"});
    }

    if (caseName == "fail_closed_policy") {
        return containsAllTokens(docs, {"Default mode must remain disabled",
                                        "unknown mode", "paper mode", "real mode", "fail closed"});
    }

    if (caseName == "no_runtime_mode_source") {
        return containsAllTokens(docs, {"runtime mode source requires separate authorization",
                                        "Runtime mode source selection is not authorized"}) &&
               containsNoTokens(selectorSource, {"getenv(", "qgetenv(", "QSettings", "commandLineMode"});
    }

    if (caseName == "no_credentials_injection") {
        return containsAllTokens(docs, {"credentials injection requires separate authorization",
                                        "Credentials injection is not authorized"}) &&
               containsNoTokens(selectorSource, credentialReadTokens());
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
