#include <Arduino.h>
#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>

#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

void setup();
void loop();

int main()
{
	setup();
	for (;;)
		loop();
}

unsigned long millis(void)
{
	static unsigned long tzero = (unsigned long)-1;
	if (tzero == (unsigned long)-1)
	{
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts );
		tzero = (ts.tv_sec * 1000 + ts.tv_nsec / 1000000L);
	}

	if (1) {
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts );
		return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000L) - tzero;
	}
}
void delay(unsigned long msec) 
{ 
	usleep(msec * msec); 
}

///////////////////////////////////////////////////////////////////////////////////

SerialClass Serial;

void SerialClass::println(const char *str) { printf("%s\n", str); fflush(stdout); }
void SerialClass::print(const char *str) { printf("%s", str); fflush(stdout); }
void SerialClass::print(int n, PrintInt t) { 
	if (t == DEC) printf("%d", n); 
	else printf("%x", n);
	fflush(stdout); ;
}
void SerialClass::println(int n, PrintInt t) { 
	if (t == DEC) printf("%d\n", n); 
	else printf("%x\n", n);
	fflush(stdout); ;
}
void SerialClass::print(uint8_t ch) { printf("%X", ch); fflush(stdout); }
void SerialClass::print(char ch) { printf("%c", ch); fflush(stdout); }
void SerialClass::print(int ch) { printf("%d", ch); fflush(stdout); }
void SerialClass::print(uint32_t ch) { printf("%u", ch);fflush(stdout);  }

///////////////////////////////7

///////////////////////////////7
HardwareSerial Serial1;


/* Place terminal referred to by 'fd' in raw mode (noncanonical mode
 *    with all input and output processing disabled). Return 0 on success,
 *       or -1 on error. If 'prevTermios' is non-NULL, then use the buffer to
 *          which it points to return the previous terminal settings. */

int ttySetRaw(int fd, struct termios *prevTermios)
{
	struct termios t;

	if (tcgetattr(fd, &t) == -1)
		return -1;

	if (prevTermios != NULL)
		*prevTermios = t;

	t.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
	/* Noncanonical mode, disable signals, extended
	 *                            input processing, and echoing */

	t.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR |
			INPCK | ISTRIP | IXON | PARMRK);
	/* Disable special handling of CR, NL, and BREAK.
	 *                            No 8th-bit stripping or parity error handling.
	 *                                                       Disable START/STOP output flow control. */

	t.c_oflag &= ~OPOST;                /* Disable all output processing */

	t.c_cc[VMIN] = 1;                   /* Character-at-a-time input */
	t.c_cc[VTIME] = 0;                  /* with blocking */

	if (tcsetattr(fd, TCSAFLUSH, &t) == -1)
		return -1;

	return 0;
}
static int fd;

void HardwareSerial::begin(int)
{
	struct termios options;

	/* open the port */
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {perror("open"); exit(1);}
	fcntl(fd, F_SETFL, 0);

	memset(&options, 0, sizeof(options));

	int e;
	/* get the current options */
	e = tcgetattr(fd, &options);
	if (e < 0) { perror("tcgetattr"); exit(1); }
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	ttySetRaw(fd, &options);
};

bool HardwareSerial::available()
{
	int nb = 0;
	int e = ioctl(fd, FIONREAD, &nb);
	if (e < 0) { perror("ioctl"); exit(1); }
	return nb > 0;
}

void HardwareSerial::write(uint8_t b)
{
	write(&b, 1);
}
void HardwareSerial::write(const uint8_t *b, int sz)
{
	if (0) {
		printf("Tx %d(", sz);
		for (int i =0; i < sz; ++i) printf("%02X ", b[i]);
		printf(") ");
		fflush(stdout);
	}
	while (sz > 0)
	{
		int e = ::write(fd, b, sz);
		if (e < 0) { perror("write"); exit(1); }
		b += e;
		sz -= e;
	}
}

uint8_t HardwareSerial::read()
{
	uint8_t r;
	int e = ::read(fd, &r, 1);
	if (e < 0) { perror("read"); exit(1); }
			
	if (0) {
		if (e <= 0)
			printf("(Rx e=%d)", e);
		else
			printf("(Rx e=%d %02X)", e, r);
		fflush(stdout);
	}
	if (e == 0)
		return -1;

	return r;
}
