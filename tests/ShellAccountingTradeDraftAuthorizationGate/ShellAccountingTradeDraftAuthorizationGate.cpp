#include "ShellAccountingTradeDraftAuthorizationGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate {
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
    result.insert(result.end(), dataAccess.begin(), dataAccess.end());
    return joinFiles(result);
}

std::string tradeDraftCreationBoundaryText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataAccess" / "include" / "DataAccess" / "ShellAccountingTradeDraftRepository.h") +
           readTextFile(root / "libs" / "DataAccess" / "src" / "ShellAccountingTradeDraftRepository.cpp");
}

std::string tradeDraftConfirmationBoundaryText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataAccess" / "include" / "DataAccess" /
                        "ShellAccountingTradeDraftConfirmationRepository.h") +
           readTextFile(root / "libs" / "DataAccess" / "src" / "ShellAccountingTradeDraftConfirmationRepository.cpp");
}

std::string docs98Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "98_shell_accounting_tradedraft_authorization_gate.md");
}

std::string docs99Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "99_shell_accounting_tradedraft_authorization_test_plan.md");
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

std::vector<std::string> draftImplementationTokens()
{
    return {
        "createTradeDraft",
        "handleAccountingTradeDraft",
        "kActionAccountingTradeDraft",
        "ShellAccountingTradeDraftRepository",
        "TradeDraftWriteRepository",
        "accounting.tradedraft.write",
    };
}

std::vector<std::string> draftWriteTokens()
{
    return {
        "INSERT INTO trade_draft",
        "UPDATE trade_draft",
        "DELETE FROM trade_draft",
        "REPLACE INTO trade_draft",
        "INSERT INTO trade_log",
        "UPDATE trade_log",
        "INSERT INTO trade_execution_group",
        "UPDATE trade_execution_group",
    };
}

std::vector<std::string> uiDraftTriggerTokens()
{
    return {
        "createTradeDraft",
        "accounting.tradedraft.write",
        "trade_draft.write",
        "draft.write",
        "submitDraft",
        "create draft button",
        "buy button",
        "sell button",
        "order button",
    };
}

std::vector<std::string> tradeExecutionTokens()
{
    return {
        "brokerOrder(",
        "placeOrder",
        "executeOrder",
        "BrokerAdapter",
        "autoTrading",
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
        "autoTrading",
    };
}

}  // namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate
