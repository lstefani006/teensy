#include <Arduino.h>
#include <errno.h>
#include <sys/stat.h>

#include "t_io.h"

static Print *s_Out = nullptr;
static Stream *s_In = nullptr;

namespace t
{
	void SetPrint(Print *p)
	{
		s_Out = p;
	}
	void SetStream(Stream *p)
	{
		s_Out = p;
		s_In = p;
	}
}

extern "C" {

	int _write(int fd, char *ptr, int len) 
	{
		if (((fd == 1 || fd == 2) && s_Out != nullptr) || 
				(Print *)fd == s_Out) 
		{
			for (int i = 0; i < len; ++i) {
				uint8_t ch = ((uint8_t*)ptr)[i];
				s_Out->write(ch);
				if (ch == '\n')
					s_Out->write('\r');
			}
			return len;
		}

		errno = EBADF;
		return -1;
	}
	int _read(int fd, char *ptr, int len) 
	{
		if (fd == 0) 
		{
			// il primo carattere deve arrivare
			int c = 0;
			while (c < len) {
				while (!s_In->available());
				int rx = s_In->read();
				if (rx < 0) break;
				if (rx == 8) {
					if (c > 0) {
						s_In->write(rx);
						s_In->write(' ');
						s_In->write(rx);
						c--;
					}
				}
				else if (rx == '\r') {
					ptr[c++] = '\n';
					s_In->write('\n');
					s_In->write('\r');
					break;
				} else {
					ptr[c++] = (char)rx;
					s_In->write(rx);
				}
			}
			return c;
		}
		else {
			errno = EBADF;
			return -1;
		}
	}

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
