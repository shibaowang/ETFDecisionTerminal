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

    AppShell {
        objectName: "appShell"
        anchors.fill: parent
        diagnosticAdapter: diagnosticAdapter
        navigationController: shellNavigationController
    }
}
