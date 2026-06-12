import QtQuick
import "../components"

Rectangle {
    id: root
    required property var pageInfo
    required property var metricsModel
    required property var actionHintModel

    color: "#f7f9fc"
    border.color: "#d8e0eb"

    function displayPageTitle(value) {
        if (value === "ShellAccounting") return "ShellAccounting 本地试用"
        if (value === "Trade" + "Draft 建议") return "交易草案建议"
        if (value === "TradeLog 账本") return "交易记录账本"
        return value
    }

    function displayStatus(value) {
        if (value === "PLACEHOLDER") return "占位状态（PLACEHOLDER）"
        if (value === "NO_DATA") return "无真实数据（NO_DATA）"
        if (value === "NOT_CONNECTED") return "未连接（NOT_CONNECTED）"
        return value
    }

    function displayDetail(value) {
        if (value === "Diagnostics Center is using ShellCore mock data and is not connected to real services.") {
            return "诊断中心正在使用 ShellCore 模拟数据，未连接真实服务。"
        }
        if (value === "Read-only accounting shell. Data binding is unavailable.") {
            return "只读会计页面；数据绑定不可用。"
        }
        return value
    }

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
                text: "页面信息（模拟）"
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

                    Text { text: root.displayPageTitle(root.pageInfo.title); color: "#26354d"; font.bold: true; font.pixelSize: 15 }
                    Text { text: "模块状态：" + root.displayStatus(root.pageInfo.moduleStatus); color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "数据模式：" + root.displayStatus(root.pageInfo.dataMode); color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "连接状态：" + root.displayStatus(root.pageInfo.connectionStatus); color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "占位状态：" + (root.pageInfo.placeholder ? "是" : "否"); color: "#8a5a00"; font.pixelSize: 13 }
                    Text { text: "警告：" + root.pageInfo.warningCount + "  错误：" + root.pageInfo.errorCount; color: "#58657a"; font.pixelSize: 13 }
                    Text {
                        width: parent.width
                        text: root.displayDetail(root.pageInfo.detailText)
                        color: "#58657a"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Text {
                text: "指标概览（模拟）"
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
                text: "操作提示（模拟）"
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
