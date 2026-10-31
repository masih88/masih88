import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "Status.js" as Status

// Result rows (label · value · unit) followed by the check messages.
ColumnLayout {
    id: view
    property var outputs: []
    property var messages: []
    readonly property color accentText: Calc.darkTheme ? "#F2C14E" : "#1F3A5F"

    Layout.fillWidth: true
    spacing: 10

    Repeater {
        model: view.outputs
        delegate: ColumnLayout {
            required property var modelData
            required property int index
            Layout.fillWidth: true
            spacing: 6
            Rectangle {
                visible: index > 0
                Layout.fillWidth: true
                implicitHeight: 1
                color: Calc.darkTheme ? "#2A3445" : "#EEF1F5"
            }
            GridLayout {
                id: resultRow
                // long text results (e.g. "decisive") go below their label
                readonly property bool stacked: modelData.value.length > 16
                Layout.fillWidth: true
                columns: stacked ? 1 : 3
                columnSpacing: 10
                rowSpacing: 2
                Label {
                    // width from the row (not from the wrapped text) – avoids layout loops
                    Layout.fillWidth: true
                    Layout.preferredWidth: resultRow.stacked
                                           ? resultRow.width
                                           : Math.max(80, resultRow.width - valueLabel.implicitWidth
                                                      - (unitLabel.visible ? unitLabel.implicitWidth + resultRow.columnSpacing : 0)
                                                      - resultRow.columnSpacing)
                    text: modelData.label
                    wrapMode: Text.Wrap
                    font.pixelSize: 14
                    opacity: 0.8
                }
                Label {
                    id: valueLabel
                    Layout.fillWidth: resultRow.stacked
                    Layout.preferredWidth: resultRow.stacked ? resultRow.width : -1
                    text: modelData.value + (resultRow.stacked && modelData.unit ? " " + modelData.unit : "")
                    wrapMode: resultRow.stacked ? Text.Wrap : Text.NoWrap
                    font.pixelSize: resultRow.stacked ? 15 : (modelData.key ? 20 : 17)
                    font.weight: Font.Bold
                    color: modelData.key ? view.accentText : (Calc.darkTheme ? "#E6EAF0" : "#1D2633")
                }
                Label {
                    id: unitLabel
                    visible: !resultRow.stacked && modelData.unit !== ""
                    text: modelData.unit
                    font.pixelSize: 13
                    opacity: 0.75
                }
            }
        }
    }

    Repeater {
        model: view.messages
        delegate: Rectangle {
            id: msg
            required property var modelData
            readonly property var colors: Status.colors(modelData.severity, Calc.darkTheme)
            Layout.fillWidth: true
            implicitHeight: msgRow.implicitHeight + 18
            radius: 10
            color: colors[0]
            RowLayout {
                id: msgRow
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 12
                spacing: 10
                Label {
                    Layout.alignment: Qt.AlignTop
                    text: msg.colors[2]
                    color: msg.colors[1]
                    font.bold: true
                    font.pixelSize: 15
                }
                Label {
                    Layout.fillWidth: true
                    text: msg.modelData.text
                    color: msg.colors[1]
                    wrapMode: Text.Wrap
                    font.pixelSize: 13
                }
            }
        }
    }
}
