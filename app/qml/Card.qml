import QtQuick
import QtQuick.Layouts

// Rounded surface with a vertical layout for its children.
Rectangle {
    id: card
    default property alias content: column.data
    property alias spacing: column.spacing
    property int padding: 16

    Layout.fillWidth: true
    implicitHeight: column.implicitHeight + 2 * padding
    radius: 14
    color: Calc.darkTheme ? "#1C2330" : "#FFFFFF"
    border.color: Calc.darkTheme ? "#2A3445" : "#E2E6EC"

    ColumnLayout {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: card.padding
        spacing: 10
    }
}
