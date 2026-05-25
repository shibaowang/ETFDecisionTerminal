import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var portfolioModel

    objectName: "portfolioReadOnlyTable"
    height: 210
    title: "Portfolios"
    subtitle: "Read-only portfolio list"
    emptyTitle: "暂无组合数据"
    emptyMessage: "连接只读 DataService 后刷新组合列表。"
    columns: [
        {"key": "name", "title": "Name", "width": 260, "sortable": true},
        {"key": "amount", "title": "Base position ratio", "width": 170, "alignment": Text.AlignRight, "sortable": true},
        {"key": "status", "title": "Status", "width": 96, "sortable": true}
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
                    width: 260
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                Text {
                    width: 170
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.basePositionRatioText
                    color: "#26354d"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
                ReadOnlyStatusBadge {
                    width: 96
                    anchors.verticalCenter: parent.verticalCenter
                    status: model.isActive ? "ACTIVE" : "DISABLED"
                }
            }
        }
    }
}
