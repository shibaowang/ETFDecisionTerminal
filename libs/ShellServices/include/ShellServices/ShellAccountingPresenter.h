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
    Q_PROPERTY(bool postWriteRefreshEnabled READ postWriteRefreshEnabled NOTIFY postWriteRefreshStateChanged)
    Q_PROPERTY(bool postWriteRefreshBusy READ postWriteRefreshBusy NOTIFY postWriteRefreshStateChanged)
    Q_PROPERTY(QString lastPostWriteRefreshStatus READ lastPostWriteRefreshStatus NOTIFY postWriteRefreshStateChanged)
    Q_PROPERTY(QString lastPostWriteRefreshIssue READ lastPostWriteRefreshIssue NOTIFY postWriteRefreshStateChanged)
    Q_PROPERTY(QString lastPostWriteRefreshSummary READ lastPostWriteRefreshSummary NOTIFY postWriteRefreshStateChanged)
    Q_PROPERTY(bool excelVbaImportPreviewBusy READ excelVbaImportPreviewBusy NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPreviewStatus READ lastExcelVbaImportPreviewStatus NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPreviewIssue READ lastExcelVbaImportPreviewIssue NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPreviewSummary READ lastExcelVbaImportPreviewSummary NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPreviewDiagnosticCodes READ lastExcelVbaImportPreviewDiagnosticCodes NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(int excelVbaImportPreviewTradeFactCount READ excelVbaImportPreviewTradeFactCount NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(int excelVbaImportPreviewCashFactCount READ excelVbaImportPreviewCashFactCount NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(int excelVbaImportPreviewMarketPriceFactCount READ excelVbaImportPreviewMarketPriceFactCount NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(int excelVbaImportPreviewFxRateFactCount READ excelVbaImportPreviewFxRateFactCount NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPreviewDigest READ lastExcelVbaImportPreviewDigest NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(bool excelVbaImportPreviewPayloadAvailable READ excelVbaImportPreviewPayloadAvailable NOTIFY excelVbaImportPreviewStateChanged)
    Q_PROPERTY(bool excelVbaImportPersistBusy READ excelVbaImportPersistBusy NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPersistStatus READ lastExcelVbaImportPersistStatus NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPersistIssue READ lastExcelVbaImportPersistIssue NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPersistSummary READ lastExcelVbaImportPersistSummary NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(QString lastExcelVbaImportPersistIssueCodes READ lastExcelVbaImportPersistIssueCodes NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(bool lastExcelVbaImportPersistTransactionCommitted READ lastExcelVbaImportPersistTransactionCommitted NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(bool lastExcelVbaImportPersistTradeLogWritten READ lastExcelVbaImportPersistTradeLogWritten NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(bool lastExcelVbaImportPersistAuditLogWritten READ lastExcelVbaImportPersistAuditLogWritten NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(bool lastExcelVbaImportPersistDuplicateImportPrevented READ lastExcelVbaImportPersistDuplicateImportPrevented NOTIFY excelVbaImportPersistStateChanged)
    Q_PROPERTY(bool lastExcelVbaImportPersistIdempotencyConflictRejected READ lastExcelVbaImportPersistIdempotencyConflictRejected NOTIFY excelVbaImportPersistStateChanged)

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
    [[nodiscard]] bool postWriteRefreshEnabled() const noexcept;
    [[nodiscard]] bool postWriteRefreshBusy() const noexcept;
    [[nodiscard]] QString lastPostWriteRefreshStatus() const;
    [[nodiscard]] QString lastPostWriteRefreshIssue() const;
    [[nodiscard]] QString lastPostWriteRefreshSummary() const;
    [[nodiscard]] bool excelVbaImportPreviewBusy() const noexcept;
    [[nodiscard]] QString lastExcelVbaImportPreviewStatus() const;
    [[nodiscard]] QString lastExcelVbaImportPreviewIssue() const;
    [[nodiscard]] QString lastExcelVbaImportPreviewSummary() const;
    [[nodiscard]] QString lastExcelVbaImportPreviewDiagnosticCodes() const;
    [[nodiscard]] int excelVbaImportPreviewTradeFactCount() const noexcept;
    [[nodiscard]] int excelVbaImportPreviewCashFactCount() const noexcept;
    [[nodiscard]] int excelVbaImportPreviewMarketPriceFactCount() const noexcept;
    [[nodiscard]] int excelVbaImportPreviewFxRateFactCount() const noexcept;
    [[nodiscard]] QString lastExcelVbaImportPreviewDigest() const;
    [[nodiscard]] bool excelVbaImportPreviewPayloadAvailable() const noexcept;
    [[nodiscard]] bool excelVbaImportPersistBusy() const noexcept;
    [[nodiscard]] QString lastExcelVbaImportPersistStatus() const;
    [[nodiscard]] QString lastExcelVbaImportPersistIssue() const;
    [[nodiscard]] QString lastExcelVbaImportPersistSummary() const;
    [[nodiscard]] QString lastExcelVbaImportPersistIssueCodes() const;
    [[nodiscard]] bool lastExcelVbaImportPersistTransactionCommitted() const noexcept;
    [[nodiscard]] bool lastExcelVbaImportPersistTradeLogWritten() const noexcept;
    [[nodiscard]] bool lastExcelVbaImportPersistAuditLogWritten() const noexcept;
    [[nodiscard]] bool lastExcelVbaImportPersistDuplicateImportPrevented() const noexcept;
    [[nodiscard]] bool lastExcelVbaImportPersistIdempotencyConflictRejected() const noexcept;

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
    Q_INVOKABLE void resetPostWriteRefreshState();
    Q_INVOKABLE bool refreshManualEntryReadback();
    Q_INVOKABLE void resetExcelVbaImportPreviewState();
    Q_INVOKABLE bool previewExcelVbaImportReadOnly(const QString& importPayloadJson);
    Q_INVOKABLE bool previewExcelVbaImportReadOnlyFromLocalFile(const QString& fileUrlOrPath);
    Q_INVOKABLE void resetExcelVbaImportPersistState();
    Q_INVOKABLE bool persistAcceptedExcelVbaImportPreview();
    bool persistExcelVbaImportManualEntry(
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
        int fxRateFactCount);
    void reset();

signals:
    void tradingUiStateChanged();
    void manualEntryStateChanged();
    void postWriteRefreshStateChanged();
    void excelVbaImportPreviewStateChanged();
    void excelVbaImportPersistStateChanged();

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
    void applyExcelVbaImportPreviewResult(const ShellAccountingServiceResult& result);
    void applyExcelVbaImportPersistManualEntryResult(const ShellAccountingServiceResult& result);
    void refreshAfterManualEntryWrite(bool includePositionList, const char* reason);
    void markManualEntryInputError(const QString& message);
    void markExcelVbaImportPreviewInputError(const QString& message);
    void markExcelVbaImportPersistInputError(const QString& message);
    [[nodiscard]] ShellAccountingServiceRequest makeExcelVbaImportPreviewRequest(
        const QString& importPayloadJson,
        bool& valid);
    [[nodiscard]] ShellAccountingServiceRequest makeExcelVbaImportPersistManualEntryRequest(
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
        bool& valid);
    [[nodiscard]] QString acceptedExcelVbaImportPreviewIdempotencyKey() const;
    [[nodiscard]] QString acceptedExcelVbaImportPreviewDigest(
        const QString& importPayloadJson,
        bool& valid) const;
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
    bool postWriteRefreshEnabled_ = true;
    bool postWriteRefreshBusy_ = false;
    QString lastPostWriteRefreshStatus_ = QStringLiteral("READY");
    QString lastPostWriteRefreshIssue_;
    QString lastPostWriteRefreshSummary_;
    bool excelVbaImportPreviewBusy_ = false;
    QString lastExcelVbaImportPreviewStatus_ = QStringLiteral("READY");
    QString lastExcelVbaImportPreviewIssue_;
    QString lastExcelVbaImportPreviewSummary_;
    QString lastExcelVbaImportPreviewDiagnosticCodes_;
    int excelVbaImportPreviewTradeFactCount_ = 0;
    int excelVbaImportPreviewCashFactCount_ = 0;
    int excelVbaImportPreviewMarketPriceFactCount_ = 0;
    int excelVbaImportPreviewFxRateFactCount_ = 0;
    QString lastExcelVbaImportPreviewDigest_;
    QString lastExcelVbaImportPreviewPayloadJson_;
    QString lastExcelVbaImportPreviewSchemaVersion_;
    QString lastExcelVbaImportPreviewSource_;
    bool excelVbaImportPersistBusy_ = false;
    QString lastExcelVbaImportPersistStatus_ = QStringLiteral("READY");
    QString lastExcelVbaImportPersistIssue_;
    QString lastExcelVbaImportPersistSummary_;
    QString lastExcelVbaImportPersistIssueCodes_;
    bool lastExcelVbaImportPersistTransactionCommitted_ = false;
    bool lastExcelVbaImportPersistTradeLogWritten_ = false;
    bool lastExcelVbaImportPersistAuditLogWritten_ = false;
    bool lastExcelVbaImportPersistDuplicateImportPrevented_ = false;
    bool lastExcelVbaImportPersistIdempotencyConflictRejected_ = false;
    ShellAccountingServiceRequest lastDraftRequest_;
};

}  // namespace etfdt::shell_services
