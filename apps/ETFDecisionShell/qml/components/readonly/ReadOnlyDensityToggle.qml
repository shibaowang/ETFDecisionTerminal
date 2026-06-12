import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    objectName: "readOnlyDensityToggle"
    property string density: "normal"
    signal densityChangedByUser(string density)

    height: 42
    radius: 8
    color: "#f8fbff"
    border.color: "#d8e0eb"

    Row {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "密度"
            color: "#34435c"
            font.pixelSize: 12
            font.bold: true
        }

        ButtonGroup {
            id: densityGroup
        }

        Repeater {
            model: [
                {"key": "compact", "title": "紧凑"},
                {"key": "normal", "title": "标准"},
                {"key": "comfortable", "title": "宽松"}
            ]
            RadioButton {
                objectName: "readOnlyDensity_" + modelData.key
                anchors.verticalCenter: parent.verticalCenter
                text: modelData.title
                checked: root.density === modelData.key
                ButtonGroup.group: densityGroup
                onClicked: {
                    root.density = modelData.key
                    root.densityChangedByUser(modelData.key)
                }
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "不会持久化"
            color: "#6a7890"
            font.pixelSize: 11
        }
    }
}
