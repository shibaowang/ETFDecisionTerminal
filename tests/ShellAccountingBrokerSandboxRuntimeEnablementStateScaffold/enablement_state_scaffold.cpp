#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "DataServiceApi/ShellAccountingBrokerRuntimeModeSource.h"
#include "DataServiceApi/ShellAccountingBrokerSandboxRuntimeEnablement.h"

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

std::string enablementHeaderText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
                        "ShellAccountingBrokerSandboxRuntimeEnablement.h");
}

std::string enablementSourceText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" /
                        "ShellAccountingBrokerSandboxRuntimeEnablement.cpp");
}

std::string orderPortSelectorText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
                        "ShellAccountingBrokerOrderPortModeSelector.h") +
           "\n" +
           readTextFile(root / "libs" / "DataServiceApi" / "src" /
                        "ShellAccountingBrokerOrderPortModeSelector.cpp");
}

std::string docsText(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" /
                        "146_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold.md") +
           "\n" +
           readTextFile(root / "docs" /
                        "147_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold_test_plan.md");
}

std::vector<std::string> externalModeTokens()
{
    return {"getenv(", "qgetenv(", "QSettings", "commandLine", "readModeFile",
            "loadModeFile", "modeConfig", "modeSetting", "secretManager",
            "credentialStore", "databaseBrokerMode", "runtimeModeFile"};
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
    return {"placeRealOrder(", "submitRealOrder(", "sendRealOrder(", "realOrderPlacement\":true",
            "brokerOrderSubmitted\":true"};
}

std::vector<std::string> writeTokens()
{
    return {"INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "CREATE ", "DROP ", "ALTER ",
            "sqlite3_exec", "QSqlQuery", "trade_log", "trade_execution_group", "trade_draft",
            "broker_order_real_status"};
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
    const auto header = enablementHeaderText(root);
    const auto source = enablementSourceText(root);
    const auto scaffold = header + "\n" + source;
    const auto runtimeHeader = runtimeHeaderText(root);
    const auto runtimeSource = runtimeSourceText(root);
    const auto actions = dataServiceActionsText(root);
    const auto runtimeProduction = runtimeHeader + "\n" + runtimeSource + "\n" +
                                   orderPortSelectorText(root) + "\n" + scaffold;
    const auto docs = docsText(root);

    if (caseName == "docs") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "146_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold.md"),
                      "docs/146 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "147_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold_test_plan.md"),
                      "docs/147 exists") &&
               containsAllTokens(docs, {"TASK-174", "direct-test-only",
                                        "disabled, unavailable, and fail-closed"});
    }

    if (caseName == "docs_index") {
        const auto readme = readTextFile(root / "README.md");
        const auto docsIndex = readTextFile(root / "docs" / "README.md");
        const auto prompt = readTextFile(root / "docs" / "12_codex_prompt_template.md");
        return containsAllTokens(readme, {"docs/146_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold.md",
                                          "docs/147_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold_test_plan.md"}) &&
               containsAllTokens(docsIndex, {"146_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold.md",
                                             "147_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold_test_plan.md"}) &&
               containsAllTokens(prompt, {"TASK-174", "docs/146", "docs/147"});
    }

    if (caseName == "header_declares_state") {
        return containsAllTokens(header, {"ShellAccountingBrokerSandboxRuntimeEnablementState",
                                         "enabled", "available", "failClosed",
                                         "runtimeMode", "status", "errorCode",
                                         "defaultShellAccountingBrokerSandboxRuntimeEnablementState"});
    }

    if (caseName == "source_defines_default") {
        return containsAllTokens(source, {"defaultShellAccountingBrokerSandboxRuntimeEnablementState",
                                         "return {};"});
    }

    const auto state = etfdt::dataservice::defaultShellAccountingBrokerSandboxRuntimeEnablementState();
    if (caseName == "default_enabled_false") {
        return expect(!state.enabled, "default enablement state enabled=false");
    }
    if (caseName == "default_available_false") {
        return expect(!state.available, "default enablement state available=false");
    }
    if (caseName == "default_fail_closed_true") {
        return expect(state.failClosed, "default enablement state failClosed=true");
    }
    if (caseName == "default_runtime_mode_stable") {
        return expect(std::string(state.runtimeMode) == "sandbox_runtime_disabled",
                      "runtimeMode remains stable");
    }
    if (caseName == "default_status_stable") {
        return expect(std::string(state.status) == "disabled", "status remains stable");
    }
    if (caseName == "default_error_code_stable") {
        return expect(std::string(state.errorCode) == "BROKER_SANDBOX_RUNTIME_DISABLED",
                      "errorCode remains stable");
    }

    if (caseName == "no_runtime_injection_api") {
        return containsNoTokens(scaffold, {"setShellAccountingBrokerSandboxRuntimeEnablement",
                                          "registerShellAccountingBrokerSandboxRuntimeEnablement",
                                          "overrideShellAccountingBrokerSandboxRuntimeEnablement",
                                          "runtimeInjection", "globalOverride"});
    }

    if (caseName == "dataserviceactions_not_modified" || caseName == "dataserviceactions_not_wired") {
        return containsAllTokens(actions, {"ShellAccountingBrokerSandboxRuntimeEnablement.h",
                                          "defaultShellAccountingBrokerSandboxRuntimeEnablementState",
                                          "!sandboxEnablementState.enabled",
                                          "!sandboxEnablementState.available",
                                          "sandboxEnablementState.failClosed"}) &&
               containsNoTokens(actions, {"ShellAccountingBrokerOrderPortMode::Sandbox",
                                         "ShellAccountingBrokerOrderPortMode::Paper",
                                         "ShellAccountingBrokerOrderPortMode::Real",
                                         "sandboxRuntimeEnabled"});
    }

    if (caseName == "dataserviceactions_disabled_default_wiring") {
        return containsAllTokens(actions,
                                 {"const auto defaultBrokerPortMode =",
                                  "defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()",
                                  "shellAccountingBrokerRuntimeModeSourceForMode(",
                                  "defaultBrokerPortMode)",
                                  ".brokerOrderPortMode()"});
    }

    if (caseName == "no_dataserviceactions_sandbox_paper_real") {
        return containsNoTokens(actions, {"ShellAccountingBrokerOrderPortMode::Sandbox",
                                         "ShellAccountingBrokerOrderPortMode::Paper",
                                         "ShellAccountingBrokerOrderPortMode::Real",
                                         "shellAccountingBrokerRuntimeModeSourceForMode(\"sandbox\")"});
    }

    if (caseName == "runtime_source_production_unchanged") {
        return containsNoTokens(runtimeHeader + "\n" + runtimeSource,
                                {"ShellAccountingBrokerSandboxRuntimeEnablement",
                                 "BROKER_SANDBOX_RUNTIME_DISABLED", "TASK-174"});
    }

    if (caseName == "default_runtime_source_disabled") {
        return expect(etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                          .brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Disabled,
                      "default runtime source remains disabled");
    }

    if (caseName == "selected_runtime_source_disabled") {
        const auto defaultMode = etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                                     .brokerOrderPortMode();
        return expect(etfdt::dataservice::shellAccountingBrokerRuntimeModeSourceForMode(defaultMode)
                          .brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Disabled,
                      "selected source from default mode remains disabled");
    }

    if (caseName == "sandbox_runtime_not_enabled") {
        return containsAllTokens(docs, {"not connected to", "runtime", "sandbox runtime remains disabled"}) &&
               containsNoTokens(actions, {"ShellAccountingBrokerOrderPortMode::Sandbox",
                                         "ShellAccountingBrokerOrderPortMode::Paper",
                                         "ShellAccountingBrokerOrderPortMode::Real"});
    }

    if (caseName == "selector_direct_test_only") {
        return containsAllTokens(docs, {"TASK-170 selector scaffold remains direct-test-only"}) &&
               containsNoTokens(actions, {"ShellAccountingBrokerOrderPortMode::Sandbox"});
    }

    if (caseName == "direct_sandbox_selector_unavailable") {
        auto& sourceForMode = etfdt::dataservice::shellAccountingBrokerRuntimeModeSourceForMode(
            etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Sandbox);
        etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
        const auto response = etfdt::dataservice::shellAccountingBrokerOrderPortForMode(
                                  sourceForMode.brokerOrderPortMode())
                                  .submitOrderEnvelope(request);
        return expect(!response.success, "direct sandbox selector response does not succeed") &&
               expect(response.disabled, "direct sandbox selector response disabled") &&
               expect(response.dryRunOnly, "direct sandbox selector response dry-run only") &&
               expect(response.brokerMode == "sandbox", "direct sandbox selector broker mode") &&
               expect(response.errorCode == "BROKER_SANDBOX_NOT_CONFIGURED",
                      "direct sandbox selector unavailable code");
    }

    if (caseName == "paper_real_not_implemented") {
        return expect(etfdt::dataservice::shellAccountingBrokerOrderPortModeFromString("paper") ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Unsupported,
                      "paper remains unsupported") &&
               expect(etfdt::dataservice::shellAccountingBrokerOrderPortModeFromString("real") ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Unsupported,
                      "real remains unsupported") &&
               containsNoTokens(runtimeHeader + "\n" + runtimeSource,
                                {"PaperShellAccountingBrokerRuntimeModeSource",
                                 "RealShellAccountingBrokerRuntimeModeSource"});
    }

    if (caseName == "no_external_mode_source") {
        return containsNoTokens(scaffold, externalModeTokens());
    }

    if (caseName == "no_credentials_endpoint_account_order_data") {
        return containsNoTokens(scaffold, {"credential", "Credential", "endpoint", "Endpoint",
                                          "accountData", "orderData", "accountId", "orderId"});
    }

    if (caseName == "no_broker_sdk") {
        return containsNoTokens(runtimeProduction, brokerSdkTokens());
    }
    if (caseName == "no_network_endpoint") {
        return containsNoTokens(runtimeProduction, networkTokens());
    }
    if (caseName == "no_credentials_secret_values") {
        return containsNoTokens(runtimeProduction, credentialValueTokens()) &&
               containsAllTokens(docs, {"Real credentials", "must not enter code"});
    }
    if (caseName == "no_real_order_id") {
        return containsNoTokens(runtimeProduction, realOrderIdTokens());
    }
    if (caseName == "no_order_placement") {
        return containsNoTokens(runtimeProduction, orderPlacementTokens());
    }
    if (caseName == "no_db_audit_ledger_write") {
        return containsNoTokens(runtimeProduction, writeTokens());
    }
    if (caseName == "no_reconciliation_cancellation_correction") {
        return containsNoTokens(runtimeProduction, reconciliationTokens());
    }
    if (caseName == "no_strategy_auto_trading") {
        return containsNoTokens(runtimeProduction, strategyTokens());
    }
    if (caseName == "schema_not_modified") {
        const auto migration = readTextFile(root / "migrations" / "001_initial_schema.sql");
        return containsAllTokens(migration, {"trade_log", "trade_draft", "audit_log"});
    }

    if (caseName == "task173_gate_evolved") {
        const auto docs144 = readTextFile(root / "docs" /
                                          "144_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_gate.md");
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        return containsAllTokens(docs144, {"TASK-174", "disabled, unavailable, and fail-closed"}) &&
               containsAllTokens(cmake, {"ShellAccountingBrokerSandboxRuntimeModeEnablementAuthorizationGate"});
    }

    if (caseName == "task172_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        return containsAllTokens(cmake, {"ShellAccountingBrokerSandboxRuntimeModeSourceSelectorDisabledWiring"});
    }

    if (caseName == "task166_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs130 = readTextFile(root / "docs" /
                                          "130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md");
        return containsAllTokens(cmake, {"ShellAccountingBrokerRuntimeModeSourceDisabledScaffold"}) &&
               containsAllTokens(docs130, {"disabled-only"});
    }

    if (caseName == "rollback_policy") {
        return containsAllTokens(docs, {"Rollback", "remove the enablement state scaffold",
                                       "TASK-172 disabled-default selector wiring",
                                       "TASK-166 disabled-only runtime source"});
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
