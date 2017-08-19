#ifndef __Timer_hpp__
#define __Timer_hpp__

#include <stdint.h>

class Timer
{
public:
	Timer(uint32_t tm) : _tm(tm) {}
	virtual ~Timer();

	enum OutputCompareType { OC1, OC2, OC3, OC4 };

	void begin(uint32_t freq, uint32_t maxCount);
	void setUpdateIrq(bool enable = true);
	void setOutputCompareIrq(OutputCompareType oc, uint32_t value);
	void setOutputCompareDigIO(OutputCompareType oc, uint32_t value);

	uint32_t getCounter();

	void irq();

protected:
	virtual void UpdateInterrupt() = 0;
	virtual void CompareInterrupt(int oc) = 0;

private:
	uint32_t _tm;
};

#endif
