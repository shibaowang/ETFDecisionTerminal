import QtQuick
import QtQuick.Window
import "layout"

Window {
    id: root
    width: 1440
    height: 900
    minimumWidth: 1180
    minimumHeight: 760
    visible: true
    color: "#0f1724"
    title: "ETFDecisionShell Mock"

    readonly property var injectedDiagnosticAdapter: diagnosticAdapter
    readonly property var injectedReadOnlyDataController: readOnlyDataController
    readonly property var injectedAccountingPresenter: accountingPresenter

    AppShell {
        objectName: "appShell"
        anchors.fill: parent
        diagnosticAdapter: root.injectedDiagnosticAdapter
        readOnlyDataController: root.injectedReadOnlyDataController
        accountingPresenter: root.injectedAccountingPresenter
        navigationController: shellNavigationController
        statusController: shellStatusController
    }
}
