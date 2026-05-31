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
                        "134_shell_accounting_broker_sandbox_runtime_mode_source_scaffold.md") +
           "\n" +
           readTextFile(root / "docs" /
                        "135_shell_accounting_broker_sandbox_runtime_mode_source_scaffold_test_plan.md");
}

std::string productionBrokerSourceText(const std::filesystem::path& root)
{
    return runtimeHeaderText(root) + "\n" + runtimeSourceText(root) + "\n" + dataServiceActionsText(root) +
           "\n" + selectorText(root);
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

bool runtimeDisabledProviderResponse(std::string* mode, bool* disabled, bool* dryRunOnly)
{
    etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
    const auto runtimeMode = etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                                 .brokerOrderPortMode();
    auto response = etfdt::dataservice::shellAccountingBrokerOrderPortForMode(runtimeMode)
                        .submitOrderEnvelope(request);
    if (mode != nullptr) {
        *mode = response.brokerMode;
    }
    if (disabled != nullptr) {
        *disabled = response.disabled;
    }
    if (dryRunOnly != nullptr) {
        *dryRunOnly = response.dryRunOnly;
    }
    return !response.success && response.errorCode == "BROKER_ORDER_DISABLED";
}

bool runCase(const std::filesystem::path& root, const std::string& caseName)
{
    const auto header = runtimeHeaderText(root);
    const auto source = runtimeSourceText(root);
    const auto actions = dataServiceActionsText(root);
    const auto selector = selectorText(root);
    const auto productionBroker = productionBrokerSourceText(root);
    const auto shell = shellPathText(root);
    const auto docs = docsText(root);

    if (caseName == "docs") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "134_shell_accounting_broker_sandbox_runtime_mode_source_scaffold.md"),
                      "docs/134 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "135_shell_accounting_broker_sandbox_runtime_mode_source_scaffold_test_plan.md"),
                      "docs/135 exists") &&
               containsAllTokens(docs, {"TASK-168", "scaffold", "not runtime enablement",
                                        "direct test", "default runtime source remains disabled-only"});
    }

    if (caseName == "docs_index") {
        const auto readme = readTextFile(root / "README.md");
        const auto docsIndex = readTextFile(root / "docs" / "README.md");
        const auto prompt = readTextFile(root / "docs" / "12_codex_prompt_template.md");
        return containsAllTokens(readme, {"docs/134_shell_accounting_broker_sandbox_runtime_mode_source_scaffold.md",
                                          "docs/135_shell_accounting_broker_sandbox_runtime_mode_source_scaffold_test_plan.md"}) &&
               containsAllTokens(docsIndex, {"134_shell_accounting_broker_sandbox_runtime_mode_source_scaffold.md",
                                             "135_shell_accounting_broker_sandbox_runtime_mode_source_scaffold_test_plan.md"}) &&
               containsAllTokens(prompt, {"TASK-168", "docs/134", "docs/135"});
    }

    if (caseName == "header_declares_scaffold") {
        return containsAllTokens(header, {"class SandboxShellAccountingBrokerRuntimeModeSourceScaffold",
                                         "public ShellAccountingBrokerRuntimeModeSource",
                                         "brokerOrderPortMode() const noexcept override"});
    }

    if (caseName == "source_defines_scaffold") {
        return containsAllTokens(source, {"SandboxShellAccountingBrokerRuntimeModeSourceScaffold::brokerOrderPortMode",
                                         "ShellAccountingBrokerOrderPortMode::Sandbox"}) &&
               containsAllTokens(source, {"DisabledShellAccountingBrokerRuntimeModeSource",
                                         "ShellAccountingBrokerOrderPortMode::Disabled"});
    }

    if (caseName == "direct_instantiation_returns_sandbox") {
        etfdt::dataservice::SandboxShellAccountingBrokerRuntimeModeSourceScaffold sourceScaffold;
        return expect(sourceScaffold.brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Sandbox,
                      "direct sandbox source scaffold returns sandbox mode");
    }

    if (caseName == "default_source_still_disabled") {
        return expect(etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                          .brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Disabled,
                      "default runtime mode source remains disabled");
    }

    if (caseName == "dataserviceactions_not_using_sandbox_source") {
        return containsAllTokens(actions, {"defaultShellAccountingBrokerRuntimeModeSource()",
                                          "shellAccountingBrokerOrderPortForMode(brokerPortMode)"}) &&
               containsNoTokens(actions, {"SandboxShellAccountingBrokerRuntimeModeSourceScaffold",
                                         "ShellAccountingBrokerOrderPortMode::Sandbox",
                                         "shellAccountingBrokerOrderPortForMode(\"sandbox\")"});
    }

    if (caseName == "runtime_dry_run_disabled_provider") {
        return expect(runtimeDisabledProviderResponse(nullptr, nullptr, nullptr),
                      "runtime still uses disabled provider response");
    }

    if (caseName == "runtime_broker_port_mode_disabled") {
        std::string mode;
        return runtimeDisabledProviderResponse(&mode, nullptr, nullptr) &&
               expect(mode == "disabled", "runtime brokerPortMode remains disabled");
    }

    if (caseName == "runtime_broker_port_disabled_true") {
        bool disabled = false;
        return runtimeDisabledProviderResponse(nullptr, &disabled, nullptr) &&
               expect(disabled, "runtime brokerPortDisabled remains true");
    }

    if (caseName == "runtime_broker_port_dry_run_only_true") {
        bool dryRunOnly = false;
        return runtimeDisabledProviderResponse(nullptr, nullptr, &dryRunOnly) &&
               expect(dryRunOnly, "runtime brokerPortDryRunOnly remains true");
    }

    if (caseName == "direct_sandbox_source_selector_scaffold_unavailable") {
        etfdt::dataservice::SandboxShellAccountingBrokerRuntimeModeSourceScaffold sourceScaffold;
        etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
        auto response = etfdt::dataservice::shellAccountingBrokerOrderPortForMode(
                            sourceScaffold.brokerOrderPortMode())
                            .submitOrderEnvelope(request);
        return expect(!response.success, "sandbox source direct selector response does not succeed") &&
               expect(response.disabled, "sandbox scaffold response disabled") &&
               expect(response.dryRunOnly, "sandbox scaffold response dry-run only") &&
               expect(response.brokerMode == "sandbox", "sandbox scaffold broker mode") &&
               expect(response.errorCode == "BROKER_SANDBOX_NOT_CONFIGURED",
                      "sandbox scaffold unavailable code");
    }

    if (caseName == "no_payload_source") {
        return containsNoTokens(actions + "\n" + header + "\n" + source, payloadModeTokens());
    }

    if (caseName == "no_qml_startup_presenter_controller_source") {
        return containsNoTokens(shell, {"SandboxShellAccountingBrokerRuntimeModeSourceScaffold",
                                       "ShellAccountingBrokerRuntimeModeSource",
                                       "defaultShellAccountingBrokerRuntimeModeSource",
                                       "brokerRuntimeMode", "sandboxRuntimeMode"});
    }

    if (caseName == "no_config_env_cli_file_db_secret_source") {
        return containsNoTokens(actions + "\n" + header + "\n" + source, externalModeTokens());
    }

    if (caseName == "no_credentials_endpoint_account_order_data") {
        return containsNoTokens(header + "\n" + source, forbiddenSensitiveSourceTokens());
    }

    if (caseName == "paper_real_sources_not_implemented") {
        return containsNoTokens(header + "\n" + source,
                                {"PaperShellAccountingBrokerRuntimeModeSource",
                                 "RealShellAccountingBrokerRuntimeModeSource",
                                 "ShellAccountingBrokerOrderPortMode::Unsupported"}) &&
               expect(etfdt::dataservice::shellAccountingBrokerOrderPortModeFromString("paper") ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Unsupported,
                      "paper mode remains unsupported") &&
               expect(etfdt::dataservice::shellAccountingBrokerOrderPortModeFromString("real") ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Unsupported,
                      "real mode remains unsupported");
    }

    if (caseName == "no_broker_sdk") {
        return containsNoTokens(productionBroker, brokerSdkTokens());
    }

    if (caseName == "no_network_endpoint") {
        return containsNoTokens(productionBroker, networkTokens());
    }

    if (caseName == "no_credentials_secret_values") {
        return containsNoTokens(productionBroker, credentialValueTokens()) &&
               containsAllTokens(docs, {"no-credentials", "no real credential values"});
    }

    if (caseName == "no_real_order_id") {
        return containsNoTokens(productionBroker, realOrderIdTokens());
    }

    if (caseName == "no_order_placement") {
        return containsNoTokens(productionBroker, orderPlacementTokens());
    }

    if (caseName == "no_db_audit_ledger_write") {
        return containsNoTokens(header + "\n" + source + "\n" + selector, writeTokens()) &&
               containsAllTokens(docs, {"No database, audit, or ledger write"});
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

    if (caseName == "task167_gate_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs132 = readTextFile(root / "docs" /
                                          "132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md");
        const auto docs133 = readTextFile(root / "docs" /
                                          "133_shell_accounting_broker_sandbox_runtime_mode_source_authorization_test_plan.md");
        return containsAllTokens(cmake, {"ShellAccountingBrokerSandboxRuntimeModeSourceAuthorizationGate"}) &&
               containsAllTokens(docs132 + "\n" + docs133,
                                 {"TASK-168", "sandbox runtime mode source scaffold",
                                  "sandbox runtime remains disabled"});
    }

    if (caseName == "task166_disabled_scaffold_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs130 = readTextFile(root / "docs" /
                                          "130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md");
        const auto docs131 = readTextFile(root / "docs" /
                                          "131_shell_accounting_broker_runtime_mode_source_disabled_scaffold_test_plan.md");
        return containsAllTokens(cmake, {"ShellAccountingBrokerRuntimeModeSourceDisabledScaffold"}) &&
               containsAllTokens(docs130 + "\n" + docs131,
                                 {"TASK-168", "default runtime source remains disabled-only"});
    }

    if (caseName == "rollback_policy") {
        return containsAllTokens(docs, {"Rollback", "disabled-only", "TASK-167",
                                       "sandbox runtime remains disabled"});
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
