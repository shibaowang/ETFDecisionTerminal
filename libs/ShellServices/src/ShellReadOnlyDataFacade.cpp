#include "ShellServices/ShellReadOnlyDataFacade.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

#include <algorithm>
#include <cctype>
#include <optional>
#include <string_view>

namespace etfdt::shell_services {
namespace {

std::string trimCopy(std::string_view value)
{
    auto begin = value.begin();
    auto end = value.end();
    while (begin != end && std::isspace(static_cast<unsigned char>(*begin)) != 0) {
        ++begin;
    }
    while (begin != end && std::isspace(static_cast<unsigned char>(*(end - 1))) != 0) {
        --end;
    }
    return std::string(begin, end);
}

std::optional<QJsonObject> parseObject(const std::string& json)
{
    QJsonParseError error;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        return std::nullopt;
    }
    return document.object();
}

int arrayCount(const std::string& json, const char* key)
{
    const auto object = parseObject(json);
    if (!object.has_value()) {
        return 0;
    }
    const auto value = object->value(QString::fromUtf8(key));
    return value.isArray() ? value.toArray().size() : 0;
}

bool boolField(const std::string& json, const char* key)
{
    const auto object = parseObject(json);
    if (!object.has_value()) {
        return false;
    }
    const auto value = object->value(QString::fromUtf8(key));
    return value.isBool() && value.toBool();
}

ShellDataResponse fromProtocolResponse(const etfdt::protocol::ProtocolResponse& response)
{
    ShellDataResponse result;
    result.success = response.success;
    result.payloadJson = response.payloadJson;
    result.errorCode = response.errorCode;
    result.errorMessage = response.errorMessage;
    return result;
}

}  // namespace

ShellReadOnlyDataFacade::ShellReadOnlyDataFacade() = default;

ShellReadOnlyDataFacade::~ShellReadOnlyDataFacade()
{
    disconnect();
}

ShellDataResult<bool> ShellReadOnlyDataFacade::connect(
    const std::string& socketName,
    int timeoutMs)
{
    state_.socketName = socketName;
    state_.disconnected = false;
    state_.connecting = true;
    state_.connected = false;
    state_.unhealthy = false;
    state_.lastError.clear();

    auto connected = client_.connect(socketName, timeoutMs);
    state_.connecting = false;
    if (!connected) {
        setError(connected.error().errorCode, connected.error().message);
        state_.disconnected = true;
        state_.connected = false;
        return ShellDataResult<bool>::failure(
            connected.error().errorCode,
            connected.error().message);
    }

    clearError();
    state_.disconnected = false;
    state_.connected = true;
    return ShellDataResult<bool>::success(true);
}

void ShellReadOnlyDataFacade::disconnect()
{
    client_.disconnect();
    state_.disconnected = true;
    state_.connecting = false;
    state_.connected = false;
}

bool ShellReadOnlyDataFacade::isConnected() const noexcept
{
    return client_.isConnected();
}

const ShellDataConnectionState& ShellReadOnlyDataFacade::connectionState() const noexcept
{
    return state_;
}

const std::string& ShellReadOnlyDataFacade::lastError() const noexcept
{
    return state_.lastError;
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::ping(int timeoutMs)
{
    if (!client_.isConnected()) {
        return requireConnected("ping");
    }
    return convertResponse(client_.ping(timeoutMs));
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::health(int timeoutMs)
{
    if (!client_.isConnected()) {
        return requireConnected("health");
    }
    return convertResponse(client_.health(timeoutMs));
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::summary(int timeoutMs)
{
    if (!client_.isConnected()) {
        return requireConnected("summary");
    }
    return convertResponse(client_.summary(timeoutMs));
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::listAccounts(int timeoutMs)
{
    if (!client_.isConnected()) {
        return requireConnected("listAccounts");
    }
    return convertResponse(client_.listAccounts(timeoutMs));
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::listPortfolios(int timeoutMs)
{
    if (!client_.isConnected()) {
        return requireConnected("listPortfolios");
    }
    return convertResponse(client_.listPortfolios(timeoutMs));
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::listInstruments(int timeoutMs)
{
    if (!client_.isConnected()) {
        return requireConnected("listInstruments");
    }
    return convertResponse(client_.listInstruments(timeoutMs));
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::listStrategies(int timeoutMs)
{
    if (!client_.isConnected()) {
        return requireConnected("listStrategies");
    }
    return convertResponse(client_.listStrategies(timeoutMs));
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::listOtc(
    const std::string& strategyCode,
    int timeoutMs)
{
    if (trimCopy(strategyCode).empty()) {
        setError(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellReadOnlyDataFacade requires a non-empty strategyCode");
        return ShellDataResult<ShellDataResponse>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            state_.lastError);
    }
    if (!client_.isConnected()) {
        return requireConnected("listOtc");
    }
    return convertResponse(client_.listOtc(strategyCode, timeoutMs));
}

ShellDataResult<ShellReadOnlyDataSnapshot> ShellReadOnlyDataFacade::loadSnapshot(int timeoutMs)
{
    ShellReadOnlyDataSnapshot snapshot;

    auto healthResult = health(timeoutMs);
    if (!healthResult || !healthResult.value().success) {
        const std::string message = healthResult
            ? healthResult.value().errorMessage
            : healthResult.error().message;
        snapshot.lastError = message.empty() ? "data.health failed" : message;
        setError(
            healthResult ? healthResult.value().errorCode : healthResult.error().errorCode,
            snapshot.lastError);
        return ShellDataResult<ShellReadOnlyDataSnapshot>::failure(
            healthResult ? healthResult.value().errorCode : healthResult.error().errorCode,
            snapshot.lastError);
    }
    snapshot.healthy = boolField(healthResult.value().payloadJson, "healthy");

    auto summaryResult = summary(timeoutMs);
    if (!summaryResult || !summaryResult.value().success) {
        const std::string message = summaryResult
            ? summaryResult.value().errorMessage
            : summaryResult.error().message;
        snapshot.lastError = message.empty() ? "data.summary failed" : message;
        setError(
            summaryResult ? summaryResult.value().errorCode : summaryResult.error().errorCode,
            snapshot.lastError);
        return ShellDataResult<ShellReadOnlyDataSnapshot>::failure(
            summaryResult ? summaryResult.value().errorCode : summaryResult.error().errorCode,
            snapshot.lastError);
    }
    snapshot.summaryJson = summaryResult.value().payloadJson;

    auto accounts = listAccounts(timeoutMs);
    auto portfolios = listPortfolios(timeoutMs);
    auto instruments = listInstruments(timeoutMs);
    auto strategies = listStrategies(timeoutMs);
    if (!accounts || !portfolios || !instruments || !strategies) {
        const auto& error = !accounts ? accounts.error()
            : !portfolios      ? portfolios.error()
            : !instruments     ? instruments.error()
                               : strategies.error();
        snapshot.lastError = error.message;
        setError(error.errorCode, error.message);
        return ShellDataResult<ShellReadOnlyDataSnapshot>::failure(
            error.errorCode,
            error.message);
    }
    if (!accounts.value().success || !portfolios.value().success || !instruments.value().success
        || !strategies.value().success) {
        const auto& failed = !accounts.value().success ? accounts.value()
            : !portfolios.value().success             ? portfolios.value()
            : !instruments.value().success            ? instruments.value()
                                                       : strategies.value();
        snapshot.lastError = failed.errorMessage;
        setError(failed.errorCode, failed.errorMessage);
        return ShellDataResult<ShellReadOnlyDataSnapshot>::failure(
            failed.errorCode,
            failed.errorMessage);
    }

    snapshot.accountCount = arrayCount(accounts.value().payloadJson, "accounts");
    snapshot.portfolioCount = arrayCount(portfolios.value().payloadJson, "portfolios");
    snapshot.instrumentCount = arrayCount(instruments.value().payloadJson, "instruments");
    snapshot.strategyCount = arrayCount(strategies.value().payloadJson, "strategies");
    clearError();
    return ShellDataResult<ShellReadOnlyDataSnapshot>::success(snapshot);
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::convertResponse(
    const etfdt::data_service_client::DataServiceClientResult<
        etfdt::protocol::ProtocolResponse>& result)
{
    if (!result) {
        setError(result.error().errorCode, result.error().message);
        return ShellDataResult<ShellDataResponse>::failure(
            result.error().errorCode,
            result.error().message);
    }

    const auto response = fromProtocolResponse(result.value());
    if (!response.success) {
        setError(response.errorCode, response.errorMessage);
    } else {
        clearError();
    }
    return ShellDataResult<ShellDataResponse>::success(response);
}

ShellDataResult<ShellDataResponse> ShellReadOnlyDataFacade::requireConnected(
    const char* operation)
{
    setError(
        etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        std::string("ShellReadOnlyDataFacade is not connected for ") + operation);
    return ShellDataResult<ShellDataResponse>::failure(
        etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        state_.lastError);
}

void ShellReadOnlyDataFacade::setError(
    std::optional<etfdt::protocol::ErrorCode>,
    std::string message)
{
    state_.lastError = std::move(message);
    state_.unhealthy = true;
}

void ShellReadOnlyDataFacade::clearError()
{
    state_.lastError.clear();
    state_.unhealthy = false;
}

}  // namespace etfdt::shell_services
