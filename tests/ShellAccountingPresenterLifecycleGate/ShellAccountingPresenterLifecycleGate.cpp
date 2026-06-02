#include "ShellAccountingPresenterLifecycleGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate {
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
    return line.find("not ") != std::string::npos || line.find("no ") != std::string::npos ||
           line.find("does not") != std::string::npos || line.find("Do not") != std::string::npos ||
           line.find("must not") != std::string::npos || line.find("without") != std::string::npos ||
           (line.find("QML") != std::string::npos &&
            line.find("DataServiceClient") != std::string::npos &&
            line.find("SQLite") != std::string::npos);
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

std::filesystem::path shellAccountingReadOnlyPage(const std::filesystem::path& root)
{
    return root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml";
}

bool containsToken(const std::vector<std::filesystem::path>& files, const std::string& token)
{
    for (const auto& file : files) {
        if (readTextFile(file).find(token) != std::string::npos) {
            std::cerr << file.generic_string() << ": unexpected token `" << token << "`\n";
            return true;
        }
    }
    return false;
}

bool containsForbiddenToken(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens)
{
    for (const auto& file : files) {
        std::ifstream input(file);
        std::string line;
        int lineNumber = 0;
        while (std::getline(input, line)) {
            ++lineNumber;
            for (const auto& token : tokens) {
                if (line.find(token) == std::string::npos || isAllowedNegativeLine(line)) {
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

int countToken(const std::vector<std::filesystem::path>& files, const std::string& token)
{
    int count = 0;
    for (const auto& file : files) {
        const auto text = readTextFile(file);
        std::size_t offset = 0;
        while ((offset = text.find(token, offset)) != std::string::npos) {
            ++count;
            offset += token.size();
        }
    }
    return count;
}

std::vector<std::string> preGateTokens()
{
    return {
        "TASK-119",
        "TASK-121",
        "TASK-122",
        "TASK-125",
        "TASK-126",
        "TASK-127",
        "TASK-128",
        "TASK-129",
        "TASK-130",
        "TASK-131",
    };
}

std::vector<std::string> contextExposureTokens()
{
    return {
        "setContextProperty",
        "setInitialProperties",
        "ShellAccountingPresenter ",
        "ShellAccountingReadOnlyController ",
        "ShellAccountingDataServiceAdapter ",
        "ShellAccountingDataServiceClientPortAdapter ",
        "accountingPresenter",
    };
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
        "trade_log write",
        "trade_draft write",
        "cash_snapshot write",
        "position_snapshot write",
        "portfolio_summary write",
        "confirmTrade",
    };
}

std::vector<std::string> forbiddenTradeUiTokens()
{
    return {
        "买入",
        "卖出",
        "下单",
        "成交确认",
        "生成建议",
        "TradeDraft",
        "brokerOrder",
        "confirmTrade",
        "strategyExecute",
    };
}

}  // namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate
