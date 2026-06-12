#include "DataServiceApi/DataServiceActionRegistrar.h"

#include "DataServiceApi/DataServiceActions.h"
#include "DataServiceApi/WriteActionPolicy.h"

namespace etfdt::data_service_api {

void registerDataServiceReadOnlyActions(
    etfdt::service_runtime::ActionDispatcher& dispatcher,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)dispatcher.registerAction(kActionDataHealth, [&connection](const auto& context) {
        return handleDataHealth(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataSummary, [&connection](const auto& context) {
        return handleDataSummary(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataAccountsList, [&connection](const auto& context) {
        return handleDataAccountsList(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataPortfoliosList, [&connection](const auto& context) {
        return handleDataPortfoliosList(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataInstrumentsList, [&connection](const auto& context) {
        return handleDataInstrumentsList(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataStrategiesList, [&connection](const auto& context) {
        return handleDataStrategiesList(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataOtcList, [&connection](const auto& context) {
        return handleDataOtcList(context, connection);
    });
    (void)dispatcher.registerAction(kActionAccountingHealth, [&connection](const auto& context) {
        return handleAccountingHealth(context, connection);
    });
    (void)dispatcher.registerAction(kActionAccountingReplayPreview, [&connection](const auto& context) {
        return handleAccountingReplayPreview(context, connection);
    });
    (void)dispatcher.registerAction(kActionAccountingPortfolioReplayReadOnlySummary, [&connection](const auto& context) {
        return handleAccountingPortfolioReplayReadOnlySummary(context, connection);
    });
    (void)dispatcher.registerAction(kActionAccountingExcelVbaImportReadOnlyPreview, [&connection](const auto& context) {
        return handleAccountingExcelVbaImportReadOnlyPreview(context, connection);
    });
    (void)dispatcher.registerAction(kActionStrategyRecommendationReadOnlySummary, [&connection](const auto& context) {
        return handleStrategyRecommendationReadOnlySummary(context, connection);
    });
    (void)dispatcher.registerAction(kActionAccountingManualTransactionCreate, [&connection](const auto& context) {
        return handleAccountingManualEntryTransactionCreate(context, connection);
    });
    (void)dispatcher.registerAction(kActionAccountingManualCashMovementCreate, [&connection](const auto& context) {
        return handleAccountingManualEntryCashMovementCreate(context, connection);
    });
    (void)dispatcher.registerAction(kActionPositionList, [&connection](const auto& context) {
        return handlePositionList(context, connection);
    });
    (void)dispatcher.registerAction(kActionCashSummary, [&connection](const auto& context) {
        return handleCashSummary(context, connection);
    });
    (void)dispatcher.registerAction(kActionPortfolioPnlSummary, [&connection](const auto& context) {
        return handlePortfolioPnlSummary(context, connection);
    });
    (void)dispatcher.registerAction(kActionBasePositionSummary, [&connection](const auto& context) {
        return handleBasePositionSummary(context, connection);
    });
    (void)dispatcher.registerAction(kActionSniperPoolSummary, [&connection](const auto& context) {
        return handleSniperPoolSummary(context, connection);
    });
}

void registerDataServiceWriteActions(
    etfdt::service_runtime::ActionDispatcher& dispatcher,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (isWriteActionAllowed(kActionDataAuditAppend)) {
        (void)dispatcher.registerAction(kActionDataAuditAppend, [&connection](const auto& context) {
            return handleDataAuditAppend(context, connection);
        });
    }
    if (isWriteActionAllowed(kActionAccountingSnapshotWrite)) {
        (void)dispatcher.registerAction(kActionAccountingSnapshotWrite, [&connection](const auto& context) {
            return handleAccountingSnapshotWrite(context, connection);
        });
    }
    if (isWriteActionAllowed(kActionAccountingAuditWrite)) {
        (void)dispatcher.registerAction(kActionAccountingAuditWrite, [&connection](const auto& context) {
            return handleAccountingAuditWrite(context, connection);
        });
    }
    if (isWriteActionAllowed(kActionAccountingExcelVbaImportPersistManualEntry)) {
        (void)dispatcher.registerAction(
            kActionAccountingExcelVbaImportPersistManualEntry,
            [&connection](const auto& context) {
                return handleAccountingExcelVbaImportPersistManualEntry(context, connection);
            });
    }
    if (isWriteActionAllowed(kActionAccountingTradeDraftCreate)) {
        (void)dispatcher.registerAction(kActionAccountingTradeDraftCreate, [&connection](const auto& context) {
            return handleAccountingTradeDraftCreate(context, connection);
        });
    }
    if (isWriteActionAllowed(kActionAccountingTradeDraftConfirm)) {
        (void)dispatcher.registerAction(kActionAccountingTradeDraftConfirm, [&connection](const auto& context) {
            return handleAccountingTradeDraftConfirm(context, connection);
        });
    }
    if (isWriteActionAllowed(kActionAccountingBrokerOrderDryRun)) {
        (void)dispatcher.registerAction(kActionAccountingBrokerOrderDryRun, [&connection](const auto& context) {
            return handleAccountingBrokerOrderDryRun(context, connection);
        });
    }
}

}  // namespace etfdt::data_service_api
