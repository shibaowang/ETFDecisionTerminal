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

    const auto requestGate = liveCallRequestMappingGate();
    for (const auto& field : {
             "accountId",
             "portfolioId",
             "sourceFromTime",
             "sourceToTime",
             "includeIssues",
             "includeDebugMetadata",
             "calculationVersion",
             "requestedOutputs",
             "timeoutMs",
         }) {
        expectTrue(contains(requestGate.requiredFields, field), "request mapping preserves required field");
    }

    for (const auto& field : {
             "writeEnabled=true",
             "TradeDraft",
             "tradeSuggestion",
             "strategyExecution",
             "brokerOrder",
         }) {
        expectTrue(contains(requestGate.forbiddenFields, field), "request mapping forbids write/trade field");
    }

    const auto responseGate = liveCallResponseMappingGate();
    for (const auto& mapping : {
             "ProtocolResponse.success -> protocolSuccess",
             "payload.implemented -> implemented",
             "payload.readOnly -> readOnly",
             "payload.writeEnabled -> writeEnabled",
             "payload.status -> payloadStatus",
             "payload.dataQualityStatus -> dataQualityStatus",
             "payload.issues -> issues",
             "payload.warnings -> warnings",
             "payload.errors -> errors",
             "raw payload -> rawPayload",
         }) {
        expectTrue(contains(responseGate.mappedFields, mapping), "response mapping gate includes mapping");
    }

    expectTrue(responseGate.implementedFalseIsDomainUnavailable, "implemented=false is domain unavailable");
    expectTrue(responseGate.preserveUnknownRawPayload, "raw payload is preserved");

    return gFailures == 0 ? 0 : 1;
}
