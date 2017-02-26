#include <Arduino.h>
#include <math.h>
#include <DallasTemperature.h>
#include "SPI.h"
#include <uprintf.hpp>
#include "t_ILI9341.hpp"
#include "Graph.hpp"


// For the Adafruit shield, these are the default.
constexpr int TFT_CS  = PB12;
constexpr int TFT_DC  = PB14;
constexpr int TFT_RST = PB13; 
t_ILI9341<TFT_CS, TFT_DC, TFT_RST> g_gr;

OneWire g_ow(PA0);
DallasTemperature g_dt(&g_ow);
DeviceAddress g_da[1];

constexpr int led = PC13;

void printAddress(DeviceAddress deviceAddress)
{
	for (auto i = 0; i < 8; i++)
		uprintf("%02x", deviceAddress[i]);
}


void setup()
{
	delay(100);
	pinMode(led, OUTPUT);

	//Serial.begin(38400);
	//uprintf_cb = [](char c) -> bool { Serial.print(c); return true; };
	if (true)
	{
		g_gr.begin();
		g_gr.clear(Color::BLACK);
		g_gr.setRotation(1);
		g_gr.setForeColor(Color::WHITE);
		g_gr.setFont(font_08x08);

		uprintf_cb = [](char c) -> bool { g_gr.drawChar(c); return true; };
	}

	g_dt.begin();
	uprintf("Found %d devices\n",g_dt.getDeviceCount());

	if (g_dt.getDeviceCount() == 0)
	{
		uprintf("No device found");
		return;
	}

	// report parasite power requirements
	uprintf("Parasite power is: "); 
	if (g_dt.isParasitePowerMode()) 
		uprintf("ON\n");
	else 
		uprintf("OFF\n");

	if (!g_dt.getAddress(g_da[0], 0)) 
		uprintf("Unable to find address for Device 0\n"); 
	else
	{
		// show the addresses we found on the bus
		uprintf("Device 0 Address: ");
		printAddress(g_da[0]);
		uprintf("\n");

		// set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
		g_dt.setResolution(g_da[0], 12);
		uprintf("Device 0 Resolution: %d\n", g_dt.getResolution(g_da[0])); 
	}

	delay(2 * 1000);
	g_gr.setFont(font_10x16);
	g_gr.clear(Color::WHITE);

	if (1)
	{
		g_gr.rect(2,2, g_gr.w()-3, g_gr.h()-3, Color::RED);
		g_gr.rect(1,1, g_gr.w()-2, g_gr.h()-2, Color::RED);
		g_gr.rect(0,0, g_gr.w()-1, g_gr.h()-1, Color::YELLOW);
	delay(5 * 1000);
	g_gr.clear(Color::BLACK);

	}
}


struct Mis { int sec; float temp; };
Mis g_t[24*10] = {0,0,};
int g_primoVuoto = 0;



class TempSource : public GraphSource
{
	int16 _i;
	Color _color;
public:
	TempSource(Color color) : _i(-1), _color(color) {}

	bool Next() override { _i += 1; return _i < g_primoVuoto; }
	void Get(Point &p) override
	{
		p.x = g_t[_i].sec;
		p.y = g_t[_i].temp;
	}
	void Reset() override { _i = -1; }

	void DrawLine(const Line &r) override
	{
		int x0 = int(r.a.x);
		int y0 = int(r.a.y);

		int x1 = int(r.b.x);
		int y1 = int(r.b.y);

		g_gr.line(x0, y0, x1, y1, _color);
	}
};


uint32_t g_sec = 0;

const uint32_t t_range = 24*60*60;
const uint32_t t_step  = 60*60;
const uint32_t t_step_lbl  = 60*60*6;
const uint32_t t_camp = 60 * 6; // uno ogni 6 minuti .... 10 per ora .... 24*10

/*
const uint32_t t_range = 60*60;
const uint32_t t_step  = 60*5;
const uint32_t t_step_lbl  = 60*15;
const int t_camp = 10;
*/

class Timer
{
public:
	Timer(uint32_t period, bool expiredOnFirst) : _period(period), _t(0), _expiredOnFirst(expiredOnFirst) {}
	bool expired()
	{
		if (_t == 0) _t = millis();

		uint32_t t = millis();
		if (_expiredOnFirst || t - _t >= _period)
		{
			_t = t;
			_expiredOnFirst = false;
			return true;
		}
		return false;
	}
private:
	uint32_t _period;
	uint32_t _t;
	bool _expiredOnFirst;
};

void loop()
{
	digitalWrite(led, LOW);
	delay(100);
	digitalWrite(led, HIGH);
	delay(900);


	bool ok = false;
	do
	{
		if (g_dt.getDeviceCount() == 0)
			break;

		g_dt.requestTemperatures();
		auto tempC = g_dt.getTempC(g_da[0]);

		if (tempC == DEVICE_DISCONNECTED_C)
			break;

		ok = true;


		static Timer tmDisegnaGrafico(1000 * 30, true);
		bool disegnaGrafico = tmDisegnaGrafico.expired();


		long sec_start;
		long sec_end;
		if (true)
		{
			uint32_t ms = millis();
			sec_end = ms / 1000;
			sec_start = sec_end - t_range;

			if (g_primoVuoto == 0) g_primoVuoto += 1;

			static Timer tmNuovoCampione(t_camp * 1000, true);
			if (tmNuovoCampione.expired() == false)
			{
				// se il tempo non e' scaduto... sovrascrivo l'ultimo valore
				g_t[g_primoVuoto-1].temp = tempC;
				g_t[g_primoVuoto-1].sec  = sec_end;
			}
			else
			{
				g_t[g_primoVuoto-1].temp = tempC;
				g_t[g_primoVuoto-1].sec  = sec_end;

				const int sz = int(sizeof(g_t)/sizeof(g_t[0]));
				if (g_primoVuoto >= sz)
				{
					for (int i = 1; i < g_primoVuoto; ++i)
						g_t[i-1] = g_t[i];
				}
				else
					g_primoVuoto += 1;
			}
		}

		if (disegnaGrafico)
			g_gr.clear(Color::BLACK);

		// le temperature le rinfresco sempre
		if (true)
		{
			g_gr.setCursor(0, 10);
			g_gr.setForeColor(Color::YELLOW);
			uprintf("T %.1fC \n", tempC);

			float M = -1000, m = +1000;
			for (int i = 0; i < g_primoVuoto; ++i)
			{
				if (g_t[i].temp > M) M = g_t[i].temp;
				if (g_t[i].temp < m) m = g_t[i].temp;
			}
			uprintf("\n");
			g_gr.setForeColor(Color::RED);
			if (M > -1000) uprintf("M %.1fC \n", M);
			g_gr.setForeColor(Color::GREEN);
			if (m < +1000) uprintf("m %.1fC \n", m);

			g_gr.setForeColor(Color::PURPLE);
			uprintf("nc %d  ", g_primoVuoto);
		}

		if (disegnaGrafico)
		{
			g_gr.setForeColor(Color::ORANGE);
			const Color graphForeColor = Color::CYAN;

			auto f = g_gr.setFont(font_08x08);

			Rect view;
			Rect screen;

			screen.a.x = 100;//g_gr.w()/3;
			screen.a.y = 0;
			screen.b.x = g_gr.w()-1;
			screen.b.y = g_gr.h()- g_gr.fontH() - 5;

			view.a.x = sec_start;
			view.b.x = sec_end;
			view.a.y = -5;
			view.b.y = +35;

			Graph gr;
			gr.SetViewScreen(&view, &screen);


			// righello asse y ==> temperature
			for (int tc = -10; tc <= 50; tc += 10)
			{
				Point t0;
				t0.x = (view.a.x+view.b.x)/2;
				t0.y = tc;

				if (gr.Clip(t0))
				{
					gr.Translate(t0);
					int y = int(t0.y);
					g_gr.line(screen.a.x, y, screen.b.x, y, graphForeColor);

					int xch = g_gr.fontW();
					g_gr.setCursor(screen.a.x - xch*3-2, y - g_gr.fontH() / 2);
					uprintf("%3d", tc);
				}
			}
			for (int tc = -10; tc <= 50; tc += 1)
			{
				Point t0;
				t0.x = (view.a.x+view.b.x)/2;
				t0.y = tc;

				if (gr.Clip(t0))
				{
					gr.Translate(t0);
					int y = int(t0.y);
					if (tc % 5 == 0)
						g_gr.line(screen.a.x, y, screen.a.x+6, y, graphForeColor);
					else
						g_gr.line(screen.a.x, y, screen.a.x+3, y, graphForeColor);
				}
			}

			// asse delle X --- 
			// Le coordinate sono in secondi
			// t_step e' in secondi la distanza tra una tacca e l'altra
			// t_range e' in secondi quanto copre l'asse delle x
			for (typeof(sec_start) t = sec_end; t >= sec_start; t -= t_step)
			{
				Point t0;
				t0.x = t;
				t0.y = (view.a.y+view.b.y)/2;

				if (gr.Clip(t0))
				{
					gr.Translate(t0);
					int x = int(t0.x);

					int8_t dd = 4;
					if ((sec_end - t) % t_step_lbl == 0)
						dd = 8;
					g_gr.line(x, screen.b.y, x, screen.b.y-dd, graphForeColor);


					int sec = sec_end - t;

					int hh = sec / 3600;
					sec -= hh * 3600;

					int mm = sec / 60;
					sec -= mm * 60;

					int ss = sec;

					if ((sec_end - t) % t_step_lbl == 0)
					{
						int w = 0;

						for (int i = 0; i < 2; ++i)
						{
							bool (*ptf)(char) = uprintf_cb;
							if (i == 0)
								ptf = nullptr;


							/***/if (hh >  0 && mm >  0 && ss >  0) w = uprintf(ptf, "%dh:%02dm:%02ds", hh, mm, ss);
							else if (hh >  0 && mm == 0 && ss == 0) w = uprintf(ptf, "%dh", hh);
							else if (hh == 0 && mm >  0 && ss == 0) w = uprintf(ptf, "%dm", mm);
							else if (hh == 0 && mm == 0 && ss >  0) w = uprintf(ptf, "%ds", ss);
							else if (hh >  0 && mm >  0 && ss == 0) w = uprintf(ptf, "%dh:%02dm", hh, mm);
							else if (hh == 0 && mm >  0 && ss >  0) w = uprintf(ptf, "%dm:%02ds", mm, ss);

							if (i == 0)
								g_gr.setCursor(x - g_gr.fontW() * w/2, screen.b.y + 3);
						}
					}
				}
			}

			g_gr.rect(screen.a.x, screen.a.y, screen.b.x, screen.b.y, graphForeColor);
			TempSource gte(Color::WHITE);

			gr.Plot(gte);
			g_gr.setFont(f);
		}
	}
	while (false);

	if (ok == false)
	{
		g_gr.clear(Color::BLACK);
		uprintf("Sonda temperatura NON connessa");
		delay(1000*3);
	}
}

