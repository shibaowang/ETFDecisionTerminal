import QtQuick

Rectangle {
    id: root
    required property string label
    required property string valueText
    property string unitText: ""
    property string trendText: ""
    property string severity: "INFO"
    property bool mock: true
    property string description: ""

    width: 190
    height: 118
    radius: 8
    color: "#ffffff"
    border.color: root.severity === "ERROR" ? "#d96b6b" : root.severity === "WARNING" ? "#d9b45f" : "#dfe6f0"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 6

        Text {
            text: root.label
            color: "#58657a"
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Row {
            spacing: 6
            height: 28
            Text {
                text: root.valueText
                color: "#1e2a3d"
                font.pixelSize: 22
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: root.unitText
                color: "#667086"
                font.pixelSize: 12
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Text {
            width: parent.width
            text: root.trendText
            color: root.severity === "WARNING" ? "#8a5a00" : "#55708f"
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.mock ? "Mock metric" : root.description
            color: "#8c96a8"
            font.pixelSize: 11
            elide: Text.ElideRight
        }
    }
}
