#include "ShellAccountingSnapshotRebuildWriteGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate {
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

std::string shellUiAndServiceBoundaryText(const std::filesystem::path& root)
{
    return joinFiles(shellBoundaryFiles(root));
}

std::string dataServiceReadOnlyAccountingRegion(const std::filesystem::path& root)
{
    const auto source = readTextFile(dataServiceActionsPath(root));
    const auto begin = source.find("bool shellAccountingReplayRequested");
    const auto end = source.find("}  // namespace etfdt::data_service_api", begin);
    if (begin == std::string::npos || end == std::string::npos || end <= begin) {
        return {};
    }
    auto region = source.substr(begin, end - begin);
    const auto auditBegin = region.find("struct AuditPayloadParseResult");
    const auto actionBegin = region.find("etfdt::protocol::ProtocolResponse handlePositionList");
    if (auditBegin != std::string::npos && actionBegin != std::string::npos && auditBegin < actionBegin) {
        region.erase(auditBegin, actionBegin - auditBegin);
    }
    return region;
}

std::filesystem::path docs87Path(const std::filesystem::path& root)
{
    return root / "docs" / "87_shell_accounting_snapshot_rebuild_write_gate.md";
}

std::filesystem::path docs88Path(const std::filesystem::path& root)
{
    return root / "docs" / "88_shell_accounting_snapshot_rebuild_write_test_plan.md";
}

std::filesystem::path docs86Path(const std::filesystem::path& root)
{
    return root / "docs" / "86_shell_accounting_readonly_replay_implementation.md";
}

std::filesystem::path dataServiceActionsPath(const std::filesystem::path& root)
{
    return root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp";
}

std::filesystem::path readOnlyFactsQueryPath(const std::filesystem::path& root)
{
    return root / "libs" / "DataAccess" / "src" / "ShellAccountingReadOnlyFactsQuery.cpp";
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

std::vector<std::string> taskPrerequisiteTokens()
{
    return {
        "TASK-119",
        "TASK-120",
        "TASK-121",
        "TASK-122",
        "TASK-123",
        "TASK-124",
        "TASK-125",
        "TASK-126",
        "TASK-127",
        "TASK-128",
        "TASK-129",
        "TASK-130",
        "TASK-131",
        "TASK-132",
        "TASK-133",
        "TASK-134",
        "TASK-135",
        "TASK-136",
        "TASK-137",
        "TASK-138",
        "TASK-139",
    };
}

std::vector<std::string> rebuildForbiddenTokens()
{
    return {
        "rebuildSnapshot",
        "rebuildFromReplay",
        "rebuildFromFacts",
        "snapshotRebuild(",
        "snapshot.rebuild",
        "accounting.snapshot.rebuild",
    };
}

std::vector<std::string> snapshotWriteForbiddenTokens()
{
    return {
        "INSERT INTO cash_snapshot",
        "INSERT INTO position_snapshot",
        "INSERT INTO portfolio_summary",
        "UPDATE cash_snapshot",
        "UPDATE position_snapshot",
        "UPDATE portfolio_summary",
        "DELETE FROM cash_snapshot",
        "DELETE FROM position_snapshot",
        "DELETE FROM portfolio_summary",
        "REPLACE INTO cash_snapshot",
        "REPLACE INTO position_snapshot",
        "REPLACE INTO portfolio_summary",
        "INSERT INTO audit_log",
        "INSERT INTO trade_log",
        "INSERT INTO trade_execution_group",
        "INSERT INTO trade_draft",
        "data.audit.append",
    };
}

std::vector<std::string> uiTriggeredWriteForbiddenTokens()
{
    return {
        "rebuildSnapshot",
        "rebuildFromReplay",
        "snapshotRebuild(",
        "snapshot.rebuild",
        "cash_snapshot",
        "position_snapshot",
        "portfolio_summary",
        "SQLiteConnection",
        "sqlite3",
        "DataAccess/",
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
    };
}

std::vector<std::string> tradeOrStrategyForbiddenTokens()
{
    return {
        "TradeDraft",
        "createTradeDraft",
        "brokerOrder(",
        "placeOrder",
        "strategyExecute",
        "executeStrategy",
        "confirmTrade",
        "order confirmation",
        "buy button",
        "sell button",
        "trading action",
    };
}

}  // namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate
