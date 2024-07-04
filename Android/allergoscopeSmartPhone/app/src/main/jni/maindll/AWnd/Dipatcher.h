/*
 * Dipatcher.h
 *
 *  Created on: Sep 26, 2014
 *      Author: alex
 */

#ifndef DIPATCHER_H_
#define DIPATCHER_H_

#include "AColor.h"
#include "mstring.h"
#include "hashmap.h"
#include "hashset.h"

#ifdef WIN32
#define THWND HANDLE
#endif

#pragma pack(push)
#pragma pack(8)

class AObject;

EXPORTNIX class EXPMDC ACmdUI        // simple helper class
{
public:
    typedef bool (*callEnableCommand)(UINT, BOOL);

    ACmdUI()
    {
        fCheck = 0;
        funcEnabled = 0;
        m_nID = (UINT) -1;
        m_bEnableChanged = true;
    }

    virtual ~ACmdUI()
    {}

    void Enable(BOOL bOn = 1)
    {
        m_bEnableChanged = bOn;
        if (funcEnabled) funcEnabled(m_nID, bOn);
    }

    void SetText(LPCSTR str)
    { text = str; }

    LPCSTR GetText()
    { return text.GetPtr(); }

    void SetCheck(int i = TRUE)
    { fCheck = i; }

    int GetCheck()
    { return fCheck; }

    BOOL m_bEnableChanged;
    UINT m_nID;
    TString text;
    int fCheck;
    callEnableCommand funcEnabled;

    virtual BOOL DoUpdate(BOOL bDisableIfNoHndler)
    { return true; }
};

class ADispatcher;

EXPORTNIX struct EXPMDC GMSG {
    ADispatcher *hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
    void *arg;

    GMSG()
    {
        time = 0;
        hwnd = 0;
        message = 0;
        wParam = lParam = 0;
        arg = 0;
    }

    GMSG(ADispatcher *wnd, UINT msg, WPARAM _wParam, WPARAM _lParam);

    void Init(ADispatcher *wnd, UINT msg, WPARAM _wParam, WPARAM _lParam, void *_arg = 0);
};

class AFont;

EXPORTNIX class EXPMDC ADispatcher {
protected:
    TString Name;
public:
    typedef bool (*callEnableWindow)(ADispatcher *, int);

    typedef void (*callSetWindowText)(ADispatcher *, LPCSTR);

    typedef void (*callPaintWindow)(ADispatcher *);

public:
    ADispatcher()
    {
        Name = "disp";
        Init();
    }

    ADispatcher(ADispatcher *_parent, UINT _ID)
    {
        Name = "disp";
        Init();
        if (_ID) ID = _ID;
        parent = _parent;
    }

    ADispatcher(UINT _ID, ADispatcher *_parent)
    {
        Name = "disp";
        Init();
        if (_ID) ID = _ID;
        parent = _parent;
    }

    virtual ~ADispatcher();

    BOOL Create(ADispatcher *_parent, UINT _ID);

    BOOL Create(UINT _ID, ADispatcher *_parent)
    { return Create(_parent, _ID); }

    virtual int DoModal();

    BOOL SetParent(ADispatcher *p);

    LRESULT SendMessage(UINT id, WPARAM a = 0, LPARAM b = 0);

    LRESULT PostMessage(UINT id, WPARAM a = 0, LPARAM b = 0);

    virtual BOOL PreTranslateMessage(GMSG *pMsg)
    { return parent && parent->PreTranslateMessage(pMsg); }

    virtual BOOL OnCommand(WPARAM, LPARAM)
    { return false; }

    virtual LRESULT WindowProc(UINT iCmd, WPARAM wParam, LPARAM lParam);

    virtual BOOL OnCmdMsg(UINT nID, int nCode, ACmdUI *pExtra)
    { return 0; }

    ADispatcher *GetChild(UINT id)
    {
        ADispatcher *ret = 0;
        return childs.Lookup(id, ret) ? ret : 0;
    }

    ADispatcher *GetDlgItem(UINT id)
    { return GetChild(id); }

    ADispatcher *GetParent()
    { return parent; }

    UINT GetID()
    { return ID; }

    UINT SetID(UINT _id)
    {
        UINT id = ID;
        ID = _id;
        return id;
    }

    virtual int EndDialog(int n)
    {
        Destroy();
        return n;
    }

    virtual ADispatcher *SetCapture()
    { return this; }

    virtual ADispatcher *ReleaseCapture()
    { return 0; }

    virtual void InvalidateRect(CONST RECT *lpRect, BOOL bErase = true)
    {}

    virtual void CenterWindow()
    {}

    BOOL Destroy()
    {
        OnDestroy();
        return true;
    }

    virtual BOOL OnInitDialog()
    {
        ShowWindow(SW_SHOW);
        return true;
    }

    virtual void OnShowWindow(BOOL _bShow, UINT nStatus)
    {}

    BOOL ShowWindow(BOOL _nCmdShow)
    {
        bShow = _nCmdShow;
        OnShowWindow(bShow == SW_SHOW, 0);
        return TRUE;
    }

    virtual void GetWindowText(TString &s)
    { s = mText; }

    virtual void SetWindowText(const TString &_p)
    {
        if (callSetText) callSetText(this, _p.GetPtr());
        mText = _p;
    }

    BOOL IsWindowVisible()
    { return bShow == SW_SHOW; }

    BOOL IsWindowEnabled()
    { return fEnable; }

    virtual ADispatcher *SetFocus()
    { return this; }

    virtual void OnPaint()
    { if (callPaint) callPaint(this); }

    virtual void Invalidate(BOOL bErase = true)
    {}

    bool Enable(int e = 1)
    {
        fEnable = e;
        return e != 0;
    }

    bool EnableWindow(int e = 1)
    { return callEnable && callEnable(this, e); }

    AObject *SetView(AObject *view);

    AObject *GetView();

    AObject *Detach();

    virtual void OnOK()
    {}

    virtual void OnCancel()
    {}

    virtual void UpdateWindow();

    void SetEnableCallback(callEnableWindow _callEnable)
    { callEnable = _callEnable; }

    void SetTextCallback(callSetWindowText _callSetText)
    { callSetText = _callSetText; }

    void SetTHWND(THWND w)
    { hThread = w; }

    THWND GetTHWND()
    { return hThread; }

    AFont *GetFont();

    AFont *SetFont(AFont *);

public:
    virtual void OnLButtonUp(UINT nFlags, CPoint point)
    {}

    virtual void OnLButtonDown(UINT nFlags, CPoint point)
    {}

    virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
    {}

    virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {}

    virtual void OnMouseMove(UINT nFlags, CPoint point)
    {}

public:
    void InitDlgFromTemplate();

protected :

    void AddFromChild(ADispatcher *);

    void RemoveFromChild(ADispatcher *);

    void Init();

protected:
    virtual void OnSize(UINT nType, int cx, int cy)
    {}

    virtual void OnDestroy();


    static LRESULT AMsgDispatch(ADispatcher *wnd, UINT id, WPARAM a, LPARAM b, bool fSend);

    callEnableWindow callEnable;
    callSetWindowText callSetText;
    callPaintWindow callPaint;
    TString mText;
    AObject *m_hView;
    THWND hThread;
    ADispatcher *parent;
    MHashMap<ADispatcher *, unsigned int> childs;
    DWORD_PTR ID;
    BOOL bShow;
    BOOL fEnable;
    AFont *font;


    static MHashSet<ADispatcher *> g_wnd;

    friend LRESULT SendMessage(ADispatcher *wnd, UINT id, WPARAM a, LPARAM b);

    friend LRESULT PostMessage(ADispatcher *wnd, UINT id, WPARAM a, LPARAM b);

};
#pragma pack(pop)

typedef ADispatcher *GHWND;

inline bool IsWindow(GHWND)
{ return true; }

#endif /* DIPATCHER_H_ */
