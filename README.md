# HB-WDS100-C6-O-2

![HB-WDS100-C6-O-2](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/kombisensor.jpg)

Ein "Reparaturset" fuer den Wetter-Kombisensor HM-WDS100-C6-O-2 auf Basis der AskSinPP-Bibliothek. Da Ausfaelle des originalen Wetter-Kombisensors wegen Korrosion der Hauptplatine offenbar nach einigen Jahren haeufiger vorkommen, die eigentliche Mechanik des Geraetes jedoch langlebig erscheint, wurde eine Ersatz-Elektronik sowie AskSinPP-basierte Firmware entwickelt, die es erlaubt, den Sensor ohne Modifikationen oder Addon-Installationen auf der CCU wieder zum Leben zu erwecken. Dazu sind sowohl die Hauptplatine als auch die Windrichtungsmesserplatine gegen die neu konzipierten Platinen auszutauschen und der Sensor neu anzulernen.



## Hardware

### Demontage des Kombisensors

Die Demontage des die Hauptplatine (und Batteriefach sowie Anemometer) tragenden Geraeteteils muss vorsichtig in folgenden Schritten erfolgen, um Schaeden an den Plastikteilen zu vermeiden:
1. Gehaeusehuelse ueber dem Batteriefach oeffnen und Batterien entfernen, Westernstecker loesen.
2. Beide Schrauben, die den Rotor mit den drei Schalen auf der Welle halten, leicht loesen und Rotor abziehen.
3. Zwei unter der Rotorkappe sichtbarwerdende vierkantige Arretierungsstifte aus Platik vorsichtig nach oben herausziehen (etwas mit dem Schraubenzieher hebeln, dann mit der Zange greifen und senkrecht herausziehen)
4. Die drei Ringe zum Sonnen- und Wetterschutz (Stevenson-Screen) mit dem oberen beginnend leicht im Urzeigersinn drehen, dass sich der jeweilige Bajonettverschluss loest, und Ring senkrecht nach oben abziehen.
5. Zwei kleine Schrauben ganz oben am Lager der Welle sowie danach fuenf lange Gehaeuseschrauben entfernen.
6. Zwei Schrauben, die den Aufbau auf dem Edelstahlrohr fixieren, loesen und Aufbau senkrecht nach oben aus dem Rohr Heraus ziehen
7. Die beiden Gehaeusehalbschalen lassen sich nun trennen und die defekte Platine ausbauen. Die Kabel koennen in der anderen Gehaeusehalbschale verbleiben.

Die Demontage der Windrichtungsmesserplatine ist unproblematisch.

### Ersatzplatinen

Als Ersatz sind zwei neue Platinen herzustellen: [Schalt- und Bestueckungsplaene sowie Gerberfiles](https://github.com/HMSteve/HB-WDS100-C6-O-2/tree/main/PCB).

Die Platinen sind bzgl. Bauteilbauformen, Padgroessen und Abstaenden grundsaetzlich fuer Handloetung vorgesehen, Details siehe unten.

### Hauptplatine

Die Platine basiert auf einem ATMega1284p. Die Temperatur-/Luftfeuchtemessung erfolgt mit einem SHT31, die Helligkeitsmessung mit einem [VEML6030](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/datasheets/VEML6030.pdf). Die Windgeschwindigkeit wird wie im Original per Reedkontakt gezaehlt, zur Reduzierung des Stromverbrauchs wird zum Zaehlen ein [PCF8593](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/datasheets/PCF8593.pdf) eingesetzt.

Ein paar Fotos vom Aufbau:

![PCB Top](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_top.jpg)

![PCB Bottom](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_bot.jpg)

Bei Bestueckung der als Layout und Gerberfiles vorliegenden Platine v2.0 sind zwei Fehler zu korrigieren. Dazu ist jeweils etwas Loetstoplack zu entfernen, um die geaenderten Bauteilpostionen bestuecken zu koennen.

![PCB Bottom Changes](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_bot_pcbchanges.jpg)

1. C17 ist nicht direkt am CC1101-Modul zu bestuecken, sondern direkt vor dem FET zur Abschaltung der Betriebsspannung bzw. an der dort vorhandenen Durchkontaktierung gegen die Masseflaeche. Ohne diese Aenderung koennen die Einschaltstromspitzen zu einem Brownout des ATMega fuehren und die Schaltung laeuft nicht oder geraet in eine Reset-Schleife.
2. Der markierte 3.3M-Pullup-Widerstand gegen 3.3V fehlt im Layout und ist wie gezeigt zu ergaenzen. Auf Grund der Konstruktion der Regenzaehlerwippe, deren Reedkontakt in einer stabilen Wippenstellung stets geschlossen ist, scheidet die Nutzung des niederohmigen internen pull-up des ATMega wegen des Energieverbrauchs aus.

Der korrigierte [Schaltplan v2.1](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/PCB/HB-WDS100-C6-O-2_Main_v2p1Schematic.pdf) (ohne Platinenlayout-Aenderung) zeigt die korrekte Schaltung.

Zunaechst sind alle Bauteile ausser dem SHT31 zu bestuecken und die Platine bspw. mit Isopropanol von Flussmittelresten zu reinigen. Ganz zum Schluss wird der SHT31 bestueckt und die Schutzkappe aufgesetzt. Der Sensor darf Loesungsmitteln nicht ausgesetzt werden.

Waehrend die Bestueckung und Loetung von Hand fuer fast alle Bauteile unproblematisch ist, gestaltet sich die Handloetung des SHT31 und VEML6030 aber etwas schwieriger. Mit Lupenbrille oder Mikroskop und geregeltem Loetkolben mit guter Spitze ist dies moeglich. Empfehlenswerter ist jedoch Reflowloeten, wobei die Sensoren nicht von oben mit Heissluft oder Infrarotstrahlung erhitzt werden sollten. Da die Platinenunterseite unter den Sensoren frei ist, bietet sich Erhitzung von unten mit Heissluft hat. Das Auftragen der Lotpaste wird mittels Schablone stark vereinfacht - Standardschablonen sind bspw. bei [Proto-Advantage](https://www.proto-advantage.com/) erhaeltlich.

Der besonders der Witterung ausgesetzte schmale (obere) Teil der Platine sollte wenn moeglich dennoch mittels Schutzlack geschuetzt werden, um die Witterungsbestaendigkeit zu erhoehen. Auch hierbei ist der SHT31 vor Loesungsmitteln zu schuetzen, bspw. durch Aufpinseln statt Spruehen von [FSC](https://electrolube.com/product/fsc-flexible-silicone-conformal-coating/) auf die Platine, nicht jedoch auf die Schutzkappe des Sensors, und Trocknung in einem gut beluefteten warmen Raum.

Beim Einbau der Platine in das Gehaeuse sollte die Aussparung, durch die der schmale Platinenteil sowie der Antennendraht gefuehrt werden, beidseitig wie im Foto zu sehen mit neutral vernetzendem Silikon abgedichtet werden.

![Einbau Main](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_encl.jpg)


### Windrichtungsmesserplatine

Auch wenn die Original-Windrichtungsmesserplatine kaum korrosionsanfaellig montiert und vermutlich nicht defekt ist, wurde zur einfacheren Einbindung in die gewaehlte AVR-Erchitektur auch diese neu erstellt. Genutzt wird ein [AS5600](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/datasheets/AS5600.pdf) Halleffekt-Drehwinkelsensor, der ueber einen FET auf der Hauptplatine zwischen den Messungen zur Verringerung des Stromverbrauchs abgeschaltet wird.

Der Aufbau erfolgt ohne Besonderheiten. Der Loetjumper SJ1 ist entsprechend dem Foto gegen Masse zu schliessen.

![PCB Top 2](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/winddir_top.jpg)

![PCB Bottom 2](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/winddir_bot.jpg)



## Firmware

Zur Verwendung des ATMega1284p ist bei Nutzung der Arduino IDE zunaechst eine zusaetzliche sog. Boardverwalter-URL (https://mcudude.github.io/MightyCore/package_MCUdude_MightyCore_index.json) in den Voreinstellungen zu hinterlegen. Folgende Boardeinstellungen sind dann auszuwaehlen:

![Boardeinstellungen](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/arduino_board.jpg)

Danach kann der Bootloader geflashed werden.

Sollte versehentlich externer Takt oder Quarzoszillator eingestellt oder die entsprechenden Fuses anderweitig falsch gesetzt worden sein, kann auf der Platine ein 8MHz-Resonator CSTNE von Murata bestueckt werden, um den Controller zu retten. Fuer den Normalbetrieb ist der Resonator nicht notwendig.

Um die [Firmware](https://github.com/HMSteve/HB-WDS100-C6-O-2/tree/main/Firmware)  zu kompilieren, muessen sich die .ino sowie die .h Dateien im gleichen Verzeichnis befinden, das ./sensors-Verzeichnis darunter. Zudem muss eine Reihe von Bibliotheken ueber den Library Manager eingebunden werden:
- AskSinPP
- EnableInterrupt
- LowPower
- Adafruit_SHT31 Arduino Library
- PCF8583 Library (Diese wird hier fuer den stromsparenderen PCF8593 genutzt.)

Anschliessend sollte die Firmware problemlos kompilierbar und das Device nach dem Flashen anlernbar sein.



## Bedienung

### Geraetetaste

Die Bedienung erfolgt grundsaetzlich analog dem originalen HM-WDS100-C6-O-2. Lediglich der Live Mode ist nicht implementiert.
Die Geraetetaste weist folgende Funktionen auf:
- kurzer Druck: Anlernmodus starten / Konfiguration uebernehmen (LED langsam rot blinkend)
- langer Druck: Einnordungsmodus starten (kurzes Blinkmuster der roten LED)
- sehr langer Druck: Zuruecksetzen auf Werkseinstellung (einmaliges Aufleuchten der gruenen LED)

### Einnorden

Bei Erstinstallation nach Montage ist der Einnordungsmodus durch langen Druck der Geraetetaste zu starten, die Windfahne Richtung Norden auszurichten und so festzuhalten und sodann die Geraetetaste kurz zu druecken. Das Abspeichern des absoluten Winkels der Nordrichtung im EEPROM wird durch Aufleuchten der gruenen LED bestaetigt. Erfolgt dies nicht, ist der Vorgang zu wiederholen.

### Helligkeit und Windgeschwindigkeit

Um den geringen Dynamikumfang des Originalsensors etwas zu verbessern und insbesondere Helligkeiten in der Daemmerung sinnvoll auswerten zu koennen, wird die Beleuchtungsstaerke in Lux logarithmiert und auf den 8-Bit-Wertebereich skaliert ausgegeben. 100 kLux entsprechen etwa einem Wert von 220, 100 Lux noch immer etwa einem Wert von 80.

Die Windgeschwindigkeit wurde mit Hausmitteln (Hand-Schalenanemometer und Laubblasgeraet) kalibriert, bei 35 Messungen ergab sich eine Regression mit Bestimmtheitsmass 0.9. Im Gegensatz zum Originalsensor werden auch hoehere Windgeschwindigkeiten noch gemessen.

### Hinweise zum Energieverbrauch

Die mittlere Stromaufnahme des Sensors bei einem Sendeintervall von etwa 3 Minuten betraegt knapp 30uA, so dass eine Batterielaufzeit von einigen Jahren erreichbar sein sollte.  



## Disclaimer

Die Nutzung der hier veroeffentlichten Inhalte erfolgt vollstaendig auf eigenes Risiko und ohne jede Gewaehr.


## Lizenz

**Creative Commons BY-NC-SA**<br>
Give Credit, NonCommercial, ShareAlike

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
