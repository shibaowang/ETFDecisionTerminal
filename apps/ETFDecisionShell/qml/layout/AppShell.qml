import QtQuick

Item {
    id: root
    required property var diagnosticAdapter
    required property var navigationController
    property string currentPageKey: navigationController.currentPageKey

    function navigateTo(key) {
        return navigationController.selectPage(key)
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
        currentPageTitle: root.navigationController.currentPageTitle
    }

    SideNavigation {
        id: sideNavigation
        objectName: "sideNavigation"
        anchors.left: parent.left
        anchors.top: topStatusBar.bottom
        anchors.bottom: bottomLogPanel.top
        width: 220
        navigationModel: root.navigationController.navigationModel
        currentKey: root.navigationController.currentPageKey
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
        pageKey: root.navigationController.currentPageKey
        pageTitle: root.navigationController.currentPageTitle
        pageDescription: root.navigationController.currentPageDescription
        pageQmlComponent: root.navigationController.currentPageQmlComponent
        pagePlaceholder: root.navigationController.currentPagePlaceholder
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
