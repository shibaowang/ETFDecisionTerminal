#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return expect(std::filesystem::exists(root / "docs" / "122_shell_accounting_broker_sandbox_mode_selector.md"),
                  "docs/122 must exist") &&
            expect(std::filesystem::exists(root / "docs" / "123_shell_accounting_broker_sandbox_mode_selector_test_plan.md"),
                   "docs/123 must exist") &&
            expect(containsAllTokens(task162DocsText(root),
                       {"TASK-162", "disabled-default selector", "not runtime broker enablement",
                           "sandbox mode only selects TASK-161 scaffold", "default provider remains disabled",
                           "unknown mode fail closed", "CI no-network", "credentials isolation",
                           "no real broker order id", "no order placement"}),
                   "TASK-162 docs must describe disabled-default selector boundary")
        ? 0
        : 1;
}
