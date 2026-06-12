import QtQuick

Rectangle {
    id: root
    objectName: "readOnlyStatusBadge"
    property string status: "READONLY"
    property string textOverride: ""

    readonly property string normalizedStatus: status.toUpperCase()

    function displayStatus(value) {
        if (value === "READONLY") return "只读"
        if (value === "ACTIVE") return "启用"
        if (value === "ENABLED") return "启用"
        if (value === "DISABLED") return "禁用"
        if (value === "PLACEHOLDER") return "占位"
        if (value === "OK") return "正常"
        if (value === "ERROR") return "错误"
        if (value === "WARNING") return "警告"
        if (value === "UNAVAILABLE") return "不可用"
        return value
    }

    radius: 10
    height: 22
    width: Math.max(72, badgeText.implicitWidth + 20)
    color: normalizedStatus === "ACTIVE" || normalizedStatus === "ENABLED" || normalizedStatus === "OK"
        ? "#e7f6ec"
        : normalizedStatus === "DISABLED" || normalizedStatus === "PLACEHOLDER"
            ? "#eef1f5"
        : normalizedStatus === "ERROR"
            ? "#fdecea"
        : normalizedStatus === "WARNING"
            ? "#fff4d6"
            : "#eaf2ff"
    border.color: normalizedStatus === "ACTIVE" || normalizedStatus === "ENABLED" || normalizedStatus === "OK"
        ? "#9dd8b0"
        : normalizedStatus === "DISABLED" || normalizedStatus === "PLACEHOLDER"
            ? "#c9d1dc"
        : normalizedStatus === "ERROR"
            ? "#eeaaa2"
        : normalizedStatus === "WARNING"
            ? "#e8ca76"
            : "#aac7f6"

    Text {
        id: badgeText
        anchors.centerIn: parent
        text: root.textOverride.length > 0 ? root.textOverride : root.displayStatus(root.normalizedStatus)
        color: root.normalizedStatus === "ERROR" ? "#8a2f27" : "#23344f"
        font.pixelSize: 11
        font.bold: true
    }
}
