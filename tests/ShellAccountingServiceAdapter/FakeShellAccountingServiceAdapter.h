#pragma once

#include "ShellServices/ShellAccountingServiceAdapter.h"

#include <functional>
#include <string>

namespace etfdt::tests::shell_accounting_service_adapter {

class FakeShellAccountingServiceAdapter final
    : public etfdt::shell_services::ShellAccountingServiceAdapter {
public:
    explicit FakeShellAccountingServiceAdapter(
        etfdt::shell_services::ShellAccountingServiceResult result = {});

    etfdt::shell_services::ShellAccountingServiceResult fetchPositionList(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;
    etfdt::shell_services::ShellAccountingServiceResult fetchCashSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;
    etfdt::shell_services::ShellAccountingServiceResult fetchPortfolioPnlSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;
    etfdt::shell_services::ShellAccountingServiceResult fetchBasePositionSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;
    etfdt::shell_services::ShellAccountingServiceResult fetchSniperPoolSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override;

    [[nodiscard]] int callCount() const noexcept;
    [[nodiscard]] const std::string& lastActionName() const noexcept;
    void setNextResult(etfdt::shell_services::ShellAccountingServiceResult result);
    void setOnFetch(std::function<void()> onFetch);

private:
    etfdt::shell_services::ShellAccountingServiceResult makeResult(
        const etfdt::shell_services::ShellAccountingServiceRequest& request,
        std::string actionName);

    etfdt::shell_services::ShellAccountingServiceResult result_;
    int callCount_ = 0;
    std::string lastActionName_;
    std::function<void()> onFetch_;
};

[[nodiscard]] etfdt::shell_services::ShellAccountingServiceRequest
makeDemoServiceRequest(std::string actionName);

[[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult
makeGuardUnavailableResult(std::string actionName, std::string status);

}  // namespace etfdt::tests::shell_accounting_service_adapter
