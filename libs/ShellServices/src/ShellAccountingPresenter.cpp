#include "ShellServices/ShellAccountingPresenter.h"

#include "ShellServices/ShellAccountingExcelVbaImportReadOnlyFileLoader.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>

#include <cmath>
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
    emit excelVbaImportPreviewStateChanged();
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
    emit excelVbaImportPreviewStateChanged();
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
