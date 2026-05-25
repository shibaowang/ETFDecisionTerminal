import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var otcChannelModel

    objectName: "otcChannelReadOnlyTable"
    height: 220
    title: "OTC Channels"
    subtitle: "Read-only OTC channel list"
    emptyTitle: "暂无 OTC 通道"
    emptyMessage: "选择 strategyCode 后刷新 OTC 通道；空结果表示该策略暂无通道映射。"
    columns: [
        {"title": "Strategy", "width": 130},
        {"title": "Actual code", "width": 130},
        {"title": "Fund class", "width": 120},
        {"title": "Status", "width": 96},
        {"title": "Daily limit", "width": 120},
        {"title": "Priority", "width": 80},
        {"title": "Min buy", "width": 120}
    ]
    rowCount: otcChannelList.count

    ListView {
        id: otcChannelList
        objectName: "otcChannelListView"
        anchors.fill: parent
        clip: true
        model: root.otcChannelModel
        spacing: 4

        delegate: Rectangle {
            width: otcChannelList.width
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
                    width: 130
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.strategyCode
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                Text {
                    width: 130
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.actualCode
                    color: "#26354d"
                    font.pixelSize: 12
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    width: 120
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.fundClass
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
                ReadOnlyStatusBadge {
                    width: 96
                    anchors.verticalCenter: parent.verticalCenter
                    status: model.enabled ? "ENABLED" : "DISABLED"
                }
                Text {
                    width: 120
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.dailyLimitText
                    color: "#26354d"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
                Text {
                    width: 80
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.priority
                    color: "#26354d"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
                Text {
                    width: 120
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.minBuyAmountText
                    color: "#26354d"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
            }
        }
    }
}
