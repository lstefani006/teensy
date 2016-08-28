#include "idDHTLib.h"
#include <t_5110.h>
#include <t_io.h>
#include "Graph.hpp"
#include "uprintf.hpp"

#include <EEPROM.h>
#include <Time.h>


#ifdef DALLAS
#include <OneWire.h>
#include "DallasTemperature.h"
#endif

constexpr int idDHTLibPin = 2;       //Digital pin for comunications
constexpr int idDHTLibIntNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dhtLib_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHTLib DHTLib(idDHTLibPin, idDHTLibIntNumber, dhtLib_wrapper);

// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dhtLib_wrapper() { DHTLib.dht11Callback(); }


constexpr int PIN_CE    = 10;
constexpr int PIN_SCLK  = 13;
constexpr int PIN_SDIN  = 11;
constexpr int PIN_RESET =  9;
constexpr int PIN_DC    = 14;
t::hwSPI<PIN_CE, PIN_SCLK, PIN_SDIN, /*-1*/SPI_MODE0> spi;
t::Lcd<typeof(spi), PIN_RESET, PIN_DC, false> lcd(spi);

#ifdef DALLAS
OneWire g_ow(6);
DallasTemperature g_dallas(&g_ow);
DeviceAddress g_addr;
#endif

static int freeRam () 
{
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == nullptr ? (int) &__heap_start : (int) __brkval); 
}

#ifdef DALLAS
enum class dallasError : uint8_t { ok, deviceNotPresent, timeout };
dallasError readDallasTemp(float &ret)
{
	bool b = g_dallas.requestTemperaturesByAddress(g_addr); // Send the command to get temperatures
	if(!b) return dallasError::deviceNotPresent;

	ret = g_dallas.getTempC(g_addr);
	if (ret == DEVICE_DISCONNECTED_C) return dallasError::deviceNotPresent;
	return dallasError::ok;
}
#endif

static void processSyncMessage() 
{
	const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
	if(Serial.find((char *)"T")) {
		unsigned long pctime = Serial.parseInt();
		if( pctime >= DEFAULT_TIME)
			setTime(pctime);
	}
}


void setup()
{
	Serial.begin(38400);
	Serial.println("Inizio");
	uprintf_cb = [](char ch) { Serial.print(ch); return true; };
	uprintf("Ciao");


	t::SetPrint(&lcd);
	spi.begin();

	lcd.begin();
	lcd.clear();
	lcd.gotoXY(0, 0);
	lcd.setContrast(35);

	lcd.clear();
	uprintf([](char ch) { lcd.print(ch); return true; }, "Ciao");
	lcd.update();
	delay(500);
	lcd.clear();

	lcd.print(F("FREE MEM="));
	lcd.println(freeRam());

	// Time
	setSyncProvider([]() -> time_t { Serial.write("7"); Serial.flush(); return 0; });
	setSyncInterval(60);

#ifdef DALLAS
	g_dallas.begin();
	delay(100);
	lcd.print(F("NUM=")); lcd.println(g_dallas.getDeviceCount());
	lcd.print(F("PARASTIC=")); lcd.println(g_dallas.isParasitePowerMode());
	if (g_dallas.getAddress(g_addr, 0) == false) lcd.println(F("getAddress ERRORE"));
	g_dallas.setResolution(g_addr, 12);
	lcd.print(F("RES="));
	lcd.println(g_dallas.getResolution(g_addr));

	delay(100);
	float t;
	readDallasTemp(t);
	lcd.print(F("T=")); lcd.println(t);

	lcd.update();
	delay(1000);
#endif

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);
}


enum class CodaType { ti, hh, dp, te };
struct CodaItem
{
	int16_t ti;
	int16_t hh;
	int16_t dp;
	int16_t te;
};


class Coda
{
public:
	Coda()
	{
		uint8_t magic;
		EEPROM.get(addr_magic, magic);
		if (magic != 0xa5)
		{
			magic = 0xa5;
			EEPROM.put(addr_magic, magic);

			PrimoVuoto(0);
			Len(0);

			CodaItem it;
			it.ti = 0;
			it.hh = 0;
			it.dp = 0;
			it.te = 0;

			for (int idx = 0; idx < _sz; ++idx)
				put(idx, it);
		}
	}

	void push(const CodaItem &a)
	{
		int8_t primoVuoto = PrimoVuoto();
		int8_t len = Len();

		put(primoVuoto, a);

		primoVuoto = (primoVuoto+1) % _sz;
		if (len < _sz) len += 1;

		PrimoVuoto(primoVuoto);
		Len(len);
	}

	int8_t Len()        const { int8_t len;        EEPROM.get(addr_len, len);                return len; }
	int8_t PrimoVuoto() const { int8_t primoVuoto; EEPROM.get(addr_primo_vuoto, primoVuoto); return primoVuoto; }

	void Len(int8_t len)       { EEPROM.put(addr_len, len);        }
	void PrimoVuoto(int8_t pv) { EEPROM.put(addr_primo_vuoto, pv); }

	int8_t sz() const { return _sz; }

	CodaItem operator [](int8_t i) const 
	{
		int idx;
		if (Len() == _sz)
			idx = (PrimoVuoto() + i) % _sz;
		else
			idx = i;

		CodaItem it;

		int a = addr_items + 4*sizeof(int16_t)*idx;
		EEPROM.get(a+0, it.ti);
		EEPROM.get(a+2, it.hh);
		EEPROM.get(a+4, it.dp);
		EEPROM.get(a+6, it.te);

		return it;
	}

private:
	constexpr static int addr_magic = 0;
	constexpr static int addr_primo_vuoto = 1;
	constexpr static int addr_len = 2;
	constexpr static int addr_items = 3;

	void put(int idx, const CodaItem &it)
	{
		int a = addr_items + 4*sizeof(int16_t)*idx;
		EEPROM.put(a+0, it.ti);
		EEPROM.put(a+2, it.hh);
		EEPROM.put(a+4, it.dp);
		EEPROM.put(a+6, it.te);
	}

private:
	constexpr static int8_t _sz = 24;
};

Coda g_coda;

static void PrintValue(const __FlashStringHelper *p, float f)
{
	lcd.print(p);
	int8_t x = lcd.getX();
	int8_t y = lcd.getY();
	lcd.gotoXY(x + 2, y);
	if (f < 10 && f > 0) lcd.print(' ');
	lcd.println(f, 1);
}



class TempSource : public GraphSource
{
	const CodaItem &_misura;
	int8_t _i;
	CodaType _ct;
	int8_t _d;
public:
	TempSource(const CodaItem &misura, CodaType ct, int d) : _misura(misura), _i(-1), _ct(ct), _d(d) {}

	bool Next() { _i += 1; return _i <= g_coda.Len(); }
	void Get(Point &p)
	{
		int16_t c=0;
		if (_i < g_coda.Len())
		{
			switch (_ct)
			{
			case CodaType::ti: c = g_coda[_i].ti; break;
			case CodaType::hh: c = g_coda[_i].hh; break;
			case CodaType::dp: c = g_coda[_i].dp; break;
			case CodaType::te: c = g_coda[_i].te; break;
			}
		}
		else
		{
			switch (_ct)
			{
			case CodaType::ti: c = _misura.ti; break;
			case CodaType::hh: c = _misura.hh; break;
			case CodaType::dp: c = _misura.dp; break;
			case CodaType::te: c = _misura.te; break;
			}
		}
		p.x = _i;
		p.y = c/100.0f;
	}
	void Reset() { _i = -1; }

	void DrawLine(const Line &r)
	{
		int x0 = int(r.a.x);
		int y0 = int(r.a.y);

		int x1 = int(r.b.x);
		int y1 = int(r.b.y);

		lcd.line(x0, y0, x1, y1, _d);
	}
};

void loop()
{
	if (Serial.available()) 
		processSyncMessage();

	const unsigned long tPeriod = 5;
	const unsigned long tStart = millis();

	lcd.clear();
	lcd.gotoXY(0, 0);


#ifdef DALLAS
	float te;
	dallasError de = readDallasTemp(te);
	switch (de)
	{
	case dallasError::deviceNotPresent:
		lcd.print(F("Dallas\n\rdevice fail"));
		break;
	case dallasError::timeout:
		lcd.print(F("Dallas\n\rtimeout"));
		break;
	case dallasError::ok:
		break;
	}
#endif

	int8_t dhtError = DHTLib.getStatus();
	switch (dhtError)
	{
	case IDDHTLIB_ERROR_CHECKSUM: 
		lcd.println(F("Error\n\r\tChecksum error")); 
		break;
	case IDDHTLIB_ERROR_TIMEOUT: 
		lcd.println(F("Error\n\r\tTime out error")); 
		break;
	case IDDHTLIB_ERROR_ACQUIRING: 
		lcd.println(F("Error\n\r\tAcquiring")); 
		break;
	case IDDHTLIB_ERROR_DELTA: 
		lcd.println(F("Error\n\r\tDelta time to small")); 
		break;
	case IDDHTLIB_ERROR_NOTSTARTED: 
		lcd.println(F("Error\n\r\tNot started")); 
		break;
	case IDDHTLIB_OK: 
		break;
	default: 
		lcd.println(F("Unknown error")); 
		break;
	}

	if (de == dallasError::ok && dhtError == IDDHTLIB_OK)
	{
		auto ti = DHTLib.getCelsius();
		auto hh = DHTLib.getHumidity();
		auto dp = DHTLib.getDewPoint();


		CodaItem cc;
		cc.ti = int16_t(ti*100);
		cc.hh = int16_t(hh*100);
		cc.dp = int16_t(dp*100);
		cc.te = int16_t(te*100);

		// ogni ora storicizzo ==> 24 ore di storia
		static unsigned long sec = 0;
		sec += tPeriod;
		if (sec >= 60*60)
		{
			sec = 0;
			g_coda.push(cc);
		}

		if (true)
		{
			PrintValue(F("CE"), te); 
			PrintValue(F("CI"), ti); 
			PrintValue(F("H "), hh); 
			PrintValue(F("DP"), dp); 

			if (true)
			{
				auto ff = [](char ch) { lcd.print(ch); return true; };
				switch (timeStatus())
				{
				case timeNotSet:
					lcd.setInverse(true);
					uprintf(ff, "????");
					lcd.setInverse(false);
					break;

				case timeNeedsSync:
					lcd.setInverse(true);
				case timeSet:
					uprintf(ff, "%02d:%02d\n", hour(), minute());
					lcd.setInverse(false);
					break;
				}

				int aa = lcd.getX();
				lcd.gotoXY(0, lcd.LCD_Y - 7);

				static bool ora = false;
				ora = !ora;
				if (ora)
				{
					switch (timeStatus())
					{
					case timeNotSet:
						lcd.setInverse(true);
						uprintf(ff, "????");
						lcd.setInverse(false);
						break;

					case timeNeedsSync:
						lcd.setInverse(true);
					case timeSet:
						{
							const char *wd="???";
							switch (weekday())
							{
							case 1: wd = "DOM"; break;
							case 2: wd = "LUN"; break;
							case 3: wd = "MAR"; break;
							case 4: wd = "MER"; break;
							case 5: wd = "GIO"; break;
							case 6: wd = "VEN"; break;
							case 7: wd = "SAB"; break;
							}
							uprintf(ff, "%s %02d/%02d/%d", wd, day(), month(), year());
							lcd.setInverse(false);
						}
						break;
					}
				}
				else
				{
					/***/if (dp <= 10) lcd.println(F("1 Molto secco"));
					else if (dp <= 12) lcd.println(F("2 Secco"));
					else if (dp <= 16) lcd.println(F("3 Confort"));
					else if (dp <= 18) lcd.println(F("4 Poco umido"));
					else if (dp <= 21) lcd.println(F("5 Umido"));
					else if (dp <= 24) lcd.println(F("6 Molto umido"));
					else               lcd.println(F("7 Afa"));
				}
			}
		}

		if (true)
		{
			// estremi inclusi

			static int8_t s_grafico = 0;

			Rect view;
			Rect screen;

			if (s_grafico == 0)
			{
				// 24 ore ....
				lcd.clear();   // a schermo intero!

				view.a.x = 0;
				view.b.x = 24;
				view.a.y = -5;
				view.b.y = +35;

				// dimensione schermo
				screen.a.x = 0;
				screen.a.y = 0;
				screen.b.x = lcd.LCD_X-1;
				screen.b.y = lcd.LCD_Y-1;


			}
			else
			{
				int8_t x0 = 5*(3+4)+2+1 - 2 - 2;
				int8_t x1 = lcd.LCD_X-1;
				int8_t y = 7*5+4;

				// 12 ore ....
				view.a.x = 12;
				view.b.x = 24;
				view.a.y = -5;
				view.b.y = +35;

				// dimensione schermo
				screen.a.x = x0;
				screen.a.y = 0;
				screen.b.x = x1;
				screen.b.y = y;

			}
			s_grafico = (s_grafico + 1) % 8;

			TempSource gte(cc, CodaType::te, 1);

			if (true)
			{
				float tMin = 100, tMax = -100;
				while(gte.Next())
				{
					Point r;
					gte.Get(r);
					if (r.y < tMin) tMin = r.y;
					if (r.y > tMax) tMax = r.y;
				}
				tMin -= 5;
				tMax += 5;
				if (tMax - tMin < 20)
				{
					auto c = (tMax+tMin)/2;
					tMax = c+10;
					tMin = c-10;
				}
				view.a.y = tMin;
				view.b.y = tMax;
			}
			gte.Reset();

			Graph gr;
			gr.SetViewScreen(&view, &screen);

			// questo voule +1 (estremi esclusi)
			lcd.box(screen.a.x, screen.a.y, screen.b.x + 1, screen.b.y + 1);

			// righello asse y ==> temperature
			for (int8_t tc = -10; tc <= 50; tc += 10)
			{
				Point t0;
				t0.x = (view.a.x+view.b.x)/2;
				t0.y = tc;

				if (gr.Clip(t0))
				{
					gr.Translate(t0);
					lcd.h_line(screen.a.x, screen.b.x, int(t0.y), tc == 0 ? 2 : 4); 
				}
			}

			// righello per le ore
			int8_t step = 4; // ogni 4 ore......
			for (int8_t i = 0; i <= g_coda.sz(); i += step)
			{
				Point t0;
				t0.x = i;
				t0.y = (view.a.y+view.b.y)/2;  // un punto del centro... cosi ci sta sicuramente dentro

				if (gr.Clip(t0))
				{
					gr.Translate(t0);

					int8_t dd = 1;
					if (i %  6 == 0) dd = 2;
					if (i % 12 == 0) dd = 4;
					lcd.v_line(int(t0.x), screen.b.y, screen.b.y-dd);
				}
			}

			// qui si plotta
			gr.Plot(gte);
		}
	}

	lcd.update();

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);

	auto tNow = millis();
	if (tStart + tPeriod * 1000 > tNow)
		delay(tPeriod * 1000 - (tNow - tStart));
}

