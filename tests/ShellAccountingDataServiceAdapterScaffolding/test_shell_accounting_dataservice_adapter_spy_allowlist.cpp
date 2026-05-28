#include "ShellAccountingDataServiceAdapterExpectedCall.h"

#include <iostream>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

bool contains(const std::vector<std::string>& values, std::string_view expected)
{
    for (const auto& value : values) {
        if (value == expected) {
            return true;
        }
    }
    return false;
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding;

    const auto allowlist = readOnlyActionAllowlist();
    expectTrue(allowlist.size() == 5, "allowlist contains exactly five accounting actions");
    expectTrue(contains(allowlist, "position.list"), "allowlist contains position.list");
    expectTrue(contains(allowlist, "cash.summary"), "allowlist contains cash.summary");
    expectTrue(contains(allowlist, "portfolio.pnl.summary"), "allowlist contains portfolio.pnl.summary");
    expectTrue(contains(allowlist, "base_position.summary"), "allowlist contains base_position.summary");
    expectTrue(contains(allowlist, "sniper_pool.summary"), "allowlist contains sniper_pool.summary");

    for (const auto& forbidden : forbiddenActionDenylist()) {
        expectTrue(!contains(allowlist, forbidden), "allowlist excludes forbidden action");
        expectTrue(!isReadOnlyAccountingActionAllowed(forbidden), "forbidden action is not allowed");
    }

    for (const auto& call : expectedReadOnlyAdapterCalls()) {
        expectTrue(call.readOnly, "every allowlisted descriptor is read-only");
        expectTrue(!call.wrapperMethod.empty(), "every allowlisted action maps to wrapper name");
        expectTrue(isReadOnlyAccountingActionAllowed(call.actionName), "descriptor action is allowlisted");
    }

    return gFailures == 0 ? 0 : 1;
}
