import QtQuick

Rectangle {
    id: root
    required property var model

    radius: 8
    color: "#ffffff"
    border.color: "#d9e0ea"

    Column {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Text {
            text: "Issues"
            color: "#18202f"
            font.pixelSize: 17
            font.bold: true
        }

        Text {
            visible: issueList.count === 0
            text: "Select a service with issues."
            color: "#687386"
            font.pixelSize: 13
        }

        ListView {
            id: issueList
            width: parent.width
            height: parent.height - 32
            clip: true
            spacing: 8
            model: root.model

            delegate: Rectangle {
                width: issueList.width
                height: 84
                radius: 7
                color: "#f8fafc"
                border.color: "#e2e8f0"

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 5

                    Text {
                        width: parent.width
                        text: level + "  " + code
                        color: level === "ERROR" ? "#9d1c1c" : "#7a4a00"
                        font.pixelSize: 13
                        font.bold: true
                        elide: Text.ElideRight
                    }

                    Text {
                        width: parent.width
                        text: message
                        color: "#465066"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        maximumLineCount: 2
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }
}
