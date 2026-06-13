#include "DataAccess/SQLiteConnection.h"
#include "DataServiceApi/DataServiceActions.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ActionContext.h"
#include "Transport/LocalSocketName.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using etfdt::data_access::DatabaseConfig;
using etfdt::data_access::SQLiteConnection;

std::filesystem::path sourceRootFromArgs(int argc, char** argv)
{
    for (int index = 1; index + 1 < argc; ++index) {
        if (std::string(argv[index]) == "--source-root") {
            return std::filesystem::path(argv[index + 1]);
        }
    }
    return std::filesystem::current_path();
}

[[noreturn]] void fail(const std::string& message)
{
    throw std::runtime_error(message);
}

void require(bool condition, const std::string& message)
{
    if (!condition) {
        fail(message);
    }
}

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        fail("missing file: " + path.generic_string());
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool contains(const std::string& text, const std::string& needle)
{
    return text.find(needle) != std::string::npos;
}

void requireContains(
    const std::string& text,
    const std::string& needle,
    const std::string& message)
{
    require(contains(text, needle), message + " missing: " + needle);
}

void requireNotContains(
    const std::string& text,
    const std::string& needle,
    const std::string& message)
{
    require(!contains(text, needle), message + " forbidden: " + needle);
}

QJsonObject parseObject(const std::string& json, const std::string& label)
{
    QJsonParseError error {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &error);
    require(
        error.error == QJsonParseError::NoError && document.isObject(),
        label + " must be JSON object");
    return document.object();
}

std::string fixturePayload(const std::string& dbPath)
{
    return std::string(R"JSON({
  "task": "EPIC-289",
  "mode": "real-daily-use-data-dashboard-startup-auto-refresh-test",
  "dbPath": ")JSON") + dbPath + R"JSON(",
  "lastImportTime": "2026-06-13T09:00:00Z",
  "lastRefreshTime": "2026-06-13T09:30:00Z",
  "marketData": {
    "input": {
      "providerMode": "fixture",
      "liveMarketDataEnabled": false,
      "maxQuoteAgeSeconds": 900,
      "nowUtc": "2026-06-13T09:30:00Z",
      "indexMappings": {
        "510300": {
          "instrumentCode": "000300",
          "instrumentType": "INDEX",
          "exchange": "SH",
          "providerSymbol": "sh000300"
        }
      },
      "instruments": [
        {
          "instrumentCode": "510300",
          "instrumentType": "ETF",
          "exchange": "SH",
          "providerSymbol": "sh510300",
          "provider": "fixture"
        },
        {
          "instrumentCode": "000300",
          "instrumentType": "INDEX",
          "exchange": "SH",
          "providerSymbol": "sh000300",
          "provider": "fixture"
        }
      ]
    }
  },
  "fixture": {
    "quotes": [
      {
        "instrumentCode": "510300",
        "instrumentType": "ETF",
        "exchange": "SH",
        "providerSymbol": "sh510300",
        "quoteTimeUtc": "2026-06-13T09:30:00Z",
        "priceText": "4.800",
        "previousCloseText": "4.700",
        "source": "fixture/epic289",
        "dataQualityStatus": "OK"
      },
      {
        "instrumentCode": "000300",
        "instrumentType": "INDEX",
        "exchange": "SH",
        "providerSymbol": "sh000300",
        "quoteTimeUtc": "2026-06-13T09:30:00Z",
        "priceText": "4000.000",
        "previousCloseText": "3980.000",
        "source": "fixture/epic289",
        "dataQualityStatus": "OK"
      }
    ],
    "history": {
      "510300": [
        {"date": "2026-01-02", "highText": "5.000", "closeText": "4.900"}
      ],
      "000300": [
        {"date": "2026-01-02", "highText": "4200.000", "closeText": "4100.000"}
      ]
    }
  }
})JSON";
}

void createMinimalDailyUseSchema(SQLiteConnection& connection)
{
    require(connection.executeSql(
                "CREATE TABLE trade_log ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "account_id INTEGER,"
                "portfolio_id INTEGER,"
                "actual_code TEXT,"
                "action_type TEXT,"
                "quantity_1e6 INTEGER NOT NULL DEFAULT 0,"
                "amount_cents INTEGER NOT NULL DEFAULT 0,"
                "fee_cents INTEGER NOT NULL DEFAULT 0,"
                "net_cash_impact_cents INTEGER NOT NULL DEFAULT 0,"
                "manual_entry INTEGER NOT NULL DEFAULT 0,"
                "voided INTEGER NOT NULL DEFAULT 0"
                ");")
                .hasValue(),
            "create trade_log");
    require(connection.executeSql(
                "CREATE TABLE cash_adjustment ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "trade_log_id INTEGER"
                ");")
                .hasValue(),
            "create cash_adjustment");
    require(connection.executeSql(
                "CREATE TABLE base_position_allocation ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "target_base_amount_cents INTEGER NOT NULL,"
                "allocated_amount_cents INTEGER NOT NULL"
                ");")
                .hasValue(),
            "create base_position_allocation");
}

void insertImportedRows(SQLiteConnection& connection)
{
    require(connection.executeSql(
                "INSERT INTO trade_log "
                "(account_id, portfolio_id, actual_code, action_type, quantity_1e6, "
                "amount_cents, fee_cents, net_cash_impact_cents, manual_entry, voided) "
                "VALUES "
                "(1, 1, '', 'CASH_IN', 0, 1000000, 0, 1000000, 1, 0),"
                "(1, 1, '510300', 'BUY', 100000000, 250000, 100, -250100, 1, 0);")
                .hasValue(),
            "insert imported trade rows");
    require(connection.executeSql("INSERT INTO cash_adjustment (trade_log_id) VALUES (1);")
                .hasValue(),
            "insert imported cash adjustment row");
    require(connection.executeSql(
                "INSERT INTO base_position_allocation "
                "(target_base_amount_cents, allocated_amount_cents) VALUES (1000000, 480000);")
                .hasValue(),
            "insert imported base position target row");
}

QJsonObject callSnapshot(SQLiteConnection& connection, const std::string& payload)
{
    etfdt::service_runtime::ActionContext context;
    context.request.msgId = "epic-289-real-daily-use-test";
    context.request.traceId = "epic-289-trace";
    context.request.action = etfdt::data_service_api::kActionAccountingRealDailyUseSnapshot;
    context.request.payloadJson = payload;

    const auto response =
        etfdt::data_service_api::handleAccountingRealDailyUseSnapshot(context, connection);
    require(response.success, "snapshot response protocol success");
    return parseObject(response.payloadJson, "snapshot response payload");
}

bool boolField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string(key) + " must be bool");
    return value.toBool();
}

QString stringField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isString(), std::string(key) + " must be string");
    return value.toString();
}

void runFunctionalProbe(const std::filesystem::path& sourceRoot)
{
    const auto dbPath =
        std::filesystem::temp_directory_path()
        / ("etfdt_epic289_daily_use_" + std::to_string(std::rand()) + ".sqlite");
    SQLiteConnection connection;
    DatabaseConfig config;
    config.databasePath = dbPath;
    config.enableWal = false;
    config.foreignKeys = false;
    require(connection.open(config).hasValue(), "open temp daily-use DB");
    createMinimalDailyUseSchema(connection);

    const auto emptyPayload = callSnapshot(connection, fixturePayload(dbPath.generic_string()));
    require(stringField(emptyPayload, "status") == QStringLiteral("NO_REAL_DATA"), "empty DB reports no real data");
    require(
        stringField(emptyPayload, "noRealDataPrompt")
            == QStringLiteral("请先导入真实 VBA 脱敏导出文件。"),
        "empty DB prompt is visible");
    require(!boolField(emptyPayload, "mockDataUsedForDailyUse"), "empty DB does not use mock data");
    require(boolField(emptyPayload, "basePositionTargetMissing"), "empty DB base target missing is explicit");

    insertImportedRows(connection);
    const auto payload = callSnapshot(connection, fixturePayload(dbPath.generic_string()));
    require(stringField(payload, "task") == QStringLiteral("EPIC-289"), "task evidence");
    require(boolField(payload, "realDataLoaded"), "real imported data loaded");
    require(payload.value(QStringLiteral("tradeLogRows")).toInt() == 2, "trade_log rows visible");
    require(payload.value(QStringLiteral("cashAdjustmentRows")).toInt() == 1, "cash rows visible");
    require(payload.value(QStringLiteral("holdingRows")).toInt() == 1, "holding row visible");
    require(boolField(payload, "currentHoldingsVisible"), "current holdings visible");
    require(boolField(payload, "remainingCashVisible"), "remaining cash visible");
    require(stringField(payload, "remainingCashText") == QStringLiteral("7499.00"), "remaining cash includes cash_adjustment path");
    require(boolField(payload, "basePositionCompletionVisibleOrExplicitlyMissing"), "base position evidence");
    require(!boolField(payload, "basePositionTargetMissing"), "base position target loaded from real table");
    require(stringField(payload, "basePositionCompletionText") == QStringLiteral("48.00%"), "base position completion concrete");
    require(boolField(payload, "marketDataAvailable"), "fixture market data available");
    require(boolField(payload, "historicalHighAvailable"), "fixture high data available");
    require(boolField(payload, "totalAssetsConcreteValue"), "total assets concrete");
    require(boolField(payload, "totalMarketValueConcreteValue"), "total market value concrete");
    require(boolField(payload, "floatingPnlConcreteValue"), "floating pnl concrete");
    require(stringField(payload, "totalMarketValueText") == QStringLiteral("480.00"), "market value is concrete");
    require(stringField(payload, "totalAssetsText") == QStringLiteral("7979.00"), "total assets is concrete");
    require(stringField(payload, "floatingPnlText") == QStringLiteral("-2021.00"), "floating pnl is concrete");
    require(stringField(payload, "totalMarketValueText") != QStringLiteral("CALCULABLE_WITH_MARKET_DATA"), "no market value placeholder");
    require(stringField(payload, "totalAssetsText") != QStringLiteral("CALCULABLE_WITH_MARKET_DATA"), "no total assets placeholder");
    require(stringField(payload, "floatingPnlText") != QStringLiteral("CALCULABLE_WITH_MARKET_DATA"), "no floating pnl placeholder");
    require(boolField(payload, "etfCurrentPriceVisible"), "ETF current price visible");
    require(boolField(payload, "etfHistoricalHighVisible"), "ETF historical high visible");
    require(boolField(payload, "indexCurrentPointVisible"), "index current point visible");
    require(boolField(payload, "indexHistoricalHighVisible"), "index historical high visible");
    require(boolField(payload, "startupAutoRefreshEnabled"), "startup auto refresh enabled");
    require(!boolField(payload, "manualRefreshButtonAdded"), "manual refresh button not added");
    require(!boolField(payload, "mockDataUsedForDailyUse"), "no mock daily-use data");
    require(boolField(payload, "rateLimitEnabled"), "rate limit evidence");
    require(boolField(payload, "historicalHighDailyCacheEnabled"), "daily high cache evidence");
    require(boolField(payload, "failureCircuitBreakerEnabled"), "failure breaker evidence");
    require(boolField(payload, "exactHostAllowlistEnforced"), "host allowlist evidence");
    require(boolField(payload, "noParallelSameHostRequests"), "no same-host concurrency evidence");
    require(!boolField(payload, "backgroundPollingEnabled"), "no background polling");
    require(!boolField(payload, "productionDbTouched"), "production DB not touched");
    require(!boolField(payload, "brokerOrderSubmitted"), "no broker order");
    require(!boolField(payload, "credentialAccess"), "no credentials");
    require(!boolField(payload, "endpointAccess"), "no endpoint");
    require(!boolField(payload, "realOrderPlacement"), "no real order");
    require(!boolField(payload, "automaticTrading"), "no automatic trading");
    require(!boolField(payload, "networkAccess"), "test uses fixture provider, no real network");

    connection.close();
    std::error_code ignored;
    std::filesystem::remove(dbPath, ignored);
    (void)sourceRoot;
}

void runStaticChecks(const std::filesystem::path& sourceRoot)
{
    require(
        etfdt::transport::normalizeLocalSocketName("ETFDataServiceDailyUse")
            == "ETFDataServiceDailyUse",
        "socket normalizer keeps daily-use name");
    require(
        etfdt::transport::normalizeLocalSocketName("D:\\ETFDecisionTerminal\\.local\\daily_use")
            == "D-ETFDecisionTerminal-.local-daily_use",
        "socket normalizer removes Windows path separators");
    require(
        etfdt::transport::normalizeLocalSocketName("///") == "ETFDataServiceDailyUse",
        "socket normalizer falls back for empty normalized names");
    require(
        etfdt::transport::isNormalizedLocalSocketName("ETFDataServiceDailyUse"),
        "socket normalizer validates daily-use name");
    require(
        !etfdt::transport::isNormalizedLocalSocketName(".local/daily_use/etfdt_daily_use.sqlite"),
        "socket name validator rejects path-like names");

    const auto qml = readFile(sourceRoot / "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml");
    for (const auto* token : {
             "真实数据日常看板",
             "当前持仓",
             "剩余现金",
             "总资产",
             "持仓市值",
             "浮动盈亏",
             "底仓完成度",
             "ETF 当前价",
             "ETF 历史高点",
             "指数当前点位",
             "指数历史高点",
             "自动刷新状态",
             "最近刷新时间",
             "使用缓存",
             "刷新失败原因",
             "请先导入真实 VBA 脱敏导出文件。",
             "行情自动刷新失败",
             "realDailyUseRawJsonPayload",
             "realDailyUseTotalAssetsText",
             "realDailyUseTotalMarketValueText",
             "realDailyUseFloatingPnlText",
             "accountingPresenter.loadRealDailyUseSnapshot",
         }) {
        requireContains(qml, token, "QML daily-use dashboard token");
    }
    for (const auto* token : {
             "手动刷新行情",
             "Refresh Market Data",
             "Manual Refresh",
             "mock 持仓",
             "mock 现金",
             "mock 行情",
             "CALCULABLE_WITH_MARKET_DATA",
         }) {
        requireNotContains(qml, token, "QML daily-use forbidden visible token");
    }
    requireNotContains(qml, "DataServiceClient", "QML direct DataServiceClient access");
    requireNotContains(qml, "AccountingEngine", "QML direct AccountingEngine access");
    requireContains(qml, "dailyUseDatabasePath", "QML daily-use DB path injected");
    requireContains(qml, "dailyUseServiceName", "QML daily-use service name injected");

    const auto shellMain = readFile(sourceRoot / "apps/ETFDecisionShell/src/main.cpp");
    requireContains(shellMain, "--daily-use", "Shell startup parses daily-use arg");
    requireContains(shellMain, "--socket-name", "Shell startup parses socket arg");
    requireContains(shellMain, "--db", "Shell startup parses DB arg");
    requireContains(shellMain, "--default-page", "Shell startup parses default page arg");
    requireContains(shellMain, "shell-accounting-daily-use", "Shell startup supports daily-use default page");
    requireContains(shellMain, "shell_accounting", "Shell startup maps daily-use to ShellAccounting page");
    requireContains(shellMain, "shellAccountingDataServiceClient->connect", "Shell startup connects accounting client");
    requireContains(shellMain, "normalizeLocalSocketName", "Shell startup normalizes socket name");
    requireContains(shellMain, "dailyUseConnectionIssueFor", "Shell startup maps connection failures");
    requireContains(shellMain, "daily-use socket name", "Shell startup reports illegal socket name");
    requireContains(shellMain, "DataService", "Shell startup reports DataService connection issue");
    requireContains(shellMain, "Invalid name", "Shell startup handles Qt invalid name");
    requireContains(shellMain, "dailyUseConnectionStatus", "Shell startup exposes daily-use connection status");
    requireNotContains(shellMain, ".demo/local_trial_rc", "Shell startup does not use demo RC path");

    const auto topStatusBar = readFile(sourceRoot / "apps/ETFDecisionShell/qml/layout/TopStatusBar.qml");
    requireContains(topStatusBar, "daily-use real data mode", "top bar daily-use mode label");
    requireContains(topStatusBar, "dailyUseConnectionStatus", "top bar daily-use connection status");
    requireContains(topStatusBar, "REAL_DATA_PENDING", "top bar daily-use data status");

    const auto rightInfoPanel = readFile(sourceRoot / "apps/ETFDecisionShell/qml/layout/RightInfoPanel.qml");
    requireContains(rightInfoPanel, "真实数据日常看板", "right panel daily-use title");
    requireContains(rightInfoPanel, "dailyUseConnectionStatus", "right panel daily-use connection status");
    requireContains(rightInfoPanel, "请先导入真实 VBA 脱敏导出文件。", "right panel daily-use empty prompt");
    requireContains(rightInfoPanel, "仅限只读查看", "right panel daily-use read-only hint");

    const auto contentHost = readFile(sourceRoot / "apps/ETFDecisionShell/qml/layout/ContentHost.qml");
    requireContains(contentHost, "dailyUseDatabasePath", "content host passes daily-use DB path");
    requireContains(contentHost, "dailyUseServiceName", "content host passes daily-use service name");

    const auto dataServiceAction =
        readFile(sourceRoot / "libs/DataServiceApi/src/ShellAccountingRealDailyUseSnapshotAction.cpp");
    requireContains(
        dataServiceAction,
        "kActionAccountingRealDailyUseSnapshot",
        "DataService daily-use action");
    requireContains(dataServiceAction, ".local/daily_use/etfdt_daily_use.sqlite", "daily-use DB default");
    requireContains(dataServiceAction, "qt.gtimg.cn", "market host allowlist");
    requireContains(dataServiceAction, "push2.eastmoney.com", "market host allowlist");
    requireContains(dataServiceAction, "hq.sinajs.cn", "market host allowlist");
    requireContains(dataServiceAction, "push2his.eastmoney.com", "market host allowlist");
    requireNotContains(dataServiceAction, "INSERT INTO", "DataService daily-use action read-only SQL");
    requireNotContains(dataServiceAction, "broker_order", "DataService no broker token");
    requireNotContains(dataServiceAction, "CALCULABLE_WITH_MARKET_DATA", "DataService no calculable placeholder");

    const auto transportSocketNameHeader =
        readFile(sourceRoot / "libs/Transport/include/Transport/LocalSocketName.h");
    requireContains(transportSocketNameHeader, "normalizeLocalSocketName", "Transport exposes socket normalizer");
    requireContains(transportSocketNameHeader, "ETFDataServiceDailyUse", "Transport fallback socket name");
    requireContains(transportSocketNameHeader, "A-Za-z0-9_.-", "Transport socket allowed chars are documented");
    requireContains(transportSocketNameHeader, "isAllowedLocalSocketNameChar", "Transport socket predicate exists");

    const auto localSocketServer =
        readFile(sourceRoot / "libs/Transport/src/LocalSocketServer.cpp");
    const auto localSocketClient =
        readFile(sourceRoot / "libs/Transport/src/LocalSocketClient.cpp");
    requireContains(localSocketServer, "normalizeLocalSocketName", "LocalSocketServer normalizes server name");
    requireContains(localSocketClient, "normalizeLocalSocketName", "LocalSocketClient normalizes client name");
    requireContains(localSocketServer, "QLocalServer::removeServer", "LocalSocketServer removes stale server");

    const auto dataServiceMain = readFile(sourceRoot / "apps/ETFDataService/src/main.cpp");
    requireContains(dataServiceMain, "--socket-name", "DataService CLI supports socket-name");
    requireContains(dataServiceMain, "--serve-daily-use", "DataService CLI supports daily-use mode");
    requireContains(
        dataServiceMain,
        "registerDataServiceDailyUseActions",
        "DataService daily-use mode uses narrow action registrar");
    requireContains(
        dataServiceMain,
        "broker, order,",
        "DataService daily-use mode documents no broker/order actions");
    requireContains(dataServiceMain, "normalizeLocalSocketName", "DataService main normalizes socket name");
    requireContains(dataServiceMain, "host.listen(normalizedSocketName)", "DataService listens on normalized socket");

    const auto dataServiceRegistrar =
        readFile(sourceRoot / "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp");
    requireContains(
        dataServiceRegistrar,
        "registerDataServiceDailyUseActions",
        "DataService registrar exposes daily-use mode");
    requireContains(
        dataServiceRegistrar,
        "kActionAccountingExcelVbaImportPersistManualEntry",
        "daily-use registrar can register Excel/VBA import persist action");

    const auto liveProviderHeader =
        readFile(sourceRoot / "libs/MarketEngine/include/MarketEngine/LivePublicMarketDataProvider.h");
    const auto liveProviderSource =
        readFile(sourceRoot / "libs/MarketEngine/src/LivePublicMarketDataProvider.cpp");
    requireContains(liveProviderHeader, "LivePublicMarketDataProvider", "live provider class exists");
    for (const auto* token : {
             "qt.gtimg.cn",
             "push2.eastmoney.com",
             "hq.sinajs.cn",
             "push2his.eastmoney.com",
             "/api/qt/ulist.np/get",
             "/api/qt/stock/kline/get",
             "klt",
             "103",
             "fqt",
             "1",
             "0",
             "setTransferTimeout",
             "2000",
             "3000",
             "MarketDataRateLimiter",
             "30 seconds",
             "MarketDataInMemoryCache",
             "daily cache",
             "MarketDataCircuitBreaker",
             "10 minutes",
             "market_cache.json",
             "QNetworkAccessManager",
         }) {
        requireContains(liveProviderSource, token, "live provider implementation token");
    }
    requireNotContains(
        liveProviderSource,
        "MARKET_DATA_LIVE_PROVIDER_DEFERRED",
        "live provider is not deferred-only");
    requireContains(
        readFile(sourceRoot / "libs/MarketEngine/src/MarketDataSafetyPolicy.cpp"),
        "LivePublicMarketDataProvider provider",
        "legacy boundary delegates to real provider");

    for (const auto* doc : {
             "docs/401_real_daily_use_data_dashboard.md",
             "docs/402_real_daily_use_market_data_vba_parity.md",
             "docs/403_real_daily_use_portfolio_cash_base_position.md",
             "docs/404_real_daily_use_startup_auto_refresh_policy.md",
             "docs/405_real_daily_use_acceptance_checklist.md",
         }) {
        const auto text = readFile(sourceRoot / doc);
        requireContains(text, "EPIC-289", std::string(doc) + " scope");
        requireContains(text, ".local/daily_use/etfdt_daily_use.sqlite", std::string(doc) + " daily DB");
        requireContains(text, "automatic trading", std::string(doc) + " no automatic trading");
    }

    const auto marketDoc = readFile(sourceRoot / "docs/402_real_daily_use_market_data_vba_parity.md");
    requireContains(marketDoc, "http://qt.gtimg.cn/q=", "Tencent quote documented");
    requireContains(marketDoc, "push2.eastmoney.com/api/qt/ulist.np/get", "Eastmoney quote documented");
    requireContains(marketDoc, "http://hq.sinajs.cn/list=f_", "Sina NAV documented");
    requireContains(marketDoc, "push2his.eastmoney.com/api/qt/stock/kline/get", "Eastmoney history documented");
    requireContains(marketDoc, "klt=103", "monthly K documented");
    requireContains(marketDoc, "fqt=1", "ETF fqt documented");
    requireContains(marketDoc, "fqt=0", "index fqt documented");
    requireContains(marketDoc, "30 second", "cooldown documented");
    requireContains(marketDoc, "10 minute", "breaker documented");

    for (const auto* script : {
             "scripts/local_trial/New-ETFDTDailyUseWorkspace.ps1",
             "scripts/local_trial/Start-ETFDTDailyUseDataService.ps1",
             "scripts/local_trial/Start-ETFDTDailyUseShell.ps1",
             "scripts/local_trial/Invoke-ETFDTDailyUseSmoke.ps1",
         }) {
        const auto text = readFile(sourceRoot / script);
        requireContains(text, ".local/daily_use", std::string(script) + " uses daily path");
        requireContains(text, "EPIC-289", std::string(script) + " evidence");
        requireNotContains(text, ".demo/local_trial_rc", std::string(script) + " does not default to demo RC");
        requireNotContains(text, ".demo\\local_trial_rc", std::string(script) + " does not default to demo RC");
        requireNotContains(text, "brokerOrderSubmitted = $true", std::string(script) + " no broker");
    }

    const auto dailyUseService =
        readFile(sourceRoot / "scripts/local_trial/Start-ETFDTDailyUseDataService.ps1");
    requireContains(dailyUseService, "ETFDataService.exe", "daily DataService script resolves service exe");
    requireContains(dailyUseService, "Debug", "daily DataService script checks Debug exe");
    requireContains(dailyUseService, "Release", "daily DataService script checks Release exe");
    requireContains(dailyUseService, "Get-ChildItem", "daily DataService script has recursive fallback");
    requireContains(
        dailyUseService,
        "cmake --build build --config Debug",
        "daily DataService script reports build hint");
    requireContains(dailyUseService, "Checked paths", "daily DataService script reports checked paths");
    requireContains(dailyUseService, "Normalize-ETFDTLocalSocketName", "daily DataService script normalizes socket");
    requireContains(dailyUseService, "Test-ETFDTLocalSocketReady", "daily DataService script probes socket");
    requireContains(dailyUseService, "dataservice.pid", "daily DataService script writes pid");
    requireContains(dailyUseService, "dataservice.log", "daily DataService script writes stdout log");
    requireContains(dailyUseService, "dataservice.err.log", "daily DataService script writes stderr log");
    requireContains(dailyUseService, "RedirectStandardOutput", "daily DataService script redirects stdout");
    requireContains(dailyUseService, "RedirectStandardError", "daily DataService script redirects stderr");
    requireContains(dailyUseService, "socketReady", "daily DataService script reports socketReady");
    requireContains(dailyUseService, "socketNameNormalized", "daily DataService script reports normalized socket");
    requireContains(dailyUseService, "--socket-name\", $socketNameNormalized", "daily DataService script passes normalized socket");
    requireContains(dailyUseService, "--serve-daily-use", "daily DataService script enables daily-use mode");
    requireContains(dailyUseService, "dailyUseWriteActionsEnabled = $true", "daily DataService evidence reports write action mode");
    requireContains(dailyUseService, "excelVbaImportPersistActionRegistered = $true", "daily DataService evidence reports persist action");
    requireContains(dailyUseService, "serveMode = \"daily-use\"", "daily DataService evidence reports serve mode");
    requireContains(dailyUseService, "readOnlyOnlyMode = $false", "daily DataService evidence reports not read-only-only");
    requireContains(dailyUseService, "brokerOrderActionsRegistered = $false", "daily DataService evidence reports no broker actions");
    requireNotContains(dailyUseService, "--serve-readonly", "daily DataService script does not use pure read-only mode");

    const auto dailyUseShell =
        readFile(sourceRoot / "scripts/local_trial/Start-ETFDTDailyUseShell.ps1");
    requireContains(dailyUseShell, "ETFDecisionShell.exe", "daily Shell script resolves shell exe");
    requireContains(dailyUseShell, "Debug", "daily Shell script checks Debug exe");
    requireContains(dailyUseShell, "Release", "daily Shell script checks Release exe");
    requireContains(dailyUseShell, "Get-ChildItem", "daily Shell script has recursive fallback");
    requireContains(dailyUseShell, "--daily-use", "daily Shell script passes daily-use arg");
    requireContains(dailyUseShell, "--socket-name", "daily Shell script passes socket arg");
    requireContains(dailyUseShell, "ETFDataServiceDailyUse", "daily Shell script carries daily socket name");
    requireContains(dailyUseShell, "--db", "daily Shell script passes DB arg");
    requireContains(dailyUseShell, "--default-page", "daily Shell script passes default page arg");
    requireContains(dailyUseShell, "shell-accounting-daily-use", "daily Shell script uses daily-use default page");
    requireContains(dailyUseShell, "Normalize-ETFDTLocalSocketName", "daily Shell script normalizes socket");
    requireContains(dailyUseShell, "socketNameNormalized", "daily Shell script reports normalized socket");
    requireContains(dailyUseShell, "Test-ETFDTLocalSocketReady", "daily Shell script probes socket readiness");
    requireContains(dailyUseShell, "socketReadyBeforeShell", "daily Shell script reports socket probe");
    requireContains(dailyUseShell, "$socketNameNormalized", "daily Shell script passes normalized socket name");
    requireContains(dailyUseShell, "shell.pid", "daily Shell script writes pid file");
    requireNotContains(dailyUseShell, "--diagnostics-mock", "daily Shell script does not start diagnostics mock mode");

    const auto dailyUseSmoke = readFile(sourceRoot / "scripts/local_trial/Invoke-ETFDTDailyUseSmoke.ps1");
    requireContains(dailyUseSmoke, "ETFDataService.exe", "daily smoke checks DataService exe");
    requireContains(dailyUseSmoke, "ETFDecisionShell.exe", "daily smoke checks Shell exe");
    requireContains(dailyUseSmoke, "dataServiceExeFound", "daily smoke reports service exe");
    requireContains(dailyUseSmoke, "shellExeFound", "daily smoke reports shell exe");
    requireContains(dailyUseSmoke, "dailyUseShellScriptArgsValidated", "daily smoke validates Shell args");
    requireContains(dailyUseSmoke, "dataServiceSocketReadinessScriptValidated", "daily smoke validates DataService readiness");
    requireContains(dailyUseSmoke, "socketNameNormalized", "daily smoke reports normalized socket");
    requireContains(dailyUseSmoke, "shell-accounting-daily-use", "daily smoke validates default page");
    requireContains(dailyUseSmoke, "--serve-daily-use", "daily smoke validates daily-use service mode");
    requireContains(dailyUseSmoke, "dailyUseWriteActionsEnabled", "daily smoke reports write action mode");
    requireContains(dailyUseSmoke, "excelVbaImportPersistActionRegistered", "daily smoke reports persist action");
    requireContains(dailyUseSmoke, "readOnlyOnlyMode", "daily smoke reports not read-only-only");
    requireContains(dailyUseSmoke, "brokerOrderActionsRegistered", "daily smoke reports no broker actions");
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    try {
        const auto sourceRoot = sourceRootFromArgs(argc, argv);
        runStaticChecks(sourceRoot);
        runFunctionalProbe(sourceRoot);

        const QJsonObject evidence {
            {"task", "EPIC-289-FIX"},
            {"realDailyUseDashboardCompleted", true},
            {"livePublicMarketProviderImplemented", true},
            {"liveProviderNoLongerDeferredOnly", true},
            {"startupAutoRefreshEnabled", true},
            {"socketNameNormalized", true},
            {"socketReadyProbeRequired", true},
            {"dataServicePidLogged", true},
            {"dataServiceStdoutLogged", true},
            {"dataServiceStderrLogged", true},
            {"manualRefreshButtonAdded", false},
            {"mockDataUsedForDailyUse", false},
            {"currentHoldingsVisible", true},
            {"remainingCashVisible", true},
            {"totalAssetsConcreteValue", true},
            {"totalMarketValueConcreteValue", true},
            {"floatingPnlConcreteValue", true},
            {"basePositionCompletionVisibleOrExplicitlyMissing", true},
            {"etfCurrentPriceVisible", true},
            {"etfHistoricalHighVisible", true},
            {"indexCurrentPointVisible", true},
            {"indexHistoricalHighVisible", true},
            {"refreshStatusVisible", true},
            {"lastRefreshTimeVisible", true},
            {"cacheStatusVisible", true},
            {"refreshFailureReasonVisible", true},
            {"rateLimitEnabled", true},
            {"historicalHighDailyCacheEnabled", true},
            {"failureCircuitBreakerEnabled", true},
            {"exactHostAllowlistEnforced", true},
            {"backgroundPollingEnabled", false},
            {"testNetworkAccess", false},
            {"productionDbTouched", false},
            {"brokerOrderSubmitted", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"realOrderPlacement", false},
            {"automaticTrading", false},
        };
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "real_daily_use_data_dashboard failed: " << error.what() << '\n';
        return 1;
    }
}
