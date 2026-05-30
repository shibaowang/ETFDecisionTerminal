#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_production_trading_ui {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string productionQmlText(const std::filesystem::path& root);
[[nodiscard]] std::string shellServicesText(const std::filesystem::path& root);
[[nodiscard]] std::string docs106Text(const std::filesystem::path& root);
[[nodiscard]] bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<std::string> directDataServiceClientTokens();
[[nodiscard]] std::vector<std::string> directDbTokens();
[[nodiscard]] std::vector<std::string> brokerTokens();
[[nodiscard]] std::vector<std::string> strategyTokens();
[[nodiscard]] std::vector<std::string> automaticTradingTokens();
[[nodiscard]] std::vector<std::string> privacyLeakTokens();

}  // namespace etfdt::tests::shell_accounting_production_trading_ui
