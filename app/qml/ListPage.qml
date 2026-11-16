import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Items of one area. Areas with several groups (catalog/layout.cpp) get filter chips ("All" + one per group) and show
// every group as a titled section; other areas show one plain list. Column + Repeater instead of ColumnLayout (Qt 6.4).
Page {
    id: page
    property string categoryId
    property int filter: -1  // -1 = all groups
    background: null

    readonly property var category: {
        Calc.language
        const cats = Calc.categories
        for (let i = 0; i < cats.length; ++i)
            if (cats[i].id === categoryId)
                return cats[i]
        return ({})
    }
    readonly property color accent: category.color || Calc.theme.brand
    title: category.title || ""
    property var groups: {
        Calc.language
        return Calc.groups(categoryId)
    }
    readonly property bool grouped: groups.length > 1

    Flickable {
        id: flick
        anchors.fill: parent
        contentHeight: col.height + 32
        clip: true
        ScrollBar.vertical: ScrollBar {}

        Column {
            id: col
            x: 16
            y: 14
            width: flick.width - 32
            spacing: 12

            // area header: badge, name, number of entries
            Row {
                spacing: 12
                width: col.width
                Rectangle {
                    width: 44
                    height: 44
                    radius: 12
                    color: page.accent
                    Label {
                        anchors.centerIn: parent
                        text: page.category.badge || ""
                        color: "white"
                        font.bold: true
                        font.pixelSize: (page.category.badge || "").length > 2 ? 12 : 16
                    }
                }
                Column {
                    width: col.width - 56
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2
                    Label {
                        width: parent.width
                        text: page.category.title || ""
                        color: Calc.theme.text
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                        elide: Text.ElideRight
                    }
                    Label {
                        text: (page.category.count || 0) + " " + Calc.ui.entries
                              + (page.grouped ? " · " + page.groups.length + " " + Calc.ui.groupsLabel : "")
                        color: Calc.theme.muted
                        font.pixelSize: 12
                    }
                }
            }

            // filter chips
            Flow {
                visible: page.grouped
                width: col.width
                spacing: 8
                Repeater {
                    model: page.grouped ? [Calc.ui.all].concat(page.groups.map(g => g.title)) : []
                    delegate: Rectangle {
                        id: chip
                        required property var modelData
                        required property int index
                        readonly property bool selected: page.filter === index - 1
                        height: 32
                        width: chipLabel.implicitWidth + 26
                        radius: 16
                        color: selected ? page.accent : (chipTap.pressed ? Calc.theme.pressed : Calc.theme.surface)
                        border.color: selected ? page.accent : Calc.theme.border
                        Label {
                            id: chipLabel
                            anchors.centerIn: parent
                            text: chip.modelData
                            color: chip.selected ? "white" : Calc.theme.text
                            font.pixelSize: 13
                            font.weight: chip.selected ? Font.DemiBold : Font.Normal
                        }
                        TapHandler {
                            id: chipTap
                            onTapped: page.filter = chip.index - 1
                        }
                    }
                }
            }

            Repeater {
                model: page.groups
                delegate: Column {
                    id: grp
                    required property var modelData
                    required property int index
                    visible: page.filter < 0 || page.filter === index
                    width: col.width
                    spacing: 8

                    Item {
                        visible: page.grouped
                        width: grp.width
                        height: 26
                        Rectangle {
                            id: dot
                            width: 4
                            height: 16
                            radius: 2
                            color: page.accent
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Label {
                            anchors.left: dot.right
                            anchors.leftMargin: 8
                            anchors.right: countChip.left
                            anchors.rightMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                            text: grp.modelData.title.toUpperCase()
                            color: Calc.theme.muted
                            font.pixelSize: 12
                            font.letterSpacing: 1.0
                            font.weight: Font.DemiBold
                            elide: Text.ElideRight
                        }
                        Label {
                            id: countChip
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            text: grp.modelData.items.length
                            color: Calc.theme.muted
                            font.pixelSize: 12
                        }
                    }

                    Card {
                        width: grp.width
                        padding: 4
                        spacing: 0
                        Repeater {
                            model: grp.modelData.items
                            delegate: ItemRow {
                                required property var modelData
                                item: modelData
                            }
                        }
                    }
                }
            }
        }
    }
}
