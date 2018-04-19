#include "idDHTLib.h"
#include <t_5110.h>
#include "Graph.hpp"
#include <uprintf.hpp>

#include <EEPROM.h>
#include <Time.h>


#ifdef DALLAS
#include <OneWire.h>
#include "DallasTemperature.h"
#endif
#include "RTC_DS1302.h"

///////////////////////////////////
constexpr int8_t DS1302_SCLK_PIN = 3;
constexpr int8_t DS1302_IO_PIN = 4;
constexpr int8_t DS1302_CE_PIN = 5;
DS1302 rtc(DS1302_SCLK_PIN, DS1302_IO_PIN, DS1302_CE_PIN);
//////////////////////////////////



constexpr int idDHTLibPin = 2;       //Digital pin for comunications
constexpr int idDHTLibIntNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dhtLib_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHTLib DHTLib(idDHTLibPin, idDHTLibIntNumber, dhtLib_wrapper);

// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dhtLib_wrapper() { DHTLib.dht11Callback(); }


constexpr int PIN_LED = 6;
constexpr int PIN_DALLAS = 7;

constexpr int PIN_CE    = 10;
constexpr int PIN_SCLK  = 13;
constexpr int PIN_SDIN  = 11;
constexpr int PIN_RESET =  9;
constexpr int PIN_DC    = 14;
t::hwSPI<PIN_CE, PIN_SCLK, PIN_SDIN, /*-1*/SPI_MODE0> spi;
t::Lcd<typeof(spi), PIN_RESET, PIN_DC, false> lcd(spi);

#ifdef DALLAS
OneWire g_ow(PIN_DALLAS);
DallasTemperature g_dallas(&g_ow);
DeviceAddress g_addr;
#endif

#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)
static int freeRam () { return 0; }
#else
static int freeRam () 
{
	extern int __heap_start, *__brkval; 
	int v; 
	return int((long) &v - (__brkval == nullptr ? (long) &__heap_start : (long) __brkval)); 
}
#endif

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
	// se c'è la seriale attaccata e il PC risponde.... felici di aggiornare sia il Time che l'rtc
	if (Serial.find(const_cast<char*>("T"))) 
	{
		unsigned long pctime = Serial.parseInt();
		const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
		if( pctime >= DEFAULT_TIME)
		{
			setTime(pctime);
			rtc.SetTime(pctime);
		}
	}
	else
	{
		// aggiorno il Time con l'rtc
		setTime(rtc);
	}
}

static time_t syncProvider()
{
	rtc.GetTime(); 

	upf_t cb;
	cb.pf = [](char ch, void *) { Serial.write(ch); return true; };
	cb.ag = nullptr;

	// se poi c'è il PC attaccato.... aggiorno....
	uprintf(cb, F("7"));

	// se per caso ho una data valida.....
	if (rtc.Y() >= 2016 && rtc.Y() < 2050)
		return rtc; 
	else
		return 0;
}

void setup()
{

	Serial.begin(38400);
	rtc.begin();
	spi.begin();
	lcd.begin();

	lcd.clear();
	lcd.gotoXY(0, 0);
	lcd.setContrast(44);

	pinMode(PIN_LED, OUTPUT);
	analogWrite(PIN_LED, 0);  // massima luce.. compatibilmente con la resistenza di 100ohm

	uprintf_cb.pf = [] (char ch, void *) { lcd.write(ch); return true; };
	uprintf_cb.ag = nullptr;

	rtc.GetTime();
	uprintf(F("%04d/%02d/%02d\n"), rtc.Y(), rtc.M(), rtc.D());
	uprintf(F("%02d:%02d:%02d\n"), rtc.h24(), rtc.m(), rtc.s());
	uprintf(F("FREE MEM=%d\n"), freeRam());
	lcd.update();
	delay(500);

	// Time
	setSyncProvider(syncProvider);
	setSyncInterval(60); // ogni 60 secondi leggo dal rtc/PC


#ifdef DALLAS
	g_dallas.begin();
	delay(100);
	lcd.clear();
	uprintf(F("NUM=%d\n"), g_dallas.getDeviceCount());
	uprintf(F("PARASTIC=%d\n"), g_dallas.isParasitePowerMode());
	if (g_dallas.getAddress(g_addr, 0) == false) uprintf(F("getAddress ERRORE"));
	g_dallas.setResolution(g_addr, 12);
	uprintf(F("RES=%d\n"), g_dallas.getResolution(g_addr));

	delay(500);
	float t;
	readDallasTemp(t);
	uprintf(F("T=%f\n"), t);

	lcd.update();
	delay(1000);
#endif

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);
}


enum class CodaType { ti, hh, dp, te };
struct CodaItem
{
	CodaItem() : ti(0), hh(0), dp(0), te(0), _tick(0) {}
	int16_t ti;
	int16_t hh;
	int16_t dp;
	int16_t te;

	static int8_t size()
	{
		return 
			sizeof(CodaItem::_tick) +
			sizeof(CodaItem::ti) +
			sizeof(CodaItem::hh) +
			sizeof(CodaItem::dp) +
			sizeof(CodaItem::te);
	}

	const time_t & tick() const { return _tick; }
	time_t & rtick() { return _tick; }
	void tick(time_t t) { _tick = t; }

	time_t gtick() const { return _tick - DEFAULT_TIME; }
	static time_t gtick(time_t t) { return t - DEFAULT_TIME; }
private:
	static const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
	time_t  _tick;
};


class Coda
{
public:
	Coda()
	{
		uint8_t magic;
		EEPROM.get(addr_magic, magic);
		if (magic != 0xb7)
		{
			magic = 0xb7;
			EEPROM.put(addr_magic, magic);

			PrimoVuoto(0);
			Len(0);

			CodaItem it;
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

		int a = addr_items + CodaItem::size()*idx;
		EEPROM.get(a+0, it.ti);
		EEPROM.get(a+2, it.hh);
		EEPROM.get(a+4, it.dp);
		EEPROM.get(a+6, it.te);
		EEPROM.get(a+8, it.rtick());

		return it;
	}

private:
	constexpr static int addr_magic = 0;
	constexpr static int addr_primo_vuoto = 1;
	constexpr static int addr_len = 2;
	constexpr static int addr_items = 3;

	void put(int idx, const CodaItem &it)
	{
		int a = addr_items + CodaItem::size()*idx;
		EEPROM.put(a+0, it.ti);
		EEPROM.put(a+2, it.hh);
		EEPROM.put(a+4, it.dp);
		EEPROM.put(a+6, it.te);
		EEPROM.put(a+8, it.tick());
	}

private:
	constexpr static int8_t _sz = 24;
};

Coda g_coda;

static void PrintValue(const __FlashStringHelper *p, float f)
{
	uprintf(p);
	int8_t x = lcd.getX();
	int8_t y = lcd.getY();
	lcd.gotoXY(x + 2, y);

	uprintf(F("%4.1f\n"), f);
}

class TempSource : public GraphSource
{
	const CodaItem &_misura;
	int8_t _i;
	CodaType _ct;
	int8_t _d;
public:
	TempSource(const CodaItem &misura, CodaType ct, int d) : _misura(misura), _i(-1), _ct(ct), _d(d) {}

	bool Next() override { _i += 1; return _i <= g_coda.Len(); }
	void Get(Point &p) override
	{
		int16_t c=0;
		float tt;
		if (_i < g_coda.Len())
		{
			switch (_ct)
			{
			case CodaType::ti: c = g_coda[_i].ti; break;
			case CodaType::hh: c = g_coda[_i].hh; break;
			case CodaType::dp: c = g_coda[_i].dp; break;
			case CodaType::te: c = g_coda[_i].te; break;
			}
			tt = g_coda[_i].gtick();
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
			tt = _misura.gtick();
		}
		p.x = tt;
		p.y = c/100.0f;
	}
	void Reset() override { _i = -1; }

	void DrawLine(const Line &r) override
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
	if (false)
	{
		static int cc = 35;
		lcd.setContrast(cc);
		cc+=1;
		lcd.clear();
		uprintf("%d   ", cc);
		lcd.update();
		delay(1000);
		return;
	}
	if (true)
	{
		int v;
		int8_t h = hour();
		if (h >= 23 || h < 6)
			v = 150;
		else
			v = 0;
		analogWrite(PIN_LED, v);
	}


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
		uprintf(F("Dallas\ndevice fail"));
		break;
	case dallasError::timeout:
		uprintf(F("Dallas\ntimeout"));
		break;
	case dallasError::ok:
		break;
	}
#endif

	int8_t dhtError = DHTLib.getStatus();
	switch (dhtError)
	{
	case IDDHTLIB_ERROR_CHECKSUM: 
		uprintf(F("Error\nChecksum error")); 
		break;
	case IDDHTLIB_ERROR_TIMEOUT: 
		uprintf(F("Error\nTime out error")); 
		break;
	case IDDHTLIB_ERROR_ACQUIRING: 
		uprintf(F("Error\nAcquiring")); 
		break;
	case IDDHTLIB_ERROR_DELTA: 
		uprintf(F("Error\nDelta time to small")); 
		break;
	case IDDHTLIB_ERROR_NOTSTARTED: 
		uprintf(F("Error\nNot started")); 
		break;
	case IDDHTLIB_OK: 
		break;
	default: 
		uprintf(F("Unknown error")); 
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
		cc.tick(now());

		// ogni ora storicizzo ==> 24 ore di storia
		static time_t last = 0;
		if (last == 0)
		{
			tmElements_t xtm;
			breakTime(cc.tick(), xtm);
			xtm.Second = 0;
			xtm.Minute = 0;
			last = makeTime(xtm);
		}
		if (cc.tick() - last >= 60L*60L)
		{
			last = cc.tick();
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
				switch (timeStatus())
				{
				case timeNotSet:
					lcd.setInverse(true);
					uprintf(F("????"));
					lcd.setInverse(false);
					break;

				case timeNeedsSync:
					lcd.setInverse(true);
				case timeSet:
					uprintf(F("%02d:%02d\n"), hour(), minute());
					lcd.setInverse(false);
					break;
				}

				lcd.gotoXY(0, lcd.LCD_Y - 7);

				static bool ora = false;
				ora = !ora;
				if (ora)
				{
					switch (timeStatus())
					{
					case timeNotSet:
						lcd.setInverse(true);
						uprintf(F("????"));
						lcd.setInverse(false);
						break;

					case timeNeedsSync:
						lcd.setInverse(true);
					case timeSet:
						{
							const __FlashStringHelper *wd;
							switch (weekday())
							{
							case 1: wd = F("DOM"); break;
							case 2: wd = F("LUN"); break;
							case 3: wd = F("MAR"); break;
							case 4: wd = F("MER"); break;
							case 5: wd = F("GIO"); break;
							case 6: wd = F("VEN"); break;
							case 7: wd = F("SAB"); break;
							default: wd = F("???"); break;
							}
							uprintf(F("%S %02d/%02d/%d"), wd, day(), month(), year());
							lcd.setInverse(false);
						}
						break;
					}
				}
				else
				{
					const __FlashStringHelper *wd;
					/***/if (dp <= 10) wd = F("1 Molto secco");
					else if (dp <= 12) wd = F("2 Secco");
					else if (dp <= 16) wd = F("3 Confort");
					else if (dp <= 18) wd = F("4 Poco umido");
					else if (dp <= 21) wd = F("5 Umido");
					else if (dp <= 24) wd = F("6 Molto umido");
					else               wd = F("7 Afa");
					uprintf(F("%S\n"), wd);
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

				//view.a.x = 0;
				//view.b.x = 24;
				view.a.x = cc.gtick() - 24L*60*60;
				view.b.x = cc.gtick();

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
				//view.a.x = 12;
				//view.b.x = 24;
				view.a.x = cc.gtick() - 12L*60*60;
				view.b.x = cc.gtick();
				view.a.y = -5;
				view.b.y = +35;

				// dimensione schermo
				screen.a.x = x0;
				screen.a.y = 0;
				screen.b.x = x1;
				screen.b.y = y;

			}
			s_grafico = (s_grafico + 1) % 5;

			TempSource gte(cc, CodaType::te, 1);

			if (true)
			{
				float tMin = 100; 
				float tMax = -100;
				while(gte.Next())
				{
					Point r;
					gte.Get(r);
					if (r.y < tMin) tMin = r.y;
					if (r.y > tMax) tMax = r.y;
				}

				auto tCentre = (tMax + tMin) / 2.f;
				auto tDelta  = (tMax - tMin) / 2.f * 1.5f;

				tMin = tCentre - tDelta;
				tMax = tCentre + tDelta;

				/*
				lcd.clear();
				uprintf("%f\n%f\n", tMin, tMax);
				lcd.update();
				delay(1000);
				*/

				view.a.y = tMin;
				view.b.y = tMax;
			}
			gte.Reset();

			Graph gr;
			gr.SetViewScreen(&view, &screen);


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
			for (int8_t tc = -10; tc <= 50; tc += 1)
			{
				Point t0;
				t0.x = (view.a.x+view.b.x)/2;
				t0.y = tc;

				if (gr.Clip(t0))
				{
					gr.Translate(t0);
					if (tc % 5 == 0)
						lcd.h_line(screen.a.x, screen.a.x+3, int(t0.y));
					else
						lcd.h_line(screen.a.x, screen.a.x+1, int(t0.y));
				}
			}

			// righello per le ore
			time_t xti;
			if (true)
			{
				tmElements_t xtm;
				breakTime(cc.tick(), xtm);
				xtm.Second = 0;
				xtm.Minute = 0;
				xti = makeTime(xtm) -26L*60*60;; // due ora in meno ... tanto poi si clippa
			}

			for (time_t xt = xti; xt <= cc.tick(); xt += 60*60)
			{
				int8_t xh = hour(xt);
				if (xh & 1) continue; // faccio vedere le ore pari

				Point t0;
				t0.x = CodaItem::gtick(xt);
				t0.y = (view.a.y+view.b.y)/2;  // un punto del centro... cosi ci sta sicuramente dentro

				if (gr.Clip(t0))
				{
					gr.Translate(t0);

					int8_t dd = 1;
					if (xh == 6 || xh == 18) dd = 2;
					if (xh == 0 || xh == 12) dd = 4;
					lcd.v_line(int(t0.x), screen.b.y, screen.b.y-dd);
				}
			}

			// qui si plotta
			gr.Plot(gte);
	
			// questo voule +1 (estremi esclusi)
			lcd.box(screen.a.x, screen.a.y, screen.b.x + 1, screen.b.y + 1);
		}
	}

	lcd.update();

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);

	auto tNow = millis();
	if (tStart + tPeriod * 1000 > tNow)
		delay(tPeriod * 1000 - (tNow - tStart));
}
