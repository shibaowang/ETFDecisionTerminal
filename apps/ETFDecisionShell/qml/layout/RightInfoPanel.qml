import QtQuick

Rectangle {
    id: root
    color: "#f7f9fc"
    border.color: "#d8e0eb"

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: "信息面板 Mock"
            color: "#1e2a3d"
            font.pixelSize: 16
            font.bold: true
        }

        Text {
            width: parent.width
            text: "当前为右侧信息占位。后续任务可接入诊断、账户摘要或服务状态数据。"
            color: "#58657a"
            font.pixelSize: 13
            wrapMode: Text.WordWrap
        }

        Rectangle {
            width: parent.width
            height: 112
            radius: 8
            color: "#ffffff"
            border.color: "#dfe6f0"

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8
                Text { text: "DataService"; color: "#26354d"; font.bold: true }
                Text { text: "状态: Mock"; color: "#68758a" }
                Text { text: "未连接真实服务"; color: "#8a5a00" }
            }
        }
    }
}
