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

    SpyAccountingDataServiceClient readOnlySpy;
    (void)readOnlySpy.positionList("{}", 100);
    (void)readOnlySpy.cashSummary("{}", 100);
    (void)readOnlySpy.portfolioPnlSummary("{}", 100);
    (void)readOnlySpy.basePositionSummary("{}", 100);
    (void)readOnlySpy.sniperPoolSummary("{}", 100);
    expectTrue(!readOnlySpy.writeActionCalled(), "read-only wrappers do not write");
    expectTrue(!readOnlySpy.generatedTradeDraft(), "read-only wrappers do not generate TradeDraft");
    expectTrue(!readOnlySpy.generatedTradeSuggestion(), "read-only wrappers do not generate trade suggestion");
    expectTrue(!readOnlySpy.strategyExecuted(), "read-only wrappers do not execute strategy");
    expectTrue(!readOnlySpy.brokerOrderSubmitted(), "read-only wrappers do not submit broker order");
    expectTrue(!readOnlySpy.forbiddenActionCalled(), "read-only wrappers do not call forbidden actions");

    SpyAccountingDataServiceClient forbiddenSpy;
    forbiddenSpy.dataAuditAppend();
    expectTrue(forbiddenSpy.forbiddenActionCalled(), "data.audit.append violation is detected");
    expectTrue(forbiddenSpy.writeActionCalled(), "write action violation is detected");

    SpyAccountingDataServiceClient draftSpy;
    draftSpy.tradeDraftCreate();
    expectTrue(draftSpy.generatedTradeDraft(), "TradeDraft violation is detected");

    SpyAccountingDataServiceClient snapshotSpy;
    snapshotSpy.snapshotWrite();
    expectTrue(snapshotSpy.writeActionCalled(), "snapshot write violation is detected");

    SpyAccountingDataServiceClient strategySpy;
    strategySpy.strategyExecute();
    expectTrue(strategySpy.strategyExecuted(), "strategy execution violation is detected");
    expectTrue(strategySpy.generatedTradeSuggestion(), "trade suggestion violation is detected");

    SpyAccountingDataServiceClient brokerSpy;
    brokerSpy.brokerOrderSubmit();
    expectTrue(brokerSpy.brokerOrderSubmitted(), "broker order violation is detected");

    return gFailures == 0 ? 0 : 1;
}
