import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    objectName: "readOnlyFilterBar"
    property string searchText: ""
    property string placeholderText: "搜索只读数据"
    property bool enabledOnly: false
    property string enabledOnlyText: "仅显示启用"
    property bool showEnabledFilter: false
    property bool showClearButton: true
    signal filterTextChanged(string text)
    signal enabledOnlyChangedByUser(bool enabledOnly)
    signal clearRequested()

    height: 42
    radius: 8
    color: "#f8fbff"
    border.color: "#d8e0eb"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 10

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "筛选"
            color: "#34435c"
            font.pixelSize: 12
            font.bold: true
        }

        TextField {
            id: searchField
            objectName: root.objectName + "SearchField"
            width: Math.min(280, parent.width * 0.42)
            anchors.verticalCenter: parent.verticalCenter
            placeholderText: root.placeholderText
            text: root.searchText
            selectByMouse: true
            onTextEdited: root.filterTextChanged(text)
        }

        CheckBox {
            objectName: root.objectName + "EnabledOnlyCheckBox"
            visible: root.showEnabledFilter
            anchors.verticalCenter: parent.verticalCenter
            text: root.enabledOnlyText
            checked: root.enabledOnly
            onToggled: {
                root.enabledOnly = checked
                root.enabledOnlyChangedByUser(checked)
            }
        }

        Button {
            objectName: root.objectName + "ClearButton"
            visible: root.showClearButton
            anchors.verticalCenter: parent.verticalCenter
            text: "清空"
            onClicked: {
                searchField.text = ""
                if (root.showEnabledFilter) {
                    root.enabledOnly = false
                }
                root.filterTextChanged("")
                root.enabledOnlyChangedByUser(false)
                root.clearRequested()
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "只读显示筛选"
            color: "#6a7890"
            font.pixelSize: 11
            elide: Text.ElideRight
        }
    }
}
