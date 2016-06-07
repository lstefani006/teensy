
#include "timer.hpp"

Timer2 timer2;

ISR(TIMER2_OVF_vect)
{
	TCNT2 = timer2._cnt;
	timer2_tick();
}
