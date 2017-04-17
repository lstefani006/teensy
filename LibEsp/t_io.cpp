#include <Arduino.h>
#include <errno.h>
#include <stdio.h>

#include "t_io.h"

static Print *s_Out = nullptr;
static Stream *s_In = nullptr;

static FILE fi;
static FILE fo;

static int s_putchar(char c, FILE *)
{
	if (s_Out)
	{
		if (c == '\n') s_Out->write('\r');
		s_Out->write(c);
	}
	return 0;
}
static int s_getchar(FILE *)
{
	if (!s_In) return -1;
	while (!s_In->available());
	return s_In->read();
}

namespace t
{
	void SetPrint(Print *p)
	{
		s_Out = p;

		memset(&fi, 0, sizeof(fi));

#if 0
		// fill in the UART file descriptor with pointer to writer.
		fdev_setup_stream (&fo, s_putchar, NULL, _FDEV_SETUP_WRITE);
		stdout = &fo;
#endif
	}
	void SetStream(Stream *p)
	{
		s_Out = p;
		s_In = p;

		memset(&fi, 0, sizeof(fi));
		memset(&fo, 0, sizeof(fo));

#if 0
		fdev_setup_stream (&fo, s_putchar, NULL, _FDEV_SETUP_WRITE);
		fdev_setup_stream (&fi, NULL, s_getchar, _FDEV_SETUP_READ);

		stdin  = &fi;
		stdout = &fo;
#endif
	}
}

/*
extern "C" {

	extern char *__brkval;

	void * _sbrk(int incr)
	{
		char t = 0;
		char *prev = __brkval;
		if (prev + incr + 128 > &t) {
			return (void *)-1;
		}
		__brkval += incr;
		return prev;
	}
}
*/
