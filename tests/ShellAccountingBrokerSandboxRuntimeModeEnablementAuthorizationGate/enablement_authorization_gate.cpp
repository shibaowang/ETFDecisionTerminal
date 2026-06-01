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
                        "144_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_gate.md") +
           "\n" +
           readTextFile(root / "docs" /
                        "145_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_test_plan.md");
}

std::string productionRuntimeText(const std::filesystem::path& root)
{
    return runtimeHeaderText(root) + "\n" + runtimeSourceText(root) + "\n" +
           orderPortSelectorText(root);
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
    const auto header = runtimeHeaderText(root);
    const auto source = runtimeSourceText(root);
    const auto actions = dataServiceActionsText(root);
    const auto shell = shellPathText(root);
    const auto docs = docsText(root);
    const auto runtimeProduction = productionRuntimeText(root);

    if (caseName == "docs") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "144_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_gate.md"),
                      "docs/144 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "145_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_test_plan.md"),
                      "docs/145 exists") &&
               containsAllTokens(docs, {"TASK-173", "gate-only", "does not enable sandbox runtime",
                                        "future sandbox runtime enablement"});
    }

    if (caseName == "docs_index") {
        const auto readme = readTextFile(root / "README.md");
        const auto docsIndex = readTextFile(root / "docs" / "README.md");
        const auto prompt = readTextFile(root / "docs" / "12_codex_prompt_template.md");
        return containsAllTokens(readme, {"docs/144_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_gate.md",
                                          "docs/145_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_test_plan.md"}) &&
               containsAllTokens(docsIndex, {"144_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_gate.md",
                                             "145_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_test_plan.md"}) &&
               containsAllTokens(prompt, {"TASK-173", "docs/144", "docs/145"});
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
                                         "shellAccountingBrokerRuntimeModeSourceForMode(\"sandbox\")",
                                         "shellAccountingBrokerRuntimeModeSourceForMode(\"paper\")",
                                         "shellAccountingBrokerRuntimeModeSourceForMode(\"real\")"});
    }

    if (caseName == "no_mode_source_inputs") {
        return containsNoTokens(actions, payloadModeTokens()) &&
               containsNoTokens(actions + "\n" + header + "\n" + source, externalModeTokens()) &&
               containsNoTokens(shell, {"brokerRuntimeMode", "sandboxRuntimeMode",
                                       "RuntimeModeSourceSelector"});
    }

    if (caseName == "runtime_source_production_unchanged") {
        return containsNoTokens(header + "\n" + source, {"TASK-173", "RuntimeModeEnablement",
                                                        "enableSandboxRuntime",
                                                        "sandboxRuntimeEnabled"});
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

    if (caseName == "broker_port_mode_disabled" || caseName == "broker_port_disabled_true" ||
        caseName == "broker_port_dry_run_only_true") {
        const auto defaultMode = etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                                     .brokerOrderPortMode();
        const auto selectedMode =
            etfdt::dataservice::shellAccountingBrokerRuntimeModeSourceForMode(defaultMode)
                .brokerOrderPortMode();
        etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
        const auto response =
            etfdt::dataservice::shellAccountingBrokerOrderPortForMode(selectedMode)
                .submitOrderEnvelope(request);
        if (caseName == "broker_port_mode_disabled") {
            return expect(response.brokerMode == "disabled", "broker mode remains disabled");
        }
        if (caseName == "broker_port_disabled_true") {
            return expect(response.disabled, "broker disabled remains true");
        }
        return expect(response.dryRunOnly, "broker dry-run-only remains true");
    }

    if (caseName == "sandbox_runtime_not_enabled") {
        return containsAllTokens(docs, {"sandbox runtime",
                                       "remains disabled",
                                       "separate TASK"}) &&
               containsNoTokens(actions, {"brokerPortMode = \"sandbox\"",
                                         "shellAccountingBrokerOrderPortForMode(\"sandbox\")"});
    }

    if (caseName == "sandbox_scaffold_direct_test_only") {
        return containsAllTokens(header, {"SandboxShellAccountingBrokerRuntimeModeSourceScaffold"}) &&
               containsAllTokens(docs, {"direct-test-only"}) &&
               containsNoTokens(actions + "\n" + shell, {"SandboxShellAccountingBrokerRuntimeModeSourceScaffold",
                                                        "ShellAccountingBrokerOrderPortMode::Sandbox"});
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
               containsNoTokens(header + "\n" + source,
                                {"PaperShellAccountingBrokerRuntimeModeSource",
                                 "RealShellAccountingBrokerRuntimeModeSource"});
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
        return containsNoTokens(runtimeProduction, writeTokens()) &&
               containsAllTokens(docs, {"database write", "audit write", "ledger write"});
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

    if (caseName == "task172_still_valid") {
        const auto docs142 = readTextFile(root / "docs" /
                                          "142_shell_accounting_broker_sandbox_runtime_mode_source_selector_disabled_wiring.md");
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        return containsAllTokens(docs142, {"TASK-173", "default disabled",
                                          "sandbox runtime remains disabled"}) &&
               containsAllTokens(cmake, {"ShellAccountingBrokerSandboxRuntimeModeSourceSelectorDisabledWiring"});
    }

    if (caseName == "task171_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        return containsAllTokens(cmake, {"ShellAccountingBrokerSandboxRuntimeModeSourceSelectorWiringAuthorizationGate"});
    }

    if (caseName == "task170_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs138 = readTextFile(root / "docs" /
                                          "138_shell_accounting_broker_sandbox_runtime_mode_source_selector_scaffold.md");
        return containsAllTokens(cmake, {"ShellAccountingBrokerSandboxRuntimeModeSourceSelectorScaffold"}) &&
               containsAllTokens(docs138, {"direct-test-only", "shellAccountingBrokerRuntimeModeSourceForMode"});
    }

    if (caseName == "task166_still_valid") {
        const auto cmake = readTextFile(root / "tests" / "CMakeLists.txt");
        const auto docs130 = readTextFile(root / "docs" /
                                          "130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md");
        return containsAllTokens(cmake, {"ShellAccountingBrokerRuntimeModeSourceDisabledScaffold"}) &&
               containsAllTokens(docs130, {"disabled-only"});
    }

    if (caseName == "docs_tests_policy_keywords_not_production") {
        return containsAllTokens(docs, {"credentials", "endpoint", "sandbox adapter", "order placement"}) &&
               containsNoTokens(runtimeProduction, {"enableSandboxRuntimeFromDocs",
                                                   "TASK-173_SANDBOX_RUNTIME_ENABLED"});
    }

    if (caseName == "rollback_policy") {
        return containsAllTokens(docs, {"Rollback", "TASK-172 disabled-default selector wiring",
                                       "TASK-166 disabled-only source",
                                       "TASK-159 disabled/null provider"});
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
