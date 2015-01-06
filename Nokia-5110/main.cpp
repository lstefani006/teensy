#include "t_SPI.h"
#include "t_5110.h"
#include "OneWire.h"
#include "t_io.h"

//////////////////////////////////////////////////////////////////

#define PIN_CE    10
#define PIN_SDIN  11
#define PIN_SCLK  13
#define PIN_RESET  9
#define PIN_DC    14

#if 1
t::hwSPI<PIN_CE, PIN_SCLK, PIN_SDIN, -1> spi;
t::Lcd<typeof(spi), PIN_RESET, PIN_DC, true> lcd(spi);
#else
t::swSPI<PIN_CE, PIN_SCLK, PIN_SDIN, -1> spi;
t::Lcd<typeof(spi),  PIN_RESET, PIN_DC> lcd(spi);
#endif

OneWire ds(22);

int readTemp(bool &ok);
int scanAddr();

void setup()
{
	t::SetPrint(&lcd);

	spi.begin();
	lcd.begin();

	lcd.clear();
	lcd.gotoXY(0, 0);

	lcd.print("funziona");
	delay(2000);
	lcd.clear();
	lcd.gotoXY(0, 0);


	int nFound = scanAddr();
	if (nFound)
		lcd.print("Device OK");
	delay(1000);

	analogReadRes(12);
	analogReadAveraging(10);
}

char ch = '*';

void loop()
{
	lcd.gotoXY(0,0);
	bool ok;
	int t = readTemp(ok);
	if (ok)
	{
		lcd.clear();
		lcd.printf("%c %d.%02dC\n", ch, t /100, t%100);
		ch = ch == '*' ? '+' : '*';

		lcd.printf("dt=%d", t::G_t);
	}

	int v = analogRead(A9);
	lcd.printf("\na=%d", v);

	lcd.line(0,lcd.LCD_Y-2, v * 84 / 4000 ,lcd.LCD_Y-2);
	lcd.line(0,lcd.LCD_Y-1, v * 84 / 4000 ,lcd.LCD_Y-1);
	lcd.line(0,lcd.LCD_Y-3, v * 84 / 4000 ,lcd.LCD_Y-3);
	lcd.line(0,lcd.LCD_Y-4, v * 84 / 4000 ,lcd.LCD_Y-4);
	lcd.line(0,lcd.LCD_Y-5, v * 84 / 4000 ,lcd.LCD_Y-5);

	static int nn = 0;
	analogWrite(3, nn);
	if (nn == 255) nn = 0;
	else {
		nn += 40;
		if (nn > 255) nn = 255;
	}
}

byte G_addr[8];

int scanAddr()
{
	byte addr[8];
	int nFound = 0;

	for (;;) 
	{
		if (!ds.search(addr)) 
		{
			ds.reset_search();
			return nFound;
		}

		if (OneWire::crc8(addr, 7) != addr[7]) 
		{
			lcd.print("CRC is not valid!\n");
			return nFound;
		}

		switch (addr[0]) 
		{
		case 0x10:
			lcd.print("Chip = DS18S20\n");  // or old DS1820
			break;
		case 0x28:
			lcd.print("Chip = DS18B20\n");
			break;
		case 0x22:
			lcd.print("Chip = DS1822\n");
			break;
		default:
			lcd.print("Device is not a DS18x20 family device.\n");
			return nFound;
		} 
		memcpy(G_addr, addr, sizeof(G_addr));
		nFound += 1;
	}
	return nFound;
}

int readTemp(bool &ok) 
{
	ok = false;

	ds.reset();
	ds.select(G_addr);
	ds.write(0x44, 1);        // start conversion, with parasite power on at the end

	delay(1000);     // maybe 750ms is enough, maybe not

	byte present = ds.reset();
	if (!present) {
		lcd.print("device not responding\n");
		return 0;
	}
	ds.select(G_addr);    
	ds.write(0xBE);         // Read Scratchpad

	byte data[12];
	for (int i = 0; i < 9; i++)           // we need 9 bytes
		data[i] = ds.read();

	if (data[8] != OneWire::crc8(data, 8))
	{
		lcd.print("CRC is not valid!\n");
		return 0;
	}

	// Convert the data to actual temperature
	// because the result is a 16 bit signed integer, it should
	// be stored to an "int16_t" type, which is always 16 bits
	// even when compiled on a 32 bit processor.
	int16_t raw = (data[1] << 8) | data[0];
	if (G_addr[0] == 0x10) {
		raw = raw << 3; // 9 bit resolution default
		if (data[7] == 0x10) {
			// "count remain" gives full 12 bit resolution
			raw = (raw & 0xFFF0) + 12 - data[6];
		}
	} else {
		byte cfg = (data[4] & 0x60);
		// at lower res, the low bits are undefined, so let's zero them
		if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
		else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
		else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
		//// default is 12 bit resolution, 750 ms conversion time
	}
	int celsius = raw * 100 / 16;
	ok = true;
	return celsius;
}
