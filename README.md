# ElektroRechner

Elektro-, Gebäudeautomations- und Industrie-Rechner für **Windows und Android** – ein C++20-Rechenkern mit einer
Qt-6-Quick-Oberfläche. Inspiriert von „Elektro-Berechnungen“, aber mit Schwerpunkt auf Planung in
**Gebäudeautomation (GA/MSR)** und **Industrie**, umschaltbar **Deutsch/Englisch** und **DE (DIN VDE) / CH (NIN)**.

| Start | Spannungsfall | Dunkles Design | Tabelle |
|---|---|---|---|
| ![Start](docs/screenshots/start.png) | ![Spannungsfall](docs/screenshots/spannungsfall.png) | ![Leitungsdimensionierung](docs/screenshots/leitungsdimensionierung-dunkel.png) | ![RJ45](docs/screenshots/tabelle-rj45.png) |

Auf breiten Fenstern (Windows, Tablet) stehen Eingaben und Ergebnisse nebeneinander:

![Desktop](docs/screenshots/desktop-zweispaltig.png)

> **Ohne Gewähr.** Die Ergebnisse ersetzen keine normgerechte Planung und Prüfung durch eine Elektrofachkraft.
> Tabellen- und Grenzwerte vor Verwendung gegen die gültige Normausgabe prüfen.

---

## Funktionsumfang (Version 1.0)

**48 Rechner und 30 Tabellen** in 8 Kategorien. Alle Formeln, Eingaben und Quellen stehen in
[docs/BERECHNUNGEN.md](docs/BERECHNUNGEN.md) (englisch: [docs/CALCULATIONS.md](docs/CALCULATIONS.md)).

| Kategorie | Inhalt |
|---|---|
| **Grundlagen** | Ohmsches Gesetz (beliebige 2 von U/I/R/P), Leistung P/Q/S/cos φ, Strom aus Leistung, Leiterwiderstand mit Temperatur, R/L- und C-Reihen-/Parallelschaltung, Energiekosten (€/CHF nach Normprofil) |
| **Leitungen & Schutz** | Leitungsdimensionierung (Belastbarkeit + Spannungsfall + Schutzorgan in einem Schritt), Spannungsfall mit Grenzwert DE/CH, Strombelastbarkeit I_z (Verlegearten A1–E, PVC/VPE, Temperatur, Häufung), Überlastschutz I_B ≤ I_n ≤ I_z, Abschaltbedingung/L_max (TN), max. Länge für 24-V-Kreise, Kurzschlussfestigkeit I²t, Rohrfüllung |
| **Motoren & Antriebe** | Bemessungsstrom, Drehmoment/Leistung/Drehzahl, Synchrondrehzahl & Schlupf, Wirkungsgrad, Stern-Dreieck-Anlauf, Steinmetzschaltung |
| **Industrie & Netze** | Blindleistungskompensation, Transformator (Ströme, I_k), Kurzschlussstrom am Leitungsende (I_k3 max / I_k1 min nach IEC 60909, vereinfacht), Schaltschrank-Wärmebilanz (IEC/TR 60890) |
| **Gebäudeautomation & MSR** | Analogsignal-Skalierung 4–20 mA/0–10 V ↔ Messwert ↔ S7-Rohwert (27648) mit NAMUR NE 43, 4–20-mA-Bürde, Pt100/Pt1000/Ni1000/LG-Ni1000, Leitungsfehler 2-Leiter, NTC, 24-V-Trafo/-Netzteil, Ventil kv/kvs & Autorität, Wärmeleistung ↔ Volumenstrom (Wasser/Luft), DALI-Leitung |
| **Elektronik** | Farbcode 4/5 Ringe, E-Reihen E6–E96, LED-Vorwiderstand, Spannungsteiler, RC-Glied, LC-Schwingkreis, Impedanz RLC, Akku-/USV-Laufzeit |
| **Umrechner** | Leistung (W, PS, hp, BTU/h, kcal/h, TR), Energie, Temperatur, Druck (bar, psi, mWS), Volumenstrom (m³/h, l/s, CFM), AWG ↔ mm² |
| **Tabellen & Pinouts** | IP-Schutzarten, Schutzklassen, Netzsysteme, Aderfarben, Spannungsfall-Grenzwerte DE/CH, Strombelastbarkeit (4 Tabellen), Temperatur- und Häufungsfaktoren, LS-Charakteristiken, D-Sicherungen, Kabeltypen, **Feldbusse** (BACnet MS/TP, Modbus, KNX, DALI, LON, PROFIBUS, CAN, IO-Link, Ethernet), Isolierstoffklassen, Betriebsarten, IE-Klassen, Klemmbrett Y/Δ, Bauformen, Pinouts RJ45/PoE, RS-485, M12, KNX, RS-232, USB, HDMI, Farbcode, SI-Vorsätze |

Weitere Funktionen: Live-Berechnung bei jeder Eingabe, Ergebnisleiste mit Ampelstatus, Suche, Favoriten,
Ergebnis kopieren (für Berichte/E-Mails), dunkles Design, Dezimalkomma oder -punkt.

---

## Projekt bauen

### Voraussetzungen

| Werkzeug | Version | Hinweis |
|---|---|---|
| Qt | **6.8 LTS empfohlen** (mind. 6.4) | Qt Online Installer: „Qt 6.8 → MSVC 2022 64-bit“ oder „MinGW“, für Android zusätzlich „Android“ |
| Qt Creator | aktuell | wird mit Qt installiert |
| CMake / Ninja | ≥ 3.21 | im Qt-Installer unter „Developer and Designer Tools“ |
| Für Android | JDK 17, Android SDK + NDK | Qt Creator lädt SDK/NDK unter *Einstellungen → Geräte → Android* („Set Up SDK“) |
| Git für Windows | aktuell | https://git-scm.com – das Repository ist bereits initialisiert |

### Windows (Qt Creator)

1. Qt Creator öffnen → **Datei → Datei oder Projekt öffnen** → `CMakeLists.txt` im Projektordner wählen.
2. Kit **„Desktop Qt 6.8 MSVC2022 64bit“** (oder MinGW) auswählen → *Projekt konfigurieren*.
3. Links unten Ziel **ElektroRechner** wählen → **Ausführen** (Strg+R).
4. Weitergeben ohne Qt-Installation: Release bauen und im Build-Ordner
   `windeployqt --qmldir ..\app\qml ElektroRechner.exe` ausführen (Qt-Eingabeaufforderung). Den Ordner kann man zippen.

### Android (APK)

1. In Qt Creator *Bearbeiten → Einstellungen → Geräte → Android*: JDK 17 und SDK einrichten („Set Up SDK“ lädt das passende NDK).
2. Unter *Projekte* das Kit **„Android Qt 6.8 Clang arm64-v8a“** hinzufügen.
3. Smartphone per USB verbinden (Entwickleroptionen → USB-Debugging) → **Ausführen**. Die App wird installiert und gestartet.
4. Signierte Release-APK/AAB: *Projekte → Build → Build Android APK* → Keystore anlegen/auswählen → *Sign package*.
   Optional *Create Templates* für eigenes App-Icon und Paketnamen (`AndroidManifest.xml`).

### Rechenkern, Tests und Kommandozeile (ohne Qt)

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DELC_BUILD_APP=OFF
cmake --build build
ctest --test-dir build --output-on-failure        # 52 Testfälle, > 5000 Prüfungen

# mit AddressSanitizer + UBSan (Linux/macOS)
cmake -S . -B build-asan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DELC_SANITIZE=ON -DELC_BUILD_APP=OFF
cmake --build build-asan && ./build-asan/tests/elc_tests
```

Kommandozeilen-Tool `elcalc` (praktisch für Prüfrechnungen und Skripte):

```text
elcalc list                                   # alle Rechner und Tabellen
elcalc show voltage_drop                      # Eingaben, Ergebnisse, Formeln
elcalc run cable_sizing IB=32 L=85 method=E   # rechnen (fehlende Werte = Standardwerte)
elcalc run voltage_drop --norm ch --lang en   # Schweizer Grenzwert, englische Ausgabe
elcalc table fieldbuses                       # Tabelle ausgeben
elcalc markdown > docs/BERECHNUNGEN.md        # Formel-Doku neu erzeugen
```

CMake-Optionen: `ELC_BUILD_APP`, `ELC_BUILD_CLI`, `ELC_BUILD_TESTS` (je `ON`), `ELC_SANITIZE` (`OFF`).

---

## Projektstruktur

```text
ElektroRechner/
├── CMakeLists.txt            Gesamtprojekt (C++20, Warnungen, Sanitizer-Option)
├── core/                     Rechenkern ohne Qt – auch für andere Oberflächen nutzbar
│   ├── include/elc/          types.hpp (Datenmodell), registry.hpp, formulas/*.hpp
│   └── src/
│       ├── formulas/         reine Formeln (getestet): basics, cables, motors, automation, electronics, units
│       ├── catalog/          Rechner-Definitionen (Eingaben, Ergebnisse, Texte DE/EN, Prüfungen)
│       └── tables/           Nachschlagetabellen
├── app/                      Qt-6-Quick-App (Windows/Android)
│   ├── CalcBridge.*          Brücke C++ ↔ QML (Sprache, Normprofil, Favoriten, Formatierung)
│   └── qml/                  Main, HomePage, ListPage, CalculatorPage (generisch), TablePage, SettingsPage
├── cli/                      Kommandozeilen-Tool elcalc
├── tests/                    Catch2-Tests (Formeln + Vertragstests für jeden Rechner)
└── docs/                     BERECHNUNGEN.md, CALCULATIONS.md, ARCHITEKTUR.md, Screenshots
```

Neuen Rechner hinzufügen: siehe [docs/ARCHITEKTUR.md](docs/ARCHITEKTUR.md#neuen-rechner-hinzufügen) –
eine Funktion in `core/src/catalog/*.cpp`, die Oberfläche entsteht automatisch.

## Normen & Quellen (Auswahl)

IEC 60364-5-52 / DIN VDE 0298-4 / NIN 5.2.3 (Strombelastbarkeit), DIN VDE 0100-520 Anh. G, DIN 18015-1, NIN
(Spannungsfall), IEC 60364-4-41/-43 (Schutz), IEC 60898-1, IEC 60269-1, IEC 60909-0, IEC/TR 60890, IEC 60751,
DIN 43760, IEC 62386 (DALI), IEC 60529, IEC 61140, IEC 60445, IEC 60204-1, IEC 60034-1/-7/-8/-30-1, IEC 60062,
IEC 60063, TIA-568, Modbus over Serial Line V1.02, NAMUR NE 43.

## Lizenz

Noch nicht festgelegt (privates Projekt). Vor einer Veröffentlichung Lizenz wählen; Qt wird unter LGPLv3 genutzt.
