/*
 * MColor.h
 *
 *  Created on: Aug 26, 2014
 *      Author: alex
 */

#ifndef __MCOLOR_H_
#define __MCOLOR_H_
#include "mswin.h"
#include "Vector2F.h"
#include "mform2f.h"
#include "Transform2F.h"

#ifndef WIN32
#include "_ffile.h"
#define R2_BLACK            1   /*  0       */
#define R2_NOTMERGEPEN      2   /* DPon     */
#define R2_MASKNOTPEN       3   /* DPna     */
#define R2_NOTCOPYPEN       4   /* PN       */
#define R2_MASKPENNOT       5   /* PDna     */
#define R2_NOT              6   /* Dn       */
#define R2_XORPEN           7   /* DPx      */
#define R2_NOTMASKPEN       8   /* DPan     */
#define R2_MASKPEN          9   /* DPa      */
#define R2_NOTXORPEN        10  /* DPxn     */
#define R2_NOP              11  /* D        */
#define R2_MERGENOTPEN      12  /* DPno     */
#define R2_COPYPEN          13  /* P        */
#define R2_MERGEPENNOT      14  /* PDno     */
#define R2_MERGEPEN         15  /* DPo      */
#define R2_WHITE            16  /*  1       */

#define SRCCOPY             (DWORD)0x00CC0020 /* dest = source                   */
#define SRCPAINT            (DWORD)0x00EE0086 /* dest = source OR dest           */
#define SRCAND              (DWORD)0x008800C6 /* dest = source AND dest          */
#define SRCINVERT           (DWORD)0x00660046 /* dest = source XOR dest          */
#define SRCERASE            (DWORD)0x00440328 /* dest = source AND (NOT dest )   */
#define NOTSRCCOPY          (DWORD)0x00330008 /* dest = (NOT source)             */
#define NOTSRCERASE         (DWORD)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
#define MERGECOPY           (DWORD)0x00C000CA /* dest = (source AND pattern)     */
#define MERGEPAINT          (DWORD)0x00BB0226 /* dest = (NOT source) OR dest     */
#define PATCOPY             (DWORD)0x00F00021 /* dest = pattern                  */
#define PATPAINT            (DWORD)0x00FB0A09 /* dest = DPSnoo                   */
#define PATINVERT           (DWORD)0x005A0049 /* dest = pattern XOR dest         */
#define DSTINVERT           (DWORD)0x00550009 /* dest = (NOT dest)               */
#define BLACKNESS           (DWORD)0x00000042 /* dest = BLACK                    */
#define WHITENESS           (DWORD)0x00FF0062 /* dest = WHITE                    */
enum TYPE_WIN32 {
	PS_SOLID,
	PS_DASH,
	PS_DOT,
	PS_NULL,
	OPAQUE,
	TRANSPARENT,
	ALTERNATE,
	WINDING
};
enum  STOCKOBJECTS {
WHITE_BRUSH,
LTGRAY_BRUSH,
GRAY_BRUSH,
DKGRAY_BRUSH,
BLACK_BRUSH,
NULL_BRUSH,
WHITE_PEN,
BLACK_PEN,
NULL_PEN,
HOLLOW_BRUSH,
SYSTEM_FONT,
COUNT_STOCKOBJECTS
};


#else
typedef int STOCKOBJECTS;
//typedef RECT IRect;
//typedef POINT IPoint;

struct tagPTR {
tagPTR() {}
virtual ~tagPTR() {}
};
typedef HANDLE pthread_t;
#define pthread_self GetCurrentThread
typedef tagPTR *jobject;
struct AndroidBitmapInfo {
  int width;
    int height;
    int stride;
};
#define COUNT_STOCKOBJECTS 10
#define EXPORTNIX

#endif
#pragma pack(push)
#pragma pack(8)

EXPORTNIX struct EXPMDC AHandle : public tagPTR {
enum TYPE_AHANDLE {
		NONE,
		MFONT,
		BMP,
		HBMP,
		PBMP,
		FGCOLOR,
		BGCOLOR,
		HFGCOLOR,
		HBGCOLOR,
		PICTURE,
		MPALETTE,
		MPRINTER,
		MRGN,
		M_METAFILE,
		MCURSOR,
		MICON,
		MEMIMAGE,
		AGDC,

		AGDIPATH,
		AWND,
		HWIN
};
	AHandle(TYPE_AHANDLE t) { type = t; handle = (HANDLE)this; }
	virtual ~AHandle() { }
	TYPE_AHANDLE GetType() { return type; }
	HANDLE Handle() { return handle; }
	int IsHandle(){ return (handle != 0 && type != NONE);}
	TYPE_AHANDLE type;
	HANDLE handle;
};
struct ColorRef {
#pragma pack(push)
#pragma pack(4)
	union {
		struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE a;
		};
		COLORREF color;
	};
#pragma pack(pop)
	COLORREF GetColor() { return color; }
	void SetColor(COLORREF c) { color = c; }
	void Set(BYTE _r, BYTE _g, BYTE _b)
	{
		r = _r;
		g = _g;
		b = _b;
	}
	void Set(BYTE _r, BYTE _g, BYTE _b, BYTE _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;

	}
	COLORREF GrayScale()
	{
		BYTE c1 =  ((((int)r * 77 + (int)g*150  + (int)b * 29 + 128))/256);
		r = b = g = c1;
		return color;
	}
	int LenHalfRGB() {  return (int)r + (int)g + (int)b; } 
	int Len2RGB() {  return (int)r * (int)r + (int)g * (int)g + (int)b * (int)b; } 
	int Len2RGBA() {  return (int)r * (int)r + (int)g * (int)g + (int)b * (int)b  + (int)a * (int)a; } 
	float LenRGB() {  return (float)sqrt(float((int)r * (int)r + (int)g * (int)g + (int)b * (int)b)); } 
	float LenRGBA() { return (float)sqrt(float((int)r * (int)r + (int)g * (int)g + (int)b * (int)b  + (int)a * (int)a)); } 
	operator COLORREF () const { return color; }
	ColorRef &operator = (const COLORREF c)  { color = c; return *this; }
	BYTE &operator [] (int i) { 
		return i & 1 ? (i & 2 ? a : g) : (!i ? r : b); 
	}
};
EXPORTNIX struct EXPMDC ImageInfo  {
    int width;
    int height;
    int wline;
    ImageInfo() {width =  height = wline = 0; }
    ImageInfo(int w, int h, int _w) {width = w; height = h; wline = _w; }
	ImageInfo(const ImageInfo &a) {width = a.width; height = a.height; wline = a.wline; }
    ImageInfo(const AndroidBitmapInfo &a) {width = a.width; height = a.height; wline = a.stride/sizeof(DWORD); }
    void Set(const ImageInfo &a) {width = a.width; height = a.height; wline = a.wline; }
    void Set(int w, int h, int _w) {width = w; height = h; wline = _w; }
    void Set(const AndroidBitmapInfo &a) {width = a.width; height = a.height; wline = a.stride/sizeof(DWORD); }

};
#include "WinBmp.h"
EXPORTNIX struct EXPMDC ImageInfo2 : public WinBmp, public ImageInfo  {
    ImageInfo2();
    ImageInfo2(const ImageInfo2 &a);
    ImageInfo2(int w, int h, int _w) {width = w; height = h; wline = _w; }
	virtual DWORD *AttachData(ImageInfo &info) { info = *this; return image.GetPtr(); }
	virtual bool IsAttachedData() { return true; }
    virtual ~ImageInfo2() {DeletePicture();}
    ImageInfo2 &operator = (const ImageInfo2 &a);
    BOOL Load(BITMAPINFOHEADER *p);
    BOOL Load(const char *p) { return WinBmp::Load(p); }

    virtual void DeletePicture();
	virtual DWORD *AttachData(int x, int y, ImageInfo &info);
	virtual BOOL DetachData() {return true; }
	void CopyPalette(APalette *);
	MArray<DWORD> image;
};
class ABmp;
#ifndef WIN32
#define HS_HORIZONTAL       0       /* ----- */
#define HS_VERTICAL         1       /* ||||| */
#define HS_FDIAGONAL        2       /* \\\\\ */
#define HS_BDIAGONAL        3       /* ///// */
#define HS_CROSS            4       /* +++++ */
#define HS_DIAGCROSS        5       /* xxxxx */
#endif

EXPORTNIX struct EXPMDC tagGBRUSH : public AHandle  {
	enum BG_TYPE {BG_COLOR, BG_IMG };
	struct ListPattern {
	unsigned char **pattern;
	int len;
	ListPattern();
	~ListPattern();
	int Add(unsigned char pat[8]);
};

#pragma pack(push)
#pragma pack(4)
	union {
		struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE a;
		};
		COLORREF color;
	};
#pragma pack(pop)
	ImageInfo2 *pattern;
	BG_TYPE typeBrush;
	tagGBRUSH(COLORREF c = 0xFFFFFFFF, BYTE _a = 255) : AHandle(HBGCOLOR) { pattern = 0; typeBrush = BG_COLOR; color = (0x00FFFFFF & c) | ((((DWORD)_a << 24)) & 0xFF000000)  ; a = _a;  }
	tagGBRUSH(ABmp *);
	virtual ~tagGBRUSH() 
	{
		if (typeBrush == BG_IMG && pattern)
			delete pattern;
	}
	tagGBRUSH (const tagGBRUSH &brush) : AHandle(HBGCOLOR)  { 
		pattern = 0; 
		typeBrush = brush.typeBrush; 
		if (typeBrush == BG_IMG)
			pattern = new ImageInfo2(*brush.pattern);
		else
			color = brush.color; 
	}
    void DeleteObject ()
    {
    	SetColor(RGB(255, 255, 255));

    }

	void SetColor (COLORREF c, BYTE _a = 255)
	{
		if (typeBrush == BG_IMG && pattern)
			delete pattern;
		pattern = 0; 
		color = (0x00FFFFFF & c) | ((((DWORD)_a << 24)) & 0xFF000000);
		a = _a;
		typeBrush = BG_COLOR;
	}
	BOOL CreateHatchBrush(int type, COLORREF color) { return CreateHatchBrush(type, color, RGB(255, 255, 255)); }
	BOOL CreateHatchBrush(int type, COLORREF bg, COLORREF fg);
	BOOL CreateHatchBrush(const BYTE pattern[8], COLORREF bg, COLORREF fg);
#ifndef WIN32
	BOOL ACreateSolidBrush
#else
	BOOL CreateSolidBrush
#endif
			(COLORREF c) {SetColor(c); return TRUE; }

	BOOL CreatePatternBrush (BITMAPINFOHEADER *p);
	BOOL CreatePatternBrush (const char *filename);
	static tagGBRUSH *FromHandle(tagGBRUSH *b) { return b; }
	operator tagGBRUSH * ()  { return this; }

	tagGBRUSH &operator = (const tagGBRUSH &brush)  { 
		if (typeBrush == BG_IMG && pattern)
			delete pattern;
		pattern = 0; 
		typeBrush = brush.typeBrush; 
		if (typeBrush == BG_IMG)
			pattern = new ImageInfo2(*brush.pattern);
		else
			color = brush.color; 
	
		return *this; 
	}

};

EXPORTNIX struct EXPMDC tagGPEN : public AHandle  {
#pragma pack(push)
#pragma pack(4)
	union {
		struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE a;
		};
		COLORREF color;
	};
#pragma pack(pop)
	tagGPEN(COLORREF c = 0) : AHandle(HFGCOLOR) {BYTE _a = 255; UINT attr = PS_SOLID; color =  (0x00FFFFFF & c) | ((((DWORD)_a << 24)) & 0xFF000000); a = _a; attrib = attr; width = 1; }
	tagGPEN(COLORREF c, int w, BYTE _a, UINT attr) : AHandle(HFGCOLOR) { color = (0x00FFFFFF & c) | ((((DWORD)_a << 24)) & 0xFF000000); a = _a; attrib = attr; width = w; }
	tagGPEN(UINT attr, int w, COLORREF c)  : AHandle(HFGCOLOR) { color = (0x00FFFFFF & c) | 0xFF000000; a = 255; attrib = attr; width = w; }
	virtual ~tagGPEN() { ; }
	tagGPEN (const tagGPEN &pen) : AHandle(HFGCOLOR)  { color = pen.color; attrib = pen.attrib; width = pen. width; }
	tagGPEN &operator = (const tagGPEN &pen) { color = pen.color; attrib = pen.attrib; width = pen. width; return *this; }
	static tagGPEN *FromHandle(tagGPEN *b) { return b; }
	operator tagGPEN * () { return this; }
#ifndef WIN32
    BOOL ACreatePen
#else
BOOL CreatePen
#endif
			(UINT attr, int w, COLORREF c) { color =  (0x00FFFFFF & c) | 0xFF000000; a = 255; attrib = attr; width = w; return true;}
    void DeleteObject () {}

	void SetColor (COLORREF c, BYTE _a = 255)
	{
		color =  (0x00FFFFFF & c) | ((((DWORD)_a << 24)) & 0xFF000000);
		a = _a;
	}
	void SetPen(COLORREF c, int w = 1,  UINT attr = PS_SOLID, BYTE _a = 255)
	{
		color =  (0x00FFFFFF & c) | ((((DWORD)_a << 24)) & 0xFF000000); a = _a; attrib = attr; width = w;
	
	}

	int attrib;
	int width;
};

/*
EXPORTNIX struct AFGColor : public AHandle {

	AFGColor(COLORREF clr = 0xFF000000, BYTE a = 255, UINT attr = PS_SOLID):AHandle(BGCOLOR)  { handle = (HANDLE)new tagGPEN(clr, a, attr); }
	AFGColor(COLORREF clr, int w, BYTE a, UINT attr):AHandle(BGCOLOR)  { handle = (HANDLE)new tagGPEN(clr, w, a, attr); }
	virtual ~AFGColor() { delete (tagGPEN*)handle; }

};
EXPORTNIX struct ABGColor : public AHandle {

	ABGColor(COLORREF c = 0xFFFFFFFF, BYTE a = 255) : AHandle(BGCOLOR)  { handle = (HANDLE)new tagGBRUSH(c, a); }
	virtual ~ABGColor() { delete (tagGBRUSH*)handle; }

};
*/
typedef tagGPEN AFGColor;
typedef tagGBRUSH ABGColor;
#define APen AFGColor
#define ABrush ABGColor
typedef ABGColor * GBRUSH;
typedef AFGColor * GPEN;
class tagGDC;

#ifndef WIN32
#define LF_FACESIZE         128
struct LOGFONT
{
    LONG      lfHeight;
    LONG      lfWidth;
    LONG      lfEscapement;
    LONG      lfOrientation;
    LONG      lfWeight;
    BYTE      lfItalic;
    BYTE      lfUnderline;
    BYTE      lfStrikeOut;
    BYTE      lfCharSet;
    BYTE      lfOutPrecision;
    BYTE      lfClipPrecision;
    BYTE      lfQuality;
    BYTE      lfPitchAndFamily;
    char     lfFaceName[LF_FACESIZE];
};
typedef LOGFONT *LPLOGFONT;
#endif
EXPORTNIX struct tagMBITMAP : public AHandle  {
	tagMBITMAP(tagGDC *, TYPE_AHANDLE t);
	virtual ~tagMBITMAP();
	virtual DWORD *SetAccess() = 0;
	virtual BOOL ReleaseAccess() = 0;
	virtual BOOL SetSize(int cx, int cy, int type = 0) = 0;

	void OnDeleteContext();
	ImageInfo info;
	DWORD *m_data;
	tagGDC *self;
	int count;

};
#pragma pack(pop)

typedef tagMBITMAP * MBITMAP;
#endif /* MCOLOR_H_ */
