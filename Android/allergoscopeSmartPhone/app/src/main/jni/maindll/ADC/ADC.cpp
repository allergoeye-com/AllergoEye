#include "stdafx.h"
#include "ADC.h"
#ifndef WIN32
#include "AObject.h"
#endif
ADC::ADC(tagGHWND *wnd)
{
    stackDC.m_Nil = 0;
    hWnd = wnd;
    m_hDC = 0;
    bmpRop = SRCCOPY;
#ifndef WIN32
    metrics = 0;
#endif
}

ADC::~ADC()
{
    CloseDC();
#ifndef WIN32

    if (metrics)
        delete metrics;
#endif
}

bool ADC::Attach(GDC dc)
{
    if (m_hDC)
        stackDC.Push(m_hDC);
    m_hDC = dc;
    return true;
}

bool ADC::Detach()
{
    m_hDC = stackDC.Pop();
    return true;
}

bool ADC::OpenMBmp(ABmp &_bmp)
{
    SelectObject(&_bmp);

    return m_hDC == &_bmp;
}

GDC ADC::SelectObject(GDC bmp)
{
    GDC old = m_hDC;
    if (m_hDC == dynamic_cast<GDC>(bmp)) return 0;
    int i = stackDC.Find(bmp);
    if (i != -1)
        m_hDC = stackDC.Pop(i);
    else
    {
        m_hDC = bmp;
        stackDC.Push(old);
        bmp->SetAccess();
    }
    if (old) old->ReleaseAccess();
    return old;
}

bool ADC::CloseMBmp()
{
    if (m_hDC)
        m_hDC->ReleaseAccess();
    m_hDC = stackDC.Pop();
    if (m_hDC) m_hDC->SetAccess();
    return 0;
}

int ADC::GetDeviceCaps(DISP_MERICS t)
{
    int ret = 1;
#ifdef WIN32
    HDC h = ::GetDC((HWND)0);
        ret = ::GetDeviceCaps (h, (int)t);
        ::ReleaseDC((HWND)0, h);
#else
    MInstance::METRICS *m = metrics ? metrics : MGetApp()->GetMetrics();
    switch (t)
    {
        case HORZSIZE:
            ret = int((float) m->widthPixels / m->xdpi * 25.4f);
            break;
        case VERTSIZE:
            ret = int((float) m->heightPixels / m->ydpi * 25.4f);
            break;
        case HORZRES:
            ret = m->widthPixels;
            break;
        case VERTRES:
            ret = m->heightPixels;
            break;
        case LOGPIXELSX:
            ret = (int) m->xdpi;

            break;
        case LOGPIXELSY:
            ret = (int) m->ydpi;
            break;
        default :
            ret = 0;
    }

#endif
    return ret;
}

bool ADC::OpenDC()
{
    if (m_hDC)
        CloseDC();
    if (hWnd && hWnd->self)
    {
        hWnd->self->SetAccess();
        m_hDC = hWnd->self;
        return true;
    } else
    {
#ifdef WIN32
        HDC dc = ::GetDC((HWND)0);
        ABmp *res = new ABmp(::GetDeviceCaps(dc, HORZRES), ::GetDeviceCaps(dc, VERTRES));
        ::ReleaseDC((HWND)0, dc);
#else
        MInstance::METRICS *m = metrics ? metrics : MGetApp()->GetMetrics();
        ABmp *res = new ABmp(m->widthPixels, m->heightPixels);
#endif
        res->fTemp = true;
        res->SetAccess();
        m_hDC = res;
    }
    return 0;
}

bool ADC::CloseDC()
{
    if (m_hDC)
        m_hDC->ReleaseAccess();
    m_hDC = 0;
    return 0;
}


void ADC::Stretch(AObject *i, IRect &rc_dst, int, IRect *rc_src) {
    if(m_hDC)
            m_hDC->Stretch(i, *rc_src, rc_dst, bmpRop);
}
