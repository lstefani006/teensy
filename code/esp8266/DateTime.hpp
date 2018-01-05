#ifndef __DateTime_hpp__
#define __DateTime_hpp__

#include <stdint.h>
#include <Arduino.h>

class DateTime
{
  public:
	struct ts
	{
		int16_t YYYY;
		int8_t DD, MM;
		int8_t hh, mm, ss;
	};

	DateTime() { _epoch = 0; }
	DateTime(int YYYY, int MM, int DD, int hh, int mm, int ss) { Set(YYYY, MM, DD, hh, mm, ss); }
	DateTime(const DateTime::ts &t) { Set(t); }

	void SetEpoch(int32_t e) { _epoch = e; }
	void AddSeconds(int32_t sec) { _epoch += sec; }

	void Set(int YYYY, int MM, int DD, int hh = 0, int mm = 0, int ss = 0);
	void Set(const DateTime::ts &t) { Set(t.YYYY, t.MM, t.DD, t.hh, t.mm, t.ss); }

	ts toDateTime() const;

	String toString() const;
	String toDateString() const;
	String toTimeString() const;

	int32_t Epoch() const { return _epoch; }

  private:
	int32_t _epoch;
};

inline int32_t operator-(const DateTime &a, const DateTime &b)
{
	return a.Epoch() - b.Epoch();
}


#endif
