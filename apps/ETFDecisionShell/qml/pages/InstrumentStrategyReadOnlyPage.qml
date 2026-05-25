import QtQuick
import QtQuick.Controls
import "../components"
import "../components/readonly"

Rectangle {
    id: root
    objectName: "instrumentStrategyReadOnlyPage"
    required property var readOnlyDataController

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    property var summary: readOnlyDataController.summaryViewModel
    property var instrumentVisibleColumns: []
    property var strategyVisibleColumns: []
    property var otcVisibleColumns: []
    property string instrumentDensity: "normal"
    property string strategyDensity: "normal"
    property string otcDensity: "normal"
    property var instrumentColumns: [
        {"key": "code", "title": "Code", "width": 110, "required": true, "visible": true, "sortable": true},
        {"key": "name", "title": "Name", "width": 180, "required": false, "visible": true, "sortable": true},
        {"key": "type", "title": "Type", "width": 110, "required": false, "visible": true, "sortable": true},
        {"key": "market", "title": "Market", "width": 90, "required": false, "visible": true, "sortable": true},
        {"key": "currency", "title": "Currency", "width": 90, "required": false, "visible": true, "sortable": true},
        {"key": "status", "title": "Status", "width": 96, "required": false, "visible": true, "sortable": true}
    ]
    property var strategyColumns: [
        {"key": "code", "title": "Strategy code", "width": 180, "required": true, "visible": true, "sortable": true},
        {"key": "name", "title": "Name", "width": 260, "required": false, "visible": true, "sortable": true},
        {"key": "status", "title": "Status", "width": 96, "required": false, "visible": true, "sortable": true}
    ]
    property var otcColumns: [
        {"key": "strategy", "title": "Strategy", "width": 130, "required": false, "visible": false, "sortable": true},
        {"key": "code", "title": "Actual code", "width": 130, "required": true, "visible": true, "sortable": true},
        {"key": "type", "title": "Fund class", "width": 120, "required": false, "visible": true, "sortable": true},
        {"key": "status", "title": "Status", "width": 96, "required": false, "visible": true, "sortable": true},
        {"key": "dailyLimit", "title": "Daily limit", "width": 120, "required": false, "visible": true, "alignment": Text.AlignRight, "sortable": false},
        {"key": "priority", "title": "Priority", "width": 80, "required": false, "visible": true, "alignment": Text.AlignRight, "sortable": true},
        {"key": "amount", "title": "Min buy", "width": 120, "required": false, "visible": false, "alignment": Text.AlignRight, "sortable": true}
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

                    Row {
                        width: parent.width
                        spacing: 16

                        ReadOnlyFieldLabel {
                            width: 180
                            label: "Connection"
                            value: root.readOnlyDataController.connectionObject.stateText
                        }

                        ReadOnlyFieldLabel {
                            width: 150
                            label: "Refresh"
                            value: root.readOnlyDataController.refreshState
                        }

                        ReadOnlyFieldLabel {
                            width: 120
                            label: "Healthy"
                            value: String(root.summary.healthy)
                        }
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

            ReadOnlyFilterBar {
                objectName: "instrumentFilterBar"
                width: parent.width
                placeholderText: "Search instrument code, name, type, market, currency"
                showEnabledFilter: true
                onFilterTextChanged: function(text) {
                    root.readOnlyDataController.setInstrumentSearchText(text)
                }
                onEnabledOnlyChangedByUser: function(enabledOnly) {
                    root.readOnlyDataController.setInstrumentEnabledOnly(enabledOnly)
                }
                onClearRequested: {
                    root.readOnlyDataController.setInstrumentSearchText("")
                    root.readOnlyDataController.setInstrumentEnabledOnly(false)
                }
            }

            Row {
                width: parent.width
                spacing: 10

                ReadOnlyColumnChooser {
                    objectName: "instrumentColumnChooser"
                    columnObjectNamePrefix: "instrumentColumnChooserColumn"
                    width: Math.min(parent.width * 0.68, 760)
                    columns: root.instrumentColumns
                    visibleColumnKeys: root.instrumentVisibleColumns
                    title: "Instrument columns"
                    onColumnVisibilityChanged: function(keys) {
                        root.instrumentVisibleColumns = keys
                    }
                }

                ReadOnlyDensityToggle {
                    objectName: "instrumentDensityToggle"
                    width: Math.min(parent.width * 0.30, 340)
                    density: root.instrumentDensity
                    onDensityChangedByUser: function(density) {
                        root.instrumentDensity = density
                    }
                }
            }

            InstrumentListView {
                id: instrumentTable
                instrumentModel: root.readOnlyDataController.filteredInstrumentModel
                width: parent.width
                columns: root.instrumentColumns
                visibleColumnKeys: root.instrumentVisibleColumns
                density: root.instrumentDensity
                onSortRequested: function(key, ascending) {
                    root.readOnlyDataController.sortInstruments(key, ascending)
                }
            }

            ReadOnlyFilterBar {
                objectName: "strategyFilterBar"
                width: parent.width
                placeholderText: "Search strategy code or name"
                showEnabledFilter: true
                onFilterTextChanged: function(text) {
                    root.readOnlyDataController.setStrategySearchText(text)
                }
                onEnabledOnlyChangedByUser: function(enabledOnly) {
                    root.readOnlyDataController.setStrategyEnabledOnly(enabledOnly)
                }
                onClearRequested: {
                    root.readOnlyDataController.setStrategySearchText("")
                    root.readOnlyDataController.setStrategyEnabledOnly(false)
                }
            }

            Row {
                width: parent.width
                spacing: 10

                ReadOnlyColumnChooser {
                    objectName: "strategyColumnChooser"
                    columnObjectNamePrefix: "strategyColumnChooserColumn"
                    width: Math.min(parent.width * 0.68, 760)
                    columns: root.strategyColumns
                    visibleColumnKeys: root.strategyVisibleColumns
                    title: "Strategy columns"
                    onColumnVisibilityChanged: function(keys) {
                        root.strategyVisibleColumns = keys
                    }
                }

                ReadOnlyDensityToggle {
                    objectName: "strategyDensityToggle"
                    width: Math.min(parent.width * 0.30, 340)
                    density: root.strategyDensity
                    onDensityChangedByUser: function(density) {
                        root.strategyDensity = density
                    }
                }
            }

            StrategyListView {
                id: strategyTable
                strategyModel: root.readOnlyDataController.filteredStrategyModel
                width: parent.width
                columns: root.strategyColumns
                visibleColumnKeys: root.strategyVisibleColumns
                density: root.strategyDensity
                onSortRequested: function(key, ascending) {
                    root.readOnlyDataController.sortStrategies(key, ascending)
                }
            }

            ReadOnlyFilterBar {
                objectName: "otcFilterBar"
                width: parent.width
                placeholderText: "Search OTC strategy, actual code, fund class"
                showEnabledFilter: true
                onFilterTextChanged: function(text) {
                    root.readOnlyDataController.setOtcSearchText(text)
                }
                onEnabledOnlyChangedByUser: function(enabledOnly) {
                    root.readOnlyDataController.setOtcEnabledOnly(enabledOnly)
                }
                onClearRequested: {
                    root.readOnlyDataController.setOtcSearchText("")
                    root.readOnlyDataController.setOtcEnabledOnly(false)
                }
            }

            Row {
                width: parent.width
                spacing: 10

                ReadOnlyColumnChooser {
                    objectName: "otcColumnChooser"
                    columnObjectNamePrefix: "otcColumnChooserColumn"
                    width: Math.min(parent.width * 0.68, 760)
                    columns: root.otcColumns
                    visibleColumnKeys: root.otcVisibleColumns
                    title: "OTC columns"
                    onColumnVisibilityChanged: function(keys) {
                        root.otcVisibleColumns = keys
                    }
                }

                ReadOnlyDensityToggle {
                    objectName: "otcDensityToggle"
                    width: Math.min(parent.width * 0.30, 340)
                    density: root.otcDensity
                    onDensityChangedByUser: function(density) {
                        root.otcDensity = density
                    }
                }
            }

            OtcChannelListView {
                id: otcTable
                otcChannelModel: root.readOnlyDataController.filteredOtcChannelModel
                width: parent.width
                columns: root.otcColumns
                visibleColumnKeys: root.otcVisibleColumns
                density: root.otcDensity
                onSortRequested: function(key, ascending) {
                    root.readOnlyDataController.sortOtcChannels(key, ascending)
                }
            }
        }
    }
}
