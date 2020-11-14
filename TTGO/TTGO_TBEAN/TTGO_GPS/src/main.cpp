#include <Arduino.h>
#include <LoRa.h>
#include <Wire.h>

// GPS
// TX=34
// RX=12

// LORA (notare che sono definiti ANCHE IN pins_arduino.h - variants/ttgo-lora32-v1)
#undef LORA_SCK
#undef LORA_MISO
#undef LORA_MOSI
#undef LORA_CS
#undef LORA_RST
#undef LORA_IRQ

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 23
#define LORA_DI0 26
#define LORA_BAND 868E6

/*
LEO: led funzionamento

rosso vicino al NEO ==> satellite agganciato
blu   vicino al NEO ==> IO14
verde vicino al Lora ==> batt carica
rosso vicino al Lora ==> batt in carica
*/
const int LED_14 = 14;

#include <TinyGPS++.h>

#ifdef AXP
#include "axp20x.h"
AXP20X_Class pmu;
void AXP192_power(bool on)
{
	if (on)
	{
		pmu.setPowerOutPut(AXP192_LDO2, AXP202_ON);	 // Lora on T-Beam V1.0
		pmu.setPowerOutPut(AXP192_LDO3, AXP202_ON);	 // Gps on T-Beam V1.0
		pmu.setPowerOutPut(AXP192_DCDC1, AXP202_ON); // OLED on T-Beam v1.0
		// pmu.setChgLEDMode(AXP20X_LED_LOW_LEVEL);
		pmu.setChgLEDMode(AXP20X_LED_BLINK_1HZ);
	}
	else
	{
		pmu.setChgLEDMode(AXP20X_LED_OFF);
		pmu.setPowerOutPut(AXP192_DCDC1, AXP202_OFF);
		pmu.setPowerOutPut(AXP192_LDO3, AXP202_OFF);
		pmu.setPowerOutPut(AXP192_LDO2, AXP202_OFF);
	}
}
void AXP192_init()
{
	Wire.begin(21, 22);
	if (pmu.begin(Wire, AXP192_SLAVE_ADDRESS))
	{
		Serial.println("AXP192 PMU initialization failed");
	}
	else
	{
		Serial.println("Initialzation...");
		// configure AXP192
		pmu.setDCDC1Voltage(3500);				// for external OLED display
		pmu.setTimeOutShutdown(false);			// no automatic shutdown
		pmu.setTSmode(AXP_TS_PIN_MODE_DISABLE); // TS pin mode off to save power

		// switch ADCs on
		pmu.adc1Enable(AXP202_BATT_VOL_ADC1, true);
		pmu.adc1Enable(AXP202_BATT_CUR_ADC1, true);
		pmu.adc1Enable(AXP202_VBUS_VOL_ADC1, true);
		pmu.adc1Enable(AXP202_VBUS_CUR_ADC1, true);

		// switch power rails on
		AXP192_power(true);

		Serial.println("AXP192 PMU initialized");
	}
}
#endif

static const int GPS_RXPin = 12, GPS_TXPin = 15;
static const uint32_t GPS_Baud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

void I2C_Scan()
{
	Wire.begin(21, 22); //

	int nDevices = 0;
	for (auto address = 1; address < 127; address++)
	{
		Wire.beginTransmission(address);
		auto error = Wire.endTransmission();
		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address < 16)
			{
				Serial.print("0");
			}
			Serial.println(address, HEX);
			nDevices++;
		}
		else if (error == 4)
		{
			Serial.print("Unknow error at address 0x");
			if (address < 16)
			{
				Serial.print("0");
			}
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0)
	{
		Serial.println("No I2C devices found\n");
	}
	else
	{
		Serial.println("done\n");
	}
}

void LoRa_Setup();

void setup()
{
	Serial.begin(9600);
	Serial1.begin(GPS_Baud, SERIAL_8N1, GPS_RXPin, GPS_TXPin);

	pinMode(LED_14, OUTPUT);

	delay(3000);
	I2C_Scan();
	delay(3000);
	Serial.println("FATTO");
	delay(5000);

#ifdef AXP
	// put your setup code here, to run once:
	AXP192_init();
	AXP192_power(true);
#endif

	Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
	Serial.print(F("Testing TinyGPS++ library v. "));
	Serial.println(TinyGPSPlus::libraryVersion());
	Serial.println();

	LoRa_Setup();
}

String LoRa_Packet = "";
volatile bool LoRa_Packet_Ready = false;
void LoRa_OnReceive(int pckSize)
{
	LoRa_Packet = "";
	while (LoRa.available())
		LoRa_Packet += (char)LoRa.read();

	//Serial.println("RX=" + incoming);
	LoRa_Packet_Ready = true;
}

/*
void LoRa_Receive()
{
	String a;
	while (LoRa.available())
		a += (char)LoRa.read();
	Serial.println("RX=" + a);
}
*/

/*

SX1276

Chirp Il segnale modulato in FM si sposta come frequenza in modo da occupare -SignlBandiwth/2 +SignlBandiwth/2 

setSignalBandwidth  ==> imposta la banda occupata dal "Chirp" valori 500khz/250khz/125khz/... può scendere fino 7800
                        questa funzione ritorna proprio 500000 ecc

setSpreadingFactor  ==> è la durata nel tempo di un Chirp   == default = 7
                        6 è la durata più corta --- 12 è la durata più lunga
						ogni passo si  raddoppia il tempo in aria per trasmettere lo STESSO ammontare di dati
						con 6 bisogna usare gli "implicit header" ( beginPacket(int implicitHeader = false) )  

DataRate  Configuration bits/s   Max payload
DR0       SF12/125kHz 	250       59
DR1       SF11/125kHz 	440       59
DR2       SF10/125kHz 	980       59
DR3       SF9/125kHz 	1.760    123
DR4       SF8/125kHz 	3.125    230
DR5       SF7/125kHz 	5.470    230
DR6       SF7/250kHz 	11.000   230
DR7       FSK: 50kpbs 	50.000   230

I canali per l'europa sono 8 - distanti .30 Mhz
Channel NumberCentral frequencyChannel 
CH_10_868	865.20 MHz
CH_11_868	865.50 MHz
CH_12_868	865.80 MHz
CH_13_868	866.10 MHz
CH_14_868	866.40 MHz
CH_15_868	866.70 MHz
CH_16_868	867.00 MHz
CH_17_868	868.00 MHz  <== questo canale non dista dal precedente di 1,0Mhz

per impostare il chanale
setFrequency(f)   <== es 868E6 

setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN) ==> imposta la potenza
	2..20 con PA_OUTPUT_PA_BOOST_PIN, 
	0..14 con PA_OUTPUT_RFO_PIN

setCodingRate4 ==> imposta la codifica - valore di default=5 range=[5..8] che corrispndono a 4/5..4/8 con 4 è il core rate fixed
	5 bassa affidabilità, alto data rate
	8 alta affidabilità, basso data rate.

*/


// LoRaWAN Parameters
#define BAND    868100000  //you can set band here directly,e.g. 868E6,915E6
#define PABOOST false
#define TXPOWER 14
#define SPREADING_FACTOR 12
#define BANDWIDTH 125000
#define CODING_RATE 5
#define PREAMBLE_LENGTH 8
#define SYNC_WORD 0x34

void configForLoRaWAN()
{
  LoRa.setTxPower(TXPOWER);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setSignalBandwidth(BANDWIDTH);
  LoRa.setCodingRate4(CODING_RATE);
  LoRa.setPreambleLength(PREAMBLE_LENGTH);
  LoRa.setSyncWord(SYNC_WORD);
  LoRa.crc();
}


void LoRa_Setup()
{
	// LORA Setup
	SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
	LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
	if (!LoRa.begin(LORA_BAND))
	{
		Serial.println("Starting LoRa failed!");
		for (;;)
			;
	}

	LoRa.onReceive(LoRa_OnReceive);
	LoRa.receive();
}

void LoRa_Send(String msg)
{
	// send packet
	LoRa.beginPacket();
	LoRa.print(msg);
	LoRa.endPacket();

	LoRa.receive();
}

void displayInfo();

void loop()
{
	// LoRa_Receive();
	if (LoRa_Packet_Ready)
	{
		String t = LoRa_Packet;
		Serial.println("RX = " + t);
		LoRa_Packet_Ready = false;
	}

	while (Serial1.available() > 0)
	{
		auto c = Serial1.read();
		auto v = digitalRead(LED_14);
		digitalWrite(LED_14, !v);
		if (gps.encode(c))
		{
			static int nn = 0;
			String s(nn++, DEC);
			LoRa_Send("Leo=" + s);
			displayInfo();
		}
	}
}

void displayInfo()
{
	Serial.print(F("LOC: "));
	if (gps.location.isValid())
	{
		char c = gps.location.isUpdated() ? '*' : ' ';
		Serial.write(c);
		Serial.print(gps.location.lat(), 6);
		Serial.print(F(","));
		Serial.print(gps.location.lng(), 6);
	}
	else
	{
		Serial.print(F("???"));
	}

	Serial.print(F(" "));
	if (gps.date.isValid())
	{
		char c = gps.date.isUpdated() ? '*' : ' ';
		Serial.write(c);
		Serial.printf("%02d/%02d/%04d",
					  gps.date.day(),
					  gps.date.month(),
					  gps.date.year());
	}
	else
	{
		Serial.print(F("???"));
	}

	Serial.print(F(" "));
	if (gps.time.isValid())
	{
		char c = gps.time.isUpdated() ? '*' : ' ';
		Serial.write(c);
		Serial.printf("%02d:%02d:%02d:%d",
					  gps.time.hour(),
					  gps.time.minute(),
					  gps.time.second(),
					  gps.time.centisecond());
	}
	else
	{
		Serial.print(F("???"));
	}

	Serial.print(" ALT=");
	if (gps.altitude.isValid())
	{
		char c = gps.altitude.isUpdated() ? '*' : ' ';
		Serial.write(c);
		Serial.print(gps.altitude.meters());
	}
	else
		Serial.print(F("???"));

	// Numero di satelliti visibili
	Serial.print(" SAT=");
	if (gps.satellites.isValid())
	{
		char c = gps.satellites.isUpdated() ? '*' : ' ';
		Serial.write(c);
		Serial.print(gps.satellites.value());
	}
	else
		Serial.print(F("???"));

	// horizontal diminution of precision
	// 1      ideale
	// 1..,2  eccellente
	// 2...5  buono
	// 5..10  moderato
	// 10..20 scarso
	// 20..   pessimo
	Serial.print(" HDOP=");
	if (gps.hdop.isValid())
	{
		char c = gps.hdop.isUpdated() ? '*' : ' ';
		Serial.write(c);
		Serial.print(gps.hdop.hdop());
	}
	else
		Serial.print("???");

	Serial.print(" SPEED=");
	if (gps.speed.isValid())
	{
		char c = gps.speed.isUpdated() ? '*' : ' ';
		Serial.write(c);
		Serial.print(gps.speed.kmph());
	}
	else
		Serial.print("???");

	// Rotta in gradi
	Serial.print(" COURSE=");
	if (gps.course.isValid())
	{
		char c = gps.course.isUpdated() ? '*' : ' ';
		Serial.write(c);
		Serial.print(gps.course.deg());
	}
	else
		Serial.print("???");

	Serial.println();
}
