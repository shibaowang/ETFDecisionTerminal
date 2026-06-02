#pragma once

#include "ShellServices/ShellAccountingIssueListModel.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "ShellServices/ShellAccountingStatusObject.h"
#include "ShellServices/ShellPositionListModel.h"

#include <QObject>
#include <QString>

#include <memory>

namespace etfdt::shell_services {

class ShellAccountingPresenter final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool readOnly READ readOnly)
    Q_PROPERTY(bool writeEnabled READ writeEnabled)
    Q_PROPERTY(bool productionTradingUiEnabled READ productionTradingUiEnabled NOTIFY tradingUiStateChanged)
    Q_PROPERTY(QString tradingUiStatus READ tradingUiStatus NOTIFY tradingUiStateChanged)
    Q_PROPERTY(QString tradingUiIssue READ tradingUiIssue NOTIFY tradingUiStateChanged)
    Q_PROPERTY(QString draftId READ draftId NOTIFY tradingUiStateChanged)
    Q_PROPERTY(QString draftUid READ draftUid NOTIFY tradingUiStateChanged)
    Q_PROPERTY(QString ledgerStatus READ ledgerStatus NOTIFY tradingUiStateChanged)
    Q_PROPERTY(bool manualEntryBusy READ manualEntryBusy NOTIFY manualEntryStateChanged)
    Q_PROPERTY(QString lastManualEntryStatus READ lastManualEntryStatus NOTIFY manualEntryStateChanged)
    Q_PROPERTY(QString lastManualEntryIssue READ lastManualEntryIssue NOTIFY manualEntryStateChanged)
    Q_PROPERTY(QString lastManualEntryResult READ lastManualEntryResult NOTIFY manualEntryStateChanged)

public:
    explicit ShellAccountingPresenter(QObject* parent = nullptr);

    [[nodiscard]] bool readOnly() const noexcept;
    [[nodiscard]] bool writeEnabled() const noexcept;
    [[nodiscard]] bool tradeDraftGenerationEnabled() const noexcept;
    [[nodiscard]] bool tradeSuggestionEnabled() const noexcept;
    [[nodiscard]] bool strategyExecutionEnabled() const noexcept;
    [[nodiscard]] bool brokerSubmissionEnabled() const noexcept;
    [[nodiscard]] bool productionTradingUiEnabled() const noexcept;
    [[nodiscard]] QString tradingUiStatus() const;
    [[nodiscard]] QString tradingUiIssue() const;
    [[nodiscard]] QString draftId() const;
    [[nodiscard]] QString draftUid() const;
    [[nodiscard]] QString ledgerStatus() const;
    [[nodiscard]] bool manualEntryBusy() const noexcept;
    [[nodiscard]] QString lastManualEntryStatus() const;
    [[nodiscard]] QString lastManualEntryIssue() const;
    [[nodiscard]] QString lastManualEntryResult() const;

    [[nodiscard]] ShellAccountingStatusObject& statusObject() noexcept;
    [[nodiscard]] const ShellAccountingStatusObject& statusObject() const noexcept;
    [[nodiscard]] ShellAccountingIssueListModel& issueListModel() noexcept;
    [[nodiscard]] const ShellAccountingIssueListModel& issueListModel() const noexcept;
    [[nodiscard]] ShellPositionListModel& positionListModel() noexcept;
    [[nodiscard]] const ShellPositionListModel& positionListModel() const noexcept;

    void setController(std::shared_ptr<ShellAccountingReadOnlyController> controller);
    void clearController() noexcept;
    [[nodiscard]] bool hasController() const noexcept;

    void setPrivacyMode(bool enabled);
    [[nodiscard]] bool privacyMode() const noexcept;

    void syncFromController();
    void refreshPositionList();
    void refreshCashSummary();
    void refreshPortfolioPnlSummary();
    void refreshBasePositionSummary();
    void refreshSniperPoolSummary();
    void refreshAllReadOnly();
    Q_INVOKABLE bool createDraft(
        const QString& accountId,
        const QString& portfolioId,
        const QString& instrumentId,
        const QString& instrumentCode,
        const QString& side,
        const QString& quantity,
        const QString& reason);
    Q_INVOKABLE bool confirmDraft();
    Q_INVOKABLE void resetTradingUi();
    Q_INVOKABLE bool submitManualTransaction(
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
        const QString& idempotencyKey);
    Q_INVOKABLE bool submitManualCashMovement(
        const QString& accountId,
        const QString& portfolioId,
        const QString& movementType,
        const QString& amountMinor,
        const QString& currency,
        const QString& occurredAt,
        const QString& sourceMemo,
        const QString& sourceReference,
        const QString& requestId,
        const QString& idempotencyKey);
    Q_INVOKABLE void resetManualEntryUi();
    void reset();

signals:
    void tradingUiStateChanged();
    void manualEntryStateChanged();

private:
    void markControllerNotConfigured(const char* actionName);
    void refreshAccountingAction(
        const char* actionName,
        void (ShellAccountingReadOnlyController::*refresh)(
            const ShellAccountingServiceRequest&));
    void applyControllerState(
        const std::string& actionName,
        ShellAccountingViewState state,
        std::vector<ShellAccountingIssue> issues);
    void applyTradingResult(const ShellAccountingServiceResult& result, bool confirming);
    void applyManualEntryResult(const ShellAccountingServiceResult& result, const QString& fallback);
    void markManualEntryInputError(const QString& message);
    [[nodiscard]] ShellAccountingServiceRequest makeDraftCreateRequest(
        const QString& accountId,
        const QString& portfolioId,
        const QString& instrumentId,
        const QString& instrumentCode,
        const QString& side,
        const QString& quantity,
        const QString& reason,
        bool& valid);
    [[nodiscard]] ShellAccountingServiceRequest makeDraftConfirmRequest(bool& valid) const;
    [[nodiscard]] ShellAccountingServiceRequest makeManualTransactionRequest(
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
        bool& valid);
    [[nodiscard]] ShellAccountingServiceRequest makeManualCashMovementRequest(
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
        bool& valid);

    ShellAccountingStatusObject status_;
    ShellAccountingIssueListModel issues_;
    ShellPositionListModel positions_;
    std::shared_ptr<ShellAccountingReadOnlyController> controller_;
    bool privacyMode_ = false;
    QString tradingUiStatus_ = QStringLiteral("READY");
    QString tradingUiIssue_;
    QString draftId_;
    QString draftUid_;
    QString ledgerStatus_ = QStringLiteral("NOT_CONFIRMED");
    bool manualEntryBusy_ = false;
    QString lastManualEntryStatus_ = QStringLiteral("READY");
    QString lastManualEntryIssue_;
    QString lastManualEntryResult_;
    ShellAccountingServiceRequest lastDraftRequest_;
};

}  // namespace etfdt::shell_services
