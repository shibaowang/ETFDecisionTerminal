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

    color: "#edf1f6"

    Loader {
        id: pageLoader
        objectName: "contentLoader"
        anchors.fill: parent
        anchors.margins: 14
        sourceComponent: root.pageQmlComponent === "DiagnosticsMockPage" ? diagnosticsPage : placeholderPage
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
            description: root.pageDescription
        }
    }
}
