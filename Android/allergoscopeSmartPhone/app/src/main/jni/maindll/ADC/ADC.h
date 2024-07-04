
#ifndef __ADC__
#define __ADC__
#include "AWnd.h"
#include "ABmp.h"
#include "mstack.h"
class AObject;
#ifndef WIN32
#ifndef USE_MDC
enum MDC_ALLIGH_TEXT { MAT_CENTER = 1,
							MAT_LEFT = 2,
							MAT_RIGHT = 4,
							MAT_VCENTER = 8,
							MAT_BOTTOM = 16,
							MAT_TOP = 32,
							MAT_WORDBREAK = 64,
                            MAT_CALCRECT = 128,
MAT_SINGLELINE =256};
#else
#incude "mdc.h"
#endif
#else

#include "mdc.h"
#endif
#ifndef WIN32
#define DT_TOP                      MAT_TOP
#define DT_LEFT                     MAT_LEFT
#define DT_CENTER                   MAT_CENTER
#define DT_RIGHT                    MAT_RIGHT
#define DT_VCENTER                  MAT_VCENTER
#define DT_BOTTOM                   MAT_BOTTOM
#define DT_WORDBREAK                MAT_WORDBREAK
#define DT_CALCRECT MAT_CALCRECT
#define DT_SINGLELINE MAT_SINGLELINE
enum DISP_MERICS {
	HORZSIZE,		//0.1mm
	VERTSIZE,		//0.1mm

	HORZRES,
	VERTRES,

	LOGPIXELSX,
	LOGPIXELSY

};
typedef struct tagDRAWITEMSTRUCT {
    UINT        CtlType;
    UINT        CtlID;
    UINT        itemID;
    UINT        itemAction;
    UINT        itemState;
    GHWND        hwndItem;
    GDC         hDC;
    IRect        rcItem;
    ULONG_PTR   itemData;
    COLORREF bg;
    COLORREF fg;
    COLORREF tx;
} DRAWITEMSTRUCT, *PDRAWITEMSTRUCT, *LPDRAWITEMSTRUCT;

#else
typedef int DISP_MERICS;
#endif
#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC ADC {
public:
	ADC (tagGHWND * hWnd = 0);

	virtual ~ADC ();
public:
	bool Attach(GDC dc);
	bool Detach();
	bool OpenDC();
	bool CloseDC();
	bool OpenMBmp(ABmp &bmp);
	GDC SelectObject(GDC bmp);
	bool CloseMBmp();
public:
#ifndef WIN32
    MInstance::METRICS *metrics;
#endif
	IRect GetClientRect() { return m_hDC ? m_hDC->GetRect() : IRect(); }
	int GetDeviceCaps(DISP_MERICS m);
	IPoint GetCurrentPosition()  const { return m_hDC ? m_hDC->GetCurrentPosition() : IPoint(0, 0); }
	operator GDC () const { return m_hDC; }
	int GetBitmapRasterOp() { return bmpRop; }
	int BitmapRasterOp(int rop) { int r = bmpRop; bmpRop = rop; return r; }

	BOOL SetViewportOrg(int x,int y,IPoint *old) { return m_hDC ? m_hDC->SetViewportOrg(x, y,old) : 0; }

	void SetClipBox(IRect *rc) { if(m_hDC) m_hDC->SetClipBox(rc); }
	IRect GetClipBox() const { return m_hDC ? m_hDC->GetClipBox() : IRect(); }
	bool GetClipBox(IRect *rc) const { if (m_hDC && rc) *rc = m_hDC->GetClipBox(); return m_hDC != 0; }
public :
	int SetROP2(int rop) { return m_hDC ? m_hDC->SetROP2(rop) : 0; }
	int GetROP2() { return m_hDC ? m_hDC->GetROP2() : 0; }
	int SetBkMode(int bk) { return m_hDC ?  m_hDC->SetBkMode(bk) : 0; }
	COLORREF SetBkColor(COLORREF bk) { return m_hDC ?  m_hDC->SetBkColor(bk) : 0; }
	int SetPolyFillMode(int flg) { return m_hDC ? m_hDC->SetPolyFillMode(flg) : 0;}
public :
	bool DrawText(LPCSTR lpchText, int cchText, IRect *lprc, UINT format, MVector<MArray<IPoint> >* out = 0) { return m_hDC ? m_hDC->DrawText(lpchText, cchText, lprc, format, out) : 0; }
	bool TextOut(int x, int y, LPCSTR lpString, int len, MVector<MArray<IPoint> >* out = 0) { return m_hDC ? m_hDC->TextOut(x, y, lpString, len, out) : 0; }
	IRect GetStringRect(LPCSTR str, int len=0) { return m_hDC ? m_hDC->GetStringRect(str, len) : IRect(); }
	bool DrawText(const wchar_t *lpchText, int cchText, IRect *lprc, UINT format, MVector<MArray<IPoint> >* out = 0) { return m_hDC ? m_hDC->DrawText(lpchText, cchText, lprc, format, out) : 0; }
	bool TextOut(int x, int y, const wchar_t *lpString, int len, MVector<MArray<IPoint> >* out = 0) { return m_hDC ? m_hDC->TextOut(x, y, lpString, len, out) : 0; }
	IRect GetStringRect(const wchar_t *str, int len=0) { return m_hDC ? m_hDC->GetStringRect(str, len) : IRect(); }
	COLORREF SetTextColor(COLORREF c) { return m_hDC ? m_hDC->SetTextColor(c) : 0; }
public :
	AFONT SelectObject(AFONT h) { return m_hDC ? m_hDC->SelectObject(h) : 0; }
	GPEN SelectObject(GPEN h) { return m_hDC ? m_hDC->SelectObject(h) : 0; }
	AHandle *SelectObject(AHandle *h){ return m_hDC ? m_hDC->SelectObject(h) : 0; }
	GPEN SetPen(COLORREF c, int w, int at) { return m_hDC ? m_hDC->SetPen(c, w, at) : 0;  }
	GPEN SetFGColor(COLORREF c)  { return m_hDC ? m_hDC->SetFGColor(c) : 0; }
	GPEN GetCurPen() { return m_hDC ? m_hDC->GetCurPen() : 0; }

	GBRUSH SelectObject(GBRUSH h){ return m_hDC ? m_hDC->SelectObject(h) : 0; }
	GBRUSH SetBgColor(COLORREF c) { return m_hDC ? m_hDC->SetBgColor(c) : 0; }
	GBRUSH GetCurBrush()  { return m_hDC ? m_hDC->GetCurBrush() : 0; }
	void UnSelect(AHandle *h) { if (m_hDC) m_hDC->UnSelect(h); }
	void UnSelectObject(AHandle *h) { UnSelect(h); }
	void Show(MemImage &i, int x, int y, int, IRect *rc_src) { if(m_hDC) m_hDC->Show(i, x, y, rc_src, bmpRop); }
	void Stretch(MemImage &i, IRect &rc_dst, int, IRect *rc_src) {
		if(m_hDC)
		{
			if (rc_src)
				m_hDC->Stretch(i, *rc_src, rc_dst, bmpRop);
			else
				m_hDC->Stretch(i, rc_dst, bmpRop);
		}
	}
	void Stretch(AObject *i, IRect &rc_dst, int, IRect *rc_src);
	void Show(MemImage &i, IRect &rc_dst, int, IRect *rc_src) {
		if(m_hDC)
		{
			if (rc_src)
				m_hDC->Show(i, *rc_src, rc_dst, bmpRop);
			else
				m_hDC->Show(i, rc_dst, bmpRop);
		}
	}
	void Show(MemImage &i, int x, int y) { if(m_hDC) m_hDC->Show(i, x, y, bmpRop); }
	void Show(MemImage &i, IRect &rc_dst) {if(m_hDC) m_hDC->Show(i, rc_dst, bmpRop); }
	void Show(MemImage &i, IRect &rc_src, IRect &rc_dst) {if(m_hDC) m_hDC->Show(i, rc_src, rc_dst, bmpRop); }
	BOOL TransparentBlt(MemImage &i, IRect &rc_src, IRect &rc_dst, COLORREF crTransparent) { return m_hDC ? m_hDC->TransparentBlt(i, rc_src, rc_dst, bmpRop) : 0; }

	void Stretch(MemImage &i, IRect &rc_src, IRect &rc_dst) {if(m_hDC) m_hDC->Stretch(i, rc_src, rc_dst, bmpRop); }

	void Scroll(int dx, int dy) {if(m_hDC) m_hDC->Scroll(dx, dy); }
	void Line(int x1, int y1, int x2, int y2) {if(m_hDC) m_hDC->Line(x1, y1, x2, y2); }
	void PolyLine(IPoint *p, int len) {if(m_hDC) m_hDC->PolyLine(p, len); }
	void Polyline(IPoint *p, int len) {if(m_hDC) m_hDC->PolyLine(p, len); }
	void Clear() {if(m_hDC) m_hDC->Clear(); }
	void FillRect(IRect &rc, ABGColor &bg) {if(m_hDC) m_hDC->FillRect(rc, bg); }
	void FillRect(IRect &rc, ABGColor *bg) {if(m_hDC) m_hDC->FillRect(rc, *bg); }
	void FillRect(IRect &rc, COLORREF clr) { FillSolidRect(rc, clr);  }
	void FillSolidRect(IRect &rc, COLORREF bg) { if(m_hDC) m_hDC->FillSolidRect(rc, bg); }


	void Rectangle(IRect &rc) {if(m_hDC) m_hDC->Rectangle(rc); }
	void RoundRect(int x, int y, int width, int height, int d1, int d2) { if(m_hDC) m_hDC->RoundRect(x, y, width, height, max(d1, d2)); }
	void Rectangle(int left, int top, int right, int bottom) { IRect r(left, top, right, bottom); Rectangle(r); }
	void Rectangle(const IRect *r) { Rectangle(*(IRect *)r); }

	void SetPixel(int x, int y, COLORREF c)  {if(m_hDC) m_hDC->SetPixel(x, y, c); }
	COLORREF GetPixel(int x, int y)  { return m_hDC ? m_hDC->GetPixel(x, y) : 0; }
	void MoveToEx(int x, int y, IPoint *oldpos) {if(m_hDC) m_hDC->MoveToEx(x, y, oldpos); }
	void MoveTo(int x, int y) {if(m_hDC) m_hDC->MoveToEx(x, y, 0); }
	void LineTo(int x, int y) { if(m_hDC) m_hDC->LineTo(x, y); }
	void MoveTo(const IPoint &p) {if(m_hDC) m_hDC->MoveToEx(p.x, p.y, 0); }
	void LineTo(const IPoint &p) { if(m_hDC) m_hDC->LineTo(p.x, p.y); }

	void Ellipse(int left, int top, int right, int bottom) { IRect r(left, top, right, bottom); Ellipse(r); }
	void Ellipse(IRect &rc)  { if(m_hDC) m_hDC->Ellipse(rc); }
	void Ellipse(const IRect *rc)  { if(m_hDC) m_hDC->Ellipse(*(IRect *)rc); }
	void Polygon(IPoint *p, int len) { if(m_hDC) m_hDC->Polygon(p, len); }
	void PatBlt(IRect &rc, int rop);
	void Rect(int x1, int y1, int x2, int y2){ if(m_hDC) m_hDC->Rect(x1, y1, x2, y2); }
	void Rect( IRect &r) { Rect(r.left, r.top, r.right, r.bottom); }
	void PolyBezier(IPoint *p, int count){ if(m_hDC) m_hDC->PolyBezier(p, count); }
	void PolyPolygon(int n, int *count, IPoint *points){ if(m_hDC) m_hDC->PolyPolygon(n, count, points); }
	void Bar(int x1, int y1, int x2, int y2) { if(m_hDC) m_hDC->Bar(x1, y1, x2, y2); }
	BOOL BeginPath(){ return  m_hDC ? m_hDC->BeginPath() : 0; }
	BOOL EndPath(){ return  m_hDC ? m_hDC->EndPath() : 0; }
	BOOL StrokePath(){ return  m_hDC ? m_hDC->StrokePath() : 0; }
	BOOL StrokeAndFillPath(){ return  m_hDC ? m_hDC->StrokeAndFillPath() : 0; }
	BOOL FillPath(){ return  m_hDC ? m_hDC->FillPath() : 0; }
	CSize GetTextExtent(LPCSTR t){ return  m_hDC ? m_hDC->GetTextExtent(t) : CSize(0, 0); }
	GDC m_hDC;
protected:

	tagGHWND *hWnd;
	int bmpRop; //bmpRop = SRCCOPY
	MLifo<tagGDC *> stackDC;
};
#pragma pack(pop)

#ifndef WIN32 
typedef ABGColor CBrush;
typedef AFGColor CPen;
typedef APalette CPalette;
typedef AWnd CWnd;
typedef ADC CDC;
typedef ABmp CBitmap;
#endif
#ifndef WIN32 
typedef AHandle  MHandle;
typedef ADC  MDC;
typedef ABGColor BGColor;
typedef AFGColor FGColor;
typedef APalette MPalette;
typedef ABGColor CBrush;
typedef AFGColor CPen;
typedef APalette CPalette;
typedef AWnd CWnd;
typedef ABmp MBmp;
#endif
#if (1)
typedef ABmp * GBITMAP;
inline int SetBkMode(GDC dc, int mod) { return dc ? dc->SetBkMode(mod) : 0; }
inline COLORREF SetBkColor(GDC dc, COLORREF mod) { return dc ? dc->SetBkColor(mod) : 0; }

inline int SetPolyFillMode(GDC dc, int flg){if (dc)   dc->SetPolyFillMode(flg); return flg; }
inline BOOL BeginPath(GDC dc) { return dc ? dc->BeginPath() : 0; }
inline BOOL EndPath(GDC dc) { return dc ? dc->EndPath() : 0; }
inline BOOL StrokePath(GDC dc) { return dc ? dc->StrokePath() : 0; }
inline BOOL StrokeAndFillPath(GDC dc) { return dc ? dc->StrokeAndFillPath() : 0; }
inline BOOL FillPath(GDC dc) { return dc ? dc->FillPath() : 0; }
inline void SetClipBox(GDC dc, IRect *rc) { if (dc) dc->SetClipBox(rc); }
inline void MoveToEx(GDC dc, int x, int y, IPoint *p = 0) { if (dc) dc->MoveToEx(x, y, p); }
#ifdef WIN32
inline void MoveToEx(GDC dc, int x, int y, POINT *p) { if (dc) dc->MoveToEx(x, y, (IPoint *)p); }
#endif
inline void LineTo(GDC dc, int x, int y) { if (dc) dc->LineTo(x, y); }
inline void SetPixel(GDC dc, int x, int y, COLORREF c) { if (dc) dc->SetPixel(x, y, c); }
inline int SetROP2(GDC dc, int rop) { return dc ? dc->SetROP2(rop) : 0; }
inline int GetROP2(GDC dc) { return dc ? dc->GetROP2() : 0; }
inline GBRUSH SelectObject(GDC dc,GBRUSH b) { return dc ? dc->SelectObject(b) : 0; }
inline AHandle *SelectObject(GDC dc,AHandle *b) { return dc ? dc->SelectObject(b) : 0; }
inline GPEN SelectObject(GDC dc,GPEN b) { return dc ? dc->SelectObject(b) : 0; }
inline AFONT SelectObject(GDC dc,AFONT b) { return dc ? dc->SelectObject(b) : 0; }

inline void Rectangle(GDC dc, int left, int top, int right, int bottom ) { if (dc) dc->Rectangle(left, top, right, bottom); }
inline void Ellipse(GDC dc, int left, int top, int right, int bottom ) { if (dc) dc->Ellipse(left, top, right, bottom); }
inline void PolyBezier(GDC dc, IPoint * apt, DWORD cpt) {	if (dc) dc->PolyBezier(apt, cpt); }
inline void Polygon(GDC dc, IPoint * apt, DWORD cpt) {	if (dc) dc->Polygon(apt, cpt); }
inline void Polyline(GDC dc, IPoint * apt, DWORD cpt) {	if (dc) dc->PolyLine(apt, cpt); }
inline void PolyPolygon(GDC dc, int n, int *count, IPoint *points) { if (dc) dc->PolyPolygon(n, count, points); }
inline void PolyPolygon(GDC dc, IPoint *points, int *count, int n) { if (dc) dc->PolyPolygon(n, count, points); }
inline void RoundRect(GDC hDC, int x, int y, int width, int height, int d1, int d2) { if(hDC) hDC->RoundRect(x, y, width, height, max(d1, d2)); }
inline BOOL TransparentBlt(GDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, GDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, UINT crTransparent)
{
	if (!hdcDest || !hdcSrc) return FALSE;
	IRect rc_src(xoriginSrc, yoriginSrc, xoriginSrc + wSrc, yoriginSrc + hSrc);
	IRect rc_dst(xoriginDest, yoriginDest, xoriginDest + wDest, yoriginDest + hDest);
	hdcDest->TransparentBlt(*hdcSrc, rc_src, rc_dst, crTransparent);
	return true;
}
EXPORTNIX AHandle *AGetStockObject(STOCKOBJECTS i);

EXPORTNIX void DeleteObject(AHandle *p);
inline GPEN ACreatePen( int iStyle, int cWidth, COLORREF color)
{
	return new tagGPEN(color, cWidth, (BYTE)0xff, iStyle);

}
inline GBRUSH ACreateSolidBrush(COLORREF clr)
{
	return new tagGBRUSH(clr);
}

inline void FillRect(GDC dc,IRect *rc, GBRUSH b)
{
	if (!dc) return;
	dc->FillRect(*rc, *dynamic_cast <ABGColor *>(b));
}
inline BOOL PatBlt(GDC dc,  int x, int y, int w, int h, DWORD op)
{
	if (dc)
	{
		IRect rc(x, y, x + w, y + h);
		dc->PatBlt(rc, op);
		return 1;
	}
	return 0;
}
inline DWORD GetGlyphOutline(GDC dc, UINT uChar, UINT fuFormat, LPGLYPHMETRICS lpgm, DWORD cjBuffer, LPVOID pvBuffer, MAT2 *lpmat)
{
	return dc ? dc->GetGlyphOutline(uChar, fuFormat, lpgm, cjBuffer, pvBuffer, lpmat) : -1;
}
inline DWORD GetKerningPairs(GDC dc,DWORD cPairs, KERNINGPAIR *kern_pair )
{
	return dc ? dc->GetKerningPairs(cPairs, kern_pair) : -1;

}

inline bool DrawText(GDC dc, LPCSTR lpchText, int cchText, IRect *lprc, UINT format, MVector<MArray<IPoint> >* out = 0)
{
	return dc ? dc->DrawText(lpchText, cchText, lprc, format, out) : false;
}
inline bool DrawText(GDC dc, const wchar_t *lpchText, int cchText, IRect *lprc, UINT format, MVector<MArray<IPoint> >* out = 0)
{
	return dc ? dc->DrawText(lpchText, cchText, lprc, format, out) : false;
}

inline bool TextOut(GDC dc,int x, int y, LPCSTR lpString, int len, MVector<MArray<IPoint> >* out = 0)
{
	return dc ? dc->TextOut(x, y, lpString, len, out) : false;
}
inline bool TextOut(GDC dc,int x, int y, const wchar_t *lpString, int len, MVector<MArray<IPoint> >* out = 0)
{
	return dc ? dc->TextOut(x, y, lpString, len, out) : false;
}

inline COLORREF SetTextColor(GDC dc, COLORREF c)
{
	return dc ? dc->SetTextColor(c) : 0;
}
inline AFONT ACreateFontIndirect(LOGFONT *lf)
{
	AFont *f = new AFont();
	if (!f->CreateFontInderect(lf))
	{
		delete f;
		f = 0;
	}
	return f;
}
inline int GetDeviceCaps(GDC gdc, DISP_MERICS m)
{
	ADC dc;
	if (gdc)
		dc.Attach(gdc);
	int ret = dc.GetDeviceCaps(m);
	if (gdc)
		dc.Detach();
	return ret;
}

inline GDC ACreateCompatibleDC(int a)
{
	ASSERT(!a);
#ifdef WIN32
	ADC dc;
	int w = dc.GetDeviceCaps((DISP_MERICS)HORZSIZE);
	int h = dc.GetDeviceCaps((DISP_MERICS)VERTSIZE);
	ABmp *bmp = new ABmp(w, h);
#else

	ABmp *bmp = new ABmp(MGetApp()->GetMetrics()->widthPixels, MGetApp()->GetMetrics()->heightPixels);
#endif
	bmp->SetAccess();
	return bmp;
}
#ifndef WIN32
#define CreateFontIndirect ACreateFontIndirect
#define CreatePen ACreatePen
#define GetStockObject AGetStockObject
#define CreateCompatibleDC ACreateCompatibleDC
#define CreateSolidBrush ACreateSolidBrush
#endif

inline void DeleteDC(GDC dc)
{
	if (dc)
	delete dc;
}
#endif

#endif
