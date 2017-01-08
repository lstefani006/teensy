#include <Arduino.h>

class Point
{
public:
	float x, y;

	void Dump(Print &p) const;
};

class AffineMatrix
{
public:
	void Identity();

	float b[2][3];

	void Translate(float x, float y);
	void Rotate(float a);
	void Scale(float a, float b);

	void ReflectX() { Scale(1, -1); }
	void ReflectY() { Scale(-1, 1); }
	void ReflectO() { Scale(-1, -1); }


	void Dump(Print &p) const;
};

AffineMatrix operator * (const AffineMatrix &a, const AffineMatrix &b);
Point        operator * (const AffineMatrix &a, const Point &b);
