#pragma once

#include "DataAccess/DataAccess.h"
#include "ServiceRuntime/ServiceRuntime.h"

namespace etfdt::data_service_api {

constexpr const char* kActionDataHealth = "data.health";
constexpr const char* kActionDataSummary = "data.summary";
constexpr const char* kActionDataAccountsList = "data.accounts.list";
constexpr const char* kActionDataPortfoliosList = "data.portfolios.list";
constexpr const char* kActionDataInstrumentsList = "data.instruments.list";
constexpr const char* kActionDataStrategiesList = "data.strategies.list";
constexpr const char* kActionDataOtcList = "data.otc.list";
constexpr const char* kActionDataAuditAppend = "data.audit.append";
constexpr const char* kActionAccountingHealth = "accounting.health";
constexpr const char* kActionAccountingReplayPreview = "accounting.replay.preview";
constexpr const char* kActionAccountingPortfolioReplayReadOnlySummary =
    "accounting.portfolio_replay.readonly_summary";
constexpr const char* kActionAccountingExcelVbaImportReadOnlyPreview =
    "accounting.excel_vba_import.readonly_preview";
constexpr const char* kActionAccountingExcelVbaImportPersistManualEntry =
    "accounting.excel_vba_import.persist_manual_entry";
constexpr const char* kActionStrategyRecommendationReadOnlySummary =
    "strategy.recommendation.readonly_summary";
constexpr const char* kActionAccountingSnapshotWrite = "accounting.snapshot.write";
constexpr const char* kActionAccountingAuditWrite = "accounting.audit.write";
constexpr const char* kActionAccountingTradeDraftCreate = "accounting.tradedraft.create";
constexpr const char* kActionAccountingTradeDraftCreateFromRecommendation =
    "accounting.tradedraft.create_from_recommendation";
constexpr const char* kActionAccountingTradeDraftReadOnlySummary =
    "accounting.tradedraft.readonly_summary";
constexpr const char* kActionAccountingTradeDraftConfirm = "accounting.tradedraft.confirm";
constexpr const char* kActionAccountingBrokerOrderDryRun = "accounting.broker_order.dry_run";
constexpr const char* kActionAccountingManualTransactionCreate =
    "accounting." "manual_transaction.create";
constexpr const char* kActionAccountingManualCashMovementCreate =
    "accounting." "manual_cash_movement.create";
constexpr const char* kActionPositionList = "position.list";
constexpr const char* kActionCashSummary = "cash.summary";
constexpr const char* kActionPortfolioPnlSummary = "portfolio.pnl.summary";
constexpr const char* kActionBasePositionSummary = "base_position.summary";
constexpr const char* kActionSniperPoolSummary = "sniper_pool.summary";

[[nodiscard]] etfdt::protocol::ProtocolResponse handleDataHealth(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleDataSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleDataAccountsList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleDataPortfoliosList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleDataInstrumentsList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleDataStrategiesList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleDataOtcList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleDataAuditAppend(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingHealth(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingReplayPreview(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingPortfolioReplayReadOnlySummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingExcelVbaImportReadOnlyPreview(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingExcelVbaImportPersistManualEntry(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleStrategyRecommendationReadOnlySummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingSnapshotWrite(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingAuditWrite(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingTradeDraftCreate(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingTradeDraftCreateFromRecommendation(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingTradeDraftReadOnlySummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingTradeDraftConfirm(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingBrokerOrderDryRun(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingManualEntryTransactionCreate(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleAccountingManualEntryCashMovementCreate(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handlePositionList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleCashSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handlePortfolioPnlSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleBasePositionSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleSniperPoolSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

}  // namespace etfdt::data_service_api
