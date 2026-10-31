# Architektur & Entwurfsentscheidungen

Stand: 31.10.2026 · Version 1.0.0

## Ziel

Eine Rechner-App für Elektroplanung mit Schwerpunkt Gebäudeautomation (GA/MSR) und Industrie, die auf
**Windows** und **Android** läuft, **Deutsch/Englisch** umschaltet und Grenzwerte für **Deutschland (DIN VDE)**
und die **Schweiz (NIN)** kennt. Die Berechnungen sollen prüfbar (Tests, Formeltexte, Quellen) und leicht
erweiterbar sein.

## Überblick

```text
┌──────────────────────── app/ (Qt 6 Quick, QML) ───────────────────────┐
│ Main.qml ─ HomePage ─ ListPage ─ CalculatorPage* ─ TablePage ─ Settings │
│             (* generisch: rendert jeden Rechner aus seiner Definition)  │
│                         CalcBridge (Kontext-Property „Calc“)            │
└───────────────────────────────────┬─────────────────────────────────────┘
                                    │ QVariantMap / Strings
┌──────────────────────── core/ (C++20, ohne Qt) ────────────────────────┐
│ registry   → categories(), calculators(), tables(), findCalculator()   │
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
| Rechner-Definition | **Datengetrieben**: jeder Rechner ist Daten + eine `compute`-Funktion | Eine generische QML-Seite für alle 48 Rechner; neue Rechner ohne QML-Änderung |
| Übersetzung | `Text{de, en}` direkt in den Definitionen; UI-Texte in `UiStrings.cpp` | Sprache zur Laufzeit umschaltbar, kein `lrelease`-Schritt nötig |
| Zahlen | Ein-/Ausgabe locale-unabhängig (`iostream` + `std::locale::classic`), Komma und Punkt erlaubt | Qt ruft `setlocale()` auf (Linux/Android) – `printf`/`strtod` würden sonst mit Dezimalkomma brechen |
| Normprofil | `Context{NormProfile}` wird jedem `compute` übergeben | Grenzwerte (3 %/4 %), Währung; weitere länderabhängige Werte leicht ergänzbar |
| Fehler | `evaluate()` validiert Pflichtfelder/Bereiche; `CalcError` für physikalisch Unmögliches | Die UI zeigt verständliche Meldungen statt Abstürzen |
| Sichtbarkeit | `InputDef::showIf(choiceId, {werte})` | Felder erscheinen nur, wenn relevant (z. B. cos φ nicht bei DC) |
| Hervorhebung | `OutputDef::primary()` | Schlüsselergebnisse in der Ergebnisleiste (Handy) |
| Tests | Formeltests mit Normwerten + **Vertragstests** für jeden Rechner/jede Tabelle | Jeder neue Rechner wird automatisch auf Vollständigkeit (DE/EN), Standardwerte und endliche Ergebnisse geprüft |
| Qualität | `-Wall -Wextra -Wpedantic -Wshadow`, ASan/UBSan-Option, MSVC `/W4 /utf-8` | Vorgaben C++ Core Guidelines; `/utf-8` nötig wegen Ω, φ, ² in Strings |

## Datenmodell (core/include/elc/types.hpp)

- `Text` – Deutsch + Englisch (ein String = beide Sprachen).
- `InputDef` – Zahl oder Auswahl; Einheit, Standardwert, optional, Bereich (`positive()`, `range()`), `showIf()`, Hinweis.
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
    c.category = "automation";              // basics, cables, motors, industry, automation, electronics, converters
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

3. Bauen und `elc_tests` ausführen – die Vertragstests prüfen den neuen Rechner automatisch.
4. `elcalc markdown > docs/BERECHNUNGEN.md` und `elcalc markdown --lang en > docs/CALCULATIONS.md` neu erzeugen.

Tabellen: `make(...)` in `core/src/tables/tables.cpp`, in `addTables()` registrieren.

## Bekannte Grenzen / Ideen für Version 2

- Strombelastbarkeit nur Kupfer, 1,5–300 mm², Verlegearten A1–E (keine Einleiter F/G, kein Aluminium).
- Kurzschlussberechnung vereinfacht (ohne K_T, ohne Motorbeiträge) – für Planung Netzberechnungsprogramm nutzen.
- Abschaltbedingung mit konventioneller Methode; Sicherungen über manuell eingegebenes I_a.
- Ideen: Elektrische Symbole, Selektivität, RCD-Typen, Spannungsfall mehrerer Abschnitte, PDF-Export,
  Projekte/Speichern von Eingaben, NIN-spezifische Tabellen, Aluminium-Belastbarkeit, Kabelrinnen-Füllung.

## Testen der Oberfläche ohne Bildschirm

Für automatische Screenshots (z. B. CI) liest die App Umgebungsvariablen:
`ELC_OPEN=<rechner-id|table-id|cat:<kategorie>|settings>`, `ELC_LANG=de|en`, `ELC_THEME=dark|light`,
`ELC_WINDOW_SIZE=1100x760`, `ELC_SCREENSHOT=datei.png` (speichert nach 1,5 s und beendet).

```bash
QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software ELC_OPEN=voltage_drop ELC_SCREENSHOT=vd.png ./ElektroRechner
```
