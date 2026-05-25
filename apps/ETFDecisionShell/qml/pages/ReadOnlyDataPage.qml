import QtQuick
import QtQuick.Controls
import "../components"

Rectangle {
    id: root
    required property var readOnlyDataController

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    property var summary: readOnlyDataController.summaryViewModel

    Flickable {
        anchors.fill: parent
        anchors.margins: 20
        contentWidth: width
        contentHeight: contentColumn.implicitHeight
        clip: true

        Column {
            id: contentColumn
            width: parent.width
            spacing: 14

            Text {
                text: "只读数据预览"
                color: "#18202f"
                font.pixelSize: 24
                font.bold: true
            }

            Rectangle {
                width: parent.width
                height: 42
                radius: 8
                color: "#eef4ff"
                border.color: "#a8c4f5"

                Text {
                    anchors.fill: parent
                    anchors.margins: 10
                    text: "开发期只读原型：仅通过 ShellReadOnlyDataController 调用 DataService 只读接口；QML 不直接调用 DataServiceClient，不访问 SQLite，不调用写入 action。"
                    color: "#244464"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                objectName: "readonlyPresetPanel"
                width: parent.width
                height: 170
                radius: 8
                color: "#f8fbff"
                border.color: "#c8d7ee"

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Text {
                        text: "Read-only connection"
                        color: "#18202f"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Text {
                        width: parent.width
                        text: root.readOnlyDataController.onboardingText
                        color: "#56657c"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }

                    Row {
                        width: parent.width
                        spacing: 8

                        ComboBox {
                            id: presetCombo
                            objectName: "readonlyPresetCombo"
                            width: Math.min(300, parent.width * 0.28)
                            model: root.readOnlyDataController.connectionPresetModel
                            textRole: "title"
                            valueRole: "key"

                            function syncSelectedPreset() {
                                if (count <= 0) {
                                    return
                                }
                                for (var i = 0; i < count; ++i) {
                                    if (valueAt(i) === root.readOnlyDataController.selectedPresetKey) {
                                        currentIndex = i
                                        return
                                    }
                                }
                                currentIndex = 0
                                root.readOnlyDataController.selectPreset(valueAt(0))
                            }

                            onActivated: root.readOnlyDataController.selectPreset(currentValue)
                            onCountChanged: syncSelectedPreset()

                            Component.onCompleted: {
                                Qt.callLater(syncSelectedPreset)
                            }

                            Connections {
                                target: root.readOnlyDataController
                                function onConnectionPresetChanged() {
                                    presetCombo.syncSelectedPreset()
                                }
                            }
                        }

                        TextField {
                            id: socketNameField
                            objectName: "readonlySocketNameField"
                            width: Math.min(360, parent.width * 0.35)
                            placeholderText: "socketName"
                            selectByMouse: true
                            onTextEdited: root.readOnlyDataController.setCustomSocketName(text)

                            Component.onCompleted: {
                                text = root.readOnlyDataController.selectedSocketName
                            }

                            Connections {
                                target: root.readOnlyDataController
                                function onConnectionPresetChanged() {
                                    if (!socketNameField.activeFocus) {
                                        socketNameField.text = root.readOnlyDataController.selectedSocketName
                                    }
                                }
                            }
                        }

                        Button {
                            objectName: "readonlyConnectButton"
                            text: "Connect"
                            enabled: !root.readOnlyDataController.isBusy
                                && root.readOnlyDataController.selectedSocketName.length > 0
                            onClicked: root.readOnlyDataController.connectToDataService()
                        }

                        Button {
                            objectName: "readonlyDisconnectButton"
                            text: "Disconnect"
                            enabled: !root.readOnlyDataController.isBusy
                            onClicked: root.readOnlyDataController.disconnect()
                        }
                    }

                    Text {
                        objectName: "readonlyCommandHint"
                        width: parent.width
                        text: "Start command: " + root.readOnlyDataController.commandHint
                        color: "#244464"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 8

                Button {
                    objectName: "readonlyRefreshAllButton"
                    text: "Refresh All"
                    enabled: root.readOnlyDataController.canRefresh
                    onClicked: root.readOnlyDataController.refreshAll()
                }
            }

            Rectangle {
                width: parent.width
                height: 150
                radius: 8
                color: "#f8fafc"
                border.color: "#d8e0eb"

                Column {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 4

                    Text {
                        objectName: "readonlyConnectionState"
                        text: "Connection: " + root.readOnlyDataController.connectionObject.stateText
                            + " | socket=" + root.readOnlyDataController.connectionObject.socketName
                            + " | healthy=" + root.readOnlyDataController.connectionObject.healthy
                        color: "#26354d"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "readonlyRefreshState"
                        text: "Refresh: " + root.readOnlyDataController.refreshState
                            + " | busy=" + root.readOnlyDataController.isBusy
                            + " | canRefresh=" + root.readOnlyDataController.canRefresh
                            + " | attempts=" + root.readOnlyDataController.refreshAttemptCount
                            + " | ok=" + root.readOnlyDataController.refreshSuccessCount
                            + " | failed=" + root.readOnlyDataController.refreshFailureCount
                            + " | throttled=" + root.readOnlyDataController.refreshThrottleCount
                        color: root.readOnlyDataController.isRefreshing ? "#245a9f" : "#26354d"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }

                    Text {
                        objectName: "readonlyLastSuccess"
                        width: parent.width
                        text: root.readOnlyDataController.lastSuccessAtText.length > 0
                            ? "Last success: " + root.readOnlyDataController.lastSuccessAtText
                            : "Last success: none"
                        color: "#667086"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }

                    Text {
                        text: "Counts: accounts=" + root.summary.accountCount
                            + " portfolios=" + root.summary.portfolioCount
                            + " instruments=" + root.summary.instrumentCount
                            + " strategies=" + root.summary.strategyCount
                        color: "#26354d"
                        font.pixelSize: 13
                    }

                    Text {
                        width: parent.width
                        text: root.summary.summaryText.length > 0 ? root.summary.summaryText : "Summary: Empty / No data"
                        color: "#667086"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }

                    Text {
                        objectName: "readonlyLastError"
                        width: parent.width
                        text: root.readOnlyDataController.errorState.hasError
                            ? "Error: " + root.readOnlyDataController.errorState.errorCode
                                + " " + root.readOnlyDataController.errorState.errorMessage
                            : "Error: none"
                        color: root.readOnlyDataController.errorState.hasError ? "#9f2f2f" : "#667086"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }
            }

            Rectangle {
                objectName: "readonlyErrorPanel"
                width: parent.width
                height: root.readOnlyDataController.errorState.hasError ? 48 : 0
                visible: root.readOnlyDataController.errorState.hasError
                radius: 8
                color: "#fff2f2"
                border.color: "#e6a6a6"

                Text {
                    anchors.fill: parent
                    anchors.margins: 10
                    text: root.readOnlyDataController.errorState.errorMessage
                    color: "#8a2727"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Grid {
                objectName: "readonlyDataGrid"
                width: parent.width
                columns: width > 980 ? 2 : 1
                columnSpacing: 12
                rowSpacing: 12

                AccountListView {
                    width: (readonlyDataGrid.width - readonlyDataGrid.columnSpacing) / readonlyDataGrid.columns
                    accountModel: root.readOnlyDataController.accountModel
                }

                PortfolioListView {
                    width: (readonlyDataGrid.width - readonlyDataGrid.columnSpacing) / readonlyDataGrid.columns
                    portfolioModel: root.readOnlyDataController.portfolioModel
                }

                InstrumentListView {
                    width: (readonlyDataGrid.width - readonlyDataGrid.columnSpacing) / readonlyDataGrid.columns
                    instrumentModel: root.readOnlyDataController.instrumentModel
                }

                StrategyListView {
                    width: (readonlyDataGrid.width - readonlyDataGrid.columnSpacing) / readonlyDataGrid.columns
                    strategyModel: root.readOnlyDataController.strategyModel
                }
            }
        }
    }
}
