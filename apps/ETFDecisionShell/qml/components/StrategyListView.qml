import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var strategyModel

    objectName: "strategyReadOnlyTable"
    height: 210
    title: "策略列表"
    subtitle: "只读策略列表"
    emptyTitle: "暂无策略"
    emptyMessage: "当前演示数据库可能没有策略行，本页面仍保持只读。"
    columns: [
        {"key": "code", "title": "策略代码", "width": 180, "required": true, "visible": true, "sortable": true},
        {"key": "name", "title": "名称", "width": 260, "required": false, "visible": true, "sortable": true},
        {"key": "status", "title": "状态", "width": 96, "required": false, "visible": true, "sortable": true}
    ]
    rowCount: strategyList.count

    ListView {
        id: strategyList
        objectName: "strategyListView"
        anchors.fill: parent
        clip: true
        model: root.strategyModel
        spacing: 4

        delegate: Rectangle {
            width: strategyList.width
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
                    visible: root.columnVisible("code")
                    width: root.columnWidth("code", 180)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.strategyCode
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("name")
                    width: root.columnWidth("name", 260)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                ReadOnlyStatusBadge {
                    visible: root.columnVisible("status")
                    width: root.columnWidth("status", 96)
                    anchors.verticalCenter: parent.verticalCenter
                    status: model.enabled ? "ENABLED" : "DISABLED"
                }
            }
        }
    }
}
