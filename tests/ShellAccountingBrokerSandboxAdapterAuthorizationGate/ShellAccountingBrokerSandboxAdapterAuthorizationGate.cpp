#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate {
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

std::string productionText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "apps", {".cpp", ".h", ".hpp", ".qml", ".js"});
    const auto libs = filesUnder(root / "libs", {".cpp", ".h", ".hpp"});
    const auto migrations = filesUnder(root / "migrations", {".sql"});
    const auto tools = filesUnder(root / "tools", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), libs.begin(), libs.end());
    files.insert(files.end(), migrations.begin(), migrations.end());
    files.insert(files.end(), tools.begin(), tools.end());
    return joinFiles(files);
}

std::string dataServiceApiText(const std::filesystem::path& root)
{
    return joinFiles(filesUnder(root / "libs" / "DataServiceApi", {".cpp", ".h", ".hpp"}));
}

std::string qmlText(const std::filesystem::path& root)
{
    return joinFiles(filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"}));
}

std::string presenterText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), shellCore.begin(), shellCore.end());
    return joinFiles(files);
}

std::string docs118Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "118_shell_accounting_broker_sandbox_adapter_authorization_gate.md");
}

std::string docs119Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "119_shell_accounting_broker_sandbox_adapter_authorization_test_plan.md");
}

std::string task160DocsText(const std::filesystem::path& root)
{
    return docs118Text(root) + "\n" + docs119Text(root);
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

std::vector<std::string> concreteAdapterTokens()
{
    return {
        "SandboxShellAccountingBrokerOrderPort",
        "SandboxBrokerOrderAdapter",
        "SandboxBrokerAdapter",
        "PaperBrokerOrderAdapter",
        "PaperBrokerAdapter",
        "RealBrokerOrderAdapter",
        "RealBrokerAdapter",
        "BrokerSdkOrderAdapter",
        "LiveBrokerOrderAdapter",
        "createSandboxBrokerAdapter",
        "createPaperBrokerAdapter",
        "createRealBrokerAdapter",
        "makeSandboxBrokerAdapter(",
        "makePaperBrokerAdapter(",
        "makeRealBrokerAdapter(",
    };
}

std::vector<std::string> brokerSdkTokens()
{
    return {"BrokerSdk", "BrokerSDK", "#include <broker", "#include \"Broker", "ctp_api", "xtp_api", "ibapi", "twsapi"};
}

std::vector<std::string> brokerNetworkTokens()
{
    return {"brokerEndpoint", "brokerUrl", "orderEndpoint", "submitOrderHttp", "sendBrokerRequest",
            "sandboxBroker", "paperTradingBroker", "realBrokerEndpoint", "brokerSocket"};
}

std::vector<std::string> credentialValueTokens()
{
    return {"BROKER_API_KEY=", "BROKER_SECRET=", "BROKER_PASSWORD=", "brokerPassword=", "brokerToken=",
            "apiSecret=", "secretKey=", "privateKey=", "-----BEGIN PRIVATE KEY-----", "AKIA", "ghp_", "xoxb-"};
}

std::vector<std::string> realOrderPlacementTokens()
{
    return {"placeRealOrder(", "submitRealOrder(", "sendRealOrder(", "realOrderPlacement\":true",
            "brokerOrderSubmitted\":true", "brokerSdkCalled\":true"};
}

std::vector<std::string> realOrderIdStorageTokens()
{
    return {"realBrokerOrderId", "brokerOrderIdStorage", "persistBrokerOrderId", "acceptedBrokerOrderId"};
}

std::vector<std::string> reconciliationTokens()
{
    return {"BrokerOrderReconciliation", "reconcileBrokerOrder", "pollBrokerOrderStatus", "brokerStatusReconciliation"};
}

std::vector<std::string> cancellationCorrectionTokens()
{
    return {"cancelBrokerOrder", "correctBrokerOrder", "BrokerOrderCancellation", "BrokerOrderCorrection",
            "submitBrokerCancellation", "submitBrokerCorrection"};
}

std::vector<std::string> qmlPresenterExposureTokens()
{
    return {"ShellAccountingBrokerOrderPort", "defaultShellAccountingBrokerOrderPort", "BrokerOrderAdapter",
            "brokerAdapter", "brokerSdk", "brokerEndpoint"};
}

std::vector<std::string> strategyExecutionTokens()
{
    return {"StrategyEngine::execute", "strategyExecute(", "executeStrategy("};
}

std::vector<std::string> automaticTradingTokens()
{
    return {"autoTrading", "automaticTradingEnabled", "AutomaticTrading", "auto_trade", "backgroundBrokerSubmission"};
}

}  // namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate
