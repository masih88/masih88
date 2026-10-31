import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "Status.js" as Status

// Edits one row of a project list; every change is written back to the list page immediately.
Page {
    id: page
    property var owner
    property string listId
    property int rowIndex: 0
    readonly property bool wide: width >= 760
    background: null

    property var def: loadDefinition(Calc.language, Calc.currency)
    property var values: owner && rowIndex < owner.rows.length ? owner.rows[rowIndex] : ({})
    readonly property string rowName: values[def.titleInput] || ""
    title: rowName !== "" ? rowName : Calc.ui.row + " " + (rowIndex + 1)

    property var result: run(values, owner ? owner.settings : ({}), Calc.norm, Calc.language, Calc.currency, Calc.chfPerEur)
    readonly property string status: Status.worst(result.messages)
    readonly property var keyOutputs: (result.outputs || []).filter(o => o.key).slice(0, 3)

    readonly property color sectionColor: Calc.darkTheme ? "#9FB3CC" : "#4A5B72"
    readonly property color accentText: Calc.darkTheme ? "#F2C14E" : "#1F3A5F"
    readonly property color lineColor: Calc.darkTheme ? "#2A3445" : "#E2E6EC"

    function loadDefinition(lang) {
        return Calc.listDef(listId)
    }
    function run(v, s, norm, lang) {
        return Object.keys(v).length > 0 ? Calc.computeRow(listId, v, s) : ({ outputs: [], messages: [] })
    }
    function setValue(id, text) {
        const v = Object.assign({}, values)
        v[id] = text
        owner.updateRow(rowIndex, v)
    }
    function goTo(i) {
        rowIndex = i
        form.rebuild()
        flick.contentY = 0
    }

    component SectionTitle: Label {
        font.pixelSize: 12
        font.letterSpacing: 1.2
        font.weight: Font.DemiBold
        color: page.sectionColor
    }

    component ActionButton: Button {
        id: ab
        property bool danger: false
        contentItem: Label {
            text: ab.text
            color: ab.danger ? Status.colors("error", Calc.darkTheme)[1] : page.accentText
            opacity: ab.enabled ? 1 : 0.4
            font.weight: Font.DemiBold
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            implicitWidth: 44
            implicitHeight: 40
            radius: 8
            color: ab.pressed ? page.lineColor : "transparent"
            border.color: page.lineColor
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

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                ActionButton {
                    text: "‹"
                    enabled: page.rowIndex > 0
                    onClicked: page.goTo(page.rowIndex - 1)
                }
                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: Calc.ui.row + " " + (page.rowIndex + 1) + " / " + (page.owner ? page.owner.rows.length : 0)
                    font.pixelSize: 13
                    opacity: 0.7
                }
                ActionButton {
                    text: "›"
                    enabled: page.owner && page.rowIndex < page.owner.rows.length - 1
                    onClicked: page.goTo(page.rowIndex + 1)
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: page.wide ? 2 : 1
                columnSpacing: 14
                rowSpacing: 14

                Card {
                    Layout.alignment: Qt.AlignTop
                    Layout.preferredWidth: 1
                    SectionTitle { text: Calc.ui.inputs.toUpperCase() }
                    InputForm {
                        id: form
                        inputs: page.def.rowInputs || []
                        values: page.values
                        onEdited: (id, text) => page.setValue(id, text)
                    }
                    Flow {
                        Layout.fillWidth: true
                        Layout.topMargin: 6
                        spacing: 8
                        ActionButton {
                            text: Calc.ui.duplicate
                            onClicked: {
                                page.owner.duplicateRow(page.rowIndex)
                                page.goTo(page.rowIndex + 1)
                            }
                        }
                        ActionButton {
                            text: Calc.ui.deleteRow
                            danger: true
                            onClicked: {
                                const o = page.owner
                                const i = page.rowIndex
                                page.StackView.view.pop()
                                o.removeRow(i)
                            }
                        }
                        ActionButton {
                            text: Calc.ui.done
                            onClicked: page.StackView.view.pop()
                        }
                    }
                }

                ColumnLayout {
                    id: resultColumn
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    spacing: 14
                    Card {
                        SectionTitle { text: Calc.ui.results.toUpperCase() }
                        ResultView {
                            outputs: page.result.outputs || []
                            messages: page.result.messages || []
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
}
