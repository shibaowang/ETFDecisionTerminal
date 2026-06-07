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

    function excelVbaImportPreviewSamplePayload() {
        return JSON.stringify({
            "schemaVersion": "excel-vba-export/v1",
            "source": "sanitized-excel-vba-export",
            "sheets": [
                {
                    "name": "InitialCash",
                    "headers": ["ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO"],
                    "rows": [
                        ["TASK268_SAMPLE_CASH_001", "2026-01-05T09:00:00Z", "DEMO_ACCOUNT", "DEMO_PORTFOLIO", "INITIAL_CASH", "1000.00", "USD", "SANITIZED_SAMPLE_INITIAL_CASH"]
                    ]
                },
                {
                    "name": "TradeLog",
                    "headers": ["ROW_ID", "TRADE_TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "INSTRUMENT_CODE", "SIDE", "QUANTITY", "PRICE", "AMOUNT", "FEE", "CASH_FLOW", "CURRENCY", "SOURCE", "MEMO"],
                    "rows": [
                        ["TASK268_SAMPLE_TRADE_001", "2026-01-05T09:30:00Z", "DEMO_ACCOUNT", "DEMO_PORTFOLIO", "DEMO_ETF", "BUY", "10", "25.50", "255.00", "1.00", "-256.00", "USD", "SANITIZED_SAMPLE", "SANITIZED_SAMPLE_BUY"]
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

    function excelVbaImportPersistReady() {
        if (!root.presenterAvailable) {
            return false
        }
        var factCount = accountingPresenter.excelVbaImportPreviewTradeFactCount
            + accountingPresenter.excelVbaImportPreviewCashFactCount
            + accountingPresenter.excelVbaImportPreviewMarketPriceFactCount
            + accountingPresenter.excelVbaImportPreviewFxRateFactCount
        return accountingPresenter.lastExcelVbaImportPreviewStatus === "ACCEPTED"
            && accountingPresenter.lastExcelVbaImportPreviewDigest.length > 0
            && accountingPresenter.excelVbaImportPreviewPayloadAvailable
            && factCount > 0
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
            }

            Rectangle {
                objectName: "shellAccountingReadOnlyNotice"
                width: parent.width
                height: 54
                radius: 8
                color: "#eef4ff"
                border.color: "#a8c4f5"

                Text {
                    anchors.fill: parent
                    anchors.margins: 12
                    text: "read-only accounting remains available, with controlled draft and ledger confirmation entry points. Service errors remain visible, disabled state remains explicit, and unavailable-safe fallback does not use silent success."
                    color: "#244464"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
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
                            : "No accounting presenter context is available. The page shows Unavailable instead of data."
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
                height: 840
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

                    Text {
                        objectName: "shellAccountingExcelVbaImportPreviewTitle"
                        width: parent.width
                        text: "Excel/VBA import preview"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
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
                            objectName: "shellAccountingExcelVbaImportPreviewButton"
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
                            objectName: "shellAccountingExcelVbaImportPreviewLoadSampleButton"
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
                            text: "Format: schemaVersion=excel-vba-export/v1; source=sanitized-excel-vba-export; sheets[{name,headers,rows}]"
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
                                objectName: "shellAccountingExcelVbaImportPreviewStatusText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Preview status: " + accountingPresenter.lastExcelVbaImportPreviewStatus
                                    : "Preview status: unavailable"
                                color: "#18202f"
                                font.pixelSize: 13
                                font.bold: true
                                wrapMode: Text.WordWrap
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
                        height: 184
                        radius: 8
                        color: "#fbfcfe"
                        border.color: "#d9e3f2"

                        Column {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 8

                            CheckBox {
                                id: excelVbaImportPersistConfirmCheck
                                objectName: "shellAccountingExcelVbaImportPersistConfirmCheck"
                                text: "I confirm this accepted preview should be persisted as manual entries."
                                enabled: root.presenterAvailable
                                    && accountingPresenter.lastExcelVbaImportPreviewStatus === "ACCEPTED"
                                    && !accountingPresenter.excelVbaImportPersistBusy
                                checked: root.excelVbaImportPersistConfirmed
                                onCheckedChanged: root.excelVbaImportPersistConfirmed = checked
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
                                objectName: "shellAccountingExcelVbaImportPersistSummaryText"
                                width: parent.width
                                text: root.presenterAvailable
                                    ? "Persist summary: " + accountingPresenter.lastExcelVbaImportPersistSummary
                                    : "Persist summary: unavailable"
                                color: "#465066"
                                font.pixelSize: 13
                                wrapMode: Text.WordWrap
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
                        }
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
