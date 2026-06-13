import QtQuick

Rectangle {
    id: root
    required property string currentPageTitle
    required property var pageInfo
    required property bool dailyUseMode
    required property string dailyUseServiceName
    required property string dailyUseConnectionStatus
    color: "#121d2e"
    border.color: "#26354d"

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
        if (value === "DATASERVICE_CLIENT_CALL_FAILED") return "DataService 调用失败（DATASERVICE_CLIENT_CALL_FAILED）"
        return value
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

    function modeBadgeText() {
        if (root.dailyUseMode) {
            return "daily-use real data mode - " + root.dailyUseConnectionStatus + " - " + root.dailyUseServiceName
        }
        return "local trial mock mode - real service not connected"
    }

    Row {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 18
        spacing: 16

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "跨境 ETF 智能投资决策终端"
            color: "#eef4ff"
            font.pixelSize: 18
            font.bold: true
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.displayPageTitle(root.currentPageTitle)
            color: "#aebbd0"
            font.pixelSize: 14
        }

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: mockLabel.implicitWidth + 24
            height: 28
            radius: 14
            color: "#342d18"
            border.color: "#8d6a20"

            Text {
                id: mockLabel
                anchors.centerIn: parent
                text: root.modeBadgeText()
                color: "#ffd77a"
                font.pixelSize: 12
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "模块：" + root.displayStatus(root.moduleStatusText())
            color: "#8fa1ba"
            font.pixelSize: 12
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "数据：" + root.displayStatus(root.dataModeText())
            color: "#8fa1ba"
            font.pixelSize: 12
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "连接状态：" + root.displayStatus(root.connectionStatusText())
            color: "#8fa1ba"
            font.pixelSize: 12
        }
    }
}
