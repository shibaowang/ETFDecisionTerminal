#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::vector<std::filesystem::path> productionStartupFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path shellAccountingReadOnlyPage(const std::filesystem::path& root);
[[nodiscard]] bool containsToken(
    const std::vector<std::filesystem::path>& files,
    const std::string& token);
[[nodiscard]] bool containsForbiddenToken(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens);
[[nodiscard]] int countToken(
    const std::vector<std::filesystem::path>& files,
    const std::string& token);
[[nodiscard]] std::vector<std::string> preGateTokens();
[[nodiscard]] std::vector<std::string> contextExposureTokens();
[[nodiscard]] std::vector<std::string> forbiddenRuntimeTokens();
[[nodiscard]] std::vector<std::string> forbiddenTradeUiTokens();
[[nodiscard]] bool shellAccountingQmlContainsForbiddenToken(
    const std::string& text,
    const std::string& token);

}  // namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate
