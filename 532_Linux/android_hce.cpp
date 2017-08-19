#include <PN532_HSU.h>
#include <PN532.h>

PN532_HSU pn532hsu(Serial1);
PN532 nfc(pn532hsu);

void printResponse(uint8_t *response, uint8_t responseLength);

void setup()
{    
	Serial.begin(115200);
	Serial.println("-------Peer to Peer HCE--------");

	nfc.begin();

	uint32_t versiondata = nfc.getFirmwareVersion();
	if (! versiondata) {
		Serial.print("Didn't find PN53x board");
		while (1); // halt
	}

	// Got ok data, print it out!
	Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
	Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
	Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

	// Set the max number of retry attempts to read from a card
	// This prevents us from waiting forever for a card, which is
	// the default behaviour of the PN532.
	//nfc.setPassiveActivationRetries(0xFF);

	// configure board to read RFID tags
	nfc.SAMConfig();
}

void loop()
{
	uint8_t responseLength = 32;
	Serial.println("Waiting for an ISO14443A card");

	// set shield to inListPassiveTarget
	auto success = nfc.inListPassiveTarget();
	if (!success)
	{
		delay(1000);
		return;
	}

	Serial.println("Found something!");
	uint8_t selectApdu[] = { 
		0x00, /* CLA */
		0xA4, /* INS */
		0x04, /* P1  */
		0x00, /* P2  */
		0x07, /* Length of AID  */
		0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* AID defined on Android App */
		0x00  /* Le */ };

	uint8_t response[32];  
	success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);
	if (!success) {
		Serial.println("Failed sending SELECT AID"); 
		delay(1000);
		return;
	}
	printf("responseLength: %d\n",responseLength);
	nfc.PrintHexChar(response, responseLength);

	for (;;) 
	{
		uint8_t apdu[] = "Hello from Arduino";
		uint8_t back[32];
		uint8_t length = sizeof(back); 

		success = nfc.inDataExchange(apdu, sizeof(apdu), back, &length);
		if (!success) {
			Serial.println("Broken connection?"); 
			break;
		}

		printf("responseLength: %d\n",length);
		printResponse(back, length);
		delay(1000 * 10);
	}
}

void printResponse(uint8_t *response, uint8_t responseLength) 
{
	printf("Resp: " );
	for (int i = 0; i < responseLength; ++i) {
		if (response[i] >= 32 && response[i] < 128)
			printf("'%c' (%02x) ", response[i], response[i]);
		else
			printf(" ? (%02X) ", response[i]);
	}
	printf("\n");
}
