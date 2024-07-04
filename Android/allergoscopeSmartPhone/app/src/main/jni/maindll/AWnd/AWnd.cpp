
#include "stdafx.h"
#include "AWnd.h"
#include "ADC.h"

//-----------------------------------------------------
//
//-----------------------------------------------------
tagGHWND::tagGHWND(tagGDC *s) : tagMBITMAP(s, HWIN)
{
	m_hBmp = 0;
	m_data = 0;
    count = 0;
	memset(&info, 0, sizeof (ImageInfo));

}
//-----------------------------------------------------
//
//-----------------------------------------------------
tagGHWND::tagGHWND(tagGDC *s, int x, int y, int defaultTypeSrface) : tagMBITMAP(s, HWIN)
{
	m_hBmp = 0;
	m_data = 0;
    count = 0;
	SetSize(x, y, defaultTypeSrface);
}
#ifdef ANDROID_NDK

//-----------------------------------------------------
//
//-----------------------------------------------------
bool tagGHWND::InitSurface(jobject _bmp)
{
    while (ReleaseAccess()) ;
    if (!m_hBmp)
        m_hBmp = new JBitmap();
    m_hBmp->Create(_bmp);
    m_hBmp->GetInfo(info);
    if (self) self->info = info;
    count = 0;

    return true;


}
//-----------------------------------------------------
//
//-----------------------------------------------------
bool tagGHWND::AttachSurface(jobject _bmp, int type_mem)
{
    while (ReleaseAccess()) ;
	if (!m_hBmp)
        m_hBmp = new JBitmap();
    m_hBmp->Attach(_bmp, type_mem);
    m_hBmp->GetInfo(info);
	if (self) self->info = info;
    count = 0;
	return true;


}

bool tagGHWND::AttachSurface(JBITMAP _bmp)
{
    while (ReleaseAccess()) ;
    if (m_hBmp)
        delete m_hBmp;
    m_hBmp = _bmp;
    m_hBmp->GetInfo(info);
    if (self) self->info = info;
    count = 0;
    return true;


}



#endif
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL tagGHWND::SetSize(int cx, int cy, int defaultTypeSrface)
{
    while (ReleaseAccess()) ;
    if (m_hBmp)
        m_hBmp->Resize(cx, cy);
    else
        m_hBmp = new JBitmap(cx, cy, defaultTypeSrface);
    m_hBmp->GetInfo(info);
    if (self) self->info = info;
    count = 0;
    return true;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
DWORD *tagGHWND::SetAccess()
{
	++count;
	if (m_data) return m_data;
	if (m_hBmp)
        m_data = m_hBmp->SetAccess();
	return m_data;

}
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL tagGHWND::ReleaseAccess()
{
	if (count)
		--count;
	if (!count)
        if (m_data)
        {
            if (m_hBmp)
                m_hBmp->ReleaseAccess();
            m_data = 0;

        }
	return m_data != 0;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
tagGHWND::~tagGHWND()
{
	if (m_hBmp)
        delete m_hBmp;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
tagGHWND &tagGHWND::Copy(const tagGHWND &b)
{
    while(count)
        ReleaseAccess();
	if (m_hBmp)
	{
        delete m_hBmp;
		m_hBmp = 0;
	}
	if (b.m_hBmp)
	{
		ImageInfo info1;
		m_hBmp = new JBitmap();
        m_hBmp->Create(*b.m_hBmp);
        m_hBmp->GetInfo(info);
	}
	if (!m_hBmp)
		memset(&info, 0, sizeof (AndroidBitmapInfo));
    if (self) self->info = info;
	m_data = 0;
	return *this;
}
//=========================================================
//
//=========================================================
AWnd::AWnd ():m_hWnd(surface)
{
	type = AWND;
	m_hWnd = 0;
	m_hView = 0;
	Name = "win";
    defaultTypeSrface = JBitmap::TJMEMORY;
}
//=========================================================
//
//=========================================================
AWnd::AWnd (UINT _ID, ADispatcher *_parent):m_hWnd(surface)
{
	type = AWND;
	m_hWnd = 0;
	m_hView = 0;
	Name = "win";
    defaultTypeSrface = JBitmap::TJMEMORY;
	ADispatcher::Create(ID, _parent);
}
//=========================================================
//
//=========================================================
AWnd::~AWnd ()
{
    ClearSize();

}
//=========================================================
//
//=========================================================
void AWnd::ClearSize()
{
	if (!surface) return;
	tagGHWND *bmp = dynamic_cast<tagGHWND *>(surface);
	if (bmp)
	{
		while(bmp->ReleaseAccess());
		delete bmp;
	}
	surface = 0;
	info.Set(0, 0, 0);

}

//=========================================================
//
//=========================================================
BOOL AWnd::SetSize(int cx, int cy)
{
	tagGHWND *bmp = 0;
	if (!surface)
	{

		bmp = new tagGHWND(this, cx, cy, defaultTypeSrface);
		surface = bmp;
	} else
		bmp = dynamic_cast<tagGHWND *>(surface);
	BOOL fRet = bmp->SetSize(cx, cy, defaultTypeSrface);
	OnSize(0, cx, cy);
	return fRet;

}
#ifdef ANDROID_NDK
//-----------------------------------------------------
//
//-----------------------------------------------------
JBITMAP AWnd::GetSurface()
{
	if (!surface) return 0;
	tagGHWND *bmp = dynamic_cast<tagGHWND *>(surface);

	return bmp ? bmp->m_hBmp : 0;


}
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL AWnd::AttachSurface(jobject _bmp, int type_mem)
{
	ClearSize();
	if (_bmp)
	{
		tagGHWND *bmp = new tagGHWND(this);
		surface = bmp;
		bmp->AttachSurface(_bmp, type_mem);
		OnSize(0, bmp->info.width, bmp->info.height);
	}
	return true;

}
//-----------------------------------------------------
//
//-----------------------------------------------------
jobject AWnd::DetachSurface()
{
	if (!surface) return 0;
	jobject ret = 0;

	tagGHWND *bmp = dynamic_cast<tagGHWND *>(surface);
	if (bmp)
	{
		while(bmp->count) bmp->ReleaseAccess();
		ret = bmp->m_hBmp->Detach();
		bmp->m_hBmp = 0;
		delete bmp;
	}
	surface = 0;
	return ret;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool AWnd::SetSurface(JBITMAP o)
{
    ClearSize();
    tagGHWND *bmp = 0;
    if (!surface)
    {
        bmp = new tagGHWND(this);
        surface = bmp;
    }
	else
		bmp = dynamic_cast<tagGHWND *>(surface);
    BOOL fRet = bmp->AttachSurface(o);
    OnSize(0, bmp->info.width, bmp->info.height);
    return fRet;
}

#endif
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL AWnd::SetAccess()
{
	if (!surface) return 0;
	tagGHWND *bmp = dynamic_cast<tagGHWND *>(surface);
	if (bmp)
	{
		DWORD *ptr = bmp->SetAccess();
		if (!ptr) return 0;
		MemImage::Init(ptr, bmp->info);
		return true;
	}
	return false;

}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void AWnd::Invalidate(BOOL bErase)
{
	if (m_hView)
	{
#if(0) //def ANDROID_NDK
		JNIEnv*env = MInstance::GetLocalJNI();
		if (env)
		{
			jclass cl = env->GetObjectClass(m_hView);
			jmethodID id =  MGetApp()->GetMethodID(env, cl, "invalidate", "()V");
			if (id)
				env->CallVoidMethod(m_hView, id); //, r.left, r.top, r.right, r.bottom);
		}
#else
		OnPaint();
	//	AObject *view = GetView();
	//	if (view)
	//		view->Run("Invalidate");
#endif
	}
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void AWnd::InvalidateRect(CONST RECT *rc, BOOL bErase)
{
	if (!rc)
		Invalidate();
	else
	if (m_hView)
	{
#if(0) //def ANDROID_NDK
		JNIEnv*env = MInstance::GetLocalJNI();
		if (env)
		{
			jclass cl = env->GetObjectClass(m_hView);
			jmethodID id =  MGetApp()->GetMethodID(env, cl, "invalidate", "(IIII)V");
			if (id)
				env->CallVoidMethod(m_hView, id, rc->left, rc->top, rc->right, rc->bottom);
		}
#else
		;
#endif
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL AWnd::ReleaseAccess()
{
	tagGHWND *bmp = dynamic_cast<tagGHWND *>(surface);
	if (!bmp) return 0;
	bmp->ReleaseAccess();
	if (!bmp->count) ptr = 0;

	return true;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
AWnd *AWnd::FromHandlePermanent(GHWND hwnd)
{
	return dynamic_cast<AWnd *>(hwnd);
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
ADC *AWnd::GetDC()
{
	ADC *dc = new ADC(dynamic_cast<tagGHWND *>(m_hWnd));
	dc->OpenDC();
	return dc;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void AWnd::ReleaseDC(ADC *dc)
{
	if (dc)
	{
		dc->CloseDC();
		delete dc;
	}
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
EXPORTNIX BOOL InvalidateRect(GHWND _hWnd, CONST RECT *lpRect, BOOL bErase)
{
	AWnd *hWnd = dynamic_cast<AWnd *>(_hWnd);
	if (hWnd && hWnd && hWnd->type == AHandle::AWND)
		hWnd->InvalidateRect(lpRect, bErase);
	return 1;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
EXPORTNIX BOOL ReleaseDC(GHWND _wnd, GDC dc)
{

	if (dc)
		dc->ReleaseAccess();
	return 1;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
EXPORTNIX GDC GetDC(GHWND _wnd)
{
	AWnd *wnd = dynamic_cast<AWnd *>(_wnd);

	if (wnd && wnd->type == AHandle::AWND  && wnd->SetAccess())
		return wnd;
#ifndef ANDROID_NDK
		HDC dc = ::GetDC((HWND)0);
		ABmp *res = new ABmp(GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES));
		::ReleaseDC((HWND)0, dc);
#else
		ABmp *res = new ABmp(MGetApp()->GetMetrics()->widthPixels, MGetApp()->GetMetrics()->heightPixels);
#endif
	res->fTemp = true;
	res->SetAccess();

	return res;
}
