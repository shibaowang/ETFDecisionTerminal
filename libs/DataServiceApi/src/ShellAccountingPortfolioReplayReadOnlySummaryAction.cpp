#include "DataServiceApi/DataServiceActions.h"

#include "AccountingEngine/ShellAccountingPortfolioReplayEngine.h"
#include "Protocol/Json.h"

#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace etfdt::data_service_api {
namespace {

using etfdt::accounting::ShellAccountingReplayCashFact;
using etfdt::accounting::ShellAccountingReplayInput;
using etfdt::accounting::ShellAccountingReplayMarketPriceFact;
using etfdt::accounting::ShellAccountingReplayTradeFact;

struct ReplayPayloadDecodeResult {
    ShellAccountingReplayInput input;
    bool ok = false;
    std::string errorCode;
};

class JsonCursor {
public:
    explicit JsonCursor(std::string_view text)
        : text_(text)
    {
    }

    [[nodiscard]] bool parseRootPayload(ShellAccountingReplayInput& input)
    {
        skipWhitespace();
        if (!consume('{')) {
            return false;
        }

        bool foundInputFacts = false;
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }

            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }

            if (*key == "schemaVersion") {
                auto value = parseString();
                if (!value.has_value()) {
                    return false;
                }
                input.schemaVersion = std::move(*value);
            } else if (*key == "inputFacts") {
                if (!parseInputFactsObject(input)) {
                    return false;
                }
                foundInputFacts = true;
            } else if (*key == "tradeFacts") {
                if (!parseTradeFactsArray(input.tradeFacts)) {
                    return false;
                }
                foundInputFacts = true;
            } else if (*key == "cashFacts") {
                if (!parseCashFactsArray(input.cashFacts)) {
                    return false;
                }
                foundInputFacts = true;
            } else if (*key == "marketPriceFacts") {
                if (!parseMarketPriceFactsArray(input.marketPriceFacts)) {
                    return false;
                }
                foundInputFacts = true;
            } else if (!skipValue()) {
                return false;
            }

            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }

        skipWhitespace();
        return foundInputFacts && position_ == text_.size();
    }

private:
    void skipWhitespace()
    {
        while (position_ < text_.size()
               && std::isspace(static_cast<unsigned char>(text_[position_])) != 0) {
            ++position_;
        }
    }

    [[nodiscard]] bool consume(char expected)
    {
        skipWhitespace();
        if (position_ >= text_.size() || text_[position_] != expected) {
            return false;
        }
        ++position_;
        return true;
    }

    [[nodiscard]] std::optional<std::string> parseString()
    {
        skipWhitespace();
        if (position_ >= text_.size() || text_[position_] != '"') {
            return std::nullopt;
        }
        ++position_;

        std::string output;
        while (position_ < text_.size()) {
            const char ch = text_[position_++];
            if (ch == '"') {
                return output;
            }
            if (ch != '\\') {
                output.push_back(ch);
                continue;
            }
            if (position_ >= text_.size()) {
                return std::nullopt;
            }
            const char escaped = text_[position_++];
            switch (escaped) {
            case '"':
            case '\\':
            case '/':
                output.push_back(escaped);
                break;
            case 'b':
                output.push_back('\b');
                break;
            case 'f':
                output.push_back('\f');
                break;
            case 'n':
                output.push_back('\n');
                break;
            case 'r':
                output.push_back('\r');
                break;
            case 't':
                output.push_back('\t');
                break;
            case 'u':
                if (position_ + 4U > text_.size()) {
                    return std::nullopt;
                }
                position_ += 4U;
                output.push_back('?');
                break;
            default:
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] bool parseInputFactsObject(ShellAccountingReplayInput& input)
    {
        if (!consume('{')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }

            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }
            if (*key == "tradeFacts") {
                if (!parseTradeFactsArray(input.tradeFacts)) {
                    return false;
                }
            } else if (*key == "cashFacts") {
                if (!parseCashFactsArray(input.cashFacts)) {
                    return false;
                }
            } else if (*key == "marketPriceFacts") {
                if (!parseMarketPriceFactsArray(input.marketPriceFacts)) {
                    return false;
                }
            } else if (!skipValue()) {
                return false;
            }

            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool parseTradeFactsArray(std::vector<ShellAccountingReplayTradeFact>& facts)
    {
        if (!consume('[')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            ShellAccountingReplayTradeFact fact;
            if (!parseTradeFactObject(fact)) {
                return false;
            }
            facts.push_back(std::move(fact));
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    [[nodiscard]] bool parseCashFactsArray(std::vector<ShellAccountingReplayCashFact>& facts)
    {
        if (!consume('[')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            ShellAccountingReplayCashFact fact;
            if (!parseCashFactObject(fact)) {
                return false;
            }
            facts.push_back(std::move(fact));
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    [[nodiscard]] bool parseMarketPriceFactsArray(
        std::vector<ShellAccountingReplayMarketPriceFact>& facts)
    {
        if (!consume('[')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            ShellAccountingReplayMarketPriceFact fact;
            if (!parseMarketPriceFactObject(fact)) {
                return false;
            }
            facts.push_back(std::move(fact));
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    [[nodiscard]] bool parseTradeFactObject(ShellAccountingReplayTradeFact& fact)
    {
        if (!consume('{')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }
            auto value = parseString();
            if (!value.has_value()) {
                if (!skipValue()) {
                    return false;
                }
            } else if (*key == "factId") {
                fact.factId = std::move(*value);
            } else if (*key == "tradeTime") {
                fact.tradeTime = std::move(*value);
            } else if (*key == "accountId") {
                fact.accountId = std::move(*value);
            } else if (*key == "portfolioId") {
                fact.portfolioId = std::move(*value);
            } else if (*key == "instrumentCode") {
                fact.instrumentCode = std::move(*value);
            } else if (*key == "instrumentType") {
                fact.instrumentType = std::move(*value);
            } else if (*key == "side" || *key == "action") {
                fact.side = std::move(*value);
            } else if (*key == "quantityText") {
                fact.quantityText = std::move(*value);
            } else if (*key == "priceText") {
                fact.priceText = std::move(*value);
            } else if (*key == "amountText") {
                fact.amountText = std::move(*value);
            } else if (*key == "feeText") {
                fact.feeText = std::move(*value);
            } else if (*key == "cashFlowText") {
                fact.cashFlowText = std::move(*value);
            } else if (*key == "currency") {
                fact.currency = std::move(*value);
            } else if (*key == "source") {
                fact.source = std::move(*value);
            } else if (*key == "note") {
                fact.note = std::move(*value);
            }
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool parseCashFactObject(ShellAccountingReplayCashFact& fact)
    {
        if (!consume('{')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }
            auto value = parseString();
            if (!value.has_value()) {
                if (!skipValue()) {
                    return false;
                }
            } else if (*key == "factId") {
                fact.factId = std::move(*value);
            } else if (*key == "time") {
                fact.time = std::move(*value);
            } else if (*key == "accountId") {
                fact.accountId = std::move(*value);
            } else if (*key == "portfolioId") {
                fact.portfolioId = std::move(*value);
            } else if (*key == "action") {
                fact.action = std::move(*value);
            } else if (*key == "amountText") {
                fact.amountText = std::move(*value);
            } else if (*key == "currency") {
                fact.currency = std::move(*value);
            } else if (*key == "note") {
                fact.note = std::move(*value);
            }
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool parseMarketPriceFactObject(ShellAccountingReplayMarketPriceFact& fact)
    {
        if (!consume('{')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }
            auto value = parseString();
            if (!value.has_value()) {
                if (!skipValue()) {
                    return false;
                }
            } else if (*key == "instrumentCode") {
                fact.instrumentCode = std::move(*value);
            } else if (*key == "priceTime") {
                fact.priceTime = std::move(*value);
            } else if (*key == "priceText") {
                fact.priceText = std::move(*value);
            } else if (*key == "currency") {
                fact.currency = std::move(*value);
            } else if (*key == "source") {
                fact.source = std::move(*value);
            }
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool skipValue()
    {
        skipWhitespace();
        if (position_ >= text_.size()) {
            return false;
        }
        if (text_[position_] == '"') {
            return parseString().has_value();
        }
        if (text_[position_] == '{') {
            return skipObject();
        }
        if (text_[position_] == '[') {
            return skipArray();
        }
        if (text_.substr(position_, 4U) == "true") {
            position_ += 4U;
            return true;
        }
        if (text_.substr(position_, 5U) == "false") {
            position_ += 5U;
            return true;
        }
        if (text_.substr(position_, 4U) == "null") {
            position_ += 4U;
            return true;
        }
        const auto start = position_;
        while (position_ < text_.size() &&
               std::string_view(",]} \t\r\n").find(text_[position_]) == std::string_view::npos) {
            ++position_;
        }
        return position_ > start;
    }

    [[nodiscard]] bool skipObject()
    {
        if (!consume('{')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                return true;
            }
            if (!parseString().has_value() || !consume(':') || !skipValue()) {
                return false;
            }
            skipWhitespace();
            if (consume('}')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    [[nodiscard]] bool skipArray()
    {
        if (!consume('[')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!skipValue()) {
                return false;
            }
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    std::string_view text_;
    std::size_t position_ = 0;
};

[[nodiscard]] std::string jsonStringValue(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

[[nodiscard]] ReplayPayloadDecodeResult decodeReplayPayload(std::string_view payloadJson)
{
    ReplayPayloadDecodeResult result;
    JsonCursor cursor(payloadJson);
    result.ok = cursor.parseRootPayload(result.input);
    if (!result.ok) {
        result.errorCode = "PORTFOLIO_REPLAY_PAYLOAD_PARSE_ERROR";
    }
    return result;
}

[[nodiscard]] std::string issueJson(const etfdt::accounting::AccountingIssueDto& issue)
{
    std::ostringstream stream;
    stream << "{"
           << "\"level\":" << jsonStringValue(issue.level) << ','
           << "\"code\":" << jsonStringValue(issue.code) << ','
           << "\"message\":" << jsonStringValue(issue.message) << ','
           << "\"blocking\":" << (issue.blocking ? "true" : "false") << ','
           << "\"field\":" << jsonStringValue(issue.field) << ','
           << "\"sourceId\":" << jsonStringValue(issue.sourceId)
           << "}";
    return stream.str();
}

template <typename T, typename Writer>
[[nodiscard]] std::string arrayJson(const std::vector<T>& values, Writer writer)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        stream << writer(values[index]);
    }
    stream << ']';
    return stream.str();
}

[[nodiscard]] std::string positionJson(
    const etfdt::accounting::ShellAccountingReplayPosition& position)
{
    std::ostringstream stream;
    stream << "{"
           << "\"accountId\":" << jsonStringValue(position.accountId) << ','
           << "\"portfolioId\":" << jsonStringValue(position.portfolioId) << ','
           << "\"instrumentCode\":" << jsonStringValue(position.instrumentCode) << ','
           << "\"instrumentType\":" << jsonStringValue(position.instrumentType) << ','
           << "\"positionBucket\":" << jsonStringValue(position.positionBucket) << ','
           << "\"quantityText\":" << jsonStringValue(position.quantityText) << ','
           << "\"averageCostText\":" << jsonStringValue(position.averageCostText) << ','
           << "\"costBasisText\":" << jsonStringValue(position.costBasisText) << ','
           << "\"marketValueText\":" << jsonStringValue(position.marketValueText) << ','
           << "\"realizedPnlText\":" << jsonStringValue(position.realizedPnlText) << ','
           << "\"unrealizedPnlText\":" << jsonStringValue(position.unrealizedPnlText) << ','
           << "\"totalFeeText\":" << jsonStringValue(position.totalFeeText) << ','
           << "\"currency\":" << jsonStringValue(position.currency) << ','
           << "\"dataQualityStatus\":" << jsonStringValue(position.dataQualityStatus)
           << "}";
    return stream.str();
}

[[nodiscard]] std::string cashSummaryJson(
    const etfdt::accounting::ShellAccountingReplayCashSummary& cash)
{
    std::ostringstream stream;
    stream << "{"
           << "\"accountId\":" << jsonStringValue(cash.accountId) << ','
           << "\"portfolioId\":" << jsonStringValue(cash.portfolioId) << ','
           << "\"currency\":" << jsonStringValue(cash.currency) << ','
           << "\"cashBalanceText\":" << jsonStringValue(cash.cashBalanceText) << ','
           << "\"dataQualityStatus\":" << jsonStringValue(cash.dataQualityStatus)
           << "}";
    return stream.str();
}

[[nodiscard]] std::string resultPayloadJson(
    const etfdt::accounting::ShellAccountingReplayResult& result)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonStringValue(kActionAccountingPortfolioReplayReadOnlySummary) << ','
            << "\"task\":\"EPIC-276\","
            << "\"mode\":\"portfolio-replay-vba-parity-readonly-full-delivery\","
            << "\"implemented\":true,"
            << "\"readOnly\":true,"
            << "\"writeEnabled\":false,"
            << "\"status\":" << jsonStringValue(result.status) << ','
            << "\"dataQualityStatus\":" << jsonStringValue(result.accepted ? result.pnlSummary.dataQualityStatus : "ERROR") << ','
            << "\"hasRows\":" << (!result.positions.empty() || !result.cashSummaries.empty() ? "true" : "false") << ','
            << "\"accepted\":" << (result.accepted ? "true" : "false") << ','
            << "\"replayExecuted\":" << (result.replayExecuted ? "true" : "false") << ','
            << "\"portfolioReplayEngineCreated\":true,"
            << "\"dataServiceReadOnlyActionCreated\":true,"
            << "\"accountingEngineCalled\":true,"
            << "\"positionCount\":" << result.positions.size() << ','
            << "\"cashSummaryCount\":" << result.cashSummaries.size() << ','
            << "\"issues\":" << arrayJson(result.issues, issueJson) << ','
            << "\"positions\":" << arrayJson(result.positions, positionJson) << ','
            << "\"cashSummaries\":" << arrayJson(result.cashSummaries, cashSummaryJson) << ','
            << "\"pnlSummary\":{"
            << "\"currency\":" << jsonStringValue(result.pnlSummary.currency) << ','
            << "\"realizedPnlText\":" << jsonStringValue(result.pnlSummary.realizedPnlText) << ','
            << "\"unrealizedPnlText\":" << jsonStringValue(result.pnlSummary.unrealizedPnlText) << ','
            << "\"totalFeeText\":" << jsonStringValue(result.pnlSummary.totalFeeText) << ','
            << "\"totalMarketValueText\":" << jsonStringValue(result.pnlSummary.totalMarketValueText) << ','
            << "\"dataQualityStatus\":" << jsonStringValue(result.pnlSummary.dataQualityStatus)
            << "},"
            << "\"readOnlyReplayNoWrite\":true,"
            << "\"tradeLogRowsWrittenByReplay\":false,"
            << "\"cashAdjustmentRowsWrittenByReplay\":false,"
            << "\"auditLogRowsWrittenByReplay\":false,"
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
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonStringValue(kActionAccountingPortfolioReplayReadOnlySummary) << ','
            << "\"task\":\"EPIC-276\","
            << "\"mode\":\"portfolio-replay-vba-parity-readonly-full-delivery\","
            << "\"implemented\":true,"
            << "\"readOnly\":true,"
            << "\"writeEnabled\":false,"
            << "\"status\":\"REJECTED\","
            << "\"dataQualityStatus\":\"ERROR\","
            << "\"hasRows\":false,"
            << "\"accepted\":false,"
            << "\"replayExecuted\":false,"
            << "\"portfolioReplayEngineCreated\":true,"
            << "\"dataServiceReadOnlyActionCreated\":true,"
            << "\"accountingEngineCalled\":false,"
            << "\"positionCount\":0,"
            << "\"cashSummaryCount\":0,"
            << "\"issues\":[{\"level\":\"ERROR\",\"code\":" << jsonStringValue(code)
            << ",\"message\":\"Replay payload could not be parsed.\",\"blocking\":true,"
            << "\"field\":\"inputFacts\",\"sourceId\":\"\"}],"
            << "\"positions\":[],"
            << "\"cashSummaries\":[],"
            << "\"pnlSummary\":{\"currency\":\"CNY\",\"realizedPnlText\":\"0.00\","
               "\"unrealizedPnlText\":\"0.00\",\"totalFeeText\":\"0.00\","
               "\"totalMarketValueText\":\"0.00\",\"dataQualityStatus\":\"ERROR\"},"
            << "\"readOnlyReplayNoWrite\":true,"
            << "\"tradeLogRowsWrittenByReplay\":false,"
            << "\"cashAdjustmentRowsWrittenByReplay\":false,"
            << "\"auditLogRowsWrittenByReplay\":false,"
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

etfdt::protocol::ProtocolResponse handleAccountingPortfolioReplayReadOnlySummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;

    const auto decoded = decodeReplayPayload(context.request.payloadJson);
    if (!decoded.ok) {
        return successResponse(context, parseErrorPayloadJson(decoded.errorCode));
    }

    const auto result =
        etfdt::accounting::ShellAccountingPortfolioReplayEngine{}.replayReadOnly(decoded.input);
    return successResponse(context, resultPayloadJson(result));
}

}  // namespace etfdt::data_service_api
