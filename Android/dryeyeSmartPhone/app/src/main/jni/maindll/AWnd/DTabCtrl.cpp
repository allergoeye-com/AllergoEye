#include "stdafx.h"
#include "DTabCtrl.h"
ATabCtrl::ATabCtrl()
{
}
ATabCtrl::~ATabCtrl()
{
}
void ATabCtrl::OnChangeTab(UINT nTabIndex)
{
	iSel = nTabIndex;
}
