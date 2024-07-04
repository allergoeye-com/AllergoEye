#ifndef __AFONT__
#define __AFONT__

#include "AColor.h"
#include "critical.h"

#pragma pack(push)
#pragma pack(4)
#ifndef WIN32
typedef struct _MAT2 {
    FIXED eM11;
    FIXED eM12;
    FIXED eM21;
    FIXED eM22;
} MAT2, FAR *LPMAT2;


typedef struct _GLYPHMETRICS {
    UINT gmBlackBoxX;
    UINT gmBlackBoxY;
    POINT gmptGlyphOrigin;
    short gmCellIncX;
    short gmCellIncY;
} GLYPHMETRICS, FAR *LPGLYPHMETRICS;

#define GGO_METRICS        0
#define GGO_BITMAP         1
#define GGO_NATIVE         2
#define TT_POLYGON_TYPE   24

#define TT_PRIM_LINE       1
#define TT_PRIM_QSPLINE    2
#define TT_PRIM_CSPLINE    3

typedef struct tagPOINTFX {
    FIXED x;
    FIXED y;
} POINTFX, FAR *LPPOINTFX;

typedef struct tagTTPOLYCURVE {
    WORD wType;
    WORD cpfx;
    POINTFX apfx[1];
} TTPOLYCURVE, FAR *LPTTPOLYCURVE;

typedef struct tagTTPOLYGONHEADER {
    DWORD cb;
    DWORD dwType;
    POINTFX pfxStart;
} TTPOLYGONHEADER, FAR *LPTTPOLYGONHEADER;
typedef struct tagKERNINGPAIR {
    WORD wFirst;
    WORD wSecond;
    int iKernAmount;
} KERNINGPAIR, *LPKERNINGPAIR;


#define FW_BOLD             700
#define FW_NORMAL           400

#endif
#ifndef FT_BOLD
enum AFONT_TYPE {
    FT_NORMAL = 0,
    FT_BOLD = 1,
    FT_ITALIC = 2,
    FT_UNDERLINE = 4,
    FT_STRIKEOUT = 8
};
#endif
#pragma pack(pop)

#pragma pack(push)
#pragma pack(8)
EXPORTNIX class EXPMDC AFont : public AHandle {
public:
    AFont(bool fCriticalSection = 0);

    AFont(AFont &);

    virtual ~AFont();

    bool Create(const char *filename);

    bool _CreateFontInderect(LOGFONT *f);

    bool CreateFontInderect(LOGFONT *f);

    IRect GetStringRect(WString text)
    {    IRect b;
    return GetRectString(text, b); }

    bool GetLogFont(LOGFONT *lf);

    void CheckPreviewString(WString &check);

    bool IsValidString(const wchar_t *text, int num_chars);

    DWORD
    getGlyphOutline(UINT uChar, UINT fuFormat, LPGLYPHMETRICS lpgm, DWORD cjBuffer, LPVOID pvBuffer,
                    MAT2 *lpmat2);

    bool OutString(WString &text, int x, int y, COLORREF fg, DWORD *ptr, int width, int height,
                   IRect *clip);
    bool OutString(WString& text, int x, int y, MVector<MArray<IPoint> > &res);
    bool OutString(WString& text, float x, float y, MVector<MArray<Vector2F> >& res);

    IRect GetRectString(WString text, IRect &r);
  //  Box2D GetRectString(WString text, Box2D &r);

    bool SetSize(int size);

    DWORD GetKerningPairs(IN DWORD, OUT LPKERNINGPAIR);

    DWORD GetKerningPairs(MArray<KERNINGPAIR> &kern_pair);

    DWORD GetAllMetrics(RBMap<GLYPHMETRICS, USHORT> &metrics, MAT2 &mat);

    void GetName(TString &name);

    DWORD GetSpacing();

    int GetAllChars(MArray<USHORT> &data);

protected:
    DWORD GetGlyphOutlineNative(void *outline, int x, int y, MVector<MArray<IPoint> >& res);
    DWORD GetGlyphOutlineNative(float x, float y, MVector<MArray<Vector2F> >& res);

    DWORD GetGlyphOutlineNative(UINT glyph, LPGLYPHMETRICS lpgm, DWORD buflen, LPVOID buf);

    DWORD GetGlyphOutlineBitmap(UINT glyph, LPGLYPHMETRICS lpgm, DWORD buflen, LPVOID buf);

    int CheckPreviewChar(wchar_t &check, MHashSet<int> &mp);

    HGLOBAL LoadResource(TString &_filename, TString &def, BOOL blf, BOOL ilf);

protected:

//	FT_Error error;
//	FT_Face face;
    int error;
    HGLOBAL _face;
    //MArray<HGLOBAL> faces;
    CriticalSection *sec;
    MString name;
    int size;
    int attr;
    //double angle;
    //static FT_Library library;
    static HGLOBAL _library;
    static int count_lib;

};
#pragma pack(pop)
typedef AFont *AFONT;
#define GFONT AFONT

#endif
