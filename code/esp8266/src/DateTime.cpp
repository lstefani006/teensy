
#include <Arduino.h>
#include <DateTime.hpp>
#include <uprintf.hpp>

void DateTime::Set(int YYYY, int MM, int DD, int hh, int mm, int ss)
{
	auto y = YYYY;
	auto m = MM;
	auto d = DD;

	y -= m <= 2;
	const int era = (y >= 0 ? y : y - 399) / 400;
	const int yoe = y - era * 400;									// [0, 399]
	const int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0, 365]
	const int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;			// [0, 146096]
	auto e = era * 146097 + doe - 719468;

	e *= 24 * 60 * 60;
	e += hh * 60 * 60;
	e += mm * 60;
	e += ss;

	this->_epoch = e;
}

DateTime::ts DateTime::toDateTime() const
{
	auto e = _epoch;

	ts r;
	r.ss = e % 60;
	e /= 60;
	r.mm = e % 60;
	e /= 60;
	r.hh = e % 24;
	e /= 24;

	e += 719468;
	const int era = (e >= 0 ? e : e - 146096) / 146097;
	const int doe = e - era * 146097;									   // [0, 146096]
	const int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365; // [0, 399]
	const int y = yoe + era * 400;
	const int doy = doe - (365 * yoe + yoe / 4 - yoe / 100); // [0, 365]
	const int mp = (5 * doy + 2) / 153;						 // [0, 11]
	const int d = doy - (153 * mp + 2) / 5 + 1;				 // [1, 31]
	const int m = mp + (mp < 10 ? 3 : -9);					 // [1, 12]

	r.YYYY = y + (m <= 2);
	r.MM = m;
	r.DD = d;

	return r;
}

String DateTime::toString() const
{
	char b[20];
	DateTime::ts t = toDateTime();
	usprintf(b, sizeof(b), "%04d/%02d/%02d %02d:%02d:%02d", t.YYYY, t.MM, t.DD, t.hh, t.mm, t.ss);
	return b;
}
String DateTime::toDateString() const
{
	char b[20];
	DateTime::ts t = toDateTime();
	usprintf(b, sizeof(b), "%04d/%02d/%02d", t.YYYY, t.MM, t.DD);
	return b;
}
String DateTime::toTimeString() const
{
	char b[20];
	DateTime::ts t = toDateTime();
	usprintf(b, sizeof(b), "%02d:%02d:%02d", t.hh, t.mm, t.ss);
	return b;
}
