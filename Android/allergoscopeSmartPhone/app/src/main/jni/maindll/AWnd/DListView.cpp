/*
 * DListView.cpp
 *
 *  Created on: Mar 12, 2015
 *      Author: alex
 */

#include "stdafx.h"
#include "DListView.h"

AListView::AListView()
{
	list = 0;
	callUpdateData = 0;
	callInsertItem = 0;
	callDeleteItem = 0;
	callSetState = 0;

}
AListView::~AListView() {

}
int AListView::GetItemCount()
{
	return items.GetLen();
}
int AListView::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,  UINT nStateMask, int nImage, LPARAM lParam)
{
	wmLVITEM item;
	nItem = items.GetLen() >= nItem ? nItem : items.GetLen();
	item.Init(nMask, nItem, (LPSTR)lpszItem, nState,  nStateMask, nImage, lParam);
	items.Insert(nItem, item);
	int i = nItem;

	for (int l = items.GetLen(); i < l; ++i)
		++items[i].iItem;
	if (callInsertItem) { callInsertItem(this, nItem); }

	return nItem;

}
void AListView::SetItemState(int i, UINT state, UINT mask)
{
	if (i < 0 || items.GetLen() <= i) return;
	LVITEM &item = items[i];
	int ch = item.state & mask;
	if (ch)
		item.state &= ~ch;

	ch = state;
	item.state |= ch;
	if (callSetState) { callSetState(this, i); }

}
int AListView::GetNextItem(int i, int nFlags)
{
	++i;
	for (int l = items.GetLen(); i < l; ++i)
		if (items[i].state & nFlags)
			return i;
	return -1;
}
bool AListView::GetItem(LVITEM *n)
{
	if (!n || n->iItem < 0 || n->iItem >= items.GetLen()) return false;
	*n = items[n->iItem];
	return true;
}
void AListView::SetItemCount(int i)
{
	items.SetLen(i);
}
bool AListView::DeleteAllItems()
{
	items.Clear();

	return UpdateData();
}
bool AListView::DeleteItem(int i )
{
	if (i < 0 || items.GetLen() <= i) return false;
	int nItem = i;
	items.Remove(i, 1);
	for (int l = items.GetLen(); i < l; ++i)
		--items[i].iItem;
	if (callDeleteItem) callDeleteItem(this, nItem);

	return true;


}
