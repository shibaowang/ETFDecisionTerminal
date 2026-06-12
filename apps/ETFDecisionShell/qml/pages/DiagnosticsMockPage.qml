import QtQuick
import "../components"

Item {
    id: root
    required property var adapter
    property int selectedServiceIndex: -1

    Rectangle {
        anchors.fill: parent
        color: "#f6f7f9"
    }

    Column {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Column {
            width: parent.width
            spacing: 6

            Text {
                text: "跨境 ETF 智能投资决策终端 - 服务监控清单（模拟）"
                color: "#18202f"
                font.pixelSize: 24
                font.bold: true
            }

            Text {
                text: "当前为模拟数据，未连接真实服务。"
                color: "#7a4a00"
                font.pixelSize: 14
            }
        }

        SummaryCard {
            width: parent.width
            height: 146
            summary: root.adapter.summaryObject
        }

        Row {
            width: parent.width
            height: 40
            spacing: 10

            Repeater {
                model: [
                    { label: "加载健康样例", action: "healthy" },
                    { label: "加载警告样例", action: "warning" },
                    { label: "加载错误样例", action: "error" },
                    { label: "加载混合样例", action: "mixed" },
                    { label: "显示全部", action: "all" },
                    { label: "仅显示阻塞项", action: "blocked" },
                    { label: "仅显示问题项", action: "issues" }
                ]

                Rectangle {
                    width: Math.max(112, buttonLabel.implicitWidth + 24)
                    height: 36
                    radius: 6
                    color: mouseArea.containsMouse ? "#e1e7ef" : "#ffffff"
                    border.color: "#cfd7e3"

                    Text {
                        id: buttonLabel
                        anchors.centerIn: parent
                        text: modelData.label
                        color: "#243047"
                        font.pixelSize: 13
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (modelData.action === "healthy") root.adapter.loadMockHealthy()
                            else if (modelData.action === "warning") root.adapter.loadMockWithWarnings()
                            else if (modelData.action === "error") root.adapter.loadMockWithErrors()
                            else if (modelData.action === "mixed") root.adapter.loadMockMixed()
                            else if (modelData.action === "all") root.adapter.showAll()
                            else if (modelData.action === "blocked") root.adapter.showOnlyBlocked()
                            else if (modelData.action === "issues") root.adapter.showOnlyIssues()
                            root.selectedServiceIndex = -1
                        }
                    }
                }
            }
        }

        Row {
            width: parent.width
            height: parent.height - 24 - 24 - 16 - 50 - 146 - 40 - 16 - 16
            spacing: 16

            ServiceListView {
                width: Math.floor(parent.width * 0.58)
                height: parent.height
                model: root.adapter.serviceModel
                selectedIndex: root.selectedServiceIndex
                onServiceSelected: function(index) {
                    root.selectedServiceIndex = index
                    root.adapter.selectService(index)
                }
            }

            IssueListView {
                width: parent.width - Math.floor(parent.width * 0.58) - 16
                height: parent.height
                model: root.adapter.issueModel
            }
        }
    }
}
