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

std::string dataServiceActionsText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string runtimeSourceText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
                        "ShellAccountingBrokerRuntimeModeSource.h") +
           "\n" +
           readTextFile(root / "libs" / "DataServiceApi" / "src" /
                        "ShellAccountingBrokerRuntimeModeSource.cpp");
}

std::string selectorText(const std::filesystem::path& root)
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
                        "132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md") +
           "\n" +
           readTextFile(root / "docs" /
                        "133_shell_accounting_broker_sandbox_runtime_mode_source_authorization_test_plan.md");
}

std::vector<std::string> sandboxRuntimeImplementationTokens()
{
    return {"SandboxRuntimeModeSource", "defaultShellAccountingBrokerSandboxRuntimeModeSource",
            "SandboxShellAccountingBrokerRuntimeModeSource", "sandboxRuntimeMode",
            "runtimeSandboxMode", "brokerPortMode = \"sandbox\""};
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
            "credentialStore", "databaseBrokerMode"};
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

std::vector<std::string> credentialTokens()
{
    return {"readCredential", "loadCredential", "brokerCredential", "brokerToken", "apiSecret",
            "secretKey", "privateKey", "certificatePath", "BROKER_API_KEY=",
            "BROKER_SECRET=", "BROKER_PASSWORD=", "-----BEGIN PRIVATE KEY-----"};
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
    const auto runtimeSource = runtimeSourceText(root);
    const auto selector = selectorText(root);
    const auto shell = shellPathText(root);
    const auto docs = docsText(root);

    if (caseName == "docs") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md"),
                      "docs/132 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "133_shell_accounting_broker_sandbox_runtime_mode_source_authorization_test_plan.md"),
                      "docs/133 exists") &&
               containsAllTokens(docs, {"TASK-167", "authorization gate only",
                                        "does not implement sandbox mode source selection",
                                        "Sandbox runtime is not enabled",
                                        "CI must remain no-network"});
    }

    if (caseName == "docs_index") {
        const auto readme = readTextFile(root / "README.md");
        const auto docsIndex = readTextFile(root / "docs" / "README.md");
        const auto prompt = readTextFile(root / "docs" / "12_codex_prompt_template.md");
        return containsAllTokens(readme, {"docs/132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md",
                                          "docs/133_shell_accounting_broker_sandbox_runtime_mode_source_authorization_test_plan.md"}) &&
               containsAllTokens(docsIndex, {"132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md",
                                             "133_shell_accounting_broker_sandbox_runtime_mode_source_authorization_test_plan.md"}) &&
               containsAllTokens(prompt, {"TASK-167", "docs/132", "docs/133"});
    }

    if (caseName == "no_sandbox_source_implementation") {
        return containsNoTokens(actions + "\n" + runtimeSource, sandboxRuntimeImplementationTokens());
    }

    if (caseName == "no_payload_mode_source") {
        return containsNoTokens(actions, payloadModeTokens());
    }

    if (caseName == "no_qml_presenter_controller_startup_source") {
        return containsNoTokens(shell, {"ShellAccountingBrokerRuntimeModeSource",
                                       "defaultShellAccountingBrokerRuntimeModeSource",
                                       "ShellAccountingBrokerOrderPortModeSelector",
                                       "shellAccountingBrokerOrderPortForMode",
                                       "brokerRuntimeMode", "sandboxRuntimeMode"});
    }

    if (caseName == "no_config_env_cli_file_db_secret_source") {
        return containsNoTokens(actions + "\n" + runtimeSource, externalModeTokens());
    }

    if (caseName == "runtime_source_disabled_only") {
        return containsAllTokens(runtimeSource, {"DisabledShellAccountingBrokerRuntimeModeSource",
                                                "ShellAccountingBrokerOrderPortMode::Disabled"}) &&
               containsNoTokens(runtimeSource, {"ShellAccountingBrokerOrderPortMode::Sandbox",
                                                "ShellAccountingBrokerOrderPortMode::Unsupported",
                                                "shellAccountingBrokerOrderPortModeFromString"});
    }

    if (caseName == "default_source_disabled") {
        return expect(etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                          .brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Disabled,
                      "default runtime mode source returns disabled");
    }

    if (caseName == "sandbox_runtime_not_enabled") {
        return containsNoTokens(actions + "\n" + runtimeSource,
                                {"shellAccountingBrokerOrderPortForMode(\"sandbox\")",
                                 "brokerPortMode = \"sandbox\"", "sandboxRuntimeMode"}) &&
               containsAllTokens(docs, {"Sandbox runtime is not enabled", "not implement sandbox runtime"});
    }

    if (caseName == "direct_sandbox_scaffold_unavailable") {
        etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
        auto response = etfdt::dataservice::shellAccountingBrokerOrderPortForMode("sandbox")
                            .submitOrderEnvelope(request);
        return expect(!response.success, "sandbox scaffold does not succeed") &&
               expect(response.disabled, "sandbox scaffold disabled") &&
               expect(response.dryRunOnly, "sandbox scaffold dry-run only") &&
               expect(response.brokerMode == "sandbox", "sandbox scaffold broker mode") &&
               expect(response.errorCode == "BROKER_SANDBOX_NOT_CONFIGURED",
                      "sandbox scaffold unavailable code");
    }

    if (caseName == "paper_real_not_implemented") {
        etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
        auto paper = etfdt::dataservice::shellAccountingBrokerOrderPortForMode("paper")
                         .submitOrderEnvelope(request);
        auto real = etfdt::dataservice::shellAccountingBrokerOrderPortForMode("real")
                        .submitOrderEnvelope(request);
        return expect(paper.disabled, "paper mode fails closed") &&
               expect(real.disabled, "real mode fails closed") &&
               expect(paper.brokerMode == "disabled", "paper returns disabled provider") &&
               expect(real.brokerMode == "disabled", "real returns disabled provider");
    }

    if (caseName == "no_broker_sdk") {
        return containsNoTokens(actions + "\n" + runtimeSource + "\n" + selector, brokerSdkTokens());
    }

    if (caseName == "no_network_or_endpoint") {
        return containsNoTokens(actions + "\n" + runtimeSource + "\n" + selector, networkTokens());
    }

    if (caseName == "no_credentials_or_secret_values") {
        return containsNoTokens(actions + "\n" + runtimeSource + "\n" + selector, credentialTokens()) &&
               containsAllTokens(docs, {"no-credentials", "no real credential values"});
    }

    if (caseName == "no_real_broker_order_id") {
        return containsNoTokens(actions + "\n" + runtimeSource + "\n" + selector, realOrderIdTokens());
    }

    if (caseName == "no_order_placement") {
        return containsNoTokens(actions + "\n" + runtimeSource + "\n" + selector, orderPlacementTokens());
    }

    if (caseName == "no_db_audit_ledger_write") {
        return containsNoTokens(runtimeSource + "\n" + selector, writeTokens()) &&
               containsAllTokens(docs, {"No DB, audit, or ledger write path"});
    }

    if (caseName == "no_reconciliation_cancellation_correction") {
        return containsNoTokens(actions + "\n" + runtimeSource + "\n" + selector, reconciliationTokens());
    }

    if (caseName == "no_strategy_or_auto_trading") {
        return containsNoTokens(actions + "\n" + runtimeSource + "\n" + selector, strategyTokens());
    }

    if (caseName == "task166_baseline") {
        const auto rootTests = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs130 = readTextFile(root / "docs" /
                                          "130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md");
        const auto docs131 = readTextFile(root / "docs" /
                                          "131_shell_accounting_broker_runtime_mode_source_disabled_scaffold_test_plan.md");
        return containsAllTokens(rootTests, {"ShellAccountingBrokerRuntimeModeSourceDisabledScaffold"}) &&
               containsAllTokens(docs130 + "\n" + docs131,
                                 {"TASK-167", "disabled-only source", "sandbox runtime remains disabled"});
    }

    if (caseName == "task165_baseline") {
        const auto rootTests = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs128 = readTextFile(root / "docs" /
                                          "128_shell_accounting_broker_runtime_mode_source_authorization_gate.md");
        return containsAllTokens(rootTests, {"ShellAccountingBrokerRuntimeModeSourceAuthorizationGate"}) &&
               containsAllTokens(docs128, {"TASK-165", "default disabled provider remains active"});
    }

    if (caseName == "docs_tests_policy_keywords_not_production") {
        return containsAllTokens(docs, {"sandbox runtime mode source", "credentials", "endpoint",
                                       "broker SDK", "order placement"}) &&
               containsNoTokens(actions + "\n" + runtimeSource, sandboxRuntimeImplementationTokens()) &&
               containsNoTokens(shell, {"brokerRuntimeMode", "sandboxRuntimeMode"});
    }

    if (caseName == "rollback_policy") {
        return containsAllTokens(docs, {"Rollback", "TASK-166 disabled-only source",
                                       "disabled/null provider"});
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
