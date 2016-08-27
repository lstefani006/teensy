
#include <stdarg.h>

int uprintf(bool (*)(char), const char *fmt, ...);

#ifdef ARDUINO
int uprintf(bool (*)(char), const __FlashStringHelper *fmt, ...);
#endif

int uvprintf(bool (*)(char), bool fmtFlash, const char *fmt, va_list vargs);
