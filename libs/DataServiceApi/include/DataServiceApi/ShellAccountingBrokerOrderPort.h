#pragma once

#include <string>
#include <vector>

namespace etfdt::dataservice {

enum class ShellAccountingBrokerOrderStatus {
    Disabled,
    DryRunOnly,
    Rejected,
    AcceptedPlaceholder,
    PendingPlaceholder,
    UnknownStatePlaceholder,
    Error,
};

struct ShellAccountingBrokerOrderRequest {
    std::string accountId;
    std::string portfolioId;
    std::string instrumentId;
    std::string symbol;
    std::string side;
    double quantity = 0.0;
    double limitPrice = 0.0;
    std::string reason;
    std::string sourceTradeDraftId;
    std::string sourceLedgerId;
    std::string sourceDryRunId;
    std::string userConfirmationId;
    std::string idempotencyKey;
    bool finalUserConfirmed = false;
    std::vector<std::string> riskFlags;
};

struct ShellAccountingBrokerOrderResponse {
    ShellAccountingBrokerOrderStatus status = ShellAccountingBrokerOrderStatus::Disabled;
    bool success = false;
    bool disabled = true;
    bool dryRunOnly = true;
    std::string errorCode = "BROKER_ORDER_DISABLED";
    std::string sanitizedMessage = "Broker order placement is disabled.";
    std::string brokerMode = "disabled";
    std::string sanitizedCorrelationId;
};

class ShellAccountingBrokerOrderPort {
public:
    virtual ~ShellAccountingBrokerOrderPort() = default;

    [[nodiscard]] virtual ShellAccountingBrokerOrderResponse submitOrderEnvelope(
        const ShellAccountingBrokerOrderRequest& request) = 0;
};

class DisabledShellAccountingBrokerOrderPort final : public ShellAccountingBrokerOrderPort {
public:
    [[nodiscard]] ShellAccountingBrokerOrderResponse submitOrderEnvelope(
        const ShellAccountingBrokerOrderRequest& request) override;
};

}  // namespace etfdt::dataservice
