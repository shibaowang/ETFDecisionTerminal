#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string shellBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string dataServiceBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string docs101Text(const std::filesystem::path& root);
[[nodiscard]] std::string docs102Text(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<std::string> confirmationImplementationTokens();
[[nodiscard]] std::vector<std::string> tradeLogWriteTokens();
[[nodiscard]] std::vector<std::string> executionGroupWriteTokens();
[[nodiscard]] std::vector<std::string> uiConfirmationTriggerTokens();
[[nodiscard]] std::vector<std::string> strategyOrBrokerTokens();

}  // namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate
