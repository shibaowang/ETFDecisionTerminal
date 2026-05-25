import QtQuick

Item {
    id: root
    required property var diagnosticAdapter
    property string currentPageKey: "dashboard"
    property string currentPageTitle: "首页驾驶舱"

    readonly property var navigationItems: [
        { key: "dashboard", title: "首页驾驶舱" },
        { key: "market", title: "行情监控" },
        { key: "accountPortfolio", title: "账户与组合" },
        { key: "position", title: "持仓明细" },
        { key: "strategy", title: "策略中心" },
        { key: "tradeDraft", title: "TradeDraft 建议" },
        { key: "tradeConfirm", title: "成交确认" },
        { key: "tradeLog", title: "TradeLog 账本" },
        { key: "cashPlan", title: "资金分配计划" },
        { key: "otcChannel", title: "OTC A/C 通道" },
        { key: "alert", title: "预警中心" },
        { key: "diagnostics", title: "诊断中心" },
        { key: "settings", title: "系统设置" }
    ]

    function navigateTo(key) {
        for (let i = 0; i < navigationItems.length; ++i) {
            if (navigationItems[i].key === key) {
                currentPageKey = key
                currentPageTitle = navigationItems[i].title
                return true
            }
        }
        return false
    }

    Rectangle {
        anchors.fill: parent
        color: "#101827"
    }

    TopStatusBar {
        id: topStatusBar
        objectName: "topStatusBar"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 52
        currentPageTitle: root.currentPageTitle
    }

    SideNavigation {
        id: sideNavigation
        objectName: "sideNavigation"
        anchors.left: parent.left
        anchors.top: topStatusBar.bottom
        anchors.bottom: bottomLogPanel.top
        width: 220
        items: root.navigationItems
        currentKey: root.currentPageKey
        onNavigate: function(key) {
            root.navigateTo(key)
        }
    }

    RightInfoPanel {
        id: rightInfoPanel
        objectName: "rightInfoPanel"
        anchors.top: topStatusBar.bottom
        anchors.right: parent.right
        anchors.bottom: bottomLogPanel.top
        width: 280
    }

    ContentHost {
        id: contentHost
        objectName: "contentHost"
        anchors.left: sideNavigation.right
        anchors.right: rightInfoPanel.left
        anchors.top: topStatusBar.bottom
        anchors.bottom: bottomLogPanel.top
        pageKey: root.currentPageKey
        pageTitle: root.currentPageTitle
        diagnosticAdapter: root.diagnosticAdapter
    }

    BottomLogPanel {
        id: bottomLogPanel
        objectName: "bottomLogPanel"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 120
    }
}
