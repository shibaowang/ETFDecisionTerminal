import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var strategyModel

    objectName: "strategyReadOnlyTable"
    height: 210
    title: "Strategies"
    subtitle: "Read-only strategy list"
    emptyTitle: "暂无策略数据"
    emptyMessage: "当前数据库没有策略记录，页面保持只读并可继续刷新。"
    columns: [
        {"title": "Strategy code", "width": 180},
        {"title": "Name", "width": 260},
        {"title": "Status", "width": 96}
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
                    width: 180
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.strategyCode
                    color: "#26354d"
                    font.pixelSize: 12
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    width: 260
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
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
