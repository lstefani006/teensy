#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

// Board Details
#define NAME "TTGO T-Beam v0.7"
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SCK 5
#define LORA_CS 18
#define LORA_IRQ 26 // LORA_DIO0
#define LORA_RST 23 // nel TTGO-LORA3 è il 14
#define LORA_IO1 33 // HPDIO1 (nel T-Beam)
#define LORA_IO2 32 // HPDIO2 (nel T-Beam)

const long frequency = 868E6; // 915E6; // LoRa Frequency

void LoRa_rxMode();
void LoRa_txMode();
void LoRa_sendMessage(String message);

void onReceive(int packetSize);
void onTxDone();

boolean runEvery(unsigned long interval);

void setup()
{
	delay(2000);
	Serial.begin(9600); // initialize serial
	while (!Serial)
		;

	SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI/*, LORA_CS*/);
	LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

	if (!LoRa.begin(frequency))
	{
		Serial.println("LoRa init failed. Check your connections.");
		while (true)
			; // if failed, do nothing
	}

	Serial.println("LoRa init succeeded.");
	Serial.println();
	Serial.println("LoRa Simple Node");
	Serial.println("Only receive messages from gateways");
	Serial.println("Tx: invertIQ disable");
	Serial.println("Rx: invertIQ enable");
	Serial.println();

	LoRa.onRxTx(onReceive, onTxDone);
/*
	LoRa.onReceive(onReceive);
	LoRa.onTxDone(onTxDone);
	*/
	LoRa_rxMode();
}

void loop()
{
	if (runEvery(1000))
	{ // repeat every 1000 millis

		String message = "HeLoRa World! ";
		message += "I'm a Node! ";
		message += millis();

		LoRa_sendMessage(message); // send a message

		Serial.println("Send Message!");
	}
}

void LoRa_rxMode()
{
	LoRa.enableInvertIQ(); // active invert I and Q signals
	LoRa.receive();		   // set receive mode
}

void LoRa_txMode()
{
	LoRa.idle();			// set standby mode
	LoRa.disableInvertIQ(); // normal mode
}

void LoRa_sendMessage(String message)
{
	LoRa_txMode();		  // set tx mode
	LoRa.beginPacket();	  // start packet
	LoRa.print(message);  // add payload
	LoRa.endPacket(true); // finish packet and send it
}

void onReceive(int packetSize)
{
	String message = "";

	while (LoRa.available())
	{
		message += (char)LoRa.read();
	}

	Serial.print("Node Receive: ");
	Serial.println(message);
}

void onTxDone()
{
	Serial.println("TxDone");
	LoRa_rxMode();
}

boolean runEvery(unsigned long interval)
{
	static unsigned long previousMillis = 0;
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= interval)
	{
		previousMillis = currentMillis;
		return true;
	}
	return false;
}
