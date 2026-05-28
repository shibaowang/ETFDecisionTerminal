#include "PositionListMappingContractTestUtils.h"

#include "AccountingEngine/AccountingReplayValidation.h"

#include <regex>
#include <sstream>

namespace etfdt::tests::position_list_mapping {
namespace {

std::string captureString(const std::string& json, const std::string& key)
{
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
    std::smatch match;
    if (!std::regex_search(json, match, pattern)) {
        return {};
    }
    return match[1].str();
}

bool captureBool(const std::string& json, const std::string& key, bool fallback)
{
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(true|false)");
    std::smatch match;
    if (!std::regex_search(json, match, pattern)) {
        return fallback;
    }
    return match[1].str() == "true";
}

std::vector<std::string> captureStringArray(const std::string& json, const std::string& key)
{
    const std::regex arrayPattern("\"" + key + "\"\\s*:\\s*\\[([^\\]]*)\\]");
    std::smatch arrayMatch;
    if (!std::regex_search(json, arrayMatch, arrayPattern)) {
        return {};
    }

    std::vector<std::string> values;
    const std::string body = arrayMatch[1].str();
    const std::regex valuePattern("\"([^\"]*)\"");
    for (auto it = std::sregex_iterator(body.begin(), body.end(), valuePattern);
         it != std::sregex_iterator();
         ++it) {
        values.push_back((*it)[1].str());
    }
    return values;
}

std::string dataQualityStatusForReplayStatus(const std::string& status)
{
    if (status == etfdt::accounting::AccountingReplayStatus::Ok) {
        return "OK";
    }
    if (status == etfdt::accounting::AccountingReplayStatus::Warning) {
        return "WARNING";
    }
    if (status == etfdt::accounting::AccountingReplayStatus::Error) {
        return "ERROR";
    }
    return "UNAVAILABLE";
}

}  // namespace

MappingResult<etfdt::accounting::ReplayRequestDto> mapPositionListRequestToReplayRequestForTest(
    const std::string& jsonPayload)
{
    MappingResult<etfdt::accounting::ReplayRequestDto> result;
    const auto first = jsonPayload.find_first_not_of(" \t\r\n");
    const auto last = jsonPayload.find_last_not_of(" \t\r\n");
    if (first == std::string::npos || jsonPayload[first] != '{' || jsonPayload[last] != '}') {
        result.errorCode = "INVALID_REQUEST";
        result.errorMessage = "position.list request payload must be a JSON object";
        return result;
    }

    result.success = true;
    result.value.accountId = captureString(jsonPayload, "accountId");
    result.value.portfolioId = captureString(jsonPayload, "portfolioId");
    result.value.sourceFromTime = captureString(jsonPayload, "sourceFromTime");
    result.value.sourceToTime = captureString(jsonPayload, "sourceToTime");
    result.value.includeIssues = captureBool(jsonPayload, "includeIssues", true);
    result.value.requestedOutputs = captureStringArray(jsonPayload, "requestedOutputs");
    result.value.calculationVersion = captureString(jsonPayload, "calculationVersion");
    return result;
}

etfdt::accounting::TradeFactDto mapTradeFactRowToTradeFactDtoForTest(
    const etfdt::data_access::TradeFactRow& row)
{
    etfdt::accounting::TradeFactDto dto;
    dto.factId = row.factId;
    dto.tradeTime = row.tradeTime;
    dto.accountId = row.accountId;
    dto.portfolioId = row.portfolioId;
    dto.instrumentCode = row.instrumentCode;
    dto.action = row.action;
    dto.quantityText = row.quantityText;
    dto.priceText = row.priceText;
    dto.amountText = row.amountText;
    dto.feeText = row.feeText;
    dto.cashFlowText = row.cashFlowText;
    dto.currency = row.currency;
    dto.source = row.source;
    dto.note = row.note;
    return dto;
}

PositionListPayloadForTest mapReplayResultToPositionListResponseForTest(
    const etfdt::accounting::AccountingReplayResult& result,
    const etfdt::accounting::ReplayRequestDto& request)
{
    PositionListPayloadForTest payload;
    payload.action = "position.list";
    payload.implemented = result.implemented;
    payload.readOnly = true;
    payload.writeEnabled = false;
    payload.status = result.status;
    payload.dataQualityStatus = result.positionList.dataQualityStatus.empty()
                                    ? dataQualityStatusForReplayStatus(result.status)
                                    : result.positionList.dataQualityStatus;
    payload.positions = result.positionList.positions;
    payload.issues = result.issues;
    payload.metadata.sourceFromTime = request.sourceFromTime;
    payload.metadata.sourceToTime = request.sourceToTime;
    payload.metadata.calculationVersion = request.calculationVersion;
    payload.metadata.replayEngineVersion = "AccountingEngine DTO-only skeleton";
    payload.metadata.factsQueryVersion = "test-only-mapping-contract";
    payload.metadata.noWriteCheck = true;

    for (const auto& issue : result.issues) {
        if (issue.level == etfdt::accounting::AccountingIssueLevel::Warning) {
            payload.warnings.push_back(issue.code);
        } else if (issue.level == etfdt::accounting::AccountingIssueLevel::Error) {
            payload.errors.push_back(issue.code);
        }
    }

    return payload;
}

std::string serializePositionListPayloadForTest(const PositionListPayloadForTest& payload)
{
    std::ostringstream stream;
    stream << "{\"action\":\"" << payload.action << "\",";
    stream << "\"implemented\":" << (payload.implemented ? "true" : "false") << ',';
    stream << "\"readOnly\":" << (payload.readOnly ? "true" : "false") << ',';
    stream << "\"writeEnabled\":" << (payload.writeEnabled ? "true" : "false") << ',';
    stream << "\"status\":\"" << payload.status << "\",";
    stream << "\"dataQualityStatus\":\"" << payload.dataQualityStatus << "\",";
    stream << "\"positions\":[";
    for (std::size_t i = 0; i < payload.positions.size(); ++i) {
        if (i != 0U) {
            stream << ',';
        }
        const auto& position = payload.positions[i];
        stream << "{\"accountId\":\"" << position.accountId << "\",";
        stream << "\"portfolioId\":\"" << position.portfolioId << "\",";
        stream << "\"instrumentCode\":\"" << position.instrumentCode << "\"}";
    }
    stream << "]}";
    return stream.str();
}

bool containsIssueCode(
    const std::vector<etfdt::accounting::AccountingIssueDto>& issues,
    const std::string& code)
{
    for (const auto& issue : issues) {
        if (issue.code == code) {
            return true;
        }
    }
    return false;
}

}  // namespace etfdt::tests::position_list_mapping
