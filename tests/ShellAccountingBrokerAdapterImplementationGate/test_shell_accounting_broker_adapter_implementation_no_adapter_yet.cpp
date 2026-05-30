#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    const auto production = productionText(sourceRoot(argc, argv));
    return production.find("defaultShellAccountingBrokerOrderPort") != std::string::npos
            && production.find("DisabledShellAccountingBrokerOrderPort") != std::string::npos
            && !containsAnyToken(production, concreteAdapterTokens())
        ? 0
        : 1;
}
