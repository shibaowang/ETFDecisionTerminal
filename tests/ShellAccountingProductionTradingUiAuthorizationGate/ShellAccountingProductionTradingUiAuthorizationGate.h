#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string productionQmlText(const std::filesystem::path& root);
[[nodiscard]] std::string shellBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string docs104Text(const std::filesystem::path& root);
[[nodiscard]] std::string docs105Text(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<std::string> productionTradingUiTokens();
[[nodiscard]] std::vector<std::string> draftCreateBindingTokens();
[[nodiscard]] std::vector<std::string> draftConfirmBindingTokens();
[[nodiscard]] std::vector<std::string> directDbWriteTokens();
[[nodiscard]] std::vector<std::string> brokerOrStrategyTokens();

}  // namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate
