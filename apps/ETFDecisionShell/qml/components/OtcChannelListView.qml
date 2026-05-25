import QtQuick
import "readonly"

ReadOnlyTable {
    id: root
    required property var otcChannelModel

    objectName: "otcChannelReadOnlyTable"
    height: 220
    title: "OTC Channels"
    subtitle: "Read-only OTC channel list"
    emptyTitle: "No OTC channels"
    emptyMessage: "Select a strategyCode and refresh OTC channels; an empty result is valid."
    columns: [
        {"key": "strategy", "title": "Strategy", "width": 130, "required": false, "visible": false, "sortable": true},
        {"key": "code", "title": "Actual code", "width": 130, "required": true, "visible": true, "sortable": true},
        {"key": "type", "title": "Fund class", "width": 120, "required": false, "visible": true, "sortable": true},
        {"key": "status", "title": "Status", "width": 96, "required": false, "visible": true, "sortable": true},
        {"key": "dailyLimit", "title": "Daily limit", "width": 120, "required": false, "visible": true, "alignment": Text.AlignRight, "sortable": false},
        {"key": "priority", "title": "Priority", "width": 80, "required": false, "visible": true, "alignment": Text.AlignRight, "sortable": true},
        {"key": "amount", "title": "Min buy", "width": 120, "required": false, "visible": false, "alignment": Text.AlignRight, "sortable": true}
    ]
    rowCount: otcChannelList.count

    ListView {
        id: otcChannelList
        objectName: "otcChannelListView"
        anchors.fill: parent
        clip: true
        model: root.otcChannelModel
        spacing: 4

        delegate: Rectangle {
            width: otcChannelList.width
            height: root.rowHeight
            radius: 6
            color: index % 2 === 0 ? "#ffffff" : "#f8fbff"
            border.color: "#edf1f5"

            Row {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 8

                Text {
                    visible: root.columnVisible("strategy")
                    width: root.columnWidth("strategy", 130)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.strategyCode
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("code")
                    width: root.columnWidth("code", 130)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.actualCode
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    font.bold: true
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("type")
                    width: root.columnWidth("type", 120)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.fundClass
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    elide: Text.ElideRight
                }
                ReadOnlyStatusBadge {
                    visible: root.columnVisible("status")
                    width: root.columnWidth("status", 96)
                    anchors.verticalCenter: parent.verticalCenter
                    status: model.enabled ? "ENABLED" : "DISABLED"
                }
                Text {
                    visible: root.columnVisible("dailyLimit")
                    width: root.columnWidth("dailyLimit", 120)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.dailyLimitText
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("priority")
                    width: root.columnWidth("priority", 80)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.priority
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
                Text {
                    visible: root.columnVisible("amount")
                    width: root.columnWidth("amount", 120)
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.minBuyAmountText
                    color: "#26354d"
                    font.pixelSize: root.bodyFontPixelSize
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                }
            }
        }
    }
}
