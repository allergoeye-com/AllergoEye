/*
 * EmbGDISurf.h
 *
 *  Created on: Sep 7, 2014
 *      Author: alex
 */

#ifndef EMBGDISURF_H_
#define EMBGDISURF_H_
#include "ABmp.h"
#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC  EmbGDISurf: public tagGDC {
public:
	EmbGDISurf():_scrb(0), hDC(0), xOff(0), yOff(0), maxe(0), maxc(0), max3De(35), max3Dc(120),brightness(125),lxx(0.0), lyy(0.0), lzz(1.0),MAXHORZ(12) {hDC = 0; }

    ~EmbGDISurf() {}
    BOOL Init(IRect *rc = 0) { if (rc) clip = *rc; return true; }
    BOOL Init(const IRect &rc) { clip = rc; return true; }
	void XLine3D(int x1, int y1, int x2, int y2, COLORREF c, int d);
	void XPolyLine3D(IPoint *pts, int npt, COLORREF cc, int dd);

	void XClear(COLORREF c) { ABGColor cc(c); FillRect(clip, cc); }
	BOOL SetDataAccess() {
		if (1) //tagGDC::SetAccess())
		{
			_scrb = ptr;
			pMin = 	_scrb;
			pMax = _scrb + info.wline;
			Init3DMax();
		}
		return 1;
	}
	BOOL ReleaseDataAccess() {return tagGDC::ReleaseAccess(); if (hDC && _scrb) hDC->Show(*this, clip.left, clip.top, &clip); _scrb = 0; return true; }
	BOOL Flush() { return hDC != 0;}
protected:
    COLORREF CalcColor(COLORREF col, double ci);
    void calcxy(int x, int y, int *xx, int *yy, int d, int dx=0, int dy=0);
    int iCalcCet(int x1, int y1, int x2, int y2, int l, int nhorz);
    void plot(DWORD *p, COLORREF c, int n);
    BOOL GoodXY(int x, int y);
    void plot(DWORD *p, COLORREF c);
    DWORD * plot(DWORD *dc, int x, int y, COLORREF c);
    void plot(DWORD *dc, int x, int y, COLORREF c, int maxX, int maxY);
    void XLine(int x1, int y1, int x2, int y2, COLORREF c, int d);
    void polypix(DWORD *dc,int n, int *x, int *y, COLORREF color);
    void Get3DMax(int &maxe, int &maxc);
    virtual void Init3DMax() { Get3DMax(maxe,maxc); }
    int SetLight(int red, int green, int blue, double intens);

    void _XPolyLine3D(IPoint *pp, int npt, COLORREF cc, int dd)
    {
			for(int i=1; i<npt; ++i)
				XLine3D(pp[i - 1].x, pp[i - 1].y, pp[i].x, pp[i].y, cc, dd);
    }

    DWORD *_scrb;
    int xOff;
	int yOff;
	GDC hDC;
	DWORD *pMin, *pMax;

		int max3Dc, max3De;
	    int brightness;
	    double lxx, lyy, lzz;
	    int MAXHORZ;
	    int maxe, maxc;

};
#pragma pack(pop)

inline BOOL EmbGDISurf::GoodXY(int x, int y){
	return ((x>=0) && (x<info.width) && (y>=0) && (y<info.height));
}
inline void EmbGDISurf::plot(DWORD *p, COLORREF c)
{
    if (p >= pMin && p < pMax)
    	*(DWORD *)p = 0xFF000000 | c;
 //   else
   //     ASSERT(0);
}
inline void EmbGDISurf::Get3DMax(int &maxe, int &maxc)
{
	maxe = brightness*max3De/100;
	maxc = brightness*max3Dc/100;
}
inline DWORD * EmbGDISurf::plot(DWORD *dc, int x, int y, COLORREF c)
{
	DWORD *p = dc +  y*info.width + x;
	if(x>=0 && x<info.width && y>=0 && y<info.height)
		*p = 0xFF000000 | c;
	return p;
}
inline void EmbGDISurf::plot(DWORD *p, COLORREF c, int n)
{
	while(n--)
    {

		 if (p >= pMin || p < pMax)
			*p = 0xFF000000 | c;
		++p;
	}
}
inline void EmbGDISurf::calcxy(int x, int y, int *xx, int *yy, int d, int dx, int dy)
{
	int rr = d >> 1;
	int rl = !(d & 1) ? d-rr-1 : rr;
	xx[0]=x-rl;		yy[0]=y+rr;
	xx[1]=xx[0];	yy[1]=y-rl;
	xx[2]=x+rr;		yy[2]=yy[1];
	xx[3]=xx[2];	yy[3]=yy[0];

}
#ifndef d2i
#define d2i(x) (int)floor(0.5+x)
#endif
inline int EmbGDISurf::iCalcCet(int x1, int y1, int x2, int y2, int l, int nhorz)
{
	double ax = x2-x1, ay = y2 - y1, az = 0;

	double llx = (lxx*ax + lyy*ay + lzz*az)/double(l);
	return d2i((llx+1.0)/2.0*nhorz);
}

inline COLORREF EmbGDISurf::CalcColor(COLORREF col, double ci){
	return (COLORREF)SetLight(GetRValue(col),GetGValue(col),GetBValue(col),ci);
}


inline void EmbGDISurf::plot(DWORD *dc, int x, int y, COLORREF c, int maxX, int maxY){
		DWORD *p = dc + y*info.width + x;
		if(x<maxX && y<maxY && x>=0 && y>=0)
			*p = 0xFF000000 | c;

}

#endif /* EMBGDISURF_H_ */
