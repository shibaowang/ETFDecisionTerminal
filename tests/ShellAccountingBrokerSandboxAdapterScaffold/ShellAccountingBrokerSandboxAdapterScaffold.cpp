#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold {
namespace {

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

std::filesystem::path argPath(int argc, char** argv, const std::string& flag)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == flag) {
            return argv[i + 1];
        }
    }
    return {};
}

}  // namespace

std::filesystem::path sourceRoot(int argc, char** argv)
{
    const auto value = argPath(argc, argv, "--source-root");
    return value.empty() ? std::filesystem::current_path() : value;
}

std::string readTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
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

std::string scaffoldSourceText(const std::filesystem::path& root)
{
    return readTextFile(
               root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
               "ShellAccountingBrokerSandboxOrderPortScaffold.h") +
           "\n" +
           readTextFile(
               root / "libs" / "DataServiceApi" / "src" /
               "ShellAccountingBrokerSandboxOrderPortScaffold.cpp");
}

std::string dataServiceApiText(const std::filesystem::path& root)
{
    return joinFiles(filesUnder(root / "libs" / "DataServiceApi", {".cpp", ".h", ".hpp"}));
}

std::string productionUiPathText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp", ".qml", ".js"});
    const auto shellServices = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), shellServices.begin(), shellServices.end());
    files.insert(files.end(), shellCore.begin(), shellCore.end());
    return joinFiles(files);
}

std::string docs120Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "120_shell_accounting_broker_sandbox_adapter_scaffold.md");
}

std::string docs121Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "121_shell_accounting_broker_sandbox_adapter_scaffold_test_plan.md");
}

std::string task161DocsText(const std::filesystem::path& root)
{
    return docs120Text(root) + "\n" + docs121Text(root);
}

std::string task160DocsText(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "118_shell_accounting_broker_sandbox_adapter_authorization_gate.md") +
           "\n" +
           readTextFile(root / "docs" / "119_shell_accounting_broker_sandbox_adapter_authorization_test_plan.md");
}

bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) != std::string::npos) {
            std::cerr << "unexpected token `" << token << "`\n";
            return true;
        }
    }
    return false;
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

bool expect(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << message << '\n';
        return false;
    }
    return true;
}

std::vector<std::string> brokerNetworkTokens()
{
    return {"QNetworkAccessManager", "QTcpSocket", "QUdpSocket", "QWebSocket", "curl_easy",
            "WinHttp", "winsock", "brokerEndpoint", "brokerUrl", "orderEndpoint",
            "submitOrderHttp", "sendBrokerRequest", "brokerSocket"};
}

std::vector<std::string> credentialValueTokens()
{
    return {"BROKER_API_KEY=", "BROKER_SECRET=", "BROKER_PASSWORD=", "brokerPassword=",
            "brokerToken=", "apiSecret=", "secretKey=", "privateKey=",
            "-----BEGIN PRIVATE KEY-----", "AKIA", "ghp_", "xoxb-"};
}

std::vector<std::string> credentialReadTokens()
{
    return {"getenv(", "qgetenv(", "QSettings", "credentialStore", "secretManager",
            "readCredential", "loadCredential", "brokerCredential"};
}

std::vector<std::string> brokerSdkTokens()
{
    return {"BrokerSdk", "BrokerSDK", "#include <broker", "#include \"Broker",
            "ctp_api", "xtp_api", "ibapi", "twsapi"};
}

std::vector<std::string> realOrderPlacementTokens()
{
    return {"placeRealOrder(", "submitRealOrder(", "sendRealOrder(", "placeOrder(",
            "submitOrder(", "realOrderPlacement", "brokerOrderSubmitted"};
}

std::vector<std::string> realOrderIdTokens()
{
    return {"realBrokerOrderId", "acceptedBrokerOrderId", "persistBrokerOrderId",
            "externalBrokerOrderId", "liveBrokerOrderId"};
}

std::vector<std::string> sqliteWriteTokens()
{
    return {"INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "CREATE ", "DROP ", "ALTER ",
            "sqlite3_exec", "QSqlQuery"};
}

std::vector<std::string> forbiddenWriteTableTokens()
{
    return {"audit_log", "trade_log", "trade_execution_group", "trade_draft"};
}

std::vector<std::string> strategyExecutionTokens()
{
    return {"StrategyEngine::execute", "strategyExecute(", "executeStrategy("};
}

std::vector<std::string> automaticTradingTokens()
{
    return {"autoTrading", "automaticTradingEnabled", "AutomaticTrading", "auto_trade",
            "backgroundBrokerSubmission"};
}

std::vector<std::string> qmlPresenterExposureTokens()
{
    return {"ShellAccountingBrokerSandboxOrderPortScaffold", "BrokerSandboxOrderPort",
            "brokerSandboxAdapter", "brokerCredential", "brokerEndpoint"};
}

}  // namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold
