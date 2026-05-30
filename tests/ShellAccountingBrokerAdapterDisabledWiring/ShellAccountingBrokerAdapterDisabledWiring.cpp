#include "ShellAccountingBrokerAdapterDisabledWiring.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring {
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

std::filesystem::path migrationPath(int argc, char** argv)
{
    return argPath(argc, argv, "--migration");
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

std::string docs117Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "117_shell_accounting_broker_adapter_disabled_wiring.md");
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

std::vector<std::string> brokerSdkTokens()
{
    return {"BrokerSdk", "BrokerSDK", "#include <broker", "#include \"Broker", "ctp_api", "xtp_api", "ibapi"};
}

std::vector<std::string> brokerNetworkTokens()
{
    return {"brokerEndpoint", "brokerUrl", "orderEndpoint", "submitOrderHttp", "sendBrokerRequest",
            "sandboxBroker", "paperTradingBroker", "realBrokerEndpoint"};
}

std::vector<std::string> credentialTokens()
{
    return {"BROKER_API_KEY", "BROKER_SECRET", "BROKER_PASSWORD", "brokerPassword", "brokerToken",
            "apiSecret", "secretKey", "privateKey"};
}

std::vector<std::string> realOrderIdTokens()
{
    return {"realBrokerOrderId", "brokerOrderIdStorage", "persistBrokerOrderId", "acceptedBrokerOrderId"};
}

std::vector<std::string> orderPlacementTokens()
{
    return {"placeRealOrder(", "submitRealOrder(", "sendRealOrder(", "realOrderPlacement\":true",
            "brokerOrderSubmitted\":true", "brokerSdkCalled\":true"};
}

std::vector<std::string> databaseWriteTokens()
{
    return {"INSERT INTO broker", "UPDATE broker", "DELETE FROM broker"};
}

std::vector<std::string> qmlExposureTokens()
{
    return {"ShellAccountingBrokerOrderPort", "defaultShellAccountingBrokerOrderPort", "brokerPort",
            "brokerAdapter", "BrokerOrderAdapter"};
}

std::vector<std::string> presenterExposureTokens()
{
    return {"ShellAccountingBrokerOrderPort", "defaultShellAccountingBrokerOrderPort", "brokerPort",
            "brokerAdapter", "BrokerOrderAdapter"};
}

std::vector<std::string> strategyExecutionTokens()
{
    return {"StrategyEngine::execute", "strategyExecute(", "executeStrategy("};
}

std::vector<std::string> automaticTradingTokens()
{
    return {"autoTrading", "automaticTradingEnabled", "AutomaticTrading", "auto_trade", "backgroundBrokerSubmission"};
}

}  // namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring
