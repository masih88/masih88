#include "UiStrings.h"

#include <QString>

namespace {

struct Entry {
    const char* key;
    const char* de;
    const char* en;
};

constexpr Entry kStrings[] = {
    {"appTitle", "ElektroRechner", "ElektroRechner"},
    {"tagline", "Elektro · Gebäudeautomation · Industrie", "Electrical · Building automation · Industry"},
    {"search", "Rechner oder Tabelle suchen …", "Search calculators or tables …"},
    {"favorites", "Favoriten", "Favourites"},
    {"categories", "Kategorien", "Categories"},
    {"entries", "Einträge", "entries"},
    {"noResults", "Keine Treffer", "No results"},
    {"inputs", "Eingaben", "Inputs"},
    {"results", "Ergebnisse", "Results"},
    {"formulas", "Formeln & Hinweise", "Formulas & notes"},
    {"reset", "Zurücksetzen", "Reset"},
    {"copy", "Kopieren", "Copy"},
    {"copied", "Ergebnis in die Zwischenablage kopiert", "Result copied to clipboard"},
    {"optional", "optional", "optional"},
    {"settings", "Einstellungen", "Settings"},
    {"language", "Sprache", "Language"},
    {"german", "Deutsch", "German"},
    {"english", "Englisch", "English"},
    {"norm", "Normenprofil", "Norm profile"},
    {"normDe", "Deutschland (DIN VDE)", "Germany (DIN VDE)"},
    {"normCh", "Schweiz (NIN / SN 411000)", "Switzerland (NIN / SN 411000)"},
    {"normHint", "Beeinflusst Grenzwerte (z. B. Spannungsfall 3 % / 4 %) und die EnEfG-Hinweise.",
     "Affects limits (e.g. voltage drop 3 % / 4 %) and the EnEfG notes."},
    {"currency", "Währung", "Currency"},
    {"euro", "Euro (€)", "Euro (€)"},
    {"franc", "Schweizer Franken (CHF)", "Swiss franc (CHF)"},
    {"rate", "Wechselkurs: 1 € =", "Exchange rate: 1 € ="},
    {"rateHint", "Preise werden in der gewählten Währung eingegeben; Kosten erscheinen zusätzlich in der anderen Währung. "
                 "Stand 31.10.2026 etwa 0,92 CHF – Tageskurs der Bank eintragen.",
     "Prices are entered in the chosen currency; costs are also shown in the other currency. About 0.92 CHF on "
     "31 Oct 2026 – enter your bank's daily rate."},
    {"appearance", "Darstellung", "Appearance"},
    {"darkTheme", "Dunkles Design", "Dark theme"},
    {"about", "Über die App", "About"},
    {"version", "Version", "Version"},
    {"disclaimer",
     "Alle Berechnungen ohne Gewähr. Die Ergebnisse ersetzen keine normgerechte Planung, Bemessung und Prüfung durch "
     "eine Elektrofachkraft. Tabellenwerte und Grenzwerte vor Verwendung gegen die gültige Normausgabe prüfen.",
     "All calculations without warranty. Results do not replace standards-compliant design and verification by a "
     "qualified electrician. Check table values and limits against the edition in force before use."},
    {"calculator", "Rechner", "Calculator"},
    {"table", "Tabelle", "Table"},
    {"source", "Quelle / Hinweis", "Source / note"},
    {"favoriteAdd", "Zu Favoriten", "Add to favourites"},
    {"favoriteRemove", "Aus Favoriten entfernen", "Remove from favourites"},
    {"list", "Liste", "List"},
    {"rows", "Zeilen", "Rows"},
    {"row", "Zeile", "Row"},
    {"totals", "Summen", "Totals"},
    {"actions", "Liste", "List"},
    {"projectSettings", "Projekteinstellungen", "Project settings"},
    {"addRow", "Zeile", "Row"},
    {"copyExcel", "Für Excel kopieren", "Copy for Excel"},
    {"pasteExcel", "Aus Excel einfügen", "Paste from Excel"},
    {"pasteReplace", "Einfügen – Liste ersetzen", "Paste – replace list"},
    {"pasteAppend", "Einfügen – an Liste anhängen", "Paste – append to list"},
    {"copyDetail", "%1 kopieren (Excel)", "Copy %1 (Excel)"},
    {"saveCsv", "Als CSV-Datei speichern", "Save as CSV file"},
    {"saveDetailCsv", "%1 als CSV speichern", "Save %1 as CSV"},
    {"loadExample", "Beispielprojekt laden", "Load example project"},
    {"clearList", "Liste leeren", "Clear list"},
    {"confirmClear", "Alle Zeilen löschen?", "Delete all rows?"},
    {"copiedTable", "Tabelle kopiert – in Excel mit Strg+V einfügen", "Table copied – paste into Excel with Ctrl+V"},
    {"savedTo", "Gespeichert: %1", "Saved: %1"},
    {"saveFailed", "Speichern fehlgeschlagen", "Saving failed"},
    {"imported", "%1 Zeilen übernommen", "%1 rows imported"},
    {"importNone", "Keine passenden Spalten in der Zwischenablage gefunden (Kopfzeile mitkopieren).",
     "No matching columns found in the clipboard (copy the header row too)."},
    {"emptyList", "Noch keine Zeilen – „+ Zeile“ tippen oder eine Tabelle aus Excel einfügen.",
     "No rows yet – tap \"+ Row\" or paste a table from Excel."},
    {"listHelp",
     "Excel: Bereich inkl. Kopfzeile markieren, kopieren, hier „Aus Excel einfügen“. Spalten werden über ihre Überschrift "
     "erkannt (auch die der Vorlage „Leistungsdaten_Kabelauslegung“). Die Liste wird automatisch gespeichert.",
     "Excel: select the range including the header row, copy, then \"Paste from Excel\". Columns are recognised by their "
     "heading. The list is saved automatically."},
    {"notOk", "nicht i. O.", "not OK"},
    {"duplicate", "Duplizieren", "Duplicate"},
    {"deleteRow", "Löschen", "Delete"},
    {"done", "Fertig", "Done"},
    {"more", "Mehr", "More"},
    {"chart", "Diagramm", "Chart"},
    {"schematic", "Prinzipskizze", "Schematic"},
    {"customize", "Anpassen", "Customize"},
    {"sectionsTitle", "Bereiche ein- und ausblenden", "Show or hide sections"},
    {"sectionsHint", "Ausgeblendete Bereiche erscheinen nicht auf der Startseite. Die Suche findet weiterhin alle Rechner.",
     "Hidden sections do not appear on the home page. Search still finds every calculator."},
    {"hiddenCount", "ausgeblendet", "hidden"},
    {"showAll", "Alle einblenden", "Show all"},
    {"close", "Schließen", "Close"},
    {"allHidden", "Alle Bereiche sind ausgeblendet.", "All sections are hidden."},
};

}  // namespace

QVariantMap uiStrings(elc::Lang lang) {
    QVariantMap m;
    for (const auto& e : kStrings) m.insert(QString::fromUtf8(e.key), QString::fromUtf8(lang == elc::Lang::De ? e.de : e.en));
    return m;
}
