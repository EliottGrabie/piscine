/*
 Name:		piscineIOT.ino
 Created:	19/04/2018 14:12:28
 Author:	Eliott
*/
/*TODO

Temps après déclenchement intentionnel de la pompe de la piscine

capteur :
-temp ds1820 lib one wire V5 Bon
-ultrason ??? lib ?? 
-bac tampon V6 Bon
-bidon ph+ V7
-bidon chlore V8
-sondePH ??? lib phMetre.h V9 

control :
-pompe chlore 
-pompe PH+ 
-relay piscine ??? V10 Bon
-relay robot V11
-valve robot ???

-Creneau1 V20
-Creneau2 V21
-Creneau3 V22
-Creneau4 V23
-Creneau5 V24

-chlore/j V12
-Ph/j v13


notion :
-horloge
-cycle loop 5/10/30/60min ?
-event ???

idee:
-pour stat: entree valeur chlore journaliere sur le tel blynk save auto in base





*/
//#define BLYNK_DEBUG // Optional, this enables lots of prints
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WidgetRTC.h>

char auth[] = " ccad56d5e6114323adf3f93ab84a1178";
BlynkTimer timer;
//Blynk

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x10 }; //0x10 = Piscine

WidgetTerminal terminal(V30);

#define ONE_WIRE_BUS 3

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
float tempDS18 = 0;

/* Constantes pour les broches de l'ultrason */
#define TRIGGER_PIN 5 // Broche TRIGGER
#define ECHO_PIN 6    // Broche ECHO
/* Constantes pour le timeout */
const unsigned long MEASURE_TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s
const float SOUND_SPEED = 340.0 / 1000; /* Vitesse du son dans l'air en mm/us */

#define PISCINE_SWITCH 8

WidgetRTC rtc;

int lHeureDebut[5];
int lHeureFin[5];

void setup()
{
	pinMode(PISCINE_SWITCH, OUTPUT);
	digitalWrite(PISCINE_SWITCH, HIGH);

	Serial.begin(9600);

	sensors.begin();
	delay(1000);

	pinMode(TRIGGER_PIN, OUTPUT);
	digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit être à LOW au repos
	pinMode(ECHO_PIN, INPUT);
	pinMode(ECHO_PIN, INPUT);

	//Blynk.begin(auth);
	// You can also specify server:
	Blynk.begin(auth, "ec2-18-194-145-182.eu-central-1.compute.amazonaws.com", 8442);
	//Blynk.begin(auth, "grabiehouse.ddns.net", 8442);
	//Blynk.begin(auth, IPAddress(18,194,145,182), 8442);

	// Setup a function to be called every 1min
	timer.setInterval(0.05 * 60000L, Main);

}

void loop()
{
	Blynk.run();
	timer.run();
}

void Main() {

	sensors.requestTemperatures();
	tempDS18 = sensors.getTempCByIndex(0);
	Blynk.virtualWrite(V5, tempDS18);

	/* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
	digitalWrite(TRIGGER_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER_PIN, LOW);

	/* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
	long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);

	/* 3. Calcul la distance à partir du temps mesuré */
	float distance_mm = (measure / 2.0 * SOUND_SPEED)/10;
	Blynk.virtualWrite(V6, distance_mm);

	String currentTime = String(hour()) + ":" + minute() + ":" + second();
	String currentDate = String(day()) + " " + month() + " " + year();
	Serial.print("Current time: ");
	Serial.print(currentTime);
	Serial.print(" ");
	Serial.print(currentDate);
	Serial.println();
	Serial.println(check_hour(hour(), lHeureDebut, lHeureFin));
	

}

bool check_hour(int _currentTime,int _lHeureDebut[], int _lHeureFin[]) {
	for (size_t i = 0; i < 5; i++) {
		if (_currentTime >= _lHeureDebut[i] && _currentTime < _lHeureFin[i]) {
			return true;
		}
	}
	return false;

}

BLYNK_CONNECTED() {
	Blynk.syncAll();
	rtc.begin();
}

BLYNK_WRITE(V30)
{

	// if you type "Marco" into Terminal Widget - it will respond: "Polo:"
	if (String("Debug") == param.asStr()) {
		//terminal.println(" ");
		terminal.println("COUCOU");

	}
	else {

		// Send it back
		terminal.println("Commande inconnu");
		//terminal.write(param.getBuffer(), param.getLength());
		terminal.println("Debug ?");
	}

	// Ensure everything is sent
	terminal.flush();
}

BLYNK_WRITE(V10)
{
	digitalWrite(PISCINE_SWITCH, !param.asInt());
}

BLYNK_WRITE(V20) {
	long startTimeInSecs = param[0].asLong();
	long stopTimeInSecs = param[1].asLong();
	lHeureDebut[0] = int(startTimeInSecs / 3600);		//startTimeInSecs to hour
	lHeureFin[0] = int(stopTimeInSecs / 3600);		//stopTimeInSecs to hour
}
BLYNK_WRITE(V21) {
	long startTimeInSecs = param[0].asLong();
	long stopTimeInSecs = param[1].asLong();
	lHeureDebut[1] = int(startTimeInSecs / 3600);		//startTimeInSecs to hour
	lHeureFin[1] = int(stopTimeInSecs / 3600);		//stopTimeInSecs to hour
}
BLYNK_WRITE(V22) {
	long startTimeInSecs = param[0].asLong();
	long stopTimeInSecs = param[1].asLong();
	lHeureDebut[2] = int(startTimeInSecs / 3600);		//startTimeInSecs to hour
	lHeureFin[2] = int(stopTimeInSecs / 3600);		//stopTimeInSecs to hour
}
BLYNK_WRITE(V23) {
	long startTimeInSecs = param[0].asLong();
	long stopTimeInSecs = param[1].asLong();
	lHeureDebut[3] = int(startTimeInSecs / 3600);		//startTimeInSecs to hour
	lHeureFin[3] = int(stopTimeInSecs / 3600);		//stopTimeInSecs to hour
}
BLYNK_WRITE(V24) {
	long startTimeInSecs = param[0].asLong();
	long stopTimeInSecs = param[1].asLong();
	lHeureDebut[4] = int(startTimeInSecs / 3600);		//startTimeInSecs to hour
	lHeureFin[4] = int(stopTimeInSecs / 3600);		//stopTimeInSecs to hour
}

