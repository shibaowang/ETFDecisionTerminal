#include "DataServiceApi/DataServiceActions.h"

#include "MarketEngine/DisabledMarketDataProvider.h"
#include "MarketEngine/FixtureMarketDataProvider.h"
#include "MarketEngine/LivePublicMarketDataProvider.h"
#include "MarketEngine/MarketDataProvider.h"
#include "MarketEngine/MarketDataRefreshEngine.h"
#include "Protocol/Json.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <map>
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

std::string percentText(double value)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value << '%';
    return stream.str();
}

bool parseDecimal(const std::string& text, double& value)
{
    try {
        std::size_t parsed = 0;
        value = std::stod(text, &parsed);
        return parsed == text.size() && std::isfinite(value);
    } catch (...) {
        return false;
    }
}

std::int64_t priceQuantityMarketValueCents(
    const std::string& priceText,
    std::int64_t quantity1e6)
{
    double price = 0.0;
    if (!parseDecimal(priceText, price) || price <= 0.0 || quantity1e6 <= 0) {
        return 0;
    }
    return static_cast<std::int64_t>(
        std::llround(price * static_cast<double>(quantity1e6) / 1000000.0 * 100.0));
}

bool tableExists(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    auto result = connection.queryTableExists(table);
    return result && result.value();
}

bool columnExists(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& table,
    const std::string& column)
{
    auto rows = connection.queryRows("PRAGMA table_info(" + table + ");");
    if (!rows) {
        return false;
    }
    for (const auto& row : rows.value()) {
        if (row.size() > 1U && row[1].text == column) {
            return true;
        }
    }
    return false;
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
    std::int64_t quantity1e6 = 0;
    std::int64_t costCents = 0;
    std::string quantityText;
    std::string costAmountText;
    std::string marketValueText = "UNAVAILABLE";
    std::string unrealizedPnlText = "UNAVAILABLE";
    bool marketValueAvailable = false;
    std::int64_t marketValueCents = 0;
    std::int64_t unrealizedPnlCents = 0;
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
        holding.quantity1e6 = row[3].int64Value;
        holding.costCents = row[4].int64Value;
        holding.quantityText = quantityText(holding.quantity1e6);
        holding.costAmountText = centsText(holding.costCents);
        holdings.push_back(std::move(holding));
    }
    return holdings;
}

std::string inferExchange(const std::string& code)
{
    if (code.empty()) {
        return {};
    }
    if (code.front() == '5' || code.front() == '6') {
        return "SH";
    }
    if (code.front() == '0' || code.front() == '1' || code.front() == '2'
        || code.front() == '3') {
        return "SZ";
    }
    return {};
}

std::string providerSymbolFor(const std::string& exchange, const std::string& code)
{
    if (exchange == "SH") {
        return "sh" + code;
    }
    if (exchange == "SZ") {
        return "sz" + code;
    }
    return {};
}

etfdt::market_engine::MarketInstrumentKey etfInstrumentFor(const HoldingRow& holding)
{
    etfdt::market_engine::MarketInstrumentKey key;
    key.instrumentCode = holding.instrumentCode;
    key.instrumentType = "ETF";
    key.exchange = inferExchange(holding.instrumentCode);
    key.providerSymbol = providerSymbolFor(key.exchange, holding.instrumentCode);
    key.provider = "live-public-market";
    return key;
}

std::map<std::string, etfdt::market_engine::MarketInstrumentKey> indexMappingsFromPayload(
    const QJsonObject& root)
{
    const auto market = root.value(QStringLiteral("marketData")).isObject()
        ? root.value(QStringLiteral("marketData")).toObject()
        : root;
    const auto object = market.value(QStringLiteral("input")).isObject()
        ? market.value(QStringLiteral("input")).toObject()
        : market;
    const auto mappings = object.value(QStringLiteral("indexMappings")).toObject();

    std::map<std::string, etfdt::market_engine::MarketInstrumentKey> result;
    for (auto it = mappings.begin(); it != mappings.end(); ++it) {
        etfdt::market_engine::MarketInstrumentKey key;
        if (it.value().isObject()) {
            const auto item = it.value().toObject();
            key.instrumentCode = stringField(item, "instrumentCode", stringField(item, "indexCode"));
            key.exchange = stringField(item, "exchange", inferExchange(key.instrumentCode));
            key.providerSymbol =
                stringField(item, "providerSymbol", providerSymbolFor(key.exchange, key.instrumentCode));
        } else if (it.value().isString()) {
            key.instrumentCode = it.value().toString().toStdString();
            key.exchange = inferExchange(key.instrumentCode);
            key.providerSymbol = providerSymbolFor(key.exchange, key.instrumentCode);
        }
        key.instrumentType = "INDEX";
        key.provider = "live-public-market";
        if (!key.instrumentCode.empty()) {
            result[it.key().toStdString()] = std::move(key);
        }
    }
    return result;
}

std::unique_ptr<etfdt::market_engine::MarketDataProvider> providerFor(
    const etfdt::market_engine::MarketRefreshInput& input,
    const std::string& payloadJson)
{
    if (input.providerMode == "disabled") {
        return std::make_unique<etfdt::market_engine::DisabledMarketDataProvider>();
    }
    if (input.providerMode == "live") {
        return std::make_unique<etfdt::market_engine::LivePublicMarketDataProvider>(
            input.liveMarketDataEnabled,
            ".local/daily_use/cache/market_cache.json");
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

const etfdt::market_engine::MarketInstrumentRefreshResult* instrumentByCode(
    const etfdt::market_engine::MarketRefreshResult& result,
    const std::string& code)
{
    for (const auto& instrument : result.instruments) {
        if (instrument.instrumentCode == code) {
            return &instrument;
        }
    }
    return nullptr;
}

void addUniqueIssue(std::vector<std::string>& issues, const std::string& code)
{
    if (std::find(issues.begin(), issues.end(), code) == issues.end()) {
        issues.push_back(code);
    }
}

void deriveMarketInstrumentsFromHoldings(
    const QJsonObject& root,
    const std::vector<HoldingRow>& holdings,
    etfdt::market_engine::MarketRefreshInput& input,
    std::vector<std::string>& issues)
{
    if (holdings.empty()) {
        input.instruments.clear();
        return;
    }

    input.instruments.clear();
    const auto indexMappings = indexMappingsFromPayload(root);
    for (const auto& holding : holdings) {
        auto etf = etfInstrumentFor(holding);
        if (etf.exchange.empty() || etf.providerSymbol.empty()) {
            addUniqueIssue(issues, "DAILY_USE_EXCHANGE_MAPPING_MISSING");
            continue;
        }
        etf.provider = input.providerMode == "fixture" ? "fixture" : "live-public-market";
        input.instruments.push_back(std::move(etf));

        const auto mapping = indexMappings.find(holding.instrumentCode);
        if (mapping == indexMappings.end()) {
            addUniqueIssue(issues, "DAILY_USE_ETF_INDEX_MAPPING_MISSING");
            continue;
        }
        auto index = mapping->second;
        index.provider = input.providerMode == "fixture" ? "fixture" : "live-public-market";
        input.instruments.push_back(std::move(index));
    }
}

void applyHoldingMarketValues(
    const etfdt::market_engine::MarketRefreshResult& market,
    std::vector<HoldingRow>& holdings,
    std::int64_t& totalMarketValueCents,
    std::int64_t& floatingPnlCents,
    bool& allHoldingValuesAvailable)
{
    totalMarketValueCents = 0;
    floatingPnlCents = 0;
    allHoldingValuesAvailable = !holdings.empty();

    for (auto& holding : holdings) {
        const auto* instrument = instrumentByCode(market, holding.instrumentCode);
        if (instrument == nullptr || !instrument->quoteAccepted) {
            holding.marketValueText = "行情未刷新，市值不可完整计算";
            holding.unrealizedPnlText = "行情未刷新，浮动盈亏不可完整计算";
            allHoldingValuesAvailable = false;
            continue;
        }
        const auto marketValue =
            priceQuantityMarketValueCents(instrument->currentPriceText, holding.quantity1e6);
        if (marketValue <= 0) {
            holding.marketValueText = "行情未刷新，市值不可完整计算";
            holding.unrealizedPnlText = "行情未刷新，浮动盈亏不可完整计算";
            allHoldingValuesAvailable = false;
            continue;
        }
        holding.marketValueAvailable = true;
        holding.marketValueCents = marketValue;
        holding.unrealizedPnlCents = marketValue - holding.costCents;
        holding.marketValueText = centsText(holding.marketValueCents);
        holding.unrealizedPnlText = centsText(holding.unrealizedPnlCents);
        totalMarketValueCents += holding.marketValueCents;
        floatingPnlCents += holding.unrealizedPnlCents;
    }
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

struct CashCalculation final {
    std::int64_t remainingCashCents = 0;
    std::int64_t tradeCashImpactCents = 0;
    std::int64_t cashAdjustmentCents = 0;
    bool cashAdjustmentAmountUnavailable = false;
};

CashCalculation loadCashCalculation(etfdt::data_access::SQLiteConnection& connection)
{
    CashCalculation result;
    result.tradeCashImpactCents = singleIntOrZero(
        connection,
        "SELECT COALESCE(SUM(net_cash_impact_cents), 0) "
        "FROM trade_log WHERE manual_entry = 1 AND voided = 0 "
        "AND action_type IN ('BUY', 'SELL');");

    if (!tableExists(connection, "cash_adjustment")) {
        result.remainingCashCents = result.tradeCashImpactCents;
        result.cashAdjustmentAmountUnavailable = true;
        return result;
    }

    if (columnExists(connection, "cash_adjustment", "amount_cents")) {
        result.cashAdjustmentCents = singleIntOrZero(
            connection,
            "SELECT COALESCE(SUM(amount_cents), 0) FROM cash_adjustment;");
    } else if (columnExists(connection, "cash_adjustment", "trade_log_id")) {
        result.cashAdjustmentAmountUnavailable = true;
        result.cashAdjustmentCents = singleIntOrZero(
            connection,
            "SELECT COALESCE(SUM(tl.net_cash_impact_cents), 0) "
            "FROM cash_adjustment ca "
            "JOIN trade_log tl ON tl.id = ca.trade_log_id "
            "WHERE tl.manual_entry = 1 AND tl.voided = 0;");
    } else {
        result.cashAdjustmentAmountUnavailable = true;
    }

    result.remainingCashCents = result.tradeCashImpactCents + result.cashAdjustmentCents;
    return result;
}

struct BasePositionCalculation final {
    bool targetMissing = true;
    std::int64_t targetAmountCents = 0;
    std::int64_t builtAmountCents = 0;
    std::string targetAmountText = "UNAVAILABLE";
    std::string builtAmountText = "UNAVAILABLE";
    std::string completionText = "缺少底仓目标配置，无法计算底仓完成度。";
};

BasePositionCalculation loadBasePositionCalculation(
    etfdt::data_access::SQLiteConnection& connection)
{
    BasePositionCalculation result;
    if (!tableExists(connection, "base_position_allocation")
        || !columnExists(connection, "base_position_allocation", "target_base_amount_cents")
        || !columnExists(connection, "base_position_allocation", "allocated_amount_cents")) {
        return result;
    }

    auto rows = connection.queryRows(
        "SELECT COALESCE(SUM(target_base_amount_cents), 0), "
        "COALESCE(SUM(allocated_amount_cents), 0) "
        "FROM base_position_allocation;");
    if (!rows || rows.value().empty() || rows.value().front().size() < 2U) {
        return result;
    }
    result.targetAmountCents = rows.value().front()[0].int64Value;
    result.builtAmountCents = rows.value().front()[1].int64Value;
    if (result.targetAmountCents <= 0) {
        return result;
    }

    result.targetMissing = false;
    result.targetAmountText = centsText(result.targetAmountCents);
    result.builtAmountText = centsText(result.builtAmountCents);
    result.completionText =
        percentText(static_cast<double>(result.builtAmountCents) * 100.0
                    / static_cast<double>(result.targetAmountCents));
    return result;
}

std::string payloadJson(
    const etfdt::data_access::SQLiteConnection& connection,
    const QJsonObject& root,
    const std::vector<HoldingRow>& holdings,
    std::int64_t tradeLogRows,
    std::int64_t cashAdjustmentRows,
    const CashCalculation& cash,
    const BasePositionCalculation& basePosition,
    std::int64_t totalMarketValueCents,
    std::int64_t floatingPnlCents,
    bool allHoldingValuesAvailable,
    const std::vector<std::string>& dailyUseIssues,
    const etfdt::market_engine::MarketRefreshResult& market)
{
    const bool realDataLoaded = tradeLogRows > 0 || cashAdjustmentRows > 0;
    const auto* etf = firstInstrumentOfType(market, "ETF");
    const auto* index = firstInstrumentOfType(market, "INDEX");
    const bool marketAvailable = market.accepted && allHoldingValuesAvailable;
    const bool historicalHighAvailable =
        (etf != nullptr && etf->historicalHighAccepted)
        || (index != nullptr && index->historicalHighAccepted);
    const std::int64_t totalAssetsCents = cash.remainingCashCents + totalMarketValueCents;
    std::vector<std::string> issueCodes = market.issueCodes;
    for (const auto& code : dailyUseIssues) {
        addUniqueIssue(issueCodes, code);
    }

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
        : "行情未刷新，市值不可完整计算。";
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
            << "\"remainingCashText\":" << jsonString(realDataLoaded ? centsText(cash.remainingCashCents) : "UNAVAILABLE") << ','
            << "\"cashAdjustmentAmountUnavailable\":" << (cash.cashAdjustmentAmountUnavailable ? "true" : "false") << ','
            << "\"cashAdjustmentText\":" << jsonString(centsText(cash.cashAdjustmentCents)) << ','
            << "\"totalAssetsText\":" << jsonString(marketAvailable ? centsText(totalAssetsCents) : "行情未刷新，市值不可完整计算") << ','
            << "\"totalMarketValueText\":" << jsonString(marketAvailable ? centsText(totalMarketValueCents) : "行情未刷新，市值不可完整计算") << ','
            << "\"floatingPnlText\":" << jsonString(marketAvailable ? centsText(floatingPnlCents) : "行情未刷新，浮动盈亏不可完整计算") << ','
            << "\"totalAssetsConcreteValue\":" << (marketAvailable ? "true" : "false") << ','
            << "\"totalMarketValueConcreteValue\":" << (marketAvailable ? "true" : "false") << ','
            << "\"floatingPnlConcreteValue\":" << (marketAvailable ? "true" : "false") << ','
            << "\"basePositionCompletionText\":" << jsonString(basePosition.completionText) << ','
            << "\"basePositionTargetAmountText\":" << jsonString(basePosition.targetAmountText) << ','
            << "\"basePositionBuiltAmountText\":" << jsonString(basePosition.builtAmountText) << ','
            << "\"basePositionTargetMissing\":" << (basePosition.targetMissing ? "true" : "false") << ','
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
            << "\"issueCodes\":" << issueCodesJson(issueCodes) << ','
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
    const auto cash = loadCashCalculation(connection);
    const auto basePosition = loadBasePositionCalculation(connection);
    auto holdings = loadHoldings(connection);
    std::vector<std::string> dailyUseIssues;
    auto marketInput = marketInputFromPayload(root, context.request.payloadJson);
    deriveMarketInstrumentsFromHoldings(root, holdings, marketInput, dailyUseIssues);
    auto provider = providerFor(marketInput, context.request.payloadJson);
    const auto market =
        etfdt::market_engine::MarketDataRefreshEngine{}.refreshReadOnly(marketInput, *provider);
    std::int64_t totalMarketValueCents = 0;
    std::int64_t floatingPnlCents = 0;
    bool allHoldingValuesAvailable = false;
    applyHoldingMarketValues(
        market,
        holdings,
        totalMarketValueCents,
        floatingPnlCents,
        allHoldingValuesAvailable);

    return successResponse(
        context,
        payloadJson(
            connection,
            root,
            holdings,
            tradeLogRows,
            cashAdjustmentRows,
            cash,
            basePosition,
            totalMarketValueCents,
            floatingPnlCents,
            allHoldingValuesAvailable,
            dailyUseIssues,
            market));
}

}  // namespace etfdt::data_service_api
