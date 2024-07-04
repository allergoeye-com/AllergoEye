/*
 * RgbIndex.h
 *
 *  Created on: Dec 3, 2015
 *      Author: alex
 */

#ifndef MAINDLL_MDC_RGBINDEX_H_
#define MAINDLL_MDC_RGBINDEX_H_

#include "AColor.h"


struct RgbIndex {
    int count;
    DWORD range;
    int shift;
    BYTE *ptr;
    BOOL KillArray;
    int *index;

    RgbIndex()
    {
        range = 0;
        count = 0;
        ptr = 0;
        index = 0;
        KillArray = FALSE;
    }

    RgbIndex(BYTE *p, int num, int sh);

    ~RgbIndex()
    { if (KillArray) free(index); }

    void Init(RgbIndex *src);

    void Init(RgbIndex *src, int from, int num);

    void InitRgbIndex(RgbIndex *src, RgbIndex *dst, int from, int num, int *help_buff,
                      int len_help_buff);

    void InitRgbIndex(RgbIndex *src, int from, int num, int *help_buff, int len_help_buff);

    void SetRange();

    BYTE GetColor();

    void Sort(int *dst, int *help_buff, int num);

    BYTE Low()
    { return *(ptr + *index + shift); }

    BYTE High()
    { return *(ptr + *(index + count - 1) + shift); }
};

inline RgbIndex::RgbIndex(BYTE *p, int num, int sh)
{
    KillArray = TRUE;
    range = 0;
    ptr = p;
    count = num;
    shift = sh;
    index = (int *) malloc(count * sizeof(int));
}

//------------------------------------------
//
//------------------------------------------
inline void RgbIndex::Init(RgbIndex *src)
{

    KillArray = FALSE;
    count = src->count;
    shift = src->shift;
    ptr = src->ptr;
    index = src->index;
    range = src->range;
}

//------------------------------------------
//
//------------------------------------------
inline void RgbIndex::Init(RgbIndex *src, int from, int num)
{
    count = num;
    shift = src->shift;
    ptr = src->ptr;
    index = src->index + from;
    KillArray = FALSE;
    SetRange();
}

//------------------------------------------
//
//------------------------------------------
inline void RgbIndex::SetRange()
{
    if (!count)
        range = 0;
    else
    {
        BYTE *_ptr = ptr + shift;
        DWORD diff = (DWORD) (*(_ptr + *(index + count - 1)) - *(_ptr + *index));
        range = diff + diff * (DWORD) (count >> 2);
    }
}


//------------------------------------------
//
//------------------------------------------
inline void RgbIndex::InitRgbIndex(RgbIndex *src, RgbIndex *dst, int from, int num, int *help_buff,
                                   int len_help_buff)
{
    count = num;
    shift = src->shift;
    ptr = src->ptr;
    KillArray = FALSE;
    index = src->index + from;
    if (count)
        Sort(dst->index + from, help_buff, len_help_buff);
    else
        range = 0;
}

//------------------------------------------
//
//------------------------------------------
inline void
RgbIndex::InitRgbIndex(RgbIndex *dst, int from, int num, int *help_buff, int len_help_buff)
{
    count = num;
    index += from;
    KillArray = FALSE;
    if (count)
        Sort(dst->index + from, help_buff, len_help_buff);
    else
        range = 0;
}

#endif /* MAINDLL_MDC_RGBINDEX_H_ */
