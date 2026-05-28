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

void expectSingleCall(
    const etfdt::shell_services::tests::SpyShellAccountingDataServiceClientPort& spy,
    std::string_view methodName)
{
    expectTrue(spy.totalCallCount() == 1, "one port method was called");
    expectTrue(spy.callCount(std::string(methodName)) == 1, "expected port method was called");
    expectTrue(!spy.forbiddenActionCalled(), "no forbidden method was called");
}

}  // namespace

int main()
{
    using etfdt::shell_services::ShellAccountingDataServiceAdapter;
    using etfdt::shell_services::tests::SpyShellAccountingDataServiceClientPort;
    using etfdt::shell_services::tests::makeLiveCallSkeletonRequest;
    using etfdt::shell_services::tests::makeOkClientResponse;

    {
        auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
        spy->setNextResponse(makeOkClientResponse("position.list"));
        ShellAccountingDataServiceAdapter adapter(spy);
        static_cast<void>(adapter.fetchPositionList(makeLiveCallSkeletonRequest("position.list")));
        expectSingleCall(*spy, "callPositionList");
    }
    {
        auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
        spy->setNextResponse(makeOkClientResponse("cash.summary"));
        ShellAccountingDataServiceAdapter adapter(spy);
        static_cast<void>(adapter.fetchCashSummary(makeLiveCallSkeletonRequest("cash.summary")));
        expectSingleCall(*spy, "callCashSummary");
    }
    {
        auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
        spy->setNextResponse(makeOkClientResponse("portfolio.pnl.summary"));
        ShellAccountingDataServiceAdapter adapter(spy);
        static_cast<void>(
            adapter.fetchPortfolioPnlSummary(makeLiveCallSkeletonRequest("portfolio.pnl.summary")));
        expectSingleCall(*spy, "callPortfolioPnlSummary");
    }
    {
        auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
        spy->setNextResponse(makeOkClientResponse("base_position.summary"));
        ShellAccountingDataServiceAdapter adapter(spy);
        static_cast<void>(
            adapter.fetchBasePositionSummary(makeLiveCallSkeletonRequest("base_position.summary")));
        expectSingleCall(*spy, "callBasePositionSummary");
    }
    {
        auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
        spy->setNextResponse(makeOkClientResponse("sniper_pool.summary"));
        ShellAccountingDataServiceAdapter adapter(spy);
        static_cast<void>(
            adapter.fetchSniperPoolSummary(makeLiveCallSkeletonRequest("sniper_pool.summary")));
        expectSingleCall(*spy, "callSniperPoolSummary");
    }

    return gFailures == 0 ? 0 : 1;
}
