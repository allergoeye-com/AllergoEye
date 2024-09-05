/*
 * Dipatcher.cpp
 *
 *  Created on: Sep 26, 2014
 *      Author: alex
 */
#include "stdafx.h"
#include "Dipatcher.h"
#include "AWnd.h"
#include "DTabCtrl.h"
#include "DListView.h"

#define WC_TREEVIEWA            "SysTreeView32"
#define WC_TABCONTROLA          "SysTabControl32"
#define WC_LISTVIEWA            "SysListView32"
#define WC_SLIDERA             "msctls_trackbar32"
#define WC_PROGRESSA             "msctls_progress32"

#define WC_BUTTONA              "Button"
#define WC_STATICA              "Static"
#define WC_EDITA                "Edit"
#define WC_LISTBOXA             "ListBox"
#define WC_COMBOBOXA            "ComboBox"
#define WC_SCROLLBARA            "ScrollBar"
#define WC_HEADERA              "SysHeader32"
#define WC_CUSTOMA              "CustomCtrl"

#define     WA_INACTIVE     0
#define     WA_ACTIVE       1
#define     WA_CLICKACTIVE  2

#define DS_ABSALIGN         0x01L
#define DS_SYSMODAL         0x02L
#define DS_LOCALEDIT        0x20L   /* Edit items get Local storage. */
#define DS_SETFONT          0x40L   /* User specified font for Dlg controls */
#define DS_MODALFRAME       0x80L   /* Can be combined with WS_CAPTION  */
#define DS_NOIDLEMSG        0x100L  /* WM_ENTERIDLE message will not be sent */
#define DS_SETFOREGROUND    0x200L  /* not in win3.1 */
/*
 * Class styles
 */
#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002
#define CS_DBLCLKS          0x0008
#define CS_OWNDC            0x0020
#define CS_CLASSDC          0x0040
#define CS_PARENTDC         0x0080
#define CS_NOCLOSE          0x0200
#define CS_SAVEBITS         0x0800
#define CS_BYTEALIGNCLIENT  0x1000
#define CS_BYTEALIGNWINDOW  0x2000
#define CS_GLOBALCLASS      0x4000
#define DLGWINDOWEXTRA 48
#define ERROR_CLASS_ALREADY_EXISTS       1410L

#define GCL_MENUNAME        (-8)
#define GCL_HBRBACKGROUND   (-10)
#define GCL_HCURSOR         (-12)
#define GCL_HICON           (-14)
#define GCL_HMODULE         (-16)
#define GCL_CBWNDEXTRA      (-18)
#define GCL_CBCLSEXTRA      (-20)
#define GCL_WNDPROC         (-24)
#define GCL_STYLE           (-26)
#pragma pack(push)
#pragma pack (2)
struct DLGTEMPLATE {
    DWORD style;
    DWORD dwExtendedStyle;
    WORD cdit;
    short x;
    short y;
    short cx;
    short cy;
};
typedef DLGTEMPLATE *LPDLGTEMPLATE;

struct DLGITEMTEMPLATE {
    DWORD style;
    DWORD dwExtendedStyle;
    short x;
    short y;
    short cx;
    short cy;
    WORD id;
};
typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATE;
#pragma pack(pop)


/*********************************
		typedef struct {
		  WORD      dlgVer;
		  WORD      signature;
		  DWORD     helpID;
		  DWORD     exStyle;
		  DWORD     style;
		  WORD      cDlgItems;
		  short     x;
		  short     y;
		  short     cx;
		  short     cy;
		  sz_Or_Ord menu;
		  sz_Or_Ord windowClass;
		  WCHAR     title[titleLen];
		  WORD      pointsize;
		  WORD      weight;
		  BYTE      italic;
		  BYTE      charset;
		  WCHAR     typeface[stringLen];
		} DLGTEMPLATEEX;
		typedef struct {
		  DWORD     helpID;
		  DWORD     exStyle;
		  DWORD     style;
		  short     x;
		  short     y;
		  short     cx;
		  short     cy;
		  DWORD     id;
		  sz_Or_Ord windowClass;
		  sz_Or_Ord title;
		  WORD      extraCount;
		} DLGITEMTEMPLATEEX;
***********************************/

struct DLGTEMPLATEEX : public DLGTEMPLATE {
    WORD *menu;
    WORD *windowClass;
    WORD *title;
    WORD fontsize;
    WORD *fontface;
    WORD *next;

    void Init(WORD *pw)
    {
        fontface = 0;
        BOOL fIsEx = ((WORD *) pw)[1] == 0xFFFF;
        if (fIsEx)
        {
            pw += 4;
            dwExtendedStyle = Mem2DWord((BYTE *) pw);
            pw += 2;
            style = Mem2DWord((BYTE *) pw);
            pw += 2;
        } else
        {
            style = Mem2DWord((BYTE *) pw);
            pw += 2;
            dwExtendedStyle = Mem2DWord((BYTE *) pw);
            pw += 2;

        }
        cdit = Mem2Short((BYTE *) pw);
        ++pw;
        x = Mem2Short((BYTE *) pw);
        ++pw;
        y = Mem2Short((BYTE *) pw);
        ++pw;
        cx = Mem2Short((BYTE *) pw);
        ++pw;
        cy = Mem2Short((BYTE *) pw);
        ++pw;
        menu = pw;

        if (*pw == (WORD) -1)        // Skip menu name ordinal or string
            pw += 2; // WORDs
        else
        {
            if (*pw == (WORD) 0)
                ++pw;
            else
            {

                while (*pw++);

            }
        }
        windowClass = pw;
        if (*pw == (WORD) -1)
            pw += 2;
        else
        {
            if (*pw == (WORD) 0)
                ++pw;
            else
            {
                while (*pw++);

            }
        }
        title = pw;
        if (*pw == (WORD) 0)
            ++pw;
        else
        {

            while (*pw++);

        }
        if (!fIsEx)
        {
            if (style & DS_SETFONT)
            {
                fontsize = *pw++;
                fontface = pw;
                if (*pw == (WORD) 0)
                    ++pw;
                else
                {

                    while (*pw++);

                }
            }
        } else
        {
            fontsize = *pw++;
            ++pw; //weight
            ++pw; //italic + charset
            fontface = pw;
            if (*pw == (WORD) 0)
                ++pw;
            else
            {

                while (*pw++);

            }
        }

        pw = (WORD *) (((DWORD_PTR) pw + 3) & ~DWORD_PTR(3));
        if (fIsEx)
            pw += 2;
        next = pw;

    }

};

struct DLGITEMTEMPLATEEX : public DLGITEMTEMPLATE {
    WORD *windowClass;
    WORD *title;
    WORD *next;

    void Init(WORD *pw, bool fIsEx)
    {
        if (fIsEx)
        {

            dwExtendedStyle = Mem2DWord((BYTE *) pw);
            pw += 2;
            style = Mem2DWord((BYTE *) pw);
            pw += 2;
        } else
        {
            style = Mem2DWord((BYTE *) pw);
            pw += 2;
            dwExtendedStyle = Mem2DWord((BYTE *) pw);
            pw += 2;
        }

        x = Mem2Short((BYTE *) pw);
        ++pw;
        y = Mem2Short((BYTE *) pw);
        ++pw;
        cx = Mem2Short((BYTE *) pw);
        ++pw;
        cy = Mem2Short((BYTE *) pw);
        ++pw;

        id = Mem2Short((BYTE *) pw);
        ++pw;
        pw += fIsEx;
        windowClass = pw;
        if (*pw == (WORD) -1)
            pw += 2;
        else if (*pw == (WORD) 0)
            ++pw;
        else
        {

            while (*pw++);

        }
        title = pw;
        if (*pw == (WORD) -1)
            pw += 2;
        else if (*pw == (WORD) 0)
            ++pw;
        else
        {

            while (*pw++);

        }
        WORD cbExtra = *pw++;

        if (cbExtra != 0)
            cbExtra -= 2;
        if (cbExtra)
            pw += cbExtra / 2;
        pw = (WORD *) (((DWORD_PTR) pw + 3) & ~DWORD_PTR(3));
        if (fIsEx)
            pw += 2;
        next = pw;
    }
};

struct DlgTempateIterator {

    BOOL fIsEx;
    DLGTEMPLATE *pTemplate;
    DLGITEMTEMPLATE *pItem;
    DLGITEMTEMPLATEEX Item;
    DLGTEMPLATEEX Template;

    DlgTempateIterator(DLGTEMPLATE *p)
    {
        pTemplate = p;
        pItem = 0;
        fIsEx = ((WORD *) p)[1] == 0xFFFF;
        Template.Init((WORD *) p);
        pTemplate = &Template;

    }

    ~DlgTempateIterator()
    {}


    WORD Count()
    {
        return pTemplate ? pTemplate->cdit : 0;
    }

    DLGITEMTEMPLATE *Begin()
    {

        Item.Init(Template.next, fIsEx);
        return pItem = &Item;

    }

    MString GetText()
    {
        WORD *pw;
        WString test;
        MString s;

        pw = pItem == 0 ? Template.title : Item.title;
        if (*pw && *pw != (WORD) -1)
        {
            while (*pw)
            {
                test.Add() = *pw;
                ++pw;
            }
            test.Add() = 0;
            s = test;
        }
        return s;
    }

    WString GetClassName()
    {
        WORD *pw;
        WString test;

        pw = pItem == 0 ? Template.windowClass : Item.windowClass;
        if (*pw && *pw != (WORD) -1)
        {
            while (*pw)
            {
                test.Add() = *pw;
                ++pw;
            }
            test.Add() = 0;
        }
        return test;
    }

    BOOL TypeIsNumber()
    {
        if (pItem)
        {
            WORD *pw;
            pw = Item.windowClass;
            return *pw == (WORD) 0xFFFF;
        }
        return 0;
    }

    BOOL GetTypeString(WString &type)
    {
        if (pItem)
        {
            WORD *pw;
            pw = Item.windowClass;

            if (*pw != (WORD) 0xFFFF)
            {
                while (*pw)
                    type.Add() = *pw++;
                type.Add() = 0;
                TString t;
                t = type;

                return TRUE;
            }
        }
        return FALSE;
    }

    WORD Type()
    {
        if (pItem)
        {
            WORD *pw;
            pw = Item.windowClass;

            if (*pw == (WORD) 0xFFFF)
            {
                ++pw;
                return *pw;
            }
        }

        return 0xFFFF;
    }

    DLGITEMTEMPLATE *Next()
    {
        if (!pItem) return Begin();
        WORD *pw = Item.next;
        Item.Init(pw, fIsEx);

        return pItem;
    }

    bool GetFont(WString &name, int &size)
    {
        DWORD dwStyle = pTemplate->style;
        if (dwStyle & DS_SETFONT)
        {
            size = Template.fontsize;
            WORD *pw = Template.fontface;
            if (*pw != (WORD) 0)
                while (*pw)
                    name.Add() = (wchar_t) *pw++;
            name.Add() = 0;
            TString t;
            t = name;

            return true;
        }
        return false;
    }
};

MHashSet<ADispatcher *> ADispatcher::g_wnd;

void ADispatcher::InitDlgFromTemplate()
{
    if (MGetApp()->dlgs.Lookup(ID))
    {
        LPDLGTEMPLATE pTemplate = (LPDLGTEMPLATE) MGetApp()->dlgs[ID].GetPtr();
        if (pTemplate)
        {
            DlgTempateIterator it(pTemplate);
            DLGITEMTEMPLATE *p;
            TString s; // = WC_CUSTOMA;

            Name = it.GetClassName();
            mText = it.GetText();
            for (int i = 0, l = it.Count(); i < l; ++i)
            {
                ADispatcher *obj = 0;
                p = it.Next();
                switch (it.Type())
                {
                    case 0x0080:
                    {
                        s = WC_BUTTONA;
                        obj = new AButton();


                        break;
                    }
                    case 0x0081:
                    {
                        s = WC_EDITA;
                        obj = new AEdit();
                        break;
                    }
                    case 0x0082:
                    {
                        s = WC_STATICA;

                        obj = new AStatic();

                        break;
                    }
                    case 0x0083:
                    {
                        s = WC_LISTBOXA;

                        obj = new AList();

                        break;
                    }
                    case 0x0084:
                    {
                        s = WC_SCROLLBARA;
                        obj = new ASliderCtrl();
                        break;
                    }
                    case 0x0085:
                    {
                        s = WC_COMBOBOXA;
                        obj = new AList();

                        break;
                    }
                    default:
                        if (!it.TypeIsNumber())
                        {
                            WString ws;
                            it.GetTypeString(ws);
                            s = ws;
                            if (s.CompareNoCase(WC_TABCONTROLA) == 0)
                                obj = new ATabCtrl();
                            else if (s.CompareNoCase(WC_LISTVIEWA) == 0)
                                obj = new AListView();
                            else if (s.CompareNoCase(WC_SLIDERA) == 0)
                                obj = new ASliderCtrl();
                            else if (s.CompareNoCase(WC_TREEVIEWA) == 0)
                                new ADispatcher(); //obj = new MTreeCtrlHandler();
                            else if (s.CompareNoCase(WC_PROGRESSA) == 0)
                                new ADispatcher(); //bj = new MProgressHandler();
                        }


                }
                if (!obj)
                {
                    if (s[0] == '{' && s.Right(1) == '}')
                    {
                        obj = new ADispatcher(); //ProxyWindowHandler();
                    }
                }
                if (obj)
                {
                    ADispatcher *disp = GetChild(p->id);
                    if (!disp)
                    {
                        obj->SetWindowText(it.GetText());
                        obj->ID = p->id;
                        obj->SetParent(this);
                    } else
                    {
                        disp->SetWindowText(it.GetText());
                        delete obj;
                    }
                }

            }

        }

    }
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
GMSG::GMSG(ADispatcher *wnd, UINT msg, WPARAM _wParam, WPARAM _lParam)
{
    Init(wnd, msg, _wParam, _lParam, arg);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void GMSG::Init(ADispatcher *wnd, UINT msg, WPARAM _wParam, WPARAM _lParam, void *_arg)
{
    hwnd = wnd;
    message = msg;
    wParam = _wParam;
    lParam = _lParam;
    arg = _arg;
    pt.x = pt.y = 0; //GetCursorPos(&pt);
}

AFont *ADispatcher::GetFont()
{
    return font;
}

AFont *ADispatcher::SetFont(AFont *f)
{
    return font = f;
}

void ADispatcher::Init()
{

    hThread = 0;
    callPaint = 0;
    callSetText = 0;
    callEnable = 0;
    g_wnd.Add(this);
    ID = (UINT_PTR) this; /*hThread = pthread_self();*/
    m_hView = 0;
    parent = 0;
    bShow = TRUE;
    fEnable = 1;
    font = dynamic_cast <AFont *>(AGetStockObject(SYSTEM_FONT));
}

AObject *ADispatcher::GetView()
{ return m_hView; }

AObject *ADispatcher::Detach()
{
    JOBJECT tmp = m_hView;
    m_hView = 0;
    return tmp;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
EXPORTNIX LRESULT SendMessage(GHWND wnd, UINT id, WPARAM a, LPARAM b)
{
    return ADispatcher::AMsgDispatch(wnd, id, a, b, true);
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
EXPORTNIX LRESULT PostMessage(GHWND wnd, UINT id, WPARAM a, LPARAM b)
{
    return ADispatcher::AMsgDispatch(wnd, id, a, b, false);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
LRESULT ADispatcher::SendMessage(UINT id, WPARAM a, LPARAM b)
{
    return AMsgDispatch(this, id, a, b, true);

}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
LRESULT ADispatcher::PostMessage(UINT id, WPARAM a, LPARAM b)
{
    return AMsgDispatch(this, id, a, b, false);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
LRESULT ADispatcher::AMsgDispatch(ADispatcher *wnd, UINT id, WPARAM a, LPARAM b, bool fSend)
{
    if (wnd && g_wnd.Find(wnd))
    {
#ifndef WIN32
        if (GetCurrentThread() != wnd->hThread)
        {
            MPThread *th = MPThread::FromHandle(wnd->hThread);
            if (th)
            {
                MSGThread *msg = MSGThread::FromHandle(th);
                if (msg)
                    return fSend ? msg->SendMessage(wnd, id, a, b) : msg->PostMessage(wnd, id, a,
                                                                                      b);

            } else
            {
                wnd->hThread = GetCurrentThread();
                return wnd->WindowProc(id, a, b);
            }


        } else
#endif
            return wnd->WindowProc(id, a, b);
    }
    return 0;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void ADispatcher::AddFromChild(ADispatcher *child)
{
    ADispatcher *child2 = 0;
    if (!childs.Lookup(child->ID, child2))
        childs[child->ID] = child;
    else if (childs[child->ID] != child)
    {
        ADispatcher *child1 = childs[child->ID];
        TString txt = child1->mText;
        delete child1;
        child->mText = txt;
        childs[child->ID] = child;
    }

}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void ADispatcher::RemoveFromChild(ADispatcher *child)
{
    if (childs.Lookup(child->ID))
        childs.Delete(child->ID);
}

//------------------------------------------------------
//
//------------------------------------------------------
AObject *ADispatcher::SetView(AObject *view)
{
#ifdef WIN32
    m_hView = view;
#else
    m_hView = view;
#endif
    return m_hView;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void ADispatcher::UpdateWindow()
{
    MHashMap<ADispatcher *, UINT> tmp(childs);
    MHashMap<ADispatcher *, UINT>::iterator it(tmp);
    for (RBData<ADispatcher *, UINT> *p = it.begin(); p; p = it.next())
        if (p->value)
            p->value->UpdateWindow();

}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
BOOL ADispatcher::Create(ADispatcher *_parent, UINT _ID)
{
//	if (g_wnd.Find(this)) return FALSE;
//	if (parent && !g_wnd.Find(parent)) return FALSE;

    g_wnd.Add(this);
    if (_ID)
        ID = _ID;
    if (_parent)
        parent = _parent;
    if (parent) parent->AddFromChild(this);
    hThread = pthread_self();
    //m_hView = 0;
    return OnInitDialog();
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
int ADispatcher::DoModal()
{
    if (g_wnd.Find(this)) return FALSE;
    if (parent && !g_wnd.Find(parent)) return FALSE;
    g_wnd.Add(this);
    if (parent) parent->AddFromChild(this);
    hThread = pthread_self();
    //m_hView = 0;
    return OnInitDialog();
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
ADispatcher::~ADispatcher()
{
    OnDestroy();
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void ADispatcher::OnDestroy()
{
    if (this == 0 || !g_wnd.Find(this))
    {
        return;
    }
    g_wnd.Delete(this);
    if (childs.m_iNum)
    {
        MHashMap<ADispatcher *, UINT> tmp(childs);
        MHashMap<ADispatcher *, UINT>::iterator it(tmp);
        for (RBData<ADispatcher *, UINT> *p = it.begin(); p; p = it.next())
            ::SendMessage(p->value, WM_DESTROY, 0, 0);
        tmp.Clear();
    }
    childs.Clear();
#ifdef ANDROID_NDK
    if (hThread)
    {
        ::SendMessage(hThread, WM_DESTROY, 0, 0);
        hThread = 0;
    }
    if (m_hView)
    {
        delete m_hView;
        m_hView = 0;
    }
#endif

    if (parent && g_wnd.Find(parent)) parent->RemoveFromChild(this);
    parent = 0;

}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
BOOL ADispatcher::SetParent(ADispatcher *p)
{
    if (parent)
        parent->RemoveFromChild(this);
    parent = p;
    if (parent && g_wnd.Find(parent))
    {
        parent->AddFromChild(this);
        return true;
    }
    parent = 0;
    return p == 0;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
LRESULT ADispatcher::WindowProc(UINT iCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    GMSG msg;
    msg.Init(this, iCmd, wParam, lParam);
    if (!(res = PreTranslateMessage(&msg)))
        switch (iCmd)
        {
            case WM_DESTROY:
                OnDestroy();
                break;
            case WM_LBUTTONDOWN:
            {
                CPoint pt(LOWORD(lParam), HIWORD(lParam));
                OnLButtonDown(wParam, pt);
                break;
            }
            case WM_LBUTTONUP:
            {
                CPoint pt(LOWORD(lParam), HIWORD(lParam));
                OnLButtonUp(wParam, pt);
                break;
            }
            case WM_MOUSEMOVE:
            {
                CPoint pt(LOWORD(lParam), HIWORD(lParam));
                OnMouseMove(wParam, pt);
                break;
            }
            case WM_KEYUP:
                OnKeyUp(wParam, LOWORD(lParam), HIWORD(lParam));
                break;
            case WM_KEYDOWN:
                OnKeyDown(wParam, LOWORD(lParam), HIWORD(lParam));
                break;

            case WM_COMMAND:
            {
#ifndef WIN32
                ADispatcher *disp = AfxGetMainWnd();
#else
                ADispatcher *disp = 0;
#endif
                if (disp && disp->OnCmdMsg(LOWORD(wParam), CN_COMMAND, 0))
                    break;
                if (!OnCmdMsg(LOWORD(wParam), CN_COMMAND, 0))
                    OnCommand(wParam, lParam);
                break;
            }
            case WM_SIZE:
            {
                OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
                break;
            }

        }
    return res;
}
