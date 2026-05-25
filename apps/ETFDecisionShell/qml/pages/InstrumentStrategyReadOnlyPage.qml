import QtQuick
import QtQuick.Controls
import "../components"

Rectangle {
    id: root
    objectName: "instrumentStrategyReadOnlyPage"
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
                text: "标的与策略 - 只读"
                color: "#18202f"
                font.pixelSize: 24
                font.bold: true
            }

            Rectangle {
                objectName: "instrumentStrategyReadonlyNotice"
                width: parent.width
                height: 46
                radius: 8
                color: "#eef4ff"
                border.color: "#a8c4f5"

                Text {
                    anchors.fill: parent
                    anchors.margins: 10
                    text: "当前页面为只读数据原型，不支持编辑、不支持策略执行、不支持交易；数据只能通过 ShellReadOnlyDataController 的只读模型展示。"
                    color: "#244464"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                objectName: "instrumentStrategyConnectionPanel"
                width: parent.width
                height: 172
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

                    Row {
                        width: parent.width
                        spacing: 8

                        ComboBox {
                            id: presetCombo
                            objectName: "instrumentStrategyPresetCombo"
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
                            Component.onCompleted: Qt.callLater(syncSelectedPreset)

                            Connections {
                                target: root.readOnlyDataController
                                function onConnectionPresetChanged() {
                                    presetCombo.syncSelectedPreset()
                                }
                            }
                        }

                        TextField {
                            id: socketNameField
                            objectName: "instrumentStrategySocketNameField"
                            width: Math.min(330, parent.width * 0.32)
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
                            objectName: "instrumentStrategyConnectButton"
                            text: "Connect"
                            enabled: !root.readOnlyDataController.isBusy
                                && root.readOnlyDataController.selectedSocketName.length > 0
                            onClicked: root.readOnlyDataController.connectToDataService()
                        }

                        Button {
                            objectName: "instrumentStrategyDisconnectButton"
                            text: "Disconnect"
                            enabled: !root.readOnlyDataController.isBusy
                            onClicked: root.readOnlyDataController.disconnect()
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 8

                        Button {
                            objectName: "instrumentStrategyRefreshButton"
                            text: "Refresh Instruments & Strategies"
                            enabled: root.readOnlyDataController.canRefresh
                            onClicked: root.readOnlyDataController.refreshInstrumentsAndStrategies()
                        }

                        ComboBox {
                            id: strategyCombo
                            objectName: "instrumentStrategyStrategyCombo"
                            width: Math.min(220, parent.width * 0.24)
                            model: root.readOnlyDataController.strategyModel
                            textRole: "strategyCode"
                            valueRole: "strategyCode"

                            function syncStrategyCode() {
                                if (count <= 0) {
                                    return
                                }
                                if (root.readOnlyDataController.selectedStrategyCode.length === 0) {
                                    currentIndex = 0
                                    root.readOnlyDataController.setSelectedStrategyCode(valueAt(0))
                                }
                            }

                            onActivated: root.readOnlyDataController.setSelectedStrategyCode(currentValue)
                            onCountChanged: syncStrategyCode()
                            Component.onCompleted: Qt.callLater(syncStrategyCode)
                        }

                        TextField {
                            id: strategyCodeField
                            objectName: "instrumentStrategyCodeField"
                            width: Math.min(220, parent.width * 0.22)
                            placeholderText: "strategyCode"
                            selectByMouse: true
                            text: root.readOnlyDataController.selectedStrategyCode
                            onTextEdited: root.readOnlyDataController.setSelectedStrategyCode(text)

                            Connections {
                                target: root.readOnlyDataController
                                function onSelectedStrategyCodeChanged() {
                                    if (!strategyCodeField.activeFocus) {
                                        strategyCodeField.text = root.readOnlyDataController.selectedStrategyCode
                                    }
                                }
                            }
                        }

                        Button {
                            objectName: "instrumentStrategyRefreshOtcButton"
                            text: "Refresh OTC"
                            enabled: root.readOnlyDataController.canRefresh
                            onClicked: root.readOnlyDataController.refreshOtcChannels()
                        }
                    }
                }
            }

            Rectangle {
                objectName: "instrumentStrategyStatusPanel"
                width: parent.width
                height: 92
                radius: 8
                color: "#f9fbfd"
                border.color: "#d8e0eb"

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 5

                    Text {
                        text: "Connection: " + root.readOnlyDataController.connectionObject.stateText
                            + " | refreshState: " + root.readOnlyDataController.refreshState
                            + " | healthy: " + root.summary.healthy
                    }

                    Text {
                        objectName: "instrumentStrategyLastSuccess"
                        text: "Last success: " + (root.readOnlyDataController.lastSuccessAtText.length > 0
                            ? root.readOnlyDataController.lastSuccessAtText : "never")
                        color: "#56657c"
                        font.pixelSize: 12
                    }

                    Text {
                        objectName: "instrumentStrategyErrorPanel"
                        width: parent.width
                        text: root.readOnlyDataController.errorState.hasError
                            ? "Error: " + root.readOnlyDataController.errorState.errorMessage
                            : "Error: none"
                        color: root.readOnlyDataController.errorState.hasError ? "#a33b2e" : "#56657c"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }
            }

            InstrumentListView {
                instrumentModel: root.readOnlyDataController.instrumentModel
                width: parent.width
            }

            StrategyListView {
                strategyModel: root.readOnlyDataController.strategyModel
                width: parent.width
            }

            OtcChannelListView {
                otcChannelModel: root.readOnlyDataController.otcChannelModel
                width: parent.width
            }
        }
    }
}
