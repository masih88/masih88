import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "Status.js" as Status

// Result rows (label · value · unit) followed by the check messages.
// Column positioner (not ColumnLayout): Qt 6.4 layouts mis-size Repeater rows created later.
Column {
    id: view
    property var outputs: []
    property var messages: []
    readonly property color accentText: Calc.darkTheme ? "#F2C14E" : "#1F3A5F"

    Layout.fillWidth: true
    spacing: 10

    // rows with anchors only (no layouts): wrapped labels always get their final height
    Repeater {
        model: view.outputs
        delegate: Item {
            id: row
            required property var modelData
            required property int index
            // long text results (e.g. "decisive") go below their label
            readonly property bool stacked: modelData.value.length > 16
            readonly property real gap: index > 0 ? 7 : 0
            width: view.width
            // first-line baselines of label and value are aligned
            readonly property real labelShift: stacked ? 0 : Math.max(0, valueLbl.baselineOffset - label.baselineOffset)
            readonly property real valueShift: Math.max(0, label.baselineOffset - valueLbl.baselineOffset)
            height: gap + (stacked ? label.height + 2 + valueText.height
                                   : Math.max(labelShift + label.height, valueShift + valueRow.height))

            Rectangle {
                visible: row.index > 0
                width: parent.width
                height: 1
                color: Calc.darkTheme ? "#2A3445" : "#EEF1F5"
            }
            Label {
                id: label
                y: row.gap + row.labelShift
                width: row.stacked ? row.width : Math.max(80, row.width - valueRow.width - 10)
                text: row.modelData.label
                wrapMode: Text.Wrap
                font.pixelSize: 14
                opacity: 0.8
            }
            Row {
                id: valueRow
                visible: !row.stacked
                anchors.right: parent.right
                y: row.gap + row.valueShift
                spacing: 6
                Label {
                    id: valueLbl
                    text: row.modelData.value
                    font.pixelSize: row.modelData.key ? 20 : 17
                    font.weight: Font.Bold
                    color: row.modelData.key ? view.accentText : (Calc.darkTheme ? "#E6EAF0" : "#1D2633")
                }
                Label {
                    id: unit
                    anchors.baseline: valueLbl.baseline
                    visible: row.modelData.unit !== ""
                    width: visible ? implicitWidth : 0
                    text: row.modelData.unit
                    font.pixelSize: 13
                    opacity: 0.75
                }
            }
            Label {
                id: valueText
                visible: row.stacked
                y: row.gap + label.height + 2
                width: row.width
                text: row.modelData.value + (row.modelData.unit ? " " + row.modelData.unit : "")
                wrapMode: Text.Wrap
                font.pixelSize: 15
                font.weight: Font.Bold
                color: row.modelData.key ? view.accentText : (Calc.darkTheme ? "#E6EAF0" : "#1D2633")
            }
        }
    }

    Repeater {
        model: view.messages
        delegate: Rectangle {
            id: msg
            required property var modelData
            readonly property var colors: Status.colors(modelData.severity, Calc.darkTheme)
            width: view.width
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
