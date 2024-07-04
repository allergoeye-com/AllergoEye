/*
 * DSliderCtrl.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: alex
 */
#include "stdafx.h"
#include "AWnd.h"
#include "DSliderCtrl.h"

ASliderCtrl::ASliderCtrl()
{
	Name = "slider";
	funcSetPos = 0;
	funcGetPos = 0;
	funcSetRange = 0;

	iMin = 0;
	iMax = 100;
	iPos = 0;

}
FloatSliderCtrl::FloatSliderCtrl()
{
	Name = "float_slider";
	funcSetPos = 0;
	funcGetPos = 0;
	funcSetRange = 0;
	step = 1.0f;
	iMin = 0.0f;
	iMax = 100.0f;
	iPos = 0.0f;
	fIsLn = FALSE;
}

ASliderCtrl::~ASliderCtrl()
{
}
ACastomSliderCtrl::ACastomSliderCtrl()
{
	Name = "castom_slider";
	iMin = 0;
	iMax = 100;
	iPos = 0;
	call = 0;

}

ACastomSliderCtrl::~ACastomSliderCtrl()
{
}

