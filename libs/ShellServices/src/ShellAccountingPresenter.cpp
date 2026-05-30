#include "ShellServices/ShellAccountingPresenter.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

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

void ShellAccountingPresenter::reset()
{
    controller_.reset();
    privacyMode_ = false;
    status_.reset();
    issues_.clearIssues();
    positions_.clearRows();
    positions_.setPrivacyMode(false);
    resetTradingUi();
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

}  // namespace etfdt::shell_services
