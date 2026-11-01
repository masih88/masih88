import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Page {
    id: page
    title: Calc.ui.appTitle
    background: null

    readonly property bool searching: search.text.trim().length > 0
    property var favItems: {
        Calc.language
        Calc.favorites
        return Calc.favoriteItems()
    }
    property var visibleCategories: {
        const hidden = Calc.hiddenCategories
        return Calc.categories.filter(c => hidden.indexOf(c.id) < 0)
    }
    property var results: {
        Calc.language
        return searching ? Calc.search(search.text) : []
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
            spacing: 14

            // Hero
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: hero.implicitHeight + 36
                radius: 16
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: "#1F3A5F" }
                    GradientStop { position: 1.0; color: "#2E5E8E" }
                }
                ColumnLayout {
                    id: hero
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 4
                    Label {
                        text: Calc.ui.appTitle
                        color: "white"
                        font.pixelSize: 24
                        font.weight: Font.Bold
                    }
                    Label {
                        Layout.fillWidth: true
                        text: Calc.ui.tagline
                        color: "#F2A900"
                        font.pixelSize: 13
                        wrapMode: Text.Wrap
                    }
                    Label {
                        text: (Calc.norm === "ch" ? Calc.ui.normCh : Calc.ui.normDe)
                        color: "#C9D6E6"
                        font.pixelSize: 11
                        topPadding: 6
                    }
                }
            }

            TextField {
                id: search
                Layout.fillWidth: true
                placeholderText: Calc.ui.search
                inputMethodHints: Qt.ImhNoPredictiveText
                selectByMouse: true
                rightPadding: 40
                ToolButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    visible: page.searching
                    text: "✕"
                    onClicked: search.clear()
                }
            }

            // Search results
            Card {
                visible: page.searching
                padding: 4
                spacing: 0
                Repeater {
                    model: page.results
                    delegate: ItemRow {
                        required property var modelData
                        item: modelData
                    }
                }
                Label {
                    visible: page.results.length === 0
                    text: Calc.ui.noResults
                    padding: 14
                    opacity: 0.7
                }
            }

            // Favourites
            Label {
                visible: !page.searching && page.favItems.length > 0
                text: Calc.ui.favorites.toUpperCase()
                font.pixelSize: 12
                font.letterSpacing: 1.2
                font.weight: Font.DemiBold
                opacity: 0.6
            }
            Card {
                visible: !page.searching && page.favItems.length > 0
                padding: 4
                spacing: 0
                Repeater {
                    model: page.favItems
                    delegate: ItemRow {
                        required property var modelData
                        item: modelData
                    }
                }
            }

            // Categories (sections can be hidden via "Customize")
            Item {
                visible: !page.searching
                Layout.fillWidth: true
                implicitHeight: Math.max(catLabel.implicitHeight, customizeButton.implicitHeight)
                Label {
                    id: catLabel
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: Calc.ui.categories.toUpperCase()
                    font.pixelSize: 12
                    font.letterSpacing: 1.2
                    font.weight: Font.DemiBold
                    opacity: 0.6
                }
                ToolButton {
                    id: customizeButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: "✎ " + Calc.ui.customize
                          + (Calc.hiddenCategories.length > 0 ? " (" + Calc.hiddenCategories.length + " " + Calc.ui.hiddenCount + ")" : "")
                    font.pixelSize: 13
                    onClicked: sectionsDialog.open()
                }
            }
            Label {
                visible: !page.searching && page.visibleCategories.length === 0
                text: Calc.ui.allHidden
                opacity: 0.7
            }
            GridLayout {
                visible: !page.searching
                Layout.fillWidth: true
                columns: Math.max(2, Math.floor(col.width / 190))
                columnSpacing: 12
                rowSpacing: 12

                Repeater {
                    model: page.visibleCategories
                    delegate: Rectangle {
                        id: tile
                        required property var modelData
                        Layout.fillWidth: true
                        Layout.preferredWidth: 100
                        implicitHeight: 128
                        radius: 16
                        color: tap.pressed ? Qt.darker(baseColor, Calc.darkTheme ? 0.8 : 1.06) : baseColor
                        border.color: Calc.darkTheme ? "#2A3445" : "#E2E6EC"
                        readonly property color baseColor: Calc.darkTheme ? "#1C2330" : "#FFFFFF"

                        Rectangle {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: 5
                            radius: 2
                            color: tile.modelData.color
                        }
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 14
                            anchors.leftMargin: 18
                            spacing: 6
                            Rectangle {
                                Layout.preferredWidth: 42
                                Layout.preferredHeight: 42
                                radius: 12
                                color: tile.modelData.color
                                Label {
                                    anchors.centerIn: parent
                                    text: tile.modelData.badge
                                    color: "white"
                                    font.bold: true
                                    font.pixelSize: tile.modelData.badge.length > 2 ? 12 : 16
                                }
                            }
                            Label {
                                Layout.fillWidth: true
                                text: tile.modelData.title
                                font.pixelSize: 14
                                font.weight: Font.DemiBold
                                wrapMode: Text.Wrap
                                maximumLineCount: 2
                                elide: Text.ElideRight
                            }
                            Label {
                                text: tile.modelData.count + " " + Calc.ui.entries
                                font.pixelSize: 11
                                opacity: 0.6
                            }
                            Item { Layout.fillHeight: true }
                        }
                        // The attached ApplicationWindow property must be read from an Item (the tile):
                        // inside a TapHandler it is null and the tap did nothing.
                        TapHandler {
                            id: tap
                            onTapped: tile.ApplicationWindow.window.openCategory(tile.modelData.id)
                        }
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                Layout.topMargin: 8
                text: Calc.ui.disclaimer
                wrapMode: Text.Wrap
                font.pixelSize: 11
                opacity: 0.55
            }
        }
    }

    Dialog {
        id: sectionsDialog
        parent: Overlay.overlay
        modal: true
        title: Calc.ui.sectionsTitle
        width: Math.min(parent ? parent.width - 32 : 400, 440)
        height: Math.min(parent ? parent.height - 48 : 600, 720)
        x: parent ? (parent.width - width) / 2 : 0
        y: parent ? (parent.height - height) / 2 : 0
        padding: 16
        background: Rectangle {
            radius: 12
            color: Calc.darkTheme ? "#1C2330" : "#FFFFFF"
            border.color: Calc.darkTheme ? "#2A3445" : "#E2E6EC"
        }

        contentItem: Flickable {
            contentHeight: dialogColumn.height
            clip: true
            ScrollBar.vertical: ScrollBar {}
            Column {
                id: dialogColumn
                width: parent.width
                spacing: 8
                Label {
                    width: parent.width
                    text: Calc.ui.sectionsHint
                    wrapMode: Text.Wrap
                    font.pixelSize: 12
                    opacity: 0.7
                }
                CategoryToggles {
                    width: parent.width
                }
            }
        }
        footer: DialogButtonBox {
            Button {
                text: Calc.ui.showAll
                flat: true
                enabled: Calc.hiddenCategories.length > 0
                onClicked: Calc.showAllCategories()
            }
            Button {
                text: Calc.ui.close
                flat: true
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }
        }
    }
}
