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

    color: "#edf1f6"

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
        }
    }

    Component {
        id: placeholderPage
        PlaceholderPage {
            objectName: "placeholderPage"
            moduleName: root.pageTitle
            title: root.pageTitle
            description: root.pageDescription
            metricsModel: root.metricsModel
            actionHintModel: root.actionHintModel
        }
    }
}
