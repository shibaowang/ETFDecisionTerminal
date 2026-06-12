#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::strategy_engine {

struct OtcMapAcChannel final {
    std::int64_t instrumentId = 0;
    std::string instrumentCode;
    std::string fundClass;
    bool enabled = false;
    bool supportsBuy = false;
    bool supportsSell = false;
    int priority = 0;
    std::string dailyBuyLimitText;
    std::string dailySellLimitText;
    std::string minBuyAmountText;
    std::string minSellAmountText;
    std::string navText;
    std::string holdingQuantityText;
    std::string holdingCostText;
    std::string currency = "CNY";
};

struct OtcMapAcMultiChannelDraftRequest final {
    std::string accountId;
    std::string portfolioId;
    std::string strategyId;
    std::string strategyCode;
    std::string side;
    std::string targetAmountText;
    std::string cashBalanceText;
    std::string requestedSellQuantityText;
    std::string principalBaseText;
    std::string holdingCostText;
    std::string basePositionRatioText = "0.20";
    std::string idempotencyKey;
    std::string otcMapDigest;
    std::string userNote;
    bool userConfirmed = false;
    std::vector<OtcMapAcChannel> channels;
};

struct OtcMapAcMultiChannelIssue final {
    std::string code;
    std::string field;
    std::string message;
    bool blocking = true;
};

struct OtcMapAcMultiChannelDraftLeg final {
    std::int64_t instrumentId = 0;
    std::string instrumentCode;
    std::string fundClass;
    std::string side;
    std::string quantityText;
    std::string amountText;
    std::string navText;
    std::string netCashImpactText;
    std::string currency = "CNY";
    int priority = 0;
};

struct OtcMapAcMultiChannelDraftResult final {
    std::string status = "REJECTED";
    std::string side;
    std::string totalQuantityText = "0";
    std::string totalAmountText = "0.00";
    std::string totalNetCashImpactText = "0.00";
    bool accepted = false;
    bool eligibleForTradeDraft = false;
    bool baseProtectionPassed = true;
    bool cashLimitApplied = false;
    bool readOnlyNoWrite = true;
    bool brokerOrderSubmitted = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool automaticTrading = false;
    std::vector<OtcMapAcMultiChannelDraftLeg> legs;
    std::vector<OtcMapAcMultiChannelIssue> issues;
    std::vector<std::string> issueCodes;
};

class OtcMapAcMultiChannelDraftEngine final {
public:
    [[nodiscard]] OtcMapAcMultiChannelDraftResult buildReadOnlyDraft(
        const OtcMapAcMultiChannelDraftRequest& request) const;
};

[[nodiscard]] bool decodeOtcMapAcMultiChannelDraftRequestJson(
    const std::string& payloadJson,
    OtcMapAcMultiChannelDraftRequest& request,
    std::string& errorCode);

}  // namespace etfdt::strategy_engine
