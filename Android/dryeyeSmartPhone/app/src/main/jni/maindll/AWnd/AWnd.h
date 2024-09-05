
#ifndef __AWND__
#define __AWND__
#ifndef WIN32
#include "android/bitmap.h"
#endif
#include "Dipatcher.h"
#include "MemImage.h"
#include "JBitmap.h"

#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC tagGHWND : public tagMBITMAP  {
public:
	tagGHWND(tagGDC *);
	tagGHWND(tagGDC *, int x, int y, int defaultTypeSrface);
	virtual ~tagGHWND() ;
	tagGHWND &Copy(const tagGHWND &b);
	virtual DWORD *SetAccess();
	virtual BOOL ReleaseAccess();
#ifdef ANDROID_NDK
	bool InitSurface(jobject _bmp);
	bool AttachSurface(jobject _bmp, int type_mem);
    bool AttachSurface(JBITMAP _bmp);

#endif
	virtual BOOL SetSize(int cx, int cy, int defaultTypeSrface);

    JBITMAP m_hBmp;

};

class ADC;
EXPORTNIX class EXPMDC AWnd : public ADispatcher, public tagGDC {
public:
	AWnd ();
	AWnd (UINT _ID, ADispatcher *_parent);

	virtual ~AWnd ();
	virtual ADC *GetDC();
	virtual void ReleaseDC(ADC *dc);
public:
	virtual BOOL SetAccess();
	virtual BOOL ReleaseAccess();
#ifdef ANDROID_NDK
    //type_mem == AObject::TJGLOBAL || AObject::TJLOCAL
	BOOL AttachSurface(jobject bmp, int type_mem);
    jobject DetachSurface();
#endif
    JBITMAP GetSurface();
    bool SetSurface(JBITMAP bmp);

public:
	static  AWnd *FromHandle(GHWND wnd) { return FromHandlePermanent(wnd); }
	static  AWnd *FromHandlePermanent(GHWND);
	virtual void InvalidateRect(CONST RECT *lpRect, BOOL bErase);
	virtual void Invalidate(BOOL bErase = true);
	BOOL SetSize(int cx, int cy);
	void ClearSize();
	void SetDefaultTypeSurface(int type) { defaultTypeSrface = type; }

	virtual void UpdateWindow() {ADispatcher::UpdateWindow(); Invalidate(); }
	MBITMAP &m_hWnd;
	int defaultTypeSrface;

};
#pragma pack(pop)
#ifndef WIN32
EXPORTNIX BOOL InvalidateRect(GHWND hWnd, CONST RECT *lpRect, BOOL bErase);
EXPORTNIX BOOL ReleaseDC( GHWND, GDC);
EXPORTNIX GDC GetDC(GHWND);
EXPORTNIX LRESULT SendMessage (GHWND wnd, UINT id, WPARAM a = 0, LPARAM b = 0);
EXPORTNIX LRESULT PostMessage (GHWND wnd, UINT id, WPARAM a = 0, LPARAM b = 0);
#endif
#include "DButton.h"
#include "DEdit.h"
#include "DList.h"
#include "DStatic.h"
#include "DSliderCtrl.h"

#endif
