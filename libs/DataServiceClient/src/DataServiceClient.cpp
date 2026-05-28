#include "DataServiceClient/DataServiceClient.h"

#include "DataServiceClient/DataServiceClientJson.h"
#include "Protocol/Json.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QThread>

#include <atomic>
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
