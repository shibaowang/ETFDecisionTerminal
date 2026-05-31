#pragma once

#include "DataServiceApi/ShellAccountingBrokerOrderPort.h"

namespace etfdt::dataservice {

class ShellAccountingBrokerSandboxOrderPortScaffold final : public ShellAccountingBrokerOrderPort {
public:
    [[nodiscard]] ShellAccountingBrokerOrderResponse submitOrderEnvelope(
        const ShellAccountingBrokerOrderRequest& request) override;
};

}  // namespace etfdt::dataservice
