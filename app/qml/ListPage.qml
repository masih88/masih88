import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
    property var entries: {
        Calc.language
        return Calc.items(categoryId)
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
            spacing: 12

            Card {
                padding: 4
                spacing: 0
                Repeater {
                    model: page.entries
                    delegate: ItemRow {
                        required property var modelData
                        item: modelData
                    }
                }
            }
        }
    }
}
