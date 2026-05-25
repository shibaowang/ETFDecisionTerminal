import QtQuick

Rectangle {
    id: root
    required property var otcChannelModel

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"
    height: 190

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Text {
            text: "OTC Channels"
            color: "#18202f"
            font.pixelSize: 16
            font.bold: true
        }

        Text {
            visible: otcChannelList.count === 0
            text: "暂无 OTC 通道"
            color: "#667086"
            font.pixelSize: 13
        }

        ListView {
            id: otcChannelList
            objectName: "otcChannelListView"
            width: parent.width
            height: parent.height - 36
            clip: true
            model: root.otcChannelModel
            spacing: 4

            delegate: Rectangle {
                width: otcChannelList.width
                height: 38
                radius: 6
                color: model.enabled ? "#eef8f2" : "#f5f7fa"
                border.color: "#d8e0eb"

                Text {
                    anchors.fill: parent
                    anchors.margins: 8
                    text: model.strategyCode + " | actual=" + model.actualCode
                        + " | class=" + model.fundClass + " | enabled=" + model.enabled
                        + " | dailyLimit=" + model.dailyLimitText
                        + " | priority=" + model.priority
                        + " | minBuy=" + model.minBuyAmountText
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
