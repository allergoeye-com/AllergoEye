/*
 * APalette.h
 *
 *  Created on: Sep 3, 2014
 *      Author: alex
 */

#ifndef APALETTE_H_
#define APALETTE_H_
#ifdef WIN32
//#include "TM_DEF.H"
//#include "MPalette.h"
#ifndef WIN32
enum PAL_TYPE { PAL_2, PAL_16, DESKTOP_PAL, RGB_PAL };
#define PALVERSION   0x300
#endif
#else
#define PALVERSION   0x300
enum PAL_TYPE { PAL_2, PAL_16, DESKTOP_PAL, RGB_PAL };
#endif
#include "AColor.h"
#ifndef BI_RGB 
#define BI_RGB 0
#endif

#pragma pack(push)
#pragma pack(8)
EXPORTNIX void CreateRGBPalette(BYTE *ptr_image, int width, int height, int size_pal, MArray<COLORREF> &palette);

EXPORTNIX class EXPMDC APalette : public AHandle {

public:

    APalette(int type = DESKTOP_PAL);
    APalette(LOGPALETTE *pal);
    APalette(DWORD *pal, int numColors);
    APalette(BYTE *bmp, ImageInfo &info);

    APalette(APalette& orig);
    APalette(APalette *orig);
    virtual ~APalette();
    void CreatePalette(LOGPALETTE *pal);
    BOOL IsDef() { return def; }
    BOOL SetDef(int d) { return def = d; }
    int GetNumColors();
    //===========================================================
    //DWORD *pp - buffer to copy palette
    //UINT len - buffer length
    // return 1 on success, other way 0
    BOOL GetRGBPalette(DWORD *pp, UINT len);
    BOOL GetPaletteEntries( int, int, PALETTEENTRY  *alpha_pl);
	void SetEntries(LPLONG rgb, int first, int num);

    int GetIndex(COLORREF rgb);
    DWORD GetRGB(int index);
    operator MArray<ColorRef> () const {return palette; }
protected:
	void GetDeafaultColors(DWORD *p);


    BOOL def;  
    MArray<ColorRef> palette; 
 
};
#pragma pack(pop)

typedef APalette * GPALETTE;
typedef APalette * APALETTE;



#endif /* APALETTE_H_ */
