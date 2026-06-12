import QtQuick

Rectangle {
    id: root
    required property var summary
    radius: 8
    color: "#ffffff"
    border.color: "#d9e0ea"

    function displayText(value) {
        if (value === "Mock") return "模拟数据"
        if (value === "Diagnostics") return "诊断中心"
        if (value === "All services healthy.") return "所有服务状态健康。"
        if (value === "Warnings present.") return "存在警告。"
        if (value === "Errors present.") return "存在错误。"
        if (value === "Mixed status.") return "存在混合状态。"
        return value
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        Row {
            width: parent.width
            spacing: 12

            Rectangle {
                width: 88
                height: 28
                radius: 14
                color: root.summary && root.summary.ok ? "#dff4e7" : "#fde3e2"

                Text {
                    anchors.centerIn: parent
                    text: root.summary && root.summary.ok ? "正常" : "错误"
                    color: root.summary && root.summary.ok ? "#17663a" : "#9d1c1c"
                    font.pixelSize: 13
                    font.bold: true
                }
            }

            Text {
                width: parent.width - 100
                text: root.summary ? root.displayText(root.summary.title) : "诊断中心"
                color: "#18202f"
                font.pixelSize: 18
                font.bold: true
                elide: Text.ElideRight
            }
        }

        Row {
            width: parent.width
            spacing: 10

            Repeater {
                model: [
                    ["总数", root.summary ? root.summary.totalServices : 0],
                    ["启用", root.summary ? root.summary.enabledServices : 0],
                    ["禁用", root.summary ? root.summary.disabledServices : 0],
                    ["错误", root.summary ? root.summary.errorCount : 0],
                    ["警告", root.summary ? root.summary.warningCount : 0],
                    ["阻塞", root.summary ? root.summary.blockedServiceCount : 0]
                ]

                Rectangle {
                    width: 112
                    height: 48
                    radius: 6
                    color: "#f6f8fb"
                    border.color: "#e1e7ef"

                    Column {
                        anchors.centerIn: parent
                        spacing: 2
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: modelData[0]
                            color: "#667086"
                            font.pixelSize: 11
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: String(modelData[1])
                            color: "#18202f"
                            font.pixelSize: 17
                            font.bold: true
                        }
                    }
                }
            }
        }

        Text {
            width: parent.width
            text: root.summary ? root.displayText(root.summary.summaryText) : ""
            color: "#465066"
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }
}
