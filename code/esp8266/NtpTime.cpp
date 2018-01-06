#include <NtpTime.hpp>
#include "ESP8266WiFi.h"

void NtpTime::begin(int localPort)
{
	_udp.begin(localPort);
	_tt.SetEpoch(0);
}

void NtpTime::handle()
{
	auto t1 = millis() / 1000;

	if (_tt.Epoch() == 0 || t1 - _t0 >= 3600)
	{
		if (readTime())
			_t0 = t1;
	}
}

DateTime NtpTime::toDateTime() const
{
	if (_tt.Epoch() == 0)
		return _tt;

	auto t1 = millis() / 1000;
	DateTime r = _tt;
	r.AddSeconds(t1 - _t0);
	return r;
}

bool NtpTime::readTime()
{
	Serial.println("sending NTP packet...");
	const int NTP_PACKET_SIZE = 48;		// NTP time stamp is in the first 48 bytes of the message
	byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

	IPAddress timeServerIP; // time.nist.gov NTP server address
	WiFi.hostByName("time.nist.gov", timeServerIP);

	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011; // LI, Version, Mode
	packetBuffer[1] = 0;		  // Stratum, or type of clock
	packetBuffer[2] = 6;		  // Polling Interval
	packetBuffer[3] = 0xEC;		  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	_udp.beginPacket(timeServerIP, 123); //NTP requests are to port 123
	_udp.write(packetBuffer, NTP_PACKET_SIZE);
	_udp.endPacket();

	delay(1000);
	int cb = _udp.parsePacket();
	if (!cb)
		return false;

	// We've received a packet, read the data from it
	_udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

	//the timestamp starts at byte 40 of the received packet and is four bytes,
	// or two words, long. First, esxtract the two words:

	unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
	unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
	// combine the four bytes (two words) into a long integer
	// this is NTP time (seconds since Jan 1 1900):
	unsigned long secsSince1900 = highWord << 16 | lowWord;

	// now convert NTP time into everyday time:
	// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
	const unsigned long seventyYears = 2208988800UL;
	// subtract seventy years:
	unsigned long epoch = secsSince1900 - seventyYears;

	this->_tt.SetEpoch(epoch + 3600);

	return true;
}
