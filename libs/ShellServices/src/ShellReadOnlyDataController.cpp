#include "ShellServices/ShellReadOnlyDataController.h"

#include <QByteArray>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>
#include <QTimer>
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

QString errorCodeText(std::optional<etfdt::protocol::ErrorCode> code)
{
    if (!code.has_value()) {
        return {};
    }
    const auto text = etfdt::protocol::toString(*code);
    return QString::fromUtf8(text.data(), static_cast<qsizetype>(text.size()));
}

}  // namespace

ShellReadOnlyDataController::ShellReadOnlyDataController(QObject* parent)
    : QObject(parent)
    , connection_(this)
    , accounts_(this)
    , portfolios_(this)
    , instruments_(this)
    , strategies_(this)
    , connectionPresets_(this)
{
}

ShellDataResult<bool> ShellReadOnlyDataController::connectToDataService(
    const std::string& socketName,
    int timeoutMs)
{
    if (socketName.empty()) {
        setLocalError(
            QStringLiteral("MISSING_SOCKET_NAME"),
            "socketName is required before connecting to DataService",
            QStringLiteral("connect"),
            true);
        setRefreshState(QStringLiteral("FAILED"));
        return ShellDataResult<bool>::failure(std::nullopt, lastError_);
    }
    if (isBusy_) {
        setLocalError(
            QStringLiteral("BUSY"),
            "connectToDataService ignored because another operation is in progress",
            QStringLiteral("connect"),
            true);
        return ShellDataResult<bool>::failure(std::nullopt, lastError_);
    }
    setBusy(true);
    setRefreshState(QStringLiteral("CONNECTING"));
    auto connected = facade_.connect(socketName, timeoutMs);
    connection_.updateFromState(facade_.connectionState());
    setBusy(false);
    if (!connected) {
        setRefreshState(QStringLiteral("FAILED"));
        auto failed = failureFromError(connected.error(), "connectToDataService failed");
        emit stateChanged();
        return failed;
    }
    clearError();
    setRefreshState(QStringLiteral("SUCCESS"));
    return ShellDataResult<bool>::success(true);
}

bool ShellReadOnlyDataController::connectToDataService(const QString& socketName)
{
    return connectToDataService(socketName.toStdString(), 2000).hasValue();
}

bool ShellReadOnlyDataController::connectToDataService()
{
    return connectToDataService(selectedSocketName_.toStdString(), 2000).hasValue();
}

void ShellReadOnlyDataController::disconnect()
{
    facade_.disconnect();
    connection_.updateFromState(facade_.connectionState());
    setBusy(false);
    setRefreshState(QStringLiteral("IDLE"));
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
    clearError();
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
    clearError();
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
    clearError();
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
    clearError();
    notifySummaryChanged();
    return ShellDataResult<bool>::success(true);
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshAccountsAndPortfolios(int timeoutMs)
{
    if (isBusy_) {
        setLocalError(
            QStringLiteral("BUSY"),
            "refreshAccountsAndPortfolios ignored because another operation is in progress",
            QStringLiteral("refreshAccountsAndPortfolios"),
            true);
        return ShellDataResult<bool>::failure(std::nullopt, lastError_);
    }
    if (isRefreshThrottled()) {
        ++refreshThrottleCount_;
        setLocalError(
            QStringLiteral("THROTTLED"),
            "refreshAccountsAndPortfolios throttled by minimum refresh interval",
            QStringLiteral("refreshAccountsAndPortfolios"),
            true);
        scheduleCanRefreshUpdate();
        emit stateChanged();
        return ShellDataResult<bool>::failure(std::nullopt, lastError_);
    }

    ++refreshAttemptCount_;
    lastRefreshStartedAtText_ = utcNowText();
    setBusy(true);
    setRefreshState(QStringLiteral("REFRESHING"));

    auto result = refreshAccounts(timeoutMs);
    if (result) {
        result = refreshPortfolios(timeoutMs);
    }

    lastRefreshFinishedAtText_ = utcNowText();
    setBusy(false);
    if (!result) {
        ++refreshFailureCount_;
        setRefreshState(QStringLiteral("FAILED"));
        emit stateChanged();
        return result;
    }

    ++refreshSuccessCount_;
    lastSuccessAtText_ = lastRefreshFinishedAtText_;
    clearError();
    setRefreshState(QStringLiteral("SUCCESS"));
    scheduleCanRefreshUpdate();
    return result;
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
    clearError();
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
    clearError();
    notifySummaryChanged();
    return ShellDataResult<bool>::success(true);
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshAll(int timeoutMs)
{
    if (isBusy_) {
        setLocalError(
            QStringLiteral("BUSY"),
            "refreshAll ignored because another operation is in progress",
            QStringLiteral("refreshAll"),
            true);
        return ShellDataResult<bool>::failure(std::nullopt, lastError_);
    }
    if (isRefreshThrottled()) {
        ++refreshThrottleCount_;
        setLocalError(
            QStringLiteral("THROTTLED"),
            "refreshAll throttled by minimum refresh interval",
            QStringLiteral("refreshAll"),
            true);
        scheduleCanRefreshUpdate();
        emit stateChanged();
        return ShellDataResult<bool>::failure(std::nullopt, lastError_);
    }

    ++refreshAttemptCount_;
    lastRefreshStartedAtText_ = utcNowText();
    setBusy(true);
    setRefreshState(QStringLiteral("REFRESHING"));

    auto result = refreshAllUnprotected(timeoutMs);
    lastRefreshFinishedAtText_ = utcNowText();
    setBusy(false);
    if (!result) {
        ++refreshFailureCount_;
        setRefreshState(QStringLiteral("FAILED"));
        emit stateChanged();
        return result;
    }

    ++refreshSuccessCount_;
    lastSuccessAtText_ = lastRefreshFinishedAtText_;
    clearError();
    setRefreshState(QStringLiteral("SUCCESS"));
    scheduleCanRefreshUpdate();
    return result;
}

ShellDataResult<bool> ShellReadOnlyDataController::refreshAllUnprotected(int timeoutMs)
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

bool ShellReadOnlyDataController::refreshAccountsAndPortfolios()
{
    return refreshAccountsAndPortfolios(2000).hasValue();
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

bool ShellReadOnlyDataController::refreshOtc(const QString& strategyCode)
{
    const auto trimmed = strategyCode.trimmed();
    if (trimmed.isEmpty()) {
        setLocalError(
            QStringLiteral("E1002_MISSING_REQUIRED_FIELD"),
            "strategyCode is required",
            QStringLiteral("listOtc"),
            true);
        return false;
    }
    auto result = facade_.listOtc(trimmed.toStdString(), 2000);
    if (!result) {
        (void)failureFromError(result.error(), "listOtc failed");
        return false;
    }
    if (!result.value().success) {
        (void)failureFromResponse(result.value(), "listOtc failed");
        return false;
    }
    clearError();
    return true;
}

bool ShellReadOnlyDataController::selectPreset(const QString& key)
{
    const auto* preset = connectionPresets_.findByKey(key);
    if (preset == nullptr) {
        setLocalError(
            QStringLiteral("UNKNOWN_PRESET"),
            "unknown read-only connection preset",
            QStringLiteral("connectionPreset"),
            true);
        return false;
    }

    selectedPresetKey_ = QString::fromStdString(preset->key);
    selectedSocketName_ = QString::fromStdString(preset->socketName);
    commandHint_ = QString::fromStdString(preset->commandHint);
    clearError();
    emit connectionPresetChanged();
    return true;
}

void ShellReadOnlyDataController::setCustomSocketName(const QString& socketName)
{
    selectedSocketName_ = socketName.trimmed();
    const auto* preset = connectionPresets_.findByKey(selectedPresetKey_);
    if (preset != nullptr && selectedSocketName_ != QString::fromStdString(preset->socketName)) {
        selectedPresetKey_ = QStringLiteral("custom");
        const auto* customPreset = connectionPresets_.findByKey(selectedPresetKey_);
        commandHint_ = customPreset == nullptr
            ? QStringLiteral("Enter a socketName manually.")
            : QString::fromStdString(customPreset->commandHint);
    }
    emit connectionPresetChanged();
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
        {QStringLiteral("refreshState"), refreshState_},
        {QStringLiteral("isBusy"), isBusy_},
        {QStringLiteral("lastSuccessAtText"), lastSuccessAtText_},
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

QAbstractItemModel* ShellReadOnlyDataController::connectionPresetItemModel()
{
    return &connectionPresets_;
}

ShellReadOnlyConnectionPresetModel* ShellReadOnlyDataController::connectionPresetModel()
{
    return &connectionPresets_;
}

const std::string& ShellReadOnlyDataController::lastError() const noexcept
{
    return lastError_;
}

QString ShellReadOnlyDataController::lastErrorText() const
{
    return QString::fromStdString(lastError_);
}

QString ShellReadOnlyDataController::selectedPresetKey() const
{
    return selectedPresetKey_;
}

QString ShellReadOnlyDataController::selectedSocketName() const
{
    return selectedSocketName_;
}

QString ShellReadOnlyDataController::commandHint() const
{
    return commandHint_;
}

QString ShellReadOnlyDataController::onboardingText() const
{
    return QStringLiteral(
        "Development read-only prototype. Start ETFDataService manually with the command hint, "
        "then connect to the selected socket. This page does not start services, persist config, "
        "or call write actions.");
}

QString ShellReadOnlyDataController::refreshState() const
{
    return refreshState_;
}

bool ShellReadOnlyDataController::isBusy() const noexcept
{
    return isBusy_;
}

bool ShellReadOnlyDataController::isConnecting() const
{
    return refreshState_ == QStringLiteral("CONNECTING");
}

bool ShellReadOnlyDataController::isRefreshing() const
{
    return refreshState_ == QStringLiteral("REFRESHING");
}

bool ShellReadOnlyDataController::canRefresh() const
{
    return !isBusy_ && connection_.connected() && !isRefreshThrottled();
}

QString ShellReadOnlyDataController::lastRefreshStartedAtText() const
{
    return lastRefreshStartedAtText_;
}

QString ShellReadOnlyDataController::lastRefreshFinishedAtText() const
{
    return lastRefreshFinishedAtText_;
}

QString ShellReadOnlyDataController::lastSuccessAtText() const
{
    return lastSuccessAtText_;
}

QVariantMap ShellReadOnlyDataController::errorStateMap() const
{
    return {
        {QStringLiteral("hasError"), hasError_},
        {QStringLiteral("errorCode"), errorCode_},
        {QStringLiteral("errorMessage"), errorMessage_},
        {QStringLiteral("errorSource"), errorSource_},
        {QStringLiteral("recoverable"), errorRecoverable_},
    };
}

int ShellReadOnlyDataController::refreshAttemptCount() const noexcept
{
    return refreshAttemptCount_;
}

int ShellReadOnlyDataController::refreshSuccessCount() const noexcept
{
    return refreshSuccessCount_;
}

int ShellReadOnlyDataController::refreshFailureCount() const noexcept
{
    return refreshFailureCount_;
}

int ShellReadOnlyDataController::refreshThrottleCount() const noexcept
{
    return refreshThrottleCount_;
}

void ShellReadOnlyDataController::setMinimumRefreshIntervalMs(int intervalMs) noexcept
{
    minimumRefreshIntervalMs_ = intervalMs < 0 ? 0 : intervalMs;
    emit stateChanged();
}

ShellDataResult<bool> ShellReadOnlyDataController::failureFromResponse(
    const ShellDataResponse& response,
    const char* fallbackMessage)
{
    setError(
        response.errorCode,
        response.errorMessage.empty() ? fallbackMessage : response.errorMessage,
        QStringLiteral("protocol"),
        true);
    return ShellDataResult<bool>::failure(response.errorCode, lastError_);
}

ShellDataResult<bool> ShellReadOnlyDataController::failureFromError(
    const ShellDataError& error,
    const char* fallbackMessage)
{
    setError(
        error.errorCode,
        error.message.empty() ? fallbackMessage : error.message,
        QStringLiteral("client"),
        true);
    return ShellDataResult<bool>::failure(error.errorCode, lastError_);
}

bool ShellReadOnlyDataController::isRefreshThrottled() const
{
    return remainingThrottleMs() > 0;
}

int ShellReadOnlyDataController::remainingThrottleMs() const
{
    if (minimumRefreshIntervalMs_ <= 0 || lastRefreshStartedAtText_.isEmpty()) {
        return 0;
    }
    const auto started =
        QDateTime::fromString(lastRefreshStartedAtText_, Qt::ISODateWithMs);
    if (!started.isValid()) {
        return 0;
    }
    const auto elapsed = started.msecsTo(QDateTime::currentDateTimeUtc());
    const auto remaining = minimumRefreshIntervalMs_ - static_cast<int>(elapsed);
    return remaining > 0 ? remaining : 0;
}

QString ShellReadOnlyDataController::utcNowText() const
{
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
}

void ShellReadOnlyDataController::setRefreshState(QString state)
{
    if (refreshState_ == state) {
        emit stateChanged();
        return;
    }
    refreshState_ = std::move(state);
    emit stateChanged();
}

void ShellReadOnlyDataController::setBusy(bool busy)
{
    if (isBusy_ == busy) {
        return;
    }
    isBusy_ = busy;
    emit stateChanged();
}

void ShellReadOnlyDataController::setError(
    std::optional<etfdt::protocol::ErrorCode> errorCode,
    std::string message,
    QString source,
    bool recoverable)
{
    errorCode_ = errorCodeText(errorCode);
    setLocalError(errorCode_, std::move(message), std::move(source), recoverable);
}

void ShellReadOnlyDataController::setLocalError(
    QString code,
    std::string message,
    QString source,
    bool recoverable)
{
    errorCode_ = std::move(code);
    errorMessage_ = QString::fromStdString(message);
    errorSource_ = std::move(source);
    errorRecoverable_ = recoverable;
    hasError_ = true;
    setLastError(std::move(message));
    emit errorStateChanged();
}

void ShellReadOnlyDataController::setLastError(std::string message)
{
    lastError_ = std::move(message);
    summary_.lastError = lastError_;
    connection_.setLastError(lastError_);
    emit lastErrorChanged();
    notifySummaryChanged();
}

void ShellReadOnlyDataController::clearError()
{
    if (lastError_.empty() && summary_.lastError.empty() && !hasError_) {
        return;
    }
    lastError_.clear();
    summary_.lastError.clear();
    hasError_ = false;
    errorCode_.clear();
    errorMessage_.clear();
    errorSource_.clear();
    errorRecoverable_ = true;
    connection_.setLastError(std::string{});
    emit lastErrorChanged();
    emit errorStateChanged();
    notifySummaryChanged();
}

void ShellReadOnlyDataController::notifySummaryChanged()
{
    emit summaryChanged();
}

void ShellReadOnlyDataController::scheduleCanRefreshUpdate()
{
    const auto remaining = remainingThrottleMs();
    if (remaining <= 0) {
        emit stateChanged();
        return;
    }
    QTimer::singleShot(remaining + 1, this, [this]() {
        emit stateChanged();
    });
}

}  // namespace etfdt::shell_services
