#ifndef __leo_hpp__
#define __leo_hpp__

#include <Arduino.h>

template<class T>
inline Stream & operator << (Stream &s, T m)
{
	s.print(m);
	return s;
}


#endif //__leo_hpp__
