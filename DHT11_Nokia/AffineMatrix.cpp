#include "AffineMatrix.hpp"

void AffineMatrix::Identity()
{
	b[0][0] = 1;
	b[0][1] = 0;
	b[0][2] = 0;

	b[1][0] = 0;
	b[1][1] = 1;
	b[1][2] = 0;
}

void AffineMatrix::Dump(Print &p) const
{
	for (int8_t r = 0; r < 2; ++r)
	{
		for (int8_t j = 0; j < 3; ++j)
		{
			p.print(b[r][j]);
			p.print(' ');
		}
		p.println();
	}
}
void Point::Dump(Print &p) const
{
	p.print(x);
	p.print(' ');
	p.println(y);
}

AffineMatrix operator * (const AffineMatrix &a, const AffineMatrix &b)
{
	AffineMatrix m;

	for (int8_t r = 0; r < 2; ++r)
		for (int8_t c = 0; c < 3; ++c)
		{
			m.b[r][c]  = a.b[r][0] * b.b[0][c];
			m.b[r][c] += a.b[r][1] * b.b[1][c];
			if (c < 2) continue;
			m.b[r][c] += a.b[r][2];
		}
	return m;
}

Point operator * (const AffineMatrix &a, const Point &b)
{
	Point r;

	r.x  = a.b[0][0] * b.x;
	r.x += a.b[0][1] * b.y;
	r.x += a.b[0][2];

	r.y  = a.b[1][0] * b.x;
	r.y += a.b[1][1] * b.y;
	r.y += a.b[1][2];

	return r;
}

void AffineMatrix::Translate(float x, float y)
{
	AffineMatrix r;
	r.Identity();

	r.b[0][2] = x;
	r.b[1][2] = y;

	*this = r * *this;
}

void AffineMatrix::Rotate(float ang)
{
	float s = sin(ang);
	float c = cos(ang);

	AffineMatrix r;
	r.Identity();

	r.b[0][0] =  c; r.b[0][1] = s;
	r.b[1][0] = -s; r.b[1][1] = c;

	*this = r * *this;
}

void AffineMatrix::Scale(float ra, float rb)
{
	AffineMatrix r;
	r.Identity();

	r.b[0][0] = ra; 
	r.b[1][1] = rb;

	*this = r * *this;
}

#if 1

struct Vector { float b[3]; };
struct Matrix { float b[3][3]; };

void Identity(Matrix &m)
{
	m.b[0][0] = 1;
	m.b[0][1] = 0;
	m.b[0][2] = 0;

	m.b[1][0] = 0;
	m.b[1][1] = 1;
	m.b[1][2] = 0;

	m.b[2][0] = 0;
	m.b[2][1] = 0;
	m.b[2][2] = 1;
}

void Mul(const Matrix &a, const Vector &b, Vector &r)
{
	for (int8_t i = 0; i < 3; ++i)
	{
		r.b[i] = 0;
		for (int8_t k = 0; k < 3; ++k)
			r.b[i] += a.b[i][k] * b.b[k];

	}
}

void Mul(const Matrix &a, const Matrix &b, Matrix &m)
{
	for (int8_t r = 0; r < 3; ++r)
		for (int8_t c = 0; c < 3; ++c)
		{
			m.b[r][c] = 0;
			for (int8_t k = 0; k < 3; ++k)
				m.b[r][c] += a.b[r][k] * b.b[k][c];
		}
}

#endif
