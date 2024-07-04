#ifndef _ATAB_CTRL_
#define _ATAB_CTRL_

#include "DImageList.h"

#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC ATabCtrl : public ADispatcher {

public:
    ATabCtrl();

    virtual ~ATabCtrl();

    void SetCurSel(int i)
    { iSel = i; }

    int GetCurSel()
    { return iSel; }

    bool AddPage(ADispatcher *page, LPCSTR text = 0, bool select = false, int imageId = -1)
    {
        pages.Add(page);
        if (text)
            page->SetWindowText(text);
        if (select)
            iSel = pages.GetLen() - 1;
        return true;
    }

    bool ReplacePage(int index, ADispatcher *page, LPCSTR text = 0, bool select = false,
                     int imageId = -1)
    {
        if (index < pages.GetLen())
        {
            pages[index]->Destroy();
            delete pages[index];
            pages[index] = page;
            if (text)
                page->SetWindowText(text);
            if (select)
                iSel = index;
            return true;
        }
        return AddPage(page, text, select, imageId);

    }

    bool
    InsertPage(int index, ADispatcher *page, LPCSTR text = 0, bool select = false, int imageId = -1)
    {
        if (index < pages.GetLen())
        {
            pages.Insert(index, page);
            if (text)
                page->SetWindowText(text);
            if (select)
                iSel = index;
            return true;
        }
        return AddPage(page, text, select, imageId);
    }

    void SetImageList(AImageList *imageList)
    {}

    int GetPageCount()
    { return pages.GetLen(); }

    void RemovePage(int index, bool fDelete = true)
    {
        if (index < pages.GetLen())
        {
            if (fDelete) delete pages[index];
            pages.Remove(index, 1);
        }
    }

    ADispatcher *GetPage(int index)
    { return index < pages.GetLen() ? pages[index] : 0; }

    void DeleteAllPages()
    {
        for (int i = 0, l = pages.GetLen(); i < l; ++i) delete pages[i];
        pages.Clear();
    }

    int SetSel(int i)
    {
        iSel = i;
        return i;
    }

    int GetSel()
    { return iSel; }

    virtual void OnChangeTab(UINT nTabIndex);

    MArray<ADispatcher *> pages;
    int iSel;
};
#pragma pack(pop)

#endif
