#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string productionText(const std::filesystem::path& root);
[[nodiscard]] std::string qmlText(const std::filesystem::path& root);
[[nodiscard]] std::string presenterText(const std::filesystem::path& root);
[[nodiscard]] std::string docs115Text(const std::filesystem::path& root);
[[nodiscard]] std::string docs116Text(const std::filesystem::path& root);
[[nodiscard]] std::string docsText(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<std::string> concreteAdapterTokens();
[[nodiscard]] std::vector<std::string> brokerSdkTokens();
[[nodiscard]] std::vector<std::string> brokerNetworkTokens();
[[nodiscard]] std::vector<std::string> credentialTokens();
[[nodiscard]] std::vector<std::string> realOrderIdTokens();
[[nodiscard]] std::vector<std::string> qmlDirectBrokerTokens();
[[nodiscard]] std::vector<std::string> presenterDirectBrokerTokens();
[[nodiscard]] std::vector<std::string> strategyExecutionTokens();
[[nodiscard]] std::vector<std::string> automaticTradingTokens();

}  // namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate
