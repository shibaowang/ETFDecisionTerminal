#pragma once

#include "AccountingEngine/AccountingReplayDtos.h"

#include <string>
#include <vector>

namespace etfdt::data_service_api {

struct ShellAccountingExcelVbaImportSheet {
    std::string sheetName;
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
};

struct ShellAccountingExcelVbaImportPayload {
    std::vector<ShellAccountingExcelVbaImportSheet> sheets;
    bool rawUserDataExposed = false;
};

struct ShellAccountingExcelVbaImportDiagnostic {
    std::string level;
    std::string code;
    std::string field;
    std::string sheetName;
    std::string rowId;
};

struct ShellAccountingExcelVbaImportReadOnlyParseResult {
    bool accepted = false;
    bool parserReadOnly = true;
    bool parserNoSqliteAccess = true;
    bool parserNoWrite = true;
    bool productionRuntimeIntegrationImplemented = false;
    bool rawUserDataExposed = false;
    int configFactCount = 0;
    int strategyFactCount = 0;
    int skippedRows = 0;
    int sensitiveHeadersIgnored = 0;
    std::vector<ShellAccountingExcelVbaImportDiagnostic> diagnostics;
    std::vector<etfdt::accounting::TradeFactDto> tradeFacts;
    std::vector<etfdt::accounting::CashFactDto> cashFacts;
    std::vector<etfdt::accounting::MarketPriceFactDto> marketPriceFacts;
    std::vector<etfdt::accounting::FxRateFactDto> fxRateFacts;
};

[[nodiscard]] ShellAccountingExcelVbaImportReadOnlyParseResult
parseShellAccountingExcelVbaImportReadOnly(const ShellAccountingExcelVbaImportPayload& payload);

}  // namespace etfdt::data_service_api
