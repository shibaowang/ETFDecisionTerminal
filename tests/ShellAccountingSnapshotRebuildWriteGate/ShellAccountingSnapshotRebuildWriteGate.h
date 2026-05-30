#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string shellUiAndServiceBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string dataServiceReadOnlyAccountingRegion(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path docs87Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path docs88Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path docs86Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path dataServiceActionsPath(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path readOnlyFactsQueryPath(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<std::string> taskPrerequisiteTokens();
[[nodiscard]] std::vector<std::string> rebuildForbiddenTokens();
[[nodiscard]] std::vector<std::string> snapshotWriteForbiddenTokens();
[[nodiscard]] std::vector<std::string> uiTriggeredWriteForbiddenTokens();
[[nodiscard]] std::vector<std::string> tradeOrStrategyForbiddenTokens();

}  // namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate
