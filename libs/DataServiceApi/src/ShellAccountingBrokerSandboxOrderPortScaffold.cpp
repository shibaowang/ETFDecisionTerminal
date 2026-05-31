#include "DataServiceApi/ShellAccountingBrokerSandboxOrderPortScaffold.h"

namespace etfdt::dataservice {

ShellAccountingBrokerOrderResponse ShellAccountingBrokerSandboxOrderPortScaffold::submitOrderEnvelope(
    const ShellAccountingBrokerOrderRequest& request)
{
    ShellAccountingBrokerOrderResponse response;
    response.status = ShellAccountingBrokerOrderStatus::Error;
    response.success = false;
    response.disabled = true;
    response.dryRunOnly = true;
    response.errorCode = "BROKER_SANDBOX_NOT_CONFIGURED";
    response.sanitizedMessage = request.idempotencyKey.empty()
        ? "Sandbox broker adapter is not configured."
        : "Sandbox broker adapter is not configured for the requested envelope.";
    response.brokerMode = "sandbox";
    response.sanitizedCorrelationId.clear();
    return response;
}

}  // namespace etfdt::dataservice
