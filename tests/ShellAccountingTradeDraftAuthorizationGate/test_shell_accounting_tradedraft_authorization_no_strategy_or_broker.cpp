#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = dataServiceBoundaryText(sourceRoot(argc, argv)) + shellBoundaryText(sourceRoot(argc, argv));
    return containsAnyToken(text, strategyOrBrokerTokens()) ? 1 : 0;
}
