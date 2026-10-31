# Berechnungen, Formeln & Tabellen

> Automatisch erzeugt mit `elcalc markdown` aus dem Rechenkern (Version 1.2.0).
> Alle Angaben ohne Gewähr – Normwerte vor Verwendung gegen die gültige Ausgabe prüfen.

## Projektlisten

### Verbraucherliste & Kabelauslegung

*Leistungsbilanz NN/NEA/USV, Schutzorgan, Querschnitt, ΔU je Verbraucher* — `consumer_list`

```text
Rechenkette je Verbraucher (wie Excel „Leistungsdaten_Kabelauslegung“):
S = P / cos φ   I_B = S / (k · U), k = √3 (400 V) bzw. 1
Schutzorgan: MSS/SelMod einstellbar (I_N = I_B, Auslegung auf I_B), LS/gG nächste Normstufe (Auslegung auf I_N)
F_ges = f1 · f2 · f3 · f4   I_r,min = I_B bzw. I_N / F_ges → Querschnitt nach DIN VDE 0298-4 / IEC 60364-5-52
I_Z = I_r · F_ges   Prüfung I_B ≤ I_N ≤ I_Z (LS/gG zusätzlich I_2 ≤ 1,45 · I_Z)
ΔU = k · I_B · L · (R' cos φ + X' sin φ), k = √3 bzw. 2, R' = ρ(θ)/A, X' nach Querschnitt
Spalten nach VDI 3814 Blatt 4.1 Tab. 6 (ELT-Verbraucherliste). f1/f2 leer = aus θ_a und Häufung berechnet.
```

**Projekteinstellungen:** Zul. Spannungsfall [%], Leitertemperatur für ΔU [°C], Querschnitt für ΔU automatisch erhöhen

**Spalten je Zeile:** Anlage/Gerät, BAS / Kennzeichnung, Gewerk, System/Schaltschrank, Aufstellort/Raum, Anzahl, Anschluss ELT/MSR, Versorgung, Nennspannung, Nennleistung [kW], cos φ, Gleichzeitigkeitsfaktor, Redundant (Standby), Schutzeinrichtung, Verlegeart, Isolierung, Umgebungstemperatur [°C], f1 Temperatur (manuell), Häufung – Anordnung, Anzahl Stromkreise in Häufung, f2 Häufung (manuell), f3 Oberschwingungen, f4 weiterer Faktor, Leitungslänge [m], Querschnitt vorgeben

**Ergebnisse je Zeile:** Anschlussleistung S [kVA], S mit Gzf, Anzahl [kVA], Betriebsstrom I_B [A], Schutzeinrichtung I_N [A], Schutzorgan, Auslegung auf, Gesamtfaktor F_ges, Mindest-Belastbarkeit I_r,min [A], Querschnitt nach Belastbarkeit [mm²], Gewählter Querschnitt [mm²], Tabellenwert I_r [A], Strombelastbarkeit I_Z [A], I_B ≤ I_N ≤ I_Z, Spannungsfall ΔU [V], Spannungsfall ΔU [%]

**Summen:** Wirkleistung gesamt [kW], Scheinleistung gesamt [kVA], Gesamtstrom (400 V) [A], Vorsicherung gG (Vorschlag) [A], S Netz (NN/AV) [kVA], S Ersatzstrom (NEA/SV) [kVA], S USV [kVA], S Anschluss ELT [kVA], S Anschluss MSR [kVA], S 400 V [kVA], S 230 V [kVA], S 24 V [kVA], Anzahl Verbraucherzeilen, Davon nicht i. O., Größter Spannungsfall [%]

### GA-Datenpunktliste (VDI 3814)

*AI/AO/BI/BO/COM zählen, Module, BAS-Adressen nach VDI 3814-4.1* — `ga_points`

```text
GA-Funktionsliste – Ein-/Ausgabefunktionen (VDI 3814 Blatt 3.1: Analoge/Binäre Eingabe AI/BI, Analoge/Binäre Ausgabe AO/BO, kommunikative Datenpunkte COM) aus Bauteil-Vorlagen.
Benutzeradresse (BAS) nach VDI 3814 Blatt 4.1: Liegenschaft_Gebäude_ISP_Gewerk_Anlage+Nr_Bauteil+Nr_Funktion+Nr, z. B. K_HG_01_L_VKA01_ZUV01_SB_01. Gewerk (Tab. 1), Anlage (Tab. 2), Bauteil (Tab. 3), Funktionskennung (Tab. 4).
Module = ⌈Anzahl · (1 + Reserve) / Kanäle je Modul⌉. Störungs-/Gefahrmeldungen als Öffnerkontakt (VDI 3814-3.1).
Die Vorlagen sind Planungspraxis und projektspezifisch anzupassen (Bauteil-Kürzel überschreibbar).
```

**Projekteinstellungen:** Liegenschaft, Gebäude/Bauteil, Informationsschwerpunkt (ISP), Trennzeichen, Reserve [%], Kanäle je AI-Modul, Kanäle je AO-Modul, Kanäle je BI-Modul, Kanäle je BO-Modul, Preis je physikalischem Datenpunkt [€ / CHF], Preis je kommunikativem Datenpunkt [€ / CHF]

**Spalten je Zeile:** Klartext Bauteil, Anlage (VDI 3814-4.1 Tab. 2), Anlagennummer, Bauteil-Vorlage, Bauteil-Kürzel (leer = Vorlage), Bauteilnummer (Start), Anzahl

**Ergebnisse je Zeile:** AI, AO, BI, BO, COM (Bus), Physikalische Datenpunkte, Erste Benutzeradresse, Funktionen

**Summen:** Physikalische Datenpunkte, Kommunikative Datenpunkte, Analoge Eingabe AI, Analoge Ausgabe AO, Binäre Eingabe BI, Binäre Ausgabe BO, Physikalisch inkl. Reserve, AI-Module, AO-Module, BI-Module, BO-Module, Kostenschätzung Datenpunkte [€]

## Grundlagen

### Ohmsches Gesetz & Leistung

*U, I, R, P – zwei Werte eingeben* — `ohm`

```text
Genau zwei der vier Größen eingeben, die übrigen werden berechnet.
U = R · I    P = U · I = I² · R = U² / R
```

**Eingaben:** Spannung U [V], Strom I [A], Widerstand R [Ω], Leistung P [W]

**Ergebnisse:** Spannung U [V], Strom I [A], Widerstand R [Ω], Leistung P [W]

### Leistung P, Q, S

*Wirk-, Blind-, Scheinleistung aus U, I, cos φ* — `power`

```text
S = k · U · I   (k = 1 bei DC/1~, k = √3 bei 3~ mit Außenleiterspannung)
P = S · cos φ    Q = S · sin φ    φ = arccos(cos φ)
```

**Eingaben:** Netz, Spannung U [V], Strom I [A], Leistungsfaktor cos φ

**Ergebnisse:** Scheinleistung S [kVA], Wirkleistung P [kW], Blindleistung Q [kvar], Phasenwinkel φ [°]

### Strom aus Leistung

*I = P / (k · U · cos φ · η)* — `current_from_power`

```text
I = P / (k · U · cos φ · η)
k = 1 bei DC/1~, k = √3 bei 3~. P ist die abgegebene Leistung; η = 100 % für die aufgenommene Leistung.
```

**Eingaben:** Netz, Leistung P [kW], Spannung U [V], Leistungsfaktor cos φ, Wirkungsgrad η [%]

**Ergebnisse:** Strom I [A], Aufgenommene Scheinleistung S [kVA]

### Leiterwiderstand

*R = ρ(θ) · L / A* — `conductor_resistance`

```text
R = ρ(θ) · L / A    ρ(θ) = ρ20 · (1 + α20 · (θ − 20 °C))
Cu: ρ20 = 0,017241 Ω·mm²/m, α20 = 0,00393 1/K (IEC 60028)
Al: ρ20 = 0,028264 Ω·mm²/m, α20 = 0,00403 1/K (IEC 60889)
```

**Eingaben:** Leitermaterial, Länge L (einfach) [m], Querschnitt A [mm²], Leitertemperatur θ [°C]

**Ergebnisse:** Widerstand eines Leiters [Ω], Schleifenwiderstand (Hin + Rück) [Ω], Spez. Widerstand ρ(θ) [Ω·mm²/m], Leitfähigkeit κ(θ) [m/(Ω·mm²)]

### Widerstände: Reihe & parallel

*R_ges für bis zu 6 Widerstände (auch L)* — `resistors_series_parallel`

```text
Reihe: R = R1 + R2 + …
Parallel: 1/R = 1/R1 + 1/R2 + …
Gilt genauso für Induktivitäten ohne Kopplung. Leere Felder werden ignoriert.
```

**Eingaben:** R1 [Ω], R2 [Ω], R3 [Ω], R4 [Ω], R5 [Ω], R6 [Ω]

**Ergebnisse:** Reihenschaltung [Ω], Parallelschaltung [Ω]

### Kondensatoren: Reihe & parallel

*C_ges für bis zu 6 Kondensatoren* — `capacitors_series_parallel`

```text
Parallel: C = C1 + C2 + …
Reihe: 1/C = 1/C1 + 1/C2 + …
Leere Felder werden ignoriert.
```

**Eingaben:** C1 [µF], C2 [µF], C3 [µF], C4 [µF], C5 [µF], C6 [µF]

**Ergebnisse:** Reihenschaltung [µF], Parallelschaltung [µF]

### Energieverbrauch & Kosten

*kWh und Kosten pro Tag/Jahr* — `energy_cost`

```text
E = P · t    Kosten = E · Preis
Die Währung ergibt sich aus dem eingegebenen Preis (€ oder CHF).
```

**Eingaben:** Leistung P [W], Betriebsstunden pro Tag [h], Betriebstage pro Jahr [d], Strompreis pro kWh [€ / CHF]

**Ergebnisse:** Energie pro Tag [kWh], Energie pro Jahr [kWh], Kosten pro Tag [€], Kosten pro Jahr [€]

## Leitungen & Schutz

### Leitungsdimensionierung

*Querschnitt nach Strombelastbarkeit, Spannungsfall und Schutzorgan* — `cable_sizing`

```text
Gewählt wird der kleinste Kupfer-Querschnitt, der alle Bedingungen erfüllt:
1) I_z = I_z0 · f_T · f_G ≥ I_B    2) ΔU ≤ ΔU_max    3) I_B ≤ I_n ≤ I_z und I_2 ≤ 1,45 · I_z
f_T aus Umgebungstemperatur (Tab. B.52.14/15), f_G Häufung (Tab. B.52.17). Spannungsfall bei max. Leitertemperatur (70/90 °C), X' = 0,08 mΩ/m.
Strombelastbarkeit Kupfer nach IEC 60364-5-52 Anhang B (entspricht DIN VDE 0298-4 bzw. NIN 5.2.3), Bezug 30 °C Luft bzw. 20 °C Erdreich. Werte vor Verwendung gegen die gültige Normausgabe prüfen.
```

**Eingaben:** Netz, Nennspannung U [V], Betriebsstrom I_B [A], Leitungslänge L (einfach) [m], Leistungsfaktor cos φ, Zul. Spannungsfall ΔU max [%], Isolierung, Verlegeart, Umgebungstemperatur θ_a [°C], Häufung – Anordnung, Anzahl Stromkreise in Häufung, Schutzorgan

**Ergebnisse:** Faktor Temperatur f_T, Faktor Häufung f_G, Erforderliches I_z0 [A], Min. Querschnitt (Belastbarkeit) [mm²], Min. Querschnitt (Spannungsfall) [mm²], Gewählter Querschnitt [mm²], Belastbarkeit I_z [A], Schutzorgan I_n [A], Spannungsfall [%], Maßgebend

### Spannungsfall

*ΔU in V und %, Verluste, Grenzwert DE/CH* — `voltage_drop`

```text
ΔU = k · L · I · (ρ(θ)/A · cos φ + X' · sin φ)
k = 2 bei DC/1~ (Hin- und Rückleiter), k = √3 bei 3~ (Außenleiterspannung)
ΔU % = ΔU / U · 100    P_V = n · I² · ρ · L / A
Typisch X' ≈ 0,08 mΩ/m für mehradrige Kabel. Leitertemperatur 70 °C (PVC) ist konservativ; IEC 60364-5-52 Anh. G nutzt ρ = 0,0225 Ω·mm²/m (Cu).
Grenzwerte: DIN 18015-1, DIN VDE 0100-520 Anhang G, NIN – gegen gültige Ausgabe und TAB des Netzbetreibers prüfen.
```

**Eingaben:** Netz, Nennspannung U [V], Betriebsstrom I_B [A], Leitungslänge L (einfach) [m], Querschnitt A [mm²], Leitermaterial, Leistungsfaktor cos φ, Leitertemperatur θ [°C], Reaktanzbelag X' [mΩ/m], Grenzwert, Eigener Grenzwert ΔU max [%]

**Ergebnisse:** Spannungsfall ΔU [V], Spannungsfall ΔU [%], Spannung am Verbraucher [V], Leitungsverluste P_V [W], Grenzwert [%], Max. Länge beim Grenzwert [m]

### Strombelastbarkeit I_z

*I_z nach Verlegeart, Temperatur und Häufung* — `ampacity`

```text
I_z = I_z0 · f_T · f_G
f_T = √((θ_max − θ_a)/(θ_max − θ_ref))
Strombelastbarkeit Kupfer nach IEC 60364-5-52 Anhang B (entspricht DIN VDE 0298-4 bzw. NIN 5.2.3), Bezug 30 °C Luft bzw. 20 °C Erdreich. Werte vor Verwendung gegen die gültige Normausgabe prüfen.
```

**Eingaben:** Querschnitt (Cu), Isolierung, Verlegeart, Belastete Adern, Umgebungstemperatur θ_a [°C], Häufung – Anordnung, Anzahl Stromkreise in Häufung

**Ergebnisse:** Tabellenwert I_z0 [A], Faktor Temperatur f_T, Faktor Häufung f_G, Strombelastbarkeit I_z [A]

### Überlastschutz (Koordination)

*I_B ≤ I_n ≤ I_z und I_2 ≤ 1,45 · I_z* — `overload_protection`

```text
Bedingungen nach IEC 60364-4-43 / DIN VDE 0100-430:
(1) I_B ≤ I_n ≤ I_z    (2) I_2 ≤ 1,45 · I_z
I_2 = 1,45 · I_n für LS-Schalter (IEC 60898-1); gG: 2,1 (≤ 4 A), 1,9 (< 16 A), 1,6 (≥ 16 A) · I_n (IEC 60269-1).
```

**Eingaben:** Betriebsstrom I_B [A], Strombelastbarkeit I_z [A], Schutzorgan

**Ergebnisse:** Bemessungsstrom I_n [A], Auslösestrom I_2 [A], Grenze 1,45 · I_z [A]

### Abschaltbedingung / max. Länge (TN)

*Z_s,max und L_max für LS B/C/D* — `disconnection`

```text
Abschaltbedingung TN (IEC 60364-4-41, 411.4.4): Z_s · I_a ≤ U_0
Konventionelle Methode: L_max = 0,8 · U_0 · S_ph / (ρ · (1 + m) · I_a), m = S_ph / S_PE
0,8: 80 % von U_0 am Anfang des Stromkreises; ρ = 0,0225 (Cu) bzw. 0,036 (Al) Ω·mm²/m.
I_a = Sofortauslösung LS: B 5 · I_n, C 10 · I_n, D 20 · I_n. Bei Sicherungen I_a aus Kennlinie (0,4 s / 5 s).
Ersetzt nicht die Messung der Schleifenimpedanz.
```

**Eingaben:** Spannung gegen Erde U_0 [V], Schutzorgan, Bemessungsstrom I_n [A], Abschaltstrom I_a [A], Querschnitt Außenleiter S_ph [mm²], Querschnitt Schutzleiter S_PE [mm²], Leitermaterial, Geplante Länge [m]

**Ergebnisse:** Abschaltstrom I_a [A], Max. Schleifenimpedanz Z_s [Ω], Max. Leitungslänge L_max [m]

### Max. Leitungslänge (Spannungsfall)

*z. B. 24-V-Feldgeräte, Stellantriebe* — `max_length_vdrop`

```text
L_max = ΔU_max / (k · I · (ρ/A · cos φ + X' · sin φ))
k = 2 bei DC/1~, √3 bei 3~.
```

**Eingaben:** Netz, Nennspannung U [V], Strom I [A], Querschnitt A [mm²], Leitermaterial, Leistungsfaktor cos φ, Leitertemperatur θ [°C], Zul. Spannungsfall ΔU max [%]

**Ergebnisse:** Max. Leitungslänge (einfach) [m], Zul. Spannungsfall [V], Min. Spannung am Gerät [V]

### Kurzschlussfestigkeit (I²t)

*Zulässige Energie und Abschaltzeit einer Leitung* — `short_circuit_withstand`

```text
Adiabatische Erwärmung (IEC 60364-4-43, 434.5.2): t = (k · S / I_k)²   bzw.   S ≥ √(I_k² · t) / k
Zulässige Energie: (k · S)²   k nach Tab. 43A: Cu/PVC 115, Cu/VPE 143, Al/PVC 76, Al/VPE 94.
Gültig für t ≤ 5 s. Für t < 0,1 s Durchlass-I²t des Schutzorgans (Herstellerangabe) vergleichen.
```

**Eingaben:** Leiter / Isolierung, Querschnitt S [mm²], Kurzschlussstrom I_k [kA], Abschaltzeit t [s]

**Ergebnisse:** Materialbeiwert k [A·√s/mm²], Zulässige Energie (k·S)² [A²s], Max. Abschaltzeit bei I_k [s], Min. Querschnitt für I_k, t [mm²]

### Rohr-/Kanalfüllung

*Füllgrad eines Installationsrohres* — `conduit_fill`

```text
Füllgrad = n · d² / D_i²  (Querschnittsflächen)
Richtwerte (u. a. NEC Chapter 9): 1 Leitung 53 %, 2 Leitungen 31 %, ab 3 Leitungen 40 %. In DE/CH gilt: Leitungen müssen sich ohne Beschädigung einziehen lassen – Herstellerangaben beachten.
```

**Eingaben:** Innendurchmesser Rohr D_i [mm], Außendurchmesser Leitung d [mm], Anzahl Leitungen

**Ergebnisse:** Füllgrad [%], Richtwert [%], Max. Anzahl bei 40 %

## Motoren & Antriebe

### Motor-Bemessungsstrom

*I_N aus P_N, U, cos φ und η* — `motor_current`

```text
I_N = P_N / (k · U · cos φ · η)    k = √3 (3~) bzw. 1 (1~)
P_N = abgegebene mechanische Leistung laut Typenschild.
P_el = P_N / η
```

**Eingaben:** Netz, Bemessungsleistung P_N [kW], Spannung U [V], Leistungsfaktor cos φ, Wirkungsgrad η [%]

**Ergebnisse:** Bemessungsstrom I_N [A], Aufgenommene Wirkleistung P_el [kW], Scheinleistung S [kVA], Blindleistung Q [kvar]

### Drehmoment · Leistung · Drehzahl

*M = 9549 · P / n – zwei Werte eingeben* — `torque`

```text
M [Nm] = 9549,3 · P [kW] / n [1/min]    (P = 2π · n · M / 60)
Genau zwei Werte eingeben.
```

**Eingaben:** Leistung P [kW], Drehzahl n [1/min], Drehmoment M [Nm]

**Ergebnisse:** Leistung P [kW], Drehzahl n [1/min], Drehmoment M [Nm]

### Drehzahl & Schlupf

*Synchrondrehzahl, Schlupf, Läuferfrequenz* — `motor_speed`

```text
n_s = 120 · f / Polzahl = 60 · f / p    s = (n_s − n) / n_s    f_2 = s · f
```

**Eingaben:** Frequenz f [Hz], Polzahl, Bemessungsdrehzahl n [1/min]

**Ergebnisse:** Synchrondrehzahl n_s [1/min], Schlupf s [%], Läuferfrequenz f_2 [Hz]

### Wirkungsgrad aus Messung

*η aus U, I, cos φ und Abgabeleistung* — `motor_efficiency`

```text
P_el = k · U · I · cos φ    η = P_mech / P_el    P_V = P_el − P_mech
```

**Eingaben:** Netz, Spannung U [V], Strom I [A], Leistungsfaktor cos φ, Abgegebene Leistung P_mech [kW]

**Ergebnisse:** Aufgenommene Leistung P_el [kW], Wirkungsgrad η [%], Verluste P_V [kW]

### Stern-Dreieck-Anlauf

*Anlaufstrom, Anlaufmoment, Motorschutz-Einstellung* — `star_delta`

```text
Im Stern: I_A,Y = I_A,Δ / 3    M_A,Y = M_A,Δ / 3
Motorschutzrelais im Motorstrang (nach dem Netzschütz): Einstellung I_N / √3 ≈ 0,58 · I_N.
Voraussetzung: Motor für Dreieck an Netzspannung (z. B. 400 V Δ / 690 V Y).
```

**Eingaben:** Bemessungsstrom I_N [A], Anlaufstromverhältnis I_A/I_N, Anzugsmomentverhältnis M_A/M_N, Lastmoment beim Anlauf M_L/M_N

**Ergebnisse:** Anlaufstrom direkt (Δ) [A], Anlaufstrom Stern (Y) [A], Anzugsmoment Stern M_A,Y / M_N, Einstellung Motorschutzrelais [A]

### Drehstrommotor am 1~-Netz (Steinmetz)

*Betriebs- und Anlaufkondensator* — `steinmetz`

```text
Faustformel: C_B ≈ 70 µF pro kW bei 230 V / 50 Hz (skaliert mit (230/U)² · 50/f)
C_A ≈ 2…3 · C_B (nur während des Anlaufs). Leistung ca. 70…80 % von P_N.
Motor in Dreieck für 230 V schalten (230/400-V-Motor). Kondensator-Nennspannung ≥ 1,25 · U.
```

**Eingaben:** Motorleistung P_N [kW], Netzspannung 1~ [V], Frequenz f [Hz]

**Ergebnisse:** Betriebskondensator C_B [µF], Anlaufkondensator C_A, Verfügbare Leistung ca. [kW]

## Industrie & Netze

### Blindleistungskompensation

*Q_C und Kondensator von cos φ1 auf cos φ2* — `pf_correction`

```text
Q_C = P · (tan φ1 − tan φ2)
Kapazität je Kondensator (U = Außenleiterspannung, ω = 2πf):
Dreieck: C = Q_C / (3 · U² · ω)    Stern: C = Q_C / (U² · ω)    1~: C = Q_C / (U² · ω)
Bei Oberschwingungen verdrosselte Anlagen vorsehen.
```

**Eingaben:** Wirkleistung P [kW], cos φ1 (Ist), cos φ2 (Ziel), Spannung U [V], Frequenz f [Hz], Schaltung

**Ergebnisse:** Kompensationsleistung Q_C [kvar], Kapazität je Kondensator [µF], Scheinleistung vorher [kVA], Scheinleistung nachher [kVA], Strom vorher [A], Strom nachher [A]

### Transformator: Ströme & I_k

*Bemessungsströme, Übersetzung, Kurzschlussstrom* — `transformer`

```text
I = S / (√3 · U) bei 3~ bzw. S / U bei 1~    ü = U1 / U2
I_k ≈ c · I_r2 · 100 % / u_k   (starres Netz, c = 1,05)
```

**Eingaben:** Ausführung, Bemessungsleistung S_r [kVA], Oberspannung U_1 [V], Unterspannung U_2 [V], Kurzschlussspannung u_k [%]

**Ergebnisse:** Bemessungsstrom primär [A], Bemessungsstrom sekundär [A], Übersetzung ü, Kurzschlussstrom sekundär I_k [kA]

### Kurzschlussstrom am Leitungsende

*I_k3 max und I_k1 min (Abschätzung IEC 60909)* — `short_circuit`

```text
Vereinfachte Abschätzung nach IEC 60909-0 (ohne Korrekturfaktor K_T):
I_k3 = c_max · U_n / (√3 · |Z_Q + Z_T + Z_L|),  c_max = 1,05, Leiter bei 20 °C
I_k1 = c_min · U_0 / |⅔Z_Q + Z_T + Z_L + Z_PE|,  c_min = 0,95, Leiter bei 80 °C (Dyn-Trafo)
Z_Q = c · U_n² / S"_kQ (X/R = 10), Z_T = u_k · U_n² / S_rT, R_T = u_R · U_n² / S_rT.
Für die Planung verbindlich: Netzberechnungsprogramm bzw. Angaben des Netzbetreibers.
```

**Eingaben:** Trafo-Bemessungsleistung S_rT [kVA], Kurzschlussspannung u_k [%], Ohmscher Anteil u_R [%], Netzspannung U_n [V], Netzkurzschlussleistung S"_kQ (leer = starr) [MVA], Leitungslänge L [m], Querschnitt Außenleiter [mm²], Querschnitt PE/PEN [mm²], Leitermaterial, Reaktanzbelag X' [mΩ/m]

**Ergebnisse:** I_k3 an Trafoklemmen [kA], I_k3 max am Leitungsende [kA], I_k1 min am Leitungsende [kA], Impedanz Trafo Z_T [mΩ], Impedanz Leitung Z_L (20 °C) [mΩ]

### Schaltschrank-Wärmebilanz

*Innentemperatur, Kühlleistung, Lüfter* — `enclosure_cooling`

```text
Wirksame Oberfläche nach IEC/TR 60890 (DIN VDE 0660-507):
freistehend: A = 1,8 · H · (B + T) + 1,4 · B · T    Wandmontage: A = 1,4 · B · (H + T) + 1,8 · T · H
ΔT = P_V / (k · A)    Kühlleistung Q_K = P_V − k · A · (θ_i,max − θ_u)
Lüfter: V̇ = 3,1 · Q / ΔT  [m³/h] (Meereshöhe). k ≈ 5,5 W/(m²·K) für Stahlblech.
```

**Eingaben:** Verlustleistung P_V [W], Höhe H [m], Breite B [m], Tiefe T [m], Aufstellung, Wärmedurchgangskoeffizient k [W/(m²·K)], Umgebungstemperatur θ_u [°C], Max. Innentemperatur θ_i [°C]

**Ergebnisse:** Wirksame Oberfläche A [m²], Übertemperatur ohne Kühlung ΔT [K], Innentemperatur ohne Kühlung [°C], Erforderliche Kühlleistung [W], Lüfter-Volumenstrom (falls θ_u < θ_i) [m³/h]

## Gebäudeautomation & MSR

### Analogsignal skalieren

*4–20 mA / 0–10 V ↔ Messwert ↔ Rohwert (SPS)* — `analog_scaling`

```text
Lineare Skalierung: y = y_min + (x − x_min) · (y_max − y_min) / (x_max − x_min)
Rohwerte: Siemens S7 0…27648 (Nennbereich), 12 Bit 0…4095, 15 Bit 0…32767, 16 Bit 0…65535.
NAMUR NE 43 (4–20 mA): Messbereich 3,8…20,5 mA, Ausfallsignal ≤ 3,6 mA bzw. ≥ 21 mA.
```

**Eingaben:** Signal, Gegeben, Wert, Messbereich Anfang, Messbereich Ende, Rohwertformat

**Ergebnisse:** Anteil am Messbereich [%], Signal [mA], Messwert, Rohwert

### 4–20-mA-Bürde / Schleife

*Max. Bürde, Leitungswiderstand, max. Länge* — `loop_burden`

```text
R_max = (U_S − U_min,Transmitter) / I_max
R_Leitung = 2 · ρ · L / A    Reserve = R_max − R_Last − R_Leitung
Typisch: U_S = 24 V, Transmitter 10…12 V, AI-Bürde 250 Ω (HART ≥ 230 Ω). I_max 20 mA (bzw. 21…22 mA für NE-43-Fehlersignal).
```

**Eingaben:** Speisespannung U_S [V], Min. Transmitterspannung [V], Max. Schleifenstrom [mA], Bürde Auswertung (AI, Anzeiger …) [Ω], Leitungslänge (einfach) [m], Querschnitt [mm²]

**Ergebnisse:** Max. Gesamtbürde [Ω], Leitungswiderstand (Hin + Rück) [Ω], Reserve [Ω], Max. Leitungslänge [m]

### Widerstandsthermometer Pt/Ni

*Temperatur ↔ Widerstand für Pt100/Pt1000/Ni1000* — `rtd`

```text
Pt (IEC 60751): R = R0 · (1 + A·T + B·T² + C·(T − 100)·T³), C nur unter 0 °C
A = 3,9083·10⁻³, B = −5,775·10⁻⁷, C = −4,183·10⁻¹²
Ni1000 (DIN 43760): R = R0 · (1 + A·T + B·T² + D·T⁴ + F·T⁶)
Ni1000 TK5000: R = R0 · (1 + 4,427·10⁻³·T + 5,172·10⁻⁶·T² + 5,585·10⁻⁹·T³)
```

**Eingaben:** Fühlertyp, Gegeben, Temperatur T [°C], Widerstand R [Ω]

**Ergebnisse:** Widerstand R [Ω], Temperatur T [°C], Empfindlichkeit dR/dT [Ω/K]

### Leitungsfehler Pt/Ni-Fühler

*Messfehler durch Zuleitung bei 2-Leiter-Anschluss* — `rtd_wire_error`

```text
2-Leiter: Der Leitungswiderstand R_L = 2 · ρ · L / A addiert sich zum Fühler.
Fehler ΔT = R_L / (dR/dT). 3-Leiter kompensiert bei gleichen Adern, 4-Leiter vollständig.
Pt100 im 2-Leiter-Anschluss nur bei sehr kurzen Leitungen; GA meist Pt1000/Ni1000.
```

**Eingaben:** Fühlertyp, Anschluss, Leitungslänge (einfach) [m], Querschnitt [mm²], Messtemperatur [°C]

**Ergebnisse:** Leitungswiderstand (Hin + Rück) [Ω], Messfehler ΔT [K]

### NTC-Thermistor (β-Modell)

*Temperatur ↔ Widerstand für NTC 10k, 20k …* — `ntc`

```text
R(T) = R25 · exp(β · (1/T − 1/298,15 K))
Typisch: NTC10k β = 3435 K oder 3950 K, NTC20k β = 4250 K (Herstellerangaben!).
```

**Eingaben:** Nennwiderstand R25 [kΩ], β-Wert (B25/85) [K], Gegeben, Temperatur T [°C], Widerstand R [kΩ]

**Ergebnisse:** Widerstand R [kΩ], Temperatur T [°C]

### 24-V-Versorgung dimensionieren

*Trafo (AC) oder Netzteil (DC) für Feldgeräte* — `psu_24v`

```text
P_erf = Σ(n · P) · g · (1 + Reserve)
Stellantriebe nach Dimensionierungsleistung (VA) laut Datenblatt, nicht nach Betriebsleistung. AC-Trafo-Normgrößen: 25 … 1000 VA; DC-Netzteile 2,5 … 40 A bei 24 V.
```

**Eingaben:** Versorgung, Anzahl Gruppe 1, Leistung je Gerät Gr. 1 [VA / W], Anzahl Gruppe 2, Leistung je Gerät Gr. 2 [VA / W], Anzahl Gruppe 3, Leistung je Gerät Gr. 3 [VA / W], Gleichzeitigkeitsfaktor g, Reserve [%]

**Ergebnisse:** Summe angeschlossen [VA], Erforderlich [VA], Empfohlene Normgröße [VA], Strom bei 24 V (Normgröße) [A]

### Ventil kv/kvs & Ventilautorität

*kv-Wert, kvs-Auswahl, Δp, Autorität – auch aus Leistung/ΔT* — `valve_kv`

```text
V̇ = P · 3600 / (ρ · c · ΔT)   (Angaben der Ventilliste nach VDI 3814 Blatt 4.1, Tab. 8: Leistung, VL/RL, ΔT)
kv = V̇ · √(ρ/1000 / Δp[bar])    Δp_V100 = (V̇ / kvs)² · ρ/1000 · 100 kPa
Ventilautorität a = Δp_V100 / (Δp_V100 + Δp_MV)
Δp_MV = Druckverlust der mengenvariablen Strecke. Empfehlung a ≥ 0,5 (mind. 0,3).
Wasser: ρ = 1000 kg/m³, c = 4,187 kJ/(kg·K). Glykolgemische: ρ und c aus dem Datenblatt des Herstellers.
```

**Eingaben:** Auslegung aus, Volumenstrom V̇ [m³/h], Leistung P [kW], Temperaturdifferenz ΔT (VL − RL) [K], Medium, Dichte ρ [kg/m³], Spez. Wärmekapazität c [kJ/(kg·K)], Gewünschter Δp am Ventil [kPa], Δp mengenvariable Strecke [kPa]

**Ergebnisse:** Volumenstrom V̇ [m³/h], Berechneter kv-Wert [m³/h], Gewählter kvs-Wert (Normreihe) [m³/h], Δp bei kvs (voll offen) [kPa], Ventilautorität a

### Wärmeleistung ↔ Volumenstrom

*Wasser oder Luft: P = V̇ · ρ · c · ΔT* — `heat_flow`

```text
P = V̇ · ρ · c · ΔT
Wasser: ρ = 1000 kg/m³, c = 4,187 kJ/(kg·K) → V̇ [m³/h] = P [kW] / (1,163 · ΔT)
Luft: ρ = 1,2 kg/m³, c = 1,005 kJ/(kg·K) → P [W] ≈ 0,335 · V̇ [m³/h] · ΔT
```

**Eingaben:** Medium, Gegeben, Leistung P [kW], Volumenstrom V̇ [m³/h], Temperaturdifferenz ΔT [K], Dichte ρ [kg/m³], Spez. Wärmekapazität c [kJ/(kg·K)]

**Ergebnisse:** Leistung P [kW], Volumenstrom V̇ [m³/h], Volumenstrom V̇ [l/s], Massenstrom ṁ [kg/h]

### DALI-Leitung (Spannungsfall)

*Querschnitt für max. 2 V Spannungsfall* — `dali_bus`

```text
DALI (IEC 62386): max. 2 V Spannungsfall auf der Busleitung, max. 250 mA, max. 300 m.
A_min = 2 · L · I · ρ / ΔU_max
Richtwerte: bis 100 m 0,5 mm², bis 150 m 0,75 mm², bis 300 m 1,5 mm².
```

**Eingaben:** Leitungslänge (einfach) [m], Busstrom [mA], Max. Spannungsfall [V]

**Ergebnisse:** Min. Querschnitt [mm²], Empfohlener Querschnitt [mm²], Spannungsfall bei Empfehlung [V]

### Feuchte Luft (h-x)

*Taupunkt, absolute Feuchte, Enthalpie* — `humid_air`

```text
Sättigungsdampfdruck (Magnus, WMO, über Wasser, −45…60 °C): p_s = 6,112 · e^(17,62·t / (243,12 + t)) hPa
p_D = φ · p_s    x = 0,622 · p_D / (p − p_D)    h = 1,006 · t + x · (2501 + 1,86 · t) kJ/kg
Taupunkt: τ = 243,12 · γ / (17,62 − γ), γ = ln(φ) + 17,62 · t / (243,12 + t). Luftdruck nach Höhe anpassen (Basel ≈ 980 hPa).
```

**Eingaben:** Lufttemperatur t [°C], Gegeben, Relative Feuchte φ [%], Taupunkt τ [°C], Luftdruck [hPa]

**Ergebnisse:** Taupunkt τ [°C], Relative Feuchte φ [%], Absolute Feuchte x [g/kg], Enthalpie h [kJ/kg], Dampfdruck p_D [hPa], Sättigungsdampfdruck p_s [hPa], Dichte [kg/m³]

### Außenluftvolumenstrom

*Nach EN 16798-1 (Personen + Fläche) oder CO₂-Bilanz* — `fresh_air`

```text
EN 16798-1 (Anhang B, Verfahren 1): q = n · q_P + A · q_B
q_P je Person: Kat. I 10, II 7, III 4 l/s.   q_B je m² (schadstoffarmes Gebäude): I 1,0, II 0,7, III 0,4 l/(s·m²); sehr schadstoffarm: 0,5 / 0,35 / 0,2; nicht schadstoffarm: 2,0 / 1,4 / 0,8.
CO₂-Bilanz (stationär): V̇ = n · G / (C_Soll − C_außen), G ≈ 20 l/h je Person bei Büroarbeit.
```

**Eingaben:** Verfahren, Personen, Raumfläche [m²], Raumhöhe (für Luftwechsel) [m], Kategorie, Gebäude-Emission, CO₂-Abgabe je Person [l/h], CO₂-Sollwert [ppm], CO₂ außen [ppm]

**Ergebnisse:** Außenluftvolumenstrom [m³/h], Außenluftvolumenstrom [l/s], Je Person [m³/h], Luftwechselrate [1/h], CO₂-Konzentration (stationär) [ppm]

### VDI 3814: Gewerkekennzeichnung

| Kostengruppe/Gewerk | DIN 276-1 | BAS kurz | BAS lang | IEC 81346 |
|---|---|---|---|---|
| Türen, Tore, Fenster, Sonnenschutz | 330 | Z | TTF | BF/BD |
| Abwasser-, Wasser-, Gasanlagen | 410 | W | GWA | TS/TT/TG |
| Wärmeversorgungsanlagen | 420 | H | HZG | TH |
| Lufttechnische Anlagen | 430 | L | RLT | TL |
| Kälteanlagen | 434 | K | KAE | TK |
| Starkstromanlagen | 440 | E | ELT | TE |
| Fernmelde- und IT-Anlagen | 450 | N | FIA | TF/TD |
| Gefahrenmelde- und Alarmanlagen | 456 | G | GMA | TF |
| Förderanlagen | 460 | F | FTA | TJ |
| Küchentechnische Anlagen | 471 | C | KTA | TQ |
| Wäscherei- und Reinigungsanlagen | 472 | D | WRA | TN |
| Medienversorgungsanlagen | 473 | O | MVA | TM |
| Medizin- und labortechnische Anlagen | 474 | M | MED | TN |
| Feuerlöschanlagen | 475 | S | FLA | TP |
| Badetechnische Anlagen | 476 | B | BTA | TN |
| Prozesswärme-, -kälte- und -luftanlagen | 477 | P | PRO | TN |
| Entsorgungsanlagen | 478 | X | ENT | TU |
| Gebäudeautomation allgemein | 480 | A | GAS | TC |
| Automations-/Informationsschwerpunkt | 482 | I | ASP/ISP | TC |
| Raumautomation | 484 | R | RAU | TC |
| Technische Anlagen in Außenanlagen | 540 | Y | TAA | BG |

*Auszug aus VDI 3814 Blatt 4.1 (2019-01). Kürzel können projektspezifisch ergänzt und angepasst werden.*

### VDI 3814: Anlagenkürzel

| KG | Anlage | Kürzel |
|---|---|---|
| 300 | Fassade | FAS |
| 300 | Natürlicher Rauchabzug | NRA |
| 300 | Rauch- und Wärmeabzugsanlage | RWA |
| 300 | Sonnenschutzanlage | SON |
| 411 | Hebeanlage | HBA |
| 411 | Neutralisationsanlage | NEU |
| 412 | Druckerhöhungsanlage | DEA |
| 412 | Enthärtungsanlage | EHA |
| 412 | Trinkwasser | TWA |
| 412 | Trinkwassererwärmung | TWE |
| 412 | Umkehrosmoseanlage | UKO |
| 412 | Vollentsalzungsanlage | VEA |
| 421 | BHKW | BHK |
| 421 | Fernwärmeübergabe | FWÜ |
| 421 | Geothermieanlage | GTH |
| 421 | Kessel | KES |
| 421 | Reindampferzeugung | RDE |
| 421 | Solaranlage | SOL |
| 421 | Wärmepumpe | WPU |
| 422 | Druckhaltung | DHG |
| 422 | Heizkreis | HZK |
| 422 | Pufferspeicher | PFS |
| 422 | Wärmeübertrager | WTR |
| 423 | Betonkernaktivierung | BKA |
| 423 | Fußbodenheizung | FBH |
| 423 | Heiz-/Kühldecke | HKD |
| 423 | Statische Heizung | STH |
| 430 | Abluftanlage | ABA |
| 430 | Kreislaufverbundsystem | KVS |
| 430 | Maschinelle Rauchabzugsanlage | MRA |
| 430 | Rauchdruckanlage | RDA |
| 430 | Raumlufttechnik allgemein | RLT |
| 430 | Teilklimaanlage | TKA |
| 430 | Umluftanlage | ULA |
| 430 | Vollklimaanlage | VKA |
| 430 | Wärmerückgewinnung | WRG |
| 430 | Zuluftanlage | ZUA |
| 430 | Entrauchungsanlage | ERA |
| 434 | Fernkälteübergabe | FKÜ |
| 434 | Kältekreis | KKR |
| 434 | Kälteverteilnetz | KVN |
| 434 | Kaltwassererzeugung | KWE |
| 434 | Kältespeicher | KSP |
| 434 | Kühldecke | KÜD |
| 434 | Rückkühlwerk | RKW |
| 441 | Schaltanlagen | STA |
| 441 | Transformator | TRF |
| 441 | Kompensationsanlage | KOA |
| 442 | Batterieanlage | BAT |
| 442 | Ersatzstromanlage | ESA |
| 442 | Fotovoltaikanlage | PVA |
| 442 | USV | USV |
| 443 | Allgemeine Stromversorgung | ASV |
| 443 | Sicherheitsstromversorgung | SSV |
| 443 | Beleuchtungsanlage | BEL |
| 443 | Sicherheitsbeleuchtung | SIB |

*Auszug aus VDI 3814 Blatt 4.1 (2019-01). Kürzel können projektspezifisch ergänzt und angepasst werden.*

### VDI 3814: Geräte- und Sensorkürzel

| Gruppe | Gerät/Komponente | Kürzel |
|---|---|---|
| 430 | Abluftventilator | ABV |
| 430 | Zuluftventilator | ZUV |
| 430 | Fortluftventilator | FOV |
| 430 | Umluftventilator | UMV |
| 430 | Befeuchter | BEF |
| 430 | Brandschutzklappe | BSK |
| 430 | Entrauchungsklappe | ERK |
| 430 | Erhitzer allgemein | ERH |
| 430 | Wasserlufterhitzer | WLH |
| 430 | Wasservorerhitzer | WVH |
| 430 | Wassernacherhitzer | WNH |
| 430 | Elektrolufterhitzer | ELH |
| 430 | Luftfilter | LFI |
| 430 | Lüftungsklappe | LKL |
| 430 | Pumpe | PPE |
| 430 | Variabler Volumenstromregler | VVR |
| 430 | Wärmerückgewinnung | WRG |
| 430 | Umluftkühlgerät | ULK |
| 434 | Kältemaschine | KMA |
| 434 | Rückkühlwerk | RKW |
| 440 | Leistungsschalter | LSS |
| 440 | Netzanalysegerät | NAG |
| 440 | Notstromaggregat | NSG |
| 440 | Transformator | TRF |
| 480 | Anlagenschalter | ANL |
| 480 | Automationsstationseinrichtung | ASE |
| 480 | Datenschnittstelleneinheit | DSE |
| 480 | Feldbusmodul | FBM |
| 481 T | Außentemperatur | TAU |
| 481 T | Zulufttemperatur | TZU |
| 481 T | Ablufttemperatur | TAB |
| 481 T | Fortlufttemperatur | TFO |
| 481 T | Mischlufttemperatur | TMI |
| 481 T | Raumtemperatur | TRU |
| 481 T | Vorlauftemperatur | TVL |
| 481 T | Rücklauftemperatur | TRL |
| 481 T | Frostschutzwächter | FSW |
| 481 T | Sicherheitstemperaturbegrenzer | STB |
| 481 T | Temperatur nach WRG | TNW |
| 481 T | Temperatur Warmwasser | TWW |
| 481 p | Differenzdruck | PDI |
| 481 p | Zuluftdruck | PZU |
| 481 p | Raumdruck | PRU |
| 481 p | Druckwächter | DRW |
| 481 Q | CO₂-Sensor | COM |
| 481 Q | Luftqualität Raum | QRU |
| 481 φ | Raumfeuchte relativ | MRU |
| 481 φ | Zuluftfeuchte relativ | MZU |
| 481 x | Zuluftfeuchte absolut | XZU |
| 481 φ | Kondensationswächter | MKW |
| 481 Z | Elektrozähler | ELZ |
| 481 Z | Wärmemengenzähler | WMZ |
| 481 Z | Kältemengenzähler | KMZ |
| 481 Z | Wasserzähler | WAZ |
| 481 S | Leckageüberwachung | LCK |
| 481 S | Strömungswächter | SSW |
| 481 S | Not-Aus-Taster | NOT |
| 481 V̇ | Volumenstrom Zuluft | FZU |
| 481 V̇ | Volumenstrom Abluft | FAB |
| 481 A | Motorische Klappe | KLA |
| 481 A | Motorventil | VEN |
| 481 A | Heizkörperventil-Antrieb | HKV |
| 485 | Präsenzmelder | PAE |
| 485 | Raumbediengerät | RBG |
| 485 | Raumautomationseinrichtung | RAE |

*Auszug aus VDI 3814 Blatt 4.1 (2019-01). Kürzel können projektspezifisch ergänzt und angepasst werden.*

### VDI 3814: Funktionskennungen

| Funktionskennung | Kürzel | Typ. E/A |
|---|---|---|
| Anforderung | AF_ | BI/COM |
| Anlagenschalter | AS_ | BI |
| Belegtzustand Raum | BEL | BI/COM |
| Betriebsbereit | BB_ | BI |
| Betriebsmeldung | BM_ | BI |
| Betriebsstunden | BZ_ | virt. |
| Einschaltfehler | EF_ | virt. |
| Entriegelung | ENT | BO |
| Freigabe | FR_ | BO |
| Gefahrmeldung | GM_ | BI |
| Grenzwert | GW_ | virt. |
| Grenzwert MAX | GWX | virt. |
| Grenzwert MIN | GWI | virt. |
| LVB-Handmeldung | HD_ | BI |
| Heizkennlinie Steilheit | HST | Par. |
| Hysterese | HYS | Par. |
| Kalender | KAL | virt. |
| Laufzeitfehler | LFZ | virt. |
| Loop-Objekt (Regler) | LP_ | virt. |
| Messwert | MW_ | AI |
| Mittelwert | MWT | virt. |
| Nachlaufzeit | NL_ | Par. |
| Nachstellzeit | TN_ | Par. |
| Programm | PR_ | virt. |
| Proportionalband | XP_ | Par. |
| Rückmeldung AUF | RMA | BI |
| Rückmeldung ZU | RMZ | BI |
| Rückführwert | RW_ | AI |
| Rückmeldung | RM_ | BI |
| Ausführkontrolle | RF_ | virt. |
| Schaltbefehl | SB_ | BO |
| Scheduler | SCH | virt. |
| Sollwert | SW_ | AO/virt. |
| Sollwert Heizen | SWH | virt. |
| Sollwert Kühlen | SWK | virt. |
| Sollwert Nacht | SWN | virt. |
| Sollwert Tag | SWT | virt. |
| Sollwertversteller | SWV | AI/COM |
| Status | STA | virt. |
| Stellsignal | ST_ | AO |
| Störung | SM_ | BI |
| Taster | TS_ | BI |
| Totzone | TZ_ | Par. |
| Trend-Log-Objekt | TR_ | virt. |
| Vorhaltezeit | TV_ | Par. |
| Wartungsmeldung | WM_ | BI |
| Wirkungsgrad | WG_ | virt. |
| Zählwert | ZW_ | BI/COM |
| Zeitprogramm | ZP_ | virt. |

*Auszug aus VDI 3814 Blatt 4.1 (2019-01). Kürzel können projektspezifisch ergänzt und angepasst werden.
Zusatz-Erläuterung möglich, z. B. SWH01_ECO, ABV01_WM_01_REP. Spalte „Typ. E/A“ = übliche Zuordnung aus der Planungspraxis, nicht Teil der Richtlinie.*

## Reinraum & Pharma

### Reinraumklasse ISO 14644-1

*Grenzwert je Klasse ↔ Klasse aus Messwert* — `iso14644_class`

```text
C_n = 10^N · (0,1 / D)^2,08   [Partikel/m³]   (ISO 14644-1:2015, Formel A.1)
N = log₁₀(C_n) + 2,08 · log₁₀(D / 0,1)
Tabellenwerte sind auf ganze Zahlen mit max. 3 signifikanten Stellen gerundet. Zwischenklassen in 0,1-Schritten zulässig. Für ≥ 5 µm bei ISO 5 und darunter gilt das Makropartikel-Verfahren (M-Deskriptor).
```

**Eingaben:** Gegeben, ISO-Klasse N, Gemessene Konzentration [1/m³], Partikelgröße

**Ergebnisse:** Max. Konzentration (Tabelle) [1/m³], Max. Konzentration [1/ft³], Max. Konzentration [1/l], Klassenzahl (exakt), ISO-Klasse

### Messstellen & Probenvolumen

*Klassifizierungsmessung nach ISO 14644-1* — `iso14644_sampling`

```text
Mindestanzahl Messstellen N_L nach ISO 14644-1:2015 Tabelle A.1 (Fläche in m²), über 1000 m²: N_L = 27 · A / 1000.
Mindest-Probenvolumen je Messstelle: V_s = 20 / C_n,m · 1000 [l], C_n,m = Grenzwert der größten betrachteten Partikelgröße. Mindestens 2 l und mindestens 1 min Messzeit.
```

**Eingaben:** Reinraumfläche [m²], ISO-Klasse, Größte betrachtete Partikelgröße, Volumenstrom Partikelzähler [l/min]

**Ergebnisse:** Mindestanzahl Messstellen, Mindest-Probenvolumen je Stelle [l], Mindest-Messzeit je Stelle [min], Messzeit gesamt (ohne Umsetzen) [min]

### Luftwechsel & Erholzeit

*Luftwechselrate, Volumenstrom, 100:1-Erholzeit* — `air_change`

```text
n = V̇ / V_Raum [1/h]    Erholzeit bei idealer Mischung: t = ln(k) / n
100:1 (Erholungstest ISO 14644-3): t = 4,61 / n [h] = 276 / n [min]. Reale Räume haben eine geringere Lüftungseffektivität ε – gemessene Werte sind meist länger. EU-GMP Annex 1: Erholzeit Klasse B/C „at rest“ in der Regel ≤ 15–20 min.
```

**Eingaben:** Raumfläche [m²], Raumhöhe [m], Gegeben, Zuluft-Volumenstrom [m³/h], Luftwechselrate [1/h], Lüftungseffektivität ε

**Ergebnisse:** Luftwechselrate [1/h], Zuluft-Volumenstrom [m³/h], Raumvolumen [m³], Erholzeit 100:1 [min], Erholzeit 10:1 [min]

### FFU-Decke (Filter-Fan-Units)

*Anzahl FFU, Luftmenge, Leistung, Energiekosten* — `ffu_ceiling`

```text
n_FFU = ⌈A_Raum · Belegung / A_FFU⌉    V̇ = n_FFU · A_FFU · v · 3600
Luftwechsel = V̇ / (A · H)    P_el = n_FFU · P_FFU (geht als Wärme in den Raum!)
Richtwert turbulenzarme Verdrängungsströmung (Klasse A/ISO 5): v ≈ 0,36–0,54 m/s (früherer EU-GMP-Leitwert 0,45 m/s ± 20 %).
```

**Eingaben:** Reinraumfläche [m²], Raumhöhe [m], Deckenbelegung [%], FFU-Größe (Nennmaß), Austrittsgeschwindigkeit [m/s], Leistung je FFU [W], Betriebsstunden pro Jahr [h], Strompreis pro kWh [€ / CHF]

**Ergebnisse:** Anzahl FFU, Luftvolumenstrom gesamt [m³/h], Luftwechselrate [1/h], Elektrische Leistung / Wärmelast [kW], Energie pro Jahr [MWh], Energiekosten pro Jahr [€]

### Überströmung / Druckkaskade

*Leckluft durch Türspalte bei Δp* — `pressure_cascade`

```text
Q = C_d · A · √(2 · Δp / ρ)    (Blendengleichung, C_d ≈ 0,61 für scharfkantige Spalte)
A = Spaltlänge · Spaltbreite + weitere Öffnungen. Das Ergebnis ist die Überströmluft, die die Lüftung zusätzlich liefern muss, um Δp zu halten.
EU-GMP Annex 1: Druckdifferenz zwischen angrenzenden Räumen unterschiedlicher Klassen typ. 10–15 Pa (Leitwert).
```

**Eingaben:** Spaltlänge gesamt (z. B. Türumfang) [m], Spaltbreite [mm], Weitere Öffnungen [cm²], Druckdifferenz Δp [Pa], Durchflussbeiwert C_d

**Ergebnisse:** Leckfläche [cm²], Überströmluft [m³/h], Geschwindigkeit im Spalt [m/s]

### ISO 14644-1: Klassengrenzwerte

| Klasse | 0,1 µm | 0,2 µm | 0,3 µm | 0,5 µm | 1 µm | 5 µm |
|---|---|---|---|---|---|---|
| ISO 1 | 10 | 2 | – | – | – | – |
| ISO 2 | 100 | 24 | 10 | 4 | – | – |
| ISO 3 | 1 000 | 237 | 102 | 35 | 8 | – |
| ISO 4 | 10 000 | 2 370 | 1 020 | 352 | 83 | – |
| ISO 5 | 100 000 | 23 700 | 10 200 | 3 520 | 832 | – |
| ISO 6 | 1 000 000 | 237 000 | 102 000 | 35 200 | 8 320 | 293 |
| ISO 7 | – | – | – | 352 000 | 83 200 | 2 930 |
| ISO 8 | – | – | – | 3 520 000 | 832 000 | 29 300 |
| ISO 9 | – | – | – | 35 200 000 | 8 320 000 | 293 000 |

*ISO 14644-1:2015 Tabelle 1, berechnet mit C_n = 10^N · (0,1/D)^2,08 und gerundet. „–“ = nicht anwendbar.*

### EU-GMP Annex 1: Partikelgrenzwerte

| Klasse | Ruhe ≥ 0,5 µm | Ruhe ≥ 5 µm | Betrieb ≥ 0,5 µm | Betrieb ≥ 5 µm |
|---|---|---|---|---|
| A | 3 520 | n. v. | 3 520 | 29 |
| B | 3 520 | n. v. | 352 000 | 2 930 |
| C | 352 000 | 2 930 | 3 520 000 | 29 300 |
| D | 3 520 000 | 29 300 | n. v. | n. v. |

*EU-GMP-Leitfaden Anhang 1 (2022), Tabelle 1/5 – max. zulässige Partikel/m³. n. v. = nicht vorgegeben (Klasse A/B: ≥ 5 µm im Ruhezustand nur über Trend). ISO-Entsprechung: A ≈ ISO 5, B ≈ ISO 5 (Ruhe) / ISO 7 (Betrieb), C ≈ ISO 7 / ISO 8, D ≈ ISO 8 (Ruhe). Gegen die gültige Fassung prüfen.*

### EU-GMP Annex 1: Keimgrenzwerte

| Klasse | Luft KBE/m³ | Sedimentation KBE/4 h | Kontakt KBE/Platte | Handschuh KBE/Hand |
|---|---|---|---|---|
| A | kein Wachstum | kein Wachstum | kein Wachstum | kein Wachstum |
| B | 10 | 5 | 5 | 5 |
| C | 100 | 50 | 25 | – |
| D | 200 | 100 | 50 | – |

*EU-GMP-Leitfaden Anhang 1 (2022), Tabelle 6 – Aktionsgrenzen im Betrieb. Klasse A: kein Wachstum erwartet.*

## Rechenzentrum

### PUE / Energieeffizienz

*PUE, DCiE, Overhead, Jahresenergie, EnEfG* — `dc_pue`

```text
PUE = P_gesamt / P_IT (ISO/IEC 30134-2, Jahresmittel)    DCiE = 1 / PUE · 100 %
Overhead = P_gesamt − P_IT (Kühlung, USV-Verluste, Licht …)
DE – Energieeffizienzgesetz (EnEfG 2023) §11: RZ mit Betriebsbeginn ab 1.7.2026 PUE ≤ 1,2; bestehende RZ PUE ≤ 1,5 ab 1.7.2027 und ≤ 1,3 ab 1.7.2030. Gültige Fassung prüfen.
```

**Eingaben:** Gesamtleistung RZ [kW], IT-Leistung [kW], Strompreis pro kWh [€ / CHF]

**Ergebnisse:** PUE, DCiE [%], Overhead-Leistung [kW], Energie pro Jahr gesamt [MWh], Kosten Overhead pro Jahr [€], Einsparung bei PUE 1,3 [MWh/a]

### Rack-Kühlung & Redundanz

*Luftmenge je Rack, Kühlgeräte N+1/2N* — `dc_rack_cooling`

```text
Die IT-Leistung wird vollständig zu Wärme.  V̇ = P / (ρ · c · ΔT)   (Luft: ρ = 1,2 kg/m³, c = 1,005 kJ/(kg·K))
ΔT = Temperaturdifferenz Server-Ausblas − Ansaugung (typ. 10–15 K). Anzahl Umluftkühlgeräte: N = ⌈Last / Leistung je Gerät⌉, zuzüglich Redundanz. Empfohlene Zulufttemperatur nach ASHRAE TC 9.9: 18–27 °C.
```

**Eingaben:** Anzahl Racks, Leistung je Rack [kW], ΔT über Server [K], Kühlleistung je Gerät (CRAH/CRAC) [kW], Redundanz

**Ergebnisse:** IT-Wärmelast gesamt [kW], Luftmenge je Rack [m³/h], Luftmenge je Rack [CFM], Luftmenge gesamt [m³/h], Kühlgeräte inkl. Redundanz, Installierte Kühlleistung [kW]

### USV-Dimensionierung

*Module mit Redundanz, Auslastung, Batterie-Autonomie* — `dc_ups`

```text
P_Auslegung = (P_IT + P_sonstige) · (1 + Reserve)    S = P / cos φ
Module N = ⌈P / (P_Modul · max. Auslastung)⌉, dazu Redundanz (N+1, 2N = zwei getrennte Systeme).
Batterie: E = P · t / η_WR    I_DC = P / (η_WR · U_DC). Für kurze Autonomien Batterien nach Leistungstabelle (W/Zelle) des Herstellers wählen – die Ah-Nennkapazität ist dann deutlich höher als E / U.
```

**Eingaben:** IT-Last [kW], Sonstige gesicherte Last [kW], Wachstumsreserve [%], Leistungsfaktor der Last, Modulleistung (Wirkleistung) [kW], Max. Auslastung je Modul [%], Redundanz, Autonomiezeit [min], Batterie-Nennspannung [V], Wirkungsgrad Wechselrichter [%]

**Ergebnisse:** Auslegungsleistung [kW], Scheinleistung [kVA], Module ohne Redundanz (N), Module gesamt inkl. Redundanz, Auslastung je Modul (Normalbetrieb) [%], Batterieenergie (Entnahme) [kWh], Batterie-Entladestrom [A]

### Verfügbarkeit & Ausfallzeit

*Prozent ↔ Ausfallzeit pro Jahr, Tier-Vergleich* — `dc_availability`

```text
Ausfallzeit = (1 − A) · 8760 h    Serie: A = A1 · A2    Parallel (redundant): A = 1 − (1 − A1)·(1 − A2)
Häufig zitierte Richtwerte: Tier I 99,671 %, Tier II 99,741 %, Tier III 99,982 %, Tier IV 99,995 % (keine offiziellen Uptime-Institute-Kriterien).
```

**Eingaben:** Verfügbarkeit einer Komponente/Strecke [%], Anzahl paralleler (redundanter) Strecken

**Ergebnisse:** Gesamtverfügbarkeit [%], Ausfallzeit pro Jahr [h], Ausfallzeit pro Jahr [min], Ausfallzeit pro Monat [min]

### ASHRAE TC 9.9: Klimaklassen IT

| Bereich | Temperatur | Feuchte | Max. Taupunkt |
|---|---|---|---|
| Empfohlen (A1–A4) | 18–27 °C | TP −9 °C … 15 °C, ≤ 60 % r. F. | 15 °C |
| Zulässig A1 | 15–32 °C | 8–80 % r. F. | 17 °C |
| Zulässig A2 | 10–35 °C | 8–80 % r. F. | 21 °C |
| Zulässig A3 | 5–40 °C | 8–85 % r. F. | 24 °C |
| Zulässig A4 | 5–45 °C | 8–90 % r. F. | 24 °C |

*ASHRAE Thermal Guidelines for Data Processing Environments (4./5. Auflage). Werte am Lufteintritt der IT-Geräte; gültige Ausgabe und Herstellerangaben prüfen.*

### Verfügbarkeitsklassen EN 50600 / Tier

| EN 50600 | Vergleich | Aufbau | Eigenschaft |
|---|---|---|---|
| Klasse 1 | Tier I | ein Versorgungspfad, keine Redundanz | geringe Verfügbarkeit |
| Klasse 2 | Tier II | ein Pfad, redundante Komponenten (N+1) | mittlere Verfügbarkeit |
| Klasse 3 | Tier III | mehrere Pfade, ein aktiver | Wartung im laufenden Betrieb |
| Klasse 4 | Tier IV | mehrere aktive Pfade (2N) | fehlertolerant |

*EN 50600-1 Verfügbarkeitsklassen 1–4; Tier I–IV nach Uptime Institute zum Vergleich (nicht identisch).*

## Photovoltaik & Speicher

### PV-String-Auslegung

*Module pro String für den Wechselrichter, Strangsicherung* — `pv_string`

```text
X(T) = X_STC · (1 + β · (T − 25 °C))   für U_oc und U_mpp (β negativ, Datenblatt)
n_max = min(⌊U_DC,max / U_oc(T_min)⌋, ⌊U_MPP,max / U_mpp(T_min)⌋)    n_min = ⌈U_MPP,min / U_mpp(T_max)⌉
T_min = tiefste Zelltemperatur (DE typ. −10 °C, Höhenlagen/CH bis −20 °C), T_max = Zelltemperatur im Sommer (typ. 70 °C).
Strangsicherung nötig, wenn (N_p − 1) · 1,25 · I_sc > Rückstrombelastbarkeit des Moduls (IEC 62548 / VDE 0100-712); Bemessung dann 1,5 · I_sc < I_n ≤ 2,4 · I_sc und ≤ Rückstromwert.
```

**Eingaben:** Modul U_oc (STC) [V], Modul U_mpp (STC) [V], Modul I_sc (STC) [A], Modulleistung [Wp], Temp.-Koeffizient U_oc [%/K], Temp.-Koeffizient U_mpp (≈ P_max) [%/K], Min. Zelltemperatur [°C], Max. Zelltemperatur [°C], WR max. DC-Eingangsspannung [V], WR MPP-Bereich min. [V], WR MPP-Bereich max. [V], WR max. I_SC je MPPT [A], Parallele Strings je MPPT, Rückstrombelastbarkeit Modul [A], Geplante Module pro String

**Ergebnisse:** U_oc je Modul bei T_min [V], U_mpp je Modul bei T_max [V], Module pro String, String-U_oc bei T_min (geplant) [V], String-U_mpp bei T_max (geplant) [V], Leistung je String (geplant) [kWp], Strom je MPPT (1,25 · I_sc · N_p) [A], Strangsicherung

### PV-Ertrag & Wirtschaftlichkeit

*Jahresertrag, Eigenverbrauch, Ersparnis, Amortisation* — `pv_yield`

```text
E = P_peak · spez. Ertrag    Ersparnis = E · EV-Anteil · Strompreis    Einspeisung = E · (1 − EV-Anteil) · Vergütung
Amortisation (statisch) = Investition / jährlicher Nutzen – ohne Zinsen, Degradation, Wartung.
Spezifischer Ertrag z. B. aus PVGIS: DE typ. 850–1100 kWh/kWp, CH-Mittelland ca. 950–1100 kWh/kWp, Alpen höher.
```

**Eingaben:** Anlagenleistung [kWp], Spezifischer Ertrag [kWh/kWp], Eigenverbrauchsanteil [%], Strompreis Bezug [€ / CHF], Einspeisevergütung [€ / CHF], Investition [€ / CHF], CO₂-Faktor Netzstrom [g/kWh]

**Ergebnisse:** Jahresertrag [kWh], Davon Eigenverbrauch [kWh], Ersparnis Eigenverbrauch [€], Einspeiseerlös [€], Nutzen pro Jahr [€], Amortisation (statisch) [a], CO₂-Einsparung pro Jahr [t]

### PV-DC-Leitung: Verluste

*Leistungsverlust und Mindestquerschnitt String-Leitung* — `pv_dc_cable`

```text
P_V = 2 · L · I² · ρ / A    Verlust % = P_V / (U_mpp · I) · 100    A_min = 2 · L · I · ρ / (v · U_mpp)
Üblich: DC-Verlust ≤ 1 %. Leitertemperatur 70 °C (Dach). Belastbarkeit der Solarleitung (H1Z2Z2-K, EN 50618) nach Datenblatt prüfen.
```

**Eingaben:** String-Spannung U_mpp [V], String-Strom I_mpp [A], Leitungslänge (einfach) [m], Querschnitt, Zul. Verlust [%]

**Ergebnisse:** Leistungsverlust [W], Verlust [%], Mindestquerschnitt für Zielwert [mm²], Energieverlust pro Jahr (≈ 1000 Vollaststunden) [kWh]

### Batteriespeicher dimensionieren

*Kapazität aus Nachtverbrauch und Faustformeln* — `pv_storage`

```text
C_nenn = E_Nacht / (DoD · η_Entladung)
Faustformeln (z. B. HTW Berlin, Eigenheim): ca. 1,0–1,5 kWh Speicher je 1000 kWh Jahresverbrauch und nicht mehr als ca. 1,5 kWh je kWp. Größere Speicher erhöhen die Autarkie nur noch wenig.
```

**Eingaben:** Verbrauch abends/nachts [kWh/d], Nutzbare Entladetiefe (DoD) [%], Wirkungsgrad Entladung [%], Jahresverbrauch [kWh], PV-Leistung [kWp]

**Ergebnisse:** Nennkapazität für Nachtbedarf [kWh], Faustformel Verbrauch, Obergrenze nach PV-Leistung [kWh]

## Elektronik

### Farbcode Widerstand / Spule

*4- und 5-Ring-Code nach IEC 60062* — `resistor_color`

```text
4 Ringe: Ziffer, Ziffer, Multiplikator, Toleranz
5 Ringe: Ziffer, Ziffer, Ziffer, Multiplikator, Toleranz
Induktivitäten: gleiche Codierung, Grundeinheit µH.
```

**Eingaben:** Bauteil, Anzahl Ringe, 1. Ring (Ziffer), 2. Ring (Ziffer), 3. Ring (Ziffer), Multiplikator, Toleranz

**Ergebnisse:** Nennwert [Ω], Toleranz [%], Minimalwert [Ω], Maximalwert [Ω]

### E-Reihe: Normwert finden

*Nächster Wert in E6 … E96 (IEC 60063)* — `e_series`

```text
Sucht den nächstgelegenen Normwert (logarithmischer Abstand) sowie den nächstkleineren und -größeren Wert.
```

**Eingaben:** Wert [Ω], Reihe

**Ergebnisse:** Nächster Normwert [Ω], Abweichung [%], Nächstkleiner [Ω], Nächstgrößer [Ω]

### LED-Vorwiderstand

*R = (U_B − n · U_F) / I_F* — `led_resistor`

```text
R = (U_B − n · U_F) / I_F    P_R = I² · R
Gewählt wird der nächstgrößere E24-Wert; Belastbarkeit ≥ 2 · P_R wählen.
Typ. U_F: rot 1,8–2,1 V, grün 2,0–3,2 V, blau/weiß 2,8–3,4 V.
```

**Eingaben:** Versorgungsspannung U_B [V], Durchlassspannung U_F [V], LED-Strom I_F [mA], LEDs in Reihe

**Ergebnisse:** Berechneter Widerstand [Ω], Gewählt (E24) [Ω], Strom mit E24-Wert [mA], Verlustleistung [mW], Empfohlene Belastbarkeit ≥ [W]

### Spannungsteiler

*U_aus unbelastet und belastet* — `voltage_divider`

```text
U_aus = U_ein · R2 / (R1 + R2)
Belastet: R2 ∥ R_L statt R2.
```

**Eingaben:** Eingangsspannung U_ein [V], R1 [kΩ], R2 [kΩ], Lastwiderstand R_L [kΩ]

**Ergebnisse:** U_aus unbelastet [V], U_aus belastet [V], Querstrom [mA], Teilerverhältnis, Verlustleistung R1 [mW]

### RC-Glied

*τ, Grenzfrequenz, Ladekurve, Energie* — `rc`

```text
τ = R · C    f_g = 1 / (2π · R · C)    u_C(t) = U_0 · (1 − e^(−t/τ))
Nach 5τ ≈ 99,3 % geladen.    E = ½ · C · U²    Q = C · U
```

**Eingaben:** Widerstand R [kΩ], Kapazität C [µF], Ladespannung U_0 [V], Zeit t [ms]

**Ergebnisse:** Zeitkonstante τ [ms], 5 τ [ms], Grenzfrequenz f_g [Hz], Spannung u_C(t) [V], Gespeicherte Energie [mJ], Ladung Q [µC]

### LC-Schwingkreis

*Resonanzfrequenz f₀ = 1 / (2π√(LC))* — `lc_resonance`

```text
f₀ = 1 / (2π · √(L · C))    Z₀ = √(L / C)
```

**Eingaben:** Induktivität L [mH], Kapazität C [µF]

**Ergebnisse:** Resonanzfrequenz f₀ [Hz], Kreisfrequenz ω₀ [1/s], Kennwiderstand Z₀ [Ω]

### Blindwiderstand & Impedanz (RLC Reihe)

*X_L, X_C, Z und φ bei Frequenz f* — `reactance`

```text
X_L = 2π · f · L    X_C = 1 / (2π · f · C)    Z = √(R² + (X_L − X_C)²)    φ = arctan((X_L − X_C)/R)
Leere Felder = Bauteil nicht vorhanden.
```

**Eingaben:** Frequenz f [Hz], Widerstand R [Ω], Induktivität L [mH], Kapazität C [µF]

**Ergebnisse:** Induktiver Blindwiderstand X_L [Ω], Kapazitiver Blindwiderstand X_C [Ω], Impedanz Z [Ω], Phasenwinkel φ [°]

### Akku-/USV-Laufzeit

*Überbrückungszeit aus Ah, V und Last* — `battery_runtime`

```text
E_nutz = U · C · DoD    t = E_nutz · η / P_Last
Vereinfachung ohne Peukert-Effekt und Alterung – Herstellerkurven beachten.
```

**Eingaben:** Batteriespannung [V], Kapazität [Ah], Lastleistung [W], Wirkungsgrad Wandler [%], Nutzbare Entladetiefe [%]

**Ergebnisse:** Nutzbare Energie [Wh], Batteriestrom [A], Laufzeit [h], Laufzeit [min]

## Umrechner

### Leistung

*W, kW, PS, hp, BTU/h, kcal/h, TR* — `conv_power`

```text
Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen (PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Watt [W], Kilowatt [kW], Megawatt [MW], Pferdestärke (metrisch) [PS], Horsepower (mechanisch) [hp], BTU pro Stunde [BTU/h], Kilokalorie pro Stunde [kcal/h], Kältetonne (Tonne of Refrigeration) [TR]

### Energie

*J, Wh, kWh, kcal, BTU* — `conv_energy`

```text
Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen (PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Joule [J], Kilojoule [kJ], Megajoule [MJ], Wattstunde [Wh], Kilowattstunde [kWh], Megawattstunde [MWh], Kilokalorie [kcal], British Thermal Unit [BTU]

### Temperatur

*°C ↔ K ↔ °F* — `conv_temperature`

```text
T[K] = θ[°C] + 273,15    θ[°F] = θ[°C] · 9/5 + 32
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Celsius [°C], Kelvin [K], Fahrenheit [°F]

### Druck

*Pa, kPa, bar, mbar, psi, mWS* — `conv_pressure`

```text
Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen (PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Pascal [Pa], Hektopascal [hPa], Kilopascal [kPa], Millibar [mbar], Bar [bar], Pound per square inch [psi], Meter Wassersäule [mWS], Millimeter Quecksilbersäule [mmHg]

### Volumenstrom

*m³/h, l/s, l/min, CFM, gpm* — `conv_flow`

```text
Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen (PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Kubikmeter pro Stunde [m³/h], Kubikmeter pro Sekunde [m³/s], Liter pro Sekunde [l/s], Liter pro Minute [l/min], Liter pro Stunde [l/h], Cubic feet per minute [CFM], US-Gallonen pro Minute [gpm (US)]

### AWG ↔ mm²

*American Wire Gauge in metrischen Querschnitt* — `conv_awg`

```text
d = 0,127 mm · 92^((36 − AWG)/39)    A = π/4 · d²
AWG 1/0 = 0, 2/0 = −1, 3/0 = −2, 4/0 = −3 eingeben.
```

**Eingaben:** Gegeben, AWG, Querschnitt [mm²]

**Ergebnisse:** AWG, Durchmesser [mm], Querschnitt [mm²], Nächster metrischer Normquerschnitt ≥ [mm²]

## Tabellen & Pinouts

### IP-Schutzart – 1. Ziffer

| Ziffer | Fremdkörperschutz | Berührungsschutz |
|---|---|---|
| 0 | kein Schutz | kein Schutz |
| 1 | ≥ 50 mm | Handrücken |
| 2 | ≥ 12,5 mm | Finger |
| 3 | ≥ 2,5 mm | Werkzeug |
| 4 | ≥ 1,0 mm | Draht |
| 5 | staubgeschützt | Draht |
| 6 | staubdicht | Draht |

*IEC 60529 / EN 60529 (DIN VDE 0470-1)*

### IP-Schutzart – 2. Ziffer

| Ziffer | Schutz gegen |
|---|---|
| 0 | kein Schutz |
| 1 | senkrecht fallendes Tropfwasser |
| 2 | Tropfwasser bis 15° Neigung |
| 3 | Sprühwasser bis 60° gegen die Senkrechte |
| 4 | Spritzwasser aus allen Richtungen |
| 5 | Strahlwasser (Düse 6,3 mm) |
| 6 | starkes Strahlwasser (Düse 12,5 mm) |
| 7 | zeitweiliges Untertauchen (1 m, 30 min) |
| 8 | dauerndes Untertauchen (nach Vereinbarung) |
| 9 | Hochdruck-/Dampfstrahl (80 °C) |

*IEC 60529 inkl. Änderung 2 (2013) für IPX9.*

### Schutzklassen

| Klasse | Schutzmaßnahme | Kennzeichen / Hinweis |
|---|---|---|
| 0 | nur Basisisolierung | in DE/CH nicht zulässig |
| I | Basisisolierung + Schutzleiteranschluss | Schutzleiterzeichen ⏚ |
| II | doppelte oder verstärkte Isolierung | Doppelquadrat ⧈, kein PE |
| III | Schutzkleinspannung SELV/PELV | ≤ 50 V AC / 120 V DC |

*IEC 61140 / DIN EN 61140 (VDE 0140-1)*

### Netzsysteme

| System | Aufbau | Hinweis |
|---|---|---|
| TN-C | N und PE als PEN kombiniert | PEN ≥ 10 mm² Cu; EMV-ungünstig |
| TN-S | N und PE getrennt im ganzen Netz | EMV-günstig, Standard für Neuanlagen |
| TN-C-S | PEN-Aufteilung an einem Punkt, danach getrennt | nach Aufteilung nie wieder verbinden |
| TT | Körper über eigenen Anlagenerder | Fehlerschutz meist mit RCD |
| IT | Quelle isoliert oder hochohmig geerdet | Isolationsüberwachung, Betrieb bei 1. Fehler (Medizin, Industrie) |

*IEC 60364-1 / DIN VDE 0100-100 / NIN 3.1.2. 1. Buchstabe: Erdung der Stromquelle, 2. Buchstabe: Erdung der Körper.*

### Aderfarben

| Leiter / Kreis | Farbe |
|---|---|
| Außenleiter L1 | braun |
| Außenleiter L2 | schwarz |
| Außenleiter L3 | grau |
| Neutralleiter N | blau |
| Schutzleiter PE | grün-gelb |
| PEN-Leiter | grün-gelb + blaue Markierung an den Enden |
| Maschine: Hauptstromkreise AC/DC | schwarz |
| Maschine: AC-Steuerstromkreise | rot |
| Maschine: DC-Steuerstromkreise | blau |
| Maschine: Fremdspannung (bleibt bei Hauptschalter AUS) | orange |

*Kennzeichnung nach IEC 60445 (DIN EN 60445 / VDE 0197); Maschinen nach IEC 60204-1 Abschn. 13.2 (Empfehlung).*

### Spannungsfall-Grenzwerte DE/CH

| Land | Regel | Bereich | ΔU max |
|---|---|---|---|
| DE | DIN 18015-1 | Zähler bis Verbraucher (Wohngebäude) | 3 % |
| DE | VDE-AR-N 4100 (TAB) | Hauptleitung bis Zähler | 0,5 % (≤ 100 kVA) |
| DE | DIN VDE 0100-520 Anh. G | öff. Netz – Beleuchtung / sonstige | 3 % / 5 % |
| DE | DIN VDE 0100-520 Anh. G | eigene Trafostation – Beleuchtung / sonstige | 6 % / 8 % |
| CH | NIN | Anschlussüberstromunterbrecher bis Verbraucher | 4 % |

*Übersicht – maßgeblich sind die gültigen Normausgaben und die TAB des Netzbetreibers.*

### Strombelastbarkeit Cu PVC 70 °C, 2 belastete Adern

| mm² | A1 | A2 | B1 | B2 | C | D1 | E |
|---|---|---|---|---|---|---|---|
| 1,5 | 14,5 | 14 | 17,5 | 16,5 | 19,5 | 22 | 22 |
| 2,5 | 19,5 | 18,5 | 24 | 23 | 27 | 29 | 30 |
| 4 | 26 | 25 | 32 | 30 | 36 | 37 | 40 |
| 6 | 34 | 32 | 41 | 38 | 46 | 46 | 51 |
| 10 | 46 | 43 | 57 | 52 | 63 | 60 | 70 |
| 16 | 61 | 57 | 76 | 69 | 85 | 78 | 94 |
| 25 | 80 | 75 | 101 | 90 | 112 | 99 | 119 |
| 35 | 99 | 92 | 125 | 111 | 138 | 119 | 148 |
| 50 | 119 | 110 | 151 | 133 | 168 | 140 | 180 |
| 70 | 151 | 139 | 192 | 168 | 213 | 173 | 232 |
| 95 | 182 | 167 | 232 | 201 | 258 | 204 | 282 |
| 120 | 210 | 192 | 269 | 232 | 299 | 231 | 328 |
| 150 | 240 | 219 | 300 | 258 | 344 | 261 | 379 |
| 185 | 273 | 248 | 341 | 294 | 392 | 292 | 434 |
| 240 | 321 | 291 | 400 | 344 | 461 | 336 | 514 |
| 300 | 367 | 334 | 458 | 394 | 530 | 379 | 593 |

*IEC 60364-5-52 Tabelle B.52.2 (Methode E: B.52.10/11); entspricht DIN VDE 0298-4 / NIN 5.2.3. Gegen gültige Ausgabe prüfen.*

### Strombelastbarkeit Cu PVC 70 °C, 3 belastete Adern

| mm² | A1 | A2 | B1 | B2 | C | D1 | E |
|---|---|---|---|---|---|---|---|
| 1,5 | 13,5 | 13 | 15,5 | 15 | 17,5 | 18 | 18,5 |
| 2,5 | 18 | 17,5 | 21 | 20 | 24 | 24 | 25 |
| 4 | 24 | 23 | 28 | 27 | 32 | 30 | 34 |
| 6 | 31 | 29 | 36 | 34 | 41 | 38 | 43 |
| 10 | 42 | 39 | 50 | 46 | 57 | 50 | 60 |
| 16 | 56 | 52 | 68 | 62 | 76 | 64 | 80 |
| 25 | 73 | 68 | 89 | 80 | 96 | 82 | 101 |
| 35 | 89 | 83 | 110 | 99 | 119 | 98 | 126 |
| 50 | 108 | 99 | 134 | 118 | 144 | 116 | 153 |
| 70 | 136 | 125 | 171 | 149 | 184 | 143 | 196 |
| 95 | 164 | 150 | 207 | 179 | 223 | 169 | 238 |
| 120 | 188 | 172 | 239 | 206 | 259 | 192 | 276 |
| 150 | 216 | 196 | 262 | 225 | 299 | 217 | 319 |
| 185 | 245 | 223 | 296 | 255 | 341 | 243 | 364 |
| 240 | 286 | 261 | 346 | 297 | 403 | 280 | 430 |
| 300 | 328 | 298 | 394 | 339 | 464 | 316 | 497 |

*IEC 60364-5-52 Tabelle B.52.4 (Methode E: B.52.10/11); entspricht DIN VDE 0298-4 / NIN 5.2.3. Gegen gültige Ausgabe prüfen.*

### Strombelastbarkeit Cu VPE 90 °C, 2 belastete Adern

| mm² | A1 | A2 | B1 | B2 | C | D1 | E |
|---|---|---|---|---|---|---|---|
| 1,5 | 19 | 18,5 | 23 | 22 | 24 | 25 | 26 |
| 2,5 | 26 | 25 | 31 | 30 | 33 | 33 | 36 |
| 4 | 35 | 33 | 42 | 40 | 45 | 43 | 49 |
| 6 | 45 | 42 | 54 | 51 | 58 | 53 | 63 |
| 10 | 61 | 57 | 75 | 69 | 80 | 71 | 86 |
| 16 | 81 | 76 | 100 | 91 | 107 | 91 | 115 |
| 25 | 106 | 99 | 133 | 119 | 138 | 116 | 149 |
| 35 | 131 | 121 | 164 | 146 | 171 | 139 | 185 |
| 50 | 158 | 145 | 198 | 175 | 209 | 164 | 225 |
| 70 | 200 | 183 | 253 | 221 | 269 | 203 | 289 |
| 95 | 241 | 220 | 306 | 265 | 328 | 239 | 352 |
| 120 | 278 | 253 | 354 | 305 | 382 | 271 | 410 |
| 150 | 318 | 290 | 393 | 334 | 441 | 306 | 473 |
| 185 | 362 | 329 | 449 | 384 | 506 | 343 | 542 |
| 240 | 424 | 386 | 528 | 459 | 599 | 395 | 641 |
| 300 | 486 | 442 | 603 | 532 | 693 | 446 | 741 |

*IEC 60364-5-52 Tabelle B.52.3 (Methode E: B.52.10/11); entspricht DIN VDE 0298-4 / NIN 5.2.3. Gegen gültige Ausgabe prüfen.*

### Strombelastbarkeit Cu VPE 90 °C, 3 belastete Adern

| mm² | A1 | A2 | B1 | B2 | C | D1 | E |
|---|---|---|---|---|---|---|---|
| 1,5 | 17 | 16,5 | 20 | 19,5 | 22 | 21 | 23 |
| 2,5 | 23 | 22 | 28 | 26 | 30 | 28 | 32 |
| 4 | 31 | 30 | 37 | 35 | 40 | 36 | 42 |
| 6 | 40 | 38 | 48 | 44 | 52 | 44 | 54 |
| 10 | 54 | 51 | 66 | 60 | 71 | 58 | 75 |
| 16 | 73 | 68 | 88 | 80 | 96 | 75 | 100 |
| 25 | 95 | 89 | 117 | 105 | 119 | 96 | 127 |
| 35 | 117 | 109 | 144 | 128 | 147 | 115 | 158 |
| 50 | 141 | 130 | 175 | 154 | 179 | 135 | 192 |
| 70 | 179 | 164 | 222 | 194 | 229 | 167 | 246 |
| 95 | 216 | 197 | 269 | 233 | 278 | 197 | 298 |
| 120 | 249 | 227 | 312 | 268 | 322 | 223 | 346 |
| 150 | 285 | 259 | 342 | 300 | 371 | 251 | 399 |
| 185 | 324 | 295 | 384 | 340 | 424 | 281 | 456 |
| 240 | 380 | 346 | 450 | 398 | 500 | 324 | 538 |
| 300 | 435 | 396 | 514 | 455 | 576 | 365 | 621 |

*IEC 60364-5-52 Tabelle B.52.5 (Methode E: B.52.10/11); entspricht DIN VDE 0298-4 / NIN 5.2.3. Gegen gültige Ausgabe prüfen.*

### Umrechnungsfaktoren Temperatur

| θ_a | Luft PVC | Luft VPE | Erde PVC | Erde VPE |
|---|---|---|---|---|
| 10 °C | 1,22 | 1,15 | 1,1 | 1,07 |
| 15 °C | 1,17 | 1,12 | 1,05 | 1,04 |
| 20 °C | 1,12 | 1,08 | 1 | 1 |
| 25 °C | 1,06 | 1,04 | 0,95 | 0,96 |
| 30 °C | 1 | 1 | 0,89 | 0,93 |
| 35 °C | 0,94 | 0,96 | 0,84 | 0,89 |
| 40 °C | 0,87 | 0,91 | 0,77 | 0,85 |
| 45 °C | 0,79 | 0,87 | 0,71 | 0,8 |
| 50 °C | 0,71 | 0,82 | 0,63 | 0,76 |
| 55 °C | 0,61 | 0,76 | 0,55 | 0,71 |
| 60 °C | 0,5 | 0,71 | 0,45 | 0,65 |
| 65 °C | 0,35 | 0,65 | 0,32 | 0,6 |
| 70 °C | – | 0,58 | – | 0,53 |
| 75 °C | – | 0,5 | – | 0,46 |
| 80 °C | – | 0,41 | – | 0,38 |

*f_T = √((θ_max − θ_a)/(θ_max − θ_ref)) – entspricht IEC 60364-5-52 Tab. B.52.14 / B.52.15.*

### Häufungsfaktoren

| Anzahl | gebündelt | Wand | Decke | gelochte Rinne | Leiter |
|---|---|---|---|---|---|
| 1 | 1 | 1 | 0,95 | 1 | 1 |
| 2 | 0,8 | 0,85 | 0,81 | 0,88 | 0,87 |
| 3 | 0,7 | 0,79 | 0,72 | 0,82 | 0,82 |
| 4 | 0,65 | 0,75 | 0,68 | 0,77 | 0,8 |
| 5 | 0,6 | 0,73 | 0,66 | 0,75 | 0,8 |
| 6 | 0,57 | 0,72 | 0,64 | 0,73 | 0,79 |
| 7 | 0,54 | 0,72 | 0,63 | 0,73 | 0,79 |
| 8 | 0,52 | 0,71 | 0,62 | 0,72 | 0,78 |
| 9 | 0,5 | 0,7 | 0,61 | 0,72 | 0,78 |
| 12 | 0,45 | 0,7 | 0,61 | 0,72 | 0,78 |
| 16 | 0,41 | 0,7 | 0,61 | 0,72 | 0,78 |
| 20 | 0,38 | 0,7 | 0,61 | 0,72 | 0,78 |

*IEC 60364-5-52 Tabelle B.52.17 (mehradrige Kabel/Stromkreise).*

### LS-Schalter-Charakteristiken

| Typ | Sofortauslösung | Einsatz |
|---|---|---|
| B | 3 … 5 · I_n | Leitungsschutz, ohmsche Lasten, Wohnbau |
| C | 5 … 10 · I_n | Motoren, Leuchten, Trafos, Industrie |
| D | 10 … 20 · I_n | hohe Einschaltströme (Trafos, Ventile) |
| K | 8 … 14 · I_n | Motoren/Trafos, enge thermische Auslösung |
| Z | 2 … 3 · I_n | Halbleiter, Messkreise |

*B/C/D nach IEC 60898-1: Nichtauslösestrom 1,13 · I_n, Auslösestrom 1,45 · I_n (< 1 h). K/Z nach IEC 60947-2 bzw. Herstellerangaben.*

### D-Sicherungen: Kennmelder

| I_n | Farbe |
|---|---|
| 2 A | rosa |
| 4 A | braun |
| 6 A | grün |
| 10 A | rot |
| 13 A | schwarz |
| 16 A | grau |
| 20 A | blau |
| 25 A | gelb |
| 35 A | schwarz |
| 50 A | weiß |
| 63 A | kupfer |
| 80 A | silber |
| 100 A | rot |

*Kennmelderfarben nach DIN VDE 0636 / IEC 60269-3.*

### Kabel- & Leitungstypen

| Typ | Aufbau | U_0/U | Einsatz |
|---|---|---|---|
| NYM-J | PVC-Mantelleitung | 300/500 V | innen, auf/unter Putz, nicht im Erdreich |
| NYY-J | PVC-Kabel | 0,6/1 kV | Erdreich, außen, Kabelrinne |
| NYCWY | Kabel mit konz. Leiter | 0,6/1 kV | EMV-günstige Einspeisung, Erdreich |
| N2XH-J | VPE, halogenfrei | 0,6/1 kV | öffentl. Gebäude, Rechenzentren |
| NHXH FE180 E30/E90 | halogenfrei mit Funktionserhalt | 0,6/1 kV | Sicherheitsanlagen, RWA, Notlicht |
| H07RN-F | Gummischlauchleitung | 450/750 V | Baustelle, außen, flexible Anschlüsse |
| H05VV-F | PVC-Schlauchleitung | 300/500 V | Geräteanschluss, leichte Beanspruchung |
| H07V-K | PVC-Aderleitung feindrähtig | 450/750 V | Schaltschrankverdrahtung, im Rohr |
| J-Y(St)Y n×2×0,8 | Fernmelde-Installationskabel, Folienschirm | 300 V Spitze | Busleitungen (KNX grün), Signale; kein Starkstrom |
| LiYCY | Steuerleitung, Cu-Geflechtschirm | Steuer-/Signal | Analogsignale, Fühler, EMV |

*Kurzübersicht – Datenblatt des Herstellers ist maßgeblich.*

### Feldbusse: Längen & Teilnehmer

| Bus | Physik | Max. Länge | Teilnehmer | Datenrate | Hinweis |
|---|---|---|---|---|---|
| BACnet MS/TP | RS-485 | 1200 m je Segment | 32 Unit Loads/Segment, MAC 0–127 (Master) | 9,6–115,2 kbit/s | 120 Ω beidseitig, Bias einmal |
| Modbus RTU | RS-485 | ca. 1000–1200 m (baudratenabh.) | 32/Segment, Adressen 1–247 | 9,6–115,2 kbit/s | 120 Ω beidseitig; A/B-Bezeichnung prüfen |
| KNX TP1 | 2-Draht, freie Topologie | Linie 1000 m; 350 m Netzteil–TN; 700 m TN–TN | 64 je Liniensegment | 9,6 kbit/s | SELV 30 V DC, kein Abschluss |
| DALI | 2-Draht, frei | 300 m | 64 Betriebsgeräte | 1,2 kbit/s | max. 2 V Spannungsfall, 250 mA |
| LON FT-10 | 2-Draht, frei oder Bus | frei 500 m, Bus 2700 m | 64 je Segment | 78 kbit/s | Terminator: frei 1×, Bus 2× |
| PROFIBUS DP | RS-485 | 1200 m (≤ 93,75 kbit/s) … 100 m (12 Mbit/s) | 32/Segment, 126 Adressen | 9,6 kbit/s – 12 Mbit/s | aktiver Abschluss beidseitig |
| CAN / CANopen | 2-Draht, Linie | 25 m (1 Mbit/s) … 1000 m (50 kbit/s) | bis 127 (CANopen) | 10 kbit/s – 1 Mbit/s | 120 Ω beidseitig |
| IO-Link | Punkt-zu-Punkt, 3/5-Leiter | 20 m | 1 Gerät je Port | 4,8/38,4/230,4 kbit/s | ungeschirmte Standardleitung, M12 |
| Ethernet (BACnet/IP, Modbus TCP, PROFINET) | Twisted Pair, Stern | 100 m je Kupferstrecke | – | 100 Mbit/s + | Cat 5e oder besser |

*Typische Grenzwerte ohne Repeater laut jeweiliger Spezifikation – Herstellerangaben und Kabeltyp beachten.*

### Isolierstoffklassen

| Klasse | Max. Temperatur | Grenzübertemperatur Motor |
|---|---|---|
| Y | 90 °C | – |
| A | 105 °C | 60 K |
| E | 120 °C | 75 K |
| B | 130 °C | 80 K |
| F | 155 °C | 105 K |
| H | 180 °C | 125 K |
| N | 200 °C | – |
| R | 220 °C | – |

*IEC 60085 (Wärmeklassen); Grenzübertemperatur Wicklung nach IEC 60034-1 (Widerstandsmethode, Kühlmittel 40 °C).*

### Motor-Betriebsarten S1–S10

| Art | Bezeichnung |
|---|---|
| S1 | Dauerbetrieb |
| S2 | Kurzzeitbetrieb |
| S3 | periodischer Aussetzbetrieb |
| S4 | Aussetzbetrieb mit Einfluss des Anlaufs |
| S5 | Aussetzbetrieb mit elektrischer Bremsung |
| S6 | ununterbrochener periodischer Betrieb |
| S7 | ununterbrochener Betrieb mit elektrischer Bremsung |
| S8 | ununterbrochener Betrieb mit Last-/Drehzahländerungen |
| S9 | Betrieb mit nichtperiodischen Last- und Drehzahländerungen |
| S10 | Betrieb mit einzelnen konstanten Belastungen |

*Betriebsarten nach IEC 60034-1 / DIN EN 60034-1.*

### Motor-Effizienzklassen IE

| Klasse | Bezeichnung |
|---|---|
| IE1 | Standard-Wirkungsgrad |
| IE2 | Hoher Wirkungsgrad |
| IE3 | Premium-Wirkungsgrad |
| IE4 | Super-Premium-Wirkungsgrad |
| IE5 | Ultra-Premium-Wirkungsgrad |

*IEC 60034-30-1; EU-Verordnung 2019/1781 (Stand: IE3 ab 0,75–1000 kW seit 07/2021, IE4 für 75–200 kW seit 07/2023). In der Schweiz gelten vergleichbare Anforderungen (EnEV).*

### Motor-Klemmbrett: Stern / Dreieck

| Thema | Ausführung |
|---|---|
| Stern (Y) | eine waagerechte Brücke W2–U2–V2, Netz an U1/V1/W1 |
| Dreieck (Δ) | drei senkrechte Brücken U1–W2, V1–U2, W1–V2 |
| Motor 230/400 V | an 400-V-Netz in Stern |
| Motor 400/690 V | an 400-V-Netz in Dreieck (Y-Δ-Anlauf möglich) |
| Rechtslauf | L1→U1, L2→V1, L3→W1 (Blick auf Wellenende) |
| Drehrichtungsumkehr | zwei Außenleiter tauschen |

*Klemmenbezeichnung nach IEC 60034-8. Klemmbrett oben: W2 U2 V2, unten: U1 V1 W1.*

### Motor-Bauformen (IM)

| Bauform | Beschreibung |
|---|---|
| IM B3 | Fußausführung, Welle waagerecht |
| IM B5 | Flansch (FF, Durchgangslöcher), waagerecht |
| IM B14 | Normflansch (FT, Gewindelöcher), waagerecht |
| IM B35 | Fuß + Flansch FF, waagerecht |
| IM V1 | Flansch FF, Welle nach unten |
| IM V3 | Flansch FF, Welle nach oben |

*Häufige Bauformen nach IEC 60034-7.*

### Pinout RJ45 / Ethernet

| Pin | T568A | T568B | 10/100BASE-T | 1000BASE-T | PoE Mode B |
|---|---|---|---|---|---|
| 1 | weiß-grün | weiß-orange | TX+ | BI_DA+ | – |
| 2 | grün | orange | TX− | BI_DA− | – |
| 3 | weiß-orange | weiß-grün | RX+ | BI_DB+ | – |
| 4 | blau | blau | – | BI_DC+ | DC + |
| 5 | weiß-blau | weiß-blau | – | BI_DC− | DC + |
| 6 | orange | grün | RX− | BI_DB− | – |
| 7 | weiß-braun | weiß-braun | – | BI_DD+ | DC − |
| 8 | braun | braun | – | BI_DD− | DC − |

*TIA-568, IEEE 802.3. Patchkabel: beide Seiten gleich (meist T568B). PoE Mode A nutzt Paare 1/2 und 3/6.*

### RS-485: Modbus RTU / BACnet MS/TP

| Leitung | Modbus | Übliche Bezeichnungen | Funktion |
|---|---|---|---|
| D1 | B / B' | B, +, Data+, TxD+/RxD+ | nicht invertierte Leitung |
| D0 | A / A' | A, −, Data−, TxD−/RxD− | invertierte Leitung |
| Common | C / C' | GND, 0 V, SC | Bezugspotential (mitführen!) |
| Abschluss | 120 Ω | an beiden Enden | zwischen D1 und D0 |
| Schirm | – | SH | nach EMV-Konzept auf PE |

*Modbus over Serial Line V1.02: D1 = B/B' (positiv bei Ruhepegel „1“), D0 = A/A'. Achtung: Viele Hersteller bezeichnen A und B umgekehrt – im Zweifel Polarität im Ruhezustand messen (D1 > D0).*

### Pinout M12 A-codiert (Sensor)

| Pin | Farbe | Funktion | IO-Link |
|---|---|---|---|
| 1 | braun (BN) | L+ (24 V DC) | L+ |
| 2 | weiß (WH) | Ausgang 2 / Öffner | DI/DO |
| 3 | blau (BU) | L− (0 V) | L− |
| 4 | schwarz (BK) | Schaltausgang Q1 | C/Q |
| 5 | grau (GY) | herstellerabhängig / FE | – |

*IEC 61076-2-101, Aderfarben nach IEC 60947-5-2; IO-Link Port Class A.*

### KNX-Busleitung

| Ader | Funktion |
|---|---|
| rot | Bus + (KNX+) |
| schwarz | Bus − (KNX−) |
| gelb | Reserve / Zusatzspannung |
| weiß | Reserve / Zusatzspannung |

*KNX TP1, Busleitung z. B. J-Y(St)Y 2×2×0,8 bzw. KNX-zertifiziert (grüner Mantel).*

### Pinout RS-232 (D-Sub 9)

| Pin | Signal | Richtung (DTE) | Bedeutung |
|---|---|---|---|
| 1 | DCD | Eingang | Data Carrier Detect |
| 2 | RxD | Eingang | Receive Data |
| 3 | TxD | Ausgang | Transmit Data |
| 4 | DTR | Ausgang | Data Terminal Ready |
| 5 | GND | – | Signal Ground |
| 6 | DSR | Eingang | Data Set Ready |
| 7 | RTS | Ausgang | Request To Send |
| 8 | CTS | Eingang | Clear To Send |
| 9 | RI | Eingang | Ring Indicator |

*TIA-232 / DIN 66020, Stecker am DTE (PC). Nullmodem: 2↔3, 7↔8, 4↔6, 5–5.*

### Pinout USB 2.0

| Pin | Typ A/B | Aderfarbe | Micro-B |
|---|---|---|---|
| 1 | VBUS +5 V | rot | VBUS +5 V |
| 2 | D− | weiß | D− |
| 3 | D+ | grün | D+ |
| 4 | GND | schwarz | ID |
| 5 | – | – | GND |

*USB 2.0 Specification.*

### Pinout HDMI Typ A

| Pin | Signal | Pin | Signal |
|---|---|---|---|
| 1 | TMDS Data2+ | 11 | TMDS Clock Shield |
| 2 | TMDS Data2 Shield | 12 | TMDS Clock− |
| 3 | TMDS Data2− | 13 | CEC |
| 4 | TMDS Data1+ | 14 | Utility / HEAC+ |
| 5 | TMDS Data1 Shield | 15 | SCL (DDC) |
| 6 | TMDS Data1− | 16 | SDA (DDC) |
| 7 | TMDS Data0+ | 17 | DDC/CEC GND / HEAC Shield |
| 8 | TMDS Data0 Shield | 18 | +5 V |
| 9 | TMDS Data0− | 19 | Hot Plug Detect / HEAC− |
| 10 | TMDS Clock+ | – | – |

*HDMI Specification 1.4.*

### Farbcode-Tabelle

| Farbe | Ziffer | Multiplikator | Toleranz |
|---|---|---|---|
| schwarz | 0 | ×1 | – |
| braun | 1 | ×10 | ±1 % |
| rot | 2 | ×100 | ±2 % |
| orange | 3 | ×1 k | – |
| gelb | 4 | ×10 k | – |
| grün | 5 | ×100 k | ±0,5 % |
| blau | 6 | ×1 M | ±0,25 % |
| violett | 7 | ×10 M | ±0,1 % |
| grau | 8 | ×100 M | ±0,05 % |
| weiß | 9 | ×1 G | – |
| Gold | – | ×0,1 | ±5 % |
| Silber | – | ×0,01 | ±10 % |
| keine | – | – | ±20 % |

*IEC 60062.*

### SI-Vorsätze

| Vorsatz | Zeichen | Faktor |
|---|---|---|
| Tera | T | 10¹² |
| Giga | G | 10⁹ |
| Mega | M | 10⁶ |
| Kilo | k | 10³ |
| Hekto | h | 10² |
| Deka | da | 10¹ |
| Dezi | d | 10⁻¹ |
| Zenti | c | 10⁻² |
| Milli | m | 10⁻³ |
| Mikro | µ | 10⁻⁶ |
| Nano | n | 10⁻⁹ |
| Piko | p | 10⁻¹² |

*ISO 80000-1 / SI-Broschüre.*

