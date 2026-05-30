#include "ShellAccountingBrokerAdapterDisabledWiring.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAnyToken(dataServiceApiText(root) + qmlText(root), credentialTokens()) ? 1 : 0;
}
