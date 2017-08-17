#ifndef __Timer_hpp__
#define __Timer_hpp__


class Timer
{
public:
	Timer(uint32_t tm) : _tm(tm) {}

	enum OutputCompareType { OC1, OC2, OC3, OC4 };

	void begin(int freq, int maxCount);
	void setUpdateIrq(bool enable = true);
	void setOutputCompareIrq(OutputCompareType oc, uint32_t value);
	void setOutputCompareDigIO(OutputCompareType oc, uint32_t value);

	void irq();

protected:
	virtual void UpdateInterrupt() = 0;
	virtual void CompareInterrupt(int oc) = 0;

private:
	uint32_t _tm;
};

#endif
