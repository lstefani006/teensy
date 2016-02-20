#ifndef _CALIBRATE_H_
#define _CALIBRATE_H_

#include <stdint.h>

struct POINT 
{ 
	int32_t x;
	int32_t y; 
};

struct MATRIX 
{
	int32_t An;
	int32_t Bn;
	int32_t Cn;
	int32_t Dn;
	int32_t En;
	int32_t Fn;
	int32_t Divider;
};

bool setCalibrationMatrix(const POINT *display, const POINT *touch, MATRIX *matrix);
bool getDisplayPoint(POINT *display, const POINT *touch, const MATRIX *matrix);


#endif  /* _CALIBRATE_H_ */
