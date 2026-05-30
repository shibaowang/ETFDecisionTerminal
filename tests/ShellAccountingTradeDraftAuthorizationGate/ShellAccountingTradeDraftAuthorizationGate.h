#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string shellBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string dataServiceBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string tradeDraftCreationBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string tradeDraftConfirmationBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string docs98Text(const std::filesystem::path& root);
[[nodiscard]] std::string docs99Text(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<std::string> draftImplementationTokens();
[[nodiscard]] std::vector<std::string> draftWriteTokens();
[[nodiscard]] std::vector<std::string> uiDraftTriggerTokens();
[[nodiscard]] std::vector<std::string> tradeExecutionTokens();
[[nodiscard]] std::vector<std::string> strategyOrBrokerTokens();

}  // namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate
