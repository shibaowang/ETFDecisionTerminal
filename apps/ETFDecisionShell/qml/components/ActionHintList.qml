import QtQuick

Column {
    id: root
    required property var hintModel
    property int maxRows: 3
    spacing: 8

    Repeater {
        model: root.hintModel

        Rectangle {
            width: parent.width
            height: 72
            radius: 8
            color: "#ffffff"
            border.color: model.severity === "WARNING" ? "#d9b45f" : "#dfe6f0"
            visible: index < root.maxRows

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 4

                Text {
                    width: parent.width
                    text: model.title
                    color: "#26354d"
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width
                    text: model.description
                    color: "#58657a"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width
                    text: model.enabled ? model.actionText : "Mock only - not executable"
                    color: "#8a5a00"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
            }
        }
    }
}
