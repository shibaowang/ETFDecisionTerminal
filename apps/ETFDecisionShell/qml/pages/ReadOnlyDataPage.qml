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

            Row {
                width: parent.width
                spacing: 8

                TextField {
                    id: socketNameField
                    objectName: "readonlySocketNameField"
                    width: Math.min(360, parent.width * 0.35)
                    text: "ETFDataServiceReadonly"
                    placeholderText: "socketName"
                    selectByMouse: true
                }

                Button {
                    text: "Connect"
                    onClicked: root.readOnlyDataController.connectToDataService(socketNameField.text)
                }

                Button {
                    text: "Refresh All"
                    onClicked: root.readOnlyDataController.refreshAll()
                }

                Button {
                    text: "Disconnect"
                    onClicked: root.readOnlyDataController.disconnect()
                }
            }

            Rectangle {
                width: parent.width
                height: 86
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
                        text: root.readOnlyDataController.lastError.length > 0
                            ? "Error: " + root.readOnlyDataController.lastError
                            : "Error: none"
                        color: root.readOnlyDataController.lastError.length > 0 ? "#9f2f2f" : "#667086"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
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
