import QtQuick

Rectangle {
    id: root
    required property string title
    required property string moduleName
    property string description: "当前为占位状态（模拟），业务功能尚未接入。"
    property var metricsModel: null
    property var actionHintModel: null

    radius: 8
    color: "#ffffff"
    border.color: "#d8e0eb"

    Column {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 14

        Text {
            text: root.title
            color: "#18202f"
            font.pixelSize: 24
            font.bold: true
        }

        Rectangle {
            width: mockText.implicitWidth + 24
            height: 30
            radius: 15
            color: "#fff4d7"
            border.color: "#e3bc5d"

            Text {
                id: mockText
                anchors.centerIn: parent
                text: "当前为占位状态（模拟）"
                color: "#7a4a00"
                font.pixelSize: 13
                font.bold: true
            }
        }

        Text {
            width: parent.width
            text: root.description
            color: "#465066"
            font.pixelSize: 15
            wrapMode: Text.WordWrap
        }

        Text {
            width: parent.width
            text: "模块：" + root.moduleName + "\n后续任务将接入真实数据。\n本页面不访问数据库、不连接服务、不包含业务逻辑。"
            color: "#667086"
            font.pixelSize: 13
            lineHeight: 1.35
        }

        Grid {
            objectName: "placeholderMetricGrid"
            width: parent.width
            columns: Math.max(1, Math.floor(width / 210))
            columnSpacing: 12
            rowSpacing: 12
            visible: root.metricsModel !== null

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
    }
}
