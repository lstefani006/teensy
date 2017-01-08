#include <stdint.h>
#include "AffineMatrix.hpp"

struct TwoPoint  { Point a, b; };
struct Line : public TwoPoint {};
struct Rect : public TwoPoint {};

struct GraphSource
{
	virtual bool Next() = 0;
	virtual void Get(Point &) = 0;
	virtual void Reset() = 0;
	virtual void DrawLine(const Line &) = 0;
};


class Graph
{
public:
	Graph() : _view(nullptr) {}
	Graph(const Rect *view, const Rect *screen) : _view(view) 
	{
		SetViewScreen(view, screen);
	}

	void SetViewScreen(const Rect *view, const Rect *screen)
	{
		_view = view;

		m.Identity();
		m.Translate(-view->a.x, -view->a.y);
		m.Scale(1 / (view->b.x - view->a.x), 1 / (view->b.y - view->a.y));
		m.Scale(screen->b.x - screen->a.x, screen->b.y - screen->a.y);
		m.Translate(screen->a.x, screen->a.y);
		m.ReflectX();
		m.Translate(0, screen->b.y - screen->a.y);
	}

	void Plot(GraphSource &gs) const;

	void Translate(Line &a) const
	{
		Translate(a.a);
		Translate(a.b);
	}
	void Translate(Point &a) const
	{
		a = m * a;
	}

	bool Clip(Line &r) const;
	bool Clip(Point &r) const;

private:
	uint8_t ComputeOutCode(const Point &p) const;

	AffineMatrix m;
	const Rect *_view;
};
