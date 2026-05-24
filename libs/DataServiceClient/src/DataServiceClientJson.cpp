#include "DataServiceClient/DataServiceClientJson.h"

#include "Protocol/Json.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <sstream>
#include <optional>
#include <string_view>

namespace etfdt::data_service_client {
namespace {

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

std::string jsonString(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

}  // namespace

DataServiceClientResult<etfdt::protocol::ProtocolResponse> parseProtocolResponseJson(
    const std::string& json)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid ProtocolResponse JSON");
    }

    const QJsonObject object = document.object();
    const auto protocolVersion = stringField(object, "protocolVersion");
    const auto msgId = stringField(object, "msgId");
    const auto traceId = stringField(object, "traceId");
    const auto errorCodeText = stringField(object, "errorCode");
    const auto errorMessage = stringField(object, "errorMessage");

    if (!protocolVersion || !msgId || !traceId || !object.value("success").isBool()
        || !errorCodeText || !errorMessage || !object.contains("payload")) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ProtocolResponse is missing one or more required fields");
    }

    const std::string payloadJson = compactJson(object.value("payload"));
    if (payloadJson.empty()) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "ProtocolResponse payload must be a JSON object or array");
    }

    etfdt::protocol::ProtocolResponse response;
    response.protocolVersion = *protocolVersion;
    response.msgId = *msgId;
    response.traceId = *traceId;
    response.success = object.value("success").toBool();
    response.errorMessage = *errorMessage;
    response.payloadJson = payloadJson;

    if (!errorCodeText->empty()) {
        auto parsedErrorCode = etfdt::protocol::fromString<etfdt::protocol::ErrorCode>(
            *errorCodeText);
        if (!parsedErrorCode) {
            return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
                etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR,
                "ProtocolResponse contains unknown errorCode");
        }
        response.errorCode = parsedErrorCode.value();
    }

    const auto validation = etfdt::protocol::validateProtocolResponse(response);
    if (!validation.valid) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            validation.error.has_value()
                ? validation.error->code
                : etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR,
            validation.error.has_value() ? validation.error->message
                                         : "ProtocolResponse validation failed");
    }

    return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::success(response);
}

std::string auditAppendPayloadJson(const AuditAppendRequest& request)
{
    std::ostringstream stream;
    stream << "{"
           << "\"entityType\":" << jsonString(request.entityType) << ','
           << "\"entityId\":" << jsonString(request.entityId) << ','
           << "\"action\":" << jsonString(request.action) << ','
           << "\"reason\":" << jsonString(request.reason) << ','
           << "\"operatorName\":" << jsonString(request.operatorName) << ','
           << "\"oldValue\":" << request.oldValueJson << ','
           << "\"newValue\":" << request.newValueJson
           << "}";
    return stream.str();
}

}  // namespace etfdt::data_service_client
