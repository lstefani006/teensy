#include "OneWire.h"
#include "DallasTemperature.h"
#include "t_io.h"

//////////////////////////////////////////////////////////////////

#define PIN_CE    10
#define PIN_SDIN  11
#define PIN_SCLK  13
#define PIN_RESET  9
#define PIN_DC    14

static OneWire ds(22);
static DallasTemperature sensors(&ds);

static DeviceAddress ds0;

void setup()
{
	Serial.begin(9600);
	t::SetPrint(&Serial);

	sensors.begin();

	sensors.getAddress(ds0, 0);
	sensors.setWaitForConversion(false);
}
void loop(void)
{
	Serial.print("Requesting temperatures...");
	bool b = sensors.requestTemperaturesByAddress(ds0);
	if (b == false) {
		Serial.println("device not present\n");
		return;
	}
	unsigned long t = millis();
	unsigned long d;
	DallasTemperature::ScratchPad sc;
	for (;;) 
	{
		delay(10);
		d = millis() - t;
		if (sensors.isConnected(ds0, sc))
			break;

		if (d > 1000)
		{
			Serial.println("Timeout\n");
			return;
		}
	}
	float temp = sensors.calculateTemperature(ds0, sc);

	static int step = 0;
	Serial.print("Step = ");
	Serial.print(step++);
	Serial.print("\n");

	Serial.print("Temp = ");
	Serial.print(temp);
	Serial.print("\n");

	Serial.print("Time = ");
	Serial.print((int)d);
	Serial.print("ms\n");

	Serial.print("Resolution = ");
	Serial.print(sensors.getResolution());
	Serial.print("\n");

	for(;;);
}
