import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var instrumentModel

    objectName: "instrumentReadOnlyTable"
    height: 250
    title: "标的列表"
    subtitle: "只读标的列表"
    emptyTitle: "暂无标的"
    emptyMessage: "连接只读 DataService 后刷新标的列表。"
    columns: [
        {"key": "code", "title": "代码", "width": 110, "required": true, "visible": true, "sortable": true},
        {"key": "name", "title": "名称", "width": 180, "required": false, "visible": true, "sortable": true},
        {"key": "type", "title": "类型", "width": 110, "required": false, "visible": true, "sortable": true},
        {"key": "market", "title": "市场", "width": 90, "required": false, "visible": true, "sortable": true},
        {"key": "currency", "title": "币种", "width": 90, "required": false, "visible": true, "sortable": true},
        {"key": "status", "title": "状态", "width": 96, "required": false, "visible": true, "sortable": true}
    ]
    rowCount: instrumentList.count

    ListView {
        id: instrumentList
        objectName: "instrumentListView"
        anchors.fill: parent
        clip: true
        model: root.instrumentModel
        spacing: 4

        delegate: Rectangle {
            width: instrumentList.width
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
                    width: root.columnWidth("code", 110)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.code
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("name")
                    width: root.columnWidth("name", 180)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("type")
                    width: root.columnWidth("type", 110)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.instrumentType
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("market")
                    width: root.columnWidth("market", 90)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.marketCode
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("currency")
                    width: root.columnWidth("currency", 90)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.currency
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
