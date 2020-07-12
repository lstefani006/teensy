#include <SPI.h>
#include <LoRa.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//define the pins used by the LoRa transceiver module
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SCK 5
#define LORA_CS 18
#define LORA_IRQ 26 // LORA_DIO0
#define LORA_RST 14
#define LORA_BAND 868E6

// https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
// http://www.lilygo.cn/prod_view.aspx?TypeId=50003&Id=1142&FId=t3:50003:3


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

const bool tx = false;

void onReceive(int packetSize);

void setup()
{
	Serial.begin(9600);
	while (!Serial)
		;

	delay(2000);

	pinMode(LED_BUILTIN, OUTPUT);

	// reset OLED display via software
	Wire.begin(OLED_SDA, OLED_SCL);
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, true, false))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			;
	}

	display.clearDisplay();
	display.setTextSize(1);		 // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
	display.setCursor(0, 0);	 // Start at top-left corner
	display.println("Hello, world!");
	display.display();

	if (tx)
	{
		Serial.println("LoRa TX");
		display.println("LoRa TX");
	}
	else
	{
		Serial.println("LoRa RX");
		display.println("LoRa RX");
	}

	SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
	LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
	if (!LoRa.begin(LORA_BAND))
	{
		Serial.println("Starting LoRa failed!");
		while (1)
			;
	}

	if (!tx)
	{
		display.clearDisplay();
		display.setTextSize(1);		 // Normal 1:1 pixel scale
		display.setTextColor(WHITE); // Draw white text
		display.setCursor(0, 0);	 // Start at top-left corner
		display.println("Ricezione");
		display.display();
	}

	LoRa.onReceive(onReceive);
	//LoRa.disableInvertIQ(); // normal mode
	LoRa.receive();
}

void displayLoraData(int packetSize, String packet, int rssi, float SN, long freqErr);

void loop()
{
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
	delay(100);

	if (tx)
	{
		// toggle the led to give a visual indication the packet was sent
		digitalWrite(LED_BUILTIN, HIGH);
		delay(250);
		digitalWrite(LED_BUILTIN, LOW);
		delay(250);

		// send packet
		static int counter = 0;

		LoRa.beginPacket();
		LoRa.print("Ciao ");
		LoRa.print(counter);
		LoRa.endPacket();

		display.clearDisplay();
		display.setTextSize(1);		 // Normal 1:1 pixel scale
		display.setTextColor(WHITE); // Draw white text
		display.setCursor(0, 0);	 // Start at top-left corner
		display.println("TX " + String(counter, DEC) + " bytes");
		display.display();

		Serial.println("TX " + String(counter, DEC) + " bytes");

		counter++;
	}
	else if (false)
	{
		// receive packet
		int packetSize = LoRa.parsePacket();
		if (packetSize)
		{
			String rx;
			while (LoRa.available())
				rx += (char)LoRa.read();

			int RSSI = LoRa.packetRssi();
			float SN = LoRa.packetSnr();
			long freqErr = LoRa.packetFrequencyError();

			displayLoraData(packetSize, rx, RSSI, SN, freqErr);

			digitalWrite(LED_BUILTIN, HIGH);
			delay(100);
			digitalWrite(LED_BUILTIN, LOW);
			delay(100);
		}
	}
}

void displayLoraData(int packetSize, String packet, int rssi, float SN, long freqErr)
{
	display.clearDisplay();
	display.setTextSize(1);		 // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
	display.setCursor(0, 0);	 // Start at top-left corner

	display.print("RX     ");
	display.print(packetSize);
	display.println(" bytes");
	display.print("RX   = ");
	display.println(packet);
	display.print("RSSI = ");
	display.println(rssi);
	display.print("SN   = ");
	display.println(SN);
	display.print("FERR = ");
	display.println(freqErr);
	display.display();
}

/*
Siamo a livello di interrupt.
Se si prova ad accendere il led ... non lampeggia
Il display non funziona.
LoRa.packetFrequencyError(); non funziona
*/
void onReceive(int packetSize)
{
	Serial.println("RX " + String(packetSize, DEC) + " bytes");

	String rx;
	for (int i = 0; i < packetSize; i++)
		rx += (char)LoRa.read();

	int RSSI = LoRa.packetRssi();
	float SN = LoRa.packetSnr();
	// long freqErr = LoRa.packetFrequencyError(); questa non funge con interrupt

	Serial.println("RX: " + rx);
	Serial.print("RSSI = ");
	Serial.println(RSSI);
	Serial.print("SN   = ");
	Serial.println(SN);
	/*	
	Serial.print("FERR = ");
	Serial.println(freqErr);
	*/
}