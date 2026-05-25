import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var accountModel

    objectName: "accountReadOnlyTable"
    height: 250
    title: "Accounts"
    subtitle: "Read-only account list"
    emptyTitle: "暂无账户数据"
    emptyMessage: "连接只读 DataService 后刷新账户列表。"
    columns: [
        {"title": "Name", "width": 190},
        {"title": "Type", "width": 100},
        {"title": "Broker", "width": 140},
        {"title": "Currency", "width": 90},
        {"title": "Status", "width": 96},
        {"title": "Initial cash", "width": 130}
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
                    width: 190
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                Text {
                    width: 100
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.accountType
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                Text {
                    width: 140
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.brokerName
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                Text {
                    width: 90
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.baseCurrency
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                ReadOnlyStatusBadge {
                    width: 96
                    anchors.verticalCenter: parent.verticalCenter
                    status: model.isActive ? "ACTIVE" : "DISABLED"
                }
                Text {
                    width: 130
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.initialCashText
                    color: "#26354d"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
            }
        }
    }
}
