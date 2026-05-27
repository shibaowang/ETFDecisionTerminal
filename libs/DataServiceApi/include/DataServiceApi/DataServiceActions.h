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
constexpr const char* kActionPositionList = "position.list";
constexpr const char* kActionCashSummary = "cash.summary";
constexpr const char* kActionPortfolioPnlSummary = "portfolio.pnl.summary";

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

[[nodiscard]] etfdt::protocol::ProtocolResponse handlePositionList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleCashSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] etfdt::protocol::ProtocolResponse handlePortfolioPnlSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection);

}  // namespace etfdt::data_service_api
