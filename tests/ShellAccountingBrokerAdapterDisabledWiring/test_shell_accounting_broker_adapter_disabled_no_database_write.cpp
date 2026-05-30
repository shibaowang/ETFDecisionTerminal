#include "ShellAccountingBrokerAdapterDisabledWiring.h"

#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    const auto response = etfdt::dataservice::defaultShellAccountingBrokerOrderPort().submitOrderEnvelope({});
    return !response.success && !containsAnyToken(dataServiceApiText(sourceRoot(argc, argv)), databaseWriteTokens())
        ? 0
        : 1;
}
