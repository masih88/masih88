# CLAUDE.md – Hinweise für KI-Assistenten

Projekt: ElektroRechner – C++20-Rechenkern (`core/`, ohne Qt) + Qt 6 Quick App (`app/`) für Windows/Android,
CLI `elcalc` (`cli/`), Catch2-Tests (`tests/`). Architektur: `docs/ARCHITEKTUR.md`.

## Befehle

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DELC_SANITIZE=ON -DELC_BUILD_APP=OFF   # Kern + Tests
cmake --build build && ./build/tests/elc_tests
./build/cli/elcalc run <id> key=value ...
./build/cli/elcalc markdown > docs/BERECHNUNGEN.md && ./build/cli/elcalc markdown --lang en > docs/CALCULATIONS.md
```

## Regeln

- Neue Rechner datengetrieben in `core/src/catalog/*.cpp`, Formeln als reine Funktionen in `core/src/formulas/`
  mit Test (Referenzwert aus Norm/Handrechnung). QML nur ändern, wenn sich das Seitenlayout ändern soll.
- Jeder Text als `Text{de, en}`; deutsche Texte mit korrekten Umlauten. Einheiten als SI-Symbole.
- Keine locale-abhängigen Funktionen (`printf`, `strtod`, `stod`) – `elc::formatNumber` / `elc::parseNumber` nutzen.
- Normwerte immer mit Quelle in `description` und „gegen gültige Ausgabe prüfen“; keine Werte raten.
- Warnungsfrei bauen (`-Wall -Wextra -Wpedantic -Wshadow`), Tests unter ASan/UBSan grün halten.
- Nach Änderungen an Rechnern die Doku in `docs/` neu erzeugen.
