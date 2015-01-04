
#include <Arduino.h>

#include <utility/FatApiConstants.h>
#include <SdFat.h>

Sd2Card card;
SdVolume volume;
SdFile root;

#ifdef __arm__
#ifdef CORE_TEENSY
#endif // CORE_TEENSY
#endif

int main()
{
	Serial.begin(9600);
	for (int i = 0; i < 10; ++i)
	{
		delay(1000);
		printf("%d\n", i);
	}
	bool ok;

	int cs = 10;
	pinMode(cs, OUTPUT);             //Pin 10 must be set as an output for the SD communication to work.   

	ok = card.init(SPI_FULL_SPEED, cs);   //Initialize the SD card and configure the I/O pins.
	if (!ok) { printf("card init failed\n"); return 0; }
	printf("card size = %dMB\n", int(512L * card.cardSize() / 1024));

	ok = volume.init(&card);              //Initialize a volume on the SD card.
	if (!ok) { printf("volume init failed\n"); return 0; }

	ok = root.openRoot(&volume);    //Open the root directory in the volume.
	if (!ok) { printf("volume init failed\n"); return 0; }

	root.ls(&Serial, LS_R | LS_DATE | LS_SIZE);

	SdFile file;
	file.open(&root, "leo.txt", O_CREAT | O_WRITE);
	file.print("ciao\n");
	file.close();

	ok = file.open(&root, "CIAO_LEO.TXT", O_READ);
	if (!ok) { printf("open failed\n"); return 0; }
	do {
		char b[100];
		int sz = file.read(b, sizeof(b) - 1);
		if (sz < 0) { printf("read failed\n"); return 0; }

		b[sz] = 0;
		printf("%s", b);

		if (sz < int(sizeof(b)) - 1) break;
	}
	while (1);
	file.close();
}
