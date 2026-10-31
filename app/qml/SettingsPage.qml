import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    title: Calc.ui.settings
    readonly property bool isSettings: true
    background: null

    component SectionTitle: Label {
        font.pixelSize: 12
        font.letterSpacing: 1.2
        font.weight: Font.DemiBold
        opacity: 0.6
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

            Card {
                SectionTitle { text: Calc.ui.language.toUpperCase() }
                RadioButton {
                    text: "Deutsch"
                    checked: Calc.language === "de"
                    onClicked: Calc.language = "de"
                }
                RadioButton {
                    text: "English"
                    checked: Calc.language === "en"
                    onClicked: Calc.language = "en"
                }
            }

            Card {
                SectionTitle { text: Calc.ui.norm.toUpperCase() }
                RadioButton {
                    text: Calc.ui.normDe
                    checked: Calc.norm === "de"
                    onClicked: Calc.norm = "de"
                }
                RadioButton {
                    text: Calc.ui.normCh
                    checked: Calc.norm === "ch"
                    onClicked: Calc.norm = "ch"
                }
                Label {
                    Layout.fillWidth: true
                    text: Calc.ui.normHint
                    wrapMode: Text.Wrap
                    font.pixelSize: 12
                    opacity: 0.65
                }
            }

            Card {
                SectionTitle { text: Calc.ui.currency.toUpperCase() }
                RadioButton {
                    text: Calc.ui.euro
                    checked: Calc.currency === "EUR"
                    onClicked: Calc.currency = "EUR"
                }
                RadioButton {
                    text: Calc.ui.franc
                    checked: Calc.currency === "CHF"
                    onClicked: Calc.currency = "CHF"
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    Label {
                        Layout.fillWidth: true
                        text: Calc.ui.rate
                        wrapMode: Text.Wrap
                    }
                    TextField {
                        id: rateField
                        Layout.preferredWidth: 100
                        horizontalAlignment: TextInput.AlignRight
                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                        selectByMouse: true
                        text: Calc.formatNumber(Calc.chfPerEur, 4)
                        onEditingFinished: {
                            const v = Calc.parseNumber(text)
                            if (v > 0)
                                Calc.chfPerEur = v
                            text = Calc.formatNumber(Calc.chfPerEur, 4)
                        }
                    }
                    Label { text: "CHF" }
                }
                Label {
                    Layout.fillWidth: true
                    text: Calc.ui.rateHint
                    wrapMode: Text.Wrap
                    font.pixelSize: 12
                    opacity: 0.65
                }
            }

            Card {
                SectionTitle { text: Calc.ui.appearance.toUpperCase() }
                Switch {
                    text: Calc.ui.darkTheme
                    checked: Calc.darkTheme
                    onToggled: Calc.darkTheme = checked
                }
            }

            Card {
                SectionTitle { text: Calc.ui.about.toUpperCase() }
                Label {
                    text: "ElektroRechner · " + Calc.ui.version + " " + Calc.version
                    font.weight: Font.DemiBold
                }
                Label {
                    Layout.fillWidth: true
                    text: Calc.ui.disclaimer
                    wrapMode: Text.Wrap
                    font.pixelSize: 12
                    opacity: 0.7
                }
            }
        }
    }
}
