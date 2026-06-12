#include "ShellServices/ShellAccountingPresenter.h"

#include "ShellServices/ShellAccountingExcelVbaImportReadOnlyFileLoader.h"

#include <QByteArray>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace etfdt::shell_services {

namespace {

ShellAccountingIssue makePresenterIssue(
    std::string code,
    std::string message,
    bool blocking = true)
{
    return {
        std::move(code),
        blocking ? "ERROR" : "WARNING",
        std::move(message),
        blocking,
        "ShellAccountingPresenter",
    };
}

ShellAccountingStateSnapshot snapshotFromControllerState(
    std::string actionName,
    ShellAccountingViewState state,
    std::vector<ShellAccountingIssue> issues)
{
    ShellAccountingStateSnapshot snapshot;
    snapshot.actionName = std::move(actionName);
    snapshot.readOnly = true;
    snapshot.writeEnabled = false;
    snapshot.issues = std::move(issues);

    switch (state) {
    case ShellAccountingViewState::Idle:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "IDLE";
        break;
    case ShellAccountingViewState::Loading:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "LOADING";
        break;
    case ShellAccountingViewState::Loaded:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "OK";
        snapshot.hasRows = true;
        break;
    case ShellAccountingViewState::Empty:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "OK";
        snapshot.hasRows = false;
        break;
    case ShellAccountingViewState::Unavailable:
        snapshot.implemented = false;
        snapshot.dataQualityStatus = "UNAVAILABLE";
        snapshot.payloadStatus = "SHELL_ACCOUNTING_CONTROLLER_UNAVAILABLE";
        break;
    case ShellAccountingViewState::Warning:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "WARNING";
        snapshot.hasRows = true;
        break;
    case ShellAccountingViewState::Error:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "ERROR";
        break;
    case ShellAccountingViewState::Stale:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "STALE";
        snapshot.stale = true;
        snapshot.hasRows = true;
        break;
    }

    return snapshot;
}

ShellAccountingServiceRequest makePresenterRefreshRequest(std::string actionName)
{
    ShellAccountingServiceRequest request;
    request.actionName = std::move(actionName);
    request.includeIssues = true;
    request.includeDebugMetadata = false;
    request.requestedOutputs = {"guard"};
    request.timeoutMs = 200;
    return request;
}

int refreshStateRank(ShellAccountingViewState state) noexcept
{
    switch (state) {
    case ShellAccountingViewState::Error:
        return 6;
    case ShellAccountingViewState::Unavailable:
        return 5;
    case ShellAccountingViewState::Stale:
        return 4;
    case ShellAccountingViewState::Warning:
        return 3;
    case ShellAccountingViewState::Loaded:
        return 2;
    case ShellAccountingViewState::Empty:
        return 1;
    case ShellAccountingViewState::Idle:
    case ShellAccountingViewState::Loading:
        return 0;
    }
    return 0;
}

ShellAccountingViewState moreSevereRefreshState(
    ShellAccountingViewState current,
    ShellAccountingViewState next) noexcept
{
    return refreshStateRank(next) > refreshStateRank(current) ? next : current;
}

void appendIssues(
    std::vector<ShellAccountingIssue>& target,
    const std::vector<ShellAccountingIssue>& source)
{
    target.insert(target.end(), source.begin(), source.end());
}

QString issueTextFromResult(const ShellAccountingServiceResult& result)
{
    if (!result.errors.empty()) {
        return QString::fromStdString(result.errors.front().message);
    }
    if (!result.issues.empty()) {
        return QString::fromStdString(result.issues.front().message);
    }
    if (!result.warnings.empty()) {
        return QString::fromStdString(result.warnings.front().message);
    }
    if (!result.payloadStatus.empty() && result.payloadStatus != "OK") {
        return QString::fromStdString(result.payloadStatus);
    }
    return {};
}

QString issueTextFromIssues(const std::vector<ShellAccountingIssue>& issues)
{
    if (issues.empty()) {
        return {};
    }
    return QString::fromStdString(issues.front().message);
}

QJsonObject payloadObject(const ShellAccountingServiceResult& result)
{
    const auto document =
        QJsonDocument::fromJson(QByteArray::fromStdString(result.rawPayload));
    return document.isObject() ? document.object() : QJsonObject {};
}

QString stringField(const QJsonObject& object, const char* name)
{
    const auto value = object.value(QString::fromUtf8(name));
    if (value.isString()) {
        return value.toString();
    }
    if (value.isDouble()) {
        return QString::number(static_cast<qint64>(value.toDouble()));
    }
    return {};
}

bool boolField(const QJsonObject& object, const char* name)
{
    const auto value = object.value(QString::fromUtf8(name));
    return value.isBool() && value.toBool();
}

bool positiveIntegerText(const QString& text)
{
    bool ok = false;
    const auto value = text.trimmed().toLongLong(&ok);
    return ok && value > 0;
}

bool nonNegativeIntegerText(const QString& text)
{
    bool ok = false;
    const auto value = text.trimmed().toLongLong(&ok);
    return ok && value >= 0;
}

std::string trimmedStdString(const QString& text)
{
    return text.trimmed().toStdString();
}

QString canonicalAsciiToken(QString value)
{
    return value.trimmed().toUpper();
}

QString canonicalPreviewSheetName(const QString& sheetName)
{
    const auto key = sheetName.trimmed().toLower();
    if (key == QStringLiteral("initialcash") || key == QStringLiteral("initial_cash")
        || key == QStringLiteral("cash") || key == QStringLiteral("cashlog")
        || key == QStringLiteral("cashsummary")) {
        return QStringLiteral("InitialCash");
    }
    if (key == QStringLiteral("trades") || key == QStringLiteral("tradelog")
        || key == QStringLiteral("trade_log") || key == QStringLiteral("transactions")) {
        return QStringLiteral("Trades");
    }
    if (key == QStringLiteral("marketprices") || key == QStringLiteral("market_prices")
        || key == QStringLiteral("prices")) {
        return QStringLiteral("MarketPrices");
    }
    if (key == QStringLiteral("fxrates") || key == QStringLiteral("fx_rates")
        || key == QStringLiteral("foreignexchange")) {
        return QStringLiteral("FxRates");
    }
    return sheetName.trimmed();
}

QString canonicalPreviewHeaderName(const QString& header)
{
    const auto key = canonicalAsciiToken(header);
    if (key == QStringLiteral("ROW_ID") || key == QStringLiteral("ROWID")
        || key == QStringLiteral("ID")) {
        return QStringLiteral("ROW_ID");
    }
    if (key == QStringLiteral("TIME_UTC") || key == QStringLiteral("TIME")
        || key == QStringLiteral("OCCURRED_AT")) {
        return QStringLiteral("TIME_UTC");
    }
    if (key == QStringLiteral("TRADE_TIME_UTC") || key == QStringLiteral("TRADE_TIME")
        || key == QStringLiteral("TRADED_AT")) {
        return QStringLiteral("TRADE_TIME_UTC");
    }
    if (key == QStringLiteral("ACCOUNT_CODE") || key == QStringLiteral("ACCOUNT_ID")
        || key == QStringLiteral("ACCOUNT")) {
        return QStringLiteral("ACCOUNT_CODE");
    }
    if (key == QStringLiteral("PORTFOLIO_CODE") || key == QStringLiteral("PORTFOLIO_ID")
        || key == QStringLiteral("PORTFOLIO")) {
        return QStringLiteral("PORTFOLIO_CODE");
    }
    if (key == QStringLiteral("INSTRUMENT_CODE") || key == QStringLiteral("SECURITY_CODE")
        || key == QStringLiteral("SYMBOL")) {
        return QStringLiteral("INSTRUMENT_CODE");
    }
    if (key == QStringLiteral("SIDE") || key == QStringLiteral("ACTION")) {
        return key == QStringLiteral("ACTION") ? QStringLiteral("ACTION") : QStringLiteral("SIDE");
    }
    if (key == QStringLiteral("QUANTITY") || key == QStringLiteral("QTY")) {
        return QStringLiteral("QUANTITY");
    }
    if (key == QStringLiteral("PRICE") || key == QStringLiteral("TRADE_PRICE")) {
        return QStringLiteral("PRICE");
    }
    if (key == QStringLiteral("AMOUNT") || key == QStringLiteral("GROSS_AMOUNT")) {
        return QStringLiteral("AMOUNT");
    }
    if (key == QStringLiteral("FEE") || key == QStringLiteral("COMMISSION")) {
        return QStringLiteral("FEE");
    }
    if (key == QStringLiteral("CASH_FLOW") || key == QStringLiteral("CASHFLOW")) {
        return QStringLiteral("CASH_FLOW");
    }
    if (key == QStringLiteral("CURRENCY") || key == QStringLiteral("CCY")) {
        return QStringLiteral("CURRENCY");
    }
    if (key == QStringLiteral("SOURCE") || key == QStringLiteral("SRC")) {
        return QStringLiteral("SOURCE");
    }
    if (key == QStringLiteral("MEMO") || key == QStringLiteral("NOTE")
        || key == QStringLiteral("REMARK")) {
        return QStringLiteral("MEMO");
    }
    if (key == QStringLiteral("PRICE_TIME_UTC") || key == QStringLiteral("PRICE_TIME")) {
        return QStringLiteral("PRICE_TIME_UTC");
    }
    if (key == QStringLiteral("BASE_CURRENCY") || key == QStringLiteral("BASE_CCY")) {
        return QStringLiteral("BASE_CURRENCY");
    }
    if (key == QStringLiteral("QUOTE_CURRENCY") || key == QStringLiteral("QUOTE_CCY")) {
        return QStringLiteral("QUOTE_CURRENCY");
    }
    if (key == QStringLiteral("RATE_TIME_UTC") || key == QStringLiteral("RATE_TIME")) {
        return QStringLiteral("RATE_TIME_UTC");
    }
    if (key == QStringLiteral("RATE") || key == QStringLiteral("FX_RATE")) {
        return QStringLiteral("RATE");
    }
    return key;
}

QString previewCellText(const QJsonValue& value)
{
    if (value.isString()) {
        return value.toString().trimmed();
    }
    if (value.isDouble()) {
        return QString::number(value.toDouble(), 'g', 15);
    }
    return {};
}

std::optional<QJsonObject> findPreviewSheet(const QJsonArray& sheets, const QString& canonicalName)
{
    for (const auto& value : sheets) {
        if (!value.isObject()) {
            continue;
        }
        const auto sheet = value.toObject();
        if (canonicalPreviewSheetName(sheet.value(QStringLiteral("name")).toString()) == canonicalName) {
            return sheet;
        }
    }
    return std::nullopt;
}

std::vector<QString> previewHeaders(const QJsonObject& sheet)
{
    std::vector<QString> headers;
    const auto values = sheet.value(QStringLiteral("headers")).toArray();
    headers.reserve(static_cast<std::size_t>(values.size()));
    for (const auto& value : values) {
        headers.push_back(canonicalPreviewHeaderName(previewCellText(value)));
    }
    return headers;
}

std::optional<int> previewHeaderIndex(const std::vector<QString>& headers, const QString& field)
{
    for (std::size_t index = 0; index < headers.size(); ++index) {
        if (headers[index] == field) {
            return static_cast<int>(index);
        }
    }
    return std::nullopt;
}

QString previewCellValue(const std::vector<QString>& headers, const QJsonArray& row, const QString& field)
{
    const auto index = previewHeaderIndex(headers, field);
    if (!index.has_value() || *index < 0 || *index >= row.size()) {
        return {};
    }
    return previewCellText(row.at(*index));
}

QString normalizedPreviewTradeAction(const QString& value)
{
    const auto action = canonicalAsciiToken(value);
    if (action == QStringLiteral("BUY") || action == QStringLiteral("SELL")) {
        return action;
    }
    return {};
}

QString normalizedPreviewCashAction(const QString& value)
{
    const auto action = canonicalAsciiToken(value);
    if (action == QStringLiteral("INITIALCASH")) {
        return QStringLiteral("INITIAL_CASH");
    }
    if (action == QStringLiteral("INITIAL_CASH") || action == QStringLiteral("DEPOSIT")
        || action == QStringLiteral("WITHDRAW") || action == QStringLiteral("ADJUSTMENT")) {
        return action;
    }
    if (action == QStringLiteral("WITHDRAWAL")) {
        return QStringLiteral("WITHDRAW");
    }
    return {};
}

void hashPreviewText(std::uint64_t& hash, const QString& value)
{
    const auto bytes = value.toStdString();
    for (const unsigned char ch : bytes) {
        hash ^= ch;
        hash *= 1099511628211ULL;
    }
}

void hashPreviewField(std::uint64_t& hash, const QString& value)
{
    hashPreviewText(hash, value);
    hashPreviewText(hash, QStringLiteral("|"));
}

QString hexFnv64(std::uint64_t hash)
{
    std::ostringstream stream;
    stream << "fnv64-" << std::hex << std::setw(16) << std::setfill('0') << hash;
    return QString::fromStdString(stream.str());
}

}  // namespace

ShellAccountingPresenter::ShellAccountingPresenter(QObject* parent)
    : QObject(parent)
{
}

bool ShellAccountingPresenter::readOnly() const noexcept
{
    return true;
}

bool ShellAccountingPresenter::writeEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::tradeDraftGenerationEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::tradeSuggestionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::strategyExecutionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::brokerSubmissionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::productionTradingUiEnabled() const noexcept
{
    return true;
}

QString ShellAccountingPresenter::tradingUiStatus() const
{
    return tradingUiStatus_;
}

QString ShellAccountingPresenter::tradingUiIssue() const
{
    return tradingUiIssue_;
}

QString ShellAccountingPresenter::draftId() const
{
    return draftId_;
}

QString ShellAccountingPresenter::draftUid() const
{
    return draftUid_;
}

QString ShellAccountingPresenter::ledgerStatus() const
{
    return ledgerStatus_;
}

bool ShellAccountingPresenter::manualEntryBusy() const noexcept
{
    return manualEntryBusy_;
}

QString ShellAccountingPresenter::lastManualEntryStatus() const
{
    return lastManualEntryStatus_;
}

QString ShellAccountingPresenter::lastManualEntryIssue() const
{
    return lastManualEntryIssue_;
}

QString ShellAccountingPresenter::lastManualEntryResult() const
{
    return lastManualEntryResult_;
}

bool ShellAccountingPresenter::postWriteRefreshEnabled() const noexcept
{
    return postWriteRefreshEnabled_;
}

bool ShellAccountingPresenter::postWriteRefreshBusy() const noexcept
{
    return postWriteRefreshBusy_;
}

QString ShellAccountingPresenter::lastPostWriteRefreshStatus() const
{
    return lastPostWriteRefreshStatus_;
}

QString ShellAccountingPresenter::lastPostWriteRefreshIssue() const
{
    return lastPostWriteRefreshIssue_;
}

QString ShellAccountingPresenter::lastPostWriteRefreshSummary() const
{
    return lastPostWriteRefreshSummary_;
}

bool ShellAccountingPresenter::excelVbaImportPreviewBusy() const noexcept
{
    return excelVbaImportPreviewBusy_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPreviewStatus() const
{
    return lastExcelVbaImportPreviewStatus_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPreviewIssue() const
{
    return lastExcelVbaImportPreviewIssue_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPreviewSummary() const
{
    return lastExcelVbaImportPreviewSummary_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPreviewDiagnosticCodes() const
{
    return lastExcelVbaImportPreviewDiagnosticCodes_;
}

int ShellAccountingPresenter::excelVbaImportPreviewTradeFactCount() const noexcept
{
    return excelVbaImportPreviewTradeFactCount_;
}

int ShellAccountingPresenter::excelVbaImportPreviewCashFactCount() const noexcept
{
    return excelVbaImportPreviewCashFactCount_;
}

int ShellAccountingPresenter::excelVbaImportPreviewMarketPriceFactCount() const noexcept
{
    return excelVbaImportPreviewMarketPriceFactCount_;
}

int ShellAccountingPresenter::excelVbaImportPreviewFxRateFactCount() const noexcept
{
    return excelVbaImportPreviewFxRateFactCount_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPreviewDigest() const
{
    return lastExcelVbaImportPreviewDigest_;
}

bool ShellAccountingPresenter::excelVbaImportPreviewPayloadAvailable() const noexcept
{
    return !lastExcelVbaImportPreviewPayloadJson_.isEmpty();
}

bool ShellAccountingPresenter::excelVbaImportPersistBusy() const noexcept
{
    return excelVbaImportPersistBusy_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPersistStatus() const
{
    return lastExcelVbaImportPersistStatus_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPersistIssue() const
{
    return lastExcelVbaImportPersistIssue_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPersistSummary() const
{
    return lastExcelVbaImportPersistSummary_;
}

QString ShellAccountingPresenter::lastExcelVbaImportPersistIssueCodes() const
{
    return lastExcelVbaImportPersistIssueCodes_;
}

bool ShellAccountingPresenter::lastExcelVbaImportPersistTransactionCommitted() const noexcept
{
    return lastExcelVbaImportPersistTransactionCommitted_;
}

bool ShellAccountingPresenter::lastExcelVbaImportPersistTradeLogWritten() const noexcept
{
    return lastExcelVbaImportPersistTradeLogWritten_;
}

bool ShellAccountingPresenter::lastExcelVbaImportPersistAuditLogWritten() const noexcept
{
    return lastExcelVbaImportPersistAuditLogWritten_;
}

bool ShellAccountingPresenter::lastExcelVbaImportPersistDuplicateImportPrevented() const noexcept
{
    return lastExcelVbaImportPersistDuplicateImportPrevented_;
}

bool ShellAccountingPresenter::lastExcelVbaImportPersistIdempotencyConflictRejected() const noexcept
{
    return lastExcelVbaImportPersistIdempotencyConflictRejected_;
}

int ShellAccountingPresenter::lastExcelVbaImportPersistTradeLogRowsWritten() const noexcept
{
    return lastExcelVbaImportPersistTradeLogRowsWritten_;
}

int ShellAccountingPresenter::lastExcelVbaImportPersistCashAdjustmentRowsWritten() const noexcept
{
    return lastExcelVbaImportPersistCashAdjustmentRowsWritten_;
}

bool ShellAccountingPresenter::portfolioReplayBusy() const noexcept
{
    return portfolioReplayBusy_;
}

QString ShellAccountingPresenter::lastPortfolioReplayStatus() const
{
    return lastPortfolioReplayStatus_;
}

QString ShellAccountingPresenter::lastPortfolioReplayIssue() const
{
    return lastPortfolioReplayIssue_;
}

QString ShellAccountingPresenter::lastPortfolioReplaySummary() const
{
    return lastPortfolioReplaySummary_;
}

QString ShellAccountingPresenter::lastPortfolioReplayIssueCodes() const
{
    return lastPortfolioReplayIssueCodes_;
}

int ShellAccountingPresenter::portfolioReplayPositionCount() const noexcept
{
    return portfolioReplayPositionCount_;
}

int ShellAccountingPresenter::portfolioReplayCashSummaryCount() const noexcept
{
    return portfolioReplayCashSummaryCount_;
}

QString ShellAccountingPresenter::portfolioReplayRealizedPnl() const
{
    return portfolioReplayRealizedPnl_;
}

QString ShellAccountingPresenter::portfolioReplayUnrealizedPnl() const
{
    return portfolioReplayUnrealizedPnl_;
}

bool ShellAccountingPresenter::strategyRecommendationBusy() const noexcept
{
    return strategyRecommendationBusy_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationStatus() const
{
    return lastStrategyRecommendationStatus_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationAction() const
{
    return lastStrategyRecommendationAction_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationSource() const
{
    return lastStrategyRecommendationSource_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationReason() const
{
    return lastStrategyRecommendationReason_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationTier() const
{
    return lastStrategyRecommendationTier_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationQuantityText() const
{
    return lastStrategyRecommendationQuantityText_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationAmountText() const
{
    return lastStrategyRecommendationAmountText_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationNetCashImpactText() const
{
    return lastStrategyRecommendationNetCashImpactText_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationIssueCodes() const
{
    return lastStrategyRecommendationIssueCodes_;
}

QString ShellAccountingPresenter::lastStrategyRecommendationSummary() const
{
    return lastStrategyRecommendationSummary_;
}

QString ShellAccountingPresenter::lastTradeDraftStatus() const
{
    return lastTradeDraftStatus_;
}

QString ShellAccountingPresenter::lastTradeDraftId() const
{
    return lastTradeDraftId_;
}

QString ShellAccountingPresenter::lastTradeDraftSide() const
{
    return lastTradeDraftSide_;
}

QString ShellAccountingPresenter::lastTradeDraftInstrumentCode() const
{
    return lastTradeDraftInstrumentCode_;
}

QString ShellAccountingPresenter::lastTradeDraftQuantityText() const
{
    return lastTradeDraftQuantityText_;
}

QString ShellAccountingPresenter::lastTradeDraftAmountText() const
{
    return lastTradeDraftAmountText_;
}

QString ShellAccountingPresenter::lastTradeDraftNetCashImpactText() const
{
    return lastTradeDraftNetCashImpactText_;
}

QString ShellAccountingPresenter::lastTradeDraftIssueCodes() const
{
    return lastTradeDraftIssueCodes_;
}

QString ShellAccountingPresenter::lastTradeDraftSummary() const
{
    return lastTradeDraftSummary_;
}

bool ShellAccountingPresenter::lastTradeDraftDuplicate() const noexcept
{
    return lastTradeDraftDuplicate_;
}

bool ShellAccountingPresenter::lastTradeDraftIdempotencyConflict() const noexcept
{
    return lastTradeDraftIdempotencyConflict_;
}

bool ShellAccountingPresenter::lastTradeDraftUserConfirmationRequired() const noexcept
{
    return lastTradeDraftUserConfirmationRequired_;
}

QString ShellAccountingPresenter::lastOtcMapPreviewStatus() const
{
    return lastOtcMapPreviewStatus_;
}

int ShellAccountingPresenter::lastOtcMapPreviewLegCount() const noexcept
{
    return lastOtcMapPreviewLegCount_;
}

QString ShellAccountingPresenter::lastOtcMapPreviewTotalAmountText() const
{
    return lastOtcMapPreviewTotalAmountText_;
}

QString ShellAccountingPresenter::lastOtcMapPreviewSummary() const
{
    return lastOtcMapPreviewSummary_;
}

QString ShellAccountingPresenter::lastOtcMapPreviewIssueCodes() const
{
    return lastOtcMapPreviewIssueCodes_;
}

QString ShellAccountingPresenter::lastOtcMapDraftStatus() const
{
    return lastOtcMapDraftStatus_;
}

QString ShellAccountingPresenter::lastOtcMapDraftId() const
{
    return lastOtcMapDraftId_;
}

int ShellAccountingPresenter::lastOtcMapDraftLegCount() const noexcept
{
    return lastOtcMapDraftLegCount_;
}

bool ShellAccountingPresenter::lastOtcMapDraftDuplicate() const noexcept
{
    return lastOtcMapDraftDuplicate_;
}

bool ShellAccountingPresenter::lastOtcMapDraftIdempotencyConflict() const noexcept
{
    return lastOtcMapDraftIdempotencyConflict_;
}

QString ShellAccountingPresenter::lastOtcMapDraftSummary() const
{
    return lastOtcMapDraftSummary_;
}

QString ShellAccountingPresenter::lastOtcMapDraftIssueCodes() const
{
    return lastOtcMapDraftIssueCodes_;
}

ShellAccountingStatusObject& ShellAccountingPresenter::statusObject() noexcept
{
    return status_;
}

const ShellAccountingStatusObject& ShellAccountingPresenter::statusObject() const noexcept
{
    return status_;
}

ShellAccountingIssueListModel& ShellAccountingPresenter::issueListModel() noexcept
{
    return issues_;
}

const ShellAccountingIssueListModel& ShellAccountingPresenter::issueListModel() const noexcept
{
    return issues_;
}

ShellPositionListModel& ShellAccountingPresenter::positionListModel() noexcept
{
    return positions_;
}

const ShellPositionListModel& ShellAccountingPresenter::positionListModel() const noexcept
{
    return positions_;
}

void ShellAccountingPresenter::setController(
    std::shared_ptr<ShellAccountingReadOnlyController> controller)
{
    controller_ = std::move(controller);
    if (controller_) {
        controller_->setPrivacyMode(privacyMode_);
        syncFromController();
    }
}

void ShellAccountingPresenter::clearController() noexcept
{
    controller_.reset();
}

bool ShellAccountingPresenter::hasController() const noexcept
{
    return static_cast<bool>(controller_);
}

void ShellAccountingPresenter::setPrivacyMode(bool enabled)
{
    privacyMode_ = enabled;
    positions_.setPrivacyMode(enabled);
    if (controller_) {
        controller_->setPrivacyMode(enabled);
    }
}

bool ShellAccountingPresenter::privacyMode() const noexcept
{
    return privacyMode_;
}

void ShellAccountingPresenter::syncFromController()
{
    if (!controller_) {
        return;
    }

    applyControllerState(
        controller_->currentActionName(),
        controller_->currentState(),
        controller_->issues());
}

void ShellAccountingPresenter::refreshPositionList()
{
    refreshAccountingAction(
        "position.list",
        &ShellAccountingReadOnlyController::refreshPositionList);
}

void ShellAccountingPresenter::refreshCashSummary()
{
    refreshAccountingAction(
        "cash.summary",
        &ShellAccountingReadOnlyController::refreshCashSummary);
}

void ShellAccountingPresenter::refreshPortfolioPnlSummary()
{
    refreshAccountingAction(
        "portfolio.pnl.summary",
        &ShellAccountingReadOnlyController::refreshPortfolioPnlSummary);
}

void ShellAccountingPresenter::refreshBasePositionSummary()
{
    refreshAccountingAction(
        "base_position.summary",
        &ShellAccountingReadOnlyController::refreshBasePositionSummary);
}

void ShellAccountingPresenter::refreshSniperPoolSummary()
{
    refreshAccountingAction(
        "sniper_pool.summary",
        &ShellAccountingReadOnlyController::refreshSniperPoolSummary);
}

void ShellAccountingPresenter::refreshAllReadOnly()
{
    if (!controller_) {
        markControllerNotConfigured("accounting.refresh_all");
        return;
    }

    struct RefreshStep final {
        const char* actionName;
        void (ShellAccountingReadOnlyController::*refresh)(
            const ShellAccountingServiceRequest&);
    };

    const RefreshStep steps[] = {
        {"position.list", &ShellAccountingReadOnlyController::refreshPositionList},
        {"cash.summary", &ShellAccountingReadOnlyController::refreshCashSummary},
        {"portfolio.pnl.summary", &ShellAccountingReadOnlyController::refreshPortfolioPnlSummary},
        {"base_position.summary", &ShellAccountingReadOnlyController::refreshBasePositionSummary},
        {"sniper_pool.summary", &ShellAccountingReadOnlyController::refreshSniperPoolSummary},
    };

    ShellAccountingViewState aggregateState = ShellAccountingViewState::Idle;
    std::vector<ShellAccountingIssue> aggregateIssues;

    for (const auto& step : steps) {
        auto request = makePresenterRefreshRequest(step.actionName);
        (controller_.get()->*step.refresh)(request);
        aggregateState = moreSevereRefreshState(aggregateState, controller_->currentState());
        appendIssues(aggregateIssues, controller_->issues());
    }

    applyControllerState("accounting.refresh_all", aggregateState, std::move(aggregateIssues));
}

bool ShellAccountingPresenter::createDraft(
    const QString& accountId,
    const QString& portfolioId,
    const QString& instrumentId,
    const QString& instrumentCode,
    const QString& side,
    const QString& quantity,
    const QString& reason)
{
    if (!controller_) {
        tradingUiStatus_ = QStringLiteral("ERROR");
        tradingUiIssue_ = QStringLiteral("Shell accounting controller is not configured.");
        emit tradingUiStateChanged();
        markControllerNotConfigured("accounting.tradedraft.create");
        return false;
    }

    bool valid = false;
    auto request = makeDraftCreateRequest(
        accountId,
        portfolioId,
        instrumentId,
        instrumentCode,
        side,
        quantity,
        reason,
        valid);
    if (!valid) {
        emit tradingUiStateChanged();
        return false;
    }

    const auto result = controller_->createDraft(request);
    applyTradingResult(result, false);
    syncFromController();
    return tradingUiStatus_ == QStringLiteral("DRAFT_ONLY")
        || tradingUiStatus_ == QStringLiteral("DRAFT_DUPLICATE");
}

bool ShellAccountingPresenter::confirmDraft()
{
    if (!controller_) {
        tradingUiStatus_ = QStringLiteral("ERROR");
        tradingUiIssue_ = QStringLiteral("Shell accounting controller is not configured.");
        emit tradingUiStateChanged();
        markControllerNotConfigured("accounting.tradedraft.confirm");
        return false;
    }

    bool valid = false;
    auto request = makeDraftConfirmRequest(valid);
    if (!valid) {
        tradingUiStatus_ = QStringLiteral("INPUT_ERROR");
        tradingUiIssue_ = QStringLiteral("Create and review a draft before confirmation.");
        emit tradingUiStateChanged();
        return false;
    }

    const auto result = controller_->confirmDraft(request);
    applyTradingResult(result, true);
    syncFromController();
    return tradingUiStatus_ == QStringLiteral("CONFIRMED_LEDGER")
        || tradingUiStatus_ == QStringLiteral("CONFIRM_DUPLICATE");
}

void ShellAccountingPresenter::resetTradingUi()
{
    tradingUiStatus_ = QStringLiteral("READY");
    tradingUiIssue_.clear();
    draftId_.clear();
    draftUid_.clear();
    ledgerStatus_ = QStringLiteral("NOT_CONFIRMED");
    lastDraftRequest_ = {};
    emit tradingUiStateChanged();
}

bool ShellAccountingPresenter::submitManualTransaction(
    const QString& accountId,
    const QString& portfolioId,
    const QString& instrumentId,
    const QString& securityCode,
    const QString& side,
    const QString& quantityUnits,
    const QString& priceAmountMinor,
    const QString& grossAmountMinor,
    const QString& feeAmountMinor,
    const QString& taxAmountMinor,
    const QString& occurredAt,
    const QString& sourceMemo,
    const QString& requestId,
    const QString& idempotencyKey)
{
    if (!controller_) {
        markManualEntryInputError(QStringLiteral("Shell accounting controller is not configured."));
        markControllerNotConfigured("accounting.manual_transaction.create");
        return false;
    }

    bool valid = false;
    auto request = makeManualTransactionRequest(
        accountId,
        portfolioId,
        instrumentId,
        securityCode,
        side,
        quantityUnits,
        priceAmountMinor,
        grossAmountMinor,
        feeAmountMinor,
        taxAmountMinor,
        occurredAt,
        sourceMemo,
        requestId,
        idempotencyKey,
        valid);
    if (!valid) {
        return false;
    }

    manualEntryBusy_ = true;
    lastManualEntryStatus_ = QStringLiteral("SUBMITTING");
    lastManualEntryIssue_.clear();
    emit manualEntryStateChanged();
    const auto result = controller_->submitManualTransaction(request);
    manualEntryBusy_ = false;
    applyManualEntryResult(result, QStringLiteral("Manual transaction write failed."));
    const bool writeSucceeded = lastManualEntryStatus_ == QStringLiteral("OK")
        || lastManualEntryStatus_ == QStringLiteral("DUPLICATE");
    if (writeSucceeded) {
        refreshAfterManualEntryWrite(true, "manual_transaction");
    }
    syncFromController();
    return writeSucceeded;
}

bool ShellAccountingPresenter::submitManualCashMovement(
    const QString& accountId,
    const QString& portfolioId,
    const QString& movementType,
    const QString& amountMinor,
    const QString& currency,
    const QString& occurredAt,
    const QString& sourceMemo,
    const QString& sourceReference,
    const QString& requestId,
    const QString& idempotencyKey)
{
    if (!controller_) {
        markManualEntryInputError(QStringLiteral("Shell accounting controller is not configured."));
        markControllerNotConfigured("accounting.manual_cash_movement.create");
        return false;
    }

    bool valid = false;
    auto request = makeManualCashMovementRequest(
        accountId,
        portfolioId,
        movementType,
        amountMinor,
        currency,
        occurredAt,
        sourceMemo,
        sourceReference,
        requestId,
        idempotencyKey,
        valid);
    if (!valid) {
        return false;
    }

    manualEntryBusy_ = true;
    lastManualEntryStatus_ = QStringLiteral("SUBMITTING");
    lastManualEntryIssue_.clear();
    emit manualEntryStateChanged();
    const auto result = controller_->submitManualCashMovement(request);
    manualEntryBusy_ = false;
    applyManualEntryResult(result, QStringLiteral("Manual cash movement write failed."));
    const bool writeSucceeded = lastManualEntryStatus_ == QStringLiteral("OK")
        || lastManualEntryStatus_ == QStringLiteral("DUPLICATE");
    if (writeSucceeded) {
        refreshAfterManualEntryWrite(false, "manual_cash_movement");
    }
    syncFromController();
    return writeSucceeded;
}

void ShellAccountingPresenter::resetManualEntryUi()
{
    manualEntryBusy_ = false;
    lastManualEntryStatus_ = QStringLiteral("READY");
    lastManualEntryIssue_.clear();
    lastManualEntryResult_.clear();
    emit manualEntryStateChanged();
}

void ShellAccountingPresenter::resetPostWriteRefreshState()
{
    postWriteRefreshBusy_ = false;
    lastPostWriteRefreshStatus_ = QStringLiteral("READY");
    lastPostWriteRefreshIssue_.clear();
    lastPostWriteRefreshSummary_.clear();
    emit postWriteRefreshStateChanged();
}

bool ShellAccountingPresenter::refreshManualEntryReadback()
{
    if (!controller_) {
        postWriteRefreshBusy_ = false;
        lastPostWriteRefreshStatus_ = QStringLiteral("UNAVAILABLE");
        lastPostWriteRefreshIssue_ = QStringLiteral("Shell accounting controller is not configured.");
        lastPostWriteRefreshSummary_.clear();
        emit postWriteRefreshStateChanged();
        markControllerNotConfigured("accounting.manual_entry.post_write_readback_refresh");
        return false;
    }

    refreshAfterManualEntryWrite(true, "manual_entry_user_refresh");
    return lastPostWriteRefreshStatus_ == QStringLiteral("OK")
        || lastPostWriteRefreshStatus_ == QStringLiteral("WARNING")
        || lastPostWriteRefreshStatus_ == QStringLiteral("EMPTY");
}

void ShellAccountingPresenter::resetExcelVbaImportPreviewState()
{
    excelVbaImportPreviewBusy_ = false;
    lastExcelVbaImportPreviewStatus_ = QStringLiteral("READY");
    lastExcelVbaImportPreviewIssue_.clear();
    lastExcelVbaImportPreviewSummary_.clear();
    lastExcelVbaImportPreviewDiagnosticCodes_.clear();
    excelVbaImportPreviewTradeFactCount_ = 0;
    excelVbaImportPreviewCashFactCount_ = 0;
    excelVbaImportPreviewMarketPriceFactCount_ = 0;
    excelVbaImportPreviewFxRateFactCount_ = 0;
    lastExcelVbaImportPreviewDigest_.clear();
    lastExcelVbaImportPreviewPayloadJson_.clear();
    lastExcelVbaImportPreviewSchemaVersion_.clear();
    lastExcelVbaImportPreviewSource_.clear();
    emit excelVbaImportPreviewStateChanged();
    resetExcelVbaImportPersistState();
}

bool ShellAccountingPresenter::previewExcelVbaImportReadOnly(
    const QString& importPayloadJson)
{
    if (!controller_) {
        excelVbaImportPreviewBusy_ = false;
        lastExcelVbaImportPreviewStatus_ = QStringLiteral("ERROR");
        lastExcelVbaImportPreviewIssue_ =
            QStringLiteral("Shell accounting controller is not configured.");
        lastExcelVbaImportPreviewSummary_.clear();
        emit excelVbaImportPreviewStateChanged();
        markControllerNotConfigured("accounting.excel_vba_import.readonly_preview");
        return false;
    }

    bool valid = false;
    auto request = makeExcelVbaImportPreviewRequest(importPayloadJson, valid);
    if (!valid) {
        return false;
    }

    excelVbaImportPreviewBusy_ = true;
    lastExcelVbaImportPreviewStatus_ = QStringLiteral("PREVIEWING");
    lastExcelVbaImportPreviewIssue_.clear();
    emit excelVbaImportPreviewStateChanged();
    const auto result = controller_->previewExcelVbaImportReadOnly(request);
    excelVbaImportPreviewBusy_ = false;
    applyExcelVbaImportPreviewResult(result);
    if (lastExcelVbaImportPreviewStatus_ == QStringLiteral("ACCEPTED")) {
        bool digestValid = false;
        const auto payloadJson = QString::fromStdString(request.importPayloadJson);
        const auto digest = acceptedExcelVbaImportPreviewDigest(payloadJson, digestValid);
        if (digestValid && !digest.isEmpty()) {
            QJsonParseError parseError {};
            const auto document =
                QJsonDocument::fromJson(payloadJson.toUtf8(), &parseError);
            const auto object = document.isObject() ? document.object() : QJsonObject {};
            lastExcelVbaImportPreviewPayloadJson_ = payloadJson;
            lastExcelVbaImportPreviewDigest_ = digest;
            lastExcelVbaImportPreviewSchemaVersion_ =
                object.value(QStringLiteral("schemaVersion")).toString().trimmed();
            lastExcelVbaImportPreviewSource_ =
                object.value(QStringLiteral("source")).toString().trimmed();
        } else {
            lastExcelVbaImportPreviewPayloadJson_.clear();
            lastExcelVbaImportPreviewDigest_.clear();
            lastExcelVbaImportPreviewSchemaVersion_.clear();
            lastExcelVbaImportPreviewSource_.clear();
        }
        emit excelVbaImportPreviewStateChanged();
    } else {
        lastExcelVbaImportPreviewPayloadJson_.clear();
        lastExcelVbaImportPreviewDigest_.clear();
        lastExcelVbaImportPreviewSchemaVersion_.clear();
        lastExcelVbaImportPreviewSource_.clear();
        emit excelVbaImportPreviewStateChanged();
    }
    syncFromController();
    return lastExcelVbaImportPreviewStatus_ == QStringLiteral("ACCEPTED");
}

bool ShellAccountingPresenter::previewExcelVbaImportReadOnlyFromLocalFile(
    const QString& fileUrlOrPath)
{
    const auto loaded =
        ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(fileUrlOrPath);
    if (!loaded.success) {
        markExcelVbaImportPreviewInputError(loaded.issue);
        return false;
    }

    return previewExcelVbaImportReadOnly(loaded.payloadJson);
}

void ShellAccountingPresenter::resetExcelVbaImportPersistState()
{
    excelVbaImportPersistBusy_ = false;
    lastExcelVbaImportPersistStatus_ = QStringLiteral("READY");
    lastExcelVbaImportPersistIssue_.clear();
    lastExcelVbaImportPersistSummary_.clear();
    lastExcelVbaImportPersistIssueCodes_.clear();
    lastExcelVbaImportPersistTransactionCommitted_ = false;
    lastExcelVbaImportPersistTradeLogWritten_ = false;
    lastExcelVbaImportPersistAuditLogWritten_ = false;
    lastExcelVbaImportPersistDuplicateImportPrevented_ = false;
    lastExcelVbaImportPersistIdempotencyConflictRejected_ = false;
    lastExcelVbaImportPersistTradeLogRowsWritten_ = 0;
    lastExcelVbaImportPersistCashAdjustmentRowsWritten_ = 0;
    emit excelVbaImportPersistStateChanged();
}

void ShellAccountingPresenter::resetPortfolioReplayState()
{
    portfolioReplayBusy_ = false;
    lastPortfolioReplayStatus_ = QStringLiteral("READY");
    lastPortfolioReplayIssue_.clear();
    lastPortfolioReplaySummary_.clear();
    lastPortfolioReplayIssueCodes_.clear();
    portfolioReplayPositionCount_ = 0;
    portfolioReplayCashSummaryCount_ = 0;
    portfolioReplayRealizedPnl_.clear();
    portfolioReplayUnrealizedPnl_.clear();
    emit portfolioReplayStateChanged();
}

bool ShellAccountingPresenter::previewPortfolioReplayReadOnlySummary(
    const QString& replayPayloadJson)
{
    if (!controller_) {
        markPortfolioReplayInputError(
            QStringLiteral("Shell accounting controller is not configured."));
        markControllerNotConfigured("accounting.portfolio_replay.readonly_summary");
        return false;
    }

    bool valid = false;
    auto request = makePortfolioReplayReadOnlySummaryRequest(replayPayloadJson, valid);
    if (!valid) {
        return false;
    }

    portfolioReplayBusy_ = true;
    lastPortfolioReplayStatus_ = QStringLiteral("REPLAYING");
    lastPortfolioReplayIssue_.clear();
    emit portfolioReplayStateChanged();
    const auto result = controller_->fetchPortfolioReplayReadOnlySummary(request);
    portfolioReplayBusy_ = false;

    lastPortfolioReplayStatus_ = QString::fromStdString(result.payloadStatus);
    if (lastPortfolioReplayStatus_.isEmpty()) {
        lastPortfolioReplayStatus_ = result.portfolioReplayAccepted
            ? QStringLiteral("COMPLETED")
            : QStringLiteral("REJECTED");
    }
    lastPortfolioReplayIssue_ = issueTextFromResult(result);
    portfolioReplayPositionCount_ = result.portfolioReplayPositionCount;
    portfolioReplayCashSummaryCount_ = result.portfolioReplayCashSummaryCount;
    portfolioReplayRealizedPnl_ =
        QString::fromStdString(result.portfolioReplayRealizedPnlText);
    portfolioReplayUnrealizedPnl_ =
        QString::fromStdString(result.portfolioReplayUnrealizedPnlText);
    lastPortfolioReplayIssueCodes_ =
        QString::fromStdString([&]() {
            std::ostringstream stream;
            for (std::size_t index = 0; index < result.portfolioReplayIssueCodes.size(); ++index) {
                if (index != 0U) {
                    stream << ',';
                }
                stream << result.portfolioReplayIssueCodes[index];
            }
            return stream.str();
        }());
    lastPortfolioReplaySummary_ = QStringLiteral(
        "positions=%1 cashSummaries=%2 realizedPnl=%3 unrealizedPnl=%4 totalFee=%5 marketValue=%6")
        .arg(result.portfolioReplayPositionCount)
        .arg(result.portfolioReplayCashSummaryCount)
        .arg(QString::fromStdString(result.portfolioReplayRealizedPnlText))
        .arg(QString::fromStdString(result.portfolioReplayUnrealizedPnlText))
        .arg(QString::fromStdString(result.portfolioReplayTotalFeeText))
        .arg(QString::fromStdString(result.portfolioReplayTotalMarketValueText));
    emit portfolioReplayStateChanged();
    syncFromController();
    return result.protocolSuccess && result.portfolioReplayAccepted;
}

void ShellAccountingPresenter::resetStrategyRecommendationState()
{
    strategyRecommendationBusy_ = false;
    lastStrategyRecommendationStatus_ = QStringLiteral("READY");
    lastStrategyRecommendationAction_.clear();
    lastStrategyRecommendationSource_.clear();
    lastStrategyRecommendationReason_.clear();
    lastStrategyRecommendationTier_.clear();
    lastStrategyRecommendationQuantityText_.clear();
    lastStrategyRecommendationAmountText_.clear();
    lastStrategyRecommendationNetCashImpactText_.clear();
    lastStrategyRecommendationIssueCodes_.clear();
    lastStrategyRecommendationSummary_.clear();
    lastStrategyRecommendationPayloadJson_.clear();
    emit strategyRecommendationStateChanged();
}

bool ShellAccountingPresenter::previewStrategyRecommendationReadOnlySummary(
    const QString& recommendationPayloadJson)
{
    if (!controller_) {
        markStrategyRecommendationInputError(
            QStringLiteral("Shell accounting controller is not configured."));
        markControllerNotConfigured("strategy.recommendation.readonly_summary");
        return false;
    }

    bool valid = false;
    auto request = makeStrategyRecommendationReadOnlySummaryRequest(recommendationPayloadJson, valid);
    if (!valid) {
        return false;
    }
    lastStrategyRecommendationPayloadJson_ =
        QString::fromStdString(request.strategyRecommendationPayloadJson);

    strategyRecommendationBusy_ = true;
    lastStrategyRecommendationStatus_ = QStringLiteral("RECOMMENDING");
    lastStrategyRecommendationIssueCodes_.clear();
    emit strategyRecommendationStateChanged();
    const auto result = controller_->fetchStrategyRecommendationReadOnlySummary(request);
    strategyRecommendationBusy_ = false;

    lastStrategyRecommendationStatus_ = QString::fromStdString(result.payloadStatus);
    if (lastStrategyRecommendationStatus_.isEmpty()) {
        lastStrategyRecommendationStatus_ = result.strategyRecommendationAccepted
            ? QStringLiteral("OK")
            : QStringLiteral("REJECTED");
    }
    lastStrategyRecommendationAction_ =
        QString::fromStdString(result.strategyRecommendationActionLabel);
    lastStrategyRecommendationSource_ =
        QString::fromStdString(result.strategyRecommendationSourceLabel);
    lastStrategyRecommendationReason_ =
        QString::fromStdString(result.strategyRecommendationReasonLabel);
    lastStrategyRecommendationTier_ =
        QString::fromStdString(result.strategyRecommendationTierLabel);
    lastStrategyRecommendationQuantityText_ =
        QString::fromStdString(result.strategyRecommendationSuggestedQuantityText);
    lastStrategyRecommendationAmountText_ =
        QString::fromStdString(result.strategyRecommendationSuggestedAmountText);
    lastStrategyRecommendationNetCashImpactText_ =
        QString::fromStdString(result.strategyRecommendationNetCashImpactText);
    lastStrategyRecommendationIssueCodes_ =
        QString::fromStdString([&]() {
            std::ostringstream stream;
            for (std::size_t index = 0; index < result.strategyRecommendationIssueCodes.size(); ++index) {
                if (index != 0U) {
                    stream << ',';
                }
                stream << result.strategyRecommendationIssueCodes[index];
            }
            return stream.str();
        }());
    lastStrategyRecommendationSummary_ = QStringLiteral(
        "action=%1 source=%2 reason=%3 tier=%4 quantity=%5 amount=%6 netCash=%7")
        .arg(lastStrategyRecommendationAction_)
        .arg(lastStrategyRecommendationSource_)
        .arg(lastStrategyRecommendationReason_)
        .arg(lastStrategyRecommendationTier_)
        .arg(lastStrategyRecommendationQuantityText_)
        .arg(lastStrategyRecommendationAmountText_)
        .arg(lastStrategyRecommendationNetCashImpactText_);
    emit strategyRecommendationStateChanged();
    syncFromController();
    return result.protocolSuccess && result.strategyRecommendationAccepted;
}

void ShellAccountingPresenter::resetTradeDraftState()
{
    lastTradeDraftStatus_ = QStringLiteral("READY");
    lastTradeDraftId_.clear();
    lastTradeDraftSide_.clear();
    lastTradeDraftInstrumentCode_.clear();
    lastTradeDraftQuantityText_.clear();
    lastTradeDraftAmountText_.clear();
    lastTradeDraftNetCashImpactText_.clear();
    lastTradeDraftIssueCodes_.clear();
    lastTradeDraftSummary_.clear();
    lastTradeDraftDuplicate_ = false;
    lastTradeDraftIdempotencyConflict_ = false;
    lastTradeDraftUserConfirmationRequired_ = false;
    emit tradeDraftStateChanged();
}

bool ShellAccountingPresenter::previewTradeDraftFromLastRecommendation()
{
    const auto action = canonicalAsciiToken(lastStrategyRecommendationAction_);
    if (lastStrategyRecommendationPayloadJson_.trimmed().isEmpty()
        || lastStrategyRecommendationStatus_ != QStringLiteral("OK")
        || (action != QStringLiteral("BUY") && action != QStringLiteral("SELL"))) {
        markTradeDraftInputError(
            QStringLiteral("Only the latest accepted BUY or SELL recommendation can become a TradeDraft."));
        return false;
    }

    lastTradeDraftStatus_ = QStringLiteral("READY_FOR_CONFIRMATION");
    lastTradeDraftId_.clear();
    lastTradeDraftSide_ = action;
    lastTradeDraftInstrumentCode_.clear();
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(
        lastStrategyRecommendationPayloadJson_.toUtf8(),
        &parseError);
    if (parseError.error == QJsonParseError::NoError && document.isObject()) {
        const auto input = document.object().value(QStringLiteral("input")).toObject();
        lastTradeDraftInstrumentCode_ = input.value(QStringLiteral("instrumentCode")).toString();
    }
    lastTradeDraftQuantityText_ = lastStrategyRecommendationQuantityText_;
    lastTradeDraftAmountText_ = lastStrategyRecommendationAmountText_;
    lastTradeDraftNetCashImpactText_ = lastStrategyRecommendationNetCashImpactText_;
    lastTradeDraftIssueCodes_.clear();
    lastTradeDraftDuplicate_ = false;
    lastTradeDraftIdempotencyConflict_ = false;
    lastTradeDraftUserConfirmationRequired_ = true;
    lastTradeDraftSummary_ = QStringLiteral(
        "action=%1 instrument=%2 quantity=%3 amount=%4 netCash=%5 confirmationRequired=true draftIsNotOrder=true")
        .arg(
            lastTradeDraftSide_,
            lastTradeDraftInstrumentCode_,
            lastTradeDraftQuantityText_,
            lastTradeDraftAmountText_,
            lastTradeDraftNetCashImpactText_);
    emit tradeDraftStateChanged();
    return true;
}

bool ShellAccountingPresenter::createTradeDraftFromLastRecommendation(bool userConfirmed)
{
    if (!controller_) {
        markTradeDraftInputError(
            QStringLiteral("Shell accounting controller is not configured."));
        markControllerNotConfigured("accounting.tradedraft.create_from_recommendation");
        return false;
    }

    bool valid = false;
    auto request = makeTradeDraftCreateFromRecommendationRequest(userConfirmed, valid);
    if (!valid) {
        return false;
    }

    const auto result = controller_->createDraftFromRecommendation(request);
    applyTradeDraftResult(result);
    const bool createdOrDuplicate = lastTradeDraftStatus_ == QStringLiteral("DRAFT_CREATED")
        || lastTradeDraftStatus_ == QStringLiteral("DUPLICATE");
    if (result.protocolSuccess && result.implemented && result.tradeDraftId > 0) {
        bool summaryValid = false;
        auto summaryRequest = makeTradeDraftReadOnlySummaryRequest(summaryValid);
        if (summaryValid) {
            const auto summary = controller_->fetchTradeDraftReadOnlySummary(summaryRequest);
            if (summary.protocolSuccess && summary.tradeDraftSummaryFound) {
                applyTradeDraftResult(summary);
            }
        }
    }
    syncFromController();
    return createdOrDuplicate;
}

bool ShellAccountingPresenter::previewOtcMapMultiChannelDraft(const QString& payloadJson)
{
    if (!controller_) {
        markTradeDraftInputError(
            QStringLiteral("Shell accounting controller is not configured."));
        markControllerNotConfigured("accounting.otcmap_multichannel.readonly_preview");
        return false;
    }

    bool valid = false;
    auto request = makeOtcMapRequest(payloadJson, false, valid);
    if (!valid) {
        return false;
    }

    lastOtcMapPreviewStatus_ = QStringLiteral("PREVIEWING");
    lastOtcMapPreviewSummary_.clear();
    lastOtcMapPreviewIssueCodes_.clear();
    lastOtcMapDraftStatus_ = QStringLiteral("READY");
    emit tradeDraftStateChanged();
    const auto result = controller_->previewOtcMapMultiChannelDraft(request);
    applyOtcMapPreviewResult(result);
    if (lastOtcMapPreviewStatus_ == QStringLiteral("ACCEPTED")) {
        lastOtcMapPayloadJson_ = QString::fromStdString(request.otcMapPayloadJson);
    } else {
        lastOtcMapPayloadJson_.clear();
    }
    syncFromController();
    return lastOtcMapPreviewStatus_ == QStringLiteral("ACCEPTED");
}

bool ShellAccountingPresenter::createOtcMapMultiChannelTradeDraft(bool userConfirmed)
{
    if (!controller_) {
        markTradeDraftInputError(
            QStringLiteral("Shell accounting controller is not configured."));
        markControllerNotConfigured("accounting.tradedraft.create_otcmap_multichannel");
        return false;
    }
    if (!userConfirmed || lastOtcMapPreviewStatus_ != QStringLiteral("ACCEPTED")
        || lastOtcMapPreviewLegCount_ <= 0 || lastOtcMapPayloadJson_.trimmed().isEmpty()) {
        markTradeDraftInputError(
            QStringLiteral("Accepted OTCMap preview and explicit confirmation are required."));
        return false;
    }

    bool valid = false;
    auto request = makeOtcMapRequest(lastOtcMapPayloadJson_, true, valid);
    if (!valid) {
        return false;
    }

    lastOtcMapDraftStatus_ = QStringLiteral("CREATING");
    emit tradeDraftStateChanged();
    const auto result = controller_->createOtcMapMultiChannelTradeDraft(request);
    applyOtcMapDraftResult(result);
    syncFromController();
    return lastOtcMapDraftStatus_ == QStringLiteral("DRAFT_CREATED")
        || lastOtcMapDraftStatus_ == QStringLiteral("DUPLICATE");
}

void ShellAccountingPresenter::resetOtcMapDraftState()
{
    lastOtcMapPreviewStatus_ = QStringLiteral("READY");
    lastOtcMapPreviewLegCount_ = 0;
    lastOtcMapPreviewTotalAmountText_.clear();
    lastOtcMapPreviewSummary_.clear();
    lastOtcMapPreviewIssueCodes_.clear();
    lastOtcMapDraftStatus_ = QStringLiteral("READY");
    lastOtcMapDraftId_.clear();
    lastOtcMapDraftLegCount_ = 0;
    lastOtcMapDraftDuplicate_ = false;
    lastOtcMapDraftIdempotencyConflict_ = false;
    lastOtcMapDraftSummary_.clear();
    lastOtcMapDraftIssueCodes_.clear();
    lastOtcMapPayloadJson_.clear();
    emit tradeDraftStateChanged();
}

bool ShellAccountingPresenter::persistExcelVbaImportManualEntry(
    const QString& previewStatus,
    const QString& previewDigest,
    const QString& idempotencyKey,
    const QString& schemaVersion,
    const QString& source,
    const QString& acceptedAt,
    const QString& importBatchLabel,
    const QString& importPayloadJson,
    int tradeFactCount,
    int cashFactCount,
    int marketPriceFactCount,
    int fxRateFactCount)
{
    if (!controller_) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Shell accounting controller is not configured."));
        markControllerNotConfigured("accounting.excel_vba_import.persist_manual_entry");
        return false;
    }

    bool valid = false;
    auto request = makeExcelVbaImportPersistManualEntryRequest(
        previewStatus,
        previewDigest,
        idempotencyKey,
        schemaVersion,
        source,
        acceptedAt,
        importBatchLabel,
        importPayloadJson,
        tradeFactCount,
        cashFactCount,
        marketPriceFactCount,
        fxRateFactCount,
        valid);
    if (!valid) {
        return false;
    }

    excelVbaImportPersistBusy_ = true;
    lastExcelVbaImportPersistStatus_ = QStringLiteral("PERSISTING");
    lastExcelVbaImportPersistIssue_.clear();
    emit excelVbaImportPersistStateChanged();
    const auto result = controller_->persistExcelVbaImportManualEntry(request);
    excelVbaImportPersistBusy_ = false;
    applyExcelVbaImportPersistManualEntryResult(result, tradeFactCount, cashFactCount);
    const bool persisted = lastExcelVbaImportPersistStatus_ == QStringLiteral("PERSISTED");
    if (persisted) {
        refreshAfterManualEntryWrite(
            tradeFactCount > 0,
            "excel_vba_import_persist_manual_entry");
    } else if (lastExcelVbaImportPersistStatus_ == QStringLiteral("DUPLICATE")) {
        postWriteRefreshBusy_ = false;
        lastPostWriteRefreshStatus_ = QStringLiteral("SKIPPED");
        lastPostWriteRefreshIssue_.clear();
        lastPostWriteRefreshSummary_ = QStringLiteral(
            "reason=excel_vba_import_duplicate actions=none newRows=false");
        emit postWriteRefreshStateChanged();
    } else if (lastExcelVbaImportPersistStatus_ == QStringLiteral("IDEMPOTENCY_CONFLICT")) {
        postWriteRefreshBusy_ = false;
        lastPostWriteRefreshStatus_ = QStringLiteral("SKIPPED");
        lastPostWriteRefreshIssue_ =
            QStringLiteral("Post-persist refresh skipped because persistence failed closed.");
        lastPostWriteRefreshSummary_ = QStringLiteral(
            "reason=excel_vba_import_idempotency_conflict actions=none");
        emit postWriteRefreshStateChanged();
    }
    syncFromController();
    return lastExcelVbaImportPersistStatus_ == QStringLiteral("PERSISTED")
        || lastExcelVbaImportPersistStatus_ == QStringLiteral("DUPLICATE");
}

bool ShellAccountingPresenter::persistAcceptedExcelVbaImportPreview()
{
    if (lastExcelVbaImportPreviewStatus_ != QStringLiteral("ACCEPTED")) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Only an ACCEPTED Excel/VBA import preview can be persisted."));
        return false;
    }
    if (lastExcelVbaImportPreviewDigest_.trimmed().isEmpty()
        || lastExcelVbaImportPreviewPayloadJson_.trimmed().isEmpty()) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Accepted preview digest and sanitized payload are required."));
        return false;
    }
    if (lastExcelVbaImportPreviewSchemaVersion_.trimmed().isEmpty()
        || lastExcelVbaImportPreviewSource_.trimmed().isEmpty()) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Accepted preview schema version and source are required."));
        return false;
    }
    if ((excelVbaImportPreviewTradeFactCount_ + excelVbaImportPreviewCashFactCount_) <= 0) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Accepted preview fact summary must contain trade or cash facts."));
        return false;
    }
    if (excelVbaImportPreviewMarketPriceFactCount_ > 0 || excelVbaImportPreviewFxRateFactCount_ > 0) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Market price and FX rate facts are not supported for persistence yet."));
        return false;
    }

    return persistExcelVbaImportManualEntry(
        lastExcelVbaImportPreviewStatus_,
        lastExcelVbaImportPreviewDigest_,
        acceptedExcelVbaImportPreviewIdempotencyKey(),
        lastExcelVbaImportPreviewSchemaVersion_,
        lastExcelVbaImportPreviewSource_,
        QDateTime::currentDateTimeUtc().toString(Qt::ISODate),
        QStringLiteral("shell-qml-accepted-preview"),
        lastExcelVbaImportPreviewPayloadJson_,
        excelVbaImportPreviewTradeFactCount_,
        excelVbaImportPreviewCashFactCount_,
        excelVbaImportPreviewMarketPriceFactCount_,
        excelVbaImportPreviewFxRateFactCount_);
}

void ShellAccountingPresenter::reset()
{
    controller_.reset();
    privacyMode_ = false;
    status_.reset();
    issues_.clearIssues();
    positions_.clearRows();
    positions_.setPrivacyMode(false);
    resetTradingUi();
    resetManualEntryUi();
    resetPostWriteRefreshState();
    resetExcelVbaImportPreviewState();
    resetExcelVbaImportPersistState();
    resetPortfolioReplayState();
    resetStrategyRecommendationState();
    resetTradeDraftState();
    resetOtcMapDraftState();
}

void ShellAccountingPresenter::markControllerNotConfigured(const char* actionName)
{
    std::vector<ShellAccountingIssue> issues{
        makePresenterIssue(
            "CONTROLLER_NOT_CONFIGURED",
            "Shell accounting controller is not configured."),
    };
    applyControllerState(actionName, ShellAccountingViewState::Unavailable, std::move(issues));
    positions_.clearRows();
}

void ShellAccountingPresenter::refreshAccountingAction(
    const char* actionName,
    void (ShellAccountingReadOnlyController::*refresh)(
        const ShellAccountingServiceRequest&))
{
    if (!controller_) {
        markControllerNotConfigured(actionName);
        return;
    }

    auto request = makePresenterRefreshRequest(actionName);
    (controller_.get()->*refresh)(request);
    syncFromController();
}

void ShellAccountingPresenter::applyControllerState(
    const std::string& actionName,
    ShellAccountingViewState state,
    std::vector<ShellAccountingIssue> issues)
{
    auto issueRows = issues;
    status_.applyStateSnapshot(
        snapshotFromControllerState(actionName, state, std::move(issues)));
    issues_.setIssues(std::move(issueRows));
}

void ShellAccountingPresenter::applyTradingResult(
    const ShellAccountingServiceResult& result,
    bool confirming)
{
    const auto payload = payloadObject(result);
    const auto status = stringField(payload, "status");
    const bool ok = result.protocolSuccess && result.implemented
        && (status == QStringLiteral("OK") || status == QStringLiteral("DUPLICATE"));

    if (!ok) {
        tradingUiStatus_ = QStringLiteral("ERROR");
        tradingUiIssue_ = issueTextFromResult(result);
        if (tradingUiIssue_.isEmpty()) {
            tradingUiIssue_ = confirming
                ? QStringLiteral("Draft confirmation failed.")
                : QStringLiteral("Draft creation failed.");
        }
        emit tradingUiStateChanged();
        return;
    }

    tradingUiIssue_.clear();
    if (confirming) {
        ledgerStatus_ = QStringLiteral("CONFIRMED_LEDGER");
        tradingUiStatus_ = status == QStringLiteral("DUPLICATE")
            ? QStringLiteral("CONFIRM_DUPLICATE")
            : QStringLiteral("CONFIRMED_LEDGER");
    } else {
        draftId_ = stringField(payload, "draftId");
        draftUid_ = stringField(payload, "draftUid");
        ledgerStatus_ = QStringLiteral("NOT_CONFIRMED");
        tradingUiStatus_ = status == QStringLiteral("DUPLICATE")
            ? QStringLiteral("DRAFT_DUPLICATE")
            : QStringLiteral("DRAFT_ONLY");
    }
    emit tradingUiStateChanged();
}

void ShellAccountingPresenter::applyManualEntryResult(
    const ShellAccountingServiceResult& result,
    const QString& fallback)
{
    const auto payload = payloadObject(result);
    const auto status = stringField(payload, "status");
    const bool idempotent = boolField(payload, "idempotent") || boolField(payload, "duplicate");
    const bool ok = result.protocolSuccess && result.implemented
        && (status == QStringLiteral("OK") || status == QStringLiteral("DUPLICATE"));

    if (!ok) {
        lastManualEntryStatus_ = status.isEmpty() ? QStringLiteral("ERROR") : status;
        lastManualEntryIssue_ = issueTextFromResult(result);
        if (lastManualEntryIssue_.isEmpty()) {
            lastManualEntryIssue_ = fallback;
        }
        lastManualEntryResult_.clear();
        emit manualEntryStateChanged();
        return;
    }

    lastManualEntryStatus_ = idempotent ? QStringLiteral("DUPLICATE") : QStringLiteral("OK");
    lastManualEntryIssue_.clear();
    lastManualEntryResult_ = QStringLiteral(
        "databaseWritten=%1 tradeLogWritten=%2 cashAdjustmentWritten=%3 idempotent=%4")
        .arg(boolField(payload, "databaseWritten") ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(boolField(payload, "tradeLogWritten") ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(boolField(payload, "cashAdjustmentWritten") ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(idempotent ? QStringLiteral("true") : QStringLiteral("false"));
    emit manualEntryStateChanged();
}

void ShellAccountingPresenter::applyExcelVbaImportPreviewResult(
    const ShellAccountingServiceResult& result)
{
    excelVbaImportPreviewTradeFactCount_ = result.importPreviewFactSummary.tradeFactCount;
    excelVbaImportPreviewCashFactCount_ = result.importPreviewFactSummary.cashFactCount;
    excelVbaImportPreviewMarketPriceFactCount_ =
        result.importPreviewFactSummary.marketPriceFactCount;
    excelVbaImportPreviewFxRateFactCount_ = result.importPreviewFactSummary.fxRateFactCount;

    QStringList codes;
    for (const auto& code : result.importPreviewDiagnosticCodes) {
        codes.append(QString::fromStdString(code));
    }
    lastExcelVbaImportPreviewDiagnosticCodes_ = codes.join(QStringLiteral(","));
    lastExcelVbaImportPreviewSummary_ = QStringLiteral(
        "tradeFacts=%1 cashFacts=%2 marketPriceFacts=%3 fxRateFacts=%4 diagnostics=%5")
        .arg(excelVbaImportPreviewTradeFactCount_)
        .arg(excelVbaImportPreviewCashFactCount_)
        .arg(excelVbaImportPreviewMarketPriceFactCount_)
        .arg(excelVbaImportPreviewFxRateFactCount_)
        .arg(codes.size());

    const bool ok = result.protocolSuccess && result.implemented
        && result.readOnly && !result.writeEnabled
        && result.importPreviewAccepted && !result.importPreviewRejected
        && !result.accountingEngineCalled && !result.productionFileLoading
        && !result.productionWrite && !result.sqliteProductionWrite
        && !result.auditWritten && !result.ledgerWritten && !result.snapshotWritten
        && !result.tradeLogWritten && !result.readModelPersistentWrite
        && isShellAccountingServiceResultReadOnly(result)
        && !result.networkAccess && !result.credentialAccess
        && !result.endpointAccess && !result.automaticTrading
        && !result.rawUserDataExposed;

    if (ok) {
        lastExcelVbaImportPreviewStatus_ = QStringLiteral("ACCEPTED");
        lastExcelVbaImportPreviewIssue_.clear();
        emit excelVbaImportPreviewStateChanged();
        return;
    }

    lastExcelVbaImportPreviewStatus_ =
        result.importPreviewRejected ? QStringLiteral("REJECTED") : QStringLiteral("ERROR");
    lastExcelVbaImportPreviewIssue_ = issueTextFromResult(result);
    if (lastExcelVbaImportPreviewIssue_.isEmpty()) {
        lastExcelVbaImportPreviewIssue_ =
            QStringLiteral("Excel/VBA import preview failed closed.");
    }
    emit excelVbaImportPreviewStateChanged();
}

void ShellAccountingPresenter::applyExcelVbaImportPersistManualEntryResult(
    const ShellAccountingServiceResult& result,
    int requestedTradeFactCount,
    int requestedCashFactCount)
{
    lastExcelVbaImportPersistTransactionCommitted_ = result.transactionCommitted;
    lastExcelVbaImportPersistTradeLogWritten_ = result.tradeLogWritten;
    lastExcelVbaImportPersistAuditLogWritten_ = result.auditWritten;
    lastExcelVbaImportPersistDuplicateImportPrevented_ = result.duplicateImportPrevented;
    lastExcelVbaImportPersistIdempotencyConflictRejected_ = result.idempotencyConflictRejected;
    lastExcelVbaImportPersistTradeLogRowsWritten_ = 0;
    lastExcelVbaImportPersistCashAdjustmentRowsWritten_ = 0;

    QStringList codes;
    for (const auto& issue : result.issues) {
        if (!issue.code.empty()) {
            codes.append(QString::fromStdString(issue.code));
        }
    }
    for (const auto& issue : result.errors) {
        if (!issue.code.empty()) {
            codes.append(QString::fromStdString(issue.code));
        }
    }
    lastExcelVbaImportPersistIssueCodes_ = codes.join(QStringLiteral(","));
    lastExcelVbaImportPersistSummary_ = QStringLiteral(
        "transactionCommitted=%1 tradeLogWritten=%2 auditLogWritten=%3 duplicateImportPrevented=%4 idempotencyConflictRejected=%5")
        .arg(result.transactionCommitted ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(result.tradeLogWritten ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(result.auditWritten ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(result.duplicateImportPrevented ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(result.idempotencyConflictRejected ? QStringLiteral("true") : QStringLiteral("false"));

    const bool persisted = result.protocolSuccess && result.implemented
        && !result.readOnly && result.writeEnabled
        && result.transactionCommitted && result.tradeLogWritten
        && result.auditWritten && result.idempotencyRequired
        && !result.duplicateImportPrevented && !result.idempotencyConflictRejected
        && !result.accountingEngineCalled
        && !result.networkAccess && !result.credentialAccess
        && !result.endpointAccess && !result.automaticTrading;
    const bool duplicate = result.protocolSuccess && result.implemented
        && result.duplicateImportPrevented && !result.idempotencyConflictRejected
        && !result.accountingEngineCalled
        && !result.networkAccess && !result.credentialAccess
        && !result.endpointAccess && !result.automaticTrading;

    if (persisted || duplicate) {
        lastExcelVbaImportPersistStatus_ =
            duplicate ? QStringLiteral("DUPLICATE") : QStringLiteral("PERSISTED");
        lastExcelVbaImportPersistIssue_.clear();
        if (persisted) {
            lastExcelVbaImportPersistTradeLogRowsWritten_ =
                requestedTradeFactCount + requestedCashFactCount;
            lastExcelVbaImportPersistCashAdjustmentRowsWritten_ = requestedCashFactCount;
        }
        lastExcelVbaImportPersistSummary_ += QStringLiteral(
            " tradeLogRowsWritten=%1 cashAdjustmentRowsWritten=%2")
            .arg(lastExcelVbaImportPersistTradeLogRowsWritten_)
            .arg(lastExcelVbaImportPersistCashAdjustmentRowsWritten_);
        emit excelVbaImportPersistStateChanged();
        return;
    }

    lastExcelVbaImportPersistStatus_ = result.idempotencyConflictRejected
        ? QStringLiteral("IDEMPOTENCY_CONFLICT")
        : (result.payloadStatus.empty()
            ? QStringLiteral("ERROR")
            : QString::fromStdString(result.payloadStatus));
    lastExcelVbaImportPersistIssue_ = issueTextFromResult(result);
    if (lastExcelVbaImportPersistIssue_.isEmpty()) {
        lastExcelVbaImportPersistIssue_ =
            QStringLiteral("Excel/VBA import manual entry persistence failed closed.");
    }
    emit excelVbaImportPersistStateChanged();
}

void ShellAccountingPresenter::refreshAfterManualEntryWrite(
    bool includePositionList,
    const char* reason)
{
    if (!postWriteRefreshEnabled_) {
        postWriteRefreshBusy_ = false;
        lastPostWriteRefreshStatus_ = QStringLiteral("DISABLED");
        lastPostWriteRefreshIssue_ = QStringLiteral("Post-write readback refresh is disabled.");
        lastPostWriteRefreshSummary_ = QStringLiteral("reason=%1 actions=none")
            .arg(QString::fromUtf8(reason));
        emit postWriteRefreshStateChanged();
        return;
    }
    if (!controller_) {
        postWriteRefreshBusy_ = false;
        lastPostWriteRefreshStatus_ = QStringLiteral("UNAVAILABLE");
        lastPostWriteRefreshIssue_ = QStringLiteral("Shell accounting controller is not configured.");
        lastPostWriteRefreshSummary_.clear();
        emit postWriteRefreshStateChanged();
        return;
    }

    struct RefreshStep final {
        const char* actionName;
        void (ShellAccountingReadOnlyController::*refresh)(
            const ShellAccountingServiceRequest&);
    };

    const RefreshStep transactionSteps[] = {
        {"position.list", &ShellAccountingReadOnlyController::refreshPositionList},
        {"cash.summary", &ShellAccountingReadOnlyController::refreshCashSummary},
        {"portfolio.pnl.summary", &ShellAccountingReadOnlyController::refreshPortfolioPnlSummary},
    };
    const RefreshStep cashMovementSteps[] = {
        {"cash.summary", &ShellAccountingReadOnlyController::refreshCashSummary},
        {"portfolio.pnl.summary", &ShellAccountingReadOnlyController::refreshPortfolioPnlSummary},
    };

    postWriteRefreshBusy_ = true;
    lastPostWriteRefreshStatus_ = QStringLiteral("REFRESHING");
    lastPostWriteRefreshIssue_.clear();
    lastPostWriteRefreshSummary_ = QStringLiteral("reason=%1 actions=pending")
        .arg(QString::fromUtf8(reason));
    emit postWriteRefreshStateChanged();

    ShellAccountingViewState aggregateState = ShellAccountingViewState::Idle;
    std::vector<ShellAccountingIssue> aggregateIssues;
    QStringList actionNames;

    const auto runStep = [&](const RefreshStep& step) {
        auto request = makePresenterRefreshRequest(step.actionName);
        (controller_.get()->*step.refresh)(request);
        aggregateState = moreSevereRefreshState(aggregateState, controller_->currentState());
        appendIssues(aggregateIssues, controller_->issues());
        actionNames.append(QString::fromUtf8(step.actionName));
    };

    if (includePositionList) {
        for (const auto& step : transactionSteps) {
            runStep(step);
        }
    } else {
        for (const auto& step : cashMovementSteps) {
            runStep(step);
        }
    }

    postWriteRefreshBusy_ = false;
    lastPostWriteRefreshIssue_ = issueTextFromIssues(aggregateIssues);
    switch (aggregateState) {
    case ShellAccountingViewState::Error:
    case ShellAccountingViewState::Unavailable:
        lastPostWriteRefreshStatus_ = QStringLiteral("ERROR");
        if (lastPostWriteRefreshIssue_.isEmpty()) {
            lastPostWriteRefreshIssue_ = QStringLiteral("Post-write readback refresh failed.");
        }
        break;
    case ShellAccountingViewState::Stale:
        lastPostWriteRefreshStatus_ = QStringLiteral("STALE");
        if (lastPostWriteRefreshIssue_.isEmpty()) {
            lastPostWriteRefreshIssue_ = QStringLiteral("Post-write readback refresh returned stale data.");
        }
        break;
    case ShellAccountingViewState::Warning:
        lastPostWriteRefreshStatus_ = QStringLiteral("WARNING");
        break;
    case ShellAccountingViewState::Empty:
        lastPostWriteRefreshStatus_ = QStringLiteral("EMPTY");
        break;
    case ShellAccountingViewState::Loaded:
    case ShellAccountingViewState::Idle:
    case ShellAccountingViewState::Loading:
        lastPostWriteRefreshStatus_ = QStringLiteral("OK");
        break;
    }
    lastPostWriteRefreshSummary_ = QStringLiteral("reason=%1 actions=%2 issueCount=%3")
        .arg(QString::fromUtf8(reason), actionNames.join(QStringLiteral(",")))
        .arg(static_cast<int>(aggregateIssues.size()));
    emit postWriteRefreshStateChanged();

    applyControllerState(
        "accounting.manual_entry.post_write_readback_refresh",
        aggregateState,
        std::move(aggregateIssues));
}

void ShellAccountingPresenter::markManualEntryInputError(const QString& message)
{
    manualEntryBusy_ = false;
    lastManualEntryStatus_ = QStringLiteral("INPUT_ERROR");
    lastManualEntryIssue_ = message;
    lastManualEntryResult_.clear();
    emit manualEntryStateChanged();
}

void ShellAccountingPresenter::markExcelVbaImportPreviewInputError(const QString& message)
{
    excelVbaImportPreviewBusy_ = false;
    lastExcelVbaImportPreviewStatus_ = QStringLiteral("INPUT_ERROR");
    lastExcelVbaImportPreviewIssue_ = message;
    lastExcelVbaImportPreviewSummary_.clear();
    lastExcelVbaImportPreviewDiagnosticCodes_.clear();
    excelVbaImportPreviewTradeFactCount_ = 0;
    excelVbaImportPreviewCashFactCount_ = 0;
    excelVbaImportPreviewMarketPriceFactCount_ = 0;
    excelVbaImportPreviewFxRateFactCount_ = 0;
    lastExcelVbaImportPreviewDigest_.clear();
    lastExcelVbaImportPreviewPayloadJson_.clear();
    lastExcelVbaImportPreviewSchemaVersion_.clear();
    lastExcelVbaImportPreviewSource_.clear();
    emit excelVbaImportPreviewStateChanged();
    resetExcelVbaImportPersistState();
}

void ShellAccountingPresenter::markExcelVbaImportPersistInputError(const QString& message)
{
    excelVbaImportPersistBusy_ = false;
    lastExcelVbaImportPersistStatus_ = QStringLiteral("INPUT_ERROR");
    lastExcelVbaImportPersistIssue_ = message;
    lastExcelVbaImportPersistSummary_.clear();
    lastExcelVbaImportPersistIssueCodes_.clear();
    lastExcelVbaImportPersistTransactionCommitted_ = false;
    lastExcelVbaImportPersistTradeLogWritten_ = false;
    lastExcelVbaImportPersistAuditLogWritten_ = false;
    lastExcelVbaImportPersistDuplicateImportPrevented_ = false;
    lastExcelVbaImportPersistIdempotencyConflictRejected_ = false;
    lastExcelVbaImportPersistTradeLogRowsWritten_ = 0;
    lastExcelVbaImportPersistCashAdjustmentRowsWritten_ = 0;
    emit excelVbaImportPersistStateChanged();
}

void ShellAccountingPresenter::markPortfolioReplayInputError(const QString& message)
{
    portfolioReplayBusy_ = false;
    lastPortfolioReplayStatus_ = QStringLiteral("INPUT_ERROR");
    lastPortfolioReplayIssue_ = message;
    lastPortfolioReplaySummary_.clear();
    lastPortfolioReplayIssueCodes_.clear();
    portfolioReplayPositionCount_ = 0;
    portfolioReplayCashSummaryCount_ = 0;
    portfolioReplayRealizedPnl_.clear();
    portfolioReplayUnrealizedPnl_.clear();
    emit portfolioReplayStateChanged();
}

void ShellAccountingPresenter::markStrategyRecommendationInputError(const QString& message)
{
    strategyRecommendationBusy_ = false;
    lastStrategyRecommendationStatus_ = QStringLiteral("INPUT_ERROR");
    lastStrategyRecommendationAction_.clear();
    lastStrategyRecommendationSource_.clear();
    lastStrategyRecommendationReason_.clear();
    lastStrategyRecommendationTier_.clear();
    lastStrategyRecommendationQuantityText_.clear();
    lastStrategyRecommendationAmountText_.clear();
    lastStrategyRecommendationNetCashImpactText_.clear();
    lastStrategyRecommendationIssueCodes_.clear();
    lastStrategyRecommendationSummary_ = message;
    emit strategyRecommendationStateChanged();
}

void ShellAccountingPresenter::markTradeDraftInputError(const QString& message)
{
    lastTradeDraftStatus_ = QStringLiteral("INPUT_ERROR");
    lastTradeDraftId_.clear();
    lastTradeDraftSide_.clear();
    lastTradeDraftInstrumentCode_.clear();
    lastTradeDraftQuantityText_.clear();
    lastTradeDraftAmountText_.clear();
    lastTradeDraftNetCashImpactText_.clear();
    lastTradeDraftIssueCodes_.clear();
    lastTradeDraftSummary_ = message;
    lastTradeDraftDuplicate_ = false;
    lastTradeDraftIdempotencyConflict_ = false;
    lastTradeDraftUserConfirmationRequired_ = true;
    emit tradeDraftStateChanged();
}

void ShellAccountingPresenter::applyTradeDraftResult(const ShellAccountingServiceResult& result)
{
    lastTradeDraftDuplicate_ = result.tradeDraftDuplicate;
    lastTradeDraftIdempotencyConflict_ = result.tradeDraftIdempotencyConflict;
    lastTradeDraftUserConfirmationRequired_ = result.tradeDraftUserConfirmationRequired;
    lastTradeDraftId_ = result.tradeDraftId > 0
        ? QString::number(result.tradeDraftId)
        : QString {};
    lastTradeDraftSide_ = QString::fromStdString(result.tradeDraftSide);
    lastTradeDraftInstrumentCode_ = QString::fromStdString(result.tradeDraftInstrumentCode);
    lastTradeDraftQuantityText_ = QString::fromStdString(result.tradeDraftQuantityText);
    lastTradeDraftAmountText_ = QString::fromStdString(result.tradeDraftAmountText);
    lastTradeDraftNetCashImpactText_ = QString::fromStdString(result.tradeDraftNetCashImpactText);

    QStringList codes;
    for (const auto& code : result.tradeDraftIssueCodes) {
        codes.append(QString::fromStdString(code));
    }
    for (const auto& issue : result.issues) {
        if (!issue.code.empty()) {
            codes.append(QString::fromStdString(issue.code));
        }
    }
    for (const auto& issue : result.errors) {
        if (!issue.code.empty()) {
            codes.append(QString::fromStdString(issue.code));
        }
    }
    lastTradeDraftIssueCodes_ = codes.join(QStringLiteral(","));

    const bool safeBoundaries = result.tradeDraftIsNotOrder
        && !result.tradeLogWritten && !result.networkAccess
        && !result.credentialAccess && !result.endpointAccess
        && !result.automaticTrading;
    const bool created = result.protocolSuccess && result.implemented
        && !result.readOnly && result.writeEnabled
        && result.tradeDraftManualRecommendationFlowCreated
        && result.tradeDraftEligible && result.tradeDraftId > 0
        && result.transactionCommitted && result.auditWritten
        && safeBoundaries;
    const bool duplicate = result.protocolSuccess && result.implemented
        && result.tradeDraftDuplicate && !result.tradeDraftIdempotencyConflict
        && safeBoundaries;
    const bool summary = result.protocolSuccess && result.implemented
        && result.readOnly && !result.writeEnabled
        && result.tradeDraftSummaryFound && safeBoundaries;

    if (created || duplicate || summary) {
        lastTradeDraftStatus_ = result.tradeDraftStatus.empty()
            ? (duplicate ? QStringLiteral("DUPLICATE") : QStringLiteral("DRAFT_CREATED"))
            : QString::fromStdString(result.tradeDraftStatus);
        lastTradeDraftSummary_ = result.tradeDraftSummary.empty()
            ? QStringLiteral(
                "status=%1 side=%2 instrument=%3 quantity=%4 amount=%5 netCash=%6 draftIsNotOrder=true")
                .arg(
                    lastTradeDraftStatus_,
                    lastTradeDraftSide_,
                    lastTradeDraftInstrumentCode_,
                    lastTradeDraftQuantityText_,
                    lastTradeDraftAmountText_,
                    lastTradeDraftNetCashImpactText_)
            : QString::fromStdString(result.tradeDraftSummary);
        emit tradeDraftStateChanged();
        return;
    }

    lastTradeDraftStatus_ = result.tradeDraftIdempotencyConflict
        ? QStringLiteral("IDEMPOTENCY_CONFLICT")
        : (result.tradeDraftStatus.empty()
            ? QStringLiteral("REJECTED")
            : QString::fromStdString(result.tradeDraftStatus));
    lastTradeDraftSummary_ = issueTextFromResult(result);
    if (lastTradeDraftSummary_.isEmpty()) {
        lastTradeDraftSummary_ = QStringLiteral("TradeDraft recommendation flow failed closed.");
    }
    emit tradeDraftStateChanged();
}

void ShellAccountingPresenter::applyOtcMapPreviewResult(const ShellAccountingServiceResult& result)
{
    lastOtcMapPreviewLegCount_ = result.otcMapPreviewLegCount;
    lastOtcMapPreviewTotalAmountText_ =
        QString::fromStdString(result.otcMapPreviewTotalAmountText);
    QStringList codes;
    for (const auto& code : result.otcMapPreviewIssueCodes) {
        codes.append(QString::fromStdString(code));
    }
    for (const auto& issue : result.issues) {
        if (!issue.code.empty()) {
            codes.append(QString::fromStdString(issue.code));
        }
    }
    lastOtcMapPreviewIssueCodes_ = codes.join(QStringLiteral(","));
    lastOtcMapPreviewSummary_ = result.otcMapPreviewSummary.empty()
        ? QStringLiteral("status=%1 legs=%2 amount=%3")
            .arg(
                QString::fromStdString(result.payloadStatus),
                QString::number(result.otcMapPreviewLegCount),
                lastOtcMapPreviewTotalAmountText_)
        : QString::fromStdString(result.otcMapPreviewSummary);

    const bool ok = result.protocolSuccess && result.implemented
        && result.readOnly && !result.writeEnabled
        && result.otcMapPreviewAccepted && result.otcMapPreviewExecuted
        && result.otcMapPreviewLegCount > 0
        && !result.productionWrite && !result.sqliteProductionWrite
        && !result.tradeLogWritten && !result.networkAccess
        && !result.credentialAccess && !result.endpointAccess
        && !result.automaticTrading;
    if (ok) {
        lastOtcMapPreviewStatus_ = QStringLiteral("ACCEPTED");
        emit tradeDraftStateChanged();
        return;
    }

    lastOtcMapPreviewStatus_ = result.payloadStatus.empty()
        ? QStringLiteral("REJECTED")
        : QString::fromStdString(result.payloadStatus);
    if (lastOtcMapPreviewSummary_.isEmpty()) {
        lastOtcMapPreviewSummary_ = issueTextFromResult(result);
    }
    emit tradeDraftStateChanged();
}

void ShellAccountingPresenter::applyOtcMapDraftResult(const ShellAccountingServiceResult& result)
{
    lastOtcMapDraftDuplicate_ = result.otcMapDraftDuplicate;
    lastOtcMapDraftIdempotencyConflict_ = result.otcMapDraftIdempotencyConflict;
    lastOtcMapDraftId_ = result.otcMapDraftId > 0
        ? QString::number(result.otcMapDraftId)
        : QString {};
    lastOtcMapDraftLegCount_ = result.otcMapDraftLegCount;
    QStringList codes;
    for (const auto& code : result.otcMapDraftIssueCodes) {
        codes.append(QString::fromStdString(code));
    }
    for (const auto& issue : result.issues) {
        if (!issue.code.empty()) {
            codes.append(QString::fromStdString(issue.code));
        }
    }
    lastOtcMapDraftIssueCodes_ = codes.join(QStringLiteral(","));
    lastOtcMapDraftSummary_ = result.otcMapDraftSummary.empty()
        ? QStringLiteral("status=%1 draftId=%2 legs=%3")
            .arg(
                QString::fromStdString(result.payloadStatus),
                lastOtcMapDraftId_,
                QString::number(result.otcMapDraftLegCount))
        : QString::fromStdString(result.otcMapDraftSummary);

    const bool safeBoundaries =
        !result.tradeLogWritten && !result.networkAccess
        && !result.credentialAccess && !result.endpointAccess
        && !result.automaticTrading;
    const bool created = result.protocolSuccess && result.implemented
        && !result.readOnly && result.writeEnabled
        && result.transactionCommitted && result.auditWritten
        && result.otcMapDraftId > 0 && result.otcMapDraftLegCount > 0
        && safeBoundaries;
    const bool duplicate = result.protocolSuccess && result.implemented
        && result.otcMapDraftDuplicate && !result.otcMapDraftIdempotencyConflict
        && safeBoundaries;
    if (created || duplicate) {
        lastOtcMapDraftStatus_ = duplicate
            ? QStringLiteral("DUPLICATE")
            : QStringLiteral("DRAFT_CREATED");
        emit tradeDraftStateChanged();
        return;
    }

    lastOtcMapDraftStatus_ = result.otcMapDraftIdempotencyConflict
        ? QStringLiteral("IDEMPOTENCY_CONFLICT")
        : (result.payloadStatus.empty()
            ? QStringLiteral("REJECTED")
            : QString::fromStdString(result.payloadStatus));
    if (lastOtcMapDraftSummary_.isEmpty()) {
        lastOtcMapDraftSummary_ = issueTextFromResult(result);
    }
    emit tradeDraftStateChanged();
}

ShellAccountingServiceRequest ShellAccountingPresenter::makeExcelVbaImportPreviewRequest(
    const QString& importPayloadJson,
    bool& valid)
{
    valid = false;
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(
        importPayloadJson.trimmed().toUtf8(),
        &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        markExcelVbaImportPreviewInputError(
            QStringLiteral("Excel/VBA import preview payload must be a JSON object."));
        return {};
    }

    const auto object = document.object();
    if (object.contains(QStringLiteral("filePath"))
        || object.contains(QStringLiteral("path"))
        || object.contains(QStringLiteral("filename"))) {
        markExcelVbaImportPreviewInputError(
            QStringLiteral("Excel/VBA import preview accepts in-memory payload JSON only."));
        return {};
    }

    ShellAccountingServiceRequest request;
    request.actionName = "accounting.excel_vba_import.readonly_preview";
    request.importPayloadJson = QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
    request.timeoutMs = 2000;
    valid = true;
    return request;
}

ShellAccountingServiceRequest ShellAccountingPresenter::makePortfolioReplayReadOnlySummaryRequest(
    const QString& replayPayloadJson,
    bool& valid)
{
    valid = false;
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(
        replayPayloadJson.trimmed().toUtf8(),
        &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        markPortfolioReplayInputError(
            QStringLiteral("Portfolio replay summary payload must be a JSON object."));
        return {};
    }

    const auto object = document.object();
    if (object.contains(QStringLiteral("filePath"))
        || object.contains(QStringLiteral("path"))
        || object.contains(QStringLiteral("filename"))
        || object.contains(QStringLiteral("credential"))
        || object.contains(QStringLiteral("endpoint"))) {
        markPortfolioReplayInputError(
            QStringLiteral("Portfolio replay accepts sanitized in-memory payload JSON only."));
        return {};
    }

    ShellAccountingServiceRequest request;
    request.actionName = "accounting.portfolio_replay.readonly_summary";
    request.portfolioReplayPayloadJson =
        QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
    request.timeoutMs = 2000;
    valid = true;
    return request;
}

ShellAccountingServiceRequest
ShellAccountingPresenter::makeStrategyRecommendationReadOnlySummaryRequest(
    const QString& recommendationPayloadJson,
    bool& valid)
{
    valid = false;
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(
        recommendationPayloadJson.trimmed().toUtf8(),
        &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        markStrategyRecommendationInputError(
            QStringLiteral("Strategy recommendation payload must be a JSON object."));
        return {};
    }

    const auto object = document.object();
    if (object.contains(QStringLiteral("filePath"))
        || object.contains(QStringLiteral("path"))
        || object.contains(QStringLiteral("filename"))
        || object.contains(QStringLiteral("credential"))
        || object.contains(QStringLiteral("endpoint"))) {
        markStrategyRecommendationInputError(
            QStringLiteral("Strategy recommendation accepts sanitized in-memory payload JSON only."));
        return {};
    }

    ShellAccountingServiceRequest request;
    request.actionName = "strategy.recommendation.readonly_summary";
    request.strategyRecommendationPayloadJson =
        QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
    request.timeoutMs = 2000;
    valid = true;
    return request;
}

ShellAccountingServiceRequest
ShellAccountingPresenter::makeTradeDraftCreateFromRecommendationRequest(
    bool userConfirmed,
    bool& valid)
{
    valid = false;
    const auto action = canonicalAsciiToken(lastStrategyRecommendationAction_);
    if (lastStrategyRecommendationPayloadJson_.trimmed().isEmpty()
        || lastStrategyRecommendationStatus_ != QStringLiteral("OK")
        || (action != QStringLiteral("BUY") && action != QStringLiteral("SELL"))) {
        markTradeDraftInputError(
            QStringLiteral("Only the latest accepted BUY or SELL recommendation can become a TradeDraft."));
        return {};
    }
    if (!userConfirmed) {
        markTradeDraftInputError(
            QStringLiteral("Explicit user confirmation is required before creating a TradeDraft."));
        return {};
    }

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(
        lastStrategyRecommendationPayloadJson_.toUtf8(),
        &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        markTradeDraftInputError(
            QStringLiteral("Latest strategy recommendation payload is not a JSON object."));
        return {};
    }

    const auto object = document.object();
    const auto input = object.value(QStringLiteral("input")).toObject();
    const auto accountId = input.value(QStringLiteral("accountId")).toString();
    const auto portfolioId = input.value(QStringLiteral("portfolioId")).toString();
    const auto instrumentCode = input.value(QStringLiteral("instrumentCode")).toString();
    if (accountId.trimmed().isEmpty() || portfolioId.trimmed().isEmpty()
        || instrumentCode.trimmed().isEmpty()) {
        markTradeDraftInputError(
            QStringLiteral("Recommendation account, portfolio, and instrument context are required."));
        return {};
    }

    const auto digest = tradeDraftRecommendationDigest();
    const auto idempotencyKey = tradeDraftIdempotencyKey();
    if (digest.isEmpty() || idempotencyKey.isEmpty()) {
        markTradeDraftInputError(
            QStringLiteral("Recommendation digest and idempotency key are required."));
        return {};
    }

    ShellAccountingServiceRequest request;
    request.actionName = "accounting.tradedraft.create_from_recommendation";
    request.strategyRecommendationPayloadJson = lastStrategyRecommendationPayloadJson_.toStdString();
    request.recommendationDigest = digest.toStdString();
    request.idempotencyKey = idempotencyKey.toStdString();
    request.accountId = accountId.trimmed().toStdString();
    request.portfolioId = portfolioId.trimmed().toStdString();
    request.strategyId = input.value(QStringLiteral("strategyId")).toString(QStringLiteral("1001")).trimmed().toStdString();
    request.instrumentId = input.value(QStringLiteral("instrumentId")).toString(QStringLiteral("1001")).trimmed().toStdString();
    request.strategyCode = input.value(QStringLiteral("strategyCode")).toString(QStringLiteral("VBA_PARITY")).trimmed().toStdString();
    request.instrumentCode = instrumentCode.trimmed().toStdString();
    request.instrumentType = input.value(QStringLiteral("instrumentType")).toString(QStringLiteral("ETF")).trimmed().toStdString();
    request.tradeSource = input.value(QStringLiteral("tradeSource")).toString(QStringLiteral("ETF_DIRECT")).trimmed().toStdString();
    request.expiresAtUtc = QDateTime::currentDateTimeUtc().addSecs(24 * 60 * 60).toString(Qt::ISODate).toStdString();
    request.userConfirmed = true;
    request.timeoutMs = 2000;
    valid = true;
    return request;
}

ShellAccountingServiceRequest
ShellAccountingPresenter::makeTradeDraftReadOnlySummaryRequest(bool& valid) const
{
    valid = false;
    ShellAccountingServiceRequest request;
    request.actionName = "accounting.tradedraft.readonly_summary";
    request.idempotencyKey = tradeDraftIdempotencyKey().toStdString();
    bool draftOk = false;
    request.draftId = lastTradeDraftId_.toLongLong(&draftOk);
    if (request.idempotencyKey.empty() && (!draftOk || request.draftId <= 0)) {
        return request;
    }
    request.timeoutMs = 2000;
    valid = true;
    return request;
}

ShellAccountingServiceRequest ShellAccountingPresenter::makeOtcMapRequest(
    const QString& payloadJson,
    bool userConfirmed,
    bool& valid)
{
    valid = false;
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(
        payloadJson.trimmed().toUtf8(),
        &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        markTradeDraftInputError(
            QStringLiteral("OTCMap multi-channel draft payload must be a JSON object."));
        return {};
    }

    auto object = document.object();
    if (object.contains(QStringLiteral("filePath"))
        || object.contains(QStringLiteral("path"))
        || object.contains(QStringLiteral("filename"))
        || object.contains(QStringLiteral("credential"))
        || object.contains(QStringLiteral("endpoint"))
        || object.contains(QStringLiteral("broker"))
        || object.contains(QStringLiteral("automaticTrading"))) {
        markTradeDraftInputError(
            QStringLiteral("OTCMap draft accepts sanitized in-memory payload JSON only."));
        return {};
    }

    if (object.value(QStringLiteral("input")).isObject()) {
        auto input = object.value(QStringLiteral("input")).toObject();
        input.insert(QStringLiteral("userConfirmed"), userConfirmed);
        object.insert(QStringLiteral("input"), input);
    }
    object.insert(QStringLiteral("userConfirmed"), userConfirmed);
    ShellAccountingServiceRequest request;
    request.actionName = userConfirmed
        ? "accounting.tradedraft.create_otcmap_multichannel"
        : "accounting.otcmap_multichannel.readonly_preview";
    request.otcMapPayloadJson =
        QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
    request.timeoutMs = 2000;
    request.userConfirmed = userConfirmed;
    valid = true;
    return request;
}

QString ShellAccountingPresenter::tradeDraftRecommendationDigest() const
{
    if (lastStrategyRecommendationPayloadJson_.trimmed().isEmpty()) {
        return {};
    }
    std::uint64_t hash = 14695981039346656037ULL;
    hashPreviewField(hash, QStringLiteral("EPIC-278"));
    hashPreviewField(hash, lastStrategyRecommendationPayloadJson_);
    hashPreviewField(hash, lastStrategyRecommendationAction_);
    hashPreviewField(hash, lastStrategyRecommendationQuantityText_);
    hashPreviewField(hash, lastStrategyRecommendationAmountText_);
    hashPreviewField(hash, lastStrategyRecommendationNetCashImpactText_);
    return hexFnv64(hash);
}

QString ShellAccountingPresenter::tradeDraftIdempotencyKey() const
{
    const auto digest = tradeDraftRecommendationDigest();
    if (digest.isEmpty()) {
        return {};
    }
    return QStringLiteral("tradedraft-recommendation:") + digest;
}

ShellAccountingServiceRequest ShellAccountingPresenter::makeExcelVbaImportPersistManualEntryRequest(
    const QString& previewStatus,
    const QString& previewDigest,
    const QString& idempotencyKey,
    const QString& schemaVersion,
    const QString& source,
    const QString& acceptedAt,
    const QString& importBatchLabel,
    const QString& importPayloadJson,
    int tradeFactCount,
    int cashFactCount,
    int marketPriceFactCount,
    int fxRateFactCount,
    bool& valid)
{
    valid = false;
    const auto normalizedStatus = previewStatus.trimmed().toUpper();
    if (normalizedStatus != QStringLiteral("ACCEPTED")) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Only an ACCEPTED Excel/VBA import preview can be persisted."));
        return {};
    }
    if (previewDigest.trimmed().isEmpty() || idempotencyKey.trimmed().isEmpty()
        || schemaVersion.trimmed().isEmpty() || source.trimmed().isEmpty()
        || acceptedAt.trimmed().isEmpty() || importBatchLabel.trimmed().isEmpty()) {
        markExcelVbaImportPersistInputError(QStringLiteral(
            "Accepted preview digest, idempotency key, schema version, source, acceptedAt, and batch label are required."));
        return {};
    }
    if (tradeFactCount < 0 || cashFactCount < 0 || marketPriceFactCount < 0
        || fxRateFactCount < 0 || (tradeFactCount + cashFactCount) <= 0) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Accepted preview fact summary must contain trade or cash facts."));
        return {};
    }
    if (marketPriceFactCount > 0 || fxRateFactCount > 0) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Market price and FX rate facts are not supported for persistence yet."));
        return {};
    }

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(
        importPayloadJson.trimmed().toUtf8(),
        &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Accepted Excel/VBA import payload must be a JSON object."));
        return {};
    }
    const auto object = document.object();
    if (object.contains(QStringLiteral("filePath"))
        || object.contains(QStringLiteral("path"))
        || object.contains(QStringLiteral("filename"))) {
        markExcelVbaImportPersistInputError(
            QStringLiteral("Excel/VBA import persistence accepts sanitized in-memory payload JSON only."));
        return {};
    }

    ShellAccountingServiceRequest request;
    request.actionName = "accounting.excel_vba_import.persist_manual_entry";
    request.previewStatus = normalizedStatus.toStdString();
    request.previewDigest = trimmedStdString(previewDigest);
    request.idempotencyKey = trimmedStdString(idempotencyKey);
    request.schemaVersion = trimmedStdString(schemaVersion);
    request.source = trimmedStdString(source);
    request.acceptedAt = trimmedStdString(acceptedAt);
    request.importBatchLabel = trimmedStdString(importBatchLabel);
    request.requestId = "shellservices-excel-vba-import-persist";
    request.importPayloadJson = QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
    request.importTradeFactCount = tradeFactCount;
    request.importCashFactCount = cashFactCount;
    request.importMarketPriceFactCount = marketPriceFactCount;
    request.importFxRateFactCount = fxRateFactCount;
    request.timeoutMs = 2000;
    valid = true;
    return request;
}

QString ShellAccountingPresenter::acceptedExcelVbaImportPreviewIdempotencyKey() const
{
    if (lastExcelVbaImportPreviewDigest_.trimmed().isEmpty()) {
        return {};
    }
    return QStringLiteral("shell-qml-excel-vba-import:") + lastExcelVbaImportPreviewDigest_.trimmed();
}

QString ShellAccountingPresenter::acceptedExcelVbaImportPreviewDigest(
    const QString& importPayloadJson,
    bool& valid) const
{
    valid = false;
    QJsonParseError parseError {};
    const auto document =
        QJsonDocument::fromJson(importPayloadJson.trimmed().toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return {};
    }
    const auto object = document.object();
    const auto sheets = object.value(QStringLiteral("sheets")).toArray();
    if (sheets.isEmpty()) {
        return {};
    }

    const auto cashSheet = findPreviewSheet(sheets, QStringLiteral("InitialCash"));
    const auto tradeSheet = findPreviewSheet(sheets, QStringLiteral("Trades"));
    if (!cashSheet.has_value() || !tradeSheet.has_value()) {
        return {};
    }

    const auto cashHeaders = previewHeaders(*cashSheet);
    const auto tradeHeaders = previewHeaders(*tradeSheet);
    for (const auto& field : {
             QStringLiteral("ROW_ID"),
             QStringLiteral("TIME_UTC"),
             QStringLiteral("ACCOUNT_CODE"),
             QStringLiteral("PORTFOLIO_CODE"),
             QStringLiteral("ACTION"),
             QStringLiteral("AMOUNT"),
             QStringLiteral("CURRENCY"),
             QStringLiteral("MEMO"),
         }) {
        if (!previewHeaderIndex(cashHeaders, field).has_value()) {
            return {};
        }
    }
    for (const auto& field : {
             QStringLiteral("ROW_ID"),
             QStringLiteral("TRADE_TIME_UTC"),
             QStringLiteral("ACCOUNT_CODE"),
             QStringLiteral("PORTFOLIO_CODE"),
             QStringLiteral("INSTRUMENT_CODE"),
             QStringLiteral("SIDE"),
             QStringLiteral("QUANTITY"),
             QStringLiteral("PRICE"),
             QStringLiteral("AMOUNT"),
             QStringLiteral("FEE"),
             QStringLiteral("CASH_FLOW"),
             QStringLiteral("CURRENCY"),
             QStringLiteral("SOURCE"),
             QStringLiteral("MEMO"),
         }) {
        if (!previewHeaderIndex(tradeHeaders, field).has_value()) {
            return {};
        }
    }

    const auto cashRows = cashSheet->value(QStringLiteral("rows")).toArray();
    const auto tradeRows = tradeSheet->value(QStringLiteral("rows")).toArray();
    const auto priceSheet = findPreviewSheet(sheets, QStringLiteral("MarketPrices"));
    const auto fxSheet = findPreviewSheet(sheets, QStringLiteral("FxRates"));
    const auto priceRows = priceSheet.has_value()
        ? priceSheet->value(QStringLiteral("rows")).toArray()
        : QJsonArray {};
    const auto fxRows = fxSheet.has_value()
        ? fxSheet->value(QStringLiteral("rows")).toArray()
        : QJsonArray {};

    std::uint64_t hash = 14695981039346656037ULL;
    hashPreviewField(hash, QStringLiteral("accepted"));
    hashPreviewField(hash, QString::number(tradeRows.size()));
    hashPreviewField(hash, QString::number(cashRows.size()));
    hashPreviewField(hash, QString::number(priceRows.size()));
    hashPreviewField(hash, QString::number(fxRows.size()));

    for (const auto& rowValue : tradeRows) {
        if (!rowValue.isArray()) {
            return {};
        }
        const auto row = rowValue.toArray();
        const auto action =
            normalizedPreviewTradeAction(previewCellValue(tradeHeaders, row, QStringLiteral("SIDE")));
        if (action.isEmpty()) {
            return {};
        }
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("ROW_ID")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("TRADE_TIME_UTC")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("ACCOUNT_CODE")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("PORTFOLIO_CODE")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("INSTRUMENT_CODE")));
        hashPreviewField(hash, action);
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("QUANTITY")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("PRICE")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("AMOUNT")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("FEE")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("CURRENCY")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("SOURCE")));
        hashPreviewField(hash, previewCellValue(tradeHeaders, row, QStringLiteral("MEMO")));
    }

    for (const auto& rowValue : cashRows) {
        if (!rowValue.isArray()) {
            return {};
        }
        const auto row = rowValue.toArray();
        const auto action =
            normalizedPreviewCashAction(previewCellValue(cashHeaders, row, QStringLiteral("ACTION")));
        if (action.isEmpty()) {
            return {};
        }
        hashPreviewField(hash, previewCellValue(cashHeaders, row, QStringLiteral("ROW_ID")));
        hashPreviewField(hash, previewCellValue(cashHeaders, row, QStringLiteral("TIME_UTC")));
        hashPreviewField(hash, previewCellValue(cashHeaders, row, QStringLiteral("ACCOUNT_CODE")));
        hashPreviewField(hash, previewCellValue(cashHeaders, row, QStringLiteral("PORTFOLIO_CODE")));
        hashPreviewField(hash, action);
        hashPreviewField(hash, previewCellValue(cashHeaders, row, QStringLiteral("AMOUNT")));
        hashPreviewField(hash, previewCellValue(cashHeaders, row, QStringLiteral("CURRENCY")));
        hashPreviewField(hash, previewCellValue(cashHeaders, row, QStringLiteral("MEMO")));
    }

    valid = true;
    return hexFnv64(hash);
}

ShellAccountingServiceRequest ShellAccountingPresenter::makeDraftCreateRequest(
    const QString& accountId,
    const QString& portfolioId,
    const QString& instrumentId,
    const QString& instrumentCode,
    const QString& side,
    const QString& quantity,
    const QString& reason,
    bool& valid)
{
    valid = false;
    tradingUiIssue_.clear();

    const auto normalizedSide = side.trimmed().toUpper();
    bool quantityOk = false;
    const auto quantityValue = quantity.trimmed().toDouble(&quantityOk);
    if (accountId.trimmed().isEmpty() || portfolioId.trimmed().isEmpty()
        || instrumentId.trimmed().isEmpty() || instrumentCode.trimmed().isEmpty()
        || reason.trimmed().isEmpty()) {
        tradingUiStatus_ = QStringLiteral("INPUT_ERROR");
        tradingUiIssue_ = QStringLiteral("Account, portfolio, instrument, quantity, and reason are required.");
        return {};
    }
    if (normalizedSide != QStringLiteral("BUY") && normalizedSide != QStringLiteral("SELL")) {
        tradingUiStatus_ = QStringLiteral("INPUT_ERROR");
        tradingUiIssue_ = QStringLiteral("Side must be BUY or SELL.");
        return {};
    }
    if (!quantityOk || quantityValue <= 0.0) {
        tradingUiStatus_ = QStringLiteral("INPUT_ERROR");
        tradingUiIssue_ = QStringLiteral("Quantity must be greater than zero.");
        return {};
    }

    ShellAccountingServiceRequest request;
    request.actionName = "accounting.tradedraft.create";
    request.accountId = accountId.trimmed().toStdString();
    request.portfolioId = portfolioId.trimmed().toStdString();
    request.strategyId = QStringLiteral("1001").toStdString();
    request.strategyCode = QStringLiteral("TASK139").toStdString();
    request.instrumentId = instrumentId.trimmed().toStdString();
    request.instrumentCode = instrumentCode.trimmed().toStdString();
    request.side = normalizedSide.toStdString();
    request.quantity1e6 = static_cast<std::int64_t>(std::llround(quantityValue * 1000000.0));
    request.reason = reason.trimmed().toStdString();
    request.source = "snapshotRebuildPreview";
    request.sourceSnapshotId = "task-144-snapshot-1001-1001-CNY";
    request.authorization = "TASK-148_TRADEDRAFT_WRITE";
    request.riskChecked = true;
    request.riskBlocked = false;
    request.timeoutMs = 2000;
    lastDraftRequest_ = request;
    valid = true;
    return request;
}

ShellAccountingServiceRequest ShellAccountingPresenter::makeDraftConfirmRequest(bool& valid) const
{
    valid = false;
    ShellAccountingServiceRequest request = lastDraftRequest_;
    request.actionName = "accounting.tradedraft.confirm";
    request.source = "existingTradeDraft";
    request.authorization = "TASK-150_TRADEDRAFT_CONFIRM";
    request.timeoutMs = 2000;
    bool draftIdOk = false;
    request.draftId = draftId_.toLongLong(&draftIdOk);
    if (!draftIdOk || request.draftId <= 0) {
        return request;
    }
    valid = true;
    return request;
}

ShellAccountingServiceRequest ShellAccountingPresenter::makeManualTransactionRequest(
    const QString& accountId,
    const QString& portfolioId,
    const QString& instrumentId,
    const QString& securityCode,
    const QString& side,
    const QString& quantityUnits,
    const QString& priceAmountMinor,
    const QString& grossAmountMinor,
    const QString& feeAmountMinor,
    const QString& taxAmountMinor,
    const QString& occurredAt,
    const QString& sourceMemo,
    const QString& requestId,
    const QString& idempotencyKey,
    bool& valid)
{
    valid = false;
    const auto normalizedSide = side.trimmed().toUpper();
    if (accountId.trimmed().isEmpty() || portfolioId.trimmed().isEmpty()
        || instrumentId.trimmed().isEmpty() || securityCode.trimmed().isEmpty()
        || occurredAt.trimmed().isEmpty() || sourceMemo.trimmed().isEmpty()
        || requestId.trimmed().isEmpty() || idempotencyKey.trimmed().isEmpty()) {
        markManualEntryInputError(QStringLiteral(
            "Account, portfolio, instrument, security, occurredAt, memo, requestId, and idempotencyKey are required."));
        return {};
    }
    if (normalizedSide != QStringLiteral("BUY") && normalizedSide != QStringLiteral("SELL")) {
        markManualEntryInputError(QStringLiteral("Manual transaction side must be BUY or SELL."));
        return {};
    }
    if (!positiveIntegerText(quantityUnits) || !positiveIntegerText(priceAmountMinor)
        || !positiveIntegerText(grossAmountMinor) || !nonNegativeIntegerText(feeAmountMinor)
        || !nonNegativeIntegerText(taxAmountMinor)) {
        markManualEntryInputError(QStringLiteral(
            "Quantity, price, and gross amount must be positive; fee and tax must be zero or greater."));
        return {};
    }

    ShellAccountingServiceRequest request;
    request.actionName = "accounting.manual_transaction.create";
    request.accountId = trimmedStdString(accountId);
    request.portfolioId = trimmedStdString(portfolioId);
    request.instrumentId = trimmedStdString(instrumentId);
    request.securityCode = trimmedStdString(securityCode);
    request.tradeSide = normalizedSide == QStringLiteral("BUY") ? "Buy" : "Sell";
    request.quantityUnits = trimmedStdString(quantityUnits);
    request.priceAmountMinor = trimmedStdString(priceAmountMinor);
    request.grossAmountMinor = trimmedStdString(grossAmountMinor);
    request.feeAmountMinor = trimmedStdString(feeAmountMinor);
    request.taxAmountMinor = trimmedStdString(taxAmountMinor);
    request.occurredAt = trimmedStdString(occurredAt);
    request.sourceMemo = trimmedStdString(sourceMemo);
    request.requestId = trimmedStdString(requestId);
    request.idempotencyKey = trimmedStdString(idempotencyKey);
    request.timeoutMs = 2000;
    valid = true;
    return request;
}

ShellAccountingServiceRequest ShellAccountingPresenter::makeManualCashMovementRequest(
    const QString& accountId,
    const QString& portfolioId,
    const QString& movementType,
    const QString& amountMinor,
    const QString& currency,
    const QString& occurredAt,
    const QString& sourceMemo,
    const QString& sourceReference,
    const QString& requestId,
    const QString& idempotencyKey,
    bool& valid)
{
    valid = false;
    const auto normalizedType = movementType.trimmed();
    if (accountId.trimmed().isEmpty() || portfolioId.trimmed().isEmpty()
        || currency.trimmed().isEmpty() || occurredAt.trimmed().isEmpty()
        || sourceMemo.trimmed().isEmpty() || sourceReference.trimmed().isEmpty()
        || requestId.trimmed().isEmpty() || idempotencyKey.trimmed().isEmpty()) {
        markManualEntryInputError(QStringLiteral(
            "Account, portfolio, currency, occurredAt, memo, reference, requestId, and idempotencyKey are required."));
        return {};
    }
    if (normalizedType != QStringLiteral("Deposit") && normalizedType != QStringLiteral("Withdrawal")) {
        markManualEntryInputError(QStringLiteral("Cash movement type must be Deposit or Withdrawal."));
        return {};
    }
    if (!positiveIntegerText(amountMinor)) {
        markManualEntryInputError(QStringLiteral("Cash movement amount must be greater than zero."));
        return {};
    }

    ShellAccountingServiceRequest request;
    request.actionName = "accounting.manual_cash_movement.create";
    request.accountId = trimmedStdString(accountId);
    request.portfolioId = trimmedStdString(portfolioId);
    request.movementType = trimmedStdString(normalizedType);
    request.amountMinor = trimmedStdString(amountMinor);
    request.currency = currency.trimmed().toUpper().toStdString();
    request.occurredAt = trimmedStdString(occurredAt);
    request.sourceMemo = trimmedStdString(sourceMemo);
    request.sourceReference = trimmedStdString(sourceReference);
    request.requestId = trimmedStdString(requestId);
    request.idempotencyKey = trimmedStdString(idempotencyKey);
    request.timeoutMs = 2000;
    valid = true;
    return request;
}

}  // namespace etfdt::shell_services
