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
    title: "ETFDecisionShell 本地试用"

    readonly property var injectedDiagnosticAdapter: diagnosticAdapter
    readonly property var injectedReadOnlyDataController: readOnlyDataController
    readonly property var injectedAccountingPresenter: accountingPresenter
    readonly property bool injectedDailyUseMode: dailyUseMode
    readonly property string injectedDailyUseServiceName: dailyUseServiceName
    readonly property string injectedDailyUseDatabasePath: dailyUseDatabasePath
    readonly property string injectedDailyUseConnectionStatus: dailyUseConnectionStatus
    readonly property string injectedDailyUseConnectionIssue: dailyUseConnectionIssue

    AppShell {
        objectName: "appShell"
        anchors.fill: parent
        diagnosticAdapter: root.injectedDiagnosticAdapter
        readOnlyDataController: root.injectedReadOnlyDataController
        accountingPresenter: root.injectedAccountingPresenter
        navigationController: shellNavigationController
        statusController: shellStatusController
        dailyUseMode: root.injectedDailyUseMode
        dailyUseServiceName: root.injectedDailyUseServiceName
        dailyUseDatabasePath: root.injectedDailyUseDatabasePath
        dailyUseConnectionStatus: root.injectedDailyUseConnectionStatus
        dailyUseConnectionIssue: root.injectedDailyUseConnectionIssue
    }
}
