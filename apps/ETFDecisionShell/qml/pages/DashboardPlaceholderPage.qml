import QtQuick
import "../components"

Rectangle {
    id: root
    required property var metricsModel
    required property var actionHintModel

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    Column {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Text {
            text: "首页驾驶舱（模拟）"
            color: "#18202f"
            font.pixelSize: 24
            font.bold: true
        }

        Text {
            width: parent.width
            text: "当前指标来自 ShellCore 模拟指标，不访问数据库、不连接真实服务。"
            color: "#667086"
            font.pixelSize: 14
            wrapMode: Text.WordWrap
        }

        Grid {
            objectName: "dashboardMetricGrid"
            width: parent.width
            columns: Math.max(1, Math.floor(width / 210))
            columnSpacing: 12
            rowSpacing: 12

            Repeater {
                model: root.metricsModel
                MetricCard {
                    label: model.label
                    valueText: model.valueText
                    unitText: model.unitText
                    trendText: model.trendText
                    severity: model.severity
                    mock: model.mock
                    description: model.description
                }
            }
        }

        Text {
            text: "操作提示（模拟）"
            color: "#26354d"
            font.pixelSize: 16
            font.bold: true
        }

        ActionHintList {
            width: parent.width
            hintModel: root.actionHintModel
            maxRows: 2
        }
    }
}
