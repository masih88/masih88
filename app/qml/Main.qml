import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

ApplicationWindow {
    id: win
    width: 440
    height: 860
    minimumWidth: 320
    minimumHeight: 480
    visible: true
    title: "ElektroRechner"

    readonly property color brand: "#1F3A5F"
    readonly property color amber: "#F2A900"

    Material.theme: Calc.darkTheme ? Material.Dark : Material.Light
    Material.primary: brand
    Material.accent: amber
    color: Calc.darkTheme ? "#121722" : "#F3F5F8"

    function openItem(id) {
        const kind = Calc.kindOf(id)
        if (kind === "calc")
            stack.push(Qt.resolvedUrl("CalculatorPage.qml"), { calcId: id })
        else if (kind === "table")
            stack.push(Qt.resolvedUrl("TablePage.qml"), { tableId: id })
        else if (kind === "list")
            stack.push(Qt.resolvedUrl("ProjectListPage.qml"), { listId: id })
    }

    function openCategory(categoryId) {
        stack.push(Qt.resolvedUrl("ListPage.qml"), { categoryId: categoryId })
    }

    function openSettings() {
        stack.push(Qt.resolvedUrl("SettingsPage.qml"))
    }

    header: ToolBar {
        Material.foreground: "white"
        background: Rectangle {
            implicitHeight: 56
            color: win.brand
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                visible: stack.depth > 1
                text: "‹"
                font.pixelSize: 30
                onClicked: stack.pop()
            }
            Label {
                Layout.fillWidth: true
                leftPadding: stack.depth > 1 ? 0 : 16
                text: stack.currentItem && stack.currentItem.title ? stack.currentItem.title : ""
                font.pixelSize: text.length > 26 ? 15 : 18
                font.weight: Font.DemiBold
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
            }
            ToolButton {
                readonly property string fid: stack.currentItem && stack.currentItem.favId ? stack.currentItem.favId : ""
                visible: fid !== ""
                text: Calc.favorites.indexOf(fid) >= 0 ? "★" : "☆"
                font.pixelSize: 22
                Material.foreground: Calc.favorites.indexOf(fid) >= 0 ? win.amber : "white"
                onClicked: Calc.toggleFavorite(fid)
                ToolTip.visible: hovered
                ToolTip.text: Calc.favorites.indexOf(fid) >= 0 ? Calc.ui.favoriteRemove : Calc.ui.favoriteAdd
            }
            ToolButton {
                text: Calc.language === "de" ? "EN" : "DE"
                font.bold: true
                onClicked: Calc.language = (Calc.language === "de" ? "en" : "de")
            }
            ToolButton {
                visible: !(stack.currentItem && stack.currentItem.isSettings)
                text: "⚙"
                font.pixelSize: 20
                onClicked: win.openSettings()
            }
        }
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: HomePage {}
    }

    // Android back button: navigate back first (desktop close button always closes)
    onClosing: function (close) {
        if (Qt.platform.os === "android" && stack.depth > 1) {
            close.accepted = false
            stack.pop()
        }
    }

    Component.onCompleted: {
        const start = Calc.startPage
        if (start === "settings")
            openSettings()
        else if (start.indexOf("cat:") === 0)
            openCategory(start.substring(4))
        else if (start.indexOf("#") > 0) {
            // "consumer_list#3": list with row editor (automated screenshots)
            openItem(start.substring(0, start.indexOf("#")))
            stack.currentItem.openRow(parseInt(start.substring(start.indexOf("#") + 1)))
        } else if (start.indexOf("!") > 0) {
            openItem(start.substring(0, start.indexOf("!")))  // "ga_points!settings": settings expanded
        } else if (start !== "")
            openItem(start)
    }
}
