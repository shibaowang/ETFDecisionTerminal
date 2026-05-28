#include "ShellAccountingDataServiceAdapterLiveCallGate.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

bool contains(const std::vector<std::string>& values, const std::string& value)
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_dataservice_adapter_live_call_gate;

    const auto gate = liveCallErrorGate();

    for (const auto& status : {
             "malformed JSON",
             "ProtocolResponse.success=false",
             "timeout",
             "transport failure",
             "server unavailable",
             "implemented=false",
             "*_NOT_AVAILABLE",
             "MARKET_PRICE_MISSING",
             "MULTI_CURRENCY_UNSUPPORTED",
             "FX_RATE_MISSING",
             "REPLAY_NOT_AVAILABLE",
             "unknown issue",
         }) {
        expectTrue(contains(gate.handledStatuses, status), "error gate handles required status");
    }

    expectTrue(gate.protocolErrorDistinctFromDomainIssue, "protocol error remains distinct from domain issue");
    expectTrue(gate.warningVisible, "warning issue remains visible");
    expectTrue(gate.blockingIssueVisible, "blocking issue remains visible");

    return gFailures == 0 ? 0 : 1;
}
