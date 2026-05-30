#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <optional>
#include <string>

namespace etfdt::data_access {

struct ShellAccountingTradeDraftCreateRequest final {
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::int64_t strategyId = 0;
    std::string strategyCode;
    std::int64_t instrumentId = 0;
    std::string instrumentCode;
    std::string side;
    std::int64_t quantity1e6 = 0;
    std::string reason;
    std::string sourceSnapshotId;
    std::string sourceReplayId;
    std::string authorizationToken;
    std::string createdAtUtc;
};

struct ShellAccountingTradeDraftCreateResult final {
    std::int64_t draftId = 0;
    std::int64_t auditLogId = 0;
    std::string draftUid;
    std::string draftSignature;
    bool draftWritten = false;
    bool auditWritten = false;
    bool duplicateDraft = false;
    bool transactionCommitted = false;
    bool idempotent = true;
};

class ShellAccountingTradeDraftRepository final {
public:
    explicit ShellAccountingTradeDraftRepository(SQLiteConnection& connection) noexcept;

    [[nodiscard]] DatabaseResult<ShellAccountingTradeDraftCreateResult> createTradeDraft(
        const ShellAccountingTradeDraftCreateRequest& request);

private:
    [[nodiscard]] DatabaseResult<bool> validateRequest(
        const ShellAccountingTradeDraftCreateRequest& request);
    [[nodiscard]] DatabaseResult<std::optional<std::int64_t>> findExistingDraft(
        const ShellAccountingTradeDraftCreateRequest& request);
    [[nodiscard]] DatabaseResult<std::int64_t> insertDraft(
        const ShellAccountingTradeDraftCreateRequest& request);
    [[nodiscard]] DatabaseResult<std::int64_t> insertAuditEvent(
        const ShellAccountingTradeDraftCreateRequest& request,
        std::int64_t draftId,
        const std::string& draftUid);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
