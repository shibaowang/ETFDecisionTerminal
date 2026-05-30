#include "ShellAccountingProductionTradingUiImplementation.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_production_trading_ui {
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
    std::string text;
    for (const auto& file : files) {
        text += "\n// FILE: " + file.generic_string() + "\n";
        text += readTextFile(file);
    }
    return text;
}

std::string productionQmlText(const std::filesystem::path& root)
{
    return joinFiles(filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"}));
}

std::string shellServicesText(const std::filesystem::path& root)
{
    return joinFiles(filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"}));
}

std::string docs106Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "106_shell_accounting_production_trading_ui_implementation.md");
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

std::vector<std::string> directDataServiceClientTokens()
{
    return {
        "DataServiceClient",
        "sendRaw(",
        "makeRequest(",
        "accounting.tradedraft.create",
        "accounting.tradedraft.confirm",
        "TASK-148_TRADEDRAFT_WRITE",
        "TASK-150_TRADEDRAFT_CONFIRM",
    };
}

std::vector<std::string> directDbTokens()
{
    return {
        "INSERT ",
        "UPDATE ",
        "DELETE ",
        "REPLACE ",
        "sqlite3_",
        "QSqlDatabase",
        "DataAccess",
        "trade_draft",
        "trade_log",
        "trade_execution_group",
    };
}

std::vector<std::string> brokerTokens()
{
    return {"brokerOrder", "placeOrder", "executeOrder", "BrokerAdapter"};
}

std::vector<std::string> strategyTokens()
{
    return {"StrategyEngine::execute", "strategyExecute", "executeStrategy"};
}

std::vector<std::string> automaticTradingTokens()
{
    return {"autoTrading", "automaticTrading\":true", "automaticTrading: true"};
}

std::vector<std::string> privacyLeakTokens()
{
    return {"rawSql", "raw SQL", "full trade_log", "full snapshot", "stackTrace", "internal stack"};
}

}  // namespace etfdt::tests::shell_accounting_production_trading_ui
