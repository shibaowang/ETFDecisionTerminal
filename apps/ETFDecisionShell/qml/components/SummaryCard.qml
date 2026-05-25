import QtQuick

Rectangle {
    id: root
    required property var summary
    radius: 8
    color: "#ffffff"
    border.color: "#d9e0ea"

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
                    text: root.summary && root.summary.ok ? "OK" : "ERROR"
                    color: root.summary && root.summary.ok ? "#17663a" : "#9d1c1c"
                    font.pixelSize: 13
                    font.bold: true
                }
            }

            Text {
                width: parent.width - 100
                text: root.summary ? root.summary.title : "Diagnostics"
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
                    ["Total", root.summary ? root.summary.totalServices : 0],
                    ["Enabled", root.summary ? root.summary.enabledServices : 0],
                    ["Disabled", root.summary ? root.summary.disabledServices : 0],
                    ["Errors", root.summary ? root.summary.errorCount : 0],
                    ["Warnings", root.summary ? root.summary.warningCount : 0],
                    ["Blocked", root.summary ? root.summary.blockedServiceCount : 0]
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
            text: root.summary ? root.summary.summaryText : ""
            color: "#465066"
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }
}
