Window "IMAGE_DLG"
{
	left = 35; top = 36; right = 931; bottom = 350; IsVisible = false;
	IsDisabled = false; Type = "popup"; Caption = "Image View";
	HasBorder = true; IsResizable = true; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; HasOwnDC = false;
	Font =
	<<
		"System", 10, 0
	>>;
}

Window "DLG_MAIN"
{
	left = 0; top = 0; right = 249; bottom = 249; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "popup"; Caption = "";
	HasBorder = true; IsResizable = false; HasSysMenu = true;
	HasMinButton = true; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = false;
}

Menu "MENU_POPUP"
{
	IsWString = false;
}

File "TRY_CON"
{
	Type = "ico"; IsRef = true; Data = "../allergoeye/res/check.ico";
}
