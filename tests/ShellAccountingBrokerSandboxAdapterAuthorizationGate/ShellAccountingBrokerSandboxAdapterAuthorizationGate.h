#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate {

std::filesystem::path sourceRoot(int argc, char** argv);
std::string readTextFile(const std::filesystem::path& path);
std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
std::string joinFiles(const std::vector<std::filesystem::path>& files);

std::string productionText(const std::filesystem::path& root);
std::string dataServiceApiText(const std::filesystem::path& root);
std::string qmlText(const std::filesystem::path& root);
std::string presenterText(const std::filesystem::path& root);
std::string docs118Text(const std::filesystem::path& root);
std::string docs119Text(const std::filesystem::path& root);
std::string task160DocsText(const std::filesystem::path& root);

bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
bool expect(bool condition, const std::string& message);

std::vector<std::string> concreteAdapterTokens();
std::vector<std::string> brokerSdkTokens();
std::vector<std::string> brokerNetworkTokens();
std::vector<std::string> credentialValueTokens();
std::vector<std::string> realOrderPlacementTokens();
std::vector<std::string> realOrderIdStorageTokens();
std::vector<std::string> reconciliationTokens();
std::vector<std::string> cancellationCorrectionTokens();
std::vector<std::string> qmlPresenterExposureTokens();
std::vector<std::string> strategyExecutionTokens();
std::vector<std::string> automaticTradingTokens();

}  // namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate
