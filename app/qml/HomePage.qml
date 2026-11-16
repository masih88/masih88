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
    readonly property int totalEntries: {
        let n = 0
        const cats = Calc.categories
        for (let i = 0; i < cats.length; ++i)
            n += cats[i].count
        return n
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

            // Hero: logo, name, tagline, size of the catalogue and active standard
            Rectangle {
                id: heroCard
                Layout.fillWidth: true
                implicitHeight: hero.implicitHeight + 36
                radius: 20
                clip: true
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: Calc.theme.brand }
                    GradientStop { position: 1.0; color: Calc.theme.brand2 }
                }
                // decorative rings
                Rectangle {
                    width: 180; height: 180; radius: 90
                    x: heroCard.width - 110; y: -70
                    color: "transparent"
                    border.color: "#FFFFFF"; border.width: 22
                    opacity: 0.05
                }
                Rectangle {
                    width: 90; height: 90; radius: 45
                    x: heroCard.width - 60; y: heroCard.height - 50
                    color: "#FFFFFF"
                    opacity: 0.05
                }
                ColumnLayout {
                    id: hero
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 12
                    RowLayout {
                        spacing: 14
                        Rectangle {
                            Layout.preferredWidth: 52
                            Layout.preferredHeight: 52
                            radius: 15
                            color: "#FFFFFF"
                            opacity: 0.95
                            Canvas {
                                // lightning bolt logo
                                anchors.fill: parent
                                onPaint: {
                                    const c = getContext("2d")
                                    c.reset()
                                    c.fillStyle = "#F2A900"
                                    c.strokeStyle = "#1F3A5F"
                                    c.lineWidth = 1.5
                                    c.beginPath()
                                    c.moveTo(30, 7); c.lineTo(15, 29); c.lineTo(25, 29)
                                    c.lineTo(21, 45); c.lineTo(37, 22); c.lineTo(27, 22); c.lineTo(32, 7)
                                    c.closePath()
                                    c.fill(); c.stroke()
                                }
                            }
                        }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Label {
                                text: Calc.ui.appTitle
                                color: "white"
                                font.pixelSize: 23
                                font.weight: Font.Bold
                            }
                            Label {
                                Layout.fillWidth: true
                                text: Calc.ui.tagline
                                color: Calc.theme.accent
                                font.pixelSize: 13
                                font.weight: Font.Medium
                                wrapMode: Text.Wrap
                            }
                        }
                    }
                    Flow {
                        Layout.fillWidth: true
                        spacing: 8
                        Repeater {
                            model: [page.totalEntries + " " + Calc.ui.entries,
                                    Calc.categories.length + " " + Calc.ui.areasLabel,
                                    (Calc.norm === "ch" ? Calc.ui.normCh : Calc.ui.normDe)]
                            delegate: Item {
                                required property var modelData
                                height: 26
                                width: statLabel.implicitWidth + 20
                                Rectangle {
                                    anchors.fill: parent
                                    radius: 13
                                    color: "#FFFFFF"
                                    opacity: 0.16
                                }
                                Label {
                                    id: statLabel
                                    anchors.centerIn: parent
                                    text: parent.modelData
                                    color: "white"
                                    font.pixelSize: 12
                                    font.weight: Font.Medium
                                }
                            }
                        }
                    }
                }
            }

            // Search field as a rounded bar with a drawn magnifier
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 50
                radius: 25
                color: Calc.theme.surface
                border.color: search.activeFocus ? Calc.theme.brand2 : Calc.theme.border
                border.width: search.activeFocus ? 2 : 1
                Item {
                    id: magnifier
                    width: 20; height: 20
                    anchors.left: parent.left
                    anchors.leftMargin: 18
                    anchors.verticalCenter: parent.verticalCenter
                    Rectangle {
                        width: 13; height: 13; radius: 6.5
                        color: "transparent"
                        border.color: Calc.theme.muted
                        border.width: 2
                    }
                    Rectangle {
                        x: 11; y: 13; width: 8; height: 2.4; radius: 1.2
                        color: Calc.theme.muted
                        rotation: 45
                        transformOrigin: Item.Left
                    }
                }
                TextField {
                    id: search
                    anchors.left: magnifier.right
                    anchors.leftMargin: 8
                    anchors.right: clearButton.left
                    anchors.verticalCenter: parent.verticalCenter
                    placeholderText: Calc.ui.search
                    inputMethodHints: Qt.ImhNoPredictiveText
                    selectByMouse: true
                    background: null
                    color: Calc.theme.text
                    font.pixelSize: 15
                    topPadding: 8
                    bottomPadding: 8
                }
                ToolButton {
                    id: clearButton
                    anchors.right: parent.right
                    anchors.rightMargin: 4
                    anchors.verticalCenter: parent.verticalCenter
                    visible: page.searching
                    width: visible ? implicitWidth : 8
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
                color: Calc.theme.muted
                font.pixelSize: 12
                font.letterSpacing: 1.2
                font.weight: Font.DemiBold
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
                    color: Calc.theme.muted
                    font.pixelSize: 12
                    font.letterSpacing: 1.2
                    font.weight: Font.DemiBold
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
                rowSpacing: 14

                Repeater {
                    model: page.visibleCategories
                    delegate: Rectangle {
                        id: tile
                        required property var modelData
                        Layout.fillWidth: true
                        Layout.preferredWidth: 100
                        implicitHeight: 124
                        radius: 16
                        color: tap.pressed ? Calc.theme.pressed : Calc.theme.surface
                        border.color: Calc.theme.border

                        // soft shadow below the tile
                        Rectangle {
                            z: -1
                            y: 2
                            width: parent.width
                            height: parent.height
                            radius: parent.radius
                            color: Calc.theme.shadow
                            opacity: Calc.darkTheme ? 0.8 : 0.55
                        }
                        // accent line at the bottom in the area colour
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.leftMargin: 16
                            anchors.rightMargin: 16
                            height: 3
                            radius: 1.5
                            color: tile.modelData.color
                            opacity: 0.85
                        }
                        Rectangle {
                            id: badge
                            x: 14
                            y: 14
                            width: 44
                            height: 44
                            radius: 13
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
                            anchors.right: parent.right
                            anchors.rightMargin: 14
                            anchors.verticalCenter: badge.verticalCenter
                            text: "›"
                            color: Calc.theme.muted
                            font.pixelSize: 22
                        }
                        Label {
                            id: tileTitle
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: badge.bottom
                            anchors.leftMargin: 14
                            anchors.rightMargin: 12
                            anchors.topMargin: 10
                            text: tile.modelData.title
                            color: Calc.theme.text
                            font.pixelSize: 14
                            font.weight: Font.DemiBold
                            wrapMode: Text.Wrap
                            maximumLineCount: 2
                            elide: Text.ElideRight
                        }
                        Label {
                            anchors.left: tileTitle.left
                            anchors.top: tileTitle.bottom
                            anchors.topMargin: 2
                            text: tile.modelData.count + " " + Calc.ui.entries
                            color: Calc.theme.muted
                            font.pixelSize: 11
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
