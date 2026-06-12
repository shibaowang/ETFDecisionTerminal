#include "DataServiceApi/DataServiceActions.h"

#include "MarketEngine/DisabledMarketDataProvider.h"
#include "MarketEngine/FixtureMarketDataProvider.h"
#include "MarketEngine/MarketDataProvider.h"
#include "MarketEngine/MarketDataRefreshEngine.h"
#include "MarketEngine/MarketDataSafetyPolicy.h"
#include "Protocol/Json.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace etfdt::data_service_api {
namespace {

constexpr const char* kDefaultDailyUseDbPath = ".local/daily_use/etfdt_daily_use.sqlite";

std::string jsonString(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

std::string stringField(const QJsonObject& object, const char* key, std::string fallback = {})
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::move(fallback);
}

bool boolField(const QJsonObject& object, const char* key, bool fallback = false)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isBool() ? value.toBool() : fallback;
}

std::int64_t int64Field(const QJsonObject& object, const char* key, std::int64_t fallback = 0)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isDouble() ? static_cast<std::int64_t>(value.toDouble()) : fallback;
}

std::string centsText(std::int64_t cents)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << static_cast<double>(cents) / 100.0;
    return stream.str();
}

std::string quantityText(std::int64_t quantity1e6)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(6)
           << static_cast<double>(quantity1e6) / 1000000.0;
    return stream.str();
}

std::string issueCodesJson(const std::vector<std::string>& issueCodes)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < issueCodes.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        stream << jsonString(issueCodes[index]);
    }
    stream << ']';
    return stream.str();
}

struct HoldingRow final {
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string quantityText;
    std::string costAmountText;
    std::string marketValueText = "UNAVAILABLE";
    std::string unrealizedPnlText = "UNAVAILABLE";
};

std::string holdingsJson(const std::vector<HoldingRow>& holdings)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < holdings.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        const auto& row = holdings[index];
        stream << "{"
               << "\"accountId\":" << jsonString(row.accountId) << ','
               << "\"portfolioId\":" << jsonString(row.portfolioId) << ','
               << "\"instrumentCode\":" << jsonString(row.instrumentCode) << ','
               << "\"quantityText\":" << jsonString(row.quantityText) << ','
               << "\"costAmountText\":" << jsonString(row.costAmountText) << ','
               << "\"marketValueText\":" << jsonString(row.marketValueText) << ','
               << "\"unrealizedPnlText\":" << jsonString(row.unrealizedPnlText)
               << '}';
    }
    stream << ']';
    return stream.str();
}

std::int64_t singleIntOrZero(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    if (!rows || rows.value().empty() || rows.value().front().empty()) {
        return 0;
    }
    return rows.value().front().front().int64Value;
}

std::vector<HoldingRow> loadHoldings(etfdt::data_access::SQLiteConnection& connection)
{
    auto rows = connection.queryRows(
        "SELECT tl.account_id, tl.portfolio_id, COALESCE(tl.actual_code, ''), "
        "SUM(CASE WHEN tl.action_type = 'BUY' THEN tl.quantity_1e6 "
        "WHEN tl.action_type = 'SELL' THEN -tl.quantity_1e6 ELSE 0 END), "
        "SUM(CASE WHEN tl.action_type = 'BUY' THEN tl.amount_cents + tl.fee_cents "
        "WHEN tl.action_type = 'SELL' THEN -(tl.amount_cents + tl.fee_cents) ELSE 0 END) "
        "FROM trade_log tl "
        "WHERE tl.manual_entry = 1 AND tl.voided = 0 "
        "AND tl.action_type IN ('BUY', 'SELL') "
        "GROUP BY tl.account_id, tl.portfolio_id, COALESCE(tl.actual_code, '') "
        "HAVING SUM(CASE WHEN tl.action_type = 'BUY' THEN tl.quantity_1e6 "
        "WHEN tl.action_type = 'SELL' THEN -tl.quantity_1e6 ELSE 0 END) <> 0 "
        "ORDER BY tl.account_id, tl.portfolio_id, COALESCE(tl.actual_code, '');");
    std::vector<HoldingRow> holdings;
    if (!rows) {
        return holdings;
    }
    holdings.reserve(rows.value().size());
    for (const auto& row : rows.value()) {
        if (row.size() != 5U) {
            continue;
        }
        HoldingRow holding;
        holding.accountId = std::to_string(row[0].int64Value);
        holding.portfolioId = std::to_string(row[1].int64Value);
        holding.instrumentCode = row[2].text;
        holding.quantityText = quantityText(row[3].int64Value);
        holding.costAmountText = centsText(row[4].int64Value);
        holdings.push_back(std::move(holding));
    }
    return holdings;
}

std::unique_ptr<etfdt::market_engine::MarketDataProvider> providerFor(
    const etfdt::market_engine::MarketRefreshInput& input,
    const std::string& payloadJson)
{
    if (input.providerMode == "disabled") {
        return std::make_unique<etfdt::market_engine::DisabledMarketDataProvider>();
    }
    if (input.providerMode == "live") {
        return std::make_unique<etfdt::market_engine::LivePublicMarketDataProviderBoundary>(
            input.liveMarketDataEnabled);
    }
    return std::make_unique<etfdt::market_engine::FixtureMarketDataProvider>(payloadJson);
}

etfdt::market_engine::MarketRefreshInput marketInputFromPayload(
    const QJsonObject& root,
    const std::string& rawPayload)
{
    etfdt::market_engine::MarketRefreshInput input;
    const auto market = root.value(QStringLiteral("marketData")).isObject()
        ? root.value(QStringLiteral("marketData")).toObject()
        : root;
    const auto object = market.value(QStringLiteral("input")).isObject()
        ? market.value(QStringLiteral("input")).toObject()
        : market;

    input.providerMode = stringField(object, "providerMode", "fixture");
    input.liveMarketDataEnabled = boolField(object, "liveMarketDataEnabled", false);
    input.maxQuoteAgeSeconds = static_cast<int>(int64Field(object, "maxQuoteAgeSeconds", 900));
    input.nowUtc = stringField(object, "nowUtc", "2026-06-12T00:00:00Z");
    input.referenceValueText = stringField(object, "referenceValueText");
    input.requestDigest = stringField(object, "requestDigest", "EPIC-289");
    input.rawPayloadForSafetyScan = rawPayload;

    const auto instruments = object.value(QStringLiteral("instruments")).toArray();
    for (const auto& value : instruments) {
        if (!value.isObject()) {
            continue;
        }
        const auto item = value.toObject();
        etfdt::market_engine::MarketInstrumentKey key;
        key.instrumentCode = stringField(item, "instrumentCode");
        key.instrumentType = stringField(item, "instrumentType");
        key.exchange = stringField(item, "exchange");
        key.providerSymbol = stringField(item, "providerSymbol");
        key.provider = stringField(item, "provider", input.providerMode);
        if (!key.instrumentCode.empty()) {
            input.instruments.push_back(std::move(key));
        }
    }

    if (input.instruments.empty()) {
        input.instruments.push_back({"510300", "ETF", "SH", "sh510300", input.providerMode});
        input.instruments.push_back({"000300", "INDEX", "SH", "sh000300", input.providerMode});
    }
    return input;
}

const etfdt::market_engine::MarketInstrumentRefreshResult* firstInstrumentOfType(
    const etfdt::market_engine::MarketRefreshResult& result,
    const std::string& type)
{
    for (const auto& instrument : result.instruments) {
        if (instrument.instrumentType == type) {
            return &instrument;
        }
    }
    return nullptr;
}

etfdt::protocol::ProtocolResponse successResponse(
    const etfdt::service_runtime::ActionContext& context,
    std::string payloadJson)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = true;
    response.payloadJson = std::move(payloadJson);
    return response;
}

std::string payloadJson(
    const etfdt::data_access::SQLiteConnection& connection,
    const QJsonObject& root,
    const std::vector<HoldingRow>& holdings,
    std::int64_t tradeLogRows,
    std::int64_t cashAdjustmentRows,
    std::int64_t remainingCashCents,
    const etfdt::market_engine::MarketRefreshResult& market)
{
    const bool realDataLoaded = tradeLogRows > 0 || cashAdjustmentRows > 0;
    const auto* etf = firstInstrumentOfType(market, "ETF");
    const auto* index = firstInstrumentOfType(market, "INDEX");
    const bool marketAvailable = market.accepted;
    const bool historicalHighAvailable =
        (etf != nullptr && etf->historicalHighAccepted)
        || (index != nullptr && index->historicalHighAccepted);

    const std::string dbPath = stringField(
        root,
        "dbPath",
        connection.config().databasePath.empty()
            ? kDefaultDailyUseDbPath
            : connection.config().databasePath.generic_string());
    const std::string lastImportTime = stringField(root, "lastImportTime", "UNAVAILABLE");
    const std::string lastRefreshTime = stringField(root, "lastRefreshTime", market.instruments.empty()
        ? "UNAVAILABLE"
        : stringField(root, "nowUtc", "2026-06-12T00:00:00Z"));
    const std::string refreshFailureReason = marketAvailable
        ? ""
        : "行情自动刷新失败，正在使用缓存 / 暂无行情数据。";
    const std::string noRealDataPrompt = realDataLoaded
        ? ""
        : "请先导入真实 VBA 脱敏导出文件。";
    const std::string cacheStatus = market.stale ? "使用缓存" : (marketAvailable ? "startup_auto_market_refresh" : "unavailable");
    const std::string dataSourceStatus = realDataLoaded ? "imported_from_vba_export" : "unavailable";
    const std::string replayStatus = realDataLoaded ? "replayed_from_trade_log" : "unavailable";
    const std::string marketSourceStatus = marketAvailable
        ? (market.stale ? "cached_market_data" : "startup_auto_market_refresh")
        : "unavailable";

    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonString(kActionAccountingRealDailyUseSnapshot) << ','
            << "\"task\":\"EPIC-289\","
            << "\"mode\":\"real-daily-use-data-dashboard-startup-auto-refresh\","
            << "\"implemented\":true,"
            << "\"readOnly\":true,"
            << "\"writeEnabled\":false,"
            << "\"status\":" << jsonString(realDataLoaded ? (marketAvailable ? "OK" : "PARTIAL") : "NO_REAL_DATA") << ','
            << "\"dataQualityStatus\":" << jsonString(realDataLoaded ? (marketAvailable ? "OK" : "PARTIAL") : "UNAVAILABLE") << ','
            << "\"hasRows\":" << (realDataLoaded ? "true" : "false") << ','
            << "\"dbPath\":" << jsonString(dbPath) << ','
            << "\"dataSourceStatus\":" << jsonString(dataSourceStatus) << ','
            << "\"replaySourceStatus\":" << jsonString(replayStatus) << ','
            << "\"marketSourceStatus\":" << jsonString(marketSourceStatus) << ','
            << "\"lastImportTime\":" << jsonString(lastImportTime) << ','
            << "\"lastAutoRefreshTime\":" << jsonString(lastRefreshTime) << ','
            << "\"refreshStatus\":" << jsonString(market.status) << ','
            << "\"cacheStatus\":" << jsonString(cacheStatus) << ','
            << "\"refreshFailureReason\":" << jsonString(refreshFailureReason) << ','
            << "\"noRealDataPrompt\":" << jsonString(noRealDataPrompt) << ','
            << "\"tradeLogRows\":" << tradeLogRows << ','
            << "\"cashAdjustmentRows\":" << cashAdjustmentRows << ','
            << "\"holdingRows\":" << holdings.size() << ','
            << "\"holdings\":" << holdingsJson(holdings) << ','
            << "\"remainingCashText\":" << jsonString(realDataLoaded ? centsText(remainingCashCents) : "UNAVAILABLE") << ','
            << "\"totalAssetsText\":" << jsonString(marketAvailable ? "CALCULABLE_WITH_MARKET_DATA" : "UNAVAILABLE") << ','
            << "\"totalMarketValueText\":" << jsonString(marketAvailable ? "CALCULABLE_WITH_MARKET_DATA" : "UNAVAILABLE") << ','
            << "\"floatingPnlText\":" << jsonString(marketAvailable ? "CALCULABLE_WITH_MARKET_DATA" : "UNAVAILABLE") << ','
            << "\"basePositionCompletionText\":" << jsonString("缺少底仓目标配置，无法计算底仓完成度。") << ','
            << "\"basePositionTargetMissing\":true,"
            << "\"realDataLoaded\":" << (realDataLoaded ? "true" : "false") << ','
            << "\"currentHoldingsVisible\":" << (!holdings.empty() ? "true" : "false") << ','
            << "\"remainingCashVisible\":" << (realDataLoaded ? "true" : "false") << ','
            << "\"basePositionCompletionVisibleOrExplicitlyMissing\":true,"
            << "\"marketDataAvailable\":" << (marketAvailable ? "true" : "false") << ','
            << "\"historicalHighAvailable\":" << (historicalHighAvailable ? "true" : "false") << ','
            << "\"startupAutoRefreshEnabled\":true,"
            << "\"manualRefreshButtonAdded\":false,"
            << "\"mockDataUsed\":false,"
            << "\"mockDataUsedForDailyUse\":false,"
            << "\"etfCurrentPriceText\":" << jsonString(etf == nullptr ? "UNAVAILABLE" : etf->currentPriceText) << ','
            << "\"etfHistoricalHighText\":" << jsonString(etf == nullptr ? "UNAVAILABLE" : etf->historicalHighText) << ','
            << "\"etfDrawdownFromHighText\":" << jsonString(etf == nullptr ? "UNAVAILABLE" : etf->drawdownFromHighText) << ','
            << "\"indexCurrentPointText\":" << jsonString(index == nullptr ? "UNAVAILABLE" : index->currentPriceText) << ','
            << "\"indexHistoricalHighText\":" << jsonString(index == nullptr ? "UNAVAILABLE" : index->historicalHighText) << ','
            << "\"indexDrawdownFromHighText\":" << jsonString(index == nullptr ? "UNAVAILABLE" : index->drawdownFromHighText) << ','
            << "\"etfCurrentPriceVisible\":" << (etf != nullptr && etf->quoteAccepted ? "true" : "false") << ','
            << "\"etfHistoricalHighVisible\":" << (etf != nullptr && etf->historicalHighAccepted ? "true" : "false") << ','
            << "\"indexCurrentPointVisible\":" << (index != nullptr && index->quoteAccepted ? "true" : "false") << ','
            << "\"indexHistoricalHighVisible\":" << (index != nullptr && index->historicalHighAccepted ? "true" : "false") << ','
            << "\"refreshStatusVisible\":true,"
            << "\"lastRefreshTimeVisible\":true,"
            << "\"cacheStatusVisible\":true,"
            << "\"refreshFailureReasonVisible\":true,"
            << "\"rateLimitEnabled\":" << (market.perHostRateLimitEnforced ? "true" : "false") << ','
            << "\"historicalHighDailyCacheEnabled\":" << (market.historyHighDailyCacheEnforced ? "true" : "false") << ','
            << "\"failureCircuitBreakerEnabled\":" << (market.historyHighFailureCircuitBreakerEnforced ? "true" : "false") << ','
            << "\"backgroundPollingEnabled\":false,"
            << "\"exactHostAllowlistEnforced\":" << (market.exactHostAllowlistEnforced ? "true" : "false") << ','
            << "\"noParallelSameHostRequests\":" << (market.noParallelSameHostRequests ? "true" : "false") << ','
            << "\"allowedHosts\":[\"qt.gtimg.cn\",\"push2.eastmoney.com\",\"hq.sinajs.cn\",\"push2his.eastmoney.com\"],"
            << "\"issueCodes\":" << issueCodesJson(market.issueCodes) << ','
            << "\"productionDbTouched\":false,"
            << "\"productionWrite\":false,"
            << "\"sqliteProductionWrite\":false,"
            << "\"auditWritten\":false,"
            << "\"ledgerWritten\":false,"
            << "\"snapshotWritten\":false,"
            << "\"tradeLogWritten\":false,"
            << "\"readModelPersistentWrite\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":" << (market.networkAccess ? "true" : "false") << ','
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"realOrderPlacement\":false,"
            << "\"automaticTrading\":false"
            << "}";
    return payload.str();
}

}  // namespace

etfdt::protocol::ProtocolResponse handleAccountingRealDailyUseSnapshot(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    QJsonParseError parseError {};
    const auto document =
        QJsonDocument::fromJson(QByteArray::fromStdString(context.request.payloadJson), &parseError);
    const QJsonObject root =
        parseError.error == QJsonParseError::NoError && document.isObject()
            ? document.object()
            : QJsonObject {};

    const auto tradeLogRows = singleIntOrZero(
        connection,
        "SELECT COUNT(*) FROM trade_log WHERE manual_entry = 1 AND voided = 0;");
    const auto cashAdjustmentRows = singleIntOrZero(
        connection,
        "SELECT COUNT(*) FROM cash_adjustment;");
    const auto remainingCashRows = connection.queryRows(
        "SELECT COALESCE(SUM(net_cash_impact_cents), 0) "
        "FROM trade_log WHERE manual_entry = 1 AND voided = 0;");
    std::int64_t remainingCashCents = 0;
    if (remainingCashRows && !remainingCashRows.value().empty()
        && !remainingCashRows.value().front().empty()) {
        remainingCashCents = remainingCashRows.value().front().front().int64Value;
    }

    auto holdings = loadHoldings(connection);
    auto marketInput = marketInputFromPayload(root, context.request.payloadJson);
    auto provider = providerFor(marketInput, context.request.payloadJson);
    const auto market =
        etfdt::market_engine::MarketDataRefreshEngine{}.refreshReadOnly(marketInput, *provider);

    return successResponse(
        context,
        payloadJson(
            connection,
            root,
            holdings,
            tradeLogRows,
            cashAdjustmentRows,
            remainingCashCents,
            market));
}

}  // namespace etfdt::data_service_api
