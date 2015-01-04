#include <Arduino.h>
#include "SPI.h"
#include "MFRC522.h"

#define SAD 10
#define RST 14

SPCRemulation SPCR;

MFRC522 nfc(SAD, RST);

static bool ok = false;

void setup() {
	Serial.begin(9600);	
	SPI.begin();
	nfc.PCD_Init();
	delay(1000 * 10);

	byte v = nfc.PCD_GetVersion();
	if (v == 0x90 || v == 0x91)
	{
		printf("MFRC522 detected version = %02X\n", v);
		ok = true;
	}
	else
		printf("MFRC522 not present\n");
}

void loop() 
{
	if (!ok) return;

	delay(500);
	printf("version = %02X\n", nfc.PCD_GetVersion());

	// Look for new cards
	if (!nfc.PICC_IsNewCardPresent())
		return;

	// Select one of the cards
	if (!nfc.PICC_ReadCardSerial())
		return;

	// Now a card is selected. The UID and SAK is in mfrc522.uid.

	// Dump UID
	printf("Card UID:");
	for (byte i = 0; i < nfc.uid.size; i++) {
		if (i > 0) printf(" ");
		printf("%02X", nfc.uid.uidByte[i]);
	}
	printf("\n");

	// Dump PICC type
	byte piccType = nfc.PICC_GetType(nfc.uid.sak);
	printf("PICC type: %s\n", nfc.PICC_GetTypeName(piccType));

	if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && 
		piccType != MFRC522::PICC_TYPE_MIFARE_1K && 
		piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
	{
		printf("This sample only works with MIFARE Classic cards.");
		return;
	}

	nfc.PICC_DumpToSerial(&nfc.uid);
	printf(">");
	char bb[20];
	scanf("%s", bb);
}
