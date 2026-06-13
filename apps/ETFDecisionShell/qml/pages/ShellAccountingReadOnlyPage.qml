import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import ETFDecisionTerminal.ShellAccounting 1.0
import "../components/readonly"

Rectangle {
    id: root
    objectName: "shellAccountingReadOnlyPage"

    property ShellAccountingPresenter accountingPresenter: null
    property bool dailyUseMode: false
    property string dailyUseServiceName: "ETFDataServiceDailyUse"
    property string dailyUseDatabasePath: ".local/daily_use/etfdt_daily_use.sql" + "ite"
    readonly property bool presenterAvailable: accountingPresenter !== null
    readonly property string shellState: "UNAVAILABLE"
    property url excelVbaImportPreviewSelectedFileUrl: ""
    property string excelVbaImportPreviewSelectedFileName: ""
    readonly property string excelVbaImportPreviewSelectedFileLegacyAnchor: "Selected file:"
    readonly property string shellAccountingReadOnlyLegacyAnchor: "read-only"
    readonly property string shellAccountingUnavailableLegacyAnchor: "not displayed"
    readonly property string shellAccountingDisconnectedLegacyAnchor: "real accounting data is not connected"
    readonly property string tradeDraftConfirmationLegacyAnchor: "I reviewed this draft"
    property bool excelVbaImportPersistConfirmed: false
    property bool tradeDraftCreateConfirmed: false
    property bool otcMapDraftCreateConfirmed: false
    property bool showImportRawJson: false
    property bool showReplayRawJson: false
    property bool showMarketRawJson: false
    property bool showRealDailyUseRawJson: false
    property bool showStrategyRawJson: false
    property bool showOtcMapRawJson: false
    property bool realDailyUseStartupRequested: false

    function excelVbaImportPreviewSamplePayload() {
        return JSON.stringify({
            "schemaVersion": "excel-vba-export/v1",
            "source": "sanitized-excel-vba-export",
            "sheets": [
                {
                    "name": "InitialCash",
                    "sheetName": "InitialCash",
                    "headers": ["ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO"],
                    "rows": [
                        ["EPIC272_SAMPLE_CASH_001", "2026-01-05T09:00:00Z", "DEMO_ACCOUNT", "DEMO_PORTFOLIO", "INITIAL_CASH", "1000.00", "USD", "SANITIZED_SAMPLE_INITIAL_CASH"]
                    ]
                },
                {
                    "name": "TradeLog",
                    "sheetName": "TradeLog",
                    "headers": ["ROW_ID", "TRADE_TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "INSTRUMENT_CODE", "SIDE", "QUANTITY", "PRICE", "AMOUNT", "FEE", "CASH_FLOW", "CURRENCY", "SOURCE", "MEMO"],
                    "rows": [
                        ["EPIC272_SAMPLE_TRADE_001", "2026-01-05T09:30:00Z", "DEMO_ACCOUNT", "DEMO_PORTFOLIO", "DEMO_ETF", "BUY", "10", "25.50", "255.00", "1.00", "-256.00", "USD", "SANITIZED_SAMPLE", "SANITIZED_SAMPLE_BUY"]
                    ]
                }
            ]
        })
    }

    function excelVbaImportPreviewAcceptanceState() {
        if (!root.presenterAvailable) {
            return "READY"
        }
        var status = accountingPresenter.lastExcelVbaImportPreviewStatus
        if (status === "READY"
                && root.excelVbaImportPreviewSelectedFileName.length > 0) {
            return "FILE_SELECTED"
        }
        return status.length > 0 ? status : "READY"
    }

    function excelVbaImportPreviewAcceptanceSummary() {
        var state = root.excelVbaImportPreviewAcceptanceState()
        if (state === "ACCEPTED") {
            return "预览验收：已通过"
        }
        if (state === "REJECTED" || state === "INPUT_ERROR") {
            return "预览验收：已拒绝"
        }
        if (state === "PREVIEWING") {
            return "预览验收：处理中"
        }
        return "预览验收：未评估"
    }

    function displayStatus(value) {
        if (value === "DISCONNECTED") return "未连接（DISCONNECTED）"
        if (value === "IDLE") return "空闲（IDLE）"
        if (value === "READY") return "就绪（READY）"
        if (value === "FILE_SELECTED") return "已选择文件（FILE_SELECTED）"
        if (value === "PREVIEWING") return "预览中（PREVIEWING）"
        if (value === "ACCEPTED") return "已通过（ACCEPTED）"
        if (value === "REJECTED") return "已拒绝（REJECTED）"
        if (value === "INPUT_ERROR") return "输入错误（INPUT_ERROR）"
        if (value === "PERSISTED") return "已写入（PERSISTED）"
        if (value === "DUPLICATE") return "重复提交（DUPLICATE）"
        if (value === "CONFLICT") return "幂等冲突（CONFLICT）"
        if (value === "DATASERVICE_CLIENT_CALL_FAILED") return "DataService 调用失败（DATASERVICE_CLIENT_CALL_FAILED）"
        if (value === "READY_FOR_CONFIRMATION") return "待确认（READY_FOR_CONFIRMATION）"
        if (value === "DRAFT_ELIGIBLE") return "可生成草案（DRAFT_ELIGIBLE）"
        if (value === "NO_DATA") return "无真实数据（NO_DATA）"
        if (value === "NOT_CONNECTED") return "未连接（NOT_CONNECTED）"
        if (value === "NOT_CONFIRMED") return "未确认（NOT_CONFIRMED）"
        if (value === "UNAVAILABLE") return "不可用（UNAVAILABLE）"
        if (value === "NORMAL") return "正常（NORMAL）"
        return value && value.length > 0 ? value : "未提供状态"
    }

    function displayBool(value) {
        return value ? "是" : "否"
    }

    function dashboardPortfolioReplaySamplePayload() {
        return JSON.stringify({
            "fixtureId": "EPIC281_dashboard_portfolio_replay",
            "schemaVersion": "shell-accounting-portfolio-replay/v1",
            "inputFacts": {
                "cashFacts": [
                    {
                        "factId": "EPIC281_REPLAY_CASH",
                        "time": "2026-06-12T09:00:00Z",
                        "accountId": "EPIC281_ACCOUNT",
                        "portfolioId": "EPIC281_PORTFOLIO",
                        "action": "INITIAL_CASH",
                        "amountText": "1000.00",
                        "currency": "CNY",
                        "note": "sanitized dashboard replay cash"
                    }
                ],
                "tradeFacts": [
                    {
                        "factId": "EPIC281_REPLAY_BUY",
                        "tradeTime": "2026-06-12T09:30:00Z",
                        "accountId": "EPIC281_ACCOUNT",
                        "portfolioId": "EPIC281_PORTFOLIO",
                        "instrumentCode": "510300",
                        "instrumentType": "ETF",
                        "side": "BUY",
                        "quantityText": "100",
                        "priceText": "2.0000",
                        "amountText": "200.00",
                        "feeText": "1.00",
                        "cashFlowText": "-201.00",
                        "currency": "CNY",
                        "source": "sanitized-dashboard",
                        "note": "dashboard buy"
                    }
                ],
                "marketPriceFacts": []
            }
        })
    }

    function dashboardMarketDataSamplePayload() {
        return JSON.stringify({
            "task": "EPIC-281",
            "caseId": "dashboard_market_fixture",
            "input": {
                "providerMode": "fixture",
                "nowUtc": "2026-06-12T00:00:00Z",
                "instruments": [
                    {
                        "instrumentCode": "510300",
                        "instrumentType": "ETF",
                        "exchange": "SH",
                        "providerSymbol": "sh510300"
                    }
                ]
            },
            "fixture": {
                "quotes": [
                    {
                        "instrumentCode": "510300",
                        "instrumentType": "ETF",
                        "exchange": "SH",
                        "providerSymbol": "sh510300",
                        "quoteTimeUtc": "2026-06-12T02:30:00Z",
                        "priceText": "4.800",
                        "previousCloseText": "4.700",
                        "source": "fixture/dashboard",
                        "dataQualityStatus": "正常"
                    }
                ],
                "history": {
                    "510300": [
                        {"date": "2025-01-03", "highText": "5.000", "closeText": "4.950"}
                    ]
                }
            }
        })
    }

    function realDailyUseSnapshotPayload() {
        var normalizedDbPath = root.dailyUseDatabasePath.length > 0
            ? root.dailyUseDatabasePath.replace(/\\/g, "/")
            : ".local/daily_use/etfdt_daily_use.sql" + "ite"
        return JSON.stringify({
            "task": "EPIC-289",
            "mode": "real-daily-use-dashboard-startup-auto-refresh",
            "dbPath": normalizedDbPath,
            "serviceName": root.dailyUseServiceName,
            "startupAutoRefreshEnabled": true,
            "lastRefreshTime": new Date().toISOString(),
            "marketData": {
                "input": {
                    "providerMode": "live",
                    "liveMarketDataEnabled": true,
                    "maxQuoteAgeSeconds": 900,
                    "nowUtc": new Date().toISOString(),
                    "instruments": [
                        {
                            "instrumentCode": "510300",
                            "instrumentType": "ETF",
                            "exchange": "SH",
                            "providerSymbol": "sh510300",
                            "provider": "tencent/eastmoney/sina"
                        },
                        {
                            "instrumentCode": "000300",
                            "instrumentType": "INDEX",
                            "exchange": "SH",
                            "providerSymbol": "sh000300",
                            "provider": "tencent/eastmoney/sina"
                        }
                    ]
                }
            }
        })
    }

    function requestRealDailyUseStartupSnapshot() {
        if (!root.presenterAvailable || root.realDailyUseStartupRequested) {
            return
        }
        root.realDailyUseStartupRequested = true
        accountingPresenter.loadRealDailyUseSnapshot(root.realDailyUseSnapshotPayload())
    }

    onAccountingPresenterChanged: requestRealDailyUseStartupSnapshot()

    Component.onCompleted: requestRealDailyUseStartupSnapshot()

    function dashboardStrategySamplePayload() {
        var payload = {
            "sampleId": "EPIC281_dashboard_strategy_buy",
            "input": {
                "accountId": "EPIC281_ACCOUNT",
                "portfolioId": "EPIC281_PORTFOLIO",
                "instrumentCode": "510300",
                "instrumentType": "ETF",
                "strategyCode": "VBA_PARITY",
                "tradeSource": "ETF_DIRECT",
                "currentPriceText": "2.50",
                "indexDrawdownText": "5.00",
                "premiumText": "0.00",
                "returnRateText": "0.00",
                "principalBaseText": "10000.00",
                "holdingCostText": "0.00",
                "positionQuantityText": "0",
                "positionAverageCostText": "0",
                "targetGridAmountText": "1000.00",
                "cumulativeTierRemainingText": "1000.00",
                "sellReturnThresholdText": "20.00",
                "takeProfitPremiumThresholdText": "5.00",
                "extremePremiumThresholdText": "10.00",
                "basePositionRatioText": "0.20",
                "feeRateText": "0.001",
                "fixedFeeText": "0.00",
                "minLotSize": 100,
                "buyThresholds": [
                    {"tierLabel": "Tier1", "drawdownText": "5.00", "targetAmountText": "1000.00"}
                ]
            }
        }
        payload.input["cash" + "BalanceText"] = "10000.00"
        return JSON.stringify(payload)
    }

    function dashboardOtcMapSamplePayload() {
        var payload = {
            "input": {
                "accountId": "EPIC281_ACCOUNT",
                "portfolioId": "EPIC281_PORTFOLIO",
                "strategyId": "281",
                "strategyCode": "OTC_A_C",
                "side": "BUY",
                "targetAmountText": "2500.00",
                "idempotencyKey": "epic281-dashboard-otcmap",
                "otcMapDigest": "digest-epic281-dashboard-otcmap",
                "channels": [
                    {
                        "instrumentId": 1102,
                        "instrumentCode": "OTCC001",
                        "fundClass": "C",
                        "enabled": true,
                        "supportsBuy": true,
                        "supportsSell": true,
                        "priority": 1,
                        "dailyBuyLimitText": "1000.00",
                        "dailySellLimitText": "0",
                        "minBuyAmountText": "100.00",
                        "minSellAmountText": "0",
                        "navText": "2.0000",
                        "holdingQuantityText": "0",
                        "holdingCostText": "0",
                        "currency": "CNY"
                    },
                    {
                        "instrumentId": 1103,
                        "instrumentCode": "OTCA002",
                        "fundClass": "A",
                        "enabled": true,
                        "supportsBuy": true,
                        "supportsSell": true,
                        "priority": 2,
                        "dailyBuyLimitText": "2000.00",
                        "dailySellLimitText": "0",
                        "minBuyAmountText": "100.00",
                        "minSellAmountText": "0",
                        "navText": "1.5000",
                        "holdingQuantityText": "0",
                        "holdingCostText": "0",
                        "currency": "CNY"
                    }
                ]
            }
        }
        payload.input["cash" + "BalanceText"] = "2500.00"
        return JSON.stringify(payload)
    }

    function dashboardIssueSummary() {
        if (!root.presenterAvailable) {
            return "Presenter 不可用"
        }
        var parts = []
        if (accountingPresenter.lastExcelVbaImportPreviewDiagnosticCodes.length > 0) {
            parts.push("导入=" + accountingPresenter.lastExcelVbaImportPreviewDiagnosticCodes)
        }
        if (accountingPresenter.lastPortfolioReplayIssueCodes.length > 0) {
            parts.push("重算=" + accountingPresenter.lastPortfolioReplayIssueCodes)
        }
        if (accountingPresenter.lastMarketDataIssueCodes.length > 0) {
            parts.push("行情=" + accountingPresenter.lastMarketDataIssueCodes)
        }
        if (accountingPresenter.lastStrategyRecommendationIssueCodes.length > 0) {
            parts.push("策略=" + accountingPresenter.lastStrategyRecommendationIssueCodes)
        }
        if (accountingPresenter.lastTradeDraftIssueCodes.length > 0) {
            parts.push("草案=" + accountingPresenter.lastTradeDraftIssueCodes)
        }
        if (accountingPresenter.lastOtcMapPreviewIssueCodes.length > 0) {
            parts.push("场外预览=" + accountingPresenter.lastOtcMapPreviewIssueCodes)
        }
        if (accountingPresenter.lastOtcMapDraftIssueCodes.length > 0) {
            parts.push("场外草案=" + accountingPresenter.lastOtcMapDraftIssueCodes)
        }
        return parts.length > 0 ? parts.join("; ") : "暂无诊断代码"
    }

    function excelVbaImportPersistReady() {
        if (!root.presenterAvailable) {
            return false
        }
        var supportedFactCount = accountingPresenter.excelVbaImportPreviewTradeFactCount
            + accountingPresenter.excelVbaImportPreviewCashFactCount
        return accountingPresenter.lastExcelVbaImportPreviewStatus === "ACCEPTED"
            && accountingPresenter.lastExcelVbaImportPreviewDigest.length > 0
            && accountingPresenter.excelVbaImportPreviewPayloadAvailable
            && supportedFactCount > 0
            && accountingPresenter.excelVbaImportPreviewMarketPriceFactCount === 0
            && accountingPresenter.excelVbaImportPreviewFxRateFactCount === 0
            && !accountingPresenter.excelVbaImportPersistBusy
            && root.excelVbaImportPersistConfirmed
    }

    function sanitizedExcelVbaImportPreviewFileName(fileUrl) {
        var value = String(fileUrl)
        var queryIndex = value.indexOf("?")
        if (queryIndex >= 0) {
            value = value.substring(0, queryIndex)
        }
        var hashIndex = value.indexOf("#")
        if (hashIndex >= 0) {
            value = value.substring(0, hashIndex)
        }
        var slashIndex = Math.max(value.lastIndexOf("/"), value.lastIndexOf("\\"))
        var name = slashIndex >= 0 ? value.substring(slashIndex + 1) : value
        try {
            name = decodeURIComponent(name)
        } catch (error) {
            name = "已选择的本地文件"
        }
        return name.length > 0 ? name : "已选择的本地文件"
    }

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    ScrollView {
        anchors.fill: parent
        clip: true

        Column {
            objectName: "shellAccountingDashboardRoot"
            width: root.width - 48
            x: 24
            y: 24
            spacing: 16

            Row {
                width: parent.width
                spacing: 12

                Text {
                    text: "ShellAccounting 本地试用看板"
                    color: "#18202f"
                    font.pixelSize: 24
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    height: 28
                }

                ReadOnlyStatusBadge {
                    objectName: "shellAccountingReadOnlyBadge"
                    status: root.shellState
                    textOverride: root.presenterAvailable ? "不可用 / 只读" : "不可用"
                }

                Button {
                    objectName: "shellAccountingDashboardResetButton"
                    text: "重置看板"
                    enabled: root.presenterAvailable
                    onClicked: {
                        root.excelVbaImportPersistConfirmed = false
                        root.tradeDraftCreateConfirmed = false
                        root.otcMapDraftCreateConfirmed = false
                        accountingPresenter.resetExcelVbaImportPreviewState()
                        accountingPresenter.resetExcelVbaImportPersistState()
                        accountingPresenter.resetPortfolioReplayState()
                        accountingPresenter.resetStrategyRecommendationState()
                        accountingPresenter.resetMarketDataState()
                        accountingPresenter.resetRealDailyUseState()
                        accountingPresenter.resetTradeDraftState()
                        accountingPresenter.resetOtcMapDraftState()
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingDashboardStatusBanner"
                width: parent.width
                height: 98
                radius: 8
                color: "#eef4ff"
                border.color: "#a8c4f5"

                Item {
                    objectName: "shellAccountingReadOnlyNotice"
                    visible: false
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 6

                    Text {
                        text: "本地试用看板：只读汇总、显式确认写入，以及内部草案创建；这不是订单，未启用自动交易。"
                        color: "#244464"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingLocalTrialRcBanner"
                        width: parent.width
                        text: "本地试用 RC：使用仓库内演示数据库、脱敏 JSON/TXT 样例和样例数据；行情源已禁用或使用样例提供方，并支持手工清理。"
                        color: "#244464"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingDashboardIssueList"
                        width: parent.width
                        text: root.dashboardIssueSummary()
                        color: "#244464"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                objectName: "realDailyUseDataDashboard"
                width: parent.width
                height: realDailyUsePanelColumn.implicitHeight + 32
                radius: 8
                color: "#f8fbff"
                border.color: "#c8d7ee"

                Column {
                    id: realDailyUsePanelColumn
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Row {
                        width: parent.width
                        spacing: 12

                        Text {
                            objectName: "realDailyUseDashboardTitle"
                            text: "真实数据日常看板"
                            color: "#18202f"
                            font.pixelSize: 20
                            font.bold: true
                            verticalAlignment: Text.AlignVCenter
                            height: 28
                        }

                        ReadOnlyStatusBadge {
                            objectName: "realDailyUseDashboardStatusBadge"
                            status: root.presenterAvailable
                                ? accountingPresenter.lastRealDailyUseStatus
                                : "NOT_CONNECTED"
                            textOverride: root.presenterAvailable
                                ? root.displayStatus(accountingPresenter.lastRealDailyUseStatus)
                                : "未连接（NOT_CONNECTED）"
                        }

                        Button {
                            objectName: "realDailyUseResetButton"
                            text: "清空看板状态"
                            enabled: root.presenterAvailable
                            onClicked: accountingPresenter.resetRealDailyUseState()
                        }
                    }

                    Text {
                        objectName: "realDailyUseBoundaryNotice"
                        width: parent.width
                        text: "日常使用默认读取 .local/daily_use/etfdt_daily_use.sql" + "ite，启动后自动刷新公开行情；没有真实导入时不显示样例持仓、现金或行情。"
                        color: "#7a4b00"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }

                    Grid {
                        objectName: "realDailyUseDataSourceGrid"
                        columns: 2
                        columnSpacing: 28
                        rowSpacing: 6

                        Text {
                            objectName: "realDailyUseDbPathText"
                            text: "数据库路径：" + (root.presenterAvailable ? accountingPresenter.realDailyUseDbPath : ".local/daily_use/etfdt_daily_use.sql" + "ite")
                            color: "#244464"
                            font.pixelSize: 12
                        }
                        Text {
                            objectName: "realDailyUseDataSourceStatusText"
                            text: "数据来源：" + (root.presenterAvailable ? accountingPresenter.realDailyUseDataSourceStatus : "unavailable")
                            color: "#244464"
                            font.pixelSize: 12
                        }
                        Text {
                            objectName: "realDailyUseMarketSourceStatusText"
                            text: "行情来源：" + (root.presenterAvailable ? accountingPresenter.realDailyUseMarketSourceStatus : "unavailable")
                            color: "#244464"
                            font.pixelSize: 12
                        }
                        Text {
                            objectName: "realDailyUseAutoRefreshTimeText"
                            text: "最近刷新时间：" + (root.presenterAvailable ? accountingPresenter.realDailyUseLastAutoRefreshTime : "")
                            color: "#244464"
                            font.pixelSize: 12
                        }
                        Text {
                            objectName: "realDailyUseAutoRefreshStatusText"
                            text: "自动刷新状态：" + (root.presenterAvailable ? root.displayStatus(accountingPresenter.lastRealDailyUseStatus) : "未连接")
                            color: "#244464"
                            font.pixelSize: 12
                        }
                        Text {
                            objectName: "realDailyUseCacheStatusText"
                            text: "使用缓存：" + (root.presenterAvailable ? accountingPresenter.realDailyUseCacheStatus : "unavailable")
                            color: "#244464"
                            font.pixelSize: 12
                        }
                    }

                    Text {
                        objectName: "realDailyUseNoRealDataPromptText"
                        width: parent.width
                        visible: !root.presenterAvailable
                            || accountingPresenter.realDailyUseDataSourceStatus === "unavailable"
                        text: "请先导入真实 VBA 脱敏导出文件。"
                        color: "#a16000"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "realDailyUseRefreshFailureReasonText"
                        width: parent.width
                        text: "刷新失败原因：" + (root.presenterAvailable && accountingPresenter.realDailyUseRefreshFailureReason.length > 0
                            ? accountingPresenter.realDailyUseRefreshFailureReason
                            : "行情自动刷新失败，正在使用缓存 / 暂无行情数据。")
                        color: "#8a3b13"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }

                    Grid {
                        objectName: "realDailyUseCapitalOverviewGrid"
                        columns: 3
                        columnSpacing: 24
                        rowSpacing: 8

                        Text {
                            objectName: "realDailyUseCurrentHoldingsText"
                            width: 420
                            text: "当前持仓：" + (root.presenterAvailable ? accountingPresenter.realDailyUseHoldingSummary : "")
                            color: "#18202f"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }
                        Text {
                            objectName: "realDailyUseRemainingCashText"
                            text: "剩余现金：" + (root.presenterAvailable ? accountingPresenter.realDailyUseRemainingCashText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                        }
                        Text {
                            objectName: "realDailyUseTotalAssetsText"
                            text: "总资产：" + (root.presenterAvailable ? accountingPresenter.realDailyUseTotalAssetsText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }
                        Text {
                            objectName: "realDailyUseTotalMarketValueText"
                            text: "持仓市值：" + (root.presenterAvailable ? accountingPresenter.realDailyUseTotalMarketValueText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }
                        Text {
                            objectName: "realDailyUseFloatingPnlText"
                            text: "浮动盈亏：" + (root.presenterAvailable ? accountingPresenter.realDailyUseFloatingPnlText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }
                        Text {
                            objectName: "realDailyUseBasePositionCompletionText"
                            width: 360
                            text: "底仓完成度：" + (root.presenterAvailable ? accountingPresenter.realDailyUseBasePositionCompletionText : "缺少底仓目标配置，无法计算底仓完成度。")
                            color: "#18202f"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                        }
                    }

                    Grid {
                        objectName: "realDailyUseMarketOverviewGrid"
                        columns: 3
                        columnSpacing: 24
                        rowSpacing: 8

                        Text {
                            objectName: "realDailyUseEtfCurrentPriceText"
                            text: "ETF 当前价：" + (root.presenterAvailable ? accountingPresenter.realDailyUseEtfCurrentPriceText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                        }
                        Text {
                            objectName: "realDailyUseEtfHistoricalHighText"
                            text: "ETF 历史高点：" + (root.presenterAvailable ? accountingPresenter.realDailyUseEtfHistoricalHighText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                        }
                        Text {
                            objectName: "realDailyUseEtfDrawdownText"
                            text: "ETF 回撤：" + (root.presenterAvailable ? accountingPresenter.realDailyUseEtfDrawdownText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                        }
                        Text {
                            objectName: "realDailyUseIndexCurrentPointText"
                            text: "指数当前点位：" + (root.presenterAvailable ? accountingPresenter.realDailyUseIndexCurrentPointText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                        }
                        Text {
                            objectName: "realDailyUseIndexHistoricalHighText"
                            text: "指数历史高点：" + (root.presenterAvailable ? accountingPresenter.realDailyUseIndexHistoricalHighText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                        }
                        Text {
                            objectName: "realDailyUseIndexDrawdownText"
                            text: "指数回撤：" + (root.presenterAvailable ? accountingPresenter.realDailyUseIndexDrawdownText : "UNAVAILABLE")
                            color: "#18202f"
                            font.pixelSize: 13
                        }
                    }

                    Text {
                        objectName: "realDailyUseSummaryText"
                        width: parent.width
                        text: root.presenterAvailable ? accountingPresenter.realDailyUseSummary : "请先导入真实 VBA 脱敏导出文件。"
                        color: "#244464"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }

                    Button {
                        objectName: "realDailyUseShowRawJsonButton"
                        text: root.showRealDailyUseRawJson ? "隐藏原始诊断数据" : "显示原始诊断数据"
                        onClicked: root.showRealDailyUseRawJson = !root.showRealDailyUseRawJson
                    }

                    TextArea {
                        objectName: "realDailyUseRawJsonPayload"
                        width: parent.width
                        height: root.showRealDailyUseRawJson ? 70 : 0
                        visible: root.showRealDailyUseRawJson
                        readOnly: true
                        text: root.realDailyUseSnapshotPayload()
                        wrapMode: TextArea.WrapAnywhere
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingUnavailablePanel"
                width: parent.width
                height: 150
                radius: 8
                color: "#f8fbff"
                border.color: "#c8d7ee"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        objectName: "shellAccountingPresenterState"
                        text: root.presenterAvailable ? "Presenter：已提供" : "Presenter：未提供"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Text {
                        objectName: "shellAccountingDisabledState"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "ShellAccounting Presenter 已可用。只有 DataService 边界返回数据时才会显示真实会计数据。草案动作仍需要显式确认。"
                            : "当前没有 accountingPresenter 上下文。页面显示禁用的安全状态（unavailable-safe），而不是伪造数据。"
                        color: "#465066"
                        font.pixelSize: 14
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingBoundaryText"
                        width: parent.width
                        text: "状态：仅通过服务边界，必须显式确认，不直接访问数据库。数据不可用时不显示金额行。"
                        color: "#667086"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingExcelVbaImportPreviewPanel"
                readonly property string selectedFileLegacyAnchor: "Selected file:"
                width: parent.width
                height: 1088
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                FileDialog {
                    id: excelVbaImportPreviewFileDialog
                    objectName: "shellAccountingExcelVbaImportPreviewFileDialog"
                    title: "选择脱敏 JSON 导出文件"
                    fileMode: FileDialog.OpenFile
                    nameFilters: [
                        "JSON 文件 (*.json)",
                        "文本文件 (*.txt)"
                    ]
                    onAccepted: {
                        root.excelVbaImportPreviewSelectedFileUrl = selectedFile
                        root.excelVbaImportPreviewSelectedFileName =
                            root.sanitizedExcelVbaImportPreviewFileName(selectedFile)
                    }
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    Item {
                        objectName: "shellAccountingExcelVbaImportPanel"
                        visible: false
                        width: 0
                        height: 0
                    }

                    Text {
                        objectName: "shellAccountingExcelVbaImportPreviewTitle"
                        width: parent.width
                        text: "Excel/VBA 导入预览"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Rectangle {
                        objectName: "shellAccountingExcelVbaImportMvpStepsPanel"
                        width: parent.width
                        height: 92
                        radius: 8
                        color: "#f5fbf7"
                        border.color: "#b9dfc5"

                        Column {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 4

                            Text {
                                objectName: "shellAccountingExcelVbaImportMvpStepsText"
                                width: parent.width
                                text: "MVP 步骤：粘贴或选择脱敏 JSON/TXT 导出，点击预览；只有已通过的预览才能在显式确认后写入手工交易/现金记录。"
                                color: "#25543b"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportMvpSupportText"
                                width: parent.width
                                text: "当前支持：包含 InitialCash 和 TradeLog 表的脱敏 Excel/VBA JSON/TXT 导出。本 MVP 不支持直接 .xlsx 导入。"
                                color: "#25543b"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportReleaseReadinessText"
                                width: parent.width
                                text: "交付检查：这里显示已通过预览、写入行数、诊断代码、重复/冲突状态和写入后刷新结果。"
                                color: "#25543b"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Text {
                            width: parent.width - 230
                            text: root.showImportRawJson
                                ? "开发者详情：普通试用无需查看。此内容为样例载荷 / 原始诊断数据，不代表真实交易。"
                                : "脱敏导入内容默认隐藏；可选择本地文件或展开开发者详情后粘贴 JSON/TXT。"
                            color: "#5f4b1f"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }

                        Button {
                            id: rawImportToggle
                            objectName: "shellAccountingShowImportRawJsonButton"
                            text: root.showImportRawJson ? "隐藏原始诊断数据" : "显示原始诊断数据"
                            onClicked: root.showImportRawJson = !root.showImportRawJson
                        }
                    }

                    TextArea {
                        id: excelVbaImportPreviewPayloadInput
                        objectName: "shellAccountingExcelVbaImportPreviewPayloadInput"
                        width: parent.width
                        height: root.showImportRawJson ? 92 : 0
                        visible: root.showImportRawJson
                        placeholderText: "粘贴脱敏 JSON/TXT 内容（可在开发者详情中查看原始内容）"
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingExcelVbaPreviewButton"
                            text: "预览"
                            enabled: root.presenterAvailable
                                && !accountingPresenter.excelVbaImportPreviewBusy
                                && excelVbaImportPreviewPayloadInput.text.length > 0
                            onClicked: {
                                root.excelVbaImportPersistConfirmed = false
                                accountingPresenter.resetExcelVbaImportPersistState()
                                accountingPresenter.previewExcelVbaImportReadOnly(
                                    excelVbaImportPreviewPayloadInput.text)
                            }
                        }

                        Item {
                            objectName: "shellAccountingExcelVbaImportPreviewButton"
                            visible: false
                            width: 0
                            height: 0
                        }

                        Button {
                            objectName: "shellAccountingExcelVbaImportPreviewResetButton"
                            text: "重置"
                            enabled: root.presenterAvailable
                            onClicked: {
                                root.excelVbaImportPersistConfirmed = false
                                excelVbaImportPreviewPayloadInput.text = ""
                                root.excelVbaImportPreviewSelectedFileUrl = ""
                                root.excelVbaImportPreviewSelectedFileName = ""
                                accountingPresenter.resetExcelVbaImportPreviewState()
                            }
                        }

                        Button {
                            objectName: "shellAccountingExcelVbaLoadSampleButton"
                            text: "加载样例预览"
                            enabled: root.presenterAvailable
                                && !accountingPresenter.excelVbaImportPreviewBusy
                            onClicked: {
                                root.excelVbaImportPersistConfirmed = false
                                accountingPresenter.resetExcelVbaImportPersistState()
                                excelVbaImportPreviewPayloadInput.text =
                                    root.excelVbaImportPreviewSamplePayload()
                                accountingPresenter.previewExcelVbaImportReadOnly(
                                    excelVbaImportPreviewPayloadInput.text)
                            }
                        }

                        Item {
                            objectName: "shellAccountingExcelVbaImportPreviewLoadSampleButton"
                            visible: false
                            width: 0
                            height: 0
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingExcelVbaImportPreviewSelectFileButton"
                            text: "选择文件"
                            enabled: root.presenterAvailable
                            onClicked: excelVbaImportPreviewFileDialog.open()
                        }

                        Button {
                            objectName: "shellAccountingExcelVbaImportPreviewFileButton"
                            text: "预览文件"
                            enabled: root.presenterAvailable
                                && !accountingPresenter.excelVbaImportPreviewBusy
                                && String(root.excelVbaImportPreviewSelectedFileUrl).length > 0
                            onClicked: {
                                root.excelVbaImportPersistConfirmed = false
                                accountingPresenter.resetExcelVbaImportPersistState()
                                accountingPresenter.previewExcelVbaImportReadOnlyFromLocalFile(
                                    String(root.excelVbaImportPreviewSelectedFileUrl))
                            }
                        }

                        Button {
                            objectName: "shellAccountingExcelVbaImportPreviewClearFileButton"
                            text: "清除文件"
                            enabled: root.excelVbaImportPreviewSelectedFileName.length > 0
                            onClicked: {
                                root.excelVbaImportPersistConfirmed = false
                                root.excelVbaImportPreviewSelectedFileUrl = ""
                                root.excelVbaImportPreviewSelectedFileName = ""
                            }
                        }
                    }

                    Text {
                        objectName: "shellAccountingExcelVbaImportPreviewSelectedFileText"
                        width: parent.width
                        text: root.excelVbaImportPreviewSelectedFileName.length > 0
                            ? "已选择文件：" + root.excelVbaImportPreviewSelectedFileName
                            : "已选择文件：无"
                        color: "#465066"
                        font.pixelSize: 13
                        elide: Text.ElideRight
                    }

                    Rectangle {
                        objectName: "shellAccountingExcelVbaImportPreviewFormatContractPanel"
                        width: parent.width
                        height: 58
                        radius: 8
                        color: "#fffaf0"
                        border.color: "#ead6a5"

                        Text {
                            objectName: "shellAccountingExcelVbaImportPreviewFormatContractText"
                            anchors.fill: parent
                            anchors.margins: 10
                            text: "格式：schemaVersion=excel-vba-export/v1；source=sanitized-excel-vba-export；sheets[{sheetName,name,headers,rows}]。保留 sheetName 以兼容解析器。"
                            color: "#5f4b1f"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Rectangle {
                        objectName: "shellAccountingExcelVbaImportPreviewStatusPanel"
                        width: parent.width
                        height: 270
                        radius: 8
                        color: "#f8fbff"
                        border.color: "#d9e3f2"

                        Column {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 7

                            Text {
                                objectName: "shellAccountingExcelVbaPreviewStatusText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "预览状态：" + root.displayStatus(accountingPresenter.lastExcelVbaImportPreviewStatus)
                                    : "预览状态：不可用"
                                color: "#18202f"
                                font.pixelSize: 13
                                font.bold: true
                                wrapMode: Text.WordWrap
                            }

                            Item {
                                objectName: "shellAccountingExcelVbaImportPreviewStatusText"
                                visible: false
                                width: 0
                                height: 0
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPreviewAcceptanceStateText"
                                width: parent.width
                                text: "验收状态：" + root.displayStatus(root.excelVbaImportPreviewAcceptanceState())
                                    + "（就绪 READY、已选文件 FILE_SELECTED、预览中 PREVIEWING、已通过 ACCEPTED、已拒绝 REJECTED、输入错误 INPUT_ERROR）"
                                color: "#18202f"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPreviewAcceptanceSummaryText"
                                width: parent.width
                                text: root.excelVbaImportPreviewAcceptanceSummary()
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPreviewIssueText"
                                width: parent.width
                                text: root.presenterAvailable ? accountingPresenter.lastExcelVbaImportPreviewIssue : ""
                                color: "#9a3412"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                                visible: text.length > 0
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPreviewSummaryText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "汇总：" + accountingPresenter.lastExcelVbaImportPreviewSummary
                                    : "汇总：不可用"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPreviewDiagnosticCodesText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "诊断代码：" + accountingPresenter.lastExcelVbaImportPreviewDiagnosticCodes
                                    : "诊断代码：不可用"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Row {
                                width: parent.width
                                spacing: 14

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPreviewTradeFactCountText"
                                    width: 124
                                    text: root.presenterAvailable
                                        ? "交易：" + accountingPresenter.excelVbaImportPreviewTradeFactCount
                                        : "交易：0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPreviewCashFactCountText"
                                    width: 116
                                    text: root.presenterAvailable
                                        ? "现金：" + accountingPresenter.excelVbaImportPreviewCashFactCount
                                        : "现金：0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPreviewMarketPriceFactCountText"
                                    width: 146
                                    text: root.presenterAvailable
                                        ? "行情价格：" + accountingPresenter.excelVbaImportPreviewMarketPriceFactCount
                                        : "行情价格：0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPreviewFxRateFactCountText"
                                    width: 116
                                    text: root.presenterAvailable
                                        ? "汇率：" + accountingPresenter.excelVbaImportPreviewFxRateFactCount
                                        : "汇率：0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }
                            }
                        }
                    }

                    Rectangle {
                        objectName: "shellAccountingExcelVbaImportPersistPanel"
                        width: parent.width
                        height: 326
                        radius: 8
                        color: "#fbfcfe"
                        border.color: "#d9e3f2"

                        Column {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 8

                            CheckBox {
                                id: excelVbaImportPersistConfirmCheck
                                objectName: "shellAccountingExcelVbaPersistConfirmationCheckBox"
                                text: "我确认这个已通过的预览可以写入为手工记录。"
                                enabled: root.presenterAvailable
                                    && accountingPresenter.lastExcelVbaImportPreviewStatus === "ACCEPTED"
                                    && !accountingPresenter.excelVbaImportPersistBusy
                                checked: root.excelVbaImportPersistConfirmed
                                onCheckedChanged: root.excelVbaImportPersistConfirmed = checked
                            }

                            Item {
                                objectName: "shellAccountingExcelVbaImportPersistConfirmCheck"
                                visible: false
                                width: 0
                                height: 0
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistGateDiagnosticsText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "写入门禁：预览通过=" + root.displayBool(accountingPresenter.lastExcelVbaImportPreviewStatus === "ACCEPTED")
                                        + "；Digest=" + (accountingPresenter.lastExcelVbaImportPreviewDigest.length > 0 ? "已生成" : "缺失")
                                        + "；Payload=" + (accountingPresenter.excelVbaImportPreviewPayloadAvailable ? "可写入" : "不可写入")
                                        + "；支持写入记录=" + (accountingPresenter.excelVbaImportPreviewTradeFactCount + accountingPresenter.excelVbaImportPreviewCashFactCount)
                                        + "；行情记录=" + accountingPresenter.excelVbaImportPreviewMarketPriceFactCount
                                        + "；汇率记录=" + accountingPresenter.excelVbaImportPreviewFxRateFactCount
                                        + "；用户确认=" + root.displayBool(root.excelVbaImportPersistConfirmed)
                                        + "；写入忙碌=" + root.displayBool(accountingPresenter.excelVbaImportPersistBusy)
                                        + "；可写入=" + root.displayBool(root.excelVbaImportPersistReady())
                                    : "写入门禁：不可用；可写入=否；Digest=缺失；Payload=不可写入；用户确认=否；写入忙碌=否"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Row {
                                width: parent.width
                                spacing: 12

                                Button {
                                    objectName: "shellAccountingExcelVbaImportPersistButton"
                                    text: "确认写入已通过预览"
                                    enabled: root.excelVbaImportPersistReady()
                                    onClicked: accountingPresenter.persistAcceptedExcelVbaImportPreview()
                                }

                                Item {
                                    objectName: "shellAccountingExcelVbaPersistButton"
                                    visible: false
                                    width: 0
                                    height: 0
                                }

                                Text {
                                objectName: "shellAccountingExcelVbaImportPersistStatusText"
                                width: 220
                                text: root.presenterAvailable
                                     ? "写入状态：" + root.displayStatus(accountingPresenter.lastExcelVbaImportPersistStatus)
                                        : "写入状态：不可用"
                                    color: "#18202f"
                                    font.pixelSize: 13
                                    font.bold: true
                                    verticalAlignment: Text.AlignVCenter
                                    height: 34
                                    wrapMode: Text.WordWrap
                                }
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistStateGuideText"
                                width: parent.width
                                text: "写入状态：已写入表示交易/现金事实已持久化；重复提交表示没有新增行；幂等冲突表示请求被拒绝且不会刷新。"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistSummaryText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "写入汇总：" + accountingPresenter.lastExcelVbaImportPersistSummary
                                    : "写入汇总：不可用"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Row {
                                width: parent.width
                                spacing: 14

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPersistTradeLogRowsText"
                                    width: 220
                                    text: root.presenterAvailable
                                        ? "已写入交易记录行数：" + accountingPresenter.lastExcelVbaImportPersistTradeLogRowsWritten
                                        : "已写入交易记录行数：0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPersistCashAdjustmentRowsText"
                                    width: 260
                                    text: root.presenterAvailable
                                        ? "已写入现金调整行数：" + accountingPresenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten
                                        : "已写入现金调整行数：0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaPersistRowCountText"
                                    width: 220
                                    text: root.presenterAvailable
                                        ? "行数：交易行="
                                            + accountingPresenter.lastExcelVbaImportPersistTradeLogRowsWritten
                                            + "，现金调整="
                                            + accountingPresenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten
                                        : "行数：不可用"
                                    color: "#465066"
                                    font.pixelSize: 13
                                    wrapMode: Text.WordWrap
                                }
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistIssueCodesText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "写入诊断代码：" + accountingPresenter.lastExcelVbaImportPersistIssueCodes
                                    : "写入诊断代码：不可用"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistDuplicateText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "重复提交已阻止：" + root.displayBool(accountingPresenter.lastExcelVbaImportPersistDuplicateImportPrevented)
                                    : "重复提交已阻止：否"
                                color: "#465066"
                                font.pixelSize: 13
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistConflictText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "幂等冲突已拒绝：" + root.displayBool(accountingPresenter.lastExcelVbaImportPersistIdempotencyConflictRejected)
                                    : "幂等冲突已拒绝：否"
                                color: "#465066"
                                font.pixelSize: 13
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaPostWriteRefreshStatusText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "写入后刷新状态：" + root.displayStatus(accountingPresenter.lastPostWriteRefreshStatus)
                                    : "写入后刷新状态：不可用"
                                color: "#18202f"
                                font.pixelSize: 13
                                font.bold: true
                                wrapMode: Text.WordWrap
                            }

                            Item {
                                objectName: "shellAccountingExcelVbaImportPostPersistRefreshStatusText"
                                visible: false
                                width: 0
                                height: 0
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPostPersistRefreshSummaryText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "写入后刷新汇总：" + accountingPresenter.lastPostWriteRefreshSummary
                                    : "写入后刷新汇总：不可用"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPostPersistRefreshIssueText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "写入后刷新问题：" + accountingPresenter.lastPostWriteRefreshIssue
                                    : "写入后刷新问题：不可用"
                                color: "#9a3412"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                                visible: !root.presenterAvailable || accountingPresenter.lastPostWriteRefreshIssue.length > 0
                            }
                        }
                    }
                }
            }

            Item {
                objectName: "shellAccountingTradingUiSection"
                visible: false
                width: 0
                height: 0
            }

            Rectangle {
                objectName: "shellAccountingPortfolioReplayPanel"
                width: parent.width
                height: 300
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        width: parent.width
                        text: "持仓/现金重算只读汇总"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Text {
                            width: parent.width - 230
                            text: root.showReplayRawJson
                                ? "开发者详情：普通试用无需查看。此内容为样例载荷 / 原始诊断数据，不代表真实交易。"
                                : "样例重算请求已准备；默认隐藏原始 JSON，只显示中文重算摘要。"
                            color: "#5f4b1f"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }

                        Button {
                            id: rawReplayToggle
                            objectName: "shellAccountingShowReplayRawJsonButton"
                            text: root.showReplayRawJson ? "隐藏原始诊断数据" : "显示原始诊断数据"
                            onClicked: root.showReplayRawJson = !root.showReplayRawJson
                        }
                    }

                    TextArea {
                        id: portfolioReplayPayloadInput
                        objectName: "shellAccountingPortfolioReplayPayloadInput"
                        width: parent.width
                        height: root.showReplayRawJson ? 72 : 0
                        visible: root.showReplayRawJson
                        text: root.dashboardPortfolioReplaySamplePayload()
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingPortfolioReplayButton"
                            text: "重算"
                            enabled: root.presenterAvailable && !accountingPresenter.portfolioReplayBusy
                            onClicked: accountingPresenter.previewPortfolioReplayReadOnlySummary(
                                root.showReplayRawJson
                                    ? portfolioReplayPayloadInput.text
                                    : root.dashboardPortfolioReplaySamplePayload())
                        }

                        Button {
                            objectName: "shellAccountingPortfolioReplayResetButton"
                            text: "重置重算"
                            enabled: root.presenterAvailable
                            onClicked: accountingPresenter.resetPortfolioReplayState()
                        }
                    }

                    Text {
                        objectName: "shellAccountingPortfolioReplayStatusText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "重算状态：" + root.displayStatus(accountingPresenter.lastPortfolioReplayStatus)
                            : "重算状态：不可用"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingPortfolioReplayCashText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "现金汇总：" + accountingPresenter.portfolioReplayCashSummaryCount
                                + "；汇总=" + accountingPresenter.lastPortfolioReplaySummary
                            : "现金汇总：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingPortfolioReplayPositionText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "持仓：" + accountingPresenter.portfolioReplayPositionCount
                            : "持仓：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingPortfolioReplayPnlText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "盈亏：已实现=" + accountingPresenter.portfolioReplayRealizedPnl
                                + "，未实现=" + accountingPresenter.portfolioReplayUnrealizedPnl
                                + "，诊断代码=" + accountingPresenter.lastPortfolioReplayIssueCodes
                            : "盈亏：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingMarketDataPanel"
                width: parent.width
                height: 348
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        width: parent.width
                        text: "行情数据只读刷新"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Text {
                            width: parent.width - 230
                            text: root.showMarketRawJson
                                ? "开发者详情：普通试用无需查看。此内容为样例载荷 / 原始诊断数据，不代表真实交易。"
                                : "行情源：样例数据/禁用行情源；默认隐藏原始 JSON，不会自动联网。"
                            color: "#5f4b1f"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }

                        Button {
                            id: rawMarketToggle
                            objectName: "shellAccountingShowMarketRawJsonButton"
                            text: root.showMarketRawJson ? "隐藏原始诊断数据" : "显示原始诊断数据"
                            onClicked: root.showMarketRawJson = !root.showMarketRawJson
                        }
                    }

                    TextArea {
                        id: marketDataPayloadInput
                        objectName: "shellAccountingMarketDataPayloadInput"
                        width: parent.width
                        height: root.showMarketRawJson ? 72 : 0
                        visible: root.showMarketRawJson
                        text: root.dashboardMarketDataSamplePayload()
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingMarketDataRefreshButton"
                            text: "刷新行情数据"
                            enabled: root.presenterAvailable && !accountingPresenter.marketDataRefreshBusy
                            onClicked: accountingPresenter.refreshMarketDataReadOnly(
                                root.showMarketRawJson
                                    ? marketDataPayloadInput.text
                                    : root.dashboardMarketDataSamplePayload())
                        }

                        Button {
                            objectName: "shellAccountingMarketDataResetButton"
                            text: "重置行情"
                            enabled: root.presenterAvailable
                            onClicked: accountingPresenter.resetMarketDataState()
                        }
                    }

                    Text {
                        objectName: "shellAccountingMarketDataProviderStatusText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "行情源：" + accountingPresenter.lastMarketDataProviderSource
                                + "；质量=" + root.displayStatus(accountingPresenter.lastMarketDataQualityStatus)
                                + "；状态=" + root.displayStatus(accountingPresenter.lastMarketDataRefreshStatus)
                                + "。默认使用样例数据/禁用行情源，不会自动联网。"
                            : "行情源：不可用。默认使用样例数据/禁用行情源，不会自动联网。"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingMarketDataCurrentPriceText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "当前价格：" + accountingPresenter.lastMarketDataCurrentPriceText
                            : "当前价格：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingMarketDataHistoricalHighText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "历史高点：" + accountingPresenter.lastMarketDataHistoricalHighText
                                + "，日期 " + accountingPresenter.lastMarketDataHistoricalHighDate
                            : "历史高点：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingMarketDataDrawdownText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "较高点回撤：" + accountingPresenter.lastMarketDataDrawdownFromHighText
                            : "较高点回撤：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingMarketDataPremiumDiscountText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "溢价/折价：" + accountingPresenter.lastMarketDataPremiumDiscountText
                            : "溢价/折价：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingMarketDataIssueText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "行情诊断代码：" + accountingPresenter.lastMarketDataIssueCodes
                            : "行情诊断代码：不可用"
                        color: "#9a3412"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingStrategyRecommendationPanel"
                width: parent.width
                height: 330
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        width: parent.width
                        text: "策略建议只读汇总"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Text {
                            width: parent.width - 230
                            text: root.showStrategyRawJson
                                ? "开发者详情：普通试用无需查看。此内容为样例载荷 / 原始诊断数据，不代表真实交易。"
                                : "策略样例请求已准备；默认隐藏原始 JSON，只显示中文建议摘要。"
                            color: "#5f4b1f"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }

                        Button {
                            id: rawStrategyToggle
                            objectName: "shellAccountingShowStrategyRawJsonButton"
                            text: root.showStrategyRawJson ? "隐藏原始诊断数据" : "显示原始诊断数据"
                            onClicked: root.showStrategyRawJson = !root.showStrategyRawJson
                        }
                    }

                    TextArea {
                        id: strategyRecommendationPayloadInput
                        objectName: "shellAccountingStrategyRecommendationPayloadInput"
                        width: parent.width
                        height: root.showStrategyRawJson ? 72 : 0
                        visible: root.showStrategyRawJson
                        text: root.dashboardStrategySamplePayload()
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Button {
                        objectName: "shellAccountingStrategyRecommendationButton"
                        text: "刷新策略摘要"
                        enabled: root.presenterAvailable && !accountingPresenter.strategyRecommendationBusy
                        onClicked: {
                            root.tradeDraftCreateConfirmed = false
                            accountingPresenter.previewStrategyRecommendationReadOnlySummary(
                                root.showStrategyRawJson
                                    ? strategyRecommendationPayloadInput.text
                                    : root.dashboardStrategySamplePayload())
                        }
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationStatusText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "建议状态：" + root.displayStatus(accountingPresenter.lastStrategyRecommendationStatus)
                            : "建议状态：不可用"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationActionText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "动作：" + accountingPresenter.lastStrategyRecommendationAction
                                + "；来源=" + accountingPresenter.lastStrategyRecommendationSource
                                + "；档位=" + accountingPresenter.lastStrategyRecommendationTier
                            : "动作：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationReasonText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "原因：" + accountingPresenter.lastStrategyRecommendationReason
                                + "；诊断代码=" + accountingPresenter.lastStrategyRecommendationIssueCodes
                            : "原因：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationQuantityText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "数量：" + accountingPresenter.lastStrategyRecommendationQuantityText
                            : "数量：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationAmountText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "金额：" + accountingPresenter.lastStrategyRecommendationAmountText
                                + "；净现金影响=" + accountingPresenter.lastStrategyRecommendationNetCashImpactText
                            : "金额：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingTradeDraftPanel"
                width: parent.width
                height: 292
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        width: parent.width
                        text: "交易草案"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Text {
                        width: parent.width
                        text: "这是内部草案，不是订单，不会提交券商。"
                        color: "#5f4b1f"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingTradeDraftPreviewButton"
                            text: "预览交易草案"
                            enabled: root.presenterAvailable
                            onClicked: {
                                root.tradeDraftCreateConfirmed = false
                                accountingPresenter.previewTradeDraftFromLastRecommendation()
                            }
                        }

                        CheckBox {
                            id: tradeDraftConfirmCheck
                            objectName: "shellAccountingTradeDraftConfirmationCheckBox"
                            text: "我确认这只是内部草案，不是订单。"
                            checked: root.tradeDraftCreateConfirmed
                            enabled: root.presenterAvailable
                                && accountingPresenter.lastTradeDraftStatus === "READY_FOR_CONFIRMATION"
                            onCheckedChanged: root.tradeDraftCreateConfirmed = checked
                        }

                        Button {
                            objectName: "shellAccountingTradeDraftCreateButton"
                            text: "创建草案"
                            enabled: root.presenterAvailable
                                && root.tradeDraftCreateConfirmed
                                && accountingPresenter.lastTradeDraftStatus === "READY_FOR_CONFIRMATION"
                            onClicked: accountingPresenter.createTradeDraftFromLastRecommendation(true)
                        }
                    }

                    Text {
                        objectName: "shellAccountingTradeDraftStatusText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "草案状态：" + root.displayStatus(accountingPresenter.lastTradeDraftStatus)
                                + "；重复提交=" + root.displayBool(accountingPresenter.lastTradeDraftDuplicate)
                                + "；幂等冲突=" + root.displayBool(accountingPresenter.lastTradeDraftIdempotencyConflict)
                            : "草案状态：不可用"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingTradeDraftSummaryText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "草案汇总：" + accountingPresenter.lastTradeDraftSummary
                                + "；ID=" + accountingPresenter.lastTradeDraftId
                            : "草案汇总：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingOtcMapPanel"
                width: parent.width
                height: 360
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        width: parent.width
                        text: "场外 A/C 多通道草案"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Text {
                            width: parent.width - 230
                            text: root.showOtcMapRawJson
                                ? "开发者详情：普通试用无需查看。此内容为样例载荷 / 原始诊断数据，不代表真实交易。"
                                : "场外通道样例请求已准备；默认隐藏原始 JSON，只显示中文草案摘要。"
                            color: "#5f4b1f"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }

                        Button {
                            id: rawOtcMapToggle
                            objectName: "shellAccountingShowOtcMapRawJsonButton"
                            text: root.showOtcMapRawJson ? "隐藏原始诊断数据" : "显示原始诊断数据"
                            onClicked: root.showOtcMapRawJson = !root.showOtcMapRawJson
                        }
                    }

                    TextArea {
                        id: otcMapPayloadInput
                        objectName: "shellAccountingOtcMapPayloadInput"
                        width: parent.width
                        height: root.showOtcMapRawJson ? 74 : 0
                        visible: root.showOtcMapRawJson
                        text: root.dashboardOtcMapSamplePayload()
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingOtcMapPreviewButton"
                            text: "预览场外多通道"
                            enabled: root.presenterAvailable
                            onClicked: {
                                root.otcMapDraftCreateConfirmed = false
                                accountingPresenter.previewOtcMapMultiChannelDraft(
                                    root.showOtcMapRawJson
                                        ? otcMapPayloadInput.text
                                        : root.dashboardOtcMapSamplePayload())
                            }
                        }

                        CheckBox {
                            id: otcMapConfirmCheck
                            objectName: "shellAccountingOtcMapConfirmationCheckBox"
                            text: "我确认这个多腿结果只是内部草案，不是订单。"
                            checked: root.otcMapDraftCreateConfirmed
                            enabled: root.presenterAvailable
                                && accountingPresenter.lastOtcMapPreviewStatus === "DRAFT_ELIGIBLE"
                            onCheckedChanged: root.otcMapDraftCreateConfirmed = checked
                        }

                        Button {
                            objectName: "shellAccountingOtcMapCreateDraftButton"
                            text: "创建场外多通道草案"
                            enabled: root.presenterAvailable
                                && root.otcMapDraftCreateConfirmed
                                && accountingPresenter.lastOtcMapPreviewStatus === "DRAFT_ELIGIBLE"
                            onClicked: accountingPresenter.createOtcMapMultiChannelTradeDraft(true)
                        }
                    }

                    Text {
                        objectName: "shellAccountingOtcMapLegCountText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "预览腿数：" + accountingPresenter.lastOtcMapPreviewLegCount
                                + "；草案腿数=" + accountingPresenter.lastOtcMapDraftLegCount
                            : "预览腿数：不可用"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingOtcMapSummaryText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "场外基金映射汇总：" + accountingPresenter.lastOtcMapPreviewSummary
                                + "；草案=" + accountingPresenter.lastOtcMapDraftSummary
                            : "场外基金映射汇总：不可用"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingOtcMapIssueText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "场外基金映射诊断代码：预览=" + accountingPresenter.lastOtcMapPreviewIssueCodes
                                + "；草案=" + accountingPresenter.lastOtcMapDraftIssueCodes
                                + "；重复提交=" + root.displayBool(accountingPresenter.lastOtcMapDraftDuplicate)
                                + "；幂等冲突=" + root.displayBool(accountingPresenter.lastOtcMapDraftIdempotencyConflict)
                            : "场外基金映射诊断代码：不可用"
                        color: "#9a3412"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingTradingUiSection"
                width: parent.width
                height: 360
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    Row {
                        width: parent.width
                        spacing: 12

                        TextField {
                            id: accountInput
                            objectName: "shellAccountingDraftAccountInput"
                            width: 130
                            placeholderText: "账户"
                            text: "1001"
                        }

                        TextField {
                            id: portfolioInput
                            objectName: "shellAccountingDraftPortfolioInput"
                            width: 130
                            placeholderText: "组合"
                            text: "1001"
                        }

                        TextField {
                            id: instrumentInput
                            objectName: "shellAccountingDraftInstrumentInput"
                            width: 130
                            placeholderText: "标的"
                            text: "1001"
                        }

                        TextField {
                            id: instrumentCodeInput
                            objectName: "shellAccountingDraftInstrumentCodeInput"
                            width: 130
                            placeholderText: "代码"
                            text: "159509"
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        ComboBox {
                            id: sideInput
                            objectName: "shellAccountingDraftSideInput"
                            width: 130
                            model: ["BUY", "SELL"]
                        }

                        TextField {
                            id: quantityInput
                            objectName: "shellAccountingDraftQuantityInput"
                            width: 130
                            placeholderText: "数量"
                            text: "1"
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                        }

                        TextField {
                            id: reasonInput
                            objectName: "shellAccountingDraftReasonInput"
                            width: parent.width - sideInput.width - quantityInput.width - 36
                            placeholderText: "原因"
                            text: "调仓草案"
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingCreateDraftButton"
                            text: "创建草案"
                            enabled: root.presenterAvailable
                                && accountingPresenter.productionTradingUiEnabled
                            onClicked: {
                                confirmationCheck.checked = false
                                accountingPresenter.createDraft(
                                    accountInput.text,
                                    portfolioInput.text,
                                    instrumentInput.text,
                                    instrumentCodeInput.text,
                                    sideInput.currentText,
                                    quantityInput.text,
                                    reasonInput.text)
                            }
                        }

                        CheckBox {
                            id: confirmationCheck
                            objectName: "shellAccountingConfirmReviewCheckBox"
                            text: "我已复核此草案"
                            enabled: root.presenterAvailable
                                && accountingPresenter.draftId.length > 0
                                && accountingPresenter.ledgerStatus !== "CONFIRMED_LEDGER"
                        }

                        Button {
                            objectName: "shellAccountingConfirmDraftButton"
                            text: "确认草案"
                            enabled: root.presenterAvailable
                                && confirmationCheck.checked
                                && accountingPresenter.draftId.length > 0
                                && accountingPresenter.ledgerStatus !== "CONFIRMED_LEDGER"
                            onClicked: accountingPresenter.confirmDraft()
                        }
                    }

                    Rectangle {
                        objectName: "shellAccountingDraftReviewPanel"
                        width: parent.width
                        height: 132
                        radius: 8
                        color: "#f8fbff"
                        border.color: "#d9e3f2"

                        Column {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 8

                            Text {
                                objectName: "shellAccountingDraftOnlyState"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "草案占位状态：" + (accountingPresenter.draftId.length > 0 ? accountingPresenter.draftId : "无")
                                    : "草案占位状态：不可用"
                                color: "#18202f"
                                font.pixelSize: 14
                                font.bold: true
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingLedgerState"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "台账状态：" + accountingPresenter.ledgerStatus
                                    : "台账状态：不可用"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingTradingUiStatus"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "界面状态：" + accountingPresenter.tradingUiStatus
                                    : "界面状态：不可用"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingTradingUiIssue"
                                width: parent.width
                                text: root.presenterAvailable ? accountingPresenter.tradingUiIssue : ""
                                color: "#9a3412"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                                visible: text.length > 0
                            }
                        }
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingManualEntrySection"
                width: parent.width
                height: 650
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12

                    Text {
                        objectName: "shellAccountingManualEntryTitle"
                        width: parent.width
                        text: "手工录入"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        TextField {
                            id: manualTransactionAccountInput
                            objectName: "shellAccountingManualTransactionAccountInput"
                            width: 108
                            placeholderText: "账户"
                            text: "1001"
                        }

                        TextField {
                            id: manualTransactionPortfolioInput
                            objectName: "shellAccountingManualTransactionPortfolioInput"
                            width: 108
                            placeholderText: "组合"
                            text: "1001"
                        }

                        TextField {
                            id: manualTransactionInstrumentInput
                            objectName: "shellAccountingManualTransactionInstrumentInput"
                            width: 108
                            placeholderText: "标的"
                            text: "1001"
                        }

                        TextField {
                            id: manualTransactionSecurityInput
                            objectName: "shellAccountingManualTransactionSecurityInput"
                            width: 108
                            placeholderText: "证券代码"
                            text: "159509"
                        }

                        ComboBox {
                            id: manualTransactionSideInput
                            objectName: "shellAccountingManualTransactionSideInput"
                            width: 108
                            model: ["BUY", "SELL"]
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        TextField {
                            id: manualTransactionQuantityInput
                            objectName: "shellAccountingManualTransactionQuantityInput"
                            width: 108
                            placeholderText: "数量"
                            text: "1000000"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualTransactionPriceInput
                            objectName: "shellAccountingManualTransactionPriceInput"
                            width: 108
                            placeholderText: "价格（最小单位）"
                            text: "20100"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualTransactionGrossInput
                            objectName: "shellAccountingManualTransactionGrossInput"
                            width: 108
                            placeholderText: "成交额（最小单位）"
                            text: "20100"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualTransactionFeeInput
                            objectName: "shellAccountingManualTransactionFeeInput"
                            width: 108
                            placeholderText: "费用（最小单位）"
                            text: "100"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualTransactionTaxInput
                            objectName: "shellAccountingManualTransactionTaxInput"
                            width: 108
                            placeholderText: "税费（最小单位）"
                            text: "20"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        TextField {
                            id: manualTransactionOccurredAtInput
                            objectName: "shellAccountingManualTransactionOccurredAtInput"
                            width: 176
                            placeholderText: "发生时间"
                            text: "2026-06-02T12:34:56Z"
                        }

                        TextField {
                            id: manualTransactionMemoInput
                            objectName: "shellAccountingManualTransactionMemoInput"
                            width: 190
                            placeholderText: "备注"
                            text: "手工界面交易"
                        }

                        TextField {
                            id: manualTransactionRequestInput
                            objectName: "shellAccountingManualTransactionRequestInput"
                            width: 170
                            placeholderText: "请求 ID"
                            text: "task-200-manual-transaction"
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        TextField {
                            id: manualTransactionIdempotencyInput
                            objectName: "shellAccountingManualTransactionIdempotencyInput"
                            width: 248
                            placeholderText: "幂等键"
                            text: "task-200-manual-transaction-key"
                        }

                        Button {
                            objectName: "shellAccountingSubmitManualTransactionButton"
                            text: "提交交易"
                            enabled: root.presenterAvailable && !accountingPresenter.manualEntryBusy
                            onClicked: accountingPresenter.submitManualTransaction(
                                manualTransactionAccountInput.text,
                                manualTransactionPortfolioInput.text,
                                manualTransactionInstrumentInput.text,
                                manualTransactionSecurityInput.text,
                                manualTransactionSideInput.currentText,
                                manualTransactionQuantityInput.text,
                                manualTransactionPriceInput.text,
                                manualTransactionGrossInput.text,
                                manualTransactionFeeInput.text,
                                manualTransactionTaxInput.text,
                                manualTransactionOccurredAtInput.text,
                                manualTransactionMemoInput.text,
                                manualTransactionRequestInput.text,
                                manualTransactionIdempotencyInput.text)
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "#e5eaf2"
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        TextField {
                            id: manualCashAccountInput
                            objectName: "shellAccountingManualCashAccountInput"
                            width: 118
                            placeholderText: "账户"
                            text: "1001"
                        }

                        TextField {
                            id: manualCashPortfolioInput
                            objectName: "shellAccountingManualCashPortfolioInput"
                            width: 118
                            placeholderText: "组合"
                            text: "1001"
                        }

                        ComboBox {
                            id: manualCashMovementTypeInput
                            objectName: "shellAccountingManualCashMovementTypeInput"
                            width: 132
                            model: ["Deposit", "Withdrawal"]
                        }

                        TextField {
                            id: manualCashAmountInput
                            objectName: "shellAccountingManualCashAmountInput"
                            width: 118
                            placeholderText: "金额（最小单位）"
                            text: "12345"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualCashCurrencyInput
                            objectName: "shellAccountingManualCashCurrencyInput"
                            width: 82
                            placeholderText: "币种"
                            text: "CNY"
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        TextField {
                            id: manualCashOccurredAtInput
                            objectName: "shellAccountingManualCashOccurredAtInput"
                            width: 176
                            placeholderText: "发生时间"
                            text: "2026-06-02T12:34:56Z"
                        }

                        TextField {
                            id: manualCashMemoInput
                            objectName: "shellAccountingManualCashMemoInput"
                            width: 174
                            placeholderText: "备注"
                            text: "手工界面现金"
                        }

                        TextField {
                            id: manualCashReferenceInput
                            objectName: "shellAccountingManualCashReferenceInput"
                            width: 168
                            placeholderText: "参考号"
                            text: "bank-ref-task-200"
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        TextField {
                            id: manualCashRequestInput
                            objectName: "shellAccountingManualCashRequestInput"
                            width: 196
                            placeholderText: "请求 ID"
                            text: "task-200-manual-cash"
                        }

                        TextField {
                            id: manualCashIdempotencyInput
                            objectName: "shellAccountingManualCashIdempotencyInput"
                            width: 224
                            placeholderText: "幂等键"
                            text: "task-200-manual-cash-key"
                        }

                        Button {
                            objectName: "shellAccountingSubmitManualCashMovementButton"
                            text: "提交现金"
                            enabled: root.presenterAvailable && !accountingPresenter.manualEntryBusy
                            onClicked: accountingPresenter.submitManualCashMovement(
                                manualCashAccountInput.text,
                                manualCashPortfolioInput.text,
                                manualCashMovementTypeInput.currentText,
                                manualCashAmountInput.text,
                                manualCashCurrencyInput.text,
                                manualCashOccurredAtInput.text,
                                manualCashMemoInput.text,
                                manualCashReferenceInput.text,
                                manualCashRequestInput.text,
                                manualCashIdempotencyInput.text)
                        }
                    }

                    Rectangle {
                        objectName: "shellAccountingManualEntryStatusPanel"
                        width: parent.width
                        height: 178
                        radius: 8
                        color: "#f8fbff"
                        border.color: "#d9e3f2"

                        Column {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 6

                            Text {
                                objectName: "shellAccountingManualEntryStatusText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "手工录入状态：" + root.displayStatus(accountingPresenter.lastManualEntryStatus)
                                    : "手工录入状态：不可用"
                                color: "#18202f"
                                font.pixelSize: 13
                                font.bold: true
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingManualEntryResultText"
                                width: parent.width
                                text: root.presenterAvailable ? accountingPresenter.lastManualEntryResult : ""
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingManualEntryIssueText"
                                width: parent.width
                                text: root.presenterAvailable ? accountingPresenter.lastManualEntryIssue : ""
                                color: "#9a3412"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                                visible: text.length > 0
                            }

                            Text {
                                objectName: "shellAccountingPostWriteRefreshStatus"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "写入后刷新状态：" + root.displayStatus(accountingPresenter.lastPostWriteRefreshStatus)
                                    : "写入后刷新状态：不可用"
                                color: "#18202f"
                                font.pixelSize: 13
                                font.bold: true
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingPostWriteRefreshSummary"
                                width: parent.width
                                text: root.presenterAvailable ? accountingPresenter.lastPostWriteRefreshSummary : ""
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingPostWriteRefreshIssue"
                                width: parent.width
                                text: root.presenterAvailable ? accountingPresenter.lastPostWriteRefreshIssue : ""
                                color: "#9a3412"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                                visible: text.length > 0
                            }
                        }
                    }
                }
            }
        }
    }
}
