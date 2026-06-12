import QtQuick

Rectangle {
    id: root
    objectName: "readOnlyEmptyState"
    property string title: "暂无数据"
    property string message: ""
    property string hint: ""

    radius: 6
    color: "#f7f9fc"
    border.color: "#d8e0eb"
    height: 76

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 4

        Text {
            text: root.title
            color: "#26354d"
            font.pixelSize: 13
            font.bold: true
        }

        Text {
            width: parent.width
            text: root.message
            visible: root.message.length > 0
            color: "#667086"
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.hint
            visible: root.hint.length > 0
            color: "#8a5a00"
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }
}
