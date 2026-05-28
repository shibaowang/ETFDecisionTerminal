#include "SpyAccountingDataServiceClient.h"

#include <iostream>
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

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding;

    std::vector<SpyProtocolResponse> responses;
    SpyProtocolResponse malformed;
    malformed.malformed = true;
    responses.push_back(malformed);

    SpyProtocolResponse protocolFailure;
    protocolFailure.success = false;
    responses.push_back(protocolFailure);

    SpyProtocolResponse timeout;
    timeout.timeout = true;
    responses.push_back(timeout);

    SpyProtocolResponse transport;
    transport.transportError = true;
    responses.push_back(transport);

    SpyProtocolResponse unavailableServer;
    unavailableServer.serverUnavailable = true;
    responses.push_back(unavailableServer);

    for (const auto& response : responses) {
        const auto mapped = mapSpyProtocolResponse(response);
        expectTrue(!mapped.protocolSuccess, "protocol/transport failure maps to protocolSuccess=false");
        expectTrue(!mapped.generatedTradeDraft, "failure does not generate TradeDraft");
        expectTrue(!mapped.generatedTradeSuggestion, "failure does not generate trade suggestion");
        expectTrue(!mapped.strategyExecuted, "failure does not execute strategy");
        expectTrue(!mapped.brokerOrderSubmitted, "failure does not submit broker order");
    }

    const std::vector<std::string> unavailableStatuses{
        "POSITION_LIST_NOT_AVAILABLE",
        "CASH_SUMMARY_NOT_AVAILABLE",
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE",
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE",
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE",
    };
    for (const auto& status : unavailableStatuses) {
        SpyProtocolResponse response;
        response.success = true;
        response.implemented = false;
        response.status = status;
        const auto mapped = mapSpyProtocolResponse(response);
        expectTrue(mapped.protocolSuccess, "implemented=false keeps protocolSuccess=true");
        expectTrue(mapped.payloadStatus == status, "guard unavailable status is preserved");
        expectTrue(mapped.domainError, "guard unavailable is a domain issue");
    }

    const std::vector<std::string> issueCodes{
        "MARKET_PRICE_MISSING",
        "MULTI_CURRENCY_UNSUPPORTED",
        "FX_RATE_MISSING",
        "REPLAY_NOT_AVAILABLE",
    };
    for (const auto& code : issueCodes) {
        SpyProtocolResponse response;
        response.success = true;
        response.implemented = true;
        response.issues.push_back({code, "ERROR", "blocking issue is visible", true});
        const auto mapped = mapSpyProtocolResponse(response);
        expectTrue(mapped.protocolSuccess, "domain issue keeps protocolSuccess=true");
        expectTrue(mapped.issues.front().code == code, "domain issue code is preserved");
        expectTrue(mapped.issues.front().blocking, "blocking issue remains visible");
    }

    return gFailures == 0 ? 0 : 1;
}
