#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <optional>
#include <string>

namespace etfdt::data_access {

struct ShellAccountingTradeDraftConfirmationRequest final {
    std::int64_t draftId = 0;
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::int64_t strategyId = 0;
    std::string strategyCode;
    std::int64_t instrumentId = 0;
    std::string instrumentCode;
    std::string side;
    std::int64_t quantity1e6 = 0;
    std::string sourceSnapshotId;
    std::string sourceReplayId;
    std::string authorizationToken;
    std::string confirmedAtUtc;
};

struct ShellAccountingTradeDraftConfirmationResult final {
    std::int64_t draftId = 0;
    std::int64_t executionGroupId = 0;
    std::int64_t tradeLogId = 0;
    std::int64_t auditLogId = 0;
    std::string executionGroupUid;
    std::string tradeLogUid;
    bool executionGroupWritten = false;
    bool tradeLogWritten = false;
    bool draftStatusUpdated = false;
    bool auditWritten = false;
    bool duplicateConfirmation = false;
    bool transactionCommitted = false;
    bool idempotent = true;
};

class ShellAccountingTradeDraftConfirmationRepository final {
public:
    explicit ShellAccountingTradeDraftConfirmationRepository(SQLiteConnection& connection) noexcept;

    [[nodiscard]] DatabaseResult<ShellAccountingTradeDraftConfirmationResult> confirmTradeDraft(
        const ShellAccountingTradeDraftConfirmationRequest& request);

private:
    struct DraftRow final {
        std::int64_t id = 0;
        std::int64_t accountId = 0;
        std::int64_t portfolioId = 0;
        std::int64_t strategyId = 0;
        std::string strategyCode;
        std::string actionType;
        std::string status;
        std::int64_t totalAmountCents = 0;
        std::int64_t totalQuantity1e6 = 0;
        std::string draftSignature;
        std::string expiresAtUtc;
    };

    [[nodiscard]] DatabaseResult<DraftRow> loadDraft(
        const ShellAccountingTradeDraftConfirmationRequest& request);
    [[nodiscard]] DatabaseResult<bool> validateRequest(
        const ShellAccountingTradeDraftConfirmationRequest& request,
        const DraftRow& draft);
    [[nodiscard]] DatabaseResult<std::optional<ShellAccountingTradeDraftConfirmationResult>>
    findExistingConfirmation(const ShellAccountingTradeDraftConfirmationRequest& request);
    [[nodiscard]] DatabaseResult<std::int64_t> insertExecutionGroup(
        const ShellAccountingTradeDraftConfirmationRequest& request,
        const DraftRow& draft);
    [[nodiscard]] DatabaseResult<std::int64_t> insertTradeLog(
        const ShellAccountingTradeDraftConfirmationRequest& request,
        const DraftRow& draft,
        std::int64_t executionGroupId);
    [[nodiscard]] DatabaseResult<bool> updateDraftStatus(
        const ShellAccountingTradeDraftConfirmationRequest& request);
    [[nodiscard]] DatabaseResult<std::int64_t> insertAuditEvent(
        const ShellAccountingTradeDraftConfirmationRequest& request,
        std::int64_t executionGroupId,
        std::int64_t tradeLogId);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
