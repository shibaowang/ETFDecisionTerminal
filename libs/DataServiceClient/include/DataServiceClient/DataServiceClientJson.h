#pragma once

#include "DataServiceClient/DataServiceClientTypes.h"

#include <string>

namespace etfdt::data_service_client {

[[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> parseProtocolResponseJson(
    const std::string& json);

[[nodiscard]] DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>
parseExcelVbaImportReadOnlyPreviewPayloadJson(const std::string& json);

[[nodiscard]] DataServiceClientResult<ExcelVbaImportPersistManualEntryResult>
parseExcelVbaImportPersistManualEntryPayloadJson(
    const std::string& json,
    bool protocolSuccess);

[[nodiscard]] DataServiceClientResult<PortfolioReplayReadOnlySummaryResult>
parsePortfolioReplayReadOnlySummaryPayloadJson(
    const std::string& json,
    bool protocolSuccess);

[[nodiscard]] DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>
parseStrategyRecommendationReadOnlySummaryPayloadJson(
    const std::string& json,
    bool protocolSuccess);

[[nodiscard]] DataServiceClientResult<TradeDraftCreateFromRecommendationResult>
parseTradeDraftCreateFromRecommendationPayloadJson(
    const std::string& json,
    bool protocolSuccess);

[[nodiscard]] DataServiceClientResult<TradeDraftReadOnlySummaryResult>
parseTradeDraftReadOnlySummaryPayloadJson(
    const std::string& json,
    bool protocolSuccess);

[[nodiscard]] DataServiceClientResult<OtcMapMultiChannelDraftResult>
parseOtcMapMultiChannelDraftPayloadJson(
    const std::string& json,
    bool protocolSuccess);

[[nodiscard]] DataServiceClientResult<MarketDataReadOnlySummaryResult>
parseMarketDataReadOnlySummaryPayloadJson(
    const std::string& json,
    bool protocolSuccess);

[[nodiscard]] std::string auditAppendPayloadJson(const AuditAppendRequest& request);

[[nodiscard]] std::string excelVbaImportPersistManualEntryPayloadJson(
    const ExcelVbaImportPersistManualEntryRequest& request);

[[nodiscard]] std::string portfolioReplayReadOnlySummaryPayloadJson(
    const PortfolioReplayReadOnlySummaryRequest& request);

[[nodiscard]] std::string strategyRecommendationReadOnlySummaryPayloadJson(
    const StrategyRecommendationReadOnlySummaryRequest& request);

[[nodiscard]] std::string tradeDraftCreateFromRecommendationPayloadJson(
    const TradeDraftCreateFromRecommendationRequest& request);

[[nodiscard]] std::string tradeDraftReadOnlySummaryPayloadJson(
    const TradeDraftReadOnlySummaryRequest& request);

[[nodiscard]] std::string otcMapMultiChannelPreviewPayloadJson(
    const OtcMapMultiChannelReadOnlyPreviewRequest& request);

[[nodiscard]] std::string otcMapTradeDraftCreatePayloadJson(
    const OtcMapTradeDraftCreateRequest& request);

[[nodiscard]] std::string marketDataReadOnlySummaryPayloadJson(
    const MarketDataReadOnlySummaryRequest& request);

}  // namespace etfdt::data_service_client
