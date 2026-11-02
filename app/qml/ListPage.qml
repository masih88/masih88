import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Items of one category. Grouped categories (area "Elektrotechnik") show collapsible groups like the tabs of
// a reference app; other categories show one plain list. Column + Repeater instead of ColumnLayout (Qt 6.4).
Page {
    id: page
    property string categoryId
    background: null

    title: {
        Calc.language
        const cats = Calc.categories
        for (let i = 0; i < cats.length; ++i)
            if (cats[i].id === categoryId)
                return cats[i].title
        return ""
    }
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
            y: 16
            width: flick.width - 32
            spacing: 10

            Repeater {
                model: page.groups
                delegate: Column {
                    id: grp
                    required property var modelData
                    required property int index
                    property bool open: !page.grouped
                    width: col.width
                    spacing: 6

                    Rectangle {
                        id: header
                        visible: page.grouped
                        width: grp.width
                        height: 48
                        radius: 12
                        color: headerTap.pressed ? (Calc.darkTheme ? "#26303F" : "#E9EDF3") : (Calc.darkTheme ? "#1C2330" : "#FFFFFF")
                        border.color: Calc.darkTheme ? "#2A3445" : "#E2E6EC"

                        Label {
                            anchors.left: parent.left
                            anchors.leftMargin: 14
                            anchors.right: countLabel.left
                            anchors.rightMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                            text: grp.modelData.title
                            font.pixelSize: 15
                            font.weight: Font.DemiBold
                            elide: Text.ElideRight
                        }
                        Label {
                            id: countLabel
                            anchors.right: arrow.left
                            anchors.rightMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                            text: grp.modelData.items.length
                            font.pixelSize: 12
                            opacity: 0.6
                        }
                        Label {
                            id: arrow
                            anchors.right: parent.right
                            anchors.rightMargin: 14
                            anchors.verticalCenter: parent.verticalCenter
                            text: grp.open ? "▾" : "▸"
                            font.pixelSize: 18
                            opacity: 0.7
                        }
                        TapHandler {
                            id: headerTap
                            onTapped: grp.open = !grp.open
                        }
                    }

                    Card {
                        visible: grp.open
                        width: grp.width
                        padding: 4
                        spacing: 0
                        Repeater {
                            model: grp.open ? grp.modelData.items : []
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
