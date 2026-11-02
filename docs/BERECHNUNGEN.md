# Berechnungen, Formeln & Tabellen

> Automatisch erzeugt mit `elcalc markdown` aus dem Rechenkern (Version 1.7.0).
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

## Elektrotechnik

### Spannungsberechnung

*U aus Leistung, Strom und cos φ* — `voltage_from_power`

```text
DC: U = P / I    1~: U = P / (I · cos φ)    3~: U = P / (√3 · I · cos φ) (Außenleiterspannung)
P ist die aufgenommene Wirkleistung.
```

**Eingaben:** Netz, Wirkleistung P [kW], Strom I [A], Leistungsfaktor cos φ

**Ergebnisse:** Spannung U [V], Strangspannung U₀ (3~) [V], Scheinleistung S [kVA]

### Leistungsfaktorberechnung

*cos φ aus P und U·I, aus P und S oder aus P und Q* — `power_factor_calc`

```text
cos φ = P / S    S = k · U · I (k = 1 bei 1~, √3 bei 3~)    S = √(P² + Q²)
φ = arccos(cos φ), Q = P · tan φ. Der Leistungsfaktor λ = P/S ist bei Oberschwingungen kleiner als cos φ₁.
```

**Eingaben:** Bekannt, Netz, Wirkleistung P [kW], Spannung U [V], Strom I [A], Scheinleistung S [kVA], Blindleistung Q [kvar]

**Ergebnisse:** Leistungsfaktor cos φ, Phasenwinkel φ [°], sin φ, tan φ, Scheinleistung S [kVA], Blindleistung Q [kvar]

### Impedanzberechnung

*Z, R und X aus Spannung, Strom und cos φ* — `impedance_ui`

```text
1~: Z = U / I    3~ (je Strang, Sternersatz): Z = U / (√3 · I)
R = Z · cos φ    X = Z · sin φ
```

**Eingaben:** Netz, Spannung U [V], Strom I [A], Leistungsfaktor cos φ

**Ergebnisse:** Impedanz Z [Ω], Wirkwiderstand R [Ω], Blindwiderstand X [Ω], Phasenwinkel φ [°]

### Betriebsstrom

*I_b mit Ausnutzungs- und Gleichzeitigkeitsfaktor* — `operating_current`

```text
P_b = P_inst · k_u · k_s    I_b = P_b / (k · U · cos φ · η)
k_u = Ausnutzungsfaktor (Last eines Verbrauchers / Bemessungsleistung, Motoren typ. 0,75), k_s = Gleichzeitigkeitsfaktor der Gruppe (z. B. 0,8–0,9 für Verteiler mit 4–9 Abgängen, IEC 61439-1 Tab. 101 als Anhalt).
```

**Eingaben:** Netz, Spannung U [V], Installierte Leistung P_inst [kW], Ausnutzungsfaktor k_u, Gleichzeitigkeitsfaktor k_s, Leistungsfaktor cos φ, Wirkungsgrad η [%]

**Ergebnisse:** Betriebsstrom I_b [A], Betriebsleistung P_b [kW], Strom bei Volllast (k_u = k_s = 1) [A]

### Joule-Effekt

*Stromwärme W = I² · R · t und Erwärmung* — `joule_effect`

```text
P = I² · R    W = I² · R · t    ΔT = W / (m · c) (ohne Wärmeverluste)
c: Wasser 4186, Kupfer 385, Aluminium 897, Luft 1005 J/(kg·K).
```

**Eingaben:** Widerstand R [Ω], Strom I [A], Zeit t [s], Masse m (optional) [kg], Spezifische Wärmekapazität c [J/(kg·K)]

**Ergebnisse:** Leistung P [W], Wärmeenergie W [kJ], Wärmeenergie W [Wh], Temperaturerhöhung ΔT [K]

### Minimaler Kurzschlussstrom

*I_k1min, I_k2min am Leitungsende (c_min = 0,95)* — `short_circuit_min`

```text
I_k1min = c_min · U₀ / |Z_V + Z_L + Z_PE|    I_k2min = c_min · √3 · U₀ / |Z_V + 2 · Z_L|
c_min = 0,95 (NS, IEC 60909-0 / DIN EN 60909-0). Leiterwiderstand bei Endtemperatur θ (Kurzschlussende, meist 80 °C für PVC bzw. Betriebstemperatur 70/90 °C). Z_V = Schleifenimpedanz am Anfang der Leitung (L–PE, gemessen oder vom Netzbetreiber), X'_L = 0,08 mΩ/m je Leiter.
Prüfung: I_k1min ≥ I_a der Schutzeinrichtung (Abschaltbedingung).
```

**Eingaben:** Nennspannung gegen Erde U₀ [V], Schleifenimpedanz am Leitungsanfang Z_V [mΩ], Leitermaterial, Leitungslänge L [m], Querschnitt Außenleiter, Querschnitt PE, Leitertemperatur θ [°C], Abschaltstrom I_a der Schutzeinrichtung (optional) [A]

**Ergebnisse:** I_k1min (L–PE) [A], I_k1min (L–N, N wie PE) [A], I_k2min (L–L) [A], Schleifenimpedanz Z_s am Ende [mΩ], I_k1min / I_a

### Erdungssystem und Koordination mit Fehlerstrom-Schutzeinrichtung

*TT: R_A · I_Δn ≤ U_L    TN: Z_s · I_a ≤ U₀* — `earthing_rcd`

```text
TT-System (IEC 60364-4-41 / DIN VDE 0100-410, 411.5.3): R_A · I_Δn ≤ U_L, U_L = 50 V AC (25 V in besonderen Räumen, z. B. landwirtschaftliche Betriebsstätten). R_A = Summe aus Erder- und Schutzleiterwiderstand.
TN-System (411.4.4): Z_s · I_a ≤ U₀; mit RCD ist I_a = I_Δn (5 · I_Δn für 0,04 s).
Gegen die gültige Ausgabe prüfen.
```

**Eingaben:** Netzsystem, Bemessungsdifferenzstrom I_Δn, Berührungsspannung U_L, Nennspannung U₀ [V], Gemessen: R_A bzw. Z_s (optional) [Ω]

**Ergebnisse:** Zulässiger Widerstand (R_A bzw. Z_s) [Ω], Berührungsspannung bei I_Δn [V], Max. I_Δn für den gemessenen Widerstand [mA]

### Risikoanalyse für Überspannungen atmosphärischen Ursprungs

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

### Blindleistungskompensation von MS/NS-Transformatoren

*Leerlauf- und Last-Blindleistung, Festkompensation* — `transformer_pfc`

```text
Leerlauf: Q₀ = i₀ · S_N (Magnetisierung, immer vorhanden)
Last: Q_L = u_k · S_N · (S/S_N)² (Streufeld)
Festkompensation des Transformators ≈ Q₀ (Kondensator auf der NS-Seite, dauernd zugeschaltet). i₀ und u_k vom Typenschild bzw. Prüfprotokoll; Öltransformatoren typ. i₀ = 0,5–2 %, Gießharz 0,5–1,5 %. Höchstwert der Festkompensation nach den Technischen Anschlussbedingungen des Netzbetreibers prüfen (Resonanz, Überkompensation).
```

**Eingaben:** Bemessungsleistung S_N [kVA], Leerlaufstrom i₀ [%], Kurzschlussspannung u_k [%], Belastung S [kVA]

**Ergebnisse:** Leerlauf-Blindleistung Q₀ [kvar], Last-Blindleistung Q_L [kvar], Gesamt Q₀ + Q_L [kvar], Auslastung S/S_N [%]

### Kondensatorleistung bei verschiedenen Spannungen

*Q = Q_N · (U/U_N)² · (f/f_N) und Kapazität* — `capacitor_voltage`

```text
Q = Q_N · (U/U_N)² · (f/f_N)
Kapazität (3~): Dreieck C_Δ = Q / (3 · 2πf · U²) je Kondensator, Stern C_Y = Q / (2πf · U²) je Kondensator (U = Außenleiterspannung). 1~: C = Q / (2πf · U²).
Kondensatoren dürfen dauerhaft höchstens mit 1,1 · U_N betrieben werden (IEC 60831-1).
```

**Eingaben:** Bemessungsleistung Q_N [kvar], Bemessungsspannung U_N [V], Bemessungsfrequenz f_N, Betriebsspannung U [V], Netzfrequenz f, Ausführung

**Ergebnisse:** Leistung bei U und f [kvar], Q / Q_N [%], Kapazität je Kondensator [µF], Strom (Außenleiter) [A]

### Strombelastbarkeit von blanken und mineralisolierten Leitern

*MI-Kabel (Cu), Mantel 70 °C, Verlegeart C* — `ampacity_mi`

```text
Mineralisolierte Kupferkabel (MICC) nach IEC 60702-1, Mantel PVC-umhüllt oder blank berührbar (Manteltemperatur 70 °C), direkt auf der Wand (Verlegeart C), Umgebung 30 °C. Werte nach BS 7671 Tab. 4G1A = IEC 60364-5-52 Tab. B.52.6 (Herstellerdatenblatt). 1~ = 2 belastete Leiter, 3~ = 3 belastete Leiter.
Umgebungstemperatur: f = √((70 − θ_a)/(70 − 30)) als Näherung von Tab. B.52.14 – für blanke, nicht berührbare Kabel (Mantel 105 °C, Tab. B.52.7) und Häufung die Norm verwenden. Gegen die gültige Ausgabe prüfen.
```

**Eingaben:** Ausführung, Aderzahl / Belastung, Querschnitt, Umgebungstemperatur θ_a [°C], Betriebsstrom I_b (optional) [A]

**Ergebnisse:** Belastbarkeit I_z [A], Tabellenwert (30 °C) [A], Faktor Umgebungstemperatur, Kabelbezeichnung

### Strombelastbarkeit von Sammelschienen

*E-Cu-Flachschienen nach DIN 43671, 1 oder 2 Schienen je Leiter* — `busbar_ampacity`

```text
Dauerströme für E-Cu-Schienen mit Rechteckquerschnitt in Innenanlagen bei 25 °C Lufttemperatur um den Leiter und max. 65 °C Schienentemperatur, Wechselstrom, Schienen hochkant (Striebel & John, Kap. 10 Sammelschienensysteme, nach DIN 43671).
Andere Umgebungstemperatur als Näherung: I = I₂₅ · √((65 − θ_a)/(65 − 25)) (Wärmeabgabe ∝ Übertemperatur). Für Auslegung die Korrekturfaktoren der DIN 43671 bzw. Herstellerangaben verwenden (Anordnung, Anstrich, Höhe, Frequenz).
```

**Eingaben:** Schiene B × D, Schienen je Leiter, Umgebungstemperatur θ_a [°C], Betriebsstrom I_b (optional) [A]

**Ergebnisse:** Dauerstrom bei θ_a [A], Tabellenwert (25 °C) [A], Querschnitt [mm²], Stromdichte [A/mm²], Gewicht je Leiter [kg/m]

### Kabelwiderstand, Reaktanz und Impedanz

*R(θ), X und Z je Leiter und Schleife* — `cable_rxz`

```text
R₂₀ = ρ₂₀ · L / A    R(θ) = R₂₀ · (1 + α₂₀ · (θ − 20 °C))    X = X' · L    Z = √(R² + X²)
Cu: ρ₂₀ = 0,017241 Ω·mm²/m, α = 0,00393 1/K; Al: 0,028264 Ω·mm²/m, 0,00403 1/K. X' ≈ 0,08 mΩ/m (mehradrige Kabel), ca. 0,1–0,15 mΩ/m für Einleiter mit Abstand. Schleife 1~/DC = Hin- und Rückleiter; 3~ = je Strang.
Gleichstromwiderstand ohne Skineffekt (bei 50 Hz ab ca. 150 mm² merklich).
```

**Eingaben:** Netz, Leitermaterial, Querschnitt, Länge L (einfach) [m], Leitertemperatur θ [°C], Reaktanzbelag X' [mΩ/m]

**Ergebnisse:** R₂₀ je Leiter [mΩ], R(θ) je Leiter [mΩ], X je Leiter [mΩ], Z je Leiter [mΩ], Z Schleife (Hin + Rück) [mΩ], R(θ) je km [Ω/km]

### Berechnung der Kabeltemperatur

*Leitertemperatur aus Auslastung I_b/I_z* — `cable_temperature`

```text
θ = θ_a + (θ_z − θ_a) · (I_b / I_z)²
θ_z = zulässige Leitertemperatur (PVC 70 °C, VPE/EPR 90 °C), I_z = Belastbarkeit bei der tatsächlichen Umgebungstemperatur θ_a und Häufung. Verluste ∝ I² (IEC 60287, ohne Änderung von R mit der Temperatur).
```

**Eingaben:** Betriebsstrom I_b [A], Belastbarkeit I_z [A], Umgebungstemperatur θ_a [°C], Isolierung

**Ergebnisse:** Leitertemperatur θ [°C], Übertemperatur [K], Auslastung I_b/I_z [%], R(θ)/R₂₀ (Cu)

### Verlustleistung in Kabeln

*P_V = n · I² · R(θ), Energie und Kosten pro Jahr* — `cable_losses`

```text
P_V = n · I² · ρ(θ) · L / A    n = 2 (DC/1~), 3 (3~, symmetrisch)
E = P_V · Betriebsstunden    Kosten = E · Strompreis. θ = mittlere Leitertemperatur (siehe Kabeltemperatur), Neutralleiter ohne Strom.
```

**Eingaben:** Netz, Strom I [A], Länge L (einfach) [m], Leitermaterial, Querschnitt, Leitertemperatur θ [°C], Spannung U (optional, für %) [V], Leistungsfaktor cos φ, Betriebsstunden pro Jahr [h], Strompreis pro kWh [€ / CHF]

**Ergebnisse:** Verlustleistung P_V [W], Verluste je Meter Leitung [W/m], Anteil an der übertragenen Leistung [%], Verlustenergie pro Jahr [kWh], Verlustkosten pro Jahr [€]

### Neutralleiterstrom

*Unsymmetrie und 3. Oberschwingung, Auslegung nach IEC 60364-5-52 Anhang E* — `neutral_current`

```text
Grundschwingung (gleiches cos φ, 120° versetzt): I_N1 = √(I₁² + I₂² + I₃² − I₁I₂ − I₂I₃ − I₃I₁)
3. Oberschwingung (h₃ in % des Außenleiterstroms) addiert sich im N: I_N3 = h₃ · (I₁ + I₂ + I₃)    I_N = √(I_N1² + I_N3²)
Auslegung (IEC 60364-5-52 Tab. E.52.1, 4-/5-adrige Kabel): h₃ ≤ 15 %: Außenleiterstrom, Faktor 1,0; 15–33 %: Außenleiterstrom, 0,86; 33–45 %: Neutralleiterstrom, 0,86; > 45 %: Neutralleiterstrom, 1,0.
```

**Eingaben:** Strom L1 [A], Strom L2 [A], Strom L3 [A], Anteil 3. Oberschwingung h₃ [%]

**Ergebnisse:** Neutralleiterstrom I_N [A], Anteil Unsymmetrie I_N1 [A], Anteil 3. Oberschwingung I_N3 [A], I_N / größter Außenleiterstrom [%], Bemessungsstrom für die Kabelwahl [A], Auslegung

### Spannungsfall bei verteilten Lasten

*n gleiche Lasten entlang einer Leitung (z. B. Leuchtenband)* — `vdrop_distributed`

```text
Momentenmethode: ΔU = k · Σ(I · L_i) · (ρ/A · cos φ + X' · sin φ), L_i = L₁ + (i − 1) · d
k = 2 (DC/1~), √3 (3~, symmetrisch verteilt); ρ bei 70 °C, X' = 0,08 mΩ/m. Σ(I·L) entspricht dem Gesamtstrom in der Ersatzlänge L₁ + (n − 1) · d / 2.
Querschnitt: A = ρ · cos φ / (ΔU_max/(k · Σ I·L) − X' · sin φ), danach nächster Normquerschnitt.
```

**Eingaben:** Netz, Nennspannung U [V], Anzahl Lasten n, Strom je Last [A], Abstand bis zur ersten Last L₁ [m], Abstand zwischen den Lasten d [m], Leitermaterial, Leistungsfaktor cos φ, Querschnitt

**Ergebnisse:** Spannungsfall an der letzten Last [%], Spannungsfall ΔU [V], Spannung an der letzten Last [V], Gesamtstrom am Anfang [A], Ersatzlänge [m], Länge bis zur letzten Last [m]

### Leiterdimensionierung bei verteilten Lasten

*Mindestquerschnitt für n gleiche Lasten entlang einer Leitung* — `sizing_distributed`

```text
Momentenmethode: ΔU = k · Σ(I · L_i) · (ρ/A · cos φ + X' · sin φ), L_i = L₁ + (i − 1) · d
k = 2 (DC/1~), √3 (3~, symmetrisch verteilt); ρ bei 70 °C, X' = 0,08 mΩ/m. Σ(I·L) entspricht dem Gesamtstrom in der Ersatzlänge L₁ + (n − 1) · d / 2.
Querschnitt: A = ρ · cos φ / (ΔU_max/(k · Σ I·L) − X' · sin φ), danach nächster Normquerschnitt.
```

**Eingaben:** Netz, Nennspannung U [V], Anzahl Lasten n, Strom je Last [A], Abstand bis zur ersten Last L₁ [m], Abstand zwischen den Lasten d [m], Leitermaterial, Leistungsfaktor cos φ, Zulässiger Spannungsfall [%]

**Ergebnisse:** Gewählter Querschnitt [mm²], Mindestquerschnitt (rechnerisch) [mm²], Spannungsfall mit gewähltem Querschnitt [%], Gesamtstrom am Anfang [A], Ersatzlänge [m]

### Widerstandsfarbcode aus Wert bestimmen

*Wert → Farbringe (4 oder 5 Ringe) mit Zeichnung* — `color_from_value`

```text
IEC 60062: 4 Ringe = 2 Ziffern, Multiplikator, Toleranz (E6–E24); 5 Ringe = 3 Ziffern, Multiplikator, Toleranz (E48–E192). Multiplikator Gold = ×0,1, Silber = ×0,01. Der Wert wird auf 2 bzw. 3 gültige Ziffern gerundet; geprüft wird, ob er in der E24- bzw. E96-Reihe liegt.
```

**Eingaben:** Widerstandswert, Einheit, Anzahl Ringe, Toleranz

**Ergebnisse:** Farbringe, Codierter Wert [Ω], Nächster Normwert [Ω], Normreihe

### SMD-Widerstandscode

*3- und 4-stellig, R-Schreibweise und EIA-96* — `smd_resistor`

```text
3-stellig (E24): zwei Ziffern + Zehnerpotenz, z. B. 472 = 47 · 10² = 4,7 kΩ
4-stellig (E96, 1 %): drei Ziffern + Zehnerpotenz, z. B. 4702 = 47 kΩ
R = Dezimalkomma: 4R7 = 4,7 Ω, R47 = 0,47 Ω; 0 / 000 = 0-Ω-Brücke
EIA-96 (1 %): zwei Ziffern = Index der E96-Reihe (01 = 100 … 96 = 976) + Buchstabe: Z ×0,001, Y/R ×0,01, X/S ×0,1, A ×1, B/H ×10, C ×100, D ×1000, E ×10⁴, F ×10⁵. Beispiel 01C = 10 kΩ.
```

**Eingaben:** Aufdruck

**Ergebnisse:** Widerstand [Ω], Codierung

### Kondensator-Code

*Aufdruck → Kapazität, Toleranz, Spannung* — `capacitor_code`

```text
3 Ziffern (pF): zwei Ziffern + Zehnerpotenz, 104 = 10 · 10⁴ pF = 100 nF; 8 = ×0,01, 9 = ×0,1.
Buchstabe als Komma: 4n7 = 4,7 nF, 47p = 47 pF, 2u2 = 2,2 µF, 1R5 = 1,5 pF; Dezimalzahl (0.1, .047) = µF.
Toleranz: B ±0,1 pF, C ±0,25 pF, D ±0,5 pF, F ±1 %, G ±2 %, J ±5 %, K ±10 %, M ±20 %, Z +80/−20 %.
Spannung (EIA, vorangestellt): z. B. 1H = 50 V, 2A = 100 V, 2J = 630 V.
```

**Eingaben:** Aufdruck

**Ergebnisse:** Kapazität [nF], Kapazität in pF [pF], Toleranz, Nennspannung [V]

### Alphanumerischer Induktivitätscode

*101 = 100 µH, 4R7 = 4,7 µH, 47N = 47 nH* — `inductor_code`

```text
Grundeinheit µH: zwei Ziffern + Zehnerpotenz (101 = 10 · 10¹ µH = 100 µH); R = Komma in µH (4R7 = 4,7 µH, R10 = 0,1 µH); N = Komma in nH (47N = 47 nH, 4N7 = 4,7 nH). Nachgestellt Toleranz J ±5 %, K ±10 %, M ±20 %.
```

**Eingaben:** Aufdruck

**Ergebnisse:** Induktivität [µH], Induktivität in nH [nH]

### Stromteiler

*Aufteilung eines Stroms auf parallele Widerstände* — `current_divider`

```text
Parallelschaltung: U = I · R_ges, 1/R_ges = Σ 1/R_i    I_i = U / R_i
Zwei Widerstände: I₁ = I · R₂ / (R₁ + R₂).
```

**Eingaben:** Gesamtstrom I [A], R₁ [Ω], R₂ [Ω], R₃ (optional) [Ω]

**Ergebnisse:** Strom durch R₁ [A], Strom durch R₂ [A], Strom durch R₃ [A], Spannung U [V], Ersatzwiderstand R_ges [Ω]

### Zener-Diode als Spannungsstabilisator

*Vorwiderstand und Belastung der Z-Diode* — `zener_regulator`

```text
R_v = (U_e,min − U_Z) / (I_Z,min + I_L,max)  →  nächstkleinerer E12-Wert
I_Z,max = (U_e,max − U_Z) / R_v − I_L,min    P_Z = U_Z · I_Z,max    P_R = (U_e,max − U_Z)² / R_v
I_Z,min typ. 5 mA (bzw. 10 % von I_Z,max). Z-Diode nach P_Z mit Reserve wählen (z. B. 0,5 W, 1,3 W).
```

**Eingaben:** Eingangsspannung min. [V], Eingangsspannung max. [V], Zenerspannung U_Z [V], Min. Zenerstrom I_Z,min [mA], Laststrom min. [mA], Laststrom max. [mA], Belastbarkeit der Z-Diode (optional) [W]

**Ergebnisse:** Vorwiderstand (E12) [Ω], Vorwiderstand rechnerisch [Ω], Verlustleistung R_v [W], Max. Zenerstrom [mA], Max. Verlustleistung Z-Diode [W]

### Vorwiderstand zur Spannungsreduzierung

*R = (U − U_L) / I, Leistung und Normwert* — `series_resistor`

```text
R_v = (U − U_L) / I_L    P_R = (U − U_L) · I_L    Wirkungsgrad η = U_L / U
Nur für konstante Last geeignet – bei schwankendem Strom ändert sich U_L (dann Spannungsregler verwenden).
```

**Eingaben:** Versorgungsspannung U [V], Spannung am Verbraucher U_L [V], Strom des Verbrauchers I_L [mA]

**Ergebnisse:** Vorwiderstand R_v [Ω], Nächster E12-Wert [Ω], U_L mit E12-Wert [V], Verlustleistung P_R [W], Wirkungsgrad [%]

### Primär-/Sekundärwicklung von Transformatoren

*Übersetzung, Ströme und Windungszahlen (Kleintransformator)* — `transformer_windings`

```text
ü = U₁/U₂ = N₁/N₂ = I₂/I₁    Windungen je Volt w = 1 / (4,44 · f · B · A_Fe)
N₁ = w · U₁    N₂ = w · U₂ · (1 + Zuschlag) (Ausgleich der Spannungsabfälle, Kleintrafos 5–10 %)
B: Dynamoblech 1,0–1,4 T (Kernblech EI). Ohne Angabe A_Fe ≈ √S cm² (Faustformel für EI-Kerne).
```

**Eingaben:** Bemessungsleistung S [VA], Primärspannung U₁ [V], Sekundärspannung U₂ [V], Frequenz, Flussdichte B [T], Eisenquerschnitt A_Fe (optional) [cm²], Zuschlag Sekundärwindungen [%]

**Ergebnisse:** Übersetzung ü, Primärstrom I₁ [A], Sekundärstrom I₂ [A], Windungen je Volt [1/V], Windungen primär N₁, Windungen sekundär N₂, Verwendeter Eisenquerschnitt [cm²]

### Antennenlängenberechnung

*Wellenlänge und Länge von Dipol, Monopol (λ/4) und 5/8 λ* — `antenna_length`

```text
λ = c / f (c = 299 792 458 m/s)    l = Bruchteil · λ · k
k = Verkürzungsfaktor (Drahtdipol ≈ 0,95, dicke Rohre kleiner); λ/2-Dipol: Gesamtlänge, jeder Arm l/2.
```

**Eingaben:** Frequenz f [MHz], Antennenform, Verkürzungsfaktor k

**Ergebnisse:** Antennenlänge [cm], Wellenlänge λ [m], Länge ohne Verkürzung [cm]

### CCTV Speicherplatz- und Bandbreitenberechnung

*Bitrate, Netzwerklast und Speicher für Videoüberwachung* — `cctv_storage`

```text
Bitrate ≈ Pixel · Bilder/s · Bits je Pixel (Richtwerte: H.264 0,08, H.265 0,04, MJPEG 0,75 bit/Pixel; stark szenenabhängig – Herstellerwert eintragen, wenn bekannt).
Bandbreite = n · Bitrate    Speicher = n · Bitrate · Aufzeichnungsdauer · Anteil / 8 (1 TB = 10¹² Byte).
```

**Eingaben:** Anzahl Kameras, Auflösung, Bilder pro Sekunde [1/s], Videokompression, Bitrate je Kamera (Herstellerwert, optional) [Mbit/s], Aufzeichnung pro Tag [h], Anteil mit Aufzeichnung (Bewegung) [%], Speicherdauer [d]

**Ergebnisse:** Speicherbedarf [TB], Bitrate je Kamera [Mbit/s], Netzwerklast gesamt [Mbit/s], Speicher pro Tag [GB]

### Kupfer-Widerstandsthermometer (Cu10/Cu50/Cu100)

*R = R₀ · (1 + α · θ), linear* — `rtd_cu`

```text
Kupfer-Messwiderstände sind im Bereich −50 … +150 °C nahezu linear: R = R₀ · (1 + α · θ).
α = 0,00427 1/K (Cu10 nach US-Praxis), 0,00428 1/K (GOST 6651). Pt- und Ni-Fühler: eigener Rechner.
```

**Eingaben:** Fühler, Temperaturkoeffizient α, Gesucht, Temperatur θ [°C], Widerstand R [Ω]

**Ergebnisse:** Widerstand R [Ω], Temperatur θ [°C], Empfindlichkeit [Ω/K]

### Thermoelemente (Typ K, J, T, N, E, R, S, B)

*Thermospannung ↔ Temperatur mit Vergleichsstelle* — `thermocouple`

```text
Grundwerte nach IEC 60584-1 (= NIST ITS-90-Referenzfunktionen, Vergleichsstelle 0 °C).
Gemessene Spannung U = E(θ_M) − E(θ_V) mit θ_V = Temperatur der Vergleichsstelle (Klemmen). Rückrechnung: E(θ_M) = U + E(θ_V).
Farben der Ausgleichsleitungen nach IEC 60584-3: K grün, J schwarz, T braun, N rosa, E violett, R/S orange, B grau (Pluspol farbig, Minus weiß).
```

**Eingaben:** Typ, Gesucht, Messtemperatur θ_M [°C], Gemessene Spannung U [mV], Vergleichsstellentemperatur θ_V [°C]

**Ergebnisse:** Thermospannung U [mV], Messtemperatur θ_M [°C], E(θ_M) bezogen auf 0 °C [mV], E(θ_V) der Vergleichsstelle [mV], Empfindlichkeit bei θ_M [µV/K]

### Motorleistung

*Abgegebene und aufgenommene Leistung aus U, I, cos φ, η* — `motor_power`

```text
P_ab = k · U · I · cos φ · η    k = 1 (1~), √3 (3~); P_auf = P_ab / η; 1 hp = 745,7 W.
```

**Eingaben:** Netz, Spannung U [V], Strom I [A], Leistungsfaktor cos φ, Wirkungsgrad η [%]

**Ergebnisse:** Abgegebene Leistung P_ab [kW], Abgegebene Leistung [hp], Aufgenommene Leistung P_auf [kW], Scheinleistung S [kVA], Verluste [kW]

### Motorspannung

*Spannung aus Leistung, Strom, cos φ und η* — `motor_voltage`

```text
P_ab = k · U · I · cos φ · η    k = 1 (1~), √3 (3~); P_auf = P_ab / η; 1 hp = 745,7 W.
```

**Eingaben:** Netz, Abgegebene Leistung P_ab [kW], Strom I [A], Leistungsfaktor cos φ, Wirkungsgrad η [%]

**Ergebnisse:** Spannung U [V], Aufgenommene Leistung P_auf [kW]

### Leistungsfaktor (Motor)

*cos φ aus Leistung, Spannung, Strom und η* — `motor_pf`

```text
P_ab = k · U · I · cos φ · η    k = 1 (1~), √3 (3~); P_auf = P_ab / η; 1 hp = 745,7 W.
```

**Eingaben:** Netz, Spannung U [V], Abgegebene Leistung P_ab [kW], Strom I [A], Wirkungsgrad η [%]

**Ergebnisse:** Leistungsfaktor cos φ, Phasenwinkel φ [°], Aufgenommene Leistung P_auf [kW], Blindleistung Q [kvar]

### Motor-Volllaststrom (NEC)

*Tabellenstrom nach NEC 430.248 / 430.250 (Nordamerika)* — `nec_flc`

```text
NEC (NFPA 70) Tab. 430.248 (Einphasen-Wechselstrommotoren) und Tab. 430.250 (Drehstrom-Asynchronmotoren). Nach NEC 430.6(A)(1) sind für Leiter, Kurzschluss- und Erdschlussschutz die Tabellenwerte (nicht die Typenschildwerte) zu verwenden; Leiter für einen Motor ≥ 125 % des Tabellenstroms (430.22). Der Motorschutz (Überlast) richtet sich nach dem Typenschildstrom. 208/200 V = Werte für 208/200-V-Motoren. Gegen die gültige NEC-Ausgabe prüfen.
```

**Eingaben:** Phasen, Motorleistung, Motorleistung, Spannung, Spannung

**Ergebnisse:** Volllaststrom (Tabelle) [A], Leiter-Bemessungsstrom 125 % [A], Leistung [kW]

### Schaltschema Drehstrommotor (6 Klemmen)

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

### Schaltschema Drehstrommotor (9 Klemmen)

*NEMA T1–T9, Stern oder Dreieck, hohe/niedrige Spannung* — `motor_wiring_9`

```text
Anschlussbezeichnung nach NEMA MG 1 (Nordamerika), Doppelspannungsmotoren z. B. 230/460 V: Hohe Spannung = Wicklungshälften in Reihe, niedrige Spannung = parallel.
9 Klemmen: Stern (innerer Sternpunkt T7-T8-T9 intern) oder Dreieck (innere Dreieckecken T1, T2, T3).
12 Klemmen: alle Wicklungsenden herausgeführt; Stern oder Dreieck frei wählbar (auch Stern-Dreieck-Anlauf, Teilwicklungsanlauf). Typenschild und Anschlussplan des Herstellers haben Vorrang.
```

**Eingaben:** Schaltung, Spannung

**Ergebnisse:** L1, L2, L3, Zusammen verbinden (isoliert)

### Schaltschema Drehstrommotor (12 Klemmen)

*NEMA T1–T12, Stern/Dreieck, Reihe/parallel* — `motor_wiring_12`

```text
Anschlussbezeichnung nach NEMA MG 1 (Nordamerika), Doppelspannungsmotoren z. B. 230/460 V: Hohe Spannung = Wicklungshälften in Reihe, niedrige Spannung = parallel.
9 Klemmen: Stern (innerer Sternpunkt T7-T8-T9 intern) oder Dreieck (innere Dreieckecken T1, T2, T3).
12 Klemmen: alle Wicklungsenden herausgeführt; Stern oder Dreieck frei wählbar (auch Stern-Dreieck-Anlauf, Teilwicklungsanlauf). Typenschild und Anschlussplan des Herstellers haben Vorrang.
```

**Eingaben:** Schaltung, Spannung

**Ergebnisse:** L1, L2, L3, Zusammen verbinden (isoliert)

### Motor-Verbindungen (Drehrichtung)

*Anschluss und Drehrichtungsumkehr von Drehstrom- und Kondensatormotoren* — `motor_connections`

```text
Drehstrom (IEC 60034-8): L1→U1, L2→V1, L3→W1 ergibt Rechtslauf (Blick auf das Wellenende, Rechtsdrehfeld). Linkslauf: zwei Außenleiter tauschen (z. B. L1↔L2).
Einphasen-Kondensatormotor: Hauptwicklung U1–U2 an L–N, Hilfswicklung Z1–Z2 über den Betriebskondensator C_B parallel. Drehrichtung umkehren: Hilfswicklung umpolen (Z1↔Z2) – die tatsächliche Richtung zeigt das Anschlussbild des Herstellers.
```

**Eingaben:** Motor, Drehrichtung

**Ergebnisse:** Anschluss, Umkehr

### Dreieck-Stern-Umwandlung

*Δ ↔ Y für Widerstände oder Impedanzen, mit Zeichnung* — `delta_star`

```text
Δ → Y: R₁ = R₁₂ · R₃₁ / ΣR    R₂ = R₁₂ · R₂₃ / ΣR    R₃ = R₂₃ · R₃₁ / ΣR    (ΣR = R₁₂ + R₂₃ + R₃₁)
Y → Δ: R₁₂ = (R₁R₂ + R₂R₃ + R₃R₁) / R₃    R₂₃ = … / R₁    R₃₁ = … / R₂
Symmetrisch: R_Y = R_Δ / 3. Gilt ebenso für komplexe Impedanzen.
```

**Eingaben:** Umwandlung, R₁₂ (zwischen 1 und 2) [Ω], R₂₃ (zwischen 2 und 3) [Ω], R₃₁ (zwischen 3 und 1) [Ω], R₁ (an Knoten 1) [Ω], R₂ (an Knoten 2) [Ω], R₃ (an Knoten 3) [Ω]

**Ergebnisse:** R₁ [Ω], R₂ [Ω], R₃ [Ω], R₁₂ [Ω], R₂₃ [Ω], R₃₁ [Ω]

### Spannung

*Einheiten, Effektiv-/Scheitel-/Spitze-Spitze-Wert (Sinus) und dB-Pegel* — `conv_voltage`

```text
Sinus: Û = √2 · U_eff    U_ss = 2 · Û    Gleichrichtwert |ū| = 2Û/π = 0,9003 · U_eff
Pegel (Effektivwert): dBV = 20 · lg(U / 1 V), dBu = 20 · lg(U / 0,7746 V), dBmV = 20 · lg(U / 1 mV), dBµV = 20 · lg(U / 1 µV).
```

**Eingaben:** Wert, Einheit, Angabe als (Sinus)

**Ergebnisse:** Effektivwert [V], Scheitelwert Û [V], Spitze-Spitze [V], Gleichrichtwert [V], Effektivwert [mV], Effektivwert [kV], Pegel [dBV], Pegel [dBu], Pegel [dBmV], Pegel [dBµV]

### Phasenwinkel

*φ in Grad/Bogenmaß ↔ cos φ, sin φ, tan φ* — `conv_phase`

```text
φ [rad] = φ [°] · π/180    cos φ = P/S    sin φ = Q/S    tan φ = Q/P
Für Leistungsfaktoren gilt 0 ≤ φ ≤ 90° (induktiv oder kapazitiv – das Vorzeichen von Q angeben).
```

**Eingaben:** Wert, Gegeben

**Ergebnisse:** Phasenwinkel φ [°], Phasenwinkel φ [rad], cos φ, sin φ, tan φ = Q/P

### Zulässige spezifische Durchlassenergie k²S²

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

### Kupfer-Sammelschienen: Dauerstrom

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

### Mineralisolierte Kabel: Belastbarkeit

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

### Sicherungen: Betriebsklassen

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

### D- und NH-Sicherungen: Größen

| Größe | System | Gewinde / Bauform | Bemessungsstrom | Spannung |
|---|---|---|---|---|
| D01 | Neozed (D0) | E14 | 2–16 A | 400 V AC |
| D02 | Neozed (D0) | E18 | 20–63 A | 400 V AC |
| D03 | Neozed (D0) | M30 × 2 | 80–100 A | 400 V AC |
| DII | Diazed (D) | E27 | 2–25 A | 500 V AC |
| DIII | Diazed (D) | E33 | 35–63 A | 500 V AC |
| DIV | Diazed (D) | R 1¼" | 80–100 A | 500 V AC |
| NH000 | NH (Messer) | Größe 000 | bis 100 A | 500/690 V AC |
| NH00 | NH (Messer) | Größe 00 | bis 160 A | 500/690 V AC |
| NH0 | NH (Messer) | Größe 0 (nicht für Neuanlagen) | bis 160 A | 500/690 V AC |
| NH1 | NH (Messer) | Größe 1 | bis 250 A | 500/690 V AC |
| NH2 | NH (Messer) | Größe 2 | bis 400 A | 500/690 V AC |
| NH3 | NH (Messer) | Größe 3 | bis 630 A | 500/690 V AC |
| NH4a | NH (Messer) | Größe 4a | bis 1250 A | 500/690 V AC |

*IEC 60269-3 (D-System, für Laien bedienbar) und IEC 60269-2 (NH, nur Fachkräfte). Größte Bemessungsströme je Größe; Passschrauben/-hülsen verhindern beim D-System zu große Einsätze. Herstellerangaben prüfen.*

### Zylindersicherungen

| Abmessung | Bemessungsstrom bis | Spannung | Hinweis |
|---|---|---|---|
| 8,5 × 31,5 mm | 20 A | 400 V AC | kleine Verbraucher |
| 10 × 38 mm | 32 A | 400–690 V AC | auch gPV bis 1000/1500 V DC |
| 14 × 51 mm | 50 A | 400–690 V AC | auch aM für Motoren |
| 22 × 58 mm | 125 A | 400–690 V AC | auch aM für Motoren |

*IEC 60269-2 (zylindrische Sicherungseinsätze, z. B. für Sicherungs-Trennschalter). Höchste übliche Bemessungsströme; je nach Hersteller und Spannung kleiner – Datenblatt prüfen.*

### Feinsicherungen (G-Sicherungen)

| Zeichen | Charakteristik | Einsatz |
|---|---|---|
| FF | superflink | Halbleiter, Messgeräte |
| F | flink | ohne Einschaltstromspitzen |
| M | mittelträge | kleine Einschaltströme |
| T | träge | Netzteile, Transformatoren, Motoren |
| TT | superträge | hohe Einschaltströme |

*IEC 60127 / DIN EN 60127. Bauformen 5 × 20 mm und 6,3 × 32 mm. Schaltvermögen: L = niedrig (Glasrohr, typ. 35 A oder 10 · I_N), E = erhöht, H = hoch (Keramik mit Sandfüllung, 1500 A). Kennzeichnung z. B. T 1,6 A H 250 V.*

### Kfz-Flachsicherungen: Farben

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

### EIA-96-Code (SMD 1 %)

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

### Kondensatorcode-Tabelle

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

### Kondensator-Spannungscodes (EIA)

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

### NEC 430.250: Volllaststrom Drehstrommotoren

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

### NEC 430.248: Volllaststrom Einphasenmotoren

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

### Kennzeichnung der Motorklemmen

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

### AWG-Umrechnungstabelle

| AWG | d in mm | A in mm² | R in Ω/km | metrisch ≥ |
|---|---|---|---|---|
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

*ASTM B258: d = 0,127 mm · 92^((36 − n)/39), n = 0 für 1/0, −1 für 2/0 …; A = π/4 · d²; R für Kupfer (massiv) bei 20 °C mit ρ = 0,017241 Ω·mm²/m. Letzte Spalte: nächster metrischer Normquerschnitt ≥ AWG-Querschnitt.*

### SWG-Umrechnungstabelle

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

### Vergleich imperialer/metrischer Leitergrößen

| mm² (IEC) | Nächste US-Größe | deren mm² | Abweichung in % | US-Größe ≥ |
|---|---|---|---|---|
| 0,5 | 20 AWG | 0,52 | 3,5 | 20 AWG |
| 0,75 | 18 AWG | 0,82 | 9,7 | 18 AWG |
| 1 | 17 AWG | 1,04 | 3,8 | 17 AWG |
| 1,5 | 15 AWG | 1,65 | 10 | 15 AWG |
| 2,5 | 13 AWG | 2,62 | 5 | 13 AWG |
| 4 | 11 AWG | 4,17 | 4,3 | 11 AWG |
| 6 | 9 AWG | 6,63 | 10,6 | 9 AWG |
| 10 | 7 AWG | 10,55 | 5,5 | 7 AWG |
| 16 | 5 AWG | 16,77 | 4,8 | 5 AWG |
| 25 | 3 AWG | 26,67 | 6,7 | 3 AWG |
| 35 | 2 AWG | 33,63 | -3,9 | 1 AWG |
| 50 | 1/0 AWG | 53,48 | 7 | 1/0 AWG |
| 70 | 2/0 AWG | 67,43 | -3,7 | 3/0 AWG |
| 95 | 3/0 AWG | 85,03 | -10,5 | 4/0 AWG |
| 120 | 250 kcmil | 126,68 | 5,6 | 250 kcmil |
| 150 | 300 kcmil | 152,01 | 1,3 | 300 kcmil |
| 185 | 350 kcmil | 177,35 | -4,1 | 400 kcmil |
| 240 | 500 kcmil | 253,35 | 5,6 | 500 kcmil |
| 300 | 600 kcmil | 304,02 | 1,3 | 600 kcmil |

*Für jeden metrischen Normquerschnitt (IEC 60228) die nächstliegende AWG-/kcmil-Größe (logarithmisch) und die nächstgrößere. Größen sind nicht gleichwertig – Belastbarkeit immer nach der jeweiligen Norm (IEC bzw. NEC) bestimmen.*

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
Währung (€ oder CHF) und Wechselkurs in den Einstellungen; der Betrag wird zusätzlich in der anderen Währung angezeigt.
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

### Pumpe/Ventilator am FU: Proportionalitätsgesetze

*V̇ ~ n, Δp ~ n², P ~ n³ – Leistung und Einsparung bei Drehzahlregelung* — `fan_affinity`

```text
V̇₂/V̇₁ = n₂/n₁ · Δp₂/Δp₁ = (n₂/n₁)² · P₂/P₁ = (n₂/n₁)³
Gilt für quadratische Anlagenkennlinien mit gleichbleibender Dichte und Wirkungsgrad (Ventilatoren, geschlossene Kreisläufe). Mit statischem Anteil (Förderhöhe, Vordruck) nicht anwendbar.
Pumpen und Ventilatoren arbeiten meist bei 50 … 90 % Drehzahl.
Quellen: Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022), S. 180 (Beispiel 4000 → 8000 m³/h, 350 → 1400 Pa); Danfoss „Wissenswertes über Frequenzumrichter“, Kap. 3.
```

**Eingaben:** Volumenstrom V̇₁ [m³/h], Druckerhöhung Δp₁ [Pa], Leistungsaufnahme P₁ [kW], Drehzahl n₁ [1/min], Neuer Betriebspunkt aus, Drehzahl n₂ [1/min], Volumenstrom V̇₂ [m³/h], Betriebsstunden im neuen Punkt [h/a], Strompreis [€ / CHF]

**Ergebnisse:** Drehzahlverhältnis n₂/n₁ [%], Drehzahl n₂ [1/min], Volumenstrom V̇₂ [m³/h], Druckerhöhung Δp₂ [Pa], Leistungsaufnahme P₂ [kW], Energieeinsparung [kWh/a], Kosteneinsparung [€]

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
I_k ≈ c · I_r2 · 100 % / u_k   (starres Netz, c = 1,05);  Dauerkurzschlussstrom I_kD = I_r · 100 % / u_k (ohne c)
Übliche u_k (Haag Band 05 Trafo, 2007): Verteiltrafos 10 … 20 kV 4 … 6 %, 110 kV 10 … 14 %, 380 kV 11 … 20 %. Beispiel Haag: 400 kVA, u_k = 4 % → I_r = 577 A, I_kD = 14,43 kA. „Spannungssteife“ Trafos (kleines u_k) liefern höhere Kurzschlussströme.
```

**Eingaben:** Ausführung, Bemessungsleistung S_r [kVA], Oberspannung U_1 [V], Unterspannung U_2 [V], Kurzschlussspannung u_k [%]

**Ergebnisse:** Bemessungsstrom primär [A], Bemessungsstrom sekundär [A], Übersetzung ü, Kurzschlussstrom sekundär I_k [kA], Dauerkurzschlussstrom I_kD (ohne c) [kA]

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
Kleinste regelbare Menge: V̇_min/V̇_100 = 1/√(1 + a·(S_v² − 1)), S_v = Stellverhältnis kvs/kvr; Wärmeleistung mit Wärmetauscher-Kennwert a_WT: Q̇/Q̇_100 = 1/(1 + a_WT·(V̇_100/V̇ − 1)). Beispiel a = 0,5, S_v = 50: ≈ 3 % (ohne WT), ≈ 4 % (a_WT = 0,7). Zwei parallele Ventile 1/3 + 2/3 in Sequenz senken die minimale Leistung auf etwa ein Drittel (QM Holzheizwerke, FAQ 37, 2015).
```

**Eingaben:** Auslegung aus, Volumenstrom V̇ [m³/h], Leistung P [kW], Temperaturdifferenz ΔT (VL − RL) [K], Medium, Dichte ρ [kg/m³], Spez. Wärmekapazität c [kJ/(kg·K)], Gewünschter Δp am Ventil [kPa], Δp mengenvariable Strecke [kPa], Stellverhältnis S_v (kvs/kvr), Wärmetauscher-Kennwert a_WT

**Ergebnisse:** Volumenstrom V̇ [m³/h], Berechneter kv-Wert [m³/h], Gewählter kvs-Wert (Normreihe) [m³/h], Δp bei kvs (voll offen) [kPa], Ventilautorität a, Kleinster regelbarer Volumenstrom [%], Kleinste regelbare Wärmeleistung [%]

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

### h,x-Diagramm (Mollier) & Zustandsänderungen

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

**Eingaben:** Höhe über Meer [m], Temperatur t₁ [°C], Relative Feuchte φ₁ [%], Art, Zieltemperatur t₂ [°C], Ziel-Feuchte φ₂ [%], Temperatur Zustand B [°C], Relative Feuchte Zustand B [%], Anteil B am Massenstrom [%], Volumenstrom V̇ (bei Zustand 1) [m³/h]

**Ergebnisse:** Luftdruck p [hPa], Wassergehalt x₁ [g/kg], Taupunkt τ₁ [°C], Enthalpie h₁ [kJ/kg], Kühlgrenztemperatur t_f,1 [°C], Dichte ρ₁ [kg/m³], Temperatur t₂ [°C], Relative Feuchte φ₂ [%], Wassergehalt x₂ [g/kg], Enthalpie h₂ [kJ/kg], Taupunkt τ₂ [°C], Enthalpiedifferenz Δh [kJ/kg], Leistung Q (+ heizen / − kühlen) [kW], Wasser (+ Befeuchtung / − Kondensat) [kg/h]

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
Außenluft ≈ 400 ppm, in Städten oft 550 … 700 ppm (Siemens „Regeln und Steuern von Lüftungsanlagen“, 10/2022, Kap. 3.3.1); bedarfsgeregelte Lüftung über CO₂/VOC siehe VDMA 24773.
```

**Eingaben:** Verfahren, Personen, Raumfläche [m²], Raumhöhe (für Luftwechsel) [m], Kategorie, Gebäude-Emission, CO₂-Abgabe je Person [l/h], CO₂-Sollwert [ppm], CO₂ außen [ppm]

**Ergebnisse:** Außenluftvolumenstrom [m³/h], Außenluftvolumenstrom [l/s], Je Person [m³/h], Luftwechselrate [1/h], CO₂-Konzentration (stationär) [ppm]

### Reglereinstellung (Ziegler/Nichols, CHR)

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

### KNX-Topologie & Linienplanung

*Linien, Bereiche, Segmente, Leitungslängen und Netzteillast (KNX TP)* — `knx_topology`

```text
KNX TP: bis 64 Teilnehmer je Liniensegment mit eigener Spannungsversorgung; mit Linienverstärkern bis 4 Segmente = 255 Teilnehmer je Linie; bis 15 Linien je Bereich (Linienkoppler), bis 15 Bereiche (Bereichskoppler); Koppler zählen auf der Haupt-/Bereichslinie mit.
Leitungslängen je Segment: Spannungsversorgung – Teilnehmer ≤ 350 m, Teilnehmer – Teilnehmer ≤ 700 m, Gesamtlänge ≤ 1000 m.
Praxis: neue Linie ab ca. 64 Teilnehmern und Reserve einplanen.
Quelle: KNX Association „Grundlagenwissen zum KNX Standard“ (2013), S. 8–9. Stromaufnahme je Gerät laut Hersteller.
```

**Eingaben:** Anzahl Busteilnehmer, Reserve [%], Teilnehmer je Linie (Planung), Leitungslänge je Segment (gesamt) [m], Größter Abstand Netzteil – Teilnehmer [m], Größter Abstand Teilnehmer – Teilnehmer [m], Stromaufnahme je Teilnehmer [mA], Spannungsversorgung

**Ergebnisse:** Linien, Bereiche, Teilnehmer je Linie (max.), Segmente je Linie, Spannungsversorgungen (Linien), Netzteilauslastung je Segment [%]

### AMEV-Datenpunktadresse

*Benutzeradresse nach AMEV GA 2005 aus Gewerk, Anlage, Anlagenteil, Datenpunkt* — `amev_address`

```text
Aufbau (25 Stellen + Klartext): Liegenschaft (5) – Gebäude (3) – Gewerk (1) – Anlage (3) – lfd. Nr. (2) – Regelfabrikat (2) – Anlagenteil (3) – lfd. Nr. (2) – Datenpunktart (2) – Datenpunktbeschreibung (2).
Beispiel: 31021-008-H-HKR-01-01-PUU-01-SM-17 = Heizkreis 01, Umwälzpumpe 01, Störmeldung „Störung allg. elektrisch“.
Quelle: AMEV „Gebäudeautomation 2005“ (Nr. 87), Anhang 2.1/2.2, S. 78–85. Bezeichnungen wie im Original (nur deutsch); projektspezifisch anpassbar, neuere AMEV-Empfehlungen (z. B. BACnet) prüfen.
```

**Eingaben:** Liegenschaftsnummer, Gebäudenummer (999 = Außenanlagen), Gewerk, Anlage, Lfd. Nr. der Anlage, Regelfabrikat (Nr.), Anlagenteil, Lfd. Nr. des Anlagenteils, Datenpunktart, Datenpunktbeschreibung

**Ergebnisse:** Datenpunktadresse, Ohne Trennzeichen (GLT), Klartext (Vorschlag, max. 30 Zeichen)

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

### Einstellregeln Ziegler/Nichols & CHR

| Regel | Regler | Kp / V0 | Tn | Tv |
|---|---|---|---|---|
| ZN | P | T/(Ks·Tt) | – | – |
| ZN | PI | 0,9·T/(Ks·Tt) | 3,3·Tt | – |
| ZN | PD | 1,2·T/(Ks·Tt) | – | 0,25·Tt |
| ZN | PID | 1,2·T/(Ks·Tt) | 2·Tt | 0,42·Tt |
| CHR aperiodisch, Störung | P | V0 = 0,3·Tg/Tu | – | – |
| CHR aperiodisch, Störung | PI | V0 = 0,6·Tg/Tu | 4·Tu | – |
| CHR aperiodisch, Störung | PID | V0 = 0,95·Tg/Tu | 2,4·Tu | 0,42·Tu |
| CHR aperiodisch, Führung | P | V0 = 0,3·Tg/Tu | – | – |
| CHR aperiodisch, Führung | PI | V0 = 0,35·Tg/Tu | 1,2·Tg | – |
| CHR aperiodisch, Führung | PID | V0 = 0,6·Tg/Tu | Tg | 0,5·Tu |
| CHR 20 %, Störung | P | V0 = 0,7·Tg/Tu | – | – |
| CHR 20 %, Störung | PI | V0 = 0,7·Tg/Tu | 2,3·Tu | – |
| CHR 20 %, Störung | PID | V0 = 1,2·Tg/Tu | 2·Tu | 0,42·Tu |
| CHR 20 %, Führung | P | V0 = 0,7·Tg/Tu | – | – |
| CHR 20 %, Führung | PI | V0 = 0,6·Tg/Tu | Tg | – |
| CHR 20 %, Führung | PID | V0 = 0,95·Tg/Tu | 1,35·Tg | 0,47·Tu |

*Quelle: Siemens „Regeltechnik – Grundlagen“ (08/2022), Fig. 6-15 und 6-18, S. 82–83. CHR: V0 = Ks · Kp. ZN in der Xp-Form: P Xp = Xh·S, PI 1,25·Xh·S / Tn 3·Tt, PD 0,83·Xh·S, PID 0,83·Xh·S.*

### h,x-Diagramm: Höhenkorrektur

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

### AMEV: Gewerke

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

### AMEV: Anlagen

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

### AMEV: Anlagenteile

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

### AMEV: Datenpunktarten

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

### AMEV: Datenpunktbeschreibung

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

## Gebäudetechnik & Energie

### Hydraulische Grundschaltungen (Regelgruppen)

*Schema, Funktion, Einsatz und Ventilauslegung – mit Auswahlhilfe* — `hydraulic_circuits`

```text
Leistung Q̇ ~ V̇ · ΔT: Durchflussregelung (mengenvariabel: Drossel-, Umlenkschaltung) ändert den Volumenstrom, Mischregelung (mengenkonstant: Beimisch-, Einspritzschaltung) die Vorlauftemperatur.
Auswahl nach Verteiler (druckbehaftet: Drossel, Umlenk, Einspritz · drucklos: Beimisch), Frostgefahr (mengenkonstant), Entfeuchtung (mengenvariabel) und gewünschter Rücklauftemperatur (Brennwert/Fernwärme: Drossel oder Einspritz mit Durchgangsventil).
Quellen: Siemens „Einführung in die HLK- und Gebäudetechnik“ (08/2022), Kap. 5.4–5.5, S. 144–153; Belimo „Auswahltabelle von hydraulischen Grundschaltungen“ (03/2023). Prinzipskizzen ohne Absperr- und Sicherheitsarmaturen; Auslegung immer projektbezogen prüfen.
```

**Eingaben:** Anzeigen, Schaltung, Verteiler, Leistung anpassen über, Lufterwärmer mit Einfriergefahr, Luftkühler mit Entfeuchtung, Tiefe Rücklauftemperatur nötig (Brennwert, Fernwärme, Wärmepumpe), Verbraucher-VL deutlich tiefer als Erzeuger-VL (Flächenheizung), Konstanter Durchfluss im Erzeugerkreis nötig

**Ergebnisse:** Schaltung, Begründung, Alternative, Regelungsart, Verteiler, Vorlauftemperatur am Verbraucher, Durchfluss Verbraucherkreis, Durchfluss Erzeugerkreis, Pumpen, Rücklauftemperatur zum Erzeuger, Funktionsweise, Eigenschaften, Typische Anwendungen, Ventil- und Bypassauslegung, Hinweis

### GA-Effizienzklasse (EN ISO 52120)

*Einsparung durch bessere Gebäudeautomation – BAC-Faktoren je Gebäudetyp* — `bac_efficiency`

```text
Energiebedarf neu = Energiebedarf alt · f_BAC,neu / f_BAC,alt;  Einsparung = 1 − f_neu / f_alt
f_BAC,th: Heizen und Kühlen; f_BAC,el: Beleuchtung und Hilfsenergie (ohne sonstige Geräte).
Klasse C = 1 (Standard-Referenz), D schlechter, B und A besser.
Quelle: Siemens „Gebäudeautomation – Einfluss auf Energie-Effizienz von Gebäuden, Anwendung der EN ISO 52120“ (09/2022), Kap. 4.4, S. 124–125. Gegen die gültige Ausgabe der EN ISO 52120-1 prüfen.
```

**Eingaben:** Gebäudetyp, GA-Klasse heute, GA-Klasse geplant, Thermischer Energiebedarf heute [kWh/a], Elektrischer Energiebedarf heute [kWh/a], Preis Wärme/Kälte [€ / CHF], Preis Strom [€ / CHF]

**Ergebnisse:** f_BAC,th heute, f_BAC,th geplant, Einsparung thermisch [%], Thermischer Bedarf geplant [kWh/a], f_BAC,el heute, f_BAC,el geplant, Einsparung elektrisch [%], Elektrischer Bedarf geplant [kWh/a], Kosteneinsparung pro Jahr [€]

### Heizkennlinie: Steilheit & Parallelverschiebung

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

### Wärmepumpe: COP, Carnot & JAZ

*Leistungszahl aus Temperaturen, Mindest-COP nach EN 14511, Jahresarbeitszahl* — `heat_pump`

```text
COP_Carnot = T_Senke / (T_Senke − T_Quelle) (Temperaturen in K); realer COP ≈ Gütegrad · COP_Carnot.
COP = Φ_c / P_el (Momentanwert für einen Betriebspunkt);  JAZ = Wärme / Strom über ein Jahr (kWh/kWh).
Mindest-COP für das Keymark-Gütesiegel (EN 14511): Luft/Wasser > 3,1 (A2/W35), Sole/Wasser > 4,3 (B0/W35), Wasser/Wasser > 5,1 (W10/W35). Typische JAZ (Fraunhofer 2011, AZ 2): Luft/Wasser 2,9, Sole/Wasser 3,9.
Quelle: Siemens „Kältetechnik“ (08/2022), Kap. 7.6, S. 112–115. Gütegrad ist eine Annahme – Herstellerdaten verwenden.
```

**Eingaben:** Wärmequelle, Quellentemperatur [°C], Vorlauftemperatur (Senke) [°C], Gütegrad (Annahme), COP laut Datenblatt (Prüfpunkt), Erzeugte Wärme pro Jahr [kWh/a], Stromverbrauch pro Jahr [kWh/a]

**Ergebnisse:** COP Carnot (ideal), COP geschätzt (Gütegrad), Mindest-COP Keymark (Prüfpunkt), Jahresarbeitszahl JAZ, Typische JAZ (Praxis)

### RLT-Gerät & Luftkanal

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

### Nachhallzeit (Sabine) & DIN 18041

*Ist- und Soll-Nachhallzeit, nötige Absorberfläche* — `reverberation`

```text
Sabine: T = 0,163 s/m · V / A,   A = α · S (äquivalente Absorptionsfläche)
Soll-Nachhallzeit DIN 18041 (besetzter Raum, mittlere Frequenzen, Toleranz ±20 %):
A1 Musik T = 0,45·lg V + 0,07 · A2 Sprache/Vortrag T = 0,37·lg V − 0,14 · A3 Unterricht/Kommunikation T = 0,32·lg V − 0,17 · A4 Unterricht inklusiv T = 0,26·lg V − 0,14 (A4 seit DIN 18041:2016).
Quelle: BAuA-Forschungsbericht Fb 1030 „Lärm in Bildungsstätten“ (2004), Kap. 3, S. 19–20 (Tab. 3.1: 150 m³ → 0,53 s); im Buch S. 50 sind die Vorzeichen der Konstanten verdruckt. Gegen DIN 18041:2016 prüfen.
```

**Eingaben:** Raumlänge [m], Raumbreite [m], Raumhöhe [m], Nutzung (DIN 18041), Mittlerer Absorptionsgrad Raumflächen, Absorptionsgrad Absorber/Akustikdecke, Vorhandene Absorberfläche [m²]

**Ergebnisse:** Raumvolumen [m³], Nachhallzeit Ist [s], Nachhallzeit Soll [s], Toleranz obere Grenze (+20 %) [s], Äquivalente Absorptionsfläche Ist [m²], Nötige Absorptionsfläche [m²], Zusätzliche Absorberfläche [m²]

### Schallpegel addieren & Entfernung

*Energetische Summe mehrerer Quellen, Abnahme mit der Entfernung* — `sound_levels`

```text
L_Σ = 10 · lg Σ 10^(L_i/10)   (zwei gleiche Quellen: +3 dB)
Punktquelle im Freifeld: L₂ = L₁ − 20 · lg(r₂/r₁)   (Verdopplung der Entfernung: −6 dB)
Grundlagen der Akustik; vgl. BAuA Fb 1030 „Lärm in Bildungsstätten“ (2004).
```

**Eingaben:** Pegel Quelle 1 [dB], Pegel Quelle 2 [dB], Pegel Quelle 3 [dB], Pegel Quelle 4 [dB], Abstand der Messung [m], Neuer Abstand [m]

**Ergebnisse:** Summenpegel [dB], Summenpegel im neuen Abstand [dB]

### Fluchtweg: Breite & Länge (ASR A2.3)

*Mindestbreiten nach Personenzahl, maximale Fluchtweglänge* — `escape_route`

```text
ASR A2.3 (Fassung 03/2022, geändert 2024), Abschn. 5: Tab. 1 Mindestbreiten (Spalte B Türen/Durchgänge, Spalte C Hauptfluchtweg): bis 5 P. 0,80/0,90 · bis 20 P. 0,90/1,00 · bis 50 P. 0,90/1,20 · bis 100 P. 1,00/1,20 · bis 200 P. 1,05/1,20 · bis 300 P. 1,65/1,80 · bis 400 P. 2,25/2,40 m; über 200 Personen linear interpolierbar. Bei Neubau für bis 5 Personen auch an Türen 0,90 m empfohlen (Barrierefreiheit).
Hauptfluchtweglänge (Luftlinie): 35 m (normal oder erhöhte Brandgefährdung mit Löschanlage), 25 m (erhöhte Brandgefährdung ohne Löschanlage), 10 m (explosionsgefährliche Stoffe); tatsächliche Laufweglänge ≤ 1,5-fach.
Das Buch (ASR A2.3:2007) nennt noch eine Spalte: 0,875 / 1,00 / 1,20 / 1,80 / 2,40 m und 20 m für giftstoff- und explosionsgefährdete Räume. Bauordnungsrecht kann längere Rettungswege zulassen.
```

**Eingaben:** Personen im Einzugsgebiet, Raumart, Fluchtweglänge (Luftlinie) [m], Tatsächliche Laufweglänge [m], Vorhandene lichte Breite Flur [m]

**Ergebnisse:** Mindestbreite Hauptfluchtweg [m], Mindestbreite Türen/Durchgänge [m], Max. Fluchtweglänge [m], Max. Laufweglänge [m]

### Aufzugsschacht: Rauchabzug & Lüftung

*Freier Querschnitt der Schachtöffnung nach MBO bzw. VKF* — `lift_shaft`

```text
DE – MBO § 39 Abs. 3: Öffnung zur Rauchableitung ≥ 2,5 % der Fahrschachtgrundfläche, mindestens 0,10 m² (Landesbauordnung prüfen). Lüftung nach DIN EN 81-1/-2 (2010): Empfehlung 1 % der Schachtgrundfläche am oberen Ende.
CH – VKF-Brandschutzrichtlinie (Stand Blatt 2007): lichter Querschnitt 5 % des Schachtquerschnitts, max. 1600 cm²; Maschinen-/Rollenraum separat entlüftet mindestens 500 cm².
Quellen: ZVEI/VDMA-Leitfaden „Rauchableitung und Lüftung von Aufzugsschächten“ (07/2012), S. 3; IG-BSK-Arbeitsblatt D_AE_001 (03/2007). EN 81-1/-2 sind durch EN 81-20/-50 ersetzt; gültige Fassungen prüfen.
```

**Eingaben:** Schachtbreite (innen) [m], Schachttiefe (innen) [m]

**Ergebnisse:** Schachtgrundfläche [m²], Freier Querschnitt Rauchabzug [m²], Freier Querschnitt Rauchabzug [cm²], Lüftung (Empfehlung 1 %) [m²]

### Auswahltabelle hydraulische Grundschaltungen

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

### GA-Effizienzfaktoren f_BAC (EN ISO 52120)

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

### Wärmepumpen: Mindest-COP & typische JAZ

| Wärmepumpe | Prüfpunkt | Mindest-COP | Typische JAZ |
|---|---|---|---|
| Luft/Wasser | A2/W35 | > 3,1 | 2,9 |
| Sole/Wasser | B0/W35 | > 4,3 | 3,9 |
| Wasser/Wasser | W10/W35 | > 5,1 | – |

*Quelle: Siemens „Kältetechnik“ (08/2022), S. 113–115 (JAZ: Studie Fraunhofer-Institut 2011, AZ 2). Prüfpunkt: A = Außenluft, B = Sole, W = Wasser; Zahl = Eintrittstemperatur Quelle bzw. Vorlauf in °C. Gegen gültige Ausgabe prüfen.*

### CO₂-Konzentrationen & Richtwerte

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

### RLT: Geschwindigkeits- und SFP-Klassen

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

### Fluchtwegbreiten ASR A2.3

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

## R&I-Kennzeichnung & Ex-Schutz

### R&I-Kennbuchstaben-Finder

*Name eingeben (z. B. Temperatur Zuluft, Gas Methan, Leckage) → ISA, EN 62424, VDI 3814* — `pid_tags`

```text
Erster Buchstabe = Messgröße (T Temperatur, P Druck, F Durchfluss, L Stand, M Feuchte, A/Q Analyse …), Folgebuchstaben = Funktion: E Element, T Transmitter, I Anzeige, C Regelung, S Schalter, A Alarm, H/L hoch/tief, Q Summe. Tag = Buchstaben + Kreisnummer, z. B. TT-101.
Für die GA zusätzlich Bauteil-Kürzel und Benutzeradresse nach VDI 3814-4.1 (Kürzel + Nr. _ Funktion _ Nr.).
Kennbuchstaben nach ANSI/ISA-5.1-2009 und DIN EN 62424 (Nachfolgerin von DIN 19227-1); GA-Kürzel nach VDI 3814 Blatt 4.1 Tab. 3/4 (Beispiele), Klassen nach IEC 81346-2 wie in VDI 3814-4.1. Projektspezifische Festlegungen haben Vorrang.
```

**Eingaben:** Messstelle / Gerät (Name), Funktion, Ort / Darstellung, Kreisnummer

**Ergebnisse:** Erkannt, Kennbuchstaben ISA-5.1, Beispiel-Tag (ISA), Bedeutung, Kennbuchstaben EN 62424 / DIN 19227, GA-Kürzel VDI 3814-4.1, Benutzeradresse (Bauteil_Funktion), Klasse IEC 81346-2, Signal, PI-Symbol, Weitere Treffer

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

### Währung € ↔ CHF

*Euro und Schweizer Franken umrechnen* — `conv_currency`

```text
CHF = € · Kurs    € = CHF / Kurs    (Kurs = Franken für 1 Euro)
Ohne Eingabe gilt der Kurs aus den Einstellungen (dort für alle Kostenrechnungen verwendet). Stand Oktober 2026 etwa 0,92 CHF/€ – Tageskurs der Bank verwenden.
```

**Eingaben:** Betrag, Von, Kurs 1 € in CHF [CHF]

**Ergebnisse:** Ergebnis, Verwendeter Kurs 1 € = [CHF], Umkehrkurs 1 CHF = [€]

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

### Querschnitt / Fläche

*mm², cm², m², in², kcmil (MCM), cmil* — `conv_area`

```text
Umrechnung über m². 1 in = 25,4 mm (exakt); 1 cmil = Fläche eines Kreises von 1 mil (0,001 in) Durchmesser = π/4 · (25,4 µm)² = 5,0671 · 10⁻⁴ mm²; 1 kcmil (MCM) = 1000 cmil = 0,5067 mm².
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Quadratmillimeter [mm²], Quadratzentimeter [cm²], Quadratmeter [m²], Quadratzoll [in²], Quadratfuß [ft²], Kilo-Circular-Mil (MCM) [kcmil], Circular Mil [cmil]

### Länge

*mm, m, km, mil, in, ft, yd, mi, sm* — `conv_length`

```text
Umrechnung über Meter. Zoll 25,4 mm, Fuß 0,3048 m, Yard 0,9144 m, Meile 1609,344 m, Seemeile 1852 m (alle exakt definiert).
```

**Eingaben:** Wert, Einheit

**Ergebnisse:** Mikrometer [µm], Millimeter [mm], Zentimeter [cm], Meter [m], Kilometer [km], Mil (1/1000 Zoll) [mil], Zoll [in], Fuß [ft], Yard [yd], Meile [mi], Seemeile [sm]

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

### 5 Sicherheitsregeln & Schutzabstände

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

