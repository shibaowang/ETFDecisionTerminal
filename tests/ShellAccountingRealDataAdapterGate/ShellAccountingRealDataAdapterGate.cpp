#include "ShellAccountingRealDataAdapterGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_real_data_adapter_gate {
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

int countToken(const std::string& text, const std::string& token)
{
    int count = 0;
    std::size_t offset = 0;
    while ((offset = text.find(token, offset)) != std::string::npos) {
        ++count;
        offset += token.size();
    }
    return count;
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

std::vector<std::filesystem::path> productionStartupFiles(const std::filesystem::path& root)
{
    return filesUnder(root / "apps" / "ETFDecisionShell" / "src", {".cpp", ".h", ".hpp"});
}

std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root)
{
    return filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"});
}

std::vector<std::filesystem::path> presenterCoreFiles(const std::filesystem::path& root)
{
    return {
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
    };
}

std::vector<std::filesystem::path> realAdapterBoundaryFiles(const std::filesystem::path& root)
{
    auto result = productionStartupFiles(root);
    const auto qmlFiles = productionQmlFiles(root);
    const auto coreFiles = presenterCoreFiles(root);
    result.insert(result.end(), qmlFiles.begin(), qmlFiles.end());
    result.insert(result.end(), coreFiles.begin(), coreFiles.end());
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

int countTokenInFiles(const std::vector<std::filesystem::path>& files, const std::string& token)
{
    int count = 0;
    for (const auto& file : files) {
        count += countToken(readTextFile(file), token);
    }
    return count;
}

std::filesystem::path docs78Path(const std::filesystem::path& root)
{
    return root / "docs" / "78_shell_accounting_real_data_adapter_gate.md";
}

std::filesystem::path docs79Path(const std::filesystem::path& root)
{
    return root / "docs" / "79_shell_accounting_real_data_adapter_test_plan.md";
}

std::vector<std::string> writePathTokens()
{
    return {
        "trade_log write",
        "trade_execution_group write",
        "trade_draft write",
        "cash_snapshot write",
        "position_snapshot write",
        "portfolio_summary write",
        "audit_log write",
        "data.audit.append",
        "confirmTrade",
        "confirmTradeDraft",
        "brokerOrder(",
        "strategyExecute",
        "writeEnabled: true",
    };
}

std::vector<std::string> sqliteDirectTokens()
{
    return {
        "SQLiteConnection",
        "sqlite3",
        "SQL SELECT",
        "trade_log SQL",
        "DataAccess repository",
        "AccountingTradeFactReader",
    };
}

std::vector<std::string> accountingEngineDirectTokens()
{
    return {
        "AccountingEngine",
        "AccountingReplayEngine",
        "AccountingTradeFactReader",
        "replayFromFacts",
    };
}

}  // namespace etfdt::tests::shell_accounting_real_data_adapter_gate
