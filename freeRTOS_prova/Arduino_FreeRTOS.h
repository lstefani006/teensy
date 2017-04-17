#define configUSE_MALLOC_FAILED_HOOK 1

#include <avr/io.h>
#include <avr/wdt.h>

#include <FreeRTOS.h>

#ifdef __cplusplus
extern "C"
{
#endif
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
#ifdef __cplusplus
}
#endif
