#ifndef WIN32

#include "ft2build.h"
#include "freetype.h"
#include "ftglyph.h"
#include "tttables.h"
#include "ftstroke.h"

#endif

#include "stdafx.h"

#ifdef WIN32
#include "ft2build.h"
#include "freetype.h"
#include "ftglyph.h"
#include "tttables.h"
#include "ftstroke.h"
#endif

#include "AFont.h"
#include "ADC.h"
#include "critical.h"
#include "ttnameid.h"

DWORD mk_tag(LPCSTR a)
{
    DWORD *d = (DWORD *) a;
    return *d;
}

#define GET_BE_WORD(x) MAKEWORD(HIBYTE(x), LOBYTE(x))
#pragma pack(push)
#pragma pack(2)
struct TT_kern_table {
    USHORT version;
    USHORT nTables;
};

struct TT_kern_subtable {
    USHORT version;
    USHORT length;
    union {
        USHORT word;
        struct {
            USHORT horizontal : 1;
            USHORT minimum : 1;
            USHORT cross_stream: 1;
            USHORT override : 1;
            USHORT reserved1 : 4;
            USHORT format : 8;
        } bits;
    } coverage;
};

struct TT_format0_kern_subtable {
    USHORT nPairs;
    USHORT searchRange;
    USHORT entrySelector;
    USHORT rangeShift;
};

struct TT_kern_pair {
    USHORT left;
    USHORT right;
    short value;
};
#pragma pack(pop)

HGLOBAL AFont::_library = 0;
int AFont::count_lib = 0;

class HashMapFontName : public MHashMap<TString, TString> {
public:
    HashMapFontName()
    {};

    ~HashMapFontName()
    {};

    bool Lookup(TString key, TString &res)
    {
        sec.Enter();
        bool fRet = MHashMap<TString, TString>::Lookup(key, res);
        sec.Leave();
        return fRet;
    }

    bool Lookup(TString key)
    {
        sec.Enter();
        bool fRet = MHashMap<TString, TString>::Lookup(key);
        sec.Leave();
        return fRet;
    }

    void Add(TString key, TString val)
    {
        sec.Enter();
        (*this)[key] = val;
        sec.Leave();

    }

    CriticalSection sec;
};

class HashMapFontFile : public MHashMap<MArray<char>, TString> {
public:
    HashMapFontFile()
    {};

    ~HashMapFontFile()
    {};

    bool Lookup(TString key, MArray<char> &res)
    {
        sec.Enter();
        bool fRet = MHashMap<MArray<char>, TString>::Lookup(key, res);
        sec.Leave();
        return fRet;
    }

    bool Lookup(TString key)
    {
        sec.Enter();
        bool fRet = MHashMap<MArray<char>, TString>::Lookup(key);
        sec.Leave();
        return fRet;
    }

    void Add(TString key, MArray<char> &val)
    {
        sec.Enter();
        (*this)[key] = val;
        sec.Leave();

    }

    CriticalSection sec;
};

HashMapFontName g_ListFontName;
HashMapFontFile g_ListFontFile;

//----------------------------------------------------------------
//
//----------------------------------------------------------------
FT_Fixed FT_FixedFromFIXED(FIXED f)
{
    return (FT_Fixed) ((int) f.value << 16 | (unsigned int) f.fract);
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
void FTVectorToPOINTFX(FT_Vector *vec, POINTFX *pt)
{
    pt->x.value = (short) (vec->x >> 6);
    pt->x.fract = (WORD) ((vec->x & 0x3f) << 10);
    pt->x.fract |= (WORD) ((pt->x.fract >> 6) | (pt->x.fract >> 12));
    pt->y.value = (short) (vec->y >> 6);
    pt->y.fract = (WORD) ((vec->y & 0x3f) << 10);
    pt->y.fract |= (WORD) (((pt->y.fract >> 6) | (pt->y.fract >> 12)));
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
void MAT2toFT_Matrix(MAT2 *lpmat, FT_Matrix *ftMat)
{
    ftMat->xx = FT_FixedFromFIXED(lpmat->eM11);
    ftMat->xy = FT_FixedFromFIXED(lpmat->eM12);
    ftMat->yx = FT_FixedFromFIXED(lpmat->eM21);
    ftMat->yy = FT_FixedFromFIXED(lpmat->eM22);
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
AFont::AFont(bool fCriticalSection) : AHandle(MFONT)
{
    size = 0;
    attr = 0;
    ++count_lib;
    _face = 0;
    //angle = 0;
    sec = fCriticalSection ? new CriticalSection() : 0;
    error = 0;
    if (!_library)
    {
        FT_Library library = 0;
        if (!FT_Init_FreeType(&library))
            _library = library;

    }
}

int GetNumFaces(FT_Library library, MArray<char> &mdata)
{
    FT_Face face;
    FT_Long num_faces;


    int error = FT_New_Memory_Face(library, (FT_Byte *) mdata.GetPtr(), mdata.GetLen(), -1, &face);
    if (error)
    { return 0; }

    num_faces = face->num_faces;
    FT_Done_Face(face);
    return num_faces;
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
AFont::AFont(AFont &a) : AHandle(MFONT)
{
    size = 0;
    attr = 0;
    sec = 0;
    ++count_lib;
    _face = 0;
    //angle = 0;
    error = 0;
    if (!_library)
    {
        FT_Library library = 0;
        if (!FT_Init_FreeType(&library))
            _library = library;

    }
    if (a._face)
    {
        FT_Face face = (FT_Face) a._face;
        TString f(face->family_name);
        TString f2(face->style_name);
        f += " ";
        f += f2;
        if (!g_ListFontName.Lookup(f))
            return;
        Create(g_ListFontName[f].GetPtr());
        SetSize(a.size);
    }
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
AFont::~AFont()
{
    FT_Face face = (FT_Face) _face;
    if (face)
    {
        FT_Done_Face(face);
        _face = 0;
    }
    --count_lib;
    if (!count_lib)
    {
        FT_Library library = (FT_Library) _library;
        FT_Done_FreeType(library);
        _library = 0;
    }
}

HGLOBAL AFont::LoadResource(TString &_filename, TString &def, BOOL blf, BOOL ilf)
{
    FT_Face face = 0;
    HGLOBAL _face = 0;
    MVector<TString> _dir;
    MVector<TString> _files;
    int bface, iface;
    TString f, f2;
    TString name;
    AFXMemFile res;
#ifndef WIN32
    MGetApp()->ReadResourceDir("", "*.ttf", _dir, _files);
    for (int i = 0; i < _files.GetLen(); ++i)
    {
        res.Close();
        name = "ASSETS/";
        name += _files[i];
//			TString assets= name;
        //if (!CreateTmpFile(assets))
        //continue;


        if (g_ListFontFile.Lookup(name, res.buff) || MGetApp()->ReadResourceFile(name, res))
        {
            FT_Library library = (FT_Library) _library;
            if (!FT_New_Memory_Face(library, (FT_Byte *) res.buff.GetPtr(), res.buff.GetLen(), 0,
                                    &face) && face)
                //	if (!FT_New_Face(library, assets.GetPtr(), 0, &face))
            {
                f = face->family_name;
                f2 = face->style_name;
                f += " ";
                f += f2;
                if (!g_ListFontName.Lookup(f, name))
                {
                    g_ListFontName.Add(f, name);
                    g_ListFontFile.Add(name, res.buff);
                }
                bface = (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD;
                iface = (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC;
                if (bface == blf && iface == ilf)
                {
                    _filename = name;
                    FT_Done_Face(face);
                    break;
                }
                if (!def) // && !face->style_flags)
                    def = name;
                FT_Done_Face(face);
            }
            face = 0;

        }
    }
#endif
    if (!_filename.StrLen() && def.StrLen())
        return 0;
    if (!_filename.StrLen())
        _filename = def;
    if (_filename.StrLen())
        Create(_filename.GetPtr());
    return _face;
}

bool AFont::_CreateFontInderect(LOGFONT *f)
{
    return CreateFontInderect(f);
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
bool AFont::CreateFontInderect(LOGFONT *lf)
{
    if (sec) sec->Enter();

    TString filename;
    TString Name = lf->lfFaceName;
    BOOL blf = lf->lfWeight == FW_BOLD;
    BOOL ilf = lf->lfItalic;
    FT_Face face = 0;
    TString def, _filename = filename;
    if (!g_ListFontName.Lookup(Name, filename))
    {
#ifdef ANDROID_NDK
        MVector<TString> _dir;
        MVector<TString> _files;
        int bface, iface;
        TString f, f2;
        TString name;
        CMemFile res;
        MGetApp()->ReadResourceDir("", "*.ttf", _dir, _files);
        for (int i = 0; i < _files.GetLen(); ++i)
        {
            res.Close();
            name = "ASSETS/";
            name += _files[i];
//			TString assets= name;
            //if (!CreateTmpFile(assets))
            //continue;


            if (g_ListFontFile.Lookup(name, res.buff) || MGetApp()->ReadResourceFile(name, res))
            {
                FT_Library library = (FT_Library) _library;
                int N = GetNumFaces(library, res.buff);

                for (int j = 0; j < N; ++j)
                {
                    if (!FT_New_Memory_Face(library, (FT_Byte *) res.buff.GetPtr(),
                                            res.buff.GetLen(), j, &face) && face)
                        //	if (!FT_New_Face(library, assets.GetPtr(), 0, &face))
                    {
                        f = face->family_name;
                        f2 = face->style_name;
                        f += " ";
                        f += f2;
                        if (!g_ListFontName.Lookup(f, name))
                        {
                            g_ListFontName.Add(f, name);
                            g_ListFontFile.Add(name, res.buff);
                        }
                        bface = (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD;
                        iface = (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC;
                        if (bface == blf && iface == ilf)
                        {
                            _filename = name;
                            FT_Done_Face(face);
                            break;
                        }
                        if (!def) // && !face->style_flags)
                            def = name;
                        FT_Done_Face(face);
                    }
                    face = 0;
                }
                if (face)
                    break;

            }
        }
        if (!_filename.StrLen() && def.StrLen())
            return false;
        if (!_filename.StrLen())
            _filename = def;
        if (_filename.StrLen())
            Create(_filename.GetPtr());
        else
            Create(filename);
#else
        {
        if (sec) sec->Leave();
        return false;
        }
#endif
    } else
        Create(filename);
    if (!_face)
    {
        if (sec) sec->Leave();
        return false;
    }
    face = (FT_Face) _face;
    TString f(face->family_name);
    TString f2(face->style_name);
    f += " ";
    f += f2;
    name = f;
    _face = face;
    if (!error)
    {
        if ((face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD)
            attr |= FT_BOLD;
        if ((face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC)
            attr |= FT_ITALIC;
    }

    ADC dc;
    size = lf->lfHeight < 0 ? abs(lf->lfHeight) : MulDiv((int) abs(lf->lfHeight), (int) 72,
                                                         (int) dc.GetDeviceCaps(LOGPIXELSY));
    if ((error = FT_Set_Char_Size(face, size << 6, size << 6, dc.GetDeviceCaps(LOGPIXELSX),
                                  dc.GetDeviceCaps(LOGPIXELSY))))
    {
        if (sec) sec->Leave();
        return false;
    }
    if (sec) sec->Leave();
    IRect r1;
    GetRectString("Aq", r1);
    size = r1.Height();

    return true;
}

int check_num_gliph(FT_Face face)
{
    int i = 0;
    if (FT_Err_Ok == FT_Select_Charmap(face, FT_ENCODING_UNICODE))
    {
        FT_UInt glyph_code;
        FT_ULong char_code;

        glyph_code = 0;
        char_code = FT_Get_First_Char(face, &glyph_code);

        while (glyph_code)
        {
            ++i;
            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }
    }
    return i;

}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
bool AFont::Create(const char *filename)
{
    FT_Library library = (FT_Library) _library;
    FT_Face face = 0;
    TString nm = filename;
    MArray<HGLOBAL> faces;
    if (g_ListFontFile.Lookup(nm))
    {
        MArray<char> &mdata = g_ListFontFile[nm];

        int i = 0;
        face = 0;
        int test = 0;
        while (!FT_New_Memory_Face(library, (FT_Byte *) mdata.GetPtr(), mdata.GetLen(), i, &face))
        {
            _face = face;
            faces.Add() = _face;
            ++i;
        }
        if (faces.GetLen())
        {
            face = 0;
            int i = 0, l = faces.GetLen();
            int mi = 0, n, mx = -1;

            for (i = 0; i < l; ++i)
            {
                n = check_num_gliph((FT_Face) faces[i]);
                if (n > mx)
                {
                    mx = n;
                    mi = i;
                }
            }
            if (FT_Err_Ok == FT_Select_Charmap((FT_Face) faces[mi], FT_ENCODING_UNICODE))
            {
                face = (FT_Face) faces[mi];
            }
        }

    }
    if (!face)
    {
#ifdef ANDROID_NDK
        if (nm.Find("ASSETS/") != -1)
        {
            CMemFile res;
            if (MGetApp()->ReadResourceFile(nm, res))
            {
                FT_Library library = (FT_Library) _library;
                if (!FT_New_Memory_Face(library, (FT_Byte *) res.buff.GetPtr(), res.buff.GetLen(),
                                        0, &face))
                {
                    _face = face;
                    TString f(face->family_name);
                    TString f2(face->style_name);
                    f += " ";
                    f += f2;
                    if (!g_ListFontName.Lookup(f))
                    {
                        g_ListFontName.Add(f, nm);
                        g_ListFontFile.Add(nm, res.buff);
                    }
                    FT_Done_Face(face);
                    return Create(g_ListFontName[f].GetPtr());
                }

            }
            return false;
        } else
#endif
        {
#if (0)
#ifdef ANDROID_NDK
            MFILE file(filename);
            file.SetOpenMode(MFILE::F_READONLY);
            if (file.Open(FALSE))
            {
                MArray<char> buff(file.GetSize());
                file.Read(0, buff.GetPtr(), buff.GetLen());
                file.Close();
#else
            FILE *file = fopen(filename, "rb+");
            if (file)
            {
                fseek(file, 0, SEEK_END);
                int sz = ftell(file);
                fseek(file, 0, SEEK_SET);
                MArray<char> buff(sz);
                fread(buff.GetPtr(), buff.GetLen(), 1, file);
                fclose(file);
#endif

                if (!FT_New_Memory_Face(library, (FT_Byte *)buff.GetPtr(),buff.GetLen(), 0, &face))
                {
                    TString f(face->family_name);
                    TString f2(face->style_name);
                    f += " ";
                    f += f2;
                    if (!g_ListFontName.Lookup(f))
                    {
                        g_ListFontName.Add(f, filename);
                        //g_ListFontFile.Add(filename, buff);
                    }
                    FT_Done_Face(face);
                    return Create(g_ListFontName[f].GetPtr());
                }

            }
            return false;
#endif
            if (!(error = FT_New_Face(library, filename, -1, &face)))
            {
                int num_faces = face->num_faces;
                FT_Done_Face(face);
                if (num_faces > 1)
                    TRACE("nashel");
                for (int i = 0; i < num_faces; ++i)
                {
                    if (!(error = FT_New_Face(library, filename, i, &face)))
                    {
                        if (FT_Err_Ok == FT_Select_Charmap(face, FT_ENCODING_UNICODE))
                        {
                            _face = face;
                            break;
                        }
                        FT_Done_Face(face);
                    }
                }
            }
        }
    }
    if (_face)
    {
        TString f(face->family_name);
        TString f2(face->style_name);
        f += " ";
        f += f2;
        name = f;
        g_ListFontName.Add(f, filename);
        _face = face;
        if (!error)
        {
            if ((face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD)
                attr |= FT_BOLD;
            if ((face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC)
                attr |= FT_ITALIC;
        }
    }
    return !error;
}


//----------------------------------------------------------------
//
//----------------------------------------------------------------
bool AFont::SetSize(int height)
{
    if (sec) sec->Enter();

    FT_Face face = (FT_Face) _face;
    if (!face) return false;
    ADC dc;
    if (!(error = FT_Set_Char_Size(face, height << 6, height << 6, dc.GetDeviceCaps(LOGPIXELSX),
                                   dc.GetDeviceCaps(LOGPIXELSY))))
    {
        IRect rc;
        GetRectString("Aq", rc);
        size = rc.Height();
    }
    if (sec) sec->Leave();
    return !error;

}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
IRect AFont::GetRectString(WString text, IRect &real)
{
    FT_Face face = (FT_Face) _face;
    IRect rc(0, 0, 0, 0);
    real.SetRect(0, 0, 0, 0);
    int num_chars;
    if (!(num_chars = text.StrLen()) || !face)
        return rc;
    if (sec) sec->Enter();
    FT_GlyphSlot slot;
    slot = face->glyph;
    int x = 0;
    int y = 0;
    for (int n = 0; n < num_chars; ++n)
    {
        INT index = FT_Get_Char_Index(face, text[n]);
        error = FT_Load_Glyph(face, index, FT_LOAD_NO_BITMAP);
        FT_Vector *pp = slot->outline.points;
        int prev = 0;
        for (int i = 0; i < slot->outline.n_contours; ++i)
        {
            if (prev) ++prev;
            pp = slot->outline.points + prev;
            for (int j = prev; j <= slot->outline.contours[i]; ++j, ++pp)
            {
                rc.Update(x + (pp->x >> 6), y - (pp->y >> 6));
                real.Update(x + (pp->x >> 6), y - (pp->y >> 6));
            }
            prev = slot->outline.contours[i];
        }
        rc.Update(x + (((face->glyph->metrics.horiAdvance) + 63) >> 6),
                  rc.top + (((face->glyph->metrics.vertAdvance) + 63) >> 6));
        x += slot->advance.x >> 6;
        y += slot->advance.y >> 6;

    }
    if (sec) sec->Leave();
    return rc;
}
#if (0)
//----------------------------------------------------------------
//
//----------------------------------------------------------------
Box2D AFont::GetRectString(WString text, Box2D& real)
{
    FT_Face face = (FT_Face)_face;
    Box2D rc;
    rc.Set(0, 0, 0, 0);
    real.Set(0, 0, 0, 0);
    int num_chars;
    if (!(num_chars = text.StrLen()) || !face)
        return rc;
    if (sec) sec->Enter();
    FT_GlyphSlot slot;
    slot = face->glyph;
    int x = 0;
    int y = 0;
    for (int n = 0; n < num_chars; ++n)
    {
        INT index = FT_Get_Char_Index(face, text[n]);
        error = FT_Load_Glyph(face, index, FT_LOAD_NO_BITMAP);
        FT_Vector* pp = slot->outline.points;
        int prev = 0;
        for (int i = 0; i < slot->outline.n_contours; ++i)
        {
            if (prev) ++prev;
            pp = slot->outline.points + prev;
            for (int j = prev; j <= slot->outline.contours[i]; ++j, ++pp)
            {
                rc.Update(x + (pp->x >> 6), y - (pp->y >> 6));
                real.Update(x + (pp->x >> 6), y - (pp->y >> 6));
            }
            prev = slot->outline.contours[i];
        }
        rc.Update(x + (((face->glyph->metrics.horiAdvance) + 63) >> 6),
            rc.top + (((face->glyph->metrics.vertAdvance) + 63) >> 6));
        x += slot->advance.x >> 6;
        y += slot->advance.y >> 6;

    }
    if (sec) sec->Leave();
    return rc;
}
#endif
void AFont::GetName(TString &name)
{
    name = "";
    FT_Face face = (FT_Face) _face;
    if (!face)
        return;
    if (sec) sec->Enter();
    name = face->family_name;
    TString f2(face->style_name);
    name += " ";
    name += f2;
    if (sec) sec->Leave();
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
bool AFont::GetLogFont(LOGFONT *lf)
{

    FT_Face face = (FT_Face) _face;
    if (!face) return 0;
    memset(lf, 0, sizeof(LOGFONT));
    if (sec) sec->Enter();
    ADC dc;
    lf->lfHeight = -size; //MulDiv(size, dc.GetDeviceCaps(LOGPIXELSY), (int)72);
    lf->lfWeight = (int) ((attr & FT_BOLD) ? FW_BOLD : FW_NORMAL);
    lf->lfItalic = (BYTE) ((attr & FT_ITALIC) ? 1 : 0);
    lf->lfUnderline = (BYTE) ((attr & FT_UNDERLINE) ? 1 : 0);
    lf->lfStrikeOut = (BYTE) ((attr & FT_STRIKEOUT) ? 1 : 0);
    lf->lfEscapement = 0;
    lf->lfOrientation = 0;
#ifdef UNICODE
    WString f(face->family_name);
    WString f2(face->style_name);
    f += L" ";
    f += f2;
    memcpy(lf->lfFaceName, f.GetPtr(), f.StrLen() * sizeof(wchar_t));
#else
    TString f(face->family_name);
    TString f2(face->style_name);
    f += " ";
    f += f2;
    strcpy(lf->lfFaceName, f.GetPtr());
#endif
    if (sec) sec->Leave();
    return true;
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
DWORD AFont::getGlyphOutline(UINT uChar, UINT fuFormat, LPGLYPHMETRICS lpgm, DWORD cjBuffer,
                             LPVOID pvBuffer, MAT2 *lpmat)
{

    FT_Face face = (FT_Face) _face;
    if (!face) return -1;
    int index = FT_Get_Char_Index(face, uChar);
    if (uChar == 65 && !index)
    {
        FT_UInt glyph_code = 0;
        FT_ULong char_code;
        char_code = FT_Get_First_Char(face, &glyph_code);
        int n = 0;
        while (glyph_code)
        {
            if (iswalpha(char_code))
            {
                index = glyph_code;
                uChar = char_code;
                break;
            }
            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }
    }
/*	if (!index)
  {
      if (!FT_Load_Char(face, uChar, FT_LOAD_RENDER | FT_LOAD_MONOCHROME))
      {
          FT_UInt glyph_code = 0;
          FT_ULong char_code;
          char_code = FT_Get_First_Char(face, &glyph_code);
          while (glyph_code)
          {
              if (char_code == uChar)
                  index = glyph_code;
              char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
          }
          if (!index)
          return -1;
      }
  }
  */
    if (sec) sec->Enter();
    if (lpmat)
    {
        FT_Matrix ftMat;
        FT_Vector pos;
        pos.x = 0;
        pos.y = 0;
        ftMat.xx = (FT_Fixed) (0x1L);
        ftMat.xy = (FT_Fixed) (0);
        ftMat.yx = (FT_Fixed) (0);
        ftMat.yy = (FT_Fixed) (0x1L);
        MAT2toFT_Matrix(lpmat, &ftMat);
        FT_Set_Transform(face, &ftMat, &pos);
    }
    if (!index)
    {

        if ((error = FT_Load_Char(face, uChar, FT_LOAD_RENDER | FT_LOAD_MONOCHROME)))
        {
            if (sec) sec->Leave();
            return -1;
        }

    } else if ((error = FT_Load_Glyph(face, index, FT_LOAD_RENDER |
                                                   FT_LOAD_MONOCHROME))) // || (error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO)))
    {
        if (sec) sec->Leave();
        return -1;
    }
    memset(lpgm, 0, sizeof(GLYPHMETRICS));
    int left = (INT) (face->glyph->metrics.horiBearingX) & -64;
    int right = (INT) ((face->glyph->metrics.horiBearingX + face->glyph->metrics.width) + 63) & -64;
    int top = (face->glyph->metrics.horiBearingY + 63) & -64;
    int bottom = (face->glyph->metrics.horiBearingY - face->glyph->metrics.height) & -64;
    lpgm->gmCellIncX = (INT) ((face->glyph->metrics.horiAdvance) + 63) >> 6;
    lpgm->gmCellIncY = 0;
    lpgm->gmBlackBoxX = (right - left) >> 6;
    lpgm->gmBlackBoxY = (top - bottom) >> 6;
    lpgm->gmptGlyphOrigin.x = left >> 6;
    lpgm->gmptGlyphOrigin.y = top >> 6;
    if (fuFormat == GGO_METRICS) return 0;
    if ((fuFormat & GGO_BITMAP) == GGO_BITMAP)
    {
        if (sec) sec->Leave();
        return GetGlyphOutlineBitmap(uChar, lpgm, cjBuffer, pvBuffer);
    } else if ((fuFormat & GGO_NATIVE) == GGO_NATIVE)
    {
        if (sec) sec->Leave();
        return GetGlyphOutlineNative(uChar, lpgm, cjBuffer, pvBuffer);
    } else if ((fuFormat & GGO_NATIVE) == GGO_METRICS)
    {
        if (sec) sec->Leave();
        return 0;
    }
    if (sec) sec->Leave();
    return -1;
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
DWORD AFont::GetGlyphOutlineBitmap(UINT glyph, LPGLYPHMETRICS lpgm, DWORD buflen, LPVOID buf)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return -1;

    //error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
    //if (error) return -1;

    int width = lpgm->gmBlackBoxX;
    int height = lpgm->gmBlackBoxY;
    int pitch = ((width + 31) >> 5) << 2;
    int needed = pitch * height;
    if (buf)
    {
        BYTE *src = face->glyph->bitmap.buffer, *dst = (BYTE *) buf; // + pitch * (face->glyph->bitmap.rows - 1);
        INT w = (face->glyph->bitmap.width + 7) >> 3;
        INT h = face->glyph->bitmap.rows;
        bool fOk = false;
        while (!fOk && h--)
        {
            for (int i = 0; i < w; ++i)
                if (src[i] && src[i] != 0xFF)
                {
                    fOk = true;
                    break;

                }
            src += face->glyph->bitmap.pitch;
        }
        if (!fOk)
            return 0;
        src = face->glyph->bitmap.buffer;
        h = face->glyph->bitmap.rows;
        while (h--)
        {
            memcpy(dst, src, w);
            src += face->glyph->bitmap.pitch;
            dst += pitch;
        }
    }
    if (0)
    {
        ABmp test;
        DWORD dw[2];
        dw[0] = 0;
        dw[1] = RGB(255, 255, 255);
        test.Load(dw, 2, (char *) buf, width, height);
        lpgm->gmBlackBoxX += 10;
        lpgm->gmBlackBoxY += 10;
        APalette pal(dw, 2);
        width = lpgm->gmBlackBoxX;
        height = lpgm->gmBlackBoxY;
        ABmp test2(width + 10, height + 10, &pal);
        test2.SetAccess();
        test2.Show(test, 5, 5);
        //test2.Save("/storage/emulated/0/test/test.bmp");
        test2.Save("/storage/emulated/legacy/test2.bmp");
        test.Save("/storage/emulated/legacy/test.bmp");
    }
//	test.Save("/storage/emulated/legacy/test.bmp");

    //return (((width + 31) >> 5) << 2) * height;

    return needed;
}
#include "AproxLine.h"
DWORD AFont::GetGlyphOutlineNative(void *_outline, int x, int y, MVector<MArray<IPoint> >& res)
{
    AproxLine <IPoint, float> exec;
    FT_Face face = (FT_Face)_face;
    if (!face) return -1;
    FT_Outline* outline = (FT_Outline *)_outline;
    FT_Vector   v_last;
    FT_Vector   v_control;
    FT_Vector   v_start;

    FT_Vector* point;
    FT_Vector* limit;
    char* tags;
    int  first = 0, last;
    FT_Int   tag;
    IPoint first_pt;
    //outline->n_contours
    for (int n = 0; n < outline->n_contours; ++n)
    {
        MArray<IPoint>& bf = res.Add();


        last = outline->contours[n];
        if (last < 0)
            return -1;
        limit = outline->points + last;

        v_start = outline->points[first];
        v_start.x = (v_start.x >> 6) + x;
        v_start.y = -(v_start.y >> 6) + y;

        v_last = outline->points[last];
        v_last.x = (v_last.x >> 6) + x;
        v_last.y = -(v_last.y >> 6) + y;

        v_control = v_start;

        point = outline->points + first;
        tags = outline->tags + first;
        tag = FT_CURVE_TAG(tags[0]);
        if (tag == FT_CURVE_TAG_CUBIC)
            return -1;

        /* check first point to determine origin */
        if (tag == FT_CURVE_TAG_CONIC)
        {
            /* first point is conic control.  Yes, this happens. */
            if (FT_CURVE_TAG(outline->tags[last]) == FT_CURVE_TAG_ON)
            {
                /* start at last point if it is on the curve */
                v_start = v_last;
                limit--;
            }
            else
            {
                /* if both first and last points are conic,         */
                /* start at their middle and record its position    */
                /* for closure                                      */
                v_start.x = (v_start.x + v_last.x) / 2;
                v_start.y = -(v_start.y + v_last.y) / 2;

                /* v_last = v_start; */
            }
            point--;
            tags--;
        }
        first_pt.Set(v_start.x, v_start.y);
       bf.Add(first_pt);
        while (point < limit)
        {
            point++;
            tags++;

            tag = FT_CURVE_TAG(tags[0]);
            switch (tag)
            {
            case FT_CURVE_TAG_ON:  /* emit a single line_to */
            {
                FT_Vector  vec;


                vec.x = (point->x >> 6) + x;
                vec.y = -(point->y >> 6) + y;
                bf.Add().Set(vec.x, vec.y);
                first_pt.Set(vec.x, vec.y);
                continue;
            }

            case FT_CURVE_TAG_CONIC:  /* consume conic arcs */
                v_control.x = (point->x >> 6) + x;;
                v_control.y = -(point->y >> 6) + y;;

            Do_Conic:
                if (point < limit)
                {
                    FT_Vector  vec;
                    FT_Vector  v_middle;


                    point++;
                    tags++;
                    tag = FT_CURVE_TAG(tags[0]);

                    vec.x = (point->x >> 6) + x;
                    vec.y = -(point->y >> 6) + y;

                    if (tag == FT_CURVE_TAG_ON)
                    {
                        exec.PathBezier(first_pt.x, first_pt.y, v_control.x, v_control.y, vec.x, vec.y, bf);
                       // bf.Add().Set(vec.x, vec.y);
                        first_pt.Set(vec.x, vec.y);
                        continue;
                    }

                    if (tag != FT_CURVE_TAG_CONIC)
                        return -1;

                    v_middle.x = (v_control.x + vec.x) / 2;
                    v_middle.y = (v_control.y + vec.y) / 2;
                    exec.PathBezier(first_pt.x, first_pt.y, v_control.x, v_control.y, v_middle.x, v_middle.y, bf);
//                    bf.Add().Set(v_middle.x, v_middle.y);

                    first_pt.Set(v_middle.x, v_middle.y);
                  
                    v_control = vec;
                    goto Do_Conic;
                }

                exec.PathBezier(first_pt.x, first_pt.y, v_control.x, v_control.y, v_start.x, v_start.y, bf);
      //          bf.Add().Set(v_start.x, v_start.y);

                goto Close;
                default:  /* FT_CURVE_TAG_CUBIC */
                {
                    FT_Vector  vec1, vec2;


                    if (point + 1 > limit || FT_CURVE_TAG(tags[1]) != FT_CURVE_TAG_CUBIC)
                        return -1;

                    point += 2;
                    tags += 2;

                    vec1.x = (point[-2].x >> 6) + x;
                    vec1.y =- (point[-2].y >> 6) + y;

                    vec2.x = (point[-1].x >> 6) + x;
                    vec2.y = -(point[-1].y >> 6) + y;

                    if (point <= limit)
                    {
                        FT_Vector  vec;


                        vec.x = (point->x >> 6) + x;
                        vec.y = -(point->y >> 6) + y;
                        
                        exec.PathBezier(first_pt.x, first_pt.y, vec1.x, vec1.y, vec2.x, vec2.y, vec.x, vec.y, bf);
                        first_pt.Set(vec.x, vec.y);
                        continue;
                    }
                    exec.PathBezier(first_pt.x, first_pt.y, vec1.x, vec1.y, vec2.x, vec2.y, v_start.x, v_start.y, bf);
                    first_pt.Set(v_start.x, v_start.y);
                    goto Close;
                }
            }
        }
        first_pt.Set(v_start.x, v_start.y);
        bf.Add() = first_pt;
      
    Close:
        first = last + 1;

    }
    return -1;
}
Vector2F FT_ToFloat(FT_Vector v, float x, float y)
{
    float fx = float(v.x & 0x3f) / 100.f;
    float fy = float(v.y & 0x3f) / 100.f;
    Vector2F vf;
    vf.x = float(v.x >> 6) + x;
    vf.y = -float(v.y >> 6) + y;
    vf.x += fabs(fx);
    vf.y += fabs(fy);
    return vf;

}
DWORD AFont::GetGlyphOutlineNative(float x, float y, MVector<MArray<Vector2F> >& res)
{
    AproxLine <Vector2F, float> exec;
    FT_Face face = (FT_Face)_face;
    if (!face) return -1;

    FT_Outline* outline = &face->glyph->outline;
    Vector2F   v_control, v_last, v_start;

    FT_Vector* point;
    FT_Vector* limit;
    char* tags;
    int  first = 0, last;
    FT_Int   tag;
    Vector2F v, first_pt;
    //outline->n_contours
    for (int n = 0; n < outline->n_contours; ++n)
    {
        MArray<Vector2F>& bf = res.Add();


        last = outline->contours[n];
        if (last < 0)
            return -1;
        limit = outline->points + last;

        v_start = FT_ToFloat(outline->points[first], x, y);
        //v_start.x = (v_start.x >> 6) + x;
        //v_start.y = -(v_start.y >> 6) + y;

        v_last = FT_ToFloat(outline->points[last], x, y);
      //  v_last.x = (v_last.x >> 6) + x;
       // v_last.y = -(v_last.y >> 6) + y;

        v_control = v_start;

        point = outline->points + first;
        tags = outline->tags + first;
        tag = FT_CURVE_TAG(tags[0]);
        if (tag == FT_CURVE_TAG_CUBIC)
            return -1;

        /* check first point to determine origin */
        if (tag == FT_CURVE_TAG_CONIC)
        {
            /* first point is conic control.  Yes, this happens. */
            if (FT_CURVE_TAG(outline->tags[last]) == FT_CURVE_TAG_ON)
            {
                /* start at last point if it is on the curve */
                v_start = v_last;
                limit--;
            }
            else
            {
                /* if both first and last points are conic,         */
                /* start at their middle and record its position    */
                /* for closure                                      */
                v_start.x = (v_start.x + v_last.x) / 2;
                v_start.y = -(v_start.y + v_last.y) / 2;

                /* v_last = v_start; */
            }
            point--;
            tags--;
        }
        first_pt = v_start;
        bf.Add(first_pt);
        while (point < limit)
        {
            point++;
            tags++;

            tag = FT_CURVE_TAG(tags[0]);
            switch (tag)
            {
            case FT_CURVE_TAG_ON:  /* emit a single line_to */
            {
                first_pt = FT_ToFloat(*point, x, y);
                bf.Add(first_pt);
                continue;
            }

            case FT_CURVE_TAG_CONIC:  /* consume conic arcs */
                //v_control.x = (point->x >> 6) + x;;
                //v_control.y = -(point->y >> 6) + y;;
                v_control = FT_ToFloat(*point, x, y);

            Do_Conic:
                if (point < limit)
                {
                   Vector2F  vec;
                   Vector2F  v_middle;


                    point++;
                    tags++;
                    tag = FT_CURVE_TAG(tags[0]);

                    vec = FT_ToFloat(*point, x, y);


                    if (tag == FT_CURVE_TAG_ON)
                    {
                        exec.PathBezier(first_pt.x, first_pt.y, v_control.x, v_control.y, vec.x, vec.y, bf);
                        // bf.Add().Set(vec.x, vec.y);
                        first_pt.Set(vec.x, vec.y);
                        continue;
                    }

                    if (tag != FT_CURVE_TAG_CONIC)
                        return -1;

                    v_middle.x = (v_control.x + vec.x) / 2;
                    v_middle.y = (v_control.y + vec.y) / 2;
                    exec.PathBezier(first_pt.x, first_pt.y, v_control.x, v_control.y, v_middle.x, v_middle.y, bf);
                    //                    bf.Add().Set(v_middle.x, v_middle.y);

                    first_pt.Set(v_middle.x, v_middle.y);

                    v_control = vec;
                    goto Do_Conic;
                }

                exec.PathBezier(first_pt.x, first_pt.y, v_control.x, v_control.y, v_start.x, v_start.y, bf);
                //          bf.Add().Set(v_start.x, v_start.y);

                goto Close;
            default:  /* FT_CURVE_TAG_CUBIC */
            {
                Vector2F  vec1, vec2;


                if (point + 1 > limit || FT_CURVE_TAG(tags[1]) != FT_CURVE_TAG_CUBIC)
                    return -1;
                point += 2;
                tags += 2;
                vec1 = FT_ToFloat(point[-2], x, y);
                vec2 = FT_ToFloat(point[-1], x, y);

                if (point <= limit)
                {
                   Vector2F  vec;
                    vec = FT_ToFloat(*point, x, y);

                    exec.PathBezier(first_pt.x, first_pt.y, vec1.x, vec1.y, vec2.x, vec2.y, vec.x, vec.y, bf);
                    first_pt.Set(vec.x, vec.y);
                    continue;
                }
                exec.PathBezier(first_pt.x, first_pt.y, vec1.x, vec1.y, vec2.x, vec2.y, v_start.x, v_start.y, bf);
                goto Close;
            }
            }
        }
        first_pt.Set(v_start.x, v_start.y);
        bf.Add() = first_pt;

    Close:
        first = last + 1;

    }
    return -1;
}
//----------------------------------------------------------------
//
//----------------------------------------------------------------
DWORD AFont::GetGlyphOutlineNative(UINT glyph, LPGLYPHMETRICS lpgm, DWORD buflen, LPVOID buf)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return -1;
    DWORD needed = 0;
    int contour, point = 0, first_pt;
    FT_Outline *outline = &face->glyph->outline;
    TTPOLYGONHEADER *pph = 0;
    TTPOLYCURVE *ppc;
    DWORD pph_start, cpfx, type;

    if (buflen == 0) buf = NULL;

    for (contour = 0; contour < outline->n_contours; contour++)
    {
        pph_start = needed;
        first_pt = point;
        if (buf)
        {
            pph = (TTPOLYGONHEADER *) ((char *) buf + needed);
            pph->dwType = TT_POLYGON_TYPE;
            FTVectorToPOINTFX(&outline->points[point], &pph->pfxStart);
        }
        needed += sizeof(TTPOLYGONHEADER);
        point++;
        while (point <= outline->contours[contour])
        {
            ppc = (TTPOLYCURVE *) ((char *) buf + needed);
            type = (outline->tags[point] & FT_Curve_Tag_On) ? TT_PRIM_LINE : TT_PRIM_QSPLINE;
            cpfx = 0;
            do
            {
                if (buf)
                    FTVectorToPOINTFX(&outline->points[point], &ppc->apfx[cpfx]);
                cpfx++;
                point++;
            } while (point <= outline->contours[contour] &&
                     (outline->tags[point] & FT_Curve_Tag_On) ==
                     (outline->tags[point - 1] & FT_Curve_Tag_On));
            if (point > outline->contours[contour] && !(outline->tags[point - 1] & FT_Curve_Tag_On))
            {
                if (buf)
                    FTVectorToPOINTFX(&outline->points[first_pt], &ppc->apfx[cpfx]);
                cpfx++;
            } else if (point <= outline->contours[contour] &&
                       (outline->tags[point] & FT_Curve_Tag_On))
            {
                if (buf)
                    FTVectorToPOINTFX(&outline->points[point], &ppc->apfx[cpfx]);
                cpfx++;
                point++;
            }
            if (buf)
            {
                ppc->wType = (WORD) type;
                ppc->cpfx = (WORD) cpfx;
            }
            needed += sizeof(*ppc) + (cpfx - 1) * sizeof(POINTFX);
        }
        if (buf)
            pph->cb = needed - pph_start;
    }
    return needed;
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
DWORD AFont::GetSpacing()
{
    FT_Face face = (FT_Face) _face;
    char tx = L' ';
    INT index = FT_Get_Char_Index(face, tx);
    error = FT_Load_Glyph(face, index, FT_LOAD_RENDER);
    if (error)
        return 0;
    return face->glyph->advance.x >> 6;
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
bool AFont::IsValidString(const wchar_t *text, int num_chars)
{
    if (sec) sec->Enter();
    FT_Face face = (FT_Face) _face;
    INT index;
    bool fRet = true;
    for (int n = 0; n < num_chars; ++n)
    {
        INT index = FT_Get_Char_Index(face, text[n]);
        error = FT_Load_Glyph(face, index, FT_LOAD_RENDER);
        if (error || !index)
        {
            fRet = 0;
            break;
        }
    }
    if (sec) sec->Leave();
    return fRet;
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
bool AFont::OutString(WString& text, int x, int y, MVector<MArray<IPoint> >& res)
{
    FT_Face face = (FT_Face)_face;
    if (!face) return false;
    if (sec) sec->Enter();

    FT_GlyphSlot slot;
    slot = face->glyph;
    int num_chars = text.StrLen();
    IRect rc1;
    IRect rc = this->GetRectString(text, rc1);
    y -= rc.top;
    int offT = (rc.Height() - rc1.Height())/2;
    y += offT;
#if (1)
    FT_Stroker stroker;
    FT_Library library = (FT_Library)_library;
    FT_Stroker_New(library, &stroker);
    FT_Stroker_Set(stroker, (int32_t)(0), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    FT_Glyph pOutlineGlyph;
#endif
    for (int n = 0; n < num_chars; ++n)
    {
        INT index = FT_Get_Char_Index(face, text[n]);
        error = FT_Load_Glyph(face, index, FT_LOAD_NO_BITMAP);
        if (error)
            continue;
#if (1)
        error = FT_Get_Glyph(face->glyph, &pOutlineGlyph);
       if (error)
           continue;
        FT_Stroker_ParseOutline(stroker, &slot->outline, 1);
      // error = FT_Glyph_StrokeBorder(&pOutlineGlyph, stroker, 0, 1);
       FT_UInt npoints = 0, ncontours = 0;
       
       FT_Stroker_GetCounts(stroker, &npoints, &ncontours);
       FT_Outline* pOutLine = 0, outline;
       pOutLine = &outline;
       FT_Outline_New(library, npoints, ncontours, pOutLine);
       outline.n_points = 0;
       outline.n_contours = 0;
       FT_Stroker_Export(stroker, &outline);

       if (error)
           continue;
       //  FT_Outline* pOutLine = &reinterpret_cast<FT_OutlineGlyph>(pOutlineGlyph)->outline;

#endif
    
        int h = slot->bitmap.rows;
        int w = slot->bitmap.width;
        IRect r(x + slot->bitmap_left, y - slot->bitmap_top, x + slot->bitmap_left + w,
            y - slot->bitmap_top + h);
        r.OffsetRect(-x - slot->bitmap_left, -slot->bitmap_top + y);
        GetGlyphOutlineNative(&slot->outline, x, y, res);
#if (0)
        FT_Done_Glyph(pOutlineGlyph);
        FT_Stroker_Done(stroker);
#endif
   x += slot->advance.x >> 6;
        y += slot->advance.y >> 6;
    }
    if (sec) sec->Leave();
    return 1;
}
//----------------------------------------------------------------
//
//----------------------------------------------------------------
bool AFont::OutString(WString& text, float x, float y, MVector<MArray<Vector2F> >& res)
{
    FT_Face face = (FT_Face)_face;
    if (!face) return false;
    if (sec) sec->Enter();

    FT_GlyphSlot slot;
    slot = face->glyph;
    int num_chars = text.StrLen();
    IRect rc1;
    IRect rc = this->GetRectString(text, rc1);
    y -= rc.top;
    int offT = (rc.Height() - rc1.Height()) / 2;
    y += offT;
    for (int n = 0; n < num_chars; ++n)
    {
        INT index = FT_Get_Char_Index(face, text[n]);

        
        error = FT_Load_Glyph(face, index, FT_LOAD_NO_BITMAP);
        if (error)
            continue;
        int h = slot->bitmap.rows;
        int w = slot->bitmap.width;
        IRect r(x + slot->bitmap_left, y - slot->bitmap_top, x + slot->bitmap_left + w,
            y - slot->bitmap_top + h);
        r.OffsetRect(-x - slot->bitmap_left, -slot->bitmap_top + y);
        GetGlyphOutlineNative((float)x, (float)y, res);

        x += slot->advance.x >> 6;
        y += slot->advance.y >> 6;
    }
    if (sec) sec->Leave();
    return 1;
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
bool AFont::OutString(WString &text, int x, int y, COLORREF fg, DWORD *ptr, int width, int height,
                      IRect *clip)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return false;
    if (sec) sec->Enter();

    FT_GlyphSlot slot;
    slot = face->glyph;
    int num_chars = text.StrLen();
    
    IRect rc1;
    IRect rc = this->GetRectString(text, rc1);
    y -= rc.top;
    int offT = (rc.Height() - rc1.Height()) / 2;
    y += offT;
    for (int n = 0; n < num_chars; ++n)
    {
        INT index = FT_Get_Char_Index(face, text[n]);
        error = FT_Load_Glyph(face, index, FT_LOAD_RENDER);
    

        if (error)
            continue;
        int h = slot->bitmap.rows;
        BYTE *src = slot->bitmap.buffer;
        int w = slot->bitmap.width;
        IRect r(x + slot->bitmap_left, y - slot->bitmap_top, x + slot->bitmap_left + w,
                y - slot->bitmap_top + h);
        IRect r1(0, 0, w, h);
        r &= clip;
        ColorRef *cfg = (ColorRef *) &fg;
        ColorRef *dst = (ColorRef *) (ptr + r.left + r.top * width);
        r.OffsetRect(-x - slot->bitmap_left, slot->bitmap_top - y);
        r1 &= r;
        double scale;
        
        if (!r1.IsRectNull())
        {
            src += r.left + r.top * slot->bitmap.pitch;
            BYTE *_src = src;
            ColorRef *_dst = dst;
            h = r1.Height();
            while (h--)
            {
                src = _src;
                dst = _dst;
                int w = r1.Width();
                while (w--)
                {
                    if (*src)
                    {

                        scale = double(*src) / 255.0;
                        {
                            dst->r = (BYTE)(cfg->r * scale + dst->r * (1.0 - scale));
                            dst->g = (BYTE)(cfg->g * scale + dst->g * (1.0 - scale));
                            dst->b = (BYTE)(cfg->b * scale + dst->b * (1.0 - scale));
                        }
                    }

                    ++dst;
                    ++src;
                }
                _src += slot->bitmap.pitch;
                _dst += width;
            }
        }
        x += slot->advance.x >> 6;
    }
    if (sec) sec->Leave();
    return 1;
}

DWORD get_font_data(FT_Face ft_face, DWORD table, DWORD offset, BYTE *buf, DWORD cbData)
{
    FT_ULong len;
    FT_Error err;

    if (!buf)
        len = 0;
    else
        len = cbData;
    if (buf && len)
    {
        FT_ULong needed = 0;
        err = FT_Load_Sfnt_Table(ft_face, table, offset, NULL, &needed);
        if (!err && needed < len) len = needed;
    }
    return !FT_Load_Sfnt_Table(ft_face, table, offset, buf, &len) ? len : -1;
}

DWORD parse_format0_kern_subtable(FT_Face font, const struct TT_format0_kern_subtable *tt_f0_ks,
                                  const USHORT *glyph_to_char,
                                  KERNINGPAIR *kern_pair, DWORD cPairs, UINT ppem)
{
    USHORT i, nPairs;
    const struct TT_kern_pair *tt_kern_pair;

    nPairs = GET_BE_WORD(tt_f0_ks->nPairs);

    if (!kern_pair || !cPairs)
        return nPairs;

    tt_kern_pair = (const struct TT_kern_pair *) (tt_f0_ks + 1);

    nPairs = min(nPairs, (USHORT) cPairs);

    for (i = 0; i < nPairs; i++)
    {
        int h = GET_BE_WORD(tt_kern_pair[i].left);
        kern_pair->wFirst = glyph_to_char[h];
        kern_pair->wSecond = glyph_to_char[GET_BE_WORD(tt_kern_pair[i].right)];
        kern_pair->iKernAmount = (short) GET_BE_WORD(tt_kern_pair[i].value) * ppem;
        if (kern_pair->iKernAmount < 0)
        {
            kern_pair->iKernAmount -= font->units_per_EM / 2;
            kern_pair->iKernAmount -= ppem;
        } else if (kern_pair->iKernAmount > 0)
        {
            kern_pair->iKernAmount += font->units_per_EM / 2;
            kern_pair->iKernAmount += ppem;
        }
        kern_pair->iKernAmount /= font->units_per_EM;


        kern_pair++;
    }
    return nPairs;
}

LONG calc_ppem_for_height(FT_Face ft_face, LONG height)
{
    TT_OS2 *pOS2;
    TT_HoriHeader *pHori;

    LONG ppem;

    pOS2 = (TT_OS2 *) FT_Get_Sfnt_Table(ft_face, ft_sfnt_os2);
    pHori = (TT_HoriHeader *) FT_Get_Sfnt_Table(ft_face, ft_sfnt_hhea);

    if (height == 0) height = 16;
    if (height > 0)
    {
        if (pOS2->usWinAscent + pOS2->usWinDescent == 0)
            ppem = ft_face->units_per_EM * height /
                   (pHori->Ascender - pHori->Descender);
        else
            ppem = ft_face->units_per_EM * height /
                   (pOS2->usWinAscent + pOS2->usWinDescent);
    } else
        ppem = -height;

    return ppem;
}

void AFont::CheckPreviewString(WString &check)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return;

    MHashSet<int> mp;

    int l = check.StrLen();
    MArray<bool> j(l);
    for (int i = 0; i < l; ++i)
    {
        if (check[i] > 32)
        {
            INT index = FT_Get_Char_Index(face, check[i]);
            error = FT_Load_Glyph(face, index, FT_LOAD_RENDER);
            if (index && !error)
            {
                mp.Add(index);
                j[i] = true;
            } else
                j[i] = false;
        } else
            j[i] = true;

    }
    for (int i = 0; i < l; ++i)
    {
        if (!j[i])
            if (CheckPreviewChar(check[i], mp) == -1)
            {
                check.Remove(i, 1);
                j.Remove(i, 1);
                --i;
                --l;
            }
    }
}

int AFont::CheckPreviewChar(wchar_t &check, MHashSet<int> &mp)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return 0;

    if (face->charmap->encoding == FT_ENCODING_UNICODE)
    {
        FT_UInt glyph_code = 0;
        FT_ULong char_code;
        char_code = FT_Get_First_Char(face, &glyph_code);
        int n = 0;
        while (glyph_code)
        {
            if (iswalpha(char_code))
            {
                {
                    if (!mp.Find(glyph_code))
                    {
                        check = (wchar_t) char_code;
                        mp.Add(glyph_code);
                        return glyph_code;
                    }
                }
            }
            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }
        glyph_code = 0;
        char_code = FT_Get_First_Char(face, &glyph_code);
        while (glyph_code)
        {
            if (iswalnum(char_code))
            {
                {
                    if (!mp.Find(glyph_code))
                    {
                        check = (wchar_t) char_code;
                        mp.Add(glyph_code);
                        return glyph_code;
                    }
                }
            }
            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }
        glyph_code = 0;
        char_code = FT_Get_First_Char(face, &glyph_code);
        while (glyph_code)
        {
            if (iswprint(char_code))
            {
                {
                    if (!mp.Find(glyph_code))
                    {
                        check = (wchar_t) char_code;
                        mp.Add(glyph_code);
                        return glyph_code;
                    }
                }
            }
            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }

    }

    return -1;
}

int AFont::GetAllChars(MArray<USHORT> &data)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return 0;

    if (face->charmap->encoding == FT_ENCODING_UNICODE)
    {
        FT_UInt glyph_code = 0;
        FT_ULong char_code;
        char_code = FT_Get_First_Char(face, &glyph_code);
        int n = 0;
        while (glyph_code)
        {
            if (iswprint(char_code))
            {
                data.Add() = (USHORT) char_code;
                data.Add() = (USHORT) L' ';
            }
            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }
    }

    return data.GetLen();
}

DWORD AFont::GetAllMetrics(RBMap<GLYPHMETRICS, USHORT> &metrics, MAT2 &mat)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return 0;
    FT_Matrix ftMat;
    FT_Vector pos;
    pos.x = 0;
    pos.y = 0;
    if (sec) sec->Enter();

    MAT2toFT_Matrix(&mat, &ftMat);
    FT_Set_Transform(face, &ftMat, &pos);
    MArray<USHORT> glyph_to_char(65536, 0);
    if (face->charmap->encoding == FT_ENCODING_UNICODE)
    {
        FT_UInt glyph_code = 0;
        FT_ULong char_code;
        char_code = FT_Get_First_Char(face, &glyph_code);
        while (glyph_code)
        {
            if (glyph_code <= 65535 && !glyph_to_char[glyph_code])
                glyph_to_char[glyph_code] = (USHORT) char_code;
            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }
    } else
    {
        ULONG n;
        for (n = 0; n <= 65535; ++n)
        {
            int glyph_code = FT_Get_Char_Index(face, n);
            if (glyph_code && glyph_code <= 65535 && !glyph_to_char[glyph_code])
                glyph_to_char[glyph_code] = (USHORT) n;
        }

    }

    for (int n = 0; n <= 65535; ++n)
    {
        if (glyph_to_char[n])
            if (!FT_Load_Glyph(face, n, FT_LOAD_RENDER | FT_LOAD_MONOCHROME))
            {
                GLYPHMETRICS &pgm = metrics[glyph_to_char[n]];
                int left = (INT) (face->glyph->metrics.horiBearingX) & -64;
                int right =
                        (INT) ((face->glyph->metrics.horiBearingX + face->glyph->metrics.width) +
                               63) & -64;
                int top = (face->glyph->metrics.horiBearingY + 63) & -64;
                int bottom =
                        (face->glyph->metrics.horiBearingY - face->glyph->metrics.height) & -64;
                pgm.gmCellIncX = (INT) ((face->glyph->metrics.horiAdvance) + 63) >> 6;
                pgm.gmCellIncY = 0;
                pgm.gmBlackBoxX = (right - left) >> 6;
                pgm.gmBlackBoxY = (top - bottom) >> 6;
                pgm.gmptGlyphOrigin.x = left >> 6;
                pgm.gmptGlyphOrigin.y = top >> 6;
            }

    }
    if (sec) sec->Leave();

    return metrics.m_iNum;

}

DWORD AFont::GetKerningPairs(DWORD cPairs, KERNINGPAIR *kern_pair)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return 0;
    int length;
    MArray<BYTE> buf;
    if (sec) sec->Enter();

    const struct TT_kern_table *tt_kern_table;
    const struct TT_kern_subtable *tt_kern_subtable;
    USHORT i, nTables;
    DWORD tag = mk_tag("nrek");
    length = get_font_data(face, tag, 0, 0, 0);

    if (length <= 0)
    {
        tag = mk_tag("kern");
        length = get_font_data(face, tag, 0, 0, 0);
        if (length <= 0)
        {
            if (sec) sec->Leave();

            return 0;
        }
    }
    buf.SetLen(length);
    get_font_data(face, tag, 0, buf.GetPtr(), length);

    MArray<USHORT> glyph_to_char(65536, 0);
    if (face->charmap->encoding == FT_ENCODING_UNICODE)
    {
        FT_UInt glyph_code;
        FT_ULong char_code;

        glyph_code = 0;
        char_code = FT_Get_First_Char(face, &glyph_code);

        while (glyph_code)
        {
            if (glyph_code <= 65535 && !glyph_to_char[glyph_code])
                glyph_to_char[glyph_code] = (USHORT) char_code;

            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }
    } else
    {
        ULONG n;
        for (n = 0; n <= 65535; ++n)
            glyph_to_char[n] = (USHORT) n;
    }
    ADC dc;
    DWORD ppem = calc_ppem_for_height(face, MulDiv(size, dc.GetDeviceCaps(LOGPIXELSY), (int) 72));
    tt_kern_table = (TT_kern_table *) buf.GetPtr();
    nTables = GET_BE_WORD(tt_kern_table->nTables);
    tt_kern_subtable = (const struct TT_kern_subtable *) (tt_kern_table + 1);
    int total_kern_pairs = -1;
    for (i = 0; i < nTables; i++)
    {
        struct TT_kern_subtable tt_kern_subtable_copy;

        tt_kern_subtable_copy.version = GET_BE_WORD(tt_kern_subtable->version);
        tt_kern_subtable_copy.length = GET_BE_WORD(tt_kern_subtable->length);
        tt_kern_subtable_copy.coverage.word = GET_BE_WORD(tt_kern_subtable->coverage.word);
        if (tt_kern_subtable_copy.coverage.bits.format == 0)
        {
            if (total_kern_pairs == -1)
                total_kern_pairs = 0;
            DWORD new_chunk, old_total = total_kern_pairs;

            new_chunk = parse_format0_kern_subtable(face,
                                                    (const struct TT_format0_kern_subtable *) (
                                                            tt_kern_subtable + 1),
                                                    glyph_to_char.GetPtr(), NULL, 0, ppem);
            total_kern_pairs += new_chunk;

            if (kern_pair)
                parse_format0_kern_subtable(face, (const struct TT_format0_kern_subtable *) (
                                                    tt_kern_subtable + 1),
                                            glyph_to_char.GetPtr(), kern_pair + old_total,
                                            new_chunk, ppem);
        }
        tt_kern_subtable = (const struct TT_kern_subtable *) ((const char *) tt_kern_subtable +
                                                              tt_kern_subtable_copy.length);
    }
    if (sec) sec->Leave();

    return total_kern_pairs == -1 ? 0 : total_kern_pairs;
}

DWORD AFont::GetKerningPairs(MArray<KERNINGPAIR> &kern_pair)
{
    FT_Face face = (FT_Face) _face;
    if (!face) return 0;
    int length;
    MArray<BYTE> buf;
    if (sec) sec->Enter();
    const struct TT_kern_table *tt_kern_table;
    const struct TT_kern_subtable *tt_kern_subtable;
    USHORT i, nTables;
    DWORD tag = mk_tag("nrek");
    length = get_font_data(face, tag, 0, 0, 0);

    if (length <= 0)
    {
        tag = mk_tag("kern");
        length = get_font_data(face, tag, 0, 0, 0);
        if (length <= 0)
        {
            if (sec) sec->Leave();

            return 0;
        }
    }

    buf.SetLen(length);
    get_font_data(face, tag, 0, buf.GetPtr(), length);

    MArray<USHORT> glyph_to_char(65536);
    if (face->charmap->encoding == FT_ENCODING_UNICODE)
    {
        FT_UInt glyph_code;
        FT_ULong char_code;

        glyph_code = 0;
        char_code = FT_Get_First_Char(face, &glyph_code);

        while (glyph_code)
        {
            if (glyph_code <= 65535 && !glyph_to_char[glyph_code])
                glyph_to_char[glyph_code] = (USHORT) char_code;

            char_code = FT_Get_Next_Char(face, char_code, &glyph_code);
        }
    } else
    {
        ULONG n;
        for (n = 0; n <= 65535; ++n)
            glyph_to_char[n] = (USHORT) n;
    }
    ADC dc;
    DWORD ppem = calc_ppem_for_height(face, MulDiv(size, dc.GetDeviceCaps(LOGPIXELSY), (int) 72));
    tt_kern_table = (TT_kern_table *) buf.GetPtr();
    nTables = GET_BE_WORD(tt_kern_table->nTables);
    tt_kern_subtable = (const struct TT_kern_subtable *) (tt_kern_table + 1);
    int total_kern_pairs = -1;
    for (i = 0; i < nTables; i++)
    {
        struct TT_kern_subtable tt_kern_subtable_copy;

        tt_kern_subtable_copy.version = GET_BE_WORD(tt_kern_subtable->version);
        tt_kern_subtable_copy.length = GET_BE_WORD(tt_kern_subtable->length);
        tt_kern_subtable_copy.coverage.word = GET_BE_WORD(tt_kern_subtable->coverage.word);
        if (tt_kern_subtable_copy.coverage.bits.format == 0)
        {
            if (total_kern_pairs == -1)
                total_kern_pairs = 0;
            DWORD new_chunk, old_total = total_kern_pairs;

            new_chunk = parse_format0_kern_subtable(face,
                                                    (const struct TT_format0_kern_subtable *) (
                                                            tt_kern_subtable + 1),
                                                    glyph_to_char.GetPtr(), NULL, 0, ppem);
            total_kern_pairs += new_chunk;

            kern_pair.SetLen(total_kern_pairs);
            parse_format0_kern_subtable(face, (const struct TT_format0_kern_subtable *) (
                                                tt_kern_subtable + 1),
                                        glyph_to_char.GetPtr(), kern_pair.GetPtr() + old_total,
                                        new_chunk, ppem);
        }
        tt_kern_subtable = (const struct TT_kern_subtable *) ((const char *) tt_kern_subtable +
                                                              tt_kern_subtable_copy.length);
    }
    if (sec) sec->Leave();
    return total_kern_pairs == -1 ? 0 : total_kern_pairs;
}
