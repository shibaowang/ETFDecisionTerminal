#include "DataServiceClient/DataServiceClient.h"

#include "DataServiceClient/DataServiceClientJson.h"
#include "Protocol/Json.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QThread>

#include <atomic>
#include <cctype>
#include <cstdint>
#include <functional>
#include <regex>
#include <sstream>
#include <string_view>

namespace etfdt::data_service_client {
namespace {

constexpr const char* kActionSystemPing = "system.ping";
constexpr const char* kActionSystemHealth = "system.health";
constexpr const char* kActionDataHealth = "data.health";
constexpr const char* kActionDataSummary = "data.summary";
constexpr const char* kActionDataAccountsList = "data.accounts.list";
constexpr const char* kActionDataPortfoliosList = "data.portfolios.list";
constexpr const char* kActionDataInstrumentsList = "data.instruments.list";
constexpr const char* kActionDataStrategiesList = "data.strategies.list";
constexpr const char* kActionDataOtcList = "data.otc.list";
constexpr const char* kActionDataAuditAppend = "data.audit.append";
constexpr const char* kActionAccountingHealth = "accounting.health";
constexpr const char* kActionAccountingReplayPreview = "accounting.replay.preview";
constexpr const char* kActionAccountingPortfolioReplayReadOnlySummary =
    "accounting.portfolio_replay.readonly_summary";
constexpr const char* kActionStrategyRecommendationReadOnlySummary =
    "strategy.recommendation.readonly_summary";
constexpr const char* kActionMarketDataRefreshReadOnlySummary =
    "marketdata.refresh.readonly_summary";
constexpr const char* kActionMarketDataHistoricalHighReadOnlySummary =
    "marketdata.historical_high.readonly_summary";
constexpr const char* kActionAccountingExcelVbaImportReadOnlyPreview =
    "accounting.excel_vba_import.readonly_preview";
constexpr const char* kActionAccountingExcelVbaImportPersistManualEntry =
    "accounting.excel_vba_import.persist_manual_entry";
constexpr const char* kActionAccountingTradeDraftCreateFromRecommendation =
    "accounting.tradedraft.create_from_recommendation";
constexpr const char* kActionAccountingTradeDraftReadOnlySummary =
    "accounting.tradedraft.readonly_summary";
constexpr const char* kActionAccountingOtcMapMultiChannelReadOnlyPreview =
    "accounting.otcmap_multichannel.readonly_preview";
constexpr const char* kActionAccountingTradeDraftCreateOtcMapMultiChannel =
    "accounting.tradedraft.create_otcmap_multichannel";
constexpr const char* kActionPositionList = "position.list";
constexpr const char* kActionCashSummary = "cash.summary";
constexpr const char* kActionPortfolioPnlSummary = "portfolio.pnl.summary";
constexpr const char* kActionBasePositionSummary = "base_position.summary";
constexpr const char* kActionSniperPoolSummary = "sniper_pool.summary";

std::string nextId(std::string_view prefix)
{
    static std::atomic<std::uint64_t> counter = 0;
    const auto now = QDateTime::currentMSecsSinceEpoch();
    std::ostringstream stream;
    stream << prefix << '-' << now << '-' << counter.fetch_add(1);
    return stream.str();
}

std::string timestampUtc()
{
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
}

bool waitUntil(const std::function<bool()>& predicate, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        if (predicate()) {
            return true;
        }
        QThread::msleep(5);
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    return predicate();
}

std::string objectWithStringField(std::string_view key, std::string_view value)
{
    return "{\"" + std::string(key) + "\":\""
        + etfdt::protocol::escapeJsonString(value) + "\"}";
}

bool containsForbiddenSqlPayload(const std::string& payloadJson)
{
    static const std::regex sqlFieldPattern(
        R"("sql"\s*:)",
        std::regex::ECMAScript | std::regex::icase);
    return std::regex_search(payloadJson, sqlFieldPattern);
}

bool isLikelyJsonObject(const std::string& payloadJson)
{
    auto first = payloadJson.begin();
    while (first != payloadJson.end()
           && std::isspace(static_cast<unsigned char>(*first)) != 0) {
        ++first;
    }
    if (first == payloadJson.end() || *first != '{') {
        return false;
    }

    auto last = payloadJson.rbegin();
    while (last != payloadJson.rend()
           && std::isspace(static_cast<unsigned char>(*last)) != 0) {
        ++last;
    }
    return last != payloadJson.rend() && *last == '}';
}

bool hasRequiredPersistRequestFields(const ExcelVbaImportPersistManualEntryRequest& request)
{
    return !request.previewStatus.empty() && !request.previewDigest.empty()
        && !request.idempotencyKey.empty() && !request.schemaVersion.empty()
        && !request.source.empty() && !request.acceptedAt.empty()
        && !request.importBatchLabel.empty()
        && isLikelyJsonObject(request.sanitizedImportPayloadJson);
}

bool hasRequiredTradeDraftRecommendationFields(const TradeDraftCreateFromRecommendationRequest& request)
{
    return isLikelyJsonObject(request.recommendationPayloadJson)
        && !request.idempotencyKey.empty()
        && !request.recommendationDigest.empty()
        && !request.accountId.empty()
        && !request.portfolioId.empty()
        && !request.strategyId.empty()
        && !request.instrumentId.empty();
}

}  // namespace

DataServiceClient::DataServiceClient(
    etfdt::protocol::ServiceName from,
    etfdt::protocol::ServiceName to)
    : from_(from)
    , to_(to)
{
    socket_.setMessageCallback([this](const std::string& json) {
        lastResponseJson_ = json;
    });
}

DataServiceClient::~DataServiceClient()
{
    disconnect();
}

DataServiceClientResult<bool> DataServiceClient::connect(
    const std::string& socketName,
    int timeoutMs)
{
    auto connected = socket_.connectToServer(socketName, timeoutMs);
    if (!connected) {
        return DataServiceClientResult<bool>::failure(
            etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
            connected.error().message);
    }
    return DataServiceClientResult<bool>::success(true);
}

void DataServiceClient::disconnect()
{
    socket_.disconnect();
    lastResponseJson_.reset();
}

bool DataServiceClient::isConnected() const noexcept
{
    return socket_.isConnected();
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::sendRaw(
    etfdt::protocol::MessageEnvelope request,
    int timeoutMs)
{
    if (!socket_.isConnected()) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
            "DataServiceClient is not connected");
    }

    const auto validation = etfdt::protocol::validateMessageEnvelope(request);
    if (!validation.valid) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            validation.error.has_value()
                ? validation.error->code
                : etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR,
            validation.error.has_value() ? validation.error->message
                                         : "MessageEnvelope validation failed");
    }
    if (containsForbiddenSqlPayload(request.payloadJson)) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "DataServiceClient rejects SQL payload fields");
    }

    lastResponseJson_.reset();
    auto sent = socket_.send(etfdt::protocol::toJsonString(request));
    if (!sent) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
            sent.error().message);
    }

    const bool received = waitUntil([&]() { return lastResponseJson_.has_value(); }, timeoutMs);
    if (!received) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E9002_HEARTBEAT_TIMEOUT,
            "Timed out waiting for ProtocolResponse");
    }

    auto parsed = parseProtocolResponseJson(*lastResponseJson_);
    if (!parsed) {
        return parsed;
    }
    if (parsed.value().traceId != request.traceId) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR,
            "ProtocolResponse traceId does not match request");
    }

    return parsed;
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::ping(
    int timeoutMs)
{
    return sendAction(kActionSystemPing, "{}", timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::health(
    int timeoutMs)
{
    return sendAction(kActionDataHealth, "{}", timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::summary(
    int timeoutMs)
{
    return sendAction(kActionDataSummary, "{}", timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::listAccounts(
    int timeoutMs)
{
    return sendAction(kActionDataAccountsList, "{}", timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::listPortfolios(
    int timeoutMs)
{
    return sendAction(kActionDataPortfoliosList, "{}", timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::listInstruments(
    int timeoutMs)
{
    return sendAction(kActionDataInstrumentsList, "{}", timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::listStrategies(
    int timeoutMs)
{
    return sendAction(kActionDataStrategiesList, "{}", timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::listOtc(
    const std::string& strategyCode,
    int timeoutMs)
{
    return sendAction(kActionDataOtcList, objectWithStringField("strategyCode", strategyCode), timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::accountingHealth(
    int timeoutMs)
{
    return sendAction(kActionAccountingHealth, "{}", timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::accountingReplayPreview(
    const std::string& payloadJson,
    int timeoutMs)
{
    return sendAction(kActionAccountingReplayPreview, payloadJson, timeoutMs);
}

DataServiceClientResult<PortfolioReplayReadOnlySummaryResult>
DataServiceClient::accountingPortfolioReplayReadOnlySummary(
    const PortfolioReplayReadOnlySummaryRequest& request,
    int timeoutMs)
{
    if (!isLikelyJsonObject(request.replayPayloadJson)) {
        return DataServiceClientResult<PortfolioReplayReadOnlySummaryResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Portfolio replay summary request payload must be a JSON object");
    }

    auto response = sendAction(
        kActionAccountingPortfolioReplayReadOnlySummary,
        portfolioReplayReadOnlySummaryPayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<PortfolioReplayReadOnlySummaryResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parsePortfolioReplayReadOnlySummaryPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>
DataServiceClient::strategyRecommendationReadOnlySummary(
    const StrategyRecommendationReadOnlySummaryRequest& request,
    int timeoutMs)
{
    if (!isLikelyJsonObject(request.recommendationPayloadJson)) {
        return DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Strategy recommendation summary request payload must be a JSON object");
    }

    auto response = sendAction(
        kActionStrategyRecommendationReadOnlySummary,
        strategyRecommendationReadOnlySummaryPayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parseStrategyRecommendationReadOnlySummaryPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<TradeDraftCreateFromRecommendationResult>
DataServiceClient::accountingTradeDraftCreateFromRecommendation(
    const TradeDraftCreateFromRecommendationRequest& request,
    int timeoutMs)
{
    if (!hasRequiredTradeDraftRecommendationFields(request)) {
        return DataServiceClientResult<TradeDraftCreateFromRecommendationResult>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "TradeDraft create-from-recommendation request is missing required fields");
    }

    auto response = sendAction(
        kActionAccountingTradeDraftCreateFromRecommendation,
        tradeDraftCreateFromRecommendationPayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<TradeDraftCreateFromRecommendationResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parseTradeDraftCreateFromRecommendationPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<TradeDraftReadOnlySummaryResult>
DataServiceClient::accountingTradeDraftReadOnlySummary(
    const TradeDraftReadOnlySummaryRequest& request,
    int timeoutMs)
{
    if (request.draftId <= 0 && request.idempotencyKey.empty()) {
        return DataServiceClientResult<TradeDraftReadOnlySummaryResult>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "TradeDraft read-only summary requires draftId or idempotencyKey");
    }

    auto response = sendAction(
        kActionAccountingTradeDraftReadOnlySummary,
        tradeDraftReadOnlySummaryPayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<TradeDraftReadOnlySummaryResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parseTradeDraftReadOnlySummaryPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<OtcMapMultiChannelDraftResult>
DataServiceClient::accountingOtcMapMultiChannelReadOnlyPreview(
    const OtcMapMultiChannelReadOnlyPreviewRequest& request,
    int timeoutMs)
{
    if (!isLikelyJsonObject(request.payloadJson)) {
        return DataServiceClientResult<OtcMapMultiChannelDraftResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "OTCMap multi-channel preview request payload must be a JSON object");
    }
    auto response = sendAction(
        kActionAccountingOtcMapMultiChannelReadOnlyPreview,
        otcMapMultiChannelPreviewPayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<OtcMapMultiChannelDraftResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parseOtcMapMultiChannelDraftPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<OtcMapMultiChannelDraftResult>
DataServiceClient::accountingTradeDraftCreateOtcMapMultiChannel(
    const OtcMapTradeDraftCreateRequest& request,
    int timeoutMs)
{
    if (!isLikelyJsonObject(request.payloadJson)) {
        return DataServiceClientResult<OtcMapMultiChannelDraftResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "OTCMap multi-channel TradeDraft request payload must be a JSON object");
    }
    auto response = sendAction(
        kActionAccountingTradeDraftCreateOtcMapMultiChannel,
        otcMapTradeDraftCreatePayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<OtcMapMultiChannelDraftResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parseOtcMapMultiChannelDraftPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<MarketDataReadOnlySummaryResult>
DataServiceClient::marketDataRefreshReadOnlySummary(
    const MarketDataReadOnlySummaryRequest& request,
    int timeoutMs)
{
    if (!isLikelyJsonObject(request.payloadJson)) {
        return DataServiceClientResult<MarketDataReadOnlySummaryResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Market data refresh request payload must be a JSON object");
    }
    auto response = sendAction(
        kActionMarketDataRefreshReadOnlySummary,
        marketDataReadOnlySummaryPayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<MarketDataReadOnlySummaryResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parseMarketDataReadOnlySummaryPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<MarketDataReadOnlySummaryResult>
DataServiceClient::marketDataHistoricalHighReadOnlySummary(
    const MarketDataReadOnlySummaryRequest& request,
    int timeoutMs)
{
    if (!isLikelyJsonObject(request.payloadJson)) {
        return DataServiceClientResult<MarketDataReadOnlySummaryResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Market data historical high request payload must be a JSON object");
    }
    auto response = sendAction(
        kActionMarketDataHistoricalHighReadOnlySummary,
        marketDataReadOnlySummaryPayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<MarketDataReadOnlySummaryResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parseMarketDataReadOnlySummaryPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>
DataServiceClient::accountingExcelVbaImportReadOnlyPreview(
    const std::string& payloadJson,
    int timeoutMs)
{
    auto response = sendAction(kActionAccountingExcelVbaImportReadOnlyPreview, payloadJson, timeoutMs);
    if (!response) {
        return DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    if (!response.value().success) {
        return DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>::failure(
            response.value().errorCode,
            response.value().errorMessage);
    }
    return parseExcelVbaImportReadOnlyPreviewPayloadJson(response.value().payloadJson);
}

DataServiceClientResult<ExcelVbaImportPersistManualEntryResult>
DataServiceClient::accountingExcelVbaImportPersistManualEntry(
    const ExcelVbaImportPersistManualEntryRequest& request,
    int timeoutMs)
{
    if (!hasRequiredPersistRequestFields(request)) {
        return DataServiceClientResult<ExcelVbaImportPersistManualEntryResult>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "Excel/VBA import persist request is missing required fields");
    }

    auto response = sendAction(
        kActionAccountingExcelVbaImportPersistManualEntry,
        excelVbaImportPersistManualEntryPayloadJson(request),
        timeoutMs);
    if (!response) {
        return DataServiceClientResult<ExcelVbaImportPersistManualEntryResult>::failure(
            response.error().errorCode,
            response.error().message);
    }
    return parseExcelVbaImportPersistManualEntryPayloadJson(
        response.value().payloadJson,
        response.value().success);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::positionList(
    const std::string& payloadJson,
    int timeoutMs)
{
    return sendAction(kActionPositionList, payloadJson, timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::cashSummary(
    const std::string& payloadJson,
    int timeoutMs)
{
    return sendAction(kActionCashSummary, payloadJson, timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::portfolioPnlSummary(
    const std::string& payloadJson,
    int timeoutMs)
{
    return sendAction(kActionPortfolioPnlSummary, payloadJson, timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::basePositionSummary(
    const std::string& payloadJson,
    int timeoutMs)
{
    return sendAction(kActionBasePositionSummary, payloadJson, timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::sniperPoolSummary(
    const std::string& payloadJson,
    int timeoutMs)
{
    return sendAction(kActionSniperPoolSummary, payloadJson, timeoutMs);
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::appendAuditDemo(
    const AuditAppendRequest& request,
    int timeoutMs)
{
    if (!etfdt::protocol::isLikelyJsonObjectOrArray(request.oldValueJson)
        || !etfdt::protocol::isLikelyJsonObjectOrArray(request.newValueJson)) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Audit oldValueJson and newValueJson must be JSON object or array fragments");
    }
    return sendAction(kActionDataAuditAppend, auditAppendPayloadJson(request), timeoutMs);
}

etfdt::protocol::MessageEnvelope DataServiceClient::makeRequest(
    const std::string& action,
    const std::string& payloadJson) const
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = nextId("msg");
    request.traceId = nextId("trace");
    request.from = from_;
    request.to = to_;
    request.action = action;
    request.timestampUtc = timestampUtc();
    request.payloadJson = payloadJson;
    return request;
}

DataServiceClientResult<etfdt::protocol::ProtocolResponse> DataServiceClient::sendAction(
    const std::string& action,
    const std::string& payloadJson,
    int timeoutMs)
{
    return sendRaw(makeRequest(action, payloadJson), timeoutMs);
}

}  // namespace etfdt::data_service_client
