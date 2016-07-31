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

constexpr int idDHTLibPin = 2; //Digital pin for comunications
constexpr int idDHTLibIntNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dhtLib_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHTLib DHTLib(idDHTLibPin,idDHTLibIntNumber,dhtLib_wrapper);

#define PIN_CE    10
#define PIN_SCLK  13
#define PIN_SDIN  11
#define PIN_RESET  9
#define PIN_DC    14

t::hwSPI<PIN_CE, PIN_SCLK, PIN_SDIN, -1> spi;
t::Lcd<typeof(spi), PIN_RESET, PIN_DC, false> lcd(spi);

void setup()
{
	t::SetPrint(&lcd);
	spi.begin();

	lcd.begin();
	lcd.clear();
	lcd.gotoXY(0, 0);
	lcd.setContrast(35);

	lcd.clear();
	lcd.print(F("START"));
	lcd.update();
	delay(2000);

	lcd.clear();
	lcd.gotoXY(0, 0);
	lcd.update();
	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);
}


// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dhtLib_wrapper() { DHTLib.dht11Callback(); }

long  g_tt = 0;
constexpr int8_t g_sz = 6;
float g_cc[g_sz];
float g_hh[g_sz];
float g_dd[g_sz];
int8_t g_top = 0;

uint8_t g_wd = 0;

void pp(float m, float a[], int asz)
{
	lcd.print(' ');
	for (int8_t i = 0; i < asz; ++i)
	{
		char c;
		/***/if (m > a[i]) c = '/';
		else if (m < a[i]) c = '\\';
		else c = '-';
		lcd.print(c);
	}
	for (int8_t i = asz; i < g_sz; ++i)
		lcd.print('.');
	lcd.println();
}
void wd()
{
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

void loop()
{
	g_tt += 2;
	auto t = millis();

	lcd.clear();
	lcd.gotoXY(0, 0);

	switch (DHTLib.getStatus())
	{
	case IDDHTLIB_OK: 
		{
			auto tcc = DHTLib.getCelsius();
			auto thh = DHTLib.getHumidity();
			auto tdd = DHTLib.getDewPoint();

			lcd.print(F("C' ")); lcd.print(tcc, 1); pp(tcc, g_cc, g_top);
			lcd.print(F("H% ")); lcd.print(thh, 1); pp(thh, g_hh, g_top);
			lcd.print(F("DP ")); lcd.print(tdd, 1); pp(tdd, g_dd, g_top);

			lcd.println();

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
					g_cc[t-1] = g_cc[t];
					g_hh[t-1] = g_hh[t];
					g_dd[t-1] = g_dd[t];
				};
				g_cc[g_top] = tcc;
				g_hh[g_top] = thh;
				g_dd[g_top] = tdd;

				g_top+=1;
				if (g_top == g_sz) g_top = g_sz - 1;
			}
		}
		break;

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

	wd();
	lcd.update();

	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);
	t = millis() - t;
	delay(2000 - t);
}
