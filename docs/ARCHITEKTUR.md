# Architektur & Entwurfsentscheidungen

Stand: 01.11.2026 · Version 1.5.0

## Ziel

Eine Rechner-App für Elektroplanung mit Schwerpunkt Gebäudeautomation (GA/MSR) und Industrie, die auf
**Windows** und **Android** läuft, **Deutsch/Englisch** umschaltet und Grenzwerte für **Deutschland (DIN VDE)**
und die **Schweiz (NIN)** kennt. Die Berechnungen sollen prüfbar (Tests, Formeltexte, Quellen) und leicht
erweiterbar sein.

## Überblick

```text
┌──────────────────────── app/ (Qt 6 Quick, QML) ───────────────────────┐
│ Main.qml ─ HomePage ─ ListPage ─ CalculatorPage* ─ TablePage ─ Settings │
│                                 └ ProjectListPage* ─ RowEditorPage*      │
│   (* generisch: rendern jeden Rechner / jede Projektliste aus der Definition;│
│      Bausteine InputForm, ResultView, ResultBar, Status.js)             │
│                         CalcBridge (Kontext-Property „Calc“)            │
└───────────────────────────────────┬─────────────────────────────────────┘
                                    │ QVariantMap / Strings
┌──────────────────────── core/ (C++20, ohne Qt) ────────────────────────┐
│ registry   → categories(), calculators(), tables(), lists(), find…()   │
│ lists/*    → ListDef{ Spalten, Zeilen-/Summenergebnisse, computeRow,    │
│              summarize, expand } + exportCsv()/importTable()           │
│ catalog/*  → Calculator{ id, Texte DE/EN, InputDef[], OutputDef[], compute }│
│ formulas/* → reine Funktionen (voltageDrop, baseAmpacity, rtdResistance …)│
│ types      → Text, Inputs, Result, evaluate() (Validierung + Ausnahmen)  │
└─────────────────────────────────────────────────────────────────────────┘
          ▲                                   ▲
      cli/elcalc                         tests/ (Catch2)
```

## Entscheidungen

| Thema | Entscheidung | Begründung |
|---|---|---|
| Plattform | Qt 6 Quick (QML) + Material-Stil | Ein C++-Code für Windows und Android, touch-tauglich |
| Rechenkern | Eigenständige C++20-Bibliothek ohne Qt | Testbar, wiederverwendbar (CLI, spätere Oberflächen), schnelle Tests |
| Rechner-Definition | **Datengetrieben**: jeder Rechner ist Daten + eine `compute`-Funktion | Eine generische QML-Seite für alle Rechner; neue Rechner ohne QML-Änderung |
| Übersetzung | `Text{de, en}` direkt in den Definitionen; UI-Texte in `UiStrings.cpp` | Sprache zur Laufzeit umschaltbar, kein `lrelease`-Schritt nötig |
| Zahlen | Ein-/Ausgabe locale-unabhängig (`iostream` + `std::locale::classic`), Komma und Punkt erlaubt | Qt ruft `setlocale()` auf (Linux/Android) – `printf`/`strtod` würden sonst mit Dezimalkomma brechen |
| Normprofil | `Context{NormProfile}` wird jedem `compute` übergeben | Grenzwerte (3 %/4 %); weitere länderabhängige Werte leicht ergänzbar |
| Währung | `Context{currency, chfPerEur}`; Ausgaben mit Einheit „€“ bekommen in `applyCurrency()` die gewählte Währung und `Result::alt` mit dem umgerechneten Betrag; Preiseingaben mit Einheit `kCurrencyUnit` zeigen die gewählte Währung | Euro und Franken unabhängig vom Normprofil, Kurs einstellbar |
| Kennbuchstaben | Datenbank `elc/instruments.hpp` (Name, Suchwörter, ISA-/EN-Buchstabe, VDI-Kürzel, IEC 81346), Codes per Regel aus Messgröße + Funktion | Ein Datensatz liefert Finder, Tabelle und Tests |
| Fehler | `evaluate()` validiert Pflichtfelder/Bereiche; `CalcError` für physikalisch Unmögliches | Die UI zeigt verständliche Meldungen statt Abstürzen |
| Sichtbarkeit | `InputDef::showIf(choiceId, {werte})` | Felder erscheinen nur, wenn relevant (z. B. cos φ nicht bei DC) |
| Hervorhebung | `OutputDef::primary()`, `abbrev()` | Schlüsselergebnisse in der Ergebnisleiste (Handy) und auf den Zeilenkarten |
| Projektlisten | `ListDef`: viele Zeilen gleicher Art + Projekteinstellungen + Summen | Verbraucherliste und Datenpunktliste nutzen dieselbe Seite, denselben Export/Import |
| Excel | Import über Spaltenüberschrift (ID, Bezeichnung DE/EN, `alias()`), Export Tab (Zwischenablage) bzw. `;` + BOM (Datei) | Kopieren/Einfügen funktioniert direkt mit Excel, auch mit der bestehenden Vorlage |
| Speicherung | Zeilen als JSON-Text in `QSettings` (`lists/<id>/rows`) | Kein Datenbank-Code; läuft gleich auf Windows (Registry) und Android (ini) |
| Tests | Formeltests mit Normwerten + **Vertragstests** für jeden Rechner/jede Tabelle | Jeder neue Rechner wird automatisch auf Vollständigkeit (DE/EN), Standardwerte und endliche Ergebnisse geprüft |
| Qualität | `-Wall -Wextra -Wpedantic -Wshadow`, ASan/UBSan-Option, MSVC `/W4 /utf-8` | Vorgaben C++ Core Guidelines; `/utf-8` nötig wegen Ω, φ, ² in Strings |

## Datenmodell (core/include/elc/types.hpp)

- `Text` – Deutsch + Englisch (ein String = beide Sprachen).
- `InputDef` – Zahl, Auswahl oder Text; Einheit, Standardwert, optional, Bereich (`positive()`, `range()`), `showIf()`,
  Hinweis, `alias()` (zusätzliche Spaltennamen für den Import), `startsSection()` (Gruppenüberschrift im Formular).
- `OutputDef` – Zahl (Einheit, Nachkommastellen) oder Text; `primary()` für Schlüsselergebnis.
- `Inputs` – Werte nach ID; fehlende Zahlen sind `NaN`.
- `Result` – Zahlenwerte, Texte, Einheiten-Overrides (z. B. kΩ/MΩ), Meldungen (`ok`, `warn`, `error`, `info`).
- `evaluate(calc, inputs, ctx)` – setzt Auswahl-Standardwerte, prüft sichtbare Pflichtfelder und Bereiche,
  ruft `compute` auf und wandelt Ausnahmen in Meldungen um. Wirft nie wegen Benutzereingaben.

## Neuen Rechner hinzufügen

1. Formel als reine Funktion in `core/include/elc/formulas/<bereich>.hpp` + `.cpp` und **Test** in `tests/`.
2. In `core/src/catalog/<kategorie>.cpp` eine Funktion schreiben und in `add<Kategorie>()` registrieren:

```cpp
Calculator myCalculator() {
    Calculator c;
    c.id = "my_calc";                       // eindeutig, wird für Favoriten gespeichert
    c.category = "automation";              // Vorgabe; maßgeblich ist der Eintrag in catalog/layout.cpp
    c.title = {"Mein Rechner", "My calculator"};
    c.summary = {"Kurzbeschreibung", "Short description"};
    c.description = {"Formel und Norm (DE)", "Formula and standard (EN)"};
    c.inputs = {
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 24).positive(),
        Num::choice("mode", {"Modus", "Mode"}, {{"a", {"A", "A"}}, {"b", {"B", "B"}}}, "a"),
    };
    c.outputs = {Out::number("I", {"Strom", "Current"}, "A", 2).primary()};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        r.set("I", in.num("U") / 12.0);
        if (r.values["I"] > 1.5) r.warn({"Warnung …", "Warning …"});
        return r;
    };
    return c;
}
```

3. Die ID genau einmal in `core/src/catalog/layout.cpp` in Bereich und Gruppe eintragen.
4. Bauen und `elc_tests` ausführen – die Vertragstests prüfen den neuen Rechner automatisch (auch, dass er im Layout steht).
5. `elcalc markdown > docs/BERECHNUNGEN.md` und `elcalc markdown --lang en > docs/CALCULATIONS.md` neu erzeugen.

Tabellen: `make(...)` in `core/src/tables/tables.cpp`, in `addTables()` registrieren und ebenfalls in `layout.cpp` eintragen.

## Diagramme

Ein Rechner kann ein Diagramm liefern: `Calculator::chart` (`ChartFn`) baut aus Eingaben und Ergebnis ein `Chart`
(`elc/chart.hpp`: Achsen, Linienzüge mit Stil, Beschriftungen, Markierungen) in Dateneinheiten; die App zeichnet es mit
`ChartView.qml` (Canvas, Clipping, Achsen). Optional `Calculator::pick` (`PickFn`): Tippen ins Diagramm liefert neue
Eingabewerte (h,x-Diagramm: t₁ und φ₁). Beispiel: `hxGrid()` in `core/src/catalog/hvac.cpp` – schiefwinklige
h,x-Koordinaten mit y = t · (1,006 + 1,86·x) / 1,006 (Isothermen leicht steigend, Isenthalpen fallend).
Tests: `tests/test_hx.cpp` (Gitter, Zustandspunkte, Rückrechnung beim Antippen).

### Prinzipskizzen (ab 1.6)

`Chart::diagram = true` macht aus dem Diagramm eine Skizze: keine Achsen, gleicher Maßstab in x und y (zentriert),
Linienzüge in der gegebenen Reihenfolge; `Chart::aspect` gibt das Seitenverhältnis (Höhe/Breite) der Zeichenfläche vor.
Stile: `vl`/`rl` Vor-/Rücklauf (rot/blau), `pipe`, `ink` (Kontur), `inkfill` (gefülltes Symbol), `bgfill` (mit
Hintergrund gefüllt und umrandet – verdeckt darunterliegende Rohre), `sig` (gestrichelte Wirkungslinie);
Beschriftungen `c`/`cb` zentriert, Marker `motor` (M) und `sensor` (T). Beispiel: `core/src/catalog/hydraulics.cpp`
(Zeichenhilfe `Sketch` mit Pumpe, Ventilen, Strangregulierventil, Rückschlagklappe, Fühler), Tests in
`tests/test_hydraulics.cpp` (alle Punkte im Zeichenbereich, Auswahlregeln).

## Werte aus Fachbüchern (ab 1.5)

Die Fachbücher des Benutzers (Ordner *Fachbücher*, nach Fachgebiet sortiert, siehe `Bücherverzeichnis.md` dort) wurden
mit `doc2md` in Markdown umgewandelt und gezielt durchsucht. Regeln für die Übernahme:

- Nur Formeln, Kennwerte und Codelisten übernehmen – mit Buch, Ausgabe und Seite in `description`; keine Buchtexte.
- Rechenbeispiele aus dem Buch werden zu Tests (`tests/test_books.cpp`, z. B. Heizkennlinie 67/33 °C → s = 1,7;
  Proportionalitätsgesetze 4000 → 8000 m³/h, 350 → 1400 Pa; Trafo 400 kVA, u_k 4 % → I_kD = 14,43 kA).
- Ist das Buch älter als die gültige Regel, rechnet die App mit der aktuellen Fassung und nennt den Buchstand in der
  Beschreibung (ASR A2.3: Buch 2007, App 2022). Druckfehler im Buch werden vermerkt (h,x-Höhentabelle k bei 800 m,
  Vorzeichen DIN-18041-Formeln im BAuA-Bericht S. 50).
- Neue Formelmodule: `control` (Regler-Einstellregeln), `heating` (Heizkennlinie, Proportionalitätsgesetze, Carnot-COP,
  kleinste regelbare Ventilleistung), `building` (BAC-Faktoren, Fluchtwege, Aufzugsschacht, KNX-Topologie),
  `acoustics` (Sabine, DIN 18041, Pegeladdition). Kataloge: `catalog/building.cpp` (Kategorie „building“) und
  `catalog/controls.cpp` (Regler, KNX, AMEV-Adresse mit Codelisten aus AMEV GA 2005 Anhang 2.1).

## Neue Projektliste hinzufügen

`ListDef` in `core/src/lists/<name>.cpp` (Vorbild `loadlist.cpp`), in `registry.cpp` → `lists()` eintragen:

- `settingsInputs` (Projektparameter), `rowInputs` (erste Eingabe = Text = Zeilenname), `rowOutputs`, `totalOutputs`.
- `computeRow(row, settings, ctx)` → `Result` je Zeile; `summarize(rows, results, settings, ctx)` → Summen.
- optional `expand` + `expandColumns` (Detailexport, z. B. ein Datenpunkt je Zeile) und `example` (Beispielprojekt als
  Tab-getrennte Tabelle mit Eingabe-IDs als Kopf – wird über `importTable` geladen und so gleich mitgetestet).
- Die Vertragstests in `tests/test_lists.cpp` prüfen Texte, Standardzeile und Beispielprojekt automatisch.

## Qt-6.4-Hinweis (Layouts und Repeater)

In Qt 6.4 positioniert ein `ColumnLayout` Kinder eines `Repeater`, die erst *nach* dem Erzeugen der Seite entstehen,
neben einem Geschwister mit wechselnder Sichtbarkeit nicht mehr (Breite 0, alle bei y = 0) – und bringt dabei das
ganze Seitenlayout durcheinander. Die Zeilenkarten stehen deshalb in einem `Column`-Positionierer mit
`width: parent.width` (siehe `ProjectListPage.qml`). Für wechselnde Listen im Zweifel `Column` statt `ColumnLayout`.
Die Ergebniszeilen (`ResultView.qml`) und das Eingabeformular (`InputForm.qml`) sind deshalb ebenfalls ohne Layouts
aufgebaut (Column + Anker): umbrechende Beschriftungen und Hinweise in `GridLayout`-Zeilen bekamen sonst gelegentlich
eine falsche Höhe und überlappten.

## Bereiche und Gruppen (zentrales Layout ab 1.9)

`catalog/layout.cpp` → `areas()` ist die einzige Stelle, die festlegt, **wo** ein Eintrag in der App steht: Liste der
Bereiche (`Category`: ID, Titel, Kürzel) mit Gruppen (`CategoryGroup{title, items}`) in Anzeigereihenfolge. Die Registry
setzt beim Aufbau `category` jedes Rechners, jeder Tabelle und Projektliste aus diesem Layout (`assignAreas()`), damit
ein Thema nur durch Verschieben seiner ID den Bereich wechselt. Bereiche: Projektlisten, Elektrotechnik, Motoren &
Antriebe, Elektronik & Bauteile, Gebäudeautomation & MSR, HLKS, Explosionsschutz (ATEX), R&I-Kennzeichnung, Reinraum &
Pharma, Rechenzentrum, Photovoltaik & Speicher, Gebäude/Brandschutz/Akustik, Kommunikation & Pinbelegungen, Umrechnung
& Einheiten.

Regeln (Test `tests/test_electrical.cpp`, Tag `[layout]`): jede ID existiert, steht **genau einmal** in der ganzen App,
jeder Eintrag steht in seinem Bereich; Bereiche mit mehreren Gruppen brauchen Gruppentitel. `categoryGroups(id)` liefert
die Gruppen; `CalcBridge::groups()` und `ListPage.qml` zeigen ab zwei Gruppen aufklappbare Überschriften, sonst eine
einfache Liste. Die CLI (`elcalc list`, `elcalc markdown`) folgt derselben Reihenfolge.

In 1.9 wurden doppelte Themen zusammengeführt (z. B. Strom/Spannung/Leistung/Motorstrom → `power` mit Auswahl
„Gesucht“, Farbcode beide Richtungen → `resistor_color`, h,x-Diagramm mit Taupunkt-Eingabe statt „Feuchte Luft“).
`currentItemId()` bildet alte IDs auf die neuen ab, `currentAreaIds()` alte Kategorien auf neue Bereiche; `CalcBridge`
migriert damit gespeicherte Favoriten und ausgeblendete Bereiche beim Start.

Diagramme können logarithmische Achsen haben (`Chart::xLog/yLog`, z. B. Auslösekennlinien); `ChartView.qml` zeichnet
dann Dekaden-Teilung und -Gitter.

Zeichnungen der Elektro-Rechner nutzen die gemeinsame Hilfe `catalog/sketch.hpp` (`Sketch`: Linien, Rechtecke, Spulen-
/Widerstandskörper `bar`, Kreise, Beschriftungen) und die Stile `fill:#RRGGBB` (Farbringe) und `line:#RRGGBB`
(Kupferbrücken); Beschriftungen „l“/„r“ links-/rechtsbündig. Thermoelemente: Koeffizienten in
`formulas/thermocouple_nist.hpp`, aus den NIST-ITS-90-Referenzfunktionen erzeugt (nicht von Hand ändern).

## Bilder zu Tabellen (ab 1.10)

`Table::drawing` (optional `Chart`, `diagram = true`) wird beim Aufbau der Registry aus `catalog/tableDrawing()`
(`catalog/pinout_drawings.cpp`) gesetzt und von `CalcBridge::table()` als `drawing` an `TablePage.qml` gegeben (Karte „Bild“
über der Tabelle, gleiche `ChartView` wie bei Rechnern). Die Pin-Farben werden aus den Tabellenzeilen abgeleitet (Versorgung,
Masse/Schirm, Signal, frei bzw. echte Aderfarben); ein Pin, den das Bild zeigt, der aber in der Tabelle fehlt, erscheint als „?“
– der Test `[pinout]` schlägt dann fehl. Seitenrichtige Ansichten nur, wo die Ansicht belegt ist; sonst Hinweis
„Kontaktfolge schematisch“ in `note`.

Tabellen können ihr Bild auch selbst setzen (`t.drawing = …` beim Erzeugen); `tableDrawing()` füllt nur Tabellen ohne Bild.
So machen es die Überspannungsschutz-Tabellen (ab 1.11): Bilder in `catalog/surge_drawings.cpp` (Blitzschutzzonen, Schutzkette,
Schaltbilder 4+0/3+1, Überspannungskategorien, Rechenzentrum, Ex-i-Messkreis), die auch die Rechner `spd_power`, `spd_signal`,
`spd_ex`, `lemp_loop` und `spd_distance` über `c.chart` nutzen. Beschriftungen stehen in der Tintenfarbe, Zonen als farbige
Umrisse (`line:#…`) – so bleibt das Bild im hellen und dunklen Modus lesbar.

Kabeltrasse: Projektliste `lists/cabletray.cpp` mit Kabeldaten aus `formulas/cabledata.cpp` (Textsuche „NYM-J 5x2,5“).
Motorabgang: `catalog/motor_feeder.cpp` mit Eaton-Richtwerten (`motors::motorGuide`, `mpcbRange`, `tripClassFor`).
Überspannungsschutz: `catalog/surge.cpp` (Bereich `surge`).

## Bereiche ein-/ausblenden (ab 1.6)

`CalcBridge::hiddenCategories` (QSettings-Schlüssel `hiddenCategories`) enthält die ausgeblendeten Kategorie-IDs;
`HomePage.qml` filtert die Kacheln, `CategoryToggles.qml` (Schalter je Kategorie) steckt im Dialog „Anpassen“ und in
den Einstellungen. Die Suche bleibt bewusst ungefiltert.

## Bekannte Grenzen / Ideen für Version 2

- Strombelastbarkeit nur Kupfer, 1,5–300 mm², Verlegearten A1–E (keine Einleiter F/G, kein Aluminium).
- Kurzschlussberechnung vereinfacht (ohne K_T, ohne Motorbeiträge) – für Planung Netzberechnungsprogramm nutzen.
- Abschaltbedingung mit konventioneller Methode; Sicherungen über manuell eingegebenes I_a.
- Projektlisten: eine Liste je Typ (noch keine Projektverwaltung); Spannungsfall je Abgang, nicht über mehrere Abschnitte.
- Ideen: Elektrische Symbole, Selektivität, RCD-Typen, Spannungsfall mehrerer Abschnitte, PDF-Export,
  mehrere Projekte, NIN-spezifische Tabellen, Aluminium-Belastbarkeit, Kabelrinnen-Füllung.

## Testen der Oberfläche ohne Bildschirm

Für automatische Screenshots (z. B. CI) liest die App Umgebungsvariablen:
`ELC_OPEN=<rechner-id|table-id|list-id|list-id#zeile|list-id!settings|cat:<kategorie>|settings>`, `ELC_LANG=de|en`,
`ELC_THEME=dark|light`, `ELC_WINDOW_SIZE=1100x760`, `ELC_SCREENSHOT=datei.png` (speichert nach 1,5 s bzw.
`ELC_SCREENSHOT_DELAY` ms und beendet). QML-Warnungen erscheinen mit `QT_FORCE_STDERR_LOGGING=1`.
Im Offscreen-Bild fehlen Elemente mit Material-Schatten (`layer.effect`), z. B. die Knöpfe der Schalter und der
Standard-Hintergrund von Dialogen (auf dem Gerät vorhanden) – Dialoge bekommen deshalb einen eigenen `background`.

```bash
QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software ELC_OPEN=voltage_drop ELC_SCREENSHOT=vd.png ./ElektroRechner
```
