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
    using etfdt::shell_services::tests::makePortIssue;

    etfdt::shell_services::ShellAccountingDataServiceClientResponse response;
    response.actionName = "position.list";
    response.protocolSuccess = true;
    response.implemented = false;
    response.readOnly = true;
    response.writeEnabled = false;
    response.payloadStatus = "POSITION_LIST_NOT_AVAILABLE";
    response.dataQualityStatus = "UNAVAILABLE";
    response.hasRows = false;
    response.issues.push_back(makePortIssue("POSITION_LIST_NOT_AVAILABLE", "ERROR", true));
    response.warnings.push_back(makePortIssue("MARKET_PRICE_MISSING", "WARNING", false));
    response.errors.push_back(makePortIssue("MULTI_CURRENCY_UNSUPPORTED", "ERROR", true));
    response.rawPayload = "{\"unknown\":\"preserved\"}";
    response.domainError = true;

    auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
    spy->setNextResponse(response);
    ShellAccountingDataServiceAdapter adapter(spy);

    const auto result = adapter.fetchPositionList(makeLiveCallSkeletonRequest("position.list"));
    expectTrue(result.protocolSuccess, "protocolSuccess maps");
    expectTrue(!result.implemented, "implemented maps");
    expectTrue(result.readOnly, "readOnly maps");
    expectTrue(!result.writeEnabled, "writeEnabled maps");
    expectTrue(result.payloadStatus == "POSITION_LIST_NOT_AVAILABLE", "payloadStatus maps");
    expectTrue(result.dataQualityStatus == "UNAVAILABLE", "dataQualityStatus maps");
    expectTrue(!result.hasRows, "hasRows maps");
    expectTrue(result.issues.size() == 1, "issues map");
    expectTrue(result.warnings.size() == 1, "warnings map");
    expectTrue(result.errors.size() == 1, "errors map");
    expectTrue(result.rawPayload.find("unknown") != std::string::npos, "rawPayload is preserved");
    expectTrue(result.domainError, "domainError maps");
    expectTrue(!result.generatedTradeDraft, "no TradeDraft generated");
    expectTrue(!result.generatedTradeSuggestion, "no trade suggestion generated");

    return gFailures == 0 ? 0 : 1;
}
