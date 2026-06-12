#include "DataServiceApi/DataServiceActions.h"

#include "Protocol/Json.h"
#include "StrategyEngine/StrategyRecommendationEngine.h"

#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace etfdt::data_service_api {
namespace {

[[nodiscard]] std::string jsonStringValue(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

[[nodiscard]] std::string issueJson(
    const etfdt::strategy_engine::StrategyRecommendationIssue& issue)
{
    std::ostringstream stream;
    stream << "{"
           << "\"level\":" << jsonStringValue(issue.level) << ','
           << "\"code\":" << jsonStringValue(issue.code) << ','
           << "\"message\":" << jsonStringValue(issue.message) << ','
           << "\"field\":" << jsonStringValue(issue.field) << ','
           << "\"blocking\":" << (issue.blocking ? "true" : "false")
           << "}";
    return stream.str();
}

[[nodiscard]] std::string issuesJson(
    const std::vector<etfdt::strategy_engine::StrategyRecommendationIssue>& issues)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < issues.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        stream << issueJson(issues[index]);
    }
    stream << ']';
    return stream.str();
}

[[nodiscard]] std::string resultPayloadJson(
    const etfdt::strategy_engine::StrategyRecommendationResult& result)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonStringValue(kActionStrategyRecommendationReadOnlySummary) << ','
            << "\"task\":\"EPIC-277\","
            << "\"mode\":\"strategy-recommendation-vba-parity-readonly-full-delivery\","
            << "\"implemented\":true,"
            << "\"readOnly\":true,"
            << "\"writeEnabled\":false,"
            << "\"status\":" << jsonStringValue(result.status) << ','
            << "\"dataQualityStatus\":" << jsonStringValue(result.accepted ? "OK" : "ERROR") << ','
            << "\"hasRows\":" << (result.accepted ? "true" : "false") << ','
            << "\"accepted\":" << (result.accepted ? "true" : "false") << ','
            << "\"recommendationComputed\":" << (result.recommendationComputed ? "true" : "false") << ','
            << "\"strategyRecommendationEngineCreated\":true,"
            << "\"dataServiceReadOnlyActionCreated\":true,"
            << "\"actionCode\":" << jsonStringValue(result.actionCode) << ','
            << "\"actionLabel\":" << jsonStringValue(result.actionLabel) << ','
            << "\"sourceCode\":" << jsonStringValue(result.sourceCode) << ','
            << "\"sourceLabel\":" << jsonStringValue(result.sourceLabel) << ','
            << "\"reasonCode\":" << jsonStringValue(result.reasonCode) << ','
            << "\"reasonLabel\":" << jsonStringValue(result.reasonLabel) << ','
            << "\"tierLabel\":" << jsonStringValue(result.tierLabel) << ','
            << "\"targetAmountText\":" << jsonStringValue(result.targetAmountText) << ','
            << "\"suggestedQuantityText\":" << jsonStringValue(result.suggestedQuantityText) << ','
            << "\"suggestedAmountText\":" << jsonStringValue(result.suggestedAmountText) << ','
            << "\"netCashImpactText\":" << jsonStringValue(result.netCashImpactText) << ','
            << "\"feeText\":" << jsonStringValue(result.feeText) << ','
            << "\"baseProtectionPassed\":" << (result.baseProtectionPassed ? "true" : "false") << ','
            << "\"cashLimitApplied\":" << (result.cashLimitApplied ? "true" : "false") << ','
            << "\"issues\":" << issuesJson(result.issues) << ','
            << "\"readOnlyRecommendationNoWrite\":true,"
            << "\"tradeDraftCreated\":false,"
            << "\"tradeLogRowsWrittenByRecommendation\":false,"
            << "\"cashAdjustmentRowsWrittenByRecommendation\":false,"
            << "\"auditLogRowsWrittenByRecommendation\":false,"
            << "\"productionWrite\":false,"
            << "\"sqliteProductionWrite\":false,"
            << "\"auditWritten\":false,"
            << "\"ledgerWritten\":false,"
            << "\"snapshotWritten\":false,"
            << "\"tradeLogWritten\":false,"
            << "\"readModelPersistentWrite\":false,"
            << "\"productionDbTouched\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"automaticTrading\":false"
            << "}";
    return payload.str();
}

[[nodiscard]] std::string parseErrorPayloadJson(std::string_view code)
{
    etfdt::strategy_engine::StrategyRecommendationResult result;
    result.issues.push_back({"ERROR", std::string(code), "Recommendation payload could not be parsed.", "input", true});
    return resultPayloadJson(result);
}

[[nodiscard]] etfdt::protocol::ProtocolResponse successResponse(
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

}  // namespace

etfdt::protocol::ProtocolResponse handleStrategyRecommendationReadOnlySummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;

    etfdt::strategy_engine::StrategyRecommendationInput input;
    std::string errorCode;
    if (!etfdt::strategy_engine::decodeStrategyRecommendationInputJson(
            context.request.payloadJson,
            input,
            errorCode)) {
        return successResponse(context, parseErrorPayloadJson(errorCode));
    }

    const auto result =
        etfdt::strategy_engine::StrategyRecommendationEngine{}.recommendReadOnly(input);
    return successResponse(context, resultPayloadJson(result));
}

}  // namespace etfdt::data_service_api
