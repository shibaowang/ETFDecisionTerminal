#include "DataServiceApi/ShellAccountingBrokerOrderPort.h"

namespace etfdt::dataservice {

ShellAccountingBrokerOrderResponse DisabledShellAccountingBrokerOrderPort::submitOrderEnvelope(
    const ShellAccountingBrokerOrderRequest& request)
{
    ShellAccountingBrokerOrderResponse response;
    response.status = ShellAccountingBrokerOrderStatus::Disabled;
    response.success = false;
    response.disabled = true;
    response.dryRunOnly = true;
    response.errorCode = "BROKER_ORDER_DISABLED";
    response.sanitizedMessage = request.idempotencyKey.empty()
        ? "Broker order placement is disabled."
        : "Broker order placement is disabled for the requested envelope.";
    response.brokerMode = "disabled";
    response.sanitizedCorrelationId.clear();
    return response;
}

}  // namespace etfdt::dataservice
