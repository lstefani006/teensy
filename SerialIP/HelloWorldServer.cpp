/*
 * SerialIP Hello World example.
 *
 * SerialIP is a TCP/IP stack that can be used over a serial port (a bit
 * like a dial-up Internet connection, but without the modem.)  It works with
 * stock Arduinos (no shields required.)  When attached to a PC supporting
 * SLIP, the Arduino can host network servers and access the Internet (if the
 * PC is configured to share its Internet connection of course!)
 *
 * SerialIP uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 * For more information see the SerialIP page on the Arduino wiki:
 *   <http://www.arduino.cc/playground/Code/SerialIP>
 *
 *      -----------------
 *
 * This Hello World example sets up a server at 192.168.5.2 on port 1000.
 * Telnet here to access the service.  The uIP stack will also respond to
 * pings to test if you have successfully established a SLIP connection to
 * the Arduino.
 *
 * SLIP connection set up under Linux:
 *
 *  # modprobe slip
 *  # slattach -L -s 115200 -p slip /dev/ttyUSB0     (see note below)
 *  # ifconfig sl0 192.168.5.1 dstaddr 192.168.5.2
 *
 *  # ping 192.168.5.2
 *  # telnet 192.168.5.2 1000
 *
 * Here 192.168.5.1 is the address you will give to your PC, and it must be
 * unique on your LAN.  192.168.5.2 is the IP you will give the Arduino.  It
 * must also be unique, and must match the address the Arduino is expecting
 * as set by the "myIP" variable below.
 *
 * Note that slattach won't return so you'll need to run ifconfig from
 * another terminal.  You can press Ctrl+C to kill slattach and release the
 * serial port, e.g. to upload another sketch.
 *
 * This example was based upon uIP hello-world by Adam Dunkels <adam@sics.se>
 * Ported to the Arduino IDE by Adam Nielsen <malvineous@shikadi.net>
 */

#include <SerialIP.h>
#include <ILI9341_t3.h>

// The connection_data struct needs to be defined in an external file.
#include "HelloWorldData.h"



// For the Adafruit shield, these are the default.
#define TFT_RESET 8
#define TFT_DC  9
#define TFT_CS 10
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_t3 tft(TFT_CS, TFT_DC, TFT_RESET);


void uip_callback(uip_tcp_appstate_t *s);
int handle_connection_server(uip_tcp_appstate_t *s, connection_data *d);
int handle_connection_client(uip_tcp_appstate_t *s, connection_data *d);

static int client = 0;

void setup() {
	tft.begin();
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextColor(ILI9341_YELLOW);
	tft.setTextSize(1);
	tft.println("Waiting for Arduino Serial Monitor...");

	// Set up the speed of our serial link.
	Serial.begin(115200);
	while (!Serial) ; // wait for Arduino Serial Monitor
	tft.fillScreen(ILI9341_BLACK);
	tft.setCursor(0, 0);
	tft.println("GOT IT");


	// Tell SerialIP which serial port to use (some boards have more than one.)
	// Currently this is limited to HardwareSerial ports, until both it and 
	// SoftwareSerial inherit from a common base class.
	//SerialIP.use_device(Serial);

	// We're going to be handling uIP events ourselves.  Perhaps one day a simpler
	// interface will be implemented for the Arduino IDE, but until then...  
	SerialIP.set_uip_callback(uip_callback);

	// Set the IP address we'll be using.  Make sure this doesn't conflict with
	// any IP addresses or subnets on your LAN or you won't be able to connect to
	// either the Arduino or your LAN...
	IP_ADDR myIP = {192,168,5,2};
	IP_ADDR subnet = {255,255,255,0};
	SerialIP.begin(myIP, subnet);

	// If you'll be making outgoing connections from the Arduino to the rest of
	// the world, you'll need a gateway set up.
	IP_ADDR gwIP = {192,168,5,1};
	SerialIP.set_gateway(gwIP);

	// Listen for incoming connections on TCP port 1000.  Each incoming
	// connection will result in the uip_callback() function being called.
	//SerialIP.listen(1000);

	delay(1000*20);
	tft.fillScreen(ILI9341_BLACK);
	tft.setCursor(0, 0);
	tft.println("START");

	client = 1;
}


void loop() {
	// Check the serial port and process any incoming data.
	SerialIP.tick();

	// We can do other things in the loop, but be aware that the loop will
	// briefly pause while IP data is being processed.
	if (client == 1)
	{
		client = 0;

		tft.fillScreen(ILI9341_BLACK);
		tft.setCursor(0,0);

		uip_ipaddr_t ipaddr;
		uip_ipaddr(&ipaddr, 192,168,5,1);

		if (uip_connect(&ipaddr, HTONS(9999)) == nullptr)
			tft.println("Cant allocate Conn");
		else
			tft.println("CONNECTING...");
	}
}

void flags()
{
	static int n = 0;
	if(uip_poll()) { n++; return; }
	tft.print(n++);
	if (uip_conn->lport == HTONS(1000)) tft.print(" S ");
	if (uip_conn->rport == HTONS(9999)) tft.print(" C ");
	if(uip_connected()) tft.println("uip_connected");
	if(uip_rexmit()) tft.println("uip_rexmit");
	if(uip_newdata()) tft.println("uip_newdata()");
	if(uip_acked()) tft.println("uip_acked()");
	if(uip_closed()) tft.println("uip_closed()");
	if(uip_aborted()) tft.println("uip_aborted()");
	if(uip_timedout()) tft.println("uip_timedout()");
	//if(uip_poll()) tft.println("uip_poll()");
}


void uip_callback(uip_tcp_appstate_t *s)
{
	flags();

	if (uip_connected()) {

		// We want to store some data in our connections, so allocate some space
		// for it.  The connection_data struct is defined in a separate .h file,
		// due to the way the Arduino IDE works.  (typedefs come after function
		// definitions.)
		connection_data *d = (connection_data *)malloc(sizeof(connection_data));

		// Save it as SerialIP user data so we can get to it later.
		s->user = d;

		// The protosocket's read functions need a per-connection buffer to store
		// data they read.  We've got some space for this in our connection_data
		// structure, so we'll tell uIP to use that.
		PSOCK_INIT(&s->p, d->input_buffer, sizeof(d->input_buffer));

	}

	// Call/resume our protosocket handler.
	if (uip_conn->lport == HTONS(1000))
		handle_connection_server(s, (connection_data *)s->user);
	else
		handle_connection_client(s, (connection_data *)s->user);

	// If the connection has been closed, release the data we allocated earlier.
	if (uip_closed()) 
	{
		if (s->user) free(s->user);
		s->user = nullptr;
	}
}

// This function is going to use uIP's protosockets to handle the connection.
// This means it must return int, because of the way the protosockets work.
// In a nutshell, when a PSOCK_* macro needs to wait for something, it will
// return from handle_connection so that other work can take place.  When the
// event is triggered, uip_callback() will call this function again and the
// switch() statement (see below) will take care of resuming execution where
// it left off.  It *looks* like this function runs from start to finish, but
// that's just an illusion to make it easier to code :-)
int handle_connection_server(uip_tcp_appstate_t *s, connection_data *d)
{
	PSOCK_BEGIN(&s->p);

	PSOCK_SEND_STR(&s->p, "Hello. What is your name?\n");

	PSOCK_READTO(&s->p, '\n');

	// Save data read
	strncpy(d->name, d->input_buffer, sizeof(d->name));

	// Send some more data over the connection.
	PSOCK_SEND_STR(&s->p, "Hello ");
	PSOCK_SEND_STR(&s->p, d->name);

	// Disconnect.
	PSOCK_CLOSE(&s->p);
	client = 1;
	tft.println("CLIENT=1");

	// All protosockets must end with this macro.  It closes the switch().
	PSOCK_END(&s->p);
}


uint16_t imgIndex = 7;
uint8_t bbb[320*2 + 32];
int bbbi = 0;
int ww;
int hh;
int yy = 0;
int xx = 0;

int b2i(const uint8_t *b) { return (b[0] << 0) | (b[1] << 8); }

int handle_connection_client(uip_tcp_appstate_t *s, connection_data *d)
{
	PSOCK_BEGIN(&s->p);

	/*
	   PSOCK_READTO(&s->p, '\n');
	   d->input_buffer[PSOCK_DATALEN(&s->p)] = 0;
	   tft.println(d->input_buffer);
	   */


	tft.println("Inizio 1");
	if (1)
	{
		static uint8_t b[2];
		b[0] = imgIndex & 0xff;
		b[1] = imgIndex >> 8;
		PSOCK_SEND(&s->p, (char *)b, 2);
		imgIndex += 1;
	}
	tft.println("Inizio 2");

	bbbi = 0;
	while (bbbi < 4)
	{
		tft.println("Inizio 3");
		PSOCK_READBUF_LEN(&s->p, 4);
		//static int sz = PSOCK_DATALEN(&s->p);
		static int sz = 4;
		memcpy(bbb + bbbi, d->input_buffer, sz);
		bbbi += sz;
	}
	tft.print("Inizio 4 - bbbi="); tft.println(bbbi);
	//delay(10*1000);
	ww = b2i(bbb + 0);
	hh = b2i(bbb + 2);
	bbbi -= 4;
	if (bbbi > 0) memmove(bbb, bbb + 4, bbbi);

	tft.println("Inizio 5");
	if (ww != 320 || hh != 240)
	{
		tft.println("Error");
	}
	else
	{
		tft.fillScreen(ILI9341_BLACK);
		tft.setCursor(0,0);
		tft.println("LETTURA IMMAGINE");
		tft.print("Buffer - bbbi="); tft.println(bbbi);
		for (yy = 0; yy < hh; yy++)
		{
			bbbi = 0;
			while (bbbi < ww*2)
			{
				static int sz = 2;
				PSOCK_READBUF_LEN(&s->p, sz);
				//static int sz = PSOCK_DATALEN(&s->p);
				memcpy(bbb + bbbi, d->input_buffer, sz);
				bbbi += sz;
			}
			if (yy < 2) {
				tft.print("FINITO - bbbi="); tft.println(bbbi);
			}

			static uint8_t ck = 0;
			for (xx = 0; xx < ww; ++xx)
			{
				ck ^= bbb[xx*2+0];
				ck ^= bbb[xx*2+1];
				int col = b2i(bbb + xx*2);
				tft.drawPixel(xx, yy + 100, col);
			}

			if (1)
			{
				PSOCK_SEND(&s->p, (char *)&ck, 1);
			}
			if (1)
			{
				static uint8_t b[2];
				b[0] = yy & 0xff;
				b[1] = yy >> 8;
				PSOCK_SEND(&s->p, (char *)b, 2);
			}
		}

		/*
		// Send some text over the connection.
		PSOCK_SEND_STR(&s->p, "Ci SONO!!");
		tft.println("dopo SEND");
		*/
	}

	PSOCK_CLOSE(&s->p);
	tft.println("dopo CLOSE");
	PSOCK_EXIT(&s->p);
	tft.println("dopo EXIT");

	PSOCK_END(&s->p);
}
