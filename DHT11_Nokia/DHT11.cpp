#include <idDHTLib.h>
#include <t_5110.h>
#include <t_io.h>
#include "Graph.hpp"
#include "uprintf.hpp"

int freeRam () {
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == nullptr ? (int) &__heap_start : (int) __brkval); 
}

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


#define PIN_CE    10
#define PIN_SCLK  13
#define PIN_SDIN  11
#define PIN_RESET  9
#define PIN_DC    14
t::hwSPI<PIN_CE, PIN_SCLK, PIN_SDIN, -1> spi;
t::Lcd<typeof(spi), PIN_RESET, PIN_DC, false> lcd(spi);


#ifdef DALLAS
OneWire g_ow(6);
DallasTemperature g_dallas(&g_ow);
DeviceAddress g_addr;
#endif

bool pf(char ch)
{
	Serial.print(ch);
	return true;
}

void setup()
{
	Serial.begin(38400);
	Serial.println("Inizio");

	uprintf(pf, "Ciao");

	t::SetPrint(&lcd);
	spi.begin();

	lcd.begin();
	lcd.clear();
	lcd.gotoXY(0, 0);
	lcd.setContrast(35);

	lcd.clear();
	lcd.print(F("FREE MEM = "));
	lcd.println(freeRam());

#ifdef DALLAS
	g_dallas.begin();
	delay(100);
	lcd.print("NUM="); lcd.println(g_dallas.getDeviceCount());
	lcd.print("PARASTIC="); lcd.println(g_dallas.isParasitePowerMode());
	if (g_dallas.getAddress(g_addr, 0) == false) lcd.println(F("getAddress ERRORE"));
	g_dallas.setResolution(g_addr, 12);
	lcd.print("RES=");
	lcd.println(g_dallas.getResolution(g_addr));
	lcd.update();
	delay(2000);
#endif

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);
}


long  g_tt = 0;
constexpr int8_t g_sz = 24;
float g_ci[g_sz];
float g_hh[g_sz];
float g_dp[g_sz];
float g_ce[g_sz];
int8_t g_top = 0; // g_top = 0 => il piu' recente

static void wd()
{
	static uint8_t g_wd = 0;

	char p=0;
	switch (g_wd)
	{
	case 0: p = '-'; break;
	case 1: p = '\\'; break;
	case 2: p = '|'; break;
	case 3: p = '/'; break;
	}
	lcd.print(p);
	g_wd = (g_wd + 1) % 4;
}

static void pf(const __FlashStringHelper *p, float f)
{
	lcd.print(p);
	int8_t x = lcd.getX();
	int8_t y = lcd.getY();
	lcd.gotoXY(x + 2, y);
	if (f < 10 && f > 0) lcd.print(' ');
	lcd.println(f, 1);
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


class TempSource : public GraphSource
{
	int8_t _i;
	const float &_tce;
	float *_t;
	int _d;
public:
	TempSource(const float &tce, float *t, int d) : _i(-1), _tce(tce), _t(t), _d(d) {}

	bool Next() { _i += 1; return _i <= g_top; }
	void Get(Point &p)
	{
		p.x = g_sz - _i;
		p.y = (_i == 0) ? _tce : _t[_i-1];
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
	int t_period = 5;
	g_tt += t_period;
	auto t = millis();

	lcd.clear();
	lcd.gotoXY(0, 0);


#ifdef DALLAS
	float tce;
	dallasError de = readDallasTemp(tce);
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
		auto tci = DHTLib.getCelsius();
		auto thh = DHTLib.getHumidity();
		auto mdp = DHTLib.getDewPoint();

		pf(F("CE"), tce); 
		pf(F("CI"), tci); 
		pf(F("H "), thh); 
		pf(F("DP"), mdp); 

		if (true)
		{
			int aa = lcd.getX();
			int bb = lcd.getY();
			lcd.gotoXY(aa, bb + 6);

			/***/if (mdp <= 10) lcd.println(F("1 Molto secco"));
			else if (mdp <= 12) lcd.println(F("2 Secco"));
			else if (mdp <= 16) lcd.println(F("3 Confort"));
			else if (mdp <= 18) lcd.println(F("4 Poco umido"));
			else if (mdp <= 21) lcd.println(F("5 Umido"));
			else if (mdp <= 24) lcd.println(F("6 Molto umido"));
			else                lcd.println(F("7 Afa"));

			lcd.print(F("Scala 1..7  "));
			wd();
		}

		// ogni ora storicizzo ==> 24 ore di storia
		if (g_tt % (1 * 60 * 60) == 0)
		//if (true)
		{
			for (int8_t i = min(g_top, g_sz-1); i >= 1; --i)
			{
				g_ci[i] = g_ci[i-1];
				g_hh[i] = g_hh[i-1];
				g_dp[i] = g_dp[i-1];
				g_ce[i] = g_ce[i-1];
			};
			g_ci[0] = tci;
			g_hh[0] = thh;
			g_dp[0] = mdp;
			g_ce[0] = tce;

			if (g_top < g_sz)
				g_top += 1;
		}

		if (true)
		{
			// estremi inclusi
			int8_t x0 = 5*(3+4)+2+1 - 2 - 2;
			int8_t x1 = lcd.LCD_X-1;
			int8_t y = 7*4+4-1;

			// questo voule +1 (estremi esclusi)
			lcd.box(x0, 0, x1+1, y+1);


			static Graph *gr = nullptr;
			if (!gr)
			{
				static Rect view;
				view.a.x = 0;
				view.b.x = g_sz;
				view.a.y = -5;
				view.b.y = +35;

				static Rect screen;
				screen.a.x = x0;
				screen.a.y = 0;
				screen.b.x = x1;
				screen.b.y = y;


				/*
				   float mint = 5;
				   float maxt = 35;

				   while (gse.Next())
				   {
				   Point pt;
				   gse.Get(pt);
				   for (;;) 
				   {
				   if (pt.y < maxt)
				   break;
				   maxt += 5;
				   }
				   for (;;) 
				   {
				   if (pt.y > mint)
				   break;
				   mint -= 5;
				   }
				   }
				   gse.Reset();

				   view.a.y = mint;
				   view.b.y = maxt;
				   */

				static Graph grs;
				gr = &grs;
				gr->SetViewScreen(&view, &screen);
			}

			TempSource gse(tce, g_ce, 1);
			TempSource gsi(tci, g_ci, 2);


			// righello asse y ==> temperature
			for (int8_t tc = -10; tc <= 50; tc += 10)
			{
				Point t;
				t.x = 0;
				t.y = tc;

				if (gr->Clip(t) == false)
					continue;

				gr->Translate(t);

				lcd.h_line(x0, x1, int(t.y), tc == 0? 4:8); 
			}

			// righello per le ore
			int8_t step = 3;
			for (int8_t i = 0; i < g_sz; i += step)
			{
				Point t;
				t.x = i;
				t.y = 0;

				gr->Translate(t);

				int8_t dd = (i % 6 == 0) ? 2 : 1;
				lcd.v_line(int(t.x), y, y-dd);
			}

			// qui si plotta
			gr->Plot(gse);
			gr->Plot(gsi);
		}
	}

	lcd.update();

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);
	auto tn = millis();
	if (t + t_period * 1000 > tn)
		delay(t_period * 1000 - (tn - t));
}

