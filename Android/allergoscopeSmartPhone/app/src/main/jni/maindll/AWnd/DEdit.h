/*
 * DEdit.h
 *
 *  Created on: Sep 26, 2014
 *      Author: alex
 */

#ifndef DEDIT_H_
#define DEDIT_H_
#include "Dipatcher.h"
#pragma pack(push)
#pragma pack(8)


EXPORTNIX class EXPMDC  AEdit : public ADispatcher {
	public:
    AEdit() {Name = "edit"; mLimit = (UINT)-1; fReadOnly = fModify = false;}
	virtual ~AEdit() {}
    void LimitText(int nChars) {mLimit = nChars; }
	DWORD GetEditSel () { return MAKELONG(mSelect.x, mSelect.y); }

    int GetLimitText() { return mLimit; }
    void Clear() { mText.Clear(); }
    void operator = (LPCSTR str) { mText = str; }
    void operator = (int i) { mText.Format("%d", i); }
    operator int ()  { return mText.ToInt(); }
    operator LPCSTR ()  { return mText.GetPtr(); }
	void ReplaceSel(TString lpszNewText, BOOL)
	{ if (mSelect.x != mSelect.y) { mText.Remove(mSelect.x, mSelect.y - mSelect.x); } mSelect.x = mSelect.y; mText.Insert(mSelect.x, lpszNewText); SetModify(1); }
	void GetSel(int &nStartChar, int &nEndChar) { nStartChar = mSelect.x; nEndChar = mSelect.y; }
	void SetSel(int nStartChar, int nEndChar, int f = 0) { mSelect.x = nStartChar; mSelect.y = nEndChar; }
    BOOL GetModify() { return fModify; }
    void SetModify(BOOL f) { fModify = f; }
    void SetReadOnly(BOOL f) { fReadOnly = f; }
    void SetTextColor(COLORREF clr) { mColor = clr; }
    void GetRect(IRect *rc) { *rc = mRect; }
	virtual void SetWindowText(LPCSTR _p) { mText = _p; }
	virtual void GetWindowText(TString &s) { s = mText; }

protected:
	UINT mLimit;
	IPoint mSelect;
	
	BOOL fModify;
	BOOL fReadOnly;
	ColorRef mColor;
	IRect mRect;
};
#pragma pack(pop)

#endif /* DEDIT_H_ */
