#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate {
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

std::vector<std::filesystem::path> shellBoundaryFiles(const std::filesystem::path& root)
{
    auto result = filesUnder(root / "apps" / "ETFDecisionShell" / "src", {".cpp", ".h", ".hpp"});
    const auto qmlFiles = filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"});
    const auto shellServices = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    result.insert(result.end(), qmlFiles.begin(), qmlFiles.end());
    result.insert(result.end(), shellServices.begin(), shellServices.end());
    result.insert(result.end(), shellCore.begin(), shellCore.end());
    return result;
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

std::string shellBoundaryText(const std::filesystem::path& root)
{
    return joinFiles(shellBoundaryFiles(root));
}

std::string dataServiceBoundaryText(const std::filesystem::path& root)
{
    auto result = filesUnder(root / "libs" / "DataServiceApi", {".cpp", ".h", ".hpp"});
    const auto dataAccess = filesUnder(root / "libs" / "DataAccess", {".cpp", ".h", ".hpp"});
    const auto dataServiceApp = filesUnder(root / "apps" / "ETFDataService", {".cpp", ".h", ".hpp"});
    result.insert(result.end(), dataAccess.begin(), dataAccess.end());
    result.insert(result.end(), dataServiceApp.begin(), dataServiceApp.end());
    return joinFiles(result);
}

std::string docs101Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "101_shell_accounting_tradedraft_confirmation_authorization_gate.md");
}

std::string docs102Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "102_shell_accounting_tradedraft_confirmation_authorization_test_plan.md");
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

std::vector<std::string> confirmationImplementationTokens()
{
    return {
        "confirmTradeDraft",
        "handleAccountingTradeDraftConfirm",
        "kActionAccountingTradeDraftConfirm",
        "accounting.tradedraft.confirm",
        "TradeDraftConfirmationRepository",
        "TradeDraft to trade_log",
        "direct ledger write from draft",
    };
}

std::vector<std::string> tradeLogWriteTokens()
{
    return {
        "INSERT INTO trade_log",
        "UPDATE trade_log",
        "DELETE FROM trade_log",
        "REPLACE INTO trade_log",
    };
}

std::vector<std::string> executionGroupWriteTokens()
{
    return {
        "INSERT INTO trade_execution_group",
        "UPDATE trade_execution_group",
        "DELETE FROM trade_execution_group",
        "REPLACE INTO trade_execution_group",
    };
}

std::vector<std::string> uiConfirmationTriggerTokens()
{
    return {
        "confirmTradeDraft",
        "accounting.tradedraft.confirm",
        "confirm draft button",
        "execute trade button",
        "buy button",
        "sell button",
        "order button",
        "submitOrder",
        "placeOrder",
    };
}

std::vector<std::string> strategyOrBrokerTokens()
{
    return {
        "StrategyEngine::execute",
        "executeStrategy(",
        "strategyExecute(",
        "BrokerAdapter",
        "brokerOrder(",
        "placeOrder(",
        "executeOrder(",
        "autoTrading",
    };
}

}  // namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate
