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

    SpyProtocolResponse response;
    response.success = true;
    response.action = "position.list";
    response.implemented = false;
    response.readOnly = true;
    response.writeEnabled = false;
    response.status = "POSITION_LIST_NOT_AVAILABLE";
    response.dataQualityStatus = "UNAVAILABLE";
    response.rawPayload = "{\"futureOutput\":null}";
    response.issues.push_back({
        "POSITION_LIST_NOT_AVAILABLE",
        "ERROR",
        "position.list is not implemented.",
        true,
    });
    response.warnings.push_back({"MARKET_PRICE_MISSING", "WARNING", "Market missing.", false});
    response.errors.push_back({"REPLAY_NOT_AVAILABLE", "ERROR", "Replay unavailable.", true});

    const auto mapped = mapSpyProtocolResponse(response);
    expectTrue(mapped.protocolSuccess, "ProtocolResponse.success=true maps to protocolSuccess=true");
    expectTrue(mapped.actionName == "position.list", "payload.action maps to actionName");
    expectTrue(!mapped.implemented, "payload.implemented maps to implemented");
    expectTrue(mapped.readOnly, "payload.readOnly maps to readOnly");
    expectTrue(!mapped.writeEnabled, "payload.writeEnabled maps to writeEnabled");
    expectTrue(mapped.payloadStatus == "POSITION_LIST_NOT_AVAILABLE", "payload.status maps to payloadStatus");
    expectTrue(mapped.dataQualityStatus == "UNAVAILABLE", "payload.dataQualityStatus maps to dataQualityStatus");
    expectTrue(mapped.issues.front().code == "POSITION_LIST_NOT_AVAILABLE", "issues code is preserved");
    expectTrue(mapped.issues.front().level == "ERROR", "issues level is preserved");
    expectTrue(mapped.issues.front().blocking, "issues blocking is preserved");
    expectTrue(mapped.warnings.front().code == "MARKET_PRICE_MISSING", "warnings are preserved");
    expectTrue(mapped.errors.front().code == "REPLAY_NOT_AVAILABLE", "errors are preserved");
    expectTrue(mapped.rawPayload == "{\"futureOutput\":null}", "raw payload is preserved");
    expectTrue(!mapped.protocolError, "implemented=false is not protocol failure");
    expectTrue(mapped.domainError, "domain issue is retained");

    SpyProtocolResponse malformed;
    malformed.success = true;
    malformed.malformed = true;
    const auto malformedMapped = mapSpyProtocolResponse(malformed);
    expectTrue(!malformedMapped.protocolSuccess, "malformed payload maps to protocol failure");
    expectTrue(malformedMapped.protocolError, "malformed payload sets protocolError");

    return gFailures == 0 ? 0 : 1;
}
