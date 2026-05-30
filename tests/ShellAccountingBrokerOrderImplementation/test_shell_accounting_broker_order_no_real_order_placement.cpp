#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAnyToken(dataServiceActionsText(root) + brokerOrderRepositoryText(root), realOrderPlacementTokens())
        ? 1
        : 0;
}
