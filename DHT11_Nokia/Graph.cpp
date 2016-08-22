#include <Arduino.h>
#include "Graph.hpp"
#include <stdint.h>


void Graph::Plot(GraphSource &gs) const
{
	if (gs.Next())
	{
		Point a, b;
		gs.Get(b);
		while (gs.Next())
		{
			a = b;
			gs.Get(b);

			Line r;
			r.a = a;
			r.b = b;

			if (Clip(r))
			{
				Translate(r);
				gs.DrawLine(r);
			}
		}
	}
}

#define INSIDE 0
#define LEFT   1
#define RIGHT  2
#define BOTTOM 4
#define TOP    8
// Compute the bit code for a point (x, y) using the clip rectangle
// bounded diagonally by (xmin, ymin), and (xmax, ymax)

uint8_t Graph::ComputeOutCode(const Point &p) const
{
	uint8_t code = INSIDE;          // initialised as being inside of [[clip window]]

	if (p.x < _view->a.x)           // to the left of clip window
		code |= LEFT;
	else if (p.x > _view->b.x)      // to the right of clip window
		code |= RIGHT;
	if (p.y < _view->a.y)           // below the clip window
		code |= BOTTOM;
	else if (p.y > _view->b.y)      // above the clip window
		code |= TOP;

	return code;
}
bool Graph::Clip(Point &p) const
{
	uint8_t ca = ComputeOutCode(p);
	return ca == INSIDE;
}

// Cohen–Sutherland clipping algorithm clips a line from
// P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with 
// diagonal from (xmin, ymin) to (xmax, ymax).
bool Graph::Clip(Line &r) const
{
	// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle

	uint8_t ca = ComputeOutCode(r.a);
	uint8_t cb = ComputeOutCode(r.b);

	for (;;) 
	{
		if (!(ca | cb)) 
		{ 
			// Bitwise OR is 0. Trivially accept and get out of loop
			return true;
		}
		else if (ca & cb) 
		{ 
			// Bitwise AND is not 0. Trivially reject and get out of loop
			return false;
		} 
		else 
		{
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge

			// At least one endpoint is outside the clip rectangle; pick it.
			uint8_t c = ca ? ca : cb;

			// Now find the intersection point;
			// use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
			Point p;
			if (c & TOP) {           // point is above the clip rectangle
				p.x = r.a.x + (r.b.x - r.a.x) * (_view->b.y - r.a.y) / (r.b.y - r.a.y);
				p.y = _view->b.y;
			} else if (c & BOTTOM) { // point is below the clip rectangle
				p.x = r.a.x + (r.b.x - r.a.x) * (_view->a.y - r.a.y) / (r.b.y - r.a.y);
				p.y = _view->a.y;
			} else if (c & RIGHT) {  // point is to the right of clip rectangle
				p.y = r.a.y + (r.b.y - r.a.y) * (_view->b.x - r.a.x) / (r.b.x - r.a.x);
				p.x = _view->b.x;
			} else {   // point is to the left of clip rectangle
				p.y = r.a.y + (r.b.y - r.a.y) * (_view->a.x - r.a.x) / (r.b.x - r.a.x);
				p.x = _view->a.x;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (c == ca) {
				r.a = p;
				ca = ComputeOutCode(r.a);
			} else {
				r.b = p;
				cb = ComputeOutCode(r.b);
			}
		}
	}
}
