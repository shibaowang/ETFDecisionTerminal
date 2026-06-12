import QtQuick

Column {
    id: root
    objectName: "readOnlyFieldLabel"
    property string label: ""
    property string value: ""
    property string hint: ""
    property string severity: "正常"

    spacing: 2

    Text {
        text: root.label
        color: "#667086"
        font.pixelSize: 11
        font.bold: true
        elide: Text.ElideRight
        width: parent.width
    }

    Text {
        text: root.value
        color: root.severity.toUpperCase() === "错误" ? "#a33b2e" : "#26354d"
        font.pixelSize: 13
        elide: Text.ElideRight
        width: parent.width
    }

    Text {
        text: root.hint
        visible: root.hint.length > 0
        color: "#8a5a00"
        font.pixelSize: 11
        elide: Text.ElideRight
        width: parent.width
    }
}
