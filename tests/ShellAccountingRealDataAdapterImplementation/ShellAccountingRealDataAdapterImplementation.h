#pragma once

#include "ShellServices/ShellAccountingServiceAdapter.h"

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_real_data_adapter_implementation {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::vector<std::filesystem::path> productionStartupFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> presenterCoreFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> adapterBoundaryFiles(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path docs80Path(const std::filesystem::path& root);
[[nodiscard]] bool containsTokenInFiles(
    const std::vector<std::filesystem::path>& files,
    const std::string& token);
[[nodiscard]] int countTokenInFiles(
    const std::vector<std::filesystem::path>& files,
    const std::string& token);
[[nodiscard]] std::vector<std::string> allowedReadOnlyActions();
[[nodiscard]] std::vector<std::string> forbiddenWriteTradeTokens();
[[nodiscard]] std::vector<std::string> sqliteDirectTokens();
[[nodiscard]] std::vector<std::string> accountingEngineDirectTokens();

class ScriptedAccountingServiceAdapter final
    : public etfdt::shell_services::ShellAccountingServiceAdapter {
public:
    explicit ScriptedAccountingServiceAdapter(
        etfdt::shell_services::ShellAccountingServiceResult result);

    [[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult fetchPositionList(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;
    [[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult fetchCashSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;
    [[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult fetchPortfolioPnlSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;
    [[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult fetchBasePositionSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;
    [[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult fetchSniperPoolSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;

private:
    [[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult responseFor(
        const etfdt::shell_services::ShellAccountingServiceRequest& request,
        const char* fallbackActionName) const;

    etfdt::shell_services::ShellAccountingServiceResult result_;
};

[[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult makeReadOnlySuccess(
    std::string actionName);
[[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult makeUnavailable(
    std::string actionName);
[[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult makeTransportError(
    std::string actionName);
[[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult makeProtocolError(
    std::string actionName);
[[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult makeTimeout(
    std::string actionName);

}  // namespace etfdt::tests::shell_accounting_real_data_adapter_implementation
