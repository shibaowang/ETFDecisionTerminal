import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var strategyModel

    objectName: "strategyReadOnlyTable"
    height: 210
    title: "Strategies"
    subtitle: "Read-only strategy list"
    emptyTitle: "No strategies"
    emptyMessage: "The current demo database may have no strategy rows; the page remains read-only."
    columns: [
        {"key": "code", "title": "Strategy code", "width": 180, "required": true, "visible": true, "sortable": true},
        {"key": "name", "title": "Name", "width": 260, "required": false, "visible": true, "sortable": true},
        {"key": "status", "title": "Status", "width": 96, "required": false, "visible": true, "sortable": true}
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
