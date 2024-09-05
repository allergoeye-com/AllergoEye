#include "stdafx.h"
#include "AColor.h"
#include "MemImage.h"
#include "ABmp.h"

//----------------------------------------------
//
//----------------------------------------------
tagMBITMAP::tagMBITMAP(tagGDC *s, TYPE_AHANDLE t) : AHandle(t)
{
    m_data = 0;
    self = s;
    count = 0;
}

//----------------------------------------------
//
//----------------------------------------------
tagMBITMAP::~tagMBITMAP()
{
    if (self)
        self->OnDeleteSurface();

}

//----------------------------------------------
//
//----------------------------------------------
void tagMBITMAP::OnDeleteContext()
{
    self = 0;
}

#if (1) //ndef WIN32
static AHandle *table[COUNT_STOCKOBJECTS] = {0};

//----------------------------------------------
//
//----------------------------------------------
void DeleteObject(AHandle *h)
{
    for (int i = 0; i < COUNT_STOCKOBJECTS; ++i)
        if (h == table[i]) return;
    if (h) delete h;
}

///////////////////////////////////////
EXPORTNIX AHandle *AGetStockObject(STOCKOBJECTS i)
{
    switch (i)
    {
        case SYSTEM_FONT :
            if (!table[SYSTEM_FONT])
            {

                AFont *f = new AFont(true);
                f->Create("ASSETS/DroidSansFallbackFull.ttf");
                f->SetSize(14);
                table[SYSTEM_FONT] = f;
            }
            break;
        case WHITE_BRUSH:
            if (!table[WHITE_BRUSH])
                table[WHITE_BRUSH] = new ABGColor(RGB(255, 255, 255));
            break;
        case LTGRAY_BRUSH:
            if (!table[LTGRAY_BRUSH])
                table[LTGRAY_BRUSH] = new ABGColor(RGB(192, 192, 192));
            break;
        case GRAY_BRUSH:
            if (!table[GRAY_BRUSH])
                table[GRAY_BRUSH] = new ABGColor(RGB(128, 128, 128));
            break;
        case DKGRAY_BRUSH:
            if (!table[DKGRAY_BRUSH])
                table[DKGRAY_BRUSH] = new ABGColor(RGB(100, 100, 100));
            break;
        case BLACK_BRUSH:
            if (!table[BLACK_BRUSH])
                table[BLACK_BRUSH] = new ABGColor(RGB(0, 0, 0));
            break;
#ifndef WIN32
        case HOLLOW_BRUSH:
            i = NULL_BRUSH;
#endif

        case NULL_BRUSH:

            if (!table[NULL_BRUSH])
            {
                ABGColor *bg = new ABGColor(RGB(0, 0, 0), 0);
                table[NULL_BRUSH] = bg;
            }

            break;
        case WHITE_PEN:
            if (!table[WHITE_PEN])
                table[WHITE_PEN] = new AFGColor(RGB(255, 255, 255));
            break;
        case BLACK_PEN:
            if (!table[BLACK_PEN])
                table[BLACK_PEN] = new AFGColor(RGB(0, 0, 0));
            break;
        case NULL_PEN:
        {
            if (!table[NULL_PEN])
            {
                AFGColor *fg = new AFGColor(RGB(0, 0, 0), 1, 0);
                table[NULL_PEN] = fg;
            }
            break;

        }
            break;

        default:
            return 0;
    }
    return table[i];
}

#endif

//----------------------------------------------
//
//----------------------------------------------
void ImageInfo2::CopyPalette(APalette *p)
{
    if (palette)
        delete palette;
    palette = 0;
    if (p)
        palette = new APalette(p);
}

//----------------------------------------------
//
//----------------------------------------------
void ImageInfo2::DeletePicture()
{
    image.Clear();
    if (palette) delete palette;
    palette = 0;
}

//----------------------------------------------
//
//----------------------------------------------
DWORD *ImageInfo2::AttachData(int x, int y, ImageInfo &info)
{
    width = wline = x;
    height = y;

    info.width = info.wline = x;
    info.height = y;
    image.SetLen(x * y);
    return image.GetPtr();
}

//----------------------------------------------
//
//----------------------------------------------
ImageInfo2::ImageInfo2()
{
    palette = 0;
}

//----------------------------------------------
//
//----------------------------------------------
BOOL ImageInfo2::Load(LPBITMAPINFOHEADER p)
{
    palette = 0;
    return CreateBmp((char *) p);
}

//----------------------------------------------
//
//----------------------------------------------
ImageInfo2::ImageInfo2(const ImageInfo2 &a)
{
    palette = 0;
    *this = a;
}

//----------------------------------------------
//
//----------------------------------------------
ImageInfo2 &ImageInfo2::operator=(const ImageInfo2 &a)
{
    width = a.width;
    height = a.height;
    wline = a.wline;
    image = a.image;
    CopyPalette(a.palette);
    return *this;
}

//----------------------------------------------
//
//----------------------------------------------
BOOL tagGBRUSH::CreatePatternBrush(LPBITMAPINFOHEADER p)
{
    pattern = new ImageInfo2();
    typeBrush = BG_IMG;
    color = 0xFFFFFFFF;
    if (!pattern->Load(p))
    {
        typeBrush = BG_COLOR;
        delete pattern;
        pattern = 0;

    }
    return pattern != 0;
}

//----------------------------------------------
//
//----------------------------------------------
tagGBRUSH::tagGBRUSH(ABmp *b) : AHandle(HBGCOLOR)
{
    pattern = 0;
    ABITMAP bmp = (ABITMAP) *b; //->Handle();
    color = 0xFFFFFFFF;
    if (bmp)
    {
        bmp->SetAccess();
        if (bmp->m_data)
        {
            pattern = new ImageInfo2();
            typeBrush = BG_IMG;
            pattern->Set(bmp->info.width, bmp->info.height, bmp->info.wline);
            pattern->image.Copy(bmp->m_data, pattern->height * pattern->wline);
        }
        bmp->ReleaseAccess();
    }
    if (!pattern)
        typeBrush = BG_COLOR;
}


//----------------------------------------------
//
//----------------------------------------------
tagGBRUSH::ListPattern::ListPattern()
{
    len = 6;
    pattern = (unsigned char **) malloc(len * sizeof(unsigned char *));
    for (int i = 0; i < len; ++i)
        pattern[i] = (unsigned char *) malloc(8);

    //			BDIAGONAL
    pattern[HS_BDIAGONAL][0] = 0x80;
    pattern[HS_BDIAGONAL][1] = 0x40;
    pattern[HS_BDIAGONAL][2] = 0x20;
    pattern[HS_BDIAGONAL][3] = 0x10;
    pattern[HS_BDIAGONAL][4] = 0x08;
    pattern[HS_BDIAGONAL][5] = 0x04;
    pattern[HS_BDIAGONAL][6] = 0x02;
    pattern[HS_BDIAGONAL][7] = 0x01;

    //			HS_CROSS
    memset(pattern[HS_CROSS], 0x08, 8);
    pattern[HS_CROSS][3] = 0xff;

    //			DIAGCROSS
    pattern[HS_DIAGCROSS][0] = 0x81;
    pattern[HS_DIAGCROSS][1] = 0x42;
    pattern[HS_DIAGCROSS][2] = 0x24;
    pattern[HS_DIAGCROSS][3] = 0x18;
    pattern[HS_DIAGCROSS][4] = 0x18;
    pattern[HS_DIAGCROSS][5] = 0x24;
    pattern[HS_DIAGCROSS][6] = 0x42;
    pattern[HS_DIAGCROSS][7] = 0x81;

    //			HS_FDIAGONAL
    pattern[HS_FDIAGONAL][0] = 0x01;
    pattern[HS_FDIAGONAL][1] = 0x02;
    pattern[HS_FDIAGONAL][2] = 0x04;
    pattern[HS_FDIAGONAL][3] = 0x08;
    pattern[HS_FDIAGONAL][4] = 0x10;
    pattern[HS_FDIAGONAL][5] = 0x20;
    pattern[HS_FDIAGONAL][6] = 0x40;
    pattern[HS_FDIAGONAL][7] = 0x80;

    //			HORIZONTAL
    memset(pattern[HS_HORIZONTAL], 0x0, 8);
    pattern[HS_HORIZONTAL][3] = 0xff;
    //			VERTICAL
    memset(pattern[HS_VERTICAL], 0x08, 8);
    pattern[HS_VERTICAL][3] = 0xff;

}

//----------------------------------------------
//
//----------------------------------------------
tagGBRUSH::ListPattern::~ListPattern()
{
    for (int i = 0; i < len; ++i)
        free(pattern[i]);
    free(pattern);
}

//----------------------------------------------
//
//----------------------------------------------
int tagGBRUSH::ListPattern::Add(unsigned char pat[8])
{
    pattern = (unsigned char **) realloc(pattern, (len + 1) * sizeof(unsigned char *));
    pattern[len] = (unsigned char *) malloc(8);
    memmove(pattern[len], pat, 8);
    ++len;
    return len - 1;
}

tagGBRUSH::ListPattern listPattern;

//----------------------------------------------
//
//----------------------------------------------
BOOL tagGBRUSH::CreatePatternBrush(const char *filename)
{
    SetColor(0);
    pattern = new ImageInfo2();

    typeBrush = BG_IMG;
    if (!pattern->Load(filename))
    {
        typeBrush = BG_COLOR;
        delete pattern;
        pattern = 0;
    }
    return pattern != 0;
}

//----------------------------------------------
//
//----------------------------------------------
BOOL tagGBRUSH::CreateHatchBrush(int type, COLORREF bg, COLORREF fg)
{
    SetColor(0);
    if (type > 5)
    {
        CreateSolidBrush(color);
        return true;
    }
    return CreateHatchBrush(listPattern.pattern[type], bg, fg);
}

//----------------------------------------------
//
//----------------------------------------------
BOOL tagGBRUSH::CreateHatchBrush(const BYTE pattern[8], COLORREF bg, COLORREF fg)
{
    BYTE *ptr = (BYTE *) HMALLOC(sizeof(BITMAPINFOHEADER) + sizeof(int) + 192);
    if (!ptr)
    {
        CreateSolidBrush(bg);
        return false;
    }

    BYTE *buff = ptr + sizeof(BITMAPINFOHEADER);
    for (int k = 0, i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
            if (((pattern[i] >> j) & 1))
            {
                buff[k++] = GetBValue(fg);
                buff[k++] = GetGValue(fg);
                buff[k++] = GetRValue(fg);
            } else
            {
                buff[k++] = GetBValue(bg);
                buff[k++] = GetGValue(bg);
                buff[k++] = GetRValue(bg);
            }
    }
    LPBITMAPINFOHEADER lpBmInfoHdr = (LPBITMAPINFOHEADER) ptr;
    memset(lpBmInfoHdr, 0, sizeof(BITMAPINFOHEADER));
    lpBmInfoHdr->biSize = sizeof(BITMAPINFOHEADER);
    lpBmInfoHdr->biWidth = 8;
    lpBmInfoHdr->biHeight = 8;
    lpBmInfoHdr->biPlanes = 1;
    lpBmInfoHdr->biBitCount = 24;
    lpBmInfoHdr->biSizeImage = 192;
    CreatePatternBrush(lpBmInfoHdr);
    HFREE(ptr);

    return pattern != 0;
}

