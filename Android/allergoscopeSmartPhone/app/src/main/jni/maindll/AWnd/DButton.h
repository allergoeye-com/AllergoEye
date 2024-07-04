/*
 * DButton.h
 *
 *  Created on: Sep 26, 2014
 *      Author: alex
 */

#ifndef DBUTTON_H_
#define DBUTTON_H_
#include "Dipatcher.h"
#include "ABmp.h"
#pragma pack(push)
#pragma pack(8)
EXPORTNIX class EXPMDC  AButton : public ADispatcher {
public:
	AButton() { Name = "button"; mBmp = 0; fCheck = false; fEnable = true;  }
	~AButton() {  }
	int GetCheck() { return fCheck; }
    void SetCheck(int i) { fCheck = i; }
    ABmp *GetBitmap() { return mBmp; }
    void SetBitmap(ABmp *hBitmap) { mBmp = hBitmap; }
	virtual void SetWindowText(const TString &_p) { if (mText != _p) ADispatcher::SetWindowText(_p); }
	virtual void UpdateWindow() {ADispatcher::SetWindowText(mText); }

protected:

	BOOL fCheck;

	ABmp *mBmp;


};
#pragma pack(pop)

#endif /* DBUTTON_H_ */
