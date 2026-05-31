#include "ShellAccountingBrokerOrderImplementation.h"

#include "DataServiceApi/ShellAccountingBrokerRuntimeModeSource.h"

#include <QJsonObject>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace broker_tests = etfdt::tests::shell_accounting_broker_order;

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
    return readTextFile(root / "docs" / "130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md") +
           "\n" +
           readTextFile(root / "docs" /
                        "131_shell_accounting_broker_runtime_mode_source_disabled_scaffold_test_plan.md");
}

std::vector<std::string> externalSourceTokens()
{
    return {"extractJsonStringField(context.request.payloadJson, \"brokerMode\"",
            "extractJsonStringField(context.request.payloadJson, \"brokerPortMode\"",
            "extractJsonStringField(context.request.payloadJson, \"sandboxMode\"",
            "extractJsonStringField(context.request.payloadJson, \"runtimeMode\"",
            "getenv(", "qgetenv(", "QSettings", "commandLine", "readModeFile",
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
            "submitOrderHttp", "sendBrokerRequest", "brokerSocket"};
}

std::vector<std::string> credentialOrEndpointTokens()
{
    return {"readCredential", "loadCredential", "brokerCredential", "brokerToken", "apiSecret",
            "secretKey", "privateKey", "certificatePath", "endpointUrl", "orderApiPath",
            "BROKER_API_KEY=", "BROKER_SECRET=", "BROKER_PASSWORD="};
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

bool runtimeDryRunDisabled(const std::filesystem::path& migration)
{
    auto connection = broker_tests::openMigratedDatabase(migration);
    broker_tests::seedReplayFixture(connection);
    const auto call = broker_tests::createConfirmedTradeAndDryRun(connection);
    bool ok = broker_tests::expectSuccessfulBrokerOrderDryRun(call);
    ok &= expect(call.payload.value("brokerPortMode").toString().toStdString() == "disabled",
                 "runtime brokerPortMode remains disabled");
    ok &= expect(call.payload.value("brokerPortDisabled").toBool(false),
                 "runtime broker port disabled");
    ok &= expect(call.payload.value("brokerPortDryRunOnly").toBool(false),
                 "runtime broker port remains dry-run-only for disabled provider");
    ok &= expect(call.payload.value("brokerPortErrorCode").toString().toStdString() ==
                     "BROKER_ORDER_DISABLED",
                 "runtime broker port error code unchanged");
    ok &= expect(call.payload.value("brokerOrderId").isNull(), "runtime has no broker order id");
    return ok;
}

bool runCase(const std::filesystem::path& root, const std::filesystem::path& migration, const std::string& caseName)
{
    const auto actions = dataServiceActionsText(root);
    const auto runtimeSource = runtimeSourceText(root);
    const auto selector = selectorText(root);
    const auto shell = shellPathText(root);
    const auto docs = docsText(root);

    if (caseName == "docs") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md"),
                      "docs/130 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "131_shell_accounting_broker_runtime_mode_source_disabled_scaffold_test_plan.md"),
                      "docs/131 exists") &&
               containsAllTokens(docs, {"TASK-166", "disabled-only internal broker runtime mode source",
                                        "External runtime mode source remains unauthorized",
                                        "sandbox runtime", "CI remains no-network",
                                        "credentials isolation", "no order placement"});
    }

    if (caseName == "source_compiles") {
        return containsAllTokens(runtimeSource, {"ShellAccountingBrokerRuntimeModeSource",
                                                "DisabledShellAccountingBrokerRuntimeModeSource",
                                                "defaultShellAccountingBrokerRuntimeModeSource"});
    }

    if (caseName == "direct_instantiation") {
        etfdt::dataservice::DisabledShellAccountingBrokerRuntimeModeSource source;
        return expect(source.brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Disabled,
                      "direct source returns disabled");
    }

    if (caseName == "default_source_disabled") {
        return expect(etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource()
                          .brokerOrderPortMode() ==
                          etfdt::dataservice::ShellAccountingBrokerOrderPortMode::Disabled,
                      "default runtime mode source returns disabled");
    }

    if (caseName == "dataserviceactions_uses_source") {
        return containsAllTokens(actions, {"ShellAccountingBrokerRuntimeModeSource.h",
                                          "defaultShellAccountingBrokerRuntimeModeSource()",
                                          ".brokerOrderPortMode()",
                                          "shellAccountingBrokerOrderPortForMode(brokerPortMode)"});
    }

    if (caseName == "dataserviceactions_disabled_mode_only") {
        return containsNoTokens(actions, {"shellAccountingBrokerOrderPortForMode(\"sandbox\")",
                                         "shellAccountingBrokerOrderPortForMode(\"paper\")",
                                         "shellAccountingBrokerOrderPortForMode(\"real\")",
                                         "brokerPortMode = \"sandbox\"", "brokerPortMode = \"paper\"",
                                         "brokerPortMode = \"real\""});
    }

    if (caseName == "runtime_dry_run_disabled" || caseName == "broker_port_mode_disabled" ||
        caseName == "error_code_unchanged" || caseName == "sandbox_scaffold_not_runtime") {
        return runtimeDryRunDisabled(migration);
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

    if (caseName == "unknown_mode_fail_closed") {
        etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
        auto response = etfdt::dataservice::shellAccountingBrokerOrderPortForMode("paper")
                            .submitOrderEnvelope(request);
        return expect(!response.success, "unknown mode does not succeed") &&
               expect(response.disabled, "unknown mode disabled") &&
               expect(response.brokerMode == "disabled", "unknown mode fails closed");
    }

    if (caseName == "no_payload_mode_source") {
        return containsNoTokens(actions, {"extractJsonStringField(context.request.payloadJson, \"brokerMode\"",
                                         "extractJsonStringField(context.request.payloadJson, \"brokerPortMode\"",
                                         "extractJsonStringField(context.request.payloadJson, \"sandboxMode\"",
                                         "extractJsonStringField(context.request.payloadJson, \"runtimeMode\""});
    }

    if (caseName == "no_qml_presenter_startup_mode_source") {
        return containsNoTokens(shell, {"ShellAccountingBrokerRuntimeModeSource",
                                       "ShellAccountingBrokerOrderPortModeSelector",
                                       "shellAccountingBrokerOrderPortForMode",
                                       "defaultShellAccountingBrokerRuntimeModeSource",
                                       "brokerRuntimeMode", "sandboxRuntimeMode"});
    }

    if (caseName == "no_config_env_cli_file_db_secret_source") {
        return containsNoTokens(actions + "\n" + runtimeSource, externalSourceTokens());
    }

    if (caseName == "no_broker_sdk") {
        return containsNoTokens(runtimeSource + "\n" + selector + "\n" + actions, brokerSdkTokens());
    }

    if (caseName == "no_network") {
        return containsNoTokens(runtimeSource + "\n" + selector + "\n" + actions, networkTokens());
    }

    if (caseName == "no_credentials_or_endpoint") {
        return containsNoTokens(runtimeSource + "\n" + selector + "\n" + actions,
                                credentialOrEndpointTokens());
    }

    if (caseName == "no_real_broker_order_id") {
        return containsNoTokens(runtimeSource + "\n" + selector + "\n" + actions, realOrderIdTokens());
    }

    if (caseName == "no_order_placement") {
        return containsNoTokens(runtimeSource + "\n" + selector + "\n" + actions, orderPlacementTokens());
    }

    if (caseName == "no_database_audit_ledger_write") {
        return containsNoTokens(runtimeSource + "\n" + selector, writeTokens()) &&
               containsAllTokens(docs, {"No Write Policy", "no audit write", "ledger write"});
    }

    if (caseName == "no_reconciliation_cancellation_correction") {
        return containsNoTokens(runtimeSource + "\n" + selector + "\n" + actions, reconciliationTokens());
    }

    if (caseName == "no_strategy_or_auto_trading") {
        return containsNoTokens(runtimeSource + "\n" + selector + "\n" + actions, strategyTokens());
    }

    if (caseName == "task160_to_165_baseline") {
        const auto rootTests = readTextFile(root / "tests" / "CMakeLists.txt");
        return containsAllTokens(rootTests, {"ShellAccountingBrokerSandboxAdapterAuthorizationGate",
                                            "ShellAccountingBrokerSandboxAdapterScaffold",
                                            "ShellAccountingBrokerSandboxModeSelector",
                                            "ShellAccountingBrokerSandboxRuntimeSelectorAuthorizationGate",
                                            "ShellAccountingBrokerSandboxRuntimeSelectorDisabledWiring",
                                            "ShellAccountingBrokerRuntimeModeSourceAuthorizationGate"}) &&
               containsAllTokens(docs, {"TASK-160 through TASK-165 regression",
                                        "TASK-165 completed"});
    }

    std::cerr << "unknown case `" << caseName << "`\n";
    return false;
}

}  // namespace

int main(int argc, char** argv)
{
    const auto rootArg = argPath(argc, argv, "--source-root");
    const auto root = rootArg.empty() ? std::filesystem::current_path() : rootArg;
    const auto migration = argPath(argc, argv, "--migration");
    const auto caseName = argValue(argc, argv, "--case");
    return runCase(root, migration, caseName) ? 0 : 1;
}
