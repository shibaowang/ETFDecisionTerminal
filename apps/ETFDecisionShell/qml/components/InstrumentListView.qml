import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var instrumentModel

    objectName: "instrumentReadOnlyTable"
    height: 250
    title: "Instruments"
    subtitle: "Read-only instrument list"
    emptyTitle: "暂无标的数据"
    emptyMessage: "连接只读 DataService 后刷新标的列表。"
    columns: [
        {"title": "Code", "width": 110},
        {"title": "Name", "width": 180},
        {"title": "Type", "width": 110},
        {"title": "Market", "width": 90},
        {"title": "Currency", "width": 90},
        {"title": "Status", "width": 96}
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
            height: 40
            radius: 6
            color: index % 2 === 0 ? "#ffffff" : "#f8fbff"
            border.color: "#edf1f5"

            Row {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 8

                Text {
                    width: 110
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.code
                    color: "#26354d"
                    font.pixelSize: 12
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    width: 180
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                Text {
                    width: 110
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.instrumentType
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                Text {
                    width: 90
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.marketCode
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                Text {
                    width: 90
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.currency
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                ReadOnlyStatusBadge {
                    width: 96
                    anchors.verticalCenter: parent.verticalCenter
                    status: model.enabled ? "ENABLED" : "DISABLED"
                }
            }
        }
    }
}
