import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Renders input definitions from the core (number, choice, text). Used by calculators,
// project list settings and the row editor. Values are kept as text, keyed by input id.
// Built with positioners and anchors (no layouts inside): wrapped labels and hints always
// get their real height (Qt 6.4 layouts sometimes kept a stale one, so texts overlapped).
Column {
    id: form
    property var inputs: []
    property var values: ({})
    signal edited(string id, string text)

    readonly property real fieldWidth: 116
    readonly property real unitWidth: 58

    Layout.fillWidth: true
    spacing: 10

    function inputVisible(input, v) {
        if (!input.visibleIf)
            return true
        return input.visibleIf.values.indexOf(v[input.visibleIf.input]) >= 0
    }
    function optionIndex(options, value) {
        for (let i = 0; i < options.length; ++i)
            if (options[i].value === value)
                return i
        return 0
    }
    // text fields keep what the user typed; after a reset/new row the delegates are recreated
    function rebuild() {
        rep.model = []
        rep.model = Qt.binding(function () { return form.inputs })
    }

    Repeater {
        id: rep
        model: form.inputs

        delegate: Column {
            id: field
            required property var modelData
            readonly property bool isChoice: modelData.kind === "choice"
            readonly property bool isText: modelData.kind === "text"
            readonly property bool stacked: isChoice || isText
            readonly property string caption: modelData.label + (modelData.optional ? " · " + Calc.ui.optional : "")
            width: form.width
            visible: form.inputVisible(modelData, form.values)
            spacing: 2

            // group heading (e.g. "Leitung & Verlegung")
            Label {
                visible: (field.modelData.section || "") !== ""
                width: field.width
                topPadding: 8
                bottomPadding: 2
                text: field.modelData.section || ""
                wrapMode: Text.Wrap
                font.pixelSize: 13
                font.weight: Font.DemiBold
                color: Calc.darkTheme ? "#F2C14E" : "#1F6F5C"
            }

            // number: label · field · unit in one row
            Item {
                visible: !field.stacked
                width: field.width
                height: Math.max(numLabel.height, numInput.height)
                Label {
                    id: numLabel
                    width: Math.max(60, field.width - form.fieldWidth - form.unitWidth - 16)
                    anchors.verticalCenter: parent.verticalCenter
                    text: field.caption
                    font.pixelSize: 14
                    wrapMode: Text.Wrap
                    opacity: 0.85
                }
                TextField {
                    id: numInput
                    x: field.width - form.unitWidth - 8 - width
                    width: form.fieldWidth
                    anchors.verticalCenter: parent.verticalCenter
                    text: form.values[field.modelData.id] || ""
                    placeholderText: field.modelData.optional ? "–" : ""
                    horizontalAlignment: TextInput.AlignRight
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    selectByMouse: true
                    font.pixelSize: 16
                    onTextEdited: form.edited(field.modelData.id, text)
                }
                Label {
                    x: field.width - form.unitWidth
                    width: form.unitWidth
                    anchors.verticalCenter: parent.verticalCenter
                    text: field.modelData.unit
                    font.pixelSize: 13
                    opacity: 0.7
                    elide: Text.ElideRight
                }
            }

            // choice / text: label above the control
            Label {
                visible: field.stacked
                width: field.width
                text: field.caption
                font.pixelSize: field.isText ? 12 : 14
                wrapMode: Text.Wrap
                opacity: field.isText ? 0.7 : 0.85
            }
            TextField {
                visible: field.isText
                width: field.width
                text: form.values[field.modelData.id] || ""
                selectByMouse: true
                font.pixelSize: 15
                onTextEdited: form.edited(field.modelData.id, text)
            }
            ComboBox {
                id: combo
                visible: field.isChoice
                width: field.width
                model: field.isChoice ? field.modelData.options : []
                textRole: "label"
                valueRole: "value"
                currentIndex: form.optionIndex(field.modelData.options || [], form.values[field.modelData.id])
                onActivated: form.edited(field.modelData.id, currentValue)
                delegate: ItemDelegate {
                    required property var modelData
                    required property int index
                    width: ListView.view ? ListView.view.width : implicitWidth
                    highlighted: combo.highlightedIndex === index
                    contentItem: Label {
                        text: modelData.label
                        wrapMode: Text.Wrap
                        font.weight: combo.currentIndex === index ? Font.DemiBold : Font.Normal
                    }
                }
            }

            Label {
                visible: (field.modelData.hint || "") !== ""
                width: field.width
                text: field.modelData.hint || ""
                font.pixelSize: 11
                opacity: 0.6
                wrapMode: Text.Wrap
            }
        }
    }
}
