import QtQuick
import "../pages"
import "../components"

Rectangle {
    id: root
    required property string pageKey
    required property string pageTitle
    required property var diagnosticAdapter

    color: "#edf1f6"

    Loader {
        id: pageLoader
        objectName: "contentLoader"
        anchors.fill: parent
        anchors.margins: 14
        sourceComponent: root.pageKey === "diagnostics" ? diagnosticsPage : placeholderPage
    }

    Component {
        id: diagnosticsPage
        DiagnosticsMockPage {
            objectName: "diagnosticsMockPage"
            adapter: root.diagnosticAdapter
        }
    }

    Component {
        id: placeholderPage
        PlaceholderPage {
            objectName: "placeholderPage"
            moduleName: root.pageTitle
            title: root.pageTitle
            description: "当前页面为占位 Mock，业务功能尚未接入。"
        }
    }
}
