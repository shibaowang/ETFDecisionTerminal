#pragma once

#include "DataAccess/DataAccess.h"

#include <string>
#include <string_view>
#include <vector>

namespace etfdt::data_service_api {

[[nodiscard]] std::string jsonStringField(
    std::string_view key,
    std::string_view value,
    bool trailingComma);

[[nodiscard]] std::string accountToJson(const etfdt::data_access::AccountRecord& record);
[[nodiscard]] std::string portfolioToJson(const etfdt::data_access::PortfolioRecord& record);
[[nodiscard]] std::string instrumentToJson(const etfdt::data_access::InstrumentRecord& record);
[[nodiscard]] std::string strategyToJson(const etfdt::data_access::StrategyRecord& record);
[[nodiscard]] std::string otcChannelToJson(const etfdt::data_access::OtcChannelRecord& record);

[[nodiscard]] std::string accountsToJsonArray(
    const std::vector<etfdt::data_access::AccountRecord>& records);
[[nodiscard]] std::string portfoliosToJsonArray(
    const std::vector<etfdt::data_access::PortfolioRecord>& records);
[[nodiscard]] std::string instrumentsToJsonArray(
    const std::vector<etfdt::data_access::InstrumentRecord>& records);
[[nodiscard]] std::string strategiesToJsonArray(
    const std::vector<etfdt::data_access::StrategyRecord>& records);
[[nodiscard]] std::string otcChannelsToJsonArray(
    const std::vector<etfdt::data_access::OtcChannelRecord>& records);

}  // namespace etfdt::data_service_api
