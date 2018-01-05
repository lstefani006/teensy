#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <DallasTemperature.h>
#include <uprintf.hpp>
#include <ArduinoOTA.h>
#include <DateTime.hpp>
#include <ESP8266mDNS.h>

ESP8266WebServer server(8123);
OneWire oneWire(13);
DallasTemperature sensors(&oneWire);

class NtpTime
{
  public:
	void begin(int localPort = 2390)
	{
		_udp.begin(localPort);
		_tt.SetEpoch(0);
	}

	bool valid() const { return _tt.Epoch() != 0; }

	void handle()
	{
		auto t1 = millis() / 1000;

		if (_tt.Epoch() == 0 || t1 - _t0 >= 3600)
		{
			if (readTime())
				_t0 = t1;
		}
	}

	DateTime toDateTime() const
	{
		if (_tt.Epoch() == 0)
			return _tt;

		auto t1 = millis() / 1000;
		DateTime r = _tt;
		r.AddSeconds(t1 - _t0);
		return r;
	}

  private:
	bool readTime()
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

	DateTime _tt;
	WiFiUDP _udp;
	int _t0;
};

const char *print_statuus(wl_status_t st)
{
	switch (st)
	{
	case WL_NO_SHIELD:
		return "WL_NO_SHIELD";
	case WL_IDLE_STATUS:
		return "WL_IDLE_STATUS";
	case WL_NO_SSID_AVAIL:
		return "WL_NO_SSID_AVAIL";
	case WL_SCAN_COMPLETED:
		return "WL_SCAN_COMPLETED";
	case WL_CONNECTED:
		return "WL_CONNECTED";
	case WL_CONNECT_FAILED:
		return "WL_CONNECT_FAILED";
	case WL_CONNECTION_LOST:
		return "WL_CONNECTION_LOST";
	case WL_DISCONNECTED:
		return "WL_DISCONNECTED";
	}
	return "???";
}

void blink(int n, int pin = 2)
{
	for (int i = 0; i < n * 2; ++i)
	{
		delay(200);
		int v = digitalRead(pin);
		if (!v)
			digitalWrite(pin, HIGH);
		else
			digitalWrite(pin, LOW);
	}
}

NtpTime ntp;

DateTime s_tm[6 * 24];
float s_temp[6 * 24];
int s_ti = 0;
float s_last_temp;
DateTime s_last_time;

void setup()
{
	Serial.begin(38400);

	pinMode(2, OUTPUT);
	digitalWrite(2, LOW);

	uprintf_cb.pf = [](char c, void *) {  Serial.print(c); return true; };
	uprintf_cb.ag = nullptr;

	uprintf("Inizio\n");
	delay(5000);

	blink(3);

	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0)
		Serial.println("no networks found");
	else
	{
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i)
		{
			// Print SSID and RSSI for each network found
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
			delay(10);
		}
	}
	blink(3);
	Serial.println();

	Serial.println("Connecting");
	WiFi.mode(WIFI_STA);
	WiFi.begin("Vodafone-11394415", "agyeiickkqbduukzzjpisbou");

	for (auto i = 0;; ++i)
	{
		auto st = WiFi.status();
		if (st == WL_CONNECTED)
			break;
		Serial.print(i);
		Serial.print(" => ");
		Serial.println(print_statuus(st));
		delay(500);
	}

	blink(10);
	Serial.println();
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	sensors.setCheckForConversion(false);
	sensors.begin();

	server.on("/", []() {
		blink(1);
		String msg = "<html>";

		upf_t e;
		e.ag = &msg;
		e.pf = [](char c, void *a) { ((String *)a)->concat(c); return true; };

		msg += "<head>";
		msg += "<script src='//cdnjs.cloudflare.com/ajax/libs/dygraph/2.1.0/dygraph.min.js'></script>";
		msg += "<link rel='stylesheet' src='//cdnjs.cloudflare.com/ajax/libs/dygraph/2.1.0/dygraph.min.css' />";
		msg += "</head>";
		msg += "<body>";
		msg += "<h0>Temperature</h0>";
		msg += "<br/>";
		uprintf(e, "<span>Ora = %s</span><br/>", s_last_time.toString().c_str());
		uprintf(e, "<span>Temp = %f</span><br/>\n", s_last_temp);
		msg += "<div id='graphdiv'></div>";
		msg += "<script type='text/javascript'>\n";
		msg += "g = new Dygraph(\n";
		msg += "document.getElementById('graphdiv'),\n";

		msg += "[\n";
		for (auto i = 0; i < s_ti; ++i)
		{
			msg += (i > 0) ? "," : " ";
			DateTime::ts t = s_tm[i].toDateTime();
			uprintf(e, "[new Date('%04d-%02d-%02dT%02d:%02d:%02d'), %f]\n", t.YYYY, t.MM, t.DD, t.hh, t.mm, t.ss, s_temp[i]);
		}
		msg += "],\n";
		msg += "{\n";
		msg += "valueRange: [-10, 50]\n";
		msg += ",labels: ['x', 'Temp']\n";
		//msg += ",axes: { x: { axisLabelFormatter: function(d, gran, opts) { return Dygraph.dateAxisLabelFormatter(new Date(d.getTime()), gran, opts); }}\n";
		msg += "}\n";
		msg += ");\n";
		msg += "</script>\n";
		msg += "</body></html>";
		server.send(200, "text/html", msg);
	});
	server.begin();

	ntp.begin();

	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	ArduinoOTA.setHostname("ESP_OTA_LEO");

	// No authentication by default
	// ArduinoOTA.setPassword("admin");

	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
		else // U_SPIFFS
			type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});

	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});

	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR)
			Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR)
			Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR)
			Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR)
			Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR)
			Serial.println("End Failed");
	});
	ArduinoOTA.begin();
}

DateTime s_last;

void loop()
{
	server.handleClient();
	ntp.handle();
	ArduinoOTA.handle();

	if (ntp.valid() == false)
		return;

	delay(1000);

	sensors.requestTemperatures();
	while (sensors.isConversionComplete() == false)
		delay(10);

	s_last_temp = sensors.getTempCByIndex(0);
	s_last_time = ntp.toDateTime();

	blink(1);
	auto ss = s_last_time.toString();
	uprintf("%s T=%f\n", ss.c_str(), s_last_temp);

	// ogni 10 minuti
	if (s_last_time - s_last >= 60 * 10)
	{
		blink(10);

		s_last = s_last_time;

		if (s_ti >= int(sizeof(s_temp) / sizeof(s_temp[0])))
		{
			for (auto i = 1; i < int(sizeof(s_temp) / sizeof(s_temp[0])); ++i)
			{
				s_temp[i - 1] = s_temp[i];
				s_tm[i - 1] = s_tm[i];
			}
			s_ti--;
		}
		s_tm[s_ti] = s_last_time;
		s_temp[s_ti] = s_last_temp;
		s_ti += 1;
	}
}
