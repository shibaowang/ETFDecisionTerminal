#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold {

std::filesystem::path sourceRoot(int argc, char** argv);
std::string readTextFile(const std::filesystem::path& path);
std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
std::string joinFiles(const std::vector<std::filesystem::path>& files);

std::string scaffoldSourceText(const std::filesystem::path& root);
std::string dataServiceApiText(const std::filesystem::path& root);
std::string productionUiPathText(const std::filesystem::path& root);
std::string docs120Text(const std::filesystem::path& root);
std::string docs121Text(const std::filesystem::path& root);
std::string task161DocsText(const std::filesystem::path& root);
std::string task160DocsText(const std::filesystem::path& root);

bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
bool expect(bool condition, const std::string& message);

std::vector<std::string> brokerNetworkTokens();
std::vector<std::string> credentialValueTokens();
std::vector<std::string> credentialReadTokens();
std::vector<std::string> brokerSdkTokens();
std::vector<std::string> realOrderPlacementTokens();
std::vector<std::string> realOrderIdTokens();
std::vector<std::string> sqliteWriteTokens();
std::vector<std::string> forbiddenWriteTableTokens();
std::vector<std::string> strategyExecutionTokens();
std::vector<std::string> automaticTradingTokens();
std::vector<std::string> qmlPresenterExposureTokens();

}  // namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold
