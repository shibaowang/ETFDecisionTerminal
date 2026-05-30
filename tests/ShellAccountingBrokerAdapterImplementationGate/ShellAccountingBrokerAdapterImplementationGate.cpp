#include "ShellAccountingBrokerAdapterImplementationGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate {
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

}  // namespace

std::filesystem::path sourceRoot(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--source-root") {
            return argv[i + 1];
        }
    }
    return std::filesystem::current_path();
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
    auto files = filesUnder(root / "apps" / "ETFDataService", {".cpp", ".h", ".hpp"});
    const auto dataService = filesUnder(root / "libs" / "DataServiceApi", {".cpp", ".h", ".hpp"});
    const auto dataAccess = filesUnder(root / "libs" / "DataAccess", {".cpp", ".h", ".hpp"});
    const auto shellServices = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), dataService.begin(), dataService.end());
    files.insert(files.end(), dataAccess.begin(), dataAccess.end());
    files.insert(files.end(), shellServices.begin(), shellServices.end());
    files.insert(files.end(), shellCore.begin(), shellCore.end());
    return joinFiles(files);
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

std::string docs115Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "115_shell_accounting_broker_adapter_implementation_gate.md");
}

std::string docs116Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "116_shell_accounting_broker_adapter_implementation_test_plan.md");
}

std::string docsText(const std::filesystem::path& root)
{
    return docs115Text(root) + docs116Text(root);
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

std::vector<std::string> concreteAdapterTokens()
{
    return {
        "SandboxBrokerOrderAdapter",
        "PaperBrokerOrderAdapter",
        "RealBrokerOrderAdapter",
        "RealBrokerAdapter",
        "BrokerSdkOrderAdapter",
        "LiveBrokerOrderAdapter",
        "createRealBrokerAdapter",
        "makeBrokerAdapter(",
    };
}

std::vector<std::string> brokerSdkTokens()
{
    return {
        "BrokerSdk",
        "BrokerSDK",
        "#include <broker",
        "#include \"Broker",
        "ctp_api",
        "xtp_api",
        "ibapi",
        "twsapi",
    };
}

std::vector<std::string> brokerNetworkTokens()
{
    return {
        "brokerEndpoint",
        "brokerUrl",
        "orderEndpoint",
        "submitOrderHttp",
        "sendBrokerRequest",
        "sandboxBroker",
        "paperTradingBroker",
        "realBrokerEndpoint",
    };
}

std::vector<std::string> credentialTokens()
{
    return {
        "BROKER_API_KEY",
        "BROKER_SECRET",
        "BROKER_PASSWORD",
        "brokerPassword",
        "brokerToken",
        "apiSecret",
        "secretKey",
        "privateKey",
    };
}

std::vector<std::string> realOrderIdTokens()
{
    return {
        "realBrokerOrderId",
        "brokerOrderIdStorage",
        "persistBrokerOrderId",
        "acceptedBrokerOrderId",
    };
}

std::vector<std::string> qmlDirectBrokerTokens()
{
    return {
        "ShellAccountingBrokerOrderPort",
        "BrokerOrderAdapter",
        "brokerOrderPort",
        "brokerAdapter",
        "brokerSdk",
        "brokerEndpoint",
        "accounting.broker_order.place",
        "TASK-158",
    };
}

std::vector<std::string> presenterDirectBrokerTokens()
{
    return {
        "ShellAccountingBrokerOrderPort",
        "BrokerOrderAdapter",
        "brokerAdapter",
        "brokerSdk",
        "brokerEndpoint",
        "submitBrokerOrder",
    };
}

std::vector<std::string> strategyExecutionTokens()
{
    return {
        "StrategyEngine::execute",
        "strategyExecute(",
        "executeStrategy(",
    };
}

std::vector<std::string> automaticTradingTokens()
{
    return {
        "autoTrading",
        "automaticTradingEnabled",
        "AutomaticTrading",
        "auto_trade",
        "backgroundBrokerSubmission",
    };
}

}  // namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate
