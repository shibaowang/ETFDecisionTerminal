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

std::size_t countOccurrences(const std::string& text, const std::string& token)
{
    std::size_t count = 0;
    std::size_t pos = 0;
    while ((pos = text.find(token, pos)) != std::string::npos) {
        ++count;
        pos += token.size();
    }
    return count;
}

std::string dataServiceActionsText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string brokerDryRunActionText(const std::filesystem::path& root)
{
    const auto actions = dataServiceActionsText(root);
    const auto start = actions.find("handleAccountingBrokerOrderDryRun");
    const auto end = actions.find("handleAccountingRealBrokerOrderGate", start);
    if (start == std::string::npos) {
        return {};
    }
    return actions.substr(start, end == std::string::npos ? std::string::npos : end - start);
}

std::string enablementText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
                        "ShellAccountingBrokerSandboxRuntimeEnablement.h") +
           "\n" +
           readTextFile(root / "libs" / "DataServiceApi" / "src" /
                        "ShellAccountingBrokerSandboxRuntimeEnablement.cpp");
}

std::string runtimeSourceText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
                        "ShellAccountingBrokerRuntimeModeSource.h") +
           "\n" +
           readTextFile(root / "libs" / "DataServiceApi" / "src" /
                        "ShellAccountingBrokerRuntimeModeSource.cpp");
}

std::string selectorSourceText(const std::filesystem::path& root)
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
                        "150_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring.md") +
           "\n" +
           readTextFile(root / "docs" /
                        "151_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring_test_plan.md");
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

etfdt::dataservice::ShellAccountingBrokerOrderResponse defaultBrokerPortResponse()
{
    const auto defaultBrokerPortMode =
        etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode();
    const auto brokerPortMode =
        etfdt::dataservice::shellAccountingBrokerRuntimeModeSourceForMode(defaultBrokerPortMode)
            .brokerOrderPortMode();
    etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
    return etfdt::dataservice::shellAccountingBrokerOrderPortForMode(brokerPortMode)
        .submitOrderEnvelope(request);
}

bool runCase(const std::filesystem::path& root, const std::string& caseName)
{
    const auto actions = dataServiceActionsText(root);
    const auto dryRunAction = brokerDryRunActionText(root);
    const auto enablement = enablementText(root);
    const auto runtimeSource = runtimeSourceText(root);
    const auto selectorSource = selectorSourceText(root);
    const auto runtimeProduction = runtimeSource + "\n" + selectorSource + "\n" + enablement;
    const auto docs = docsText(root);

    if (caseName == "docs") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "150_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring.md"),
                      "docs/150 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "151_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring_test_plan.md"),
                      "docs/151 exists") &&
               containsAllTokens(docs, {"TASK-176", "disabled/fail-closed",
                                        "does not enable sandbox runtime",
                                        "brokerPortDryRunOnly"});
    }

    if (caseName == "docs_index") {
        const auto readme = readTextFile(root / "README.md");
        const auto docsIndex = readTextFile(root / "docs" / "README.md");
        const auto prompt = readTextFile(root / "docs" / "12_codex_prompt_template.md");
        return containsAllTokens(readme, {"docs/150_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring.md",
                                          "docs/151_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring_test_plan.md"}) &&
               containsAllTokens(docsIndex, {"150_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring.md",
                                             "151_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring_test_plan.md"}) &&
               containsAllTokens(prompt, {"TASK-176", "docs/150", "docs/151"});
    }

    if (caseName == "dataserviceactions_reads_default_state") {
        return containsAllTokens(actions, {"ShellAccountingBrokerSandboxRuntimeEnablement.h",
                                          "defaultShellAccountingBrokerSandboxRuntimeEnablementState",
                                          "sandboxEnablementState.enabled",
                                          "sandboxEnablementState.available",
                                          "sandboxEnablementState.failClosed"});
    }

    if (caseName == "dataserviceactions_reads_only_default_state") {
        return expect(countOccurrences(actions, "defaultShellAccountingBrokerSandboxRuntimeEnablementState") == 1,
                      "DataServiceActions reads the default enablement state once") &&
               containsNoTokens(actions, {"setShellAccountingBrokerSandboxRuntimeEnablement",
                                         "registerShellAccountingBrokerSandboxRuntimeEnablement",
                                         "overrideShellAccountingBrokerSandboxRuntimeEnablement",
                                         "runtimeInjection", "sandboxEnablementFrom"});
    }

    if (caseName == "dataserviceactions_no_response_field") {
        const auto payloadStart = dryRunAction.find("std::ostringstream payload");
        const auto payloadSegment = payloadStart == std::string::npos ? std::string{} : dryRunAction.substr(payloadStart);
        return containsNoTokens(payloadSegment, {"sandboxEnablement", "enablementState",
                                                "sandboxRuntimeEnabled", "sandboxRuntimeAvailable",
                                                "BROKER_SANDBOX_RUNTIME_DISABLED"});
    }

    if (caseName == "dataserviceactions_no_request_field") {
        return containsNoTokens(dryRunAction,
                                {"extractJsonStringField(context.request.payloadJson, \"brokerPortMode\")",
                                 "extractJsonStringField(context.request.payloadJson, \"runtimeMode\")",
                                 "extractJsonStringField(context.request.payloadJson, \"brokerMode\")",
                                 "extractJsonBoolField(context.request.payloadJson, \"sandboxRuntimeEnabled\")",
                                 "extractJsonBoolField(context.request.payloadJson, \"sandboxEnablement\")"});
    }

    if (caseName == "dataserviceactions_disabled_default_selector") {
        return containsAllTokens(actions,
                                 {"const auto defaultBrokerPortMode =",
                                  "defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()",
                                  "shellAccountingBrokerRuntimeModeSourceForMode(",
                                  "defaultBrokerPortMode)",
                                  ".brokerOrderPortMode()",
                                  "shellAccountingBrokerOrderPortForMode(brokerPortMode)"});
    }

    if (caseName == "no_dataserviceactions_sandbox_paper_real") {
        return containsNoTokens(actions, {"ShellAccountingBrokerOrderPortMode::Sandbox",
                                         "ShellAccountingBrokerOrderPortMode::Paper",
                                         "ShellAccountingBrokerOrderPortMode::Real",
                                         "shellAccountingBrokerRuntimeModeSourceForMode(\"sandbox\")"});
    }

    if (caseName == "enablement_source_unmodified") {
        return containsAllTokens(enablement,
                                 {"ShellAccountingBrokerSandboxRuntimeEnablementState",
                                  "enabled{false}", "available{false}", "failClosed{true}",
                                  "defaultShellAccountingBrokerSandboxRuntimeEnablementState"}) &&
               containsNoTokens(enablement, {"TASK-176", "DataServiceActions", "QSettings",
                                            "getenv(", "qgetenv(", "credentialStore",
                                            "brokerEndpoint"});
    }

    if (caseName == "runtime_source_unmodified") {
        return containsNoTokens(runtimeSource, {"ShellAccountingBrokerSandboxRuntimeEnablement",
                                               "BROKER_SANDBOX_RUNTIME_DISABLED", "TASK-176"});
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
    if (caseName == "default_state_stable") {
        return expect(std::string(state.runtimeMode) == "sandbox_runtime_disabled",
                      "runtime mode remains disabled") &&
               expect(std::string(state.status) == "disabled", "status remains disabled") &&
               expect(std::string(state.errorCode) == "BROKER_SANDBOX_RUNTIME_DISABLED",
                      "error code remains disabled");
    }

    const auto portResponse = defaultBrokerPortResponse();
    if (caseName == "broker_response_disabled") {
        return expect(!portResponse.success, "disabled broker port does not succeed") &&
               expect(portResponse.errorCode == "BROKER_ORDER_DISABLED", "disabled broker code");
    }
    if (caseName == "broker_port_mode_disabled") {
        return expect(portResponse.brokerMode == "disabled", "broker mode remains disabled");
    }
    if (caseName == "broker_port_disabled_true") {
        return expect(portResponse.disabled, "broker port disabled remains true");
    }
    if (caseName == "broker_port_dry_run_only_true") {
        return expect(portResponse.dryRunOnly, "broker port dry-run-only remains true");
    }

    if (caseName == "sandbox_runtime_not_enabled") {
        return containsAllTokens(docs, {"does not enable sandbox runtime",
                                       "disabled default source"}) &&
               containsNoTokens(actions, {"ShellAccountingBrokerOrderPortMode::Sandbox"});
    }

    if (caseName == "selector_direct_test_only") {
        const auto docs138 = readTextFile(root / "docs" /
                                          "138_shell_accounting_broker_sandbox_runtime_mode_source_selector_scaffold.md");
        return containsAllTokens(docs138, {"direct-test"}) &&
               containsNoTokens(actions, {"ShellAccountingBrokerOrderPortMode::Sandbox"});
    }

    if (caseName == "direct_sandbox_selector_unavailable") {
        auto& sourceForMode = etfdt::dataservice::shellAccountingBrokerRuntimeModeSourceForMode(
            etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Sandbox);
        etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
        const auto response = etfdt::dataservice::shellAccountingBrokerOrderPortForMode(
                                  sourceForMode.brokerOrderPortMode())
                                  .submitOrderEnvelope(request);
        return expect(!response.success, "direct sandbox selector does not succeed") &&
               expect(response.disabled, "direct sandbox selector remains disabled") &&
               expect(response.dryRunOnly, "direct sandbox selector remains dry-run only") &&
               expect(response.brokerMode == "sandbox", "direct sandbox selector broker mode") &&
               expect(response.errorCode == "BROKER_SANDBOX_NOT_CONFIGURED",
                      "direct sandbox selector unavailable code");
    }

    if (caseName == "sandbox_adapter_not_implemented") {
        return containsNoTokens(runtimeProduction, {"SandboxBrokerAdapter",
                                                   "ShellAccountingSandboxBrokerAdapter",
                                                   "sandboxBrokerAdapter"});
    }

    if (caseName == "paper_real_not_implemented") {
        return expect(etfdt::dataservice::shellAccountingBrokerOrderPortModeFromString("paper") ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Unsupported,
                      "paper remains unsupported") &&
               expect(etfdt::dataservice::shellAccountingBrokerOrderPortModeFromString("real") ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Unsupported,
                      "real remains unsupported") &&
               containsNoTokens(runtimeSource, {"PaperShellAccountingBrokerRuntimeModeSource",
                                               "RealShellAccountingBrokerRuntimeModeSource"});
    }

    if (caseName == "no_external_mode_source") {
        return containsNoTokens(runtimeProduction + "\n" + dryRunAction, externalModeTokens());
    }
    if (caseName == "no_credentials_endpoint_account_order") {
        return containsNoTokens(runtimeProduction, {"credential", "Credential", "endpoint", "Endpoint",
                                                   "accountData", "orderData", "accountId", "orderId"});
    }
    if (caseName == "no_broker_sdk") {
        return containsNoTokens(runtimeProduction, brokerSdkTokens());
    }
    if (caseName == "no_network_endpoint") {
        return containsNoTokens(runtimeProduction, networkTokens());
    }
    if (caseName == "no_credentials_secret_values") {
        return containsNoTokens(runtimeProduction + "\n" + docs, credentialValueTokens()) &&
               containsAllTokens(docs, {"Real credentials", "must not"});
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
        return containsAllTokens(migration, {"trade_log", "trade_draft", "audit_log"}) &&
               containsNoTokens(readTextFile(root / "migrations" / "001_initial_schema.sql"),
                                {"sandbox_runtime_enablement"});
    }

    if (caseName == "task175_evolved") {
        const auto docs148 = readTextFile(root / "docs" /
                                          "148_shell_accounting_broker_sandbox_runtime_enablement_state_wiring_authorization_gate.md");
        return containsAllTokens(docs148, {"TASK-176", "may read the default disabled",
                                          "does not enable sandbox runtime"});
    }
    if (caseName == "task174_still_valid") {
        const auto docs146 = readTextFile(root / "docs" /
                                          "146_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold.md");
        return containsAllTokens(docs146, {"TASK-174", "disabled, unavailable, and fail-closed"}) &&
               containsAllTokens(enablement, {"enabled{false}", "available{false}", "failClosed{true}"});
    }
    if (caseName == "task172_still_valid") {
        const auto docs142 = readTextFile(root / "docs" /
                                          "142_shell_accounting_broker_sandbox_runtime_mode_source_selector_disabled_wiring.md");
        return containsAllTokens(docs142, {"TASK-172", "disabled-default selector wiring"}) &&
               expect(portResponse.brokerMode == "disabled", "TASK-172 disabled selector remains active");
    }
    if (caseName == "task166_still_valid") {
        const auto docs130 = readTextFile(root / "docs" /
                                          "130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md");
        return containsAllTokens(docs130, {"TASK-166", "disabled-only"}) &&
               expect(etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                          .brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Disabled,
                      "default runtime source remains disabled-only");
    }
    if (caseName == "rollback_policy") {
        return containsAllTokens(docs, {"Rollback", "TASK-172",
                                       "TASK-166", "disabled/null broker dry-run behavior"});
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
