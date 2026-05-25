import QtQuick

Rectangle {
    id: root
    objectName: "readOnlyTable"
    property string title: ""
    property string subtitle: ""
    property string emptyTitle: "No data"
    property string emptyMessage: ""
    property string emptyHint: "Read-only table. No edit actions are available."
    property var columns: []
    property int rowCount: 0
    property bool empty: rowCount === 0
    default property alias content: body.data

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        ReadOnlySectionHeader {
            objectName: root.objectName + "Header"
            width: parent.width
            title: root.title
            subtitle: root.subtitle
            count: root.rowCount
        }

        Rectangle {
            width: parent.width
            height: 30
            radius: 6
            color: "#edf2f8"
            border.color: "#d8e0eb"

            Row {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 8

                Repeater {
                    model: root.columns
                    Text {
                        width: modelData.width || 120
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.title || ""
                        color: "#34435c"
                        font.pixelSize: 12
                        font.bold: true
                        elide: Text.ElideRight
                    }
                }
            }
        }

        ReadOnlyEmptyState {
            objectName: root.objectName + "EmptyState"
            width: parent.width
            visible: root.empty
            title: root.emptyTitle
            message: root.emptyMessage
            hint: root.emptyHint
        }

        Item {
            id: body
            width: parent.width
            height: Math.max(0, parent.height - 42 - 30 - 24)
            visible: !root.empty
        }
    }
}
