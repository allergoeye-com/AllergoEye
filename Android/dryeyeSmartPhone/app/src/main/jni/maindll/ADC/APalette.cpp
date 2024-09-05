/*
 * APalette.cpp
 *
 *  Created on: Sep 3, 2014
 *      Author: alex
 */


#include "stdafx.h"
#include "APalette.h"

//-------------------------------------
//
//-------------------------------------
APalette::APalette(int type): AHandle(MPALETTE)
{
    def = type == DESKTOP_PAL ? 1 : 0;
    if (type == PAL_2)
    {
		palette.SetLen(2);
		palette[0].color = 0;
		palette[1].color = 0xFFFFFF;
	}
    if (type == PAL_16)
    {

		DWORD pal[16] = {
					BLACK,
					RED,
					GREEN,
					BROWN,
					BLUE,
					MAGENTA,
					CYAN,
					DARKGRAY,
					LIGHTGRAY,
					LIGHTRED,
					LIGHTGREEN,
					YELLOW,
					LIGHTBLUE,
					LIGHTCYAN,
					LIGHTMAGENTA,
					WHITE
		};
		palette.Copy((ColorRef *)pal, 16);
		
    }
    else
    {
		palette.SetLen(256);
		ColorRef *p = palette.GetPtr();
		BYTE red, green, blue;
		red = green = blue = 0;
		for (int i = 0; i < 256; ++i)
		{
			p[i].r = red;
			p[i].b = blue;
			p[i].g = green;
			  if (!(red += 32))
				  if (!(green += 32))
					  blue += 64;
		}
		GetDeafaultColors((DWORD *)p + 236);

	}


}
APalette::APalette(BYTE *ptr_image, ImageInfo &info):AHandle(MPALETTE)
{
	MArray<COLORREF> c;
   CreateRGBPalette(ptr_image, info.width, info.height, 236, c);
   palette.Copy((ColorRef *)c.GetPtr(), c.GetLen());
   palette.SetLen(256);
	GetDeafaultColors((DWORD *)palette.GetPtr() + 236);
}
void APalette::GetDeafaultColors(DWORD *p)
{
	DWORD d[20] = {0x0, 0x80, 0x8000, 0x8080, 0x800000, 0x800080, 0x808000, 0xc0c0c0, 0xc0dcc0, 0xf0caa6, 0xf0fbff, 0xa4a0a0, 0x808080, 0xff, 0xff00, 0xffff, 0xff0000, 0xff00ff, 0xffff00, 0xffffff };
	memcpy(p, d, 20 * sizeof (DWORD));

}
//-------------------------------------
//
//-------------------------------------
APalette::APalette(LOGPALETTE *lpLogPal):AHandle(MPALETTE)
{
    palette = 0;

    def = 0;
    CreatePalette(lpLogPal);
	
}

//-------------------------------------
//
//-------------------------------------
void APalette::CreatePalette(LOGPALETTE *lpLogPal)
{
	if (lpLogPal == 0L)
		return;
   palette.Clear();
    def = 0;
    int colors;
	colors = lpLogPal->palNumEntries;
    palette.SetLen(colors);
	ColorRef *p = palette.GetPtr();

	for (int i = 0; i < colors; ++i)
	{
		p[i].r = (BYTE)lpLogPal->palPalEntry[i].peRed;
        p[i].g = (BYTE)lpLogPal->palPalEntry[i].peGreen;
        p[i].b = (BYTE)lpLogPal->palPalEntry[i].peBlue;
	}

 }
//-------------------------------------
//
//-------------------------------------
APalette::APalette(DWORD *pal, int colors):AHandle(MPALETTE)
{
    def = 0;
    palette.Copy((ColorRef *)pal, colors);


}
//-------------------------------------
//
//-------------------------------------
APalette::APalette(APalette& orig): AHandle(MPALETTE)
{
	    
    def = 0;
    palette = orig.palette;

}
//-------------------------------------
//
//-------------------------------------
APalette::APalette(APalette *orig): AHandle(MPALETTE)
{
    def = 0;
    palette = orig->palette;

}

//-------------------------------------
//
//-------------------------------------
APalette::~APalette()
{
}
//-------------------------------------
//
//-------------------------------------
int APalette::GetNumColors()
{
    return def  ? 0 : palette.GetLen(); 
}
//-------------------------------------
//
//-------------------------------------
BOOL APalette::GetPaletteEntries( int start, int end, PALETTEENTRY  *pl)
{
    if (palette.GetLen())
    {
        int colors = palette.GetLen(); 
        if (colors < end) end = colors;
        ColorRef *p = palette.GetPtr();
        for (int k = 0, i = start; i < end; ++i, ++k)
        {
           pl[k].peBlue = p[i].b;
           pl[k].peGreen = p[i].g;
           pl[k].peRed = p[i].r;
           pl[k].peFlags = 0;
        }
        return true;
    }
    return false;
    
}

//-------------------------------------
//
//-------------------------------------
BOOL APalette::GetRGBPalette(DWORD *pp, UINT len)
{
    if (palette.GetLen())
    {
        int colors = palette.GetLen();
        if (colors < (int)len) len = colors;
		memcpy (pp, palette.GetPtr(), sizeof(DWORD) * len);
        return colors;
    }
    return false;
}
//-------------------------------------
//
//-------------------------------------
int APalette::GetIndex(COLORREF rgb)
{
	
	ColorRef *clr = palette.GetPtr();
	ColorRef cr;
	cr.color = rgb;
	int dz, z;

	int curent = 0;
	z =  ((int)cr.b - (int)clr[0].b) * ((int)cr.b - (int)clr[0].b) + ((int)cr.g - (int)clr[0].g) * ((int)cr.g - (int)clr[0].g) + ((int)cr.r - (int)clr[0].r) * ((int)cr.r - (int)clr[0].r);
	
	for (int i = 1, l = palette.GetLen(); i < l; ++i)
	{
		dz =  ((int)cr.b - (int)clr[i].b) * ((int)cr.b - (int)clr[i].b) + ((int)cr.g - (int)clr[i].g) * ((int)cr.g - (int)clr[i].g) + ((int)cr.r - (int)clr[i].r) * ((int)cr.r - (int)clr[i].r);
		if (dz < z)
		{
			z = dz;
			curent = i;
			
		}
	}
	
    return curent;
}
//-------------------------------------
//
//-------------------------------------
DWORD APalette::GetRGB(int index)
{
int colors;
    if ((colors = palette.GetLen()))
    {
        if (index < 0 || colors < index) index = 0;
        return palette[index].color & 0xFFFFFF;
    }
    return 0;
}
//-------------------------------------
//
//-------------------------------------
void APalette::SetEntries(LPLONG rgb, int first, int num)
{
     int colors = palette.GetLen();
     if (first >= colors) return;
     palette.Replace(first, (ColorRef*)rgb, num);
    
   
}
