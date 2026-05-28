#include "ShellAccountingDataServiceAdapterExpectedCall.h"
#include "SpyAccountingDataServiceClient.h"

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

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding;

    const auto calls = expectedReadOnlyAdapterCalls();
    expectTrue(calls[0].fetchMethod == "fetchPositionList", "fetchPositionList descriptor exists");
    expectTrue(calls[0].wrapperMethod == "positionList", "fetchPositionList maps to positionList");
    expectTrue(calls[1].fetchMethod == "fetchCashSummary", "fetchCashSummary descriptor exists");
    expectTrue(calls[1].wrapperMethod == "cashSummary", "fetchCashSummary maps to cashSummary");
    expectTrue(calls[2].fetchMethod == "fetchPortfolioPnlSummary", "fetchPortfolioPnlSummary descriptor exists");
    expectTrue(calls[2].wrapperMethod == "portfolioPnlSummary", "fetchPortfolioPnlSummary maps to portfolioPnlSummary");
    expectTrue(calls[3].fetchMethod == "fetchBasePositionSummary", "fetchBasePositionSummary descriptor exists");
    expectTrue(calls[3].wrapperMethod == "basePositionSummary", "fetchBasePositionSummary maps to basePositionSummary");
    expectTrue(calls[4].fetchMethod == "fetchSniperPoolSummary", "fetchSniperPoolSummary descriptor exists");
    expectTrue(calls[4].wrapperMethod == "sniperPoolSummary", "fetchSniperPoolSummary maps to sniperPoolSummary");

    SpyAccountingDataServiceClient spy;
    (void)spy.positionList("{}", 100);
    (void)spy.cashSummary("{}", 100);
    (void)spy.portfolioPnlSummary("{}", 100);
    (void)spy.basePositionSummary("{}", 100);
    (void)spy.sniperPoolSummary("{}", 100);

    expectTrue(spy.calledWrappers().size() == 5, "five read-only wrapper calls are recorded");
    expectTrue(spy.calledWrappers()[0] == "positionList", "positionList wrapper recorded");
    expectTrue(spy.calledWrappers()[1] == "cashSummary", "cashSummary wrapper recorded");
    expectTrue(spy.calledWrappers()[2] == "portfolioPnlSummary", "portfolioPnlSummary wrapper recorded");
    expectTrue(spy.calledWrappers()[3] == "basePositionSummary", "basePositionSummary wrapper recorded");
    expectTrue(spy.calledWrappers()[4] == "sniperPoolSummary", "sniperPoolSummary wrapper recorded");
    expectTrue(!spy.forbiddenActionCalled(), "read-only wrapper calls do not call forbidden methods");
    expectTrue(!spy.writeActionCalled(), "read-only wrapper calls do not write");

    return gFailures == 0 ? 0 : 1;
}
