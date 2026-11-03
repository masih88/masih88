# Berechnungen, Formeln & Tabellen

> Automatisch erzeugt mit `elcalc markdown` aus dem Rechenkern (Version 1.11.0).
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

### Kabeltrasse: Belegung, Breite und Last

*Kabel eintragen → Trassenbreite mit Reserve, Füllgrad, Gewicht, Häufung* — `cable_tray`

```text
Platzbedarf je Kabel d² (Quadrat um den Kabelquerschnitt), Summe × (1 + Reserve); Richtwert Reserve ≈ 30 %.
Trassenbreite = kleinste Normbreite (50 … 600 mm) mit B · H ≥ Bedarf; Füllgrad = Σ d² / (B · H).
Kabellast q = Σ n · m' (kg/m), F = q · 9,81 N/kg; Bemessungslast nach VDE 0639: 0,028 N/(m·mm²) × B · H.
Zulässige Last je Stützabstand aus dem Belastungsdiagramm des Herstellers eintragen (Stützabstand möglichst ≤ 1,5 m).
Häufungsfaktor der Energiekabel nach DIN VDE 0298-4 / IEC 60364-5-52: einlagig auf gelochter Rinne bzw. Leiter, sonst gebündelt.
Kabeldaten (d, Gewicht) aus Datenblättern: NYM (Faber), NYY-J (Waskönig+Walter), J-Y(St)Y und LiYCY (Lapp), Cat 7 (DIGITUS) – andere Typen mit d und Gewicht eintragen. Quelle Methode: OBO Bettermann KTS-Guide „Systemauswahl“.
```

**Projekteinstellungen:** Trassenart, Seitenhöhe (Nutzhöhe), Platzreserve [%], Stützabstand [m], Zulässige Last laut Belastungsdiagramm [kN/m], Trennsteg Energie/Daten

**Spalten je Zeile:** Bezeichnung, Kabeltyp, Anzahl, Außendurchmesser (falls abweichend) [mm], Gewicht (falls abweichend) [kg/km], Art

**Ergebnisse je Zeile:** d [mm], Platzbedarf n · d² [mm²], Last [kg/m], Breite nebeneinander [mm], Art, Erkannt als

**Summen:** Trassenbreite (Normbreite) [mm], Anzahl Trassen, Füllgrad ohne Reserve [%], Belegung mit Reserve [%], Platzbedarf Σ n · d² [mm²], Bedarf mit Reserve [mm²], Kabelgewicht [kg/m], Kabellast je Trasse [kN/m], Bemessungslast VDE 0639 [kN/m], Energiekabel, Daten-/Steuerkabel, Breite Energiekabel nebeneinander [mm], Häufungsfaktor Energiekabel

## Elektrotechnik

### Grundlagen

#### Leistung, Strom und Spannung

*P, Q, S, I oder U für DC, 1~ und 3~ – auch Motor mit cos φ und η* — `power`

```text
P_auf = k · U · I · cos φ    P_ab = P_auf · η    k = 1 (DC, 1~), √3 (3~, U = Außenleiterspannung)
Strom: I = P_ab / (k · U · cos φ · η)    Spannung: U = P_ab / (k · I · cos φ · η)
S = k · U · I    Q = S · sin φ    φ = arccos(cos φ)    U₀ = U / √3    1 hp = 745,7 W
Motor: P_ab = Bemessungsleistung vom Typenschild (mechanisch) mit η; sonst η = 100 % (P = aufgenommene Wirkleistung).
```

**Eingaben:** Gesucht, Netz, Spannung U [V], Strom I [A], Leistung P (abgegeben) [kW], Leistungsfaktor cos φ, Wirkungsgrad η [%]

**Ergebnisse:** Strom I [A], Spannung U [V], Strangspannung U₀ (3~) [V], Abgegebene Leistung P_ab [kW], Abgegebene Leistung [hp], Aufgenommene Wirkleistung P_auf [kW], Scheinleistung S [kVA], Blindleistung Q [kvar], Phasenwinkel φ [°], Verluste P_auf − P_ab [kW]

#### Leistungsfaktorberechnung

*cos φ aus P und U·I, aus P und S oder aus P und Q* — `power_factor_calc`

```text
cos φ = P / S    S = k · U · I (k = 1 bei 1~, √3 bei 3~)    S = √(P² + Q²)
Motor: P_auf = P_ab / η. φ = arccos(cos φ), Q = P · tan φ. Der Leistungsfaktor λ = P/S ist bei Oberschwingungen kleiner als cos φ₁.
```

**Eingaben:** Bekannt, Netz, Wirkleistung P [kW], Spannung U [V], Strom I [A], Scheinleistung S [kVA], Blindleistung Q [kvar], Wirkungsgrad η [%]

**Ergebnisse:** Leistungsfaktor cos φ, Phasenwinkel φ [°], sin φ, tan φ, Scheinleistung S [kVA], Blindleistung Q [kvar], Aufgenommene Wirkleistung P / η [kW]

#### Ohmsches Gesetz & Leistung

*U, I, R, P – zwei Werte eingeben* — `ohm`

```text
Genau zwei der vier Größen eingeben, die übrigen werden berechnet.
U = R · I    P = U · I = I² · R = U² / R
```

**Eingaben:** Spannung U [V], Strom I [A], Widerstand R [Ω], Leistung P [W]

**Ergebnisse:** Spannung U [V], Strom I [A], Widerstand R [Ω], Leistung P [W]

#### Impedanzberechnung

*Z, R und X aus Spannung, Strom und cos φ* — `impedance_ui`

```text
1~: Z = U / I    3~ (je Strang, Sternersatz): Z = U / (√3 · I)
R = Z · cos φ    X = Z · sin φ
```

**Eingaben:** Netz, Spannung U [V], Strom I [A], Leistungsfaktor cos φ

**Ergebnisse:** Impedanz Z [Ω], Wirkwiderstand R [Ω], Blindwiderstand X [Ω], Phasenwinkel φ [°]

#### Betriebsstrom

*I_b mit Ausnutzungs- und Gleichzeitigkeitsfaktor* — `operating_current`

```text
P_b = P_inst · k_u · k_s    I_b = P_b / (k · U · cos φ · η)
k_u = Ausnutzungsfaktor (Last eines Verbrauchers / Bemessungsleistung, Motoren typ. 0,75), k_s = Gleichzeitigkeitsfaktor der Gruppe (z. B. 0,8–0,9 für Verteiler mit 4–9 Abgängen, IEC 61439-1 Tab. 101 als Anhalt).
```

**Eingaben:** Netz, Spannung U [V], Installierte Leistung P_inst [kW], Ausnutzungsfaktor k_u, Gleichzeitigkeitsfaktor k_s, Leistungsfaktor cos φ, Wirkungsgrad η [%]

**Ergebnisse:** Betriebsstrom I_b [A], Betriebsleistung P_b [kW], Strom bei Volllast (k_u = k_s = 1) [A]

#### Joule-Effekt

*Stromwärme W = I² · R · t und Erwärmung* — `joule_effect`

```text
P = I² · R    W = I² · R · t    ΔT = W / (m · c) (ohne Wärmeverluste)
c: Wasser 4186, Kupfer 385, Aluminium 897, Luft 1005 J/(kg·K).
```

**Eingaben:** Widerstand R [Ω], Strom I [A], Zeit t [s], Masse m [kg], Spezifische Wärmekapazität c [J/(kg·K)]

**Ergebnisse:** Leistung P [W], Wärmeenergie W [kJ], Wärmeenergie W [Wh], Temperaturerhöhung ΔT [K]

#### Energieverbrauch & Kosten

*kWh und Kosten pro Tag/Jahr* — `energy_cost`

```text
E = P · t    Kosten = E · Preis
Währung (€ oder CHF) und Wechselkurs in den Einstellungen; der Betrag wird zusätzlich in der anderen Währung angezeigt.
```

**Eingaben:** Leistung P [W], Betriebsstunden pro Tag [h], Betriebstage pro Jahr [d], Strompreis pro kWh [€ / CHF]

**Ergebnisse:** Energie pro Tag [kWh], Energie pro Jahr [kWh], Kosten pro Tag [€], Kosten pro Jahr [€]

#### Widerstände: Reihe & parallel

*R_ges für bis zu 6 Widerstände (auch L)* — `resistors_series_parallel`

```text
Reihe: R = R1 + R2 + …
Parallel: 1/R = 1/R1 + 1/R2 + …
Gilt genauso für Induktivitäten ohne Kopplung. Leere Felder werden ignoriert.
```

**Eingaben:** R1 [Ω], R2 [Ω], R3 [Ω], R4 [Ω], R5 [Ω], R6 [Ω]

**Ergebnisse:** Reihenschaltung [Ω], Parallelschaltung [Ω]

#### Kondensatoren: Reihe & parallel

*C_ges für bis zu 6 Kondensatoren* — `capacitors_series_parallel`

```text
Parallel: C = C1 + C2 + …
Reihe: 1/C = 1/C1 + 1/C2 + …
Leere Felder werden ignoriert.
```

**Eingaben:** C1 [µF], C2 [µF], C3 [µF], C4 [µF], C5 [µF], C6 [µF]

**Ergebnisse:** Reihenschaltung [µF], Parallelschaltung [µF]

#### Stromteiler

*Aufteilung eines Stroms auf parallele Widerstände* — `current_divider`

```text
Parallelschaltung: U = I · R_ges, 1/R_ges = Σ 1/R_i    I_i = U / R_i
Zwei Widerstände: I₁ = I · R₂ / (R₁ + R₂).
```

**Eingaben:** Gesamtstrom I [A], R₁ [Ω], R₂ [Ω], R₃ [Ω]

**Ergebnisse:** Strom durch R₁ [A], Strom durch R₂ [A], Strom durch R₃ [A], Spannung U [V], Ersatzwiderstand R_ges [Ω]

#### Dreieck-Stern-Umwandlung

*Δ ↔ Y für Widerstände oder Impedanzen, mit Zeichnung* — `delta_star`

```text
Δ → Y: R₁ = R₁₂ · R₃₁ / ΣR    R₂ = R₁₂ · R₂₃ / ΣR    R₃ = R₂₃ · R₃₁ / ΣR    (ΣR = R₁₂ + R₂₃ + R₃₁)
Y → Δ: R₁₂ = (R₁R₂ + R₂R₃ + R₃R₁) / R₃    R₂₃ = … / R₁    R₃₁ = … / R₂
Symmetrisch: R_Y = R_Δ / 3. Gilt ebenso für komplexe Impedanzen.
```

**Eingaben:** Umwandlung, R₁₂ (zwischen 1 und 2) [Ω], R₂₃ (zwischen 2 und 3) [Ω], R₃₁ (zwischen 3 und 1) [Ω], R₁ (an Knoten 1) [Ω], R₂ (an Knoten 2) [Ω], R₃ (an Knoten 3) [Ω]

**Ergebnisse:** R₁ [Ω], R₂ [Ω], R₃ [Ω], R₁₂ [Ω], R₂₃ [Ω], R₃₁ [Ω]

### Leitungen berechnen

#### Leitungsdimensionierung

*Querschnitt nach Strombelastbarkeit, Spannungsfall und Schutzorgan* — `cable_sizing`

```text
Gewählt wird der kleinste Kupfer-Querschnitt, der alle Bedingungen erfüllt:
1) I_z = I_z0 · f_T · f_G ≥ I_B    2) ΔU ≤ ΔU_max    3) I_B ≤ I_n ≤ I_z und I_2 ≤ 1,45 · I_z
f_T aus Umgebungstemperatur (Tab. B.52.14/15), f_G Häufung (Tab. B.52.17). Spannungsfall bei max. Leitertemperatur (70/90 °C), X' = 0,08 mΩ/m.
Strombelastbarkeit Kupfer nach IEC 60364-5-52 Anhang B (entspricht DIN VDE 0298-4 bzw. NIN 5.2.3), Bezug 30 °C Luft bzw. 20 °C Erdreich. Werte vor Verwendung gegen die gültige Normausgabe prüfen.
```

**Eingaben:** Netz, Nennspannung U [V], Betriebsstrom I_B [A], Leitungslänge L (einfach) [m], Leistungsfaktor cos φ, Zul. Spannungsfall ΔU max [%], Isolierung, Verlegeart, Umgebungstemperatur θ_a [°C], Häufung – Anordnung, Anzahl Stromkreise in Häufung, Schutzorgan

**Ergebnisse:** Faktor Temperatur f_T, Faktor Häufung f_G, Erforderliches I_z0 [A], Min. Querschnitt (Belastbarkeit) [mm²], Min. Querschnitt (Spannungsfall) [mm²], Gewählter Querschnitt [mm²], Belastbarkeit I_z [A], Schutzorgan I_n [A], Spannungsfall [%], Maßgebend

#### Spannungsfall

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

#### Max. Leitungslänge (Spannungsfall)

*z. B. 24-V-Feldgeräte, Stellantriebe* — `max_length_vdrop`

```text
L_max = ΔU_max / (k · I · (ρ/A · cos φ + X' · sin φ))
k = 2 bei DC/1~, √3 bei 3~.
```

**Eingaben:** Netz, Nennspannung U [V], Strom I [A], Querschnitt A [mm²], Leitermaterial, Leistungsfaktor cos φ, Leitertemperatur θ [°C], Zul. Spannungsfall ΔU max [%]

**Ergebnisse:** Max. Leitungslänge (einfach) [m], Zul. Spannungsfall [V], Min. Spannung am Gerät [V]

#### Spannungsfall bei verteilten Lasten

*n gleiche Lasten entlang einer Leitung (z. B. Leuchtenband)* — `vdrop_distributed`

```text
Momentenmethode: ΔU = k · Σ(I · L_i) · (ρ/A · cos φ + X' · sin φ), L_i = L₁ + (i − 1) · d
k = 2 (DC/1~), √3 (3~, symmetrisch verteilt); ρ bei 70 °C, X' = 0,08 mΩ/m. Σ(I·L) entspricht dem Gesamtstrom in der Ersatzlänge L₁ + (n − 1) · d / 2.
Querschnitt: A = ρ · cos φ / (ΔU_max/(k · Σ I·L) − X' · sin φ), danach nächster Normquerschnitt.
```

**Eingaben:** Netz, Nennspannung U [V], Anzahl Lasten n, Strom je Last [A], Abstand bis zur ersten Last L₁ [m], Abstand zwischen den Lasten d [m], Leitermaterial, Leistungsfaktor cos φ, Querschnitt

**Ergebnisse:** Spannungsfall an der letzten Last [%], Spannungsfall ΔU [V], Spannung an der letzten Last [V], Gesamtstrom am Anfang [A], Ersatzlänge [m], Länge bis zur letzten Last [m]

#### Leiterdimensionierung bei verteilten Lasten

*Mindestquerschnitt für n gleiche Lasten entlang einer Leitung* — `sizing_distributed`

```text
Momentenmethode: ΔU = k · Σ(I · L_i) · (ρ/A · cos φ + X' · sin φ), L_i = L₁ + (i − 1) · d
k = 2 (DC/1~), √3 (3~, symmetrisch verteilt); ρ bei 70 °C, X' = 0,08 mΩ/m. Σ(I·L) entspricht dem Gesamtstrom in der Ersatzlänge L₁ + (n − 1) · d / 2.
Querschnitt: A = ρ · cos φ / (ΔU_max/(k · Σ I·L) − X' · sin φ), danach nächster Normquerschnitt.
```

**Eingaben:** Netz, Nennspannung U [V], Anzahl Lasten n, Strom je Last [A], Abstand bis zur ersten Last L₁ [m], Abstand zwischen den Lasten d [m], Leitermaterial, Leistungsfaktor cos φ, Zulässiger Spannungsfall [%]

**Ergebnisse:** Gewählter Querschnitt [mm²], Mindestquerschnitt (rechnerisch) [mm²], Spannungsfall mit gewähltem Querschnitt [%], Gesamtstrom am Anfang [A], Ersatzlänge [m]

#### Strombelastbarkeit I_z

*I_z nach Verlegeart, Temperatur und Häufung* — `ampacity`

```text
I_z = I_z0 · f_T · f_G
f_T = √((θ_max − θ_a)/(θ_max − θ_ref))
Strombelastbarkeit Kupfer nach IEC 60364-5-52 Anhang B (entspricht DIN VDE 0298-4 bzw. NIN 5.2.3), Bezug 30 °C Luft bzw. 20 °C Erdreich. Werte vor Verwendung gegen die gültige Normausgabe prüfen.
```

**Eingaben:** Querschnitt (Cu), Isolierung, Verlegeart, Belastete Adern, Umgebungstemperatur θ_a [°C], Häufung – Anordnung, Anzahl Stromkreise in Häufung

**Ergebnisse:** Tabellenwert I_z0 [A], Faktor Temperatur f_T, Faktor Häufung f_G, Strombelastbarkeit I_z [A]

#### Strombelastbarkeit von blanken und mineralisolierten Leitern

*MI-Kabel (Cu), Mantel 70 °C, Verlegeart C* — `ampacity_mi`

```text
Mineralisolierte Kupferkabel (MICC) nach IEC 60702-1, Mantel PVC-umhüllt oder blank berührbar (Manteltemperatur 70 °C), direkt auf der Wand (Verlegeart C), Umgebung 30 °C. Werte nach BS 7671 Tab. 4G1A = IEC 60364-5-52 Tab. B.52.6 (Herstellerdatenblatt). 1~ = 2 belastete Leiter, 3~ = 3 belastete Leiter.
Umgebungstemperatur: f = √((70 − θ_a)/(70 − 30)) als Näherung von Tab. B.52.14 – für blanke, nicht berührbare Kabel (Mantel 105 °C, Tab. B.52.7) und Häufung die Norm verwenden. Gegen die gültige Ausgabe prüfen.
```

**Eingaben:** Ausführung, Aderzahl / Belastung, Querschnitt, Umgebungstemperatur θ_a [°C], Betriebsstrom I_b [A]

**Ergebnisse:** Belastbarkeit I_z [A], Tabellenwert (30 °C) [A], Faktor Umgebungstemperatur, Kabelbezeichnung

#### Strombelastbarkeit von Sammelschienen

*E-Cu-Flachschienen nach DIN 43671, 1 oder 2 Schienen je Leiter* — `busbar_ampacity`

```text
Dauerströme für E-Cu-Schienen mit Rechteckquerschnitt in Innenanlagen bei 25 °C Lufttemperatur um den Leiter und max. 65 °C Schienentemperatur, Wechselstrom, Schienen hochkant (Striebel & John, Kap. 10 Sammelschienensysteme, nach DIN 43671).
Andere Umgebungstemperatur als Näherung: I = I₂₅ · √((65 − θ_a)/(65 − 25)) (Wärmeabgabe ∝ Übertemperatur). Für Auslegung die Korrekturfaktoren der DIN 43671 bzw. Herstellerangaben verwenden (Anordnung, Anstrich, Höhe, Frequenz).
```

**Eingaben:** Schiene B × D, Schienen je Leiter, Umgebungstemperatur θ_a [°C], Betriebsstrom I_b [A]

**Ergebnisse:** Dauerstrom bei θ_a [A], Tabellenwert (25 °C) [A], Querschnitt [mm²], Stromdichte [A/mm²], Gewicht je Leiter [kg/m]

#### Leiterwiderstand, Reaktanz und Impedanz

*R(θ) = ρ(θ) · L / A, X und Z je Leiter und Schleife, ρ und κ* — `cable_rxz`

```text
R₂₀ = ρ₂₀ · L / A    R(θ) = R₂₀ · (1 + α₂₀ · (θ − 20 °C))    X = X' · L    Z = √(R² + X²)
Cu: ρ₂₀ = 0,017241 Ω·mm²/m, α = 0,00393 1/K; Al: 0,028264 Ω·mm²/m, 0,00403 1/K. X' ≈ 0,08 mΩ/m (mehradrige Kabel), ca. 0,1–0,15 mΩ/m für Einleiter mit Abstand. Schleife 1~/DC = Hin- und Rückleiter; 3~ = je Strang.
Gleichstromwiderstand ohne Skineffekt (bei 50 Hz ab ca. 150 mm² merklich).
```

**Eingaben:** Netz, Leitermaterial, Querschnitt, Länge L (einfach) [m], Leitertemperatur θ [°C], Reaktanzbelag X' [mΩ/m]

**Ergebnisse:** R₂₀ je Leiter [mΩ], R(θ) je Leiter [mΩ], X je Leiter [mΩ], Z je Leiter [mΩ], Z Schleife (Hin + Rück) [mΩ], R(θ) je km [Ω/km], R(θ) Schleife (Hin + Rück) [mΩ], Spez. Widerstand ρ(θ) [Ω·mm²/m], Leitfähigkeit κ(θ) [m/(Ω·mm²)]

#### Berechnung der Kabeltemperatur

*Leitertemperatur aus Auslastung I_b/I_z* — `cable_temperature`

```text
θ = θ_a + (θ_z − θ_a) · (I_b / I_z)²
θ_z = zulässige Leitertemperatur (PVC 70 °C, VPE/EPR 90 °C), I_z = Belastbarkeit bei der tatsächlichen Umgebungstemperatur θ_a und Häufung. Verluste ∝ I² (IEC 60287, ohne Änderung von R mit der Temperatur).
```

**Eingaben:** Betriebsstrom I_b [A], Belastbarkeit I_z [A], Umgebungstemperatur θ_a [°C], Isolierung

**Ergebnisse:** Leitertemperatur θ [°C], Übertemperatur [K], Auslastung I_b/I_z [%], R(θ)/R₂₀ (Cu)

#### Verlustleistung in Kabeln

*P_V = n · I² · R(θ), Energie und Kosten pro Jahr* — `cable_losses`

```text
P_V = n · I² · ρ(θ) · L / A    n = 2 (DC/1~), 3 (3~, symmetrisch)
E = P_V · Betriebsstunden    Kosten = E · Strompreis. θ = mittlere Leitertemperatur (siehe Kabeltemperatur), Neutralleiter ohne Strom.
```

**Eingaben:** Netz, Strom I [A], Länge L (einfach) [m], Leitermaterial, Querschnitt, Leitertemperatur θ [°C], Spannung U (für %) [V], Leistungsfaktor cos φ, Betriebsstunden pro Jahr [h], Strompreis pro kWh [€ / CHF]

**Ergebnisse:** Verlustleistung P_V [W], Verluste je Meter Leitung [W/m], Anteil an der übertragenen Leistung [%], Verlustenergie pro Jahr [kWh], Verlustkosten pro Jahr [€]

#### Neutralleiterstrom

*Unsymmetrie und 3. Oberschwingung, Auslegung nach IEC 60364-5-52 Anhang E* — `neutral_current`

```text
Grundschwingung (gleiches cos φ, 120° versetzt): I_N1 = √(I₁² + I₂² + I₃² − I₁I₂ − I₂I₃ − I₃I₁)
3. Oberschwingung (h₃ in % des Außenleiterstroms) addiert sich im N: I_N3 = h₃ · (I₁ + I₂ + I₃)    I_N = √(I_N1² + I_N3²)
Auslegung (IEC 60364-5-52 Tab. E.52.1, 4-/5-adrige Kabel): h₃ ≤ 15 %: Außenleiterstrom, Faktor 1,0; 15–33 %: Außenleiterstrom, 0,86; 33–45 %: Neutralleiterstrom, 0,86; > 45 %: Neutralleiterstrom, 1,0.
```

**Eingaben:** Strom L1 [A], Strom L2 [A], Strom L3 [A], Anteil 3. Oberschwingung h₃ [%]

**Ergebnisse:** Neutralleiterstrom I_N [A], Anteil Unsymmetrie I_N1 [A], Anteil 3. Oberschwingung I_N3 [A], I_N / größter Außenleiterstrom [%], Bemessungsstrom für die Kabelwahl [A], Auslegung

#### Kurzschlussfestigkeit (I²t)

*Zulässige Energie und Abschaltzeit einer Leitung* — `short_circuit_withstand`

```text
Adiabatische Erwärmung (IEC 60364-4-43, 434.5.2): t = (k · S / I_k)²   bzw.   S ≥ √(I_k² · t) / k
Zulässige Energie: (k · S)²   k nach Tab. 43A: Cu/PVC 115, Cu/VPE 143, Al/PVC 76, Al/VPE 94.
Gültig für t ≤ 5 s. Für t < 0,1 s Durchlass-I²t des Schutzorgans (Herstellerangabe) vergleichen.
```

**Eingaben:** Leiter / Isolierung, Querschnitt S [mm²], Kurzschlussstrom I_k [kA], Abschaltzeit t [s]

**Ergebnisse:** Materialbeiwert k [A·√s/mm²], Zulässige Energie (k·S)² [A²s], Max. Abschaltzeit bei I_k [s], Min. Querschnitt für I_k, t [mm²]

#### Rohr-/Kanalfüllung

*Füllgrad eines Installationsrohres* — `conduit_fill`

```text
Füllgrad = n · d² / D_i²  (Querschnittsflächen)
Richtwerte (u. a. NEC Chapter 9): 1 Leitung 53 %, 2 Leitungen 31 %, ab 3 Leitungen 40 %. In DE/CH gilt: Leitungen müssen sich ohne Beschädigung einziehen lassen – Herstellerangaben beachten.
```

**Eingaben:** Innendurchmesser Rohr D_i [mm], Außendurchmesser Leitung d [mm], Anzahl Leitungen

**Ergebnisse:** Füllgrad [%], Richtwert [%], Max. Anzahl bei 40 %

### Kabeltabellen & Aderfarben

#### Spannungsfall-Grenzwerte DE/CH

| Land | Regel | Bereich | ΔU max |
|---|---|---|---|
| DE | DIN 18015-1 | Zähler bis Verbraucher (Wohngebäude) | 3 % |
| DE | VDE-AR-N 4100 (TAB) | Hauptleitung bis Zähler | 0,5 % (≤ 100 kVA) |
| DE | DIN VDE 0100-520 Anh. G | öff. Netz – Beleuchtung / sonstige | 3 % / 5 % |
| DE | DIN VDE 0100-520 Anh. G | eigene Trafostation – Beleuchtung / sonstige | 6 % / 8 % |
| CH | NIN | Anschlussüberstromunterbrecher bis Verbraucher | 4 % |

*Übersicht – maßgeblich sind die gültigen Normausgaben und die TAB des Netzbetreibers.*

#### Strombelastbarkeit Cu PVC 70 °C, 2 belastete Adern

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

#### Strombelastbarkeit Cu PVC 70 °C, 3 belastete Adern

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

#### Strombelastbarkeit Cu VPE 90 °C, 2 belastete Adern

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

#### Strombelastbarkeit Cu VPE 90 °C, 3 belastete Adern

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

#### Umrechnungsfaktoren Temperatur

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

#### Häufungsfaktoren

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

#### Zulässige spezifische Durchlassenergie k²S²

| S in mm² | Cu PVC | Cu VPE/EPR | Al PVC | Al VPE |
|---|---|---|---|---|
| 1,5 | 2,98 · 10⁴ | 4,6 · 10⁴ | 1,3 · 10⁴ | 1,99 · 10⁴ |
| 2,5 | 8,27 · 10⁴ | 1,28 · 10⁵ | 3,61 · 10⁴ | 5,52 · 10⁴ |
| 4 | 2,12 · 10⁵ | 3,27 · 10⁵ | 9,24 · 10⁴ | 1,41 · 10⁵ |
| 6 | 4,76 · 10⁵ | 7,36 · 10⁵ | 2,08 · 10⁵ | 3,18 · 10⁵ |
| 10 | 1,32 · 10⁶ | 2,04 · 10⁶ | 5,78 · 10⁵ | 8,84 · 10⁵ |
| 16 | 3,39 · 10⁶ | 5,23 · 10⁶ | 1,48 · 10⁶ | 2,26 · 10⁶ |
| 25 | 8,27 · 10⁶ | 1,28 · 10⁷ | 3,61 · 10⁶ | 5,52 · 10⁶ |
| 35 | 1,62 · 10⁷ | 2,51 · 10⁷ | 7,08 · 10⁶ | 1,08 · 10⁷ |
| 50 | 3,31 · 10⁷ | 5,11 · 10⁷ | 1,44 · 10⁷ | 2,21 · 10⁷ |
| 70 | 6,48 · 10⁷ | 1 · 10⁸ | 2,83 · 10⁷ | 4,33 · 10⁷ |
| 95 | 1,19 · 10⁸ | 1,85 · 10⁸ | 5,21 · 10⁷ | 7,97 · 10⁷ |
| 120 | 1,9 · 10⁸ | 2,94 · 10⁸ | 8,32 · 10⁷ | 1,27 · 10⁸ |
| 150 | 2,98 · 10⁸ | 4,6 · 10⁸ | 1,3 · 10⁸ | 1,99 · 10⁸ |
| 185 | 4,53 · 10⁸ | 7 · 10⁸ | 1,98 · 10⁸ | 3,02 · 10⁸ |
| 240 | 7,62 · 10⁸ | 1,18 · 10⁹ | 3,33 · 10⁸ | 5,09 · 10⁸ |
| 300 | 1,19 · 10⁹ | 1,84 · 10⁹ | 5,2 · 10⁸ | 7,95 · 10⁸ |

*Adiabatische Erwärmung (IEC 60364-4-43, 434.5.2): Die Durchlassenergie I²t der Schutzeinrichtung darf k²S² nicht überschreiten (gilt für t ≤ 5 s). k nach Tab. 43A: Cu/PVC 115 (> 300 mm²: 103), Cu/VPE-EPR 143, Al/PVC 76, Al/VPE 94. I²t der Schutzeinrichtung aus dem Herstellerdatenblatt. Gegen die gültige Ausgabe prüfen.*

#### Kupfer-Sammelschienen: Dauerstrom

| B × D in mm | A in mm² | kg/m | 1 Schiene in A | 2 Schienen in A |
|---|---|---|---|---|
| 12 × 5 | 59 | 0,525 | 250 | – |
| 12 × 10 | 119 | 1,059 | 360 | – |
| 20 × 5 | 99 | 0,881 | 320 | – |
| 20 × 10 | 199 | 1,771 | 500 | 850 |
| 25 × 5 | 124 | 1,104 | 390 | – |
| 30 × 5 | 149 | 1,331 | 440 | – |
| 30 × 10 | 299 | 2,661 | 630 | 1250 |
| 40 × 10 | 399 | 3,551 | 850 | 1500 |
| 50 × 10 | 499 | 4,441 | 1000 | – |
| 60 × 10 | 599 | 5,331 | 1250 | – |
| 80 × 10 | 799 | 7,111 | 1450 | – |
| 100 × 10 | 999 | 8,891 | 1700 | – |

*Innenanlagen, 25 °C Lufttemperatur um den Leiter, max. 65 °C Schienentemperatur, Wechselstrom, hochkant (Striebel & John, Kap. 10 Sammelschienensysteme, nach DIN 43671). Bei Abweichungen Korrekturfaktoren der DIN 43671.*

#### Mineralisolierte Kabel: Belastbarkeit

| Ausführung | Aderzahl | mm² | I_z in A | Bezeichnung |
|---|---|---|---|---|
| 500 V (leicht) | 2 Adern, 1~/DC | 1 | 18,5 | 2L1 |
| 500 V (leicht) | 2 Adern, 1~/DC | 1,5 | 23 | 2L1.5 |
| 500 V (leicht) | 2 Adern, 1~/DC | 2,5 | 31 | 2L2.5 |
| 500 V (leicht) | 2 Adern, 1~/DC | 4 | 40 | 2L4 |
| 500 V (leicht) | 3 Adern, 3~ | 1 | 15 | 3L1 |
| 500 V (leicht) | 3 Adern, 3~ | 1,5 | 19 | 3L1.5 |
| 500 V (leicht) | 3 Adern, 3~ | 2,5 | 26 | 3L2.5 |
| 500 V (leicht) | 4 Adern, 3~ | 1 | 15 | 4L1 |
| 500 V (leicht) | 4 Adern, 3~ | 1,5 | 19 | 4L1.5 |
| 500 V (leicht) | 4 Adern, 3~ | 2,5 | 26 | 4L2.5 |
| 500 V (leicht) | 7 Adern | 1,5 | 13 | 7L1.5 |
| 500 V (leicht) | 7 Adern | 2,5 | 17,5 | 7L2.5 |
| 750 V (schwer) | 2 Adern, 1~/DC | 1,5 | 25 | 2H1.5 |
| 750 V (schwer) | 2 Adern, 1~/DC | 2,5 | 34 | 2H2.5 |
| 750 V (schwer) | 2 Adern, 1~/DC | 4 | 45 | 2H4 |
| 750 V (schwer) | 2 Adern, 1~/DC | 6 | 57 | 2H6 |
| 750 V (schwer) | 2 Adern, 1~/DC | 10 | 77 | 2H10 |
| 750 V (schwer) | 2 Adern, 1~/DC | 16 | 102 | 2H16 |
| 750 V (schwer) | 2 Adern, 1~/DC | 25 | 133 | 2H25 |
| 750 V (schwer) | 3 Adern, 3~ | 1,5 | 21 | 3H1.5 |
| 750 V (schwer) | 3 Adern, 3~ | 2,5 | 28 | 3H2.5 |
| 750 V (schwer) | 3 Adern, 3~ | 4 | 37 | 3H4 |
| 750 V (schwer) | 3 Adern, 3~ | 6 | 48 | 3H6 |
| 750 V (schwer) | 3 Adern, 3~ | 10 | 65 | 3H10 |
| 750 V (schwer) | 3 Adern, 3~ | 16 | 86 | 3H16 |
| 750 V (schwer) | 3 Adern, 3~ | 25 | 112 | 3H25 |
| 750 V (schwer) | 4 Adern, 3~ | 1,5 | 21 | 4H1.5 |
| 750 V (schwer) | 4 Adern, 3~ | 2,5 | 28 | 4H2.5 |
| 750 V (schwer) | 4 Adern, 3~ | 4 | 37 | 4H4 |
| 750 V (schwer) | 4 Adern, 3~ | 6 | 47 | 4H6 |
| 750 V (schwer) | 4 Adern, 3~ | 10 | 64 | 4H10 |
| 750 V (schwer) | 4 Adern, 3~ | 16 | 85 | 4H16 |
| 750 V (schwer) | 4 Adern, 3~ | 25 | 110 | 4H25 |
| 750 V (schwer) | Einleiter, 1~ | 6 | 57 | 1H6 |
| 750 V (schwer) | Einleiter, 1~ | 10 | 77 | 1H10 |
| 750 V (schwer) | Einleiter, 1~ | 16 | 102 | 1H16 |
| 750 V (schwer) | Einleiter, 1~ | 25 | 133 | 1H25 |
| 750 V (schwer) | Einleiter, 1~ | 35 | 163 | 1H35 |
| 750 V (schwer) | Einleiter, 1~ | 50 | 202 | 1H50 |
| 750 V (schwer) | Einleiter, 1~ | 70 | 247 | 1H70 |
| 750 V (schwer) | Einleiter, 1~ | 95 | 296 | 1H95 |
| 750 V (schwer) | Einleiter, 1~ | 120 | 340 | 1H120 |
| 750 V (schwer) | Einleiter, 1~ | 150 | 388 | 1H150 |
| 750 V (schwer) | Einleiter, 1~ | 185 | 440 | 1H185 |
| 750 V (schwer) | Einleiter, 1~ | 240 | 514 | 1H240 |
| 750 V (schwer) | Einleiter, 3~ | 6 | 52 | 1H6 |
| 750 V (schwer) | Einleiter, 3~ | 10 | 70 | 1H10 |
| 750 V (schwer) | Einleiter, 3~ | 16 | 92 | 1H16 |
| 750 V (schwer) | Einleiter, 3~ | 25 | 120 | 1H25 |
| 750 V (schwer) | Einleiter, 3~ | 35 | 147 | 1H35 |
| 750 V (schwer) | Einleiter, 3~ | 50 | 181 | 1H50 |
| 750 V (schwer) | Einleiter, 3~ | 70 | 221 | 1H70 |
| 750 V (schwer) | Einleiter, 3~ | 95 | 264 | 1H95 |
| 750 V (schwer) | Einleiter, 3~ | 120 | 303 | 1H120 |
| 750 V (schwer) | Einleiter, 3~ | 150 | 346 | 1H150 |
| 750 V (schwer) | Einleiter, 3~ | 185 | 392 | 1H185 |
| 750 V (schwer) | Einleiter, 3~ | 240 | 457 | 1H240 |
| 750 V (schwer) | 7 Adern | 1,5 | 14,5 | 7H1.5 |
| 750 V (schwer) | 7 Adern | 2,5 | 19,5 | 7H2.5 |
| 750 V (schwer) | 12 Adern | 2,5 | 16 | 12H2.5 |
| 750 V (schwer) | 19 Adern | 1,5 | 10 | 19H1.5 |

*BS 7671 Tab. 4G1A = IEC 60364-5-52 Tab. B.52.6 (Herstellerdatenblatt): PVC-umhüllt oder blank berührbar, direkt auf der Wand, alle Adern belastet. Gegen die gültige Ausgabe prüfen.*

#### Reaktanzbeläge von Kabeln

| A in mm² | mehradrig | Einleiter, Dreieck | Einleiter, Abstand |
|---|---|---|---|
| 1,5 | 0,108 | 0,157 | 0,201 |
| 2,5 | 0,099 | 0,145 | 0,189 |
| 4 | 0,099 | 0,138 | 0,181 |
| 6 | 0,093 | 0,129 | 0,172 |
| 10 | 0,087 | 0,119 | 0,162 |
| 16 | 0,082 | 0,11 | 0,154 |
| 25 | 0,082 | 0,105 | 0,148 |
| 35 | 0,079 | 0,1 | 0,143 |
| 50 | 0,078 | 0,096 | 0,14 |
| 70 | 0,076 | 0,092 | 0,135 |
| 95 | 0,075 | 0,09 | 0,133 |
| 120 | 0,074 | 0,087 | 0,13 |
| 150 | 0,074 | 0,086 | 0,129 |
| 185 | 0,074 | 0,085 | 0,128 |
| 240 | 0,073 | 0,083 | 0,126 |
| 300 | 0,073 | 0,082 | 0,125 |

*Berechnet je Leiter: X' = 2πf · μ₀/(2π) · (1/4 + ln(a/r)), r = √(A/π), a = Achsabstand der Leiter. Mehradrig: a = 2 · (r + Isolierdicke nach IEC 60502-1); Einleiter im Dreieck: zusätzlich 1,8 mm Mantel; Einleiter mit Abstand = doppelter Kabeldurchmesser. Näherung (Sektorleiter, Verseilung und Bewehrung nicht berücksichtigt) – Herstellerwerte bevorzugen. Üblicher Rechenwert für mehradrige Kabel: 0,08 mΩ/m. Bei 60 Hz × 1,2.*

#### Spezifischer Widerstand und Leitfähigkeit

| Werkstoff | ρ in Ω·mm²/m | κ in m/(Ω·mm²) | α in 10⁻³/K |
|---|---|---|---|
| Silber | 0,0159 | 62,89 | 3,8 |
| Kupfer (IACS, weich) | 0,0172 | 58 | 3,93 |
| Gold | 0,0244 | 40,98 | 3,4 |
| Aluminium (Leiter) | 0,0283 | 35,38 | 4,03 |
| Wolfram | 0,055 | 18,18 | 4,5 |
| Zink | 0,059 | 16,95 | 3,7 |
| Nickel | 0,069 | 14,49 | 6 |
| Messing (CuZn37) | 0,067 | 14,93 | 1,5 |
| Eisen (rein) | 0,1 | 10 | 6,5 |
| Platin | 0,106 | 9,43 | 3,85 |
| Zinn | 0,115 | 8,7 | 4,5 |
| Stahl (unlegiert) | 0,14 | 7,14 | 5 |
| Blei | 0,208 | 4,81 | 3,9 |
| Edelstahl 1.4301 | 0,73 | 1,37 | 1 |
| Manganin | 0,43 | 2,33 | 0,01 |
| Konstantan | 0,49 | 2,04 | 0,01 |
| Quecksilber | 0,958 | 1,04 | 0,9 |
| Nickelchrom (NiCr 80/20) | 1,1 | 0,91 | 0,1 |

*Typische Werte bei 20 °C (Tabellenbuch); je nach Reinheit/Legierung abweichend. Kupfer und Aluminium für Leiter nach IEC 60028 bzw. IEC 60889. R = ρ · L / A, κ = 1/ρ, R(θ) = R₂₀ · (1 + α · (θ − 20 °C)).*

#### Spannungsfall je Ampere und Meter

| A in mm² | 1~ cos φ = 1 | 1~ cos φ = 0,8 | 3~ cos φ = 1 | 3~ cos φ = 0,8 |
|---|---|---|---|---|
| 1,5 | 27,51 | 22,1 | 23,82 | 19,14 |
| 2,5 | 16,5 | 13,3 | 14,29 | 11,52 |
| 4 | 10,31 | 8,35 | 8,93 | 7,23 |
| 6 | 6,88 | 5,6 | 5,96 | 4,85 |
| 10 | 4,13 | 3,4 | 3,57 | 2,94 |
| 16 | 2,58 | 2,16 | 2,23 | 1,87 |
| 25 | 1,65 | 1,42 | 1,43 | 1,23 |
| 35 | 1,18 | 1,04 | 1,02 | 0,9 |
| 50 | 0,83 | 0,76 | 0,71 | 0,65 |
| 70 | 0,59 | 0,57 | 0,51 | 0,49 |
| 95 | 0,43 | 0,44 | 0,38 | 0,38 |
| 120 | 0,34 | 0,37 | 0,3 | 0,32 |
| 150 | 0,28 | 0,32 | 0,24 | 0,27 |
| 185 | 0,22 | 0,27 | 0,19 | 0,24 |
| 240 | 0,17 | 0,23 | 0,15 | 0,2 |
| 300 | 0,14 | 0,21 | 0,12 | 0,18 |

*ΔU = k · (ρ/A · cos φ + X' · sin φ) mit k = 2 (1~, Hin- und Rückleiter) bzw. √3 (3~), ρ bei 70 °C (PVC), X' = 0,08 mΩ/m. Spannungsfall = Tabellenwert · I · L / 1000 in V. Rechner „Spannungsfall“ rechnet genauer.*

#### Abmessungen und Gewicht von Leitern

| A in mm² | d in mm | R₂₀ max. in Ω/km | Cu in kg/km | Al in kg/km | Isolierung in mm |
|---|---|---|---|---|---|
| 1,5 | 1,38 | 12,1 | 13,3 | 4,1 | 0,8 |
| 2,5 | 1,78 | 7,41 | 22,2 | 6,8 | 0,8 |
| 4 | 2,26 | 4,61 | 35,6 | 10,8 | 1 |
| 6 | 2,76 | 3,08 | 53,3 | 16,2 | 1 |
| 10 | 3,57 | 1,83 | 88,9 | 27 | 1 |
| 16 | 4,51 | 1,15 | 142,2 | 43,2 | 1 |
| 25 | 5,64 | 0,727 | 222,2 | 67,5 | 1,2 |
| 35 | 6,68 | 0,524 | 311,2 | 94,5 | 1,2 |
| 50 | 7,98 | 0,387 | 444,5 | 135 | 1,4 |
| 70 | 9,44 | 0,268 | 622,3 | 189 | 1,4 |
| 95 | 11 | 0,193 | 844,6 | 256,5 | 1,6 |
| 120 | 12,36 | 0,153 | 1066,8 | 324 | 1,6 |
| 150 | 13,82 | 0,124 | 1333,5 | 405 | 1,8 |
| 185 | 15,35 | 0,0991 | 1644,7 | 499,5 | 2 |
| 240 | 17,48 | 0,0754 | 2133,6 | 648 | 2,2 |
| 300 | 19,54 | 0,0601 | 2667 | 810 | 2,4 |

*d = Durchmesser eines massiven Rundleiters; R₂₀ max. nach IEC 60228 (Kupfer blank, Klasse 1/2); Metallgewicht je Leiter mit Dichte Cu 8,89 und Al 2,70 kg/dm³ (Kabelgewicht = Metall · Aderzahl + Isolierung und Mantel, siehe Herstellerdatenblatt); Isolierwanddicke PVC 0,6/1 kV nach IEC 60502-1. Außendurchmesser sind herstellerabhängig.*

#### Kabel: Außendurchmesser und Gewicht

| Kabel | d in mm | Gewicht in kg/km | Quelle |
|---|---|---|---|
| NYM-J 1x2,5 | 5,8 | 70 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 1x4 | 6,4 | 80 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 1x6 | 6,8 | 105 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 1x10 | 8 | 155 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 1x16 | 9,1 | 230 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 1x25 | 12,3 | 330 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-O 2x1,5 | 7,8 | 115 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-O 2x2,5 | 8,9 | 157 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 3x1,5 | 8,2 | 135 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 3x2,5 | 9,4 | 190 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 3x4 | 10,8 | 265 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 3x6 | 12,2 | 315 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 3x10 | 14,7 | 465 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 4x1,5 | 8,8 | 160 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 4x2,5 | 10,2 | 230 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 4x4 | 12,1 | 330 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 4x6 | 13,3 | 460 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 4x10 | 16,1 | 690 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 4x16 | 19 | 1090 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 4x25 | 23,4 | 1640 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 4x35 | 25,7 | 2090 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 5x1,5 | 9,5 | 190 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 5x2,5 | 11 | 270 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 5x4 | 13,2 | 410 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 5x6 | 14,5 | 540 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 5x10 | 17,7 | 850 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 5x16 | 21,2 | 1350 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 5x25 | 25,7 | 1990 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 7x1,5 | 10,5 | 235 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 7x2,5 | 12,6 | 350 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 8x1,5 | 12,5 | 237 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 10x1,5 | 14,3 | 330 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 12x1,5 | 14,4 | 400 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 12x2,5 | 15,4 | 660 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYM-J 16x1,5 | 15,8 | 457 | Faber Kabel, Datenblatt NYM-J/NYM-O (Richtwerte) |
| NYY-J 1x10 | 10 | 168 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x16 | 11 | 230 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x25 | 13 | 349 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x35 | 14 | 448 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x50 | 15 | 584 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x70 | 17 | 796 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x95 | 19 | 1067 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x120 | 20 | 1296 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x150 | 22 | 1592 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x185 | 24 | 1954 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x240 | 27 | 2525 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 1x300 | 30 | 3150 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 3x1,5 | 11 | 178 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 3x2,5 | 12 | 223 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 3x4 | 14 | 313 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 3x6 | 15 | 390 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 3x10 | 17 | 539 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 3x16 | 19 | 749 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x1,5 | 12 | 208 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x2,5 | 13 | 261 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x4 | 15 | 371 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x6 | 16 | 469 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x10 | 19 | 656 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x16 | 20 | 929 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x25 | 25 | 1452 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x35 | 25 | 1697 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x50 | 30 | 2357 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x70 | 33 | 3142 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x95 | 37 | 4301 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x120 | 43 | 5439 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x150 | 44 | 6483 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x185 | 51 | 8244 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 4x240 | 56 | 10501 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x1,5 | 13 | 238 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x2,5 | 14 | 303 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x4 | 16 | 433 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x6 | 18 | 551 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x10 | 20 | 779 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x16 | 22 | 1122 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x25 | 28 | 1758 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x35 | 31 | 2314 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x50 | 35 | 3129 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x70 | 40 | 4314 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x95 | 47 | 5905 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 5x120 | 50 | 7166 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 7x1,5 | 14 | 289 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 7x2,5 | 15 | 374 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 10x1,5 | 16 | 394 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 12x1,5 | 17 | 455 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 12x2,5 | 19 | 599 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 19x1,5 | 20 | 627 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| NYY-J 24x1,5 | 22 | 792 | Waskönig+Walter, Datenblatt NYY-J (2019) |
| J-Y(St)Y 1x2x0,6 | 5 | 30 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 2x2x0,6 | 5,5 | 40 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 4x2x0,6 | 6,7 | 60 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 10x2x0,6 | 9 | 110 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 1x2x0,8 | 6 | 40 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 2x2x0,8 | 7 | 60 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 3x2x0,8 | 8,5 | 80 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 4x2x0,8 | 9 | 100 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 5x2x0,8 | 9,5 | 120 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 6x2x0,8 | 10,5 | 140 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 8x2x0,8 | 11,5 | 170 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 10x2x0,8 | 13 | 220 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 12x2x0,8 | 14 | 250 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 16x2x0,8 | 15,5 | 320 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 20x2x0,8 | 16,5 | 380 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 24x2x0,8 | 19 | 460 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 30x2x0,8 | 20 | 560 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 40x2x0,8 | 22,5 | 710 | Lapp, Produktinformation J-Y(ST)Y…LG |
| J-Y(St)Y 50x2x0,8 | 25,5 | 900 | Lapp, Produktinformation J-Y(ST)Y…LG |
| LiYCY 2x0,25 | 4,5 | 32 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 3x0,25 | 4,7 | 37 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 4x0,25 | 5 | 41,3 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 6x0,25 | 6 | 58 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 8x0,25 | 7,1 | 73 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 10x0,25 | 7,5 | 82 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 12x0,25 | 7,7 | 98 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 2x0,5 | 5,6 | 47 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 3x0,5 | 5,9 | 55 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 4x0,5 | 6,3 | 70 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 5x0,5 | 7 | 90 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 7x0,5 | 7,6 | 112 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 10x0,5 | 9,3 | 139 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 12x0,5 | 9,6 | 177 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 2x0,75 | 6 | 53 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 3x0,75 | 6,3 | 65 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 4x0,75 | 7 | 79 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 5x0,75 | 7,6 | 109 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 7x0,75 | 8,2 | 156 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 12x0,75 | 10,8 | 218 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 2x1 | 6,3 | 72 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 3x1 | 6,8 | 90 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 4x1 | 7,3 | 109 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 7x1 | 8,6 | 171 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 2x1,5 | 7,1 | 90 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 3x1,5 | 7,5 | 115 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 4x1,5 | 8,1 | 129 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 5x1,5 | 8,8 | 176 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| LiYCY 7x1,5 | 9,5 | 220 | Lapp, Datenblatt UNITRONIC LiYCY (2020) |
| Cat 7 S/FTP 4x2xAWG23 | 7,2 | 51 | DIGITUS DK-1743-VH (Cat 7, Dca) |

*Richtwerte aus den genannten Herstellerdatenblättern (Durchmesser ca.-Werte, NYY-J auf mm gerundet). Andere Hersteller weichen ab – für die Ausführung Datenblatt des eingesetzten Kabels verwenden. Genutzt von der Projektliste „Kabeltrasse“.*

#### Kabel- & Leitungstypen

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

#### Aderfarben (Europa und international)

| Land / System | L1 | L2 | L3 | N | PE |
|---|---|---|---|---|---|
| Europa (IEC 60445) | braun | schwarz | grau | blau | grün-gelb |
| UK vor 2004 | rot | gelb | blau | schwarz | grün-gelb |
| USA 120/208 V | schwarz | rot | blau | weiß | grün |
| USA 277/480 V | braun | orange | gelb | grau | grün |
| Kanada | rot | schwarz | blau | weiß | grün |
| Australien / Neuseeland | rot | weiß | dunkelblau | schwarz | grün-gelb |
| China | gelb | grün | rot | hellblau | grün-gelb |

*Europa: IEC 60445 / HD 308 S2 (DIN EN 60445 / VDE 0197, seit 2004 in DE, UK nach 2004). PEN-Leiter: grün-gelb mit blauer Markierung an den Enden (oder blau mit grün-gelber Markierung). Maschinen nach IEC 60204-1 Abschn. 13.2 (Empfehlung): Hauptstromkreise AC/DC schwarz, AC-Steuerstromkreise rot, DC-Steuerstromkreise blau, Fremdspannung (bleibt bei Hauptschalter AUS unter Spannung) orange. USA/Kanada: NEC/CEC schreiben nur N (weiß/grau) und Erde (grün, grün-gelb oder blank) vor, Außenleiterfarben sind übliche Praxis. Ohne Gewähr – vor Arbeiten immer messen.*

#### Kabelverschraubungen und Installationsrohre

| Gewinde | Gewinde-Ø in mm | Klemmbereich in mm |
|---|---|---|
| M12 × 1,5 | 12 | 3–7 |
| M16 × 1,5 | 16 | 4,5–10 |
| M20 × 1,5 | 20 | 6–13 |
| M25 × 1,5 | 25 | 9–17 |
| M32 × 1,5 | 32 | 13–21 |
| M40 × 1,5 | 40 | 16–28 |
| M50 × 1,5 | 50 | 21–35 |
| M63 × 1,5 | 63 | 34–48 |
| PG 7 | 12,5 | – |
| PG 9 | 15,2 | – |
| PG 11 | 18,6 | – |
| PG 13,5 | 20,4 | – |
| PG 16 | 22,5 | – |
| PG 21 | 28,3 | – |
| PG 29 | 37,0 | – |
| PG 36 | 47,0 | – |
| PG 42 | 54,0 | – |
| PG 48 | 59,3 | – |

*Metrisch EN 62444 / EN 50262: Gewinde-Außen-Ø = Kennzahl (= Außendurchmesser der Installationsrohre M16–M63 nach EN 61386); Klemmbereich typisch, herstellerabhängig. PG (DIN 40430, nicht mehr genormt): Gewinde-Außen-Ø.*

### Schutz & Sicherungen

#### Überlastschutz (Koordination)

*I_B ≤ I_n ≤ I_z und I_2 ≤ 1,45 · I_z* — `overload_protection`

```text
Bedingungen nach IEC 60364-4-43 / DIN VDE 0100-430:
(1) I_B ≤ I_n ≤ I_z    (2) I_2 ≤ 1,45 · I_z
I_2 = 1,45 · I_n für LS-Schalter (IEC 60898-1); gG: 2,1 (≤ 4 A), 1,9 (< 16 A), 1,6 (≥ 16 A) · I_n (IEC 60269-1).
```

**Eingaben:** Betriebsstrom I_B [A], Strombelastbarkeit I_z [A], Schutzorgan

**Ergebnisse:** Bemessungsstrom I_n [A], Auslösestrom I_2 [A], Grenze 1,45 · I_z [A]

#### Abschaltbedingung / max. Länge (TN)

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

#### Auslösekennlinien

*Zeit-Strom-Bereich von LS-Schaltern B, C, D (IEC 60898-1)* — `trip_curves`

```text
Auslösebereich zwischen Nichtauslöse- und Auslösekennlinie nach den Prüfpunkten der IEC 60898-1: 1,13 · I_n ≥ 1 h, 1,45 · I_n < 1 h, 2,55 · I_n 1 … 60 s (I_n ≤ 32 A), magnetisch B 3–5, C 5–10, D 10–20 · I_n (< 0,1 s). Zwischen den Prüfpunkten schematisch (im log-log-Diagramm gerade). Genaue Kennlinien liefert der Hersteller.
Mit Kurzschlussstrom: Auslösezeit-Bereich am Fehlerort (Abschaltbedingung: Auslösung im magnetischen Bereich).
```

**Eingaben:** Charakteristik, Bemessungsstrom I_n [A], Fehler-/Kurzschlussstrom [A]

**Ergebnisse:** Sicher magnetisch ab [A], Keine Schnellauslösung bis [A], I_k / I_n, Bereich bei I_k

#### Auslösekennlinien: Prüfströme

| Gerät | Nichtauslösen | Auslösen | Hinweis |
|---|---|---|---|
| LS B, C, D (thermisch) | 1,13 · I_n ≥ 1 h | 1,45 · I_n < 1 h | 2,55 · I_n: 1–60 s (I_n ≤ 32 A), 1–120 s (> 32 A) |
| LS B (magnetisch) | 3 · I_n ≥ 0,1 s | 5 · I_n < 0,1 s | Leitungsschutz |
| LS C (magnetisch) | 5 · I_n ≥ 0,1 s | 10 · I_n < 0,1 s | Motoren, Leuchten |
| LS D (magnetisch) | 10 · I_n ≥ 0,1 s | 20 · I_n < 0,1 s | hohe Einschaltströme |
| LS K | 1,05 · I_n; 8 · I_n | 1,2 · I_n; 14 · I_n | IEC 60947-2, Herstellerangaben |
| LS Z | 1,05 · I_n; 2 · I_n | 1,2 · I_n; 3 · I_n | Halbleiter, Messkreise |
| Sicherung gG (I_n ≥ 16 A) | 1,25 · I_n (I_nf) | 1,6 · I_n (I_f) | Prüfdauer 1–4 h je nach I_n |
| RCD unverzögert | 0,5 · I_Δn | I_Δn ≤ 300 ms, 2 · I_Δn ≤ 150 ms, 5 · I_Δn ≤ 40 ms | Typ AC, A, F, B je nach Fehlerstromform |
| RCD selektiv (S) | I_Δn: ≥ 130 ms | I_Δn ≤ 500 ms, 5 · I_Δn ≤ 150 ms | vorgeschaltet für Selektivität |

*LS-Schalter IEC 60898-1 (B, C, D) bzw. IEC 60947-2 (K, Z); gG-Sicherungen IEC 60269-2; RCD IEC 61008/61009 (Abschaltzeiten bei Bemessungsdifferenzstrom, unverzögert/selektiv S). Verlauf der Kennlinien: Rechner „Auslösekennlinien“. Gegen die gültige Ausgabe prüfen.*

#### Sicherungen: Betriebsklassen

| Klasse | Bereich | Schutzobjekt / Einsatz |
|---|---|---|
| gG | Ganzbereich | Kabel und Leitungen, allgemein |
| gL | Ganzbereich | alte deutsche Bezeichnung für Leitungsschutz, heute gG |
| aM | Teilbereich | Motorstromkreise (Kurzschlussschutz, Überlast durch Motorschutzrelais) |
| gR | Ganzbereich | Halbleiter (sehr flink) |
| gS | Ganzbereich | Halbleiter und Leitungen |
| aR | Teilbereich | Halbleiter (Kurzschlussschutz) |
| gTr | Ganzbereich | Transformatoren (NS-Seite) |
| gPV | Ganzbereich | Photovoltaik-Strings (DC, IEC 60269-6) |

*IEC 60269-1/-2/-4/-6 (DIN VDE 0636). 1. Buchstabe: g = Ganzbereich (Überlast und Kurzschluss), a = Teilbereich (nur Kurzschluss, Überlastschutz durch andere Geräte). 2. Buchstabe: Schutzobjekt.*

#### D-Sicherungen (Neozed/Diazed): Größen und Kennmelder

| I_n | Kennmelder | Neozed (D0) | Diazed (D) |
|---|---|---|---|
| 2 A | rosa | D01 | DII |
| 4 A | braun | D01 | DII |
| 6 A | grün | D01 | DII |
| 10 A | rot | D01 | DII |
| 13 A | schwarz | D01 | DII |
| 16 A | grau | D01 | DII |
| 20 A | blau | D02 | DII |
| 25 A | gelb | D02 | DII |
| 35 A | schwarz | D02 | DIII |
| 50 A | weiß | D02 | DIII |
| 63 A | kupfer | D02 | DIII |
| 80 A | silber | D03 | DIV |
| 100 A | rot | D03 | DIV |

*IEC 60269-3 / DIN VDE 0636-3 (D-System, für Laien bedienbar). Neozed D0: D01 E14 (2–16 A), D02 E18 (20–63 A), D03 M30 × 2 (80–100 A), 400 V AC. Diazed D: DII E27 (2–25 A), DIII E33 (35–63 A), DIV R 1¼" (80–100 A), 500 V AC. Passschrauben/-hülsen verhindern zu große Einsätze. Größe = Bereich der Größe; einzelne Stufen herstellerabhängig.*

#### NH-Sicherungen: Größen

| Größe | Bemessungsstrom | Spannung | Hinweis |
|---|---|---|---|
| NH000 | bis 100 A | 500/690 V AC | kleinste Bauform |
| NH00 | bis 160 A | 500/690 V AC | häufig in Verteilungen |
| NH0 | bis 160 A | 500/690 V AC | nicht für Neuanlagen |
| NH1 | bis 250 A | 500/690 V AC | – |
| NH2 | bis 400 A | 500/690 V AC | – |
| NH3 | bis 630 A | 500/690 V AC | – |
| NH4a | bis 1250 A | 500/690 V AC | Sonderbauform |

*IEC 60269-2 / DIN VDE 0636-2 (NH-Messersicherungen, nur durch Fachkräfte oder unterwiesene Personen bedienbar, Aufsteckgriff mit Stulpe). Größte Bemessungsströme je Größe für 500/690 V AC; Herstellerangaben prüfen.*

#### Zylindersicherungen

| Abmessung | Bemessungsstrom bis | Spannung | Hinweis |
|---|---|---|---|
| 8,5 × 31,5 mm | 20 A | 400 V AC | kleine Verbraucher |
| 10 × 38 mm | 32 A | 400–690 V AC | auch gPV bis 1000/1500 V DC |
| 14 × 51 mm | 50 A | 400–690 V AC | auch aM für Motoren |
| 22 × 58 mm | 125 A | 400–690 V AC | auch aM für Motoren |

*IEC 60269-2 (zylindrische Sicherungseinsätze, z. B. für Sicherungs-Trennschalter). Höchste übliche Bemessungsströme; je nach Hersteller und Spannung kleiner – Datenblatt prüfen.*

#### Feinsicherungen (G-Sicherungen)

| Zeichen | Charakteristik | Einsatz |
|---|---|---|
| FF | superflink | Halbleiter, Messgeräte |
| F | flink | ohne Einschaltstromspitzen |
| M | mittelträge | kleine Einschaltströme |
| T | träge | Netzteile, Transformatoren, Motoren |
| TT | superträge | hohe Einschaltströme |

*IEC 60127 / DIN EN 60127. Bauformen 5 × 20 mm und 6,3 × 32 mm. Schaltvermögen: L = niedrig (Glasrohr, typ. 35 A oder 10 · I_N), E = erhöht, H = hoch (Keramik mit Sandfüllung, 1500 A). Kennzeichnung z. B. T 1,6 A H 250 V.*

#### Kfz-Flachsicherungen: Farben

| Strom | Farbe |
|---|---|
| 1 A | schwarz |
| 2 A | grau |
| 3 A | violett |
| 4 A | rosa |
| 5 A | hellbraun |
| 7,5 A | braun |
| 10 A | rot |
| 15 A | blau |
| 20 A | gelb |
| 25 A | farblos/weiß |
| 30 A | grün |
| 35 A | blaugrün |
| 40 A | orange |

*ISO 8820-3 (Flachstecksicherungen ATO/ATC und Mini). 35 A ist nicht in allen Bauformen üblich. Maxi-Sicherungen verwenden teilweise andere Farben.*

#### UL/CSA-Sicherungsklassen

| Klasse | Spannung | Bemessungsstrom | Schaltvermögen | Merkmal / Einsatz |
|---|---|---|---|---|
| CC | 600 V AC | 0–30 A | 200 kA | klein (13/32" × 1½"), Steuerstromkreise, Motoren |
| CD | 600 V AC | 35–60 A | 200 kA | wie CC, größere Ströme |
| G | 480–600 V AC | 0–60 A | 100 kA | Längen je Strombereich verschieden |
| H | 250/600 V AC | 0–600 A | 10 kA | ältere Bauart, geringes Schaltvermögen |
| J | 600 V AC | 0–600 A | 200 kA | strombegrenzend, kompakt |
| K | 250/600 V AC | 0–600 A | 50–200 kA | Maße wie H, ohne Unverwechselbarkeit |
| L | 600 V AC | 601–6000 A | 200 kA | Schraubbefestigung, Einspeisungen |
| R (RK1, RK5) | 250/600 V AC | 0–600 A | 200 kA | Unverwechselbarkeit durch Nut, Motorabgänge |
| T | 300/600 V AC | 0–1200 A | 200 kA | sehr kompakt, sehr flink |

*UL 248 / CSA C22.2 No. 248. Klassen mit Unverwechselbarkeit (rejection: CC, J, L, R, T) nehmen keine Sicherung mit geringerem Schaltvermögen auf. Werte als Übersicht – Herstellerdaten und gültige Normausgabe prüfen.*

### Kurzschluss & Erdung

#### Kurzschlussstrom am Leitungsende

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

#### Minimaler Kurzschlussstrom

*I_k1min, I_k2min am Leitungsende (c_min = 0,95)* — `short_circuit_min`

```text
I_k1min = c_min · U₀ / |Z_V + Z_L + Z_PE|    I_k2min = c_min · √3 · U₀ / |Z_V + 2 · Z_L|
c_min = 0,95 (NS, IEC 60909-0 / DIN EN 60909-0). Leiterwiderstand bei Endtemperatur θ (Kurzschlussende, meist 80 °C für PVC bzw. Betriebstemperatur 70/90 °C). Z_V = Schleifenimpedanz am Anfang der Leitung (L–PE, gemessen oder vom Netzbetreiber), X'_L = 0,08 mΩ/m je Leiter.
Prüfung: I_k1min ≥ I_a der Schutzeinrichtung (Abschaltbedingung).
```

**Eingaben:** Nennspannung gegen Erde U₀ [V], Schleifenimpedanz am Leitungsanfang Z_V [mΩ], Leitermaterial, Leitungslänge L [m], Querschnitt Außenleiter, Querschnitt PE, Leitertemperatur θ [°C], Abschaltstrom I_a der Schutzeinrichtung [A]

**Ergebnisse:** I_k1min (L–PE) [A], I_k1min (L–N, N wie PE) [A], I_k2min (L–L) [A], Schleifenimpedanz Z_s am Ende [mΩ], I_k1min / I_a

#### Netzsysteme TN, TT, IT (Schaltbild)

*Aufbau von TN-S, TN-C, TN-C-S, TT und IT mit Zeichnung* — `earthing_systems`

```text
IEC 60364-1 / DIN VDE 0100-100: 1. Buchstabe Erdung der Stromquelle (T = direkt geerdet, I = isoliert oder hochohmig), 2. Buchstabe Erdung der Körper (T = eigener Erder, N = über den Schutzleiter mit dem Sternpunkt). S = N und PE getrennt, C = kombiniert als PEN. Schutzmaßnahmen siehe Rechner Erdung/RCD und Abschaltbedingung.
```

**Eingaben:** Netzsystem

**Ergebnisse:** Stromquelle, Körper (Gehäuse), Fehlerschutz, Hinweis

#### Erdungssystem und Koordination mit Fehlerstrom-Schutzeinrichtung

*TT: R_A · I_Δn ≤ U_L    TN: Z_s · I_a ≤ U₀* — `earthing_rcd`

```text
TT-System (IEC 60364-4-41 / DIN VDE 0100-410, 411.5.3): R_A · I_Δn ≤ U_L, U_L = 50 V AC (25 V in besonderen Räumen, z. B. landwirtschaftliche Betriebsstätten). R_A = Summe aus Erder- und Schutzleiterwiderstand.
TN-System (411.4.4): Z_s · I_a ≤ U₀; mit RCD ist I_a = I_Δn (5 · I_Δn für 0,04 s).
Gegen die gültige Ausgabe prüfen.
```

**Eingaben:** Netzsystem, Bemessungsdifferenzstrom I_Δn, Berührungsspannung U_L, Nennspannung U₀ [V], Gemessen: R_A bzw. Z_s [Ω]

**Ergebnisse:** Zulässiger Widerstand (R_A bzw. Z_s) [Ω], Berührungsspannung bei I_Δn [V], Max. I_Δn für den gemessenen Widerstand [mA]

### Transformator & Kompensation

#### Transformator: Ströme & I_k

*Bemessungsströme, Übersetzung, Kurzschlussstrom* — `transformer`

```text
I = S / (√3 · U) bei 3~ bzw. S / U bei 1~    ü = U1 / U2
I_k ≈ c · I_r2 · 100 % / u_k   (starres Netz, c = 1,05);  Dauerkurzschlussstrom I_kD = I_r · 100 % / u_k (ohne c)
Übliche u_k (Haag Band 05 Trafo, 2007): Verteiltrafos 10 … 20 kV 4 … 6 %, 110 kV 10 … 14 %, 380 kV 11 … 20 %. Beispiel Haag: 400 kVA, u_k = 4 % → I_r = 577 A, I_kD = 14,43 kA. „Spannungssteife“ Trafos (kleines u_k) liefern höhere Kurzschlussströme.
```

**Eingaben:** Ausführung, Bemessungsleistung S_r [kVA], Oberspannung U_1 [V], Unterspannung U_2 [V], Kurzschlussspannung u_k [%]

**Ergebnisse:** Bemessungsstrom primär [A], Bemessungsstrom sekundär [A], Übersetzung ü, Kurzschlussstrom sekundär I_k [kA], Dauerkurzschlussstrom I_kD (ohne c) [kA]

#### Primär-/Sekundärwicklung von Transformatoren

*Übersetzung, Ströme und Windungszahlen (Kleintransformator)* — `transformer_windings`

```text
ü = U₁/U₂ = N₁/N₂ = I₂/I₁    Windungen je Volt w = 1 / (4,44 · f · B · A_Fe)
N₁ = w · U₁    N₂ = w · U₂ · (1 + Zuschlag) (Ausgleich der Spannungsabfälle, Kleintrafos 5–10 %)
B: Dynamoblech 1,0–1,4 T (Kernblech EI). Ohne Angabe A_Fe ≈ √S cm² (Faustformel für EI-Kerne).
```

**Eingaben:** Bemessungsleistung S [VA], Primärspannung U₁ [V], Sekundärspannung U₂ [V], Frequenz, Flussdichte B [T], Eisenquerschnitt A_Fe [cm²], Zuschlag Sekundärwindungen [%]

**Ergebnisse:** Übersetzung ü, Primärstrom I₁ [A], Sekundärstrom I₂ [A], Windungen je Volt [1/V], Windungen primär N₁, Windungen sekundär N₂, Verwendeter Eisenquerschnitt [cm²]

#### Blindleistungskompensation

*Q_C und Kondensator von cos φ1 auf cos φ2* — `pf_correction`

```text
Q_C = P · (tan φ1 − tan φ2)
Kapazität je Kondensator (U = Außenleiterspannung, ω = 2πf):
Dreieck: C = Q_C / (3 · U² · ω)    Stern: C = Q_C / (U² · ω)    1~: C = Q_C / (U² · ω)
Bei Oberschwingungen verdrosselte Anlagen vorsehen.
```

**Eingaben:** Wirkleistung P [kW], cos φ1 (Ist), cos φ2 (Ziel), Spannung U [V], Frequenz f [Hz], Schaltung

**Ergebnisse:** Kompensationsleistung Q_C [kvar], Kapazität je Kondensator [µF], Scheinleistung vorher [kVA], Scheinleistung nachher [kVA], Strom vorher [A], Strom nachher [A]

#### Blindleistungskompensation von MS/NS-Transformatoren

*Leerlauf- und Last-Blindleistung, Festkompensation* — `transformer_pfc`

```text
Leerlauf: Q₀ = i₀ · S_N (Magnetisierung, immer vorhanden)
Last: Q_L = u_k · S_N · (S/S_N)² (Streufeld)
Festkompensation des Transformators ≈ Q₀ (Kondensator auf der NS-Seite, dauernd zugeschaltet). i₀ und u_k vom Typenschild bzw. Prüfprotokoll; Öltransformatoren typ. i₀ = 0,5–2 %, Gießharz 0,5–1,5 %. Höchstwert der Festkompensation nach den Technischen Anschlussbedingungen des Netzbetreibers prüfen (Resonanz, Überkompensation).
```

**Eingaben:** Bemessungsleistung S_N [kVA], Leerlaufstrom i₀ [%], Kurzschlussspannung u_k [%], Belastung S [kVA]

**Ergebnisse:** Leerlauf-Blindleistung Q₀ [kvar], Last-Blindleistung Q_L [kvar], Gesamt Q₀ + Q_L [kvar], Auslastung S/S_N [%]

#### Kondensatorleistung bei verschiedenen Spannungen

*Q = Q_N · (U/U_N)² · (f/f_N) und Kapazität* — `capacitor_voltage`

```text
Q = Q_N · (U/U_N)² · (f/f_N)
Kapazität (3~): Dreieck C_Δ = Q / (3 · 2πf · U²) je Kondensator, Stern C_Y = Q / (2πf · U²) je Kondensator (U = Außenleiterspannung). 1~: C = Q / (2πf · U²).
Kondensatoren dürfen dauerhaft höchstens mit 1,1 · U_N betrieben werden (IEC 60831-1).
```

**Eingaben:** Bemessungsleistung Q_N [kvar], Bemessungsspannung U_N [V], Bemessungsfrequenz f_N, Betriebsspannung U [V], Netzfrequenz f, Ausführung

**Ergebnisse:** Leistung bei U und f [kvar], Q / Q_N [%], Kapazität je Kondensator [µF], Strom (Außenleiter) [A]

### Schutzarten, Schaltschrank & Sicherheit

#### IP-Schutzarten (IP-Code)

| Ziffer | 1. Ziffer: Fremdkörper | 1. Ziffer: Berührung | 2. Ziffer: Wasser |
|---|---|---|---|
| 0 | kein Schutz | kein Schutz | kein Schutz |
| 1 | ≥ 50 mm | Handrücken | senkrecht fallendes Tropfwasser |
| 2 | ≥ 12,5 mm | Finger | Tropfwasser bis 15° Neigung |
| 3 | ≥ 2,5 mm | Werkzeug | Sprühwasser bis 60° gegen die Senkrechte |
| 4 | ≥ 1,0 mm | Draht | Spritzwasser aus allen Richtungen |
| 5 | staubgeschützt | Draht | Strahlwasser (Düse 6,3 mm) |
| 6 | staubdicht | Draht | starkes Strahlwasser (Düse 12,5 mm) |
| 7 | – | – | zeitweiliges Untertauchen (1 m, 30 min) |
| 8 | – | – | dauerndes Untertauchen (nach Vereinbarung) |
| 9 | – | – | Hochdruck-/Dampfstrahl (80 °C), IPX9 |

*IEC 60529 / EN 60529 (DIN VDE 0470-1) inkl. Änderung 2 (2013) für IPX9. Beispiel IP54: staubgeschützt, Draht; Spritzwasser. X = nicht geprüft. Zusatzbuchstaben A–D (Berührungsschutz) und H, M, S, W möglich.*

#### IK-Code (Stoßfestigkeit)

| IK | Energie | Masse | Fallhöhe |
|---|---|---|---|
| IK00 | kein Schutz | – | – |
| IK01 | 0,14 J | 0,25 kg | 56 mm |
| IK02 | 0,2 J | 0,25 kg | 80 mm |
| IK03 | 0,35 J | 0,25 kg | 140 mm |
| IK04 | 0,5 J | 0,25 kg | 200 mm |
| IK05 | 0,7 J | 0,25 kg | 280 mm |
| IK06 | 1 J | 0,25 kg | 400 mm |
| IK07 | 2 J | 0,5 kg | 400 mm |
| IK08 | 5 J | 1,7 kg | 300 mm |
| IK09 | 10 J | 5 kg | 200 mm |
| IK10 | 20 J | 5 kg | 400 mm |

*IEC 62262 / EN 62262: Schutz von Gehäusen gegen mechanische Stöße. Prüfung mit Fallhammer (Masse, Fallhöhe).*

#### NEMA-Gehäusetypen

| Typ | Einsatz / Schutz | ≈ IP |
|---|---|---|
| 1 | innen, Berührung, fallender Schmutz | IP10 |
| 2 | innen, Tropfwasser | IP11 |
| 3 | außen, Regen, Schnee, Staub, Eis | IP54 |
| 3R | außen, Regen, Schnee, Eis (nicht Staub) | IP14 |
| 3S | wie 3, bedienbar bei Vereisung | IP54 |
| 4 | innen/außen, Strahlwasser | IP56 |
| 4X | wie 4 + korrosionsbeständig | IP56 |
| 5 | innen, Staub, Tropfwasser | IP52 |
| 6 | zeitweises Untertauchen | IP67 |
| 6P | längeres Untertauchen | IP67 |
| 7, 8, 9, 10 | explosionsgefährdete Bereiche / Bergbau | – |
| 12, 12K | innen, Industrie: Staub, Tropföl, Kühlmittel | IP52 |
| 13 | innen, Staub, Spritzöl und Kühlmittel | IP54 |

*NEMA 250 (Nordamerika). Die IP-Angabe ist die Mindestentsprechung nach NEMA-Umrechnungstabelle – NEMA-Typen verlangen zusätzliche Prüfungen (Korrosion, Eis, Öl); eine Umrechnung von IP nach NEMA ist nicht zulässig.*

#### Schutzklassen

| Klasse | Schutzmaßnahme | Kennzeichen / Hinweis |
|---|---|---|
| 0 | nur Basisisolierung | in DE/CH nicht zulässig |
| I | Basisisolierung + Schutzleiteranschluss | Schutzleiterzeichen ⏚ |
| II | doppelte oder verstärkte Isolierung | Doppelquadrat ⧈, kein PE |
| III | Schutzkleinspannung SELV/PELV | ≤ 50 V AC / 120 V DC |

*IEC 61140 / DIN EN 61140 (VDE 0140-1)*

#### Schaltschrank-Wärmebilanz

*Innentemperatur, Kühlleistung, Lüfter* — `enclosure_cooling`

```text
Wirksame Oberfläche nach IEC/TR 60890 (DIN VDE 0660-507):
freistehend: A = 1,8 · H · (B + T) + 1,4 · B · T    Wandmontage: A = 1,4 · B · (H + T) + 1,8 · T · H
ΔT = P_V / (k · A)    Kühlleistung Q_K = P_V − k · A · (θ_i,max − θ_u)
Lüfter: V̇ = 3,1 · Q / ΔT  [m³/h] (Meereshöhe). k ≈ 5,5 W/(m²·K) für Stahlblech.
```

**Eingaben:** Verlustleistung P_V [W], Höhe H [m], Breite B [m], Tiefe T [m], Aufstellung, Wärmedurchgangskoeffizient k [W/(m²·K)], Umgebungstemperatur θ_u [°C], Max. Innentemperatur θ_i [°C]

**Ergebnisse:** Wirksame Oberfläche A [m²], Übertemperatur ohne Kühlung ΔT [K], Innentemperatur ohne Kühlung [°C], Erforderliche Kühlleistung [W], Lüfter-Volumenstrom (falls θ_u < θ_i) [m³/h]

#### 5 Sicherheitsregeln & Schutzabstände

| Thema | Wert / Regel |
|---|---|
| Regel 1 | Freischalten |
| Regel 2 | Gegen Wiedereinschalten sichern |
| Regel 3 | Spannungsfreiheit feststellen (allpolig) |
| Regel 4 | Erden und kurzschließen |
| Regel 5 | Benachbarte, unter Spannung stehende Teile abdecken oder abschranken |
| Schutzabstand Elektrofachkraft ≤ 1 kV | 0,5 m |
| Schutzabstand Elektrofachkraft 1 … 30 kV | 1,5 m |
| Schutzabstand Elektrofachkraft 30 … 110 kV | 2,0 m |
| Schutzabstand Elektrofachkraft 110 … 220 kV | 3,0 m |
| Schutzabstand Elektrofachkraft 220 … 380 kV | 4,0 m |
| Bauarbeiten/Laien ≤ 1 kV | 1,0 m |
| Bauarbeiten/Laien 1 … 110 kV | 3,0 m |
| Bauarbeiten/Laien 110 … 220 kV | 4,0 m |
| Bauarbeiten/Laien 220 … 380 kV | 5,0 m |
| Gefahrenzone ≤ 1 kV | Abdecken (keine Berührung) |
| Gefahrenzone 10 kV innen / 20 kV / 110 kV | 120 / 220 / 1100 mm |

*Quelle: Haag „Grundlagen der Stromversorgung“, Band 09 Arbeitssicherheit (2007), S. 7–9 (DIN VDE 0105-100 Tab. 101–103, Ausschnitt). Gegen die gültige DIN VDE 0105-100 und DGUV-Vorschrift 3 prüfen.*

### Schaltzeichen & Abkürzungen

#### Schaltzeichen

*Grafische Symbole nach IEC 60617 (vereinfacht)* — `electrical_symbols`

```text
Schaltzeichen nach IEC 60617 / DIN EN 60617 (Stromlaufpläne) und Installationssymbole nach DIN EN 60617-11 (Installationspläne), vereinfacht gezeichnet. Bauteilkennbuchstaben nach IEC 81346-2 (z. B. R Widerstand, C Kondensator, Q Schaltgerät, K Relais, F Schutzgerät, M Motor, P Messgerät, E Leuchte).
```

**Eingaben:** Gruppe

**Ergebnisse:** Symbole

#### ANSI-Gerätenummern (IEEE C37.2)

| Nr. | Funktion |
|---|---|
| 2 | Zeitrelais (Anlauf/Verzögerung) |
| 21 | Distanzschutz |
| 25 | Synchronisierungs-/Synchroncheck-Relais |
| 27 | Unterspannungsschutz |
| 32 | Rückleistungs-/Richtungsleistungsschutz |
| 37 | Unterstrom-/Unterleistungsschutz |
| 40 | Erregungsverlustschutz |
| 46 | Schieflastschutz (Gegensystem) |
| 47 | Phasenfolge-/Spannungsasymmetrieschutz |
| 49 | thermischer Überlastschutz |
| 50 | unverzögerter Überstromschutz |
| 51 | abhängiger/verzögerter Überstromschutz |
| 52 | Leistungsschalter (AC) |
| 59 | Überspannungsschutz |
| 63 | Druck-/Buchholzschutz |
| 64 | Erdschlussschutz |
| 67 | gerichteter Überstromschutz |
| 79 | Wiedereinschaltrelais (AWE) |
| 81 | Frequenzschutz (81O/81U) |
| 86 | Verriegelungs-/Lockout-Relais |
| 87 | Differentialschutz |
| 89 | Trennschalter |
| 94 | Auslöserelais |

*IEEE C37.2 (ANSI): Nummern für Schutz- und Schaltfunktionen, z. B. 50/51 für Überstromzeitschutz. Zusätze: N = Neutralleiter/Erde (berechnet), G = Erde (gemessen), P = Phase, V = spannungsabhängig (51V), T = Transformator (87T).*

#### Geräteabkürzungen

| Kürzel | Deutsch | Englisch |
|---|---|---|
| MCB | Leitungsschutzschalter (LS) | miniature circuit breaker |
| MCCB | Kompaktleistungsschalter | moulded-case circuit breaker |
| ACB | offener Leistungsschalter | air circuit breaker |
| MPCB | Motorschutzschalter | motor protection circuit breaker |
| RCD | Fehlerstrom-Schutzeinrichtung | residual current device |
| RCCB | FI-Schutzschalter (ohne Überstromauslöser) | residual current circuit breaker |
| RCBO | FI/LS-Schalter | RCCB with overcurrent protection |
| AFDD | Brandschutzschalter | arc fault detection device |
| SPD | Überspannungsschutzgerät | surge protective device |
| IMD | Isolationsüberwachungsgerät | insulation monitoring device |
| RCM | Differenzstromüberwachung | residual current monitor |
| OLR | Überlastrelais (Motorschutzrelais) | overload relay |
| VFD / VSD | Frequenzumrichter (FU) | variable frequency / speed drive |
| UPS | unterbrechungsfreie Stromversorgung (USV) | uninterruptible power supply |
| ATS | automatischer Netzumschalter | automatic transfer switch |
| CT / VT | Strom-/Spannungswandler | current / voltage transformer |
| PLC | speicherprogrammierbare Steuerung (SPS) | programmable logic controller |
| HMI | Bedien- und Beobachtungsgerät | human-machine interface |
| SELV / PELV | Schutzkleinspannung / Funktionskleinspannung mit Erdung | safety / protective extra-low voltage |
| MV / LV | Mittelspannung / Niederspannung (MS/NS) | medium / low voltage |
| EMC | elektromagnetische Verträglichkeit (EMV) | electromagnetic compatibility |
| LOTO | Freischalten und Sichern (Lockout-Tagout) | lockout-tagout |

*Häufige Abkürzungen aus Datenblättern und Normen (IEC/IEV) mit deutscher Bezeichnung.*

### Netze & Stecker weltweit

#### Netzspannung und Stecker weltweit

| Land | Spannung in V | Frequenz in Hz | Stecker |
|---|---|---|---|
| Afghanistan | 220 | 50 | C, F |
| Ägypten | 220 | 50 | C, F |
| Argentinien | 220 | 50 | C, I |
| Australien | 230 | 50 | I |
| Belgien | 230 | 50 | C, E |
| Brasilien | 127 / 220 | 60 | C, N |
| Chile | 220 | 50 | C, L |
| China | 220 | 50 | A, C, I |
| Dänemark | 230 | 50 | C, E, F, K |
| Deutschland | 230 | 50 | C, F |
| Finnland | 230 | 50 | C, F |
| Frankreich | 230 | 50 | C, E |
| Griechenland | 230 | 50 | C, F |
| Großbritannien | 230 | 50 | G |
| Indien | 230 | 50 | C, D, M |
| Indonesien | 230 | 50 | C, F |
| Iran | 230 | 50 | C, F |
| Irland | 230 | 50 | G |
| Israel | 230 | 50 | C, H, M |
| Italien | 230 | 50 | C, F, L |
| Japan | 100 | 50 / 60 | A, B |
| Kanada | 120 | 60 | A, B |
| Kenia | 240 | 50 | G |
| Kroatien | 230 | 50 | C, F |
| Marokko | 220 | 50 | C, E |
| Mexiko | 127 | 60 | A, B |
| Neuseeland | 230 | 50 | I |
| Niederlande | 230 | 50 | C, F |
| Nigeria | 230 | 50 | D, G |
| Norwegen | 230 | 50 | C, F |
| Österreich | 230 | 50 | C, F |
| Pakistan | 230 | 50 | C, D |
| Philippinen | 220 | 60 | A, B, C |
| Polen | 230 | 50 | C, E |
| Portugal | 230 | 50 | C, F |
| Rumänien | 230 | 50 | C, F |
| Russland | 230 | 50 | C, F |
| Saudi-Arabien | 230 | 60 | G |
| Schweden | 230 | 50 | C, F |
| Schweiz | 230 | 50 | C, J |
| Singapur | 230 | 50 | G |
| Spanien | 230 | 50 | C, F |
| Südafrika | 230 | 50 | C, D, M, N |
| Südkorea | 220 | 60 | C, F |
| Taiwan | 110 | 60 | A, B |
| Thailand | 230 | 50 | A, B, C, O |
| Tschechien | 230 | 50 | C, E |
| Türkei | 230 | 50 | C, F |
| Ukraine | 230 | 50 | C, F |
| Ungarn | 230 | 50 | C, F |
| USA | 120 | 60 | A, B |
| Vereinigte Arabische Emirate | 230 | 50 | G |
| Vietnam | 220 | 50 | A, C |

*Haushalts-Nennspannung (Außenleiter–Neutralleiter) und Frequenz, übliche Steckertypen (A–O, siehe Tabelle Steckertypen). Nach IEC World Plugs; ohne Gewähr – vor Ort und am Gerät prüfen. Drehstrom meist √3 · U (z. B. 230/400 V, 120/208 V, 127/220 V).*

#### Steckertypen A–O

| Typ | Aufbau | Bemessung | Norm / Verbreitung |
|---|---|---|---|
| A | 2 Flachstifte | 15 A 125 V | NEMA 1-15; USA, Kanada, Japan |
| B | 2 Flachstifte + Rund-Erde | 15 A 125 V | NEMA 5-15; Nordamerika |
| C | 2 Rundstifte 4 mm (Eurostecker) | 2,5 A 250 V | CEE 7/16; Europa |
| D | 3 Rundstifte (Dreieck) | 5 A 250 V | BS 546; Indien |
| E | 2 Rundstifte, Erdstift in der Dose | 16 A 250 V | CEE 7/5; Frankreich, Belgien |
| F | 2 Rundstifte 4,8 mm, seitliche Erdbügel (Schuko) | 16 A 250 V | CEE 7/3, 7/4; Deutschland, Europa |
| G | 3 Rechteckstifte, Sicherung im Stecker | 13 A 250 V | BS 1363; UK, Irland |
| H | 3 Stifte (V-Form bzw. rund) | 16 A 250 V | SI 32; Israel |
| I | 2 schräge Flachstifte + Erde | 10 A 250 V | AS/NZS 3112; Australien, China |
| J | 3 Rundstifte (versetzt) | 10 A 250 V | SN 441011 (T12/T13); Schweiz |
| K | 2 Rundstifte + halbrunde Erde | 13 A 250 V | DS 60884-2-D1; Dänemark |
| L | 3 Rundstifte in einer Reihe | 10 / 16 A 250 V | CEI 23-50; Italien |
| M | 3 dicke Rundstifte (Dreieck) | 15 A 250 V | BS 546; Südafrika, Indien |
| N | 3 Rundstifte (kompakt) | 10 / 20 A 250 V | IEC 60906-1, NBR 14136; Brasilien |
| O | 3 Rundstifte | 16 A 250 V | TIS 166-2549; Thailand |

*Typbezeichnung nach IEC World Plugs (IEC TR 60083). Bemessungsströme typisch; Länderzuordnung siehe Tabelle „Netzspannung und Stecker weltweit“.*

#### Gerätesteckverbinder IEC 60320

| Kupplung / Stecker | Bemessung | Schutzleiter | Bezeichnung / Einsatz |
|---|---|---|---|
| C1 / C2 | 0,2 A 250 V | nein | Rasierer, Kleingeräte |
| C5 / C6 | 2,5 A 250 V | ja | „Mickey-Mouse“, Notebook-Netzteile |
| C7 / C8 | 2,5 A 250 V | nein | „Achter“, Radio, Ladegeräte |
| C13 / C14 | 10 A 250 V | ja | Kaltgerätestecker: PC, Monitor, IT |
| C15 / C16 | 10 A 250 V | ja | Warmgerät bis 120 °C (Wasserkocher) |
| C15A / C16A | 10 A 250 V | ja | Heißgerät bis 155 °C |
| C17 / C18 | 10 A 250 V | nein | wie C13 ohne Erde |
| C19 / C20 | 16 A 250 V | ja | Server, USV, Rechenzentrum |
| C21 / C22 | 16 A 250 V | ja | wie C19 bis 155 °C |
| C23 / C24 | 16 A 250 V | nein | wie C19 ohne Erde |

*IEC 60320-1: ungerade Nummer = Kupplung (Buchse am Kabel), gerade = Gerätestecker (am Gerät). Bemessungsströme für IEC-Länder; in Nordamerika teils höher (z. B. C13 15 A). Höchsttemperatur am Stift: 70 °C (kalt), 120 °C (warm), 155 °C (heiß).*

#### CEE-Steckvorrichtungen (IEC 60309)

| Farbe | Spannung | Typischer Einsatz |
|---|---|---|
| violett | 20–25 V | Kleinspannung |
| weiß | 40–50 V | Kleinspannung |
| gelb | 100–130 V | Baustellen (UK 110 V) |
| blau | 200–250 V | 1~ 230 V: Camping, Baustelle, Marina |
| rot | 380–480 V | 3~ 400 V: Maschinen, Baustromverteiler |
| schwarz | 500–690 V | Industrie, Bergbau |
| grün | > 60–500 Hz | Sonderfrequenzen |

*IEC 60309-2 (CEE): Farbe = Spannungsbereich, Lage der Schutzkontaktbuchse (Uhrzeit, Nase bei 6 h) codiert Spannung/Frequenz; Standard 50/60 Hz: 6 h (z. B. blau 230 V, rot 400 V). Bemessungsströme 16, 32, 63, 125 A; IP44 (spritzwassergeschützt) oder IP67 (wasserdicht). Polzahl 2P+E (3-polig), 3P+E (4-polig), 3P+N+E (5-polig).*

#### NEMA-Steckverbinder

| Typ | Spannung | Strom | Pole / Leiter | Einsatz |
|---|---|---|---|---|
| 1-15 | 125 V | 15 A | 2P 2W | ohne Erde (Altbestand) |
| 5-15 | 125 V | 15 A | 2P 3W | Standardsteckdose |
| 5-20 | 125 V | 20 A | 2P 3W | Küche, Werkstatt |
| 6-15 / 6-20 | 250 V | 15 / 20 A | 2P 3W | Klimageräte, Werkzeuge |
| 6-30 / 6-50 | 250 V | 30 / 50 A | 2P 3W | Schweißgeräte |
| 10-30 / 10-50 | 125/250 V | 30 / 50 A | 3P 3W | alte Trockner/Herde (ohne Erde) |
| 14-30 | 125/250 V | 30 A | 3P 4W | Wäschetrockner |
| 14-50 | 125/250 V | 50 A | 3P 4W | Herde, Wohnmobile, E-Auto-Laden |
| L5-20 / L5-30 | 125 V | 20 / 30 A | 2P 3W | verriegelbar, Industrie |
| L6-20 / L6-30 | 250 V | 20 / 30 A | 2P 3W | verriegelbar, USV, Server |
| L14-20 / L14-30 | 125/250 V | 20 / 30 A | 3P 4W | Generatoren |
| L15-20 / L15-30 | 3~ 250 V | 20 / 30 A | 3P 4W | Drehstrom |
| L21-20 / L21-30 | 3~ 120/208 V | 20 / 30 A | 4P 5W | Drehstrom mit N, Rechenzentren |

*NEMA WD 6: Nummer = Spannungs-/Polkonfiguration, Zahl nach dem Strich = Strom (z. B. 5-15: 125 V, 15 A); L = Verriegelung (Twist-Lock). P = Pole, W = Leiter (inkl. Schutzleiter).*

#### Ladestecker für Elektrofahrzeuge

| Stecker | Norm | Strom / Leistung | Region |
|---|---|---|---|
| Typ 1 | SAE J1772, IEC 62196-2 | AC 1~, bis 32 A (7,4 kW) | Nordamerika, Japan |
| Typ 2 (Mennekes) | IEC 62196-2 | AC 1~/3~, bis 63 A (43 kW), üblich 11/22 kW | Europa (EU-Standard) |
| CCS Combo 1 | IEC 62196-3 | DC bis ca. 350 kW | Nordamerika |
| CCS Combo 2 | IEC 62196-3 | DC bis ca. 400 kW (1000 V, 500 A) | Europa |
| CHAdeMO | IEC 62196-3 | DC bis 62,5 kW (v1), höher in v2 | Japan |
| GB/T | GB/T 20234 | AC und DC (getrennte Stecker) | China |
| NACS | SAE J3400 | AC und DC über einen Stecker | Nordamerika |
| Schuko / CEE (Mode 2) | IEC 61851-1 | 1~ 8–10 A bzw. CEE 16/32 A | Notladen, mobil |

*IEC 62196 (Stecker), IEC 61851-1 (Lademodi): Mode 1 Haushaltssteckdose ohne Kommunikation (in DE/CH nicht üblich), Mode 2 Haushalts-/CEE-Steckdose mit Kabel-Schutzeinrichtung (IC-CPD), Mode 3 Ladestation AC (Wallbox), Mode 4 DC-Schnellladung. Leistungen als Richtwerte; Fahrzeug und Ladestation begrenzen.*

## Motoren & Antriebe

### Motorabgang

#### Motorabgang: Motor, Pumpe, Ventilator

*Was der Abgang braucht: MSS/LS, Schütze, Sanftanlasser/FU, Leitung, Reparaturschalter, RCD* — `motor_feeder`

```text
Motorstrom: Richtwerte für 4-polige Käfigläufer (Eaton Schaltungsbuch 06/11, S. 10-43/44) oder Typenschild.
Anlaufstrom: direkt ≤ 6 · I_N (t_A ≤ 5 s), Stern-Dreieck ≤ 2 · I_N (t_A ≤ 15 s), Sanftanlauf ca. 2–6 · I_N, FU ≈ 1 (…2) · I_N.
Überlastschutz I_r = I_N; Stern-Dreieck: Relais in der Motorleitung 0,58 · I_N (t_A ≤ 15 s), in der Netzzuleitung I_N (15–40 s), darüber in der Dreieckverbindung (im Anlauf kein Schutz). Schütze Y-Δ: Netz- und Dreieckschütz 0,58 · I_N, Sternschütz 0,33 · I_N (Eaton S. 8-37).
Auslöseklasse IEC 60947-4-1 bei 7,2 · I_r: CLASS 10 > 4 s, 20 > 6 s, 30 > 9 s – die Hochlaufzeit muss darunter liegen.
Motorschutzschalter: Einstellbereiche Eaton PKZM0 (0,1–32 A, kurzschlussfest bis 50 kA bei 400 V) und PKZM4 (bis 65 A).
Leitung Cu/PVC mit I_z ≥ I_r (DIN VDE 0298-4, Temperatur- und Häufungsfaktor); bei FU/EC für den Leitungsschutz I_n.
RCD bei Umrichtern mit B6-Gleichrichter: Typ B (allstromsensitiv), da glatte Gleichfehlerströme möglich sind (EN 50178, DIN VDE 0100-530). Herstellerangaben (Koordinationstabellen Typ 1/2) haben Vorrang.
```

**Eingaben:** Anwendung, Anlaufart, Drehrichtung, Schutzkonzept, Motorleistung P_N [kW], Netz, Motordaten, Bemessungsstrom I_N (Typenschild) [A], Leistungsfaktor cos φ (Typenschild), Hochlaufzeit t_A [s], Leitungslänge (einfach) [m], Verlegeart, Gehäufte Stromkreise, Umgebungstemperatur [°C]

**Ergebnisse:** Motorbemessungsstrom I_N [A], Anlaufstrom (ca.) [A], Einstellwert Überlastschutz I_r [A], Kurzschluss- und Überlastschutz, Kleinste Vorsicherung gL/gG (Richtwert) [A], Leitungsschutzschalter I_n [A], Schütze (AC-3), Anlaufgerät / Umrichter, Leiterquerschnitt Cu [mm²], Belastbarkeit I_z [A], Spannungsfall bei I_N [%], Leitung, Reparaturschalter, Fehlerstromschutz (RCD), Auslöseklasse / Anordnung, Zubehör und Hinweise

#### Motorbemessungsströme und Absicherung

| P in kW | cos φ | η in % | I_N 400 V | Sicherung direkt 400 V | Sicherung Y-Δ 400 V | I_N 230 V | Sicherung direkt 230 V | Sicherung Y-Δ 230 V |
|---|---|---|---|---|---|---|---|---|
| 0,06 | 0,7 | 58 | 0,21 | 2 | – | 0,37 | 2 | – |
| 0,09 | 0,7 | 60 | 0,31 | 2 | – | 0,54 | 2 | – |
| 0,12 | 0,7 | 60 | 0,41 | 2 | – | 0,72 | 4 | 2 |
| 0,18 | 0,7 | 62 | 0,6 | 2 | – | 1,04 | 4 | 2 |
| 0,25 | 0,7 | 62 | 0,8 | 4 | 2 | 1,4 | 4 | 2 |
| 0,37 | 0,72 | 66 | 1,1 | 4 | 2 | 2 | 6 | 4 |
| 0,55 | 0,75 | 69 | 1,5 | 4 | 2 | 2,7 | 10 | 4 |
| 0,75 | 0,79 | 74 | 1,9 | 6 | 4 | 3,2 | 10 | 4 |
| 1,1 | 0,81 | 74 | 2,6 | 6 | 4 | 4,6 | 10 | 6 |
| 1,5 | 0,81 | 74 | 3,6 | 6 | 4 | 6,3 | 16 | 10 |
| 2,2 | 0,81 | 78 | 5 | 10 | 6 | 8,7 | 20 | 10 |
| 3 | 0,82 | 80 | 6,6 | 16 | 10 | 11,5 | 25 | 16 |
| 4 | 0,82 | 83 | 8,5 | 20 | 10 | 14,8 | 32 | 16 |
| 5,5 | 0,82 | 86 | 11,3 | 25 | 16 | 19,6 | 32 | 25 |
| 7,5 | 0,82 | 87 | 15,2 | 32 | 16 | 26,4 | 50 | 32 |
| 11 | 0,84 | 87 | 21,7 | 40 | 25 | 38 | 80 | 40 |
| 15 | 0,84 | 88 | 29,3 | 63 | 32 | 51 | 100 | 63 |
| 18,5 | 0,84 | 88 | 36 | 63 | 40 | 63 | 125 | 80 |
| 22 | 0,84 | 92 | 41 | 80 | 50 | 71 | 125 | 80 |
| 30 | 0,85 | 92 | 55 | 100 | 63 | 96 | 200 | 100 |
| 37 | 0,86 | 92 | 68 | 125 | 80 | 117 | 200 | 125 |
| 45 | 0,86 | 93 | 81 | 160 | 100 | 141 | 250 | 160 |
| 55 | 0,86 | 93 | 99 | 200 | 125 | 173 | 250 | 200 |
| 75 | 0,86 | 94 | 134 | 200 | 160 | 233 | 315 | 250 |
| 90 | 0,86 | 94 | 161 | 250 | 200 | 279 | 400 | 315 |
| 110 | 0,86 | 94 | 196 | 315 | 200 | 342 | 500 | 400 |
| 132 | 0,87 | 95 | 231 | 400 | 250 | 401 | 630 | 500 |
| 160 | 0,87 | 95 | 279 | 400 | 315 | 486 | 630 | 630 |
| 200 | 0,87 | 95 | 349 | 500 | 400 | 607 | 800 | 630 |
| 250 | 0,87 | 95 | 437 | 630 | 500 | – | – | – |
| 315 | 0,87 | 96 | 544 | 800 | 630 | – | – | – |
| 400 | 0,88 | 96 | 683 | 1000 | 800 | – | – | – |
| 450 | 0,88 | 96 | 769 | 1000 | 800 | – | – | – |

*Eaton Schaltungsbuch 06/11, S. 10-43/10-44: Richtwerte für innen- und oberflächengekühlte Drehstrommotoren mit 1500 min⁻¹. Kleinstmögliche Kurzschlusssicherung („träge“ bzw. gL, DIN VDE 0636) für Direktanlauf (I_A ≤ 6 · I_N, t_A ≤ 5 s) und Stern-Dreieck (I_A ≤ 2 · I_N, t_A ≤ 15 s); der Höchstwert richtet sich nach Schaltgerät bzw. Motorschutzrelais. Bei höherem Anlaufstrom oder längerer Anlaufzeit größere Sicherung; aM-Sicherung = Bemessungsstrom.*

### Motordaten

#### Wirkungsgrad aus Messung

*η aus U, I, cos φ und Abgabeleistung* — `motor_efficiency`

```text
P_el = k · U · I · cos φ    η = P_mech / P_el    P_V = P_el − P_mech
```

**Eingaben:** Netz, Spannung U [V], Strom I [A], Leistungsfaktor cos φ, Abgegebene Leistung P_mech [kW]

**Ergebnisse:** Aufgenommene Leistung P_el [kW], Wirkungsgrad η [%], Verluste P_V [kW]

#### Drehmoment · Leistung · Drehzahl

*M = 9549 · P / n – zwei Werte eingeben* — `torque`

```text
M [Nm] = 9549,3 · P [kW] / n [1/min]    (P = 2π · n · M / 60)
Genau zwei Werte eingeben.
```

**Eingaben:** Leistung P [kW], Drehzahl n [1/min], Drehmoment M [Nm]

**Ergebnisse:** Leistung P [kW], Drehzahl n [1/min], Drehmoment M [Nm]

#### Drehzahl & Schlupf

*Synchrondrehzahl, Schlupf, Läuferfrequenz* — `motor_speed`

```text
n_s = 120 · f / Polzahl = 60 · f / p    s = (n_s − n) / n_s    f_2 = s · f
```

**Eingaben:** Frequenz f [Hz], Polzahl, Bemessungsdrehzahl n [1/min]

**Ergebnisse:** Synchrondrehzahl n_s [1/min], Schlupf s [%], Läuferfrequenz f_2 [Hz]

#### Pumpe/Ventilator am FU: Proportionalitätsgesetze

*V̇ ~ n, Δp ~ n², P ~ n³ – Leistung und Einsparung bei Drehzahlregelung* — `fan_affinity`

```text
V̇₂/V̇₁ = n₂/n₁ · Δp₂/Δp₁ = (n₂/n₁)² · P₂/P₁ = (n₂/n₁)³
Gilt für quadratische Anlagenkennlinien mit gleichbleibender Dichte und Wirkungsgrad (Ventilatoren, geschlossene Kreisläufe). Mit statischem Anteil (Förderhöhe, Vordruck) nicht anwendbar.
Pumpen und Ventilatoren arbeiten meist bei 50 … 90 % Drehzahl.
Quellen: Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022), S. 180 (Beispiel 4000 → 8000 m³/h, 350 → 1400 Pa); Danfoss „Wissenswertes über Frequenzumrichter“, Kap. 3.
```

**Eingaben:** Volumenstrom V̇₁ [m³/h], Druckerhöhung Δp₁ [Pa], Leistungsaufnahme P₁ [kW], Drehzahl n₁ [1/min], Neuer Betriebspunkt aus, Drehzahl n₂ [1/min], Volumenstrom V̇₂ [m³/h], Betriebsstunden im neuen Punkt [h/a], Strompreis [€ / CHF]

**Ergebnisse:** Drehzahlverhältnis n₂/n₁ [%], Drehzahl n₂ [1/min], Volumenstrom V̇₂ [m³/h], Druckerhöhung Δp₂ [Pa], Leistungsaufnahme P₂ [kW], Energieeinsparung [kWh/a], Kosteneinsparung [€]

#### Motor-Volllaststrom (NEC)

*Tabellenstrom nach NEC 430.248 / 430.250 (Nordamerika)* — `nec_flc`

```text
NEC (NFPA 70) Tab. 430.248 (Einphasen-Wechselstrommotoren) und Tab. 430.250 (Drehstrom-Asynchronmotoren). Nach NEC 430.6(A)(1) sind für Leiter, Kurzschluss- und Erdschlussschutz die Tabellenwerte (nicht die Typenschildwerte) zu verwenden; Leiter für einen Motor ≥ 125 % des Tabellenstroms (430.22). Der Motorschutz (Überlast) richtet sich nach dem Typenschildstrom. 208/200 V = Werte für 208/200-V-Motoren. Gegen die gültige NEC-Ausgabe prüfen.
```

**Eingaben:** Phasen, Motorleistung, Motorleistung, Spannung, Spannung

**Ergebnisse:** Volllaststrom (Tabelle) [A], Leiter-Bemessungsstrom 125 % [A], Leistung [kW]

#### NEC 430.250: Volllaststrom Drehstrommotoren

| hp | kW | 115 V | 200 V | 208 V | 230 V | 460 V | 575 V | 2300 V |
|---|---|---|---|---|---|---|---|---|
| 1/2 | 0,37 | 4,4 | 2,5 | 2,4 | 2,2 | 1,1 | 0,9 | – |
| 3/4 | 0,56 | 6,4 | 3,7 | 3,5 | 3,2 | 1,6 | 1,3 | – |
| 1 | 0,75 | 8,4 | 4,8 | 4,6 | 4,2 | 2,1 | 1,7 | – |
| 1 1/2 | 1,12 | 12 | 6,9 | 6,6 | 6 | 3 | 2,4 | – |
| 2 | 1,49 | 13,6 | 7,8 | 7,5 | 6,8 | 3,4 | 2,7 | – |
| 3 | 2,24 | – | 11 | 10,6 | 9,6 | 4,8 | 3,9 | – |
| 5 | 3,73 | – | 17,5 | 16,7 | 15,2 | 7,6 | 6,1 | – |
| 7 1/2 | 5,59 | – | 25,3 | 24,2 | 22 | 11 | 9 | – |
| 10 | 7,46 | – | 32,2 | 30,8 | 28 | 14 | 11 | – |
| 15 | 11,19 | – | 48,3 | 46,2 | 42 | 21 | 17 | – |
| 20 | 14,91 | – | 62,1 | 59,4 | 54 | 27 | 22 | – |
| 25 | 18,64 | – | 78,2 | 74,8 | 68 | 34 | 27 | – |
| 30 | 22,37 | – | 92 | 88 | 80 | 40 | 32 | – |
| 40 | 29,83 | – | 120 | 114 | 104 | 52 | 41 | – |
| 50 | 37,28 | – | 150 | 143 | 130 | 65 | 52 | – |
| 60 | 44,74 | – | 177 | 169 | 154 | 77 | 62 | 16 |
| 75 | 55,93 | – | 221 | 211 | 192 | 96 | 77 | 20 |
| 100 | 74,57 | – | 285 | 273 | 248 | 124 | 99 | 26 |
| 125 | 93,21 | – | 359 | 343 | 312 | 156 | 125 | 31 |
| 150 | 111,85 | – | 414 | 396 | 360 | 180 | 144 | 37 |
| 200 | 149,14 | – | 552 | 528 | 480 | 240 | 192 | 49 |
| 250 | 186,42 | – | – | – | – | 302 | 242 | 60 |
| 300 | 223,71 | – | – | – | – | 361 | 289 | 72 |
| 350 | 260,99 | – | – | – | – | 414 | 336 | 83 |
| 400 | 298,28 | – | – | – | – | 477 | 382 | 95 |
| 450 | 335,56 | – | – | – | – | 515 | 412 | 103 |
| 500 | 372,85 | – | – | – | – | 590 | 472 | 118 |

*NEC (NFPA 70) Tab. 430.250 (Drehstrom-Asynchronmotoren) bzw. 430.248 (Einphasen-Wechselstrommotoren); für Leiter und Schutz nach 430.6(A)(1). Gegen die gültige NEC-Ausgabe prüfen.*

#### NEC 430.248: Volllaststrom Einphasenmotoren

| hp | kW | 115 V | 200 V | 208 V | 230 V |
|---|---|---|---|---|---|
| 1/6 | 0,12 | 4,4 | 2,5 | 2,4 | 2,2 |
| 1/4 | 0,19 | 5,8 | 3,3 | 3,2 | 2,9 |
| 1/3 | 0,25 | 7,2 | 4,1 | 4 | 3,6 |
| 1/2 | 0,37 | 9,8 | 5,6 | 5,4 | 4,9 |
| 3/4 | 0,56 | 13,8 | 7,9 | 7,6 | 6,9 |
| 1 | 0,75 | 16 | 9,2 | 8,8 | 8 |
| 1 1/2 | 1,12 | 20 | 11,5 | 11 | 10 |
| 2 | 1,49 | 24 | 13,8 | 13,2 | 12 |
| 3 | 2,24 | 34 | 19,6 | 18,7 | 17 |
| 5 | 3,73 | 56 | 32,2 | 30,8 | 28 |
| 7 1/2 | 5,59 | 80 | 46 | 44 | 40 |
| 10 | 7,46 | 100 | 57,5 | 55 | 50 |

*NEC (NFPA 70) Tab. 430.250 (Drehstrom-Asynchronmotoren) bzw. 430.248 (Einphasen-Wechselstrommotoren); für Leiter und Schutz nach 430.6(A)(1). Gegen die gültige NEC-Ausgabe prüfen.*

### Anschluss & Anlauf

#### Stern-Dreieck-Anlauf

*Anlaufstrom, Anlaufmoment, Motorschutz-Einstellung* — `star_delta`

```text
Im Stern: I_A,Y = I_A,Δ / 3    M_A,Y = M_A,Δ / 3
Motorschutzrelais im Motorstrang (nach dem Netzschütz): Einstellung I_N / √3 ≈ 0,58 · I_N.
Voraussetzung: Motor für Dreieck an Netzspannung (z. B. 400 V Δ / 690 V Y).
```

**Eingaben:** Bemessungsstrom I_N [A], Anlaufstromverhältnis I_A/I_N, Anzugsmomentverhältnis M_A/M_N, Lastmoment beim Anlauf M_L/M_N

**Ergebnisse:** Anlaufstrom direkt (Δ) [A], Anlaufstrom Stern (Y) [A], Anzugsmoment Stern M_A,Y / M_N, Einstellung Motorschutzrelais [A]

#### Drehstrommotor am 1~-Netz (Steinmetz)

*Betriebs- und Anlaufkondensator* — `steinmetz`

```text
Faustformel: C_B ≈ 70 µF pro kW bei 230 V / 50 Hz (skaliert mit (230/U)² · 50/f)
C_A ≈ 2…3 · C_B (nur während des Anlaufs). Leistung ca. 70…80 % von P_N.
Motor in Dreieck für 230 V schalten (230/400-V-Motor). Kondensator-Nennspannung ≥ 1,25 · U.
```

**Eingaben:** Motorleistung P_N [kW], Netzspannung 1~ [V], Frequenz f [Hz]

**Ergebnisse:** Betriebskondensator C_B [µF], Anlaufkondensator C_A, Verfügbare Leistung ca. [kW]

#### Schaltschema Drehstrommotor (6 Klemmen)

*Stern- oder Dreieckschaltung am Klemmbrett (U1…W2)* — `motor_wiring_6`

```text
Klemmenbezeichnung nach IEC 60034-8 (alt DIN VDE 0530: U, V, W / X, Y, Z). Oben W2 U2 V2, unten U1 V1 W1.
Stern (Y): Brücke W2–U2–V2, Netz an U1/V1/W1; jede Wicklung liegt an U/√3.
Dreieck (Δ): Brücken W2–U1, U2–V1, V2–W1; jede Wicklung liegt an U.
Typenschild 230/400 V am 400-V-Netz → Stern; 400/690 V am 400-V-Netz → Dreieck (auch für Stern-Dreieck-Anlauf).
L1→U1, L2→V1, L3→W1 = Rechtslauf (Blick auf das Wellenende).
```

**Eingaben:** Netzspannung (Außenleiter) [V], Typenschild, Darstellung

**Ergebnisse:** Schaltung, Brücken, Netzanschluss, Spannung je Wicklung [V]

#### Schaltschema Drehstrommotor (9 Klemmen)

*NEMA T1–T9, Stern oder Dreieck, hohe/niedrige Spannung* — `motor_wiring_9`

```text
Anschlussbezeichnung nach NEMA MG 1 (Nordamerika), Doppelspannungsmotoren z. B. 230/460 V: Hohe Spannung = Wicklungshälften in Reihe, niedrige Spannung = parallel.
9 Klemmen: Stern (innerer Sternpunkt T7-T8-T9 intern) oder Dreieck (innere Dreieckecken T1, T2, T3).
12 Klemmen: alle Wicklungsenden herausgeführt; Stern oder Dreieck frei wählbar (auch Stern-Dreieck-Anlauf, Teilwicklungsanlauf). Typenschild und Anschlussplan des Herstellers haben Vorrang.
```

**Eingaben:** Schaltung, Spannung

**Ergebnisse:** L1, L2, L3, Zusammen verbinden (isoliert)

#### Schaltschema Drehstrommotor (12 Klemmen)

*NEMA T1–T12, Stern/Dreieck, Reihe/parallel* — `motor_wiring_12`

```text
Anschlussbezeichnung nach NEMA MG 1 (Nordamerika), Doppelspannungsmotoren z. B. 230/460 V: Hohe Spannung = Wicklungshälften in Reihe, niedrige Spannung = parallel.
9 Klemmen: Stern (innerer Sternpunkt T7-T8-T9 intern) oder Dreieck (innere Dreieckecken T1, T2, T3).
12 Klemmen: alle Wicklungsenden herausgeführt; Stern oder Dreieck frei wählbar (auch Stern-Dreieck-Anlauf, Teilwicklungsanlauf). Typenschild und Anschlussplan des Herstellers haben Vorrang.
```

**Eingaben:** Schaltung, Spannung

**Ergebnisse:** L1, L2, L3, Zusammen verbinden (isoliert)

#### Motor-Verbindungen (Drehrichtung)

*Anschluss und Drehrichtungsumkehr von Drehstrom- und Kondensatormotoren* — `motor_connections`

```text
Drehstrom (IEC 60034-8): L1→U1, L2→V1, L3→W1 ergibt Rechtslauf (Blick auf das Wellenende, Rechtsdrehfeld). Linkslauf: zwei Außenleiter tauschen (z. B. L1↔L2).
Einphasen-Kondensatormotor: Hauptwicklung U1–U2 an L–N, Hilfswicklung Z1–Z2 über den Betriebskondensator C_B parallel. Drehrichtung umkehren: Hilfswicklung umpolen (Z1↔Z2) – die tatsächliche Richtung zeigt das Anschlussbild des Herstellers.
```

**Eingaben:** Motor, Drehrichtung

**Ergebnisse:** Anschluss, Umkehr

#### Kennzeichnung der Motorklemmen

| Maschine / Wicklung | IEC 60034-8 | Alt (DIN VDE 0530) | NEMA MG 1 |
|---|---|---|---|
| Drehstrom, 6 Klemmen | U1 V1 W1 / U2 V2 W2 | U V W / X Y Z | T1 T2 T3 / T4 T5 T6 |
| Drehstrom, 9 Klemmen | – | – | T1 … T9 |
| Drehstrom, 12 Klemmen | U1 U2 U5 U6, V…, W… | – | T1 … T12 |
| Dahlander (2 Drehzahlen) | 1U 1V 1W / 2U 2V 2W | – | – |
| Zwei getrennte Wicklungen | 1U1 … 1W2 / 2U1 … 2W2 | – | – |
| Einphasen: Haupt-/Hilfswicklung | U1–U2 / Z1–Z2 | – | T1–T4 / T5, T8 |
| Gleichstrom: Anker | A1–A2 | A–B | A1–A2 |
| Gleichstrom: Nebenschluss | E1–E2 | C–D | F1–F2 |
| Gleichstrom: Reihenschluss | D1–D2 | E–F | S1–S2 |
| Gleichstrom: Wendepol | B1–B2 | G–H | – |
| Gleichstrom: Fremderregung | F1–F2 | J–K | F1–F2 |

*IEC 60034-8 (DIN EN 60034-8): Ziffer 1 = Wicklungsanfang, 2 = Wicklungsende; vorangestellte Ziffer = Drehzahlstufe bzw. Wicklung. Alte Bezeichnung nach DIN VDE 0530 (bis 1977). NEMA MG 1: T-Nummern. Typenschild hat Vorrang.*

### Klassen & Bauformen

#### Motor-Effizienzklassen IE

| Klasse | Bezeichnung |
|---|---|
| IE1 | Standard-Wirkungsgrad |
| IE2 | Hoher Wirkungsgrad |
| IE3 | Premium-Wirkungsgrad |
| IE4 | Super-Premium-Wirkungsgrad |
| IE5 | Ultra-Premium-Wirkungsgrad |

*IEC 60034-30-1; EU-Verordnung 2019/1781 (Stand: IE3 ab 0,75–1000 kW seit 07/2021, IE4 für 75–200 kW seit 07/2023). In der Schweiz gelten vergleichbare Anforderungen (EnEV).*

#### Isolierstoffklassen

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

#### Motor-Betriebsarten S1–S10

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

#### Motor-Bauformen (IM)

| Bauform | Beschreibung |
|---|---|
| IM B3 | Fußausführung, Welle waagerecht |
| IM B5 | Flansch (FF, Durchgangslöcher), waagerecht |
| IM B14 | Normflansch (FT, Gewindelöcher), waagerecht |
| IM B35 | Fuß + Flansch FF, waagerecht |
| IM V1 | Flansch FF, Welle nach unten |
| IM V3 | Flansch FF, Welle nach oben |

*Häufige Bauformen nach IEC 60034-7.*

## Elektronik & Bauteile

### Bauteile & Codes

#### Farbcode Widerstand / Spule

*Farbringe → Wert und Wert → Farbringe (4 und 5 Ringe, IEC 60062) mit Zeichnung* — `resistor_color`

```text
4 Ringe: Ziffer, Ziffer, Multiplikator, Toleranz (E6–E24)
5 Ringe: Ziffer, Ziffer, Ziffer, Multiplikator, Toleranz (E48–E192)
Schwarz 0 ×1 · Braun 1 ×10 ±1 % · Rot 2 ×100 ±2 % · Orange 3 ×1 k · Gelb 4 ×10 k · Grün 5 ×100 k ±0,5 % · Blau 6 ×1 M ±0,25 % · Violett 7 ×10 M ±0,1 % · Grau 8 ×100 M ±0,05 % · Weiß 9 ×1 G · Gold ×0,1 ±5 % · Silber ×0,01 ±10 % · kein Ring ±20 %
Wert → Farbringe: auf 2 bzw. 3 gültige Ziffern gerundet und gegen E24 bzw. E96 geprüft. Induktivitäten: gleiche Codierung, Grundeinheit µH.
```

**Eingaben:** Richtung, Bauteil, Widerstandswert, Einheit, Anzahl Ringe, 1. Ring (Ziffer), 2. Ring (Ziffer), 3. Ring (Ziffer), Multiplikator, Toleranz

**Ergebnisse:** Nennwert [Ω], Farbringe, Toleranz [%], Minimalwert [Ω], Maximalwert [Ω], Nächster Normwert [Ω], Normreihe

#### E-Reihe: Normwert finden

*Nächster Wert in E6 … E96 (IEC 60063)* — `e_series`

```text
Sucht den nächstgelegenen Normwert (logarithmischer Abstand) sowie den nächstkleineren und -größeren Wert.
```

**Eingaben:** Wert [Ω], Reihe

**Ergebnisse:** Nächster Normwert [Ω], Abweichung [%], Nächstkleiner [Ω], Nächstgrößer [Ω]

#### Normwerte von Widerständen (E-Reihen)

| E24 | E12 | E6 | E3 |
|---|---|---|---|
| 1 | ✓ | ✓ | ✓ |
| 1,1 | – | – | – |
| 1,2 | ✓ | – | – |
| 1,3 | – | – | – |
| 1,5 | ✓ | ✓ | – |
| 1,6 | – | – | – |
| 1,8 | ✓ | – | – |
| 2 | – | – | – |
| 2,2 | ✓ | ✓ | ✓ |
| 2,4 | – | – | – |
| 2,7 | ✓ | – | – |
| 3 | – | – | – |
| 3,3 | ✓ | ✓ | – |
| 3,6 | – | – | – |
| 3,9 | ✓ | – | – |
| 4,3 | – | – | – |
| 4,7 | ✓ | ✓ | ✓ |
| 5,1 | – | – | – |
| 5,6 | ✓ | – | – |
| 6,2 | – | – | – |
| 6,8 | ✓ | ✓ | – |
| 7,5 | – | – | – |
| 8,2 | ✓ | – | – |
| 9,1 | – | – | – |

*IEC 60063: Werte je Dekade (× 1, 10, 100 … Ω). Toleranz: E3 > ±20 %, E6 ±20 %, E12 ±10 %, E24 ±5 %; E48 ±2 %, E96 ±1 %, E192 ≤ ±0,5 % – E96 siehe Tabelle EIA-96-Code. Rechner „E-Reihe“ findet den nächsten Normwert.*

#### SMD-Widerstandscode

*3- und 4-stellig, R-Schreibweise und EIA-96* — `smd_resistor`

```text
3-stellig (E24): zwei Ziffern + Zehnerpotenz, z. B. 472 = 47 · 10² = 4,7 kΩ
4-stellig (E96, 1 %): drei Ziffern + Zehnerpotenz, z. B. 4702 = 47 kΩ
R = Dezimalkomma: 4R7 = 4,7 Ω, R47 = 0,47 Ω; 0 / 000 = 0-Ω-Brücke
EIA-96 (1 %): zwei Ziffern = Index der E96-Reihe (01 = 100 … 96 = 976) + Buchstabe: Z ×0,001, Y/R ×0,01, X/S ×0,1, A ×1, B/H ×10, C ×100, D ×1000, E ×10⁴, F ×10⁵. Beispiel 01C = 10 kΩ.
```

**Eingaben:** Aufdruck

**Ergebnisse:** Widerstand [Ω], Codierung

#### EIA-96-Code (SMD 1 %)

| Code | Wert | Code | Wert | Code | Wert | Code | Wert |
|---|---|---|---|---|---|---|---|
| 01 | 100 | 25 | 178 | 49 | 316 | 73 | 562 |
| 02 | 102 | 26 | 182 | 50 | 324 | 74 | 576 |
| 03 | 105 | 27 | 187 | 51 | 332 | 75 | 590 |
| 04 | 107 | 28 | 191 | 52 | 340 | 76 | 604 |
| 05 | 110 | 29 | 196 | 53 | 348 | 77 | 619 |
| 06 | 113 | 30 | 200 | 54 | 357 | 78 | 634 |
| 07 | 115 | 31 | 205 | 55 | 365 | 79 | 649 |
| 08 | 118 | 32 | 210 | 56 | 374 | 80 | 665 |
| 09 | 121 | 33 | 215 | 57 | 383 | 81 | 681 |
| 10 | 124 | 34 | 221 | 58 | 392 | 82 | 698 |
| 11 | 127 | 35 | 226 | 59 | 402 | 83 | 715 |
| 12 | 130 | 36 | 232 | 60 | 412 | 84 | 732 |
| 13 | 133 | 37 | 237 | 61 | 422 | 85 | 750 |
| 14 | 137 | 38 | 243 | 62 | 432 | 86 | 768 |
| 15 | 140 | 39 | 249 | 63 | 442 | 87 | 787 |
| 16 | 143 | 40 | 255 | 64 | 453 | 88 | 806 |
| 17 | 147 | 41 | 261 | 65 | 464 | 89 | 825 |
| 18 | 150 | 42 | 267 | 66 | 475 | 90 | 845 |
| 19 | 154 | 43 | 274 | 67 | 487 | 91 | 866 |
| 20 | 158 | 44 | 280 | 68 | 499 | 92 | 887 |
| 21 | 162 | 45 | 287 | 69 | 511 | 93 | 909 |
| 22 | 165 | 46 | 294 | 70 | 523 | 94 | 931 |
| 23 | 169 | 47 | 301 | 71 | 536 | 95 | 953 |
| 24 | 174 | 48 | 309 | 72 | 549 | 96 | 976 |

*Zwei Ziffern = Index der E96-Reihe, danach Multiplikator-Buchstabe: Z ×0,001, Y/R ×0,01, X/S ×0,1, A ×1, B/H ×10, C ×100, D ×1000, E ×10⁴, F ×10⁵. Beispiel 01C = 100 · 100 = 10 kΩ, 68X = 499 · 0,1 = 49,9 Ω.*

#### Kondensator-Code

*Aufdruck → Kapazität, Toleranz, Spannung* — `capacitor_code`

```text
3 Ziffern (pF): zwei Ziffern + Zehnerpotenz, 104 = 10 · 10⁴ pF = 100 nF; 8 = ×0,01, 9 = ×0,1.
Buchstabe als Komma: 4n7 = 4,7 nF, 47p = 47 pF, 2u2 = 2,2 µF, 1R5 = 1,5 pF; Dezimalzahl (0.1, .047) = µF.
Toleranz: B ±0,1 pF, C ±0,25 pF, D ±0,5 pF, F ±1 %, G ±2 %, J ±5 %, K ±10 %, M ±20 %, Z +80/−20 %.
Spannung (EIA, vorangestellt): z. B. 1H = 50 V, 2A = 100 V, 2J = 630 V.
```

**Eingaben:** Aufdruck

**Ergebnisse:** Kapazität [nF], Kapazität in pF [pF], Toleranz, Nennspannung [V]

#### Kondensatorcode-Tabelle

| Code | pF | nF | µF |
|---|---|---|---|
| 100 | 10 | 0,01 | 1e-05 |
| 150 | 15 | 0,015 | 1,5e-05 |
| 220 | 22 | 0,022 | 2,2e-05 |
| 330 | 33 | 0,033 | 3,3e-05 |
| 470 | 47 | 0,047 | 4,7e-05 |
| 680 | 68 | 0,068 | 6,8e-05 |
| 101 | 100 | 0,1 | 0,0001 |
| 151 | 150 | 0,15 | 0,00015 |
| 221 | 220 | 0,22 | 0,00022 |
| 331 | 330 | 0,33 | 0,00033 |
| 471 | 470 | 0,47 | 0,00047 |
| 681 | 680 | 0,68 | 0,00068 |
| 102 | 1000 | 1 | 0,001 |
| 152 | 1500 | 1,5 | 0,0015 |
| 222 | 2200 | 2,2 | 0,0022 |
| 332 | 3300 | 3,3 | 0,0033 |
| 472 | 4700 | 4,7 | 0,0047 |
| 682 | 6800 | 6,8 | 0,0068 |
| 103 | 10000 | 10 | 0,01 |
| 153 | 15000 | 15 | 0,015 |
| 223 | 22000 | 22 | 0,022 |
| 333 | 33000 | 33 | 0,033 |
| 473 | 47000 | 47 | 0,047 |
| 683 | 68000 | 68 | 0,068 |
| 104 | 100000 | 100 | 0,1 |
| 154 | 150000 | 150 | 0,15 |
| 224 | 220000 | 220 | 0,22 |
| 334 | 330000 | 330 | 0,33 |
| 474 | 470000 | 470 | 0,47 |
| 684 | 680000 | 680 | 0,68 |
| 105 | 1000000 | 1000 | 1 |
| 155 | 1500000 | 1500 | 1,5 |
| 225 | 2200000 | 2200 | 2,2 |
| 335 | 3300000 | 3300 | 3,3 |
| 475 | 4700000 | 4700 | 4,7 |
| 685 | 6800000 | 6800 | 6,8 |

*Zwei Ziffern + Anzahl Nullen in pF (EIA/IEC 60062). Toleranzbuchstabe danach: J ±5 %, K ±10 %, M ±20 %. Andere Werte siehe Rechner Kondensator-Code.*

#### Kondensator-Spannungscodes (EIA)

| Code | Nennspannung in V |
|---|---|
| 0G | 4 |
| 0L | 5,5 |
| 0J | 6,3 |
| 1A | 10 |
| 1C | 16 |
| 1D | 20 |
| 1E | 25 |
| 1V | 35 |
| 1H | 50 |
| 1J | 63 |
| 1K | 80 |
| 2A | 100 |
| 2B | 125 |
| 2C | 160 |
| 2D | 200 |
| 2E | 250 |
| 2F | 315 |
| 2V | 350 |
| 2G | 400 |
| 2W | 450 |
| 2J | 630 |
| 3A | 1000 |

*EIA-Spannungscode auf Folien-, Keramik- und Elektrolytkondensatoren (z. B. 2A104J = 100 V, 100 nF, ±5 %).*

#### Alphanumerischer Induktivitätscode

*101 = 100 µH, 4R7 = 4,7 µH, 47N = 47 nH* — `inductor_code`

```text
Grundeinheit µH: zwei Ziffern + Zehnerpotenz (101 = 10 · 10¹ µH = 100 µH); R = Komma in µH (4R7 = 4,7 µH, R10 = 0,1 µH); N = Komma in nH (47N = 47 nH, 4N7 = 4,7 nH). Nachgestellt Toleranz J ±5 %, K ±10 %, M ±20 %.
```

**Eingaben:** Aufdruck

**Ergebnisse:** Induktivität [µH], Induktivität in nH [nH]

### Schaltungen

#### Spannungsteiler

*U_aus unbelastet und belastet* — `voltage_divider`

```text
U_aus = U_ein · R2 / (R1 + R2)
Belastet: R2 ∥ R_L statt R2.
```

**Eingaben:** Eingangsspannung U_ein [V], R1 [kΩ], R2 [kΩ], Lastwiderstand R_L [kΩ]

**Ergebnisse:** U_aus unbelastet [V], U_aus belastet [V], Querstrom [mA], Teilerverhältnis, Verlustleistung R1 [mW]

#### Vorwiderstand (LED und Verbraucher)

*R = (U − n · U_F) / I_F bzw. (U − U_L) / I_L mit Normwert und Leistung* — `series_resistor`

```text
LED: R = (U − n · U_F) / I_F, gewählt wird der nächstgrößere E24-Wert (Strom bleibt ≤ I_F). Typ. U_F: rot 1,8–2,1 V, grün 2,0–3,2 V, blau/weiß 2,8–3,4 V.
Verbraucher: R_v = (U − U_L) / I_L, nächster E12-Wert; U_L mit diesem Wert bei ohmscher Last. Nur für konstante Last geeignet – bei schwankendem Strom Spannungsregler verwenden.
P_R = I² · R, Belastbarkeit ≥ 2 · P_R wählen. Wirkungsgrad η = U_Last / U.
```

**Eingaben:** Last, Versorgungsspannung U [V], Durchlassspannung U_F [V], LED-Strom I_F [mA], LEDs in Reihe, Spannung am Verbraucher U_L [V], Strom des Verbrauchers I_L [mA]

**Ergebnisse:** Berechneter Widerstand [Ω], Gewählter Normwert [Ω], Strom mit Normwert [mA], Spannung an der Last mit Normwert [V], Verlustleistung P_R [W], Empfohlene Belastbarkeit ≥ [W], Wirkungsgrad [%]

#### Zener-Diode als Spannungsstabilisator

*Vorwiderstand und Belastung der Z-Diode* — `zener_regulator`

```text
R_v = (U_e,min − U_Z) / (I_Z,min + I_L,max)  →  nächstkleinerer E12-Wert
I_Z,max = (U_e,max − U_Z) / R_v − I_L,min    P_Z = U_Z · I_Z,max    P_R = (U_e,max − U_Z)² / R_v
I_Z,min typ. 5 mA (bzw. 10 % von I_Z,max). Z-Diode nach P_Z mit Reserve wählen (z. B. 0,5 W, 1,3 W).
```

**Eingaben:** Eingangsspannung min. [V], Eingangsspannung max. [V], Zenerspannung U_Z [V], Min. Zenerstrom I_Z,min [mA], Laststrom min. [mA], Laststrom max. [mA], Belastbarkeit der Z-Diode [W]

**Ergebnisse:** Vorwiderstand (E12) [Ω], Vorwiderstand rechnerisch [Ω], Verlustleistung R_v [W], Max. Zenerstrom [mA], Max. Verlustleistung Z-Diode [W]

#### RC-Glied

*τ, Grenzfrequenz, Ladekurve, Energie* — `rc`

```text
τ = R · C    f_g = 1 / (2π · R · C)    u_C(t) = U_0 · (1 − e^(−t/τ))
Nach 5τ ≈ 99,3 % geladen.    E = ½ · C · U²    Q = C · U
```

**Eingaben:** Widerstand R [kΩ], Kapazität C [µF], Ladespannung U_0 [V], Zeit t [ms]

**Ergebnisse:** Zeitkonstante τ [ms], 5 τ [ms], Grenzfrequenz f_g [Hz], Spannung u_C(t) [V], Gespeicherte Energie [mJ], Ladung Q [µC]

#### LC-Schwingkreis

*Resonanzfrequenz f₀ = 1 / (2π√(LC))* — `lc_resonance`

```text
f₀ = 1 / (2π · √(L · C))    Z₀ = √(L / C)
```

**Eingaben:** Induktivität L [mH], Kapazität C [µF]

**Ergebnisse:** Resonanzfrequenz f₀ [Hz], Kreisfrequenz ω₀ [1/s], Kennwiderstand Z₀ [Ω]

#### Blindwiderstand & Impedanz (RLC Reihe)

*X_L, X_C, Z und φ bei Frequenz f* — `reactance`

```text
X_L = 2π · f · L    X_C = 1 / (2π · f · C)    Z = √(R² + (X_L − X_C)²)    φ = arctan((X_L − X_C)/R)
Leere Felder = Bauteil nicht vorhanden.
```

**Eingaben:** Frequenz f [Hz], Widerstand R [Ω], Induktivität L [mH], Kapazität C [µF]

**Ergebnisse:** Induktiver Blindwiderstand X_L [Ω], Kapazitiver Blindwiderstand X_C [Ω], Impedanz Z [Ω], Phasenwinkel φ [°]

#### Akku-/USV-Laufzeit

*Überbrückungszeit aus Ah, V und Last* — `battery_runtime`

```text
E_nutz = U · C · DoD    t = E_nutz · η / P_Last
Vereinfachung ohne Peukert-Effekt und Alterung – Herstellerkurven beachten.
```

**Eingaben:** Batteriespannung [V], Kapazität [Ah], Lastleistung [W], Wirkungsgrad Wandler [%], Nutzbare Entladetiefe [%]

**Ergebnisse:** Nutzbare Energie [Wh], Batteriestrom [A], Laufzeit [h], Laufzeit [min]

## Gebäudeautomation & MSR

### Sensoren & Signale

#### Analogsignal skalieren

*4–20 mA / 0–10 V ↔ Messwert ↔ Rohwert (SPS)* — `analog_scaling`

```text
Lineare Skalierung: y = y_min + (x − x_min) · (y_max − y_min) / (x_max − x_min)
Rohwerte: Siemens S7 0…27648 (Nennbereich), 12 Bit 0…4095, 15 Bit 0…32767, 16 Bit 0…65535.
NAMUR NE 43 (4–20 mA): Messbereich 3,8…20,5 mA, Ausfallsignal ≤ 3,6 mA bzw. ≥ 21 mA.
```

**Eingaben:** Signal, Gegeben, Wert, Messbereich Anfang, Messbereich Ende, Rohwertformat

**Ergebnisse:** Anteil am Messbereich [%], Signal [mA], Messwert, Rohwert

#### 4–20-mA-Bürde / Schleife

*Max. Bürde, Leitungswiderstand, max. Länge* — `loop_burden`

```text
R_max = (U_S − U_min,Transmitter) / I_max
R_Leitung = 2 · ρ · L / A    Reserve = R_max − R_Last − R_Leitung
Typisch: U_S = 24 V, Transmitter 10…12 V, AI-Bürde 250 Ω (HART ≥ 230 Ω). I_max 20 mA (bzw. 21…22 mA für NE-43-Fehlersignal).
```

**Eingaben:** Speisespannung U_S [V], Min. Transmitterspannung [V], Max. Schleifenstrom [mA], Bürde Auswertung (AI, Anzeiger …) [Ω], Leitungslänge (einfach) [m], Querschnitt [mm²]

**Ergebnisse:** Max. Gesamtbürde [Ω], Leitungswiderstand (Hin + Rück) [Ω], Reserve [Ω], Max. Leitungslänge [m]

#### Widerstandsthermometer Pt/Ni/Cu

*Temperatur ↔ Widerstand für Pt100/Pt1000/Ni1000/Cu100* — `rtd`

```text
Pt (IEC 60751): R = R0 · (1 + A·T + B·T² + C·(T − 100)·T³), C nur unter 0 °C
A = 3,9083·10⁻³, B = −5,775·10⁻⁷, C = −4,183·10⁻¹²
Ni1000 (DIN 43760): R = R0 · (1 + A·T + B·T² + D·T⁴ + F·T⁶)
Ni1000 TK5000: R = R0 · (1 + 4,427·10⁻³·T + 5,172·10⁻⁶·T² + 5,585·10⁻⁹·T³)
Cu10/Cu50/Cu100: R = R0 · (1 + α · T), linear −50 … 150 °C; α = 0,00427 1/K (Cu10, US-Praxis), 0,00428 1/K (GOST 6651)
```

**Eingaben:** Fühlertyp, Gegeben, Temperatur T [°C], Widerstand R [Ω]

**Ergebnisse:** Widerstand R [Ω], Temperatur T [°C], Empfindlichkeit dR/dT [Ω/K]

#### Leitungsfehler Pt/Ni-Fühler

*Messfehler durch Zuleitung bei 2-Leiter-Anschluss* — `rtd_wire_error`

```text
2-Leiter: Der Leitungswiderstand R_L = 2 · ρ · L / A addiert sich zum Fühler.
Fehler ΔT = R_L / (dR/dT). 3-Leiter kompensiert bei gleichen Adern, 4-Leiter vollständig.
Pt100 im 2-Leiter-Anschluss nur bei sehr kurzen Leitungen; GA meist Pt1000/Ni1000.
```

**Eingaben:** Fühlertyp, Anschluss, Leitungslänge (einfach) [m], Querschnitt [mm²], Messtemperatur [°C]

**Ergebnisse:** Leitungswiderstand (Hin + Rück) [Ω], Messfehler ΔT [K]

#### NTC-Thermistor (β-Modell)

*Temperatur ↔ Widerstand für NTC 10k, 20k …* — `ntc`

```text
R(T) = R25 · exp(β · (1/T − 1/298,15 K))
Typisch: NTC10k β = 3435 K oder 3950 K, NTC20k β = 4250 K (Herstellerangaben!).
```

**Eingaben:** Nennwiderstand R25 [kΩ], β-Wert (B25/85) [K], Gegeben, Temperatur T [°C], Widerstand R [kΩ]

**Ergebnisse:** Widerstand R [kΩ], Temperatur T [°C]

#### Thermoelemente (Typ K, J, T, N, E, R, S, B)

*Thermospannung ↔ Temperatur mit Vergleichsstelle* — `thermocouple`

```text
Grundwerte nach IEC 60584-1 (= NIST ITS-90-Referenzfunktionen, Vergleichsstelle 0 °C).
Gemessene Spannung U = E(θ_M) − E(θ_V) mit θ_V = Temperatur der Vergleichsstelle (Klemmen). Rückrechnung: E(θ_M) = U + E(θ_V).
Farben der Ausgleichsleitungen nach IEC 60584-3: K grün, J schwarz, T braun, N rosa, E violett, R/S orange, B grau (Pluspol farbig, Minus weiß).
```

**Eingaben:** Typ, Gesucht, Messtemperatur θ_M [°C], Gemessene Spannung U [mV], Vergleichsstellentemperatur θ_V [°C]

**Ergebnisse:** Thermospannung U [mV], Messtemperatur θ_M [°C], E(θ_M) bezogen auf 0 °C [mV], E(θ_V) der Vergleichsstelle [mV], Empfindlichkeit bei θ_M [µV/K]

#### Thermoelemente: Farben und Daten

| Typ | Paarung | Bereich | E(100 °C) in mV | IEC 60584-3 | ANSI MC96.1 |
|---|---|---|---|---|---|
| K | NiCr – NiAl | -270 … 1372 °C | 4,096 | grün / weiß | gelb / rot, Mantel gelb |
| J | Fe – CuNi | -210 … 1200 °C | 5,269 | schwarz / weiß | weiß / rot, Mantel schwarz |
| T | Cu – CuNi | -270 … 400 °C | 4,279 | braun / weiß | blau / rot, Mantel blau |
| E | NiCr – CuNi | -270 … 1000 °C | 6,319 | violett / weiß | violett / rot, Mantel violett |
| N | NiCrSi – NiSi | -270 … 1300 °C | 2,774 | rosa / weiß | orange / rot, Mantel orange |
| R | Pt13Rh – Pt | -50 … 1768 °C | 0,647 | orange / weiß | schwarz / rot, Mantel grün |
| S | Pt10Rh – Pt | -50 … 1768 °C | 0,646 | orange / weiß | schwarz / rot, Mantel grün |
| B | Pt30Rh – Pt6Rh | 0 … 1820 °C | 0,033 | grau / weiß | grau / rot, Mantel grau |

*Paarung (Plus – Minus), Definitionsbereich der Grundwerte (IEC 60584-1 / NIST ITS-90), Thermospannung bei 100 °C (Vergleichsstelle 0 °C), Aderfarben Plus/Minus der Ausgleichsleitung nach IEC 60584-3 (Mantel wie Plusader) und ANSI/ASTM MC96.1 (Minus immer rot). Einsatzbereich je nach Drahtstärke kleiner. Rechner „Thermoelemente“.*

#### 24-V-Versorgung dimensionieren

*Trafo (AC) oder Netzteil (DC) für Feldgeräte* — `psu_24v`

```text
P_erf = Σ(n · P) · g · (1 + Reserve)
Stellantriebe nach Dimensionierungsleistung (VA) laut Datenblatt, nicht nach Betriebsleistung. AC-Trafo-Normgrößen: 25 … 1000 VA; DC-Netzteile 2,5 … 40 A bei 24 V.
```

**Eingaben:** Versorgung, Anzahl Gruppe 1, Leistung je Gerät Gr. 1 [VA / W], Anzahl Gruppe 2, Leistung je Gerät Gr. 2 [VA / W], Anzahl Gruppe 3, Leistung je Gerät Gr. 3 [VA / W], Gleichzeitigkeitsfaktor g, Reserve [%]

**Ergebnisse:** Summe angeschlossen [VA], Erforderlich [VA], Empfohlene Normgröße [VA], Strom bei 24 V (Normgröße) [A]

### Regelung & Bussysteme

#### Reglereinstellung (Ziegler/Nichols, CHR)

*Kp, Tn, Tv aus Streckenkennwerten Ks, Tu, Tg – Schwierigkeitsgrad S* — `controller_tuning`

```text
Schwierigkeitsgrad S = Tu / Tg (bzw. Tt / T): < 0,1 leicht, ≈ 0,2 mittel, > 0,3 schwierig zu regeln.
Ziegler/Nichols (1942, max. 20 % Überschwingen; Einspeicherstrecke mit Totzeit Tt, Zeitkonstante T):
P: Kp = T/(Ks·Tt) · PI: 0,9·T/(Ks·Tt), Tn = 3,3·Tt · PD: 1,2·T/(Ks·Tt), Tv = 0,25·Tt · PID: 1,2·T/(Ks·Tt), Tn = 2·Tt, Tv = 0,42·Tt.
Chien/Hrones/Reswick (1952, Mehrspeicherstrecke, V0 = Ks·Kp):
aperiodisch – Störung: P 0,3; PI 0,6 / Tn 4·Tu; PID 0,95 / Tn 2,4·Tu / Tv 0,42·Tu – Führung: P 0,3; PI 0,35 / Tn 1,2·Tg; PID 0,6 / Tn Tg / Tv 0,5·Tu;
20 % Überschwingen – Störung: P 0,7; PI 0,7 / Tn 2,3·Tu; PID 1,2 / Tn 2·Tu / Tv 0,42·Tu – Führung: P 0,7; PI 0,6 / Tn Tg; PID 0,95 / Tn 1,35·Tg / Tv 0,47·Tu (V0-Faktoren jeweils · Tg/Tu).
Quelle: Siemens „Regeltechnik – Grundlagen“ (08/2022), Kap. 3.3 und 6.5, S. 33–34 und 82–83 (Fig. 6-15, 6-18). Startwerte – an der Anlage nachoptimieren.
```

**Eingaben:** Streckenverstärkung Ks, Verzugszeit Tu bzw. Totzeit Tt [s], Ausgleichszeit Tg bzw. Zeitkonstante T [s], Reglertyp, Einstellregel

**Ergebnisse:** Schwierigkeitsgrad S, Regelbarkeit, Proportionalbeiwert Kp, Proportionalbereich Xp = 100 %/Kp [%], Nachstellzeit Tn [s], Vorhaltezeit Tv [s]

#### KNX-Topologie & Linienplanung

*Linien, Bereiche, Segmente, Leitungslängen und Netzteillast (KNX TP)* — `knx_topology`

```text
KNX TP: bis 64 Teilnehmer je Liniensegment mit eigener Spannungsversorgung; mit Linienverstärkern bis 4 Segmente = 255 Teilnehmer je Linie; bis 15 Linien je Bereich (Linienkoppler), bis 15 Bereiche (Bereichskoppler); Koppler zählen auf der Haupt-/Bereichslinie mit.
Leitungslängen je Segment: Spannungsversorgung – Teilnehmer ≤ 350 m, Teilnehmer – Teilnehmer ≤ 700 m, Gesamtlänge ≤ 1000 m.
Praxis: neue Linie ab ca. 64 Teilnehmern und Reserve einplanen.
Quelle: KNX Association „Grundlagenwissen zum KNX Standard“ (2013), S. 8–9. Stromaufnahme je Gerät laut Hersteller.
```

**Eingaben:** Anzahl Busteilnehmer, Reserve [%], Teilnehmer je Linie (Planung), Leitungslänge je Segment (gesamt) [m], Größter Abstand Netzteil – Teilnehmer [m], Größter Abstand Teilnehmer – Teilnehmer [m], Stromaufnahme je Teilnehmer [mA], Spannungsversorgung

**Ergebnisse:** Linien, Bereiche, Teilnehmer je Linie (max.), Segmente je Linie, Spannungsversorgungen (Linien), Netzteilauslastung je Segment [%]

#### DALI-Leitung (Spannungsfall)

*Querschnitt für max. 2 V Spannungsfall* — `dali_bus`

```text
DALI (IEC 62386): max. 2 V Spannungsfall auf der Busleitung, max. 250 mA, max. 300 m.
A_min = 2 · L · I · ρ / ΔU_max
Richtwerte: bis 100 m 0,5 mm², bis 150 m 0,75 mm², bis 300 m 1,5 mm².
```

**Eingaben:** Leitungslänge (einfach) [m], Busstrom [mA], Max. Spannungsfall [V]

**Ergebnisse:** Min. Querschnitt [mm²], Empfohlener Querschnitt [mm²], Spannungsfall bei Empfehlung [V]

#### Feldbusse: Längen & Teilnehmer

| Bus | Physik | Max. Länge | Teilnehmer | Datenrate | Hinweis |
|---|---|---|---|---|---|
| BACnet MS/TP | RS-485 | 1200 m je Segment | 32 Unit Loads/Segment, max. 128 Geräte je Bus, MAC 0–127 (Manager) | 9,6–115,2 kbit/s | 120 Ω beidseitig, Bias einmal; > 750 m ggf. 76,8 kbit/s |
| Modbus RTU | RS-485 | ca. 1000–1200 m (baudratenabh.) | 32/Segment, Adressen 1–247 | 9,6–115,2 kbit/s | 120 Ω beidseitig; A/B-Bezeichnung prüfen |
| KNX TP1 | 2-Draht, freie Topologie | Linie 1000 m; 350 m Netzteil–TN; 700 m TN–TN | 64 je Liniensegment; 4 Segmente = 255 je Linie; 15 Linien × 15 Bereiche | 9,6 kbit/s | SELV 30 V DC, kein Abschluss |
| DALI | 2-Draht, frei | 300 m | 64 Betriebsgeräte | 1,2 kbit/s | max. 2 V Spannungsfall, 250 mA |
| LON FT-10 | 2-Draht, frei oder Bus | frei 500 m, Bus 2700 m | 64 je Segment | 78 kbit/s | Terminator: frei 1×, Bus 2× |
| PROFIBUS DP | RS-485 | 1200 m (≤ 93,75 kbit/s) … 100 m (12 Mbit/s) | 32/Segment, 126 Adressen | 9,6 kbit/s – 12 Mbit/s | aktiver Abschluss beidseitig |
| CAN / CANopen | 2-Draht, Linie | 25 m (1 Mbit/s) … 1000 m (50 kbit/s) | bis 127 (CANopen) | 10 kbit/s – 1 Mbit/s | 120 Ω beidseitig |
| IO-Link | Punkt-zu-Punkt, 3/5-Leiter | 20 m | 1 Gerät je Port | 4,8/38,4/230,4 kbit/s | ungeschirmte Standardleitung, M12 |
| Ethernet (BACnet/IP, Modbus TCP, PROFINET) | Twisted Pair, Stern | 100 m je Kupferstrecke | – | 100 Mbit/s + | Cat 5e oder besser |

*Typische Grenzwerte ohne Repeater laut jeweiliger Spezifikation – Herstellerangaben und Kabeltyp beachten. Abgeglichen mit Siemens „Ethernet, TCP/IP, MS/TP und BACnet“ (2023), Kap. 7.1, und KNX Association „Grundlagenwissen zum KNX Standard“ (2013), S. 8–9.*

### Datenpunkte & Kennzeichnung

#### AMEV-Datenpunktadresse

*Benutzeradresse nach AMEV GA 2005 aus Gewerk, Anlage, Anlagenteil, Datenpunkt* — `amev_address`

```text
Aufbau (25 Stellen + Klartext): Liegenschaft (5) – Gebäude (3) – Gewerk (1) – Anlage (3) – lfd. Nr. (2) – Regelfabrikat (2) – Anlagenteil (3) – lfd. Nr. (2) – Datenpunktart (2) – Datenpunktbeschreibung (2).
Beispiel: 31021-008-H-HKR-01-01-PUU-01-SM-17 = Heizkreis 01, Umwälzpumpe 01, Störmeldung „Störung allg. elektrisch“.
Quelle: AMEV „Gebäudeautomation 2005“ (Nr. 87), Anhang 2.1/2.2, S. 78–85. Bezeichnungen wie im Original (nur deutsch); projektspezifisch anpassbar, neuere AMEV-Empfehlungen (z. B. BACnet) prüfen.
```

**Eingaben:** Liegenschaftsnummer, Gebäudenummer (999 = Außenanlagen), Gewerk, Anlage, Lfd. Nr. der Anlage, Regelfabrikat (Nr.), Anlagenteil, Lfd. Nr. des Anlagenteils, Datenpunktart, Datenpunktbeschreibung

**Ergebnisse:** Datenpunktadresse, Ohne Trennzeichen (GLT), Klartext (Vorschlag, max. 30 Zeichen)

#### AMEV: Gewerke

| Kürzel | Bedeutung |
|---|---|
| A | Aufzugs- und Fördertechnik |
| E | Elektrotechnische Anlagen |
| G | Gasanlagen |
| H | Heizungstechnische Anlagen |
| I | Informationstechnische und Sicherheitstechnische Anlagen |
| K | Kältetechnische Anlagen |
| L | Lufttechnische Anlagen |
| N | Nutzungsspezifische Anlagen |
| S | Sanitärtechnische Anlagen |
| Y | Gewerkeübergreifende Anlagen |
| Z | Energiemanagement/Verbrauchserfassung |

*Quelle: AMEV „Gebäudeautomation 2005“ (Nr. 87), Anhang 2.1/2.2, S. 78–85. Bezeichnungen wie im Original (nur deutsch); projektspezifisch anpassbar, neuere AMEV-Empfehlungen (z. B. BACnet) prüfen.*

#### AMEV: Anlagen

| Kürzel | Bedeutung |
|---|---|
| AA_ | Abluftanlage |
| AFF | Antennen-, Funk-, Fernsehanlage |
| ANA | Anlage Nachtauskühlung (Fensterlüftung) |
| AUF | Hebeanlage |
| AUL | Aufzug Lasten |
| AUP | Aufzug Personen |
| AV_ | Allgemeine Stromversorgung |
| AWA | Abwasseranlage |
| AWB | Abwasserbehandlungsanlage |
| AWH | Abwasserhebeanlage |
| BEA | Beleuchtungsanlage außen |
| BEI | Beleuchtungsanlage innen |
| BES | Beleuchtung als Sicherheitsbeleuchtung |
| BMA | Brandmeldeanlage |
| BNR | Behindertennotruf |
| BUS | Übertragungsnetz, Komponenten |
| CO_ | CO-Warnanlage |
| DEA | Druckerhöhungsanlage |
| DLA | Druckluftanlage |
| DRA | Druckregelanlage |
| DSA | Dosieranlage |
| DV_ | Datenverarbeitung |
| EFA | Entfeuchtungsanlage |
| EHA | Enthärtungsanlage |
| EIN | Einspeisung (öffentliches Versorgungsnetz) |
| ELA | Elektroakustische Anlage |
| EMA | Einbruchmeldeanlage (auch Überfall) |
| EOA | Energieoptimierungsanlage |
| ERD | Erdungs- und Blitzschutzanlage |
| ESA | Entsalzungsanlage |
| FIA | Filteranlage |
| FLA | Feuerlöschanlage (Sprinkleranlage, Gaslöschanlage) |
| FTS | Fluchttürsysteme, -steueranlage, Rettungswegtechnik |
| GAL | Gaslagerungsanlage |
| GAR | Gasregelstrecke |
| GAW | Gaswarnanlage |
| GHV | Gebäudehauptverteilung |
| GUV | Gebäudeunterverteilung |
| HAS | Hausanschlussstation (HAST), Übergabe-, Trafostation |
| HER | Be-Heizung (Einläufe, Rinnen, Rohre) |
| HKR | Heizkreis |
| ISP | Informationsschwerpunkt |
| KEA | Kälteerzeugungsanlage |
| KTA | Küchentechnische Anlage |
| KVA | Kälteverteilanlage |
| LFA | Leichtflüssigkeitsabscheider |
| LWA | Leckwarnanlage |
| MLA | Medizin-/Labortechnische Anlage |
| MMA | Multimediaanlage |
| MSA | Mittelspannungsanlage |
| MSS | Mittelspannungsschaltanlage |
| MZE | Mess-/Zählwerterfassung, Wetterstation |
| NEA | Netzersatzanlage |
| NEU | Neutralisationsanlage |
| NSS | Niederspannungsschaltanlage |
| PVA | Photovoltaikanlage |
| RBS | Raumbedienstation |
| RKA | Rückkühlanlage |
| RWA | Rauchwärmeabzugsanlage |
| SIG | Signalanlage |
| SON | Sonnenschutz- und Verdunklungsanlage |
| SRA | Schrankenanlage |
| SV_ | Sicherheitsstromversorgung |
| TAA | Tankstellen-, Tankanlage |
| TEL | Telekommunikationsanlage |
| TKA | Teilklimaanlage |
| TTA | Tür-, Toranlage |
| USV | unterbrechungsfreie Stromversorgung |
| VID | Video-, Überwachungsanlage |
| VKA | Vollklimaanlage |
| WAA | Wasseranlage |
| WAU | Wasseraufbereitungsanlage (a. Trinkwasser) |
| WEA | Wärmeerzeugungsanlage |
| WED | Wassererwärmer dezentral |
| WEZ | Wassererwärmungsanlage zentral |
| WPU | Wärmepumpe |
| WRG | Wärmerückgewinnungsanlage |
| WVN | Wärmeverteilnetz |
| ZAA | Zu- und Abluftanlage |
| ZA_ | Zuluftanlage |
| ZDA | Zeitdienstanlage |
| ZKA | Zugangskontrollanlage |

*Quelle: AMEV „Gebäudeautomation 2005“ (Nr. 87), Anhang 2.1/2.2, S. 78–85. Bezeichnungen wie im Original (nur deutsch); projektspezifisch anpassbar, neuere AMEV-Empfehlungen (z. B. BACnet) prüfen.*

#### AMEV: Anlagenteile

| Kürzel | Bedeutung |
|---|---|
| AB_ | Abluft |
| AKT | Aktor allgemein |
| AST | Automatisierungsstation (allgemein) |
| ATR | Antrieb (Tür, Tor, Schranke, Fenster, Jalousie) |
| AUK | Außenluftklappe |
| AUM | Aufzugsmaschine |
| AUT | Aufzugstür |
| AU_ | Außenluft |
| BAT | Batterie/Akkumulatoren |
| BEF | Befeuchter |
| BRE | Brenner |
| BSK | Brandschutzklappe |
| DAE | Dacheinlauf, Dachrinne (Heizung) |
| DE_ | Druckerhöhung |
| DH_ | Druckhaltung |
| DR_ | Druckreduzierung |
| EG_ | Endgerät allgemein (auch Datenendgerät) |
| EH_ | Erhitzer |
| EIN | Spannungsversorgung |
| ELS | Endlagenschalter |
| ENK | Entrauchungsklappe |
| ERR | Einzelraumregler |
| FBH | Fußbodenheizung |
| FIA | Filter Abluft |
| FIG | Filter Gas |
| FIL | Filter Luft |
| FIR | Rückspülfilter |
| FIU | Filter Außenluft |
| FIW | Filter Wasser |
| FIZ | Filter Zuluft |
| FI_ | Filter allgemein |
| FOK | Fortluftklappe |
| FO_ | Fortluft |
| FU_ | Frequenzumrichter |
| GEN | Generator |
| HKE | Heizkessel |
| INT | intern (anlagenintern) |
| ISO | Isolationsüberwachung |
| JAK | Jalousieklappe |
| KGK | Küchenkühlgerät |
| KGW | Küchenwärmegerät |
| KLK | Kühlwasserkreislauf |
| KLP | Primärkreislauf |
| KLS | Sekundärkreislauf |
| KLW | Kaltwasserkreislauf |
| KM_ | Kältemaschine/Kälteerzeuger |
| KOM | Kompressor, Verdichter |
| KON | Kondensator, Verflüssiger |
| KUD | Kühl-/Lüftungsdecke |
| KUL | Kühler, Verdampfer |
| KUR | Kühlraum/Kühlzelle/Tiefkühltruhe |
| LBF | Luftbefeuchter |
| LEA | Leuchte allgemein |
| LER | Leuchte Rettungsweg (Rettungsweg-Kennzeichenleuchte) |
| LES | Leuchte Sicherheitsbeleuchtung |
| LE_ | Luft-Erhitzer |
| LKE | Luft-Kühler und -Erhitzer |
| LK_ | Luft-Kühler |
| LNE | Luft-Nacherhitzer |
| LNK | Luft-Nachkühler |
| LSF | Fehlerstromschutzschalter |
| LSR | Leser |
| LS_ | Leistungsschalter (allgemein) |
| LTS | Lasttrennschalter |
| LVE | Luft-Vorerhitzer |
| LVK | Luft-Vorkühler |
| LZ_ | Leitzentrale |
| MBM | Bewegungs-/Präsenzmelder |
| MBR | Brandmelder |
| MEB | Einbruchmelder (auch Überfallmelder) |
| MES | Messeinrichtung (physikalischer Größen) |
| MGM | Gasmelder |
| MI_ | Mischluft |
| MOT | Motor (allgemein) |
| MRM | Rauchmelder |
| M__ | Melder allgemein |
| PUE | Erhitzerpumpe |
| PUK | Kühlwasserpumpe |
| PUL | Ladepumpe |
| PUP | Pumpe Primärkreislauf |
| PUU | Umwälzpumpe |
| PUW | Kaltwasserpumpe |
| PUZ | Zirkulationspumpe |
| PU_ | Pumpe allgemein |
| REG | Regler/Controller |
| RL_ | Rücklauf |
| R__ | Raum(-luft) |
| SCH | Schalter |
| SEN | Sensor allgemein |
| SIE | Sicherheitseinrichtung |
| SIG | Signalisierung (optisch, akustisch) |
| SIL | Sicherungslasttrennschalter |
| SI_ | Sicherung, Auslöser |
| SPU | Spannungsüberwachung (Phasenüberwachung) |
| SP_ | Speicher |
| TAN | Tank/Behälter |
| TL_ | Terminal/Tableau |
| TRA | Transformator |
| TRO | Trockner |
| TTF | Tür-, Tor-, Fensterkontakt |
| UMK | Umluftklappe |
| UM_ | Umluft |
| USS | Überspannungsschutz, Phasenwächter |
| VB_ | Ventil Bypass |
| VD_ | Ventil Durchgang |
| VE_ | Ventil Erhitzer |
| VHK | Ventil Heizkreis |
| VK_ | Ventil Kühler |
| VL_ | Vorlauf |
| VNE | Ventil Nacherhitzer |
| VNK | Ventil Nachkühler |
| VRA | Abluftventilator |
| VRU | Umluftventilator |
| VRZ | Zuluftventilator |
| VR_ | Ventilator |
| VSR | Volumenstromregler |
| VST | Verstärker |
| VVE | Ventil Vorerhitzer |
| VVK | Ventil Vorkühler |
| VWR | Ventil Wärmerückgewinnung |
| VWT | Ventil Wärmetauscher |
| V__ | Ventil |
| WE_ | Wärmeerzeuger |
| WRG | Wärmerückgewinnung |
| WT_ | Wärmetauscher |
| ZTR | Zentrale allgemein |
| ZU_ | Zuluft |
| Z__ | Zähler (Verbrauchszähler/Maximumzähler) |

*Quelle: AMEV „Gebäudeautomation 2005“ (Nr. 87), Anhang 2.1/2.2, S. 78–85. Bezeichnungen wie im Original (nur deutsch); projektspezifisch anpassbar, neuere AMEV-Empfehlungen (z. B. BACnet) prüfen.*

#### AMEV: Datenpunktarten

| Kürzel | Bedeutung |
|---|---|
| AM | Alarm-/Gefahrenmeldung |
| BM | Betriebsmeldung |
| MW | Messwert |
| PA | Parameter, Sollwert (-verstellung) |
| SB | Schaltbefehl |
| SM | Störmeldung |
| ST | Stellbefehl |
| VA | Virtueller Datenpunkt analog |
| VD | Virtueller Datenpunkt digital |
| VS | Virtueller Datenpunkt »Szene« |
| VZ | Virtueller Datenpunkt »Zeitprogramm« |
| WM | Wartungsmeldung |
| ZP | Zeitprogramm |
| ZW | Zählwert |

*Quelle: AMEV „Gebäudeautomation 2005“ (Nr. 87), Anhang 2.1/2.2, S. 78–85. Bezeichnungen wie im Original (nur deutsch); projektspezifisch anpassbar, neuere AMEV-Empfehlungen (z. B. BACnet) prüfen.*

#### AMEV: Datenpunktbeschreibung

| Kürzel | Bedeutung |
|---|---|
| 00 | Sonderfall |
| 01 | Aus, Zu |
| 02 | Ein, Auf, Stufe 1 |
| 03 | Stufe 2 |
| 04 | Stufe 3 |
| 05 | Stufe 4 |
| 06 | Zentral Aus, Zentral Zu |
| 07 | Zentral Ein, Zentral Auf, Hauptschaltbefehl |
| 08 | Anforderung |
| 09 | Freigabe |
| 10 | Betrieb |
| 11 | Wartung |
| 12 | Warnung |
| 13 | Alarm, Gefahr, Notruf |
| 14 | Not-Aus |
| 15 | Störung |
| 16 | Sammelstörung |
| 17 | Störung allg. elektrisch |
| 18 | Netzausfall |
| 19 | Ausfall Hilfsenergie |
| 20 | Schaltzustandsfehler |
| 21 | SV-Betrieb |
| 22 | Störung allg. mechanisch |
| 23 | Keilriemen |
| 24 | Stellantrieb |
| 25 | Auslösung (Melder) |
| 26 | Scharfschaltung |
| 27 | Quittierung |
| 28 | Hand/Auto(-Umschaltung) |
| 29 | Anlagenschalter |
| 30 | Reparaturschalter |
| 31 | Endlage »Zu« |
| 32 | Endlage »Auf« |
| 33 | Regelantrieb |
| 34 | Regelantrieb (Rückmeldung) |
| 35 | Sollwert (berechnet) |
| 36 | Sollwertanhebung, Sollwertsteller |
| 37 | Hysterese |
| 38 | Krümmung (Heizkurve) |
| 39 | Steigung (Heizkurve) |
| 40 | Sicherheitstemperaturwächter (STW) |
| 41 | Druckwächter |
| 42 | Durchflusswächter |
| 43 | Frostschutzwächter |
| 44 | Feuchte-/Kondensations-Leckagewächter |
| 45 | Max-Wächter |
| 46 | Sicherheitstemperaturbegrenzer (STB) |
| 47 | Sicherheitsdruckbegrenzer (SDB) |
| 48 | Durchflussbegrenzer |
| 49 | Temperatur |
| 50 | Temperatur min./unten |
| 51 | Temperatur max./oben |
| 52 | Temperatur Mittelwert/Mitte |
| 53 | Vorlauftemperatur |
| 54 | Rücklauftemperatur |
| 55 | Luftqualität |
| 56 | Enthalpie |
| 57 | Feuchte |
| 58 | Druck |
| 59 | Druck max., Hochdruck |
| 60 | Druck min., Niederdruck |
| 61 | Differenzdruck |
| 62 | Volumenstrom |
| 63 | Durchfluss/Menge |
| 64 | Durchfluss max./Menge max. |
| 65 | Durchfluss min./Menge min. |
| 66 | pH-Wert |
| 67 | Drehzahl (Geschwindigkeit) |
| 68 | Schlupf |
| 69 | Wassermangel |
| 70 | Füllstand |
| 71 | Füllstand min. |
| 72 | Füllstand max. |
| 73 | Wind |
| 74 | Windrichtung |
| 75 | Niederschlag, Eis |
| 76 | Helligkeit innen/außen |
| 77 | Spannung |
| 78 | Spannung min. (auch Unterspannung) |
| 79 | Spannung max. |
| 80 | Strom |
| 81 | Strom min. |
| 82 | Strom max. |
| 83 | Leistung |
| 84 | Leistung max. (1/4 h Leistung) |
| 85 | Blindleistungswert cos ϕ |
| 86 | Zeit (Laufzeit, Betriebsstunden) |
| 87 | elektrische Arbeit, elektrische Arbeit HT |
| 88 | elektrische Arbeit NT |
| 89 | Wärmemenge |
| 90 | Gasmenge |
| 91 | Ölmenge |
| 92 | Dampfmenge |
| 93 | Kaltwasser |
| 94 | Warmwasser |
| 95 | Fahrtenanzahl |
| 96 | Haltestelle |
| 97 | Feuerwehrsteuerung, Sonder-(fahrt)steuerung |
| 98 | Szenenaufruf |

*Quelle: AMEV „Gebäudeautomation 2005“ (Nr. 87), Anhang 2.1/2.2, S. 78–85. Bezeichnungen wie im Original (nur deutsch); projektspezifisch anpassbar, neuere AMEV-Empfehlungen (z. B. BACnet) prüfen.*

#### VDI 3814: Gewerkekennzeichnung

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

#### VDI 3814: Anlagenkürzel

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

#### VDI 3814: Geräte- und Sensorkürzel

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

#### VDI 3814: Funktionskennungen

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

### GA-Effizienz

#### GA-Effizienzklasse (EN ISO 52120)

*Einsparung durch bessere Gebäudeautomation – BAC-Faktoren je Gebäudetyp* — `bac_efficiency`

```text
Energiebedarf neu = Energiebedarf alt · f_BAC,neu / f_BAC,alt;  Einsparung = 1 − f_neu / f_alt
f_BAC,th: Heizen und Kühlen; f_BAC,el: Beleuchtung und Hilfsenergie (ohne sonstige Geräte).
Klasse C = 1 (Standard-Referenz), D schlechter, B und A besser.
Quelle: Siemens „Gebäudeautomation – Einfluss auf Energie-Effizienz von Gebäuden, Anwendung der EN ISO 52120“ (09/2022), Kap. 4.4, S. 124–125. Gegen die gültige Ausgabe der EN ISO 52120-1 prüfen.
```

**Eingaben:** Gebäudetyp, GA-Klasse heute, GA-Klasse geplant, Thermischer Energiebedarf heute [kWh/a], Elektrischer Energiebedarf heute [kWh/a], Preis Wärme/Kälte [€ / CHF], Preis Strom [€ / CHF]

**Ergebnisse:** f_BAC,th heute, f_BAC,th geplant, Einsparung thermisch [%], Thermischer Bedarf geplant [kWh/a], f_BAC,el heute, f_BAC,el geplant, Einsparung elektrisch [%], Elektrischer Bedarf geplant [kWh/a], Kosteneinsparung pro Jahr [€]

#### GA-Effizienzfaktoren f_BAC (EN ISO 52120)

| Gebäudetyp | thermisch D / C / B / A | elektrisch D / C / B / A |
|---|---|---|
| Büro | 1,51 / 1 / 0,8 / 0,7 | 1,1 / 1 / 0,93 / 0,87 |
| Hörsaal | 1,24 / 1 / 0,75 / 0,5 a) | 1,06 / 1 / 0,94 / 0,89 |
| Bildungseinrichtung (Schule) | 1,2 / 1 / 0,88 / 0,8 | 1,07 / 1 / 0,93 / 0,86 |
| Krankenhaus | 1,31 / 1 / 0,91 / 0,86 | 1,05 / 1 / 0,98 / 0,96 |
| Hotel | 1,31 / 1 / 0,85 / 0,68 | 1,07 / 1 / 0,95 / 0,9 |
| Restaurant | 1,23 / 1 / 0,77 / 0,68 | 1,04 / 1 / 0,96 / 0,92 |
| Groß- und Einzelhandel | 1,56 / 1 / 0,73 / 0,6 a) | 1,08 / 1 / 0,95 / 0,91 |
| Sport, Lager, Industrie u. a. | 1 / 1 / 1 / 1 | 1 / 1 / 1 / 1 |
| Wohngebäude | 1,1 / 1 / 0,88 / 0,81 | 1,08 / 1 / 0,93 / 0,92 |

*Klasse C = 1 (Referenz). a) Wert hängt stark vom Heiz-/Kühlbedarf der Lüftung ab.
Quelle: Siemens „Gebäudeautomation – Einfluss auf Energie-Effizienz von Gebäuden, Anwendung der EN ISO 52120“ (09/2022), Kap. 4.4, S. 124–125. Gegen die gültige Ausgabe der EN ISO 52120-1 prüfen.*

## HLKS

### Luft & Klima

#### h,x-Diagramm (Mollier) & Zustandsänderungen

*Taupunkt, Feuchte, Heizen/Kühlen/Befeuchten/Mischen im Diagramm* — `hx_diagram`

```text
Luftdruck nach Normatmosphäre: p = 1013,25 · (1 − 0,0065 · H / 288,15)^5,255 hPa
p_s nach Magnus (WMO, über Wasser): p_s = 6,112 · e^(17,62·t / (243,12 + t)) hPa    p_D = φ · p_s
x = 0,622 · p_D / (p − p_D)    h = 1,006 · t + x · (2501 + 1,86 · t) kJ/kg tr. L.
Taupunkt τ aus p_D (Magnus umgekehrt); Kühlgrenze t_f: h + (x_s(t_f) − x) · 4,19 · t_f = h_s(t_f)
Erwärmen/Kühlen: x = konst.; unter dem Taupunkt ideal gesättigt (x₂ = x_s(t₂), Kondensat ṁ · Δx)
Dampfbefeuchtung: t ≈ konst.; Verdunstungsbefeuchtung: h = konst.; Mischen: x und h massengewichtet
Leistung Q = ṁ · Δh, ṁ = V̇ · ρ₁ / 3600. Diagramm: schiefwinklige h,x-Koordinaten (0-°C-Isotherme waagrecht).
Unter 0 °C mit Sättigung über Wasser gerechnet. Reale Kühler: Bypass-Faktor / Oberflächentemperatur beachten.
```

**Eingaben:** Höhe über Meer [m], Temperatur t₁ [°C], Gegeben, Relative Feuchte φ₁ [%], Taupunkt τ₁ [°C], Art, Zieltemperatur t₂ [°C], Ziel-Feuchte φ₂ [%], Temperatur Zustand B [°C], Relative Feuchte Zustand B [%], Anteil B am Massenstrom [%], Volumenstrom V̇ (bei Zustand 1) [m³/h]

**Ergebnisse:** Luftdruck p [hPa], Wassergehalt x₁ [g/kg], Taupunkt τ₁ [°C], Enthalpie h₁ [kJ/kg], Kühlgrenztemperatur t_f,1 [°C], Relative Feuchte φ₁ [%], Dampfdruck p_D,1 [hPa], Sättigungsdampfdruck p_s,1 [hPa], Dichte ρ₁ [kg/m³], Temperatur t₂ [°C], Relative Feuchte φ₂ [%], Wassergehalt x₂ [g/kg], Enthalpie h₂ [kJ/kg], Taupunkt τ₂ [°C], Enthalpiedifferenz Δh [kJ/kg], Leistung Q (+ heizen / − kühlen) [kW], Wasser (+ Befeuchtung / − Kondensat) [kg/h]

#### h,x-Diagramm: Höhenkorrektur

| Höhe ü. M. | p (Buch) | k (Buch) | p ISA | k ISA |
|---|---|---|---|---|
| 0 m | 1013 mbar | 1 | 1013 mbar | 1 |
| 200 m | 989 mbar | 0,976 | 989 mbar | 0,977 |
| 400 m | 966 mbar | 0,953 | 966 mbar | 0,953 |
| 600 m | 943 mbar | 0,931 | 943 mbar | 0,931 |
| 800 m | 921 mbar | 0,901 | 921 mbar | 0,909 |
| 1000 m | 899 mbar | 0,887 | 899 mbar | 0,887 |
| 1500 m | 842 mbar | 0,831 | 846 mbar | 0,835 |
| 2000 m | 795 mbar | 0,785 | 795 mbar | 0,785 |

*Buchwerte: Siemens „h,x-Diagramm – Aufbau und Anwendung“ (08/2022), S. 59. Spalte ISA: Normatmosphäre, wie sie der h,x-Rechner verwendet. Bei 800 m nennt das Buch k = 0,901; aus 921/1013 mbar ergibt sich 0,909 – vermutlich ein Druckfehler. Bei 1500 m liegt der Buchwert rund 0,5 % unter der Normatmosphäre.*

#### Außenluftvolumenstrom

*Nach EN 16798-1 (Personen + Fläche) oder CO₂-Bilanz* — `fresh_air`

```text
EN 16798-1 (Anhang B, Verfahren 1): q = n · q_P + A · q_B
q_P je Person: Kat. I 10, II 7, III 4 l/s.   q_B je m² (schadstoffarmes Gebäude): I 1,0, II 0,7, III 0,4 l/(s·m²); sehr schadstoffarm: 0,5 / 0,35 / 0,2; nicht schadstoffarm: 2,0 / 1,4 / 0,8.
CO₂-Bilanz (stationär): V̇ = n · G / (C_Soll − C_außen), G ≈ 20 l/h je Person bei Büroarbeit.
Außenluft ≈ 400 ppm, in Städten oft 550 … 700 ppm (Siemens „Regeln und Steuern von Lüftungsanlagen“, 10/2022, Kap. 3.3.1); bedarfsgeregelte Lüftung über CO₂/VOC siehe VDMA 24773.
```

**Eingaben:** Verfahren, Personen, Raumfläche [m²], Raumhöhe (für Luftwechsel) [m], Kategorie, Gebäude-Emission, CO₂-Abgabe je Person [l/h], CO₂-Sollwert [ppm], CO₂ außen [ppm]

**Ergebnisse:** Außenluftvolumenstrom [m³/h], Außenluftvolumenstrom [l/s], Je Person [m³/h], Luftwechselrate [1/h], CO₂-Konzentration (stationär) [ppm]

#### CO₂-Konzentrationen & Richtwerte

| CO₂ in ppm | Bedeutung |
|---|---|
| 400 – 450 | Normale Außenluft (städtisch oft 550 … 700) |
| 1000 | 20 % der eintretenden Personen unzufrieden (Pettenkofer) |
| 1500 | Grenzwert nach DIN 1946 (laut Buch) |
| 2000 | Empfindliche Personen klagen über Kopfweh |
| 4000 | Maximalwerte im Klassenzimmer nach Unterricht |
| 5000 | MAK-Wert (Arbeitsplatzgrenzwert) |
| 100000 | Kerze erlischt, Bewusstseinsverlust |

*Quelle: Siemens „Regeln und Steuern von Lüftungsanlagen“ (10/2022), Fig. 3-1, S. 28. Die Werte für DIN 1946 und MAK sind Buchangaben – gültige Regeln (z. B. ASR A3.6, UBA, TRGS 900) prüfen.*

#### RLT-Gerät & Luftkanal

*Geschwindigkeitsklasse (EN 13053), SFP-Klasse, Kanaldurchmesser* — `ahu_sizing`

```text
Anströmgeschwindigkeit v = V̇ / (3600 · B · H) im freien Gerätequerschnitt.
Geschwindigkeitsklassen EN 13053: V1 ≤ 1,6 · V2 ≤ 1,8 · V3 ≤ 2,0 · V4 ≤ 2,2 · V5 ≤ 2,5 m/s (darüber V6 …).
SFP = P_el / V̇ in W/(m³/s); Klassen EN 13779 / EN 16798-3: SFP 1 ≤ 500, 2 ≤ 750, 3 ≤ 1250, 4 ≤ 2000, 5 ≤ 3000, 6 ≤ 4500, 7 > 4500.
Rundkanal d = √(4 · V̇ / (3600 · π · v)); Niedergeschwindigkeitsanlagen < 10 m/s im Kanal.
Quellen: TROX „Planungshandbuch RLT-Geräte“ (2018), S. 6; Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022), Kap. 6. Gegen gültige Ausgaben prüfen.
```

**Eingaben:** Volumenstrom V̇ [m³/h], Gerätequerschnitt innen: Breite [m], Gerätequerschnitt innen: Höhe [m], Elektr. Leistung Ventilator(en) [kW], Geschwindigkeit im Kanal [m/s]

**Ergebnisse:** Anströmgeschwindigkeit [m/s], Geschwindigkeitsklasse, Spezifische Ventilatorleistung SFP [W/(m³/s)], SFP-Klasse, Rundkanal-Durchmesser [mm], Kanalquerschnitt [m²]

#### RLT: Geschwindigkeits- und SFP-Klassen

| Klasse | Grenze |
|---|---|
| V1 | ≤ 1,6 m/s |
| V2 | 1,6 – 1,8 m/s |
| V3 | 1,8 – 2,0 m/s |
| V4 | 2,0 – 2,2 m/s |
| V5 | 2,2 – 2,5 m/s |
| SFP 1 | ≤ 500 W/(m³/s) |
| SFP 2 | 500 – 750 W/(m³/s) |
| SFP 3 | 750 – 1250 W/(m³/s) |
| SFP 4 | 1250 – 2000 W/(m³/s) |
| SFP 5 | 2000 – 3000 W/(m³/s) |
| SFP 6 | 3000 – 4500 W/(m³/s) |
| SFP 7 | > 4500 W/(m³/s) |

*Quellen: TROX „Planungshandbuch RLT-Geräte“ (2018), S. 6 (V2–V5); SFP-Klassen nach EN 13779:2007 (übernommen in EN 16798-3). Gegen gültige Ausgabe prüfen.*

### Heizung & Wärmepumpe

#### Wärmeleistung ↔ Volumenstrom

*Wasser oder Luft: P = V̇ · ρ · c · ΔT* — `heat_flow`

```text
P = V̇ · ρ · c · ΔT
Wasser: ρ = 1000 kg/m³, c = 4,187 kJ/(kg·K) → V̇ [m³/h] = P [kW] / (1,163 · ΔT)
Luft: ρ = 1,2 kg/m³, c = 1,005 kJ/(kg·K) → P [W] ≈ 0,335 · V̇ [m³/h] · ΔT
```

**Eingaben:** Medium, Gegeben, Leistung P [kW], Volumenstrom V̇ [m³/h], Temperaturdifferenz ΔT [K], Dichte ρ [kg/m³], Spez. Wärmekapazität c [kJ/(kg·K)]

**Ergebnisse:** Leistung P [kW], Volumenstrom V̇ [m³/h], Volumenstrom V̇ [l/s], Massenstrom ṁ [kg/h]

#### Heizkennlinie: Steilheit & Parallelverschiebung

*Vorlauftemperatur nach Außentemperatur, Nachtabsenkung* — `heating_curve`

```text
Steilheit s = (T_V(−5 °C) − T_V(+15 °C)) / 20 K, üblicher Einstellbereich 0,25 … 4,0.
Raumsollwert-Korrektur/Absenkung: ΔT_V = ΔT_R · (1 + s) – Parallelverschiebung der Kennlinie.
Stützwerte: Gerade durch die Punkte bei −5 °C und +15 °C (lineares Raster wie am Regler).
Auslegung: T_V = T_i + (T_m,a − T_i)·q^(1/n) + (T_V,a − T_R,a)·q/2, q = (T_i − T_a)/(T_i − T_a,a), Heizkörperexponent n ≈ 1,3 (Fußbodenheizung ≈ 1,1).
Quelle: Siemens „Regeln und Steuern von Heizungsanlagen“ (03/2023), Kap. 3.1, S. 18–20 (Beispiel 67/33 °C, s = 1,7, −5 K → −13,5 K).
```

**Eingaben:** Kennlinie aus, Vorlauf bei −5 °C [°C], Vorlauf bei +15 °C [°C], Raumtemperatur (Auslegung) [°C], Norm-Außentemperatur [°C], Vorlauf bei Auslegung [°C], Rücklauf bei Auslegung [°C], Heizkörperexponent n, Raumsollwert-Korrektur / Absenkung [K], Aktuelle Außentemperatur [°C]

**Ergebnisse:** Steilheit s, Vorlauf bei −5 °C [°C], Vorlauf bei +15 °C [°C], Verschiebung Vorlauf ΔT_V [K], Verschoben: Vorlauf bei −5 °C [°C], Verschoben: Vorlauf bei +15 °C [°C], Vorlauf-Sollwert bei T_a [°C], Vorlauf-Sollwert verschoben [°C]

#### Wärmepumpe: COP, Carnot & JAZ

*Leistungszahl aus Temperaturen, Mindest-COP nach EN 14511, Jahresarbeitszahl* — `heat_pump`

```text
COP_Carnot = T_Senke / (T_Senke − T_Quelle) (Temperaturen in K); realer COP ≈ Gütegrad · COP_Carnot.
COP = Φ_c / P_el (Momentanwert für einen Betriebspunkt);  JAZ = Wärme / Strom über ein Jahr (kWh/kWh).
Mindest-COP für das Keymark-Gütesiegel (EN 14511): Luft/Wasser > 3,1 (A2/W35), Sole/Wasser > 4,3 (B0/W35), Wasser/Wasser > 5,1 (W10/W35). Typische JAZ (Fraunhofer 2011, AZ 2): Luft/Wasser 2,9, Sole/Wasser 3,9.
Quelle: Siemens „Kältetechnik“ (08/2022), Kap. 7.6, S. 112–115. Gütegrad ist eine Annahme – Herstellerdaten verwenden.
```

**Eingaben:** Wärmequelle, Quellentemperatur [°C], Vorlauftemperatur (Senke) [°C], Gütegrad (Annahme), COP laut Datenblatt (Prüfpunkt), Erzeugte Wärme pro Jahr [kWh/a], Stromverbrauch pro Jahr [kWh/a]

**Ergebnisse:** COP Carnot (ideal), COP geschätzt (Gütegrad), Mindest-COP Keymark (Prüfpunkt), Jahresarbeitszahl JAZ, Typische JAZ (Praxis)

#### Wärmepumpen: Mindest-COP & typische JAZ

| Wärmepumpe | Prüfpunkt | Mindest-COP | Typische JAZ |
|---|---|---|---|
| Luft/Wasser | A2/W35 | > 3,1 | 2,9 |
| Sole/Wasser | B0/W35 | > 4,3 | 3,9 |
| Wasser/Wasser | W10/W35 | > 5,1 | – |

*Quelle: Siemens „Kältetechnik“ (08/2022), S. 113–115 (JAZ: Studie Fraunhofer-Institut 2011, AZ 2). Prüfpunkt: A = Außenluft, B = Sole, W = Wasser; Zahl = Eintrittstemperatur Quelle bzw. Vorlauf in °C. Gegen gültige Ausgabe prüfen.*

### Hydraulik & Ventile

#### Hydraulische Grundschaltungen (Regelgruppen)

*Schema, Funktion, Einsatz und Ventilauslegung – mit Auswahlhilfe* — `hydraulic_circuits`

```text
Leistung Q̇ ~ V̇ · ΔT: Durchflussregelung (mengenvariabel: Drossel-, Umlenkschaltung) ändert den Volumenstrom, Mischregelung (mengenkonstant: Beimisch-, Einspritzschaltung) die Vorlauftemperatur.
Auswahl nach Verteiler (druckbehaftet: Drossel, Umlenk, Einspritz · drucklos: Beimisch), Frostgefahr (mengenkonstant), Entfeuchtung (mengenvariabel) und gewünschter Rücklauftemperatur (Brennwert/Fernwärme: Drossel oder Einspritz mit Durchgangsventil).
Quellen: Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022), Kap. 5.4–5.5, S. 144–153; Belimo „Auswahltabelle von hydraulischen Grundschaltungen“ (03/2023). Prinzipskizzen ohne Absperr- und Sicherheitsarmaturen; Auslegung immer projektbezogen prüfen.
```

**Eingaben:** Anzeigen, Schaltung, Verteiler, Leistung anpassen über, Lufterwärmer mit Einfriergefahr, Luftkühler mit Entfeuchtung, Tiefe Rücklauftemperatur nötig (Brennwert, Fernwärme, Wärmepumpe), Verbraucher-VL deutlich tiefer als Erzeuger-VL (Flächenheizung), Konstanter Durchfluss im Erzeugerkreis nötig

**Ergebnisse:** Schaltung, Begründung, Alternative, Regelungsart, Verteiler, Vorlauftemperatur am Verbraucher, Durchfluss Verbraucherkreis, Durchfluss Erzeugerkreis, Pumpen, Rücklauftemperatur zum Erzeuger, Funktionsweise, Eigenschaften, Typische Anwendungen, Ventil- und Bypassauslegung, Hinweis

#### Auswahltabelle hydraulische Grundschaltungen

| Schaltung | Regelung | Verteiler | VL am Verbraucher | Durchfluss Verbraucher / Erzeuger | Pumpen | Rücklauf | Typisch für |
|---|---|---|---|---|---|---|---|
| Drosselschaltung | mengenvariabel | druckbehaftet | = Erzeuger-VL | Variabel / Variabel | Erzeugerpumpe (drehzahlgeregelt empfohlen) | Tief bei Teillast (Heizen) | Zonen, Heizkörper, Luftkühler mit Entfeuchtung |
| Drosselschaltung mit PICV | mengenvariabel | druckbehaftet | = Erzeuger-VL | Variabel / Variabel | Erzeugerpumpe (drehzahlgeregelt) | Hoch bei Teillast (Kühlen), tief bei Teillast (Heizen) | Luftkühler mit Entfeuchtung |
| Einspritzschaltung mit Durchgangsventil | mengenkonstant | druckbehaftet | variabel | Konstant / Variabel | Erzeuger- und Verbraucherpumpe | Relativ tief (Heizen) | Wärmepumpe, Brennwert, Fußbodenheizung |
| Einspritzschaltung mit PICV | mengenkonstant | druckbehaftet | variabel | Konstant / Variabel | Erzeuger- und Verbraucherpumpe | Relativ tief (Heizen) | Lufterwärmer |
| Beimischschaltung | mengenkonstant | drucklos | variabel | Konstant / Variabel | Nur Verbraucherpumpe | Tief bei kleiner Last (Heizen), hoch bei kleiner Last (Kühlen) | Heizgruppen, Lufterwärmer mit Frostgefahr |
| Beimischschaltung mit fester Vormischung | mengenkonstant | drucklos | variabel | Konstant / Variabel | Nur Verbraucherpumpe | Allgemein tief | Fußboden-/Wandheizung, Betonkern |
| Umlenkschaltung | mengenvariabel | druckbehaftet | = Erzeuger-VL | Variabel / Konstant | Erzeugerpumpe | Heizen: mittel bis hoch · Kühlen: mittel bis tief | Nachwärmer, WRG, Kühler mit Entfeuchtung |
| Einspritzschaltung mit Dreiwegventil | mengenkonstant | druckbehaftet | variabel | Konstant / Konstant | Erzeuger- und Verbraucherpumpe | Relativ hoch (bei 0 % Last = Erzeuger-Vorlauf) | Lufterwärmer mit Außenluft |

*Quellen: Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022), Kap. 5.4–5.5, S. 144–153; Belimo „Auswahltabelle von hydraulischen Grundschaltungen“ (03/2023). Prinzipskizzen ohne Absperr- und Sicherheitsarmaturen; Auslegung immer projektbezogen prüfen.*

#### Ventil kv/kvs & Ventilautorität

*kv-Wert, kvs-Auswahl, Δp, Autorität – auch aus Leistung/ΔT* — `valve_kv`

```text
V̇ = P · 3600 / (ρ · c · ΔT)   (Angaben der Ventilliste nach VDI 3814 Blatt 4.1, Tab. 8: Leistung, VL/RL, ΔT)
kv = V̇ · √(ρ/1000 / Δp[bar])    Δp_V100 = (V̇ / kvs)² · ρ/1000 · 100 kPa
Ventilautorität a = Δp_V100 / (Δp_V100 + Δp_MV)
Δp_MV = Druckverlust der mengenvariablen Strecke. Empfehlung a ≥ 0,5 (mind. 0,3).
Wasser: ρ = 1000 kg/m³, c = 4,187 kJ/(kg·K). Glykolgemische: ρ und c aus dem Datenblatt des Herstellers.
Kleinste regelbare Menge: V̇_min/V̇_100 = 1/√(1 + a·(S_v² − 1)), S_v = Stellverhältnis kvs/kvr; Wärmeleistung mit Wärmetauscher-Kennwert a_WT: Q̇/Q̇_100 = 1/(1 + a_WT·(V̇_100/V̇ − 1)). Beispiel a = 0,5, S_v = 50: ≈ 3 % (ohne WT), ≈ 4 % (a_WT = 0,7). Zwei parallele Ventile 1/3 + 2/3 in Sequenz senken die minimale Leistung auf etwa ein Drittel (QM Holzheizwerke, FAQ 37, 2015).
```

**Eingaben:** Auslegung aus, Volumenstrom V̇ [m³/h], Leistung P [kW], Temperaturdifferenz ΔT (VL − RL) [K], Medium, Dichte ρ [kg/m³], Spez. Wärmekapazität c [kJ/(kg·K)], Gewünschter Δp am Ventil [kPa], Δp mengenvariable Strecke [kPa], Stellverhältnis S_v (kvs/kvr), Wärmetauscher-Kennwert a_WT

**Ergebnisse:** Volumenstrom V̇ [m³/h], Berechneter kv-Wert [m³/h], Gewählter kvs-Wert (Normreihe) [m³/h], Δp bei kvs (voll offen) [kPa], Ventilautorität a, Kleinster regelbarer Volumenstrom [%], Kleinste regelbare Wärmeleistung [%]

#### Rohrabmessungen (Stahlrohr)

| Nennweite | Zoll | Außen-Ø in mm |
|---|---|---|
| DN 6 | 1/8" | 10,2 |
| DN 8 | 1/4" | 13,5 |
| DN 10 | 3/8" | 17,2 |
| DN 15 | 1/2" | 21,3 |
| DN 20 | 3/4" | 26,9 |
| DN 25 | 1" | 33,7 |
| DN 32 | 1 1/4" | 42,4 |
| DN 40 | 1 1/2" | 48,3 |
| DN 50 | 2" | 60,3 |
| DN 65 | 2 1/2" | 76,1 |
| DN 80 | 3" | 88,9 |
| DN 100 | 4" | 114,3 |
| DN 125 | 5" | 139,7 |
| DN 150 | 6" | 165,1 / 168,3 |

*Gewinderohre EN 10255 / ISO 65 bzw. nahtlose/geschweißte Rohre EN 10220 (Außendurchmesser in mm). Die Zollgröße ist eine Nenngröße, nicht der Durchmesser. Wanddicke je Reihe (mittel/schwer) verschieden.*

## Überspannungs- & Blitzschutz

### Energietechnik 230/400 V

#### Überspannungsschutz 230/400 V: Auswahl

*Typ 1+2, 2 oder 3 je Einbauort und Blitzschutzzone, Schaltung, U_C, I_imp/I_n, U_p* — `spd_power`

```text
Blitzschutzzonen-Konzept (IEC 62305-4): LPZ 0A → 1 (äußerer Blitzschutz oder Freileitung) Blitzstrom-Ableiter Typ 1 bzw. Kombi-Ableiter Typ 1+2; LPZ 0B → 1 und 1 → 2 Überspannungs-Ableiter Typ 2; LPZ 2 → 3 Typ 3 am Endgerät.
Folgestufe: Typ 2 in der Unterverteilung, wenn die Leitung zum vorgeschalteten SPD länger als 10 m ist; Typ 3 vor dem Gerät entsprechend (DEHN-Auswahlmatrix).
I_imp je Pfad (äußerer Blitzschutz, IEC 62305-1 Anhang E vereinfacht): 50 % des Blitzstroms der Blitzschutzklasse fließen in die Erdungsanlage, 50 % verteilen sich auf die n Versorgungsleitungen und deren m Leiter: I_imp = 0,5 · I / (n · m). Ohne Bestimmung mindestens 12,5 kA je Pfad (DIN VDE 0100-534). N–PE-Funkenstrecke (3+1) führt die Summe.
Typ 2: I_n ≥ 5 kA je Pfad (8/20 µs), üblich 20 kA. U_C ≥ 1,1 · U₀ = 253 V (L–N, L–PE), N–PE ≥ U₀; IT ohne N: L–PE ≥ U.
Schutzpegel U_p ≤ 2,5 kV (Überspannungskategorie II), wirksamer Schutz bis ca. 10 m bei U_p ≤ 0,8 · U_W = 2,0 kV.
Anschluss: a + b ≤ 0,5 m (V-Verdrahtung), Cu ≥ 16 mm² (Typ 1) bzw. ≥ 6 mm² (Typ 2) zum Potentialausgleich. In DE im Vorzählerbereich nur Typ 1 (VDE-AR-N 4100). Produktbeispiele DEHN: DEHNventil, DEHNshield (Typ 1+2), DEHNguard (Typ 2), DEHNrail, DEHNflex, DEHNsafe (Typ 3). Gegen gültige Normausgabe und Herstellerdaten prüfen.
```

**Eingaben:** Einbauort, Gebäude / Einspeisung, Blitzschutzklasse, Metallene Versorgungsleitungen n, Leitungslänge zum vorgeschalteten SPD [m], Netzsystem, Versorgung

**Ergebnisse:** Ableiter, Zonenübergang, Schaltung, U_C mindestens (L–N / L–PE) [V], U_C mindestens N–PE [V], I_imp je Pfad (10/350 µs) [kA], I_imp gesamt / N–PE [kA], I_n je Pfad (8/20 µs) mindestens [kA], Schutzpegel U_p höchstens [kV], Anschluss, Vorsicherung, Beispiele DEHN, Nächste Schutzstufe

#### Wirksamer Schutzpegel und Schutzbereich

*U_p,f = U_p + ΔU, Vergleich mit U_W, Schwingungsschutzbereich l_po* — `spd_distance`

```text
Wirksamer Schutzpegel U_p,f = U_p + ΔU (Anschlussleitungen): für 230/400 V pauschal ΔU = 0,5 kV (OVE Fachinformation E 09) bzw. bei Blitzstrom ca. 1 kV je m Anschlussleitung (IEC 62305-4).
U_p,f ≤ 0,5 · U_W: Schutzbereich unabhängig von der Leitungslänge. Sonst gilt der konventionelle Schutzbereich von 10 m, wenn U_p,f ≤ 0,8 · U_W; darüber hinaus Schwingungsschutzbereich l_po = (U_W − U_p,f) / k mit k = 25 V/m.
U_W = Bemessungs-Stoßspannung des Geräts (230/400 V: Kategorie I 1,5 kV, II 2,5 kV, III 4 kV, IV 6 kV, IEC 60664-1). Induktiver Schutzbereich bei nahen/direkten Einschlägen zusätzlich nach IEC 62305-4 Anhang prüfen.
```

**Eingaben:** Schutzpegel U_p des SPD [kV], Zuschlag Anschlussleitung, Länge Anschlussleitungen a + b [m], Gerät (Überspannungskategorie), Leitungslänge SPD → Gerät [m]

**Ergebnisse:** Wirksamer Schutzpegel U_p,f [kV], Bemessungs-Stoßspannung U_W [kV], U_p,f / U_W [%], Schwingungsschutzbereich l_po [m], Bewertung

#### SPD-Schaltungen je Netzsystem

| Netzsystem | Schaltung | U_C L–N / L–PE | U_C N–PE |
|---|---|---|---|
| TN-C (3~) | 3+0 | ≥ 253 V | – |
| TN-S (3~) | 4+0 oder 3+1 | ≥ 253 V | ≥ 230 V |
| TN-S (1~) | 2+0 oder 1+1 | ≥ 253 V | ≥ 230 V |
| TT (3~) | 3+1 | ≥ 253 V | ≥ 230 V |
| TT (1~) | 1+1 | ≥ 253 V | ≥ 230 V |
| IT ohne N (3~) | 3+0 | L–PE ≥ U (400 V) | – |

*DIN VDE 0100-534 / IEC 60364-5-53 (Mindest-U_C, Tabelle 534.2) und DEHN-Typbezeichnungen. 3+1/1+1: Varistoren L–N und Summenfunkenstrecke N–PE (CT2); in TT-Systemen vor dem RCD üblich.*

#### Überspannungskategorien und Bemessungs-Stoßspannung

| Kategorie | U_W | Betriebsmittel |
|---|---|---|
| IV | 6 kV (4 kV) | am Speisepunkt: Zähler, Hauptsicherung, Rundsteuerempfänger |
| III | 4 kV (2,5 kV) | feste Installation: Verteiler, Schalter, Steckdosen, Motoren |
| II | 2,5 kV (1,5 kV) | Verbrauchsgeräte: Haushaltsgeräte, Werkzeuge |
| I | 1,5 kV (0,8 kV) | besonders schutzbedürftige Elektronik |

*IEC 60664-1 / DIN VDE 0100-443 Tabelle 443.2 für Netze 230/400 V (120/208 V in Klammern). Das SPD muss U_p,f unter U_W der zu schützenden Betriebsmittel halten.*

#### SPD Energietechnik: Kenndaten (DEHN)

| Gerät (Art.-Nr.) | Typ | Netz | U_C | I_imp (10/350 µs) | I_n (8/20 µs) | U_p | Vorsicherung / Kurzschluss | Bauform |
|---|---|---|---|---|---|---|---|---|
| DEHNvenCI DVCI 1 255 FM (961 205) | Typ 1 + Typ 2 (+ Typ 3 koordiniert ≤ 10 m) | 230 V, 1-polig | 255 V | 25 kA | 25 kA | ≤ 1,5 kV | nicht nötig (integriert, 100 kA); I_fi 50 kA | 2 TE, IP 20 |
| DEHNventil DV M2 TNS 255 FM (954 405) | Typ 1 + 2 + 3 | TN-S 230/400 V | 255 V | 25 kA je Pol, 100 kA gesamt | 25 / 100 kA | ≤ 1,5 kV | max. 250 A gG; I_SCCR 50 kA (100 kA geprüft) | 4 TE, IP 20 |
| DEHNguard DG M TNS ACI 275 FM (952 440) | Typ 2 (+ Typ 3 koordiniert ≤ 10 m) | TN-S 230/400 V | 275 V L–PE, 255 V N–PE | – | 20 kA | ≤ 1,5 kV | keine externe Sicherung (ACI); I_SCCR 25 kA | 4 TE, IP 20 |
| DEHNrail DR M 2P 255 FM (953 205) | Typ 3 | 230 V, I_L 25 A | 255 V AC/DC | – | 3 kA (gesamt 5 kA); U_OC 6 kV | ≤ 1,25 kV L–N, ≤ 1,5 kV L/N–PE | 25 A gG oder B 25; I_SCCR 6 kA | 1 TE, IP 20 |
| DEHNcube DCU YPV SCI 1000 2M (900 920) | Typ 2 PV (EN 61643-31) | PV-Gleichspannung, 2 MPP | U_CPV 1000 V | – | 12,5 kA (I_max 25 kA, gesamt 40 kA) | ≤ 4 kV (≤ 3,5 kV bei 5 kA) | I_SCPV 1000 A | IP 65, 130 × 94 × 81 mm |

*Datenblattwerte aus DEHN WPX050 (Schutzvorschlag Rechenzentren, 04/2022). Typ nach EN 61643-11 (PV: EN 61643-31). „Koordiniert ≤ 10 m“ = energetisch koordinierte Schutzwirkung zum Endgerät bis 10 m Leitung. Beispiele – gegen aktuellen Katalog prüfen.*

### MSR, Sensoren & Datenleitungen

#### Überspannungsschutz MSR, Sensoren und Datenleitungen

*Ableiterart nach Zone, U_C-Klasse, Bandbreite, Pt100-Messfehler – mit DEHN-Beispielen* — `spd_signal`

```text
Leitungen, die das Gebäude verlassen (Feldgeräte im Freien, Leitungen zwischen Gebäuden): Zonenübergang LPZ 0A → 1, Kombi-Ableiter mit Blitzstromtragfähigkeit D1 (10/350 µs), z. B. 2,5 kA je Ader (DEHN BLITZDUCTOR XT ML4 BE 24: D1 2,5 kA je Ader, C2 10 kA je Ader). Innerhalb des Gebäudes (LPZ 1 → 2): Überspannungs-Ableiter Kategorie C2 (8/20 µs).
U_C ≥ höchste Betriebsspannung des Signals einschließlich Toleranz; Nennstrom des Ableiters ≥ Signal-/Laststrom; Grenzfrequenz ≥ Übertragungsfrequenz.
Pt100/Pt1000 im 2-Leiter-Anschluss: jede Ader hat die Längsimpedanz R des Ableiters, Messfehler ΔT = 2 · R · Stufen / (dR/dT); 3-/4-Leiter-Anschluss kompensiert. In Ex-i-Kreisen nur dafür zugelassene Ableiter.
Blitzschutzklasse I (z. B. Rechenzentrum) nach VdS 2833: I_imp je Ader 2,5 kA bei weniger als 3 Doppeladern, 1 kA ab 3, 0,5 kA ab 10 Doppeladern (DEHN WPX050 Tabelle 3).
Werte nach DEHN Auswahlmatrix Industriegebäude (DS390) und BLITZDUCTOR-Datenblatt; IEC 61643-21/-22, IEC 62305-4.
```

**Eingaben:** Signal / Schnittstelle, Leitungsweg, Höchste Betriebsspannung des Signals [V], Fühler, Anschluss, Längsimpedanz je Ader und Ableiter [Ω], Ableiter im Messkreis, Doppeladern im Kabel

**Ergebnisse:** Ableiter, Zonenübergang, U_C-Klasse mindestens [V], Übertragung, Beispiele DEHN, Einbau, Messfehler durch Ableiter (2-Leiter) [K], I_imp je Ader bei LPL I (VdS 2833) [kA]

#### Prüfkategorien für Datenleitungs-Ableiter

| Kategorie | Prüfspannung | Prüfstrom | Einsatz |
|---|---|---|---|
| C1 | 0,5–1 kV (1,2/50 µs) | 0,25–0,5 kA (8/20 µs) | induktive Kopplung, Feinschutz |
| C2 | 2–10 kV (1,2/50 µs) | 1–5 kA (8/20 µs) | Überspannung, LPZ 0B → 1 und höher |
| C3 | ≥ 1 kV (1 kV/µs) | 10–100 A (10/1000 µs) | Schutzpegel-Prüfung (steile Flanke) |
| D1 | ≥ 1 kV | 0,5–2,5 kA (10/350 µs) | Blitzteilströme, LPZ 0A → 1 |

*IEC 61643-21 / DIN EN 61643-21 (Auswahl). D1 = Blitzstromtragfähigkeit für LPZ 0A → 1, C2 = Überspannung LPZ 0B → 1 und höher. DEHN-Beispiel BLITZDUCTOR XT ML4 BE 24: D1 2,5 kA je Ader (10 kA gesamt), C2 10 kA je Ader. Gegen gültige Ausgabe prüfen.*

#### SPD MSR und Daten: Kenndaten (DEHN)

| Gerät (Art.-Nr.) | Schaltung | U_N / U_C | I_L | I_imp D1 je Ader | I_n C2 je Ader | U_p Ader–Ader / Ader–PG | f_G | Einsatz |
|---|---|---|---|---|---|---|---|---|
| BLITZDUCTORconnect BCO ML2 BE 24 (927 224) | 2 Einzeladern, gemeinsamer Bezug (unsymmetrisch) | 24 V / 33 V DC | 0,75 A | 1,5 kA (3 kA gesamt) | 5 kA (10 kA gesamt) | ≤ 90 V / ≤ 75 V (C3: ≤ 45 V Ader–PG) | 3,4 MHz | 4–20 mA, 0–10 V, Binär 24 V; Dachaufbauten, Brandmeldeanlage |
| BLITZDUCTORconnect BCO ML2 BD 24 (927 244) | 1 Doppelader, erdpotentialfrei (symmetrisch) | 24 V / 36 V DC | 0,75 A | 1,5 kA | 5 kA | ≤ 57 V / ≤ 600 V | 5,8 MHz | erdfreie Signale und Meldungen; NSHV, Notstrom |
| BLITZDUCTORconnect BCO ML2 BD HF 5 (927 271) | 1 Doppelader, hochfrequent (symmetrisch) | 5 V / 8,5 V DC | 0,75 A | 1,5 kA | 5 kA | ≤ 42 V / ≤ 600 V (C3: ≤ 15 V Ader–Ader) | 100 MHz | Bussysteme (z. B. RS-485); Serverraum, Sicherheitstechnik |
| DEHNpatch DPA CLE IP66 (929 221) | RJ45, alle 4 Paare, Klasse E | 5 V / 8,5 V DC; PoE 60 V Paar–Paar | 1 A | 0,8 kA (4 kA gesamt) | 2,5 kA Ader–PG (10 kA gesamt) | ≤ 170 V / ≤ 600 V; PoE ≤ 120 V | 250 MHz | Gigabit-Ethernet und PoE++ im Freien (IP 66), IP-Kameras |
| BLITZDUCTOR XT BXT ML4 BD EX 24 | eigensicher ia, erdfrei 500 V | U_i 30 V | I_i 500 mA | – | 20 kA gesamt (8/20 µs) | ≤ 52 V Ader–Ader | 7,7 MHz | 4–20 mA/HART in Ex-Zone 1 (ATEX, IECEx); C_i/L_i vernachlässigbar |

*Datenblattwerte aus DEHN WPX050 (04/2022) und WPX035 Tabelle 2 (01/2022); Prüfkategorien nach IEC 61643-21 (D1 = 10/350 µs, C2 = 8/20 µs). Unsymmetrisch (BE) = Einzeladern mit gemeinsamem Bezug, z. B. 4–20 mA gegen 0 V; symmetrisch (BD) = erdpotentialfreie Doppelader. BLITZDUCTORconnect ist für Einbau in Ex-Zone 2 zugelassen (II 3G Ex ec IIC T4 Gc). Beispiele – gegen aktuellen Katalog prüfen.*

### Anwendungen: Rechenzentrum & Ex-Bereich

#### Rechenzentrum: Schutzvorschlag nach Anwendung

| Nr. | Schutz für | Zone | Energietechnik | Informationstechnik | Art.-Nr. |
|---|---|---|---|---|---|
| 1 | Niederspannungsschaltanlage (NSHV) | LPZ 0A → 1 | Kombi-Ableiter Typ 1+2(+3): DEHNvenCI DVCI 1 255 FM, alternativ DEHNventil DV M2 TNS 255 FM | BLITZDUCTORconnect BCO ML2 BD 24; Netzanalyse DEHNrecord DRC SD 2 1 | 961 205 / 954 405; 927 244; 910 921 |
| 2 | Notstromversorgung | LPZ 1 | Typ 2: DEHNguard DG M TNS ACI 275 FM | BLITZDUCTORconnect BCO ML2 BD 24 | 952 440; 927 244 |
| 3 | Serverraum | LPZ 1 → 2 (Server LPZ 3) | Typ 2 im Bodentank: DEHNguard ACI; große Räume zusätzlich Typ 3 | BLITZDUCTORconnect BCO ML2 BD HF 5 | 952 440; 927 271 |
| 4 | Brandmeldeanlage | LPZ 1 → 2 | Typ 3: DEHNrail DR M 2P 255 | BLITZDUCTORconnect BCO ML2 BE 24 | 953 205; 927 224 |
| 5 | Sicherheitszentrale, Büros, IP-Kameras | LPZ 0B → 1, 1 → 2 | Typ 3: DEHNrail DR M 2P 255 | DEHNpatch DPA CL8 EA 4PPOE, DEHNpatch Outdoor DPA CLE IP66, BCO ML2 BD HF 5 | 953 205; 929 161; 929 221; 927 271 |
| 6 | PV-Anlage | LPZ 0B → 1 | Typ 2 PV: DEHNcube DCU YPV SCI 1000 2M | – | 900 920 |
| 7 | Dachaufbauten (Heizung, Klima, Lüftung) | LPZ 0B → 1 | Typ 2: DEHNguard ACI (I_n 20 kA je Pol bei LPL I) | BLITZDUCTORconnect BCO ML2 BE 24 | 952 440; 927 224 |

*Nach DEHN WPX050 Bild 11 (Rechenzentrum, Blitzschutzklasse I). NSHV: Kombi-Ableiter auf Funkenstreckenbasis mit 100 kA (10/350 µs) gesamt bei LPL I; Unterverteilungen wegen Leitungslängen > 10 m mit Typ 2 (DEHNguard ACI ohne Vorsicherung). Serverraum: nicht geschirmt Typ 2 im Bodentank ausreichend, größere Räume zusätzlich Typ 3 im Abgang der Stromschiene oder in der Rack-Stromversorgung; Raum = LPZ 2, Servergehäuse kann LPZ 3 sein (nachweisbare Dämpfung, Anbindung ≥ 6 mm² Cu). Kenndaten siehe Tabellen „SPD Energietechnik“ und „SPD MSR und Daten“. Beispiele – gegen aktuellen Katalog prüfen.*

#### Rechenzentrum: Blitzschutz, Erdung, Schirmung

| Thema | Anforderung | Grundlage |
|---|---|---|
| Blitzschutzklasse | in der Regel LPL I (Risikoanalyse mit dem Betreiber abstimmen); Stoßströme bis 200 kA (10/350 µs) | EN 62305-2, EN 50600 |
| Äquipotentialflächen | Bewehrung mit Maschen 5 × 5 m, alle 1 m mit der Bewehrung verbinden; verkürzt den Trennungsabstand und senkt die Impedanz | EN 62305-4 |
| Erdung | ein gemeinsames Erdungssystem (Blitzschutz, MS, NS, TK, Schirmung, Potentialausgleich); Ringerder mit Masche 5 × 5 m unter oder außerhalb des Fundamenterders, alle 5 m damit verbunden | EN 62305-4, EN 50600-2-2 |
| Raumschirmung | reine Gitterstruktur 5 × 5 m dämpft das Blitzfeld etwa um Faktor 2 (6 dB); Durchbrüche minimieren, Fenster und Türen einbeziehen; Bewertung gegen Prüfschärfegrad 3 (EN 61000-4-9/-10) | EN 62305-4 |
| Potentialausgleichsnetzwerk | dreidimensional vermascht: Rohre, Bewehrung, Gitterroste, Kabel- und Lüftungskanäle, Aufzugsschienen, Fenster/Türen; Racks kurz anbinden (möglichst kein Runddraht) | EN 62305-4, DIN VDE 0100-444 |
| Dachaufbauten | getrennte Fangeinrichtung (HVI-Leitung) hält den Trennungsabstand; sonst Leitungen über SPD einbinden: Typ 2 mit I_n 20 kA je Pol (LPL I), 15 kA (LPL II), 10 kA (LPL III/IV), Informationstechnik 10 kA je Ader | WPX050 Tabelle 1 |
| Blitzschutzzonen | NSHV-Einspeisung LPZ 0A → 1 (Typ 1/Kombi-Ableiter); Unterverteilungen Typ 2 (Leitungen > 10 m: Induktion); Serverraum LPZ 2, Servergehäuse LPZ 3 möglich | EN 62305-4 |
| Anschluss SPD | Anschlusslänge ≤ 0,5 m; Einspeisungen über Kuppelschalter (mehrere Trafos) gleich beschalten; Typ 2 auch in den Abgängen großer NSHV | IEC 60364-5-53 |
| Serverraum | nicht geschirmt: Typ 2 im Bodentank ausreichend; größere Räume zusätzlich Typ 3 im Abgang der Stromschiene oder in der Rack-Stromversorgung; Servergehäuse als LPZ 3 nur mit nachgewiesener Dämpfung, Anbindung ≥ 6 mm² Cu | WPX050 |

*Planungsgrundsätze aus DEHN WPX050 „Blitz- und Überspannungsschutzkonzept für Rechenzentren“ (04/2022) mit EN 62305-2/-3/-4, EN 50600-1/-2-2, IEC 60364-5-53, DIN VDE 0100-444. Gegen gültige Normausgabe prüfen.*

#### Überspannungsschutz im Ex-Bereich (eigensicherer Messkreis)

*Ableiter für Ex-i-Messkreise: Zündschutzart, U_i/I_i gegen U_o/I_o, Erdfreiheit, Einbau – nach DEHN WPX035* — `spd_ex`

```text
Nach DEHN WPX035 „Blitz- und Überspannungsschutz für explosionsgefährdete Bereiche“ und DIN EN 60079-11/-14/-25, DIN EN 62305-4:
• Einbauort: Übergang LPZ 0B → 1 am Feldgerät (z. B. Temperatur-Messumformer am Tank) und am anderen Ende der Feldleitung mit gleichem Ableitvermögen; nach dem Trennverstärker in der Warte (LPZ 2) ist kein weiteres SPD nötig, wenn der Schirm beidseitig aufgelegt ist.
• Ableitvermögen mindestens 10 Impulse à 10 kA (8/20 µs) ohne Defekt; Ableiter im metallisch geschirmten Gehäuse, Erdung mindestens 4 mm² Cu; Leitung Ableiter → Betriebsmittel im beidseitig geerdeten Metallrohr oder geschirmt mit höchstens 1 m.
• Zündschutzart: Zone 0/20 ia, Zone 1/21 ia oder ib, Zone 2/22 ia, ib oder ic; ein nach ia bescheinigter Ableiter eignet sich auch für ib/ic.
• Zusammenschaltung: U_i (bzw. U_C) des Ableiters ≥ U_o des Speisegeräts, I_i (Nennstrom) ≥ I_o; innere Kapazität C_i und Induktivität L_i in den Nachweis der Eigensicherheit aufnehmen.
• Erdfreiheit: Messumformer mit Isolationsfestigkeit ≥ 500 V AC gelten als erdfrei – der Ableiter darf das nicht aufheben; bei < 500 V AC gilt der Kreis als geerdet, dann Schutzpegel U_p (Ader–PG) bei 10 kA (8/20 µs) unterhalb der Isolationsfestigkeit (z. B. ≤ 35 V).
• Endgeräte nach NAMUR NE 21: 0,5 kV Ader/Ader, 1 kV Ader/Erde (IEC 61000-4-5).
Vorgabewerte = Beispiel Tabelle 2 (Messumformer U_o 29,4 V, I_o 130 mA; BLITZDUCTOR XT ML4 BD EX 24: ia, U_i 30 V, I_i 500 mA, 20 kA, U_p ≤ 52 V). Produktangaben gegen aktuelle Bescheinigung prüfen.
```

**Eingaben:** Ex-Zone am Feldgerät, U_o des Speisegeräts / Messkreises [V], I_o (Kurzschlussstrom im Fehlerfall) [mA], Zündschutzart des Ableiters, U_i bzw. U_C des Ableiters [V], I_i bzw. Nennstrom des Ableiters [mA], Isolation Messumformer gegen Erde, Leitung Ableiter → Feldgerät [m], Verlegung dieser Leitung

**Ergebnisse:** Geforderte Zündschutzart, U_i des Ableiters mindestens [V], I_i des Ableiters mindestens [mA], Einbauort, Ableitvermögen, Schutzpegel / Erdfreiheit, Leitung zum Feldgerät, Erdung und Schirm, Beispiele DEHN

#### Ex-Bereich: Blitz- und Überspannungsschutz

| Thema | Anforderung | Grundlage |
|---|---|---|
| Blitzschutzklasse | mindestens II (Blitzkugel 30 m, Masche 10 × 10 m); Abweichung nur begründet oder nach Risikoanalyse | DIN EN 62305-3 |
| Fangeinrichtung | außerhalb der Ex-Zonen errichten (Funken am Einschlagpunkt); natürliche Teile (Dach, Rohre, Behälter) nur ab 5 mm Stahl und ohne gefährliche Erwärmung | DIN EN 62305-3 Anhang D |
| Ableitungen | je 10 m Umfang der Dachaußenkante eine, mindestens 2; Potentialausgleich am Boden und alle 20 m Höhe; durchverbundene Bewehrung zulässig | DIN EN 62305-3 |
| Trennungsabstand | an der Näherungsstelle berechnen (Aufteilung des Blitzstroms auf die Ableitungen) | DIN EN 62305-3 |
| Gebäudeschirmung | Metallfassade und Bewehrung zum Schirmkäfig verbinden: Magnetfeld 10- bis 300-mal kleiner | DIN EN 62305-4 |
| SPD-Einbauort | Übergang LPZ 0B → 1, bevor Überspannungen Ex-Zone 0/20 erreichen; am anderen Ende der Feldleitung gleiches Ableitvermögen | DIN EN 62305-4 |
| SPD-Ableitvermögen | mindestens 10 Impulse à 10 kA (8/20 µs) ohne Defekt | DIN EN 60079-11/-14/-25 |
| SPD-Gehäuse | metallisch geschirmtes Gehäuse, Erdung ≥ 4 mm² Cu | DIN EN 60079-14 |
| Leitung SPD → Gerät | im beidseitig geerdeten Metallrohr oder geschirmt mit höchstens 1 m | DIN EN 60079-14 |
| Erdfreiheit | Messumformer ≥ 500 V AC gegen Erde = erdfrei, darf durch das SPD nicht aufgehoben werden; < 500 V AC = geerdet: U_p (Ader–PG) bei 10 kA unter der Isolationsfestigkeit, z. B. ≤ 35 V | DIN EN 60079-11 |
| Zündschutzart | SPD mit ia-Bescheinigung auch für ib/ic; U_i ≥ U_o und I_i ≥ I_o des Speisegeräts; C_i/L_i in den Nachweis aufnehmen | DIN EN 60079-11/-25 |
| Störfestigkeit | NAMUR NE 21: 0,5 kV Ader/Ader, 1 kV Ader/Erde (IEC 61000-4-5); Prüfschärfegrad 1–4 auf den Schutzgeräten (z. B. P1) | NE 21, IEC 61000-4-5 |
| Erdungsanlage | je Gebäude/Anlagenteil eigene Erdung, alle vermascht; Masche 20 × 20 m, im Ex-Bereich und bei Elektronik 10 × 10 m | DIN 18014, DIN EN 62305, DIN VDE 0151 |
| Potentialausgleich | konsequent einschließlich Stützen, Rohre, Behälter; Anschlüsse gegen Selbstlockern sichern; zusätzlicher PA nach DIN EN 60079-14 (DIN VDE 0100-410/-540); Erdleiter am SPD ≥ 4 mm² Cu | DIN EN 60079-14 |
| Kabelschirm | beidseitig erden, wenn die Erdung vermascht ist (10 × 10 m) und ein isolierter PA-Leiter ≥ 4 mm² (besser 16 mm²) parallel zur Leitung an dieselbe PA-Schiene geführt wird | DIN EN 60079-14 |
| Warte (LPZ 1 → 2) | geschirmte Leitung beidseitig aufgelegt: am Übergang LPZ 1 → 2 kein weiteres SPD nötig | WPX035 |

*Aus DEHN WPX035 „Blitz- und Überspannungsschutz für explosionsgefährdete Bereiche“ (01/2022) mit DIN EN 62305-3/-4, DIN EN 60079-11/-14/-25, NAMUR NE 21, DIN 18014, DIN VDE 0151. Auswahl der Ableiter mit dem Rechner „Überspannungsschutz im Ex-Bereich“. Gegen gültige Normausgabe prüfen.*

### Blitzschutzzonen & Risiko

#### Blitzschutzzonen (LPZ) und Ableiter

| Zone | Gefährdung | Übergang | Energietechnik | Informationstechnik |
|---|---|---|---|---|
| LPZ 0A | direkter Einschlag, volles Blitzfeld | 0A → 1 | Typ 1 bzw. Kombi-Ableiter Typ 1+2 | Kombi-Ableiter, D1 (10/350 µs) |
| LPZ 0B | kein Direkteinschlag, volles Feld | 0B → 1 | Typ 2 | Überspannungs-Ableiter C2 |
| LPZ 1 | Teilblitzströme und Überspannungen begrenzt | 1 → 2 | Typ 2 | Überspannungs-Ableiter C2 |
| LPZ 2 | weiter gedämpft (geschirmte Räume, Schränke) | 2 → 3 | Typ 3 | Feinschutz C1/C2 |
| LPZ 3 | Gerät/Baugruppe | – | – | – |

*IEC 62305-4 / DIN EN 62305-4 (VDE 0185-305-4), Darstellung nach DEHN Blitzplaner. An jeder Zonengrenze werden alle Leitungen in den Potentialausgleich einbezogen – direkt oder über SPD.*

#### SPD-Typen Energietechnik

| Typ | Prüfung | Einsatz | Typische Werte | Beispiele DEHN |
|---|---|---|---|---|
| Typ 1 (Blitzstrom-Ableiter) | I_imp 10/350 µs | Einspeisung bei LPS/Freileitung (LPZ 0A → 1) | 12,5–25 kA je Pfad, U_p ≤ 1,5–2,5 kV | DEHNbloc, DEHNgap, DEHNvenCI |
| Typ 1+2 (Kombi-Ableiter) | I_imp und I_n | Hauptverteilung, Zählerschrank | U_p ≤ 1,5 kV, I_imp 12,5–25 kA je Pfad | DEHNventil, DEHNshield |
| Typ 2 (Überspannungs-Ableiter) | I_n 8/20 µs | Haupt- und Unterverteilung (LPZ 0B → 1, 1 → 2) | I_n 20 kA, I_max 40 kA, U_p ≤ 1,5 kV | DEHNguard M/MP/S |
| Typ 3 (Geräteschutz) | Kombiwelle 1,2/50 µs, 8/20 µs | vor dem Endgerät (LPZ 2 → 3) | U_OC 6 kV, I_L 16–25 A | DEHNrail, DEHNflex, DEHNsafe, DEHNprotector |

*IEC 61643-11 / DIN EN 61643-11 (Typ = Klasse I/II/III bzw. T1/T2/T3). Energetische Koordination der Stufen nach Herstellerangabe; Produktbeispiele DEHN (Kurzkatalog Elektrohandwerk 08/2023).*

#### Ableitvermögen nach Blitzschutzklasse

| Anwendung | Schaltung / Leitung | Ableitvermögen | Quelle |
|---|---|---|---|
| Energietechnik, LPL I | 4+0: L–PE und N–PE | je 25 kA (10/350 µs) | VdS 2833 (WPX050 Tab. 2) |
| Energietechnik, LPL I | 3+1: L–N / N–PE | 25 kA / 100 kA (10/350 µs) | VdS 2833 (WPX050 Tab. 2) |
| Informationstechnik, LPL I | < 3 Doppeladern | 2,5 kA je Ader (10/350 µs) | VdS 2833 (WPX050 Tab. 3) |
| Informationstechnik, LPL I | ≥ 3 Doppeladern | 1 kA je Ader | VdS 2833 (WPX050 Tab. 3) |
| Informationstechnik, LPL I | ≥ 10 Doppeladern | 0,5 kA je Ader | VdS 2833 (WPX050 Tab. 3) |
| Typ 2 ohne Trennungsabstand, LPL I | Energie / Informationstechnik | I_n 20 kA je Pol / 10 kA je Ader (8/20 µs) | WPX050 Tab. 1 |
| Typ 2 ohne Trennungsabstand, LPL II | Energie / Informationstechnik | I_n 15 kA je Pol / 10 kA je Ader | WPX050 Tab. 1 |
| Typ 2 ohne Trennungsabstand, LPL III/IV | Energie / Informationstechnik | I_n 10 kA je Pol / 10 kA je Ader | WPX050 Tab. 1 |
| Ex-Bereich, eigensicherer Kreis | jede Ader | 10 Impulse à 10 kA (8/20 µs) ohne Defekt | DIN EN 60079-14 (WPX035) |

*Nach DEHN WPX050 Tabelle 1–3 (VdS 2833) und WPX035. LPL I = höchste Blitzschutzklasse (Rechenzentren). Für andere Klassen I_imp mit dem Rechner „Überspannungsschutz 230/400 V“ (Stromaufteilung) bestimmen. Gegen gültige Ausgabe prüfen.*

#### Blitzschutzklassen: Blitzstrom und Fangeinrichtung

| Klasse | I_max (10/350 µs) | Ladung Q_kurz | W/R | I_min | Blitzkugel | Maschenweite | Abstand Ableitungen |
|---|---|---|---|---|---|---|---|
| I | 200 kA | 100 C | 10 MJ/Ω | 3 kA | 20 m | 5 × 5 m | 10 m |
| II | 150 kA | 75 C | 5,6 MJ/Ω | 5 kA | 30 m | 10 × 10 m | 10 m |
| III | 100 kA | 50 C | 2,5 MJ/Ω | 10 kA | 45 m | 15 × 15 m | 15 m |
| IV | 100 kA | 50 C | 2,5 MJ/Ω | 16 kA | 60 m | 20 × 20 m | 20 m |

*IEC 62305-1 Tabelle 3/4 und IEC 62305-3 Tabelle 2/4 (DIN EN 62305), typischer Abstand der Ableitungen nach DEHN WPX035 Tabelle 1 (im Ex-Bereich zusätzlich je 10 m Umfang eine Ableitung, mindestens 2). Höchstwerte bestimmen die Bemessung der Ableiter, Mindestwerte den Fangradius. Gegen gültige Ausgabe prüfen.*

#### Induzierte Spannung in Installationsschleifen

*U = M · Δi/Δt einer Leiterschleife neben der Ableitung (LEMP) – Beispiel DEHN WPX050: 720 kV* — `lemp_loop`

```text
Induzierte Rechteckspannung in einer offenen oder geschlossenen Leiterschleife (z. B. Alarm- oder Versorgungsleitung) neben einer blitzstromführenden Ableitung: U = M · Δi/Δt (DEHN WPX050 Bild 14/15).
M = Gegeninduktivität der Schleife (aus dem DEHN-Diagramm für quadratische Schleifen a × a im Abstand s; Beispiel a = 10 m, s = 3 m → M₂ ≈ 4,8 µH). Δi/Δt = Steilheit des Blitzstroms nach IEC 62305-1 Tabelle 3: erster Stoß 20/15/10 kA/µs, Folgestoß 200/150/100 kA/µs für Blitzschutzklasse I/II/III–IV.
Beispiel WPX050: 4,8 µH · 150 kA/µs = 720 kV. Abhilfe: Schleifen klein halten (Hin- und Rückleiter gemeinsam verlegen), Abstand zu Ableitungen, Schirmung (beidseitig aufgelegt) und SPD an den Schleifenenden.
```

**Eingaben:** Gegeninduktivität M der Schleife [µH], Blitzschutzklasse, Blitzstrom-Anteil, Eigene Steilheit Δi/Δt [kA/µs]

**Ergebnisse:** Induzierte Spannung U [kV], Steilheit Δi/Δt [kA/µs]

#### Risikoanalyse für Überspannungen atmosphärischen Ursprungs

*Bewertung nach IEC 60364-4-44, 443.5 (CRL)* — `surge_risk`

```text
IEC 60364-4-44:2007+A1:2015 Abschnitt 443 (DIN VDE 0100-443:2016-10):
Schutz gegen transiente Überspannungen ist immer vorzusehen, wenn Überspannungen Folgen haben für Menschenleben (z. B. Sicherheitsdienste, Medizin), öffentliche Einrichtungen und Kulturgüter, Gewerbe/Industrie oder eine große Anzahl von Personen (443.4). Sonst Risikobewertung:
L_P = 2 · L_PAL + L_PCL + 0,4 · L_PAH + 0,2 · L_PCH (km; NS-Freileitung, NS-Kabel, HS-Freileitung, HS-Kabel ab dem Gebäude, zusammen höchstens 1 km)    CRL = f_env / (L_P · N_g)
f_env = 85 (ländlich/vorstädtisch), 850 (städtisch). CRL < 1000 → Überspannungsschutz erforderlich.
N_g = Blitzdichte (Erdblitze je km² und Jahr), z. B. aus Blitzdichtekarten (Deutschland meist 0,5–4).
Gegen die gültige Ausgabe (DE: DIN VDE 0100-443/-534, CH: NIN 4.4.3) prüfen.
```

**Eingaben:** Folgen für Leben, öffentliche Dienste, Gewerbe oder viele Personen?, Umgebung, Blitzdichte N_g [1/(km²·a)], NS-Freileitung L_PAL [km], NS-Kabel L_PCL [km], HS-Freileitung L_PAH [km], HS-Kabel L_PCH [km]

**Ergebnisse:** Ergebnis, Bewertungslänge L_P [km], Berechneter Risikopegel CRL

## Explosionsschutz (ATEX)

### Explosionsschutz: Temperaturklasse & Kennzeichnung

*Zone → Kategorie/EPL, Stoff → T-Klasse & Gruppe, Staub → T_max, Geräteprüfung* — `ex_selection`

```text
Gas/Dampf: Die maximale Oberflächentemperatur des Geräts muss unter der Zündtemperatur liegen.
Temperaturklassen (IEC 60079-0): T1 450 °C · T2 300 °C · T3 200 °C · T4 135 °C · T5 100 °C · T6 85 °C.
Explosionsgruppe (IEC 60079-20-1): IIA ⊂ IIB ⊂ IIC – ein IIC-Gerät darf auch für IIB/IIA eingesetzt werden.
Staub (IEC 60079-14): T_max ≤ 2/3 · T_Wolke und T_max ≤ T_5mm − 75 K (Schicht bis 5 mm).
Zone 0/1/2 → Kategorie 1G/2G/3G bzw. EPL Ga/Gb/Gc; Zone 20/21/22 → 1D/2D/3D bzw. Da/Db/Dc (ATEX 2014/34/EU, 1999/92/EG). Stoffwerte sind Richtwerte (Herstellerunterlagen Schmersal/Festo, GESTIS) – maßgeblich sind Sicherheitsdatenblatt, IEC 60079-20-1 und das Explosionsschutzdokument.
```

**Eingaben:** Zone, Stoff (Gas/Dampf), Zündtemperatur (eigener Stoff) [°C], Explosionsgruppe (eigener Stoff), Staubgruppe, Zündtemperatur Staubwolke [°C], Glimmtemperatur 5-mm-Schicht [°C], Staubschichtdicke [mm], Gerät: Temperaturklasse, Gerät: Explosionsgruppe, Gerät: max. Oberflächentemperatur [°C]

**Ergebnisse:** Gerätekategorie (EPL), Auftreten explosionsfähiger Atmosphäre, Zündtemperatur [°C], Erforderliche Temperaturklasse, Zulässige Oberflächentemperatur [°C], Erforderliche Gruppe, Kennzeichnung (Beispiel), Geeignete Zündschutzarten

### Ex-Zonen, Kategorien, EPL

| Zone | Atmosphäre | Auftreten | Kategorie | EPL | Zündschutzarten |
|---|---|---|---|---|---|
| Zone 0 | Gas/Dampf | ständig, über lange Zeiträume oder häufig | 1G | Ga | ia, ma (oder zwei unabhängige Gb-Zündschutzarten) |
| Zone 1 | Gas/Dampf | gelegentlich im Normalbetrieb | 2G | Gb | db, eb, ib, mb, pxb, pyb, ob, qb – und alle Ga-Geräte |
| Zone 2 | Gas/Dampf | im Normalbetrieb nicht, wenn doch nur kurzzeitig | 3G | Gc | ec (früher nA), ic, mc, pzc, oc, nR – und alle Ga/Gb-Geräte |
| Zone 20 | Staub | Staubwolke ständig, langzeitig oder häufig | 1D | Da | ta, ia, ma |
| Zone 21 | Staub | Staubwolke gelegentlich im Normalbetrieb | 2D | Db | tb, ib, mb, pb – und alle Da-Geräte |
| Zone 22 | Staub | Staubwolke normal nicht, wenn doch nur kurzzeitig | 3D | Dc | tc, ic, mc, pc – und alle Da/Db-Geräte |

*Quellen: IEC/EN 60079-0, -14, -20-1, ATEX 2014/34/EU und 1999/92/EG, Herstellerunterlagen (Schmersal, Festo). Gegen gültige Ausgabe und Explosionsschutzdokument prüfen.*

### Ex-Gruppen (Gas und Staub)

| Gruppe | Bereich | MESG (Grenzspaltweite) | MIC-Verhältnis | Beispiel |
|---|---|---|---|---|
| I | Bergbau (Grubengas) | – | – | Methan |
| IIA | Gas – übrige Bereiche | > 0,9 mm | > 0,8 | Propan |
| IIB | Gas | 0,5 … 0,9 mm | 0,45 … 0,8 | Ethylen |
| IIC | Gas | < 0,5 mm | < 0,45 | Wasserstoff, Acetylen |
| IIIA | Staub | – | – | brennbare Flusen |
| IIIB | Staub | – | – | nicht leitfähiger Staub (Holz, Mehl) |
| IIIC | Staub | – | – | leitfähiger Staub (Metall, Kohlenstoff) |

*Quellen: IEC/EN 60079-0, -14, -20-1, ATEX 2014/34/EU und 1999/92/EG, Herstellerunterlagen (Schmersal, Festo). Gegen gültige Ausgabe und Explosionsschutzdokument prüfen.*

### Ex-Temperaturklassen T1–T6

| Klasse | Max. Oberflächentemperatur | Zündtemperatur des Stoffs | Beispiele |
|---|---|---|---|
| T1 | 450 °C | > 450 °C | Methan, Wasserstoff, Ammoniak, Propan, Aceton |
| T2 | 300 °C | > 300 … 450 °C | Ethylen, Ethanol, Methanol, n-Butan, Acetylen |
| T3 | 200 °C | > 200 … 300 °C | Diesel, Cyclohexan, Schwefelwasserstoff |
| T4 | 135 °C | > 135 … 200 °C | Diethylether, Acetaldehyd |
| T5 | 100 °C | > 100 … 135 °C | (selten) |
| T6 | 85 °C | > 85 … 100 °C | Schwefelkohlenstoff |

*Quellen: IEC/EN 60079-0, -14, -20-1, ATEX 2014/34/EU und 1999/92/EG, Herstellerunterlagen (Schmersal, Festo). Gegen gültige Ausgabe und Explosionsschutzdokument prüfen.*

### Zündschutzarten

| Kürzel | Zündschutzart | Prinzip | Schutzniveau → Zone | Norm |
|---|---|---|---|---|
| d | Druckfeste Kapselung | Explosion bleibt im Gehäuse | db → 1, dc → 2 | IEC 60079-1 |
| e | Erhöhte Sicherheit | keine Funken/heißen Stellen | eb → 1, ec → 2 | IEC 60079-7 |
| i | Eigensicherheit | Energie begrenzt | ia → 0, ib → 1, ic → 2 | IEC 60079-11 |
| p | Überdruckkapselung | Schutzgas mit Überdruck | pxb/pyb → 1, pzc → 2 | IEC 60079-2 |
| m | Vergusskapselung | in Masse eingegossen | ma → 0, mb → 1, mc → 2 | IEC 60079-18 |
| o | Flüssigkeitskapselung | unter Schutzflüssigkeit | ob → 1, oc → 2 | IEC 60079-6 |
| q | Sandkapselung | mit Sand gefüllt | qb → 1 | IEC 60079-5 |
| nR | Schwadenhemmendes Gehäuse | dichtes Gehäuse | nR → 2 | IEC 60079-15 |
| t | Schutz durch Gehäuse (Staub) | staubdicht, Temperatur begrenzt | ta → 20, tb → 21, tc → 22 | IEC 60079-31 |
| op | Optische Strahlung | is/pr/sh | Ga/Gb/Gc | IEC 60079-28 |

*Quellen: IEC/EN 60079-0, -14, -20-1, ATEX 2014/34/EU und 1999/92/EG, Herstellerunterlagen (Schmersal, Festo). Gegen gültige Ausgabe und Explosionsschutzdokument prüfen.*

### Zündtemperaturen von Gasen und Dämpfen

| Stoff | Zündtemperatur | Temperaturklasse | Explosionsgruppe |
|---|---|---|---|
| Methan (Erdgas) | 595 °C | T1 | IIA |
| Wasserstoff | 560 °C | T1 | IIC |
| Ammoniak | 630 °C | T1 | IIA |
| Propan | 470 °C | T1 | IIA |
| Aceton | 535 °C | T1 | IIA |
| Ethylen | 425 °C | T2 | IIB |
| Methanol | 440 °C | T2 | IIA |
| Ethanol | 400 °C | T2 | IIB |
| 2-Propanol (Isopropanol) | 425 °C | T2 | IIA |
| n-Butan | 365 °C | T2 | IIA |
| Acetylen | 305 °C | T2 | IIC |
| Cyclohexan | 259 °C | T3 | IIA |
| Dieselkraftstoff | ≥ 220 °C | T3 | IIA |
| Schwefelwasserstoff | 270 °C | T3 | IIB |
| Diethylether | 170 °C | T4 | IIB |
| Acetaldehyd | 155 °C | T4 | IIA |
| Schwefelkohlenstoff | 95 °C | T6 | IIC |

*Richtwerte (Schmersal-Ex-Poster, Festo-Handout Explosionsschutz, GESTIS); je nach Quelle abweichend. Maßgeblich sind Sicherheitsdatenblatt und IEC 60079-20-1.*

## R&I-Kennzeichnung

### R&I-Kennbuchstaben-Finder

*Name eingeben (z. B. Temperatur Zuluft, Gas Methan, Leckage) → ISA, EN 62424, VDI 3814* — `pid_tags`

```text
Erster Buchstabe = Messgröße (T Temperatur, P Druck, F Durchfluss, L Stand, M Feuchte, A/Q Analyse …), Folgebuchstaben = Funktion: E Element, T Transmitter, I Anzeige, C Regelung, S Schalter, A Alarm, H/L hoch/tief, Q Summe. Tag = Buchstaben + Kreisnummer, z. B. TT-101.
Für die GA zusätzlich Bauteil-Kürzel und Benutzeradresse nach VDI 3814-4.1 (Kürzel + Nr. _ Funktion _ Nr.).
Kennbuchstaben nach ANSI/ISA-5.1-2009 und DIN EN 62424 (Nachfolgerin von DIN 19227-1); GA-Kürzel nach VDI 3814 Blatt 4.1 Tab. 3/4 (Beispiele), Klassen nach IEC 81346-2 wie in VDI 3814-4.1. Projektspezifische Festlegungen haben Vorrang.
```

**Eingaben:** Messstelle / Gerät (Name), Funktion, Ort / Darstellung, Kreisnummer

**Ergebnisse:** Erkannt, Kennbuchstaben ISA-5.1, Beispiel-Tag (ISA), Bedeutung, Kennbuchstaben EN 62424 / DIN 19227, GA-Kürzel VDI 3814-4.1, Benutzeradresse (Bauteil_Funktion), Klasse IEC 81346-2, Signal, PI-Symbol, Weitere Treffer

### Sensoren & Wächter – Kennbuchstaben

| Messstelle / Gerät | Bereich | ISA-5.1 | EN 62424 | VDI 3814 | IEC 81346 |
|---|---|---|---|---|---|
| Temperatur | MSR | TT | TI | TZU, TAB, TAU, TRU, TVL, TRL | BT |
| Zulufttemperatur | HLKS | TT | TI | TZU | BT |
| Ablufttemperatur | HLKS | TT | TI | TAB | BT |
| Außentemperatur | HLKS | TT | TI | TAU | BT |
| Raumtemperatur | HLKS | TT | TI | TRU | BT |
| Fortlufttemperatur | HLKS | TT | TI | TFO | BT |
| Mischlufttemperatur | HLKS | TT | TI | TMI | BT |
| Vorlauftemperatur | HLKS | TT | TI | TVL | BT |
| Rücklauftemperatur | HLKS | TT | TI | TRL | BT |
| Warmwassertemperatur | HLKS | TT | TI | TWW | BT |
| Speichertemperatur | HLKS | TT | TI | TSP | BT |
| Abgastemperatur | HLKS | TT | TI | TAG | BT |
| Temperatur nach Kühler | HLKS | TT | TI | TNK | BT |
| Frostschutzwächter | HLKS | TSL | TSL | FSW | BT |
| Sicherheitstemperaturbegrenzer (STB) | HLKS | TSHH | TZH | STB | BT |
| Sicherheitstemperaturwächter (STW) | HLKS | TSHH | TZH | STW | BT |
| Thermostat / Temperaturwächter | HLKS | TSH | TSH | THM | BT |
| Taupunkttemperatur | HLKS | TT | TI | TTP | BT |
| Taupunktwächter | HLKS | MSH | MSH | TPW | BT |
| Kondensationswächter | HLKS | MSH | MSH | MKW | BM |
| Feuchte (relativ) | HLKS | MT | MI | MZU, MAB, MAU, MRU | BM |
| Zuluftfeuchte | HLKS | MT | MI | MZU | BM |
| Abluftfeuchte | HLKS | MT | MI | MAB | BM |
| Außenfeuchte | HLKS | MT | MI | MAU | BM |
| Raumfeuchte | HLKS | MT | MI | MRU | BM |
| Absolute Feuchte (Wassergehalt) | HLKS | MT | MI | XZU, XAB, XAU, XRU | BX |
| Hygrostat / Feuchtewächter | HLKS | MSH | MSH | – | BM |
| Druck | MSR | PT | PI | PZU, PAB, PVI | BP |
| Differenzdruck | MSR | PDT | PDI | PDI | BP |
| Raumdruck (Reinraum) | HLKS | PDT | PDI | PRU | BP |
| Zuluftdruck (Kanaldruck) | HLKS | PT | PI | PZU | BP |
| Wasserdruck / Anlagendruck | HLKS | PT | PI | PVI | BP |
| Druckwächter / Pressostat | MSR | PSH | PSH | DRW | BP |
| Sicherheitsdruckbegrenzer (SDB) | HLKS | PSHH | PZH | SDB | BP |
| Filterüberwachung (Differenzdruckwächter) | HLKS | PDSH | PDSH | LFI | BP |
| Volumenstrom Luft | HLKS | FT | FI | FZU, FAB, FAU, FFO | BS |
| Durchfluss Wasser | MSR | FT | FI | FKW, FWW | BS |
| Strömungswächter (Luft) | HLKS | FSL | FSL | SSW | BS |
| Wasserströmungswächter | HLKS | FSL | FSL | WSW | BL |
| Wasserzähler | HLKS | FQI | FQI | WAZ | BG |
| Gaszähler | HLKS | FQI | FQI | EGZ | BF |
| Wärmemengenzähler | HLKS | JQI | – | WMZ | BG |
| Kältemengenzähler | HLKS | JQI | – | KMZ | BG |
| Elektrozähler | Elektro | JQI | EQI | ELZ | BC |
| Füllstand | MSR | LT | LI | LSP | BL |
| Niveauschalter max (Überlauf) | MSR | LSH | LSH | UBL | BL |
| Niveauschalter min (Trockenlauf) | MSR | LSL | LSL | – | BL |
| Wassermangelsicherung | HLKS | LSL | LSL | WMS | BL |
| Leckageüberwachung | HLKS | LSH | LSH | LCK | BM |
| CO₂ (Luftqualität) | HLKS | AT (CO₂) | QI | COM | BQ |
| Luftqualität (VOC/Mischgas) | HLKS | AT (VOC) | QI | LQF | BQ |
| Gaswarnung Methan (Erdgas) | Prozess | AT (CH₄) | QI | – | BQ |
| Gaswarnung Wasserstoff | Prozess | AT (H₂) | QI | – | BQ |
| Gaswarnung Kohlenmonoxid | HLKS | AT (CO) | QI | – | BQ |
| Gaswarnung Ammoniak | Prozess | AT (NH₃) | QI | – | BQ |
| Kältemittelwarnung | HLKS | AT (Rxxx) | QI | – | BQ |
| Sauerstoffüberwachung (O₂-Mangel) | Prozess | AT (O₂) | QI | – | BQ |
| Gaswarnanlage (allgemein) | Prozess | AT (Gas) | QI | – | BQ |
| Rauchmelder (Kanal) | HLKS | ASH (Rauch) | QSH | RME | BR |
| Flammenwächter | Prozess | BSL | – | – | – |
| pH-Wert | Prozess | AT (pH) | QI | – | BQ |
| Leitfähigkeit | Prozess | AT (µS/cm) | QI | – | BQ |
| Drehzahl | MSR | ST | SI | SZU, SAB, SPP | BS |
| Schwingung / Vibration | Prozess | VT | GI | – | – |
| Stellungsrückmeldung (Ventil/Klappe) | MSR | ZT | GI | – | BG |
| Endlagenschalter (AUF/ZU) | MSR | ZSO / ZSC | GSH / GSL | – | BG |
| Gewicht / Wägezelle | Prozess | WT | WI | – | – |
| Strom (elektrisch) | Elektro | IT | EI | – | – |
| Spannung | Elektro | ET | EI | – | – |
| Wirkleistung (elektrisch) | Elektro | JT | EI | – | – |
| Beleuchtungsstärke / Helligkeit | HLKS | RT | RI | EAU, ERU | – |
| Windgeschwindigkeit | HLKS | ST (Wind) | SI | WDG | BF |
| Regenmelder | HLKS | MSH | MSH | RGM | BM |
| Sonneneinstrahlung (Globalstrahlung) | HLKS | RT | RI | SES | BR |
| Präsenzmelder / Bewegungsmelder | HLKS | YS | XS | – | – |
| Not-Aus-Taster | MSR | HS | HS | NOT | SF |
| Anlagenschalter (Hand-0-Auto) | HLKS | HS | HS | ANL | SF |
| Reparaturschalter | HLKS | HS | HS | – | SF |

*Typische Funktion je Gerät; andere Funktion (Fühler, Anzeige, Regler, Max/Min) im Kennbuchstaben-Finder wählen.
Kennbuchstaben nach ANSI/ISA-5.1-2009 und DIN EN 62424 (Nachfolgerin von DIN 19227-1); GA-Kürzel nach VDI 3814 Blatt 4.1 Tab. 3/4 (Beispiele), Klassen nach IEC 81346-2 wie in VDI 3814-4.1. Projektspezifische Festlegungen haben Vorrang.*

### Kennbuchstaben ISA-5.1

| Buchstabe | Messgröße (1. Buchstabe) | Ergänzung | Anzeige-/passive Funktion | Ausgabefunktion | Funktionszusatz |
|---|---|---|---|---|---|
| A | Analyse | – | Alarm | – | – |
| B | Brenner, Verbrennung | – | frei | frei | frei |
| C | frei (oft Leitfähigkeit) | – | – | Regelung | Schließen |
| D | frei (oft Dichte) | Differenz | – | – | Abweichung |
| E | Spannung | – | Messelement, Sensor | – | – |
| F | Durchfluss | Verhältnis | – | – | – |
| G | frei | – | Schauglas, Anzeiger | – | – |
| H | Hand | – | – | – | Hoch |
| I | Strom | – | Anzeige | – | – |
| J | Leistung | – | Abfrage | – | – |
| K | Zeit, Zeitplan | Änderungsgeschwindigkeit | – | Bedienstation | – |
| L | Stand, Niveau | – | Leuchte | – | Tief |
| M | frei (oft Feuchte) | – | – | – | Mittel |
| N | frei | – | frei | frei | frei |
| O | frei | – | Blende, Drossel | – | Öffnen |
| P | Druck | – | Messanschluss | – | – |
| Q | Menge | Integral, Summe | Integral, Summe | – | – |
| R | Strahlung | – | Registrierung | – | Lauf |
| S | Drehzahl, Frequenz | Sicherheit | – | Schalter | Stopp |
| T | Temperatur | – | – | Transmitter | – |
| U | Mehrere Größen | – | Mehrfachfunktion | Mehrfachfunktion | – |
| V | Schwingung | – | – | Ventil, Klappe | – |
| W | Gewicht, Kraft | – | Tauchhülse, Sonde | – | – |
| X | nicht klassifiziert | X-Achse | nicht klassifiziert | nicht klassifiziert | – |
| Y | Ereignis, Zustand, Anwesenheit | Y-Achse | – | Hilfsgerät (Relais, Umformer) | – |
| Z | Position, Abmessung | Z-Achse | – | Antrieb, Stellglied | – |

*ANSI/ISA-5.1-2009 Tab. 4.1 (sinngemäß). „frei“ = vom Anwender festzulegen. Beispiel: TIC-101 = Temperatur, Anzeige, Regelung, Kreis 101.*

### Kennbuchstaben EN 62424 / DIN 19227

| Buchstabe | Als Erstbuchstabe | Als Ergänzung/Folgebuchstabe |
|---|---|---|
| A | – | Störungsmeldung (Alarm) |
| C | – | selbsttätige Regelung |
| D | Dichte | Differenz |
| E | elektrische Größen | – |
| F | Durchfluss | Verhältnis |
| G | Abstand, Länge, Stellung, Amplitude | – |
| H | Handeingabe | oberer Grenzwert |
| I | – | Anzeige |
| K | Zeit | – |
| L | Stand (auch Trennschicht) | unterer Grenzwert |
| M | Feuchte | – |
| O | – | Sichtzeichen, Ja/Nein-Anzeige |
| P | Druck | – |
| Q | Stoffeigenschaft, Analyse | Integral, Summe |
| R | Strahlungsgrößen | Registrierung |
| S | Geschwindigkeit, Drehzahl, Frequenz | Schaltung, Ablaufsteuerung |
| T | Temperatur | Messumformer (Gerätebuchstabe) |
| U | zusammengesetzte Größen | zusammengefasste Antriebsfunktionen |
| V | Viskosität | Stellgerät (Gerätebuchstabe) |
| W | Gewichtskraft, Masse | – |
| X | sonstige Größen | – |
| Y | – | Rechenfunktion (Gerätebuchstabe) |
| Z | – | Noteingriff, Schutz durch Auslösung |

*Erstbuchstabe = Messgröße, Ergänzung D/F/Q, Folgebuchstaben in der Reihenfolge I R C S Z A O, Grenzwerte H/L (früher +/−). Beispiel: TIC = Temperatur, Anzeige, Regelung; PSZ = Druck, Schaltung, Schutzauslösung. DIN 19227-1 / DIN EN 62424, gegen gültige Ausgabe prüfen.*

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

*PUE aus Leistung oder Jahresverbrauch, DCiE, Kosten, EnEfG* — `dc_pue`

```text
PUE = E_gesamt / E_IT (ISO/IEC 30134-2: Jahresenergie, gemessen am Übergabepunkt bzw. am Ausgang USV/PDU)
Aus Leistung (Momentanwert): PUE = P_gesamt / P_IT, Jahresenergie = P · 8760 h    DCiE = 1 / PUE · 100 %
Overhead = Gesamt − IT (Kühlung, USV-Verluste, Licht …)
EnEfG (2023) §11: Neubau mit Betriebsbeginn ab 1.7.2026 PUE ≤ 1,2; Bestand PUE ≤ 1,5 ab 1.7.2027 und ≤ 1,3 ab 1.7.2030; Neubau ERF ≥ 10 % (ab 1.7.2026), 15 % (2027), 20 % (2028). Kabinettsentwurf EnEfG-Novelle vom 24.6.2026: Bestand 1,6 / 1,4, Neubau 1,2 (4 Jahre Frist), Geltung ab 500 kW IT-Leistung – im Bundestag noch nicht verabschiedet (Stand Juli 2026). Gültige Fassung prüfen.
```

**Eingaben:** Grundlage, Gesamtleistung RZ [kW], IT-Leistung [kW], Gesamtverbrauch RZ pro Jahr [MWh], IT-Verbrauch pro Jahr [MWh], Strompreis pro kWh [€ / CHF], Rechenzentrum, EnEfG-Fassung (nur DE)

**Ergebnisse:** PUE, DCiE [%], Overhead-Leistung (Mittel) [kW], Energie pro Jahr gesamt [MWh], Kosten Overhead pro Jahr [€], Einsparung bei PUE 1,3 [MWh/a]

### PUE-Bilanz aus Verbrauchern

*PUE, pPUE, Jahresverbrauch, Kosten, CO₂, Abwärme – aus IT, USV, Kühlung* — `dc_pue_balance`

```text
USV-Verlust = P_IT · (1/η_USV − 1)    Verteilverluste = (P_IT + USV-Verlust) · v (Trafo, Leitungen vor der USV)
Kühllast Q = P_IT + USV-Verlust + Sonstiges    P_Kühlung = Q · (1 − Anteil freie Kühlung) / EER + Q · Hilfsenergie
P_gesamt = P_IT + USV- + Verteilverluste + P_Kühlung + Sonstiges    PUE = P_gesamt / P_IT
pPUE Kühlung = (P_IT + P_Kühlung) / P_IT    pPUE Strom = (P_IT + Verluste) / P_IT (ISO/IEC 30134-2 Anh.)
ERF = genutzte Abwärme / Gesamtenergie (ISO/IEC 30134-6). Jahreswerte mit 8760 h und mittleren Leistungen.
EER und Anteil freie Kühlung als Jahresmittel (Klima, Kaltwassertemperatur) beim Planer/Hersteller erfragen.
EnEfG (2023) §11: Neubau mit Betriebsbeginn ab 1.7.2026 PUE ≤ 1,2; Bestand PUE ≤ 1,5 ab 1.7.2027 und ≤ 1,3 ab 1.7.2030; Neubau ERF ≥ 10 % (ab 1.7.2026), 15 % (2027), 20 % (2028). Kabinettsentwurf EnEfG-Novelle vom 24.6.2026: Bestand 1,6 / 1,4, Neubau 1,2 (4 Jahre Frist), Geltung ab 500 kW IT-Leistung – im Bundestag noch nicht verabschiedet (Stand Juli 2026). Gültige Fassung prüfen.
```

**Eingaben:** IT-Leistung (Jahresmittel) [kW], USV-Wirkungsgrad [%], Verluste Trafo/Verteilung [%], Beleuchtung, Sicherheit, Sonstiges [kW], EER Kälteerzeugung (Jahresmittel), Anteil freie Kühlung [%], Hilfsenergie Ventilatoren/Pumpen/Rückkühler (bezogen auf Kühllast) [%], Strompreis pro kWh [€ / CHF], CO₂-Emissionsfaktor Strom [kg/kWh], Genutzte Abwärme (Mittel) [kW], Rechenzentrum, EnEfG-Fassung (nur DE)

**Ergebnisse:** PUE, Gesamtleistung RZ (Mittel) [kW], pPUE Kühlung, pPUE Stromversorgung, DCiE [%], USV-Verluste [kW], Verteilverluste [kW], Kühllast [kW], Leistung Kühlung [kW], Anteil Kühlung am Gesamtverbrauch [%], Jahresverbrauch gesamt [MWh], Jahresverbrauch IT [MWh], Stromkosten pro Jahr [€], davon Overhead (nicht IT) [€], CO₂ pro Jahr [t], Abwärme verfügbar (Kühllast) [MWh/a], ERF (Energiewiederverwendung) [%]

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

## Gebäude, Brandschutz & Akustik

### Brandschutz & Fluchtwege

#### Fluchtweg: Breite & Länge (ASR A2.3)

*Mindestbreiten nach Personenzahl, maximale Fluchtweglänge* — `escape_route`

```text
ASR A2.3 (Fassung 03/2022, geändert 2024), Abschn. 5: Tab. 1 Mindestbreiten (Spalte B Türen/Durchgänge, Spalte C Hauptfluchtweg): bis 5 P. 0,80/0,90 · bis 20 P. 0,90/1,00 · bis 50 P. 0,90/1,20 · bis 100 P. 1,00/1,20 · bis 200 P. 1,05/1,20 · bis 300 P. 1,65/1,80 · bis 400 P. 2,25/2,40 m; über 200 Personen linear interpolierbar. Bei Neubau für bis 5 Personen auch an Türen 0,90 m empfohlen (Barrierefreiheit).
Hauptfluchtweglänge (Luftlinie): 35 m (normal oder erhöhte Brandgefährdung mit Löschanlage), 25 m (erhöhte Brandgefährdung ohne Löschanlage), 10 m (explosionsgefährliche Stoffe); tatsächliche Laufweglänge ≤ 1,5-fach.
Das Buch (ASR A2.3:2007) nennt noch eine Spalte: 0,875 / 1,00 / 1,20 / 1,80 / 2,40 m und 20 m für giftstoff- und explosionsgefährdete Räume. Bauordnungsrecht kann längere Rettungswege zulassen.
```

**Eingaben:** Personen im Einzugsgebiet, Raumart, Fluchtweglänge (Luftlinie) [m], Tatsächliche Laufweglänge [m], Vorhandene lichte Breite Flur [m]

**Ergebnisse:** Mindestbreite Hauptfluchtweg [m], Mindestbreite Türen/Durchgänge [m], Max. Fluchtweglänge [m], Max. Laufweglänge [m]

#### Fluchtwegbreiten ASR A2.3

| Personen | Türen/Durchgänge 2022 | Hauptfluchtweg 2022 | Fluchtweg 2007 |
|---|---|---|---|
| bis 5 | 0,80 m (Neubau 0,90) | 0,90 m | 0,875 m |
| bis 20 | 0,90 m | 1,00 m | 1,00 m |
| bis 50 | 0,90 m | 1,20 m | 1,20 m |
| bis 100 | 1,00 m | 1,20 m | 1,20 m |
| bis 200 | 1,05 m | 1,20 m | 1,20 m |
| bis 300 | 1,65 m | 1,80 m | 1,80 m |
| bis 400 | 2,25 m | 2,40 m | 2,40 m |

*ASR A2.3, Fassung 03/2022 (GMBl. 2022 S. 227, geändert 2024), Abschn. 5 Tab. 1. Die Fassung 2007 steht im Fachbuch (Ordner Brandschutz & Arbeitsstätten). Gültige Fassung bei der BAuA prüfen.*

#### Aufzugsschacht: Rauchabzug & Lüftung

*Freier Querschnitt der Schachtöffnung nach MBO bzw. VKF* — `lift_shaft`

```text
DE – MBO § 39 Abs. 3: Öffnung zur Rauchableitung ≥ 2,5 % der Fahrschachtgrundfläche, mindestens 0,10 m² (Landesbauordnung prüfen). Lüftung nach DIN EN 81-1/-2 (2010): Empfehlung 1 % der Schachtgrundfläche am oberen Ende.
CH – VKF-Brandschutzrichtlinie (Stand Blatt 2007): lichter Querschnitt 5 % des Schachtquerschnitts, max. 1600 cm²; Maschinen-/Rollenraum separat entlüftet mindestens 500 cm².
Quellen: ZVEI/VDMA-Leitfaden „Rauchableitung und Lüftung von Aufzugsschächten“ (07/2012), S. 3; IG-BSK-Arbeitsblatt D_AE_001 (03/2007). EN 81-1/-2 sind durch EN 81-20/-50 ersetzt; gültige Fassungen prüfen.
```

**Eingaben:** Schachtbreite (innen) [m], Schachttiefe (innen) [m]

**Ergebnisse:** Schachtgrundfläche [m²], Freier Querschnitt Rauchabzug [m²], Freier Querschnitt Rauchabzug [cm²], Lüftung (Empfehlung 1 %) [m²]

### Akustik

#### Nachhallzeit (Sabine) & DIN 18041

*Ist- und Soll-Nachhallzeit, nötige Absorberfläche* — `reverberation`

```text
Sabine: T = 0,163 s/m · V / A,   A = α · S (äquivalente Absorptionsfläche)
Soll-Nachhallzeit DIN 18041 (besetzter Raum, mittlere Frequenzen, Toleranz ±20 %):
A1 Musik T = 0,45·lg V + 0,07 · A2 Sprache/Vortrag T = 0,37·lg V − 0,14 · A3 Unterricht/Kommunikation T = 0,32·lg V − 0,17 · A4 Unterricht inklusiv T = 0,26·lg V − 0,14 (A4 seit DIN 18041:2016).
Quelle: BAuA-Forschungsbericht Fb 1030 „Lärm in Bildungsstätten“ (2004), Kap. 3, S. 19–20 (Tab. 3.1: 150 m³ → 0,53 s); im Buch S. 50 sind die Vorzeichen der Konstanten verdruckt. Gegen DIN 18041:2016 prüfen.
```

**Eingaben:** Raumlänge [m], Raumbreite [m], Raumhöhe [m], Nutzung (DIN 18041), Mittlerer Absorptionsgrad Raumflächen, Absorptionsgrad Absorber/Akustikdecke, Vorhandene Absorberfläche [m²]

**Ergebnisse:** Raumvolumen [m³], Nachhallzeit Ist [s], Nachhallzeit Soll [s], Toleranz obere Grenze (+20 %) [s], Äquivalente Absorptionsfläche Ist [m²], Nötige Absorptionsfläche [m²], Zusätzliche Absorberfläche [m²]

#### Schallpegel addieren & Entfernung

*Energetische Summe mehrerer Quellen, Abnahme mit der Entfernung* — `sound_levels`

```text
L_Σ = 10 · lg Σ 10^(L_i/10)   (zwei gleiche Quellen: +3 dB)
Punktquelle im Freifeld: L₂ = L₁ − 20 · lg(r₂/r₁)   (Verdopplung der Entfernung: −6 dB)
Grundlagen der Akustik; vgl. BAuA Fb 1030 „Lärm in Bildungsstätten“ (2004).
```

**Eingaben:** Pegel Quelle 1 [dB], Pegel Quelle 2 [dB], Pegel Quelle 3 [dB], Pegel Quelle 4 [dB], Abstand der Messung [m], Neuer Abstand [m]

**Ergebnisse:** Summenpegel [dB], Summenpegel im neuen Abstand [dB]

## Kommunikation & Pinbelegungen

### Netzwerk & Telefon

#### Pinout RJ45 / Ethernet

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

#### Pinbelegung Ethernet mit PoE

| Pin | T568B-Farbe | Daten (10/100) | PoE Modus A | PoE Modus B |
|---|---|---|---|---|
| 1 | weiß-orange | TX+ | DC+ / DC− | – |
| 2 | orange | TX− | DC+ / DC− | – |
| 3 | weiß-grün | RX+ | DC− / DC+ | – |
| 4 | blau | frei | – | DC+ |
| 5 | weiß-blau | frei | – | DC+ |
| 6 | grün | RX− | DC− / DC+ | – |
| 7 | weiß-braun | frei | – | DC− |
| 8 | braun | frei | – | DC− |

*IEEE 802.3af/at (PoE, PoE+): Modus A speist über die Datenpaare 1/2 und 3/6 (Phantomspeisung), Modus B über die freien Paare 4/5 und 7/8 (10/100 Mbit/s). IEEE 802.3bt (4PPoE, bis 90 W) nutzt alle vier Paare. Polarität bei Modus A je nach MDI/MDI-X. Leistung am Gerät: af 12,95 W, at 25,5 W, bt Typ 3 51 W, Typ 4 71,3 W.*

#### Pinbelegung RJ-9, 11, 14, 25, 48

| Stecker | Kontakte | Belegung |
|---|---|---|
| RJ9 (4P4C) | 4 | 1 Mikrofon, 2/3 Hörer, 4 Mikrofon (herstellerabhängig) |
| RJ11 (6P2C) | 6 | 3 Ring, 4 Tip (Leitung 1) |
| RJ14 (6P4C) | 6 | 3/4 Leitung 1, 2/5 Leitung 2 |
| RJ25 (6P6C) | 6 | 3/4 Leitung 1, 2/5 Leitung 2, 1/6 Leitung 3 |
| RJ48C (8P8C) | 8 | 1 RX Ring, 2 RX Tip, 4 TX Ring, 5 TX Tip, 3/6 Schirm |

*Registered Jacks (FCC Part 68 / TIA-968): RJ11 = 6P2C (1 Leitung), RJ14 = 6P4C (2 Leitungen), RJ25 = 6P6C (3 Leitungen), RJ9 = 4P4C (Hörerkabel), RJ48C = 8P8C für T1/E1. Kontakte von links bei Blick auf die Kontakte, Rastnase unten. Tip (a) / Ring (b) einer Leitung liegen symmetrisch zur Mitte.*

#### Farbcode optische Fasern

| Faser | Farbe TIA-598 |
|---|---|
| 1 | blau |
| 2 | orange |
| 3 | grün |
| 4 | braun |
| 5 | schiefergrau |
| 6 | weiß |
| 7 | rot |
| 8 | schwarz |
| 9 | gelb |
| 10 | violett |
| 11 | rosa |
| 12 | aqua (türkis) |

*Faser- und Bündeladerfarben nach TIA-598-D (international üblich; DIN VDE 0888 nutzt teils andere Reihenfolge: rot, grün, blau, gelb, weiß, grau, braun, violett, türkis, schwarz, orange, rosa). Ab Faser 13 Wiederholung mit Ring/Strich. Mantelfarben: OS1/OS2 gelb, OM1/OM2 orange, OM3 aqua, OM4 aqua oder violett, OM5 limettengrün. Stecker: UPC blau, APC (8°) grün, Multimode beige/aqua.*

### Feldbus & Industrie

#### RS-485: Modbus RTU / BACnet MS/TP

| Leitung | Modbus | Übliche Bezeichnungen | Funktion |
|---|---|---|---|
| D1 | B / B' | B, +, Data+, TxD+/RxD+ | nicht invertierte Leitung |
| D0 | A / A' | A, −, Data−, TxD−/RxD− | invertierte Leitung |
| Common | C / C' | GND, 0 V, SC | Bezugspotential (mitführen!) |
| Abschluss | 120 Ω | an beiden Enden | zwischen D1 und D0 |
| Schirm | – | SH | nach EMV-Konzept auf PE |

*Modbus over Serial Line V1.02: D1 = B/B' (positiv bei Ruhepegel „1“), D0 = A/A'. Achtung: Viele Hersteller bezeichnen A und B umgekehrt – im Zweifel Polarität im Ruhezustand messen (D1 > D0).*

#### Pinout RS-232 (D-Sub 9)

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

#### Pinout M12 A-codiert (Sensor)

| Pin | Farbe | Funktion | IO-Link |
|---|---|---|---|
| 1 | braun (BN) | L+ (24 V DC) | L+ |
| 2 | weiß (WH) | Ausgang 2 / Öffner | DI/DO |
| 3 | blau (BU) | L− (0 V) | L− |
| 4 | schwarz (BK) | Schaltausgang Q1 | C/Q |
| 5 | grau (GY) | herstellerabhängig / FE | – |

*IEC 61076-2-101, Aderfarben nach IEC 60947-5-2; IO-Link Port Class A.*

#### Pinbelegung IO-Link

| Pin | Port Class A | Port Class B |
|---|---|---|
| 1 | L+ (24 V) | L+ (24 V) |
| 2 | I/Q (DI/DO) | P24 (Aktor +24 V) |
| 3 | L− (0 V) | L− (0 V) |
| 4 | C/Q (IO-Link / SIO) | C/Q (IO-Link / SIO) |
| 5 | frei | N24 (Aktor 0 V) |

*IEC 61131-9 (IO-Link). M12 A-codiert. Class A: Pin 2 frei verwendbar (DI/DO), Class B: Pin 2/5 zusätzliche galvanisch getrennte Aktorversorgung. Ungeschirmte 3- oder 5-adrige Leitung, max. 20 m. Aderfarben: 1 braun, 2 weiß, 3 blau, 4 schwarz, 5 grau.*

#### KNX-Busleitung

| Ader | Funktion |
|---|---|
| rot | Bus + (KNX+) |
| schwarz | Bus − (KNX−) |
| gelb | Reserve / Zusatzspannung |
| weiß | Reserve / Zusatzspannung |

*KNX TP1, Busleitung z. B. J-Y(St)Y 2×2×0,8 bzw. KNX-zertifiziert (grüner Mantel).*

### Computer & Video

#### Pinout USB 2.0

| Pin | Typ A/B | Aderfarbe | Micro-B |
|---|---|---|---|
| 1 | VBUS +5 V | rot | VBUS +5 V |
| 2 | D− | weiß | D− |
| 3 | D+ | grün | D+ |
| 4 | GND | schwarz | ID |
| 5 | – | – | GND |

*USB 2.0 Specification.*

#### Pinbelegung Thunderbolt 3/4 (USB-C)

| Pin A | Signal | Pin B | Signal |
|---|---|---|---|
| A1 | GND | B12 | GND |
| A2 | TX1+ | B11 | RX1+ |
| A3 | TX1− | B10 | RX1− |
| A4 | VBUS | B9 | VBUS |
| A5 | CC1 | B8 | SBU2 |
| A6 | D+ | B7 | D− |
| A7 | D− | B6 | D+ |
| A8 | SBU1 | B5 | CC2 (VCONN) |
| A9 | VBUS | B4 | VBUS |
| A10 | RX2− | B3 | TX2− |
| A11 | RX2+ | B2 | TX2+ |
| A12 | GND | B1 | GND |

*Thunderbolt 3/4 nutzen die USB-C-Buchse (USB Type-C Spezifikation): je Seite ein Hochgeschwindigkeits-Sende-/Empfangspaar (TX/RX), USB 2.0 (D+/D−), CC für Erkennung und Power Delivery, SBU für Seitenbandsignale. Thunderbolt 1/2 nutzten den Mini-DisplayPort-Stecker.*

#### Pinbelegung Apple Lightning

| Pin | Signal | Beschreibung |
|---|---|---|
| 1 | GND | Masse |
| 2 | L0p | Datenpaar 0 + |
| 3 | L0n | Datenpaar 0 − |
| 4 | ID0 | Kennung/Steuerung 0 |
| 5 | PWR | Versorgung (Ladegerät/Akku) |
| 6 | L1n | Datenpaar 1 − |
| 7 | L1p | Datenpaar 1 + |
| 8 | ID1 | Kennung/Steuerung 1 |

*Apple Lightning (2012–2023): Der Stecker ist drehbar, die Buchse ordnet die Kontakte über einen Chip im Stecker zu. Belegung einer Steckerseite (nach Wikipedia „Lightning (connector)“).*

#### Pinbelegung Apple-Dock (30-polig)

| Pin | Signal | Beschreibung |
|---|---|---|
| 1, 2 | GND | Masse (Audio/Video) |
| 3 | Line out R | Audio rechts |
| 4 | Line out L | Audio links |
| 5 | Line in R | Audio rechts |
| 6 | Line in L | Audio links |
| 8 | Video aus | Composite |
| 9 | S-Video C | Chrominanz |
| 10 | S-Video Y | Luminanz |
| 11 | GND | Masse seriell |
| 12 | TxD | seriell senden |
| 13 | RxD | seriell empfangen |
| 15, 16 | GND | Masse (USB) |
| 18 | +3,3 V | Versorgung |
| 19, 20 | +12 V | FireWire-Versorgung |
| 21 | ACC ID | Zubehörkennung (Widerstand) |
| 22 | TPA− | FireWire |
| 23 | +5 V | USB-Versorgung |
| 24 | TPA+ | FireWire |
| 25 | USB D− | USB |
| 26 | TPB− | FireWire |
| 27 | USB D+ | USB |
| 28 | TPB+ | FireWire |
| 29, 30 | GND | Masse (FireWire) |
| 7, 14, 17 | – | nicht belegt |

*Apple 30-Pin Dock Connector (2003–2012), Belegung nach The Apple Wiki. Video und FireWire nur bei bestimmten Modellen.*

#### Pinout HDMI Typ A

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

#### Pinbelegung DisplayPort

| Pin | Signal | Beschreibung |
|---|---|---|
| 1 | ML_Lane 0+ | – |
| 2 | GND | – |
| 3 | ML_Lane 0− | – |
| 4 | ML_Lane 1+ | – |
| 5 | GND | – |
| 6 | ML_Lane 1− | – |
| 7 | ML_Lane 2+ | – |
| 8 | GND | – |
| 9 | ML_Lane 2− | – |
| 10 | ML_Lane 3+ | – |
| 11 | GND | – |
| 12 | ML_Lane 3− | – |
| 13 | CONFIG1 | Masse bzw. Konfiguration |
| 14 | CONFIG2 | Masse bzw. Konfiguration |
| 15 | AUX+ | Hilfskanal |
| 16 | GND | – |
| 17 | AUX− | Hilfskanal |
| 18 | Hot Plug | Monitor erkannt |
| 19 | DP_PWR Return | Masse Versorgung |
| 20 | DP_PWR | +3,3 V |

*VESA DisplayPort 1.x, Steckerbelegung an der Quelle (Grafikkarte). 4 Datenkanäle (Main Link), AUX-Kanal für EDID/DPCD, Hot Plug Detect, DP_PWR = 3,3 V / 500 mA.*

#### Pinbelegung DVI

| Pin | Signal | Beschreibung |
|---|---|---|
| 1 | TMDS 2− | – |
| 2 | TMDS 2+ | – |
| 3 | Schirm 2/4 | – |
| 4 | TMDS 4− | Dual Link |
| 5 | TMDS 4+ | Dual Link |
| 6 | DDC SCL | Takt |
| 7 | DDC SDA | Daten |
| 8 | Analog VSync | DVI-I |
| 9 | TMDS 1− | – |
| 10 | TMDS 1+ | – |
| 11 | Schirm 1/3 | – |
| 12 | TMDS 3− | Dual Link |
| 13 | TMDS 3+ | Dual Link |
| 14 | +5 V | Versorgung DDC |
| 15 | GND | Masse (+5 V, Sync) |
| 16 | Hot Plug | Monitor erkannt |
| 17 | TMDS 0− | – |
| 18 | TMDS 0+ | – |
| 19 | Schirm 0/5 | – |
| 20 | TMDS 5− | Dual Link |
| 21 | TMDS 5+ | Dual Link |
| 22 | Schirm Takt | – |
| 23 | TMDS Takt+ | – |
| 24 | TMDS Takt− | – |
| C1 | Analog Rot | DVI-I |
| C2 | Analog Grün | DVI-I |
| C3 | Analog Blau | DVI-I |
| C4 | Analog HSync | DVI-I |
| C5 | Analog Masse | DVI-I |

*DDWG DVI 1.0: TMDS-Kanäle 0–2 (Single Link), 3–5 zusätzlich bei Dual Link. C1–C5 nur bei DVI-I/DVI-A (analog, VGA-kompatibel).*

#### Pinbelegung VGA

| Pin | Signal | Beschreibung |
|---|---|---|
| 1 | Rot | 0,7 V / 75 Ω |
| 2 | Grün | 0,7 V / 75 Ω |
| 3 | Blau | 0,7 V / 75 Ω |
| 4 | ID2 | reserviert |
| 5 | GND | Masse (HSync) |
| 6 | Masse Rot | – |
| 7 | Masse Grün | – |
| 8 | Masse Blau | – |
| 9 | +5 V | Versorgung DDC |
| 10 | GND | Masse (VSync, DDC) |
| 11 | ID0 | reserviert |
| 12 | SDA | DDC-Daten |
| 13 | HSync | Horizontalsynchronisation |
| 14 | VSync | Vertikalsynchronisation |
| 15 | SCL | DDC-Takt |

*VESA DDC2 / E-DDC. RGB analog 0,7 V an 75 Ω, H-/V-Sync TTL. Pin 9 = +5 V für das EDID-EEPROM (bei älteren Karten frei).*

#### Pinbelegung SCART

| Pin | Signal | Beschreibung |
|---|---|---|
| 1 | Audio R aus | 0,5 V eff |
| 2 | Audio R ein | 0,5 V eff |
| 3 | Audio L aus | 0,5 V eff |
| 4 | Audio-Masse | – |
| 5 | Masse Blau | – |
| 6 | Audio L ein | 0,5 V eff |
| 7 | RGB Blau | 0,7 V / 75 Ω |
| 8 | Schaltspannung / Format | 0–12 V |
| 9 | Masse Grün | – |
| 10 | Daten 2 (Clock) | – |
| 11 | RGB Grün | 0,7 V / 75 Ω |
| 12 | Daten 1 | – |
| 13 | Masse Rot | – |
| 14 | Masse Daten | – |
| 15 | RGB Rot / Chroma (C) | 0,7 V / 75 Ω |
| 16 | Austastung / RGB-Umschaltung | 1–3 V |
| 17 | Masse Video | – |
| 18 | Masse Austastung | – |
| 19 | FBAS (Composite) aus | 1 V / 75 Ω |
| 20 | FBAS ein / Luma (Y) | 1 V / 75 Ω |
| 21 | Schirm / Masse | – |

*Péritel/SCART nach EN 50049-1 (IEC 60933-1). Pin 8: 0–2 V aus, 5–8 V 16:9, 9,5–12 V 4:3. Pin 16: 1–3 V = RGB.*

#### Pinbelegung FireWire (IEEE 1394)

| 6-polig | 4-polig | Signal |
|---|---|---|
| 1 | – | Versorgung + |
| 2 | – | Masse |
| 3 | 1 | TPB− |
| 4 | 2 | TPB+ |
| 5 | 3 | TPA− |
| 6 | 4 | TPA+ |

*IEEE 1394a. 6-polig mit Versorgung (8–30 V DC, max. 1,5 A), 4-polig (i.LINK) ohne Versorgung: Pins 3–6 des 6-poligen entsprechen 1–4 des 4-poligen.*

#### Pinbelegung PS/2

| Pin | Signal | Beschreibung |
|---|---|---|
| 1 | DATA | Daten |
| 2 | NC | frei |
| 3 | GND | Masse |
| 4 | +5 V | Versorgung (max. ca. 275 mA) |
| 5 | CLK | Takt |
| 6 | NC | frei |

*IBM PS/2 (Mini-DIN 6). Synchrones serielles Protokoll, Takt vom Gerät. Farben nach PC 99: Tastatur violett, Maus grün.*

#### Pinbelegung Molex und SATA

| Stecker | Pin | Signal |
|---|---|---|
| Molex 4-polig | 1 | +12 V (gelb) |
| Molex 4-polig | 2 | Masse (schwarz) |
| Molex 4-polig | 3 | Masse (schwarz) |
| Molex 4-polig | 4 | +5 V (rot) |
| SATA-Daten | 1, 4, 7 | Masse |
| SATA-Daten | 2 / 3 | A+ / A− (Senden) |
| SATA-Daten | 5 / 6 | B− / B+ (Empfangen) |
| SATA-Strom | 1–3 | +3,3 V |
| SATA-Strom | 4–6 | Masse |
| SATA-Strom | 7–9 | +5 V |
| SATA-Strom | 10, 12 | Masse |
| SATA-Strom | 11 | Aktivität / Staggered Spin-up |
| SATA-Strom | 13–15 | +12 V |

*Molex 8981 (4-polig, Laufwerke) nach ATX-Netzteilspezifikation; SATA-Daten 7-polig und SATA-Strom 15-polig (Serial ATA Rev. 3). Aderfarben: +12 V gelb, +5 V rot, +3,3 V orange, Masse schwarz.*

#### Pinbelegung SD-Karte

| SD-Pin | microSD-Pin | SD-Modus | SPI-Modus |
|---|---|---|---|
| 1 | 2 | CD/DAT3 | CS |
| 2 | 3 | CMD | MOSI |
| 3 | – | VSS1 (GND) | GND |
| 4 | 4 | VDD | VDD |
| 5 | 5 | CLK | SCLK |
| 6 | 6 | VSS2 (GND) | GND |
| 7 | 7 | DAT0 | MISO |
| 8 | 8 | DAT1 | – |
| 9 | 1 | DAT2 | – |

*SD Association Physical Layer Spec. Versorgung 2,7–3,6 V. Im SPI-Modus: CS, MOSI (CMD), MISO (DAT0), SCLK.*

#### Pinbelegung SIM-Karte

| Pin | Signal | Beschreibung |
|---|---|---|
| C1 | VCC | Versorgung 1,8 / 3 / 5 V |
| C2 | RST | Reset |
| C3 | CLK | Takt |
| C4 | (AUX1) | optional, z. B. USB D+ |
| C5 | GND | Masse |
| C6 | VPP / SWP | NFC (Single Wire Protocol) |
| C7 | I/O | Daten (halbduplex) |
| C8 | (AUX2) | optional, z. B. USB D− |

*ISO/IEC 7816-2/-3 (Chipkarten), ETSI TS 102 221 (UICC). Gilt für Mini-, Micro- und Nano-SIM (gleiche Kontaktlage).*

### Audio

#### Pinbelegung XLR (Audio/DMX)

| Pin | Audio (3-polig) | DMX (3/5-polig) |
|---|---|---|
| 1 | Masse / Schirm | Masse / Schirm |
| 2 | heiß (+) | Daten 1 − |
| 3 | kalt (−) | Daten 1 + |
| 4 | – | Daten 2 − (optional) |
| 5 | – | Daten 2 + (optional) |

*Audio symmetrisch nach IEC 60268-12 / AES14 (Pin 2 = heiß). DMX512 nach ANSI E1.11 (USITT): 5-poliger XLR genormt, 3-polig verbreitet; RS-485, 120-Ω-Kabel und Abschluss am Ende.*

#### Pinbelegung Klinke (Jack)

| Kontakt | Mono (TS) | Stereo (TRS) | Symmetrisch (TRS) | Headset CTIA | Headset OMTP |
|---|---|---|---|---|---|
| Spitze (Tip) | Signal | links | heiß (+) | links | links |
| Ring 1 | – | rechts | kalt (−) | rechts | rechts |
| Ring 2 | – | – | – | Masse | Mikrofon |
| Schaft (Sleeve) | Masse | Masse | Masse / Schirm | Mikrofon | Masse |

*Tip = Spitze, Ring, Sleeve = Schaft. TRRS-Headset: CTIA/AHJ (heute üblich: Apple, Android) vs. OMTP (ältere Geräte) – Mikrofon und Masse vertauscht.*

#### Pinbelegung MIDI

| Pin | Signal | Beschreibung |
|---|---|---|
| 1 | NC | frei |
| 2 | Schirm | Masse nur an MIDI OUT |
| 3 | NC | frei |
| 4 | Stromquelle | +5 V über 220 Ω |
| 5 | Stromsenke / Daten | über 220 Ω |

*MIDI 1.0 (MMA): Stromschleife 5 mA mit Optokoppler im Empfänger, 31 250 Bit/s. Schirm (Pin 2) nur an der Senderseite (MIDI OUT) mit Masse verbunden.*

#### RCA-Farbcode (Cinch)

| Farbe | Signal |
|---|---|
| gelb | Composite-Video (FBAS) |
| weiß (oder schwarz) | Audio links (analog) |
| rot | Audio rechts (analog) bzw. Component Pr |
| grün | Component Y (Luminanz) |
| blau | Component Pb |
| orange | Digital-Audio S/PDIF koaxial |
| violett | Subwoofer / LFE |

*Übliche Farbkennzeichnung nach IEC 60603-14 / CEA-863 für Cinch-Verbindungen (Consumer-Video und -Audio).*

### Fahrzeug

#### Pinbelegung ISO 10487 (Autoradio)

| Pin | Signal | Beschreibung |
|---|---|---|
| A4 | +12 V Dauerplus | Speicher (gelb) |
| A5 | +12 V Antenne/Verstärker | geschaltet (blau) |
| A6 | Beleuchtung | (orange) |
| A7 | +12 V Zündungsplus | (rot) |
| A8 | Masse | (braun/schwarz) |
| A1–A3 | herstellerabhängig | z. B. Telefon-Stummschaltung |
| B1 / B2 | hinten rechts + / − | violett |
| B3 / B4 | vorne rechts + / − | grau |
| B5 / B6 | vorne links + / − | weiß |
| B7 / B8 | hinten links + / − | grün |

*ISO 10487-1/-2. Achtung: Einige Fahrzeughersteller tauschen A4 (Dauerplus) und A7 (Zündungsplus) – vor dem Einbau messen.*

#### Pinbelegung OBD II

| Pin | Signal | Beschreibung |
|---|---|---|
| 2 | J1850 Bus+ | PWM/VPW |
| 4 | Fahrzeugmasse | – |
| 5 | Signalmasse | – |
| 6 | CAN High | ISO 15765-4 |
| 7 | K-Line | ISO 9141-2 / 14230 |
| 10 | J1850 Bus− | PWM |
| 14 | CAN Low | ISO 15765-4 |
| 15 | L-Line | ISO 9141-2 / 14230 |
| 16 | +12 V Batterie | Dauerplus |

*SAE J1962 / ISO 15031-3. Protokolle: CAN ISO 15765-4 (6/14, Pflicht in USA ab 2008, EU-OBD), K-Line ISO 9141-2 / ISO 14230 (7/15), SAE J1850 PWM/VPW (2/10). Nicht genannte Pins: herstellerspezifisch.*

### Elektronik-Module

#### Pinbelegung Raspberry Pi

| Pin | Funktion | Pin | Funktion |
|---|---|---|---|
| 1 | 3,3 V | 2 | 5 V |
| 3 | GPIO2 (SDA1) | 4 | 5 V |
| 5 | GPIO3 (SCL1) | 6 | GND |
| 7 | GPIO4 (GPCLK0) | 8 | GPIO14 (TXD) |
| 9 | GND | 10 | GPIO15 (RXD) |
| 11 | GPIO17 | 12 | GPIO18 (PWM0) |
| 13 | GPIO27 | 14 | GND |
| 15 | GPIO22 | 16 | GPIO23 |
| 17 | 3,3 V | 18 | GPIO24 |
| 19 | GPIO10 (MOSI) | 20 | GND |
| 21 | GPIO9 (MISO) | 22 | GPIO25 |
| 23 | GPIO11 (SCLK) | 24 | GPIO8 (CE0) |
| 25 | GND | 26 | GPIO7 (CE1) |
| 27 | GPIO0 (ID_SD) | 28 | GPIO1 (ID_SC) |
| 29 | GPIO5 | 30 | GND |
| 31 | GPIO6 | 32 | GPIO12 (PWM0) |
| 33 | GPIO13 (PWM1) | 34 | GND |
| 35 | GPIO19 (PCM_FS) | 36 | GPIO16 |
| 37 | GPIO26 | 38 | GPIO20 (PCM_DIN) |
| 39 | GND | 40 | GPIO21 (PCM_DOUT) |

*Raspberry Pi B+, 2, 3, 4, 5, Zero: physische Pinnummer und BCM-GPIO-Nummer. GPIO-Pegel 3,3 V (nicht 5-V-tolerant!). Pin 27/28 für HAT-EEPROM reserviert.*

#### Pinbelegung Display LCD 16x2

| Pin | Signal | Beschreibung |
|---|---|---|
| 1 | VSS | Masse |
| 2 | VDD | +5 V |
| 3 | V0 | Kontrast |
| 4 | RS | 0 = Befehl, 1 = Daten |
| 5 | R/W | 0 = schreiben, 1 = lesen |
| 6 | E | Enable (Takt) |
| 7–10 | D0–D3 | Datenbits (nur 8-Bit-Betrieb) |
| 11–14 | D4–D7 | Datenbits |
| 15 | A | Beleuchtung + |
| 16 | K | Beleuchtung − |

*Zeichendisplays mit HD44780-Controller. 4-Bit-Betrieb: nur D4–D7 anschließen. Kontrast an V0 über Poti 10 kΩ; Hintergrundbeleuchtung meist mit Vorwiderstand.*

#### Pinbelegung LED

| Merkmal / Farbe | Anschluss bzw. U_F |
|---|---|
| langes Bein | Anode (+) |
| kurzes Bein, flache Seite | Kathode (−) |
| RGB, längstes Bein | gemeinsame Anode oder Kathode |
| rot | 1,8–2,1 V |
| gelb / orange | 2,0–2,2 V |
| grün (klassisch) | 2,0–2,4 V |
| blau, weiß, grün (InGaN) | 2,9–3,4 V |
| IR | 1,2–1,5 V |

*Bedrahtete LED (3/5 mm): langes Bein = Anode (+), kurzes Bein = Kathode (−), abgeflachte Gehäuseseite = Kathode. RGB-LED mit 4 Beinen: längstes Bein = gemeinsamer Anschluss (Anode oder Kathode). Vorwiderstand siehe Rechner. Typische Flussspannungen bei 20 mA siehe Tabelle.*

### Antenne & Videoüberwachung

#### Antennenlängenberechnung

*Wellenlänge und Länge von Dipol, Monopol (λ/4) und 5/8 λ* — `antenna_length`

```text
λ = c / f (c = 299 792 458 m/s)    l = Bruchteil · λ · k
k = Verkürzungsfaktor (Drahtdipol ≈ 0,95, dicke Rohre kleiner); λ/2-Dipol: Gesamtlänge, jeder Arm l/2.
```

**Eingaben:** Frequenz f [MHz], Antennenform, Verkürzungsfaktor k

**Ergebnisse:** Antennenlänge [cm], Wellenlänge λ [m], Länge ohne Verkürzung [cm]

#### CCTV Speicherplatz- und Bandbreitenberechnung

*Bitrate, Netzwerklast und Speicher für Videoüberwachung* — `cctv_storage`

```text
Bitrate ≈ Pixel · Bilder/s · Bits je Pixel (Richtwerte: H.264 0,08, H.265 0,04, MJPEG 0,75 bit/Pixel; stark szenenabhängig – Herstellerwert eintragen, wenn bekannt).
Bandbreite = n · Bitrate    Speicher = n · Bitrate · Aufzeichnungsdauer · Anteil / 8 (1 TB = 10¹² Byte).
```

**Eingaben:** Anzahl Kameras, Auflösung, Bilder pro Sekunde [1/s], Videokompression, Bitrate je Kamera (Herstellerwert) [Mbit/s], Aufzeichnung pro Tag [h], Anteil mit Aufzeichnung (Bewegung) [%], Speicherdauer [d]

**Ergebnisse:** Speicherbedarf [TB], Bitrate je Kamera [Mbit/s], Netzwerklast gesamt [Mbit/s], Speicher pro Tag [GB]

#### CCTV-Auflösungen

| Bezeichnung | Pixel | Megapixel | Format |
|---|---|---|---|
| QCIF | 176 × 144 | 0,03 | 11:9 |
| CIF | 352 × 288 | 0,1 | 11:9 |
| 4CIF | 704 × 576 | 0,41 | 11:9 |
| D1 (PAL) | 720 × 576 | 0,41 | 5:4 |
| 960H | 960 × 576 | 0,55 | 5:3 |
| 720p (HD) | 1280 × 720 | 0,92 | 16:9 |
| 960p (1,3 MP) | 1280 × 960 | 1,23 | 4:3 |
| 1080p (Full HD, 2 MP) | 1920 × 1080 | 2,07 | 16:9 |
| 3 MP | 2048 × 1536 | 3,15 | 4:3 |
| 4 MP | 2560 × 1440 | 3,69 | 16:9 |
| 5 MP | 2592 × 1944 | 5,04 | 4:3 |
| 6 MP | 3072 × 2048 | 6,29 | 3:2 |
| 4K UHD (8 MP) | 3840 × 2160 | 8,29 | 16:9 |
| 12 MP | 4000 × 3000 | 12 | 4:3 |

*Übliche Auflösungen von Überwachungskameras (analog: CIF/D1/960H, IP: HD bis 4K). Speicherbedarf siehe Rechner „CCTV Speicherplatz“. Bezeichnungen „MP“ sind herstellerüblich, die genaue Pixelzahl kann abweichen.*

## Umrechnung & Einheiten

### Elektrische Größen

#### Spannung

*Einheiten, Effektiv-/Scheitel-/Spitze-Spitze-Wert (Sinus) und dB-Pegel* — `conv_voltage`

```text
Sinus: Û = √2 · U_eff    U_ss = 2 · Û    Gleichrichtwert |ū| = 2Û/π = 0,9003 · U_eff
Pegel (Effektivwert): dBV = 20 · lg(U / 1 V), dBu = 20 · lg(U / 0,7746 V), dBmV = 20 · lg(U / 1 mV), dBµV = 20 · lg(U / 1 µV).
```

**Eingaben:** Wert, Einheit, Angabe als (Sinus)

**Ergebnisse:** Effektivwert [V], Scheitelwert Û [V], Spitze-Spitze [V], Gleichrichtwert [V], Effektivwert [mV], Effektivwert [kV], Pegel [dBV], Pegel [dBu], Pegel [dBmV], Pegel [dBµV]

#### Leistung

*W, kW, PS, hp, BTU/h, kcal/h, TR* — `conv_power`

```text
Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen (PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Watt [W], Kilowatt [kW], Megawatt [MW], Pferdestärke (metrisch) [PS], Horsepower (mechanisch) [hp], BTU pro Stunde [BTU/h], Kilokalorie pro Stunde [kcal/h], Kältetonne (Tonne of Refrigeration) [TR]

#### Energie / Arbeit

*J, Wh, kWh, kcal, BTU* — `conv_energy`

```text
Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen (PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Joule [J], Kilojoule [kJ], Megajoule [MJ], Wattstunde [Wh], Kilowattstunde [kWh], Megawattstunde [MWh], Kilokalorie [kcal], British Thermal Unit [BTU]

#### var / µF Umrechnung

*Blindleistung eines Kondensators ↔ Kapazität* — `conv_kvar_uf`

```text
Q = ω · C · U_C² je Kondensator, ω = 2πf.
1~: U_C = U. 3~ Stern: U_C = U/√3, Q_ges = 3 · ω · C · U²/3 = ω · C · U². 3~ Dreieck: U_C = U, Q_ges = 3 · ω · C · U² (U = Außenleiterspannung, C = Kapazität je Kondensator).
```

**Eingaben:** Gegeben, Blindleistung Q (gesamt) [kvar], Kapazität je Kondensator [µF], Spannung U [V], Frequenz, Schaltung

**Ergebnisse:** Kapazität je Kondensator [µF], Blindleistung gesamt [kvar], Strom (Außenleiter) [A], Spannung am Kondensator [V]

#### Winkel und trigonometrische Funktionen

*Grad, Bogenmaß, Gon ↔ sin, cos, tan (auch Phasenwinkel cos φ)* — `trig_angle`

```text
φ [rad] = φ [°] · π/180    φ [gon] = φ [°] · 400/360
sin, cos, tan, cot des Winkels; aus sin/cos/tan wird der Hauptwert (arcsin −90…90°, arccos 0…180°, arctan −90…90°) bestimmt. Leistungsfaktor: cos φ = P/S, tan φ = Q/P.
```

**Eingaben:** Wert, Gegeben

**Ergebnisse:** Winkel [°], Winkel [rad], Winkel [gon], sin, cos, tan, cot

### Leiterquerschnitte

#### AWG ↔ mm²

*American Wire Gauge in metrischen Querschnitt* — `conv_awg`

```text
d = 0,127 mm · 92^((36 − AWG)/39)    A = π/4 · d²
AWG 1/0 = 0, 2/0 = −1, 3/0 = −2, 4/0 = −3 eingeben.
```

**Eingaben:** Gegeben, AWG, Querschnitt [mm²]

**Ergebnisse:** AWG, Durchmesser [mm], Querschnitt [mm²], Nächster metrischer Normquerschnitt ≥ [mm²]

#### AWG-/kcmil-Umrechnungstabelle

| AWG / kcmil | d in mm | A in mm² | R in Ω/km | metrisch ≥ |
|---|---|---|---|---|
| 1000 kcmil | 25,4 | 506,71 | 0,034 | – |
| 750 kcmil | 21,997 | 380,03 | 0,0454 | – |
| 600 kcmil | 19,675 | 304,02 | 0,0567 | – |
| 500 kcmil | 17,961 | 253,35 | 0,0681 | 300 |
| 400 kcmil | 16,064 | 202,68 | 0,0851 | 240 |
| 350 kcmil | 15,027 | 177,35 | 0,0972 | 185 |
| 300 kcmil | 13,912 | 152,01 | 0,1134 | 185 |
| 250 kcmil | 12,7 | 126,68 | 0,1361 | 150 |
| 4/0 | 11,684 | 107,22 | 0,1608 | 120 |
| 3/0 | 10,405 | 85,03 | 0,2028 | 95 |
| 2/0 | 9,266 | 67,43 | 0,2557 | 70 |
| 1/0 | 8,251 | 53,48 | 0,3224 | 70 |
| 1 | 7,348 | 42,41 | 0,4066 | 50 |
| 2 | 6,544 | 33,63 | 0,5127 | 35 |
| 3 | 5,827 | 26,67 | 0,6464 | 35 |
| 4 | 5,189 | 21,15 | 0,8152 | 25 |
| 5 | 4,621 | 16,77 | 1,03 | 25 |
| 6 | 4,115 | 13,3 | 1,3 | 16 |
| 7 | 3,665 | 10,55 | 1,63 | 16 |
| 8 | 3,264 | 8,37 | 2,06 | 10 |
| 9 | 2,906 | 6,63 | 2,6 | 10 |
| 10 | 2,588 | 5,26 | 3,28 | 6 |
| 11 | 2,305 | 4,17 | 4,13 | 6 |
| 12 | 2,053 | 3,31 | 5,21 | 4 |
| 13 | 1,828 | 2,62 | 6,57 | 4 |
| 14 | 1,628 | 2,08 | 8,29 | 2,5 |
| 15 | 1,45 | 1,65 | 10,45 | 2,5 |
| 16 | 1,291 | 1,31 | 13,17 | 1,5 |
| 17 | 1,15 | 1,04 | 16,61 | 1,5 |
| 18 | 1,024 | 0,823 | 20,95 | 1 |
| 19 | 0,912 | 0,6527 | 26,41 | 0,75 |
| 20 | 0,812 | 0,5176 | 33,31 | 0,75 |
| 21 | 0,723 | 0,4105 | 42 | 0,5 |
| 22 | 0,644 | 0,3255 | 52,96 | 0,5 |
| 23 | 0,573 | 0,2582 | 66,78 | 0,5 |
| 24 | 0,511 | 0,2047 | 84,21 | 0,5 |
| 25 | 0,455 | 0,1624 | 106,19 | 0,5 |
| 26 | 0,405 | 0,1288 | 133,9 | 0,5 |
| 27 | 0,361 | 0,1021 | 168,85 | 0,5 |
| 28 | 0,321 | 0,081 | 212,92 | 0,5 |
| 29 | 0,286 | 0,0642 | 268,48 | 0,5 |
| 30 | 0,255 | 0,0509 | 338,55 | 0,5 |
| 31 | 0,227 | 0,0404 | 426,9 | 0,5 |
| 32 | 0,202 | 0,032 | 538,32 | 0,5 |
| 33 | 0,18 | 0,0254 | 678,8 | 0,5 |
| 34 | 0,16 | 0,0201 | 855,96 | 0,5 |
| 35 | 0,143 | 0,016 | 1079,34 | 0,5 |
| 36 | 0,127 | 0,0127 | 1361,02 | 0,5 |
| 37 | 0,113 | 0,01 | 1716,22 | 0,5 |
| 38 | 0,101 | 0,008 | 2164,11 | 0,5 |
| 39 | 0,09 | 0,0063 | 2728,89 | 0,5 |
| 40 | 0,08 | 0,005 | 3441,07 | 0,5 |

*ASTM B258: d = 0,127 mm · 92^((36 − n)/39), n = 0 für 1/0, −1 für 2/0 …; A = π/4 · d²; R für Kupfer (massiv) bei 20 °C mit ρ = 0,017241 Ω·mm²/m. Über 4/0 AWG: 1 kcmil = 0,506707 mm² (d = Durchmesser eines gleich großen Massivleiters). Letzte Spalte: nächster metrischer Normquerschnitt ≥ US-Querschnitt (IEC 60228). Größen sind nicht gleichwertig – Belastbarkeit immer nach der jeweiligen Norm (IEC bzw. NEC) bestimmen.*

#### SWG-Umrechnungstabelle

| SWG | d in Zoll | d in mm | A in mm² |
|---|---|---|---|
| 7/0 | 0,5 | 12,7 | 126,68 |
| 6/0 | 0,464 | 11,786 | 109,09 |
| 5/0 | 0,432 | 10,973 | 94,56 |
| 4/0 | 0,4 | 10,16 | 81,07 |
| 3/0 | 0,372 | 9,449 | 70,12 |
| 2/0 | 0,348 | 8,839 | 61,36 |
| 1/0 | 0,324 | 8,23 | 53,19 |
| 1 | 0,3 | 7,62 | 45,6 |
| 2 | 0,276 | 7,01 | 38,6 |
| 3 | 0,252 | 6,401 | 32,18 |
| 4 | 0,232 | 5,893 | 27,27 |
| 5 | 0,212 | 5,385 | 22,77 |
| 6 | 0,192 | 4,877 | 18,68 |
| 7 | 0,176 | 4,47 | 15,7 |
| 8 | 0,16 | 4,064 | 12,97 |
| 9 | 0,144 | 3,658 | 10,51 |
| 10 | 0,128 | 3,251 | 8,3 |
| 11 | 0,116 | 2,946 | 6,82 |
| 12 | 0,104 | 2,642 | 5,48 |
| 13 | 0,092 | 2,337 | 4,29 |
| 14 | 0,08 | 2,032 | 3,24 |
| 15 | 0,072 | 1,829 | 2,63 |
| 16 | 0,064 | 1,626 | 2,08 |
| 17 | 0,056 | 1,422 | 1,59 |
| 18 | 0,048 | 1,219 | 1,17 |
| 19 | 0,04 | 1,016 | 0,81 |
| 20 | 0,036 | 0,914 | 0,6567 |
| 21 | 0,032 | 0,813 | 0,5189 |
| 22 | 0,028 | 0,711 | 0,3973 |
| 23 | 0,024 | 0,61 | 0,2919 |
| 24 | 0,022 | 0,559 | 0,2452 |
| 25 | 0,02 | 0,508 | 0,2027 |
| 26 | 0,018 | 0,457 | 0,1642 |
| 27 | 0,0164 | 0,417 | 0,1363 |
| 28 | 0,0148 | 0,376 | 0,111 |
| 29 | 0,0136 | 0,345 | 0,0937 |
| 30 | 0,0124 | 0,315 | 0,0779 |
| 31 | 0,0116 | 0,295 | 0,0682 |
| 32 | 0,0108 | 0,274 | 0,0591 |
| 33 | 0,01 | 0,254 | 0,0507 |
| 34 | 0,0092 | 0,234 | 0,0429 |
| 35 | 0,0084 | 0,213 | 0,0358 |
| 36 | 0,0076 | 0,193 | 0,0293 |
| 37 | 0,0068 | 0,173 | 0,0234 |
| 38 | 0,006 | 0,152 | 0,0182 |
| 39 | 0,0052 | 0,132 | 0,0137 |
| 40 | 0,0048 | 0,122 | 0,0117 |
| 41 | 0,0044 | 0,112 | 0,0098 |
| 42 | 0,004 | 0,102 | 0,0081 |
| 43 | 0,0036 | 0,091 | 0,0066 |
| 44 | 0,0032 | 0,081 | 0,0052 |
| 45 | 0,0028 | 0,071 | 0,004 |
| 46 | 0,0024 | 0,061 | 0,0029 |
| 47 | 0,002 | 0,051 | 0,002 |
| 48 | 0,0016 | 0,041 | 0,0013 |
| 49 | 0,0012 | 0,03 | 0,0007297 |
| 50 | 0,001 | 0,025 | 0,0005067 |

*Britische Drahtlehre (Imperial Standard Wire Gauge, 1884; BS 3737), Durchmesser in Zoll definiert, 1 in = 25,4 mm.*

#### Querschnitt / Fläche

*mm², cm², m², in², kcmil (MCM), cmil* — `conv_area`

```text
Umrechnung über m². 1 in = 25,4 mm (exakt); 1 cmil = Fläche eines Kreises von 1 mil (0,001 in) Durchmesser = π/4 · (25,4 µm)² = 5,0671 · 10⁻⁴ mm²; 1 kcmil (MCM) = 1000 cmil = 0,5067 mm².
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Quadratmillimeter [mm²], Quadratzentimeter [cm²], Quadratmeter [m²], Quadratzoll [in²], Quadratfuß [ft²], Kilo-Circular-Mil (MCM) [kcmil], Circular Mil [cmil]

### Allgemeine Größen

#### Länge

*mm, m, km, mil, in, ft, yd, mi, sm* — `conv_length`

```text
Umrechnung über Meter. Zoll 25,4 mm, Fuß 0,3048 m, Yard 0,9144 m, Meile 1609,344 m, Seemeile 1852 m (alle exakt definiert).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Mikrometer [µm], Millimeter [mm], Zentimeter [cm], Meter [m], Kilometer [km], Mil (1/1000 Zoll) [mil], Zoll [in], Fuß [ft], Yard [yd], Meile [mi], Seemeile [sm]

#### Temperatur

*°C ↔ K ↔ °F* — `conv_temperature`

```text
T[K] = θ[°C] + 273,15    θ[°F] = θ[°C] · 9/5 + 32
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Celsius [°C], Kelvin [K], Fahrenheit [°F]

#### Druck

*Pa, kPa, bar, mbar, psi, mWS* — `conv_pressure`

```text
Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen (PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Pascal [Pa], Hektopascal [hPa], Kilopascal [kPa], Millibar [mbar], Bar [bar], Pound per square inch [psi], Meter Wassersäule [mWS], Millimeter Quecksilbersäule [mmHg]

#### Volumenstrom

*m³/h, l/s, l/min, CFM, gpm* — `conv_flow`

```text
Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen (PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Kubikmeter pro Stunde [m³/h], Kubikmeter pro Sekunde [m³/s], Liter pro Sekunde [l/s], Liter pro Minute [l/min], Liter pro Stunde [l/h], Cubic feet per minute [CFM], US-Gallonen pro Minute [gpm (US)]

#### Drehzahl

*1/min, 1/s, rad/s, °/s* — `conv_speed`

```text
Umrechnung über rad/s: ω = 2π · n/60 (n in 1/min); 1/s = Hz. Synchrondrehzahl n_s = 60 · f / p (p = Polpaarzahl) siehe Rechner Drehzahl & Schlupf.
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Umdrehungen pro Minute [1/min], Umdrehungen pro Sekunde (Hz) [1/s], Winkelgeschwindigkeit [rad/s], Grad pro Sekunde [°/s]

#### Währung € ↔ CHF

*Euro und Schweizer Franken umrechnen* — `conv_currency`

```text
CHF = € · Kurs    € = CHF / Kurs    (Kurs = Franken für 1 Euro)
Ohne Eingabe gilt der Kurs aus den Einstellungen (dort für alle Kostenrechnungen verwendet). Stand Oktober 2026 etwa 0,92 CHF/€ – Tageskurs der Bank verwenden.
```

**Eingaben:** Betrag, Von, Kurs 1 € in CHF [CHF]

**Ergebnisse:** Ergebnis, Verwendeter Kurs 1 € = [CHF], Umkehrkurs 1 CHF = [€]

### Einheiten & Vorsätze

#### Maßeinheiten der Elektrotechnik

| Größe | Einheit | Zeichen | Definition |
|---|---|---|---|
| Stromstärke | Ampere | A | SI-Basiseinheit |
| Spannung | Volt | V | 1 V = 1 W/A |
| Widerstand | Ohm | Ω | 1 Ω = 1 V/A |
| Leitwert | Siemens | S | 1 S = 1/Ω |
| Ladung | Coulomb | C | 1 C = 1 A·s (1 Ah = 3600 C) |
| Kapazität | Farad | F | 1 F = 1 C/V |
| Induktivität | Henry | H | 1 H = 1 V·s/A |
| Magnetischer Fluss | Weber | Wb | 1 Wb = 1 V·s |
| Flussdichte | Tesla | T | 1 T = 1 Wb/m² |
| Frequenz | Hertz | Hz | 1 Hz = 1/s |
| Wirkleistung | Watt | W | 1 W = 1 J/s = 1 V·A |
| Scheinleistung | Voltampere | VA | S = U · I |
| Blindleistung | Var | var | Q = U · I · sin φ |
| Energie | Joule | J | 1 J = 1 W·s; 1 kWh = 3,6 MJ |
| Elektrische Feldstärke | Volt pro Meter | V/m | E = U / d |
| Magnetische Feldstärke | Ampere pro Meter | A/m | H = I · N / l |
| Lichtstärke | Candela | cd | SI-Basiseinheit |
| Lichtstrom | Lumen | lm | 1 lm = 1 cd·sr |
| Beleuchtungsstärke | Lux | lx | 1 lx = 1 lm/m² |
| Temperatur | Kelvin | K | SI-Basiseinheit; θ [°C] = T − 273,15 |
| Kraft | Newton | N | 1 N = 1 kg·m/s² |
| Druck | Pascal | Pa | 1 Pa = 1 N/m²; 1 bar = 10⁵ Pa |

*Internationales Einheitensystem (SI, ISO 80000 / IEC 80000). Vorsätze siehe Tabelle SI-Vorsätze, Umrechnungen siehe Umrechner.*

#### SI-Vorsätze

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

