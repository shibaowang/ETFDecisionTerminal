import QtQuick

Rectangle {
    id: root
    required property var model
    property int selectedIndex: -1
    signal serviceSelected(int index)

    function displayBool(value) {
        return value ? "是" : "否"
    }

    function displaySeverity(value) {
        if (value === "ERROR") return "错误"
        if (value === "WARNING") return "警告"
        if (value === "DISABLED") return "禁用"
        if (value === "OK") return "正常"
        return value
    }

    radius: 8
    color: "#ffffff"
    border.color: "#d9e0ea"

    Column {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Text {
            text: "服务列表"
            color: "#18202f"
            font.pixelSize: 17
            font.bold: true
        }

        ListView {
            id: listView
            width: parent.width
            height: parent.height - 32
            clip: true
            spacing: 8
            model: root.model

            delegate: Rectangle {
                width: listView.width
                height: 78
                radius: 7
                color: root.selectedIndex === index ? "#eaf1ff" : "#f8fafc"
                border.color: root.selectedIndex === index ? "#7da5e8" : "#e2e8f0"

                Row {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 12

                    Rectangle {
                        width: 10
                        height: parent.height
                        radius: 5
                        color: statusSeverity === "ERROR" ? "#d92d20"
                             : statusSeverity === "WARNING" ? "#d88a00"
                             : statusSeverity === "DISABLED" ? "#8a94a6"
                             : "#1a7f4b"
                    }

                    Column {
                        width: parent.width - 22
                        spacing: 6

                        Text {
                            width: parent.width
                            text: serviceName
                            color: "#18202f"
                            font.pixelSize: 15
                            font.bold: true
                            elide: Text.ElideRight
                        }

                        Text {
                            width: parent.width
                            text: "启用=" + root.displayBool(enabled)
                                  + "  可启动=" + root.displayBool(canStart)
                                  + "  状态=" + root.displaySeverity(statusSeverity)
                                  + "  问题=" + issueCount
                            color: "#4f5b70"
                            font.pixelSize: 12
                            elide: Text.ElideRight
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.serviceSelected(index)
                }
            }
        }
    }
}
