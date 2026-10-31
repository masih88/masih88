import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "Status.js" as Status

// Bottom bar on phones: key results always visible, tap jumps to the full results.
Rectangle {
    id: bar
    property var outputs: []
    property string status: "none"
    signal tapped()

    readonly property color accentText: Calc.darkTheme ? "#F2C14E" : "#1F3A5F"
    readonly property var colors: Status.colors(status, Calc.darkTheme)

    implicitHeight: visible ? barRow.implicitHeight + 20 : 0
    color: Calc.darkTheme ? "#1C2330" : "#FFFFFF"

    Rectangle {
        anchors.top: parent.top
        width: parent.width
        height: 1
        color: Calc.darkTheme ? "#2A3445" : "#E2E6EC"
    }
    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 5
        color: bar.status === "none" ? "#1F3A5F" : bar.colors[1]
    }
    RowLayout {
        id: barRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 18
        anchors.rightMargin: 14
        spacing: 16

        Repeater {
            model: bar.outputs
            delegate: ColumnLayout {
                required property var modelData
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                spacing: 0
                Label {
                    Layout.fillWidth: true
                    text: modelData.short || modelData.label
                    font.pixelSize: 11
                    opacity: 0.65
                    elide: Text.ElideRight
                }
                Label {
                    Layout.fillWidth: true
                    text: modelData.value + (modelData.unit ? " " + modelData.unit : "")
                    font.pixelSize: 20
                    font.weight: Font.Bold
                    color: bar.accentText
                    elide: Text.ElideRight
                }
            }
        }
        Rectangle {
            visible: bar.status !== "none"
            Layout.preferredWidth: 30
            Layout.preferredHeight: 30
            radius: 15
            color: bar.colors[0]
            Label {
                anchors.centerIn: parent
                text: bar.colors[2]
                color: bar.colors[1]
                font.bold: true
                font.pixelSize: 15
            }
        }
    }
    TapHandler {
        onTapped: bar.tapped()
    }
}
