/*
 * AClip.cpp
 *
 *  Created on: Sep 4, 2014
 *      Author: alex
 */
#include "stdafx.h"
#include "AClip.h"
//-------------------------------------------
//
//-------------------------------------------
void AFill::Fill(MFifo<IPoint> &fifo, int _x, int _y, IRect &clip, BYTE mark)
{
	IPoint p;
	fifo.Push(IPoint(_x, _y));

	buff[_x + _y * width] = mark;
	while (!fifo.IsEmpty())
	{

		p = fifo.Pop();
		if (clip.PtInRegion(p.x + 1, p.y) && buff[p.x + 1  + p.y  * width]  == 255)
		{
				fifo.Push().Set(p.x + 1, p.y);
				buff[p.x + 1 + p.y * width] = mark;
		}
		if (clip.PtInRegion(p.x - 1, p.y) && buff[p.x - 1  + p.y * width]  == 255)
		{
				fifo.Push().Set(p.x - 1, p.y);
				buff[p.x - 1 + p.y * width] = mark;
		}
		if (clip.PtInRegion(p.x, p.y + 1) && buff[p.x  + (p.y + 1) * width]  == 255)
		{
				fifo.Push().Set(p.x, p.y + 1);
				buff[p.x + (p.y + 1)* width] = mark;
		}
		if (clip.PtInRegion(p.x, p.y - 1) && buff[p.x  + (p.y - 1) * width]  == 255)
		{
				fifo.Push().Set(p.x, p.y - 1);
				buff[p.x + (p.y - 1)* width] = mark;
		}

	}
}
//-------------------------------------------
//
//-------------------------------------------
void AFill::Polygon (IPoint *p, int num)
{
	int i, len = 0;
	IRect rc(0, 0, width, height);

	for(i = 1; i < num; ++i)
	{
		Line(p[i - 1].x, p[i - 1].y, p[i].x, p[i].y);
	}
	int j = rc.bottom - rc.top;
	int w = rc.right - rc.left;
	if (w && j)
	{

		BYTE *top = buff + width * rc.top + rc.left;
		MFifo<IPoint> fifo;
		BYTE *right = buff + width * rc.top + rc.right;
		BYTE *bottom = buff + width * rc.bottom + rc.left;
		for (i = 0; i < w; ++i)
		{
			if (bottom[i] == 255)
				Fill(fifo, rc.left + i, rc.bottom, rc);

			if (top[i] == 255)
				Fill(fifo, rc.left + i, rc.top, rc);
		}
		int k = j * width;
		for (i = 0; i < k; i += width)
		{
			if (top[i] == 255)
				Fill(fifo, rc.left, rc.top + i/width, rc);

			if (right[i] == 255)
				Fill(fifo, rc.right, rc.top + i/width, rc);

		}
		int y = j;
		while (j--)
		{
			for (i = 0; i < w; ++i)
				if (top[i] != 3)
					top[i] = markB;
			top += width;

		}

	}

}
//-------------------------------------------
//
//-------------------------------------------
void AFill::PolygonConvex(IPoint *p, int num, int mark)
{
	if (!num) return;
	int i, len = 0;
	IRect rc(0, 0, width, height);
	IPoint c = p[0];
	for(i = 1; i < num; ++i)
	{
		c += p[i];
		Line(p[i - 1].x, p[i - 1].y, p[i].x, p[i].y);
	}
	int j = rc.bottom - rc.top;
	int w = rc.right - rc.left;
	if (w && j)
	{
		c /= num;
		MFifo<IPoint> fifo;
		BYTE *top = buff + width * rc.top + rc.left;
		Fill(fifo, c.x, c.y, rc);
		int y = j;
		if (mark != markC)
		while (j--)
		{
			for (i = 0; i < w; ++i)
				if (top[i] == markC)
					top[i] = mark;
			top += width;
		}

	}

}

void AFill::Ellipse(IRect &rc)
{
	int dX = rc.right - rc.left;
	int dY = rc.bottom - rc.top;
	X = (rc.right + rc.left) >> 1;
	Y = (rc.bottom + rc.top) >> 1;

	dDx = (double)dX/4.0 * (double)dX/4.0;


	dX >>= 1;
	dY >>= 1;

	int xr2 = (dX * dX) << 1;
	int yr2 = (dY * dY) << 1;
	if (xr2	< 0 || yr2 < 0)
		return;
	int x = 0;
	int y = dY;
	int d = (yr2 - xr2 * y) + (xr2 >> 1);
	int xm = (int)(xr2/sqrt(double((xr2 + yr2) << 1)) - 1);
	if (max(X - xm, 0) < min(X + xm, width))
	{
		if (Y + y > 0 && Y - y < height && X + x > 0 && X - x < width)
			LineEllipse(x, y);
		while (x < xm)
		{
			if (y > 0)
			{
				if (d > 0)
				{
					--y;
					d -= xr2 * (y <<  1);
				}
			}
			d += yr2 * ((x << 1) + 3);
			++x;
			if (Y + y > 0 && Y - y < height && X + x > 0 && X - x < width)
				LineEllipse(x, y);
		}
	}
	d = (xr2 - yr2 * dX) + (yr2 >> 1);
	x = dX;
	y = 0;

	if (Y + y > 0 && Y - y < height && X + x > 0 && X - x < width)
		LineEllipse(x, y);


	xm += 2;

	if (max(X - x, 0) < min(X - xm, width) ||
		max(X + xm, 0) < min(X + x, width))
	{
		bool fD;
		while (x >= 0 && x >= xm)
		{
			if ((fD = d > 0))
			{
				--x;
				d -= yr2 * x * 2;
			}
			d += xr2 * ((y << 1) + 3);
			++y;
			if (fD && Y + y > 0 && Y - y < height && X + x > 0 && X - x < width)
				LineEllipse(x, y);
		}
	}
}


//-------------------------------------------
//
//-------------------------------------------
void AFill::LineEllipse(int x, int y)
{
	int x1, x2, y1, y2;
	bool fX1, fX2, fY1, fY2;
	double dx2 = (double)x * (double)x/dDx;
	BYTE *b1, *b2;
	x1 = X + x;
	x2 = X - x;
	fX1 = x1 >= 0 && x1 < width;
	fX2 = x2 >= 0 && x2 < width;
 	y2 = height - y - Y - 1;
	y1 = Y - y;
	if (y1 < 0 && y2 < 0)
		y += max(y1, y2);
	if (!y) y = 1;

	int Y1 = Y + y;
	int Y2 = Y - y;

	while (y--)
	{
		 y1 = Y + y;
		 y2 = Y - y;
		 fY1 = y1 >= 0 && y1 < height;
		 fY2 = y2 >= 0 && y2 < height;
		 if (!fY1 && !fY2)
				break;
		 if (fY1)
			b1 = buff + y1 * width;

		 if (fY2)
			b2 = buff + y2 * width;
		y2 *= width;
		if (fX1)
		{
			if (fY1)
				*(b1 + x1) = markB;
			if (y && fY2)
				*(b2 + x1) = markB;
		}
		if (fX2 && x1 != x2)
		{
			if (fY1)
				*(b1 + x2) = markB;
			if (y && fY2)
				*(b2 + x2) = markB;
		}
	}


}
//-------------------------------------------
//
//-------------------------------------------
void AFill::InitLine(int x1, int y1, int x2, int y2)
{
	if (y1 == y2)
		if (x1 > x2)
		{
			int n = x1;
			x1 = x2;
			x2 = n;
		}

	if (x1 == x2)
		if (y1 > y2)
		{
			int n = y1;
			y1 = y2;
			y2 = n;
		}


	sX = cX = x1;
	sY = cY = y1;

	XEnd = x2;
	YEnd = y2;
	Dx = x2 - x1;
	Dy = y2 - y1;
	if (Dx != 0)
	{
		dX = abs(Dx) / Dx;
		Dx = abs(Dx);
	}
	else
		dX = 0;
	Dx2 = Dx + Dx;
	if (Dy != 0)
	{
		dY = abs(Dy) / Dy;
		Dy = abs(Dy);
	}
	else
		dY = 0;
	Dy2 = Dy + Dy;
	if (x1 == x2)
		type = VERT;
	else
	if (y1 == y2)
		type = HORZ;
	else
	if (Dx >= Dy)
		type = NHORZ;
	else
		type = NVERT;

	if (Dx >= Dy)
	{
		Ey0 = Ey = 2 + Dy - Dx;
		_Next = &AFill::NextForX;
	}
	else
	{
		Ex0 = Ex = 2 + Dx - Dy;
		_Next = &AFill::NextForY;
	}
	cX = sX; cY = sY; Ex = Ex0; Ey = Ey0;
}
