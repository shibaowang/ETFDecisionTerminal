#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <utility>

namespace etfdt::shell_services {

namespace {

ShellAccountingIssue makeControllerIssue(
    std::string code,
    std::string message,
    bool blocking = true)
{
    return {
        std::move(code),
        blocking ? "ERROR" : "WARNING",
        std::move(message),
        blocking,
        "ShellAccountingReadOnlyController",
    };
}

void appendIssues(
    std::vector<ShellAccountingIssue>& target,
    const std::vector<ShellAccountingIssue>& source)
{
    target.insert(target.end(), source.begin(), source.end());
}

}  // namespace

bool ShellAccountingReadOnlyController::readOnly() const noexcept
{
    return true;
}

bool ShellAccountingReadOnlyController::writeEnabled() const noexcept
{
    return false;
}

bool ShellAccountingReadOnlyController::tradeDraftGenerationEnabled() const noexcept
{
    return false;
}

bool ShellAccountingReadOnlyController::tradeSuggestionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingReadOnlyController::strategyExecutionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingReadOnlyController::brokerOrderEnabled() const noexcept
{
    return false;
}

ShellAccountingViewState ShellAccountingReadOnlyController::currentState() const noexcept
{
    return state_;
}

const std::vector<ShellAccountingIssue>& ShellAccountingReadOnlyController::issues() const noexcept
{
    return issues_;
}

const std::string& ShellAccountingReadOnlyController::currentActionName() const noexcept
{
    return actionName_;
}

void ShellAccountingReadOnlyController::setPrivacyMode(bool enabled) noexcept
{
    privacyMode_ = enabled;
}

bool ShellAccountingReadOnlyController::privacyMode() const noexcept
{
    return privacyMode_;
}

std::string ShellAccountingReadOnlyController::displayText(
    const ShellAccountingDisplayText& text) const
{
    return shellAccountingDisplayText(text, privacyMode_, state_);
}

void ShellAccountingReadOnlyController::applyStateSnapshot(
    ShellAccountingStateSnapshot snapshot)
{
    actionName_ = std::move(snapshot.actionName);
    issues_ = std::move(snapshot.issues);

    if (!snapshot.implemented ||
        isShellAccountingGuardUnavailableStatus(snapshot.payloadStatus)) {
        state_ = ShellAccountingViewState::Unavailable;
        return;
    }

    if (snapshot.stale) {
        state_ = ShellAccountingViewState::Stale;
        return;
    }

    state_ = shellAccountingViewStateFromDataQuality(
        snapshot.dataQualityStatus,
        snapshot.hasRows);
}

void ShellAccountingReadOnlyController::beginRefresh(std::string actionName)
{
    actionName_ = std::move(actionName);
    state_ = ShellAccountingViewState::Loading;
    issues_.clear();
}

void ShellAccountingReadOnlyController::markUnavailable(
    std::string actionName,
    ShellAccountingIssue issue)
{
    actionName_ = std::move(actionName);
    state_ = ShellAccountingViewState::Unavailable;
    issues_.clear();
    issues_.push_back(std::move(issue));
}

void ShellAccountingReadOnlyController::setServiceAdapter(
    std::shared_ptr<ShellAccountingServiceAdapter> adapter) noexcept
{
    serviceAdapter_ = std::move(adapter);
}

bool ShellAccountingReadOnlyController::hasServiceAdapter() const noexcept
{
    return static_cast<bool>(serviceAdapter_);
}

void ShellAccountingReadOnlyController::clearServiceAdapter() noexcept
{
    serviceAdapter_.reset();
}

void ShellAccountingReadOnlyController::refreshPositionList(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("position.list");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("position.list");
        return;
    }
    applyServiceResult(serviceAdapter_->fetchPositionList(request));
}

void ShellAccountingReadOnlyController::refreshCashSummary(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("cash.summary");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("cash.summary");
        return;
    }
    applyServiceResult(serviceAdapter_->fetchCashSummary(request));
}

void ShellAccountingReadOnlyController::refreshPortfolioPnlSummary(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("portfolio.pnl.summary");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("portfolio.pnl.summary");
        return;
    }
    applyServiceResult(serviceAdapter_->fetchPortfolioPnlSummary(request));
}

void ShellAccountingReadOnlyController::refreshBasePositionSummary(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("base_position.summary");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("base_position.summary");
        return;
    }
    applyServiceResult(serviceAdapter_->fetchBasePositionSummary(request));
}

void ShellAccountingReadOnlyController::refreshSniperPoolSummary(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("sniper_pool.summary");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("sniper_pool.summary");
        return;
    }
    applyServiceResult(serviceAdapter_->fetchSniperPoolSummary(request));
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::createDraft(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.tradedraft.create");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.tradedraft.create");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.tradedraft.create";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = false;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->createDraft(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::createDraftFromRecommendation(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.tradedraft.create_from_recommendation");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.tradedraft.create_from_recommendation");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.tradedraft.create_from_recommendation";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = false;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->createDraftFromRecommendation(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::fetchTradeDraftReadOnlySummary(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.tradedraft.readonly_summary");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.tradedraft.readonly_summary");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.tradedraft.readonly_summary";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = true;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->fetchTradeDraftReadOnlySummary(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::confirmDraft(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.tradedraft.confirm");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.tradedraft.confirm");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.tradedraft.confirm";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = false;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->confirmDraft(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::submitManualTransaction(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.manual_transaction.create");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.manual_transaction.create");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.manual_transaction.create";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = false;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->submitManualTransaction(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::submitManualCashMovement(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.manual_cash_movement.create");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.manual_cash_movement.create");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.manual_cash_movement.create";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = false;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->submitManualCashMovement(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::previewExcelVbaImportReadOnly(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.excel_vba_import.readonly_preview");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.excel_vba_import.readonly_preview");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.excel_vba_import.readonly_preview";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = true;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.importPreviewRejected = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->previewExcelVbaImportReadOnly(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::persistExcelVbaImportManualEntry(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.excel_vba_import.persist_manual_entry");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.excel_vba_import.persist_manual_entry");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.excel_vba_import.persist_manual_entry";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = false;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->persistExcelVbaImportManualEntry(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult ShellAccountingReadOnlyController::fetchPortfolioReplayReadOnlySummary(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("accounting.portfolio_replay.readonly_summary");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("accounting.portfolio_replay.readonly_summary");
        ShellAccountingServiceResult result;
        result.actionName = "accounting.portfolio_replay.readonly_summary";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = true;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->fetchPortfolioReplayReadOnlySummary(request);
    applyServiceResult(result);
    return result;
}

ShellAccountingServiceResult
ShellAccountingReadOnlyController::fetchStrategyRecommendationReadOnlySummary(
    const ShellAccountingServiceRequest& request)
{
    beginRefresh("strategy.recommendation.readonly_summary");
    if (!serviceAdapter_) {
        markServiceAdapterNotConfigured("strategy.recommendation.readonly_summary");
        ShellAccountingServiceResult result;
        result.actionName = "strategy.recommendation.readonly_summary";
        result.protocolSuccess = false;
        result.implemented = false;
        result.readOnly = true;
        result.writeEnabled = false;
        result.payloadStatus = "SERVICE_ADAPTER_NOT_CONFIGURED";
        result.dataQualityStatus = "UNAVAILABLE";
        result.transportError = true;
        result.issues = issues_;
        return result;
    }
    auto result = serviceAdapter_->fetchStrategyRecommendationReadOnlySummary(request);
    applyServiceResult(result);
    return result;
}

void ShellAccountingReadOnlyController::reset()
{
    actionName_.clear();
    state_ = ShellAccountingViewState::Idle;
    issues_.clear();
    privacyMode_ = false;
    serviceAdapter_.reset();
}

void ShellAccountingReadOnlyController::applyServiceResult(
    ShellAccountingServiceResult result)
{
    if (!result.actionName.empty()) {
        actionName_ = std::move(result.actionName);
    }

    issues_.clear();
    appendIssues(issues_, result.issues);
    appendIssues(issues_, result.warnings);
    appendIssues(issues_, result.errors);

    if (!result.protocolSuccess || result.protocolError) {
        issues_.push_back(makeControllerIssue(
            "PROTOCOL_ERROR",
            "Shell accounting service adapter returned a protocol error."));
        state_ = ShellAccountingViewState::Error;
        return;
    }

    if (result.timeout) {
        issues_.push_back(makeControllerIssue(
            "TIMEOUT",
            "Shell accounting service adapter timed out."));
        state_ = ShellAccountingViewState::Error;
        return;
    }

    if (result.transportError) {
        issues_.push_back(makeControllerIssue(
            "TRANSPORT_ERROR",
            "Shell accounting service adapter returned a transport error."));
        state_ = ShellAccountingViewState::Error;
        return;
    }

    if (!result.implemented ||
        isShellAccountingGuardUnavailableStatus(result.payloadStatus)) {
        state_ = ShellAccountingViewState::Unavailable;
        return;
    }

    state_ = shellAccountingViewStateFromDataQuality(
        result.dataQualityStatus,
        result.hasRows);
}

void ShellAccountingReadOnlyController::markServiceAdapterNotConfigured(
    std::string actionName)
{
    markUnavailable(
        std::move(actionName),
        makeControllerIssue(
            "SERVICE_ADAPTER_NOT_CONFIGURED",
            "Shell accounting service adapter is not configured."));
}

}  // namespace etfdt::shell_services
