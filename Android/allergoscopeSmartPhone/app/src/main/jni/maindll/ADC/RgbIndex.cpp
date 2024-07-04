/*
 * RgbIndex.cpp
 *
 *  Created on: Dec 3, 2015
 *      Author: alex
 */
#include "stdafx.h"
#include "RgbIndex.h"

//------------------------------------------
//
//------------------------------------------
int __cdecl CompRgb2(const void *elem1, const void *elem2)
{
    DWORD *a = (DWORD *) elem1;
    DWORD *b = (DWORD *) elem2;
    if (*a > *b)
        return 1;
    else if (*a < *b)
        return -1;
    return 0;
}

//------------------------------------------
//
//------------------------------------------
void RgbIndex::Sort(int *dst, int *help_buff, int num)
{
    memset((char *) help_buff, 0, num * sizeof(int));
    int *_dst = dst;
    BYTE *_ptr = ptr + shift;
    int *_help_buff = help_buff + 1;
    int i;
    for (i = 0; i < count; ++i)
        ++*(_help_buff + *(_ptr + *(_dst++)));
    int n = 0;
    _help_buff = help_buff;
    for (i = 0; i < num; ++i)
    {
        *_help_buff += n;
        n = *(_help_buff++);
    }
    _dst = dst;
    for (i = 0; i < count; ++i, ++_dst)
        *(index + (*(help_buff + *(_ptr + *_dst)))++) = *_dst;
    SetRange();

}

//------------------------------------------
//
//------------------------------------------
BYTE RgbIndex::GetColor()
{
    DWORD clr = 0;
    BYTE *_ptr = ptr + shift;
    int *_index = index;
    for (int i = 0; i < count; ++i)
        clr += *(_ptr + *(_index++));
    if (count)
        return (BYTE) (clr / count);
    else
        return *(ptr + *index);
}

//------------------------------------------
//
//------------------------------------------
void CreateRGBPalette(RgbIndex (*rgb)[3], int width, int height, int N)
{

    int i, j;
    RgbIndex *red = &(rgb[0][0]);
    RgbIndex *green = &(rgb[0][1]);
    RgbIndex *blue = &(rgb[0][2]);

    int k = 0, n = 0, m = (width * 3) % 4 ? 4 - (width * 3) % 4 : 0;

    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
        {
            blue->index[n] = k;
            k += 3;
            ++n;
        }
        k += m;
    }
    int *help_buff = new int[257];
    red->count = n;
    green->count = n;
    blue->count = n;
    red->Sort(blue->index, help_buff, 257);
    green->Sort(red->index, help_buff, 257);
    blue->Sort(green->index, help_buff, 257);
    int index;
    DWORD tmp, mx;
    RgbIndex (*_rgb)[3];
    for (i = 1; i < N; ++i)
    {
        _rgb = &(rgb[i]);
        if (red->range > green->range && red->range > blue->range)
        {

            index = red->count / 2;
            if (index)
            {
                (*_rgb)[1].InitRgbIndex(green, red, index, red->count - index, help_buff, 257);
                (*_rgb)[2].InitRgbIndex(blue, red, index, red->count - index, help_buff, 257);
            } else
            {
                (*_rgb)[1].InitRgbIndex(green, red, 0, 0, help_buff, 257);
                (*_rgb)[2].InitRgbIndex(blue, red, 0, 0, help_buff, 257);
            }
            green->InitRgbIndex(red, 0, index, help_buff, 257);
            blue->InitRgbIndex(red, 0, index, help_buff, 257);
            if (index)
                (*_rgb)[0].Init(red, index, red->count - index);
            else
                (*_rgb)[0].Init(red, 0, 0);

            red->count = index;
            red->SetRange();
        } else if (green->range > red->range && green->range > blue->range)
        {
            index = green->count / 2;
            if (index > 0)
            {
                (*_rgb)[0].InitRgbIndex(red, green, index, green->count - index, help_buff, 257);
                (*_rgb)[2].InitRgbIndex(blue, green, index, green->count - index, help_buff, 257);
            } else
            {
                (*_rgb)[0].InitRgbIndex(red, green, 0, 0, help_buff, 257);
                (*_rgb)[2].InitRgbIndex(blue, green, 0, 0, help_buff, 257);
            }
            red->InitRgbIndex(green, 0, index, help_buff, 257);
            blue->InitRgbIndex(green, 0, index, help_buff, 257);
            if (index > 0)
                (*_rgb)[1].Init(green, index, green->count - index);
            else
                (*_rgb)[1].Init(green, 0, 0);
            green->count = index;
            green->SetRange();
        } else
        {
            index = blue->count / 2;
            if (index)
            {
                (*_rgb)[0].InitRgbIndex(red, blue, index, blue->count - index, help_buff, 257);
                (*_rgb)[1].InitRgbIndex(green, blue, index, blue->count - index, help_buff, 257);
            } else
            {
                (*_rgb)[0].InitRgbIndex(red, blue, 0, 0, help_buff, 257);
                (*_rgb)[1].InitRgbIndex(green, blue, 0, 0, help_buff, 257);
            }
            red->InitRgbIndex(blue, 0, index, help_buff, 257);
            green->InitRgbIndex(blue, 0, index, help_buff, 257);
            if (index > 0)
                (*_rgb)[2].Init(blue, index, blue->count - index);
            else
                (*_rgb)[2].Init(blue, 0, 0);

            blue->count = index;
            blue->SetRange();
        }
        mx = 0;
        index = 0;
        for (j = 0; j <= i; ++j)
        {
            _rgb = &(rgb[j]);
            tmp = max(max((*_rgb)[0].range, (*_rgb)[1].range), (*_rgb)[2].range);
            if (tmp > mx)
            {
                index = j;
                mx = tmp;
            }
        }
        red = &(rgb[index][0]);
        green = &(rgb[index][1]);
        blue = &(rgb[index][2]);
    }
    delete[] help_buff;
}
//------------------------------------------
//
//------------------------------------------
EXPORTNIX void
CreateRGBPalette(BYTE *ptr_image, int width, int height, int size_pal, MArray<COLORREF> &palette)
{
    RgbIndex r((BYTE *) ptr_image, height * width, 2);
    RgbIndex g((BYTE *) ptr_image, height * width, 1);
    RgbIndex b((BYTE *) ptr_image, height * width, 0);

    RgbIndex (*rgb)[3] = new RgbIndex[size_pal][3];
    rgb[0][0].Init(&r);
    rgb[0][1].Init(&g);
    rgb[0][2].Init(&b);

    CreateRGBPalette(rgb, width, height, size_pal);

    palette.SetLen(size_pal);
    COLORREF *pal = palette.GetPtr();
    for (int i = 0; i < size_pal; ++i)
    {
        pal[i] = RGB(rgb[i][0].GetColor(),
                     rgb[i][1].GetColor(),
                     rgb[i][2].GetColor());
    }
    qsort(pal, size_pal, sizeof(DWORD), CompRgb2);

    delete[] rgb;
}
