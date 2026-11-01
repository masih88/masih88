import QtQuick
import QtQuick.Controls

// One switch per category: show or hide it on the home page (stored by the bridge, search still finds everything).
// Column + anchors instead of layouts (Qt 6.4 layout bug with Repeater, see ARCHITEKTUR.md).
Column {
    id: list
    spacing: 0

    Repeater {
        model: Calc.categories
        delegate: Item {
            id: rowItem
            required property var modelData
            readonly property bool shown: Calc.hiddenCategories.indexOf(modelData.id) < 0
            width: list.width
            height: 50

            Rectangle {
                id: badge
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: 32
                height: 32
                radius: 9
                color: rowItem.modelData.color
                opacity: rowItem.shown ? 1 : 0.45
                Label {
                    anchors.centerIn: parent
                    text: rowItem.modelData.badge
                    color: "white"
                    font.bold: true
                    font.pixelSize: rowItem.modelData.badge.length > 2 ? 10 : 13
                }
            }
            Label {
                anchors.left: badge.right
                anchors.leftMargin: 12
                anchors.right: toggle.left
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: rowItem.modelData.title
                font.pixelSize: 14
                elide: Text.ElideRight
                opacity: rowItem.shown ? 1 : 0.5
            }
            Switch {
                id: toggle
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                checked: rowItem.shown
                onToggled: {
                    Calc.setCategoryVisible(rowItem.modelData.id, checked)
                    // keep following the stored state (e.g. after "show all")
                    checked = Qt.binding(function () { return rowItem.shown })
                }
            }
        }
    }
}
