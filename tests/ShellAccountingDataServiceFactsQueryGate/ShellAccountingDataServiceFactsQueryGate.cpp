#include "ShellAccountingDataServiceFactsQueryGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate {
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

std::string dataServiceAccountingActionRegion(const std::filesystem::path& root)
{
    const auto source = readTextFile(dataServiceActionsPath(root));
    const auto begin = source.find("etfdt::protocol::ProtocolResponse handlePositionList");
    const auto end = source.find("}  // namespace etfdt::data_service_api", begin);
    if (begin == std::string::npos || end == std::string::npos || end <= begin) {
        return {};
    }
    return source.substr(begin, end - begin);
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

std::vector<std::filesystem::path> shellAccountingProductionBoundaryFiles(const std::filesystem::path& root)
{
    auto result = filesUnder(root / "apps" / "ETFDecisionShell" / "src", {".cpp", ".h", ".hpp"});
    const auto qmlFiles = filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"});
    const std::vector<std::filesystem::path> shellFiles = {
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceAdapter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceClientPortAdapter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp",
    };
    result.insert(result.end(), qmlFiles.begin(), qmlFiles.end());
    result.insert(result.end(), shellFiles.begin(), shellFiles.end());
    return result;
}

bool containsTokenInFiles(const std::vector<std::filesystem::path>& files, const std::string& token)
{
    for (const auto& file : files) {
        if (readTextFile(file).find(token) != std::string::npos) {
            std::cerr << file.generic_string() << ": unexpected token `" << token << "`\n";
            return true;
        }
    }
    return false;
}

int countTokenInText(const std::string& text, const std::string& token)
{
    int count = 0;
    std::size_t offset = 0;
    while ((offset = text.find(token, offset)) != std::string::npos) {
        ++count;
        offset += token.size();
    }
    return count;
}

std::filesystem::path docs81Path(const std::filesystem::path& root)
{
    return root / "docs" / "81_shell_accounting_dataservice_readonly_facts_query_gate.md";
}

std::filesystem::path docs82Path(const std::filesystem::path& root)
{
    return root / "docs" / "82_shell_accounting_dataservice_readonly_facts_query_test_plan.md";
}

std::filesystem::path dataServiceActionsPath(const std::filesystem::path& root)
{
    return root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp";
}

std::vector<std::string> authorizedReadOnlyActions()
{
    return {
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "base_position.summary",
        "sniper_pool.summary",
    };
}

std::vector<std::string> allowedReadSourceTokens()
{
    return {
        "trade_log",
        "position_snapshot",
        "cash_snapshot",
        "portfolio_summary",
        "base_position_allocation",
        "trade_tier_allocation",
        "grid_cycle",
        "strategy / portfolio / account metadata",
    };
}

std::vector<std::string> forbiddenWritePolicyTokens()
{
    return {
        "INSERT",
        "UPDATE",
        "DELETE",
        "REPLACE",
        "DROP",
        "ALTER",
        "CREATE TABLE",
        "transaction write",
        "write repository",
        "audit append",
        "trade_log write",
        "trade_execution_group write",
        "trade_draft write",
        "cash_snapshot write",
        "position_snapshot write",
        "portfolio_summary write",
        "audit_log write",
        "data.audit.append",
        "confirmTrade",
        "manualEntry",
        "cashAdjustment",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
    };
}

std::vector<std::string> sqliteDirectTokens()
{
    return {
        "SQLiteConnection",
        "sqlite3",
        "SQL SELECT",
        "AccountingTradeFactReader",
        "ReadOnlyRepository",
    };
}

std::vector<std::string> accountingEngineReplayTokens()
{
    return {
        "AccountingReplayEngine",
        "replayFromFacts",
        "replay rebuild",
        "rebuild summary",
    };
}

}  // namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate
