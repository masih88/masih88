import QtQuick

// Small drawn icon for the kind of an item – calculator, table or project list – built from rectangles so it looks
// the same on every platform (no font glyphs needed).
Item {
    id: icon
    property string kind: "calc"
    property color color: "#1F3A5F"
    implicitWidth: 20
    implicitHeight: 20

    // calculator: body, display and four keys
    Item {
        anchors.fill: parent
        visible: icon.kind !== "table" && icon.kind !== "list"
        Rectangle {
            x: 3; y: 1; width: 14; height: 18; radius: 3
            color: "transparent"
            border.color: icon.color
            border.width: 2
        }
        Rectangle { x: 6; y: 4; width: 8; height: 3; radius: 1; color: icon.color }
        Repeater {
            model: 4
            Rectangle {
                required property int index
                x: 6 + (index % 2) * 5
                y: 10 + Math.floor(index / 2) * 4
                width: 3; height: 2.5; radius: 1
                color: icon.color
            }
        }
    }

    // table: frame with header row and grid
    Item {
        anchors.fill: parent
        visible: icon.kind === "table"
        Rectangle {
            x: 1; y: 3; width: 18; height: 14; radius: 2
            color: "transparent"
            border.color: icon.color
            border.width: 2
        }
        Rectangle { x: 1; y: 3; width: 18; height: 5; radius: 2; color: icon.color }
        Rectangle { x: 9; y: 7; width: 2; height: 10; color: icon.color }
        Rectangle { x: 1; y: 11.5; width: 18; height: 1.6; color: icon.color }
    }

    // project list: three bullet lines
    Item {
        anchors.fill: parent
        visible: icon.kind === "list"
        Repeater {
            model: 3
            Item {
                required property int index
                y: 3 + index * 6
                width: 20
                height: 3
                Rectangle { x: 1; width: 3; height: 3; radius: 1.5; color: icon.color }
                Rectangle { x: 6; y: 0.25; width: 13; height: 2.5; radius: 1; color: icon.color }
            }
        }
    }
}
