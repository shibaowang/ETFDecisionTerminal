import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    objectName: "readOnlyColumnChooser"
    property var columns: []
    property var visibleColumnKeys: []
    property string title: "列显示"
    property string columnObjectNamePrefix: "readOnlyColumnChooser"
    property int minimumVisibleColumns: 1
    signal columnVisibilityChanged(var keys)

    height: Math.max(42, chooserRow.implicitHeight + 16)
    radius: 8
    color: "#f8fbff"
    border.color: "#d8e0eb"

    function defaultVisibleKeys() {
        var keys = []
        for (var i = 0; i < root.columns.length; ++i) {
            var column = root.columns[i]
            if (column.required === true || column.visible !== false) {
                keys.push(column.key)
            }
        }
        return keys
    }

    function effectiveKeys() {
        return root.visibleColumnKeys.length > 0 ? root.visibleColumnKeys : defaultVisibleKeys()
    }

    function containsKey(keys, key) {
        return keys.indexOf(key) >= 0
    }

    function isRequiredColumn(key) {
        for (var i = 0; i < root.columns.length; ++i) {
            if (root.columns[i].key === key && root.columns[i].required === true) {
                return true
            }
        }
        return false
    }

    function canHideColumn(key) {
        return !isRequiredColumn(key)
    }

    function setColumnVisible(key, visible) {
        var keys = effectiveKeys().slice()
        var index = keys.indexOf(key)
        if (visible && index < 0) {
            keys.push(key)
        } else if (!visible && index >= 0) {
            if (!canHideColumn(key) || keys.length <= root.minimumVisibleColumns) {
                return
            }
            keys.splice(index, 1)
        }
        root.visibleColumnKeys = keys
        root.columnVisibilityChanged(keys)
    }

    Row {
        id: chooserRow
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.title
            color: "#34435c"
            font.pixelSize: 12
            font.bold: true
        }

        Repeater {
            model: root.columns
            CheckBox {
                objectName: root.columnObjectNamePrefix + "_" + (modelData.key || "")
                anchors.verticalCenter: parent.verticalCenter
                text: modelData.title || modelData.key || ""
                checked: root.containsKey(root.effectiveKeys(), modelData.key)
                enabled: modelData.required !== true
                onToggled: root.setColumnVisible(modelData.key, checked)
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "仅运行时显示"
            color: "#6a7890"
            font.pixelSize: 11
        }
    }
}
