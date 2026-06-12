#include "DataServiceApi/DataServiceActions.h"

#include "DataAccess/ShellAccountingTradeDraftRepository.h"
#include "Protocol/Json.h"
#include "StrategyEngine/OtcMapAcMultiChannelDraftEngine.h"

#include <cmath>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace etfdt::data_service_api {
namespace {

std::string jsonString(const std::string& value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

std::int64_t parseInt64(const std::string& value)
{
    try {
        std::size_t parsed = 0;
        const auto result = std::stoll(value, &parsed, 10);
        return parsed == value.size() ? result : 0;
    } catch (...) {
        return 0;
    }
}

std::int64_t quantity1e6(const std::string& value)
{
    try {
        return static_cast<std::int64_t>(std::llround(std::stod(value) * 1000000.0));
    } catch (...) {
        return 0;
    }
}

std::string issueCodesJson(const std::vector<std::string>& issueCodes)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < issueCodes.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        stream << jsonString(issueCodes[index]);
    }
    stream << ']';
    return stream.str();
}

std::string legsJson(const std::vector<etfdt::strategy_engine::OtcMapAcMultiChannelDraftLeg>& legs)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < legs.size(); ++index) {
        const auto& leg = legs[index];
        if (index != 0U) {
            stream << ',';
        }
        stream << "{"
               << "\"instrumentId\":" << leg.instrumentId << ','
               << "\"instrumentCode\":" << jsonString(leg.instrumentCode) << ','
               << "\"fundClass\":" << jsonString(leg.fundClass) << ','
               << "\"side\":" << jsonString(leg.side) << ','
               << "\"quantityText\":" << jsonString(leg.quantityText) << ','
               << "\"amountText\":" << jsonString(leg.amountText) << ','
               << "\"navText\":" << jsonString(leg.navText) << ','
               << "\"netCashImpactText\":" << jsonString(leg.netCashImpactText) << ','
               << "\"currency\":" << jsonString(leg.currency)
               << "}";
    }
    stream << ']';
    return stream.str();
}

etfdt::protocol::ProtocolResponse successResponse(
    const etfdt::service_runtime::ActionContext& context,
    std::string payloadJson)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = true;
    response.payloadJson = std::move(payloadJson);
    return response;
}

etfdt::protocol::ProtocolResponse errorResponse(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::protocol::ErrorCode code,
    std::string message)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = false;
    response.errorCode = code;
    response.errorMessage = std::move(message);
    response.payloadJson = "{}";
    return response;
}

std::string previewPayload(
    const char* action,
    const etfdt::strategy_engine::OtcMapAcMultiChannelDraftResult& result)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonString(action) << ','
            << "\"task\":\"EPIC-279\","
            << "\"mode\":\"otcmap-ac-multichannel-draft-engine-full-delivery\","
            << "\"otcMapAcMultiChannelEngineCreated\":true,"
            << "\"dataServiceReadOnlyActionCreated\":"
            << (std::string(action) == kActionAccountingOtcMapMultiChannelReadOnlyPreview ? "true" : "false") << ','
            << "\"previewActionExecuted\":"
            << (std::string(action) == kActionAccountingOtcMapMultiChannelReadOnlyPreview ? "true" : "false") << ','
            << "\"accepted\":" << (result.accepted ? "true" : "false") << ','
            << "\"eligibleForTradeDraft\":" << (result.eligibleForTradeDraft ? "true" : "false") << ','
            << "\"status\":" << jsonString(result.status) << ','
            << "\"side\":" << jsonString(result.side) << ','
            << "\"legCount\":" << result.legs.size() << ','
            << "\"totalQuantityText\":" << jsonString(result.totalQuantityText) << ','
            << "\"totalAmountText\":" << jsonString(result.totalAmountText) << ','
            << "\"totalNetCashImpactText\":" << jsonString(result.totalNetCashImpactText) << ','
            << "\"baseProtectionPassed\":" << (result.baseProtectionPassed ? "true" : "false") << ','
            << "\"cashLimitApplied\":" << (result.cashLimitApplied ? "true" : "false") << ','
            << "\"legs\":" << legsJson(result.legs) << ','
            << "\"issueCodes\":" << issueCodesJson(result.issueCodes) << ','
            << "\"readOnlyNoWrite\":true,"
            << "\"tradeDraftCreated\":false,"
            << "\"tradeLogRowsWrittenByOtcMapDraft\":false,"
            << "\"cashAdjustmentRowsWrittenByOtcMapDraft\":false,"
            << "\"positionChangedByOtcMapDraft\":false,"
            << "\"cashChangedByOtcMapDraft\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"realOrderPlacement\":false,"
            << "\"automaticTrading\":false,"
            << "\"productionDbTouched\":false"
            << "}";
    return payload.str();
}

std::string createPayload(
    const etfdt::data_access::ShellAccountingTradeDraftCreateResult& result)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonString(kActionAccountingTradeDraftCreateOtcMapMultiChannel) << ','
            << "\"task\":\"EPIC-279\","
            << "\"mode\":\"otcmap-ac-multichannel-draft-engine-full-delivery\","
            << "\"dataServiceWriteActionCreated\":true,"
            << "\"multiLegTradeDraftCreated\":" << (result.draftWritten ? "true" : "false") << ','
            << "\"draftWritten\":" << (result.draftWritten ? "true" : "false") << ','
            << "\"legWritten\":" << (result.legWritten ? "true" : "false") << ','
            << "\"legCount\":" << result.legCount << ','
            << "\"auditWritten\":" << (result.auditWritten ? "true" : "false") << ','
            << "\"transactionCommitted\":" << (result.transactionCommitted ? "true" : "false") << ','
            << "\"duplicateDraft\":" << (result.duplicateDraft ? "true" : "false") << ','
            << "\"idempotencyConflict\":" << (result.idempotencyConflict ? "true" : "false") << ','
            << "\"duplicateSameKeyNoGrowth\":" << (result.duplicateDraft ? "true" : "false") << ','
            << "\"idempotencyConflictNoGrowth\":" << (result.idempotencyConflict ? "true" : "false") << ','
            << "\"status\":" << jsonString(result.status) << ','
            << "\"draftId\":" << result.draftId << ','
            << "\"auditLogId\":" << result.auditLogId << ','
            << "\"draftUid\":" << jsonString(result.draftUid) << ','
            << "\"draftSignature\":" << jsonString(result.draftSignature) << ','
            << "\"side\":" << jsonString(result.side) << ','
            << "\"instrumentCode\":" << jsonString(result.instrumentCode) << ','
            << "\"quantityText\":" << jsonString(result.quantityText) << ','
            << "\"amountText\":" << jsonString(result.amountText) << ','
            << "\"netCashImpactText\":" << jsonString(result.netCashImpactText) << ','
            << "\"issueCodes\":" << issueCodesJson(result.issueCodes) << ','
            << "\"tradeLogRowsWrittenByOtcMapDraft\":false,"
            << "\"cashAdjustmentRowsWrittenByOtcMapDraft\":false,"
            << "\"positionChangedByOtcMapDraft\":false,"
            << "\"cashChangedByOtcMapDraft\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"realOrderPlacement\":false,"
            << "\"automaticTrading\":false,"
            << "\"productionDbTouched\":false"
            << "}";
    return payload.str();
}

std::string rejectedCreatePayload(
    const etfdt::strategy_engine::OtcMapAcMultiChannelDraftResult& result)
{
    std::ostringstream payload;
    payload << previewPayload(kActionAccountingTradeDraftCreateOtcMapMultiChannel, result);
    auto text = payload.str();
    const auto pos = text.rfind('}');
    if (pos != std::string::npos) {
        text.insert(
            pos,
            ",\"dataServiceWriteActionCreated\":true,\"draftWritten\":false,"
            "\"legWritten\":false,\"auditWritten\":false,\"transactionCommitted\":false,"
            "\"duplicateDraft\":false,\"idempotencyConflict\":false,\"draftId\":0,\"auditLogId\":0");
    }
    return text;
}

}  // namespace

etfdt::protocol::ProtocolResponse handleAccountingOtcMapMultiChannelReadOnlyPreview(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;
    etfdt::strategy_engine::OtcMapAcMultiChannelDraftRequest request;
    std::string errorCode;
    if (!etfdt::strategy_engine::decodeOtcMapAcMultiChannelDraftRequestJson(
            context.request.payloadJson,
            request,
            errorCode)) {
        auto rejected = etfdt::strategy_engine::OtcMapAcMultiChannelDraftResult {};
        rejected.issueCodes.push_back(errorCode);
        rejected.issues.push_back({errorCode, "payload", "OTCMap payload could not be parsed.", true});
        return successResponse(
            context,
            previewPayload(kActionAccountingOtcMapMultiChannelReadOnlyPreview, rejected));
    }

    const auto result =
        etfdt::strategy_engine::OtcMapAcMultiChannelDraftEngine{}.buildReadOnlyDraft(request);
    return successResponse(
        context,
        previewPayload(kActionAccountingOtcMapMultiChannelReadOnlyPreview, result));
}

etfdt::protocol::ProtocolResponse handleAccountingTradeDraftCreateOtcMapMultiChannel(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::strategy_engine::OtcMapAcMultiChannelDraftRequest request;
    std::string errorCode;
    if (!etfdt::strategy_engine::decodeOtcMapAcMultiChannelDraftRequestJson(
            context.request.payloadJson,
            request,
            errorCode)) {
        return errorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "OTCMap multi-channel TradeDraft payload must be a JSON object");
    }

    const auto result =
        etfdt::strategy_engine::OtcMapAcMultiChannelDraftEngine{}.buildReadOnlyDraft(request);
    if (!request.userConfirmed) {
        auto rejected = result;
        rejected.status = "REJECTED";
        rejected.accepted = false;
        rejected.eligibleForTradeDraft = false;
        rejected.issueCodes.push_back("OTCMAP_USER_CONFIRMATION_REQUIRED");
        return successResponse(context, rejectedCreatePayload(rejected));
    }
    if (!result.eligibleForTradeDraft) {
        return successResponse(context, rejectedCreatePayload(result));
    }

    etfdt::data_access::ShellAccountingTradeDraftMultiLegCreateRequest create;
    create.draft.accountId = parseInt64(request.accountId);
    create.draft.portfolioId = parseInt64(request.portfolioId);
    create.draft.strategyId = parseInt64(request.strategyId);
    create.draft.strategyCode = request.strategyCode;
    create.draft.instrumentId = result.legs.front().instrumentId;
    create.draft.instrumentCode = "OTCMAP_MULTI";
    create.draft.instrumentType = result.legs.front().fundClass;
    create.draft.side = result.side;
    create.draft.quantity1e6 = quantity1e6(result.totalQuantityText);
    create.draft.quantityText = result.totalQuantityText;
    create.draft.amountText = result.totalAmountText;
    create.draft.priceText = "0.00";
    create.draft.feeEstimateText = "0.00";
    create.draft.netCashImpactText = result.totalNetCashImpactText;
    create.draft.tradeSource = "OTC_REPLACEMENT_GENERIC";
    create.draft.reason = "EPIC-279 OTCMap A/C multi-channel TradeDraft create";
    create.draft.authorizationToken = "EPIC-279_OTCMAP_MULTICHANNEL_TRADEDRAFT_WRITE";
    create.draft.createdAtUtc = context.request.timestampUtc;
    create.draft.idempotencyKey = request.idempotencyKey;
    create.draft.recommendationDigest = request.otcMapDigest;
    create.draft.sourceRecommendationAction = result.side;
    create.draft.sourceRecommendationReason = "OTCMAP_AC_MULTICHANNEL";
    create.draft.userConfirmationRequired = true;
    create.draft.userConfirmed = true;
    create.draft.baseProtectionPassed = result.baseProtectionPassed;
    create.draft.cashLimitApplied = result.cashLimitApplied;
    create.draft.otcGenericDraft = true;

    for (const auto& leg : result.legs) {
        etfdt::data_access::ShellAccountingTradeDraftLegCreateRequest repoLeg;
        repoLeg.instrumentId = leg.instrumentId;
        repoLeg.instrumentCode = leg.instrumentCode;
        repoLeg.instrumentType = leg.fundClass;
        repoLeg.side = leg.side;
        repoLeg.quantity1e6 = quantity1e6(leg.quantityText);
        repoLeg.quantityText = leg.quantityText;
        repoLeg.amountText = leg.amountText;
        repoLeg.priceText = leg.navText;
        repoLeg.feeEstimateText = "0.00";
        repoLeg.netCashImpactText = leg.netCashImpactText;
        repoLeg.tradeSource = "OTC_REPLACEMENT_GENERIC";
        repoLeg.sourceRecommendationReason = "OTCMAP_AC_" + leg.fundClass;
        repoLeg.priority = leg.priority;
        create.legs.push_back(std::move(repoLeg));
    }

    etfdt::data_access::ShellAccountingTradeDraftRepository repository(connection);
    auto write = repository.createTradeDraftWithLegs(create);
    if (!write) {
        return errorResponse(context, write.error().errorCode, write.error().message);
    }
    return successResponse(context, createPayload(write.value()));
}

}  // namespace etfdt::data_service_api
