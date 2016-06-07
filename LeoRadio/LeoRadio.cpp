#include <Wire.h>
#include "RDA5807M.h"
#include "RDSParser.h"

#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include "XPT2046_Touchscreen.h"
#include "calibrate.h"

/////////////////////
// MOSI=11, MISO=12, SCK=13
#define CS_PIN  5
#define TIRQ_PIN  3
XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN);     // Param 2 - 255 - No interrupts


// For the Adafruit shield, these are the default.
#define TFT_RESET 8
#define TFT_DC  9
#define TFT_CS 10
//
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
#if VIDEO
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RESET);
MATRIX matrix;
#endif
RDA5807M radio;
RDSParser rds;

namespace W
{
	bool bb = 1;

	void clear() { if (!bb) return; Serial.print(F("\033[2J")); }
	void move_cur(int r, int c) 
	{
		if (!bb) return;
		Serial.print(F("\033["));
		Serial.print(r);
		Serial.print(F(";"));
		Serial.print(c);
		Serial.print(F("H"));
	}
	void move_cur_0_0() { if (!bb) return; Serial.print(F("\033[H")); }
	void clear_to_end_of_line() { if (!bb) return; Serial.print(F("\033[K")); }

	enum Color {
		Black = 0,
		Red = 1,
		Green = 2,
		Yellow = 3,
		Blue = 4,
		Magenta = 5,
		Cyan = 6,
		White = 7
	};

	void FG(Color c) { if (!bb) return; Serial.print(F("\033[")); Serial.print(30 + c); Serial.print(F("m")); }
	void BG(Color c) { if (!bb) return; Serial.print(F("\033[")); Serial.print(40 + c); Serial.print(F("m")); }

	enum Attr {
		Reset = 0,
		Bright = 1,
		Dim = 2,
		Underscore = 4,
		Blink = 5,
		Reverse = 7,
		Hidden = 8
	};
	void set_attr(Attr a) { if (!bb) return; Serial.print(F("\033[")); Serial.print(int(a)); Serial.print(F("m")); }

	void cursor(bool on)
	{
		if (on)
			Serial.print(F("\033[?25h"));
		else
			Serial.print(F("\033[?25l"));
	}
}
//	W::clear_to_end_of_line(); \

#define DRI(ri, a) \
	W::move_cur(r++, 0); \
	Serial.print(F(#a)); \
	Serial.print(F(" ")); \
	Serial.print(a(ri)); \
	Serial.print(F("       "));


void DisplayRadio()
{
	RDA5807M::RADIO_INFO ri;
	radio.getRadioInfo(ri);
}
void DisplayRadio(RDA5807M::RADIO_INFO &ri)
{
	W::cursor(false);
	W::move_cur(0, 0);

	uint16_t f = radio.frequency();
	char s[12];
	radio.formatFrequency(s, sizeof(s));
	Serial.print(F("freq = ")); Serial.print(s); Serial.print(F("      "));

	uint8_t r = 3;

	DRI(ri, ra_rds_ready);
	DRI(ri, ra_seek_tune_complete);
	DRI(ri, ra_seek_fail);
	DRI(ri, ra_rds_synchronization);
	DRI(ri, ra_block_e_found);
	DRI(ri, ra_stereo);

	DRI(ri, rb_rssi);
	DRI(ri, rb_fm_true);
	DRI(ri, rb_fm_ready);
	DRI(ri, rb_abcd_e);
	DRI(ri, rb_blera);
	DRI(ri, rb_blerb);
	W::cursor(true);
}

void print(uint32_t v, uint8_t nc)
{
	if (nc > 0)
	{
		for (uint8_t i = 0; i < nc; ++i)
		{
			uint32_t c = v >> (4 * (nc - 1 - i));
			Serial.print(c & 0xf, HEX);
		}
	}
}

void DisplayRawRDS(uint16_t pi, uint8_t gt, uint8_t pty, const uint16_t *b, const pi_stat_t *pi_stat, uint8_t pi_stat_sz)
{
	W::move_cur(19, 0);

	Serial.print(F("PI:")); print(pi, 4); 
	Serial.print(F(" GT:")); print(gt, 2); 
	if (false)
	{
		Serial.print(F(" PTY:")); print(pty, 2); 
		for (uint8_t i = 0; i < 4; ++i)
		{
			Serial.print(F(" B")); 
			Serial.print(i); 
			Serial.print(F(":")); 
			print(b[i], 4);
		}
	}
	if (true)
	{
		Serial.print(F(" pi stat "));
		for (uint8_t i = 0; i < pi_stat_sz; ++i)
		{
			if (pi_stat[i].pi == 0) 
			{
				W::clear_to_end_of_line();
			}
			else
			{
				Serial.print(pi_stat[i].pi, HEX);
				Serial.print(F("(N:")); 
				Serial.print(pi_stat[i].n);
				Serial.print(F("-W:")); 
				Serial.print(pi_stat[i].w);
				Serial.print(F(") ")); 
			}
		}
	}
	W::move_cur(20, 0);

	static uint8_t gg[20] = { 0, };

	bool found = false;
	for (uint8_t i = 0; i < 20; ++i)
	{
		if (gg[i] == gt) { found = true; break; }
	}
	if (!found)
		for (uint8_t i = 0; i < 20; ++i)
			if (gg[i] == 0)
			{
				gg[i] = gt;
				break;
			}

	for (uint8_t i = 0; i < 20; ++i)
	{
		if (gg[i] == 0) break;
		print(gg[i], 2); 
		Serial.print(F(" "));
	}
}

void DisplayServiceName(const char *name) { 
	W::move_cur(21, 0);
	Serial.print(F("SN  :")); 
	Serial.println(name); 
}
void DisplayText(const char *name) { 
	W::move_cur(22, 0);
	Serial.print(F("TXT :")); 
	Serial.println(name); 
}

void DisplayTime(uint32_t mjdc, uint8_t hh, uint8_t mm, int8_t off)
{
	static uint8_t z = 0;

	W::move_cur(23, 0);
	Serial.print(z++); if (z == 10) z = 0;
	Serial.print(F("MJDC:")); Serial.print(mjdc);
	Serial.print(F(" HH:"));  Serial.print(int(hh));
	Serial.print(F(" MM:"));  Serial.print(int(mm));
	Serial.print(F(" OFF:")); Serial.print(int(off));

	int h = int(hh) * 60 + int(mm) + 30 * off;

	Serial.print(F(" time:")); Serial.print(h/60);
	Serial.print(F(":"));      Serial.print(h%60);
	Serial.print(F("      ")); 

	static uint32_t s_mjdc = 0;
	if (s_mjdc != mjdc)
	{
		s_mjdc = mjdc;

		int32_t yp = (int32_t)((mjdc - 15078.2)/365.25);
		int32_t mp = (int32_t)( ( mjdc - 14956.1 - (int32_t)(yp * 365.25) ) / 30.6001 );
		int32_t day = mjdc - 14956 - (int32_t)( yp * 365.25 ) - (int32_t)( mp * 30.6001 );
		int32_t k = (mp == 14 || mp == 15) ? 1 : 0;
		int32_t year = 1900 + yp + k;
		int32_t month = mp - 1 - k * 12;

		Serial.print(year); Serial.print(F("-"));
		Serial.print(month);Serial.print(F("-"));
		Serial.print(day);Serial.print(F("-"));
	}
}


bool runSerialCommand(char cmd)
{
	if (cmd == '?') 
	{
		W::clear();
		Serial.println();
		Serial.println(F("? Help"));
		Serial.println(F("+ increase volume"));
		Serial.println(F("- decrease volume"));
		Serial.println(F(". scan up   : scan up to next sender"));
		Serial.println(F(", scan down ; scan down to next sender"));
		Serial.println(F("fnnnnn: direct frequency input"));
		Serial.println(F("s mono/stereo mode"));
		Serial.println(F("b bass boost"));
		Serial.println(F("u mute/unmute"));
		Serial.println(F("d dump registers"));
		delay(3000);
		W::clear();
	}
	else if (cmd == '+') { radio.volumeUp(); }
	else if (cmd == '-') { radio.volumeDown(); }
	else if (cmd == 'u') { radio.setMute(!radio.mute()); }
	else if (cmd == 's') { radio.setMono(!radio.mono()); }
	else if (cmd == 'f') { radio.setFrequency(3333); }
	else if (cmd == '.') { radio.seekUp(false); return true; }
	else if (cmd == ':') { radio.seekUp(true); return true; }
	else if (cmd == ',') { radio.seekDown(false); return true; }
	else if (cmd == ';') { radio.seekDown(true); return true; }
	else if (cmd == 'b') { radio.setBassBoost(! radio.bassBoost()); }
	else if (cmd == 'd') { W::clear(); radio.dumpRegisters(); delay(5000); }
	return false;
}

void radio_init()
{
	while (!radio.begin())
	{
		Serial.println(F("Cannot find RDA5807M"));
		delay(1000 * 3);
	}

	Serial.println(F("RDA5807M ready !!!"));

	radio.setMono(false);
	radio.setMute(false);
	radio.setVolume(1);
}

void setup() 
{
	//Serial.begin(19200);
	//Serial.begin(38400);
	Serial.begin(115200);
	Serial.println(F("Radio..."));
	delay(500);

	pinMode(13, OUTPUT);

	radio_init();

	rds.attachServicenNameCallback(DisplayServiceName);
	rds.attachTextCallback(DisplayText);
	rds.attachTimeCallback(DisplayTime);
	rds.attachDebugCallback(DisplayRawRDS);

	delay(1000);
	W::clear();
}

int freeRam () 
{
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == nullptr ? (int) &__heap_start : (int) __brkval); 
}

void loop() 
{
	if (radio.error())
	{
		delay(100);
		W::clear();
		W::move_cur(0,0);
		radio_init();
		delay(1000);
		W::clear();
	}

	static bool fseek = false;
	if (true)
	{
		char c = 0;
		while (Serial.available() > 0) 
		{
			char cc = Serial.read();
			if (cc >= 32 && cc < 127)
				c = cc;
		}

		if (c)
		{
			W::move_cur(23, 0);
			Serial.print(F("Cmd"));
			fseek = runSerialCommand(c);
		}
	}

	RDA5807M::RADIO_INFO ri;
	radio.getRadioInfo(ri);

	DisplayRadio(ri);

	if (fseek)
	{
		rds.init();

		if (rb_fm_true(ri))
		{
			fseek = false;
			W::clear();
		}
	}

	if (fseek == false && ra_rds_ready(ri))
	{
		static bool bb = false;
		digitalWrite(13, bb);
		bb = !bb;

		rds.processData(ri.blk, rb_blera(ri), rb_blerb(ri));
	}
}
