import QtQuick

Rectangle {
    id: root
    objectName: "readOnlySectionHeader"
    property string title: ""
    property string subtitle: ""
    property string readonlyText: "READONLY"
    property int count: -1

    color: "transparent"
    height: 42

    Row {
        anchors.fill: parent
        spacing: 10

        Column {
            width: parent.width - badge.width - 12
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2

            Text {
                text: root.title
                color: "#18202f"
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
                width: parent.width
            }

            Text {
                text: root.subtitle + (root.count >= 0 ? " · rows=" + root.count : "")
                color: "#667086"
                font.pixelSize: 12
                visible: text.length > 0
                elide: Text.ElideRight
                width: parent.width
            }
        }

        ReadOnlyStatusBadge {
            id: badge
            status: "READONLY"
            textOverride: root.readonlyText
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
