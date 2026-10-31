import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

// Generic page: renders any calculator from its definition and recomputes on every change.
// Phone: one column + result bar at the bottom. Wide window: inputs left, results right.
Page {
    id: page
    property string calcId
    readonly property string favId: calcId
    readonly property bool wide: width >= 760
    background: null

    property var def: loadDefinition(Calc.language)
    title: def.title || ""

    property var values: ({})
    property var result: run(values, Calc.norm, Calc.language)

    // worst message severity drives the status colour
    readonly property string status: {
        const order = ["info", "ok", "warning", "error"]
        let worst = -1
        const msgs = result.messages || []
        for (let i = 0; i < msgs.length; ++i)
            worst = Math.max(worst, order.indexOf(msgs[i].severity))
        return worst < 0 ? "none" : order[worst]
    }
    readonly property var keyOutputs: {
        const outs = result.outputs || []
        const keys = outs.filter(o => o.key)
        return (keys.length > 0 ? keys : outs).slice(0, 2)
    }

    readonly property color cardColor: Calc.darkTheme ? "#1C2330" : "#FFFFFF"
    readonly property color lineColor: Calc.darkTheme ? "#2A3445" : "#E2E6EC"
    readonly property color sectionColor: Calc.darkTheme ? "#9FB3CC" : "#4A5B72"
    readonly property color accentText: Calc.darkTheme ? "#F2C14E" : "#1F3A5F"

    function statusColors(sev) {
        const dark = Calc.darkTheme
        switch (sev) {
        case "ok":      return dark ? ["#1D3B2A", "#8FE0AE", "✓"] : ["#E3F4EA", "#1E6B3C", "✓"]
        case "warning": return dark ? ["#3D3215", "#F5D27A", "!"] : ["#FFF4D6", "#7A5A00", "!"]
        case "error":   return dark ? ["#45201C", "#F4A69C", "✕"] : ["#FDE4E1", "#9B2C1F", "✕"]
        default:        return dark ? ["#1B2D45", "#9CC3F5", "i"] : ["#E4EEFB", "#1F4E89", "i"]
        }
    }

    function loadDefinition(lang) {
        return Calc.calculator(calcId)
    }
    function run(v, norm, lang) {
        return Object.keys(v).length > 0 ? Calc.compute(calcId, v) : ({ outputs: [], messages: [] })
    }
    function setValue(id, text) {
        const v = Object.assign({}, values)
        v[id] = text
        values = v
    }
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
    function reset() {
        values = Calc.defaults(calcId)
        inputRepeater.model = []
        inputRepeater.model = Qt.binding(function () { return page.def.inputs || [] })
    }

    Component.onCompleted: values = Calc.defaults(calcId)

    component SectionTitle: Label {
        font.pixelSize: 12
        font.letterSpacing: 1.2
        font.weight: Font.DemiBold
        color: page.sectionColor
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: col.implicitHeight + 32
        clip: true
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: col
            x: 16
            y: 16
            width: flick.width - 32
            spacing: 14

            // Context line
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Rectangle {
                    Layout.preferredWidth: catLabel.implicitWidth + 16
                    Layout.preferredHeight: 22
                    radius: 11
                    color: page.def.color || "#888888"
                    Label {
                        id: catLabel
                        anchors.centerIn: parent
                        text: page.def.category || ""
                        color: "white"
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                    }
                }
                Label {
                    Layout.fillWidth: true
                    text: page.def.summary || ""
                    font.pixelSize: 12
                    opacity: 0.7
                    elide: Text.ElideRight
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: page.wide ? 2 : 1
                columnSpacing: 14
                rowSpacing: 14

                // ---------------- Inputs ----------------
                Card {
                    Layout.alignment: Qt.AlignTop
                    Layout.preferredWidth: 1
                    spacing: 6
                    SectionTitle { text: Calc.ui.inputs.toUpperCase() }

                    Repeater {
                        id: inputRepeater
                        model: page.def.inputs || []

                        delegate: GridLayout {
                            id: field
                            required property var modelData
                            readonly property bool isChoice: modelData.kind === "choice"
                            Layout.fillWidth: true
                            visible: page.inputVisible(modelData, page.values)
                            columns: isChoice ? 1 : 3
                            columnSpacing: 8
                            rowSpacing: 0

                            Label {
                                // explicit width from the parent avoids the wrap/implicit-size layout loop
                                Layout.fillWidth: true
                                Layout.preferredWidth: field.isChoice ? field.width
                                                                      : Math.max(60, field.width - 116 - 58 - 2 * field.columnSpacing)
                                Layout.alignment: Qt.AlignVCenter
                                text: field.modelData.label + (field.modelData.optional ? " · " + Calc.ui.optional : "")
                                font.pixelSize: 14
                                wrapMode: Text.Wrap
                                opacity: 0.85
                            }
                            TextField {
                                visible: !field.isChoice
                                Layout.preferredWidth: 116
                                Layout.alignment: Qt.AlignVCenter
                                text: page.values[field.modelData.id] || ""
                                placeholderText: field.modelData.optional ? "–" : ""
                                horizontalAlignment: TextInput.AlignRight
                                inputMethodHints: Qt.ImhFormattedNumbersOnly
                                selectByMouse: true
                                font.pixelSize: 16
                                onTextEdited: page.setValue(field.modelData.id, text)
                            }
                            Label {
                                visible: !field.isChoice
                                Layout.preferredWidth: 58
                                Layout.alignment: Qt.AlignVCenter
                                text: field.modelData.unit
                                font.pixelSize: 13
                                opacity: 0.7
                                elide: Text.ElideRight
                            }
                            ComboBox {
                                id: combo
                                visible: field.isChoice
                                Layout.fillWidth: true
                                model: field.modelData.options || []
                                textRole: "label"
                                valueRole: "value"
                                currentIndex: page.optionIndex(field.modelData.options || [], page.values[field.modelData.id])
                                onActivated: page.setValue(field.modelData.id, currentValue)
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
                                Layout.columnSpan: field.columns
                                Layout.fillWidth: true
                                Layout.preferredWidth: field.width
                                text: field.modelData.hint || ""
                                font.pixelSize: 11
                                opacity: 0.6
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.topMargin: 6
                        Button {
                            text: Calc.ui.reset
                            flat: true
                            onClicked: page.reset()
                        }
                        Item { Layout.fillWidth: true }
                        Button {
                            id: copyButton
                            text: Calc.ui.copy
                            // explicit look: independent of shader effects (software renderer, old GPUs)
                            contentItem: Label {
                                text: copyButton.text.toUpperCase()
                                color: "white"
                                font.weight: Font.DemiBold
                                font.pixelSize: 14
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                implicitWidth: 112
                                implicitHeight: 40
                                radius: 8
                                color: copyButton.pressed ? "#152A45" : "#1F3A5F"
                            }
                            onClicked: {
                                Calc.copyToClipboard(page.result.copyText || "")
                                toast.open()
                            }
                        }
                    }
                }

                // ---------------- Results + formulas ----------------
                ColumnLayout {
                    id: resultColumn
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    spacing: 14

                    Card {
                        id: resultsCard
                        SectionTitle { text: Calc.ui.results.toUpperCase() }

                        Repeater {
                            model: page.result.outputs || []
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
                                        color: modelData.key ? page.accentText : (Calc.darkTheme ? "#E6EAF0" : "#1D2633")
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
                            model: page.result.messages || []
                            delegate: Rectangle {
                                id: msg
                                required property var modelData
                                readonly property var colors: page.statusColors(modelData.severity)
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

                    Card {
                        SectionTitle { text: Calc.ui.formulas.toUpperCase() }
                        Label {
                            Layout.fillWidth: true
                            text: page.def.description || ""
                            wrapMode: Text.Wrap
                            font.pixelSize: 13
                            lineHeight: 1.25
                            opacity: 0.85
                        }
                    }
                }
            }
        }
    }

    // Result bar (phones): key results always visible, tap to jump to the full results
    footer: Rectangle {
        visible: !page.wide && page.keyOutputs.length > 0
        implicitHeight: visible ? barRow.implicitHeight + 20 : 0
        color: page.cardColor

        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1
            color: page.lineColor
        }
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 5
            color: page.status === "none" ? "#1F3A5F" : page.statusColors(page.status)[1]
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
                model: page.keyOutputs
                delegate: ColumnLayout {
                    required property var modelData
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    spacing: 0
                    Label {
                        Layout.fillWidth: true
                        text: modelData.label
                        font.pixelSize: 11
                        opacity: 0.65
                        elide: Text.ElideRight
                    }
                    Label {
                        Layout.fillWidth: true
                        text: modelData.value + (modelData.unit ? " " + modelData.unit : "")
                        font.pixelSize: 20
                        font.weight: Font.Bold
                        color: page.accentText
                        elide: Text.ElideRight
                    }
                }
            }
            Rectangle {
                visible: page.status !== "none"
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                radius: 15
                color: page.statusColors(page.status)[0]
                Label {
                    anchors.centerIn: parent
                    text: page.statusColors(page.status)[2]
                    color: page.statusColors(page.status)[1]
                    font.bold: true
                    font.pixelSize: 15
                }
            }
        }
        TapHandler {
            onTapped: flick.contentY = Math.max(0, Math.min(resultColumn.mapToItem(col, 0, 0).y + col.y - 8,
                                                            flick.contentHeight - flick.height))
        }
    }

    ToolTip {
        id: toast
        text: Calc.ui.copied
        timeout: 1800
        x: (page.width - width) / 2
        y: page.height - height - 24
    }
}
