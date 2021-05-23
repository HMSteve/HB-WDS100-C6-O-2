# HB-WDS100-C6-O-2

![HB-WDS100-C6-O-2](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_encl.jpg)

Ein "Reparaturset" fuer den Wetter-Kombisensor HB-WDS100-C6-O-2 auf Basis der AskSinPP-Bibliothek. Da Ausfaelle des originalen Wetter-Kombisensors wegen Korrosion der Hauptplatine offenbar nach einigen Jahren haeufiger vorkommen, die eigentliche Mechanik jedoch langlebig erscheint, wurde eine Ersatz-Elektronik sowie AskSinPP-basierte Firmware entwickelt, die es erlaubt, den Sensor ohne Modifikationen oder Addon-Installationen auf der CCU wieder zum Leben zu erwecken. Dazu sind sowohl die Hauptplatine als auch die Windrichtungsmesserplatine gegen die neu konzipierten Platinen auszutauschen und der Sensor neu anzulernen.

## Hardware

Die Demontage des die Hauptplatine (und Batteriefach sowie Anemometer) tragenden Geraeteteils muss vorsichtig in folgenden Schritten erfolgen, um Schaeden an den Plastikteilen zu vermeiden:
1. Gehaeusehuelse ueber dem Batteriefach oeffnen und Batterien entfernen, Westernstecker loesen.
2. Beide Schrauben, die den Rotor mit den drei Schalen auf der Welle halten, leicht loesen und Rotor abziehen.
3. Zwei unter der Rotorkappe sichtbarwerdende virkantige Arretierungsstifte aus Platik vorsichtig nach oben herausziehen (etwas mit dem Schraubenzieher hebeln, dann mit der Zange greifen und senkrecht herausziehen)
4. Die drei Ringe zu Sonnen- und Wetterschutz (Stevenson-Screen) mit dem oberen beginnend leicht im Urzeigersinn drehen, dass sich der jeweilige Bajonettverschluss loest, und Ring senkrecht nach oben abziehen.
5. Zwei kleine Schrauben ganz oben am Lager der Welle sowie danach fuenf lange Gehaeuseschrauben entfernen.
6. Zwei Schrauben, die den Aufbau auf dem Edelstahlrohr fixieren, loesen und Aufbau senkrecht nach oben einige cm aus dem Rohr abziehen
7. Die beiden Gehaeusehalbschalen lassen sich nun trennen und die defekte Platine ausbauen.

Die Demontage der Windrichtungsmesserplatine ist unproblematisch.

Als Ersatz sind zwei neue Platinen herzustellen: [Schalt- und Bestueckungsplaene sowie Gerberfiles](https://github.com/HMSteve/HB-WDS100-C6-O-2/tree/main/PCB).

### Hauptplatine

Die Platine basiert auf einem ATMega1284p. Die Temperatur-/Luftfeuchtemessung erfolgt mit einem SHT31, die Helligkeitsmessung mit einem VEML6030. Die Windgeschwindigkeit wird wie im Original per Reedkontakt gezaehlt, zur Reduzierung des Stromverbrauchs wird hierfuer ein PCF8593 eingesetzt.

Ein paar Fotos vom Aufbau:

![PCB Top](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_top.jpg)

![PCB Bottom](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_bot.jpg)

Bei Nutzung der aktuellen Platine v2 sind zwei Fehler zu korrigieren. Dazu ist jeweils etwas Loetstoplack zu entfernen, um die geaenderten Bauteilpostionen bestuecken zu koennen.

![PCB Bottom Changes](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_bot_pcbchanges.jpg)

1. C17 ist nicht direkt am CC1101-Modul zu bestuecken, sondern direkt vor dem FET zur Abschaltung der Betriebsspannung bzw der vorhandenen Durchkontaktierung gegen die Masseflaeche. Ohne diese Aenderung koennen die Einschaltstromspitzen zu einem Brownout des ATMega fuehren und die Schaltung laeuft nicht oder geraet in eine Reset-Schleife.
2. Der markierte 3.3M-Pullup-Widerstand gegen 3.3V fehlt im Layout und ist wie gezeigt zu ergaenzen.

Zunaechst sind alle Bauteile ausser dem SHT31 zu bestuecken und die Platine bspw. mit Isopropanol von Flussmittelresten zu reinigen. Ganz zum Schluss wird der SHT31 bestueckt und die Schutzkappe aufgesetzt. Der Sensor darf Loesungsmitteln nicht ausgesetzt werden.

Der besonders der Witterung ausgesetzte schmale (obere) Teil der Platine sollte wenn moeglich dennoch mittels Schutzlack geschuetzt werden, um die Witterungsbestaendigkeit zu erhoehen. Auch hierbei ist der SHT31 vor Loesungsmitteln zu schuetzen, bspw. durch Aufpinseln statt Spruehen von [FSC](https://electrolube.com/product/fsc-flexible-silicone-conformal-coating/) auf die Platine, nicht jedoch auf die Schutzkappe des Sensors, und Trocknung in einem gut beluefteten warmen Raum.

Beim Einbau der Platine in das Gehaeuse sollte die Aussparung, durch die der schmale Platinenteil sowie der Antennendraht gefuehrt werden, beidseitig wie im Foto zu sehen mit neutral vernetzendem Silikon abgedichtet werden.

![Einbau Main](https://github.com/HMSteve/HB-WDS100-C6-O-2/blob/main/images/main_encl.jpg)


### Windrichtungsmesserplatine

Auch wenn die Original-Windrichtungsmesserplatine kaum korrosionsanfaellig montiert und vermutlich nicht defekt ist, wurde zur einfacheren Einbindung in die gewaehlte AVR-Erchitektur auch diese neu erstellt. Genutzt wird ein AS5600 Halleffekt-Drehwinkelsensor, der ueber einen FET auf der Hauptplatine zwischen den Messungen zur Verringerung des Stromverbrauchs abgeschaltet wird.

Der Aufbau erfolgt ohne Besonderheiten.

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

Es gibt neben der AskSinPP-Config-Taste eine weitere Bedientaste. Ein kurzer Druck auf diese schaltet die Ampel-LED ein bzw. aus, ein langer Druck fuehrt zu einer forced calibration des SCD30. Hierzu sollte sich der Sensor bereits einige Minuten an der frischen Luft ohne nennenswerte Druckschwankungen durch Wind befunden haben. Der lange Tastendruck kalibriert dann den Sensor auf den im WebUI hinterlegten CO2-Referenzwert (im Freien ca. 410ppm). Die automatische Kalibrierung wird nicht verwendet auf Basis der These, dass ein Wegdriften der Auto-Kalibrierung bei nicht ausreichendem regelmaessigem Lueften problematischer ist als die Alterungsdrift nach forced calibration. Langzeitbeobachtungen hierzu fehlen mir jedoch.

Unterschreitet die Akkuspannung 2.2V, wird ein Warnsymbol im Display angezeigt und ein USB-Ladegerate sollte angeschlossen werden. Geschieht das nicht und die Spannung sinkt unter 2.0V, schaltet sich der Sensor zum Schutz vor Tiefentladung ab und zeigt dies an. Ein Reset (Wiedereinschalten) erfolgt automatisch beim Anschluss eines Ladegeraetes. Die gruene LED zeigt das Anliegen der Ladespannung, die gelbe den Schnelladevorgang. Es ist zu beachten, dass die Erwaermung beim Schnelladen natuerlich die Messwerte im Gehaeuse verfaelscht.

Die Schaltschwellen der Ampelfarben, die Hoehe ueber NN sowie ein vom SCD30-Temperaturmesswert zu subtrahierender Offset zur Korrektur der Anzeige koennen im WebUI konfiguriert werden.

![WebUI Settings](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/webui_settings.jpg)

Die Messwerte werden dann so angezeigt:

![WebUI Status](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/webui_status.jpg)

### Hinweise zum Energieverbrauch

Der SCD30 zieht einen nennenswerten Ruhestrom (mind. 5mA). Ein Power Cycling wird vom Hersteller nicht empfohlen, siehe [hier](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Addl/CD_AN_SCD30_Low_Power_Mode_D2.pdf).
Damit ist fuer das Geraet bei der Voreinstellung einer Sensor-Abtastperiode von 16s eine Akkulaufzeit von hoechstens 5 Tagen zu erwarten. Das heisst, der mobile Betrieb waehrend eines oder einiger (Arbeits)tage ist problemslos moeglich. Der Dauerbetrieb sollte jedoch eher mit USB-Netzteil erfolgen. Dabei versorgt nach Ende der Schnelladung der von der MAX712-Ladeschaltung gelieferte, von R20 gemaess [Datenblatt MAX712](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Addl/MAX712-MAX713.pdf) feinjustierte Erhaltungsladestrom das Device.  

Hier eine Messung zum Stromverbrauch:

![Power Consumption Detail](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/power_consumption_detail.jpg)

Ein Excel-Workbook zum power budgeting findet sich [hier](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Addl/BatteryBudgeting.xlsx).


## Gehaeuse

Zwischen Display und Platine sind vier zu druckende Abstandshuelsen einzusetzen. Das Gehaeuse stellt beim Drucken keine groesseren Herausforderungen. Die LED-Loecher sind ggf. mit 10.5mm bzw. 3.5mm aufzubohren, die Loecher im Boden mit 3mm.
Fuer die Befestigung der Platine sind [M3-Einpressmuttern](https://www.amazon.de/dp/B08BCRZZS3) vorgesehen. Die Befestigung der Platine im Gehaeuse erfolgt mit M3x10 Distanzbolzen. Die Rueckwand wird dann mit M3x10 Zylinderkopfschrauben an diesen befestigt.


## Disclaimer

Die Nutzung der hier veroeffentlichten Inhalte erfolgt vollstaendig auf eigenes Risiko und ohne jede Gewaehr.


## Lizenz

**Creative Commons BY-NC-SA**<br>
Give Credit, NonCommercial, ShareAlike

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
