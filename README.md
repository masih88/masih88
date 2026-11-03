# ElektroRechner

Elektro-, Gebäudeautomations- und Industrie-Rechner für **Windows und Android** – ein C++20-Rechenkern mit einer
Qt-6-Quick-Oberfläche. Inspiriert von „Elektro-Berechnungen“, aber mit Schwerpunkt auf Planung in
**Gebäudeautomation (GA/MSR)** und **Industrie**, umschaltbar **Deutsch/Englisch** und **DE (DIN VDE) / CH (NIN)**.

**Neu in 1.11 – Überspannungsschutz für Rechenzentren und Ex-Bereiche, mit Bildern:**
- **Bilder im Bereich „Überspannungs- & Blitzschutz“:** Blitzschutzzonen LPZ 0A–3 mit Ableiterpositionen, Schutzkette
  Typ 1+2 → 2 → 3, Schaltbilder 4+0/3+1/3+0 (Varistor, Funkenstrecke), Überspannungskategorien, Rechenzentrum mit nummerierten
  Schutzpunkten, eigensicherer Messkreis am Tank; die Rechner zeigen Schaltung, Leitungsweg, Schleife bzw. Schutzbereich.
- **Überspannungsschutz im Ex-Bereich (eigensicherer Messkreis):** Zündschutzart je Zone (ia/ib/ic), Prüfung U_i ≥ U_o und
  I_i ≥ I_o, Erdfreiheit (500 V AC), 10 × 10 kA, Leitung Ableiter → Feldgerät ≤ 1 m geschirmt – nach DEHN WPX035.
- **Rechenzentrum:** Schutzvorschlag je Anwendung (NSHV, Notstrom, Server, BMA, IP-Kameras, PV, Dachaufbauten) mit Typ und
  Art.-Nr., Planungsgrundsätze (LPL I, Maschen 5 × 5 m, Ringerder, Raumschirmung) – nach DEHN WPX050.
- **Kenndaten der Komponenten:** DEHNvenCI, DEHNventil, DEHNguard ACI, DEHNrail, DEHNcube, BLITZDUCTORconnect BE/BD/BD HF,
  DEHNpatch und BXT ML4 BD EX 24 mit U_C, I_imp, I_n, U_p, Vorsicherung und Grenzfrequenz; Ableitvermögen nach VdS 2833.
- **Induzierte Spannung in Installationsschleifen** (U = M · Δi/Δt, Beispiel 720 kV) und I_imp je Ader für Datenleitungen.

| Ex-Bereich: eigensicherer Messkreis | Rechenzentrum: Schutzvorschlag |
|---|---|
| ![Ex-Bereich](docs/screenshots/ueberspannung_ex.png) | ![Rechenzentrum](docs/screenshots/ueberspannung_rz.png) |

**Neu in 1.10 – Pinbelegungen mit Bildern, Kabeltrasse, Motorabgang und Überspannungsschutz:**
- **Bilder für alle 32 Pinbelegungen:** Steckeransichten (RJ45/PoE mit T568B-Aderfarben, Telefon-RJ, D-Sub 9, VGA, M12 Stecker
  und Buchse, MIDI, USB-C, OBD II, SIM, Raspberry Pi, LCD 16×2) bzw. Kontaktfolgen (HDMI, DisplayPort, DVI, SCART …); die
  Pins sind nach Funktion eingefärbt (Versorgung, Masse/Schirm, Signal, frei) – direkt aus der Tabelle, also immer stimmig.
- **Kabeltrasse (Projektliste):** Kabel eintragen (z. B. „NYM-J 5x2,5“) → Normbreite mit Reserve, Füllgrad, Kabelgewicht und
  Last je m, Bemessungslast VDE 0639, Häufungsfaktor, Hinweis Trennsteg. Kabeldaten (d, kg/km) für NYM, NYY-J, J-Y(St)Y, LiYCY
  und Cat 7 aus Herstellerdatenblättern.
- **Motorabgang für Motoren, Pumpen und Ventilatoren:** Direkt, Stern-Dreieck, Sanftanlasser, FU oder EC → Motorschutzschalter
  mit Einstellbereich, Vorsicherung, Schütze (AC-3, Y-Δ 0,58/0,33·I_N), Anlaufgerät/FU, Leitung, Reparaturschalter, RCD-Typ,
  Auslöseklasse und Zubehör – nach Eaton Schaltungsbuch; Entrauchungsventilatoren mit Sonderhinweisen.
- **Neuer Bereich „Überspannungs- & Blitzschutz“** (an DEHN orientiert): Auswahl für 230/400 V je Einbauort und
  Blitzschutzzone (Typ 1+2/2/3, Schaltung 3+0/4+0/3+1, U_C, I_imp aus der Blitzstromaufteilung, U_p), MSR/Sensoren/Bus/
  Ethernet (U_C-Klasse, D1/C2, Pt100-Messfehler durch den Ableiter), Schutzbereich (U_p,f, l_po) und Tabellen zu Zonen,
  SPD-Typen, Schaltungen, Überspannungskategorien, Blitzschutzklassen und Prüfkategorien.

| Überspannungs- & Blitzschutz | Pinbelegung mit Bild |
|---|---|
| ![Überspannungsschutz](docs/screenshots/ueberspannung.png) | ![M12](docs/screenshots/pinbild_m12.png) |

**Neu in 1.9 – aufgeräumt und nach Fachbereichen geordnet.** Jedes Thema steht jetzt **genau einmal** in der App;
doppelte Rechner wurden zur jeweils besten Fassung zusammengeführt, und alles ist in 14 Bereiche sortiert
(ATEX-Themen im Bereich „Explosionsschutz“, HLKS-Themen im Bereich „HLKS“ usw.):
- **Neu:** Bereich **„Kommunikation & Pinbelegungen“** mit 25 neuen Pinbelegungen (PoE, RJ-11/12/45-Telefon, SCART, VGA,
  DVI, DisplayPort, Thunderbolt, Lightning, Apple-Dock, FireWire, PS/2, Molex/SATA, SD-/SIM-Karte, Raspberry Pi, LCD 16×2,
  LED, Autoradio ISO 10487, OBD II, XLR/DMX, MIDI, Klinke, Cinch-Farben, LWL-Farbcode, IO-Link); Umrechnung um
  **Winkel/Trigonometrie**, **var ↔ µF** und **Drehzahl** ergänzt.
- **Zusammengeführt:** „Leistung, Strom und Spannung“ (ersetzt Strom aus Leistung, Spannungsberechnung, Leistung P/Q/S,
  Motorstrom, Motorleistung, Motorspannung), Leistungsfaktor mit η (ersetzt Motor-cos φ), **Farbcode in beide
  Richtungen** mit Zeichnung, Widerstandsthermometer Pt/Ni/**Cu**, Leiterwiderstand in „Leiterwiderstand, Reaktanz und
  Impedanz“, **h,x-Diagramm mit Taupunkt-Eingabe** (ersetzt „Feuchte Luft“), Vorwiderstand für LED und Verbraucher,
  IP-Code als eine Tabelle, D-Sicherungen mit Kennmelderfarben, NH-Größen, AWG-Tabelle mit kcmil, Aderfarben inkl. PEN
  und Maschinenverdrahtung. Gespeicherte Favoriten werden automatisch auf die neuen Einträge umgestellt.

| 14 Bereiche | Kommunikation & Pinbelegungen | Leistung, Strom und Spannung |
|---|---|---|
| ![Bereiche](docs/screenshots/bereiche_v19.png) | ![Pinbelegungen](docs/screenshots/pinbelegungen.png) | ![Leistung, Strom und Spannung](docs/screenshots/leistung_strom_spannung.png) |

**Neu in 1.8 – Kapitel „Ressourcen“** im Bereich Elektrotechnik (alle 27 Punkte des Reiters „Ressourcen“ der App
„Elektroberechnungen“): Sicherungs-Kennzeichnung und UL/CSA-Klassen, E-Reihen, **Auslösekennlinien** (Diagramm mit
logarithmischen Achsen), Reaktanzbeläge, spezifische Widerstände, Spannungsfall je A·m, Leiterdaten, **Netzsysteme TN/TT/IT
als Schaltbild**, IP/IK/NEMA, ATEX, Schutzklassen, CCTV-Auflösungen, Thermoelement-Farben, ANSI-Gerätenummern,
Geräteabkürzungen, **Schaltzeichen nach IEC 60617** (5 Seiten), Netzspannung und Stecker weltweit, Steckertypen A–O,
IEC 60320, CEE (IEC 60309), NEMA-Stecker, E-Auto-Ladestecker, Aderfarben international, SI-Vorsätze und Einheiten,
Rohrabmessungen und Kabelverschraubungen.

| Ressourcen | Auslösekennlinien | Schaltzeichen |
|---|---|---|
| ![Ressourcen](docs/screenshots/ressourcen.png) | ![Auslösekennlinien](docs/screenshots/ausloesekennlinien.png) | ![Schaltzeichen](docs/screenshots/schaltzeichen.png) |

**Neu in 1.7 – Bereich „Elektrotechnik“** (nach der Menüliste der App „Elektroberechnungen“, Screenshots per
doc2md/OCR erfasst): eine Kachel mit 10 aufklappbaren Gruppen und über 100 Einträgen – Strom/Spannung/Leistung,
Leitungen & Schutz, Sicherungen, Kurzschluss/Erdung/Überspannung, Kompensation, Bauteile & Codes, Schaltungen,
Transformator/Antenne/CCTV/Sensoren, Motor und Umrechnung. Vorhandene Rechner erscheinen dort mit, neu sind u. a.:
- **Leitungen:** mineralisolierte Kabel, Kupfer-Sammelschienen, Kabel-R/X/Z, Kabeltemperatur, Verlustleistung,
  Neutralleiterstrom mit 3. Oberschwingung, Spannungsfall und Querschnitt bei verteilten Lasten, k²S²-Tabelle.
- **Schutz:** minimaler Kurzschlussstrom, Erdung/RCD (TT/TN), Überspannungs-Risikobewertung (CRL nach 443),
  Sicherungs-Tabellen (Betriebsklassen, D/NH, Zylinder, Feinsicherungen, Kfz).
- **Bauteile:** Farbringe aus Wert (mit Zeichnung), SMD-Code inkl. EIA-96, Kondensator- und Induktivitätscode,
  Stromteiler, Z-Diode, Vorwiderstand, Trafowicklung, Antennenlänge, CCTV-Speicher, Thermoelemente (NIST ITS-90), Cu-Fühler.
- **Motor:** Leistung/Spannung/cos φ, NEC-Volllaststrom, **Schaltschemata mit 6, 9 und 12 Klemmen** (Klemmbrett und
  Wicklungsbild), Drehrichtung, Klemmenkennzeichnung.
- **Umrechnung:** Dreieck-Stern (mit Zeichnung), AWG-/SWG-Tabellen, mm² ↔ AWG/kcmil, Querschnitt, Länge, Spannung (dB), Phasenwinkel.

| Elektrotechnik | Klemmbrett 6 Klemmen | Farbringe |
|---|---|---|
| ![Elektrotechnik](docs/screenshots/elektrotechnik.png) | ![Motor 6 Klemmen](docs/screenshots/motor-6-klemmen.png) | ![Farbcode](docs/screenshots/farbringe.png) |

**Neu in 1.6:**
- **Hydraulische Grundschaltungen (Regelgruppen)** als Anleitung in *Gebäudetechnik & Energie*: acht Schaltungen
  (Drossel, Drossel mit PICV, Einspritz mit Durchgangsventil / PICV / Dreiwegventil, Beimisch, Beimisch mit fester
  Vormischung, Umlenk) mit **Prinzipskizze**, Funktionsweise, Eigenschaften, Einsatzbereich und Ventilauslegung.
  **Auswahlhilfe:** Verteiler, Einfriergefahr, Entfeuchtung, Rücklauf- und Vorlauftemperatur angeben → empfohlene
  Schaltung mit Begründung und Alternative. Dazu die Tabelle **Auswahl hydraulischer Grundschaltungen**.
  Quellen: Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022) Kap. 5.4–5.5, Belimo-Auswahltabelle (03/2023);
  die Skizzen sind eigene Zeichnungen.
- **Bereiche ein- und ausblenden:** Auf der Startseite „Anpassen“ (oder in den Einstellungen) – ausgeblendete Bereiche
  verschwinden von der Startseite, die Suche findet weiterhin alle Rechner. Die Auswahl bleibt gespeichert.

| Beimischschaltung | Bereiche anpassen |
|---|---|
| ![Hydraulische Grundschaltungen](docs/screenshots/hydraulik.png) | ![Bereiche ein-/ausblenden](docs/screenshots/bereiche.png) |

**1.5.1:** Kategorie-Kacheln auf der Startseite lassen sich wieder antippen (Fehler in 1.5.0).

**Neu in 1.5 – aktualisiert auf Basis der Fachbücher** (Siemens-Grundlagenbücher, Haag, KNX, AMEV, TROX, BAuA u. a.;
Verzeichnis und Markdown-Fassungen liegen im Ordner *Fachbücher*). Jeder neue Wert nennt Buch, Ausgabe und Seite:
- **Neue Kategorie Gebäudetechnik & Energie:** GA-Effizienzklasse nach EN ISO 52120 (BAC-Faktoren je Gebäudetyp,
  Einsparung in kWh und €/CHF), **Heizkennlinie** mit Steilheit, Parallelverschiebung und Diagramm, **Wärmepumpe**
  (Carnot-COP, Mindest-COP EN 14511, JAZ), **RLT-Gerät** (Geschwindigkeitsklasse EN 13053, SFP-Klasse, Kanaldurchmesser),
  **Nachhallzeit** nach Sabine mit Soll-Wert DIN 18041 und nötiger Absorberfläche, Schallpegel addieren,
  **Fluchtweg nach ASR A2.3** (aktuelle Fassung 2022 – das Buch enthält noch 2007), **Aufzugsschacht-Rauchabzug** (MBO/VKF).
- **Reglereinstellung** nach Ziegler/Nichols und Chien/Hrones/Reswick mit Schwierigkeitsgrad S = Tu/Tg,
  **KNX-Topologie** (Linien, Bereiche, Segmente, Längen, Netzteillast), **AMEV-Datenpunktadresse** (GA 2005) mit 5 Codetabellen.
- **Pumpe/Ventilator am FU:** Proportionalitätsgesetze mit Diagramm und Energieeinsparung.
- Bestehende Rechner gegen die Bücher geprüft und ergänzt: Ventil (kleinste regelbare Leistung, 1/3-2/3-Ventile),
  Transformator (I_kD, u_k-Richtwerte), Feldbusse (MS/TP, KNX), h,x-Höhentabelle, 5 Sicherheitsregeln & Schutzabstände.

| Heizkennlinie | Reglereinstellung | AMEV-Adresse (dunkel) |
|---|---|---|
| ![Heizkennlinie](docs/screenshots/heizkennlinie.png) | ![Regler](docs/screenshots/reglereinstellung.png) | ![AMEV](docs/screenshots/amev-adresse.png) |

| Gebäudetechnik & Energie | Nachhallzeit |
|---|---|
| ![Kategorie](docs/screenshots/gebaeudetechnik.png) | ![Nachhall](docs/screenshots/nachhall.png) |

![Pumpe/Ventilator am FU](docs/screenshots/fu-pumpe.png)

**Neu in 1.4:**
- **Währung € und CHF** – in den Einstellungen wählbar mit Wechselkurs; jedes Kostenergebnis zeigt zusätzlich den Betrag
  in der anderen Währung, dazu ein Umrechner € ↔ CHF.
- **Explosionsschutz** – Zone → Kategorie/EPL, Stoff → Temperaturklasse und Explosionsgruppe, Staub → zulässige
  Oberflächentemperatur, Geräteprüfung und Kennzeichnungsbeispiel; Tabellen T1–T6, Gruppen, Zonen, Zündschutzarten, Stoffe.
- **R&I-Kennbuchstaben-Finder** – Name eingeben („Temperatur Zuluft“, „Gas Methan“, „Leckage“, „Druckwächter min“) →
  ISA-5.1 (z. B. TT-101), EN 62424/DIN 19227, VDI-3814-Kürzel mit Benutzeradresse, IEC 81346, Signal und PI-Symbol;
  Tabelle mit 78 Sensoren und Wächtern für HLKS, MSR und Prozess sowie die Buchstabentabellen.

| Kennbuchstaben-Finder | Explosionsschutz |
|---|---|
| ![Kennbuchstaben](docs/screenshots/kennbuchstaben-finder.png) | ![Ex-Schutz](docs/screenshots/ex-schutz.png) |

**Neu in 1.3: h,x-Diagramm (Mollier)** mit Taupunkt, Kühlgrenze und Zustandsänderungen (Heizen, Kühlen/Entfeuchten,
Dampf- und Verdunstungsbefeuchten, Mischen) – ins Diagramm tippen setzt Zustand 1. Dazu die **PUE-Bilanz** für
Rechenzentren aus den Verbrauchern (IT, USV, Verteilung, Kühlung mit EER und freier Kühlung) mit Jahresverbrauch,
Kosten, CO₂, Abwärme/ERF und EnEfG-Prüfung (geltende Fassung oder Kabinettsentwurf 2026).

| h,x-Diagramm (Desktop) | h,x-Diagramm (dunkel) | PUE-Bilanz |
|---|---|---|
| ![h,x-Diagramm](docs/screenshots/hx-diagramm-desktop.png) | ![h,x dunkel](docs/screenshots/hx-diagramm-dunkel.png) | ![PUE](docs/screenshots/pue-bilanz.png) |

**Neu in 1.2: Projektlisten** – Verbraucherliste mit Kabelauslegung (Rechenweg der Excel-Vorlage
„Leistungsdaten_Kabelauslegung“) und GA-Datenpunktliste nach VDI 3814, mit Excel-Import/-Export:

![Verbraucherliste](docs/screenshots/verbraucherliste-desktop.png)

| GA-Datenpunktliste | Verbraucher bearbeiten |
|---|---|
| ![GA-Datenpunktliste](docs/screenshots/ga-datenpunktliste.png) | ![Zeile](docs/screenshots/verbraucher-zeile.png) |

| Start | Spannungsfall | Dunkles Design | Tabelle |
|---|---|---|---|
| ![Start](docs/screenshots/start.png) | ![Spannungsfall](docs/screenshots/spannungsfall.png) | ![Leitungsdimensionierung](docs/screenshots/leitungsdimensionierung-dunkel.png) | ![RJ45](docs/screenshots/tabelle-rj45.png) |

| Reinraum: FFU-Decke | Rechenzentrum: USV (dunkel) |
|---|---|
| ![FFU](docs/screenshots/reinraum-ffu.png) | ![USV](docs/screenshots/rechenzentrum-usv-dunkel.png) |

Auf breiten Fenstern (Windows, Tablet) stehen Eingaben und Ergebnisse nebeneinander:

![Desktop](docs/screenshots/desktop-zweispaltig.png)

> **Ohne Gewähr.** Die Ergebnisse ersetzen keine normgerechte Planung und Prüfung durch eine Elektrofachkraft.
> Tabellen- und Grenzwerte vor Verwendung gegen die gültige Normausgabe prüfen.

---

## Funktionsumfang (Version 1.11)

**3 Projektlisten, 124 Rechner und 132 Tabellen** in 15 Bereichen – jedes Thema genau einmal. Alle Formeln, Eingaben und
Quellen stehen in [docs/BERECHNUNGEN.md](docs/BERECHNUNGEN.md) (englisch: [docs/CALCULATIONS.md](docs/CALCULATIONS.md)).

| Bereich | Inhalt |
|---|---|
| **Projektlisten** | **Kabeltrasse** (Breite, Füllgrad, Last, Häufung); **Verbraucherliste & Kabelauslegung**: je Verbraucher S, I_B, Schutzorgan (MSS/SelMod einstellbar, LS B/C, gG mit NH-Größe), F_ges = f1·f2·f3·f4, Querschnitt nach Belastbarkeit, Prüfung I_B ≤ I_N ≤ I_Z (+ I_2 ≤ 1,45·I_Z), Spannungsfall mit R′(70 °C) und X′, Summen je NN/NEA/USV, ELT/MSR, 400/230/24 V. **GA-Datenpunktliste (VDI 3814)**: Bauteil-Vorlagen → AI/AO/BI/BO/COM, Module mit Reserve, Kosten, Benutzeradressen (BAS) |
| **Elektrotechnik** | *Grundlagen:* Leistung/Strom/Spannung (DC, 1~, 3~, Motor mit η), cos φ, Ohm, Impedanz, Betriebsstrom, Joule, Energiekosten, R/C-Schaltungen, Stromteiler, Δ-Y. *Leitungen:* Dimensionierung, Spannungsfall (auch verteilte Lasten), Belastbarkeit (auch MI-Kabel, Sammelschienen), R/X/Z, Temperatur, Verluste, Neutralleiter, I²t, Rohrfüllung; Kabeltabellen und Aderfarben. *Schutz:* Überlast, Abschaltbedingung, Auslösekennlinien, Sicherungen (D, NH, Zylinder, G, Kfz, UL). *Kurzschluss/Erdung/Überspannung, Transformator & Kompensation, Schutzarten (IP/IK/NEMA), Schaltschrank, 5 Sicherheitsregeln, Schaltzeichen, ANSI-Nummern, Netze & Stecker weltweit* |
| **Motoren & Antriebe** | **Motorabgang** (MSS/LS, Schütze, Sanftanlasser/FU, Leitung, Reparaturschalter, RCD) mit Motorstrom-Tabelle; Wirkungsgrad, Drehmoment, Drehzahl & Schlupf, Pumpe/Ventilator am FU, NEC-Volllaststrom; Stern-Dreieck, Steinmetz, Schaltschemata 6/9/12 Klemmen, Drehrichtung, Klemmenbezeichnung; IE-Klassen, Isolierstoffklassen, Betriebsarten, Bauformen |
| **Elektronik & Bauteile** | Farbcode beide Richtungen, E-Reihen, SMD/EIA-96, Kondensator- und Induktivitätscode; Spannungsteiler, Vorwiderstand (LED/Verbraucher), Z-Diode, RC, LC, RLC-Impedanz, Akkulaufzeit |
| **Gebäudeautomation & MSR** | Analogsignal-Skalierung (NAMUR NE 43), Bürde, Pt/Ni/Cu-Fühler, Leitungsfehler, NTC, Thermoelemente (NIST), 24-V-Versorgung; Reglereinstellung (Ziegler/Nichols, CHR), KNX-Topologie, DALI, Feldbusse; AMEV- und VDI-3814-Kennzeichnung; GA-Effizienz EN ISO 52120 |
| **HLKS** | h,x-Diagramm (Mollier) mit Zustandsänderungen und Taupunkt, Außenluft (EN 16798-1/CO₂), RLT-Gerät & Kanal; Wärmeleistung ↔ Volumenstrom, Heizkennlinie, Wärmepumpe; hydraulische Grundschaltungen, Ventil kv/kvs, Rohrabmessungen |
| **Überspannungs- & Blitzschutz** | SPD-Auswahl 230/400 V nach Zone und Netzsystem, MSR/Sensoren/Datenleitungen (mit I_imp je Ader nach VdS 2833), wirksamer Schutzpegel und Schutzbereich, Ex-i-Messkreise, induzierte Schleifenspannung, Risikobewertung (CRL); Tabellen LPZ, SPD-Typen, Schaltungen, Überspannungskategorien, Blitzschutzklassen, Prüfkategorien, Kenndaten DEHN-Komponenten (Energie, MSR/Daten), Rechenzentrum (Schutzvorschlag, Planung), Ex-Bereich, Ableitvermögen – mit Bildern |
| **Explosionsschutz (ATEX)** | Temperaturklasse, Gruppe, Zone/Kategorie/EPL, Staub, Kennzeichnung; Tabellen Zonen, Gruppen, T-Klassen, Zündschutzarten, Zündtemperaturen |
| **R&I-Kennzeichnung** | Kennbuchstaben-Finder (ISA-5.1, EN 62424, VDI 3814) für 78 Sensoren/Wächter; Kennbuchstaben-Tabellen |
| **Reinraum & Pharma** | ISO 14644-1 Klasse, Messstellen, Luftwechsel & Erholzeit, FFU-Decke, Druckkaskade; ISO- und GMP-Annex-1-Tabellen |
| **Rechenzentrum** | PUE/DCiE, PUE-Bilanz, EnEfG, Rack-Kühlung mit Redundanz, USV, Verfügbarkeit; ASHRAE TC 9.9, EN 50600/Tier |
| **Photovoltaik & Speicher** | String-Auslegung, Ertrag & Amortisation, DC-Leitungsverluste, Speichergröße |
| **Gebäude, Brandschutz & Akustik** | Fluchtweg ASR A2.3, Aufzugsschacht-Rauchabzug DE/CH; Nachhallzeit DIN 18041, Schallpegel |
| **Kommunikation & Pinbelegungen** | 30 Pinbelegungen mit Bild (Netzwerk/Telefon, Feldbus/Industrie, Computer/Video, Audio, Fahrzeug, Module), LWL- und Cinch-Farben, Antennenlänge, CCTV-Speicher und -Auflösungen |
| **Umrechnung & Einheiten** | Spannung (Effektiv/Scheitel/dB), Leistung, Energie/Arbeit, var ↔ µF, Winkel/Trigonometrie; AWG/kcmil/SWG ↔ mm², Querschnitt; Länge, Temperatur, Druck, Volumenstrom, Drehzahl, € ↔ CHF; Einheiten, SI-Vorsätze |

Weitere Funktionen: Live-Berechnung bei jeder Eingabe, Ergebnisleiste mit Ampelstatus, Suche, Favoriten,
Ergebnis kopieren (für Berichte/E-Mails), dunkles Design, Dezimalkomma oder -punkt.

### Projektlisten und Excel

- **Aus Excel einfügen:** In Excel den Bereich **mit Kopfzeile** markieren → Strg+C → in der App *Aus Excel einfügen*
  (Liste ersetzen oder anhängen). Spalten werden über die Überschrift erkannt – auch die Überschriften der Vorlage
  „Leistungsdaten_Kabelauslegung“ (z. B. „Nennleistung ASV / kW“, „f2 Häufung“, „Länge Leitung / m“).
  Leere Vorlagenzeilen ohne Namen werden übersprungen; fehlende Spalten erhalten Standardwerte.
- **Für Excel kopieren:** Tabelle mit allen Eingaben, Ergebnissen und Status in die Zwischenablage → in Excel Strg+V.
  *Mehr → Als CSV-Datei speichern* legt eine Datei in *Dokumente/ElektroRechner* ab (UTF-8, Semikolon, Dezimalkomma).
- GA-Datenpunktliste: *Mehr → Datenpunktliste kopieren* gibt **eine Zeile je Datenpunkt** mit BAS, Klartext und Typ aus.
- Die Listen werden automatisch gespeichert; *Mehr → Beispielprojekt laden / Liste leeren*.

**Unterschiede zur Excel-Vorlage** (bewusst, fachlich begründet):

| Punkt | Excel-Vorlage | App |
|---|---|---|
| Wirkwiderstand R′ | Spalte „bei 70 Grad“ enthält ≈ 20-°C-Werte (z. B. 1,5 mm²: 11,79 Ω/km) | R′ = ρ(70 °C)/A (1,5 mm²: 13,75 Ω/km) → ΔU ca. 13–17 % höher, auf der sicheren Seite |
| Verkettungsfaktor | 1,73 | √3 (I_B um 0,1 % kleiner) |
| Überlastschutz | nur I_B ≤ I_N ≤ I_Z | zusätzlich I_2 ≤ 1,45 · I_Z für LS und gG |
| Spannungsfall | nur Anzeige | Grenzwert einstellbar (Standard 2,5 %), Querschnitt wahlweise automatisch erhöhen („nein“ = wie Excel) |
| f1 / f2 | feste Liste | manuell **oder** aus Umgebungstemperatur, Isolierung, Anordnung und Anzahl Stromkreise |

Gegenprobe: Zeilen 6 und 9 der Vorlage (0,5 kW/230 V → 1,5 mm², I_Z 16,02 A; 15 kW/400 V → 4 mm², I_Z 25,37 A)
sind als Tests hinterlegt (`tests/test_lists.cpp`).

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
ctest --test-dir build --output-on-failure        # 96 Testfälle, > 9400 Prüfungen

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
elcalc run energy_cost --currency chf --rate 0.92   # Kosten in CHF (mit Betrag in €)
elcalc run pid_tags "name=Gas Methan"         # Kennbuchstaben: AT-101 (CH₄), QI, …
elcalc table fieldbuses                       # Tabelle ausgeben
elcalc sheet consumer_list liste.txt          # Verbraucherliste aus Excel-Kopie (Tab/;) rechnen → CSV
elcalc sheet ga_points - --detail < gaa.txt   # Datenpunktliste je Datenpunkt (stdin)
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
│       ├── formulas/         reine Formeln (getestet): basics, cables, motors, automation, electronics, units, air,
│       │                     datacenter, explosion, control, heating, building, acoustics
│       ├── catalog/          Rechner-Definitionen (Eingaben, Ergebnisse, Texte DE/EN, Prüfungen)
│       ├── lists/            Projektlisten: Verbraucherliste, GA-Datenpunktliste, CSV-Export/Import
│       └── tables/           Nachschlagetabellen (inkl. VDI 3814-4.1 Codes)
├── app/                      Qt-6-Quick-App (Windows/Android)
│   ├── CalcBridge.*          Brücke C++ ↔ QML (Sprache, Normprofil, Favoriten, Formatierung)
│   └── qml/                  Main, HomePage, ListPage, CalculatorPage (generisch), ProjectListPage,
│                             RowEditorPage, TablePage, SettingsPage; Bausteine InputForm, ResultView, ResultBar
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
IEC 60063, TIA-568, Modbus over Serial Line V1.02, NAMUR NE 43, ISO 14644-1/-3, EU-GMP Annex 1 (2022),
EN 16798-1, ISO/IEC 30134-2, EnEfG, EN 50600, ASHRAE TC 9.9, IEC 62548 / DIN VDE 0100-712,
VDI 3814 Blatt 3.1 (GA-Funktionen) und Blatt 4.1 (Kennzeichnung, Listen) – nur Codelisten/Strukturen, keine Normtexte,
ANSI/ISA-5.1, DIN EN 62424 / DIN 19227-1, IEC 81346-2, IEC/EN 60079-0/-14/-20-1, ATEX 2014/34/EU und 1999/92/EG,
EN ISO 52120-1, EN 13053, EN 13779 / EN 16798-3, EN 14511, DIN 18041, ASR A2.3 (2022), MBO § 39, VKF-Brandschutzrichtlinien,
DIN VDE 0105-100, AMEV „Gebäudeautomation 2005“.

**Fachbücher (1.5):** Siemens-Grundlagenbücher (Regeltechnik, Heizung, Lüftung, h,x-Diagramm, Kältetechnik, HLK-Einführung,
EN ISO 52120, BACnet/MS/TP), Haag „Grundlagen der Stromversorgung“, KNX Association „Grundlagenwissen“, Danfoss
„Wissenswertes über Frequenzumrichter“, Eaton Schaltungsbuch, TROX Planungshandbuch RLT, BAuA Fb 1030 „Lärm in
Bildungsstätten“, ZVEI/VDMA-Leitfaden Aufzugsschächte, QM Holzheizwerke FAQ 37. Die Bücher selbst liegen nicht im Repository.

## Ausbauplan

- **Stufe 2 – erledigt (1.2):** Verbraucherliste/Kabelauslegung, GA-Datenpunktliste nach VDI 3814, VDI-Codetabellen,
  Ventil aus Leistung. **Erledigt in 1.5:** AMEV-Datenpunktadresse (GA 2005), GA-Effizienzklassen EN ISO 52120-1.
  **Offen:** AMEV-BACnet-Profile (neuere AMEV-Unterlage fehlt), Sensor-/Ventil-/BSK-Liste nach VDI 3814-4.1 Tab. 7–9.
- **Stufe 3:** mehrere Projekte je Liste, PDF-Bericht, Spannungsfall über mehrere Abschnitte, Selektivität, Elektro-Symbole.

## Lizenz

Noch nicht festgelegt (privates Projekt). Vor einer Veröffentlichung Lizenz wählen; Qt wird unter LGPLv3 genutzt.
