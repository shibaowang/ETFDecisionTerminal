#include "DataServiceApi/DataServiceActions.h"

#include "DataServiceApi/JsonBuilders.h"
#include "Protocol/Json.h"

#include <cctype>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <utility>

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
"contractVersion":"0.3-draft",
"calculationVersion":"not-implemented",
"replayImplemented":false,
"snapshotImplemented":false,
"writeEnabled":false,
"implementedActions":["accounting.health"],
"futureActions":[
"accounting.replay.preview",
"position.list",
"cash.summary",
"portfolio.pnl.summary",
"base_position.summary",
"sniper_pool.summary"
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

etfdt::protocol::ProtocolResponse handlePositionList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;

    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "position.list payload must be a JSON object");
    }

    const std::string payload = R"json({
"module":"accounting",
"action":"position.list",
"implemented":false,
"readOnly":true,
"writeEnabled":false,
"replayExecuted":false,
"dataSourceAccessed":false,
"sqliteAccessed":false,
"accountingEngineCalled":false,
"contractVersion":"0.4-draft",
"calculationVersion":"not-implemented",
"status":"POSITION_LIST_NOT_AVAILABLE",
"message":"position.list is a read-only contract guard and is not implemented yet.",
"futureOutput":{
"type":"PositionListResponse",
"positions":[]
},
"forbiddenWrites":[
"trade_log",
"trade_execution_group",
"trade_draft",
"cash_snapshot",
"position_snapshot",
"portfolio_summary",
"audit_log"
],
"requiredNextTasks":[
"SQLite read-only facts query implementation",
"AccountingEngine integration boundary",
"position.list no-write integration test",
"real PositionListResponse mapping"
],
"issues":[
{
"level":"ERROR",
"code":"POSITION_LIST_NOT_AVAILABLE",
"message":"position.list is not implemented.",
"blocking":true
}
],
"warnings":[
"POSITION_LIST_NOT_IMPLEMENTED",
"NO_SQLITE_FACTS_QUERY",
"NO_ACCOUNTING_ENGINE_CALL"
]
})json";

    return successResponse(context, payload);
}

etfdt::protocol::ProtocolResponse handleCashSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;

    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "cash.summary payload must be a JSON object");
    }

    const std::string payload = R"json({
"module":"accounting",
"action":"cash.summary",
"implemented":false,
"readOnly":true,
"writeEnabled":false,
"replayExecuted":false,
"dataSourceAccessed":false,
"sqliteAccessed":false,
"cashFactsAccessed":false,
"snapshotAccessed":false,
"portfolioSummaryAccessed":false,
"accountingEngineCalled":false,
"contractVersion":"0.4-draft",
"calculationVersion":"not-implemented",
"status":"CASH_SUMMARY_NOT_AVAILABLE",
"message":"cash.summary is a read-only contract guard and is not implemented yet.",
"futureOutput":{
"type":"CashSummaryResponse",
"cashSummary":null,
"accountCashSummaries":[]
},
"forbiddenSources":[
"cash_snapshot",
"position_snapshot",
"portfolio_summary"
],
"forbiddenWrites":[
"trade_log",
"trade_execution_group",
"trade_draft",
"cash_snapshot",
"position_snapshot",
"portfolio_summary",
"audit_log"
],
"requiredNextTasks":[
"Cash facts source schema review",
"Cash facts read-only query implementation",
"AccountingEngine integration boundary",
"cash.summary no-write integration test",
"real CashSummaryResponse mapping"
],
"issues":[
{
"level":"ERROR",
"code":"CASH_SUMMARY_NOT_AVAILABLE",
"message":"cash.summary is not implemented.",
"blocking":true
}
],
"warnings":[
"CASH_SUMMARY_NOT_IMPLEMENTED",
"NO_CASH_FACTS_SOURCE",
"NO_SQLITE_FACTS_QUERY",
"NO_ACCOUNTING_ENGINE_CALL"
]
})json";

    return successResponse(context, payload);
}

etfdt::protocol::ProtocolResponse handlePortfolioPnlSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;

    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "portfolio.pnl.summary payload must be a JSON object");
    }

    const std::string payload = R"json({
"module":"accounting",
"action":"portfolio.pnl.summary",
"implemented":false,
"readOnly":true,
"writeEnabled":false,
"replayExecuted":false,
"dataSourceAccessed":false,
"sqliteAccessed":false,
"tradeFactsAccessed":false,
"cashFactsAccessed":false,
"marketPriceFactsAccessed":false,
"snapshotAccessed":false,
"portfolioSummaryAccessed":false,
"accountingEngineCalled":false,
"contractVersion":"0.4-draft",
"calculationVersion":"not-implemented",
"status":"PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE",
"message":"portfolio.pnl.summary is a read-only contract guard and is not implemented yet.",
"futureOutput":{
"type":"PortfolioPnlSummaryResponse",
"portfolioPnl":null
},
"forbiddenSources":[
"cash_snapshot",
"position_snapshot",
"portfolio_summary"
],
"forbiddenWrites":[
"trade_log",
"trade_execution_group",
"trade_draft",
"cash_snapshot",
"position_snapshot",
"portfolio_summary",
"audit_log"
],
"requiredNextTasks":[
"Trade facts read-only query integration review",
"Cash facts source review",
"Market price facts source review",
"AccountingEngine integration boundary",
"portfolio.pnl.summary no-write integration test",
"real PortfolioPnlSummaryResponse mapping"
],
"issues":[
{
"level":"ERROR",
"code":"PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE",
"message":"portfolio.pnl.summary is not implemented.",
"blocking":true
}
],
"warnings":[
"PORTFOLIO_PNL_SUMMARY_NOT_IMPLEMENTED",
"NO_TRADE_FACTS_QUERY_IN_ACTION",
"NO_CASH_FACTS_SOURCE",
"NO_MARKET_PRICE_FACTS_SOURCE",
"NO_SQLITE_FACTS_QUERY",
"NO_ACCOUNTING_ENGINE_CALL"
]
})json";

    return successResponse(context, payload);
}

}  // namespace etfdt::data_service_api
