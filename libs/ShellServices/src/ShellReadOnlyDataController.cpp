#include "ShellServices/ShellReadOnlyDataController.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>
#include <QVariant>
#include <QVariantMap>

#include <optional>
#include <sstream>
#include <utility>

namespace etfdt::shell_services {
namespace {

std::optional<QJsonObject> parseObject(const std::string& json)
{
    QJsonParseError error;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        return std::nullopt;
    }
    return document.object();
}

std::int64_t int64Value(const QJsonObject& object, const char* key)
{
    return static_cast<std::int64_t>(object.value(QString::fromUtf8(key)).toVariant().toLongLong());
}

std::string stringValue(const QJsonObject& object, const char* key)
{
    return object.value(QString::fromUtf8(key)).toString().toStdString();
}

bool boolValue(const QJsonObject& object, const char* key)
{
    return object.value(QString::fromUtf8(key)).toBool(false);
}

std::vector<ShellAccountRow> parseAccounts(const std::string& json)
{
    std::vector<ShellAccountRow> rows;
    const auto object = parseObject(json);
    if (!object.has_value()) {
        return rows;
    }
    const auto values = object->value("accounts").toArray();
    rows.reserve(static_cast<std::size_t>(values.size()));
    for (const auto& value : values) {
        const auto item = value.toObject();
        ShellAccountRow row;
        row.id = int64Value(item, "id");
        row.uid = stringValue(item, "uid");
        row.name = stringValue(item, "name");
        row.accountType = stringValue(item, "accountType");
        row.brokerName = stringValue(item, "brokerName");
        row.baseCurrency = stringValue(item, "baseCurrency");
        row.isActive = boolValue(item, "isActive");
        row.initialCashText = std::to_string(int64Value(item, "initialCashCents"));
        rows.push_back(std::move(row));
    }
    return rows;
}

std::vector<ShellPortfolioRow> parsePortfolios(const std::string& json)
{
    std::vector<ShellPortfolioRow> rows;
    const auto object = parseObject(json);
    if (!object.has_value()) {
        return rows;
    }
    const auto values = object->value("portfolios").toArray();
    rows.reserve(static_cast<std::size_t>(values.size()));
    for (const auto& value : values) {
        const auto item = value.toObject();
        ShellPortfolioRow row;
        row.id = int64Value(item, "id");
        row.uid = stringValue(item, "uid");
        row.name = stringValue(item, "name");
        row.basePositionRatioText = std::to_string(int64Value(item, "basePositionRatioPpm"));
        row.isActive = boolValue(item, "isActive");
        rows.push_back(std::move(row));
    }
    return rows;
}

std::vector<ShellInstrumentRow> parseInstruments(const std::string& json)
{
    std::vector<ShellInstrumentRow> rows;
    const auto object = parseObject(json);
    if (!object.has_value()) {
        return rows;
    }
    const auto values = object->value("instruments").toArray();
    rows.reserve(static_cast<std::size_t>(values.size()));
    for (const auto& value : values) {
        const auto item = value.toObject();
        ShellInstrumentRow row;
        row.id = int64Value(item, "id");
        row.uid = stringValue(item, "uid");
        row.code = stringValue(item, "code");
        row.name = stringValue(item, "name");
        row.instrumentType = stringValue(item, "instrumentType");
        row.marketCode = stringValue(item, "marketCode");
        row.currency = stringValue(item, "currency");
        row.enabled = boolValue(item, "enabled");
        rows.push_back(std::move(row));
    }
    return rows;
}

std::vector<ShellStrategyRow> parseStrategies(const std::string& json)
{
    std::vector<ShellStrategyRow> rows;
    const auto object = parseObject(json);
    if (!object.has_value()) {
        return rows;
    }
    const auto values = object->value("strategies").toArray();
    rows.reserve(static_cast<std::size_t>(values.size()));
    for (const auto& value : values) {
        const auto item = value.toObject();
        ShellStrategyRow row;
        row.id = int64Value(item, "id");
        row.uid = stringValue(item, "uid");
        row.strategyCode = stringValue(item, "strategyCode");
        row.name = stringValue(item, "name");
        row.enabled = boolValue(item, "enabled");
        rows.push_back(std::move(row));
    }
    return rows;
}

std::string summaryTextFromPayload(const std::string& json)
{
    const auto object = parseObject(json);
    if (!object.has_value()) {
        return {};
    }
    std::ostringstream stream;
    stream << "schemaVersion=" << stringValue(*object, "schemaVersion")
           << "; enabledInstrumentCount="
           << object->value("enabledInstrumentCount").toInt()
           << "; enabledStrategyCount="
           << object->value("enabledStrategyCount").toInt()
           << "; otcChannelCount="
           << object->value("otcChannelCount").toInt();
    return stream.str();
}

}  // namespace

ShellReadOnlyDataController::ShellReadOnlyDataController(QObject* parent)
    : QObject(parent)
    , connection_(this)
    , accounts_(this)
    , portfolios_(this)
    , instruments_(this)
    , strategies_(this)
{
}

ShellDataResult<bool> ShellReadOnlyDataController::connectToDataService(
    const std::string& socketName,
    int timeoutMs)
{
    auto connected = facade_.connect(socketName, timeoutMs);
    connection_.updateFromState(facade_.connectionState());
    if (!connected) {
        return failureFromError(connected.error(), "connectToDataService failed");
    }
    clearLastError();
    return ShellDataResult<bool>::success(true);
}

bool ShellReadOnlyDataController::connectToDataService(const QString& socketName)
{
    return connectToDataService(socketName.toStdString(), 2000).hasValue();
}

void ShellReadOnlyDataController::disconnect()
{
    facade_.disconnect();
    connection_.updateFromState(facade_.connectionState());
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshHealth(int timeoutMs)
{
    auto result = facade_.health(timeoutMs);
    connection_.updateFromState(facade_.connectionState());
    if (!result) {
        return failureFromError(result.error(), "refreshHealth failed");
    }
    if (!result.value().success) {
        return failureFromResponse(result.value(), "refreshHealth failed");
    }

    const auto object = parseObject(result.value().payloadJson);
    summary_.healthy = object.has_value() && object->value("healthy").toBool(false);
    connection_.setHealthy(summary_.healthy);
    clearLastError();
    notifySummaryChanged();
    return ShellDataResult<bool>::success(true);
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshSummary(int timeoutMs)
{
    auto result = facade_.summary(timeoutMs);
    if (!result) {
        return failureFromError(result.error(), "refreshSummary failed");
    }
    if (!result.value().success) {
        return failureFromResponse(result.value(), "refreshSummary failed");
    }

    summary_.summaryText = summaryTextFromPayload(result.value().payloadJson);
    clearLastError();
    notifySummaryChanged();
    return ShellDataResult<bool>::success(true);
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshAccounts(int timeoutMs)
{
    auto result = facade_.listAccounts(timeoutMs);
    if (!result) {
        return failureFromError(result.error(), "refreshAccounts failed");
    }
    if (!result.value().success) {
        return failureFromResponse(result.value(), "refreshAccounts failed");
    }
    auto rows = parseAccounts(result.value().payloadJson);
    summary_.accountCount = static_cast<int>(rows.size());
    accounts_.setRows(std::move(rows));
    clearLastError();
    notifySummaryChanged();
    return ShellDataResult<bool>::success(true);
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshPortfolios(int timeoutMs)
{
    auto result = facade_.listPortfolios(timeoutMs);
    if (!result) {
        return failureFromError(result.error(), "refreshPortfolios failed");
    }
    if (!result.value().success) {
        return failureFromResponse(result.value(), "refreshPortfolios failed");
    }
    auto rows = parsePortfolios(result.value().payloadJson);
    summary_.portfolioCount = static_cast<int>(rows.size());
    portfolios_.setRows(std::move(rows));
    clearLastError();
    notifySummaryChanged();
    return ShellDataResult<bool>::success(true);
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshInstruments(int timeoutMs)
{
    auto result = facade_.listInstruments(timeoutMs);
    if (!result) {
        return failureFromError(result.error(), "refreshInstruments failed");
    }
    if (!result.value().success) {
        return failureFromResponse(result.value(), "refreshInstruments failed");
    }
    auto rows = parseInstruments(result.value().payloadJson);
    summary_.instrumentCount = static_cast<int>(rows.size());
    instruments_.setRows(std::move(rows));
    clearLastError();
    notifySummaryChanged();
    return ShellDataResult<bool>::success(true);
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshStrategies(int timeoutMs)
{
    auto result = facade_.listStrategies(timeoutMs);
    if (!result) {
        return failureFromError(result.error(), "refreshStrategies failed");
    }
    if (!result.value().success) {
        return failureFromResponse(result.value(), "refreshStrategies failed");
    }
    auto rows = parseStrategies(result.value().payloadJson);
    summary_.strategyCount = static_cast<int>(rows.size());
    strategies_.setRows(std::move(rows));
    clearLastError();
    notifySummaryChanged();
    return ShellDataResult<bool>::success(true);
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshAll(int timeoutMs)
{
    if (auto result = refreshHealth(timeoutMs); !result) {
        return result;
    }
    if (auto result = refreshSummary(timeoutMs); !result) {
        return result;
    }
    if (auto result = refreshAccounts(timeoutMs); !result) {
        return result;
    }
    if (auto result = refreshPortfolios(timeoutMs); !result) {
        return result;
    }
    if (auto result = refreshInstruments(timeoutMs); !result) {
        return result;
    }
    return refreshStrategies(timeoutMs);
}

bool ShellReadOnlyDataController::refreshHealth()
{
    return refreshHealth(2000).hasValue();
}

bool ShellReadOnlyDataController::refreshSummary()
{
    return refreshSummary(2000).hasValue();
}

bool ShellReadOnlyDataController::refreshAccounts()
{
    return refreshAccounts(2000).hasValue();
}

bool ShellReadOnlyDataController::refreshPortfolios()
{
    return refreshPortfolios(2000).hasValue();
}

bool ShellReadOnlyDataController::refreshInstruments()
{
    return refreshInstruments(2000).hasValue();
}

bool ShellReadOnlyDataController::refreshStrategies()
{
    return refreshStrategies(2000).hasValue();
}

bool ShellReadOnlyDataController::refreshAll()
{
    return refreshAll(2000).hasValue();
}

ShellDataConnectionObject* ShellReadOnlyDataController::connectionObject()
{
    return &connection_;
}

const ShellReadOnlyDataViewModel& ShellReadOnlyDataController::summaryViewModel() const noexcept
{
    return summary_;
}

QVariantMap ShellReadOnlyDataController::summaryViewModelMap() const
{
    return {
        {QStringLiteral("healthy"), summary_.healthy},
        {QStringLiteral("summaryText"), QString::fromStdString(summary_.summaryText)},
        {QStringLiteral("accountCount"), summary_.accountCount},
        {QStringLiteral("portfolioCount"), summary_.portfolioCount},
        {QStringLiteral("instrumentCount"), summary_.instrumentCount},
        {QStringLiteral("strategyCount"), summary_.strategyCount},
        {QStringLiteral("lastError"), QString::fromStdString(summary_.lastError)},
    };
}

ShellAccountListModel* ShellReadOnlyDataController::accountModel()
{
    return &accounts_;
}

ShellPortfolioListModel* ShellReadOnlyDataController::portfolioModel()
{
    return &portfolios_;
}

ShellInstrumentListModel* ShellReadOnlyDataController::instrumentModel()
{
    return &instruments_;
}

ShellStrategyListModel* ShellReadOnlyDataController::strategyModel()
{
    return &strategies_;
}

const std::string& ShellReadOnlyDataController::lastError() const noexcept
{
    return lastError_;
}

QString ShellReadOnlyDataController::lastErrorText() const
{
    return QString::fromStdString(lastError_);
}

ShellDataResult<bool> ShellReadOnlyDataController::failureFromResponse(
    const ShellDataResponse& response,
    const char* fallbackMessage)
{
    setLastError(response.errorMessage.empty() ? fallbackMessage : response.errorMessage);
    return ShellDataResult<bool>::failure(response.errorCode, lastError_);
}

ShellDataResult<bool> ShellReadOnlyDataController::failureFromError(
    const ShellDataError& error,
    const char* fallbackMessage)
{
    setLastError(error.message.empty() ? fallbackMessage : error.message);
    return ShellDataResult<bool>::failure(error.errorCode, lastError_);
}

void ShellReadOnlyDataController::setLastError(std::string message)
{
    lastError_ = std::move(message);
    summary_.lastError = lastError_;
    connection_.setLastError(lastError_);
    emit lastErrorChanged();
    notifySummaryChanged();
}

void ShellReadOnlyDataController::clearLastError()
{
    if (lastError_.empty() && summary_.lastError.empty()) {
        return;
    }
    lastError_.clear();
    summary_.lastError.clear();
    connection_.setLastError({});
    emit lastErrorChanged();
    notifySummaryChanged();
}

void ShellReadOnlyDataController::notifySummaryChanged()
{
    emit summaryChanged();
}

}  // namespace etfdt::shell_services
