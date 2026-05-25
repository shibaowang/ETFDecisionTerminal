import QtQuick

Rectangle {
    id: root
    required property var navigationModel
    required property string currentKey
    signal navigate(string key)

    color: "#162235"
    border.color: "#26354d"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 6

        Text {
            text: "导航"
            color: "#9fb0c9"
            font.pixelSize: 12
            font.bold: true
            height: 24
            verticalAlignment: Text.AlignVCenter
        }

        Repeater {
            model: root.navigationModel

            Rectangle {
                width: parent.width
                height: 38
                radius: 6
                opacity: model.enabled ? 1.0 : 0.45
                color: root.currentKey === model.key ? "#244464" : navMouse.containsMouse && model.enabled ? "#1f314a" : "transparent"
                border.color: root.currentKey === model.key ? "#3d77a8" : "transparent"

                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    text: model.title
                    color: root.currentKey === model.key ? "#ffffff" : "#c7d2e4"
                    font.pixelSize: 13
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                MouseArea {
                    id: navMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: model.enabled
                    onClicked: root.navigate(model.key)
                }
            }
        }
    }
}
