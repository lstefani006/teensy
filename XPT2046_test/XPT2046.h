#ifndef _XPT2046_h_
#define _XPT2046_h_

class XPT2046 
{
public:
	enum class adc_ref_t : uint8_t { MODE_SER, MODE_DFR };

	XPT2046(uint8_t cs_pin, uint8_t irq_pin);

	void begin();
	bool isTouching() const;
	void getPosition(uint16_t &x, uint16_t &y, adc_ref_t mode = adc_ref_t::MODE_DFR, uint8_t max_samples = 0xff) const;

private:
	void powerDown() const;
	void getRaw(uint16_t &vi, uint16_t &vj, adc_ref_t mode, uint8_t max_samples) const;

	uint8_t _cs_pin, _irq_pin;

	uint16_t _readLoop(uint8_t ctrl, uint8_t max_samples) const;
};

inline bool XPT2046::isTouching() const { return (digitalRead(_irq_pin) == LOW); }

#endif  // _XPT2046_h_
