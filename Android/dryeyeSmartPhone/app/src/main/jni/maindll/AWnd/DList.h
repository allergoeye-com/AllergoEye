/*
 * DList.h
 *
 *  Created on: Sep 26, 2014
 *      Author: alex
 */

#ifndef DLIST_H_
#define DLIST_H_
#ifndef CB_ERR
#define CB_ERR -1
#endif

#include "DEdit.h"

#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC  AList : public ADispatcher {
public:
    struct ListEntry {
        void *data;
        TString text;

        ListEntry()
        { data = 0; }

        ~ListEntry()
        {}

        void Set(const TString &t, void *p = 0)
        {
            text = t;
            data = p;
        }
    };

public:
    AList()
    {
        Name = "list";
        iSel = -1;
    }

    virtual ~AList()
    {}

    int GetCount()
    { return (int) mList.GetLen(); }

    int SetSel(int n)
    {
        iSel = n;
        if (n >= 0 && n < mList.GetLen())
            edit.SetWindowText(mList[n].text);
        else
            edit.SetWindowText(0);
        return iSel;
    }

    int SetCurSel(int n)
    { return iSel = n; }

    void SetStr(const TString &s, int n)
    { if (n >= 0 && n < (int) mList.GetLen()) mList[n].text = s; }

    int GetSel()
    { return iSel; }

    int GetCurSel()
    { return iSel; }

    void GetText(int n, TString &s)
    { if (n >= 0 && n < (int) mList.GetLen()) s = mList[n].text; }

    void GetText(int n, LPSTR str)
    { if (n >= 0 && n < (int) mList.GetLen()) strcpy(str, mList[n].text.GetPtr()); }

    int GetTextLen(int n)
    { return (n >= 0 && n < (int) mList.GetLen()) ? mList[n].text.StrLen() : 0; }

    TString GetStr(int n)
    { return (n >= 0 && n < (int) mList.GetLen()) ? mList[n].text : ""; }

    TString GetStrSel()
    { return (iSel > -1 && iSel < (int) mList.GetLen()) ? mList[iSel].text : ""; }

    void Remove(int n)
    { mList.Remove(n, 1); }

    int DeleteString(const TString &s)
    {
        int i, l;
        for (i = 0, l = (int) mList.GetLen(); i < l; ++i)
            if (mList[i].text == s)
            {
                mList.Remove(i, 1);
                break;
            }
        return i;
    }

    int DeleteString(int i)
    {
        mList.Remove(i, 1);
        return i;
    }

    int Find(const TString &s)
    {
        for (int i = 0, l = (int) mList.GetLen(); i < l; ++i)
            if (mList[i].text == s)
            { return i; }
        return -1;
    }

    int FindString(const TString &s)
    {
        for (int i = 0, l = (int) mList.GetLen(); i < l; ++i)
            if (mList[i].text == s)
            { return i; }
        return -1;
    }

    int FindStringNoCase(const TString &s)
    {
        for (int i = 0, l = (int) mList.GetLen(); i < l; ++i)
            if (!mList[i].text.CompareNoCase(s))
            { return i; }
        return -1;
    }

    void SetRedraw(BOOL b)
    { Invalidate(b); }

    int FindString(int iAfter, const TString &s)
    {
        for (int i = iAfter + 1, l = (int) mList.GetLen(); i < l; ++i)
            if (mList[i].text == s)
            { return i; }
        return -1;
    }

    void Reset()
    {
        iSel = -1;
        mList.Clear();
    }

    void ResetContent()
    { Reset(); }

    void SetStrSel(const TString &s)
    {
        for (int i = 0, l = (int) mList.GetLen(); i < l; ++i)
            if (mList[i].text == s)
            {
                iSel = i;
                break;
            }
    }

    void Remove(const TString &s)
    { DeleteString(s); }

    void Add(const TString &s)
    { mList.Add().Set(s); }

    int AddString(const TString &s)
    {
        mList.Add().Set(s);
        return (int) mList.GetLen() - 1;
    }

    int Insert(const TString &s, int n)
    {
        if (n >= 0 && n < (int) mList.GetLen())
        {
            ListEntry l;
            l.Set(s);
            mList.Insert(n, l);
        }
        else mList.Add().Set(s);
        return n;
    }

    int Insert(int n, const TString &s)
    {
        if (n >= 0 && n < (int) mList.GetLen())
        {
            ListEntry l;
            l.Set(s);
            mList.Insert(n, l);
        }
        else mList.Add().Set(s);
        return n;
    }

    int InsertString(const TString &s, int n)
    {
        if (n >= 0 && n < (int) mList.GetLen())
        {
            ListEntry l;
            l.Set(s);
            mList.Insert(n, l);
        }
        else mList.Add().Set(s);
        return n;
    }

    int InsertString(int n, const TString &s)
    {
        if (n >= 0 && n < (int) mList.GetLen())
        {
            ListEntry l;
            l.Set(s);
            mList.Insert(n, l);
        }
        else mList.Add().Set(s);
        return n;
    }

    void SetItemCount(int nItems)
    { mList.SetLen(nItems); }

    BOOL SetItemData(int n, DWORD_PTR dwData)
    {
        if (n >= 0 && n < (int) mList.GetLen())
        {
            mList[n].data = (void *) dwData;
            return true;
        }
        return false;
    }

    DWORD_PTR GetItemData(int n) const
    { return n >= 0 && n < (int) mList.GetLen() ? (DWORD_PTR) mList[n].data : 0; }

    virtual void SetWindowText(const TString &s)
    {
        iSel = -1;
        for (int i = 0, l = (int) mList.GetLen(); i < l; ++i)
            if (mList[i].text == s)
            {
                iSel = i;
                break;
            }
    }

    int SelectString(int n, LPCSTR s)
    {
        iSel = -1;
        if (n + 1 < (int) mList.GetLen())
            for (int i = n + 1, l = (int) mList.GetLen(); i < l; ++i)
                if (mList[i].text == s)
                {
                    iSel = i;
                    break;
                }
        return SetSel(iSel);
    }

    void GetLBText(int i, TString &str)
    { GetText(i, str); }

    BOOL SelectItem(TString &str)
    {
        SetWindowText(str);
        return iSel != -1;
    }

    BOOL SelectItemNoCase(TString &str)
    {
        iSel = FindStringNoCase(str);
        return iSel != -1;
    }

    TString GetText()
    {
        TString str;
        GetText(iSel, str);
        return str;
    }

    void SelectItem(int i)
    { SetSel(i); }

    void RemoveAllItems()
    {
        iSel = -1;
        mList.Clear();
    }

    void AddItem(const TString &s)
    { AddString(s); }

    int FindStringExact(int nIndexStart, LPCTSTR lpszFind)
    {
        if (nIndexStart == -1)
            nIndexStart = 0;

        int index = nIndexStart;
        CString str = lpszFind;

        for (int i = 0, l = mList.GetLen(); i < l; i++)
        {
            if (str.Compare(mList[i].text.GetPtr()) == 0)
                return i;
        }
        return -1; //CB_ERR;
    }

    DWORD GetEditSel()
    { return edit.GetEditSel(); }

    void SetEditSel(int a, int b)
    { edit.SetSel(a, b); }


public:
    MVector<ListEntry> mList;
    int iSel;
    AEdit edit;
};
#pragma pack(pop)


#endif /* DLIST_H_ */
