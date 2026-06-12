#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

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
    std::string idempotencyKey;
    std::string recommendationDigest;
    std::string sourceRecommendationAction;
    std::string sourceRecommendationReason;
    std::string instrumentType;
    std::string tradeSource = "SYSTEM";
    std::string quantityText;
    std::string amountText;
    std::string priceText;
    std::string feeEstimateText;
    std::string netCashImpactText;
    std::string currency = "CNY";
    std::string status = "ACTIVE";
    std::string expiresAtUtc;
    bool userConfirmationRequired = true;
    bool userConfirmed = false;
    bool baseProtectionPassed = true;
    bool cashLimitApplied = false;
    bool otcGenericDraft = false;
};

struct ShellAccountingTradeDraftCreateResult final {
    std::int64_t draftId = 0;
    std::int64_t auditLogId = 0;
    std::string draftUid;
    std::string draftSignature;
    bool draftWritten = false;
    bool auditWritten = false;
    bool duplicateDraft = false;
    bool idempotencyConflict = false;
    bool transactionCommitted = false;
    bool idempotent = true;
    bool legWritten = false;
    int legCount = 0;
    std::string status = "DRAFT_CREATED";
    std::string side;
    std::string instrumentCode;
    std::string quantityText;
    std::string amountText;
    std::string netCashImpactText;
    std::vector<std::string> issueCodes;
};

struct ShellAccountingTradeDraftLegCreateRequest final {
    std::int64_t instrumentId = 0;
    std::string instrumentCode;
    std::string instrumentType;
    std::string side;
    std::int64_t quantity1e6 = 0;
    std::string quantityText;
    std::string amountText;
    std::string priceText;
    std::string feeEstimateText;
    std::string netCashImpactText;
    std::string tradeSource = "OTC_REPLACEMENT_GENERIC";
    std::string sourceRecommendationReason;
    int priority = 0;
};

struct ShellAccountingTradeDraftMultiLegCreateRequest final {
    ShellAccountingTradeDraftCreateRequest draft;
    std::vector<ShellAccountingTradeDraftLegCreateRequest> legs;
};

struct ShellAccountingTradeDraftLegSummary final {
    std::string instrumentCode;
    std::string instrumentType;
    std::string side;
    std::string quantityText;
    std::string amountText;
    std::string netCashImpactText;
};

struct ShellAccountingTradeDraftSummary final {
    std::int64_t draftId = 0;
    std::string draftUid;
    std::string draftSignature;
    std::string status;
    std::string side;
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string strategyCode;
    std::string quantityText;
    std::string amountText;
    std::string netCashImpactText;
    std::string idempotencyKey;
    int legCount = 0;
    std::vector<ShellAccountingTradeDraftLegSummary> legs;
    bool found = false;
    bool readOnly = true;
};

class ShellAccountingTradeDraftRepository final {
public:
    explicit ShellAccountingTradeDraftRepository(SQLiteConnection& connection) noexcept;

    [[nodiscard]] DatabaseResult<ShellAccountingTradeDraftCreateResult> createTradeDraft(
        const ShellAccountingTradeDraftCreateRequest& request);
    [[nodiscard]] DatabaseResult<ShellAccountingTradeDraftCreateResult> createTradeDraftWithLegs(
        const ShellAccountingTradeDraftMultiLegCreateRequest& request);
    [[nodiscard]] DatabaseResult<ShellAccountingTradeDraftSummary> readTradeDraftSummary(
        const std::string& idempotencyKey,
        std::int64_t draftId = 0);

private:
    [[nodiscard]] DatabaseResult<bool> validateRequest(
        const ShellAccountingTradeDraftCreateRequest& request);
    [[nodiscard]] DatabaseResult<bool> validateMultiLegRequest(
        const ShellAccountingTradeDraftMultiLegCreateRequest& request);
    [[nodiscard]] DatabaseResult<std::optional<std::int64_t>> findExistingDraft(
        const ShellAccountingTradeDraftCreateRequest& request);
    [[nodiscard]] DatabaseResult<std::optional<std::int64_t>> findExistingDraftBySignature(
        const ShellAccountingTradeDraftCreateRequest& request,
        const std::string& signature);
    [[nodiscard]] DatabaseResult<std::optional<std::int64_t>> findExistingDraftByIdempotencyKey(
        const ShellAccountingTradeDraftCreateRequest& request);
    [[nodiscard]] DatabaseResult<std::string> loadDraftSignature(std::int64_t draftId);
    [[nodiscard]] DatabaseResult<std::int64_t> insertDraft(
        const ShellAccountingTradeDraftCreateRequest& request);
    [[nodiscard]] DatabaseResult<std::int64_t> insertDraft(
        const ShellAccountingTradeDraftCreateRequest& request,
        const std::string& signature,
        const std::string& uid);
    [[nodiscard]] DatabaseResult<std::int64_t> insertDraftLeg(
        const ShellAccountingTradeDraftCreateRequest& request,
        std::int64_t draftId);
    [[nodiscard]] DatabaseResult<std::int64_t> insertDraftLeg(
        const ShellAccountingTradeDraftCreateRequest& request,
        const ShellAccountingTradeDraftLegCreateRequest& leg,
        std::int64_t draftId,
        int legIndex,
        const std::string& draftUid);
    [[nodiscard]] DatabaseResult<std::int64_t> insertAuditEvent(
        const ShellAccountingTradeDraftCreateRequest& request,
        std::int64_t draftId,
        const std::string& draftUid);
    [[nodiscard]] DatabaseResult<std::int64_t> insertAuditEvent(
        const ShellAccountingTradeDraftCreateRequest& request,
        std::int64_t draftId,
        const std::string& draftUid,
        const std::string& signature,
        int legCount);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
