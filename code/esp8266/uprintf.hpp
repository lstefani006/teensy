#ifndef __uprintf_h__
#define __uprintf_h__

#include <stdarg.h>

struct upf_t
{
	bool (*pf)(char ch, void *ag);
	void *ag;
};

int uvprintf(upf_t cb, bool fmtFlash, const char *fmt, va_list vargs);
int uprintf(upf_t cb, const char *fmt, ...);
#ifdef ARDUINO
int uprintf(upf_t cb, const __FlashStringHelper *fmt, ...);
#endif

/////////////////////////////////////////
extern upf_t uprintf_cb;

int uprintf(const char *fmt, ...);
int usprintf(int sz, char *b, const char *fmt, ...);

#ifdef ARDUINO
int usprintf(int sz, char *b, const __FlashStringHelper *fmt, ...);
int uprintf(const __FlashStringHelper *fmt, ...);
#endif

//int uvprintf(bool fmtFlash, const char *fmt, va_list vargs);

#endif
