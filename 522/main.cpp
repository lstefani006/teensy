/*
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 * Released into the public domain.
 *
 * Sample program showing how to read data from a PICC using a MFRC522 reader on the Arduino SPI interface.
 *----------------------------------------------------------------------------- empty_skull 
 * Aggiunti pin per arduino Mega
 * add pin configuration for arduino mega
 * http://mac86project.altervista.org/
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <t_io.h>
#include <MFRC522.h>

#include "crypto/des.h"
#include "crypto/des3.h"
#include "crypto/aes.h"
#include "crypto/rsa.h"

#define SS_PIN 10
#define RST_PIN 9
MFRC522 pdc(SS_PIN, RST_PIN);	// Create MFRC522 instance.



void kk()
{
	if (true)
	{
		DesContext dc;
		desInit(&dc, (const uint8_t *)"ww", 2);

		const uint8_t *in = nullptr;
		uint8_t *out = nullptr;
		desEncryptBlock(&dc, in, out);
		desDecryptBlock(&dc, in, out);

	}

	if (true)
	{
		Des3Context dc3;
		des3Init(&dc3, (const uint8_t *)"ww", 2);

		const uint8_t *in = nullptr;
		uint8_t *out = nullptr;
		des3EncryptBlock(&dc3, in, out);
		des3DecryptBlock(&dc3, in, out);
	}
	if (true)
	{
		AesContext dc3;
		aesInit(&dc3, (const uint8_t *)"ww", 2);

		const uint8_t *in = nullptr;
		uint8_t *out = nullptr;
		aesEncryptBlock(&dc3, in, out);
		aesDecryptBlock(&dc3, in, out);
	}
	if (true)
	{
	}
}

void setup() 
{
	delay(1000*2);
	Serial.begin(38400);	// Initialize serial communications with the PC
	while (!Serial);
	Serial.println("Start");
	t::SetPrint(&Serial);

	SPI.begin();		// Init SPI bus
	pdc.PCD_Init();	// Init MFRC522 card

	int v = pdc.PCD_GetVersion();
	Serial.print("Version = ");
	Serial.println(v, HEX);
	if (!(v == 0x91 || v == 0x92))
	{
		Serial.println("522 not found");
		delay(1000*10);
		while(1);
	}

	kk();


	Serial.println("Scan PICC to see UID and type...");
}

void dumpUL()
{
	uint8_t LB1 = 0, LB2 = 0;
	for (int page = 0; page < 16; page++)
	{
		byte bb[20];
		byte bsz = sizeof(bb);
		if (pdc.MIFARE_Read(page, bb, &bsz) == MFRC522::STATUS_OK)
		{
			if (bsz == 18)
			{
				Serial.print(page);
				Serial.print(" ");
				for (int i = 0; i < 4; ++i)
					printf(" %02x", bb[i]);
				printf("\n");
			}
			else
				printf("Ritorna %d\n", bsz);
		}
		else
			printf("Errore\n");

		if (page == 3) {
			LB1 = bb[2];
			LB2 = bb[3];
		}
	}

	if (true)
	{
		printf("LOCK BITS\n");
		printf("Lock Bit - se settato la pagina e' a sola lettura\n");
		uint8_t m = 0b1000'0000;
		for (int i = 7; i >= 3; i--)
		{
			if (i == 3)
				printf("L%01x OTP", i);
			else
				printf("L%01x    ", i);
			if (LB1 & m) printf(" LOCKED"); else printf(" unlocked");
			printf("\n");
			m>>=1;
		}
		m = 0b1000'0000;
		for (int i = 7; i >= 0; i--)
		{
			printf("L%01x    ", i+8);
			if (LB2 & m) printf(" LOCKED"); else printf(" unlocked");
			printf("\n");
			m>>=1;
		}
		printf("BLOCK BITS\n");
		printf("Block Bits - se settato blocca la modifica ai LOCK BITS\n");
		
		printf("\nLB for 0xf to 0xa pages %s", ((LB1 & 0b100) ? "FROZEN" : "free"));
		printf("\nLB for 0x9 to 0x4 pages %s", ((LB1 & 0b010) ? "FROZEN" : "free"));
		printf("\nLB for 0x3 (OTP)        %s", ((LB1 & 0b001) ? "FROZEN" : "free"));

		printf("\nOTP\n");
		printf("Sono bit settati a ZERO in fabbrica\n");
		printf("se scritti a UNO rimangono a UNO");
		printf("se si fa write con un ZERO su di un UNO rimane uno\n");
	}
}


void loop() {
	delay(100);

	// Look for new cards
	if (! pdc.PICC_IsNewCardPresent())
		return;

	// Select one of the cards
	if (! pdc.PICC_ReadCardSerial())
		return;

	// Dump debug info about the card. PICC_HaltA() is automatically called.
	printf("\n\n");
	pdc.PICC_DumpToSerial(&(pdc.uid));

	if (pdc.PICC_GetType(pdc.uid.sak) == MFRC522::PICC_TYPE_MIFARE_UL)
	{
		Serial.println("Ci provo");
		dumpUL();


		byte bb[18];
		byte bbsz = sizeof(bb);

		byte addr = 0x6;
		auto st = pdc.MIFARE_Read(addr, bb, &bbsz);
		if (st == MFRC522::STATUS_OK)
		{
			printf("Leggo pagina=%02x ==> %02x\n", addr, bb[0]);

			bb[0] += 1;
			st = pdc.MIFARE_Ultralight_Write(addr, bb, 4);
			if (st == MFRC522::STATUS_OK)
			{
				printf("OK\n");
				delay(1000*3);
			}
			else
				printf("error %s\n", pdc.GetStatusCodeName(st));
		}
		else
			printf("error %s\n", pdc.GetStatusCodeName(st));
	}
}


