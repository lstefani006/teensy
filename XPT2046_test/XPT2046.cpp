#include <Arduino.h>
#include <SPI.h>

#include "XPT2046.h"

constexpr uint8_t CTRL_LO_DFR = 0b0011;
constexpr uint8_t CTRL_LO_SER = 0b0100;
constexpr uint8_t CTRL_HI_X = 0b1001  << 4;
constexpr uint8_t CTRL_HI_Y = 0b1101  << 4;


XPT2046::XPT2046 (uint8_t cs_pin, uint8_t irq_pin) 
	: _cs_pin(cs_pin), _irq_pin(irq_pin)
{
}

void XPT2046::begin() {
	pinMode(_cs_pin, OUTPUT);
	pinMode(_irq_pin, INPUT_PULLUP);

	SPI.begin();

	powerDown();  // Make sure PENIRQ is enabled
}

uint16_t XPT2046::_readLoop(uint8_t ctrl, uint8_t max_samples) const {
	uint16_t prev = 0xffff, cur = 0xffff;
	uint8_t i = 0;
	do {
		prev = cur;
		cur = SPI.transfer(0);
		cur = (cur << 4) | (SPI.transfer(ctrl) >> 4);  // 16 clocks -> 12-bits (zero-padded at end)
	} while ((prev != cur) && (++i < max_samples));
	//Serial.print("RL i: "); Serial.println(i); Serial.flush();  // DEBUG
	return cur;
}

// TODO: Caveat - MODE_SER is completely untested!!
//   Need to measure current draw and see if it even makes sense to keep it as an option
void XPT2046::getRaw(uint16_t &vi, uint16_t &vj, adc_ref_t mode, uint8_t max_samples) const {
	// Implementation based on TI Technical Note http://www.ti.com/lit/an/sbaa036/sbaa036.pdf

	uint8_t ctrl_lo = ((mode == adc_ref_t::MODE_DFR) ? CTRL_LO_DFR : CTRL_LO_SER);

	digitalWrite(_cs_pin, LOW);
	SPI.transfer(CTRL_HI_X | ctrl_lo);  // Send first control byte
	vi = _readLoop(CTRL_HI_X | ctrl_lo, max_samples);
	vj = _readLoop(CTRL_HI_Y | ctrl_lo, max_samples);

	if (mode == adc_ref_t::MODE_DFR) {
		// Turn off ADC by issuing one more read (throwaway)
		// This needs to be done, because PD=0b11 (needed for MODE_DFR) will disable PENIRQ
		SPI.transfer(0);  // Maintain 16-clocks/conversion; _readLoop always ends after issuing a control byte
		SPI.transfer(CTRL_HI_Y | CTRL_LO_SER);
	}
	SPI.transfer16(0);  // Flush last read, just to be sure

	digitalWrite(_cs_pin, HIGH);
}

void XPT2046::getPosition(uint16_t &x, uint16_t &y, adc_ref_t mode, uint8_t max_samples) const 
{
	if (!isTouching()) 
	{
		x = y = 0xffff;
		return;
	}

	x = y = 0xffff;
	uint16_t vi, vj;
	getRaw(vi, vj, mode, max_samples);
	x = vi;
	y = vj;
}

void XPT2046::powerDown() const 
{
	digitalWrite(_cs_pin, LOW);
	// Issue a throw-away read, with power-down enabled (PD{1,0} == 0b00)
	// Otherwise, ADC is disabled
	SPI.transfer(CTRL_HI_Y | CTRL_LO_SER);
	SPI.transfer16(0);  // Flush, just to be sure
	digitalWrite(_cs_pin, HIGH);
}
