#include "ShellAccountingRealDataAdapterImplementation.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

namespace etfdt::tests::shell_accounting_real_data_adapter_implementation {
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

bool containsAll(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) == std::string::npos) {
            return false;
        }
    }
    return true;
}

bool isAllowedEpic281DashboardToken(
    const std::filesystem::path& file,
    const std::string& text,
    const std::string& token)
{
    if (file.filename().string() != "ShellAccountingReadOnlyPage.qml" ||
        token != "createTradeDraft") {
        return false;
    }
    return containsAll(
        text,
        {
            "shellAccountingDashboardRoot",
            "shellAccountingTradeDraftPanel",
            "shellAccountingTradeDraftConfirmationCheckBox",
            "previewTradeDraftFromLastRecommendation()",
            "createTradeDraftFromLastRecommendation(true)",
            "\xE8\xBF\x99\xE6\x98\xAF\xE5\x86\x85\xE9\x83\xA8\xE8\x8D\x89\xE6\xA1\x88",
            "\xE4\xB8\x8D\xE6\x98\xAF\xE8\xAE\xA2\xE5\x8D\x95",
        });
}

etfdt::shell_services::ShellAccountingIssue makeIssue(
    std::string code,
    std::string message)
{
    return {std::move(code), "ERROR", std::move(message), true, "TASK-135-test"};
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

std::vector<std::filesystem::path> adapterBoundaryFiles(const std::filesystem::path& root)
{
    auto result = productionStartupFiles(root);
    const std::vector<std::filesystem::path> adapterFiles{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceAdapter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceClientPortAdapter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp",
    };
    result.insert(result.end(), adapterFiles.begin(), adapterFiles.end());
    return result;
}

std::filesystem::path docs80Path(const std::filesystem::path& root)
{
    return root / "docs" / "80_shell_accounting_real_data_adapter_implementation.md";
}

bool containsTokenInFiles(const std::vector<std::filesystem::path>& files, const std::string& token)
{
    for (const auto& file : files) {
        const auto text = readTextFile(file);
        if (text.find(token) != std::string::npos &&
            !isAllowedEpic281DashboardToken(file, text, token)) {
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

std::vector<std::string> allowedReadOnlyActions()
{
    return {
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "base_position.summary",
        "sniper_pool.summary",
    };
}

std::vector<std::string> forbiddenWriteTradeTokens()
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
        "createTradeDraft",
        "brokerOrder(",
        "strategyExecute(",
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

ScriptedAccountingServiceAdapter::ScriptedAccountingServiceAdapter(
    etfdt::shell_services::ShellAccountingServiceResult result)
    : result_(std::move(result))
{
}

etfdt::shell_services::ShellAccountingServiceResult
ScriptedAccountingServiceAdapter::fetchPositionList(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return responseFor(request, "position.list");
}

etfdt::shell_services::ShellAccountingServiceResult
ScriptedAccountingServiceAdapter::fetchCashSummary(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return responseFor(request, "cash.summary");
}

etfdt::shell_services::ShellAccountingServiceResult
ScriptedAccountingServiceAdapter::fetchPortfolioPnlSummary(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return responseFor(request, "portfolio.pnl.summary");
}

etfdt::shell_services::ShellAccountingServiceResult
ScriptedAccountingServiceAdapter::fetchBasePositionSummary(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return responseFor(request, "base_position.summary");
}

etfdt::shell_services::ShellAccountingServiceResult
ScriptedAccountingServiceAdapter::fetchSniperPoolSummary(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return responseFor(request, "sniper_pool.summary");
}

etfdt::shell_services::ShellAccountingServiceResult
ScriptedAccountingServiceAdapter::responseFor(
    const etfdt::shell_services::ShellAccountingServiceRequest& request,
    const char* fallbackActionName) const
{
    auto result = result_;
    result.actionName = request.actionName.empty() ? fallbackActionName : request.actionName;
    return result;
}

etfdt::shell_services::ShellAccountingServiceResult makeReadOnlySuccess(std::string actionName)
{
    etfdt::shell_services::ShellAccountingServiceResult result;
    result.actionName = std::move(actionName);
    result.protocolSuccess = true;
    result.implemented = true;
    result.readOnly = true;
    result.writeEnabled = false;
    result.payloadStatus = "OK";
    result.dataQualityStatus = "OK";
    result.hasRows = true;
    return result;
}

etfdt::shell_services::ShellAccountingServiceResult makeUnavailable(std::string actionName)
{
    etfdt::shell_services::ShellAccountingServiceResult result;
    result.actionName = std::move(actionName);
    result.protocolSuccess = true;
    result.implemented = false;
    result.readOnly = true;
    result.writeEnabled = false;
    result.payloadStatus = "POSITION_LIST_NOT_AVAILABLE";
    result.dataQualityStatus = "UNAVAILABLE";
    result.domainError = true;
    result.issues.push_back(makeIssue("POSITION_LIST_NOT_AVAILABLE", "Read-only data is unavailable."));
    return result;
}

etfdt::shell_services::ShellAccountingServiceResult makeTransportError(std::string actionName)
{
    auto result = makeUnavailable(std::move(actionName));
    result.protocolSuccess = false;
    result.payloadStatus = "DATASERVICE_CLIENT_CALL_FAILED";
    result.transportError = true;
    result.issues.push_back(makeIssue("TRANSPORT_ERROR", "Transport unavailable."));
    return result;
}

etfdt::shell_services::ShellAccountingServiceResult makeProtocolError(std::string actionName)
{
    auto result = makeUnavailable(std::move(actionName));
    result.protocolSuccess = false;
    result.payloadStatus = "DATASERVICE_CLIENT_MALFORMED_PAYLOAD";
    result.protocolError = true;
    result.issues.push_back(makeIssue("PROTOCOL_ERROR", "Protocol error."));
    return result;
}

etfdt::shell_services::ShellAccountingServiceResult makeTimeout(std::string actionName)
{
    auto result = makeUnavailable(std::move(actionName));
    result.protocolSuccess = false;
    result.payloadStatus = "DATASERVICE_CLIENT_CALL_FAILED";
    result.timeout = true;
    result.issues.push_back(makeIssue("TIMEOUT", "Timed out."));
    return result;
}

}  // namespace etfdt::tests::shell_accounting_real_data_adapter_implementation
