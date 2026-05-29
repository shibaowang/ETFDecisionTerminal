#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::string dataServiceAccountingActionRegion(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::vector<std::filesystem::path> shellAccountingProductionBoundaryFiles(
    const std::filesystem::path& root);
[[nodiscard]] bool containsTokenInFiles(const std::vector<std::filesystem::path>& files, const std::string& token);
[[nodiscard]] int countTokenInText(const std::string& text, const std::string& token);
[[nodiscard]] std::filesystem::path docs81Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path docs82Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path dataServiceActionsPath(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::string> authorizedReadOnlyActions();
[[nodiscard]] std::vector<std::string> allowedReadSourceTokens();
[[nodiscard]] std::vector<std::string> forbiddenWritePolicyTokens();
[[nodiscard]] std::vector<std::string> sqliteDirectTokens();
[[nodiscard]] std::vector<std::string> accountingEngineReplayTokens();

}  // namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate
