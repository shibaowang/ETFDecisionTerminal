import QtQuick

Rectangle {
    id: root
    required property var logModel
    color: "#111827"
    border.color: "#26354d"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 6

        Text {
            text: "日志 / 预警 Mock - 未读取真实 system_log / error_log"
            color: "#dce6f5"
            font.pixelSize: 14
            font.bold: true
        }

        ListView {
            width: parent.width
            height: parent.height - 28
            clip: true
            model: root.logModel

            delegate: Row {
                width: ListView.view.width
                height: 22
                spacing: 12

                Text { width: 70; text: model.timeText; color: "#9fb0c9"; font.pixelSize: 12; elide: Text.ElideRight }
                Text { width: 70; text: model.level; color: model.level === "WARNING" ? "#ffd77a" : "#a8d5ff"; font.pixelSize: 12; elide: Text.ElideRight }
                Text { width: 110; text: model.source; color: "#c8d6ea"; font.pixelSize: 12; elide: Text.ElideRight }
                Text { width: parent.width - 290; text: model.message; color: "#dce6f5"; font.pixelSize: 12; elide: Text.ElideRight }
            }
        }
    }
}
