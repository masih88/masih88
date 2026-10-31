import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "Status.js" as Status

// Project list (consumer list, data point list): totals, project settings and one card per row.
// Rows are stored automatically; export/import works via the clipboard (Excel) or CSV files.
Page {
    id: page
    property string listId
    readonly property string favId: listId
    readonly property bool wide: width >= 760
    background: null

    property var def: loadDefinition(Calc.language, Calc.currency)
    title: def.title || ""

    property var rows: []
    property var settings: ({})
    property bool showSettings: Calc.startPage.indexOf("!settings") > 0
    property var result: compute(rows, settings, Calc.norm, Calc.language, Calc.currency, Calc.chfPerEur)

    readonly property var keyTotals: (result.totals || []).filter(o => o.key)
    readonly property var otherTotals: (result.totals || []).filter(o => !o.key)

    readonly property color cardColor: Calc.darkTheme ? "#1C2330" : "#FFFFFF"
    readonly property color lineColor: Calc.darkTheme ? "#2A3445" : "#E2E6EC"
    readonly property color sectionColor: Calc.darkTheme ? "#9FB3CC" : "#4A5B72"
    readonly property color accentText: Calc.darkTheme ? "#F2C14E" : "#1F3A5F"
    readonly property color brand: "#1F6F5C"

    function loadDefinition(lang) {
        return Calc.listDef(listId)
    }
    function compute(r, s, norm, lang) {
        return Object.keys(s).length > 0 ? Calc.computeList(listId, r, s) : ({ rows: [], totals: [] })
    }
    function persist() {
        Calc.saveList(listId, rows, settings)
    }
    function setRows(r) {
        rows = r
        persist()
    }
    function updateRow(i, values) {
        const r = rows.slice()
        r[i] = values
        setRows(r)
    }
    function addRow() {
        const r = rows.slice()
        r.push(Calc.defaultListRow(listId))
        setRows(r)
        openRow(r.length - 1)
    }
    function duplicateRow(i) {
        const r = rows.slice()
        r.splice(i + 1, 0, Object.assign({}, rows[i]))
        setRows(r)
    }
    function removeRow(i) {
        const r = rows.slice()
        r.splice(i, 1)
        setRows(r)
    }
    function setSetting(id, text) {
        const s = Object.assign({}, settings)
        s[id] = text
        settings = s
        persist()
    }
    function openRow(i) {
        page.StackView.view.push(Qt.resolvedUrl("RowEditorPage.qml"), { owner: page, listId: listId, rowIndex: i })
    }
    function notify(text) {
        toast.text = text
        toast.open()
    }
    function copyTable(detail) {
        Calc.copyToClipboard(Calc.exportList(listId, rows, settings, detail, true))
        notify(Calc.ui.copiedTable)
    }
    function saveFile(detail) {
        const path = Calc.saveCsv(listId + (detail ? "_detail" : ""), Calc.exportList(listId, rows, settings, detail, false))
        notify(path !== "" ? Calc.ui.savedTo.arg(path) : Calc.ui.saveFailed)
    }
    function paste(replace) {
        const imported = Calc.importList(listId, Calc.clipboardText())
        if (imported.length === 0) {
            notify(Calc.ui.importNone)
            return
        }
        setRows(replace ? imported : rows.concat(imported))
        notify(Calc.ui.imported.arg(imported.length))
    }

    Component.onCompleted: {
        rows = Calc.listRows(listId)
        settings = Calc.listSettings(listId)
    }

    component SectionTitle: Label {
        font.pixelSize: 12
        font.letterSpacing: 1.2
        font.weight: Font.DemiBold
        color: page.sectionColor
    }

    // explicit look: independent of shader effects (software renderer, old GPUs)
    component ActionButton: Button {
        id: ab
        property bool primary: false
        contentItem: Label {
            text: ab.text
            color: ab.primary ? "white" : page.accentText
            font.weight: Font.DemiBold
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            implicitWidth: 44
            implicitHeight: 40
            radius: 8
            color: ab.primary ? (ab.pressed ? "#14503F" : page.brand) : (ab.pressed ? page.lineColor : "transparent")
            border.color: ab.primary ? "transparent" : page.lineColor
        }
    }

    component PlainMenu: Menu {
        background: Rectangle {
            implicitWidth: 280
            radius: 10
            color: page.cardColor
            border.color: page.lineColor
        }
    }

    component RowCard: Rectangle {
        id: rc
        required property var modelData
        required property int index
        readonly property var colors: Status.colors(modelData.status, Calc.darkTheme)
        // key results as one wrapping line of styled text – no nested layouts, no polish loops
        readonly property string keyText: {
            const parts = []
            const outs = modelData.keyOutputs || []
            for (let i = 0; i < outs.length; ++i)
                parts.push(outs[i].short + "&nbsp;<b><font color=\"" + page.accentText + "\">"
                           + outs[i].value + (outs[i].unit ? "&nbsp;" + outs[i].unit : "") + "</font></b>")
            return parts.join("&nbsp;&nbsp;·&nbsp;&nbsp; ")
        }

        Layout.fillWidth: true
        implicitHeight: rcCol.implicitHeight + 22
        radius: 12
        color: tap.pressed ? (Calc.darkTheme ? "#232C3B" : "#F0F3F7") : page.cardColor
        border.color: page.lineColor

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 8
            width: 4
            radius: 2
            color: rc.modelData.status === "none" ? page.lineColor : rc.colors[1]
        }
        ColumnLayout {
            id: rcCol
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: 20
            anchors.rightMargin: 12
            anchors.topMargin: 11
            spacing: 6

            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                Label {
                    Layout.preferredWidth: 20
                    text: rc.index + 1
                    font.pixelSize: 12
                    opacity: 0.5
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0
                    Label {
                        Layout.fillWidth: true
                        text: rc.modelData.title !== "" ? rc.modelData.title : Calc.ui.row + " " + (rc.index + 1)
                        font.pixelSize: 15
                        font.weight: Font.Medium
                        elide: Text.ElideRight
                    }
                    Label {
                        Layout.fillWidth: true
                        visible: rc.modelData.subtitle !== ""
                        text: rc.modelData.subtitle
                        font.pixelSize: 12
                        opacity: 0.6
                        elide: Text.ElideRight
                    }
                }
                Rectangle {
                    visible: rc.modelData.status !== "none"
                    Layout.preferredWidth: 24
                    Layout.preferredHeight: 24
                    radius: 12
                    color: rc.colors[0]
                    Label {
                        anchors.centerIn: parent
                        text: rc.colors[2]
                        color: rc.colors[1]
                        font.bold: true
                        font.pixelSize: 13
                    }
                }
                Label {
                    text: "›"
                    font.pixelSize: 22
                    opacity: 0.45
                }
            }
            Label {
                visible: rc.keyText !== ""
                // explicit width (not from the wrapped text) avoids the implicit-size loop
                Layout.preferredWidth: rcCol.width - 30
                Layout.leftMargin: 30
                text: rc.keyText
                textFormat: Text.StyledText
                wrapMode: Text.Wrap
                font.pixelSize: 13
                lineHeight: 1.15
            }
        }
        TapHandler {
            id: tap
            onTapped: page.openRow(rc.index)
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

            GridLayout {
                Layout.fillWidth: true
                columns: page.wide ? 2 : 1
                columnSpacing: 14
                rowSpacing: 14

                // ---------------- Totals, actions, settings ----------------
                ColumnLayout {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    spacing: 14

                    Card {
                        SectionTitle { text: Calc.ui.totals.toUpperCase() }
                        GridLayout {
                            id: tiles
                            Layout.fillWidth: true
                            columns: 2
                            columnSpacing: 10
                            rowSpacing: 10
                            Repeater {
                                model: page.keyTotals
                                delegate: Rectangle {
                                    required property var modelData
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: 1
                                    implicitHeight: tileCol.implicitHeight + 20
                                    radius: 10
                                    color: Calc.darkTheme ? "#16302A" : "#E6F2EE"
                                    Column {
                                        id: tileCol
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.margins: 12
                                        spacing: 2
                                        Label {
                                            width: parent.width
                                            text: modelData.label
                                            font.pixelSize: 12
                                            opacity: 0.7
                                            elide: Text.ElideRight
                                        }
                                        Label {
                                            width: parent.width
                                            text: modelData.value + (modelData.unit ? " " + modelData.unit : "")
                                            font.pixelSize: 22
                                            font.weight: Font.Bold
                                            color: Calc.darkTheme ? "#8FE0C4" : page.brand
                                            elide: Text.ElideRight
                                        }
                                    }
                                }
                            }
                        }
                        ResultView {
                            outputs: page.otherTotals
                            messages: page.result.totalMessages || []
                        }
                    }

                    Card {
                        SectionTitle { text: Calc.ui.actions.toUpperCase() }
                        GridLayout {
                            // fixed 2 × 2 grid: a Flow here makes the page layout oscillate (polish loop)
                            Layout.fillWidth: true
                            columns: 2
                            columnSpacing: 8
                            rowSpacing: 8
                            ActionButton {
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                primary: true
                                text: "+ " + Calc.ui.addRow
                                onClicked: page.addRow()
                            }
                            ActionButton {
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                text: Calc.ui.copyExcel
                                onClicked: page.copyTable(false)
                            }
                            ActionButton {
                                id: pasteButton
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                text: Calc.ui.pasteExcel
                                onClicked: pasteMenu.popup(pasteButton, 0, pasteButton.height)
                                PlainMenu {
                                    id: pasteMenu
                                    MenuItem { text: Calc.ui.pasteReplace; onTriggered: page.paste(true) }
                                    MenuItem { text: Calc.ui.pasteAppend; onTriggered: page.paste(false) }
                                }
                            }
                            ActionButton {
                                id: moreButton
                                Layout.fillWidth: true
                                Layout.preferredWidth: 1
                                text: "⋯  " + Calc.ui.more
                                onClicked: moreMenu.popup(moreButton, 0, moreButton.height)
                                PlainMenu {
                                    id: moreMenu
                                    MenuItem {
                                        visible: page.def.hasDetail === true
                                        height: visible ? implicitHeight : 0
                                        text: Calc.ui.copyDetail.arg(page.def.detailTitle || "")
                                        onTriggered: page.copyTable(true)
                                    }
                                    MenuItem { text: Calc.ui.saveCsv; onTriggered: page.saveFile(false) }
                                    MenuItem {
                                        visible: page.def.hasDetail === true
                                        height: visible ? implicitHeight : 0
                                        text: Calc.ui.saveDetailCsv.arg(page.def.detailTitle || "")
                                        onTriggered: page.saveFile(true)
                                    }
                                    MenuSeparator {}
                                    MenuItem { text: Calc.ui.loadExample; onTriggered: page.setRows(Calc.exampleRows(page.listId)) }
                                    MenuItem { text: Calc.ui.clearList; onTriggered: clearDialog.open() }
                                }
                            }
                        }
                        Label {
                            Layout.fillWidth: true
                            text: Calc.ui.listHelp
                            font.pixelSize: 12
                            opacity: 0.65
                            wrapMode: Text.Wrap
                        }
                    }

                    Card {
                        RowLayout {
                            Layout.fillWidth: true
                            SectionTitle {
                                Layout.fillWidth: true
                                text: Calc.ui.projectSettings.toUpperCase()
                            }
                            Label {
                                text: page.showSettings ? "▾" : "▸"
                                font.pixelSize: 16
                                color: page.sectionColor
                            }
                            TapHandler { onTapped: page.showSettings = !page.showSettings }
                        }
                        InputForm {
                            visible: page.showSettings
                            inputs: page.def.settingsInputs || []
                            values: page.settings
                            onEdited: (id, text) => page.setSetting(id, text)
                        }
                    }
                }

                // ---------------- Rows + notes ----------------
                ColumnLayout {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    spacing: 10

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.topMargin: page.wide ? 0 : 4
                        SectionTitle {
                            Layout.fillWidth: true
                            text: Calc.ui.rows.toUpperCase() + " (" + page.rows.length + ")"
                        }
                        Rectangle {
                            visible: (page.result.nok || 0) > 0
                            Layout.preferredWidth: nokLabel.implicitWidth + 16
                            Layout.preferredHeight: 22
                            radius: 11
                            color: Status.colors("error", Calc.darkTheme)[0]
                            Label {
                                id: nokLabel
                                anchors.centerIn: parent
                                text: (page.result.nok || 0) + " " + Calc.ui.notOk
                                color: Status.colors("error", Calc.darkTheme)[1]
                                font.pixelSize: 11
                                font.weight: Font.DemiBold
                            }
                        }
                    }
                    // Column positioner, not a layout: Qt 6.4 layouts lose track of Repeater children
                    // that are added after the page was created (they stay at 0,0 with zero width)
                    Column {
                        id: rowsBox
                        Layout.fillWidth: true
                        spacing: 10
                        Label {
                            visible: page.rows.length === 0
                            width: rowsBox.width
                            text: Calc.ui.emptyList
                            wrapMode: Text.Wrap
                            opacity: 0.7
                        }
                        Repeater {
                            model: page.result.rows || []
                            delegate: RowCard {
                                width: rowsBox.width
                            }
                        }
                    }

                    Card {
                        Layout.topMargin: 4
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

    Dialog {
        id: clearDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(360, page.width - 32)
        modal: true
        title: Calc.ui.confirmClear
        standardButtons: Dialog.Ok | Dialog.Cancel
        background: Rectangle {
            radius: 12
            color: page.cardColor
            border.color: page.lineColor
        }
        onAccepted: page.setRows([])
    }

    ToolTip {
        id: toast
        timeout: 2600
        x: (page.width - width) / 2
        y: page.height - height - 24
    }
}
