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
    {"normHint", "Beeinflusst Grenzwerte (z. B. Spannungsfall 3 % / 4 %) und Währung.",
     "Affects limits (e.g. voltage drop 3 % / 4 %) and currency."},
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
};

}  // namespace

QVariantMap uiStrings(elc::Lang lang) {
    QVariantMap m;
    for (const auto& e : kStrings) m.insert(QString::fromUtf8(e.key), QString::fromUtf8(lang == elc::Lang::De ? e.de : e.en));
    return m;
}
