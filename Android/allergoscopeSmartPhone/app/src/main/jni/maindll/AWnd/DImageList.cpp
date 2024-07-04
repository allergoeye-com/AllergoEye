/*
 * DImageList.cpp
 *
 *  Created on: Mar 12, 2015
 *      Author: alex
 */

#include "stdafx.h"
#include "ABmp.h"

#include "DImageList.h"

AImageList::AImageList() {
	m_h = m_w = 0;

}

AImageList::~AImageList() {
	DeleteImageList();

}
int AImageList::Add(ABmp *h, COLORREF cl)
{
	ABmp *bmp = MList<ABmp>::Add();
	bmp->Create(m_w, m_h);
	bmp->SetAccess();
	if (h)
	{
		bmp->Show(*h, 0, 0);
		bmp->SetFGColor(RGB(255, 255, 255));
		bmp->Rect(0, 0, m_w - 1, m_h - 1);
//		bmp->FillSurfaceBorder(0, 0, cl);
	}
	return MList<ABmp>::m_iNum - 1;
}

