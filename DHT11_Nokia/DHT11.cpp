/*
   Board	          int.0	  int.1	  int.2	  int.3	  int.4	  int.5
   Uno, Ethernet	  2	  3
   Mega2560	          2	  3	  21	  20	  19	  18
   Leonardo	          3	  2	  0	       1
   Due	          (any pin, more info http://arduino.cc/en/Reference/AttachInterrupt)

   This example, as difference to the other, make use of the new method acquireAndWait()
*/
#include <idDHTLib.h>
#include <t_5110.h>
#include <t_io.h>

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

void setup()
{
	t::SetPrint(&lcd);
	spi.begin();

	lcd.begin();
	lcd.clear();
	lcd.gotoXY(0, 0);
	lcd.setContrast(35);

	lcd.clear();
	lcd.println(F("... test..."));
#ifdef DALLAS
	g_dallas.begin();
	delay(100);
	lcd.print("NUM="); lcd.println(g_dallas.getDeviceCount());
	lcd.print("PARASTIC="); lcd.println(g_dallas.isParasitePowerMode());
	if (g_dallas.getAddress(g_addr, 0) == false) lcd.println(F("getADDR ERRORE"));
	g_dallas.setResolution(g_addr, 12);
	lcd.print("RES=");
	lcd.println(g_dallas.getResolution(g_addr));
	lcd.update();
	delay(2000);
#endif

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);
}


// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dhtLib_wrapper() { DHTLib.dht11Callback(); }


long  g_tt = 0;
constexpr int8_t g_sz = 8;
float g_cc[g_sz];
float g_hh[g_sz];
float g_dd[g_sz];
float g_ds[g_sz];
int8_t g_top = 0; // g_top = 0 => il piu' recente

void pp(float m, float a[], int8_t asz)
{
	lcd.print(' ');
	for (int8_t i = 0; i < asz; ++i)
	{
		auto v = (i == 0) ? m : a[i-1];
		char c;
		/***/if (v > a[i]) c = '/';
		else if (v < a[i]) c = '\\';
		else c = '-';
		lcd.print(c);
	}
	for (int8_t i = asz; i < g_sz; ++i)
		lcd.print('.');
	lcd.println();
}
void wd()
{
	static uint8_t g_wd = 0;

	lcd.gotoXY(lcd.LCD_X-6, lcd.LCD_Y-8);
	char p=0;
	switch (g_wd)
	{
	case 0: p = '-'; break;
	case 1: p = '\\'; break;
	case 2: p = '|'; break;
	case 3: p = '/'; break;
	}
	lcd.println(p);
	g_wd = (g_wd + 1) % 4;
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

void loop()
{
	g_tt += 2;
	auto t = millis();

	lcd.clear();
	lcd.gotoXY(0, 0);


#ifdef DALLAS
	float tds;
	dallasError de = readDallasTemp(tds);
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
	default: 
		lcd.println(F("Unknown error")); 
		break;
	}

	if (de == dallasError::ok && dhtError == IDDHTLIB_OK)
	{
		auto tcc = DHTLib.getCelsius();
		auto thh = DHTLib.getHumidity();
		auto tdd = DHTLib.getDewPoint();

#ifdef DALLAS
		lcd.print(F("CE ")); lcd.print(tds, 1);pp(tcc, g_ds, g_top);
#endif
		lcd.print(F("CI ")); lcd.print(tcc, 1); pp(tcc, g_cc, g_top);
		lcd.print(F("H  ")); lcd.print(thh, 1); pp(thh, g_hh, g_top);

		if (tdd < 10) {
		lcd.print(F("DP  ")); lcd.print(tdd, 1); pp(tdd, g_dd, g_top);
		}
		else {
		lcd.print(F("DP ")); lcd.print(tdd, 1); pp(tdd, g_dd, g_top);
		}

		//lcd.println();

		/***/if (tdd <= 10) lcd.println(F("1 Molto secco"));
		else if (tdd <= 12) lcd.println(F("2 Secco"));
		else if (tdd <= 16) lcd.println(F("3 Confort"));
		else if (tdd <= 18) lcd.println(F("4 Poco umido"));
		else if (tdd <= 21) lcd.println(F("5 Umido"));
		else if (tdd <= 24) lcd.println(F("6 Molto umido"));
		else                lcd.println(F("7 Afa"));
		lcd.println(F("Scala 1..7"));

		// ogni due ore storicizzo ==> 12ore di storia
		if (g_tt % (2 * 60 * 60) == 0)
		{
			for (int8_t t = 1; t < g_top; ++t)
			{
				g_cc[t] = g_cc[t-1];
				g_hh[t] = g_hh[t-1];
				g_dd[t] = g_dd[t-1];
				g_ds[t] = g_ds[t-1];
			};
			g_cc[0] = tcc;
			g_hh[0] = thh;
			g_dd[0] = tdd;
			g_ds[0] = tds;

			g_top+=1;
			if (g_top == g_sz) g_top = g_sz - 1;
		}
	}

	wd();
	lcd.update();

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);
	auto tn = millis();
	if (t + 2000 > tn)
		delay(2000 - (tn - t));
}
