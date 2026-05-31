#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return expect(std::filesystem::exists(root / "docs" / "120_shell_accounting_broker_sandbox_adapter_scaffold.md"),
                  "docs/120 must exist") &&
            expect(std::filesystem::exists(root / "docs" / "121_shell_accounting_broker_sandbox_adapter_scaffold_test_plan.md"),
                   "docs/121 must exist") &&
            expect(containsAllTokens(task161DocsText(root),
                       {"TASK-161", "scaffold-only", "not a usable broker adapter", "default remains disabled",
                           "CI no-network", "no real broker order id", "no order placement", "no database write",
                           "no audit write", "no strategy execution", "no automatic trading"}),
                   "TASK-161 docs must describe scaffold-only broker boundary")
        ? 0
        : 1;
}
