/*
 * DListView.h
 *
 *  Created on: Mar 12, 2015
 *      Author: alex
 */

#ifndef MAINDLL_MDC_DLISTVIEW_H_
#define MAINDLL_MDC_DLISTVIEW_H_

#include "DImageList.h"

#pragma pack(push)
#pragma pack(8)

#ifndef WIN32
#define LVIF_TEXT               0x0001
#define LVIF_IMAGE              0x0002
#define LVIF_PARAM              0x0004
#define LVIF_STATE              0x0008
#define LVIS_FOCUSED            0x0001
#define LVIS_SELECTED           0x0002
#define LVIS_CUT                0x0004
#define LVIS_DROPHILITED        0x0008
#define LVIS_GLOW               0x0010
#define LVIS_ACTIVATING         0x0020
struct NMHDR {
    GHWND hwndFrom;
    UINT_PTR idFrom;
    UINT code;         // NM_ code
};
typedef NMHDR *LPNMHDR;

struct LVITEM {
    LVITEM()
    {
        stateMask = state = mask = 0;
        cchTextMax = iItem = iSubItem = 0;
        iImage = -1;
        lParam = 0;

    }

    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    TString pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;

    void Init(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage,
              LPARAM _lParam)
    {
        mask = nMask;
        state = nState;
        pszText = lpszItem;
        stateMask = nStateMask;
        iItem = nItem;
        iImage = nImage;
        lParam = _lParam;
    }
};

#define LV_ITEM LVITEM
#define wmLVITEM LVITEM
struct NMLISTVIEW : public NMHDR {
    NMHDR hdr;
    int iItem;
    int iSubItem;
    UINT uNewState;
    UINT uOldState;
    UINT uChanged;
};
#define NM_LISTVIEW NMLISTVIEW
struct LVDISPINFO : public NMHDR {
    LVITEM item;
};
#define LV_DISPINFO LVDISPINFO

#define LVFI_PARAM              0x0001
#define LVFI_STRING             0x0002
#define LVFI_SUBSTRING          0x0004  // Same as LVFI_PARTIAL
#define LVFI_PARTIAL            0x0008
#define LVFI_WRAP               0x0020
#define LVFI_NEARESTXY          0x0040
struct LVFINDINFO {
    UINT flags;
    MString psz;
    LPARAM lParam;
    IPoint pt;
    UINT vkDirection;
};
#else
struct wmLVITEM : public LVITEM {
    wmLVITEM() {
        stateMask = state = mask = 0;
        cchTextMax = iItem = iSubItem = 0;
        iImage = -1;
        lParam = 0;

    }
    void Init(UINT nMask, int nItem, LPSTR lpszItem, UINT nState,  UINT nStateMask, int nImage, LPARAM _lParam)
    {
        mask = nMask;
        state = nState;
        pszText = lpszItem;
        stateMask = nStateMask;
        iItem = nItem;
        iImage = nImage;
         lParam =  _lParam;
    }
};

#include "Dipatcher.h"
#endif

class AListView : public ADispatcher {
public:
    typedef bool (*_callUpdateData)(ADispatcher *);

    typedef void (*_callInsertItem)(ADispatcher *, int);

    typedef void (*_callDeleteItem)(ADispatcher *, int);

    typedef void (*_callSetState)(ADispatcher *, int);

    AListView();

    virtual ~AListView();

    void SetImageList(AImageList *a, int)
    { list = a; }

    int
    InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage,
               LPARAM lParam);

    void SetItemState(int i, UINT state, UINT mask);

    int GetNextItem(int item, int nFlags);

    int GetItemCount();

    POSITION GetFirstSelectedItemPosition()
    { return (POSITION) ((INT_PTR) 1 + GetNextItem(-1, LVIS_SELECTED)); }

    POSITION GetNextSelectedItem(POSITION &pos)
    {
        POSITION nOldPos = pos - 1;
        pos = (POSITION) ((INT_PTR) 1 + GetNextItem((int) (INT_PTR) pos, LVIS_SELECTED));
        return nOldPos;
    }

    bool GetItem(LVITEM *);

    void SetItemCount(int);

    bool DeleteAllItems();

    bool DeleteItem(int nItem);

    bool UpdateData()
    { return callUpdateData && callUpdateData(this); }

    _callUpdateData callUpdateData;
    _callInsertItem callInsertItem;
    _callDeleteItem callDeleteItem;
    _callSetState callSetState;

    MVector<LVITEM> items;
    AImageList *list;
};

#pragma pack(pop)

#endif /* MAINDLL_MDC_DLISTVIEW_H_ */
