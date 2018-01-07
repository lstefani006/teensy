#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <DallasTemperature.h>
#include <uprintf.hpp>
#include <ArduinoOTA.h>
#include <DateTime.hpp>
#include <ESP8266mDNS.h>
#include <NtpTime.hpp>

#include <FS.h>

#include "leo.hpp"

ESP8266WebServer server(8123);
OneWire oneWire(13);
DallasTemperature sensors(&oneWire);
NtpTime ntp;
float s_last_temp;
DateTime s_last_time;

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

	if (false)
	{
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
				Serial << (i + 1) << ": " << WiFi.SSID(i) << " (" << WiFi.RSSI(i) << ")"
					   << ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*") << "\n";
				delay(10);
			}
		}
		blink(3);
	}
	Serial.println();
	Serial.println("Connecting");

	WiFi.setAutoConnect(true);
	WiFi.mode(WIFI_STA);
	WiFi.begin("Vodafone-11394415", "agyeiickkqbduukzzjpisbou");

	for (auto i = 0;; ++i)
	{
		auto st = WiFi.status();
		if (st == WL_CONNECTED)
			break;
		Serial << i << " => " << print_statuus(st) << "\n";
		delay(500);
	}

	Serial.println();
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	blink(10);

	// Initialize file system.
	if (!SPIFFS.begin())
		Serial.println("Failed to mount file system");
	else
	{
		auto f = SPIFFS.open("/leo.txt", "r");
		if (!f)
			Serial << "cannot open leo.txt\n";
		f.close();

		FSInfo fs_info;
		SPIFFS.info(fs_info);

#define A(a)          \
	Serial.print(#a); \
	Serial.println(a)
		A(fs_info.usedBytes);
		A(fs_info.totalBytes);
	}

	sensors.setCheckForConversion(false);
	sensors.begin();

	server.on("/", []() {

		if (true)
		{
			char b[50];

			server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
			server.sendHeader("Pragma", "no-cache");
			server.sendHeader("Expires", "-1");
			server.setContentLength(CONTENT_LENGTH_UNKNOWN);
			server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.

			server.sendContent_P(PSTR("<html>\n"));

			server.sendContent_P(PSTR("<head>\n"));
			server.sendContent("<script src='//cdnjs.cloudflare.com/ajax/libs/dygraph/2.1.0/dygraph.min.js'></script>\n");
			server.sendContent("<link rel='stylesheet' src='//cdnjs.cloudflare.com/ajax/libs/dygraph/2.1.0/dygraph.min.css' />\n");
			server.sendContent("</head>\n");

			server.sendContent("<body>\n");
			server.sendContent("<h0>Temperature</h0>\n");
			server.sendContent("<br/>\n");

			usprintf(50, b, "<span>Ora = %s</span><br/>\n", s_last_time.toString().c_str());
			server.sendContent(b);
			usprintf(50, b, "<span>Temp = %f</span><br/>\n", s_last_temp);
			server.sendContent(b);
			server.sendContent("<br/>\n");
			server.sendContent("<br/>\n");
			server.sendContent("<div style='width:600px; height:400px;' id='graphdiv'></div>\n");
			server.sendContent("<script type='text/javascript'>\n");
			server.sendContent("g = new Dygraph(\n");
			server.sendContent("document.getElementById('graphdiv'),\n");
			server.sendContent("[\n");

			float tMin = 100;
			float tMax = -100;
			if (true)
			{
				auto f = SPIFFS.open("/leo.txt", "r");

				// un campione al minuto 60 * 24 ==> 1 giorno
				if (f)
				{
					const char *pp = ",[new Date('2018-01-06T13:45:13'), 14.81]\n";
					int nrighe = f.size() / strlen(pp);
					if (nrighe >= 24 * 60)
						nrighe -= 24 * 60;

					bool first = true;
					while (f.available())
					{
						auto ss = f.readStringUntil('\n');
						if (ss[0] == '#')
							continue;

						if (nrighe > 0)
							nrighe -= 1;
						else
						{
							pp = strchr(ss.c_str(), ')') + 2;
							auto t = atof(pp);
							if (t > tMax)
								tMax = t;
							if (t < tMin)
								tMin = t;

							server.sendContent(!first ? "," : " ");
							server.sendContent(ss);
							server.sendContent("\n");
							first = false;
						}
					}
					f.close();
				}
			}
			server.sendContent("],\n");
			server.sendContent("{\n");
			server.sendContent("title: 'Temperatura Casa'\n");
			server.sendContent(",showRoller: true\n");

			usprintf(50, b, ",valueRange: [%d, %d]\n", int(tMin - 1), int(tMax + 1));
			server.sendContent(b);
			//server.sendContent(",valueRange: [-10, 50]\n");

			server.sendContent(",labels: ['x', 'Temp']\n");
			//msg += ",axes: { x: { axisLabelFormatter: function(d, gran, opts) { return Dygraph.dateAxisLabelFormatter(new Date(d.getTime()), gran, opts); }}\n";
			server.sendContent("}\n");
			server.sendContent(");\n");
			server.sendContent("</script>\n");

			server.sendContent("<span>");
			usprintf(50, b, ",%f %f", tMin, tMax);
			server.sendContent(b);
			server.sendContent("</span>");

			server.sendContent("</body>\n");
			server.sendContent("</html>\n");
			server.client().stop();
		}
		else
		{
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
			msg += "<br/>";
			msg += "<br/>";
			msg += "<div style='width:600px; height:400px;' id='graphdiv'></div>";
			msg += "<script type='text/javascript'>\n";
			msg += "g = new Dygraph(\n";
			msg += "document.getElementById('graphdiv'),\n";

			msg += "[\n";
			{
				auto f = SPIFFS.open("/leo.txt", "r");
				if (f)
				{
					bool first = true;
					while (f.available())
					{
						auto ss = f.readStringUntil('\n');
						if (ss[0] == '#')
							continue;
						msg += !first ? "," : " ";
						msg += ss;
						msg += "\n";
						first = false;
					}
					f.close();
				}
			}
			msg += "\n";
			msg += "],\n";
			msg += "{\n";
			msg += "title: 'Temperatura Casa'\n";
			msg += ",showRoller: true\n";
			msg += ",valueRange: [-10, 50]\n";
			msg += ",labels: ['x', 'Temp']\n";
			//msg += ",axes: { x: { axisLabelFormatter: function(d, gran, opts) { return Dygraph.dateAxisLabelFormatter(new Date(d.getTime()), gran, opts); }}\n";
			msg += "}\n";
			msg += ");\n";
			msg += "</script>\n";
			msg += "</body></html>";
			server.send(200, "text/html", msg);
		}
	});
	server.begin();

	ntp.begin();

	// Port defaults to 8266
	ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	ArduinoOTA.setHostname("ESP_OTA_LEO");

	// No authentication by default
	ArduinoOTA.setPassword("spectrum");

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
		SPIFFS.end();
	});

	ArduinoOTA.onEnd([]() {
		SPIFFS.begin();
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

	auto time = s_last_time.toTS();
	auto temp = s_last_temp;

	static DateTime s_last;

	// ogni minuto
	if (s_last_time - s_last >= 60)
	{
		s_last = s_last_time;

		blink(2);

		auto f = SPIFFS.open("/leo.txt", "a");
		if (f)
		{
			char bb[64];
			bb[0] = 0;
			usprintf(64, bb, F("[new Date('%04d-%02d-%02dT%02d:%02d:%02d'), %f]"), time.YYYY, time.MM, time.DD, time.hh, time.mm, time.ss, temp);

			f.println(bb);
			f.close();
		}
	}
}
