#ifndef __uprintf_h__
#define __uprintf_h__

#include <stdarg.h>

int uprintf(bool (*)(char), const char *fmt, ...);

#ifdef ARDUINO
int uprintf(bool (*)(char), const __FlashStringHelper *fmt, ...);
#endif

int uvprintf(bool (*)(char), bool fmtFlash, const char *fmt, va_list vargs);

/////////////////////////////////////////
extern bool (*uprintf_cb)(char);

int uprintf(const char *fmt, ...);

#ifdef ARDUINO
int uprintf(const __FlashStringHelper *fmt, ...);
#endif

int uvprintf(bool fmtFlash, const char *fmt, va_list vargs);

#endif
