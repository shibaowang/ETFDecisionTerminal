import QtQuick
import "../components"

Rectangle {
    id: root
    required property var pageInfo
    required property var metricsModel
    required property var actionHintModel

    color: "#f7f9fc"
    border.color: "#d8e0eb"

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: contentColumn.implicitHeight + 32
        clip: true

        Column {
            id: contentColumn
            width: parent.width - 32
            x: 16
            y: 16
            spacing: 12

            Text {
                text: "Page Info Mock"
                color: "#1e2a3d"
                font.pixelSize: 16
                font.bold: true
            }

            Rectangle {
                width: parent.width
                height: pageInfoColumn.implicitHeight + 24
                radius: 8
                color: "#ffffff"
                border.color: "#dfe6f0"

                Column {
                    id: pageInfoColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 12
                    spacing: 8

                    Text { text: root.pageInfo.title; color: "#26354d"; font.bold: true; font.pixelSize: 15 }
                    Text { text: "Module status: " + root.pageInfo.moduleStatus; color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "Data mode: " + root.pageInfo.dataMode; color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "Connection: " + root.pageInfo.connectionStatus; color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "Placeholder: " + (root.pageInfo.placeholder ? "Yes" : "No"); color: "#8a5a00"; font.pixelSize: 13 }
                    Text { text: "Warnings: " + root.pageInfo.warningCount + "  Errors: " + root.pageInfo.errorCount; color: "#58657a"; font.pixelSize: 13 }
                    Text {
                        width: parent.width
                        text: root.pageInfo.detailText
                        color: "#58657a"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Text {
                text: "Metrics Mock"
                color: "#1e2a3d"
                font.pixelSize: 16
                font.bold: true
            }

            Repeater {
                model: root.metricsModel

                Rectangle {
                    width: contentColumn.width
                    height: 48
                    radius: 8
                    color: "#ffffff"
                    border.color: "#dfe6f0"

                    Row {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 8

                        Text {
                            width: parent.width * 0.54
                            text: model.label
                            color: "#58657a"
                            font.pixelSize: 12
                            elide: Text.ElideRight
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            text: model.valueText + (model.unitText.length > 0 ? " " + model.unitText : "")
                            color: "#26354d"
                            font.pixelSize: 13
                            font.bold: true
                            elide: Text.ElideRight
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }

            Text {
                text: "Action Hints Mock"
                color: "#1e2a3d"
                font.pixelSize: 16
                font.bold: true
            }

            ActionHintList {
                width: parent.width
                hintModel: root.actionHintModel
                maxRows: 2
            }
        }
    }
}
