#pragma once

#include "DataServiceApi/ShellAccountingBrokerOrderPort.h"

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_broker_adapter_interface {

class TestOnlyFakeShellAccountingBrokerOrderPort final : public etfdt::dataservice::ShellAccountingBrokerOrderPort {
public:
    [[nodiscard]] etfdt::dataservice::ShellAccountingBrokerOrderResponse submitOrderEnvelope(
        const etfdt::dataservice::ShellAccountingBrokerOrderRequest& request) override;

    int callCount = 0;
    etfdt::dataservice::ShellAccountingBrokerOrderResponse nextResponse;
};

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::string joinFiles(const std::vector<std::filesystem::path>& files);
[[nodiscard]] std::string brokerInterfaceText(const std::filesystem::path& root);
[[nodiscard]] std::string qmlText(const std::filesystem::path& root);
[[nodiscard]] std::string shellServicesText(const std::filesystem::path& root);
[[nodiscard]] std::string docs114Text(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens);
[[nodiscard]] bool expect(bool condition, const std::string& message);
[[nodiscard]] etfdt::dataservice::ShellAccountingBrokerOrderRequest validRequest();

}  // namespace etfdt::tests::shell_accounting_broker_adapter_interface
