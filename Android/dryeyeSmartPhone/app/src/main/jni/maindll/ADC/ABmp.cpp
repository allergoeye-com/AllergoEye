/*
 * ABmp.cpp
 *
 *  Created on: Aug 27, 2014
 *      Author: alex
 */

#include "stdafx.h"

#include "ABmp.h"
#include "JBitmap.h"


//-----------------------------------------------------
//
//-----------------------------------------------------
tagABITMAP::tagABITMAP(tagGDC *s) : tagMBITMAP(s, HBMP)
{
	m_hBmp = 0;
	m_data = 0;
	memset(&info, 0, sizeof (AndroidBitmapInfo));
}
//-----------------------------------------------------
//
//-----------------------------------------------------
tagABITMAP::tagABITMAP(tagGDC *s, int x, int y) : tagMBITMAP(s, HBMP)
{

	m_data = 0;
	m_hBmp = (DWORD *)HMALLOC(x * y * sizeof(DWORD));
	info.height = y;
	info.width = info.width = x;
	info.wline = info.width;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
tagABITMAP::tagABITMAP(tagGDC *s, MArray<DWORD> &data, int x, int y) : tagMBITMAP(s, HBMP) //data returned empty
{
	m_data = 0;
	m_hBmp = data.GetPtr();
	info.height = y;
	info.width = info.width = x;
	info.wline = info.width;
	data.Detach();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
DWORD *tagABITMAP::SetAccess()
{
	++count;
	if (m_data) return m_data;
	m_data = m_hBmp;
	return m_data;
	
}
//---------------------------------------------------
//
//--------------------------------------------------
BOOL tagABITMAP::SetSize(int cx, int cy, int)
{
	ABmp *data = 0;
	MemImage img;

	if (m_hBmp)
	{
		if (info.width == cx && info.height == cy) return true;
		data = new ABmp(info.width, info.height);
		SetAccess();
		img.Init(m_data, info);
		data->SetAccess();
		data->Show(img, 0, 0, SRCCOPY);
		img.Release();
		ReleaseAccess();
		m_hBmp = 0;
	}
	m_hBmp = (DWORD *)HMALLOC(cx * cy * sizeof(DWORD));
	info.height = cy;
	info.width = cx;
	info.wline = cx;
	if (data)
	{
		SetAccess();
		img.Init(m_data, info);
		img.Show(*data, 0, 0, SRCCOPY);
		ReleaseAccess();
		data->ReleaseAccess();
		delete data;
		return true;

	}
	return FALSE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL tagABITMAP::ReleaseAccess()
{
	if (count)
		--count;

	if (!count)
	{
		if (m_data)
			m_data = 0;
	}
	return true;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
tagABITMAP::~tagABITMAP()
{
	if (m_hBmp)
		HFREE(m_hBmp);
}
//------------------------------------------------
//
//------------------------------------------------
tagABITMAP &tagABITMAP::Copy(const tagABITMAP &b)
{
	if (m_hBmp)
	{
		HFREE(m_hBmp);
		m_hBmp = 0;
	}
	info = b.info;
	if ((m_hBmp = (DWORD *)HMALLOC(info.height * info.wline * sizeof(DWORD))))
		memcpy(m_hBmp, b.m_hBmp, info.height * info.wline * sizeof(DWORD));
	if (!m_hBmp)
		memset(&info, 0, sizeof (ImageInfo));
	m_data = 0;
	return *this;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
ABmp::ABmp()
{
	fInverse = false;
	palette = 0;
	type = BMP;
	palette = 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
ABmp::ABmp(int x, int y, APalette *pal)
{
	fInverse = false;
	type = BMP;
	if (pal)
		palette = new APalette(pal);
	else
		palette = new APalette();
	ABITMAP bmp = new tagABITMAP(this, x, y);
	surface = bmp;
	if (bmp)
	{
		width = bmp->info.width;
		height = bmp->info.height;

	}
	palette->SetDef(pal == 0);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
void ABmp::DeleteImage()
{
	if (IsHandle())
		delete dynamic_cast<ABITMAP> (surface);
	if (palette) delete palette;
	palette = 0;	
	surface = 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
ABmp::~ABmp()
{
	DeleteImage();
}
//-----------------------------------------------------
//
//-----------------------------------------------------
ABmp::ABmp(ABmp &bmp)
{
	fInverse = false;
	palette = 0;
	type = BMP;
	palette = 0;

	ABITMAP p = dynamic_cast<ABITMAP>(bmp.surface);
	if (!p) return;
	DWORD *src = p->SetAccess();
	if (src)
	{
		surface = new tagABITMAP(this, bmp.width, bmp.height);
		ABITMAP abmp = dynamic_cast<ABITMAP> (surface);
		if (abmp)
		{
			width = abmp->info.width;
			height = abmp->info.height;
			palette = new APalette(*bmp.palette);
			DWORD *dst = abmp->SetAccess();
			memcpy(dst, src, bmp.height * bmp.info.wline * sizeof(DWORD));
			abmp->ReleaseAccess();
		}
		p->ReleaseAccess();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::Create(int x, int y)
{
	if (IsHandle())
	{
		ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
		width = bmp->info.width;
		height = bmp->info.height;
		if (width == x && height == y)
			return TRUE;
	}

	if (IsHandle())
	{
		delete dynamic_cast<ABITMAP> (surface);
		if (palette) delete palette;
		palette = 0;
	}
	surface = new tagABITMAP(this, x, y);
	ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
	if (bmp)
	{
		width = bmp->info.width;
		height = bmp->info.height;
		if (!palette)
			palette = new APalette();
	}
	return IsHandle();

}
BOOL ABmp::SetSize(int x, int y)
{
	if (IsHandle())
	{
		ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
		if (bmp)
		if (bmp->SetSize(x, y))
		{
			width = bmp->info.width;
			height = bmp->info.height;
			return true;
		}

	}
	return Create(x, y);



}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::Attach(ABITMAP g)
{
	if (IsHandle())
		delete dynamic_cast<ABITMAP> (surface);
	surface = g;
	ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
	if (bmp)
	{
		width = bmp->info.width;
		height = bmp->info.height;
	}
	if (!palette)
		palette = new APalette();
	return IsHandle();
}
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::Detach()
{
	surface = 0;
	if (palette)
		delete palette;
	palette = 0;
	return true;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::Load(const char *filename)
{
	DeleteImage();
	TString name;
	name = filename;
	int i = name.ReverseFind('.');
	TString type = name.Right(name.StrLen() - i - 1);
	type.Upper();
	if (type == "BMP" || type == "BMC")
		return WinBmp::Load(filename);
#ifndef WIN32
	else
	{
		JBitmap b;
		if (b.Load(filename))
		{
			MArray<DWORD> dw, dw2;
			ImageInfo info;
			b.GetData(dw, info);
            dw2 = dw;
            MemImage img, img2;
			fInverse = false;
            img.Init((DWORD *)dw.GetPtr(), info);
            img2.Init((DWORD *)dw2.GetPtr(), info);

            for (int y = 0; y < info.height; ++y)
                for (int x = 0; x < info.width; ++x)
                {

                    ColorRef *p = img(x, y);
                    if (!p->a)
                    {
                        DWORD color = 0xFFFFFFFF;
                        if (x > 0)
                            if (img2(x - 1, y)->color == 0xFFFFFFFF)
                                color = 0xFF000000;

                        if (color != 0xFF000000 && x < info.width - 1)
                            if (img2(x + 1, y)->color == 0xFFFFFFFF)
                                color = 0xFF000000;
                        if (color != 0xFF000000 && y > 0)
                            if (img2(x, y - 1)->color == 0xFFFFFFFF)
                                color = 0xFF000000;
                        if (color != 0xFF000000 && y < info.height - 1)
                            if (img2(x, y + 1)->color == 0xFFFFFFFF)
                                color = 0xFF000000;
                        p->color = color;
                    }


                }
            dw2.Clear();
			ABITMAP bmp = new tagABITMAP(this, dw, info.width, info.height);
			surface = bmp;
			if (bmp)
			{
				width = bmp->info.width;
				height = bmp->info.height;

			}
			palette = new APalette();
			palette->SetDef(1);
			return true;
		}
	}
	#endif
	return false;

}
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::Save(const char *filename)
{
	TString name;

	name = filename;
	int i = name.ReverseFind('.');
	TString type = name.Right(name.StrLen() - i - 1);
	type.Upper();
	if (type == "PNG");
	else
	if (type == "JPG" || type == "JPEG")
		type = "JPEG";
	else
		return WinBmp::Save(filename);
#ifndef WIN32		
	return JBitmap::Save(*this, filename, type.GetPtr());
#else
	return 0;
#endif
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::GetRGB(char *&lpBits, int &len)
{
	BOOL ret = 0;
	if (IsHandle())
	{
		ABITMAP p = dynamic_cast<ABITMAP> (surface);
		DWORD *src = p->SetAccess();
		if (src)
		{

			int _width = WIDTHBYTES(p->info.width * 24);
			int y = p->info.height;
			if (!lpBits)
			{
				lpBits = (char *)HMALLOC(_width * y);
				if (!lpBits) return 0;
				len = _width * y;
			}
			MArray<char> c;
			MemImage img(src, p->info);
			c.Attach(lpBits, len, false);
			img.GetRGB(c);
			c.Detach();
			ret =  TRUE;
			p->ReleaseAccess();
		}

	}
	return ret;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::SetRGB(char *lpBits, int len)
{
	BOOL ret = 0;
	if (IsHandle())
	{
		ABITMAP p = dynamic_cast<ABITMAP> (surface);
		DWORD *src = p->SetAccess();
		if (src)
		{
			MArray<char> c;
			MemImage img(src, p->info);
			c.Attach(lpBits, len, false);
			img.SetRGB(c);
			c.Detach();
			p->ReleaseAccess();
			ret =  TRUE;
		}

	}
	return ret;
}


//-----------------------------------------------------
//
//-----------------------------------------------------
int ABmp::GetHeight()
{

	return height;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
int ABmp::GetWidth()
{

	return width;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::SetAccess()
{
	ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
	if (!bmp) return 0;
	DWORD *ptr = bmp->SetAccess();

	if (!ptr) return 0;
	MemImage::Init(ptr, bmp->info);
	return ptr != 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::ReleaseAccess()
{
	ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
	if (!bmp) return 0;
	bmp->ReleaseAccess();
	if (!bmp->count) ptr = 0;
	return true;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
DWORD *ABmp::AttachData(int x, int y, ImageInfo &info)
{
	if (Create(x, y))
	{
		ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
		if (bmp)
		{
			DWORD *src = bmp->SetAccess();
			if (src)
				info = bmp->info;
			return src;
		}
	}
	return 0;
}
DWORD *ABmp::AttachData(ImageInfo &info)
{
	ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
	if (bmp)
	{
		DWORD *src = bmp->SetAccess();
		if (src)
			info = bmp->info;
		return src;
	}
	return 0;
}
bool ABmp::IsAttachedData() 
{ 
	ABITMAP bmp = dynamic_cast<ABITMAP> (surface);
	return bmp != 0 && bmp->m_data != 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL ABmp::DetachData()
{
	return ReleaseAccess();
}
//-------------------------------------
//
//-------------------------------------
ADIBSECTION::~ADIBSECTION()
{
	Release();
}
void ADIBSECTION::Release()
{
	if (pBmp && dib)
	{
		UpdateBitmap();
		GlobalFree(dib);

	}
    pBmp = 0;
    dib = 0;
}
void ADIBSECTION::UpdateBitmap()
{
	if (pBmp && dib && dsBm.bmBits)
	pBmp->SetBitmapBits((char *)dsBm.bmBits, dsBm.bmWidthBytes * dsBm.bmHeight);
}
//-------------------------------------
//
//-------------------------------------
EXPORTNIX BOOL GetObject(ABmp *hbitmap, int, BITMAP *db)
{
    if (!hbitmap || !hbitmap->IsHandle()) return 0;
	ABITMAPINFOHEADER   bmInfoHdr;
	hbitmap->InitBitmapInfoHeader (&bmInfoHdr,
	                     hbitmap->width,
	                     hbitmap->height,
	                     hbitmap->palette->GetNumColors(), hbitmap->palette->IsDef());

    db->bmType = DIB_HEADER_MARKER;
    db->bmWidth = bmInfoHdr.biWidth;
    db->bmHeight = bmInfoHdr.biHeight;
    db->bmWidthBytes = WIDTHBYTES(bmInfoHdr.biWidth * hbitmap->BitCount());
    db->bmPlanes = bmInfoHdr.biPlanes;
    db->bmBitsPixel = hbitmap->BitCount();
    db->bmBits = 0;
    return 1;
}

//-------------------------------------
//
//-------------------------------------
EXPORTNIX BOOL GetObject(ABmp *hbitmap, int, ADIBSECTION *db)
{
    if (!hbitmap || !hbitmap->IsHandle()) return 0;
    HGLOBAL gl = hbitmap->BitmapToDIB (false);
    if (db->dib)
        GlobalFree(db->dib);
    BITMAPINFOHEADER *hd = (BITMAPINFOHEADER *)(gl);
    db->dsBm.bmType = DIB_HEADER_MARKER;
    db->dsBm.bmWidth = hd->biWidth;
    db->dsBm.bmHeight = hd->biHeight;
    db->dsBm.bmWidthBytes = WIDTHBYTES(hd->biWidth * hbitmap->BitCount());
    db->dsBm.bmPlanes = hd->biPlanes;
    db->dsBm.bmBitsPixel = hbitmap->BitCount();
    db->dsBm.bmBits = (BYTE  *)hbitmap->FindDIBBits((char  *)hd);
    db->dsBmih = *hd;
    db->dib = gl;
	db->pBmp = hbitmap;
  return 1;
}
ADIBSECTION &ADIBSECTION::operator = (ADIBSECTION &b)
{
	dsBm = b.dsBm;
	dsBmih = b.dsBmih;
    return *this;

}
EXPORTNIX int GetDIBits(void *hdc, ABmp *hbm, int start, int cLines,  LPVOID lpvBits, LPBITMAPINFO lpbmi, UINT)
{
	int fRes = 0;
	char *lpBits = 0;
	int len = 0;
	hbm->GetDibBits(lpBits, len);
	if (len)
	{

		int _width = WIDTHBYTES(hbm->GetWidth() * hbm->BitCount());
		int offset = _width * start;
		cLines = min(cLines, hbm->GetHeight() - start);
		if (cLines > 0)
		{
			fRes = cLines;
			memcpy(lpvBits, lpBits + offset, cLines * _width);
		}
		HFREE(lpBits);
	}
	return fRes;

}
void ABmp::Inverce()
{
    BOOL ret = 0;
    if (IsHandle())
    {
        ABITMAP p = dynamic_cast<ABITMAP> (surface);
        DWORD *src = p->SetAccess();
        if (src)
        {

            int _width = p->info.width * sizeof(DWORD);
            int y = p->info.height;
			int LineLen = _width;
			char *buff = new char [LineLen];
			if (buff)
			{
				memset(buff, 0, LineLen);
				for (int i = 0; i < y / 2; ++i)
				{
					memcpy(buff,ptr + LineLen * i, LineLen);
					memcpy(ptr + LineLen * i, ptr + (int)LineLen * (y - 1 - i), LineLen);
					memcpy(ptr + LineLen * (y - 1 - i), buff, LineLen);
				}
				delete [] buff;
			}

			p->ReleaseAccess();
        }

    }

}