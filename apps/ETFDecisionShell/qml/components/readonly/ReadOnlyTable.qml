import QtQuick

Rectangle {
    id: root
    objectName: "readOnlyTable"
    property string title: ""
    property string subtitle: ""
    property string readonlyText: "READONLY"
    property string emptyTitle: "No data"
    property string emptyMessage: ""
    property string emptyHint: "Read-only table. No edit actions are available."
    property var columns: []
    property int rowCount: 0
    property bool empty: rowCount === 0
    property bool showHeader: true
    property int rowHeight: compact ? 34 : 40
    property bool compact: false
    property string sortKey: ""
    property bool sortAscending: true
    default property alias content: body.data
    signal sortRequested(string key, bool ascending)

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
            readonlyText: root.readonlyText
            count: root.rowCount
        }

        Rectangle {
            width: parent.width
            height: 30
            visible: root.showHeader
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
                    Rectangle {
                        width: modelData.width || 120
                        height: parent.height
                        color: "transparent"

                        Text {
                            anchors.left: parent.left
                            anchors.right: sortIndicator.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData.title || ""
                            color: "#34435c"
                            font.pixelSize: 12
                            font.bold: true
                            horizontalAlignment: modelData.alignment || Text.AlignLeft
                            elide: Text.ElideRight
                        }

                        Text {
                            id: sortIndicator
                            width: 16
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            text: (modelData.sortable && root.sortKey === modelData.key)
                                ? (root.sortAscending ? "▲" : "▼")
                                : (modelData.sortable ? "⇅" : "")
                            color: modelData.sortable ? "#53657f" : "transparent"
                            font.pixelSize: 10
                            horizontalAlignment: Text.AlignRight
                        }

                        MouseArea {
                            anchors.fill: parent
                            enabled: modelData.sortable === true
                            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                            onClicked: {
                                var nextAscending = root.sortKey === modelData.key ? !root.sortAscending : true
                                root.sortKey = modelData.key || ""
                                root.sortAscending = nextAscending
                                root.sortRequested(root.sortKey, nextAscending)
                            }
                        }
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
            height: Math.max(0, parent.height - 42 - (root.showHeader ? 30 : 0) - 24)
            visible: !root.empty
        }
    }
}
