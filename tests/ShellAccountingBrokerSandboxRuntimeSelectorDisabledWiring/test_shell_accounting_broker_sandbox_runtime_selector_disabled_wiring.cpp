#include "ShellAccountingBrokerOrderImplementation.h"

#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"

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

std::string shellPathText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp", ".qml", ".js"});
    const auto shellServices = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), shellServices.begin(), shellServices.end());
    files.insert(files.end(), shellCore.begin(), shellCore.end());
    return joinFiles(files);
}

std::string docs126Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" /
                        "126_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring.md");
}

std::string docs127Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" /
                        "127_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring_test_plan.md");
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

std::vector<std::string> credentialTokens()
{
    return {"getenv(", "qgetenv(", "QSettings", "credentialStore", "secretManager",
            "readCredential", "loadCredential", "brokerCredential", "BROKER_API_KEY=",
            "BROKER_SECRET=", "BROKER_PASSWORD=", "brokerToken=", "apiSecret=", "secretKey=",
            "privateKey=", "-----BEGIN PRIVATE KEY-----", "http://", "https://"};
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

std::vector<std::string> selectorExposureTokens()
{
    return {"ShellAccountingBrokerOrderPortModeSelector", "shellAccountingBrokerOrderPortForMode",
            "defaultShellAccountingBrokerOrderPortMode", "brokerRuntimeMode", "sandboxRuntimeMode"};
}

std::vector<std::string> externalModeSourceTokens()
{
    return {"extractJsonStringField(context.request.payloadJson, \"broker", "getenv(", "qgetenv(",
            "QSettings", "commandLine", "secretManager", "credentialStore", "runtimeBrokerMode",
            "sandboxRuntimeMode"};
}

bool runtimeDryRunDisabled(const std::filesystem::path& root, const std::filesystem::path& migration)
{
    auto connection = broker_tests::openMigratedDatabase(migration);
    broker_tests::seedReplayFixture(connection);
    const auto call = broker_tests::createConfirmedTradeAndDryRun(connection);
    bool ok = broker_tests::expectSuccessfulBrokerOrderDryRun(call);
    ok &= expect(call.payload.value("brokerPortMode").toString().toStdString() == "disabled",
                 "runtime brokerPortMode remains disabled");
    ok &= expect(call.payload.value("brokerPortDisabled").toBool(false), "runtime broker port disabled");
    ok &= expect(call.payload.value("brokerPortDryRunOnly").toBool(false), "runtime broker port dry-run only");
    ok &= expect(call.payload.value("brokerPortErrorCode").toString().toStdString() == "BROKER_ORDER_DISABLED",
                 "runtime broker port error code unchanged");
    ok &= expect(call.payload.value("brokerOrderId").isNull(), "runtime has no broker order id");
    (void)root;
    return ok;
}

bool runCase(const std::filesystem::path& root, const std::filesystem::path& migration, const std::string& caseName)
{
    const auto dataServiceActions = dataServiceActionsText(root);
    const auto selectorSource = selectorSourceText(root);
    const auto shellText = shellPathText(root);
    const auto docs126 = docs126Text(root);
    const auto docs127 = docs127Text(root);
    const auto docs = docs126 + "\n" + docs127;

    if (caseName == "docs") {
        return expect(std::filesystem::exists(
                          root / "docs" /
                          "126_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring.md"),
                      "docs/126 exists") &&
               expect(std::filesystem::exists(
                          root / "docs" /
                          "127_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring_test_plan.md"),
                      "docs/127 exists") &&
               containsAllTokens(docs, {"TASK-164", "disabled-default runtime selector wiring",
                                        "runtime mode source not authorized", "sandbox runtime not enabled"});
    }

    if (caseName == "uses_selector") {
        return containsAllTokens(dataServiceActions, {"ShellAccountingBrokerOrderPortModeSelector.h",
                                                     "defaultShellAccountingBrokerOrderPortMode()",
                                                     "shellAccountingBrokerOrderPortForMode(brokerPortMode)"});
    }

    if (caseName == "default_mode_only") {
        return containsAllTokens(dataServiceActions, {"const auto brokerPortMode",
                                                     "defaultShellAccountingBrokerOrderPortMode()"}) &&
               containsNoTokens(dataServiceActions, externalModeSourceTokens());
    }

    if (caseName == "no_sandbox_paper_real_runtime_strings") {
        return containsNoTokens(dataServiceActions, {"shellAccountingBrokerOrderPortForMode(\"sandbox\")",
                                                    "shellAccountingBrokerOrderPortForMode(\"paper\")",
                                                    "shellAccountingBrokerOrderPortForMode(\"real\")",
                                                    "brokerPortMode = \"sandbox\"", "brokerPortMode = \"paper\"",
                                                    "brokerPortMode = \"real\""});
    }

    if (caseName == "no_payload_mode_source") {
        return containsNoTokens(dataServiceActions, {"extractJsonStringField(context.request.payloadJson, \"brokerMode\"",
                                                    "extractJsonStringField(context.request.payloadJson, \"brokerPortMode\"",
                                                    "extractJsonStringField(context.request.payloadJson, \"sandboxMode\""});
    }

    if (caseName == "no_external_mode_source") {
        return containsNoTokens(dataServiceActions, externalModeSourceTokens());
    }

    if (caseName == "runtime_disabled_semantics" || caseName == "error_code_unchanged"
        || caseName == "broker_port_mode_disabled" || caseName == "sandbox_not_runtime") {
        return runtimeDryRunDisabled(root, migration);
    }

    if (caseName == "direct_sandbox_scaffold_unavailable") {
        using namespace etfdt::dataservice;
        ShellAccountingBrokerOrderRequest request;
        auto response = shellAccountingBrokerOrderPortForMode("sandbox").submitOrderEnvelope(request);
        return expect(!response.success, "sandbox scaffold does not succeed") &&
               expect(response.disabled, "sandbox scaffold disabled") &&
               expect(response.dryRunOnly, "sandbox scaffold dry-run only") &&
               expect(response.brokerMode == "sandbox", "sandbox scaffold broker mode") &&
               expect(response.errorCode == "BROKER_SANDBOX_NOT_CONFIGURED",
                      "sandbox scaffold unavailable code");
    }

    if (caseName == "unknown_mode_fail_closed") {
        using namespace etfdt::dataservice;
        ShellAccountingBrokerOrderRequest request;
        auto response = shellAccountingBrokerOrderPortForMode("paper").submitOrderEnvelope(request);
        return expect(!response.success, "unknown mode does not succeed") &&
               expect(response.disabled, "unknown mode disabled") &&
               expect(response.brokerMode == "disabled", "unknown mode fails closed");
    }

    if (caseName == "task163_gate_updated") {
        const auto docs124 = readTextFile(root / "docs" /
                                          "124_shell_accounting_broker_sandbox_runtime_selector_authorization_gate.md");
        const auto docs125 = readTextFile(root / "docs" /
                                          "125_shell_accounting_broker_sandbox_runtime_selector_authorization_test_plan.md");
        return containsAllTokens(docs124 + "\n" + docs125,
                                 {"TASK-164", "disabled-default selector wiring",
                                  "no external runtime mode source", "Sandbox runtime remains disabled"});
    }

    if (caseName == "no_qml_startup_presenter_exposure") {
        return containsNoTokens(shellText, selectorExposureTokens()) &&
               containsNoTokens(shellText, {"brokerRuntimeMode", "sandboxRuntimeMode", "brokerCredential"});
    }

    if (caseName == "no_broker_sdk") {
        return containsNoTokens(selectorSource + "\n" + dataServiceActions, brokerSdkTokens());
    }

    if (caseName == "no_network") {
        return containsNoTokens(selectorSource + "\n" + dataServiceActions, networkTokens());
    }

    if (caseName == "no_credentials_or_endpoint") {
        return containsNoTokens(selectorSource + "\n" + dataServiceActions + "\n" + docs, credentialTokens());
    }

    if (caseName == "no_real_broker_order_id") {
        return containsNoTokens(selectorSource + "\n" + dataServiceActions, realOrderIdTokens());
    }

    if (caseName == "no_order_placement") {
        return containsNoTokens(selectorSource + "\n" + dataServiceActions, orderPlacementTokens());
    }

    if (caseName == "no_new_database_audit_ledger_write") {
        return containsNoTokens(selectorSource, writeTokens()) &&
               containsAllTokens(docs, {"no database / audit / ledger write"});
    }

    if (caseName == "no_reconciliation_cancellation_correction") {
        return containsNoTokens(selectorSource + "\n" + dataServiceActions, reconciliationTokens());
    }

    if (caseName == "no_strategy_or_auto_trading") {
        return containsNoTokens(selectorSource + "\n" + dataServiceActions, strategyTokens());
    }

    if (caseName == "docs_tests_policy_keywords_not_production") {
        return containsAllTokens(docs, {"broker SDK", "order placement", "credentials isolation"}) &&
               containsNoTokens(dataServiceActions, {"shellAccountingBrokerOrderPortForMode(\"sandbox\")",
                                                    "runtimeBrokerMode", "sandboxRuntimeMode"}) &&
               containsNoTokens(shellText, selectorExposureTokens());
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
