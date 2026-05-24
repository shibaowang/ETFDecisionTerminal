#include "ServiceHost/DemoServiceHost.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <optional>
#include <utility>

namespace etfdt::service_host {
namespace {

constexpr const char* kFallbackMsgId = "invalid-message";
constexpr const char* kFallbackTraceId = "invalid-trace";

std::string compactJson(const QJsonValue& value)
{
    if (value.isObject()) {
        return QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact).toStdString();
    }
    if (value.isArray()) {
        return QJsonDocument(value.toArray()).toJson(QJsonDocument::Compact).toStdString();
    }
    return {};
}

std::optional<std::string> stringField(const QJsonObject& object, const QString& key)
{
    const auto value = object.value(key);
    if (!value.isString()) {
        return std::nullopt;
    }
    return value.toString().toStdString();
}

}  // namespace

struct DemoServiceHost::EnvelopeParseResult final {
    bool success = false;
    etfdt::protocol::MessageEnvelope envelope;
    etfdt::protocol::ErrorCode errorCode = etfdt::protocol::ErrorCode::E1001_INVALID_JSON;
    std::string errorMessage;
    std::string msgId;
    std::string traceId;
};

DemoServiceHost::DemoServiceHost(etfdt::protocol::ServiceName serviceName)
    : dispatcher_(serviceName)
{
    etfdt::service_runtime::registerBuiltinActions(dispatcher_);
    server_.setMessageCallback(
        [this](etfdt::transport::LocalSocketServer::ClientId clientId, const std::string& json) {
            handleTransportMessage(clientId, json);
        });
}

DemoServiceHost::~DemoServiceHost()
{
    close();
}

etfdt::transport::TransportResult<bool> DemoServiceHost::listen(const std::string& socketName)
{
    return server_.listen(socketName);
}

void DemoServiceHost::close()
{
    server_.close();
}

bool DemoServiceHost::isListening() const noexcept
{
    return server_.isListening();
}

std::size_t DemoServiceHost::clientCount() const noexcept
{
    return server_.clientCount();
}

void DemoServiceHost::handleTransportMessage(
    etfdt::transport::LocalSocketServer::ClientId clientId,
    const std::string& json)
{
    const auto parsed = parseEnvelope(json);
    if (!parsed.success) {
        sendResponse(clientId, makeParseErrorResponse(parsed));
        return;
    }

    sendResponse(clientId, dispatcher_.dispatch(parsed.envelope));
}

DemoServiceHost::EnvelopeParseResult DemoServiceHost::parseEnvelope(const std::string& json) const
{
    EnvelopeParseResult result;

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        result.errorCode = etfdt::protocol::ErrorCode::E1001_INVALID_JSON;
        result.errorMessage = "Invalid MessageEnvelope JSON";
        return result;
    }

    const QJsonObject object = document.object();
    result.msgId = stringField(object, "msgId").value_or("");
    result.traceId = stringField(object, "traceId").value_or("");

    const auto protocolVersion = stringField(object, "protocolVersion");
    const auto msgId = stringField(object, "msgId");
    const auto traceId = stringField(object, "traceId");
    const auto from = stringField(object, "from");
    const auto to = stringField(object, "to");
    const auto action = stringField(object, "action");
    const auto timestamp = stringField(object, "timestamp");

    if (!protocolVersion || !msgId || !traceId || !from || !to || !action || !timestamp
        || !object.contains("payload")) {
        result.errorCode = etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD;
        result.errorMessage = "MessageEnvelope is missing one or more required fields";
        return result;
    }

    const QJsonValue payloadValue = object.value("payload");
    const std::string payloadJson = compactJson(payloadValue);
    if (payloadJson.empty()) {
        result.errorCode = etfdt::protocol::ErrorCode::E1001_INVALID_JSON;
        result.errorMessage = "MessageEnvelope payload must be a JSON object or array";
        return result;
    }

    auto fromService = etfdt::protocol::fromString<etfdt::protocol::ServiceName>(*from);
    auto toService = etfdt::protocol::fromString<etfdt::protocol::ServiceName>(*to);
    if (!fromService || !toService) {
        result.errorCode = etfdt::protocol::ErrorCode::E1005_INVALID_SERVICE_NAME;
        result.errorMessage = "MessageEnvelope contains invalid service name";
        return result;
    }

    etfdt::protocol::MessageEnvelope envelope;
    envelope.protocolVersion = *protocolVersion;
    envelope.msgId = *msgId;
    envelope.traceId = *traceId;
    envelope.from = fromService.value();
    envelope.to = toService.value();
    envelope.action = *action;
    envelope.timestampUtc = *timestamp;
    envelope.payloadJson = payloadJson;

    const auto validation = etfdt::protocol::validateMessageEnvelope(envelope);
    if (!validation.valid) {
        result.errorCode = validation.error.has_value()
            ? validation.error->code
            : etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR;
        result.errorMessage = validation.error.has_value()
            ? validation.error->message
            : std::string("MessageEnvelope validation failed");
        return result;
    }

    result.success = true;
    result.envelope = std::move(envelope);
    return result;
}

etfdt::protocol::ProtocolResponse DemoServiceHost::makeParseErrorResponse(
    const EnvelopeParseResult& result) const
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = result.msgId.empty() ? kFallbackMsgId : result.msgId;
    response.traceId = result.traceId.empty() ? kFallbackTraceId : result.traceId;
    response.success = false;
    response.errorCode = result.errorCode;
    response.errorMessage = result.errorMessage.empty() ? "MessageEnvelope parse failed"
                                                        : result.errorMessage;
    response.payloadJson = "{}";
    return response;
}

void DemoServiceHost::sendResponse(
    etfdt::transport::LocalSocketServer::ClientId clientId,
    const etfdt::protocol::ProtocolResponse& response)
{
    const auto responseJson = etfdt::protocol::toJsonString(response);
    (void)server_.sendToClient(clientId, responseJson);
}

}  // namespace etfdt::service_host
