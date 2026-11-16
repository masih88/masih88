import QtQuick
import QtQuick.Layouts

// Rounded surface with a soft shadow and a vertical layout for its children.
Rectangle {
    id: card
    default property alias content: column.data
    property alias spacing: column.spacing
    property int padding: 16

    Layout.fillWidth: true
    implicitHeight: column.implicitHeight + 2 * padding
    radius: 14
    color: Calc.theme.surface
    border.color: Calc.theme.border

    // shadow: drawn behind the card (negative z renders below the parent)
    Rectangle {
        z: -1
        x: 0
        y: 2
        width: parent.width
        height: parent.height
        radius: parent.radius
        color: Calc.theme.shadow
        opacity: Calc.darkTheme ? 0.8 : 0.55
    }

    ColumnLayout {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: card.padding
        spacing: 10
    }
}
