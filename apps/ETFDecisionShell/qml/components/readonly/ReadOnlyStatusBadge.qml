import QtQuick

Rectangle {
    id: root
    objectName: "readOnlyStatusBadge"
    property string status: "READONLY"
    property string textOverride: ""

    readonly property string normalizedStatus: status.toUpperCase()

    function displayStatus(value) {
        if (value === "READONLY") return "只读（READONLY）"
        if (value === "ACTIVE") return "启用（ACTIVE）"
        if (value === "ENABLED") return "启用（ENABLED）"
        if (value === "DISABLED") return "禁用（DISABLED）"
        if (value === "PLACEHOLDER") return "占位（PLACEHOLDER）"
        if (value === "OK") return "正常（OK）"
        if (value === "ERROR") return "错误（ERROR）"
        if (value === "WARNING") return "警告（WARNING）"
        if (value === "UNAVAILABLE") return "不可用（UNAVAILABLE）"
        if (value === "DISCONNECTED") return "未连接（DISCONNECTED）"
        if (value === "IDLE") return "空闲（IDLE）"
        if (value === "READY") return "就绪（READY）"
        if (value === "NO_DATA") return "无真实数据（NO_DATA）"
        if (value === "MOCK_DATA") return "模拟数据（MOCK_DATA）"
        if (value === "NOT_CONNECTED") return "未连接（NOT_CONNECTED）"
        if (value === "NOT_CONFIRMED") return "未确认（NOT_CONFIRMED）"
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
