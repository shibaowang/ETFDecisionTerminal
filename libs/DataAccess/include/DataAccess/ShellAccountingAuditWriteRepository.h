#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <optional>
#include <string>

namespace etfdt::data_access {

struct ShellAccountingAuditWriteRequest final {
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::string snapshotBatchId;
    std::string sourceAction = "accounting.snapshot.write";
    std::string sourceStatus = "OK";
    std::string authorizationToken;
    std::string sanitizedIssueCode;
    int cashRows = 0;
    int positionRows = 0;
    int portfolioSummaryRows = 0;
    std::string createdAtUtc;
};

struct ShellAccountingAuditWriteResult final {
    std::int64_t auditLogId = 0;
    bool auditWritten = false;
    bool duplicateEvent = false;
    bool transactionCommitted = false;
    bool idempotent = true;
};

class ShellAccountingAuditWriteRepository final {
public:
    explicit ShellAccountingAuditWriteRepository(SQLiteConnection& connection) noexcept;

    [[nodiscard]] DatabaseResult<ShellAccountingAuditWriteResult> writeSnapshotWriteAudit(
        const ShellAccountingAuditWriteRequest& request);

private:
    [[nodiscard]] DatabaseResult<std::optional<std::int64_t>> findExistingAuditEvent(
        const ShellAccountingAuditWriteRequest& request);
    [[nodiscard]] DatabaseResult<std::int64_t> insertAuditEvent(
        const ShellAccountingAuditWriteRequest& request);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
