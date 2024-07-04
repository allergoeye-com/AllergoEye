/*
 * DStatic.h
 *
 *  Created on: Sep 26, 2014
 *      Author: alex
 */

#ifndef DSTATIC_H_
#define DSTATIC_H_

#include "ABmp.h"

#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC  AStatic : public ADispatcher {
public:
    AStatic()
    {
        Name = "static";
        mBmp = 0;
    }

    virtual ~AStatic()
    {}

public:
    void SetBitmap(ABmp *hBitmap)
    { mBmp = hBitmap; }

    virtual void UpdateWindow()
    { ADispatcher::SetWindowText(mText); }

protected:
    ABmp *mBmp;
};
#pragma pack(pop)

#endif /* DSTATIC_H_ */
