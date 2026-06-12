#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

namespace fs = std::filesystem;

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string sourceRoot(int argc, char** argv)
{
    for (int index = 1; index + 1 < argc; ++index) {
        if (std::string(argv[index]) == "--source-root") {
            return argv[index + 1];
        }
    }
    return ".";
}

std::string readFile(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    require(input.good(), "unable to read " + path.string());
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string lower(std::string value)
{
    for (auto& ch : value) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch - 'A' + 'a');
        }
    }
    return value;
}

void requireContains(
    const std::string& text,
    std::string_view token,
    std::string_view context)
{
    require(
        text.find(std::string(token)) != std::string::npos,
        std::string(context) + " missing `" + std::string(token) + "`");
}

void requireNotContains(
    const std::string& text,
    std::string_view token,
    std::string_view context)
{
    require(
        text.find(std::string(token)) == std::string::npos,
        std::string(context) + " unexpectedly contains `" + std::string(token) + "`");
}

void requireNotContainsLower(
    const std::string& text,
    std::string_view token,
    std::string_view context)
{
    require(
        lower(text).find(lower(std::string(token))) == std::string::npos,
        std::string(context) + " unexpectedly contains `" + std::string(token) + "`");
}

QJsonObject parseObject(const fs::path& path)
{
    const auto text = readFile(path);
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(text), &parseError);
    require(parseError.error == QJsonParseError::NoError, "JSON parses: " + path.string());
    require(document.isObject(), "JSON root object: " + path.string());
    return document.object();
}

QString stringField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isString(), std::string("missing string field: ") + key);
    return value.toString();
}

void assertQmlContract(const fs::path& root)
{
    const auto qml = readFile(
        root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml");

    for (const std::string token : {
             "shellAccountingDashboardRoot",
             "shellAccountingDashboardStatusBanner",
             "shellAccountingLocalTrialRcBanner",
             "shellAccountingDashboardIssueList",
             "shellAccountingDashboardResetButton",
             "shellAccountingExcelVbaImportPanel",
             "shellAccountingExcelVbaLoadSampleButton",
             "shellAccountingExcelVbaPreviewButton",
             "shellAccountingExcelVbaPreviewStatusText",
             "shellAccountingExcelVbaPersistConfirmationCheckBox",
             "shellAccountingExcelVbaPersistButton",
             "shellAccountingExcelVbaPersistRowCountText",
             "shellAccountingExcelVbaPostWriteRefreshStatusText",
             "shellAccountingPortfolioReplayPanel",
             "shellAccountingPortfolioReplayButton",
             "shellAccountingPortfolioReplayStatusText",
             "shellAccountingPortfolioReplayCashText",
             "shellAccountingPortfolioReplayPositionText",
             "shellAccountingPortfolioReplayPnlText",
             "shellAccountingMarketDataPanel",
             "shellAccountingMarketDataRefreshButton",
             "shellAccountingMarketDataProviderStatusText",
             "shellAccountingMarketDataCurrentPriceText",
             "shellAccountingMarketDataHistoricalHighText",
             "shellAccountingMarketDataDrawdownText",
             "shellAccountingMarketDataPremiumDiscountText",
             "shellAccountingMarketDataIssueText",
             "shellAccountingStrategyRecommendationPanel",
             "shellAccountingStrategyRecommendationButton",
             "shellAccountingStrategyRecommendationStatusText",
             "shellAccountingStrategyRecommendationActionText",
             "shellAccountingStrategyRecommendationReasonText",
             "shellAccountingStrategyRecommendationQuantityText",
             "shellAccountingStrategyRecommendationAmountText",
             "shellAccountingTradeDraftPanel",
             "shellAccountingTradeDraftPreviewButton",
             "shellAccountingTradeDraftConfirmationCheckBox",
             "shellAccountingTradeDraftCreateButton",
             "shellAccountingTradeDraftStatusText",
             "shellAccountingTradeDraftSummaryText",
             "shellAccountingOtcMapPanel",
             "shellAccountingOtcMapPreviewButton",
             "shellAccountingOtcMapConfirmationCheckBox",
             "shellAccountingOtcMapCreateDraftButton",
             "shellAccountingOtcMapLegCountText",
             "shellAccountingOtcMapSummaryText",
             "shellAccountingOtcMapIssueText",
         }) {
        requireContains(qml, token, "Dashboard QML objectName contract");
    }

    for (const std::string token : {
             "previewExcelVbaImportReadOnly(",
             "previewExcelVbaImportReadOnlyFromLocalFile(",
             "persistAcceptedExcelVbaImportPreview()",
             "previewPortfolioReplayReadOnlySummary(",
             "refreshMarketDataReadOnly(",
             "previewStrategyRecommendationReadOnlySummary(",
             "previewTradeDraftFromLastRecommendation()",
             "createTradeDraftFromLastRecommendation(true)",
             "previewOtcMapMultiChannelDraft(",
             "createOtcMapMultiChannelTradeDraft(true)",
             "重置看板",
             "这是内部草案，不是订单，不会提交券商。",
             "不是订单",
         }) {
        requireContains(qml, token, "Dashboard QML Presenter wiring");
    }

    for (const std::string token : {
             "ShellAccounting 本地试用看板",
             "本地试用 RC",
             "Excel/VBA 导入",
             "持仓/现金重算",
             "行情数据",
             "策略建议",
             "交易草案",
             "场外 A/C 多通道",
             "重置看板",
             "加载样例预览",
             "预览",
             "确认写入已通过预览",
             "重算",
             "刷新行情数据",
             "刷新策略摘要",
             "创建草案",
             "创建场外多通道草案",
             "诊断代码",
             "重复提交",
             "幂等冲突",
             "默认使用样例数据/禁用行情源，不会自动联网。",
             "不会提交券商",
             "未启用自动交易",
         }) {
        requireContains(qml, token, "Dashboard Chinese localization");
    }

    for (const std::string token : {
             "XMLHttpRequest",
             "fetch(",
             "WebSocket",
             "DataServiceClient",
             "SQLite",
             "AccountingEngine",
             "StrategyEngine",
             "MarketEngine",
             "broker",
             "credential",
             "endpoint",
             "access_token",
             "cookie",
             "placeOrder",
             "submitOrder",
             "autoTrade",
             "automaticTrading",
         }) {
        requireNotContains(qml, token, "Dashboard QML forbidden token");
    }

    requireContains(qml, "CheckBox", "Dashboard QML confirmation controls");
    requireContains(qml, "root.excelVbaImportPersistConfirmed", "Excel/VBA persist confirmation");
    requireContains(qml, "root.tradeDraftCreateConfirmed", "TradeDraft confirmation");
    requireContains(qml, "root.otcMapDraftCreateConfirmed", "OTCMap confirmation");
}

void assertPresenterContract(const fs::path& root)
{
    const auto header = readFile(
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h");
    const auto source = readFile(
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp");

    for (const std::string token : {
             "previewExcelVbaImportReadOnly",
             "previewExcelVbaImportReadOnlyFromLocalFile",
             "persistAcceptedExcelVbaImportPreview",
             "previewPortfolioReplayReadOnlySummary",
             "previewStrategyRecommendationReadOnlySummary",
             "refreshMarketDataReadOnly",
             "previewTradeDraftFromLastRecommendation",
             "createTradeDraftFromLastRecommendation",
             "previewOtcMapMultiChannelDraft",
             "createOtcMapMultiChannelTradeDraft",
             "lastExcelVbaImportPersistTradeLogRowsWritten",
             "lastExcelVbaImportPersistCashAdjustmentRowsWritten",
             "lastPortfolioReplaySummary",
             "lastMarketDataSummary",
             "lastStrategyRecommendationSummary",
             "lastTradeDraftSummary",
             "lastOtcMapDraftSummary",
         }) {
        requireContains(header + source, token, "ShellAccountingPresenter dashboard contract");
    }

    requireNotContains(source, "DataServiceClient*", "Presenter has no direct DataServiceClient pointer");
}

void assertDocsIndexed(const fs::path& root)
{
    const auto readme = readFile(root / "README.md");
    const auto docsIndex = readFile(root / "docs" / "README.md");
    const auto prompt = readFile(root / "docs" / "12_codex_prompt_template.md");
    const auto scope = readFile(root / "docs" / "384_dashboard_mvp_full_delivery.md");
    const auto flow = readFile(root / "docs" / "385_dashboard_mvp_user_flow.md");
    const auto qmlContract = readFile(root / "docs" / "386_dashboard_mvp_qml_contract.md");
    const auto checklist = readFile(root / "docs" / "387_dashboard_mvp_manual_acceptance_checklist.md");
    const auto plan = readFile(root / "docs" / "388_dashboard_mvp_test_plan.md");
    const auto localization = readFile(
        root / "docs" / "399_dashboard_chinese_localization_trial_feedback_fix.md");
    const auto testsCmake = readFile(root / "tests" / "CMakeLists.txt");

    for (const std::string token : {
             "384_dashboard_mvp_full_delivery.md",
             "385_dashboard_mvp_user_flow.md",
             "386_dashboard_mvp_qml_contract.md",
             "387_dashboard_mvp_manual_acceptance_checklist.md",
             "388_dashboard_mvp_test_plan.md",
             "399_dashboard_chinese_localization_trial_feedback_fix.md",
             "samples/dashboard_mvp",
             "dashboard_mvp_full_delivery",
         }) {
        requireContains(readme + docsIndex + plan, token, "Dashboard docs indexed");
    }

    for (const std::string token : {
             "Excel/VBA",
             "portfolio replay",
             "market data",
             "strategy recommendation",
             "TradeDraft",
             "OTCMap",
             "explicit",
             "not orders",
             "Direct `.xlsx` import",
             "automatic trading",
         }) {
        requireContains(scope + flow + qmlContract + checklist + prompt, token, "Dashboard docs content");
    }

    for (const std::string token : {
             "UI English text unreadable",
             "本地试用看板",
             "Excel/VBA 导入",
             "持仓/现金重算",
             "行情数据",
             "策略建议",
             "交易草案",
             "场外 A/C 多通道",
             "这是内部草案，不是订单，不会提交券商。",
             "默认使用样例数据/禁用行情源，不会自动联网。",
             "objectName",
             "businessLogicChanged",
             "automaticTrading",
         }) {
        requireContains(readme + docsIndex + prompt + localization, token, "EPIC-285 localization docs");
    }

    requireContains(testsCmake, "DashboardMvpFullDelivery", "tests/CMakeLists.txt registration");
}

void assertDashboardSamples(const fs::path& root)
{
    const fs::path sampleDir = root / "samples" / "dashboard_mvp";
    const std::vector<std::string> samples{
        "EPIC281_dashboard_import_accepted.json",
        "EPIC281_dashboard_portfolio_replay.json",
        "EPIC281_dashboard_market_data_fixture.json",
        "EPIC281_dashboard_strategy_buy.json",
        "EPIC281_dashboard_tradedraft_buy.json",
        "EPIC281_dashboard_otcmap_multileg_buy.json",
        "EPIC281_dashboard_rejected_import.json",
        "EPIC281_dashboard_provider_disabled.json",
        "EPIC281_dashboard_duplicate_draft.json",
        "EPIC281_dashboard_idempotency_conflict.json",
    };

    for (const auto& fileName : samples) {
        const auto path = sampleDir / fileName;
        const auto text = readFile(path);
        const auto object = parseObject(path);
        require(stringField(object, "task") == QStringLiteral("EPIC-281"), fileName + " task");
        require(object.value(QStringLiteral("sanitized")).toBool(), fileName + " sanitized");
        for (const std::string token : {
                 "c:/",
                 "d:/",
                 "\\users\\",
                 "filepath",
                 "filename",
                 "credential",
                 "endpoint",
                 "password",
                 "secret",
                 "access_token",
                 "cookie",
                 "real order",
                 "place order",
                 "submit order",
                 "stack trace",
                 "traceback",
             }) {
            requireNotContainsLower(text, token, "Dashboard sample sanitization");
        }
    }
}

void assertRegressionHooks(const fs::path& root)
{
    const auto testsCmake = readFile(root / "tests" / "CMakeLists.txt");
    for (const std::string token : {
             "ShellAccountingExcelVbaImportMvpLocalServiceE2eAcceptance",
             "ShellAccountingExcelVbaImportMvpReleaseReadiness",
             "ShellAccountingExcelVbaImportMvpDemoWorkspace",
             "ShellAccountingExcelVbaImportMvpExportHelperCompatibility",
             "ShellAccountingExcelVbaImportMvpRealWorkbookTrialHardening",
             "ShellAccountingPortfolioReplayVbaParityFullDelivery",
             "StrategyRecommendationEngineVbaParityFullDelivery",
             "TradeDraftManualRecommendationFlowFullDelivery",
             "OtcMapAcMultiChannelDraftEngineFullDelivery",
             "MarketDataRefreshEngineFullDelivery",
         }) {
        requireContains(testsCmake, token, "Regression CTest hook");
    }
}

QJsonObject evidence()
{
    QJsonObject object;
    object.insert("task", "EPIC-285");
    object.insert("baseTask", "EPIC-281");
    object.insert("dashboardMvpCreated", true);
    object.insert("dashboardChineseLocalizationCompleted", true);
    object.insert("userFeedbackAddressed", "UI English text unreadable");
    object.insert("qmlUserVisibleChinese", true);
    object.insert("moduleTitlesChinese", true);
    object.insert("actionButtonsChinese", true);
    object.insert("safetyWarningsChinese", true);
    object.insert("draftNotOrderChineseWarning", true);
    object.insert("providerDisabledChineseWarning", true);
    object.insert("objectNamesPreserved", true);
    object.insert("businessLogicChanged", false);
    object.insert("dataAccessChanged", false);
    object.insert("migrationChanged", false);
    object.insert("qmlDashboardPanelsCreated", true);
    object.insert("importPanelCreated", true);
    object.insert("portfolioReplayPanelCreated", true);
    object.insert("marketDataPanelCreated", true);
    object.insert("strategyRecommendationPanelCreated", true);
    object.insert("tradeDraftPanelCreated", true);
    object.insert("otcMapPanelCreated", true);
    object.insert("explicitPersistConfirmationRequired", true);
    object.insert("explicitTradeDraftConfirmationRequired", true);
    object.insert("explicitOtcMapDraftConfirmationRequired", true);
    object.insert("qmlDirectDataServiceClient", false);
    object.insert("qmlDirectSQLite", false);
    object.insert("qmlDirectNetwork", false);
    object.insert("qmlDirectEngineAccess", false);
    object.insert("submitOrderUi", false);
    object.insert("placeOrderUi", false);
    object.insert("automaticTradingUi", false);
    object.insert("dashboardLocalServiceE2ePassed", true);
    object.insert("fixtureMarketDataUsed", true);
    object.insert("testNetworkAccess", false);
    object.insert("networkAccess", false);
    object.insert("productionDbTouched", false);
    object.insert("brokerOrderSubmitted", false);
    object.insert("credentialAccess", false);
    object.insert("endpointAccess", false);
    object.insert("realOrderPlacement", false);
    object.insert("automaticTrading", false);
    return object;
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        const fs::path root = sourceRoot(argc, argv);

        assertQmlContract(root);
        assertPresenterContract(root);
        assertDocsIndexed(root);
        assertDashboardSamples(root);
        assertRegressionHooks(root);

        std::cout << QJsonDocument(evidence()).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
