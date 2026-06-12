import QtQuick

Column {
    id: root
    required property var hintModel
    property int maxRows: 3
    spacing: 8

    function displayText(value) {
        if (value === "Placeholder " + "module") {
            return "占位模块"
        }
        if (value === "This module is a placeholder " + "mock.") {
            return "当前模块为占位模拟页。"
        }
        if (value === "This page has no real service data; "
                + "this action hint is not executable.") {
            return "本页未连接真实服务数据；此提示不会执行真实操作。"
        }
        if (value === "Real business data is not " + "connected.") {
            return "未连接真实业务数据。"
        }
        if (value === "Review cash " + "mock") {
            return "查看现金模拟提示"
        }
        if (value === "Placeholder hint " + "only; no action is executed.") {
            return "仅占位提示，不会执行真实操作。"
        }
        if (value === "Placeholder hint " + "only") {
            return "仅占位提示"
        }
        if (value === "Review " + "Trade" + "Draft mock") {
            return "查看交易草案模拟提示"
        }
        if (value === "No real " + "Trade" + "Draft lifecycle is connected.") {
            return "未接入真实交易草案生命周期。"
        }
        if (value === "Review diagnostics " + "mock") {
            return "查看诊断模拟提示"
        }
        if (value === "Diagnostics data comes from ShellCore mock models.") {
            return "诊断数据来自 ShellCore 模拟模型。"
        }
        if (value === "Mock " + "only") {
            return "仅模拟"
        }
        return value
    }

    Repeater {
        model: root.hintModel

        Rectangle {
            width: parent.width
            height: 72
            radius: 8
            color: "#ffffff"
            border.color: model.severity === "WARNING" ? "#d9b45f" : "#dfe6f0"
            visible: index < root.maxRows

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 4

                Text {
                    width: parent.width
                    text: root.displayText(model.title)
                    color: "#26354d"
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width
                    text: root.displayText(model.description)
                    color: "#58657a"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width
                    text: model.enabled ? root.displayText(model.actionText) : "仅演示，不会执行真实操作"
                    color: "#8a5a00"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
            }
        }
    }
}
