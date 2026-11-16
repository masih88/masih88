import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// One calculator, table or project list in a list: tinted kind icon in the area colour, title, summary, kind chip.
ItemDelegate {
    id: row
    property var item: ({})
    readonly property color tint: row.item.color || "#5E6C7F"

    Layout.fillWidth: true
    leftPadding: 10
    rightPadding: 8
    topPadding: 9
    bottomPadding: 9
    onClicked: row.ApplicationWindow.window.openItem(row.item.id)

    background: Rectangle {
        radius: 10
        color: row.down ? Calc.theme.pressed : (row.hovered ? Calc.theme.surfaceAlt : "transparent")
    }

    contentItem: RowLayout {
        spacing: 12

        Item {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Rectangle {
                anchors.fill: parent
                radius: 12
                color: row.tint
                opacity: Calc.darkTheme ? 0.24 : 0.12
            }
            KindIcon {
                anchors.centerIn: parent
                kind: row.item.kind || "calc"
                color: Calc.darkTheme ? Qt.lighter(row.tint, 1.7) : row.tint
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Label {
                Layout.fillWidth: true
                text: row.item.title || ""
                color: Calc.theme.text
                font.pixelSize: 15
                font.weight: Font.Medium
                elide: Text.ElideRight
            }
            Label {
                Layout.fillWidth: true
                text: row.item.summary || ""
                color: Calc.theme.muted
                font.pixelSize: 12
                elide: Text.ElideRight
            }
        }
        Label {
            visible: Calc.favorites.indexOf(row.item.id) >= 0
            text: "★"
            color: Calc.theme.accent
            font.pixelSize: 15
        }
        Rectangle {
            visible: row.item.kind === "table" || row.item.kind === "list"
            Layout.preferredHeight: 20
            Layout.preferredWidth: kindLabel.implicitWidth + 14
            radius: 10
            color: "transparent"
            border.color: Calc.theme.border
            Label {
                id: kindLabel
                anchors.centerIn: parent
                text: row.item.kind === "table" ? Calc.ui.table : Calc.ui.list
                color: Calc.theme.muted
                font.pixelSize: 10
                font.weight: Font.DemiBold
            }
        }
        Label {
            text: "›"
            color: Calc.theme.muted
            font.pixelSize: 22
        }
    }
}
