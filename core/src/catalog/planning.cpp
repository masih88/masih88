// Area "Entwurfsplanung (Lph 3)" (v1.12): design planning of building services (HOAI stage 3) after
// M. Vielhauer, H. Geyer, T. Hürst, M. Adelmann, "Entwurfsplanung der Technischen Gebäudeausrüstung – Planungsinhalte,
// Beispiele und Leistungsdefinitionen", Springer Vieweg 2025 (doi 10.1007/978-3-658-45464-7). Contents are summarised in
// our own words with the chapter given; the HOAI list is quoted from the official text (HOAI 2021, Anlage 15).
// Calculators: GA cabinet supply (book Abb. 11.10), GA cabinet area (Abb. 11.12), KG 480 cost calculation (11.2.10).
#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/sketch.hpp"
#include "elc/formulas/cables.hpp"

namespace elc::catalog {

namespace {

constexpr const char* kCat = "planning";
const Text kBook{"Vielhauer/Geyer/Hürst/Adelmann, Entwurfsplanung der Technischen Gebäudeausrüstung, Springer Vieweg 2025",
                 "Vielhauer/Geyer/Hürst/Adelmann, Entwurfsplanung der Technischen Gebäudeausrüstung, Springer Vieweg 2025"};

Text cat(std::initializer_list<Text> parts) {
    Text t;
    for (const auto& p : parts) {
        t.de += p.de;
        t.en += p.en;
    }
    return t;
}

Table make(std::string id, Text title, Text summary, Text description, std::vector<Text> columns, std::vector<std::vector<Text>> rows) {
    Table t;
    t.id = std::move(id);
    t.category = kCat;
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = std::move(description);
    t.columns = std::move(columns);
    t.rows = std::move(rows);
    return t;
}

// ------------------------------------------------------------------ general tables

Table services() {
    return make(
        "lph3_services", {"HOAI Lph 3: Grundleistungen Technische Ausrüstung", "HOAI stage 3: basic services building services"},
        {"Grundleistungen a) bis h) der Entwurfsplanung und was sie praktisch bedeuten", "Basic services a) to h) of the design stage and what they mean"},
        cat({{"Wortlaut nach HOAI 2021, Anlage 15 (zu § 55 Abs. 3, § 56 Abs. 3), Leistungsphase 3 „Entwurfsplanung (System- und "
              "Integrationsplanung)“ – das Leistungsbild hat sich gegenüber der HOAI 2013 nicht verändert. Spalte „In der Praxis“ "
              "zusammengefasst nach ",
              "Wording per HOAI 2021, annex 15 (re § 55 (3), § 56 (3)), stage 3 \"design planning (system and integration "
              "planning)\" – unchanged from HOAI 2013. Column \"In practice\" summarised after "},
             kBook,
             {", Kap. 4.2: Kernphase der TA-Planung – vollständiger Entwurf, alle Systeme festgelegt, alle Berechnungen, Platzbedarf "
              "und Tragwerk abgestimmt, Behörden vorab verhandelt. Die HOAI ist Honorarrecht; als Leistungsbild gilt sie nur, wenn "
              "der Vertrag darauf verweist.",
              ", ch. 4.2: core stage of building services design – complete design, all systems fixed, all calculations, space "
              "and structure coordinated, authorities consulted. The HOAI is fee law; it defines the services only if the "
              "contract refers to it."}}),
        {{"Nr.", "No."}, {"Grundleistung (HOAI 2021)", "Basic service (HOAI 2021)"}, {"In der Praxis", "In practice"}},
        {{"a)",
          {"Durcharbeiten des Planungskonzepts (stufenweise Erarbeitung einer Lösung) unter Berücksichtigung aller fachspezifischen "
           "Anforderungen sowie unter Beachtung der durch die Objektplanung integrierten Fachplanungen bis zum vollständigen Entwurf",
           "Working through the concept (step-by-step solution) considering all trade-specific requirements and the specialist "
           "designs integrated by the architect up to the complete design"},
          {"Alle Fachplanungen auf gleichem Stand, Integration durch den Objektplaner rechtzeitig", "All trades at the same level, integrated in time by the architect"}},
         {"b)", {"Festlegen aller Systeme und Anlagenteile", "Fixing all systems and plant components"},
          {"Endgültige Systementscheidung, damit Lph 5 ohne grundsätzliche Änderungen anschließt – nicht jedes Einzelteil",
           "Final decision on the systems so that stage 5 follows without fundamental changes – not every single part"}},
         {"c)",
          {"Berechnen und Bemessen der technischen Anlagen und Anlagenteile; Abschätzen von jährlichen Bedarfswerten (z. B. Nutz-, "
           "End- und Primärenergiebedarf) und Betriebskosten; Abstimmen des Platzbedarfs; zeichnerische Darstellung des Entwurfs "
           "im abgestimmten Ausgabemaßstab mit maßbestimmenden Dimensionen; Fortschreiben und Detaillieren der Funktions- und "
           "Strangschemata; Auflisten aller Anlagen mit technischen Daten; Anlagenbeschreibungen mit Angabe der Nutzungsbedingungen",
           "Calculating and sizing plant and components; estimating annual demand (useful, final, primary energy) and operating "
           "costs; coordinating space; drawings at the agreed scale with dimension-defining sizes; updating and detailing the "
           "function and riser schematics; listing all plant with technical data; plant descriptions with conditions of use"},
          {"Heiz-/Kühllast, Hauptleitungen und Netze bemessen, Leistungsbilanz; Pläne in der Regel 1:100",
           "Heating/cooling loads, mains and networks sized, power balance; drawings usually 1:100"}},
         {"d)",
          {"Übergeben der Berechnungsergebnisse an andere Planungsbeteiligte zum Aufstellen vorgeschriebener Nachweise; Angabe und "
           "Abstimmung der für die Tragwerksplanung notwendigen Angaben über Durchführungen und Lastangaben (ohne Anfertigen von "
           "Schlitz- und Durchführungsplänen)",
           "Handing over results for required certificates; stating and coordinating openings and loads for the structural "
           "design (without drawing slot and opening plans)"},
          {"Gewichte, Aufstellflächen und Durchbrüche an Tragwerksplanung; Ergebnisse an Bauphysik/Brandschutz",
           "Weights, footprints and openings to the structural engineer; results to building physics/fire safety"}},
         {"e)", {"Verhandlungen mit Behörden und anderen zu beteiligten Stellen über die Genehmigungsfähigkeit",
                 "Negotiations with authorities and other bodies about approvability"},
          {"Vorabstimmung mit Behörden, Versorgern und Prüfsachverständigen", "Prior consultation with authorities, utilities and inspection experts"}},
         {"f)", {"Kostenberechnung nach DIN 276 (3. Ebene) und Terminplanung", "Cost calculation per DIN 276 (3rd level) and scheduling"},
          {"Mengen müssen sich in den Plänen wiederfinden lassen", "Quantities must be traceable in the drawings"}},
         {"g)", {"Kostenkontrolle durch Vergleich der Kostenberechnung mit der Kostenschätzung", "Cost control by comparing the cost calculation with the estimate"},
          {"Abweichungen erklären können", "Be able to explain deviations"}},
         {"h)", {"Zusammenfassen, Erläutern und Dokumentieren der Ergebnisse", "Summarising, explaining and documenting the results"},
          {"Erläuterungsbericht mit Anlagenbeschreibungen", "Explanatory report with plant descriptions"}}});
}

Table premises() {
    return make("lph3_premises", {"Qualitätsprämissen der Entwurfsplanung", "Quality premises of the design stage"},
                {"Acht Prämissen, an denen sich eine vollständige Lph 3 messen lässt", "Eight premises a complete stage 3 is measured against"},
                cat({{"Zusammengefasst nach ", "Summarised after "}, kBook,
                     {", Kap. 4.6 (abgeleitet aus HOAI, HOAI-Kommentaren, VDI 6026 und Rechtsprechung). Gelten für alle Gewerke.",
                      ", ch. 4.6 (derived from the HOAI, HOAI commentaries, VDI 6026 and case law). Apply to all trades."}}),
                {{"Nr.", "No."}, {"Prämisse", "Premise"}, {"Woran man es prüft", "How to check it"}},
                {{"1", {"Änderungsfreie Weiterplanung", "Further planning without changes"},
                  {"Lph 5 kann ohne grundsätzliche Änderungen auf dem Entwurf aufbauen", "Stage 5 can build on the design without fundamental changes"}},
                 {"2", {"Alle Systeme und Anlagenteile endgültig festgelegt", "All systems and components finally fixed"},
                  {"Keine offenen Varianten mehr bei Erzeugern, Netzen und Hauptkomponenten", "No open variants for generators, networks and main components"}},
                 {"3", {"Berechnungsergebnisse für alle Anlagenteile liegen sorgfältig vor", "Calculation results available for all components"},
                  {"Lasten, Netze und Komponenten bemessen und dokumentiert", "Loads, networks and components sized and documented"}},
                 {"4", {"Lasten und statische Kräfte mit der Tragwerksplanung abgestimmt", "Loads and forces coordinated with the structural design"},
                  {"Gewichte, Aufstellflächen und Durchbrüche übergeben und bestätigt", "Weights, footprints and openings handed over and confirmed"}},
                 {"5", {"Vorverhandlung mit den Behörden erfolgt", "Prior negotiation with the authorities done"},
                  {"Genehmigungsfähigkeit geklärt (Brandschutz, Wasser, Emissionen)", "Approvability clarified (fire, water, emissions)"}},
                 {"6", {"Verortung festgelegt; Pläne, Schemata, Berechnung und Beschreibung in sich schlüssig",
                        "Locations fixed; drawings, schematics, calculation and description consistent"},
                  {"Gleiche Zahlen und Bezeichnungen in allen Dokumenten", "Same figures and names in every document"}},
                 {"7", {"Aufgabe umfänglich bearbeitet, Integration und Koordination abgeschlossen", "Task fully covered, integration and coordination complete"},
                  {"Kollisionsfreiheit nachgewiesen (Grundrisse, Schnitte, 3D)", "Freedom from collisions proven (plans, sections, 3D)"}},
                 {"8", {"Themen mit erheblicher Kostenwirkung vom Fachplaner bearbeitet", "Topics with major cost impact handled by the specialist"},
                  {"Keine kostenrelevanten Punkte offen oder auf die Ausführung verschoben", "No cost-relevant points left open or deferred to execution"}}});
}

Table formal() {
    return make("lph3_formal", {"Formale Anforderungen und Darstellung (Lph 3)", "Formal requirements and presentation (stage 3)"},
                {"Vollständigkeit, Legende, Maßstab, 3-Strich-Darstellung, Schnittstellen, Kosten, Termine", "Completeness, legend, scale, 3-line drawing, interfaces, cost, schedule"},
                cat({{"Zusammengefasst nach ", "Summarised after "}, kBook,
                     {", Kap. 2 und 4.7, mit VDI 6026 Blatt 1 (2022), Tabelle 5.1. Darstellungsart und Detailtiefe je Gewerk vertraglich "
                      "festlegen – für alle Fachplaner gleich.",
                      ", ch. 2 and 4.7, with VDI 6026 part 1 (2022), table 5.1. Agree presentation and level of detail per trade in the "
                      "contract – the same for all designers."}}),
                {{"Thema", "Topic"}, {"Anforderung", "Requirement"}, {"Kapitel", "Chapter"}},
                {{{"Vollständigkeit", "Completeness"}, {"Alle Zeichnungen, Schemata, Berechnungen und Spezifikationen, die zum Verständnis nötig sind", "All drawings, schematics, calculations and specifications needed"}, "4.7"},
                 {{"Klarheit", "Clarity"}, {"Für Auftraggeber, Errichter und Prüfer verständlich; Fachbegriffe und Abkürzungen erklären", "Understandable for client, installer and checker; explain terms and abbreviations"}, "4.7"},
                 {{"Normkonformität", "Conformity"}, {"Geltende Normen, Vorschriften und technische Regeln einhalten", "Comply with current standards and rules"}, "4.7"},
                 {{"Kennzeichnung", "Identification"}, {"Eindeutige Nummerierung und Versionsstand jedes Dokuments; Planliste", "Unique numbering and revision of each document; drawing list"}, "4.7"},
                 {{"Struktur und Prüfbarkeit", "Structure and checkability"}, {"Logische Gliederung, Verzeichnisse; Nachweise so, dass Prüfingenieure und Sachverständige prüfen können", "Logical structure, indexes; evidence that checkers can verify"}, "4.7"},
                 {{"Legende", "Legend"}, {"Alle verwendeten Symbole aufführen und laienverständlich beschreiben", "List every symbol used and describe it in plain words"}, "4.7"},
                 {{"Maßstab", "Scale"}, {"Grundrisse in der Regel 1:100 mit maßbestimmenden Dimensionen", "Floor plans usually 1:100 with dimension-defining sizes"}, "2.3"},
                 {{"Strichdarstellung", "Line representation"},
                  {"3-Strich (Rohrachse + Außenkante Dämmung) empfohlen, bemaßt auf die Achse; 5-Strich bei 1:100 nicht lesbar; 1-Strich nur bei kleinen Maßnahmen mit Schnitten",
                   "3-line (pipe axis + outer edge of insulation) recommended, dimensioned to the axis; 5-line unreadable at 1:100; single line only for small jobs with sections"},
                  {"2.3, VDI 6026", "2.3, VDI 6026"}},
                 {{"Kollisionsfreiheit", "Freedom from collisions"},
                  {"Maßgebend sind rohrführende Gewerke (Heizung, Kälte, Sanitär, Sprinkler) und Lüftungskanäle; Nachweis über Grundrisse, Schnitte oder 3D",
                   "Governed by piped trades (heating, cooling, plumbing, sprinkler) and ducts; proof by plans, sections or 3D"},
                  "2.3"},
                 {{"Bautoleranzen", "Construction tolerances"}, {"Bei Schächten, Doppel-/Hohlraumböden und Deckenkoffern mitdenken und ggf. einzeichnen", "Allow for them at shafts, raised floors and bulkheads, draw where needed"}, "2.2"},
                 {{"Schnittstellen", "Interfaces"}, {"Schnittstellenliste, -piktogramm und -grundriss zwischen den Gewerken", "Interface list, pictogram and plan between the trades"}, "2.4"},
                 {{"Kosten", "Cost"}, {"Kostenberechnung DIN 276 in der 3. Ebene; Mengen aus den Plänen nachvollziehbar", "Cost calculation DIN 276 at level 3; quantities traceable in the drawings"}, "2.5"},
                 {{"Betriebskosten", "Operating cost"}, {"Jährliche Bedarfswerte und Energiekennwerte abschätzen", "Estimate annual demand and energy indicators"}, "2.6"},
                 {{"Termine", "Schedule"}, {"Planungs- und Bauzeitenterminplan fortschreiben, Zeiträume für Qualitätssicherung vorsehen", "Update design and construction schedules, allow time for quality assurance"}, "2.7"}});
}

Table documents() {
    return make("lph3_documents", {"Planungsdokumente Lph 3 je Gewerk", "Stage 3 documents per trade"},
                {"Schemata, Grundrisse, Berechnungen und Bericht für KG 410 bis 480", "Schematics, plans, calculations and report for cost groups 410 to 480"},
                cat({{"Checkliste, zusammengefasst nach ", "Checklist summarised after "}, kBook,
                     {", Kap. 5 bis 11 (Abschnitte „Planungsdokumente“). Jede Zeile: was in der Entwurfsplanung mindestens vorliegen "
                      "sollte. Zusätzlich je Gewerk Planliste und Dokumentenliste passend zu allen Grundrissen, Schemata und Berechnungen.",
                      ", ch. 5 to 11 (sections \"planning documents\"). Each row: what a design stage should contain at least. Plus a "
                      "drawing list and document list per trade matching all plans, schematics and calculations."}}),
                {"KG", {"Schemata", "Schematics"}, {"Grundrisse / Schnitte", "Plans / sections"}, {"Berechnung / Bemessung", "Calculation / sizing"},
                 {"Bericht / Sonstiges", "Report / other"}},
                {{{"410 Abwasser", "410 waste water"},
                  {"Fall-, Sammel- und Grundleitungen mit Nennweiten, Hebeanlagen, Rückstauverschlüsse, Abscheider, Kanalanschlüsse",
                   "Stacks, collectors and drains with sizes, lifting plants, backflow stops, separators, sewer connections"},
                  {"Leitungen zu massiven Bauteilen bemaßt, DN und Gefälle, Sohlhöhen, Fließrichtung an Fallleitungen, Durchbrüche, Dacheinläufe mit Einzugsfläche",
                   "Pipes dimensioned to solid elements, DN and slope, invert levels, flow arrows at stacks, openings, roof outlets with catchment area"},
                  {"Schmutz- und Regenwasser (DIN 1986-100, DIN EN 12056), Abflusskennzahl K 0,5/0,7/1,0, Hebeanlagen, Fettabscheider, ggf. Versickerung",
                   "Foul and rain water (DIN 1986-100, EN 12056), frequency factor K 0.5/0.7/1.0, lifting plants, grease separators, soakaways"},
                  {"Abstimmung Entsorger, Vorgaben Brandschutz und Küche, Abflussbeiwerte, Funktionsbeschreibung", "Agreement with disposer, fire and kitchen requirements, runoff coefficients, function description"}},
                 {{"412 Trinkwasser", "412 drinking water"},
                  {"Kalt, warm und Zirkulation mit Nennweiten, Erwärmer, Druckerhöhung, Aufbereitung", "Cold, hot and circulation with sizes, heater, booster, treatment"},
                  {"Abstände zu Bauteilen, Trennung warm/kalt, DN, Steigleitungen mit Pfeilen, Höhen der Rohrachsen, Durchbrüche",
                   "Distances to elements, hot/cold separation, DN, risers with arrows, axis levels, openings"},
                  {"Rohrnetz nach DIN 1988-300 (Gebäudetyp, Mindestfließdrücke, v_max), Zirkulation, Trinkwassererwärmer, Bedarf kalt/warm",
                   "Pipe network per DIN 1988-300 (building type, minimum flow pressures, v_max), circulation, water heaters, cold/hot demand"},
                  {"Versorgungsdruck, Hygiene (VDI 6023 Raumbuch), Pumpen- und Aufbereitungsdimensionierung", "Supply pressure, hygiene (VDI 6023 room book), pump and treatment sizing"}},
                 {{"420 Wärme", "420 heat"},
                  {"Funktions- und Strangschema: Erzeuger, Puffer, Verteiler, Pumpengruppen, Regelventile, MAG, Wärmetauscher mit Temperaturen, Druckverhältnisse",
                   "Function and riser schematics: generators, buffers, manifolds, pump groups, control valves, expansion vessels, heat exchangers with temperatures, pressures"},
                  {"Heizflächen, Ventile, Pumpen, Rohre mit Dämmung, auf Rohrmitte bemaßt, Durchbrüche, Entlüftung an Hochpunkten",
                   "Heating surfaces, valves, pumps, pipes with insulation, dimensioned to the axis, openings, venting at high points"},
                  {"Norm-Heizlast (DIN EN 12831), Raumheizflächen, Rohrnetz, alle Komponenten, Dämm-Matrix", "Design heat load (EN 12831), room heating surfaces, network, all components, insulation matrix"},
                  {"Energiekonzept, System je Raumtyp, Technikflächen, Temperaturen", "Energy concept, system per room type, plant rooms, temperatures"}},
                 {{"430 Lüftung", "430 ventilation"},
                  {"RLT-Geräte, Brandschutzklappen, Volumenstromregler, Schalldämpfer, Überströmer, Sonderabluft (Küche, Labor, Batterie)",
                   "AHUs, fire dampers, VAV boxes, silencers, transfer units, special extract (kitchen, lab, batteries)"},
                  {"Raumnummern und -stempel, Kanäle rund/eckig bemaßt, Luftart, Höhen, Dämmung/Brandschutzverkleidung, Einbauteile, Luftmengen",
                   "Room numbers and stamps, round/rectangular ducts dimensioned, air type, levels, insulation/fire cladding, components, air flows"},
                  {"Luftmengen raumweise (DIN EN 16798), Kanalnetz je Strang (+ ca. 10 % Reserve auf den Druckverlust), Dichtheitsklasse ATC, Schall",
                   "Air flows per room (EN 16798), duct network per branch (+ about 10 % pressure-drop reserve), tightness class ATC, acoustics"},
                  {"Systeme je Bereich, Kanalgeschwindigkeiten, Feuchte, Auslegung im h,x-Diagramm", "Systems per area, duct velocities, humidity, design in the psychrometric chart"}},
                 {{"434 Kälte", "434 cooling"},
                  {"Kälteerzeuger, Rückkühler, Puffer, Verteiler, Pumpengruppen, Umluftkühler, Flächenkühlung", "Chillers, re-coolers, buffers, manifolds, pump groups, fan coils, surface cooling"},
                  {"Wie Heizung: Geräte, Rohre mit Dämmung, Höhen, Durchbrüche", "As heating: units, insulated pipes, levels, openings"},
                  {"Kühllast (VDI 2078), Raumkühlflächen, Rohrnetz, Komponenten, Dämm-Matrix", "Cooling load (VDI 2078), room cooling surfaces, network, components, insulation matrix"},
                  {"Kälteerzeugungskonzept, Temperaturen, Schalt- und Anschlusspläne der Kältemaschinen", "Cooling concept, temperatures, wiring and connection diagrams of chillers"}},
                 {{"440 Elektro", "440 electrical"},
                  {"Mittelspannung, Netzersatz, PV, NSHV, Steigleitungen, Unterverteilungen, KNX, Spannungsfälle, Sicherheitsbeleuchtung, Potentialausgleich",
                   "MV, standby generation, PV, LV main, risers, sub-distributions, KNX, voltage drops, emergency lighting, bonding"},
                  {"Trassen, Installation, Beleuchtung, Erdung, Blitzschutz", "Cable routes, installation, lighting, earthing, lightning protection"},
                  {"Leistungsbilanz, Trafokühlung, Druckentlastung, Netzberechnung, Trassenbedarf, Beleuchtung, Trennungsabstand Blitzschutz",
                   "Power balance, transformer cooling, pressure relief, network calculation, route space, lighting, LPS separation distance"},
                  {"Leuchtenkatalog, Raumkonditionen, Datenpunktliste für die GA", "Luminaire catalogue, room conditions, data point list for BACS"}},
                 {{"450 Kommunikation und Sicherheit", "450 communication and security"},
                  {"Türsprech, ELA, Brandmelde- (BMA), Sprachalarm- (SAA), Einbruchmelde- (EMA), Video (VÜA), Zutritt (ZUKO), Datennetze",
                   "Door intercom, PA, fire alarm, voice alarm, intrusion, CCTV, access control, data networks"},
                  {"Melder, Lautsprecher, Kameras, Datenanschlüsse; Türdetails, Aufbau Datenschränke", "Detectors, loudspeakers, cameras, outlets; door details, rack layouts"},
                  {"BMA-Auslegung, Beschallung, weitere Simulationen", "Fire alarm design, PA calculation, further simulations"},
                  {"Brandmelde- und Alarmierungskonzept nach DIN 14675", "Fire detection and alarm concept per DIN 14675"}},
                 {{"480 Gebäudeautomation", "480 building automation"},
                  {"Systemaufbau (Topologie) und Automationsschemata je Anlage", "System architecture (topology) and automation schematics per plant"},
                  {"ISP und Trassen lagerichtig, maßstäblich, bemaßt; Schaltschrankansichten", "Control panels and routes in position, to scale, dimensioned; panel views"},
                  {"GA-Funktionslisten, Leistungsbilanz, Einspeisungen, Schaltschrankflächen, Regelventile", "BACS function lists, power balance, supplies, panel areas, control valves"},
                  {"Siehe Gruppe „Gebäudeautomation (KG 480)“", "See group \"Building automation (KG 480)\""}}});
}

// ------------------------------------------------------------------ building automation (KG 480)

Table gaDocuments() {
    return make("ga_lph3_documents", {"Entwurfsplanung GA: Planungsdokumente", "BACS design stage: documents"},
                {"Die 14 Dokumente der Lph 3 in der Gebäudeautomation und ihr Inhalt", "The 14 stage 3 documents in building automation and their content"},
                cat({{"Zusammengefasst nach ", "Summarised after "}, kBook,
                     {", Kap. 11.2 (KG 480 Planungsdokumente). Leistungsdefinition je nach Vertrag aus HOAI, DIN EN ISO 16484, "
                      "VDI 3814, VDI 6026 und AMEV GA – siehe Tabelle „Regelwerke“.",
                      ", ch. 11.2 (KG 480 planning documents). Service definition depending on the contract from HOAI, EN ISO 16484, "
                      "VDI 3814, VDI 6026 and AMEV GA – see table \"Rules\"."}}),
                {{"Nr.", "No."}, {"Dokument", "Document"}, {"Inhalt in der Entwurfsplanung", "Content at the design stage"}, {"Hinweis", "Note"}},
                {{"1", {"Systemaufbau (Topologie)", "System architecture (topology)"},
                  {"Anzahl und Ort der ISP, dezentrale Komponenten, angeschlossene Anlagen, Kommunikationswege ISP–Anlage, ISP–ISP, zur "
                   "Managementebene und zur Außenwelt; als Gebäudeschnitt mit allen Ebenen",
                   "Number and location of control panels, remote components, connected plant, communication paths panel–plant, "
                   "panel–panel, to the management level and outside; as a building section with all levels"},
                  {"Topologie und Netze in einem Dokument", "Topology and networks in one document"}},
                 {"2", {"Automationsschema", "Automation schematic"},
                  {"Je Anlage (Typicals nur nach Vereinbarung), mit Sensoren und Aktoren; farbig nach Medien, GA-Lieferumfang hervorgehoben",
                   "Per plant (typicals only if agreed), with sensors and actuators; coloured by media, BACS scope highlighted"},
                  {"Regelaufgabe prüfen, z. B. Schwachlastventil am Wärmeübertrager an den Anlagenplaner zurückmelden",
                   "Check the control task, e.g. report a low-load valve at the heat exchanger back to the plant designer"}},
                 {"3", {"GA-Funktionsliste", "BACS function list"},
                  {"Datenpunkte (physikalisch und kommunikativ) und Funktionen je Anlage nach VDI 3814 bzw. DIN EN ISO 16484-3",
                   "Data points (physical and communicated) and functions per plant per VDI 3814 / EN ISO 16484-3"},
                  {"Mengengerüst für die Kosten", "Quantities for the cost calculation"}},
                 {"4", {"Elektrische Berechnungen", "Electrical calculations"},
                  {"Leistungsbilanz je ISP (Leistung, Gleichzeitigkeit, Spannung, Netzart AV/SV/USV, Nennstrom); Energiebedarf nur des "
                   "GA-Eigenbedarfs; Leistungskabel nach Spannungsfall; Selektivität per Faustregel; Verlegearten A1 … G",
                   "Power balance per panel (power, diversity, voltage, supply type normal/safety/UPS, rated current); energy only "
                   "for the BACS' own consumption; power cables by voltage drop; selectivity by rule of thumb; installation methods A1 … G"},
                  {"Rechner „GA-Schaltschrank: Einspeisung“", "Calculator \"BACS panel: supply\""}},
                 {"5", {"Schaltschrankauslegung", "Control panel design"},
                  {"Einspeisungen (Strom, Schalter, Zuleitung, Klemmen), Flächenbedarf aus Einspeise-, Leistungs- und Automationsteil "
                   "mit mindestens 20 % Reserve, Türöffnung und Fluchtweg, Schaltschrankansichten",
                   "Supplies (current, switch, feeder, terminals), area from supply, power and automation sections with at least "
                   "20 % spare, door swing and escape route, panel views"},
                  {"Rechner „GA-Schaltschrank: Fläche“", "Calculator \"BACS panel: area\""}},
                 {"6", {"Feldgeräte", "Field devices"},
                  {"Regelorgane (Ventile) berechnen; Sensor- und Aktorlisten nur, wenn sie auch geprüft werden", "Calculate control valves; sensor and actuator lists only if they are checked"},
                  {"Rechner „Ventil kv/kvs“ im Bereich HLKS", "Calculator \"Valve kv/kvs\" in HVAC"}},
                 {"7", {"Grundrisse", "Floor plans"},
                  {"ISP und Trassen lagerichtig, maßstäblich und bemaßt (mit Höhenlage); GA-Komponenten außerhalb der Zentralen",
                   "Panels and routes in position, to scale and dimensioned (with levels); BACS components outside plant rooms"},
                  {"Bezug auf feste Gebäudeteile", "Refer to fixed building elements"}},
                 {"8", {"Beschreibungen", "Descriptions"},
                  {"Systembeschreibung der „Anlage GA“ mit Nutzungsbedingungen, Funktionsbeschreibungen, Erläuterungsbericht",
                   "System description of the BACS with conditions of use, function descriptions, explanatory report"},
                  {"Keine Leistungen anderer Gewerke beschreiben", "Do not describe other trades' work"}},
                 {"9", {"Adressierungssystem", "Addressing system"},
                  {"Gewerkeübergreifende Benutzeradresse mit fester Stellenzahl; vom Bauherrn vertraglich für alle Gewerke festzulegen",
                   "Cross-trade user address with fixed positions; to be agreed by the client for all trades"},
                  {"Bereich GA: AMEV- und VDI-3814-Kennzeichnung", "Area BACS: AMEV and VDI 3814 identification"}},
                 {"10", {"Kosten", "Costs"},
                  {"Kostenberechnung DIN 276 3. Ebene (KG 481–485, 489) je Datenpunkt; nach VDI 6026 anlagenbezogen; Kostenkontrolle gegen die Schätzung",
                   "Cost calculation DIN 276 level 3 (KG 481–485, 489) per data point; per plant under VDI 6026; cost control against the estimate"},
                  {"Rechner „Kosten KG 480“", "Calculator \"Cost KG 480\""}},
                 {"11", {"Schnittstellen", "Interfaces"},
                  {"Schnittstellendokument: Einspeisekabel und Drehfeldmessung, Liefern und Einbauen von Sensoren, Aktoren und Antrieben",
                   "Interface document: feeder and phase rotation test, supply and fitting of sensors, actuators and drives"},
                  {"Jeder Abgabe beilegen", "Attach to every issue"}},
                 {"12", {"Systemintegration", "System integration"},
                  {"Systemintegrationsliste: jede Anlage nach Kostengruppe mit Integrationsart", "System integration list: every plant by cost group with type of integration"},
                  "VDI 3814"},
                 {"13", {"Brandschutz", "Fire safety"},
                  {"Anforderungen aus dem Brandschutzkonzept (z. B. Entrauchungs-Nebenschaltstellen), Brandfallsteuermatrix, Funktionserhalt",
                   "Requirements from the fire concept (e.g. secondary smoke control points), fire control matrix, circuit integrity"},
                  {"Taster in Grundrissen, Verschaltung im Automationsschema", "Push buttons in plans, wiring in the schematic"}},
                 {"14", {"Wartung", "Maintenance"},
                  {"Wartungsflächen (Türöffnung, Fluchtweg), Revisionsöffnungen, Wartungshinweise und -kosten", "Maintenance areas (door swing, escape route), access panels, maintenance notes and costs"},
                  "VDI 6026"}});
}

Table gaInterfaces() {
    return make("ga_lph3_interfaces", {"Entwurfsplanung GA: Abhängigkeiten zu anderen Gewerken", "BACS design stage: dependencies on other trades"},
                {"Was die GA braucht (Input) und liefert (Output)", "What the BACS needs (input) and delivers (output)"},
                cat({{"Zusammengefasst nach ", "Summarised after "}, kBook,
                     {", Kap. 11.1.6. Die GA plant zeitversetzt: Sie braucht Zwischenstände der Anlagenplanung und liefert selbst Ergebnisse "
                      "zurück – einen gemeinsamen Planungsstand (Design-Freeze) vereinbaren.",
                      ", ch. 11.1.6. BACS design lags behind: it needs intermediate results from the plant design and returns results – "
                      "agree a common planning status (design freeze)."}}),
                {{"Richtung", "Direction"}, {"Thema", "Topic"}, {"Von / an", "From / to"}, {"Wozu", "Purpose"}},
                {{"Input", {"Grundrisse", "Floor plans"}, {"Objektplanung, TGA", "Architect, building services"}, {"Standorte von ISP und Feldgeräten, Trassen", "Locations of panels and field devices, routes"}},
                 {"Input", {"Anlagenschemata", "Plant schematics"}, {"Heizung, Lüftung, Kälte, Sanitär", "Heating, ventilation, cooling, plumbing"}, {"Grundlage der Automationsschemata", "Basis of the automation schematics"}},
                 {"Input", {"Anlagen- und Funktionsbeschreibungen", "Plant and function descriptions"}, {"TGA-Fachplaner", "Services designers"}, {"Regel- und Steuerfunktionen", "Control functions"}},
                 {"Input", {"Liste elektrischer TGA-Verbraucher", "List of electrical loads"}, {"HLKS, Elektro", "HVAC, electrical"}, {"Leistungsteil, Leistungsbilanz", "Power section, power balance"}},
                 {"Input", {"Thermische Daten der Verbraucherkreise", "Thermal data of consumer circuits"}, {"Heizung, Kälte", "Heating, cooling"}, {"Ventilauslegung (kvs, Autorität)", "Valve sizing (kvs, authority)"}},
                 {"Input", {"Ex-Zonen- und Sicherheitskonzepte", "Ex zone and safety concepts"}, {"Bauherr, Sachverständige", "Client, experts"}, {"Geräteauswahl, Sicherheitsfunktionen", "Device selection, safety functions"}},
                 {"Output", {"Bauliche Angaben", "Building data"}, {"Objekt- und Tragwerksplanung", "Architect, structural engineer"}, {"Stellflächen der ISP, Durchbrüche, Revisionsöffnungen", "Panel footprints, openings, access panels"}},
                 {"Output", {"Elektrische Leistungsbilanz", "Electrical power balance"}, {"Elektro", "Electrical"}, {"Einspeiseleistung je Netzart (AV/SV/USV) anmelden", "Request supply capacity per supply type"}},
                 {"Output", {"Wärmelasten der Schaltschränke", "Heat loads of the panels"}, {"Lüftung, Kälte", "Ventilation, cooling"}, {"Kühlung der Technikräume", "Cooling of plant rooms"}},
                 {"Output", {"Terminplanung", "Scheduling"}, {"Projektsteuerung", "Project management"}, {"Nachlauf der GA einplanen", "Allow for the BACS lag"}}});
}

Table gaRules() {
    return make("ga_lph3_rules", {"Entwurfsplanung GA: Regelwerke", "BACS design stage: rules"},
                {"HOAI, DIN EN ISO 16484, VDI 3814, VDI 6026, AMEV GA, DIN 276 – was sie für Lph 3 bedeuten", "HOAI, EN ISO 16484, VDI 3814, VDI 6026, AMEV GA, DIN 276 – what they mean for stage 3"},
                cat({{"Zusammengefasst nach ", "Summarised after "}, kBook,
                     {", Kap. 11.1.3. Welche Regel gilt, entscheidet der Vertrag; ohne Vereinbarung frühzeitig mit dem Auftraggeber "
                      "abstimmen. Ausgaben gegen den aktuellen Stand prüfen.",
                      ", ch. 11.1.3. The contract decides which rule applies; without agreement coordinate early with the client. Check "
                      "editions against the current status."}}),
                {{"Regelwerk", "Rule"}, {"Bedeutung für die Entwurfsplanung", "Meaning for the design stage"}},
                {{"HOAI 2021, Anlage 15", {"Honorarrecht; Grundleistungen a) bis h) sehr allgemein – für die GA durch weitere Regeln zu konkretisieren",
                                           "Fee law; basic services a) to h) very general – to be detailed for BACS by further rules"}},
                 {"DIN EN ISO 16484", {"Grundlegende GA-Norm (anerkannte Regel der Technik), aber ohne Trennung nach Leistungsphasen",
                                       "Basic BACS standard (generally accepted rule), but without separation by stages"}},
                 {{"VDI 3814 (alt, 2003–2010)", "VDI 3814 (old, 2003–2010)"}, {"Zurückgezogen, in der Praxis noch Grundlage vieler Planungen; Blatt 1 Nr. 11.2 nennt Planungsinhalte ohne Phasentrennung",
                                                                              "Withdrawn but still the basis of many designs; part 1 no. 11.2 lists contents without stages"}},
                 {{"VDI 3814 (neu, ab 2019)", "VDI 3814 (new, from 2019)"}, {"Leistungen je Phase (Blatt 2.2, Nr. 5.5); fordert fertige Entwurfsergebnisse der anderen Gewerke; nur bei vertraglicher Vereinbarung",
                                                                             "Services per stage (part 2.2, no. 5.5); requires finished designs of other trades; only if agreed by contract"}},
                 {"VDI 6026 Blatt 1 (2022)", {"Detaillierte Dokumentinhalte je Gewerk und Phase (Tab. 8 GA), z. B. Schaltschrankansichten, anlagenbezogene Kosten, Wartungsflächen",
                                              "Detailed document contents per trade and stage (tab. 8 BACS), e.g. panel views, per-plant costs, maintenance areas"}},
                 {"AMEV GA", {"Stromversorgungskonzept, Mitwirken bei Listen (Ventile, Zähler), Kostenkennwerte 300–580 € je Hardware-Datenpunkt",
                              "Power supply concept, assisting with lists (valves, meters), cost indicators 300–580 € per hardware data point"}},
                 {"DIN 276 (2018)", {"Kostenberechnung in der 3. Ebene: KG 481–485 und 489, Bezugseinheit Datenpunkt", "Cost calculation at level 3: KG 481–485 and 489, unit data point"}}});
}

// ------------------------------------------------------------------ calculators

/// Next rating ≥ value in a list (last entry if larger)
double nextRating(const std::vector<double>& series, double value) {
    for (const double r : series) {
        if (r >= value - 1e-9) return r;
    }
    return series.back();
}

const std::vector<double>& switchRatings() {
    // switch-disconnector ratings commonly used for panel supplies
    static const std::vector<double> s{16, 20, 25, 32, 40, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630};
    return s;
}

Calculator ispSupply() {
    Calculator c;
    c.id = "ga_isp_supply";
    c.category = kCat;
    c.title = {"GA-Schaltschrank: Einspeisung", "BACS panel: supply"};
    c.summary = {"Strom, Einspeiseschalter und Zuleitung je Netzart AV/SV/USV, Selektivität – nach Buchbeispiel Lph 3",
                 "Current, supply switch and feeder per supply type normal/safety/UPS, selectivity – after the book example"};
    c.description = cat(
        {{"Planung der Einspeisungen eines Informationsschwerpunkts (ISP) in der Entwurfsplanung nach ",
          "Planning the supplies of a BACS control panel at the design stage after "},
         kBook,
         {", Kap. 11.2.4/11.2.5, Abb. 11.10:\n"
          "• Leistung je Netzart aus der Leistungsbilanz (Summe P · Gleichzeitigkeit + Reserve), Strom I = P / (√3 · U · cos φ) bei "
          "400 V Drehstrom bzw. I = P / (U · cos φ) bei 230 V Wechselstrom, cos φ als Mittelwert der Hauptverbraucher (Buch: 0,8).\n"
          "• Einspeiseschalter und Kabel mit mindestens 10 % Reserve: nächster Schalter-Nennstrom ≥ 1,1 · I.\n"
          "• Zuleitung: größerer Wert aus Spannungsfall (Cu, 70 °C) und Belastbarkeit I_z ≥ I_n (Verlegeart C, PVC, 30 °C). Nur "
          "Planungswert – die endgültige Bemessung macht das Gewerk Elektro bzw. der Errichter.\n"
          "• Selektivität (Faustregel des Buchs): zwei Sicherungen sind selektiv, wenn die nachgeschaltete mindestens zwei "
          "Nennstromstufen kleiner ist (Beispiel 32 A zu 10 A).\n"
          "Buchbeispiel: AV 100 kW → 180,4 A, Schalter 200 A; SV 20 kW → 36,1 A (Buch 63 A, nächste Stufe wäre 40 A). USV 2 kW: das "
          "Buch rechnet 3,6 A wie bei 400 V Drehstrom; bei 230 V Wechselstrom sind es 10,9 A.",
          ", ch. 11.2.4/11.2.5, fig. 11.10:\n"
          "• Power per supply type from the power balance (Σ P · diversity + spare), current I = P / (√3 · U · cos φ) at 400 V "
          "three-phase or I = P / (U · cos φ) at 230 V single-phase, cos φ as average of the main loads (book: 0.8).\n"
          "• Supply switch and cable with at least 10 % spare: next switch rating ≥ 1.1 · I.\n"
          "• Feeder: larger of voltage drop (Cu, 70 °C) and ampacity I_z ≥ I_n (method C, PVC, 30 °C). Planning value only – "
          "final sizing by the electrical trade or installer.\n"
          "• Selectivity (rule of thumb of the book): two fuses are selective if the downstream one is at least two rating steps "
          "smaller (example 32 A vs. 10 A).\n"
          "Book example: normal 100 kW → 180.4 A, switch 200 A; safety 20 kW → 36.1 A (book 63 A, next step would be 40 A). UPS 2 kW: "
          "the book calculates 3.6 A as for 400 V three-phase; at 230 V single-phase it is 10.9 A."}});
    const std::vector<Option> volt{{"400", {"400 V Drehstrom (3L, N, PE)", "400 V three-phase (3L, N, PE)"}},
                                   {"230", {"230 V Wechselstrom (L, N, PE)", "230 V single-phase (L, N, PE)"}}};
    const double nan = std::numeric_limits<double>::quiet_NaN();
    c.inputs = {
        Num::number("P_av", {"Leistung allgemeine Versorgung (AV)", "Power normal supply"}, "kW", 100).positive()
            .startsSection({"Einspeiseleistungen aus der Leistungsbilanz", "Supply powers from the power balance"}),
        Num::choice("U_av", {"Spannung AV", "Voltage normal supply"}, volt, "400"),
        Num::number("P_sv", {"Leistung Sicherheitsversorgung (SV)", "Power safety supply"}, "kW", 20).positive().opt(),
        Num::choice("U_sv", {"Spannung SV", "Voltage safety supply"}, volt, "400"),
        Num::number("P_usv", {"Leistung USV", "Power UPS"}, "kW", 2).positive().opt(),
        Num::choice("U_usv", {"Spannung USV", "Voltage UPS"}, volt, "230"),
        Num::number("cosphi", {"Leistungsfaktor cos φ (Mittelwert)", "Power factor cos φ (average)"}, "", 0.8).range(0.3, 1.0)
            .startsSection({"Auslegung", "Design"}),
        Num::number("reserve", {"Reserve Schalter und Kabel", "Spare switch and cable"}, "%", 10).range(0, 100),
        Num::number("L", {"Länge der Zuleitung", "Feeder length"}, "m", 30).positive(),
        Num::number("dU", {"Zulässiger Spannungsfall", "Permitted voltage drop"}, "%", 3).range(0.5, 10),
        Num::number("fuse", {"Größte Abgangssicherung im ISP", "Largest outgoing fuse in the panel"}, "A", nan).positive().opt()
            .withHint({"für die Selektivitätsprüfung, z. B. 10 A (Zuluftventilator)", "for the selectivity check, e.g. 10 A (supply fan)"}),
    };
    c.outputs = {Out::number("I_av", {"Strom AV", "Current normal"}, "A", 1).primary(),
                 Out::number("In_av", {"Einspeiseschalter AV", "Supply switch normal"}, "A", 0).primary(),
                 Out::number("A_av", {"Zuleitung AV (Planungswert)", "Feeder normal (planning value)"}, "mm²", 1),
                 Out::number("I_sv", {"Strom SV", "Current safety"}, "A", 1),
                 Out::number("In_sv", {"Einspeiseschalter SV", "Supply switch safety"}, "A", 0),
                 Out::number("A_sv", {"Zuleitung SV (Planungswert)", "Feeder safety (planning value)"}, "mm²", 1),
                 Out::number("I_usv", {"Strom USV", "Current UPS"}, "A", 1),
                 Out::number("In_usv", {"Einspeiseschalter USV", "Supply switch UPS"}, "A", 0),
                 Out::number("A_usv", {"Zuleitung USV (Planungswert)", "Feeder UPS (planning value)"}, "mm²", 1),
                 Out::text("selectivity", {"Selektivität (Faustregel)", "Selectivity (rule of thumb)"})};
    c.keywords = {"einspeisung", "isp", "informationsschwerpunkt", "schaltschrank", "msr", "ga", "leistungsbilanz", "selektivität",
                  "einspeiseschalter", "entwurfsplanung", "lph 3", "panel supply"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double cosphi = in.num("cosphi");
        const double res = 1.0 + in.num("reserve") / 100.0;
        const double rho = cables::resistivity(cables::Material::Cu, 70.0);
        auto feed = [&](const char* p, const char* u, const std::string& tag) {
            const auto P = in.maybe(p);
            if (!P) return;
            const bool three = in.choice(u) == "400";
            const double U = three ? 400.0 : 230.0;
            const double I = *P * 1000.0 / ((three ? std::sqrt(3.0) : 1.0) * U * cosphi);
            const double In = nextRating(switchRatings(), res * I);
            const auto sys = three ? System::AC3 : System::AC1;
            const double aDrop = cables::requiredArea(sys, in.num("L"), I, rho, cosphi, 0.08e-3, in.num("dU") / 100.0 * U);
            double aAmp = std::numeric_limits<double>::infinity();
            for (const double a : cables::kAmpacityAreas) {
                const auto iz = cables::baseAmpacity(cables::Insulation::PVC, cables::Method::C, three ? 3 : 2, a);
                if (iz && *iz >= In) {
                    aAmp = a;
                    break;
                }
            }
            const double need = std::max(aDrop, aAmp);
            double area = need;
            for (const double a : cables::kStandardAreas) {
                if (a >= need - 1e-9) {
                    area = a;
                    break;
                }
            }
            r.set("I_" + tag, I);
            r.set("In_" + tag, In);
            if (std::isfinite(area) && area <= 300.0) r.set("A_" + tag, area);
            else r.warn({"Zuleitung " + tag + " über 300 mm² – mehrere parallele Kabel oder Stromschiene vorsehen.",
                         "Feeder " + tag + " above 300 mm² – plan parallel cables or a busbar."});
        };
        feed("P_av", "U_av", "av");
        feed("P_sv", "U_sv", "sv");
        feed("P_usv", "U_usv", "usv");
        if (const auto f = in.maybe("fuse")) {
            const auto gg = cables::ratedCurrents(cables::Device::FuseGG);
            auto index = [&](double v) {
                for (std::size_t i = 0; i < gg.size(); ++i) {
                    if (gg[i] >= v - 1e-9) return static_cast<int>(i);
                }
                return static_cast<int>(gg.size()) - 1;
            };
            const double up = r.values.at("In_av");
            const int steps = index(up) - index(*f);
            if (steps >= 2) {
                r.setText("selectivity", {"selektiv – " + formatNumber(*f, 0, Lang::De) + " A liegt " + std::to_string(steps) + " Stufen unter " +
                                              formatNumber(up, 0, Lang::De) + " A",
                                          "selective – " + formatNumber(*f, 0, Lang::En) + " A is " + std::to_string(steps) + " steps below " +
                                              formatNumber(up, 0, Lang::En) + " A"});
            } else {
                r.setText("selectivity", {"nicht sicher selektiv – weniger als zwei Nennstromstufen Abstand", "not reliably selective – less than two rating steps apart"});
                r.warn({"Abgangssicherung zu nah an der Einspeisung – Einspeisung größer wählen oder Abgang kleiner absichern.",
                        "Outgoing fuse too close to the supply – choose a larger supply or a smaller outgoing fuse."});
            }
        }
        r.info({"Zuleitungen sind Planungswerte (Spannungsfall, Verlegeart C) – die Bemessung durch das Gewerk Elektro ist maßgebend.",
                "Feeders are planning values (voltage drop, method C) – sizing by the electrical trade governs."});
        return r;
    };
    return c;
}

Calculator cabinetArea() {
    Calculator c;
    c.id = "ga_cabinet_area";
    c.category = kCat;
    c.title = {"GA-Schaltschrank: Fläche und Felder", "BACS panel: area and sections"};
    c.summary = {"Leistungsfelder aus der Bauteilfläche mit Mindestreserve, Gesamtbreite und Platzbedarf mit Türöffnung",
                 "Power sections from the component area with minimum spare, total width and footprint with door swing"};
    c.description = cat(
        {{"Schaltschrankflächenberechnung eines ISP in der Entwurfsplanung nach ", "Control panel area calculation at the design stage after "},
         kBook,
         {", Kap. 11.2.5, Abb. 11.12:\n"
          "• Einspeise- und Automationsfelder werden als ganze Felder festgelegt (zwei Netzarten: je ein Einspeisefeld an den Enden der Reihe).\n"
          "• Leistungsteil rechnerisch: Summe der Montageflächen der Bauteile (dm²) auf Leistungsfelder verteilen, bis die "
          "Reserve (Nutzfläche − Bedarf) / Nutzfläche den Mindestwert (Buch: 20 %) erreicht. Große Einzelteile (FU, "
          "Lasttrenner) skizzieren – sie lassen sich nicht teilen.\n"
          "• Nutzfläche je Leistungsfeld = Feldbreite · Nutzbreitenanteil · nutzbare Höhe. Die Vorgaben 80 % und 1,2 m (76,8 dm² bei "
          "0,8 m Feldbreite) sind unsere Annahme; sie ergeben das Buchbeispiel (218 dm² → 4 Felder, Reserve 29,0 %).\n"
          "• Platzbedarf: Breite der Reihe × (Schranktiefe + Türöffnung = breitestes Feld bei einflügeliger Tür). Fluchtwegbreite "
          "vor dem Schrank zusätzlich beachten.",
          ", ch. 11.2.5, fig. 11.12:\n"
          "• Supply and automation sections are fixed as whole sections (two supply types: one supply section at each end).\n"
          "• Power section by calculation: spread the sum of component mounting areas (dm²) over power sections until the "
          "spare (usable − required) / usable reaches the minimum (book: 20 %). Sketch large single parts (VFD, switch "
          "disconnector) – they cannot be split.\n"
          "• Usable area per power section = section width · usable width share · usable height. The defaults 80 % and 1.2 m "
          "(76.8 dm² at 0.8 m width) are our assumption; they reproduce the book example (218 dm² → 4 sections, 29.0 % spare).\n"
          "• Footprint: row width × (cabinet depth + door swing = widest section with a single door). Mind the escape route width "
          "in front of the panel as well."}});
    c.inputs = {
        Num::number("area", {"Montagefläche Leistungsteil", "Mounting area power section"}, "dm²", 218).positive()
            .startsSection({"Leistungsteil", "Power section"}),
        Num::number("wPow", {"Breite Leistungsfeld", "Width power section"}, "m", 0.8).range(0.3, 1.6),
        Num::number("useW", {"Nutzbreitenanteil (Kabelkanäle abgezogen)", "Usable width share (ducts deducted)"}, "%", 80).range(30, 100),
        Num::number("useH", {"Nutzbare Montagehöhe", "Usable mounting height"}, "m", 1.2).range(0.3, 2.2),
        Num::number("reserveMin", {"Mindestreserve", "Minimum spare"}, "%", 20).range(0, 80),
        Num::number("nFeed", {"Einspeisefelder", "Supply sections"}, "", 2).range(0, 6).startsSection({"Weitere Felder", "Other sections"}),
        Num::number("wFeed", {"Breite Einspeisefeld", "Width supply section"}, "m", 0.6).range(0.3, 1.6),
        Num::number("nAuto", {"Automationsfelder", "Automation sections"}, "", 3).range(0, 20),
        Num::number("wAuto", {"Breite Automationsfeld", "Width automation section"}, "m", 0.6).range(0.3, 1.6),
        Num::number("depth", {"Schranktiefe", "Cabinet depth"}, "m", 0.4).range(0.2, 1.0),
        Num::choice("door", {"Türen", "Doors"},
                    {{"single", {"einflügelig (Türbreite = Feldbreite)", "single (door width = section width)"}},
                     {"double", {"zweiflügelig (halbe Feldbreite)", "double (half section width)"}}},
                    "single"),
    };
    c.outputs = {Out::number("nPower", {"Leistungsfelder", "Power sections"}, "", 0).primary(),
                 Out::number("reserve", {"Tatsächliche Reserve", "Actual spare"}, "%", 1).primary(),
                 Out::number("fields", {"Felder gesamt", "Sections total"}, "", 0),
                 Out::number("width", {"Breite der Schrankreihe", "Width of the row"}, "m", 2),
                 Out::number("depth", {"Tiefe mit Türöffnung", "Depth with door swing"}, "m", 2),
                 Out::number("footprint", {"Platzbedarf", "Footprint"}, "m²", 2),
                 Out::number("perField", {"Nutzfläche je Leistungsfeld", "Usable area per power section"}, "dm²", 1)};
    c.keywords = {"schaltschrank", "isp", "felder", "schrankfläche", "reserve", "msr-schaltschrank", "entwurfsplanung", "lph 3", "panel area"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double perField = in.num("wPow") * in.num("useW") / 100.0 * in.num("useH") * 100.0;  // dm²
        const double need = in.num("area");
        const double resMin = in.num("reserveMin") / 100.0;
        int n = 1;
        while ((n * perField - need) / (n * perField) < resMin - 1e-9 && n < 200) ++n;
        const double reserve = (n * perField - need) / (n * perField) * 100.0;
        const int nFeed = static_cast<int>(std::lround(in.num("nFeed")));
        const int nAuto = static_cast<int>(std::lround(in.num("nAuto")));
        const double width = nFeed * in.num("wFeed") + n * in.num("wPow") + nAuto * in.num("wAuto");
        double widest = in.num("wPow");
        if (nFeed > 0) widest = std::max(widest, in.num("wFeed"));
        if (nAuto > 0) widest = std::max(widest, in.num("wAuto"));
        const double door = in.choice("door") == "double" ? widest / 2.0 : widest;
        const double depth = in.num("depth") + door;
        r.set("perField", perField);
        r.set("nPower", n);
        r.set("reserve", reserve);
        r.set("fields", nFeed + n + nAuto);
        r.set("width", width);
        r.set("depth", depth);
        r.set("footprint", width * depth);
        r.ok({"Reserve ≥ Mindestreserve.", "Spare ≥ minimum spare."});
        r.info({"Nur Platzbedarf einer Reihe – Aufstellung über Eck oder Rücken an Rücken und den Türanschlag örtlich festlegen.",
                "Footprint of one row only – fix corner or back-to-back layouts and the door hinge on site."});
        return r;
    };
    c.chart = [](const Inputs& in, const Result& r, const Context&) {
        const int nFeed = static_cast<int>(std::lround(in.num("nFeed")));
        const int nAuto = static_cast<int>(std::lround(in.num("nAuto")));
        const int nPow = r.values.contains("nPower") ? static_cast<int>(r.values.at("nPower")) : 1;
        const double W = r.values.contains("width") ? r.values.at("width") : 1.0;
        const double s = std::min(92.0 / W, 20.0);  // drawing units per metre
        const double H = 1.8 * s, D = in.num("depth") * s;
        const double door = (r.values.contains("depth") ? r.values.at("depth") : in.num("depth")) * s - D;
        Sketch k;
        const double h = H + D + door + 22;
        k.frame(100, h, h / 100.0);
        const double x0 = (100 - W * s) / 2.0;
        const double yTop = 8 + door + D + 6;  // front view sits above the top view
        struct Field {
            char kind;
            double w;
        };
        std::vector<Field> row;
        if (nFeed >= 1) row.push_back({'E', in.num("wFeed")});
        for (int i = 0; i < nPow; ++i) row.push_back({'L', in.num("wPow")});
        for (int i = 0; i < nAuto; ++i) row.push_back({'A', in.num("wAuto")});
        for (int i = 1; i < nFeed; ++i) row.push_back({'E', in.num("wFeed")});
        double x = x0;
        for (const auto& f : row) {
            const double w = f.w * s;
            k.rect(f.kind == 'E' ? "fill:#FFE0B2" : f.kind == 'L' ? "fill:#BBDEFB" : "fill:#C8E6C9", x, yTop, x + w, yTop + H);
            k.rect("bgfill", x + w / 2 - 3, yTop + H / 2 - 2.6, x + w / 2 + 3, yTop + H / 2 + 2.6);
            k.label(x + w / 2, yTop + H / 2, std::string(1, f.kind), "cb");
            // top view
            k.rect("ink", x, 8 + door, x + w, 8 + door + D);
            x += w;
        }
        k.line("ink", {{x0 - 2, yTop}, {x + 2, yTop}});
        k.closed("sig", {{x0, 8}, {x, 8}, {x, 8 + door}, {x0, 8 + door}});
        k.label(50, 8 + door / 2, {"Türöffnung / Bedienfläche", "Door swing / operating area"}, "c");
        k.label(50, yTop + H + 3.5, cat({{"Breite ", "Width "}, formatNumber(W, 2), " m · ", formatNumber(static_cast<double>(row.size()), 0), {" Felder", " sections"}}), "cb");
        k.label(x0, 3, {"Ansicht (oben) und Draufsicht (unten)", "Front view (top) and plan (bottom)"}, "l");
        k.ch.note = {"E = Einspeisefeld, L = Leistungsfeld, A = Automationsfeld (schematisch, nach Buch Abb. 11.12/11.13). Gestrichelt: Fläche "
                     "für die Türöffnung vor der Schrankreihe.",
                     "E = supply section, L = power section, A = automation section (schematic, after book fig. 11.12/11.13). Dashed: area "
                     "for the door swing in front of the row."};
        return k.ch;
    };
    return c;
}

Calculator cost480() {
    Calculator c;
    c.id = "ga_cost_480";
    c.category = kCat;
    c.title = {"Kostenberechnung KG 480 und Kostenkontrolle", "Cost calculation KG 480 and cost control"};
    c.summary = {"Datenpunkte × Durchschnittspreis, Plausibilität gegen AMEV-Kennwert, Abweichung zur Kostenschätzung",
                 "Data points × average price, plausibility against the AMEV indicator, deviation from the estimate"};
    c.description = cat(
        {{"Kostenberechnung der Gebäudeautomation in der Entwurfsplanung nach ", "Building automation cost calculation at the design stage after "},
         kBook,
         {", Kap. 11.2.10:\n"
          "• DIN 276 (2018) verlangt die 3. Ebene – in der GA sechs Werte: KG 481 Automationseinrichtungen, 482 Schaltschränke/"
          "Automationsschwerpunkte, 483 Automationsmanagement, 484 Kabel, Leitungen und Verlegesysteme, 485 Datenübertragungsnetze, "
          "489 Sonstiges; Bezugseinheit jeweils der Datenpunkt.\n"
          "• Datenpunkte = physikalische und kommunikative aus der GA-Funktionsliste; günstigere kommunikative Punkte über einen "
          "angepassten Durchschnittspreis berücksichtigen.\n"
          "• AMEV GA, Tab. 2: übliche Spanne 300 … 580 € je Hardware-Datenpunkt (Vorgabe 440 € = Mitte). Projektwerte aus "
          "Erfahrung verwenden.\n"
          "• Kostenkontrolle: Kostenberechnung gegen Kostenschätzung, Abweichung begründen. VDI 6026 fordert zusätzlich eine "
          "anlagenbezogene Aufstellung.",
          ", ch. 11.2.10:\n"
          "• DIN 276 (2018) requires level 3 – six values for BACS: KG 481 automation equipment, 482 panels/automation centres, "
          "483 automation management, 484 cables, wiring and containment, 485 data networks, 489 other; unit the data point.\n"
          "• Data points = physical and communicated from the BACS function list; account for cheaper communicated points by an "
          "adjusted average price.\n"
          "• AMEV GA, tab. 2: usual range 300 … 580 € per hardware data point (default 440 € = middle). Use project experience.\n"
          "• Cost control: calculation against the estimate, explain the deviation. VDI 6026 additionally requires a per-plant breakdown."}});
    const double nan = std::numeric_limits<double>::quiet_NaN();
    c.inputs = {
        Num::number("dpPhys", {"Physikalische Datenpunkte", "Physical data points"}, "", 300).nonNegative(),
        Num::number("dpCom", {"Kommunikative Datenpunkte", "Communicated data points"}, "", 100).nonNegative(),
        Num::number("price", {"Durchschnittspreis je Datenpunkt", "Average price per data point"}, "€ / CHF", 440).positive()
            .withHint({"AMEV: 300 … 580 € je Hardware-Datenpunkt", "AMEV: 300 … 580 € per hardware data point"}),
        Num::number("estimate", {"Kostenschätzung (Lph 2)", "Cost estimate (stage 2)"}, "€ / CHF", nan).positive().opt(),
    };
    c.outputs = {Out::number("cost", {"Kosten KG 480", "Cost KG 480"}, "€", 0).primary(),
                 Out::number("dp", {"Datenpunkte gesamt", "Data points total"}, "", 0),
                 Out::number("deviation", {"Abweichung zur Kostenschätzung", "Deviation from the estimate"}, "%", 1).primary(),
                 Out::text("kg", {"Gliederung 3. Ebene", "Level 3 breakdown"})};
    c.keywords = {"kosten", "kostenberechnung", "din 276", "kg 480", "kg 481", "datenpunkt", "amev", "kostenkontrolle", "lph 3", "cost"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const double dp = in.num("dpPhys") + in.num("dpCom");
        const double price = in.num("price");
        const double cost = dp * price;
        r.set("dp", dp);
        r.set("cost", cost, currencySymbol(ctx));
        if (const auto est = in.maybe("estimate")) {
            const double dev = (cost - *est) / *est * 100.0;
            r.set("deviation", dev);
            r.info({"Abweichung zur Kostenschätzung im Erläuterungsbericht begründen (Kostenkontrolle, HOAI g).",
                    "Explain the deviation from the estimate in the report (cost control, HOAI g)."});
        }
        if (price < 300 || price > 580) {
            r.warn({"Preis je Datenpunkt außerhalb der AMEV-Spanne 300 … 580 € – Ansatz begründen (z. B. viele kommunikative Punkte, Sonderanlagen).",
                    "Price per data point outside the AMEV range 300 … 580 € – justify (e.g. many communicated points, special plant)."});
        }
        r.setText("kg", {"481 Automationseinrichtungen · 482 Schaltschränke · 483 Management · 484 Kabel und Verlegesysteme · 485 Datennetze · 489 Sonstiges – je Datenpunkt",
                         "481 automation equipment · 482 panels · 483 management · 484 cables and containment · 485 data networks · 489 other – per data point"});
        return r;
    };
    return c;
}

}  // namespace

void addPlanning(std::vector<Calculator>& out) {
    out.push_back(ispSupply());
    out.push_back(cabinetArea());
    out.push_back(cost480());
}

void addPlanningTables(std::vector<Table>& out) {
    out.push_back(services());
    out.push_back(premises());
    out.push_back(formal());
    out.push_back(documents());
    out.push_back(gaDocuments());
    out.push_back(gaInterfaces());
    out.push_back(gaRules());
}

}  // namespace elc::catalog
