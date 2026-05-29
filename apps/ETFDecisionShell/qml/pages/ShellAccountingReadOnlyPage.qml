import QtQuick
import ETFDecisionTerminal.ShellAccounting 1.0
import "../components/readonly"

Rectangle {
    id: root
    objectName: "shellAccountingReadOnlyPage"

    property ShellAccountingPresenter accountingPresenter: null
    readonly property bool presenterAvailable: accountingPresenter !== null
    readonly property string shellState: "UNAVAILABLE"

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    Column {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Row {
            width: parent.width
            spacing: 12

            Text {
                text: "ShellAccounting"
                color: "#18202f"
                font.pixelSize: 24
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                height: 28
            }

            ReadOnlyStatusBadge {
                objectName: "shellAccountingReadOnlyBadge"
                status: root.shellState
                textOverride: root.presenterAvailable ? "UNAVAILABLE / READ ONLY" : "UNAVAILABLE"
            }
        }

        Rectangle {
            objectName: "shellAccountingReadOnlyNotice"
            width: parent.width
            height: 54
            radius: 8
            color: "#eef4ff"
            border.color: "#a8c4f5"

            Text {
                anchors.fill: parent
                anchors.margins: 12
                text: "Read-only accounting shell. Production data binding is not connected yet, so this page remains disabled and unavailable-safe."
                color: "#244464"
                font.pixelSize: 13
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            objectName: "shellAccountingUnavailablePanel"
            width: parent.width
            height: 172
            radius: 8
            color: "#f8fbff"
            border.color: "#c8d7ee"

            Column {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 10

                Text {
                    objectName: "shellAccountingPresenterState"
                    text: root.presenterAvailable ? "Presenter: provided" : "Presenter: not provided"
                    color: "#18202f"
                    font.pixelSize: 16
                    font.bold: true
                }

                Text {
                    objectName: "shellAccountingDisabledState"
                    width: parent.width
                    text: root.presenterAvailable
                        ? "The production-owned ShellAccounting presenter is available, but real accounting data is not connected."
                        : "No accounting presenter context is available. The page shows Unavailable instead of data."
                    color: "#465066"
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                }

                Text {
                    objectName: "shellAccountingBoundaryText"
                    width: parent.width
                    text: "Status: read-only, disabled, unavailable-safe. Amounts and rows are intentionally not displayed."
                    color: "#667086"
                    font.pixelSize: 13
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
