import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var portfolioModel

    objectName: "portfolioReadOnlyTable"
    height: 210
    title: "Portfolios"
    subtitle: "Read-only portfolio list"
    emptyTitle: "No portfolios"
    emptyMessage: "Connect to the read-only DataService and refresh portfolios."
    columns: [
        {"key": "name", "title": "Name", "width": 260, "required": true, "visible": true, "sortable": true},
        {"key": "amount", "title": "Base position ratio", "width": 170, "required": false, "visible": true, "alignment": Text.AlignRight, "sortable": true},
        {"key": "status", "title": "Status", "width": 96, "required": false, "visible": true, "sortable": true}
    ]
    rowCount: portfolioList.count

    ListView {
        id: portfolioList
        objectName: "portfolioListView"
        anchors.fill: parent
        clip: true
        model: root.portfolioModel
        spacing: 4

        delegate: Rectangle {
            width: portfolioList.width
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
                    width: root.columnWidth("name", 260)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("amount")
                    width: root.columnWidth("amount", 170)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.basePositionRatioText
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
                ReadOnlyStatusBadge {
                    visible: root.columnVisible("status")
                    width: root.columnWidth("status", 96)
                    anchors.verticalCenter: parent.verticalCenter
                    status: model.isActive ? "ACTIVE" : "DISABLED"
                }
            }
        }
    }
}
