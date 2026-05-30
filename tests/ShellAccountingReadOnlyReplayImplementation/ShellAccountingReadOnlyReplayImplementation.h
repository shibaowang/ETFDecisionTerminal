#pragma once

#include "DataAccess/DataAccess.h"

#include <QJsonObject>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_readonly_replay {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::filesystem::path migrationPath(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string shellBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string dataServiceActionsText(const std::filesystem::path& root);
[[nodiscard]] std::string readOnlyRegionText(const std::filesystem::path& root);
[[nodiscard]] std::string docs86Text(const std::filesystem::path& root);

[[nodiscard]] std::unique_ptr<etfdt::data_access::SQLiteConnection> openMigratedDatabase(
    const std::filesystem::path& migration);
void seedReplayFixture(etfdt::data_access::SQLiteConnection& connection);
void seedUsdReplayFixture(etfdt::data_access::SQLiteConnection& connection);
void seedSellExceedsFixture(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] QJsonObject invokeReplayAction(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& action,
    std::string payloadJson = "{\"accountId\":\"1001\",\"portfolioId\":\"1001\",\"calculationMode\":\"readonlyReplay\"}");
[[nodiscard]] bool expect(bool condition, const std::string& message);
[[nodiscard]] bool expectReplayPayload(
    const QJsonObject& payload,
    const std::string& action,
    const std::string& label);
[[nodiscard]] bool hasIssue(const QJsonObject& payload, const std::string& code);

[[nodiscard]] std::vector<std::string> snapshotWriteTokens();
[[nodiscard]] std::vector<std::string> tradeOrStrategyTokens();
[[nodiscard]] std::vector<std::string> uiDirectEngineTokens();

}  // namespace etfdt::tests::shell_accounting_readonly_replay
