#include "DataServiceApi/JsonBuilders.h"

#include "Protocol/Json.h"

#include <sstream>

namespace etfdt::data_service_api {
namespace {

std::string boolLiteral(bool value)
{
    return value ? "true" : "false";
}

std::string rawField(std::string_view key, std::string_view value, bool trailingComma)
{
    std::ostringstream stream;
    stream << '"' << key << "\": " << value;
    if (trailingComma) {
        stream << ',';
    }
    return stream.str();
}

template <typename T, typename JsonFunction>
std::string recordsToJsonArray(const std::vector<T>& records, JsonFunction toJson)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t i = 0; i < records.size(); ++i) {
        if (i != 0U) {
            stream << ',';
        }
        stream << toJson(records[i]);
    }
    stream << ']';
    return stream.str();
}

}  // namespace

std::string jsonStringField(
    std::string_view key,
    std::string_view value,
    bool trailingComma)
{
    std::ostringstream stream;
    stream << '"' << key << "\": \""
           << etfdt::protocol::escapeJsonString(value) << '"';
    if (trailingComma) {
        stream << ',';
    }
    return stream.str();
}

std::string accountToJson(const etfdt::data_access::AccountRecord& record)
{
    std::ostringstream stream;
    stream << '{'
           << rawField("id", std::to_string(record.id), true)
           << jsonStringField("uid", record.uid.raw(), true)
           << jsonStringField("name", record.name, true)
           << jsonStringField("accountType", etfdt::core::toString(record.accountType), true)
           << jsonStringField("brokerName", record.brokerName, true)
           << jsonStringField("baseCurrency", record.baseCurrency, true)
           << rawField("isActive", boolLiteral(record.isActive), true)
           << rawField("initialCashCents", std::to_string(record.initialCashCents.raw()), false)
           << '}';
    return stream.str();
}

std::string portfolioToJson(const etfdt::data_access::PortfolioRecord& record)
{
    std::ostringstream stream;
    stream << '{'
           << rawField("id", std::to_string(record.id), true)
           << jsonStringField("uid", record.uid.raw(), true)
           << jsonStringField("name", record.name, true)
           << rawField(
                  "basePositionRatioPpm",
                  std::to_string(record.basePositionRatioPpm.raw()),
                  true)
           << rawField("isActive", boolLiteral(record.isActive), false)
           << '}';
    return stream.str();
}

std::string instrumentToJson(const etfdt::data_access::InstrumentRecord& record)
{
    std::ostringstream stream;
    stream << '{'
           << rawField("id", std::to_string(record.id), true)
           << jsonStringField("uid", record.uid.raw(), true)
           << jsonStringField("code", record.code.raw(), true)
           << jsonStringField("name", record.name, true)
           << jsonStringField("instrumentType", etfdt::core::toString(record.instrumentType), true)
           << jsonStringField("marketCode", etfdt::core::toString(record.marketCode), true)
           << jsonStringField("currency", record.currency, true)
           << rawField("enabled", boolLiteral(record.enabled), false)
           << '}';
    return stream.str();
}

std::string strategyToJson(const etfdt::data_access::StrategyRecord& record)
{
    std::ostringstream stream;
    stream << '{'
           << rawField("id", std::to_string(record.id), true)
           << jsonStringField("uid", record.uid.raw(), true)
           << jsonStringField("strategyCode", record.strategyCode.raw(), true)
           << rawField("instrumentId", std::to_string(record.instrumentId), true)
           << jsonStringField("name", record.name, true)
           << rawField("enabled", boolLiteral(record.enabled), true)
           << rawField(
                  "externalPremiumThresholdPpm",
                  std::to_string(record.externalPremiumThresholdPpm.raw()),
                  true)
           << rawField(
                  "takeProfitPremiumThresholdPpm",
                  std::to_string(record.takeProfitPremiumThresholdPpm.raw()),
                  true)
           << rawField(
                  "returnTakeProfitRatioPpm",
                  std::to_string(record.returnTakeProfitRatioPpm.raw()),
                  true)
           << rawField("addPremiumLimitPpm", std::to_string(record.addPremiumLimitPpm.raw()), true)
           << rawField("adjustmentFactor1e6", std::to_string(record.adjustmentFactor1e6), false)
           << '}';
    return stream.str();
}

std::string otcChannelToJson(const etfdt::data_access::OtcChannelRecord& record)
{
    std::ostringstream stream;
    stream << '{'
           << rawField("id", std::to_string(record.id), true)
           << jsonStringField("uid", record.uid.raw(), true)
           << jsonStringField("strategyCode", record.strategyCode.raw(), true)
           << jsonStringField("actualCode", record.actualCode.raw(), true)
           << jsonStringField("fundClass", record.fundClass, true)
           << rawField("enabled", boolLiteral(record.enabled), true)
           << rawField("dailyLimitCents", std::to_string(record.dailyLimitCents.raw()), true)
           << rawField("priority", std::to_string(record.priority), true)
           << rawField("minBuyAmountCents", std::to_string(record.minBuyAmountCents.raw()), false)
           << '}';
    return stream.str();
}

std::string accountsToJsonArray(const std::vector<etfdt::data_access::AccountRecord>& records)
{
    return recordsToJsonArray(records, accountToJson);
}

std::string portfoliosToJsonArray(const std::vector<etfdt::data_access::PortfolioRecord>& records)
{
    return recordsToJsonArray(records, portfolioToJson);
}

std::string instrumentsToJsonArray(
    const std::vector<etfdt::data_access::InstrumentRecord>& records)
{
    return recordsToJsonArray(records, instrumentToJson);
}

std::string strategiesToJsonArray(const std::vector<etfdt::data_access::StrategyRecord>& records)
{
    return recordsToJsonArray(records, strategyToJson);
}

std::string otcChannelsToJsonArray(
    const std::vector<etfdt::data_access::OtcChannelRecord>& records)
{
    return recordsToJsonArray(records, otcChannelToJson);
}

}  // namespace etfdt::data_service_api
