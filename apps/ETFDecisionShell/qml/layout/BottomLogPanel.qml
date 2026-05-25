import QtQuick

Rectangle {
    id: root
    color: "#111827"
    border.color: "#26354d"

    Column {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 8

        Text {
            text: "日志 / 预警 Mock"
            color: "#dce6f5"
            font.pixelSize: 14
            font.bold: true
        }

        Text {
            width: parent.width
            text: "暂无真实日志，当前为 Mock。此区域不会写 system_log / error_log，也不会连接服务。"
            color: "#9fb0c9"
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }
}
