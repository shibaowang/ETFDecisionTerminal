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

[[nodiscard]] std::string auditAppendPayloadJson(const AuditAppendRequest& request);

[[nodiscard]] std::string excelVbaImportPersistManualEntryPayloadJson(
    const ExcelVbaImportPersistManualEntryRequest& request);

[[nodiscard]] std::string portfolioReplayReadOnlySummaryPayloadJson(
    const PortfolioReplayReadOnlySummaryRequest& request);

}  // namespace etfdt::data_service_client
