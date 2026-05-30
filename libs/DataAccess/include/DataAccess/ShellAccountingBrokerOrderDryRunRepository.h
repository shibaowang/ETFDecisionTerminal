#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <optional>
#include <string>

namespace etfdt::data_access {

struct ShellAccountingBrokerOrderDryRunRequest final {
    std::int64_t draftId = 0;
    std::int64_t tradeLogId = 0;
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::string source;
    std::string authorizationToken;
    bool userConfirmed = false;
    std::string requestedAtUtc;
};

struct ShellAccountingBrokerOrderDryRunResult final {
    std::int64_t draftId = 0;
    std::int64_t executionGroupId = 0;
    std::int64_t tradeLogId = 0;
    std::int64_t auditLogId = 0;
    bool auditWritten = false;
    bool duplicateDryRun = false;
    bool transactionCommitted = false;
    bool idempotent = true;
};

class ShellAccountingBrokerOrderDryRunRepository final {
public:
    explicit ShellAccountingBrokerOrderDryRunRepository(SQLiteConnection& connection) noexcept;

    [[nodiscard]] DatabaseResult<ShellAccountingBrokerOrderDryRunResult> dryRunBrokerOrder(
        const ShellAccountingBrokerOrderDryRunRequest& request);

    struct ConfirmedTradeRow final {
        std::int64_t draftId = 0;
        std::int64_t accountId = 0;
        std::int64_t portfolioId = 0;
        std::string draftStatus;
        std::int64_t executionGroupId = 0;
        std::int64_t tradeLogId = 0;
        std::string side;
        std::int64_t quantity1e6 = 0;
        std::string instrumentCode;
    };

private:
    [[nodiscard]] DatabaseResult<ConfirmedTradeRow> loadConfirmedTrade(
        const ShellAccountingBrokerOrderDryRunRequest& request);
    [[nodiscard]] DatabaseResult<bool> validateRequest(
        const ShellAccountingBrokerOrderDryRunRequest& request,
        const ConfirmedTradeRow& trade);
    [[nodiscard]] DatabaseResult<std::optional<std::int64_t>> findExistingDryRunAudit(
        const ConfirmedTradeRow& trade);
    [[nodiscard]] DatabaseResult<std::int64_t> insertAuditEvent(
        const ShellAccountingBrokerOrderDryRunRequest& request,
        const ConfirmedTradeRow& trade);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
