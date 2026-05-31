#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"
#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    auto& provider = defaultShellAccountingBrokerOrderPort();
    const auto response = provider.submitOrderEnvelope({});
    const auto dataServiceApi = dataServiceApiText(sourceRoot(argc, argv));
    return expect(response.status == ShellAccountingBrokerOrderStatus::Disabled, "default provider must remain disabled") &&
            expect(response.errorCode == "BROKER_ORDER_DISABLED", "default provider must preserve disabled error code") &&
            expect(response.brokerMode == "disabled", "default provider must preserve disabled broker mode") &&
            expect(dataServiceApi.find("static DisabledShellAccountingBrokerOrderPort disabledPort") != std::string::npos,
                   "default provider must still instantiate the disabled port")
        ? 0
        : 1;
}
