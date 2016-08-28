#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#ifdef ARDUINO
#include <Arduino.h>
#else
inline uint8_t pgm_read_byte(const char *s) { return *s; }
inline size_t strlen_P(const char *s) { return strlen(s); }
#endif

#include "uprintf.hpp"

static inline int ulong_to_a(unsigned long v, uint8_t base, char *p, bool neg)
{
	char *b = p;
	do
	{
		char r = char(v % base);
		v /= base;
		*p++ = r + (r < 10 ? '0' : 'A' - 10);
	}
	while (v);

	if (neg)
		*p++ = '-';

	*p = 0;

	int bi = 0;
	int be = p-b-1;
	while (bi < be)
	{
		char c = b[bi];
		b[bi] = b[be];
		b[be] = c;

		++bi;
		--be;
	}

	return p-b;
}
static inline int numeric_to_a(unsigned long v, uint8_t base, char *p) { return ulong_to_a(v, base, p, false); }

static inline int numeric_to_a(long v, uint8_t base, char *p) 
{
	bool neg = false;
	if (v < 0) { neg = true; v = -v; }
	return ulong_to_a(v, base, p, neg); 
}


static inline int float_to_a(float f, char *p, int8_t prec)
{
	const char *b = p;

	if (f < 0)
	{
		f = -f;
		*p++ = '-';
	}

	long nf = (long)f;
	p += numeric_to_a(nf, 10, p);

	if (prec > 0)
	{
		f -= nf;
		*p++ = '.';

		while (prec > 0)
		{
			f *= 10;
			nf = (long)f;
			f -= nf;

			*p++ = (nf + '0');
			--prec;
		}
	}
	*p = 0;

	return p-b;
}


typedef bool (*pfp)(char);

static inline bool align(pfp pf, char ch, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (!pf(ch))
			return false;
	}
	return true;
}

static inline bool pf_dummy(char) { return true; }


int uvprintf(pfp pf, bool fmtFlash, const char *ffmt, va_list vargs)
{
	char b[20];
	va_list args;
	va_copy(args, vargs);

	int ret = 0;

	if (!pf) pf = pf_dummy;


	for (;;)
	{
#define rfmt(ss) (fmtFlash ? pgm_read_byte(ss) : *ss)
		char fmt = rfmt(ffmt++);
		if (!fmt)
			break;

		if (fmt == '%')
		{
			bool opz_l = false;
			int  opz_wl = 0;
			int  opz_wr = 0;
			char opz_align_char = ' ';
			bool opz_align_left = false;
			int  opz_prec = 2;

			// [flags][width][.precision][length]specifier
			// flags      ==> - 0
			// width      ==> (0-9)+
			// .precision ==> .(0-9)+
			// lenght     ==> l
			// specificer ==> s d x f

			fmt = rfmt(ffmt++);

			while (fmt == '-' || fmt == '0')
			{
				if (fmt == '-')
					opz_align_left = true;
				else
					opz_align_char = '0';
				fmt = rfmt(ffmt++);
			}


			while (fmt >= '0' && fmt <= '9')
			{
				opz_wr = opz_wr * 10 + (fmt - '0');
				fmt = rfmt(ffmt++);
			}
			if (opz_align_left) { opz_wl = opz_wr; opz_wr = 0; }

			if (fmt=='.')
			{
				opz_prec = 0;
				fmt = rfmt(ffmt++);
				while (fmt >= '0' && fmt <= '9')
				{
					opz_prec = opz_prec * 10 + (fmt - '0');
					fmt = rfmt(ffmt++);
				}
			}
			if (fmt == 'l')
			{
				opz_l = true;
				fmt = rfmt(ffmt++);
			}

			int n=0;
			const char *s = nullptr;
			const char *f = nullptr;
			switch (fmt)
			{
			case 0:
				return -1;

			case 'd':
			case 'x':
				{
					int8_t base = (fmt == 'x')? 16 : 10;
					long v;
					if (!opz_l)
						v = va_arg(args, int);
					else
						v = va_arg(args, long);
					s = b;
					n = numeric_to_a(v, base, b);
				}
				break;

			case 'c':
				{
					int v = va_arg(args, int);
					b[0] = v;
					b[1] = 0;
					s = b;
					n = 1;
				}
				break;

			case 's':
			case 'S':
				{
					s = va_arg(args, const char *);
					if (!opz_l)
						n = (int)strlen(s);
					else
						n = (int)strlen_P(s);
				}
				break;

			case 'f':
				{
					double v = va_arg(args, double);
					s = b;
					n = float_to_a(v, b, opz_prec);
				}
				break;

			default:
				return -1;
			}

			int a = opz_wr-n;
			if (a > 0) { if (!align(pf, opz_align_char, a)) return -1; else ret += a; }
			for (;;)
			{
				char cc = s? *s++ : pgm_read_byte(f++);
				if (!cc) break;
				if (!pf(cc)) return -1;
			}
			ret += n;
			a = opz_wl-n;
			if (a > 0) { if (!align(pf, opz_align_char, a)) return -1; else ret += a; }
		}
		else
		{
			if (!pf(fmt)) return -1;
			ret += 1;
		}
	}

	va_end(args);
	return ret;
}

int uprintf(pfp pf, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int rc = uvprintf(pf, false, fmt, args);
	va_end (args);
	return rc;
}


/////////////////////////////////////////////////////////////////////////////////

#ifdef ARDUINO
int uprintf(pfp pf, const __FlashStringHelper *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int rc = uvprintf(pf, true, (const char *)fmt, args);
	va_end (args);
	return rc;
}
#endif
/////////////////////////////////////////////////////////////////////////////////
bool (*uprintf_cb)(char) = nullptr;

int uprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	auto rc = uvprintf(uprintf_cb, false, fmt, args);
	va_end (args);
	return rc;
}

#ifdef ARDUINO
int uprintf(const __FlashStringHelper *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	auto rc = uvprintf(uprintf_cb, true, (const char *)fmt, args);
	va_end (args);
	return rc;
}
#endif

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#ifndef ARDUINO

#include <stdio.h>

bool pf2(char s)
{
	printf("%c", s);
	return true;
}

main()
{
	int r;
	
	//printf("%d\n", sizeof(int));
	//printf("%d\n", sizeof(long));

	r = uprintf(pf2, "leo\n");
	printf("%d\n", r);
	r = uprintf(pf2, "leo #%6d#\n", -1234);
	printf("%d\n", r);
	r = uprintf(pf2, "leo #%-10d#\n", 1234);
	printf("%d\n", r);
	r = uprintf(pf2, "leo #%10.4f#\n", 33.5);
	printf("%d\n", r);
	r = uprintf(pf2, "leo #%-10.4f#\n", -3.5);
	printf("%d\n", r);
}

#endif
