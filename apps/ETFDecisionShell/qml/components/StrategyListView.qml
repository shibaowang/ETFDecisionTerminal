import QtQuick

Rectangle {
    id: root
    required property var strategyModel

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"
    height: 180

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Text {
            text: "Strategies"
            color: "#18202f"
            font.pixelSize: 16
            font.bold: true
        }

        Text {
            visible: strategyList.count === 0
            text: "Empty / No data"
            color: "#667086"
            font.pixelSize: 13
        }

        ListView {
            id: strategyList
            objectName: "strategyListView"
            width: parent.width
            height: parent.height - 36
            clip: true
            model: root.strategyModel
            spacing: 4

            delegate: Rectangle {
                width: strategyList.width
                height: 38
                radius: 6
                color: model.enabled ? "#eef8f2" : "#f5f7fa"
                border.color: "#d8e0eb"

                Text {
                    anchors.fill: parent
                    anchors.margins: 8
                    text: model.strategyCode + " | " + model.name + " | enabled=" + model.enabled
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
