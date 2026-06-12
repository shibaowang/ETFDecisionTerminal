import QtQuick

Rectangle {
    id: root
    required property var logModel
    color: "#111827"
    border.color: "#26354d"

    function displayLevel(value) {
        if (value === "INFO") return "信息"
        if (value === "WARNING") return "警告"
        if (value === "ERROR") return "错误"
        return value
    }

    function displaySource(value) {
        if (value === "Shell") return "Shell 界面"
        if (value === "Diagnostics") return "诊断中心"
        return value
    }

    function displayMessage(value) {
        if (value === "Shell UI Mock started.") return "Shell 模拟界面已启动。"
        if (value === "No real services are connected.") return "未连接真实服务。"
        if (value === "Diagnostics Center is using mock data.") return "诊断中心正在使用模拟数据。"
        return value
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 6

        Text {
            text: "日志 / 预警（模拟）- 未读取真实 system_log / error_log"
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
                Text { width: 70; text: root.displayLevel(model.level); color: model.level === "WARNING" ? "#ffd77a" : "#a8d5ff"; font.pixelSize: 12; elide: Text.ElideRight }
                Text { width: 110; text: root.displaySource(model.source); color: "#c8d6ea"; font.pixelSize: 12; elide: Text.ElideRight }
                Text { width: parent.width - 290; text: root.displayMessage(model.message); color: "#dce6f5"; font.pixelSize: 12; elide: Text.ElideRight }
            }
        }
    }
}
