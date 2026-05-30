#include "ShellAccountingBrokerAdapterDisabledWiring.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(docs117Text(root), {"No Real Order ID Policy"})
        && !containsAnyToken(dataServiceApiText(root), realOrderIdTokens()) ? 0 : 1;
}
