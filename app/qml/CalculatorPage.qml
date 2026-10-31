import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "Status.js" as Status

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
    readonly property string status: Status.worst(result.messages)
    readonly property var keyOutputs: {
        const outs = result.outputs || []
        const keys = outs.filter(o => o.key)
        return (keys.length > 0 ? keys : outs).slice(0, 2)
    }

    readonly property color sectionColor: Calc.darkTheme ? "#9FB3CC" : "#4A5B72"

    readonly property bool hasChart: def.hasChart === true
    property var chartData: hasChart ? loadChart(values, Calc.norm, Calc.language) : ({})

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
    function loadChart(v, norm, lang) {
        return Object.keys(v).length > 0 ? Calc.chart(calcId, v) : ({})
    }
    // tap into the chart: the core says which inputs change (e.g. t₁ and φ₁ in the h,x chart)
    function pick(x, y) {
        const v = Calc.chartPick(calcId, values, x, y)
        if (Object.keys(v).length > 0) {
            values = v
            inputForm.rebuild()
        }
    }
    function reset() {
        values = Calc.defaults(calcId)
        inputForm.rebuild()
    }

    Component.onCompleted: values = Calc.defaults(calcId)

    component SectionTitle: Label {
        font.pixelSize: 12
        font.letterSpacing: 1.2
        font.weight: Font.DemiBold
        color: page.sectionColor
    }

    component ChartCard: Card {
        SectionTitle { text: Calc.ui.chart.toUpperCase() }
        ChartView {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(width * 0.95, 640)
            chart: page.chartData
            pickable: page.def.hasPick === true
            onPicked: (x, y) => page.pick(x, y)
        }
        Label {
            Layout.fillWidth: true
            text: page.chartData.note || ""
            visible: text !== ""
            wrapMode: Text.Wrap
            font.pixelSize: 11
            opacity: 0.65
        }
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

            // phones: the diagram first, it is what the page is about
            ChartCard {
                visible: page.hasChart && !page.wide
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
                    spacing: 10
                    SectionTitle { text: Calc.ui.inputs.toUpperCase() }

                    InputForm {
                        id: inputForm
                        inputs: page.def.inputs || []
                        values: page.values
                        onEdited: (id, text) => page.setValue(id, text)
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

                    ChartCard {
                        visible: page.hasChart && page.wide
                    }

                    Card {
                        id: resultsCard
                        SectionTitle { text: Calc.ui.results.toUpperCase() }
                        ResultView {
                            outputs: page.result.outputs || []
                            messages: page.result.messages || []
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

    footer: ResultBar {
        visible: !page.wide && page.keyOutputs.length > 0
        outputs: page.keyOutputs
        status: page.status
        onTapped: flick.contentY = Math.max(0, Math.min(resultColumn.mapToItem(col, 0, 0).y + col.y - 8,
                                                        flick.contentHeight - flick.height))
    }

    ToolTip {
        id: toast
        text: Calc.ui.copied
        timeout: 1800
        x: (page.width - width) / 2
        y: page.height - height - 24
    }
}
