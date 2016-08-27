/*
	FILE: 		idDHTLib.cpp
	VERSION: 	0.0.3
	PURPOSE: 	Interrupt driven Lib for DHT11 and DHT22 with Arduino.
	LICENCE:	GPL v3 (http://www.gnu.org/licenses/gpl.html)
	DATASHEET: http://www.micro4you.com/files/sensor/DHT11.pdf
	DATASHEET: http://www.adafruit.com/datasheets/DHT22.pdf
	
	Based on idDHT11 library: https://github.com/niesteszeck/idDHT11
	Based on DHTLib library: http://playground.arduino.cc/Main/DHTLib
	Based on code proposed: http://forum.arduino.cc/index.php?PHPSESSID=j6n105kl2h07nbj72ac4vbh4s5&topic=175356.0
	
	Changelog:
		v 0.0.1
			fork from idDHT11 lib
			change names to idDHTLib
			added DHT22 functionality
		v 0.0.2
			Optimization on shift var (pylon from Arduino Forum)
		v 0.0.3
			Timing correction to finally work properly on DHT22
			(Dessimat0r from Arduino forum)
 */

#include "idDHTLib.h"
#define DEBUG_idDHTLIB

idDHTLib::idDHTLib(int pin, int intNumber,void (*callback_wrapper)()) {
	init(pin, intNumber,callback_wrapper);
}

void idDHTLib::init(int pin, int intNumber, void (*callback_wrapper) ()) {
	this->_intNumber = intNumber;
	this->_pin = pin;
	this->isrCallback_wrapper = callback_wrapper;
	this->_hum = 0;
	this->_temp = 0;
	pinMode(pin, OUTPUT);
	digitalWrite(pin, HIGH);
	this->_state = STOPPED;
	this->_status = IDDHTLIB_ERROR_NOTSTARTED;
}

int idDHTLib::acquire() {
	if (this->_state == STOPPED || this->_state == ACQUIRED) {

		//set the state machine for interruptions analisis of the signal
		this->_state = RESPONSE;

		// EMPTY BUFFER and vars
		for (int i=0; i < 5; i++) this->_bits[i] = 0;
		this->_cnt = 7;
		this->_idx = 0;
		this->_hum = 0;
		this->_temp = 0;

		// REQUEST SAMPLE
		pinMode(this->_pin, OUTPUT);
		digitalWrite(this->_pin, LOW);
		delay(18);
		digitalWrite(this->_pin, HIGH);
		delayMicroseconds(25);
		pinMode(this->_pin, INPUT);

		// Analize the data in an interrupt
		us = micros();
		attachInterrupt(this->_intNumber,isrCallback_wrapper,FALLING);

		return IDDHTLIB_ACQUIRING;
	} else
		return IDDHTLIB_ERROR_ACQUIRING;
}
int idDHTLib::acquireAndWait() {
	acquire();
	while(acquiring())
		;
	return getStatus();
}
void idDHTLib::dht11Callback() {
	isrCallback(false);
}
void idDHTLib::dht22Callback() {
	isrCallback(true);
}
void idDHTLib::isrCallback(bool dht22) {
	int newUs = micros();
	int delta = (newUs-us);
	us = newUs;
	if (delta>6000) {
		this->_status = IDDHTLIB_ERROR_TIMEOUT;
		this->_state = STOPPED;
		detachInterrupt(this->_intNumber);
		return;
	}
	switch(this->_state) {
	case STOPPED:
	case ACQUIRING:
	case ACQUIRED:
		// non erano gestite....
		break;
	case RESPONSE:
		if(delta<25){
			us -= delta;
			break; //do nothing, it started the response signal
		} if(125<delta && delta<190) {
			this->_state = DATA;
		} else {
			detachInterrupt(this->_intNumber);
			this->_status = IDDHTLIB_ERROR_TIMEOUT;
			this->_state = STOPPED;
		}
		break;
	case DATA:
		if(60<delta && delta<145) { //valid in timing
			this->_bits[this->_idx] <<= 1; //shift the data
			if(delta>100) //is a one
				this->_bits[this->_idx] |= 1;
			if (this->_cnt == 0) {  // when we have fulfilled the byte, go to the next
				this->_cnt = 7;    // restart at MSB
				if(++this->_idx == 5) { // go to next byte; when we have got 5 bytes, stop.
					detachInterrupt(this->_intNumber);
					// WRITE TO RIGHT VARS 
					uint8_t sum;
					if (dht22) {
						this->_hum = word(this->_bits[0], this->_bits[1]) * 0.1;
						this->_temp = (this->_bits[2] & 0x80 ?
								-word(this->_bits[2] & 0x7F, this->_bits[3]) :
								word(this->_bits[2], this->_bits[3]))
							* 0.1;
						sum = this->_bits[0] + this->_bits[1] + this->_bits[2] + this->_bits[3];
					} else {
						this->_hum    = this->_bits[0]; 
						// as bits[1] and bits[3] are always zero they are omitted in formulas.
						this->_temp = this->_bits[2];
						sum = this->_bits[0] + this->_bits[2];
					}  
					if (this->_bits[4] != (sum&0xFF)) {
						this->_status = IDDHTLIB_ERROR_CHECKSUM;
						this->_state = STOPPED;
					} else {
						this->_status = IDDHTLIB_OK;
						this->_state = ACQUIRED;
					}
					break;
				}
			} else this->_cnt--;
		} else if(delta<10) {
			detachInterrupt(this->_intNumber);
			this->_status = IDDHTLIB_ERROR_DELTA;
			this->_state = STOPPED;
		} else {
			detachInterrupt(this->_intNumber);
			this->_status = IDDHTLIB_ERROR_TIMEOUT;
			this->_state = STOPPED;
		}
		break;
	default:
		break;
	}
}
bool idDHTLib::acquiring() {
	if (this->_state != ACQUIRED && this->_state != STOPPED)
		return true;
	return false;
}
int idDHTLib::getStatus() {
	return this->_status;
}
float idDHTLib::getCelsius() {
	IDDHTLIB_CHECK_STATE;
	return this->_temp;
}

float idDHTLib::getHumidity() {
	IDDHTLIB_CHECK_STATE;
	return this->_hum;
}

float idDHTLib::getFahrenheit() {
	IDDHTLIB_CHECK_STATE;
	return this->_temp * 1.8f + 32;
}

float idDHTLib::getKelvin() {
	IDDHTLIB_CHECK_STATE;
	return this->_temp + 273.15f;
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
float idDHTLib::getDewPoint() {
	IDDHTLIB_CHECK_STATE;
	float a = 17.271f;
	float b = 237.7f;
	float temp_ = (a * (float) this->_temp) / (b + (float) this->_temp) + (float)log( (float) this->_hum/100);
	float Td = (b * temp_) / (a - temp_);
	return Td;

}
// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
float idDHTLib::getDewPointSlow() {
	IDDHTLIB_CHECK_STATE;
	float fA0= 373.15f/(273.15f + (float) this->_temp);
	float SUM = -7.90298f * (fA0-1);
	SUM += 5.02808f * (float)log10(fA0);
	SUM += -1.3816e-7f * float(pow(10, (11.344f*(1-1/fA0)))-1) ;
	SUM += 8.1328e-3f * float(pow(10,(-3.49149f*(fA0-1)))-1) ;
	SUM += (float)log10(1013.246f);
	float VP = (float)pow(10, SUM-3) * (float) this->_hum;
	float T = (float)log(VP/0.61078f);   // temp var
	return (241.88f * T) / (17.558f-T);
}
// EOF
