import QtQuick
import QtQuick.Controls
import "../components"
import "../components/readonly"

Rectangle {
    id: root
    objectName: "accountPortfolioReadOnlyPage"
    required property var readOnlyDataController

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    property var summary: readOnlyDataController.summaryViewModel
    property var accountVisibleColumns: []
    property var portfolioVisibleColumns: []
    property string accountDensity: "normal"
    property string portfolioDensity: "normal"
    property var accountColumns: [
        {"key": "name", "title": "名称", "width": 190, "required": true, "visible": true, "sortable": true},
        {"key": "type", "title": "类型", "width": 100, "required": false, "visible": true, "sortable": true},
        {"key": "broker", "title": "券商", "width": 140, "required": false, "visible": false, "sortable": false},
        {"key": "currency", "title": "币种", "width": 90, "required": false, "visible": true, "sortable": false},
        {"key": "status", "title": "状态", "width": 96, "required": false, "visible": true, "sortable": true},
        {"key": "amount", "title": "初始现金", "width": 130, "required": false, "visible": true, "alignment": Text.AlignRight, "sortable": true}
    ]
    property var portfolioColumns: [
        {"key": "name", "title": "名称", "width": 260, "required": true, "visible": true, "sortable": true},
        {"key": "amount", "title": "底仓比例", "width": 170, "required": false, "visible": true, "alignment": Text.AlignRight, "sortable": true},
        {"key": "status", "title": "状态", "width": 96, "required": false, "visible": true, "sortable": true}
    ]

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
                text: "账户与组合 - 只读"
                color: "#18202f"
                font.pixelSize: 24
                font.bold: true
            }

            Rectangle {
                objectName: "accountPortfolioReadonlyNotice"
                width: parent.width
                height: 46
                radius: 8
                color: "#eef4ff"
                border.color: "#a8c4f5"

                Text {
                    anchors.fill: parent
                    anchors.margins: 10
                    text: "当前页面为只读数据原型，不支持编辑、不支持入账、不支持交易；数据只能通过 ShellReadOnlyDataController 的只读模型展示。"
                    color: "#244464"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                objectName: "accountPortfolioConnectionPanel"
                width: parent.width
                height: 132
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

                    Row {
                        width: parent.width
                        spacing: 8

                        ComboBox {
                            id: presetCombo
                            objectName: "accountPortfolioPresetCombo"
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
                            objectName: "accountPortfolioSocketNameField"
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
                            objectName: "accountPortfolioConnectButton"
                            text: "连接"
                            enabled: !root.readOnlyDataController.isBusy
                                && root.readOnlyDataController.selectedSocketName.length > 0
                            onClicked: root.readOnlyDataController.connectToDataService()
                        }

                        Button {
                            objectName: "accountPortfolioRefreshButton"
                            text: "刷新账户与组合"
                            enabled: root.readOnlyDataController.canRefresh
                            onClicked: root.readOnlyDataController.refreshAccountsAndPortfolios()
                        }

                        Button {
                            objectName: "accountPortfolioDisconnectButton"
                            text: "断开连接"
                            enabled: !root.readOnlyDataController.isBusy
                            onClicked: root.readOnlyDataController.disconnect()
                        }
                    }

                    Text {
                        objectName: "accountPortfolioCommandHint"
                        width: parent.width
                        text: "启动命令：" + root.readOnlyDataController.commandHint
                        color: "#244464"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }
            }

            Rectangle {
                objectName: "accountPortfolioStatusPanel"
                width: parent.width
                height: 92
                radius: 8
                color: "#f9fbfd"
                border.color: "#d8e0eb"

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 5

                    Row {
                        width: parent.width
                        spacing: 16

                        ReadOnlyFieldLabel {
                            width: 180
                            label: "连接状态"
                            value: root.readOnlyDataController.connectionObject.stateText
                        }

                        ReadOnlyFieldLabel {
                            width: 150
                            label: "刷新状态"
                            value: root.readOnlyDataController.refreshState
                        }

                        ReadOnlyFieldLabel {
                            width: 120
                            label: "健康状态"
                            value: String(root.summary.healthy)
                        }
                    }

                    Text {
                        objectName: "accountPortfolioLastSuccess"
                        text: "最近成功：" + (root.readOnlyDataController.lastSuccessAtText.length > 0
                            ? root.readOnlyDataController.lastSuccessAtText : "从未")
                        color: "#56657c"
                        font.pixelSize: 12
                    }

                    Text {
                        objectName: "accountPortfolioErrorPanel"
                        width: parent.width
                        text: root.readOnlyDataController.errorState.hasError
                            ? "错误：" + root.readOnlyDataController.errorState.errorMessage
                            : "错误：无"
                        color: root.readOnlyDataController.errorState.hasError ? "#a33b2e" : "#56657c"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }
            }

            ReadOnlyFilterBar {
                objectName: "accountFilterBar"
                width: parent.width
                placeholderText: "搜索账户名称、类型、券商、币种"
                showEnabledFilter: true
                enabledOnlyText: "仅显示启用"
                onFilterTextChanged: function(text) {
                    root.readOnlyDataController.setAccountSearchText(text)
                }
                onEnabledOnlyChangedByUser: function(enabledOnly) {
                    root.readOnlyDataController.setAccountActiveOnly(enabledOnly)
                }
                onClearRequested: {
                    root.readOnlyDataController.setAccountSearchText("")
                    root.readOnlyDataController.setAccountActiveOnly(false)
                }
            }

            Row {
                width: parent.width
                spacing: 10

                ReadOnlyColumnChooser {
                    objectName: "accountColumnChooser"
                    columnObjectNamePrefix: "accountColumnChooserColumn"
                    width: Math.min(parent.width * 0.68, 760)
                    columns: root.accountColumns
                    visibleColumnKeys: root.accountVisibleColumns
                    title: "账户列"
                    onColumnVisibilityChanged: function(keys) {
                        root.accountVisibleColumns = keys
                    }
                }

                ReadOnlyDensityToggle {
                    objectName: "accountDensityToggle"
                    width: Math.min(parent.width * 0.30, 340)
                    density: root.accountDensity
                    onDensityChangedByUser: function(density) {
                        root.accountDensity = density
                    }
                }
            }

            AccountListView {
                id: accountTable
                accountModel: root.readOnlyDataController.filteredAccountModel
                width: parent.width
                columns: root.accountColumns
                visibleColumnKeys: root.accountVisibleColumns
                density: root.accountDensity
                onSortRequested: function(key, ascending) {
                    root.readOnlyDataController.sortAccounts(key, ascending)
                }
            }

            ReadOnlyFilterBar {
                objectName: "portfolioFilterBar"
                width: parent.width
                placeholderText: "搜索组合名称或底仓比例"
                showEnabledFilter: true
                enabledOnlyText: "仅显示启用"
                onFilterTextChanged: function(text) {
                    root.readOnlyDataController.setPortfolioSearchText(text)
                }
                onEnabledOnlyChangedByUser: function(enabledOnly) {
                    root.readOnlyDataController.setPortfolioActiveOnly(enabledOnly)
                }
                onClearRequested: {
                    root.readOnlyDataController.setPortfolioSearchText("")
                    root.readOnlyDataController.setPortfolioActiveOnly(false)
                }
            }

            Row {
                width: parent.width
                spacing: 10

                ReadOnlyColumnChooser {
                    objectName: "portfolioColumnChooser"
                    columnObjectNamePrefix: "portfolioColumnChooserColumn"
                    width: Math.min(parent.width * 0.68, 760)
                    columns: root.portfolioColumns
                    visibleColumnKeys: root.portfolioVisibleColumns
                    title: "组合列"
                    onColumnVisibilityChanged: function(keys) {
                        root.portfolioVisibleColumns = keys
                    }
                }

                ReadOnlyDensityToggle {
                    objectName: "portfolioDensityToggle"
                    width: Math.min(parent.width * 0.30, 340)
                    density: root.portfolioDensity
                    onDensityChangedByUser: function(density) {
                        root.portfolioDensity = density
                    }
                }
            }

            PortfolioListView {
                id: portfolioTable
                portfolioModel: root.readOnlyDataController.filteredPortfolioModel
                width: parent.width
                columns: root.portfolioColumns
                visibleColumnKeys: root.portfolioVisibleColumns
                density: root.portfolioDensity
                onSortRequested: function(key, ascending) {
                    root.readOnlyDataController.sortPortfolios(key, ascending)
                }
            }
        }
    }
}
