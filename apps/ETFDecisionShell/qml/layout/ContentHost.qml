import QtQuick
import "../pages"
import "../components"

Rectangle {
    id: root
    required property string pageKey
    required property string pageTitle
    required property string pageDescription
    required property string pageQmlComponent
    required property bool pagePlaceholder
    required property var diagnosticAdapter
    required property var readOnlyDataController
    required property var accountingPresenter
    required property var metricsModel
    required property var actionHintModel
    required property bool dailyUseMode
    required property string dailyUseServiceName
    required property string dailyUseDatabasePath

    color: "#edf1f6"

    function displayPageTitle(value) {
        if (value === "ShellAccounting") return "ShellAccounting 本地试用"
        if (value === "Trade" + "Draft 建议") return "交易草案建议"
        if (value === "TradeLog 账本") return "交易记录账本"
        return value
    }

    function displayPageDescription(value) {
        if (value === "Read-only accounting shell. Data binding is unavailable.") {
            return "只读会计页面；数据绑定不可用。"
        }
        return value
    }

    Loader {
        id: pageLoader
        objectName: "contentLoader"
        anchors.fill: parent
        anchors.margins: 14
        sourceComponent: root.pageQmlComponent === "DiagnosticsMockPage"
            ? diagnosticsPage
            : root.pageQmlComponent === "ReadOnlyDataPage"
                ? readOnlyDataPage
            : root.pageQmlComponent === "AccountPortfolioReadOnlyPage"
                ? accountPortfolioReadOnlyPage
            : root.pageQmlComponent === "InstrumentStrategyReadOnlyPage"
                ? instrumentStrategyReadOnlyPage
            : root.pageQmlComponent === "ShellAccountingReadOnlyPage"
                ? shellAccountingReadOnlyPage
            : root.pageQmlComponent === "DashboardPlaceholderPage"
                ? dashboardPage
                : placeholderPage
    }

    Component {
        id: diagnosticsPage
        DiagnosticsMockPage {
            objectName: "diagnosticsMockPage"
            adapter: root.diagnosticAdapter
        }
    }

    Component {
        id: dashboardPage
        DashboardPlaceholderPage {
            objectName: "dashboardPlaceholderPage"
            metricsModel: root.metricsModel
            actionHintModel: root.actionHintModel
        }
    }

    Component {
        id: readOnlyDataPage
        ReadOnlyDataPage {
            objectName: "readOnlyDataPage"
            readOnlyDataController: root.readOnlyDataController
        }
    }

    Component {
        id: accountPortfolioReadOnlyPage
        AccountPortfolioReadOnlyPage {
            objectName: "accountPortfolioReadOnlyPage"
            readOnlyDataController: root.readOnlyDataController
        }
    }

    Component {
        id: instrumentStrategyReadOnlyPage
        InstrumentStrategyReadOnlyPage {
            objectName: "instrumentStrategyReadOnlyPage"
            readOnlyDataController: root.readOnlyDataController
        }
    }

    Component {
        id: shellAccountingReadOnlyPage
        ShellAccountingReadOnlyPage {
            objectName: "shellAccountingReadOnlyPage"
            accountingPresenter: root.accountingPresenter
            dailyUseMode: root.dailyUseMode
            dailyUseServiceName: root.dailyUseServiceName
            dailyUseDatabasePath: root.dailyUseDatabasePath
        }
    }

    Component {
        id: placeholderPage
        PlaceholderPage {
            objectName: "placeholderPage"
            moduleName: root.displayPageTitle(root.pageTitle)
            title: root.displayPageTitle(root.pageTitle)
            description: root.displayPageDescription(root.pageDescription)
            metricsModel: root.metricsModel
            actionHintModel: root.actionHintModel
        }
    }
}
