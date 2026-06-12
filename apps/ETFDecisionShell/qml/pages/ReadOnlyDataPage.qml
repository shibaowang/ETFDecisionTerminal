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
                    text: "开发期只读原型：仅通过只读控制器展示数据；界面不直接访问客户端、数据库或写入接口。"
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
                        text: "只读连接"
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
                            placeholderText: "本地套接字名称"
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
                            text: "连接"
                            enabled: !root.readOnlyDataController.isBusy
                                && root.readOnlyDataController.selectedSocketName.length > 0
                            onClicked: root.readOnlyDataController.connectToDataService()
                        }

                        Button {
                            objectName: "readonlyDisconnectButton"
                            text: "断开连接"
                            enabled: !root.readOnlyDataController.isBusy
                            onClicked: root.readOnlyDataController.disconnect()
                        }
                    }

                    Text {
                        objectName: "readonlyCommandHint"
                        width: parent.width
                        text: "启动命令：" + root.readOnlyDataController.commandHint
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
                    text: "刷新全部"
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
                        text: "连接状态：" + root.readOnlyDataController.connectionObject.stateText
                            + " | 本地套接字=" + root.readOnlyDataController.connectionObject.socketName
                            + " | 健康=" + root.readOnlyDataController.connectionObject.healthy
                        color: "#26354d"
                        font.pixelSize: 13
                    }

                    Text {
                        objectName: "readonlyRefreshState"
                        text: "刷新：" + root.readOnlyDataController.refreshState
                            + " | 忙碌=" + root.readOnlyDataController.isBusy
                            + " | 可刷新=" + root.readOnlyDataController.canRefresh
                            + " | 尝试=" + root.readOnlyDataController.refreshAttemptCount
                            + " | 成功=" + root.readOnlyDataController.refreshSuccessCount
                            + " | 失败=" + root.readOnlyDataController.refreshFailureCount
                            + " | 限流=" + root.readOnlyDataController.refreshThrottleCount
                        color: root.readOnlyDataController.isRefreshing ? "#245a9f" : "#26354d"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }

                    Text {
                        objectName: "readonlyLastSuccess"
                        width: parent.width
                        text: root.readOnlyDataController.lastSuccessAtText.length > 0
                            ? "最近成功：" + root.readOnlyDataController.lastSuccessAtText
                            : "最近成功：无"
                        color: "#667086"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }

                    Text {
                        text: "数量：账户=" + root.summary.accountCount
                            + "，组合=" + root.summary.portfolioCount
                            + "，标的=" + root.summary.instrumentCount
                            + "，策略=" + root.summary.strategyCount
                        color: "#26354d"
                        font.pixelSize: 13
                    }

                    Text {
                        width: parent.width
                        text: root.summary.summaryText.length > 0 ? root.summary.summaryText : "汇总：暂无数据"
                        color: "#667086"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }

                    Text {
                        objectName: "readonlyLastError"
                        width: parent.width
                        text: root.readOnlyDataController.errorState.hasError
                            ? "错误：" + root.readOnlyDataController.errorState.errorCode
                                + " " + root.readOnlyDataController.errorState.errorMessage
                            : "错误：无"
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
