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
t::Lcd<typeof(spi), PIN_RESET, PIN_DC, true> lcd(spi);

void setup()
{
	t::SetPrint(&lcd);
	spi.begin();

	lcd.begin();
	lcd.clear();
	lcd.gotoXY(0, 0);
	lcd.setContrast(35);

	lcd.print(F("START"));
	delay(2000);

	lcd.clear();
	lcd.gotoXY(0, 0);
}


// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dhtLib_wrapper() { DHTLib.dht11Callback(); }

long  g_tt = 0;
float g_cc = 0;
float g_hh = 0;
float g_dd = 0;

void pp(float m, float a)
{
	/***/if (m > a) lcd.println("+");
	else if (m < a) lcd.println("-");
	else lcd.println();
}

void loop()
{
	DHTLib.acquire();
	while (DHTLib.acquiring()) delay(10);

	lcd.clear();
	lcd.gotoXY(0, 0);

	switch (DHTLib.getStatus())
	{
	case IDDHTLIB_OK: 
		{
			auto tcc = DHTLib.getCelsius();
			auto thh = DHTLib.getHumidity();
			auto tdd = DHTLib.getDewPoint();

			if (g_tt == 0) 
			{
				g_cc = tcc;
				g_hh = thh;
				g_dd = tdd;
			}

			lcd.print(/*F*/("C' : ")); lcd.print(int16_t(tcc + 0.5)); pp(tcc, g_cc);
			lcd.print(F("H% : ")); lcd.print(int16_t(thh + 0.5)); pp(thh, g_hh);
			lcd.print(F("DP : ")); lcd.print(tdd, 2); pp(tdd, g_dd);

			lcd.println();

			g_tt++;
			if (g_tt >= 30 * 60)
			{
				g_cc = tcc;
				g_hh = thh;
				g_dd = tdd;

				g_tt = 0;
			}

			auto dp = tdd;
			/***/if (dp <= 10) lcd.println(F("1 Molto secco"));
			else if (dp <= 12) lcd.println(F("2 Secco"));
			else if (dp <= 16) lcd.println(F("3 Confort"));
			else if (dp <= 18) lcd.println(F("4 Poco umido"));
			else if (dp <= 21) lcd.println(F("5 Umido"));
			else if (dp <= 24) lcd.println(F("6 Molto umido"));
			else               lcd.println(F("7 Afa"));
			lcd.println(F("Scala 1..7"));
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

	delay(2000);
}
