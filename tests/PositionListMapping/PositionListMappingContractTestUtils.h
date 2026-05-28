#pragma once

#include "AccountingEngine/AccountingIssue.h"
#include "AccountingEngine/AccountingReplayDtos.h"
#include "AccountingEngine/AccountingReplayResult.h"
#include "DataAccess/AccountingTradeFactReader.h"

#include <string>
#include <vector>

namespace etfdt::tests::position_list_mapping {

inline constexpr const char* kKnownTradeLogFeeSchemaGapForTest =
    "trade_log.fee_cents NOT NULL DEFAULT 0 cannot distinguish missing fee from explicit zero fee";

template <typename T>
struct MappingResult final {
    bool success = false;
    std::string errorCode;
    std::string errorMessage;
    T value{};
};

struct PositionListMetadataForTest final {
    std::string sourceFromTime;
    std::string sourceToTime;
    std::string calculationVersion;
    std::string replayEngineVersion;
    std::string factsQueryVersion;
    bool noWriteCheck = true;
};

struct PositionListPayloadForTest final {
    std::string action;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    std::string status;
    std::string dataQualityStatus;
    std::vector<etfdt::accounting::PositionSummaryDto> positions;
    std::vector<etfdt::accounting::AccountingIssueDto> issues;
    PositionListMetadataForTest metadata;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

[[nodiscard]] MappingResult<etfdt::accounting::ReplayRequestDto>
mapPositionListRequestToReplayRequestForTest(const std::string& jsonPayload);

[[nodiscard]] etfdt::accounting::TradeFactDto mapTradeFactRowToTradeFactDtoForTest(
    const etfdt::data_access::TradeFactRow& row);

[[nodiscard]] PositionListPayloadForTest mapReplayResultToPositionListResponseForTest(
    const etfdt::accounting::AccountingReplayResult& result,
    const etfdt::accounting::ReplayRequestDto& request = {});

[[nodiscard]] std::string serializePositionListPayloadForTest(const PositionListPayloadForTest& payload);

[[nodiscard]] bool containsIssueCode(
    const std::vector<etfdt::accounting::AccountingIssueDto>& issues,
    const std::string& code);

}  // namespace etfdt::tests::position_list_mapping
