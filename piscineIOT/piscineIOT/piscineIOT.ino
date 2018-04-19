/*
 Name:		piscineIOT.ino
 Created:	19/04/2018 14:12:28
 Author:	Eliott
*/
/*TODO
capteur :
-temp ds1820 lib one wire
-ultrason ??? lib ??
-bac tampon
-bidon ph+
-bidon chlore
-sondePH ??? lib phMetre.h

control :
-pompe chlore
-pompe PH+
-relay piscine ???
-relay robot
-valve robot ???

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

char auth[] = " ccad56d5e6114323adf3f93ab84a1178";
BlynkTimer timer;
//Blynk

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x10 }; //0x10 = Piscine

WidgetTerminal terminal(V30);


void setup()
{
	Serial.begin(9600);

	//Blynk.begin(auth);
	// You can also specify server:
	Blynk.begin(auth, "ec2-18-194-145-182.eu-central-1.compute.amazonaws.com", 8442);
	//Blynk.begin(auth, "grabiehouse.ddns.net", 8442);
	//Blynk.begin(auth, IPAddress(18,194,145,182), 8442);

	// Setup a function to be called every 15min
	timer.setInterval(15 * 60000L, Main);

}

void loop()
{
	Blynk.run();
	timer.run();
}

void Main() {
	Serial.println("Coucou 15 min");
}

BLYNK_CONNECTED() {
	Blynk.syncAll();
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

