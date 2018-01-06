#ifndef __NtpTime_hpp__
#define __NtpTime_hpp__

#include <DateTime.hpp>
#include <WiFiUdp.h>

class NtpTime
{
  public:
	void begin(int localPort = 2390);

	bool valid() const { return _tt.Epoch() != 0; }

	void handle();

	DateTime toDateTime() const;

  private:
	bool readTime();

	DateTime _tt;
	WiFiUDP _udp;
	int _t0;
};

#endif
