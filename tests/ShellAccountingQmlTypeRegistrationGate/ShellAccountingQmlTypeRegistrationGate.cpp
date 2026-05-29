#include "ShellAccountingQmlTypeRegistrationGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_qml_type_registration_gate {
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

std::vector<std::filesystem::path> collectFilesUnder(
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
    return collectFilesUnder(root, extensions);
}

std::vector<std::string> preGateTokens()
{
    return {"TASK-119", "TASK-121", "TASK-122", "TASK-125"};
}

std::vector<std::string> firstStageAllowedTypes()
{
    return {
        "ShellAccountingPresenter",
        "ShellAccountingStatusObject",
        "ShellAccountingIssueListModel",
        "ShellPositionListModel",
    };
}

std::vector<std::string> futureButNotFirstStageTypes()
{
    return {
        "ShellCashSummaryObject",
        "ShellPortfolioPnlObject",
        "ShellBasePositionObject",
        "ShellSniperPoolObject",
        "ShellSniperTierListModel",
    };
}

std::vector<std::string> forbiddenTypeTokens()
{
    return {
        "DataServiceClient",
        "DataServiceApi",
        "ShellAccountingDataServiceAdapter",
        "ShellAccountingDataServiceClientPortAdapter",
        "DataAccess",
        "SQLite",
        "SQLiteConnection",
        "AccountingEngine",
        "AccountingReplayEngine",
        "AccountingTradeFactReader",
        "StrategyEngine",
        "MarketEngine",
        "Broker",
        "write service",
    };
}

std::vector<std::string> forbiddenMethodPropertyTokens()
{
    return {
        "buy",
        "sell",
        "createTradeDraft",
        "brokerOrder",
        "submitOrder",
        "strategyExecute",
        "executeStrategy",
        "data.audit.append",
        "writeEnabled: true",
        "writeAction",
        "snapshotWrite",
        "tradeLogWrite",
        "positionSnapshotWrite",
        "cashSnapshotWrite",
        "portfolioSummaryWrite",
        "setWriteEnabled",
        "enableTrading",
        "submitTrade",
        "placeOrder",
        "generateSuggestion",
    };
}

std::vector<std::string> registrationLocationRules()
{
    return {
        "centralized",
        "not be scattered",
        "easy to static scan",
        "documented",
        "no-forbidden-registration test",
        "not be performed dynamically from QML",
        "Test-only helpers must not pretend",
        "Hidden plugin or service registration paths are not allowed",
    };
}

std::vector<std::string> rollbackRules()
{
    return {
        "Disable accounting QML binding",
        "Show Unavailable",
        "Do not fake data",
        "Do not directly call `DataServiceClient`",
        "Do not access SQLite",
        "Do not write",
        "Do not generate `TradeDraft`",
        "Do not generate trade suggestions",
    };
}

std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root)
{
    return collectFilesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"});
}

std::vector<std::filesystem::path> productionRegistrationFiles(const std::filesystem::path& root)
{
    return collectFilesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp"});
}

bool containsAnyToken(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens)
{
    for (const auto& file : files) {
        const auto text = readTextFile(file);
        for (const auto& token : tokens) {
            if (text.find(token) != std::string::npos) {
                std::cerr << file.generic_string() << ": forbidden production token `"
                          << token << "`\n";
                return true;
            }
        }
    }
    return false;
}

}  // namespace etfdt::tests::shell_accounting_qml_type_registration_gate
