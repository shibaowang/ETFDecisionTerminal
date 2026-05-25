import QtQuick

Rectangle {
    id: root
    required property var pageInfo
    color: "#f7f9fc"
    border.color: "#d8e0eb"

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: "页面信息 Mock"
            color: "#1e2a3d"
            font.pixelSize: 16
            font.bold: true
        }

        Rectangle {
            width: parent.width
            height: 220
            radius: 8
            color: "#ffffff"
            border.color: "#dfe6f0"

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Text { text: root.pageInfo.title; color: "#26354d"; font.bold: true; font.pixelSize: 15 }
                Text { text: "模块状态: " + root.pageInfo.moduleStatus; color: "#58657a"; font.pixelSize: 13 }
                Text { text: "数据模式: " + root.pageInfo.dataMode; color: "#58657a"; font.pixelSize: 13 }
                Text { text: "连接状态: " + root.pageInfo.connectionStatus; color: "#58657a"; font.pixelSize: 13 }
                Text { text: "Placeholder: " + (root.pageInfo.placeholder ? "Yes" : "No"); color: "#8a5a00"; font.pixelSize: 13 }
                Text { text: "Warnings: " + root.pageInfo.warningCount + "  Errors: " + root.pageInfo.errorCount; color: "#58657a"; font.pixelSize: 13 }
                Text {
                    width: parent.width
                    text: root.pageInfo.detailText
                    color: "#58657a"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
