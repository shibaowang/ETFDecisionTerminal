#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_production_qml_binding_implementation {
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

bool isAllowedNegativeLine(const std::string& line)
{
    return line.find("not ") != std::string::npos ||
           line.find("no ") != std::string::npos ||
           line.find("must not") != std::string::npos ||
           line.find("without ") != std::string::npos ||
           (line.find("QML") != std::string::npos &&
            line.find("DataServiceClient") != std::string::npos &&
            line.find("SQLite") != std::string::npos);
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

bool containsAll(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) == std::string::npos) {
            return false;
        }
    }
    return true;
}

bool containsAny(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool isAllowedEpic281DashboardDraftLine(
    const std::filesystem::path& file,
    const std::string& fileText,
    const std::string& line,
    const std::string& token)
{
    if (file.filename().string() != "ShellAccountingReadOnlyPage.qml") {
        return false;
    }
    if (token != "TradeDraft" && token != "createTradeDraft") {
        return false;
    }
    if (!containsAll(
            fileText,
            {
                "shellAccountingDashboardRoot",
                "shellAccountingTradeDraftPanel",
                "shellAccountingTradeDraftConfirmationCheckBox",
                "previewTradeDraftFromLastRecommendation()",
                "createTradeDraftFromLastRecommendation(true)",
                "\xE8\xBF\x99\xE6\x98\xAF\xE5\x86\x85\xE9\x83\xA8\xE8\x8D\x89\xE6\xA1\x88",
                "\xE4\xB8\x8D\xE6\x98\xAF\xE8\xAE\xA2\xE5\x8D\x95",
            })) {
        return false;
    }
    return containsAny(
        line,
        {
            "lastTradeDraftIssueCodes",
            "resetTradeDraftState()",
            "shellAccountingTradeDraft",
            "TradeDraft from recommendation",
            "Preview TradeDraft",
            "previewTradeDraftFromLastRecommendation()",
            "lastTradeDraftStatus",
            "createTradeDraftFromLastRecommendation(true)",
            "lastTradeDraftDuplicate",
            "lastTradeDraftIdempotencyConflict",
            "lastTradeDraftSummary",
            "lastTradeDraftId",
            "createOtcMapMultiChannelTradeDraft(true)",
        });
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
    return filesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp"});
}

std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root)
{
    return filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"});
}

std::vector<std::filesystem::path> productionAppFiles(const std::filesystem::path& root)
{
    auto files = productionStartupFiles(root);
    const auto qmlFiles = productionQmlFiles(root);
    files.insert(files.end(), qmlFiles.begin(), qmlFiles.end());
    return files;
}

std::filesystem::path authorizedPagePath(const std::filesystem::path& root)
{
    return root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml";
}

int countTokenInFiles(
    const std::vector<std::filesystem::path>& files,
    const std::string& token)
{
    int count = 0;
    for (const auto& file : files) {
        count += countToken(readTextFile(file), token);
    }
    return count;
}

bool containsForbiddenToken(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens)
{
    for (const auto& file : files) {
        const auto fileText = readTextFile(file);
        std::ifstream input(file);
        std::string line;
        int lineNumber = 0;
        while (std::getline(input, line)) {
            ++lineNumber;
            for (const auto& token : tokens) {
                if (line.find(token) == std::string::npos || isAllowedNegativeLine(line)) {
                    continue;
                }
                if (isAllowedEpic281DashboardDraftLine(file, fileText, line, token)) {
                    continue;
                }
                std::cerr << file.generic_string() << ':' << lineNumber
                          << ": forbidden token `" << token << "`\n";
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> forbiddenRuntimeTokens()
{
    return {
        "AccountingEngine",
        "SQLite",
        "DataAccess",
        "setContextProperty(\"DataServiceClient\"",
        "setContextProperty(\"ShellAccountingDataServiceAdapter\"",
        "setContextProperty(\"ShellAccountingDataServiceClientPortAdapter\"",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
        "writeEnabled: true",
        "data.audit.append",
        "trade_log",
        "trade_draft",
        "cash_snapshot",
        "position_snapshot",
        "portfolio_summary",
    };
}

std::vector<std::string> tradeUiTokens()
{
    return {
        "buyButton",
        "sellButton",
        "BuyButton",
        "SellButton",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
        "submitOrder",
        "executeStrategy",
        "confirmTrade",
    };
}

}  // namespace etfdt::tests::shell_accounting_production_qml_binding_implementation
