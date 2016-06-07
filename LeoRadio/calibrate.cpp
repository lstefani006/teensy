/*
 *
 *
 *   This file contains functions that implement calculations
 *    necessary to obtain calibration factors for a touch screen
 *    that suffers from multiple distortion effects: namely,
 *    translation, scaling and rotation.
 *
 *   The following set of equations represent a valid display
 *    point given a corresponding set of touch screen points:
 *
 *
 *                                              /-     -\
 *              /-    -\     /-            -\   |       |
 *              |      |     |              |   |   Xs  |
 *              |  Xd  |     | A    B    C  |   |       |
 *              |      |  =  |              | * |   Ys  |
 *              |  Yd  |     | D    E    F  |   |       |
 *              |      |     |              |   |   1   |
 *              \-    -/     \-            -/   |       |
 *                                              \-     -/
 *
 *
 *    where:
 *
 *           (Xd,Yd) represents the desired display point
 *                    coordinates,
 *
 *           (Xs,Ys) represents the available touch screen
 *                    coordinates, and the matrix
 *
 *           /-   -\
 *           |A,B,C|
 *           |D,E,F| represents the factors used to translate
 *           \-   -/  the available touch screen point values
 *                    into the corresponding display
 *                    coordinates.
 *
 *
 *    Note that for practical considerations, the utilitities
 *     within this file do not use the matrix coefficients as
 *     defined above, but instead use the following
 *     equivalents, since floating point math is not used:
 *
 *            A = An/Divider
 *            B = Bn/Divider
 *            C = Cn/Divider
 *            D = Dn/Divider
 *            E = En/Divider
 *            F = Fn/Divider
 *
 *
 *
 *    The functions provided within this file are:
 *
 *          setCalibrationMatrix() - calculates the set of factors
 *                                    in the above equation, given
 *                                    three sets of test points.
 *               getDisplayPoint() - returns the actual display
 *                                    coordinates, given a set of
 *                                    touch screen coordinates.
 * translateRawScreenCoordinates() - helper function to transform
 *                                    raw screen points into values
 *                                    scaled to the desired display
 *                                    resolution.
 *
 *
 */
#include "calibrate.h"

/**********************************************************************
 *
 *     Function: setCalibrationMatrix()
 *
 *  Description: Calling this function with valid input data
 *                in the display and screen input arguments
 *                causes the calibration factors between the
 *                screen and display points to be calculated,
 *                and the output argument - m - to be
 *                populated.
 *
 *               This function needs to be called only when new
 *                calibration factors are desired.
 *
 *
 *  Argument(s): display (input) - Pointer to an array of three
 *                                     sample, reference points.
 *               touch (input) - Pointer to the array of touch
 *                                    screen points corresponding
 *                                    to the reference display points.
 *               m (output) - Pointer to the calibration
 *                                     matrix computed for the set
 *                                     of points being provided.
 *
 *                 NOTE!    NOTE!    NOTE!
 *
 *  setCalibrationMatrix() and getDisplayPoint() will do fine
 *  for you as they are, provided that your digitizer
 *  resolution does not exceed 10 bits (1024 values).  Higher
 *  resolutions may cause the integer operations to overflow
 *  and return incorrect values.  If you wish to use these
 *  functions with digitizer resolutions of 12 bits (4096
 *  values) you will either have to a) use 64-bit signed
 *  integer variables and math, or b) judiciously modify the
 *  operations to scale results by a factor of 2 or even 4.
 *
 *
 */
bool setCalibrationMatrix(const POINT *display, const POINT *touch, MATRIX *m)
{
	m->Divider = ((touch[0].x - touch[2].x) * (touch[1].y - touch[2].y)) -
		((touch[1].x - touch[2].x) * (touch[0].y - touch[2].y)) ;

	if (m->Divider == 0)
		return false;

	m->An = ((display[0].x - display[2].x) * (touch[1].y - touch[2].y)) -
		((display[1].x - display[2].x) * (touch[0].y - touch[2].y)) ;

	m->Bn = ((touch[0].x - touch[2].x) * (display[1].x - display[2].x)) -
		((display[0].x - display[2].x) * (touch[1].x - touch[2].x)) ;

	m->Cn = (touch[2].x * display[1].x - touch[1].x * display[2].x) * touch[0].y +
		(touch[0].x * display[2].x - touch[2].x * display[0].x) * touch[1].y +
		(touch[1].x * display[0].x - touch[0].x * display[1].x) * touch[2].y ;

	m->Dn = ((display[0].y - display[2].y) * (touch[1].y - touch[2].y)) -
		((display[1].y - display[2].y) * (touch[0].y - touch[2].y)) ;

	m->En = ((touch[0].x - touch[2].x) * (display[1].y - display[2].y)) -
		((display[0].y - display[2].y) * (touch[1].x - touch[2].x)) ;

	m->Fn = (touch[2].x * display[1].y - touch[1].x * display[2].y) * touch[0].y +
		(touch[0].x * display[2].y - touch[2].x * display[0].y) * touch[1].y +
		(touch[1].x * display[0].y - touch[0].x * display[1].y) * touch[2].y ;

	return true;
}



/**********************************************************************
 *
 *     Function: getDisplayPoint()
 *
 *  Description: Given a valid set of calibration factors and a point
 *                value reported by the touch screen, this function
 *                calculates and returns the true (or closest to true)
 *                display point below the spot where the touch screen
 *                was touched.
 *
 *
 *
 *  Argument(s): display (output) - Pointer to the calculated
 *                                      (true) display point.
 *               touch (input) - Pointer to the reported touch
 *                                    screen point.
 *               m (input) - Pointer to calibration factors
 *                                    matrix previously calculated
 *                                    from a call to
 *                                    setCalibrationMatrix()
 *
 *
 *  The function simply solves for Xd and Yd by implementing the
 *   computations required by the translation matrix.
 *
 *                                              /-     -\
 *              /-    -\     /-            -\   |       |
 *              |      |     |              |   |   Xs  |
 *              |  Xd  |     | A    B    C  |   |       |
 *              |      |  =  |              | * |   Ys  |
 *              |  Yd  |     | D    E    F  |   |       |
 *              |      |     |              |   |   1   |
 *              \-    -/     \-            -/   |       |
 *                                              \-     -/
 *
 *  It must be kept brief to avoid consuming CPU cycles.
 *
 *
 *       Return: OK - the display point was correctly calculated
 *                     and its value is in the output argument.
 *               NOT_OK - an error was detected and the function
 *                         failed to return a valid point.
 *
 *
 *
 *                 NOTE!    NOTE!    NOTE!
 *
 *  setCalibrationMatrix() and getDisplayPoint() will do fine
 *  for you as they are, provided that your digitizer
 *  resolution does not exceed 10 bits (1024 values).  Higher
 *  resolutions may cause the integer operations to overflow
 *  and return incorrect values.  If you wish to use these
 *  functions with digitizer resolutions of 12 bits (4096
 *  values) you will either have to a) use 64-bit signed
 *  integer variables and math, or b) judiciously modify the
 *  operations to scale results by a factor of 2 or even 4.
 *
 *
 */
bool getDisplayPoint(POINT *display, const POINT *touch, const MATRIX *m)
{
	if (m->Divider == 0)
		return false;

	display->x = ((m->An * touch->x) + (m->Bn * touch->y) + m->Cn) / m->Divider;
	display->y = ((m->Dn * touch->x) + (m->En * touch->y) + m->Fn) / m->Divider;
	return true;
}

