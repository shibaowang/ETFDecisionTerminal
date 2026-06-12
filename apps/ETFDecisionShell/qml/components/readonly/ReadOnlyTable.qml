import QtQuick

Rectangle {
    id: root
    objectName: "readOnlyTable"
    property string title: ""
    property string subtitle: ""
    property string readonlyText: "只读"
    property string emptyTitle: "暂无数据"
    property string emptyMessage: ""
    property string emptyHint: "只读表格，不提供编辑操作。"
    property var columns: []
    property var visibleColumnKeys: []
    property int minimumVisibleColumns: 1
    property var effectiveColumns: computeEffectiveColumns()
    property int rowCount: 0
    property bool empty: rowCount === 0
    property bool showHeader: true
    property string density: "normal"
    property int rowHeightCompact: 28
    property int rowHeightNormal: 36
    property int rowHeightComfortable: 44
    property int rowHeight: density === "compact"
        ? rowHeightCompact
        : (density === "comfortable" ? rowHeightComfortable : rowHeightNormal)
    property int bodyFontPixelSize: density === "compact" ? 11 : (density === "comfortable" ? 13 : 12)
    property bool compact: false
    property string sortKey: ""
    property bool sortAscending: true
    default property alias content: body.data
    signal sortRequested(string key, bool ascending)

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    function computeEffectiveColumns() {
        var result = []
        var explicitKeys = root.visibleColumnKeys || []
        var useDefaults = explicitKeys.length === 0
        for (var i = 0; i < root.columns.length; ++i) {
            var column = root.columns[i]
            var key = column.key || ""
            var required = column.required === true
            var visibleByDefault = column.visible !== false
            var explicitlyVisible = explicitKeys.indexOf(key) >= 0
            if (required || (useDefaults && visibleByDefault) || (!useDefaults && explicitlyVisible)) {
                result.push(column)
            }
        }
        if (result.length < root.minimumVisibleColumns) {
            for (var j = 0; j < root.columns.length && result.length < root.minimumVisibleColumns; ++j) {
                if (result.indexOf(root.columns[j]) < 0) {
                    result.push(root.columns[j])
                }
            }
        }
        return result
    }

    function columnVisible(key) {
        for (var i = 0; i < root.effectiveColumns.length; ++i) {
            if (root.effectiveColumns[i].key === key) {
                return true
            }
        }
        return false
    }

    function columnWidth(key, fallback) {
        for (var i = 0; i < root.effectiveColumns.length; ++i) {
            if (root.effectiveColumns[i].key === key) {
                return root.effectiveColumns[i].width || fallback
            }
        }
        return 0
    }

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
                    model: root.effectiveColumns
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
                            width: 24
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            text: (modelData.sortable && root.sortKey === modelData.key)
                                ? (root.sortAscending ? "^" : "v")
                                : (modelData.sortable ? "排序" : "")
                            color: modelData.sortable ? "#53657f" : "transparent"
                            font.pixelSize: 9
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
