import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// One calculator or table in a list: coloured badge, title, summary.
ItemDelegate {
    id: row
    property var item: ({})

    Layout.fillWidth: true
    padding: 10
    onClicked: row.ApplicationWindow.window.openItem(row.item.id)

    contentItem: RowLayout {
        spacing: 12

        Rectangle {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            radius: 20
            color: row.item.color || "#888888"
            Label {
                anchors.centerIn: parent
                text: row.item.badge || ""
                color: "white"
                font.bold: true
                font.pixelSize: (row.item.badge || "").length > 2 ? 11 : 14
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Label {
                Layout.fillWidth: true
                text: row.item.title || ""
                font.pixelSize: 15
                font.weight: Font.Medium
                elide: Text.ElideRight
            }
            Label {
                Layout.fillWidth: true
                text: row.item.summary || ""
                font.pixelSize: 12
                opacity: 0.65
                elide: Text.ElideRight
            }
        }
        Label {
            visible: Calc.favorites.indexOf(row.item.id) >= 0
            text: "★"
            color: "#F2A900"
            font.pixelSize: 16
        }
        Label {
            readonly property bool tag: row.item.kind === "table" || row.item.kind === "list"
            text: row.item.kind === "table" ? Calc.ui.table : (row.item.kind === "list" ? Calc.ui.list : "›")
            font.pixelSize: tag ? 11 : 22
            opacity: 0.45
        }
    }
}
