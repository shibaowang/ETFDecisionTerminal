#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto service = dataServiceActionsText(root) + brokerOrderRepositoryText(root);
    const auto shell = shellBoundaryText(root);
    bool ok = expect(service.find("accounting.broker_order.dry_run") != std::string::npos,
                     "DataService dry-run action exists");
    ok &= expect(service.find("ShellAccountingBrokerOrderDryRunRepository") != std::string::npos,
                 "DataService uses dry-run repository");
    ok &= expect(shell.find("accounting.broker_order.dry_run") == std::string::npos,
                 "Shell does not bind dry-run action");
    ok &= expect(!containsAnyToken(shell, brokerSdkTokens()), "Shell has no broker SDK tokens");
    return ok ? 0 : 1;
}
