import QtQuick
import QtQuick.Window
import "pages"

Window {
    id: root
    width: 1180
    height: 760
    minimumWidth: 900
    minimumHeight: 620
    visible: true
    color: "#f6f7f9"
    title: "ETFDecisionShell Diagnostics Mock"

    DiagnosticsMockPage {
        anchors.fill: parent
        adapter: diagnosticAdapter
    }
}
