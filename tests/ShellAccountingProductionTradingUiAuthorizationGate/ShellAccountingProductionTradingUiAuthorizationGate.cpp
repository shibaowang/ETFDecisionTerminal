#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate {
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

std::string productionQmlText(const std::filesystem::path& root)
{
    return joinFiles(filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"}));
}

std::string shellBoundaryText(const std::filesystem::path& root)
{
    return joinFiles(shellBoundaryFiles(root));
}

std::string docs104Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "104_shell_accounting_production_trading_ui_authorization_gate.md");
}

std::string docs105Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "105_shell_accounting_production_trading_ui_authorization_test_plan.md");
}

bool containsAll(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) == std::string::npos) {
            return false;
        }
    }
    return true;
}

bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (token == "createTradeDraft" &&
            containsAll(
                text,
                {
                    "shellAccountingDashboardRoot",
                    "shellAccountingTradeDraftPanel",
                    "shellAccountingTradeDraftConfirmationCheckBox",
                    "previewTradeDraftFromLastRecommendation()",
                    "createTradeDraftFromLastRecommendation(true)",
                    "Draft, not order",
                    "not order",
                })) {
            continue;
        }
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

std::vector<std::string> authorizedTradingUiTokens()
{
    return {
        "objectName: \"shellAccountingTradingUiSection\"",
        "objectName: \"shellAccountingCreateDraftButton\"",
        "objectName: \"shellAccountingConfirmDraftButton\"",
        "objectName: \"shellAccountingConfirmReviewCheckBox\"",
        ".createDraft(",
        ".confirmDraft(",
    };
}

std::vector<std::string> forbiddenTradingUiTokens()
{
    return {
        "createTradeDraft",
        "confirmTradeDraft",
        "objectName: \"buyButton\"",
        "objectName: \"sellButton\"",
        "objectName: \"orderButton\"",
        "objectName: \"confirmTradeButton\"",
        "text: \"Buy\"",
        "text: \"Sell\"",
        "text: \"Order\"",
        "text: \"Confirm trade\"",
        "text: \"Confirm Trade\"",
    };
}

std::vector<std::string> authorizedDraftCreateBindingTokens()
{
    return {
        "accounting.tradedraft.create",
        "createDraft",
        "TASK-148_TRADEDRAFT_WRITE",
        "callTradeDraftCreate",
    };
}

std::vector<std::string> authorizedDraftConfirmBindingTokens()
{
    return {
        "accounting.tradedraft.confirm",
        "confirmDraft",
        "TASK-150_TRADEDRAFT_CONFIRM",
        "callTradeDraftConfirm",
    };
}

std::vector<std::string> directDbWriteTokens()
{
    return {
        "INSERT INTO",
        "UPDATE trade_draft",
        "UPDATE trade_log",
        "UPDATE trade_execution_group",
        "DELETE FROM",
        "REPLACE INTO",
        "sqlite3_",
        "QSqlDatabase",
        "#include \"DataAccess",
        "#include <DataAccess",
    };
}

std::vector<std::string> brokerOrStrategyTokens()
{
    return {
        "brokerOrder(",
        "placeOrder(",
        "executeOrder(",
        "submitOrder(",
        "StrategyEngine::execute",
        "strategyExecute(",
        "executeStrategy(",
        "autoTrading",
        "BrokerAdapter",
    };
}

}  // namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate
