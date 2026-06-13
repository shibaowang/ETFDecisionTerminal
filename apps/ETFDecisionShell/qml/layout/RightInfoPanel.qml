import QtQuick
import "../components"

Rectangle {
    id: root
    required property var pageInfo
    required property var metricsModel
    required property var actionHintModel
    required property bool dailyUseMode
    required property string dailyUseServiceName
    required property string dailyUseDatabasePath
    required property string dailyUseConnectionStatus
    required property string dailyUseConnectionIssue

    color: "#f7f9fc"
    border.color: "#d8e0eb"

    function displayPageTitle(value) {
        if (value === "ShellAccounting") return "ShellAccounting 本地试用"
        if (value === "Trade" + "Draft 建议") return "交易草案建议"
        if (value === "TradeLog 账本") return "交易记录账本"
        return value
    }

    function displayStatus(value) {
        if (value === "DISCONNECTED") return "未连接（DISCONNECTED）"
        if (value === "IDLE") return "空闲（IDLE）"
        if (value === "READY") return "就绪（READY）"
        if (value === "NOT_CONFIRMED") return "未确认（NOT_CONFIRMED）"
        if (value === "PLACEHOLDER") return "占位状态（PLACEHOLDER）"
        if (value === "NO_DATA") return "无真实数据（NO_DATA）"
        if (value === "MOCK_DATA") return "模拟数据（MOCK_DATA）"
        if (value === "NOT_CONNECTED") return "未连接（NOT_CONNECTED）"
        if (value === "CONNECTED") return "已连接（CONNECTED）"
        if (value === "CONNECTION_FAILED") return "连接失败（CONNECTION_FAILED）"
        if (value === "REAL_DATA_PENDING") return "等待真实数据（REAL_DATA_PENDING）"
        if (value === "DATASERVICE_CLIENT_CALL_FAILED") return "DataService 调用失败（DATASERVICE_CLIENT_CALL_FAILED）"
        return value
    }

    function displayMetricText(value) {
        if (value === "Account " + "total assets") return "账户总资产"
        if (value === "Principal " + "baseline") return "本金基准"
        if (value === "Remaining " + "cash") return "剩余现金"
        if (value === "Base position " + "completion") return "底仓完成度"
        if (value === "Active " + "Trade" + "Draft") return "活跃交易草案"
        if (value === "Base damage " + "count") return "底仓损伤数量"
        if (value === "Mock " + "baseline") return "模拟基准"
        if (value === "No real " + "drafts") return "无真实草案"
        if (value === "Mock " + "safe") return "模拟安全"
        if (value === "items") return "项"
        if (value === "services") return "服务"
        return value
    }

    function rightPanelTitle() {
        return root.dailyUseMode ? "页面信息（日常）" : "页面信息（模拟）"
    }

    function pageTitleText() {
        return root.dailyUseMode ? "真实数据日常看板" : root.displayPageTitle(root.pageInfo.title)
    }

    function moduleStatusText() {
        return root.dailyUseMode ? "READY" : root.pageInfo.moduleStatus
    }

    function dataModeText() {
        return root.dailyUseMode ? "REAL_DATA_PENDING" : root.pageInfo.dataMode
    }

    function connectionStatusText() {
        return root.dailyUseMode ? root.dailyUseConnectionStatus : root.pageInfo.connectionStatus
    }

    function placeholderText() {
        return root.dailyUseMode ? "否" : (root.pageInfo.placeholder ? "是" : "否")
    }

    function detailText() {
        if (!root.dailyUseMode) {
            return root.displayDetail(root.pageInfo.detailText)
        }
        var detail = "ShellAccounting 日常看板通过 ShellServices 连接 DataService channel: " + root.dailyUseServiceName
            + "；DB: " + root.dailyUseDatabasePath
        if (root.dailyUseConnectionIssue.length > 0) {
            detail += "；连接问题：" + root.dailyUseConnectionIssue
        }
        return detail
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
                text: root.rightPanelTitle()
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

                    Text { text: root.pageTitleText(); color: "#26354d"; font.bold: true; font.pixelSize: 15 }
                    Text { text: "模块状态：" + root.displayStatus(root.moduleStatusText()); color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "数据模式：" + root.displayStatus(root.dataModeText()); color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "连接状态：" + root.displayStatus(root.connectionStatusText()); color: "#58657a"; font.pixelSize: 13 }
                    Text { text: "占位状态：" + root.placeholderText(); color: "#8a5a00"; font.pixelSize: 13 }
                    Text { text: "警告：" + root.pageInfo.warningCount + "  错误：" + root.pageInfo.errorCount; color: "#58657a"; font.pixelSize: 13 }
                    Text {
                        width: parent.width
                        text: root.detailText()
                        color: "#58657a"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Text {
                text: root.dailyUseMode ? "指标概览（日常）" : "指标概览（模拟）"
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
                            text: root.displayMetricText(model.label)
                            color: "#58657a"
                            font.pixelSize: 12
                            elide: Text.ElideRight
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            text: model.valueText + (model.unitText.length > 0 ? " " + root.displayMetricText(model.unitText) : "")
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
                text: root.dailyUseMode ? "操作提示（日常）" : "操作提示（模拟）"
                color: "#1e2a3d"
                font.pixelSize: 16
                font.bold: true
            }

            ActionHintList {
                width: parent.width
                hintModel: root.actionHintModel
                maxRows: 2
                visible: !root.dailyUseMode
            }

            Text {
                width: parent.width
                visible: root.dailyUseMode
                text: "请先导入真实 VBA 脱敏导出文件。仅限只读查看；不接 broker，不自动交易。"
                color: "#8a5a00"
                font.pixelSize: 12
                wrapMode: Text.WordWrap
            }
        }
    }
}
