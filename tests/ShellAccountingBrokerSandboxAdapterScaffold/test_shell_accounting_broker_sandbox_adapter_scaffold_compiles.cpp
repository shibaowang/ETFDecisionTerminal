#include "DataServiceApi/ShellAccountingBrokerSandboxOrderPortScaffold.h"
#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    ShellAccountingBrokerSandboxOrderPortScaffold scaffold;
    ShellAccountingBrokerOrderRequest request;
    request.accountId = "account";
    request.portfolioId = "portfolio";
    request.instrumentId = "instrument";
    request.side = "BUY";
    request.quantity = 1.0;
    const auto response = scaffold.submitOrderEnvelope(request);
    return expect(response.errorCode == "BROKER_SANDBOX_NOT_CONFIGURED", "sandbox scaffold must compile and return its fixed error") &&
            expect(scaffoldSourceText(root).find("ShellAccountingBrokerSandboxOrderPortScaffold") != std::string::npos,
                   "scaffold source must exist")
        ? 0
        : 1;
}
