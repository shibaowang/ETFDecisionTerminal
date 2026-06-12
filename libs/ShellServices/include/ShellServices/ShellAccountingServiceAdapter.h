#pragma once

#include "ShellServices/ShellAccountingServiceTypes.h"

namespace etfdt::shell_services {

class ShellAccountingServiceAdapter {
public:
    virtual ~ShellAccountingServiceAdapter() = default;

    virtual ShellAccountingServiceResult fetchPositionList(
        const ShellAccountingServiceRequest& request) = 0;
    virtual ShellAccountingServiceResult fetchCashSummary(
        const ShellAccountingServiceRequest& request) = 0;
    virtual ShellAccountingServiceResult fetchPortfolioPnlSummary(
        const ShellAccountingServiceRequest& request) = 0;
    virtual ShellAccountingServiceResult fetchBasePositionSummary(
        const ShellAccountingServiceRequest& request) = 0;
    virtual ShellAccountingServiceResult fetchSniperPoolSummary(
        const ShellAccountingServiceRequest& request) = 0;
    virtual ShellAccountingServiceResult createDraft(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult createDraftFromRecommendation(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult fetchTradeDraftReadOnlySummary(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult confirmDraft(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult submitManualTransaction(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult submitManualCashMovement(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult previewExcelVbaImportReadOnly(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult persistExcelVbaImportManualEntry(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult fetchPortfolioReplayReadOnlySummary(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult fetchStrategyRecommendationReadOnlySummary(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult previewOtcMapMultiChannelDraft(
        const ShellAccountingServiceRequest& request);
    virtual ShellAccountingServiceResult createOtcMapMultiChannelTradeDraft(
        const ShellAccountingServiceRequest& request);
};

}  // namespace etfdt::shell_services
