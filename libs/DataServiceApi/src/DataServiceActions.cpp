#include "DataServiceApi/DataServiceActions.h"

#include "AccountingEngine/AccountingReplayEngine.h"
#include "DataAccess/AccountingTradeFactReader.h"
#include "DataServiceApi/JsonBuilders.h"
#include "DataAccess/ShellAccountingAuditWriteRepository.h"
#include "DataAccess/ShellAccountingBrokerOrderDryRunRepository.h"
#include "DataAccess/ShellAccountingReadOnlyFactsQuery.h"
#include "DataAccess/ShellAccountingSnapshotWriteRepository.h"
#include "DataAccess/ShellAccountingTradeDraftConfirmationRepository.h"
#include "DataAccess/ShellAccountingTradeDraftRepository.h"
#include "DataServiceApi/ShellAccountingBrokerRuntimeModeSource.h"
#include "DataServiceApi/ShellAccountingBrokerSandboxRuntimeEnablement.h"
#include "DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h"
#include "Protocol/Json.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace etfdt::data_service_api {
namespace {

template <typename T>
etfdt::protocol::ProtocolResponse errorResponse(
    const etfdt::service_runtime::ActionContext& context,
    const etfdt::data_access::DatabaseResult<T>& result)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = false;
    response.errorCode = result.error().errorCode;
    response.errorMessage = result.error().message.empty() ? "DataService query failed"
                                                           : result.error().message;
    response.payloadJson = "{}";
    return response;
}

etfdt::protocol::ProtocolResponse protocolErrorResponse(
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

std::string normalizedSchemaName(
    const std::vector<etfdt::data_access::SchemaVersionRecord>& versions)
{
    if (versions.empty()) {
        return {};
    }

    const std::string& migrationName = versions.back().migrationName;
    if (migrationName.find("001_initial_schema") != std::string::npos) {
        return "001_initial_schema";
    }
    return migrationName;
}

std::string jsonStringValue(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

std::string decodeJsonString(std::string_view value)
{
    std::string decoded;
    decoded.reserve(value.size());
    bool escaping = false;
    for (const char ch : value) {
        if (!escaping) {
            if (ch == '\\') {
                escaping = true;
            }
            else {
                decoded.push_back(ch);
            }
            continue;
        }

        switch (ch) {
        case '"':
        case '\\':
        case '/':
            decoded.push_back(ch);
            break;
        case 'b':
            decoded.push_back('\b');
            break;
        case 'f':
            decoded.push_back('\f');
            break;
        case 'n':
            decoded.push_back('\n');
            break;
        case 'r':
            decoded.push_back('\r');
            break;
        case 't':
            decoded.push_back('\t');
            break;
        default:
            decoded.push_back(ch);
            break;
        }
        escaping = false;
    }
    return decoded;
}

std::string trimCopy(std::string value)
{
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1U);
}

std::string extractStrategyCode(const std::string& payloadJson)
{
    static const std::regex pattern(
        R"json("strategyCode"\s*:\s*"((?:\\.|[^"\\])*)")json",
        std::regex::ECMAScript);

    std::smatch match;
    if (!std::regex_search(payloadJson, match, pattern) || match.size() < 2U) {
        return {};
    }
    return trimCopy(decodeJsonString(match[1].str()));
}

std::optional<std::string> extractJsonStringField(
    const std::string& payloadJson,
    const std::string& fieldName)
{
    const std::regex pattern(
        "\"" + fieldName + "\"\\s*:\\s*\"((?:\\\\.|[^\"\\\\])*)\"",
        std::regex::ECMAScript);

    std::smatch match;
    if (!std::regex_search(payloadJson, match, pattern) || match.size() < 2U) {
        return std::nullopt;
    }
    return decodeJsonString(match[1].str());
}

std::optional<bool> extractJsonBoolField(
    const std::string& payloadJson,
    const std::string& fieldName)
{
    const std::regex pattern(
        "\"" + fieldName + "\"\\s*:\\s*(true|false)",
        std::regex::ECMAScript);

    std::smatch match;
    if (!std::regex_search(payloadJson, match, pattern) || match.size() < 2U) {
        return std::nullopt;
    }
    return match[1].str() == "true";
}

std::optional<int> extractJsonIntField(
    const std::string& payloadJson,
    const std::string& fieldName)
{
    const std::regex pattern(
        "\"" + fieldName + "\"\\s*:\\s*(-?[0-9]+)",
        std::regex::ECMAScript);

    std::smatch match;
    if (!std::regex_search(payloadJson, match, pattern) || match.size() < 2U) {
        return std::nullopt;
    }

    try {
        std::size_t parsed = 0;
        const auto value = std::stoi(match[1].str(), &parsed, 10);
        if (parsed != match[1].str().size()) {
            return std::nullopt;
        }
        return value;
    }
    catch (...) {
        return std::nullopt;
    }
}

std::optional<std::string> extractJsonFragmentField(
    const std::string& payloadJson,
    const std::string& fieldName)
{
    const std::regex keyPattern("\"" + fieldName + R"("\s*:)", std::regex::ECMAScript);
    std::smatch match;
    if (!std::regex_search(payloadJson, match, keyPattern)) {
        return std::nullopt;
    }

    std::size_t position = static_cast<std::size_t>(match.position() + match.length());
    while (position < payloadJson.size()
           && std::isspace(static_cast<unsigned char>(payloadJson[position])) != 0) {
        ++position;
    }
    if (position >= payloadJson.size()
        || (payloadJson[position] != '{' && payloadJson[position] != '[')) {
        return std::string {};
    }

    const char opening = payloadJson[position];
    const char closing = opening == '{' ? '}' : ']';
    int depth = 0;
    bool inString = false;
    bool escaping = false;

    for (std::size_t i = position; i < payloadJson.size(); ++i) {
        const char ch = payloadJson[i];
        if (inString) {
            if (escaping) {
                escaping = false;
            }
            else if (ch == '\\') {
                escaping = true;
            }
            else if (ch == '"') {
                inString = false;
            }
            continue;
        }

        if (ch == '"') {
            inString = true;
            continue;
        }
        if (ch == opening) {
            ++depth;
        }
        else if (ch == closing) {
            --depth;
            if (depth == 0) {
                return payloadJson.substr(position, i - position + 1U);
            }
        }
    }

    return std::string {};
}

std::optional<std::int64_t> parseOptionalInt64(const std::string& value)
{
    const std::string trimmed = trimCopy(value);
    if (trimmed.empty()) {
        return std::nullopt;
    }

    try {
        std::size_t parsed = 0;
        const auto integer = std::stoll(trimmed, &parsed, 10);
        if (parsed != trimmed.size()) {
            return std::nullopt;
        }
        return integer;
    }
    catch (...) {
        return std::nullopt;
    }
}

std::int64_t parseNumericTextScaled(
    std::string value,
    int scale,
    std::int64_t fallback = 0)
{
    value = trimCopy(value);
    if (value.empty() || value == "UNAVAILABLE") {
        return fallback;
    }

    const auto firstSpace = value.find(' ');
    if (firstSpace != std::string::npos) {
        value = value.substr(0, firstSpace);
    }

    try {
        return static_cast<std::int64_t>(std::llround(std::stold(value) * scale));
    }
    catch (...) {
        return fallback;
    }
}

std::string snapshotUid(
    const char* table,
    std::int64_t accountId,
    std::int64_t portfolioId,
    const std::string& discriminator)
{
    std::ostringstream uid;
    uid << "task-144-" << table << '-' << accountId << '-' << portfolioId;
    if (!discriminator.empty()) {
        uid << '-' << discriminator;
    }
    return uid.str();
}

bool isJsonObjectPayloadShape(const std::string& payloadJson)
{
    const std::string trimmedPayload = trimCopy(payloadJson);
    if (!etfdt::protocol::isLikelyJsonObjectOrArray(payloadJson) || trimmedPayload.empty()
        || trimmedPayload.front() != '{') {
        return false;
    }
    if (trimmedPayload == "{}") {
        return true;
    }
    return trimmedPayload.find('"') != std::string::npos
        && trimmedPayload.find(':') != std::string::npos;
}

etfdt::data_access::ShellAccountingFactsQueryRequest shellAccountingQueryRequest(
    const std::string& payloadJson)
{
    etfdt::data_access::ShellAccountingFactsQueryRequest request;
    request.accountId = extractJsonStringField(payloadJson, "accountId").value_or("");
    request.portfolioId = extractJsonStringField(payloadJson, "portfolioId").value_or("");
    return request;
}

void appendJsonField(
    std::ostringstream& stream,
    const char* name,
    const std::string& value,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << '"' << name << "\":" << jsonStringValue(value);
    needsComma = true;
}

void appendJsonField(
    std::ostringstream& stream,
    const char* name,
    int value,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << '"' << name << "\":" << value;
    needsComma = true;
}

template <typename T, typename Writer>
std::string jsonArray(const std::vector<T>& rows, Writer writer)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < rows.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        writer(stream, rows[index]);
    }
    stream << ']';
    return stream.str();
}

void appendIssueObject(
    std::ostringstream& stream,
    const char* level,
    const char* code,
    const char* message,
    bool blocking)
{
    stream << "{"
           << "\"level\":" << jsonStringValue(level) << ','
           << "\"code\":" << jsonStringValue(code) << ','
           << "\"message\":" << jsonStringValue(message) << ','
           << "\"blocking\":" << (blocking ? "true" : "false") << ','
           << "\"sourceId\":\"DataServiceShellAccountingFactsQuery\""
           << "}";
}

std::string shellAccountingPayloadPrefix(
    const char* action,
    const char* status,
    const char* dataQualityStatus,
    bool hasRows,
    bool replayExecuted = false,
    bool accountingEngineCalled = false)
{
    std::ostringstream stream;
    stream << "{"
           << "\"module\":\"accounting\","
           << "\"action\":" << jsonStringValue(action) << ','
           << "\"implemented\":true,"
           << "\"readOnly\":true,"
           << "\"writeEnabled\":false,"
           << "\"replayExecuted\":" << (replayExecuted ? "true" : "false") << ','
           << "\"snapshotRebuilt\":false,"
           << "\"dataSourceAccessed\":true,"
           << "\"sqliteAccessed\":true,"
           << "\"accountingEngineCalled\":" << (accountingEngineCalled ? "true" : "false") << ','
           << "\"tradeDraftGenerated\":false,"
           << "\"tradeSuggestionGenerated\":false,"
           << "\"strategyExecuted\":false,"
           << "\"brokerOrderSubmitted\":false,"
           << "\"contractVersion\":"
           << jsonStringValue(replayExecuted ? "0.6-readonly-replay" : "0.5-readonly-facts") << ','
           << "\"calculationVersion\":"
           << jsonStringValue(replayExecuted ? "readonly-replay-v1" : "readonly-facts-query-v1") << ','
           << "\"status\":" << jsonStringValue(status) << ','
           << "\"dataQualityStatus\":" << jsonStringValue(dataQualityStatus) << ','
           << "\"hasRows\":" << (hasRows ? "true" : "false") << ','
           << "\"stale\":false,";
    return stream.str();
}

bool shellAccountingReplayRequested(const std::string& payloadJson)
{
    const auto calculationMode = extractJsonStringField(payloadJson, "calculationMode").value_or("");
    return calculationMode == "readonlyReplay";
}

std::string removeTrailingCurrency(std::string value, const std::string& currency)
{
    const std::string suffix = " " + currency;
    if (!currency.empty() && value.size() > suffix.size()
        && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0) {
        value.erase(value.size() - suffix.size());
    }
    return value;
}

std::string portfolioForAccount(
    const std::vector<etfdt::data_access::TradeFactRow>& tradeFacts,
    const std::string& accountId,
    const std::string& fallbackPortfolioId)
{
    if (!fallbackPortfolioId.empty()) {
        return fallbackPortfolioId;
    }
    for (const auto& fact : tradeFacts) {
        if (fact.accountId == accountId) {
            return fact.portfolioId;
        }
    }
    return {};
}

etfdt::accounting::TradeFactDto toReplayTradeFact(
    const etfdt::data_access::TradeFactRow& row)
{
    return etfdt::accounting::TradeFactDto{
        row.factId,
        row.tradeTime,
        row.accountId,
        row.portfolioId,
        row.instrumentCode,
        row.action,
        row.quantityText,
        removeTrailingCurrency(row.priceText, row.currency),
        removeTrailingCurrency(row.amountText, row.currency),
        removeTrailingCurrency(row.feeText, row.currency),
        removeTrailingCurrency(row.cashFlowText, row.currency),
        row.currency.empty() ? "CNY" : row.currency,
        row.source,
        {},
    };
}

std::vector<etfdt::accounting::TradeFactDto> toReplayTradeFacts(
    const std::vector<etfdt::data_access::TradeFactRow>& rows)
{
    std::vector<etfdt::accounting::TradeFactDto> facts;
    facts.reserve(rows.size());
    for (const auto& row : rows) {
        facts.push_back(toReplayTradeFact(row));
    }
    return facts;
}

std::vector<etfdt::accounting::CashFactDto> toReplayCashFacts(
    const std::vector<etfdt::data_access::ShellAccountingInitialCashFactRow>& rows,
    const std::vector<etfdt::data_access::TradeFactRow>& tradeFacts,
    const etfdt::data_access::ShellAccountingFactsQueryRequest& request)
{
    std::vector<etfdt::accounting::CashFactDto> facts;
    facts.reserve(rows.size());
    for (const auto& row : rows) {
        facts.push_back(etfdt::accounting::CashFactDto{
            "initial-cash-" + row.accountId,
            row.updatedAtUtc,
            row.accountId,
            portfolioForAccount(tradeFacts, row.accountId, request.portfolioId),
            etfdt::accounting::CashAction::InitialCash,
            row.amountText,
            row.currency.empty() ? "CNY" : row.currency,
            {},
        });
    }
    return facts;
}

std::vector<std::string> requestedReplayOutputsForAction(const char* action)
{
    const std::string actionName(action);
    if (actionName == "position.list") {
        return {etfdt::accounting::RequestedReplayOutput::Positions};
    }
    if (actionName == "cash.summary") {
        return {etfdt::accounting::RequestedReplayOutput::Cash};
    }
    if (actionName == "portfolio.pnl.summary") {
        return {etfdt::accounting::RequestedReplayOutput::Positions,
                etfdt::accounting::RequestedReplayOutput::Cash,
                etfdt::accounting::RequestedReplayOutput::Pnl};
    }
    if (actionName == "base_position.summary") {
        return {etfdt::accounting::RequestedReplayOutput::Positions,
                etfdt::accounting::RequestedReplayOutput::BasePosition};
    }
    return {etfdt::accounting::RequestedReplayOutput::Positions,
            etfdt::accounting::RequestedReplayOutput::SniperPool};
}

struct ShellAccountingReplayAttempt final {
    bool requested = false;
    bool queryFailed = false;
    const char* queryErrorCode = "";
    etfdt::accounting::AccountingReplayResult result;
};

ShellAccountingReplayAttempt runShellAccountingReadOnlyReplay(
    etfdt::data_access::SQLiteConnection& connection,
    const char* action,
    const std::string& payloadJson)
{
    ShellAccountingReplayAttempt attempt;
    attempt.requested = shellAccountingReplayRequested(payloadJson);
    if (!attempt.requested) {
        return attempt;
    }

    const auto request = shellAccountingQueryRequest(payloadJson);

    etfdt::data_access::AccountingTradeFactReader tradeReader(connection);
    etfdt::data_access::TradeFactQueryRequest tradeRequest;
    tradeRequest.accountId = request.accountId;
    tradeRequest.portfolioId = request.portfolioId;
    const auto tradeFactsResult = tradeReader.loadTradeFacts(tradeRequest);
    if (!tradeFactsResult) {
        attempt.queryFailed = true;
        attempt.queryErrorCode = "READONLY_REPLAY_TRADE_FACT_QUERY_ERROR";
        return attempt;
    }

    etfdt::data_access::ShellAccountingReadOnlyFactsQuery factsQuery(connection);
    const auto initialCashFacts = factsQuery.loadInitialCashFacts(request);
    if (!initialCashFacts) {
        attempt.queryFailed = true;
        attempt.queryErrorCode = "READONLY_REPLAY_INITIAL_CASH_QUERY_ERROR";
        return attempt;
    }

    etfdt::accounting::ReplayRequestDto replayRequest;
    replayRequest.accountId = request.accountId;
    replayRequest.portfolioId = request.portfolioId;
    replayRequest.requestedOutputs = requestedReplayOutputsForAction(action);
    replayRequest.includeIssues = true;
    replayRequest.calculationVersion = "readonly-replay-v1";

    etfdt::accounting::AccountingReplayEngine engine;
    attempt.result = engine.replayReadOnly(
        replayRequest,
        toReplayTradeFacts(tradeFactsResult.value().rows),
        toReplayCashFacts(initialCashFacts.value(), tradeFactsResult.value().rows, request),
        {},
        {});
    return attempt;
}

void appendReplayIssueObject(
    std::ostringstream& stream,
    const etfdt::accounting::AccountingIssueDto& issue)
{
    stream << "{"
           << "\"level\":" << jsonStringValue(issue.level) << ','
           << "\"code\":" << jsonStringValue(issue.code) << ','
           << "\"message\":" << jsonStringValue(issue.message) << ','
           << "\"blocking\":" << (issue.blocking ? "true" : "false") << ','
           << "\"field\":" << jsonStringValue(issue.field) << ','
           << "\"sourceId\":\"AccountingReplayEngine\""
           << "}";
}

std::string replayIssuesJson(const std::vector<etfdt::accounting::AccountingIssueDto>& issues)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < issues.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        appendReplayIssueObject(stream, issues[index]);
    }
    stream << ']';
    return stream.str();
}

std::string replayDataQualityStatus(const etfdt::accounting::AccountingReplayResult& result);
bool replayHasBlockingIssue(const etfdt::accounting::AccountingReplayResult& result);

std::string snapshotPreviewStatus(const etfdt::accounting::AccountingReplayResult& result)
{
    if (!result.implemented || !result.replayExecuted) {
        return "UNAVAILABLE";
    }
    if (replayHasBlockingIssue(result)) {
        return "ISSUE";
    }
    return "READY";
}

void writeSnapshotPreviewPosition(
    std::ostringstream& stream,
    const etfdt::accounting::PositionSummaryDto& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "instrumentCode", row.instrumentCode, comma);
    appendJsonField(stream, "quantityText", row.quantityText, comma);
    appendJsonField(stream, "costAmountText", row.costAmountText, comma);
    appendJsonField(stream, "marketValueText", row.marketValueText, comma);
    appendJsonField(stream, "unrealizedPnlText", row.unrealizedPnlText, comma);
    appendJsonField(stream, "currency", row.currency, comma);
    appendJsonField(stream, "dataQualityStatus", row.dataQualityStatus, comma);
    stream << '}';
}

void writeSnapshotPreviewCash(
    std::ostringstream& stream,
    const etfdt::accounting::CashSummaryDto& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "currency", row.currency, comma);
    appendJsonField(stream, "cashBalanceText", row.cashBalanceText, comma);
    appendJsonField(stream, "dataQualityStatus", row.dataQualityStatus, comma);
    stream << '}';
}

void writeSnapshotPreviewPortfolio(
    std::ostringstream& stream,
    const etfdt::accounting::PortfolioPnlDto& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "currency", row.currency, comma);
    appendJsonField(stream, "realizedPnlText", row.realizedPnlText, comma);
    appendJsonField(stream, "unrealizedPnlText", row.unrealizedPnlText, comma);
    appendJsonField(stream, "totalAssetsText", row.totalAssetsText, comma);
    appendJsonField(stream, "totalPnlText", row.totalPnlText, comma);
    appendJsonField(stream, "dataQualityStatus", row.dataQualityStatus, comma);
    stream << '}';
}

std::vector<etfdt::accounting::CashSummaryDto> replayCashSummariesForPreview(
    const etfdt::accounting::AccountingReplayResult& result)
{
    auto summaries = result.accountCashSummaries;
    if (summaries.empty() && result.hasCashSummary) {
        summaries.push_back(result.cashSummary);
    }
    return summaries;
}

etfdt::data_access::ShellAccountingSnapshotWriteBatch snapshotWriteBatchFromReplay(
    const etfdt::accounting::AccountingReplayResult& result,
    const etfdt::data_access::ShellAccountingFactsQueryRequest& request,
    std::string_view updatedAtUtc)
{
    etfdt::data_access::ShellAccountingSnapshotWriteBatch batch;
    const auto fallbackAccountId = parseOptionalInt64(request.accountId).value_or(0);
    const auto fallbackPortfolioId = parseOptionalInt64(request.portfolioId).value_or(0);

    std::int64_t holdingCostCents = 0;
    std::int64_t totalMarketValueCents = 0;
    for (const auto& row : result.positionList.positions) {
        const auto accountId = parseOptionalInt64(row.accountId).value_or(fallbackAccountId);
        const auto portfolioId = parseOptionalInt64(row.portfolioId).value_or(fallbackPortfolioId);
        const auto costAmountCents = parseNumericTextScaled(row.costAmountText, 100);
        const auto marketValueCents = parseNumericTextScaled(row.marketValueText, 100);
        holdingCostCents += costAmountCents;
        totalMarketValueCents += marketValueCents;

        etfdt::data_access::ShellAccountingPositionSnapshotWriteRow snapshot;
        snapshot.uid = snapshotUid("position_snapshot", accountId, portfolioId, row.instrumentCode);
        snapshot.accountId = accountId;
        snapshot.portfolioId = portfolioId;
        snapshot.actualCode = row.instrumentCode;
        snapshot.quantity1e6 = parseNumericTextScaled(row.quantityText, 1000000);
        snapshot.costAmountCents = costAmountCents;
        snapshot.marketValueCents = marketValueCents;
        snapshot.unrealizedPnlCents = parseNumericTextScaled(row.unrealizedPnlText, 100);
        snapshot.updatedAtUtc = std::string(updatedAtUtc);
        batch.positions.push_back(std::move(snapshot));
    }

    std::int64_t primaryCashBalanceCents = 0;
    std::int64_t primaryAccountId = fallbackAccountId;
    std::int64_t primaryPortfolioId = fallbackPortfolioId;
    for (const auto& row : replayCashSummariesForPreview(result)) {
        const auto accountId = parseOptionalInt64(row.accountId).value_or(fallbackAccountId);
        const auto portfolioId = parseOptionalInt64(row.portfolioId).value_or(fallbackPortfolioId);
        const auto cashBalanceCents = parseNumericTextScaled(row.cashBalanceText, 100);
        if (batch.cash.empty()) {
            primaryCashBalanceCents = cashBalanceCents;
            primaryAccountId = accountId;
            primaryPortfolioId = portfolioId;
        }

        etfdt::data_access::ShellAccountingCashSnapshotWriteRow snapshot;
        snapshot.uid = snapshotUid("cash_snapshot", accountId, portfolioId, row.currency);
        snapshot.accountId = accountId;
        snapshot.portfolioId = portfolioId;
        snapshot.cashBalanceCents = cashBalanceCents;
        snapshot.updatedAtUtc = std::string(updatedAtUtc);
        batch.cash.push_back(std::move(snapshot));
    }

    if (result.hasPortfolioPnl) {
        etfdt::data_access::ShellAccountingPortfolioSummaryWriteRow summary;
        summary.uid = snapshotUid("portfolio_summary", primaryAccountId, primaryPortfolioId, result.portfolioPnl.currency);
        summary.accountId = primaryAccountId;
        summary.portfolioId = parseOptionalInt64(result.portfolioPnl.portfolioId).value_or(primaryPortfolioId);
        summary.totalAssetsCents = parseNumericTextScaled(result.portfolioPnl.totalAssetsText, 100);
        summary.totalMarketValueCents = totalMarketValueCents;
        summary.cashBalanceCents = primaryCashBalanceCents;
        summary.holdingCostCents = holdingCostCents;
        summary.totalPnlCents = parseNumericTextScaled(result.portfolioPnl.totalPnlText, 100);
        summary.updatedAtUtc = std::string(updatedAtUtc);
        batch.portfolioSummaries.push_back(std::move(summary));
    }

    return batch;
}

etfdt::data_access::ShellAccountingAuditWriteRequest auditWriteRequestFromPayload(
    const std::string& payloadJson,
    std::string_view timestampUtc)
{
    etfdt::data_access::ShellAccountingAuditWriteRequest request;
    request.accountId = parseOptionalInt64(extractJsonStringField(payloadJson, "accountId").value_or(""))
                            .value_or(0);
    request.portfolioId = parseOptionalInt64(extractJsonStringField(payloadJson, "portfolioId").value_or(""))
                              .value_or(0);
    request.snapshotBatchId = extractJsonStringField(payloadJson, "snapshotBatchId").value_or("");
    request.sourceAction = extractJsonStringField(payloadJson, "sourceAction").value_or("");
    request.sourceStatus = extractJsonStringField(payloadJson, "resultStatus").value_or("");
    request.authorizationToken = extractJsonStringField(payloadJson, "authorization").value_or("");
    request.sanitizedIssueCode = extractJsonStringField(payloadJson, "sanitizedIssueCode").value_or("NONE");
    request.cashRows = extractJsonIntField(payloadJson, "cash_snapshot").value_or(0);
    request.positionRows = extractJsonIntField(payloadJson, "position_snapshot").value_or(0);
    request.portfolioSummaryRows = extractJsonIntField(payloadJson, "portfolio_summary").value_or(0);
    request.createdAtUtc = std::string(timestampUtc);
    return request;
}

std::int64_t extractJsonInt64OrStringField(
    const std::string& payloadJson,
    const std::string& fieldName)
{
    if (const auto text = extractJsonStringField(payloadJson, fieldName); text.has_value()) {
        return parseOptionalInt64(*text).value_or(0);
    }
    return extractJsonIntField(payloadJson, fieldName).value_or(0);
}

std::int64_t extractTradeDraftQuantity1e6(const std::string& payloadJson)
{
    if (const auto integer = extractJsonIntField(payloadJson, "quantity1e6"); integer.has_value()) {
        return *integer;
    }
    if (const auto quantity = extractJsonStringField(payloadJson, "quantity"); quantity.has_value()) {
        return parseNumericTextScaled(*quantity, 1000000);
    }
    return 0;
}

bool tradeDraftRiskBlocked(const std::string& payloadJson)
{
    const auto riskFlags = extractJsonFragmentField(payloadJson, "riskFlags").value_or("");
    if (riskFlags.empty()) {
        return false;
    }
    return extractJsonBoolField(riskFlags, "blocked").value_or(false);
}

bool tradeDraftRiskFlagsPresent(const std::string& payloadJson)
{
    const auto riskFlags = extractJsonFragmentField(payloadJson, "riskFlags");
    return riskFlags.has_value() && !riskFlags->empty() && riskFlags->front() == '{';
}

etfdt::data_access::ShellAccountingTradeDraftCreateRequest tradeDraftRequestFromPayload(
    const std::string& payloadJson,
    std::string_view timestampUtc)
{
    etfdt::data_access::ShellAccountingTradeDraftCreateRequest request;
    request.accountId = extractJsonInt64OrStringField(payloadJson, "accountId");
    request.portfolioId = extractJsonInt64OrStringField(payloadJson, "portfolioId");
    request.strategyId = extractJsonInt64OrStringField(payloadJson, "strategyId");
    request.strategyCode = extractJsonStringField(payloadJson, "strategyCode").value_or("");
    request.instrumentId = extractJsonInt64OrStringField(payloadJson, "instrumentId");
    request.instrumentCode = extractJsonStringField(payloadJson, "instrumentCode")
                                 .value_or(extractJsonStringField(payloadJson, "symbol").value_or(""));
    request.side = extractJsonStringField(payloadJson, "side").value_or("");
    request.quantity1e6 = extractTradeDraftQuantity1e6(payloadJson);
    request.reason = extractJsonStringField(payloadJson, "reason").value_or("");
    request.sourceSnapshotId = extractJsonStringField(payloadJson, "sourceSnapshotId").value_or("");
    request.sourceReplayId = extractJsonStringField(payloadJson, "sourceReplayId").value_or("");
    request.authorizationToken = extractJsonStringField(payloadJson, "authorization").value_or("");
    request.createdAtUtc = std::string(timestampUtc);
    return request;
}

etfdt::data_access::ShellAccountingTradeDraftConfirmationRequest tradeDraftConfirmationRequestFromPayload(
    const std::string& payloadJson,
    std::string_view timestampUtc)
{
    etfdt::data_access::ShellAccountingTradeDraftConfirmationRequest request;
    request.draftId = extractJsonInt64OrStringField(payloadJson, "draftId");
    request.accountId = extractJsonInt64OrStringField(payloadJson, "accountId");
    request.portfolioId = extractJsonInt64OrStringField(payloadJson, "portfolioId");
    request.strategyId = extractJsonInt64OrStringField(payloadJson, "strategyId");
    request.strategyCode = extractJsonStringField(payloadJson, "strategyCode").value_or("");
    request.instrumentId = extractJsonInt64OrStringField(payloadJson, "instrumentId");
    request.instrumentCode = extractJsonStringField(payloadJson, "instrumentCode")
                                 .value_or(extractJsonStringField(payloadJson, "symbol").value_or(""));
    request.side = extractJsonStringField(payloadJson, "side").value_or("");
    request.quantity1e6 = extractTradeDraftQuantity1e6(payloadJson);
    request.sourceSnapshotId = extractJsonStringField(payloadJson, "sourceSnapshotId").value_or("");
    request.sourceReplayId = extractJsonStringField(payloadJson, "sourceReplayId").value_or("");
    request.authorizationToken = extractJsonStringField(payloadJson, "authorization").value_or("");
    request.confirmedAtUtc = std::string(timestampUtc);
    return request;
}

etfdt::data_access::ShellAccountingBrokerOrderDryRunRequest brokerOrderDryRunRequestFromPayload(
    const std::string& payloadJson,
    std::string_view timestampUtc)
{
    etfdt::data_access::ShellAccountingBrokerOrderDryRunRequest request;
    request.draftId = extractJsonInt64OrStringField(payloadJson, "draftId");
    request.tradeLogId = extractJsonInt64OrStringField(payloadJson, "tradeLogId");
    request.accountId = extractJsonInt64OrStringField(payloadJson, "accountId");
    request.portfolioId = extractJsonInt64OrStringField(payloadJson, "portfolioId");
    request.source = extractJsonStringField(payloadJson, "source").value_or("");
    request.authorizationToken = extractJsonStringField(payloadJson, "authorization").value_or("");
    request.userConfirmed = extractJsonBoolField(payloadJson, "userConfirmed").value_or(false);
    request.requestedAtUtc = std::string(timestampUtc);
    return request;
}

etfdt::dataservice::ShellAccountingBrokerOrderRequest brokerOrderPortRequestFromDryRun(
    const etfdt::data_access::ShellAccountingBrokerOrderDryRunRequest& request,
    const etfdt::data_access::ShellAccountingBrokerOrderDryRunResult& result)
{
    etfdt::dataservice::ShellAccountingBrokerOrderRequest portRequest;
    portRequest.accountId = std::to_string(request.accountId);
    portRequest.portfolioId = std::to_string(request.portfolioId);
    portRequest.sourceTradeDraftId = std::to_string(result.draftId);
    portRequest.sourceLedgerId = std::to_string(result.tradeLogId);
    portRequest.sourceDryRunId = std::to_string(result.auditLogId);
    portRequest.userConfirmationId = request.userConfirmed ? "TASK-154_DRY_RUN_USER_CONFIRMED" : "";
    portRequest.idempotencyKey = "broker-dry-run:" + std::to_string(result.tradeLogId);
    portRequest.finalUserConfirmed = false;
    portRequest.riskFlags = {"DRY_RUN_ONLY", "BROKER_DISABLED"};
    return portRequest;
}

const char* brokerOrderStatusCode(etfdt::dataservice::ShellAccountingBrokerOrderStatus status) noexcept
{
    switch (status) {
    case etfdt::dataservice::ShellAccountingBrokerOrderStatus::Disabled:
        return "DISABLED";
    case etfdt::dataservice::ShellAccountingBrokerOrderStatus::DryRunOnly:
        return "DRY_RUN_ONLY";
    case etfdt::dataservice::ShellAccountingBrokerOrderStatus::Rejected:
        return "REJECTED";
    case etfdt::dataservice::ShellAccountingBrokerOrderStatus::AcceptedPlaceholder:
        return "ACCEPTED_PLACEHOLDER";
    case etfdt::dataservice::ShellAccountingBrokerOrderStatus::PendingPlaceholder:
        return "PENDING_PLACEHOLDER";
    case etfdt::dataservice::ShellAccountingBrokerOrderStatus::UnknownStatePlaceholder:
        return "UNKNOWN_STATE_PLACEHOLDER";
    case etfdt::dataservice::ShellAccountingBrokerOrderStatus::Error:
        return "ERROR";
    }
    return "ERROR";
}

void appendSnapshotRebuildPreview(
    std::ostringstream& stream,
    const etfdt::accounting::AccountingReplayResult& result)
{
    const auto cashSummaries = replayCashSummariesForPreview(result);
    stream << "\"snapshotRebuildPreview\":{"
           << "\"implemented\":true,"
           << "\"previewOnly\":true,"
           << "\"source\":\"TASK-139 read-only replay\","
           << "\"input\":\"readonlyReplay\","
           << "\"output\":\"in-memory derived snapshot payload\","
           << "\"databaseWritten\":false,"
           << "\"snapshotWritten\":false,"
           << "\"tradeLogModified\":false,"
           << "\"tradeDraftGenerated\":false,"
           << "\"tradeSuggestionGenerated\":false,"
           << "\"strategyExecuted\":false,"
           << "\"brokerOrderSubmitted\":false,"
           << "\"status\":" << jsonStringValue(snapshotPreviewStatus(result)) << ','
           << "\"dataQualityStatus\":" << jsonStringValue(replayDataQualityStatus(result)) << ','
           << "\"positionSnapshots\":"
           << jsonArray(result.positionList.positions, writeSnapshotPreviewPosition)
           << ",\"cashSnapshots\":" << jsonArray(cashSummaries, writeSnapshotPreviewCash)
           << ",\"portfolioSummaries\":";
    if (result.hasPortfolioPnl) {
        stream << '[';
        writeSnapshotPreviewPortfolio(stream, result.portfolioPnl);
        stream << ']';
    } else {
        stream << "[]";
    }
    stream << ",\"issues\":" << replayIssuesJson(result.issues)
           << ",\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
           << "\"rawReplayPayloadExposed\":false,\"internalStackExposed\":false}"
           << "},";
}

void appendReplayPrivacyAndPreview(
    std::ostringstream& stream,
    const etfdt::accounting::AccountingReplayResult& result)
{
    stream << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
           << "\"rawReplayPayloadExposed\":false},";
    appendSnapshotRebuildPreview(stream, result);
}

std::string shellAccountingReplayErrorPayload(
    const char* action,
    const char* errorCode)
{
    std::ostringstream stream;
    stream << shellAccountingPayloadPrefix(action, "QUERY_ERROR", "ERROR", false, false, false)
           << "\"message\":\"ShellAccounting read-only replay input query failed.\","
           << "\"issues\":[";
    appendIssueObject(
        stream,
        "ERROR",
        errorCode,
        "ShellAccounting read-only replay input query failed without exposing raw SQL or payload.",
        true);
    stream << "],\"warnings\":[],\"errors\":[],"
           << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
           << "\"rawReplayPayloadExposed\":false}"
           << "}";
    return stream.str();
}

std::string shellAccountingEmptyPayload(
    const char* action,
    const char* emptyCode,
    const char* outputField,
    const char* emptyValue)
{
    std::ostringstream stream;
    stream << shellAccountingPayloadPrefix(action, "EMPTY", "OK", false)
           << "\"message\":\"ShellAccounting read-only facts query returned no rows.\","
           << "\"issues\":[";
    appendIssueObject(stream, "WARNING", emptyCode, "No read-only ShellAccounting facts are available.", false);
    stream << "],\"warnings\":[],\"errors\":[],"
           << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false},"
           << '"' << outputField << "\":" << emptyValue << "}";
    return stream.str();
}

std::string shellAccountingQueryErrorPayload(
    const char* action,
    const char* errorCode)
{
    std::ostringstream stream;
    stream << shellAccountingPayloadPrefix(action, "QUERY_ERROR", "ERROR", false)
           << "\"message\":\"ShellAccounting read-only facts query failed.\","
           << "\"issues\":[";
    appendIssueObject(
        stream,
        "ERROR",
        errorCode,
        "ShellAccounting read-only facts query failed without exposing raw SQL or payload.",
        true);
    stream << "],\"warnings\":[],\"errors\":[],"
           << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false}"
           << "}";
    return stream.str();
}

void writePositionRow(
    std::ostringstream& stream,
    const etfdt::data_access::ShellAccountingPositionRow& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "strategyCode", row.strategyCode, comma);
    appendJsonField(stream, "instrumentCode", row.instrumentCode, comma);
    appendJsonField(stream, "tradeSource", row.tradeSource, comma);
    appendJsonField(stream, "quantityText", row.quantityText, comma);
    appendJsonField(stream, "costAmountText", row.costAmountText, comma);
    appendJsonField(stream, "marketValueText", row.marketValueText, comma);
    appendJsonField(stream, "unrealizedPnlText", row.unrealizedPnlText, comma);
    appendJsonField(stream, "updatedAtUtc", row.updatedAtUtc, comma);
    stream << '}';
}

void writeCashSummaryRow(
    std::ostringstream& stream,
    const etfdt::data_access::ShellAccountingCashSummaryRow& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "cashBalanceText", row.cashBalanceText, comma);
    appendJsonField(stream, "principalBaseText", row.principalBaseText, comma);
    appendJsonField(stream, "updatedAtUtc", row.updatedAtUtc, comma);
    stream << '}';
}

void writePortfolioSummaryRow(
    std::ostringstream& stream,
    const etfdt::data_access::ShellAccountingPortfolioSummaryRow& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "totalAssetsText", row.totalAssetsText, comma);
    appendJsonField(stream, "totalMarketValueText", row.totalMarketValueText, comma);
    appendJsonField(stream, "cashBalanceText", row.cashBalanceText, comma);
    appendJsonField(stream, "principalBaseText", row.principalBaseText, comma);
    appendJsonField(stream, "holdingCostText", row.holdingCostText, comma);
    appendJsonField(stream, "totalPnlText", row.totalPnlText, comma);
    appendJsonField(stream, "totalReturnText", row.totalReturnText, comma);
    appendJsonField(stream, "baseCompletionRatioText", row.baseCompletionRatioText, comma);
    appendJsonField(stream, "sniperPoolText", row.sniperPoolText, comma);
    appendJsonField(stream, "activeGridCycleCount", row.activeGridCycleCount, comma);
    appendJsonField(stream, "activeDraftCount", row.activeDraftCount, comma);
    appendJsonField(stream, "baseViolationCount", row.baseViolationCount, comma);
    appendJsonField(stream, "updatedAtUtc", row.updatedAtUtc, comma);
    stream << '}';
}

void writeBasePositionSummaryRow(
    std::ostringstream& stream,
    const etfdt::data_access::ShellAccountingBasePositionSummaryRow& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "strategyCode", row.strategyCode, comma);
    appendJsonField(stream, "instrumentCode", row.instrumentCode, comma);
    appendJsonField(stream, "targetBaseAmountText", row.targetBaseAmountText, comma);
    appendJsonField(stream, "allocatedAmountText", row.allocatedAmountText, comma);
    appendJsonField(stream, "updatedAtUtc", row.updatedAtUtc, comma);
    stream << '}';
}

void writeSniperPoolTierRow(
    std::ostringstream& stream,
    const etfdt::data_access::ShellAccountingSniperPoolTierRow& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "strategyCode", row.strategyCode, comma);
    appendJsonField(stream, "tierNo", row.tierNo, comma);
    appendJsonField(stream, "tierName", row.tierName, comma);
    appendJsonField(stream, "allocatedAmountText", row.allocatedAmountText, comma);
    appendJsonField(stream, "updatedAtUtc", row.updatedAtUtc, comma);
    stream << '}';
}

void writeReplayPositionRow(
    std::ostringstream& stream,
    const etfdt::accounting::PositionSummaryDto& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "instrumentCode", row.instrumentCode, comma);
    appendJsonField(stream, "quantityText", row.quantityText, comma);
    appendJsonField(stream, "costAmountText", row.costAmountText, comma);
    appendJsonField(stream, "costPriceText", row.costPriceText, comma);
    appendJsonField(stream, "marketValueText", row.marketValueText, comma);
    appendJsonField(stream, "unrealizedPnlText", row.unrealizedPnlText, comma);
    appendJsonField(stream, "currency", row.currency, comma);
    appendJsonField(stream, "dataQualityStatus", row.dataQualityStatus, comma);
    stream << '}';
}

void writeReplayCashSummary(
    std::ostringstream& stream,
    const etfdt::accounting::CashSummaryDto& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "accountId", row.accountId, comma);
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "currency", row.currency, comma);
    appendJsonField(stream, "cashBalanceText", row.cashBalanceText, comma);
    appendJsonField(stream, "dataQualityStatus", row.dataQualityStatus, comma);
    stream << '}';
}

void writeReplayPortfolioPnl(
    std::ostringstream& stream,
    const etfdt::accounting::PortfolioPnlDto& row)
{
    bool comma = false;
    stream << '{';
    appendJsonField(stream, "portfolioId", row.portfolioId, comma);
    appendJsonField(stream, "currency", row.currency, comma);
    appendJsonField(stream, "realizedPnlText", row.realizedPnlText, comma);
    appendJsonField(stream, "unrealizedPnlText", row.unrealizedPnlText, comma);
    appendJsonField(stream, "totalAssetsText", row.totalAssetsText, comma);
    appendJsonField(stream, "totalPnlText", row.totalPnlText, comma);
    appendJsonField(stream, "dataQualityStatus", row.dataQualityStatus, comma);
    stream << '}';
}

std::string replayDataQualityStatus(const etfdt::accounting::AccountingReplayResult& result)
{
    if (!result.positionList.dataQualityStatus.empty()) {
        return result.positionList.dataQualityStatus;
    }
    if (result.hasCashSummary && !result.cashSummary.dataQualityStatus.empty()) {
        return result.cashSummary.dataQualityStatus;
    }
    if (result.hasPortfolioPnl && !result.portfolioPnl.dataQualityStatus.empty()) {
        return result.portfolioPnl.dataQualityStatus;
    }
    return result.status.empty() ? "UNKNOWN" : result.status;
}

bool replayHasBlockingIssue(const etfdt::accounting::AccountingReplayResult& result)
{
    for (const auto& issue : result.issues) {
        if (issue.blocking) {
            return true;
        }
    }
    return false;
}

std::string replayStatusForPayload(const etfdt::accounting::AccountingReplayResult& result)
{
    if (result.status == etfdt::accounting::AccountingReplayStatus::Ok
        || result.status == etfdt::accounting::AccountingReplayStatus::Warning) {
        return result.status;
    }
    return replayHasBlockingIssue(result) ? "ERROR" : "UNAVAILABLE";
}

struct AuditPayloadParseResult final {
    bool ok = false;
    etfdt::protocol::ErrorCode errorCode = etfdt::protocol::ErrorCode::E1001_INVALID_JSON;
    std::string errorMessage;
    etfdt::data_access::AuditLogEntry entry;
};

AuditPayloadParseResult parseAuditAppendPayload(const std::string& payloadJson)
{
    AuditPayloadParseResult result;
    const std::string trimmedPayload = trimCopy(payloadJson);
    if (!etfdt::protocol::isLikelyJsonObjectOrArray(payloadJson) || trimmedPayload.empty()
        || trimmedPayload.front() != '{') {
        result.errorCode = etfdt::protocol::ErrorCode::E1001_INVALID_JSON;
        result.errorMessage = "data.audit.append payload must be a JSON object";
        return result;
    }

    const auto entityType = extractJsonStringField(payloadJson, "entityType");
    if (!entityType.has_value() || trimCopy(*entityType).empty()) {
        result.errorCode = etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD;
        result.errorMessage = "data.audit.append requires payload.entityType";
        return result;
    }

    const auto action = extractJsonStringField(payloadJson, "action");
    if (!action.has_value() || trimCopy(*action).empty()) {
        result.errorCode = etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD;
        result.errorMessage = "data.audit.append requires payload.action";
        return result;
    }

    const auto reason = extractJsonStringField(payloadJson, "reason");
    if (!reason.has_value() || trimCopy(*reason).empty()) {
        result.errorCode = etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD;
        result.errorMessage = "data.audit.append requires payload.reason";
        return result;
    }

    std::string oldValueJson = "{}";
    if (const auto oldValue = extractJsonFragmentField(payloadJson, "oldValue"); oldValue.has_value()) {
        if (oldValue->empty() || !etfdt::protocol::isLikelyJsonObjectOrArray(*oldValue)) {
            result.errorCode = etfdt::protocol::ErrorCode::E1001_INVALID_JSON;
            result.errorMessage = "data.audit.append oldValue must be a JSON object or array";
            return result;
        }
        oldValueJson = *oldValue;
    }

    std::string newValueJson = "{}";
    if (const auto newValue = extractJsonFragmentField(payloadJson, "newValue"); newValue.has_value()) {
        if (newValue->empty() || !etfdt::protocol::isLikelyJsonObjectOrArray(*newValue)) {
            result.errorCode = etfdt::protocol::ErrorCode::E1001_INVALID_JSON;
            result.errorMessage = "data.audit.append newValue must be a JSON object or array";
            return result;
        }
        newValueJson = *newValue;
    }

    result.entry.entityType = trimCopy(*entityType);
    result.entry.action = trimCopy(*action);
    result.entry.reason = trimCopy(*reason);
    result.entry.oldValueJson = std::move(oldValueJson);
    result.entry.newValueJson = std::move(newValueJson);

    if (const auto entityId = extractJsonStringField(payloadJson, "entityId"); entityId.has_value()) {
        result.entry.entityId = parseOptionalInt64(*entityId);
    }
    if (const auto operatorName = extractJsonStringField(payloadJson, "operatorName");
        operatorName.has_value() && !trimCopy(*operatorName).empty()) {
        result.entry.operatorName = trimCopy(*operatorName);
    }

    result.ok = true;
    return result;
}

}  // namespace

etfdt::protocol::ProtocolResponse handleDataHealth(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    auto health = connection.healthCheck();
    if (!health) {
        return errorResponse(context, health);
    }

    etfdt::data_access::SchemaVersionRepository schemaVersions(connection);
    auto versions = schemaVersions.listVersions();
    if (!versions) {
        return errorResponse(context, versions);
    }

    const auto& status = health.value();
    const bool healthy = status.opened && status.walEnabled && status.foreignKeysEnabled
        && status.schemaVersionPresent && status.initialMigrationApplied
        && status.missingCoreTables.empty();

    std::ostringstream payload;
    payload << "{"
            << "\"healthy\": " << (healthy ? "true" : "false") << ','
            << "\"schemaVersion\": " << jsonStringValue(normalizedSchemaName(versions.value()))
            << ','
            << "\"walEnabled\": " << (status.walEnabled ? "true" : "false") << ','
            << "\"foreignKeysEnabled\": " << (status.foreignKeysEnabled ? "true" : "false")
            << '}';
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleDataSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::SchemaVersionRepository schemaVersions(connection);
    etfdt::data_access::AccountRepository accounts(connection);
    etfdt::data_access::PortfolioRepository portfolios(connection);
    etfdt::data_access::InstrumentRepository instruments(connection);
    etfdt::data_access::StrategyRepository strategies(connection);
    etfdt::data_access::OtcChannelRepository otcChannels(connection);

    auto versions = schemaVersions.listVersions();
    if (!versions) {
        return errorResponse(context, versions);
    }
    auto activeAccount = accounts.getActiveAccount();
    if (!activeAccount) {
        return errorResponse(context, activeAccount);
    }
    auto activePortfolio = portfolios.getActivePortfolio();
    if (!activePortfolio) {
        return errorResponse(context, activePortfolio);
    }
    auto enabledInstruments = instruments.listEnabledInstruments();
    if (!enabledInstruments) {
        return errorResponse(context, enabledInstruments);
    }
    auto enabledStrategies = strategies.listEnabledStrategies();
    if (!enabledStrategies) {
        return errorResponse(context, enabledStrategies);
    }
    auto otcCount = otcChannels.countAll();
    if (!otcCount) {
        return errorResponse(context, otcCount);
    }

    std::ostringstream payload;
    payload << "{"
            << "\"activeAccount\": " << accountToJson(activeAccount.value()) << ','
            << "\"activePortfolio\": " << portfolioToJson(activePortfolio.value()) << ','
            << "\"enabledInstrumentCount\": " << enabledInstruments.value().size() << ','
            << "\"enabledStrategyCount\": " << enabledStrategies.value().size() << ','
            << "\"otcChannelCount\": " << otcCount.value() << ','
            << "\"schemaVersion\": " << jsonStringValue(normalizedSchemaName(versions.value()))
            << '}';
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleDataAccountsList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::AccountRepository repository(connection);
    auto records = repository.listAccounts();
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(context, "{\"accounts\":" + accountsToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataPortfoliosList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::PortfolioRepository repository(connection);
    auto records = repository.listPortfolios();
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(
        context,
        "{\"portfolios\":" + portfoliosToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataInstrumentsList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::InstrumentRepository repository(connection);
    auto records = repository.listEnabledInstruments();
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(
        context,
        "{\"instruments\":" + instrumentsToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataStrategiesList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::StrategyRepository repository(connection);
    auto records = repository.listEnabledStrategies();
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(
        context,
        "{\"strategies\":" + strategiesToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataOtcList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    const std::string strategyCode = extractStrategyCode(context.request.payloadJson);
    if (strategyCode.empty()) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "data.otc.list requires payload.strategyCode");
    }

    etfdt::data_access::OtcChannelRepository repository(connection);
    auto records = repository.listAllByStrategyCode(strategyCode);
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(
        context,
        "{\"strategyCode\":" + jsonStringValue(strategyCode)
            + ",\"channels\":" + otcChannelsToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataAuditAppend(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    auto parsed = parseAuditAppendPayload(context.request.payloadJson);
    if (!parsed.ok) {
        return protocolErrorResponse(context, parsed.errorCode, std::move(parsed.errorMessage));
    }

    etfdt::data_access::AuditLogRepository auditLogs(connection);
    etfdt::data_access::TransactionRunner runner(connection);
    std::int64_t auditLogId = 0;

    auto transactionResult = runner.runInTransaction([&]() {
        auto inserted = auditLogs.insertAuditLog(parsed.entry);
        if (!inserted) {
            return etfdt::data_access::DatabaseResult<bool>::failure(
                inserted.error().errorCode,
                inserted.error().message);
        }
        auditLogId = inserted.value();
        return etfdt::data_access::DatabaseResult<bool>::success(true);
    });
    if (!transactionResult) {
        return errorResponse(context, transactionResult);
    }

    std::ostringstream payload;
    payload << "{"
            << "\"inserted\": true,"
            << "\"auditLogId\": " << auditLogId << ','
            << "\"entityType\": " << jsonStringValue(parsed.entry.entityType) << ','
            << "\"action\": " << jsonStringValue(parsed.entry.action)
            << '}';
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleAccountingHealth(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;

    const std::string payload = R"json({
"module":"accounting",
"healthy":true,
"readOnly":true,
"contractVersion":"0.5-readonly-facts",
"calculationVersion":"readonly-facts-query-v1",
"replayImplemented":false,
"snapshotImplemented":false,
"writeEnabled":false,
"implementedActions":[
"accounting.health",
"position.list",
"cash.summary",
"portfolio.pnl.summary",
"base_position.summary",
"sniper_pool.summary"
],
"futureActions":[
"accounting.replay.preview"
],
"sourceOfTruth":"trade_log",
"derivedTables":[
"cash_snapshot",
"position_snapshot",
"portfolio_summary"
],
"boundaries":[
"trade_log is the factual ledger",
"snapshots are derived data",
"this action does not replay accounting",
"this action does not write database tables",
"QML must not calculate accounting fields"
],
"warnings":[
{"code":"REPLAY_NOT_IMPLEMENTED","message":"Accounting replay is not implemented yet.","blocking":false}
],
"errors":[]
})json";

    return successResponse(context, payload);
}

etfdt::protocol::ProtocolResponse handleAccountingReplayPreview(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;

    const std::string trimmedPayload = trimCopy(context.request.payloadJson);
    if (!etfdt::protocol::isLikelyJsonObjectOrArray(context.request.payloadJson)
        || trimmedPayload.empty() || trimmedPayload.front() != '{') {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "accounting.replay.preview payload must be a JSON object");
    }

    const auto accountId = extractJsonStringField(context.request.payloadJson, "accountId").value_or("");
    const auto portfolioId = extractJsonStringField(context.request.payloadJson, "portfolioId").value_or("");
    const auto sourceFromTime =
        extractJsonStringField(context.request.payloadJson, "sourceFromTime").value_or("");
    const auto sourceToTime =
        extractJsonStringField(context.request.payloadJson, "sourceToTime").value_or("");

    std::string requestedOutputsJson = "[]";
    if (const auto requestedOutputs = extractJsonFragmentField(context.request.payloadJson, "requestedOutputs");
        requestedOutputs.has_value() && !requestedOutputs->empty()
        && requestedOutputs->front() == '[') {
        requestedOutputsJson = *requestedOutputs;
    }

    std::ostringstream payload;
    payload << "{"
            << "\"module\":\"accounting\","
            << "\"action\":\"accounting.replay.preview\","
            << "\"implemented\":false,"
            << "\"readOnly\":true,"
            << "\"replayExecuted\":false,"
            << "\"writeEnabled\":false,"
            << "\"contractVersion\":\"0.3-draft\","
            << "\"calculationVersion\":\"not-implemented\","
            << "\"status\":\"REPLAY_NOT_AVAILABLE\","
            << "\"message\":\"Accounting replay preview is not implemented yet.\","
            << "\"requestedScope\":{"
            << "\"accountId\":" << jsonStringValue(accountId) << ','
            << "\"portfolioId\":" << jsonStringValue(portfolioId) << ','
            << "\"sourceFromTime\":" << jsonStringValue(sourceFromTime) << ','
            << "\"sourceToTime\":" << jsonStringValue(sourceToTime) << ','
            << "\"requestedOutputs\":" << requestedOutputsJson
            << "},"
            << "\"requiredFixtures\":["
            << "\"FX001_EMPTY_LEDGER\","
            << "\"FX002_SINGLE_BUY\","
            << "\"FX003_BUY_SELL_PARTIAL\","
            << "\"FX004_SELL_EXCEEDS_POSITION\","
            << "\"FX005_MISSING_FEE\","
            << "\"FX006_NEGATIVE_CASH\","
            << "\"FX007_MULTI_INSTRUMENT\","
            << "\"FX008_MULTI_ACCOUNT\","
            << "\"FX009_BASE_POSITION_LOCKED\","
            << "\"FX010_SNIPER_TIER_COMPLETED\","
            << "\"FX011_STALE_SNAPSHOT\","
            << "\"FX012_MISSING_MARKET_PRICE\","
            << "\"FX013_MULTI_CURRENCY_UNSUPPORTED\""
            << "],"
            << "\"futureOutputs\":["
            << "\"PositionListResponse\","
            << "\"CashSummaryDto\","
            << "\"PortfolioPnlDto\","
            << "\"BasePositionDto\","
            << "\"SniperPoolDto\","
            << "\"AccountingIssueDto\""
            << "],"
            << "\"forbiddenWrites\":["
            << "\"trade_log\","
            << "\"trade_execution_group\","
            << "\"trade_draft\","
            << "\"cash_snapshot\","
            << "\"position_snapshot\","
            << "\"portfolio_summary\","
            << "\"audit_log\""
            << "],"
            << "\"boundaries\":["
            << "\"trade_log is the factual ledger\","
            << "\"snapshots are derived data\","
            << "\"this action does not replay accounting yet\","
            << "\"this action does not write database tables\","
            << "\"this action does not return calculated positions, cash, or pnl\""
            << "],"
            << "\"nextRequiredTask\":\"implement fixture-backed accounting replay before returning preview results\","
            << "\"warnings\":["
            << "{\"code\":\"REPLAY_NOT_IMPLEMENTED\","
            << "\"message\":\"Fixture-backed accounting replay must be implemented in a future task.\","
            << "\"blocking\":false}"
            << "],"
            << "\"errors\":["
            << "{\"code\":\"REPLAY_NOT_AVAILABLE\","
            << "\"message\":\"Accounting replay preview is not available yet.\","
            << "\"blocking\":true}"
            << "]"
            << "}";

    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleAccountingSnapshotWrite(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "accounting.snapshot.write payload must be a JSON object");
    }

    if (extractJsonBoolField(context.request.payloadJson, "snapshotWriteDisabled").value_or(false)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "accounting.snapshot.write is disabled by request");
    }

    const auto authorization =
        extractJsonStringField(context.request.payloadJson, "authorization").value_or("");
    if (authorization != "TASK-144_SNAPSHOT_WRITE") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "accounting.snapshot.write requires TASK-144_SNAPSHOT_WRITE authorization");
    }

    const auto source = extractJsonStringField(context.request.payloadJson, "source").value_or("");
    if (source != "snapshotRebuildPreview") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.snapshot.write requires payload.source=snapshotRebuildPreview");
    }

    if (!shellAccountingReplayRequested(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.snapshot.write requires payload.calculationMode=readonlyReplay");
    }

    const auto request = shellAccountingQueryRequest(context.request.payloadJson);
    if (!parseOptionalInt64(request.accountId).has_value()
        || !parseOptionalInt64(request.portfolioId).has_value()) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.snapshot.write requires numeric accountId and portfolioId");
    }

    const auto replay = runShellAccountingReadOnlyReplay(
        connection,
        "portfolio.pnl.summary",
        context.request.payloadJson);
    if (replay.queryFailed) {
        std::ostringstream payload;
        payload << "{"
                << "\"module\":\"accounting\","
                << "\"action\":\"accounting.snapshot.write\","
                << "\"implemented\":true,"
                << "\"writeAuthorized\":true,"
                << "\"databaseWritten\":false,"
                << "\"snapshotWritten\":false,"
                << "\"partialWrite\":false,"
                << "\"transactionRolledBack\":true,"
                << "\"status\":\"PREVIEW_UNAVAILABLE\","
                << "\"errorCode\":" << jsonStringValue(replay.queryErrorCode) << ','
                << "\"issues\":[";
        appendIssueObject(
            payload,
            "ERROR",
            replay.queryErrorCode,
            "Snapshot write preview input query failed.",
            true);
        payload << "],\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
                << "\"internalStackExposed\":false}"
                << "}";
        return successResponse(context, payload.str());
    }

    if (!replay.requested || snapshotPreviewStatus(replay.result) != "READY") {
        std::ostringstream payload;
        payload << "{"
                << "\"module\":\"accounting\","
                << "\"action\":\"accounting.snapshot.write\","
                << "\"implemented\":true,"
                << "\"writeAuthorized\":true,"
                << "\"databaseWritten\":false,"
                << "\"snapshotWritten\":false,"
                << "\"partialWrite\":false,"
                << "\"transactionRolledBack\":false,"
                << "\"status\":\"PREVIEW_UNAVAILABLE\","
                << "\"issues\":" << replayIssuesJson(replay.result.issues) << ','
                << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
                << "\"internalStackExposed\":false}"
                << "}";
        return successResponse(context, payload.str());
    }

    etfdt::data_access::ShellAccountingSnapshotWriteRepository repository(connection);
    auto batch = snapshotWriteBatchFromReplay(replay.result, request, context.request.timestampUtc);
    auto writeResult = repository.writeSnapshotBatch(batch);
    if (!writeResult) {
        return errorResponse(context, writeResult);
    }

    std::ostringstream payload;
    payload << "{"
            << "\"module\":\"accounting\","
            << "\"action\":\"accounting.snapshot.write\","
            << "\"implemented\":true,"
            << "\"writeAuthorized\":true,"
            << "\"source\":\"snapshotRebuildPreview\","
            << "\"input\":\"TASK-142 snapshotRebuildPreview\","
            << "\"databaseWritten\":true,"
            << "\"snapshotWritten\":true,"
            << "\"partialWrite\":false,"
            << "\"transactionCommitted\":"
            << (writeResult.value().transactionCommitted ? "true" : "false") << ','
            << "\"transactionRolledBack\":false,"
            << "\"idempotent\":"
            << (writeResult.value().idempotentUpsert ? "true" : "false") << ','
            << "\"duplicateHandling\":\"UPSERT_BY_UID\","
            << "\"status\":\"OK\","
            << "\"allowlistTables\":[\"cash_snapshot\",\"position_snapshot\",\"portfolio_summary\"],"
            << "\"forbiddenTables\":[\"trade_log\",\"trade_execution_group\",\"trade_draft\",\"audit_log\"],"
            << "\"rowsWritten\":{"
            << "\"cash_snapshot\":" << writeResult.value().cashRows << ','
            << "\"position_snapshot\":" << writeResult.value().positionRows << ','
            << "\"portfolio_summary\":" << writeResult.value().portfolioSummaryRows
            << "},"
            << "\"tradeLogModified\":false,"
            << "\"tradeDraftGenerated\":false,"
            << "\"tradeSuggestionGenerated\":false,"
            << "\"strategyExecuted\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"issues\":[],"
            << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
            << "\"internalStackExposed\":false}"
            << "}";
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleAccountingAuditWrite(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "accounting.audit.write payload must be a JSON object");
    }

    if (extractJsonBoolField(context.request.payloadJson, "auditWriteDisabled").value_or(false)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "accounting.audit.write is disabled by request");
    }

    const auto authorization =
        extractJsonStringField(context.request.payloadJson, "authorization").value_or("");
    if (authorization != "TASK-146_AUDIT_WRITE") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "accounting.audit.write requires TASK-146_AUDIT_WRITE authorization");
    }

    const auto source = extractJsonStringField(context.request.payloadJson, "source").value_or("");
    if (source != "snapshotWriteResult") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.audit.write requires payload.source=snapshotWriteResult");
    }

    const auto sourceAction =
        extractJsonStringField(context.request.payloadJson, "sourceAction").value_or("");
    if (sourceAction != "accounting.snapshot.write") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.audit.write requires sourceAction=accounting.snapshot.write");
    }

    const auto resultStatus =
        extractJsonStringField(context.request.payloadJson, "resultStatus").value_or("");
    const auto snapshotWritten =
        extractJsonBoolField(context.request.payloadJson, "snapshotWritten").value_or(false);
    if (resultStatus != "OK" || !snapshotWritten) {
        std::ostringstream payload;
        payload << "{"
                << "\"module\":\"accounting\","
                << "\"action\":\"accounting.audit.write\","
                << "\"implemented\":true,"
                << "\"writeAuthorized\":true,"
                << "\"auditWritten\":false,"
                << "\"databaseWritten\":false,"
                << "\"partialAuditWrite\":false,"
                << "\"transactionRolledBack\":false,"
                << "\"status\":\"SOURCE_WRITE_FAILED\","
                << "\"issues\":[";
        appendIssueObject(payload, "ERROR", "SOURCE_WRITE_FAILED",
                          "Audit write requires a successful snapshot write result.", true);
        payload << "],"
                << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
                << "\"fullSnapshotPayloadExposed\":false,\"internalStackExposed\":false},"
                << "\"tradeLogModified\":false,"
                << "\"tradeDraftGenerated\":false,"
                << "\"tradeSuggestionGenerated\":false,"
                << "\"strategyExecuted\":false,"
                << "\"brokerOrderSubmitted\":false"
                << "}";
        return successResponse(context, payload.str());
    }

    auto request = auditWriteRequestFromPayload(context.request.payloadJson, context.request.timestampUtc);
    etfdt::data_access::ShellAccountingAuditWriteRepository repository(connection);
    auto writeResult = repository.writeSnapshotWriteAudit(request);
    if (!writeResult) {
        return errorResponse(context, writeResult);
    }

    std::ostringstream payload;
    payload << "{"
            << "\"module\":\"accounting\","
            << "\"action\":\"accounting.audit.write\","
            << "\"implemented\":true,"
            << "\"writeAuthorized\":true,"
            << "\"source\":\"snapshotWriteResult\","
            << "\"input\":\"TASK-144 snapshot write result\","
            << "\"authorization\":\"TASK-146_AUDIT_WRITE\","
            << "\"auditWritten\":" << (writeResult.value().auditWritten ? "true" : "false") << ','
            << "\"databaseWritten\":" << (writeResult.value().auditWritten ? "true" : "false") << ','
            << "\"partialAuditWrite\":false,"
            << "\"transactionCommitted\":" << (writeResult.value().transactionCommitted ? "true" : "false") << ','
            << "\"transactionRolledBack\":false,"
            << "\"idempotent\":" << (writeResult.value().idempotent ? "true" : "false") << ','
            << "\"duplicateEvent\":" << (writeResult.value().duplicateEvent ? "true" : "false") << ','
            << "\"duplicateHandling\":\"SKIP_EXISTING_BY_SOURCE_RESULT\","
            << "\"status\":" << jsonStringValue(writeResult.value().duplicateEvent ? "DUPLICATE" : "OK") << ','
            << "\"auditLogId\":" << writeResult.value().auditLogId << ','
            << "\"allowlistTables\":[\"audit_log\"],"
            << "\"forbiddenTables\":[\"trade_log\",\"trade_execution_group\",\"trade_draft\"],"
            << "\"payloadPolicy\":{"
            << "\"sanitized\":true,"
            << "\"rawSqlExposed\":false,"
            << "\"rawTradeLogPayloadExposed\":false,"
            << "\"fullSnapshotPayloadExposed\":false,"
            << "\"internalStackExposed\":false"
            << "},"
            << "\"tradeLogModified\":false,"
            << "\"tradeDraftGenerated\":false,"
            << "\"tradeSuggestionGenerated\":false,"
            << "\"strategyExecuted\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"issues\":[],"
            << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
            << "\"fullSnapshotPayloadExposed\":false,\"internalStackExposed\":false}"
            << "}";
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleAccountingTradeDraftCreate(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "accounting.tradedraft.create payload must be a JSON object");
    }

    if (extractJsonBoolField(context.request.payloadJson, "tradeDraftDisabled").value_or(false)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "accounting.tradedraft.create is disabled by request");
    }

    const auto authorization =
        extractJsonStringField(context.request.payloadJson, "authorization").value_or("");
    if (authorization != "TASK-148_TRADEDRAFT_WRITE") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "accounting.tradedraft.create requires TASK-148_TRADEDRAFT_WRITE authorization");
    }

    const auto source = extractJsonStringField(context.request.payloadJson, "source").value_or("");
    if (source != "snapshotRebuildPreview" && source != "readonlyReplay") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.tradedraft.create requires source=snapshotRebuildPreview or source=readonlyReplay");
    }

    if (!tradeDraftRiskFlagsPresent(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.tradedraft.create requires structured riskFlags");
    }

    if (tradeDraftRiskBlocked(context.request.payloadJson)) {
        std::ostringstream payload;
        payload << "{"
                << "\"module\":\"accounting\","
                << "\"action\":\"accounting.tradedraft.create\","
                << "\"implemented\":true,"
                << "\"writeAuthorized\":true,"
                << "\"draftWritten\":false,"
                << "\"auditWritten\":false,"
                << "\"databaseWritten\":false,"
                << "\"transactionRolledBack\":false,"
                << "\"status\":\"RISK_CHECK_BLOCKED\","
                << "\"issues\":[";
        appendIssueObject(
            payload,
            "ERROR",
            "RISK_CHECK_BLOCKED",
            "TradeDraft creation was blocked by risk flags.",
            true);
        payload << "],"
                << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
                << "\"fullSnapshotPayloadExposed\":false,\"internalStackExposed\":false},"
                << "\"tradeLogModified\":false,"
                << "\"tradeExecutionGroupModified\":false,"
                << "\"tradeSuggestionGenerated\":false,"
                << "\"strategyExecuted\":false,"
                << "\"brokerOrderSubmitted\":false"
                << "}";
        return successResponse(context, payload.str());
    }

    auto request = tradeDraftRequestFromPayload(context.request.payloadJson, context.request.timestampUtc);
    etfdt::data_access::ShellAccountingTradeDraftRepository repository(connection);
    auto writeResult = repository.createTradeDraft(request);
    if (!writeResult) {
        return errorResponse(context, writeResult);
    }

    std::ostringstream payload;
    payload << "{"
            << "\"module\":\"accounting\","
            << "\"action\":\"accounting.tradedraft.create\","
            << "\"implemented\":true,"
            << "\"writeAuthorized\":true,"
            << "\"source\":" << jsonStringValue(source) << ','
            << "\"authorization\":\"TASK-148_TRADEDRAFT_WRITE\","
            << "\"draftOnly\":true,"
            << "\"notExecution\":true,"
            << "\"draftWritten\":" << (writeResult.value().draftWritten ? "true" : "false") << ','
            << "\"auditWritten\":" << (writeResult.value().auditWritten ? "true" : "false") << ','
            << "\"databaseWritten\":"
            << ((writeResult.value().draftWritten || writeResult.value().auditWritten) ? "true" : "false") << ','
            << "\"partialWrite\":false,"
            << "\"transactionCommitted\":"
            << (writeResult.value().transactionCommitted ? "true" : "false") << ','
            << "\"transactionRolledBack\":false,"
            << "\"idempotent\":" << (writeResult.value().idempotent ? "true" : "false") << ','
            << "\"duplicateDraft\":" << (writeResult.value().duplicateDraft ? "true" : "false") << ','
            << "\"duplicateHandling\":\"SKIP_EXISTING_BY_DRAFT_SIGNATURE\","
            << "\"status\":" << jsonStringValue(writeResult.value().duplicateDraft ? "DUPLICATE" : "OK") << ','
            << "\"draftId\":" << writeResult.value().draftId << ','
            << "\"auditLogId\":" << writeResult.value().auditLogId << ','
            << "\"draftUid\":" << jsonStringValue(writeResult.value().draftUid) << ','
            << "\"draftSignature\":" << jsonStringValue(writeResult.value().draftSignature) << ','
            << "\"allowlistTables\":[\"trade_draft\",\"audit_log\"],"
            << "\"forbiddenTables\":[\"trade_log\",\"trade_execution_group\"],"
            << "\"payloadPolicy\":{"
            << "\"sanitized\":true,"
            << "\"rawSqlExposed\":false,"
            << "\"rawTradeLogPayloadExposed\":false,"
            << "\"fullSnapshotPayloadExposed\":false,"
            << "\"internalStackExposed\":false,"
            << "\"sensitiveRawPayloadExposed\":false"
            << "},"
            << "\"tradeLogModified\":false,"
            << "\"tradeExecutionGroupModified\":false,"
            << "\"tradeSuggestionGenerated\":false,"
            << "\"strategyExecuted\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"issues\":[],"
            << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
            << "\"fullSnapshotPayloadExposed\":false,\"internalStackExposed\":false,"
            << "\"sensitiveRawPayloadExposed\":false}"
            << "}";
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleAccountingTradeDraftConfirm(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "accounting.tradedraft.confirm payload must be a JSON object");
    }

    if (extractJsonBoolField(context.request.payloadJson, "confirmationDisabled").value_or(false)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "accounting.tradedraft.confirm is disabled by request");
    }

    const auto authorization =
        extractJsonStringField(context.request.payloadJson, "authorization").value_or("");
    if (authorization != "TASK-150_TRADEDRAFT_CONFIRM") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "accounting.tradedraft.confirm requires TASK-150_TRADEDRAFT_CONFIRM authorization");
    }

    const auto source = extractJsonStringField(context.request.payloadJson, "source").value_or("");
    if (source != "existingTradeDraft") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.tradedraft.confirm requires source=existingTradeDraft");
    }

    auto request = tradeDraftConfirmationRequestFromPayload(
        context.request.payloadJson,
        context.request.timestampUtc);
    etfdt::data_access::ShellAccountingTradeDraftConfirmationRepository repository(connection);
    auto writeResult = repository.confirmTradeDraft(request);
    if (!writeResult) {
        return errorResponse(context, writeResult);
    }

    const auto& result = writeResult.value();
    std::ostringstream payload;
    payload << "{"
            << "\"module\":\"accounting\","
            << "\"action\":\"accounting.tradedraft.confirm\","
            << "\"implemented\":true,"
            << "\"writeAuthorized\":true,"
            << "\"source\":\"existingTradeDraft\","
            << "\"authorization\":\"TASK-150_TRADEDRAFT_CONFIRM\","
            << "\"confirmationOnly\":true,"
            << "\"notBrokerOrder\":true,"
            << "\"notStrategyExecution\":true,"
            << "\"executionGroupWritten\":" << (result.executionGroupWritten ? "true" : "false") << ','
            << "\"tradeLogWritten\":" << (result.tradeLogWritten ? "true" : "false") << ','
            << "\"draftStatusUpdated\":" << (result.draftStatusUpdated ? "true" : "false") << ','
            << "\"auditWritten\":" << (result.auditWritten ? "true" : "false") << ','
            << "\"databaseWritten\":"
            << ((result.executionGroupWritten || result.tradeLogWritten || result.draftStatusUpdated
                 || result.auditWritten)
                    ? "true"
                    : "false")
            << ','
            << "\"partialWrite\":false,"
            << "\"transactionCommitted\":" << (result.transactionCommitted ? "true" : "false") << ','
            << "\"transactionRolledBack\":false,"
            << "\"idempotent\":" << (result.idempotent ? "true" : "false") << ','
            << "\"duplicateConfirmation\":" << (result.duplicateConfirmation ? "true" : "false") << ','
            << "\"duplicateHandling\":\"SKIP_EXISTING_BY_DRAFT_ID\","
            << "\"status\":" << jsonStringValue(result.duplicateConfirmation ? "DUPLICATE" : "OK") << ','
            << "\"draftId\":" << result.draftId << ','
            << "\"tradeExecutionGroupId\":" << result.executionGroupId << ','
            << "\"tradeLogId\":" << result.tradeLogId << ','
            << "\"auditLogId\":" << result.auditLogId << ','
            << "\"executionGroupUid\":" << jsonStringValue(result.executionGroupUid) << ','
            << "\"tradeLogUid\":" << jsonStringValue(result.tradeLogUid) << ','
            << "\"allowlistTables\":[\"trade_execution_group\",\"trade_log\",\"audit_log\",\"trade_draft\"],"
            << "\"forbiddenBehaviors\":[\"brokerOrder\",\"strategyExecute\",\"automaticTrading\",\"productionTradingUI\"],"
            << "\"payloadPolicy\":{"
            << "\"sanitized\":true,"
            << "\"rawSqlExposed\":false,"
            << "\"rawTradeLogPayloadExposed\":false,"
            << "\"fullDraftPayloadExposed\":false,"
            << "\"internalStackExposed\":false,"
            << "\"sensitiveRawPayloadExposed\":false"
            << "},"
            << "\"brokerOrderSubmitted\":false,"
            << "\"strategyExecuted\":false,"
            << "\"automaticTrading\":false,"
            << "\"productionTradingUiChanged\":false,"
            << "\"issues\":[],"
            << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
            << "\"fullDraftPayloadExposed\":false,\"internalStackExposed\":false,"
            << "\"sensitiveRawPayloadExposed\":false}"
            << "}";
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleAccountingBrokerOrderDryRun(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "accounting.broker_order.dry_run payload must be a JSON object");
    }

    if (extractJsonBoolField(context.request.payloadJson, "brokerOrderDisabled").value_or(false)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "accounting.broker_order.dry_run is disabled by request");
    }

    const auto authorization =
        extractJsonStringField(context.request.payloadJson, "authorization").value_or("");
    if (authorization != "TASK-154_BROKER_ORDER_DRY_RUN") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "accounting.broker_order.dry_run requires TASK-154_BROKER_ORDER_DRY_RUN authorization");
    }

    const auto source = extractJsonStringField(context.request.payloadJson, "source").value_or("");
    if (source != "confirmedTradeDraft" && source != "confirmedLedgerFact") {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "accounting.broker_order.dry_run requires source=confirmedTradeDraft or source=confirmedLedgerFact");
    }

    if (!extractJsonBoolField(context.request.payloadJson, "userConfirmed").value_or(false)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "accounting.broker_order.dry_run requires explicit user confirmation");
    }

    if (extractJsonFragmentField(context.request.payloadJson, "rawOrderPayload").has_value()
        || extractJsonStringField(context.request.payloadJson, "rawOrderPayload").has_value()) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "accounting.broker_order.dry_run rejects raw UI order payload");
    }

    auto request = brokerOrderDryRunRequestFromPayload(
        context.request.payloadJson,
        context.request.timestampUtc);
    etfdt::data_access::ShellAccountingBrokerOrderDryRunRepository repository(connection);
    auto dryRunResult = repository.dryRunBrokerOrder(request);
    if (!dryRunResult) {
        return errorResponse(context, dryRunResult);
    }

    const auto& result = dryRunResult.value();
    const auto& sandboxEnablementState =
        etfdt::dataservice::defaultShellAccountingBrokerSandboxRuntimeEnablementState();
    const auto sandboxEnablementDisabledFailClosed = !sandboxEnablementState.enabled
                                                    && !sandboxEnablementState.available
                                                    && sandboxEnablementState.failClosed;
    (void)sandboxEnablementDisabledFailClosed;
    const auto defaultBrokerPortMode =
        etfdt::dataservice::defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode();
    const auto brokerPortMode = etfdt::dataservice::shellAccountingBrokerRuntimeModeSourceForMode(
                                    defaultBrokerPortMode)
                                    .brokerOrderPortMode();
    const auto brokerPortResponse =
        etfdt::dataservice::shellAccountingBrokerOrderPortForMode(brokerPortMode).submitOrderEnvelope(
            brokerOrderPortRequestFromDryRun(request, result));
    std::ostringstream payload;
    payload << "{"
            << "\"module\":\"accounting\","
            << "\"action\":\"accounting.broker_order.dry_run\","
            << "\"implemented\":true,"
            << "\"writeAuthorized\":true,"
            << "\"source\":\"" << (source == "confirmedLedgerFact" ? "confirmedLedgerFact" : "confirmedTradeDraft")
            << "\","
            << "\"authorization\":\"TASK-154_BROKER_ORDER_DRY_RUN\","
            << "\"dryRun\":true,"
            << "\"brokerDisabled\":true,"
            << "\"dryRunOnly\":true,"
            << "\"realOrderPlacement\":false,"
            << "\"brokerSdkCalled\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"brokerOrderId\":null,"
            << "\"brokerPortMode\":" << jsonStringValue(brokerPortResponse.brokerMode) << ','
            << "\"brokerPortStatus\":" << jsonStringValue(brokerOrderStatusCode(brokerPortResponse.status)) << ','
            << "\"brokerPortDisabled\":" << (brokerPortResponse.disabled ? "true" : "false") << ','
            << "\"brokerPortDryRunOnly\":" << (brokerPortResponse.dryRunOnly ? "true" : "false") << ','
            << "\"brokerPortErrorCode\":" << jsonStringValue(brokerPortResponse.errorCode) << ','
            << "\"strategyExecuted\":false,"
            << "\"automaticTrading\":false,"
            << "\"auditWritten\":" << (result.auditWritten ? "true" : "false") << ','
            << "\"databaseWritten\":" << (result.auditWritten ? "true" : "false") << ','
            << "\"partialWrite\":false,"
            << "\"transactionCommitted\":" << (result.transactionCommitted ? "true" : "false") << ','
            << "\"transactionRolledBack\":false,"
            << "\"idempotent\":" << (result.idempotent ? "true" : "false") << ','
            << "\"duplicateDryRun\":" << (result.duplicateDryRun ? "true" : "false") << ','
            << "\"duplicateHandling\":\"SKIP_EXISTING_BY_CONFIRMED_TRADE\","
            << "\"status\":" << jsonStringValue(result.duplicateDryRun ? "DUPLICATE" : "DRY_RUN_ONLY") << ','
            << "\"draftId\":" << result.draftId << ','
            << "\"tradeExecutionGroupId\":" << result.executionGroupId << ','
            << "\"tradeLogId\":" << result.tradeLogId << ','
            << "\"auditLogId\":" << result.auditLogId << ','
            << "\"allowlistTables\":[\"audit_log\"],"
            << "\"forbiddenWrites\":[\"trade_log\",\"trade_execution_group\",\"trade_draft\","
            << "\"broker_order_real_status\"],"
            << "\"forbiddenBehaviors\":[\"brokerSdkCall\",\"realOrderPlacement\","
            << "\"strategyExecute\",\"automaticTrading\"],"
            << "\"payloadPolicy\":{"
            << "\"sanitized\":true,"
            << "\"rawSqlExposed\":false,"
            << "\"rawTradeLogPayloadExposed\":false,"
            << "\"fullTradeDraftPayloadExposed\":false,"
            << "\"fullBrokerPayloadExposed\":false,"
            << "\"internalStackExposed\":false"
            << "},"
            << "\"issues\":[],"
            << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
            << "\"fullTradeDraftPayloadExposed\":false,\"fullBrokerPayloadExposed\":false,"
            << "\"internalStackExposed\":false}"
            << "}";
    return successResponse(context, payload.str());
}

bool isManualEntryJsonObjectPayload(const std::string& payloadJson)
{
    const std::string trimmedPayload = trimCopy(payloadJson);
    return etfdt::protocol::isLikelyJsonObjectOrArray(payloadJson) && !trimmedPayload.empty()
        && trimmedPayload.front() == '{' && trimmedPayload.back() == '}';
}

std::string normalizedManualEntryEnumValue(std::string value)
{
    value = trimCopy(value);
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

ShellAccountingManualTradeSide manualTradeSideFromPayload(const std::string& payloadJson)
{
    const auto value =
        normalizedManualEntryEnumValue(extractJsonStringField(payloadJson, "tradeSide").value_or(""));
    if (value == "buy") {
        return ShellAccountingManualTradeSide::Buy;
    }
    if (value == "sell") {
        return ShellAccountingManualTradeSide::Sell;
    }
    return ShellAccountingManualTradeSide::Unknown;
}

ShellAccountingManualCashMovementType manualCashMovementTypeFromPayload(const std::string& payloadJson)
{
    const auto value =
        normalizedManualEntryEnumValue(extractJsonStringField(payloadJson, "movementType").value_or(""));
    if (value == "deposit") {
        return ShellAccountingManualCashMovementType::Deposit;
    }
    if (value == "withdrawal") {
        return ShellAccountingManualCashMovementType::Withdrawal;
    }
    return ShellAccountingManualCashMovementType::Unknown;
}

ShellAccountingManualTransactionEntry manualTransactionEntryFromPayload(const std::string& payloadJson)
{
    ShellAccountingManualTransactionEntry entry;
    entry.accountId = extractJsonStringField(payloadJson, "accountId").value_or("");
    entry.portfolioId = extractJsonStringField(payloadJson, "portfolioId").value_or("");
    entry.securityCode = extractJsonStringField(payloadJson, "securityCode").value_or("");
    entry.tradeSide = manualTradeSideFromPayload(payloadJson);
    entry.quantityUnits = extractJsonInt64OrStringField(payloadJson, "quantityUnits");
    entry.priceAmountMinor = extractJsonInt64OrStringField(payloadJson, "priceAmountMinor");
    entry.grossAmountMinor = extractJsonInt64OrStringField(payloadJson, "grossAmountMinor");
    entry.feeAmountMinor = extractJsonInt64OrStringField(payloadJson, "feeAmountMinor");
    entry.taxAmountMinor = extractJsonInt64OrStringField(payloadJson, "taxAmountMinor");
    entry.occurredAt = extractJsonStringField(payloadJson, "occurredAt").value_or("");
    entry.sourceMemo = extractJsonStringField(payloadJson, "sourceMemo").value_or("");
    return entry;
}

ShellAccountingManualCashMovementEntry manualCashMovementEntryFromPayload(const std::string& payloadJson)
{
    ShellAccountingManualCashMovementEntry entry;
    entry.accountId = extractJsonStringField(payloadJson, "accountId").value_or("");
    entry.portfolioId = extractJsonStringField(payloadJson, "portfolioId").value_or("");
    entry.movementType = manualCashMovementTypeFromPayload(payloadJson);
    entry.amountMinor = extractJsonInt64OrStringField(payloadJson, "amountMinor");
    entry.occurredAt = extractJsonStringField(payloadJson, "occurredAt").value_or("");
    entry.sourceMemo = extractJsonStringField(payloadJson, "sourceMemo").value_or("");
    return entry;
}

ShellAccountingManualEntryValidationResult manualEntryPayloadShapeRejected()
{
    ShellAccountingManualEntryValidationResult result;
    result.accepted = false;
    result.issues.push_back(ShellAccountingManualEntryValidationIssue{
        "PAYLOAD_JSON_OBJECT_REQUIRED",
        "Payload must be a JSON object.",
        "payload",
    });
    return result;
}

std::string manualEntryValidationIssuesJson(const ShellAccountingManualEntryValidationResult& result)
{
    std::ostringstream payload;
    payload << '[';
    bool first = true;
    for (const auto& issue : result.issues) {
        if (!first) {
            payload << ',';
        }
        payload << "{"
                << "\"code\":" << jsonStringValue(issue.code) << ','
                << "\"field\":" << jsonStringValue(issue.field) << ','
                << "\"message\":" << jsonStringValue(issue.message)
                << "}";
        first = false;
    }
    payload << ']';
    return payload.str();
}

etfdt::protocol::ProtocolResponse manualEntryValidationOnlyResponse(
    const etfdt::service_runtime::ActionContext& context,
    std::string_view actionName,
    const ShellAccountingManualEntryValidationResult& validation)
{
    std::ostringstream payload;
    const bool accepted = validation.valid();
    payload << "{"
            << "\"module\":\"accounting\","
            << "\"action\":" << jsonStringValue(actionName) << ','
            << "\"implemented\":false,"
            << "\"validationOnly\":true,"
            << "\"validationAccepted\":" << (accepted ? "true" : "false") << ','
            << "\"writeImplemented\":false,"
            << "\"manualEntryEnabled\":false,"
            << "\"writeEnabled\":false,"
            << "\"databaseWritten\":false,"
            << "\"tradeLogWritten\":false,"
            << "\"cashFactsWritten\":false,"
            << "\"cashLedgerWritten\":false,"
            << "\"auditWritten\":false,"
            << "\"ledgerWritten\":false,"
            << "\"repositoryCalled\":false,"
            << "\"accountingReplayCalled\":false,"
            << "\"tradeDraftGenerated\":false,"
            << "\"tradeSuggestionGenerated\":false,"
            << "\"brokerSdkCalled\":false,"
            << "\"networkAccessed\":false,"
            << "\"credentialsAccessed\":false,"
            << "\"endpointAccessed\":false,"
            << "\"realOrderPlacement\":false,"
            << "\"automaticTrading\":false,"
            << "\"status\":"
            << jsonStringValue(
                   accepted ? "VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED" : "VALIDATION_REJECTED")
            << ','
            << "\"reason\":"
            << jsonStringValue(
                   accepted ? "MANUAL_ENTRY_VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED"
                            : "MANUAL_ENTRY_VALIDATION_FAILED")
            << ','
            << "\"errorCode\":"
            << jsonStringValue(
                   accepted ? "MANUAL_ENTRY_WRITE_NOT_IMPLEMENTED" : "MANUAL_ENTRY_VALIDATION_FAILED")
            << ','
            << "\"issues\":" << manualEntryValidationIssuesJson(validation) << ','
            << "\"message\":"
            << jsonStringValue(
                   accepted ? "Manual entry payload validation passed; write implementation is not available."
                            : "Manual entry payload validation failed.")
            << "}";

    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = false;
    response.errorCode = accepted ? etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE
                                  : etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD;
    response.errorMessage = accepted ? "Manual entry write implementation is not available"
                                     : "Manual entry payload validation failed";
    response.payloadJson = payload.str();
    return response;
}

etfdt::protocol::ProtocolResponse handleAccountingManualEntryTransactionCreate(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;
    const auto validation =
        isManualEntryJsonObjectPayload(context.request.payloadJson)
            ? validateManualTransactionEntry(manualTransactionEntryFromPayload(context.request.payloadJson))
            : manualEntryPayloadShapeRejected();
    return manualEntryValidationOnlyResponse(
        context,
        kActionAccountingManualTransactionCreate,
        validation);
}

etfdt::protocol::ProtocolResponse handleAccountingManualEntryCashMovementCreate(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;
    const auto validation =
        isManualEntryJsonObjectPayload(context.request.payloadJson)
            ? validateManualCashMovement(manualCashMovementEntryFromPayload(context.request.payloadJson))
            : manualEntryPayloadShapeRejected();
    return manualEntryValidationOnlyResponse(
        context,
        kActionAccountingManualCashMovementCreate,
        validation);
}

etfdt::protocol::ProtocolResponse handlePositionList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "position.list payload must be a JSON object");
    }

    const auto replay = runShellAccountingReadOnlyReplay(connection, "position.list", context.request.payloadJson);
    if (replay.requested) {
        if (replay.queryFailed) {
            return successResponse(context, shellAccountingReplayErrorPayload("position.list", replay.queryErrorCode));
        }

        const auto hasRows = !replay.result.positionList.positions.empty();
        std::ostringstream payload;
        payload << shellAccountingPayloadPrefix(
                       "position.list",
                       replayStatusForPayload(replay.result).c_str(),
                       replayDataQualityStatus(replay.result).c_str(),
                       hasRows,
                       replay.result.replayExecuted,
                       true)
                << "\"message\":" << jsonStringValue(replay.result.message) << ','
                << "\"issues\":" << replayIssuesJson(replay.result.issues)
                << ",\"warnings\":[],\"errors\":[],"
                << "";
        appendReplayPrivacyAndPreview(payload, replay.result);
        payload << "\"positions\":" << jsonArray(replay.result.positionList.positions, writeReplayPositionRow)
                << "}";
        return successResponse(context, payload.str());
    }

    etfdt::data_access::ShellAccountingReadOnlyFactsQuery factsQuery(connection);
    auto positions = factsQuery.loadPositions(shellAccountingQueryRequest(context.request.payloadJson));
    if (!positions) {
        return successResponse(
            context,
            shellAccountingQueryErrorPayload("position.list", "POSITION_LIST_QUERY_ERROR"));
    }
    if (positions.value().empty()) {
        return successResponse(
            context,
            shellAccountingEmptyPayload("position.list", "POSITION_LIST_EMPTY", "positions", "[]"));
    }

    std::ostringstream payload;
    payload << shellAccountingPayloadPrefix("position.list", "OK", "OK", true)
            << "\"message\":\"position.list read-only facts query completed.\","
            << "\"issues\":[],\"warnings\":[],\"errors\":[],"
            << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false},"
            << "\"positions\":" << jsonArray(positions.value(), writePositionRow) << "}";
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleCashSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "cash.summary payload must be a JSON object");
    }

    const auto replay = runShellAccountingReadOnlyReplay(connection, "cash.summary", context.request.payloadJson);
    if (replay.requested) {
        if (replay.queryFailed) {
            return successResponse(context, shellAccountingReplayErrorPayload("cash.summary", replay.queryErrorCode));
        }

        std::vector<etfdt::accounting::CashSummaryDto> summaries = replay.result.accountCashSummaries;
        if (summaries.empty() && replay.result.hasCashSummary) {
            summaries.push_back(replay.result.cashSummary);
        }
        const auto hasRows = !summaries.empty();
        std::ostringstream payload;
        payload << shellAccountingPayloadPrefix(
                       "cash.summary",
                       replayStatusForPayload(replay.result).c_str(),
                       replayDataQualityStatus(replay.result).c_str(),
                       hasRows,
                       replay.result.replayExecuted,
                       true)
                << "\"message\":" << jsonStringValue(replay.result.message) << ','
                << "\"issues\":" << replayIssuesJson(replay.result.issues)
                << ",\"warnings\":[],\"errors\":[],"
                << "";
        appendReplayPrivacyAndPreview(payload, replay.result);
        payload << "\"accountCashSummaries\":" << jsonArray(summaries, writeReplayCashSummary)
                << ",\"cashSummary\":";
        if (hasRows) {
            writeReplayCashSummary(payload, summaries.front());
        } else {
            payload << "{}";
        }
        payload << "}";
        return successResponse(context, payload.str());
    }

    etfdt::data_access::ShellAccountingReadOnlyFactsQuery factsQuery(connection);
    auto summaries = factsQuery.loadCashSummaries(shellAccountingQueryRequest(context.request.payloadJson));
    if (!summaries) {
        return successResponse(
            context,
            shellAccountingQueryErrorPayload("cash.summary", "CASH_SUMMARY_QUERY_ERROR"));
    }
    if (summaries.value().empty()) {
        return successResponse(
            context,
            shellAccountingEmptyPayload("cash.summary", "CASH_SUMMARY_EMPTY", "accountCashSummaries", "[]"));
    }

    std::ostringstream payload;
    payload << shellAccountingPayloadPrefix("cash.summary", "OK", "OK", true)
            << "\"message\":\"cash.summary read-only facts query completed.\","
            << "\"issues\":[],\"warnings\":[],\"errors\":[],"
            << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false},"
            << "\"accountCashSummaries\":" << jsonArray(summaries.value(), writeCashSummaryRow)
            << ",\"cashSummary\":";
    writeCashSummaryRow(payload, summaries.value().front());
    payload << "}";
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handlePortfolioPnlSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "portfolio.pnl.summary payload must be a JSON object");
    }

    const auto replay =
        runShellAccountingReadOnlyReplay(connection, "portfolio.pnl.summary", context.request.payloadJson);
    if (replay.requested) {
        if (replay.queryFailed) {
            return successResponse(
                context,
                shellAccountingReplayErrorPayload(
                    "portfolio.pnl.summary",
                    replay.queryErrorCode));
        }

        const auto hasRows = replay.result.hasPortfolioPnl;
        std::ostringstream payload;
        payload << shellAccountingPayloadPrefix(
                       "portfolio.pnl.summary",
                       replayStatusForPayload(replay.result).c_str(),
                       replayDataQualityStatus(replay.result).c_str(),
                       hasRows,
                       replay.result.replayExecuted,
                       true)
                << "\"message\":" << jsonStringValue(replay.result.message) << ','
                << "\"issues\":" << replayIssuesJson(replay.result.issues)
                << ",\"warnings\":[],\"errors\":[],"
                << "";
        appendReplayPrivacyAndPreview(payload, replay.result);
        payload << "\"portfolioPnlSummaries\":";
        if (hasRows) {
            payload << '[';
            writeReplayPortfolioPnl(payload, replay.result.portfolioPnl);
            payload << ']';
        } else {
            payload << "[]";
        }
        payload << ",\"portfolioPnl\":";
        if (hasRows) {
            writeReplayPortfolioPnl(payload, replay.result.portfolioPnl);
        } else {
            payload << "{}";
        }
        payload << "}";
        return successResponse(context, payload.str());
    }

    etfdt::data_access::ShellAccountingReadOnlyFactsQuery factsQuery(connection);
    auto summaries =
        factsQuery.loadPortfolioSummaries(shellAccountingQueryRequest(context.request.payloadJson));
    if (!summaries) {
        return successResponse(
            context,
            shellAccountingQueryErrorPayload(
                "portfolio.pnl.summary",
                "PORTFOLIO_PNL_SUMMARY_QUERY_ERROR"));
    }
    if (summaries.value().empty()) {
        return successResponse(
            context,
            shellAccountingEmptyPayload(
                "portfolio.pnl.summary",
                "PORTFOLIO_PNL_SUMMARY_EMPTY",
                "portfolioPnlSummaries",
                "[]"));
    }

    std::ostringstream payload;
    payload << shellAccountingPayloadPrefix("portfolio.pnl.summary", "OK", "OK", true)
            << "\"message\":\"portfolio.pnl.summary read-only facts query completed.\","
            << "\"issues\":[],\"warnings\":[],\"errors\":[],"
            << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false},"
            << "\"portfolioPnlSummaries\":" << jsonArray(summaries.value(), writePortfolioSummaryRow)
            << ",\"portfolioPnl\":";
    writePortfolioSummaryRow(payload, summaries.value().front());
    payload << "}";
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleBasePositionSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "base_position.summary payload must be a JSON object");
    }

    const auto replay =
        runShellAccountingReadOnlyReplay(connection, "base_position.summary", context.request.payloadJson);
    if (replay.requested && replay.queryFailed) {
        return successResponse(
            context,
            shellAccountingReplayErrorPayload(
                "base_position.summary",
                replay.queryErrorCode));
    }

    etfdt::data_access::ShellAccountingReadOnlyFactsQuery factsQuery(connection);
    auto summaries =
        factsQuery.loadBasePositionSummaries(shellAccountingQueryRequest(context.request.payloadJson));
    if (!summaries) {
        return successResponse(
            context,
            shellAccountingQueryErrorPayload(
                "base_position.summary",
                "BASE_POSITION_SUMMARY_QUERY_ERROR"));
    }
    if (summaries.value().empty()) {
        return successResponse(
            context,
            shellAccountingEmptyPayload(
                "base_position.summary",
                "BASE_POSITION_SUMMARY_EMPTY",
                "basePositions",
                "[]"));
    }

    std::ostringstream payload;
    payload << shellAccountingPayloadPrefix(
                   "base_position.summary",
                   replay.requested ? replayStatusForPayload(replay.result).c_str() : "OK",
                   replay.requested ? replayDataQualityStatus(replay.result).c_str() : "OK",
                   true,
                   replay.requested ? replay.result.replayExecuted : false,
                   replay.requested)
            << "\"message\":\"base_position.summary read-only facts query completed.\","
            << "\"issues\":"
            << (replay.requested ? replayIssuesJson(replay.result.issues) : "[]")
            << ",\"warnings\":[],\"errors\":[],"
            << "";
    if (replay.requested) {
        appendReplayPrivacyAndPreview(payload, replay.result);
    } else {
        payload << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
                << "\"rawReplayPayloadExposed\":false},";
    }
    payload << "\"basePositions\":" << jsonArray(summaries.value(), writeBasePositionSummaryRow)
            << ",\"basePosition\":";
    writeBasePositionSummaryRow(payload, summaries.value().front());
    payload << "}";
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleSniperPoolSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "sniper_pool.summary payload must be a JSON object");
    }

    const auto replay =
        runShellAccountingReadOnlyReplay(connection, "sniper_pool.summary", context.request.payloadJson);
    if (replay.requested && replay.queryFailed) {
        return successResponse(
            context,
            shellAccountingReplayErrorPayload(
                "sniper_pool.summary",
                replay.queryErrorCode));
    }

    etfdt::data_access::ShellAccountingReadOnlyFactsQuery factsQuery(connection);
    auto tiers = factsQuery.loadSniperPoolTiers(shellAccountingQueryRequest(context.request.payloadJson));
    if (!tiers) {
        return successResponse(
            context,
            shellAccountingQueryErrorPayload(
                "sniper_pool.summary",
                "SNIPER_POOL_SUMMARY_QUERY_ERROR"));
    }
    if (tiers.value().empty()) {
        return successResponse(
            context,
            shellAccountingEmptyPayload(
                "sniper_pool.summary",
                "SNIPER_POOL_SUMMARY_EMPTY",
                "tierSummary",
                "[]"));
    }

    std::ostringstream payload;
    payload << shellAccountingPayloadPrefix(
                   "sniper_pool.summary",
                   replay.requested ? replayStatusForPayload(replay.result).c_str() : "OK",
                   replay.requested ? replayDataQualityStatus(replay.result).c_str() : "OK",
                   true,
                   replay.requested ? replay.result.replayExecuted : false,
                   replay.requested)
            << "\"message\":\"sniper_pool.summary read-only facts query completed.\","
            << "\"issues\":"
            << (replay.requested ? replayIssuesJson(replay.result.issues) : "[]")
            << ",\"warnings\":[],\"errors\":[],"
            << "";
    if (replay.requested) {
        appendReplayPrivacyAndPreview(payload, replay.result);
    } else {
        payload << "\"privacy\":{\"rawSqlExposed\":false,\"rawTradeLogPayloadExposed\":false,"
                << "\"rawReplayPayloadExposed\":false},";
    }
    payload << "\"tierSummary\":" << jsonArray(tiers.value(), writeSniperPoolTierRow)
            << ",\"sniperPool\":";
    writeSniperPoolTierRow(payload, tiers.value().front());
    payload << "}";
    return successResponse(context, payload.str());
}

}  // namespace etfdt::data_service_api
