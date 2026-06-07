#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::data_access {

struct ShellAccountingExcelVbaImportManualEntryTradeFact final {
    std::string factId;
    std::string tradeTimeUtc;
    std::string accountCode;
    std::string portfolioCode;
    std::string instrumentCode;
    std::string side;
    std::string quantityText;
    std::string priceText;
    std::string amountText;
    std::string feeText;
    std::string currency;
    std::string source;
    std::string memo;
};

struct ShellAccountingExcelVbaImportManualEntryCashFact final {
    std::string factId;
    std::string timeUtc;
    std::string accountCode;
    std::string portfolioCode;
    std::string action;
    std::string amountText;
    std::string currency;
    std::string memo;
};

struct ShellAccountingExcelVbaImportManualEntryPersistenceRequest final {
    std::string previewStatus;
    std::string previewDigest;
    std::string idempotencyKey;
    std::string requestId;
    std::string schemaVersion;
    std::string source;
    std::string acceptedAtUtc;
    std::string importBatchLabel;
    int expectedTradeFactCount = 0;
    int expectedCashFactCount = 0;
    int expectedMarketPriceFactCount = 0;
    int expectedFxRateFactCount = 0;
    std::vector<ShellAccountingExcelVbaImportManualEntryTradeFact> tradeFacts;
    std::vector<ShellAccountingExcelVbaImportManualEntryCashFact> cashFacts;
};

struct ShellAccountingExcelVbaImportManualEntryPersistenceResult final {
    bool accepted = false;
    bool success = false;
    bool writeImplemented = true;
    bool databaseWritten = false;
    bool tradeLogWritten = false;
    bool cashAdjustmentWritten = false;
    bool auditLogWritten = false;
    bool transactionCommitted = false;
    bool idempotentReplay = false;
    bool duplicateImportPrevented = false;
    bool idempotencyConflictRejected = false;
    bool singleTransactionBoundaryUsed = true;
    bool nestedTransactionAttempted = false;
    bool productionDbTouched = false;
    int tradeLogRowsWritten = 0;
    int cashAdjustmentRowsWritten = 0;
    std::int64_t auditLogId = 0;
    std::string previewDigest;
    std::string idempotencyKey;
    std::string requestId;
    etfdt::protocol::ErrorCode errorCode = etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE;
    std::string status = "EXCEL_VBA_IMPORT_MANUAL_ENTRY_PERSISTENCE_NOT_RUN";
    std::vector<std::string> issues;
};

class ShellAccountingExcelVbaImportManualEntryPersistenceRepository final {
public:
    explicit ShellAccountingExcelVbaImportManualEntryPersistenceRepository(
        SQLiteConnection& connection) noexcept;

    [[nodiscard]] ShellAccountingExcelVbaImportManualEntryPersistenceResult persistAcceptedPreview(
        const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request);

private:
    [[nodiscard]] ShellAccountingExcelVbaImportManualEntryPersistenceResult failure(
        etfdt::protocol::ErrorCode code,
        std::string status,
        std::string issue,
        const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request) const;
    [[nodiscard]] ShellAccountingExcelVbaImportManualEntryPersistenceResult validate(
        const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request) const;
    [[nodiscard]] DatabaseResult<std::int64_t> findIdByUid(
        const char* tableName,
        const std::string& uid,
        const char* label);
    [[nodiscard]] DatabaseResult<std::int64_t> findInstrumentIdByCode(const std::string& code);
    [[nodiscard]] DatabaseResult<ShellAccountingExcelVbaImportManualEntryPersistenceResult>
        findExistingImportMarker(
            const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request);
    [[nodiscard]] DatabaseResult<std::int64_t> writeAuditMarker(
        const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request,
        int tradeLogRowsWritten,
        int cashAdjustmentRowsWritten);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
