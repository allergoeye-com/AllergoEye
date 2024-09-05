Window "DLG_EDIT_CONTOURS_WND"
{
	left = 0; top = 0; right = 1240; bottom = 826; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "popup"; Caption = "Editor";
	HasBorder = true; IsResizable = true; HasSysMenu = true;
	HasMinButton = true; HasMaxButton = true; HasVScroll = false;
	HasHScroll = false; ToClipChildren = true; ToClipSiblings = true;
	HasOwnDC = true;
}

Window "DLG_EDIT_CONTOUR_MENU"
{
	left = 0; top = 0; right = 375; bottom = 34; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "child"; HasBorder = true;
	IsResizable = false; HasSysMenu = false; HasMinButton = false;
	HasMaxButton = false; HasVScroll = false; HasHScroll = false;
	ToClipChildren = false; ToClipSiblings = false; HasOwnDC = false;
	<<
		FixButton
		{
			left = 1; top = 0; right = 41; bottom = 28; IsVisible = true;
			IsDisabled = false; ID = "ID_NEW_ROI"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_NEW_ROI"; IsDefault = false;
			IsExNotify = false; ToStretch = true; State = false;
			IsRadio = false; IsAuto = true;
		},
		FixButton
		{
			left = 100; top = 1; right = 139; bottom = 29; IsVisible = true;
			IsDisabled = false; ID = "ID_EDIT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_EDIT"; IsDefault = false;
			IsExNotify = false; ToStretch = true; State = false;
			IsRadio = false; IsAuto = true;
		},
		Button
		{
			left = 231; top = 0; right = 269; bottom = 26; IsVisible = true;
			IsDisabled = false; ID = "ID_DELETE"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_DELETE"; IsDefault = false;
			IsExNotify = false; ToStretch = true;
		},
		Button
		{
			left = 141; top = 0; right = 182; bottom = 28; IsVisible = true;
			IsDisabled = false; ID = "ID_UNDO"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BM_UNDO"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 186; top = 0; right = 224; bottom = 28; IsVisible = true;
			IsDisabled = false; ID = "ID_REDO"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_REDO"; IsDefault = false;
			IsExNotify = false; ToStretch = true;
		},
		Button
		{
			left = 274; top = 0; right = 314; bottom = 28; IsVisible = true;
			IsDisabled = false; ID = "ID_SAVE"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_SAVE"; IsDefault = false;
			IsExNotify = false; ToStretch = true;
		},
		Button
		{
			left = 322; top = 0; right = 361; bottom = 27; IsVisible = true;
			IsDisabled = false; ID = "ID_OPTIONS"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_OPTIONS"; IsDefault = false;
			IsExNotify = false; ToStretch = true;
		},
		FixButton
		{
			left = 49; top = 0; right = 88; bottom = 28; IsVisible = true;
			IsDisabled = false; ID = "ID_NEW_RECT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_ROI_RC"; IsDefault = false;
			IsExNotify = false; ToStretch = true; State = false;
			IsRadio = false; IsAuto = true;
		}
	>>
}

File "BMP_EDIT"
{
	Type = "bmp"; IsRef = true; Data = "res/2edit.bmp";
}



File "EDIT"
{
	Type = "ico"; IsRef = true; Data = "res/edit.ico";
}

File "BMP_NEW_ROI"
{
	Type = "bmp"; IsRef = true; Data = "res/add.bmp";
}

File "BM_UNDO"
{
	Type = "bmp"; IsRef = true; Data = "res/undo.bmp";
}

File "BMP_REDO"
{
	Type = "bmp"; IsRef = true; Data = "res/redo.bmp";
}

File "BLANK"
{
	Type = "ico"; IsRef = true; Data = "res/blank.ico";
}
File "BMP_DELETE"
{
	Type = "bmp"; IsRef = true; Data = "res/del.bmp";
}
File "CHECK"
{
	Type = "ico"; IsRef = true; Data = "res/check.ico";
}

File "UNCHECK"
{
	Type = "ico"; IsRef = true; Data = "res/check_red.ico";
}

Window "ROI_TOOL"
{
	left = 0; top = 0; right = 329; bottom = 770; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "child"; HasBorder = true;
	IsResizable = false; HasSysMenu = false; HasMinButton = false;
	HasMaxButton = false; HasVScroll = false; HasHScroll = false;
	ToClipChildren = false; ToClipSiblings = false; HasOwnDC = false;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "ID_AUTO_SAVE"; ThatID = "ID_LIST_FILES";
			Relation = "GLUE_UPTODOWN"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_AUTO_SAVE"; ThatID = "ID_ROI_LIST";
			Relation = "GLUE_DOWNTOUP"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_LIST_FILES"; ThatID = -1;
			Relation = "GLUE_UPTOUP | GLUE_DOWNTOVCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_ROI_LIST"; ThatID = -1; Relation = "GLUE_DOWNTODOWN";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_ROI_LIST"; ThatID = "ID_LIST_FILES";
			Relation = "GLUE_UPTODOWN"; IsProportional = false;
		}
	>>;
	<<
		Table
		{
			left = 0; top = 26; right = 307; bottom = 461; IsVisible = true;
			IsDisabled = false; ID = "ID_LIST_FILES"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 3;
			HasBorder = true; HasHeader = true; IsColumnTrack = false;
			IsColumnRescale = false; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Props =
			<<
				<<
					"File", 186, "TC_CENTER | TC_READONLY"
				>>,
				<<
					"*", 38, "TC_CENTER | TC_READONLY"
				>>,
				<<
					"Chanels", 65, "TC_CENTER | TC_READONLY"
				>>
			>>;
			ImageList =
			<<
				"BLANK", "CHECK", "BLANK", "EDIT"
			>>;
		},
		Edit
		{
			left = 1; top = 0; right = 273; bottom = 18; IsVisible = true;
			IsDisabled = false; ID = "ID_PATH"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Button
		{
			left = 277; top = 1; right = 300; bottom = 19; IsVisible = true;
			IsDisabled = false; ID = "ID_ONPATH"; IsTabStop = true;
			IsGroupStart = false; Text = "..."; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Table
		{
			left = 3; top = 522; right = 309; bottom = 752; IsVisible = true;
			IsDisabled = false; ID = "ID_ROI_LIST"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = true;
			ToDitherSel = true;
			Props =
			<<
				<<
					W10_CONTOUR, 186, "TC_CENTER | TC_READONLY | TC_COMBOBOX"
				>>,
				<<
					"*", 43, "TC_CENTER | TC_READONLY"
				>>
			>>;
			ImageList =
			<<
				"EDIT", "CHECK"
			>>;
		},
		Check
		{
			left = 8; top = 483; right = 79; bottom = 498; IsVisible = true;
			IsDisabled = false; ID = "ID_AUTO_SAVE"; IsTabStop = true;
			IsGroupStart = false; Text = "AutoSave"; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = false; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		}
	>>
}

Window "IMAGE_WND"
{
	left = 0; top = 0; right = 221; bottom = 241; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "child"; HasBorder = false;
	IsResizable = false; HasSysMenu = false; HasMinButton = false;
	HasMaxButton = false; HasVScroll = false; HasHScroll = false;
	ToClipChildren = false; ToClipSiblings = false; HasOwnDC = true;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "ID_FRAME"; ThatID = -1;
			Relation = "GLUE_DOWNTODOWN | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		}
	>>;
	<<
		Frame
		{
			left = 3; top = 0; right = 215; bottom = 233; IsVisible = true;
			IsDisabled = false; ID = "ID_FRAME"; IsTabStop = false;
			IsGroupStart = false; Type = "dip"; IsRim = true; IsDrag = false;
		}
	>>
}

Menu "POPUP_MENU"
{
	IsWString = false;
}

File "BMP_SAVE"
{
	Type = "bmp"; IsRef = true; Data = "res/loaddata.bmp";
}

Window "DLG_SELECT_PLANE"
{
	left = 0; top = 0; right = 265; bottom = 228; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "popup";
	Caption = "Select plane"; HasBorder = true; IsResizable = false;
	HasSysMenu = true; HasMinButton = false; HasMaxButton = false;
	HasVScroll = false; HasHScroll = false; ToClipChildren = false;
	ToClipSiblings = false; HasOwnDC = false;
	<<
		Table
		{
			left = 37; top = 9; right = 236; bottom = 176; IsVisible = true;
			IsDisabled = false; ID = "ID_TABLE_PLANE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 1;
			HasBorder = true; HasHeader = false; IsColumnTrack = false;
			IsColumnRescale = false; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT"; HasVScroll = true;
			HasHScroll = false; IsReport = true; IsInputReturn = true;
			IsInputTab = false; ToStretchImage = false; ToDitherSel = false;
			Props =
			<<
				<<
					"", 182
				>>
			>>;
		},
		Button
		{
			left = 84; top = 190; right = 172; bottom = 212; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "OK"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		}
	>>
}

Window "DLG_COLOR_PLANE"
{
	left = 0; top = 0; right = 309; bottom = 252; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "popup"; Caption = "";
	HasBorder = true; IsResizable = false; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = false;
	<<
		Table
		{
			left = 37; top = 16; right = 274; bottom = 182; IsVisible = true;
			IsDisabled = false; ID = "ID_TABLE_PLANE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = true; IsColumnTrack = false;
			IsColumnRescale = false; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = true;
			Props =
			<<
				<<
					"Curve", 41, "TC_LEFT | TC_READONLY | TC_COMBOBOX"
				>>,
				<<
					"Plane", 175, "TC_LEFT | TC_READONLY"
				>>
			>>;
		},
		Button
		{
			left = 41; top = 202; right = 129; bottom = 223; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "OK"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 159; top = 201; right = 247; bottom = 223; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = "Cancel"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		}
	>>
}

File "BMP_OPTIONS"
{
	Type = "bmp"; IsRef = true; Data = "res/options.bmp";
}

File "BMP_ROI_RC"
{
	Type = "bmp"; IsRef = true; Data = "res/addrect.bmp";
}
