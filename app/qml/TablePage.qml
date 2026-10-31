import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Reference table: only the table itself scrolls sideways, the texts stay readable.
Page {
    id: page
    property string tableId
    readonly property string favId: tableId
    background: null

    property var def: loadDefinition(Calc.language)
    title: def.title || ""

    function loadDefinition(lang) {
        return Calc.table(tableId)
    }

    // Header cells first, then all rows, flattened for the GridLayout
    property var cells: {
        const out = []
        const cols = def.columns || []
        const rows = def.rows || []
        for (let c = 0; c < cols.length; ++c)
            out.push({ text: cols[c], header: true, row: -1, first: c === 0 })
        for (let r = 0; r < rows.length; ++r)
            for (let c = 0; c < rows[r].length; ++c)
                out.push({ text: rows[r][c], header: false, row: r, first: c === 0 })
        return out
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: col.implicitHeight + 32
        clip: true
        flickableDirection: Flickable.VerticalFlick
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: col
            x: 16
            y: 16
            width: flick.width - 32
            spacing: 14

            Label {
                Layout.fillWidth: true
                text: page.def.summary || ""
                opacity: 0.7
                wrapMode: Text.Wrap
                font.pixelSize: 13
            }

            Rectangle {
                id: frame
                Layout.fillWidth: true
                implicitHeight: grid.implicitHeight + 2 + (hflick.contentWidth > hflick.width + 1 ? 14 : 0)
                radius: 12
                clip: true
                color: Calc.darkTheme ? "#1C2330" : "#FFFFFF"
                border.color: Calc.darkTheme ? "#2A3445" : "#E2E6EC"

                Flickable {
                    id: hflick
                    anchors.fill: parent
                    anchors.margins: 1
                    contentWidth: Math.max(width, Math.ceil(grid.implicitWidth))
                    contentHeight: height
                    flickableDirection: Flickable.HorizontalFlick
                    boundsBehavior: Flickable.StopAtBounds
                    clip: true
                    ScrollBar.horizontal: ScrollBar { policy: hflick.contentWidth > hflick.width + 1 ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff }

                    GridLayout {
                        id: grid
                        width: hflick.contentWidth
                        columns: Math.max(1, (page.def.columns || []).length)
                        rowSpacing: 0
                        columnSpacing: 0

                        Repeater {
                            model: page.cells
                            delegate: Rectangle {
                                required property var modelData
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                implicitWidth: Math.min(cellText.implicitWidth, 220) + 20
                                implicitHeight: cellText.implicitHeight + 14
                                color: modelData.header ? (page.def.color || "#1F3A5F")
                                                        : (modelData.row % 2 === 1 ? (Calc.darkTheme ? "#222B3A" : "#F5F7FA") : "transparent")
                                Label {
                                    id: cellText
                                    x: 10
                                    y: 7
                                    width: parent.width - 20
                                    text: modelData.text
                                    wrapMode: Text.Wrap
                                    font.pixelSize: 13
                                    font.weight: modelData.header || modelData.first ? Font.DemiBold : Font.Normal
                                    color: modelData.header ? "white" : (Calc.darkTheme ? "#E6EAF0" : "#1D2633")
                                }
                            }
                        }
                    }
                }
            }

            Card {
                Label {
                    text: Calc.ui.source.toUpperCase()
                    font.pixelSize: 12
                    font.letterSpacing: 1.2
                    font.weight: Font.DemiBold
                    opacity: 0.6
                }
                Label {
                    Layout.fillWidth: true
                    text: page.def.description || ""
                    wrapMode: Text.Wrap
                    font.pixelSize: 13
                }
            }
        }
    }
}
