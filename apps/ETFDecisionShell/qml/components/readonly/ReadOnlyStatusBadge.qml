import QtQuick

Rectangle {
    id: root
    objectName: "readOnlyStatusBadge"
    property string status: "READONLY"
    property string textOverride: ""

    readonly property string normalizedStatus: status.toUpperCase()

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
        text: root.textOverride.length > 0 ? root.textOverride : root.normalizedStatus
        color: root.normalizedStatus === "ERROR" ? "#8a2f27" : "#23344f"
        font.pixelSize: 11
        font.bold: true
    }
}
