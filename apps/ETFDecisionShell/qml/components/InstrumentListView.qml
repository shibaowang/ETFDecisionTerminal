import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var instrumentModel

    objectName: "instrumentReadOnlyTable"
    height: 250
    title: "Instruments"
    subtitle: "Read-only instrument list"
    emptyTitle: "No instruments"
    emptyMessage: "Connect to the read-only DataService and refresh instruments."
    columns: [
        {"key": "code", "title": "Code", "width": 110, "required": true, "visible": true, "sortable": true},
        {"key": "name", "title": "Name", "width": 180, "required": false, "visible": true, "sortable": true},
        {"key": "type", "title": "Type", "width": 110, "required": false, "visible": true, "sortable": true},
        {"key": "market", "title": "Market", "width": 90, "required": false, "visible": true, "sortable": true},
        {"key": "currency", "title": "Currency", "width": 90, "required": false, "visible": true, "sortable": true},
        {"key": "status", "title": "Status", "width": 96, "required": false, "visible": true, "sortable": true}
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
