#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    const auto text = dataServiceActionsText(sourceRoot(argc, argv));
    return containsAnyToken(text, brokerTokens()) ? 1 : 0;
}
