import QtQuick

Item {
    id: root
    required property var diagnosticAdapter
    required property var readOnlyDataController
    required property var accountingPresenter
    required property var navigationController
    required property var statusController
    property string currentPageKey: navigationController.currentPageKey

    function navigateTo(key) {
        return navigationController.selectPage(key)
    }

    Component.onCompleted: statusController.updateCurrentPageStatus(navigationController.currentPageKey)

    Connections {
        target: root.navigationController
        function onCurrentPageChanged() {
            root.statusController.updateCurrentPageStatus(root.navigationController.currentPageKey)
        }
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
        pageInfo: root.statusController.pageInfo
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
        pageInfo: root.statusController.pageInfo
        metricsModel: root.statusController.metricsModel
        actionHintModel: root.statusController.actionHintModel
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
        readOnlyDataController: root.readOnlyDataController
        accountingPresenter: root.accountingPresenter
        metricsModel: root.statusController.metricsModel
        actionHintModel: root.statusController.actionHintModel
    }

    BottomLogPanel {
        id: bottomLogPanel
        objectName: "bottomLogPanel"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 120
        logModel: root.statusController.logModel
    }
}
