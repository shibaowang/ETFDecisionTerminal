#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::filesystem::path migrationPath(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string dataServiceApiText(const std::filesystem::path& root);
[[nodiscard]] std::string qmlText(const std::filesystem::path& root);
[[nodiscard]] std::string presenterText(const std::filesystem::path& root);
[[nodiscard]] std::string docs117Text(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool expect(bool condition, const std::string& message);
[[nodiscard]] std::vector<std::string> brokerSdkTokens();
[[nodiscard]] std::vector<std::string> brokerNetworkTokens();
[[nodiscard]] std::vector<std::string> credentialTokens();
[[nodiscard]] std::vector<std::string> realOrderIdTokens();
[[nodiscard]] std::vector<std::string> orderPlacementTokens();
[[nodiscard]] std::vector<std::string> databaseWriteTokens();
[[nodiscard]] std::vector<std::string> qmlExposureTokens();
[[nodiscard]] std::vector<std::string> presenterExposureTokens();
[[nodiscard]] std::vector<std::string> strategyExecutionTokens();
[[nodiscard]] std::vector<std::string> automaticTradingTokens();

}  // namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring
