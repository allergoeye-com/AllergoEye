Window "HNO_SWITCH_WND"
{
	left = 0; top = 0; right = 210; bottom = 88; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "popup";
	Caption = "NHO-Presentation Switch"; HasBorder = true; IsResizable = false;
	HasSysMenu = true; HasMinButton = false; HasMaxButton = false;
	HasVScroll = false; HasHScroll = false; ToClipChildren = false;
	ToClipSiblings = false; HasOwnDC = false;
	<<
		Check
		{
			left = 40; top = 13; right = 159; bottom = 29; IsVisible = true;
			IsDisabled = false; ID = "ID_PRESENTATION"; IsTabStop = true;
			IsGroupStart = false; Text = "Presentation Mode"; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = false; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Button
		{
			left = 50; top = 54; right = 137; bottom = 75; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "OK"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		}
	>>
}
