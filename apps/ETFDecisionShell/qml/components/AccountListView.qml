import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var accountModel

    objectName: "accountReadOnlyTable"
    height: 250
    title: "账户列表"
    subtitle: "只读账户列表"
    emptyTitle: "暂无账户数据"
    emptyMessage: "连接只读 DataService 后刷新账户列表。"
    columns: [
        {"key": "name", "title": "名称", "width": 190, "required": true, "visible": true, "sortable": true},
        {"key": "type", "title": "类型", "width": 100, "required": false, "visible": true, "sortable": true},
        {"key": "broker", "title": "券商", "width": 140, "required": false, "visible": false, "sortable": false},
        {"key": "currency", "title": "币种", "width": 90, "required": false, "visible": true, "sortable": false},
        {"key": "status", "title": "状态", "width": 96, "required": false, "visible": true, "sortable": true},
        {"key": "amount", "title": "初始现金", "width": 130, "required": false, "visible": true, "alignment": Text.AlignRight, "sortable": true}
    ]
    rowCount: accountList.count

    ListView {
        id: accountList
        objectName: "accountListView"
        anchors.fill: parent
        clip: true
        model: root.accountModel
        spacing: 4

        delegate: Rectangle {
            width: accountList.width
            height: root.rowHeight
            radius: 6
            color: index % 2 === 0 ? "#ffffff" : "#f8fbff"
            border.color: "#edf1f5"

            Row {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 8

                Text {
                    visible: root.columnVisible("name")
                    width: root.columnWidth("name", 190)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("type")
                    width: root.columnWidth("type", 100)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.accountType
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("broker")
                    width: root.columnWidth("broker", 140)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.brokerName
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("currency")
                    width: root.columnWidth("currency", 90)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.baseCurrency
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                ReadOnlyStatusBadge {
                    visible: root.columnVisible("status")
                    width: root.columnWidth("status", 96)
                    anchors.verticalCenter: parent.verticalCenter
                    status: model.isActive ? "ACTIVE" : "DISABLED"
                }
                Text {
                    visible: root.columnVisible("amount")
                    width: root.columnWidth("amount", 130)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.initialCashText
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
            }
        }
    }
}
