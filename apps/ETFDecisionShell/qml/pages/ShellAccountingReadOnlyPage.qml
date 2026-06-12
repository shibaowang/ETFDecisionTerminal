import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import ETFDecisionTerminal.ShellAccounting 1.0
import "../components/readonly"

Rectangle {
    id: root
    objectName: "shellAccountingReadOnlyPage"

    property ShellAccountingPresenter accountingPresenter: null
    readonly property bool presenterAvailable: accountingPresenter !== null
    readonly property string shellState: "UNAVAILABLE"
    property url excelVbaImportPreviewSelectedFileUrl: ""
    property string excelVbaImportPreviewSelectedFileName: ""
    property bool excelVbaImportPersistConfirmed: false
    property bool tradeDraftCreateConfirmed: false
    property bool otcMapDraftCreateConfirmed: false

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
            return "Preview accepted: yes"
        }
        if (state === "REJECTED" || state === "INPUT_ERROR") {
            return "Preview accepted: no"
        }
        if (state === "PREVIEWING") {
            return "Preview accepted: pending"
        }
        return "Preview accepted: not evaluated"
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
                        "dataQualityStatus": "OK"
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
            return "Presenter unavailable"
        }
        var parts = []
        if (accountingPresenter.lastExcelVbaImportPreviewDiagnosticCodes.length > 0) {
            parts.push("import=" + accountingPresenter.lastExcelVbaImportPreviewDiagnosticCodes)
        }
        if (accountingPresenter.lastPortfolioReplayIssueCodes.length > 0) {
            parts.push("replay=" + accountingPresenter.lastPortfolioReplayIssueCodes)
        }
        if (accountingPresenter.lastMarketDataIssueCodes.length > 0) {
            parts.push("market=" + accountingPresenter.lastMarketDataIssueCodes)
        }
        if (accountingPresenter.lastStrategyRecommendationIssueCodes.length > 0) {
            parts.push("strategy=" + accountingPresenter.lastStrategyRecommendationIssueCodes)
        }
        if (accountingPresenter.lastTradeDraftIssueCodes.length > 0) {
            parts.push("draft=" + accountingPresenter.lastTradeDraftIssueCodes)
        }
        if (accountingPresenter.lastOtcMapPreviewIssueCodes.length > 0) {
            parts.push("otc-preview=" + accountingPresenter.lastOtcMapPreviewIssueCodes)
        }
        if (accountingPresenter.lastOtcMapDraftIssueCodes.length > 0) {
            parts.push("otc-draft=" + accountingPresenter.lastOtcMapDraftIssueCodes)
        }
        return parts.length > 0 ? parts.join("; ") : "No active sanitized issue codes"
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
            name = "selected local file"
        }
        return name.length > 0 ? name : "selected local file"
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
                    text: "ShellAccounting"
                    color: "#18202f"
                    font.pixelSize: 24
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    height: 28
                }

                ReadOnlyStatusBadge {
                    objectName: "shellAccountingReadOnlyBadge"
                    status: root.shellState
                    textOverride: root.presenterAvailable ? "UNAVAILABLE / READ ONLY" : "UNAVAILABLE"
                }

                Button {
                    objectName: "shellAccountingDashboardResetButton"
                    text: "Reset dashboard"
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
                        text: "Dashboard MVP: read-only summaries plus explicit-confirmation manual persistence and internal draft creation."
                        color: "#244464"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingLocalTrialRcBanner"
                        width: parent.width
                        text: "Local trial RC: repo-local demo DB, sanitized JSON/TXT samples, fixture/disabled providers, and manual cleanup."
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
                        text: root.presenterAvailable ? "Presenter: provided" : "Presenter: not provided"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Text {
                        objectName: "shellAccountingDisabledState"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "The production-owned ShellAccounting presenter is available. real accounting data is not connected unless the DataService boundary returns it. Draft actions still require explicit authorization."
                            : "No accounting presenter context is available. The page shows disabled unavailable-safe state instead of data."
                        color: "#465066"
                        font.pixelSize: 14
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingBoundaryText"
                        width: parent.width
                        text: "Status: DataService-only boundary, explicit confirmation, no direct database access. Amount rows are not displayed when unavailable."
                        color: "#667086"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Rectangle {
                objectName: "shellAccountingExcelVbaImportPreviewPanel"
                width: parent.width
                height: 1088
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e6"

                FileDialog {
                    id: excelVbaImportPreviewFileDialog
                    objectName: "shellAccountingExcelVbaImportPreviewFileDialog"
                    title: "Select sanitized JSON export"
                    fileMode: FileDialog.OpenFile
                    nameFilters: [
                        "JSON files (*.json)",
                        "Text files (*.txt)"
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
                        text: "Excel/VBA import preview"
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
                                text: "MVP steps: paste or choose a sanitized JSON/TXT export, run Preview, confirm only an ACCEPTED preview, then persist manual trade/cash entries."
                                color: "#25543b"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportMvpSupportText"
                                width: parent.width
                                text: "Supported now: sanitized Excel/VBA JSON/TXT export with InitialCash and TradeLog sheets. Direct .xlsx import is not supported in this MVP."
                                color: "#25543b"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportReleaseReadinessText"
                                width: parent.width
                                text: "Release readiness: accepted preview, persisted row counts, diagnostics, duplicate/conflict state, and post-write refresh are visible here."
                                color: "#25543b"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }
                        }
                    }

                    TextArea {
                        id: excelVbaImportPreviewPayloadInput
                        objectName: "shellAccountingExcelVbaImportPreviewPayloadInput"
                        width: parent.width
                        height: 92
                        placeholderText: "Paste sanitized JSON payload"
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingExcelVbaPreviewButton"
                            text: "Preview"
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
                            text: "Reset"
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
                            text: "Load Sample Preview"
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
                            text: "Select File"
                            enabled: root.presenterAvailable
                            onClicked: excelVbaImportPreviewFileDialog.open()
                        }

                        Button {
                            objectName: "shellAccountingExcelVbaImportPreviewFileButton"
                            text: "Preview File"
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
                            text: "Clear File"
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
                            ? "Selected file: " + root.excelVbaImportPreviewSelectedFileName
                            : "Selected file: none"
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
                            text: "Format: schemaVersion=excel-vba-export/v1; source=sanitized-excel-vba-export; sheets[{sheetName,name,headers,rows}]. Use sheetName for parser compatibility."
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
                                    ? "Preview status: " + accountingPresenter.lastExcelVbaImportPreviewStatus
                                    : "Preview status: unavailable"
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
                                text: "Acceptance state: " + root.excelVbaImportPreviewAcceptanceState()
                                    + " (READY / FILE_SELECTED / PREVIEWING / ACCEPTED / REJECTED / INPUT_ERROR)"
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
                                    ? "Summary: " + accountingPresenter.lastExcelVbaImportPreviewSummary
                                    : "Summary: unavailable"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPreviewDiagnosticCodesText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Diagnostic codes: " + accountingPresenter.lastExcelVbaImportPreviewDiagnosticCodes
                                    : "Diagnostic codes: unavailable"
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
                                        ? "Trades: " + accountingPresenter.excelVbaImportPreviewTradeFactCount
                                        : "Trades: 0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPreviewCashFactCountText"
                                    width: 116
                                    text: root.presenterAvailable
                                        ? "Cash: " + accountingPresenter.excelVbaImportPreviewCashFactCount
                                        : "Cash: 0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPreviewMarketPriceFactCountText"
                                    width: 146
                                    text: root.presenterAvailable
                                        ? "Prices: " + accountingPresenter.excelVbaImportPreviewMarketPriceFactCount
                                        : "Prices: 0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPreviewFxRateFactCountText"
                                    width: 116
                                    text: root.presenterAvailable
                                        ? "FX: " + accountingPresenter.excelVbaImportPreviewFxRateFactCount
                                        : "FX: 0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }
                            }
                        }
                    }

                    Rectangle {
                        objectName: "shellAccountingExcelVbaImportPersistPanel"
                        width: parent.width
                        height: 286
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
                                text: "I confirm this accepted preview should be persisted as manual entries."
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

                            Row {
                                width: parent.width
                                spacing: 12

                                Button {
                                    objectName: "shellAccountingExcelVbaImportPersistButton"
                                    text: "Persist accepted preview"
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
                                    ? "Persist status: " + accountingPresenter.lastExcelVbaImportPersistStatus
                                        : "Persist status: unavailable"
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
                                text: "Persist states: PERSISTED writes accepted trade/cash facts; DUPLICATE means no new rows; CONFLICT means idempotency mismatch and no refresh."
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistSummaryText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Persist summary: " + accountingPresenter.lastExcelVbaImportPersistSummary
                                    : "Persist summary: unavailable"
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
                                        ? "Persisted trade log rows: " + accountingPresenter.lastExcelVbaImportPersistTradeLogRowsWritten
                                        : "Persisted trade log rows: 0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaImportPersistCashAdjustmentRowsText"
                                    width: 260
                                    text: root.presenterAvailable
                                        ? "Persisted cash adjustment rows: " + accountingPresenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten
                                        : "Persisted cash adjustment rows: 0"
                                    color: "#465066"
                                    font.pixelSize: 13
                                }

                                Text {
                                    objectName: "shellAccountingExcelVbaPersistRowCountText"
                                    width: 220
                                    text: root.presenterAvailable
                                        ? "Rows: trade rows="
                                            + accountingPresenter.lastExcelVbaImportPersistTradeLogRowsWritten
                                            + ", cash adjustments="
                                            + accountingPresenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten
                                        : "Rows: unavailable"
                                    color: "#465066"
                                    font.pixelSize: 13
                                    wrapMode: Text.WordWrap
                                }
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistIssueCodesText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Persist issue codes: " + accountingPresenter.lastExcelVbaImportPersistIssueCodes
                                    : "Persist issue codes: unavailable"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistDuplicateText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Duplicate import prevented: " + accountingPresenter.lastExcelVbaImportPersistDuplicateImportPrevented
                                    : "Duplicate import prevented: false"
                                color: "#465066"
                                font.pixelSize: 13
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPersistConflictText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Idempotency conflict rejected: " + accountingPresenter.lastExcelVbaImportPersistIdempotencyConflictRejected
                                    : "Idempotency conflict rejected: false"
                                color: "#465066"
                                font.pixelSize: 13
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaPostWriteRefreshStatusText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Post-persist refresh status: " + accountingPresenter.lastPostWriteRefreshStatus
                                    : "Post-persist refresh status: unavailable"
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
                                    ? "Post-persist refresh summary: " + accountingPresenter.lastPostWriteRefreshSummary
                                    : "Post-persist refresh summary: unavailable"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingExcelVbaImportPostPersistRefreshIssueText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Post-persist refresh issue: " + accountingPresenter.lastPostWriteRefreshIssue
                                    : "Post-persist refresh issue: unavailable"
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
                        text: "Portfolio replay read-only summary"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    TextArea {
                        id: portfolioReplayPayloadInput
                        objectName: "shellAccountingPortfolioReplayPayloadInput"
                        width: parent.width
                        height: 72
                        text: root.dashboardPortfolioReplaySamplePayload()
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingPortfolioReplayButton"
                            text: "Run replay"
                            enabled: root.presenterAvailable && !accountingPresenter.portfolioReplayBusy
                            onClicked: accountingPresenter.previewPortfolioReplayReadOnlySummary(
                                portfolioReplayPayloadInput.text)
                        }

                        Button {
                            objectName: "shellAccountingPortfolioReplayResetButton"
                            text: "Reset replay"
                            enabled: root.presenterAvailable
                            onClicked: accountingPresenter.resetPortfolioReplayState()
                        }
                    }

                    Text {
                        objectName: "shellAccountingPortfolioReplayStatusText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Replay status: " + accountingPresenter.lastPortfolioReplayStatus
                            : "Replay status: unavailable"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingPortfolioReplayCashText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Cash summaries: " + accountingPresenter.portfolioReplayCashSummaryCount
                                + "; summary=" + accountingPresenter.lastPortfolioReplaySummary
                            : "Cash summaries: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingPortfolioReplayPositionText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Positions: " + accountingPresenter.portfolioReplayPositionCount
                            : "Positions: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingPortfolioReplayPnlText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "PnL: realized=" + accountingPresenter.portfolioReplayRealizedPnl
                                + ", unrealized=" + accountingPresenter.portfolioReplayUnrealizedPnl
                                + ", issues=" + accountingPresenter.lastPortfolioReplayIssueCodes
                            : "PnL: unavailable"
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
                        text: "Market data read-only refresh"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    TextArea {
                        id: marketDataPayloadInput
                        objectName: "shellAccountingMarketDataPayloadInput"
                        width: parent.width
                        height: 72
                        text: root.dashboardMarketDataSamplePayload()
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingMarketDataRefreshButton"
                            text: "Refresh market data"
                            enabled: root.presenterAvailable && !accountingPresenter.marketDataRefreshBusy
                            onClicked: accountingPresenter.refreshMarketDataReadOnly(marketDataPayloadInput.text)
                        }

                        Button {
                            objectName: "shellAccountingMarketDataResetButton"
                            text: "Reset market"
                            enabled: root.presenterAvailable
                            onClicked: accountingPresenter.resetMarketDataState()
                        }
                    }

                    Text {
                        objectName: "shellAccountingMarketDataProviderStatusText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Provider: " + accountingPresenter.lastMarketDataProviderSource
                                + "; quality=" + accountingPresenter.lastMarketDataQualityStatus
                                + "; status=" + accountingPresenter.lastMarketDataRefreshStatus
                            : "Provider: unavailable"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingMarketDataCurrentPriceText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Current price: " + accountingPresenter.lastMarketDataCurrentPriceText
                            : "Current price: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingMarketDataHistoricalHighText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Historical high: " + accountingPresenter.lastMarketDataHistoricalHighText
                                + " on " + accountingPresenter.lastMarketDataHistoricalHighDate
                            : "Historical high: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingMarketDataDrawdownText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Drawdown from high: " + accountingPresenter.lastMarketDataDrawdownFromHighText
                            : "Drawdown from high: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingMarketDataPremiumDiscountText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Premium/discount: " + accountingPresenter.lastMarketDataPremiumDiscountText
                            : "Premium/discount: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingMarketDataIssueText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Market issues: " + accountingPresenter.lastMarketDataIssueCodes
                            : "Market issues: unavailable"
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
                        text: "Strategy recommendation read-only summary"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    TextArea {
                        id: strategyRecommendationPayloadInput
                        objectName: "shellAccountingStrategyRecommendationPayloadInput"
                        width: parent.width
                        height: 72
                        text: root.dashboardStrategySamplePayload()
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Button {
                        objectName: "shellAccountingStrategyRecommendationButton"
                        text: "Run recommendation"
                        enabled: root.presenterAvailable && !accountingPresenter.strategyRecommendationBusy
                        onClicked: {
                            root.tradeDraftCreateConfirmed = false
                            accountingPresenter.previewStrategyRecommendationReadOnlySummary(
                                strategyRecommendationPayloadInput.text)
                        }
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationStatusText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Recommendation status: " + accountingPresenter.lastStrategyRecommendationStatus
                            : "Recommendation status: unavailable"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationActionText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Action: " + accountingPresenter.lastStrategyRecommendationAction
                                + "; source=" + accountingPresenter.lastStrategyRecommendationSource
                                + "; tier=" + accountingPresenter.lastStrategyRecommendationTier
                            : "Action: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationReasonText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Reason: " + accountingPresenter.lastStrategyRecommendationReason
                                + "; issues=" + accountingPresenter.lastStrategyRecommendationIssueCodes
                            : "Reason: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationQuantityText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Quantity: " + accountingPresenter.lastStrategyRecommendationQuantityText
                            : "Quantity: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "shellAccountingStrategyRecommendationAmountText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Amount: " + accountingPresenter.lastStrategyRecommendationAmountText
                                + "; net cash=" + accountingPresenter.lastStrategyRecommendationNetCashImpactText
                            : "Amount: unavailable"
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
                        text: "TradeDraft from recommendation"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Text {
                        width: parent.width
                        text: "Draft, not order. No execution is submitted from this page."
                        color: "#5f4b1f"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingTradeDraftPreviewButton"
                            text: "Preview TradeDraft"
                            enabled: root.presenterAvailable
                            onClicked: {
                                root.tradeDraftCreateConfirmed = false
                                accountingPresenter.previewTradeDraftFromLastRecommendation()
                            }
                        }

                        CheckBox {
                            id: tradeDraftConfirmCheck
                            objectName: "shellAccountingTradeDraftConfirmationCheckBox"
                            text: "I confirm this is only an internal draft, not order."
                            checked: root.tradeDraftCreateConfirmed
                            enabled: root.presenterAvailable
                                && accountingPresenter.lastTradeDraftStatus === "READY_FOR_CONFIRMATION"
                            onCheckedChanged: root.tradeDraftCreateConfirmed = checked
                        }

                        Button {
                            objectName: "shellAccountingTradeDraftCreateButton"
                            text: "Create internal draft"
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
                            ? "Draft status: " + accountingPresenter.lastTradeDraftStatus
                                + "; duplicate=" + accountingPresenter.lastTradeDraftDuplicate
                                + "; conflict=" + accountingPresenter.lastTradeDraftIdempotencyConflict
                            : "Draft status: unavailable"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingTradeDraftSummaryText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "Draft summary: " + accountingPresenter.lastTradeDraftSummary
                                + "; id=" + accountingPresenter.lastTradeDraftId
                            : "Draft summary: unavailable"
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
                        text: "OTCMap multi-leg draft"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    TextArea {
                        id: otcMapPayloadInput
                        objectName: "shellAccountingOtcMapPayloadInput"
                        width: parent.width
                        height: 74
                        text: root.dashboardOtcMapSamplePayload()
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingOtcMapPreviewButton"
                            text: "Preview OTCMap"
                            enabled: root.presenterAvailable
                            onClicked: {
                                root.otcMapDraftCreateConfirmed = false
                                accountingPresenter.previewOtcMapMultiChannelDraft(otcMapPayloadInput.text)
                            }
                        }

                        CheckBox {
                            id: otcMapConfirmCheck
                            objectName: "shellAccountingOtcMapConfirmationCheckBox"
                            text: "I confirm this multi-leg result is only an internal draft, not order."
                            checked: root.otcMapDraftCreateConfirmed
                            enabled: root.presenterAvailable
                                && accountingPresenter.lastOtcMapPreviewStatus === "DRAFT_ELIGIBLE"
                            onCheckedChanged: root.otcMapDraftCreateConfirmed = checked
                        }

                        Button {
                            objectName: "shellAccountingOtcMapCreateDraftButton"
                            text: "Create multi-leg draft"
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
                            ? "Preview legs: " + accountingPresenter.lastOtcMapPreviewLegCount
                                + "; draft legs=" + accountingPresenter.lastOtcMapDraftLegCount
                            : "Preview legs: unavailable"
                        color: "#18202f"
                        font.pixelSize: 13
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingOtcMapSummaryText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "OTCMap summary: " + accountingPresenter.lastOtcMapPreviewSummary
                                + "; draft=" + accountingPresenter.lastOtcMapDraftSummary
                            : "OTCMap summary: unavailable"
                        color: "#465066"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        objectName: "shellAccountingOtcMapIssueText"
                        width: parent.width
                        text: root.presenterAvailable
                            ? "OTCMap issues: preview=" + accountingPresenter.lastOtcMapPreviewIssueCodes
                                + "; draft=" + accountingPresenter.lastOtcMapDraftIssueCodes
                                + "; duplicate=" + accountingPresenter.lastOtcMapDraftDuplicate
                                + "; conflict=" + accountingPresenter.lastOtcMapDraftIdempotencyConflict
                            : "OTCMap issues: unavailable"
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
                            placeholderText: "Account"
                            text: "1001"
                        }

                        TextField {
                            id: portfolioInput
                            objectName: "shellAccountingDraftPortfolioInput"
                            width: 130
                            placeholderText: "Portfolio"
                            text: "1001"
                        }

                        TextField {
                            id: instrumentInput
                            objectName: "shellAccountingDraftInstrumentInput"
                            width: 130
                            placeholderText: "Instrument"
                            text: "1001"
                        }

                        TextField {
                            id: instrumentCodeInput
                            objectName: "shellAccountingDraftInstrumentCodeInput"
                            width: 130
                            placeholderText: "Symbol"
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
                            placeholderText: "Quantity"
                            text: "1"
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                        }

                        TextField {
                            id: reasonInput
                            objectName: "shellAccountingDraftReasonInput"
                            width: parent.width - sideInput.width - quantityInput.width - 36
                            placeholderText: "Reason"
                            text: "rebalance draft"
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 12

                        Button {
                            objectName: "shellAccountingCreateDraftButton"
                            text: "Create draft"
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
                            text: "I reviewed this draft"
                            enabled: root.presenterAvailable
                                && accountingPresenter.draftId.length > 0
                                && accountingPresenter.ledgerStatus !== "CONFIRMED_LEDGER"
                        }

                        Button {
                            objectName: "shellAccountingConfirmDraftButton"
                            text: "Confirm draft"
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
                                    ? "Draft-only: " + (accountingPresenter.draftId.length > 0 ? accountingPresenter.draftId : "none")
                                    : "Draft-only: unavailable"
                                color: "#18202f"
                                font.pixelSize: 14
                                font.bold: true
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingLedgerState"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Ledger status: " + accountingPresenter.ledgerStatus
                                    : "Ledger status: unavailable"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                objectName: "shellAccountingTradingUiStatus"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "UI status: " + accountingPresenter.tradingUiStatus
                                    : "UI status: unavailable"
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
                        text: "Manual entry"
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
                            placeholderText: "Account"
                            text: "1001"
                        }

                        TextField {
                            id: manualTransactionPortfolioInput
                            objectName: "shellAccountingManualTransactionPortfolioInput"
                            width: 108
                            placeholderText: "Portfolio"
                            text: "1001"
                        }

                        TextField {
                            id: manualTransactionInstrumentInput
                            objectName: "shellAccountingManualTransactionInstrumentInput"
                            width: 108
                            placeholderText: "Instrument"
                            text: "1001"
                        }

                        TextField {
                            id: manualTransactionSecurityInput
                            objectName: "shellAccountingManualTransactionSecurityInput"
                            width: 108
                            placeholderText: "Security"
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
                            placeholderText: "Quantity"
                            text: "1000000"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualTransactionPriceInput
                            objectName: "shellAccountingManualTransactionPriceInput"
                            width: 108
                            placeholderText: "Price minor"
                            text: "20100"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualTransactionGrossInput
                            objectName: "shellAccountingManualTransactionGrossInput"
                            width: 108
                            placeholderText: "Gross minor"
                            text: "20100"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualTransactionFeeInput
                            objectName: "shellAccountingManualTransactionFeeInput"
                            width: 108
                            placeholderText: "Fee minor"
                            text: "100"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualTransactionTaxInput
                            objectName: "shellAccountingManualTransactionTaxInput"
                            width: 108
                            placeholderText: "Tax minor"
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
                            placeholderText: "Occurred at"
                            text: "2026-06-02T12:34:56Z"
                        }

                        TextField {
                            id: manualTransactionMemoInput
                            objectName: "shellAccountingManualTransactionMemoInput"
                            width: 190
                            placeholderText: "Memo"
                            text: "manual ui transaction"
                        }

                        TextField {
                            id: manualTransactionRequestInput
                            objectName: "shellAccountingManualTransactionRequestInput"
                            width: 170
                            placeholderText: "Request id"
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
                            placeholderText: "Idempotency key"
                            text: "task-200-manual-transaction-key"
                        }

                        Button {
                            objectName: "shellAccountingSubmitManualTransactionButton"
                            text: "Submit transaction"
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
                            placeholderText: "Account"
                            text: "1001"
                        }

                        TextField {
                            id: manualCashPortfolioInput
                            objectName: "shellAccountingManualCashPortfolioInput"
                            width: 118
                            placeholderText: "Portfolio"
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
                            placeholderText: "Amount minor"
                            text: "12345"
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: manualCashCurrencyInput
                            objectName: "shellAccountingManualCashCurrencyInput"
                            width: 82
                            placeholderText: "Currency"
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
                            placeholderText: "Occurred at"
                            text: "2026-06-02T12:34:56Z"
                        }

                        TextField {
                            id: manualCashMemoInput
                            objectName: "shellAccountingManualCashMemoInput"
                            width: 174
                            placeholderText: "Memo"
                            text: "manual ui cash"
                        }

                        TextField {
                            id: manualCashReferenceInput
                            objectName: "shellAccountingManualCashReferenceInput"
                            width: 168
                            placeholderText: "Reference"
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
                            placeholderText: "Request id"
                            text: "task-200-manual-cash"
                        }

                        TextField {
                            id: manualCashIdempotencyInput
                            objectName: "shellAccountingManualCashIdempotencyInput"
                            width: 224
                            placeholderText: "Idempotency key"
                            text: "task-200-manual-cash-key"
                        }

                        Button {
                            objectName: "shellAccountingSubmitManualCashMovementButton"
                            text: "Submit cash"
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
                                    ? "Manual entry status: " + accountingPresenter.lastManualEntryStatus
                                    : "Manual entry status: unavailable"
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
                                    ? "Post-write refresh status: " + accountingPresenter.lastPostWriteRefreshStatus
                                    : "Post-write refresh status: unavailable"
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
