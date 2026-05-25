import QtQuick

Rectangle {
    id: root
    required property string currentPageTitle
    color: "#121d2e"
    border.color: "#26354d"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 18
        spacing: 18

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "跨境 ETF 智能投资决策终端"
            color: "#eef4ff"
            font.pixelSize: 18
            font.bold: true
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.currentPageTitle
            color: "#aebbd0"
            font.pixelSize: 14
        }

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: mockLabel.implicitWidth + 24
            height: 28
            radius: 14
            color: "#342d18"
            border.color: "#8d6a20"

            Text {
                id: mockLabel
                anchors.centerIn: parent
                text: "UI Mock - 未连接真实服务"
                color: "#ffd77a"
                font.pixelSize: 12
            }
        }

        Item {
            width: parent.width - 720
            height: 1
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "DataService: Mock   MarketService: Mock   StrategyService: Mock"
            color: "#8fa1ba"
            font.pixelSize: 12
        }
    }
}
