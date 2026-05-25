import QtQuick

Rectangle {
    id: root
    required property var portfolioModel

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"
    height: 180

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Text {
            text: "Portfolios"
            color: "#18202f"
            font.pixelSize: 16
            font.bold: true
        }

        Text {
            visible: portfolioList.count === 0
            text: "Empty / No data"
            color: "#667086"
            font.pixelSize: 13
        }

        ListView {
            id: portfolioList
            objectName: "portfolioListView"
            width: parent.width
            height: parent.height - 36
            clip: true
            model: root.portfolioModel
            spacing: 4

            delegate: Rectangle {
                width: portfolioList.width
                height: 38
                radius: 6
                color: model.isActive ? "#eef8f2" : "#f5f7fa"
                border.color: "#d8e0eb"

                Text {
                    anchors.fill: parent
                    anchors.margins: 8
                    text: model.name + " | basePositionRatio=" + model.basePositionRatioText
                        + " | active=" + model.isActive
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
