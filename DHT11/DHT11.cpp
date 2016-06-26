/*
   Board	          int.0	  int.1	  int.2	  int.3	  int.4	  int.5
   Uno, Ethernet	  2	  3
   Mega2560	          2	  3	  21	  20	  19	  18
   Leonardo	          3	  2	  0	       1
   Due	          (any pin, more info http://arduino.cc/en/Reference/AttachInterrupt)

   This example, as difference to the other, make use of the new method acquireAndWait()
*/

#include <idDHTLib.h>

/*
#include "timer.hpp"

void timer2_tick()
{
	static uint16_t t = 0;
	t += 1;
	if (t == 1000)
	{
		t = 0;
		uint8_t a = digitalRead(LED_BUILTIN);
		digitalWrite(LED_BUILTIN, !a);
	}
}
*/

constexpr int idDHTLibPin = 2; //Digital pin for comunications
constexpr int idDHTLibIntNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dhtLib_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHTLib DHTLib(idDHTLibPin,idDHTLibIntNumber,dhtLib_wrapper);


void setup()
{
	Serial.begin(38400);

	pinMode(LED_BUILTIN, OUTPUT);
	/*
#ifdef __AVR__
	timer2.setup_overflow(1000ul);
#endif
	*/

	Serial.println("idDHTLib Example program");
	Serial.print("LIB version: ");
	Serial.println(IDDHTLIB_VERSION);
	Serial.println("---------------");
	delay(3000); // The sensor need like 2 sec to initialize, if you have some code before this that make a delay, you can eliminate this delay
}


// This wrapper is in charge of calling 
// mus be defined like this for the lib work
void dhtLib_wrapper() { DHTLib.dht11Callback(); }

void loop()
{
	Serial.print("\nRetrieving information from sensor: ");
	Serial.print("Read sensor: ");

	// int result = DHTLib.acquireAndWait();

	DHTLib.acquire();
	while(DHTLib.acquiring());

	auto result =  DHTLib.getStatus();
	switch (result)
	{
	case IDDHTLIB_OK: 
		Serial.println("OK"); 
		break;
	case IDDHTLIB_ERROR_CHECKSUM: 
		Serial.println("Error\n\r\tChecksum error"); 
		break;
	case IDDHTLIB_ERROR_TIMEOUT: 
		Serial.println("Error\n\r\tTime out error"); 
		break;
	case IDDHTLIB_ERROR_ACQUIRING: 
		Serial.println("Error\n\r\tAcquiring"); 
		break;
	case IDDHTLIB_ERROR_DELTA: 
		Serial.println("Error\n\r\tDelta time to small"); 
		break;
	case IDDHTLIB_ERROR_NOTSTARTED: 
		Serial.println("Error\n\r\tNot started"); 
		break;
	default: 
		Serial.println("Unknown error"); 
		break;
	}
	Serial.print("Humidity (%): ");
	Serial.println(DHTLib.getHumidity(), 2);

	Serial.print("Temperature (oC): ");
	Serial.println(DHTLib.getCelsius(), 2);

	Serial.print("Dew Point (oC): ");
	Serial.println(DHTLib.getDewPoint());

	Serial.print("Dew Point Slow (oC): ");
	Serial.println(DHTLib.getDewPointSlow());

	Serial.println("< 17 confort");
	Serial.println("= 17 debole afa");
	Serial.println("> 21 afa fastidiosa");

	delay(2000);
}
