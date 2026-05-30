#include "ShellAccountingBrokerAdapterDisabledWiring.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    return containsAnyToken(presenterText(sourceRoot(argc, argv)), presenterExposureTokens()) ? 1 : 0;
}
