#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "SpyShellAccountingDataServiceClientPort.h"

#include <iostream>
#include <memory>
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

}  // namespace

int main()
{
    using etfdt::shell_services::ShellAccountingDataServiceAdapter;
    using etfdt::shell_services::tests::SpyShellAccountingDataServiceClientPort;
    using etfdt::shell_services::tests::makeLiveCallSkeletonRequest;
    using etfdt::shell_services::tests::makeOkClientResponse;

    auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
    spy->setNextResponse(makeOkClientResponse("position.list"));
    ShellAccountingDataServiceAdapter adapter(spy);

    const auto result = adapter.fetchPositionList(makeLiveCallSkeletonRequest("position.list"));

    expectTrue(!result.generatedTradeDraft, "adapter result does not generate TradeDraft");
    expectTrue(!result.generatedTradeSuggestion, "adapter result does not generate trade suggestion");
    expectTrue(!result.strategyExecuted, "adapter result does not execute strategy");
    expectTrue(!result.brokerOrderSubmitted, "adapter result does not submit broker order");
    expectTrue(!spy->forbiddenActionCalled(), "adapter does not call forbidden spy methods");
    expectTrue(!spy->generatedTradeDraft(), "spy did not generate TradeDraft");
    expectTrue(!spy->generatedTradeSuggestion(), "spy did not generate trade suggestion");
    expectTrue(!spy->strategyExecuted(), "spy did not execute strategy");
    expectTrue(!spy->brokerOrderSubmitted(), "spy did not submit broker order");

    spy->dataAuditAppend();
    expectTrue(spy->forbiddenActionCalled(), "spy can detect forbidden data.audit.append violation");

    return gFailures == 0 ? 0 : 1;
}
