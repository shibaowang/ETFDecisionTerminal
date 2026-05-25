import QtQuick

Rectangle {
    id: root
    required property var accountModel

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"
    height: 220

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Text {
            text: "Accounts"
            color: "#18202f"
            font.pixelSize: 16
            font.bold: true
        }

        Text {
            visible: accountList.count === 0
            text: "Empty / No data"
            color: "#667086"
            font.pixelSize: 13
        }

        ListView {
            id: accountList
            objectName: "accountListView"
            width: parent.width
            height: parent.height - 36
            clip: true
            model: root.accountModel
            spacing: 4

            delegate: Rectangle {
                width: accountList.width
                height: 42
                radius: 6
                color: model.isActive ? "#eef8f2" : "#f5f7fa"
                border.color: "#d8e0eb"

                Text {
                    anchors.fill: parent
                    anchors.margins: 8
                    text: model.name + " | " + model.accountType + " | " + model.baseCurrency + " | cash=" + model.initialCashText
                    color: "#26354d"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
