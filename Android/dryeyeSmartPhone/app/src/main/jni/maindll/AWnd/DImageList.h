/*
 * DImageList.h
 *
 *  Created on: Mar 12, 2015
 *      Author: alex
 */

#ifndef MAINDLL_MDC_DIMAGELIST_H_
#define MAINDLL_MDC_DIMAGELIST_H_

#include "ABmp.h"

#pragma pack(push)
#pragma pack(8)

class AImageList : public MList<ABmp> {
public:
    AImageList();

    virtual ~AImageList();

    bool Create(int width, int height)
    {
        DeleteImageList();
        m_w = width;
        m_h = height;
        return true;
    }

    void DeleteImageList()
    { MList<ABmp>::Clear(); }

    AImageList *GetSafeHandle()
    { return GetImageCount() ? this : 0; }

    int GetImageCount()
    { return MList<ABmp>::m_iNum; }

    int Add(ABmp *h, COLORREF cl);

    int Remove(int i)
    { return MList<ABmp>::Delete(i); }

    int m_w;
    int m_h;
};

#pragma pack(pop)

#endif /* MAINDLL_MDC_DIMAGELIST_H_ */
