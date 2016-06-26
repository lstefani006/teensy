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

	lcd.print("START");
	delay(3000);
	lcd.clear();
	lcd.gotoXY(0, 0);
	delay(3000); // The sensor need like 2 sec to initialize, if you have some code before this that make a delay, you can eliminate this delay
}


// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dhtLib_wrapper() { DHTLib.dht11Callback(); }

void loop()
{
	lcd.clear();
	lcd.gotoXY(0, 0);
	//lcd.print("\nRetrieving information from sensor: ");
	lcd.print("RX....");
	lcd.gotoXY(0, 0);

	// int result = DHTLib.acquireAndWait();

	DHTLib.acquire();
	while(DHTLib.acquiring()) delay(10);

	auto result =  DHTLib.getStatus();
	switch (result)
	{
	case IDDHTLIB_OK: 
		{
			lcd.print("C'   : ");
			lcd.println(DHTLib.getCelsius(), 2);

			lcd.print("H%   : ");
			lcd.println(DHTLib.getHumidity(), 2);

			lcd.print("DP  C: ");
			auto dp = DHTLib.getDewPoint();
			lcd.println(dp);

			lcd.println();

			/***/if (dp <= 10) lcd.println("1 Molto secco");
			else if (dp <= 12) lcd.println("2 Molto confort");
			else if (dp <= 16) lcd.println("3 Confort");
			else if (dp <= 18) lcd.println("4 Poco umido");
			else if (dp <= 21) lcd.println("5 Umido");
			else if (dp <= 24) lcd.println("6 Molto umido");
			else               lcd.println("7 Afa");
			lcd.println("Scala 1..7");
		}
		break;
	case IDDHTLIB_ERROR_CHECKSUM: 
		lcd.println("Error\n\r\tChecksum error"); 
		break;
	case IDDHTLIB_ERROR_TIMEOUT: 
		lcd.println("Error\n\r\tTime out error"); 
		break;
	case IDDHTLIB_ERROR_ACQUIRING: 
		lcd.println("Error\n\r\tAcquiring"); 
		break;
	case IDDHTLIB_ERROR_DELTA: 
		lcd.println("Error\n\r\tDelta time to small"); 
		break;
	case IDDHTLIB_ERROR_NOTSTARTED: 
		lcd.println("Error\n\r\tNot started"); 
		break;
	default: 
		lcd.println("Unknown error"); 
		break;
	}

	delay(2000);
}
