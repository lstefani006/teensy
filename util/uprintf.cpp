#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#ifdef ARDUINO
#include <Arduino.h>
#else
inline uint8_t pgm_read_byte(const char *s) { return *s; }
inline unsigned long pgm_read_dword(const unsigned long *s) { return *s; }
inline size_t strlen_P(const char *s) { return strlen(s); }
#define PROGMEM
#endif

#include "uprintf.hpp"

int ulong_to_10a(unsigned long v, char *b, bool neg)
{
	static const unsigned long dec[] PROGMEM = {
		1000000000UL,
		100000000UL,
		10000000UL,
		1000000UL,
		100000UL,
		10000UL,
		1000UL,
		100UL,
		10UL,
		1UL,
	};

	char *p = b;

	if (v == 0) 
	{
		*p++ = '0';
	}
	else
	{
		if (neg)
			*p++ = '-';

		const unsigned long *ks = &dec[0];

		while (v < pgm_read_dword(ks))
			ks++;

		do
		{
			char c = '0';
			auto t = pgm_read_dword(ks);
			while (v >= t)
			{
				v -= t;
				++c;
			}
			*p++ = c;
		}
		while (++ks != &dec[sizeof(dec)/sizeof(dec[0])]);
	}
	*p = 0;
	return p-b;
}

static inline int ulong_to_a(unsigned long v, uint8_t base, char *p, bool neg)
{
	char *b = p;
	do
	{
		char r = char(v % base);
		v /= base;
		*p++ = char(r + (r < 10 ? '0' : 'A' - 10));
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
static inline int numeric_to_asc(unsigned long v, uint8_t base, char *p) { return ulong_to_a(v, base, p, false); }

static inline int numeric_to_asc(long v, uint8_t base, char *p) 
{
	bool neg = false;
	if (v < 0) { neg = true; v = -v; }
	return ulong_to_a(v, base, p, neg); 
}


static inline int float_to_asc(float f, char *p, int prec)
{
	const char *b = p;

	if (f < 0)
	{
		f = -f;
		*p++ = '-';
	}

	long nf = (long)f;
	p += numeric_to_asc(nf, 10, p);

	if (prec > 0)
	{
		f -= float(nf);
		*p++ = '.';

		while (prec > 0)
		{
			f *= 10;
			nf = (long)f;
			f -= float(nf);

			*p++ = char(char(nf) + '0');
			--prec;
		}
	}
	*p = 0;

	return p-b;
}

template<typename T> class Yield {
protected:
	Yield() { __line__ = 0; }
	void Init() { __line__ = 0; }
	unsigned short __line__; 
	T __ret__;

public:
	const T & Current() const { return __ret__; }
};
template<> class Yield<void> {
protected:
	Yield() { Init(); }
	void Init() { __line__ = 0; }
	unsigned short __line__; 
};
#define Y_BEGIN()         switch (this->__line__) { case 0:
#define Y_YIELD0()                               this->__line__ = __LINE__; return true; case __LINE__:
#define Y_YIELD1(__r__)   this->__ret__ = __r__; this->__line__ = __LINE__; return true; case __LINE__:
#define Y_END()           } this->__line__ = 0; return false

class binFormatter : public Yield<char>
{
	typedef Yield<char> base;
public:
	void Init(unsigned long _v)
	{
		base::Init();
		this->k = 1UL << (sizeof(unsigned long)*8 - 1);
		this->v = _v;
		this->__ret__ = 0;
	}
	int8_t Len()
	{
		int8_t ret = 0;
		while (NumToBin())
			ret += 1;
		return ret;
	}
	char Next()
	{
		if (NumToBin())
			return Current();
		return 0;
	}

private:
	bool NumToBin()
	{
		Y_BEGIN();

		if (v == 0)
		{
			Y_YIELD1('0');
		}
		else
		{
			while ((v & k) == 0)
				k = k >> 1;

			while (k)
			{
				Y_YIELD1('0' + ((v & k) ? 1 : 0));
				k = k >> 1;
			}
		}
		Y_YIELD1(0);
		Y_END();
	}

	unsigned long v;
	unsigned long k;
};

/////////////////////////////////////////

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
			binFormatter bf;
			switch (fmt)
			{
			case 0:
				return -1;

			case 'd':
				{
					long v;
					if (!opz_l)
						v = va_arg(args, int);
					else
						v = va_arg(args, long);
					s = b;
					n = numeric_to_asc(v, 10, b);
				}
				break;

			case 'u':
			case 'o':
			case 'x':
				{
					uint8_t base;
					if (fmt == 'x') base = 16;
					else if (fmt == 'o') base = 8;
					else base = 10;

					unsigned long v;
					if (!opz_l)
						v = va_arg(args, unsigned int);
					else
						v = va_arg(args, unsigned long);
					s = b;
					n = numeric_to_asc(v, base, b);
				}
				break;

			case 'b':
				{
					unsigned long v;
					if (!opz_l)
						v = va_arg(args, unsigned int);
					else
						v = va_arg(args, unsigned long);
					bf.Init(v);
					n = bf.Len();
					bf.Init(v);
				}
				break;

			case 'p':
				{
					unsigned long v = (unsigned long)va_arg(args, void *);
					s = b;
					n = numeric_to_asc(v, 16, b);
				}
				break;
			case 'c':
				{
					int v = va_arg(args, int);
					b[0] = char(v);
					b[1] = 0;
					s = b;
					n = 1;
				}
				break;

			case 's':
				{
					s = va_arg(args, const char *);
					n = (int)strlen(s);
				}
				break;

			case 'S':
				{
					f = va_arg(args, const char *);
					n = (int)strlen_P(s);
				}
				break;

			case 'f':
				{
					double v = va_arg(args, double);
					s = b;
					n = float_to_asc(v, b, opz_prec);
				}
				break;

			default:
				return -1;
			}

			int a = opz_wr-n;
			if (a > 0) { if (!align(pf, opz_align_char, a)) return -1; else ret += a; }
			for (;;)
			{
				char cc;
				/***/if (s) cc = *s++ ;
				else if (f) cc = pgm_read_byte(f++);
				else cc = bf.Next();

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

int main()
{
	int r;

	//printf("%d\n", sizeof(int));
	//printf("%d\n", sizeof(long));

	r = uprintf(pf2, "%b\n", 18);
	if (false)
	{
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

		uprintf(pf2, "$$$%b\n", 17+2);

		char b[100];
		ulong_to_10a(153, b, false);
		printf("%s\n", b);
		for (long i = 0; i < 1000*1000*20; ++i)
		{
			//numeric_to_asc(i, 10, b);
			ulong_to_10a(i, b, false);
			//printf("%s\n", b);
		}
		printf("%s\n", b);
	}

	return 0;
}

#endif
