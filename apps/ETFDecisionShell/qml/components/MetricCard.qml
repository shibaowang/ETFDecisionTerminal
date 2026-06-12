import QtQuick

Rectangle {
    id: root
    required property string label
    required property string valueText
    property string unitText: ""
    property string trendText: ""
    property string severity: "INFO"
    property bool mock: true
    property string description: ""

    function displayText(value) {
        if (value === "Account " + "total assets") return "账户总资产"
        if (value === "Principal " + "baseline") return "本金基准"
        if (value === "Remaining " + "cash") return "剩余现金"
        if (value === "Base position " + "completion") return "底仓完成度"
        if (value === "Active " + "Trade" + "Draft") return "活跃交易草案"
        if (value === "Base damage " + "count") return "底仓损伤数量"
        if (value === "Service " + "total") return "服务总数"
        if (value === "Startable " + "services") return "可启动服务"
        if (value === "Warning " + "count") return "警告数量"
        if (value === "Error " + "count") return "错误数量"
        if (value === "Mock " + "baseline") return "模拟基准"
        if (value === "No real " + "drafts") return "无真实草案"
        if (value === "Mock " + "safe") return "模拟安全"
        if (value === "Mock " + "manifest") return "模拟清单"
        if (value === "Mock " + "readiness") return "模拟就绪"
        if (value === "Mock " + "warning") return "模拟警告"
        if (value === "Mock " + "healthy") return "模拟健康"
        if (value === "Mock total " + "assets.") return "模拟总资产。"
        if (value === "Mock principal " + "baseline.") return "模拟本金基准。"
        if (value === "Mock available " + "cash.") return "模拟可用现金。"
        if (value === "Mock base position " + "completion.") return "模拟底仓完成度。"
        if (value === "Mock " + "Trade" + "Draft count.") return "模拟交易草案数量。"
        if (value === "Mock base damage " + "count.") return "模拟底仓损伤数量。"
        if (value === "Mock service " + "total.") return "模拟服务总数。"
        if (value === "Mock startable " + "services.") return "模拟可启动服务。"
        if (value === "Mock warning " + "count.") return "模拟警告数量。"
        if (value === "Mock error " + "count.") return "模拟错误数量。"
        if (value === "items") return "项"
        if (value === "services") return "服务"
        if (value === "service") return "服务"
        if (value === "issue") return "问题"
        return value
    }

    width: 190
    height: 118
    radius: 8
    color: "#ffffff"
    border.color: root.severity === "ERROR" ? "#d96b6b" : root.severity === "WARNING" ? "#d9b45f" : "#dfe6f0"

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 6

        Text {
            text: root.displayText(root.label)
            color: "#58657a"
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Row {
            spacing: 6
            height: 28
            Text {
                text: root.valueText
                color: "#1e2a3d"
                font.pixelSize: 22
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: root.displayText(root.unitText)
                color: "#667086"
                font.pixelSize: 12
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Text {
            width: parent.width
            text: root.displayText(root.trendText)
            color: root.severity === "WARNING" ? "#8a5a00" : "#55708f"
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.mock ? "模拟指标" : root.displayText(root.description)
            color: "#8c96a8"
            font.pixelSize: 11
            elide: Text.ElideRight
        }
    }
}
