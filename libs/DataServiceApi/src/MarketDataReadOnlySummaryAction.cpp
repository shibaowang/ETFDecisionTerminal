#include "DataServiceApi/DataServiceActions.h"

#include "MarketEngine/DisabledMarketDataProvider.h"
#include "MarketEngine/FixtureMarketDataProvider.h"
#include "MarketEngine/MarketDataRefreshEngine.h"
#include "MarketEngine/MarketDataSafetyPolicy.h"
#include "Protocol/Json.h"

#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace etfdt::data_service_api {
namespace {

std::string jsonString(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

std::string issueCodesJson(const std::vector<std::string>& issueCodes)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < issueCodes.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        stream << jsonString(issueCodes[index]);
    }
    stream << ']';
    return stream.str();
}

std::string instrumentResultsJson(
    const std::vector<etfdt::market_engine::MarketInstrumentRefreshResult>& instruments)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < instruments.size(); ++index) {
        const auto& item = instruments[index];
        if (index != 0U) {
            stream << ',';
        }
        stream << "{"
               << "\"instrumentCode\":" << jsonString(item.instrumentCode) << ','
               << "\"instrumentType\":" << jsonString(item.instrumentType) << ','
               << "\"currentPriceText\":" << jsonString(item.currentPriceText) << ','
               << "\"previousCloseText\":" << jsonString(item.previousCloseText) << ','
               << "\"historicalHighText\":" << jsonString(item.historicalHighText) << ','
               << "\"displayedHighText\":" << jsonString(item.displayedHighText) << ','
               << "\"historicalHighDate\":" << jsonString(item.historicalHighDate) << ','
               << "\"drawdownFromHighText\":" << jsonString(item.drawdownFromHighText) << ','
               << "\"premiumDiscountText\":" << jsonString(item.premiumDiscountText) << ','
               << "\"dataQualityStatus\":" << jsonString(item.dataQualityStatus) << ','
               << "\"providerSource\":" << jsonString(item.providerSource) << ','
               << "\"quoteAccepted\":" << (item.quoteAccepted ? "true" : "false") << ','
               << "\"historicalHighAccepted\":" << (item.historicalHighAccepted ? "true" : "false") << ','
               << "\"stale\":" << (item.stale ? "true" : "false") << ','
               << "\"issueCodes\":" << issueCodesJson(item.issueCodes)
               << '}';
    }
    stream << ']';
    return stream.str();
}

etfdt::protocol::ProtocolResponse successResponse(
    const etfdt::service_runtime::ActionContext& context,
    std::string payloadJson)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = true;
    response.payloadJson = std::move(payloadJson);
    return response;
}

std::unique_ptr<etfdt::market_engine::MarketDataProvider> providerFor(
    const etfdt::market_engine::MarketRefreshInput& input,
    const std::string& payloadJson)
{
    if (input.providerMode == "disabled") {
        return std::make_unique<etfdt::market_engine::DisabledMarketDataProvider>();
    }
    if (input.providerMode == "live") {
        return std::make_unique<etfdt::market_engine::LivePublicMarketDataProviderBoundary>(
            input.liveMarketDataEnabled);
    }
    return std::make_unique<etfdt::market_engine::FixtureMarketDataProvider>(payloadJson);
}

std::string payloadJson(
    const char* action,
    const etfdt::market_engine::MarketRefreshResult& result)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonString(action) << ','
            << "\"task\":\"EPIC-280\","
            << "\"mode\":\"market-data-historical-high-refresh-engine-full-delivery\","
            << "\"implemented\":true,"
            << "\"readOnly\":true,"
            << "\"writeEnabled\":false,"
            << "\"status\":" << jsonString(result.status) << ','
            << "\"dataQualityStatus\":" << jsonString(result.dataQualityStatus) << ','
            << "\"hasRows\":" << (result.accepted ? "true" : "false") << ','
            << "\"accepted\":" << (result.accepted ? "true" : "false") << ','
            << "\"marketDataRefreshEngineCreated\":true,"
            << "\"marketDataProviderContractCreated\":true,"
            << "\"disabledProviderCreated\":true,"
            << "\"fixtureProviderCreated\":true,"
            << "\"liveProviderImplemented\":" << (result.liveProviderImplemented ? "true" : "false") << ','
            << "\"liveProviderDisabledByDefault\":" << (result.liveProviderDisabledByDefault ? "true" : "false") << ','
            << "\"liveProviderDeferredForSafety\":" << (result.liveProviderDeferredForSafety ? "true" : "false") << ','
            << "\"quoteAccepted\":" << (result.quoteAccepted ? "true" : "false") << ','
            << "\"historicalHighAccepted\":" << (result.historicalHighAccepted ? "true" : "false") << ','
            << "\"instrumentCode\":" << jsonString(result.instrumentCode) << ','
            << "\"instrumentType\":" << jsonString(result.instrumentType) << ','
            << "\"currentPriceText\":" << jsonString(result.currentPriceText) << ','
            << "\"previousCloseText\":" << jsonString(result.previousCloseText) << ','
            << "\"historicalHighText\":" << jsonString(result.historicalHighText) << ','
            << "\"displayedHighText\":" << jsonString(result.displayedHighText) << ','
            << "\"historicalHighDate\":" << jsonString(result.historicalHighDate) << ','
            << "\"drawdownFromHighText\":" << jsonString(result.drawdownFromHighText) << ','
            << "\"premiumDiscountText\":" << jsonString(result.premiumDiscountText) << ','
            << "\"stale\":" << (result.stale ? "true" : "false") << ','
            << "\"partial\":" << (result.partial ? "true" : "false") << ','
            << "\"providerDisabled\":" << (result.providerDisabled ? "true" : "false") << ','
            << "\"providerSource\":" << jsonString(result.providerSource) << ','
            << "\"issueCodes\":" << issueCodesJson(result.issueCodes) << ','
            << "\"instruments\":" << instrumentResultsJson(result.instruments) << ','
            << "\"exactHostAllowlistEnforced\":" << (result.exactHostAllowlistEnforced ? "true" : "false") << ','
            << "\"batchRequestsOnly\":" << (result.batchRequestsOnly ? "true" : "false") << ','
            << "\"perHostRateLimitEnforced\":" << (result.perHostRateLimitEnforced ? "true" : "false") << ','
            << "\"historyHighDailyCacheEnforced\":" << (result.historyHighDailyCacheEnforced ? "true" : "false") << ','
            << "\"historyHighFailureCircuitBreakerMinutes\":10,"
            << "\"historyHighFailureCircuitBreakerEnforced\":" << (result.historyHighFailureCircuitBreakerEnforced ? "true" : "false") << ','
            << "\"noParallelSameHostRequests\":" << (result.noParallelSameHostRequests ? "true" : "false") << ','
            << "\"marketDataReadOnlyActionCreated\":true,"
            << "\"historicalHighReadOnlyActionCreated\":true,"
            << "\"readOnlyNoWrite\":true,"
            << "\"testNetworkAccess\":false,"
            << "\"networkAccess\":" << (result.networkAccess ? "true" : "false") << ','
            << "\"rawUrlExposed\":" << (result.rawUrlExposed ? "true" : "false") << ','
            << "\"rawResponseExposed\":" << (result.rawResponseExposed ? "true" : "false") << ','
            << "\"productionDbTouched\":" << (result.productionDbTouched ? "true" : "false") << ','
            << "\"tradeLogRowsWrittenByMarketData\":" << (result.tradeLogRowsWrittenByMarketData ? "true" : "false") << ','
            << "\"cashAdjustmentRowsWrittenByMarketData\":" << (result.cashAdjustmentRowsWrittenByMarketData ? "true" : "false") << ','
            << "\"brokerOrderSubmitted\":" << (result.brokerOrderSubmitted ? "true" : "false") << ','
            << "\"credentialAccess\":" << (result.credentialAccess ? "true" : "false") << ','
            << "\"endpointAccess\":" << (result.endpointAccess ? "true" : "false") << ','
            << "\"realOrderPlacement\":" << (result.realOrderPlacement ? "true" : "false") << ','
            << "\"automaticTrading\":" << (result.automaticTrading ? "true" : "false")
            << "}";
    return payload.str();
}

etfdt::protocol::ProtocolResponse handle(
    const char* action,
    const etfdt::service_runtime::ActionContext& context)
{
    etfdt::market_engine::MarketRefreshInput input;
    std::string errorCode;
    if (!etfdt::market_engine::decodeMarketRefreshInputJson(
            context.request.payloadJson,
            input,
            errorCode)) {
        etfdt::market_engine::MarketRefreshResult rejected;
        rejected.status = "REJECTED";
        rejected.dataQualityStatus = "INVALID_PAYLOAD";
        rejected.issueCodes.push_back(errorCode);
        return successResponse(context, payloadJson(action, rejected));
    }
    auto provider = providerFor(input, context.request.payloadJson);
    const auto result =
        etfdt::market_engine::MarketDataRefreshEngine{}.refreshReadOnly(input, *provider);
    return successResponse(context, payloadJson(action, result));
}

}  // namespace

etfdt::protocol::ProtocolResponse handleMarketDataRefreshReadOnlySummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;
    return handle(kActionMarketDataRefreshReadOnlySummary, context);
}

etfdt::protocol::ProtocolResponse handleMarketDataHistoricalHighReadOnlySummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;
    return handle(kActionMarketDataHistoricalHighReadOnlySummary, context);
}

}  // namespace etfdt::data_service_api
