Window "DLG_EDIT_CONTOURS_WND"
{
	left = 0; top = 0; right = 998; bottom = 690; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup"; Caption = "";
	HasBorder = true; IsResizable = true; HasSysMenu = true;
	HasMinButton = true; HasMaxButton = true; HasVScroll = false;
	HasHScroll = false; ToClipChildren = true; ToClipSiblings = true;
	HasOwnDC = true;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "ID_STATUS"; ThatID = -1;
			Relation = "GLUE_UPTODOWN | GLUE_DOWNTODOWN | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		}
	>>;
	<<
		Text
		{
			left = 13; top = 644; right = 885; bottom = 659; IsVisible = true;
			IsDisabled = false; ID = "ID_STATUS"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = "";
			TextAlign = "left";
		}
	>>
}

Window "IMAGE_WND"
{
	left = 0; top = 0; right = 236; bottom = 236; IsVisible = true;
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
			left = 7; top = 7; right = 234; bottom = 234; IsVisible = true;
			IsDisabled = false; ID = "ID_FRAME"; IsTabStop = false;
			IsGroupStart = false; Type = "dip"; IsRim = true; IsDrag = false;
		}
	>>
}


Window "DLG_EDIT_CONTOUR_MENU"
{
	left = 0; top = 0; right = 1021; bottom = 160; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "child"; HasBorder = false;
	IsResizable = false; HasSysMenu = false; HasMinButton = false;
	HasMaxButton = false; HasVScroll = false; HasHScroll = false;
	ToClipChildren = true; ToClipSiblings = true; HasOwnDC = true;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "ID_BUILD"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_DELETE"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_EDIT"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_EXIT"; ThatID = -1;
			Relation = "GLUE_LEFTTORIGHT | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_FRAME_TXT"; ThatID = -1;
			Relation = "GLUE_LEFTTOHCENTER | GLUE_RIGHTTOHCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_REDO"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_RESULT"; ThatID = -1; Relation = "GLUE_WHOLETOHCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TXT1"; ThatID = -1;
			Relation = "GLUE_LEFTTOHCENTER | GLUE_RIGHTTOHCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TXT2"; ThatID = -1;
			Relation = "GLUE_LEFTTOHCENTER | GLUE_RIGHTTOHCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TXT3"; ThatID = -1;
			Relation = "GLUE_LEFTTOHCENTER | GLUE_RIGHTTOHCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TXT4"; ThatID = -1;
			Relation = "GLUE_LEFTTOHCENTER | GLUE_RIGHTTOHCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TXT5"; ThatID = -1;
			Relation = "GLUE_LEFTTOHCENTER | GLUE_RIGHTTOHCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_UNDO"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "TXT_BUILD"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "TXT_DELETE"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "TXT_EDIT"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "TXT_EXIT"; ThatID = -1;
			Relation = "GLUE_LEFTTORIGHT | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "TXT_NEW"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "TXT_REDO"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "TXT_UNDO"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		}
	>>;
	<<
		FixButton
		{
			left = 7; top = 5; right = 67; bottom = 65; IsVisible = true;
			IsDisabled = false; ID = "ID_NEW_ROI"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_NEW_ROI"; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = false; IsAuto = true;
		},
		FixButton
		{
			left = 70; top = 5; right = 130; bottom = 65; IsVisible = true;
			IsDisabled = false; ID = "ID_EDIT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_EDIT"; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = false; IsAuto = true;
		},
		Button
		{
			left = 334; top = 4; right = 395; bottom = 65; IsVisible = true;
			IsDisabled = false; ID = "ID_DELETE"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_DELETE"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 133; top = 5; right = 193; bottom = 65; IsVisible = true;
			IsDisabled = false; ID = "ID_UNDO"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BM_UNDO"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 196; top = 5; right = 256; bottom = 65; IsVisible = true;
			IsDisabled = false; ID = "ID_REDO"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_REDO"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 259; top = 5; right = 319; bottom = 65; IsVisible = true;
			IsDisabled = false; ID = "ID_BUILD"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BM_BUILD"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Frame
		{
			left = 462; top = 1; right = 929; bottom = 89; IsVisible = true;
			IsDisabled = true; ID = "ID_FRAME_TXT"; IsTabStop = false;
			IsGroupStart = false; Type = "dip"; IsRim = true; IsDrag = false;
		},
		Text
		{
			left = 9; top = 79; right = 66; bottom = 111; IsVisible = true;
			IsDisabled = false; ID = "TXT_NEW"; IsWString = true;
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = W11_NEW_CONTOUR; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 73; top = 79; right = 130; bottom = 111; IsVisible = true;
			IsDisabled = false; ID = "TXT_EDIT"; IsWString = true;
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = W11_EDIT_CONTOUR; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 333; top = 79; right = 390; bottom = 111; IsVisible = true;
			IsDisabled = false; ID = "TXT_DELETE"; IsWString = true;
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = W11_DELETE_CONTOUR; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 258; top = 79; right = 322; bottom = 127; IsVisible = true;
			IsDisabled = false; ID = "TXT_BUILD"; IsWString = true;
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = W11_CALCULATE_AND_SAVE; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Button
		{
			left = 934; top = 10; right = 988; bottom = 64; IsVisible = true;
			IsDisabled = false; ID = "ID_EXIT"; IsTabStop = true;
			IsGroupStart = false; ToolTip = "Exit"; Bmp = "BMP_EXIT";
			IsDefault = false; IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 470; top = 4; right = 925; bottom = 25; IsVisible = true;
			IsDisabled = false; FgColor = color(0, 0, 255); ID = "ID_TXT1";
			IsWString = true; IsTabStop = false; IsGroupStart = false;
			HasBorder = false; Text = W11_HELP_LINE_1; TextAlign = "left";
			Font =
			<<
				"Times New Roman", 12, 1, 0
			>>;
		},
		Text
		{
			left = 470; top = 20; right = 925; bottom = 38; IsVisible = true;
			IsDisabled = false; FgColor = color(0, 0, 255); ID = "ID_TXT2";
			IsWString = true; IsTabStop = false; IsGroupStart = false;
			HasBorder = false; Text = W11_HELP_LINE_2; TextAlign = "left";
			Font =
			<<
				"Times New Roman", 12, 1, 0
			>>;
		},
		Text
		{
			left = 470; top = 50; right = 925; bottom = 70; IsVisible = true;
			IsDisabled = false; FgColor = color(0, 0, 255); ID = "ID_TXT3";
			IsWString = true; IsTabStop = false; IsGroupStart = false;
			HasBorder = false; Text = W11_HELP_LINE_4; TextAlign = "left";
			Font =
			<<
				"Times New Roman", 12, 1, 0
			>>;
		},
		Text
		{
			left = 470; top = 66; right = 925; bottom = 86; IsVisible = true;
			IsDisabled = false; FgColor = color(0, 0, 255); ID = "ID_TXT4";
			IsWString = true; IsTabStop = false; IsGroupStart = false;
			HasBorder = false; Text = W11_HELP_LINE_5; TextAlign = "left";
			Font =
			<<
				"Times New Roman", 12, 1, 0
			>>;
		},
		Text
		{
			left = 936; top = 71; right = 996; bottom = 89; IsVisible = true;
			IsDisabled = false; ID = "TXT_EXIT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W9_EXIT;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 470; top = 34; right = 925; bottom = 50; IsVisible = true;
			IsDisabled = false; FgColor = color(0, 0, 255); ID = "ID_TXT5";
			IsWString = true; IsTabStop = false; IsGroupStart = false;
			HasBorder = false; Text = W11_HELP_LINE_3; TextAlign = "left";
			Font =
			<<
				"Times New Roman", 12, 1, 0
			>>;
		},
		Text
		{
			left = 453; top = 94; right = 920; bottom = 119; IsVisible = true;
			IsDisabled = false; FgColor = color(255, 0, 0); ID = "ID_RESULT";
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = ""; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Text
		{
			left = 143; top = 79; right = 187; bottom = 114; IsVisible = true;
			IsDisabled = false; ID = "TXT_UNDO"; IsWString = true;
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = W11_UNDO; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 201; top = 79; right = 253; bottom = 112; IsVisible = true;
			IsDisabled = false; ID = "TXT_REDO"; IsWString = true;
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = W11_REDO; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Button
		{
			left = 398; top = 4; right = 459; bottom = 65; IsVisible = true;
			IsDisabled = false; ID = "ID_DELETE_IMAGE"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_DEL_IMAGE"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 402; top = 79; right = 452; bottom = 114; IsVisible = true;
			IsDisabled = false; ID = "TXT_DELETE_IMAGE"; IsWString = true;
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = W11_DELETE_IMAGE; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		}
	>>
}
File "BMP_SEARCH"
{
	Type = "bmp"; IsRef = true; Data = "res/select.bmp";
}

File "BMP_PLUS"
{
	Type = "bmp"; IsRef = true; Data = "res/plus.bmp";
}

File "BMP_MINUS"
{
	Type = "bmp"; IsRef = true; Data = "res/minus.bmp";
}

File "BMP_NEW_REC"
{
	Type = "bmp"; IsRef = true; Data = "res/qq2.bmp";
}

File "BMP_EDIT"
{
	Type = "bmp"; IsRef = true; Data = "res/edit.bmp";
}

File "BMP_SELECT"
{
	Type = "bmp"; IsRef = true; Data = "res/find.bmp";
}

File "BMP_SMART"
{
	Type = "bmp"; IsRef = true; Data = "res/smph.bmp";
}

File "BMP_EXIT"
{
	Type = "bmp"; IsRef = true; Data = "res/porta.bmp";
}

File "BMP_OPTIONS"
{
	Type = "bmp"; IsRef = true; Data = "res/options.bmp";
}

Window "DLG_NEW_CARD"
{
	left = 0; top = 0; right = 287; bottom = 144; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "popup";
	Caption = "Patient Info"; HasBorder = true; IsResizable = false;
	HasSysMenu = true; HasMinButton = false; HasMaxButton = false;
	HasVScroll = false; HasHScroll = false; ToClipChildren = false;
	ToClipSiblings = false; HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	<<
		Edit
		{
			left = 86; top = 18; right = 264; bottom = 38; IsVisible = true;
			IsDisabled = false; ID = "ID_ID"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Edit
		{
			left = 86; top = 38; right = 264; bottom = 57; IsVisible = true;
			IsDisabled = false; ID = "ID_SURNAME"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Edit
		{
			left = 86; top = 59; right = 264; bottom = 79; IsVisible = true;
			IsDisabled = false; ID = "ID_NAME"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Edit
		{
			left = 86; top = 80; right = 264; bottom = 100; IsVisible = true;
			IsDisabled = false; ID = "ID_DATE"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Button
		{
			left = 50; top = 109; right = 144; bottom = 132; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "Ok"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 155; top = 109; right = 249; bottom = 132; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = W2_CANCEL; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 18; top = 20; right = 79; bottom = 36; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = "ID";
			TextAlign = "right";
		},
		Text
		{
			left = 18; top = 40; right = 79; bottom = 56; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_LAST_NAME;
			TextAlign = "right";
		},
		Text
		{
			left = 18; top = 61; right = 79; bottom = 77; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_FIRST_NAME;
			TextAlign = "right";
		},
		Text
		{
			left = 9; top = 82; right = 79; bottom = 98; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_BIRTHDATE;
			TextAlign = "right";
		}
	>>
}



Window "DLG_SETTING"
{
	left = 0; top = 0; right = 607; bottom = 145; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup"; Caption = W5_SETTINGS;
	HasBorder = true; IsResizable = false; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	<<
		Text
		{
			left = 6; top = 16; right = 178; bottom = 35; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W6_DATA_DIRECTORY;
			TextAlign = "right";
		},
		Edit
		{
			left = 189; top = 13; right = 387; bottom = 35; IsVisible = true;
			IsDisabled = false; ID = "ID_TXT_DIR"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "C:\\Allergoscope";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Button
		{
			left = 391; top = 11; right = 482; bottom = 38; IsVisible = true;
			IsDisabled = false; ID = "ID_FILE_DLG"; IsTabStop = true;
			IsGroupStart = false; Text = WS_DATA_PATH; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 376; top = 109; right = 466; bottom = 134; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "Ok"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 505; top = 109; right = 598; bottom = 136; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = W2_CANCEL; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		ComboBox
		{
			left = 470; top = 75; right = 601; bottom = 144; IsVisible = true;
			IsDisabled = false; ID = "ID_LANGUAGE"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 50; top = 48; right = 178; bottom = 63; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = WS_GDT_PATH;
			TextAlign = "right";
		},
		Edit
		{
			left = 189; top = 45; right = 388; bottom = 65; IsVisible = true;
			IsDisabled = false; ID = "ID_GDT_DIR"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Button
		{
			left = 390; top = 41; right = 482; bottom = 69; IsVisible = true;
			IsDisabled = false; ID = "ID_FILE_DLG_1"; IsTabStop = true;
			IsGroupStart = false; Text = WS_GDT_BROWSE; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 13; top = 75; right = 130; bottom = 98; IsVisible = true;
			IsDisabled = false; ID = "ID_REBUILD_DATABASE"; IsTabStop = true;
			IsGroupStart = false; Text = W1_REBUILD_DB; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 13; top = 110; right = 129; bottom = 133; IsVisible = true;
			IsDisabled = false; ID = "ID_CLEAR_DATABASE"; IsTabStop = true;
			IsGroupStart = false; Text = W1_CLEAR_DATABASE; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 299; top = 80; right = 356; bottom = 93; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_PRAXIS_CITY;
			TextAlign = "right";
		},
		Edit
		{
			left = 365; top = 77; right = 445; bottom = 96; IsVisible = true;
			IsDisabled = false; ID = "ID_CITY_NAME"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "P6B0000000001####"W;
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Button
		{
			left = 139; top = 75; right = 287; bottom = 96; IsVisible = true;
			IsDisabled = false; ID = "ID_BACKUP"; IsTabStop = true;
			IsGroupStart = false; Text = W1_COPY_DB; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		}
	>>
}

Window "DLG_LIST_SMART"
{
	left = 0; top = 0; right = 425; bottom = 311; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup";
	Caption = "Smartphone auswaehlen"; HasBorder = true; IsResizable = false;
	HasSysMenu = true; HasMinButton = false; HasMaxButton = false;
	HasVScroll = false; HasHScroll = false; ToClipChildren = false;
	ToClipSiblings = false; HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	<<
		Table
		{
			left = 2; top = 8; right = 419; bottom = 250; IsVisible = true;
			IsDisabled = false; ID = "ID_LIST_PHONE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					W1_NAME, 206, "TC_LEFT | TC_READONLY"
				>>,
				<<
					W2_IPADDRESS, 216, "TC_LEFT | TC_READONLY"
				>>
			>>;
		},
		Button
		{
			left = 203; top = 262; right = 294; bottom = 288; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = W2_SELECT; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 306; top = 263; right = 398; bottom = 290; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = W2_CANCEL; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		}
	>>
}

Window "DLG_LIST_CARD"
{
	left = 0; top = 0; right = 696; bottom = 561; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup"; Caption = "";
	HasBorder = true; IsResizable = true; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = true; HasVScroll = false;
	HasHScroll = false; ToClipChildren = true; ToClipSiblings = true;
	HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "IDCANCEL"; ThatID = -1;
			Relation = "GLUE_UPTODOWN | GLUE_DOWNTODOWN | GLUE_LEFTTORIGHT | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "IDOK"; ThatID = -1;
			Relation = "GLUE_UPTODOWN | GLUE_DOWNTODOWN | GLUE_LEFTTORIGHT | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TREE"; ThatID = -1; Relation = "GLUE_DOWNTODOWN";
			IsProportional = false;
		}
	>>;
	<<
		Button
		{
			left = 469; top = 503; right = 560; bottom = 529; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = W2_SELECT; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 582; top = 503; right = 674; bottom = 530; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = W2_CANCEL; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Tree
		{
			left = 10; top = 8; right = 214; bottom = 485; IsVisible = true;
			IsDisabled = false; ID = "ID_TREE"; IsTabStop = true;
			IsGroupStart = false; HasLines = true; HasExpandButtons = true;
			HasChecks = false; IsReadOnly = true; ToStretchImage = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		}
	>>
}

Window "LIST_CARD_VIEW"
{
	left = 0; top = 0; right = 493; bottom = 454; IsVisible = true;
	IsDisabled = false; FgColor = color(0, 0, 0); IsWString = true;
	Type = "child"; HasBorder = false; IsResizable = false; HasSysMenu = false;
	HasMinButton = false; HasMaxButton = false; HasVScroll = true;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = true;
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
			left = 0; top = 0; right = 512; bottom = 456; IsVisible = true;
			IsDisabled = false; ID = "ID_FRAME"; IsTabStop = false;
			IsGroupStart = false; Type = "dip"; IsRim = true; IsDrag = false;
		}
	>>
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
	left = 0; top = 0; right = 226; bottom = 509; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "child"; HasBorder = false;
	IsResizable = false; HasSysMenu = false; HasMinButton = false;
	HasMaxButton = false; HasVScroll = false; HasHScroll = false;
	ToClipChildren = true; ToClipSiblings = true; HasOwnDC = false;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "ID_LIST_JPG"; ThatID = -1;
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
			ThisID = "ID_ROI_LIST"; ThatID = "ID_LIST_JPG";
			Relation = "GLUE_UPTODOWN"; IsProportional = false;
		}
	>>;
	<<
		Table
		{
			left = 3; top = 252; right = 222; bottom = 506; IsVisible = true;
			IsDisabled = false; ID = "ID_ROI_LIST"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = true;
			ToDitherSel = false;
			Props =
			<<
				<<
					W10_CONTOUR, 150, "TC_CENTER | TC_READONLY | TC_COMBOBOX"
				>>,
				<<
					"*", 43
				>>
			>>;
			ImageList =
			<<
				"EDIT", "CHECK"
			>>;
		},
		Table
		{
			left = 3; top = 2; right = 222; bottom = 246; IsVisible = true;
			IsDisabled = false; ID = "ID_LIST_JPG"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = false; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT"; HasVScroll = true;
			HasHScroll = false; IsReport = false; IsInputReturn = true;
			IsInputTab = false; ToStretchImage = false; ToDitherSel = false;
			Props =
			<<
				<<
					W11_IMAGE, 150, "TC_CENTER | TC_READONLY"
				>>,
				<<
					"*", 43
				>>
			>>;
			ImageList =
			<<
				"BLANK", "EDIT"
			>>;
		}
	>>
}

Window "MAIN_MENU"
{
	left = 0; top = 0; right = 532; bottom = 134; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup";
	Caption = "AllergoEye"; HasBorder = true; IsResizable = false;
	HasSysMenu = true; HasMinButton = true; HasMaxButton = false;
	HasVScroll = false; HasHScroll = false; ToClipChildren = false;
	ToClipSiblings = false; HasOwnDC = false;
	<<
		Text
		{
			left = 11; top = 98; right = 95; bottom = 118; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W9_NEW_PATIENT;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 112; top = 98; right = 196; bottom = 118; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_PATIENTS;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 216; top = 98; right = 299; bottom = 118; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_EXPORT_TO_EXCEL;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 320; top = 98; right = 404; bottom = 116; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W5_SETTINGS;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Text
		{
			left = 424; top = 98; right = 507; bottom = 116; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W9_EXIT;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Button
		{
			left = 15; top = 9; right = 89; bottom = 91; IsVisible = true;
			IsDisabled = false; ID = "ID_NEW_RECORD"; IsTabStop = true;
			IsGroupStart = false; ToolTip = "New record"; Bmp = "BMP_NEW_REC";
			IsDefault = false; IsExNotify = false; ToStretch = true;
		},
		Button
		{
			left = 118; top = 9; right = 192; bottom = 91; IsVisible = true;
			IsDisabled = false; ID = "ID_SELECT"; IsTabStop = true;
			IsGroupStart = false; ToolTip = "Select record"; Bmp = "BMP_SELECT";
			IsDefault = false; IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 219; top = 9; right = 294; bottom = 93; IsVisible = true;
			IsDisabled = false; ID = "ID_TOEXCEL"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_EXCEL"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 322; top = 9; right = 397; bottom = 91; IsVisible = true;
			IsDisabled = false; ID = "ID_OPTIONS"; IsTabStop = true;
			IsGroupStart = false; ToolTip = "Options"; Bmp = "BMP_OPTIONS";
			IsDefault = false; IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 424; top = 9; right = 498; bottom = 91; IsVisible = true;
			IsDisabled = false; ID = "ID_EXIT"; IsTabStop = true;
			IsGroupStart = false; ToolTip = "Exit"; Bmp = "BMP_EXIT";
			IsDefault = false; IsExNotify = false; ToStretch = false;
		}
	>>
}

Menu "POPUP_MENU"
{
	IsWString = true;
}


File "BMP_SYNC"
{
	Type = "bmp"; IsRef = true; Data = "res/sync.bmp";
}

Window "ID_SELECT_RECORD"
{
	left = 0; top = 0; right = 616; bottom = 467; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup";
	Caption = W7_CHOOSE_RECORD; HasBorder = true; IsResizable = true;
	HasSysMenu = true; HasMinButton = false; HasMaxButton = true;
	HasVScroll = false; HasHScroll = false; ToClipChildren = true;
	ToClipSiblings = true; HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "IDCANCEL"; ThatID = -1;
			Relation = "GLUE_UPTODOWN | GLUE_DOWNTODOWN | GLUE_LEFTTORIGHT | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "IDCANCEL"; ThatID = -1;
			Relation = "GLUE_UPTODOWN | GLUE_DOWNTODOWN | GLUE_LEFTTORIGHT | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TREE"; ThatID = -1; Relation = "GLUE_DOWNTODOWN";
			IsProportional = false;
		}
	>>;
	<<
		Table
		{
			left = 6; top = 7; right = 234; bottom = 393; IsVisible = true;
			IsDisabled = false; ID = "ID_TREE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					"#", 27
				>>,
				<<
					W8_RECORDS, 178
				>>
			>>;
			ImageList =
			<<
				"CHECK_BOX", "UNCHECK_BOX"
			>>;
		},
		Button
		{
			left = 369; top = 411; right = 459; bottom = 436; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = W2_SELECT; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 482; top = 411; right = 574; bottom = 438; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = W2_CANCEL; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		}
	>>
}

File "CHECK_BOX"
{
	Type = "ico"; IsRef = true; Data = "res/check_box.ico";
}

File "UNCHECK_BOX"
{
	Type = "ico"; IsRef = true; Data = "res/uncheck_box.ico";
}

File "BMP_EXCEL"
{
	Type = "bmp"; IsRef = true; Data = "res/excel.bmp";
}

File "BMP_DELETE"
{
	Type = "bmp"; IsRef = true; Data = "res/del.bmp";
}

File "BM_BUILD"
{
	Type = "bmp"; IsRef = true; Data = "res/build.bmp";
}

File "DLG_EDIT_CONTOURS_WND"
{
	Type = "ico"; IsRef = true; Data = "res/allergoeye.ico";
}

File "MAIN_MENU"
{
	Type = "ico"; IsRef = true; Data = "res/allergoeye.ico";
}



Window "DLG_CARD_TOOLS"
{
	left = 0; top = 0; right = 1268; bottom = 694; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup"; Caption = W1_PATIENTS;
	HasBorder = true; IsResizable = true; HasSysMenu = true;
	HasMinButton = true; HasMaxButton = true; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = true;
	HasOwnDC = true;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "ID_ADD"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_EXCEL"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_EXCEL_TXT"; ThatID = -1;
			Relation = "GLUE_WHOLETORIGHT"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_EXIT"; ThatID = -1;
			Relation = "GLUE_WHOLETODOWN | GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_NEWPATIENT"; ThatID = -1;
			Relation = "GLUE_WHOLETODOWN"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_GRAPH"; ThatID = -1;
			Relation = "GLUE_UPTOVCENTER | GLUE_DOWNTODOWN";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_HEADERS_TXT"; ThatID = -1;
			Relation = "GLUE_RIGHTTOHCENTER"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_HELP"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_HELP_TXT"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_LIST_ALL"; ThatID = -1; Relation = "GLUE_DOWNTODOWN";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_LIST_SELECT"; ThatID = -1;
			Relation = "GLUE_DOWNTODOWN | GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_LOAD_REPORT"; ThatID = -1;
			Relation = "GLUE_WHOLETORIGHT"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_MEAN_SCORE"; ThatID = -1;
			Relation = "GLUE_LEFTTOHCENTER | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_NEW_TEST"; ThatID = -1; Relation = "GLUE_WHOLETODOWN";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_PHONE_TXT"; ThatID = -1;
			Relation = "GLUE_WHOLETORIGHT"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_REMOVE"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_REPORT"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_REPORT_TXT"; ThatID = -1;
			Relation = "GLUE_WHOLETORIGHT"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_SEPARATOR"; ThatID = -1;
			Relation = "GLUE_DOWNTODOWN | GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_SEPARATOR_1"; ThatID = -1;
			Relation = "GLUE_WHOLETORIGHT"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_SMART"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_SYNC"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_SYNC_TXT"; ThatID = -1; Relation = "GLUE_WHOLETORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TEXT_LOAD_REORT"; ThatID = -1;
			Relation = "GLUE_WHOLETORIGHT"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_THERAPY"; ThatID = -1; Relation = "GLUE_WHOLETODOWN";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_THERAPY_TXT"; ThatID = -1;
			Relation = "GLUE_WHOLETODOWN"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TOTAL_SCORE"; ThatID = -1;
			Relation = "GLUE_WHOLETOVCENTER"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TRANSFER_TXT"; ThatID = -1;
			Relation = "GLUE_WHOLETORIGHT"; IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TREE"; ThatID = -1; Relation = "GLUE_DOWNTOVCENTER";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_VIEW"; ThatID = -1;
			Relation = "GLUE_DOWNTODOWN | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_WB"; ThatID = -1;
			Relation = "GLUE_WHOLETODOWN | GLUE_WHOLETORIGHT";
			IsProportional = false;
		}
	>>;
	<<
		Table
		{
			left = 527; top = 612; right = 916; bottom = 690; IsVisible = true;
			IsDisabled = false; ID = "ID_THERAPY"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 3;
			HasBorder = false; HasHeader = true; IsColumnTrack = false;
			IsColumnRescale = false; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false;
			State = "TC_CENTER | TC_READONLY | TC_IMAGE_CENTER";
			HasVScroll = true; HasHScroll = false; IsReport = false;
			IsInputReturn = false; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Props =
			<<
				<<
					W1_START_DATE, 90
				>>,
				<<
					W1_ALLERGEN, 144
				>>,
				<<
					W1_MEDICATION, 139
				>>
			>>;
		},
		Edit
		{
			left = 63; top = 18; right = 139; bottom = 38; IsVisible = true;
			IsDisabled = false; ID = "ID_PATIENT_ID"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Edit
		{
			left = 191; top = 18; right = 287; bottom = 38; IsVisible = true;
			IsDisabled = false; ID = "ID_PATIENT_LASTNAME"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Edit
		{
			left = 63; top = 40; right = 139; bottom = 59; IsVisible = true;
			IsDisabled = false; ID = "ID_TXT_FROM"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Edit
		{
			left = 191; top = 38; right = 267; bottom = 57; IsVisible = true;
			IsDisabled = false; ID = "ID_TXT_TO"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Frame
		{
			left = 4; top = 9; right = 379; bottom = 125; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; Type = "dip"; IsRim = true; IsDrag = false;
		},
		Text
		{
			left = 132; top = 4; right = 212; bottom = 16; IsVisible = true;
			IsDisabled = false; ID = "ID_DBSEARCH_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_FIND_TXT;
			TextAlign = "left";
		},
		Frame
		{
			left = 509; top = 128; right = 923; bottom = 584; IsVisible = true;
			IsDisabled = false; ID = "ID_VIEW"; IsTabStop = false;
			IsGroupStart = false; Type = "dip"; IsRim = true; IsDrag = false;
		},
		Button
		{
			left = 408; top = 6; right = 479; bottom = 64; IsVisible = true;
			IsDisabled = false; ID = "BTN_EDIT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_2EDIT"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 507; top = 6; right = 577; bottom = 64; IsVisible = true;
			IsDisabled = false; ID = "ID_BUILD"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BM_BUILD"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 793; top = 6; right = 863; bottom = 64; IsVisible = true;
			IsDisabled = false; ID = "ID_EXCEL"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_THERAPY_CTRL"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Table
		{
			left = 4; top = 127; right = 272; bottom = 651; IsVisible = true;
			IsDisabled = false; ID = "ID_LIST_ALL"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 4;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					"ID", 53
				>>,
				<<
					W1_LAST_NAME, 64
				>>,
				<<
					W1_FIRST_NAME, 65
				>>,
				<<
					W1_BIRTHDATE, 69
				>>
			>>;
		},
		Button
		{
			left = 297; top = 56; right = 344; bottom = 96; IsVisible = true;
			IsDisabled = false; ID = "BTN_SERACH"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_SEARCH"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 930; top = 217; right = 1000; bottom = 272; IsVisible = true;
			IsDisabled = false; ID = "ID_ADD"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_PLUS"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 930; top = 320; right = 1000; bottom = 376; IsVisible = true;
			IsDisabled = false; ID = "ID_REMOVE"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_MINUS"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 1152; top = 32; right = 1229; bottom = 88; IsVisible = true;
			IsDisabled = false; ID = "ID_SYNC"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_SYNC"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 1176; top = 612; right = 1247; bottom = 678;
			IsVisible = true; IsDisabled = false; ID = "ID_EXIT";
			IsTabStop = true; IsGroupStart = false; Bmp = "BMP_EXIT";
			IsDefault = false; IsExNotify = false; ToStretch = false;
		},
		Table
		{
			left = 1008; top = 116; right = 1247; bottom = 607;
			IsVisible = true; IsDisabled = false; ID = "ID_LIST_SELECT";
			IsWString = true; IsTabStop = true; IsGroupStart = false;
			LineCount = 0; ColumnCount = 2; HasBorder = true; HasHeader = true;
			IsColumnTrack = true; IsColumnRescale = true; HasGrid = true;
			IsMultiSelect = false; IsExSelect = false;
			State = "TC_LEFT | TC_READONLY"; HasVScroll = true;
			HasHScroll = false; IsReport = true; IsInputReturn = true;
			IsInputTab = false; ToStretchImage = false; ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					W1_NAME, 109
				>>,
				<<
					W10_TEST, 112
				>>
			>>;
		},
		Table
		{
			left = 274; top = 281; right = 502; bottom = 418; IsVisible = true;
			IsDisabled = false; ID = "ID_TREE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 5;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					"#", 22
				>>,
				<<
					W1_ALLERGEN, 60
				>>,
				<<
					W1_DILUTION, 60
				>>,
				<<
					W1_EYE, 33
				>>,
				<<
					W1_TIME, 36
				>>
			>>;
			ImageList =
			<<
				"CHECK_BOX", "UNCHECK_BOX"
			>>;
		},
		Text
		{
			left = 996; top = 8; right = 1144; bottom = 31; IsVisible = true;
			IsDisabled = false; FgColor = color(255, 0, 0); ID = "ID_PHONE_TXT";
			IsTabStop = false; IsGroupStart = false; HasBorder = false;
			Text = W1_SMARTPHONE; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 16, 1, 0
			>>;
		},
		Table
		{
			left = 1008; top = 34; right = 1149; bottom = 116; IsVisible = true;
			IsDisabled = false; ID = "ID_SMART"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 1;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER"; HasVScroll = true;
			HasHScroll = false; IsReport = false; IsInputReturn = true;
			IsInputTab = false; ToStretchImage = false; ToDitherSel = false;
			Props =
			<<
				<<
					W1_SMARTPHONE, 124
				>>
			>>;
		},
		Text
		{
			left = 777; top = 68; right = 880; bottom = 84; IsVisible = true;
			IsDisabled = false; ID = "ID_EXCEL_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_THERAPY_CONTROL;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Text
		{
			left = 386; top = 68; right = 495; bottom = 84; IsVisible = true;
			IsDisabled = false; ID = "ID_EDIT_CONT_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_EDIT_CONTOUR;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Text
		{
			left = 509; top = 68; right = 575; bottom = 84; IsVisible = true;
			IsDisabled = false; ID = "ID_CALCULATE_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_CALCULATE;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Table
		{
			left = 275; top = 172; right = 501; bottom = 277; IsVisible = true;
			IsDisabled = false; ID = "ID_DATE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 3;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					"#", 27
				>>,
				<<
					W1_TESTDATE, 82
				>>,
				<<
					W1_ALLERGEN, 99, "TC_LEFT | TC_READONLY"
				>>
			>>;
			ImageList =
			<<
				"CHECK_BOX", "UNCHECK_BOX"
			>>;
		},
		Button
		{
			left = 351; top = 656; right = 446; bottom = 680; IsVisible = true;
			IsDisabled = false; ID = "ID_NEW_TEST"; IsTabStop = true;
			IsGroupStart = false; Text = W1_NEWTEST; IsDefault = false;
			IsExNotify = false; ToStretch = false;
			Font =
			<<
				"Times New Roman", 12, 1, 0
			>>;
		},
		Text
		{
			left = 925; top = 287; right = 1005; bottom = 315; IsVisible = true;
			IsDisabled = false; ID = "ID_TRANSFER_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_TRANSFER;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Text
		{
			left = 1144; top = 95; right = 1243; bottom = 111; IsVisible = true;
			IsDisabled = false; ID = "ID_SYNC_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_SYNCHRONIZE;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Button
		{
			left = 697; top = 6; right = 772; bottom = 64; IsVisible = true;
			IsDisabled = false; ID = "ID_REPORT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_REPORT"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 694; top = 68; right = 770; bottom = 84; IsVisible = true;
			IsDisabled = false; ID = "ID_REPORT_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_REPORT;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Button
		{
			left = 600; top = 6; right = 676; bottom = 64; IsVisible = true;
			IsDisabled = false; ID = "ID_LOAD_REPORT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_LOAD_REPORT"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 591; top = 68; right = 696; bottom = 84; IsVisible = true;
			IsDisabled = false; ID = "ID_TEXT_LOAD_REORT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_LOAD_REPORT;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Text
		{
			left = 9; top = 24; right = 59; bottom = 38; IsVisible = true;
			IsDisabled = false; ID = "ID_ID_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = "ID";
			TextAlign = "right";
		},
		Text
		{
			left = 143; top = 22; right = 187; bottom = 38; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_LAST_NAME;
			TextAlign = "right";
		},
		ComboBox
		{
			left = 63; top = 61; right = 168; bottom = 269; IsVisible = true;
			IsDisabled = false; ID = "ID_TEST_ALERGEN"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		ComboBox
		{
			left = 191; top = 61; right = 296; bottom = 272; IsVisible = true;
			IsDisabled = false; ID = "ID_TEST_DIL"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		CustomControl
		{
			left = 144; top = 40; right = 160; bottom = 57; IsVisible = true;
			IsDisabled = false; ID = "ID_SDATE_FROM"; IsTabStop = true;
			IsGroupStart = false; ClassName = "GDatePicker";
			Attr =
			<<
				<<
					"CanBeEmpty", false
				>>,
				<<
					"IsDropDown", true
				>>,
				<<
					"IsLongFormat", false
				>>,
				<<
					"DateFormat", "dmy"
				>>,
				<<
					"Is2DigitYear", false
				>>
			>>;
		},
		Text
		{
			left = 9; top = 40; right = 59; bottom = 54; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_FROM;
			TextAlign = "right";
		},
		CustomControl
		{
			left = 271; top = 40; right = 287; bottom = 57; IsVisible = true;
			IsDisabled = false; ID = "ID_SDATE_TO"; IsTabStop = true;
			IsGroupStart = false; ClassName = "GDatePicker";
			Attr =
			<<
				<<
					"CanBeEmpty", false
				>>,
				<<
					"IsDropDown", true
				>>,
				<<
					"IsLongFormat", false
				>>,
				<<
					"DateFormat", "dmy"
				>>,
				<<
					"Is2DigitYear", false
				>>
			>>;
		},
		Text
		{
			left = 171; top = 40; right = 187; bottom = 54; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_TO;
			TextAlign = "right";
		},
		Button
		{
			left = 297; top = 13; right = 344; bottom = 54; IsVisible = true;
			IsDisabled = false; ID = "ID_CLEAR_SELECT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_CLEAR"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 9; top = 63; right = 59; bottom = 77; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_ALLERGEN;
			TextAlign = "right";
		},
		Edit
		{
			left = 781; top = 100; right = 920; bottom = 123; IsVisible = true;
			IsDisabled = false; FgColor = color(255, 0, 0);
			ID = "ID_MEAN_SCORE"; IsTabStop = true; IsGroupStart = false;
			HasBorder = false; Text = ""; TextAlign = "left"; IsReadOnly = true;
			IsOverride = false; IsAutoHScroll = false; IsPassword = false;
			ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 11, 1, 0
			>>;
		},
		Button
		{
			left = 56; top = 656; right = 146; bottom = 681; IsVisible = true;
			IsDisabled = false; ID = "ID_NEWPATIENT"; IsTabStop = true;
			IsGroupStart = false; Text = W9_NEW_PATIENT; IsDefault = false;
			IsExNotify = false; ToStretch = false;
			Font =
			<<
				"Times New Roman", 12, 1, 0
			>>;
		},
		Frame
		{
			left = 274; top = 441; right = 505; bottom = 653; IsVisible = false;
			IsDisabled = false; ID = "ID_GRAPH"; IsTabStop = false;
			IsGroupStart = false; Type = "dip"; IsRim = true; IsDrag = false;
		},
		Edit
		{
			left = 274; top = 422; right = 504; bottom = 440; IsVisible = true;
			IsDisabled = false; FgColor = color(255, 0, 0);
			ID = "ID_TOTAL_SCORE"; IsTabStop = true; IsGroupStart = false;
			HasBorder = true; Text = ""; TextAlign = "left"; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsPassword = false;
			ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Button
		{
			left = 507; top = 102; right = 592; bottom = 125; IsVisible = true;
			IsDisabled = false; ID = "ID_EDIT_OPSCORE"; IsTabStop = true;
			IsGroupStart = false; Text = W1_ADD_SCORE; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Table
		{
			left = 596; top = 89; right = 777; bottom = 127; IsVisible = true;
			IsDisabled = false; ID = "ID_HEADERS_TXT"; IsTabStop = true;
			IsGroupStart = false; LineCount = 2; ColumnCount = 4;
			HasBorder = false; HasHeader = false; IsColumnTrack = false;
			IsColumnRescale = true; HasGrid = true;
			GridColor = color(192, 192, 192); IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = false; HasHScroll = false; IsReport = false;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					"", 45
				>>,
				<<
					"", 45
				>>,
				<<
					"", 45
				>>,
				<<
					"", 45
				>>
			>>;
		},
		Frame
		{
			left = 973; top = 6; right = 976; bottom = 207; IsVisible = true;
			IsDisabled = false; ID = "ID_SEPARATOR_1"; IsTabStop = false;
			IsGroupStart = false; Type = "bump"; IsRim = false; IsDrag = false;
		},
		Frame
		{
			left = 973; top = 390; right = 976; bottom = 608; IsVisible = true;
			IsDisabled = false; ID = "ID_SEPARATOR"; IsTabStop = false;
			IsGroupStart = false; Type = "bump"; IsRim = false; IsDrag = false;
		},
		Text
		{
			left = 16; top = 105; right = 86; bottom = 116; IsVisible = true;
			IsDisabled = false; ID = "ID_NUM_PATIENTS"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = "";
			TextAlign = "left";
		},
		Check
		{
			left = 228; top = 104; right = 296; bottom = 120; IsVisible = true;
			IsDisabled = false; ID = "ID_NOSCORE"; IsTabStop = true;
			IsGroupStart = false; Text = W1_NO_SCORE; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = false; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 137; top = 104; right = 221; bottom = 120; IsVisible = true;
			IsDisabled = false; ID = "ID_FAILED_CONTOURS"; IsTabStop = true;
			IsGroupStart = false; Text = W1_FAILED; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = false; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 310; top = 104; right = 368; bottom = 120; IsVisible = true;
			IsDisabled = false; ID = "ID_TREATED"; IsTabStop = true;
			IsGroupStart = false; Text = W1_TREATED; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = false; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Text
		{
			left = 9; top = 84; right = 59; bottom = 96; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_MEDICATION;
			TextAlign = "right";
		},
		ComboBox
		{
			left = 63; top = 82; right = 168; bottom = 267; IsVisible = true;
			IsDisabled = false; ID = "ID_MED_LIST"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
		},
		Button
		{
			left = 384; top = 102; right = 429; bottom = 121; IsVisible = true;
			IsDisabled = false; ID = "ID_CAP"; IsTabStop = true;
			IsGroupStart = false; Text = W1_CAP_TXT; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Table
		{
			left = 274; top = 130; right = 500; bottom = 171; IsVisible = true;
			IsDisabled = false; ID = "ID_CAP_TXT"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = false; IsColumnTrack = false;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = false;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Props =
			<<
				<<
					"", 158
				>>,
				<<
					"", 48
				>>
			>>;
		},
		Text
		{
			left = 550; top = 591; right = 875; bottom = 608; IsVisible = true;
			IsDisabled = false; ID = "ID_THERAPY_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_TREATED;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 11, 1, 0
			>>;
		},
		Button
		{
			left = 896; top = 6; right = 966; bottom = 64; IsVisible = true;
			IsDisabled = false; ID = "ID_HELP"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_ALLERGOEYE"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 893; top = 68; right = 964; bottom = 84; IsVisible = true;
			IsDisabled = false; ID = "ID_HELP_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_HELP;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		}
	>>
}

File "BMP_2EDIT"
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

Window "DLG_NEW_TEST"
{
	left = 0; top = 0; right = 859; bottom = 96; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup"; Caption = W1_NEWTEST;
	HasBorder = true; IsResizable = false; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	<<
		Text
		{
			left = 5; top = 13; right = 204; bottom = 32; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W3_SELECT_TEST_NAME;
			TextAlign = "right";
		},
		Button
		{
			left = 322; top = 51; right = 411; bottom = 75; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "OK"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 440; top = 51; right = 531; bottom = 77; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = W2_CANCEL; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		ComboBox
		{
			left = 212; top = 11; right = 346; bottom = 218; IsVisible = true;
			IsDisabled = false; ID = "ID_TEST_ALERGEN"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		ComboBox
		{
			left = 350; top = 11; right = 485; bottom = 221; IsVisible = true;
			IsDisabled = false; ID = "ID_DIL"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		ComboBox
		{
			left = 499; top = 11; right = 633; bottom = 222; IsVisible = true;
			IsDisabled = false; ID = "ID_SIDE"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		ComboBox
		{
			left = 658; top = 11; right = 793; bottom = 223; IsVisible = true;
			IsDisabled = false; ID = "ID_TIME"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		}
	>>
}

File "BMP_DEL_IMAGE"
{
	Type = "bmp"; IsRef = true; Data = "res/del_image.bmp";
}

Window "DLG_RENAME_TEST"
{
	left = 0; top = 0; right = 340; bottom = 137; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup"; Caption = W4_RENAME;
	HasBorder = true; IsResizable = false; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	<<
		Button
		{
			left = 54; top = 98; right = 143; bottom = 122; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "Ok"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 185; top = 98; right = 275; bottom = 123; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = W2_CANCEL; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Check
		{
			left = 116; top = 18; right = 247; bottom = 40; IsVisible = true;
			IsDisabled = false; ID = "ID_MERGE"; IsTabStop = true;
			IsGroupStart = false; Text = W4_MERGE; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = true; IsRadio = true;
			IsAuto = true; Is3State = false; CheckAlign = "left";
		},
		Text
		{
			left = 142; top = 41; right = 181; bottom = 56; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W4_OR;
			TextAlign = "left";
		},
		Check
		{
			left = 117; top = 57; right = 200; bottom = 76; IsVisible = true;
			IsDisabled = false; ID = "ID_DEL"; IsTabStop = true;
			IsGroupStart = false; Text = W4_CLEAR; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		}
	>>
}

File "ALLERGOSCOPE"
{
	Type = "ico"; IsRef = true; Data = "res/allergoeye.ico";
}


Window "DLG_EXPORT"
{
	left = 0; top = 0; right = 1016; bottom = 646; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup"; Caption = "";
	HasBorder = true; IsResizable = false; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	<<
		Table
		{
			left = 13; top = 70; right = 365; bottom = 592; IsVisible = true;
			IsDisabled = false; ID = "ID_LIST_ALL"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 4;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = true;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Props =
			<<
				<<
					"ID", 70
				>>,
				<<
					W1_LAST_NAME, 93
				>>,
				<<
					W1_FIRST_NAME, 87
				>>,
				<<
					W1_BIRTHDATE, 73
				>>
			>>;
		},
		Table
		{
			left = 537; top = 72; right = 921; bottom = 592; IsVisible = true;
			IsDisabled = false; ID = "ID_TREE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Props =
			<<
				<<
					"#", 27, "TC_CENTER | TC_READONLY"
				>>,
				<<
					W1_TESTNAME, 335
				>>
			>>;
			ImageList =
			<<
				"CHECK_BOX", "UNCHECK_BOX"
			>>;
		},
		Table
		{
			left = 370; top = 72; right = 530; bottom = 592; IsVisible = true;
			IsDisabled = false; ID = "ID_DATE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 2;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Props =
			<<
				<<
					"#", 27
				>>,
				<<
					W1_TESTDATE, 98
				>>
			>>;
			ImageList =
			<<
				"CHECK_BOX", "UNCHECK_BOX"
			>>;
		},
		Edit
		{
			left = 13; top = 40; right = 84; bottom = 61; IsVisible = true;
			IsDisabled = false; ID = "ID_SEARCH_ID"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Edit
		{
			left = 89; top = 40; right = 171; bottom = 61; IsVisible = true;
			IsDisabled = false; ID = "ID_SEARCH_FAM"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Button
		{
			left = 936; top = 571; right = 1005; bottom = 633; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_EXIT"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 936; top = 11; right = 1003; bottom = 73; IsVisible = true;
			IsDisabled = false; ID = "ID_EXCEL"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_EXCEL"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Edit
		{
			left = 374; top = 41; right = 496; bottom = 66; IsVisible = true;
			IsDisabled = false; ID = "ID_SEARCH_DATE"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Edit
		{
			left = 537; top = 41; right = 882; bottom = 63; IsVisible = true;
			IsDisabled = false; ID = "ID_SEARCH_TEST"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = true; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Button
		{
			left = 13; top = 601; right = 367; bottom = 626; IsVisible = true;
			IsDisabled = false; ID = "BTN_CLEAR_SELECT"; IsTabStop = true;
			IsGroupStart = false; Text = W1_CLEAR_SELECT; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 502; top = 41; right = 530; bottom = 68; IsVisible = true;
			IsDisabled = false; ID = "ID_CLEAR_DATE"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_CLEAR_SMALL"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 891; top = 41; right = 918; bottom = 68; IsVisible = true;
			IsDisabled = false; ID = "ID_CLEAR_TEST"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_CLEAR_SMALL"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 16; top = 25; right = 96; bottom = 38; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_FILTER;
			TextAlign = "left";
		}
	>>
}

File "BMP_CLEAR"
{
	Type = "bmp"; IsRef = true; Data = "res/clear.bmp";
}

File "BMP_CLEAR_SMALL"
{
	Type = "bmp"; IsRef = true; Data = "res/clear1.bmp";
}

Window "REPORT_WND"
{
	left = 0; top = 0; right = 700; bottom = 600; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup";
	Caption = W1_NEWREPORT; HasBorder = true; IsResizable = true;
	HasSysMenu = true; HasMinButton = true; HasMaxButton = true;
	HasVScroll = false; HasHScroll = false; ToClipChildren = true;
	ToClipSiblings = false; HasOwnDC = false;
}

Window "REPORT_CHILD"
{
	left = 0; top = 0; right = 543; bottom = 291; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "child"; HasBorder = false;
	IsResizable = false; HasSysMenu = false; HasMinButton = false;
	HasMaxButton = false; HasVScroll = true; HasHScroll = true;
	ToClipChildren = true; ToClipSiblings = false; HasOwnDC = false;
}

Window "REPORT_MENU"
{
	left = 0; top = 0; right = 872; bottom = 101; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "child"; HasBorder = true;
	IsResizable = false; HasSysMenu = false; HasMinButton = false;
	HasMaxButton = false; HasVScroll = false; HasHScroll = false;
	ToClipChildren = true; ToClipSiblings = true; HasOwnDC = false;
	GluePairs =
	<<
		GluePair
		{
			ThisID = "CMD_REPPRINT"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_EXIT"; ThatID = -1;
			Relation = "GLUE_LEFTTORIGHT | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_LOAD_REPORT"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_SAVE_SRLZ"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TEXT_LOAD_REORT"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TEXT_PRINT_REORT"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TEXT_SAVE_REORT"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_TEXT_VIEW_IMG"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "ID_VIEW_IMAGES"; ThatID = -1;
			Relation = "GLUE_LEFTTOLEFT | GLUE_RIGHTTOLEFT";
			IsProportional = false;
		},
		GluePair
		{
			ThisID = "TXT_EXIT"; ThatID = -1;
			Relation = "GLUE_LEFTTORIGHT | GLUE_RIGHTTORIGHT";
			IsProportional = false;
		}
	>>;
	<<
		Button
		{
			left = 6; top = 0; right = 84; bottom = 63; IsVisible = true;
			IsDisabled = false; ID = "CMD_REPPRINT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_PRINT_REPORT"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 102; top = 0; right = 180; bottom = 63; IsVisible = true;
			IsDisabled = false; ID = "ID_SAVE_SRLZ"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_SAVE_REPORT"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		FixButton
		{
			left = 198; top = 0; right = 276; bottom = 63; IsVisible = true;
			IsDisabled = false; ID = "ID_VIEW_IMAGES"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_EYEVIS"; IsDefault = false;
			IsExNotify = false; ToStretch = true; State = true; IsRadio = false;
			IsAuto = true;
		},
		Button
		{
			left = 797; top = 10; right = 851; bottom = 64; IsVisible = true;
			IsDisabled = false; ID = "ID_EXIT"; IsTabStop = true;
			IsGroupStart = false; ToolTip = "Exit"; Bmp = "BMP_EXIT";
			IsDefault = false; IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 92; top = 70; right = 171; bottom = 94; IsVisible = true;
			IsDisabled = false; ID = "ID_TEXT_SAVE_REORT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_SAVE_REPORT;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Text
		{
			left = 3; top = 70; right = 83; bottom = 94; IsVisible = true;
			IsDisabled = false; ID = "ID_TEXT_PRINT_REORT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_PRINT_REPORT;
			TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Text
		{
			left = 181; top = 70; right = 288; bottom = 94; IsVisible = true;
			IsDisabled = false; ID = "ID_TEXT_VIEW_IMG"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false;
			Text = W1_VIEW_IMAGES_REPORT; TextAlign = "center";
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Text
		{
			left = 796; top = 71; right = 855; bottom = 88; IsVisible = true;
			IsDisabled = false; ID = "TXT_EXIT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W9_EXIT;
			TextAlign = "center";
		}
	>>
}

File "BMP_LOAD_REPORT"
{
	Type = "bmp"; IsRef = true; Data = "res/loadreport.bmp";
}

File "BMP_SAVE_REPORT"
{
	Type = "bmp"; IsRef = true; Data = "res/savereport.bmp";
}

File "BMP_PRINT_REPORT"
{
	Type = "bmp"; IsRef = true; Data = "res/printreport.bmp";
}

File "BMP_REPORT_VIEW"
{
	Type = "bmp"; IsRef = true; Data = "res/reportview.bmp";
}

Window "DLG_SELECT_REPORT"
{
	left = 0; top = 0; right = 361; bottom = 275; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup";
	Caption = "Select report"; HasBorder = true; IsResizable = false;
	HasSysMenu = true; HasMinButton = false; HasMaxButton = false;
	HasVScroll = false; HasHScroll = false; ToClipChildren = false;
	ToClipSiblings = false; HasOwnDC = false;
	Font =
	<<
		"Times New Roman", 9, 0, 0
	>>;
	<<
		Button
		{
			left = 61; top = 229; right = 129; bottom = 253; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "Ok"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 229; top = 229; right = 297; bottom = 253; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = "Cancel"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Table
		{
			left = 16; top = 9; right = 351; bottom = 219; IsVisible = true;
			IsDisabled = false; ID = "ID_LIST_REPORTS"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 3;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					"ID", 103
				>>,
				<<
					"Date", 103
				>>,
				<<
					"Time", 108
				>>
			>>;
		}
	>>
}

Window "GRAPH_WND"
{
	left = 0; top = 0; right = 369; bottom = 302; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "child"; HasBorder = false;
	IsResizable = false; HasSysMenu = false; HasMinButton = false;
	HasMaxButton = false; HasVScroll = false; HasHScroll = false;
	ToClipChildren = true; ToClipSiblings = true; HasOwnDC = false;
}


File "BMP_EYEVIS"
{
	Type = "bmp"; IsRef = true; Data = "res/eyevis.bmp";
}

File "BMP_REPORT"
{
	Type = "bmp"; IsRef = true; Data = "res/reportview.bmp";
}

File "BMP_EYEINVIS"
{
	Type = "bmp"; IsRef = true; Data = "res/eyeinvis.bmp";
}

Window "DLG_OP_SCORE"
{
	left = 0; top = 0; right = 322; bottom = 233; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup";
	Caption = "Symptom Score"; HasBorder = true; IsResizable = false;
	HasSysMenu = true; HasMinButton = false; HasMaxButton = false;
	HasVScroll = false; HasHScroll = false; ToClipChildren = false;
	ToClipSiblings = false; HasOwnDC = false;
	<<
		Table
		{
			left = 30; top = 16; right = 311; bottom = 39; IsVisible = true;
			IsDisabled = false; ID = "ID_HEADERS_TXT"; IsTabStop = true;
			IsGroupStart = false; LineCount = 1; ColumnCount = 4;
			HasBorder = false; HasHeader = false; IsColumnTrack = false;
			IsColumnRescale = false; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = false; HasHScroll = false; IsReport = false;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
			Props =
			<<
				<<
					"", 69
				>>,
				<<
					"", 69
				>>,
				<<
					"", 69
				>>,
				<<
					"", 73
				>>
			>>;
		},
		Table
		{
			left = 9; top = 36; right = 34; bottom = 185; IsVisible = true;
			IsDisabled = false; ID = "ID_SCORE_VALUE_TXT"; IsTabStop = true;
			IsGroupStart = false; LineCount = 8; ColumnCount = 1;
			HasBorder = false; HasHeader = false; IsColumnTrack = false;
			IsColumnRescale = false; HasGrid = false; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = false; HasHScroll = false; IsReport = false;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 1, 0
			>>;
		},
		Check
		{
			left = 59; top = 57; right = 89; bottom = 76; IsVisible = true;
			IsDisabled = false; ID = "ID_ITCHING_0"; IsTabStop = true;
			IsGroupStart = true; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 59; top = 91; right = 89; bottom = 110; IsVisible = true;
			IsDisabled = false; ID = "ID_ITCHING_1"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 59; top = 125; right = 89; bottom = 144; IsVisible = true;
			IsDisabled = false; ID = "ID_ITCHING_2"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 59; top = 159; right = 89; bottom = 177; IsVisible = true;
			IsDisabled = false; ID = "ID_ITCHING_3"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 118; top = 57; right = 148; bottom = 76; IsVisible = true;
			IsDisabled = false; ID = "ID_IRRITATION_0"; IsTabStop = true;
			IsGroupStart = true; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 118; top = 91; right = 148; bottom = 110; IsVisible = true;
			IsDisabled = false; ID = "ID_IRRITATION_1"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 118; top = 125; right = 148; bottom = 144; IsVisible = true;
			IsDisabled = false; ID = "ID_IRRITATION_2"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 118; top = 159; right = 148; bottom = 177; IsVisible = true;
			IsDisabled = false; ID = "ID_IRRITATION_3"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 189; top = 57; right = 219; bottom = 76; IsVisible = true;
			IsDisabled = false; ID = "ID_TEARS_0"; IsTabStop = true;
			IsGroupStart = true; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 189; top = 91; right = 219; bottom = 110; IsVisible = true;
			IsDisabled = false; ID = "ID_TEARS_1"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 189; top = 125; right = 219; bottom = 144; IsVisible = true;
			IsDisabled = false; ID = "ID_TEARS_2"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 189; top = 159; right = 219; bottom = 177; IsVisible = true;
			IsDisabled = false; ID = "ID_TEARS_3"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 256; top = 57; right = 287; bottom = 76; IsVisible = true;
			IsDisabled = false; ID = "ID_REDNESS_0"; IsTabStop = true;
			IsGroupStart = true; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 256; top = 91; right = 287; bottom = 110; IsVisible = true;
			IsDisabled = false; ID = "ID_REDNESS_1"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 256; top = 125; right = 287; bottom = 144; IsVisible = true;
			IsDisabled = false; ID = "ID_REDNESS_2"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Check
		{
			left = 256; top = 159; right = 287; bottom = 177; IsVisible = true;
			IsDisabled = false; ID = "ID_REDNESS_3"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsDefault = false;
			IsExNotify = false; ToStretch = false; State = false;
			IsRadio = true; IsAuto = true; Is3State = false;
			CheckAlign = "left";
		},
		Button
		{
			left = 46; top = 194; right = 135; bottom = 217; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "OK"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 178; top = 193; right = 268; bottom = 217; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = "Cancel"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		}
	>>
}

Window "DLG_ADD_THERAPY"
{
	left = 0; top = 0; right = 260; bottom = 125; IsVisible = true;
	IsDisabled = false; IsWString = true; Type = "popup"; Caption = "Therapy";
	HasBorder = true; IsResizable = false; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = false;
	<<
		Edit
		{
			left = 143; top = 16; right = 223; bottom = 36; IsVisible = true;
			IsDisabled = false; ID = "ID_START_DATE"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		ComboBox
		{
			left = 143; top = 38; right = 240; bottom = 232; IsVisible = true;
			IsDisabled = false; ID = "ID_ALLERGEN"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = true;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
		},
		Edit
		{
			left = 143; top = 61; right = 223; bottom = 80; IsVisible = true;
			IsDisabled = false; ID = "ID_MEDICATION"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Button
		{
			left = 34; top = 89; right = 121; bottom = 111; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Text = "Ok"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 132; top = 89; right = 219; bottom = 111; IsVisible = true;
			IsDisabled = false; ID = "IDCANCEL"; IsTabStop = true;
			IsGroupStart = false; Text = W2_CANCEL; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 16; top = 18; right = 134; bottom = 34; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_START_DATE;
			TextAlign = "right";
		},
		Text
		{
			left = 16; top = 41; right = 134; bottom = 57; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_ALLERGEN;
			TextAlign = "right";
		},
		Text
		{
			left = 16; top = 63; right = 134; bottom = 79; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_MEDICATION;
			TextAlign = "right";
		}
	>>
}

File "BMP_ALLERGOEYE"
{
	Type = "bmp"; IsRef = true; Data = "res/view1.bmp";
}

File "BMP_THERAPY_CTRL"
{
	Type = "bmp"; IsRef = true; Data = "res/graph.bmp";
}

Accelerator "DLG_CARD_TOOLS"
{
	<<
		AccelItem
		{
			Key = "VK_F"; ID = "ID_FILTER_LIST";
			Modifiers =
			<<
				"VK_CONTROL"
			>>;
		}
	>>
}

Window "DLG_EXPORT2"
{
	left = 0; top = 0; right = 1044; bottom = 706; IsVisible = true;
	IsDisabled = false; IsWString = false; Type = "popup"; Caption = "";
	HasBorder = true; IsResizable = false; HasSysMenu = true;
	HasMinButton = false; HasMaxButton = false; HasVScroll = false;
	HasHScroll = false; ToClipChildren = false; ToClipSiblings = false;
	HasOwnDC = false;
	<<
		Button
		{
			left = 950; top = 633; right = 1019; bottom = 696; IsVisible = true;
			IsDisabled = false; ID = "IDOK"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_EXIT"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 952; top = 25; right = 1017; bottom = 86; IsVisible = true;
			IsDisabled = false; ID = "ID_EXCEL"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_EXCEL"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Button
		{
			left = 11; top = 636; right = 364; bottom = 661; IsVisible = true;
			IsDisabled = false; ID = "BTN_CLEAR_SELECT"; IsTabStop = true;
			IsGroupStart = false; Text = W1_CLEAR_SELECT; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Edit
		{
			left = 73; top = 16; right = 173; bottom = 36; IsVisible = true;
			IsDisabled = false; ID = "ID_PATIENT_ID"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Edit
		{
			left = 244; top = 16; right = 333; bottom = 36; IsVisible = true;
			IsDisabled = false; ID = "ID_PATIENT_LASTNAME"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
		},
		Edit
		{
			left = 73; top = 38; right = 155; bottom = 57; IsVisible = true;
			IsDisabled = false; ID = "ID_TXT_FROM"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Edit
		{
			left = 201; top = 40; right = 283; bottom = 57; IsVisible = true;
			IsDisabled = false; ID = "ID_TXT_TO"; IsTabStop = true;
			IsGroupStart = false; HasBorder = true; Text = "";
			TextAlign = "left"; IsReadOnly = false; IsOverride = false;
			IsAutoHScroll = false; IsPassword = false; ToKeepSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Frame
		{
			left = 13; top = 7; right = 389; bottom = 96; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; Type = "dip"; IsRim = true; IsDrag = false;
		},
		Button
		{
			left = 338; top = 54; right = 386; bottom = 93; IsVisible = true;
			IsDisabled = false; ID = "BTN_SERACH"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_SEARCH"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 20; top = 22; right = 70; bottom = 36; IsVisible = true;
			IsDisabled = false; ID = "ID_ID_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = "ID";
			TextAlign = "right";
		},
		Text
		{
			left = 178; top = 18; right = 235; bottom = 34; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_LAST_NAME;
			TextAlign = "right";
		},
		ComboBox
		{
			left = 73; top = 59; right = 178; bottom = 267; IsVisible = true;
			IsDisabled = false; ID = "ID_TEST_ALERGEN"; IsTabStop = true;
			IsGroupStart = false; Text = ""; IsReadOnly = false;
			IsOverride = false; IsAutoHScroll = false; IsSorted = false;
			HasVScroll = true; HasHScroll = false; IsDropDown = true;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
		},
		Button
		{
			left = 338; top = 11; right = 386; bottom = 52; IsVisible = true;
			IsDisabled = false; ID = "ID_CLEAR_SELECT"; IsTabStop = true;
			IsGroupStart = false; Bmp = "BMP_CLEAR"; IsDefault = false;
			IsExNotify = false; ToStretch = false;
		},
		Text
		{
			left = 145; top = 0; right = 226; bottom = 13; IsVisible = true;
			IsDisabled = false; ID = "ID_DBSEARCH_TXT"; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_FIND_TXT;
			TextAlign = "left";
		},
		Table
		{
			left = 15; top = 100; right = 367; bottom = 623; IsVisible = true;
			IsDisabled = false; ID = "ID_LIST_ALL"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 4;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = true;
			IsExSelect = false; State = "TC_LEFT | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Props =
			<<
				<<
					"ID", 70
				>>,
				<<
					W1_LAST_NAME, 93
				>>,
				<<
					W1_FIRST_NAME, 87
				>>,
				<<
					W1_BIRTHDATE, 73
				>>
			>>;
		},
		Table
		{
			left = 370; top = 100; right = 612; bottom = 624; IsVisible = true;
			IsDisabled = false; ID = "ID_DATE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 3;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					"#", 27
				>>,
				<<
					W1_TESTDATE, 82
				>>,
				<<
					W1_ALLERGEN, 113, "TC_LEFT | TC_READONLY"
				>>
			>>;
			ImageList =
			<<
				"CHECK_BOX", "UNCHECK_BOX"
			>>;
		},
		Table
		{
			left = 617; top = 100; right = 1024; bottom = 623; IsVisible = true;
			IsDisabled = false; ID = "ID_TREE"; IsTabStop = true;
			IsGroupStart = false; LineCount = 0; ColumnCount = 5;
			HasBorder = true; HasHeader = true; IsColumnTrack = true;
			IsColumnRescale = true; HasGrid = true; IsMultiSelect = false;
			IsExSelect = false; State = "TC_CENTER | TC_READONLY";
			HasVScroll = true; HasHScroll = false; IsReport = true;
			IsInputReturn = true; IsInputTab = false; ToStretchImage = false;
			ToDitherSel = false;
			Font =
			<<
				"Times New Roman", 9, 0, 0
			>>;
			Props =
			<<
				<<
					"#", 22
				>>,
				<<
					W1_ALLERGEN, 105
				>>,
				<<
					W1_DILUTION, 105
				>>,
				<<
					W1_EYE, 97
				>>,
				<<
					W1_TIME, 57
				>>
			>>;
			ImageList =
			<<
				"CHECK_BOX", "UNCHECK_BOX"
			>>;
		},
		Text
		{
			left = 16; top = 40; right = 66; bottom = 54; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_FROM;
			TextAlign = "right";
		},
		Text
		{
			left = 16; top = 61; right = 66; bottom = 75; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_ALLERGEN;
			TextAlign = "right";
		},
		Text
		{
			left = 177; top = 40; right = 193; bottom = 54; IsVisible = true;
			IsDisabled = false; ID = -1; IsTabStop = false;
			IsGroupStart = false; HasBorder = false; Text = W1_TO;
			TextAlign = "right";
		},
		CustomControl
		{
			left = 160; top = 40; right = 176; bottom = 56; IsVisible = true;
			IsDisabled = false; ID = "ID_SDATE_FROM"; IsTabStop = true;
			IsGroupStart = false; ClassName = "GDatePicker";
			Attr =
			<<
				<<
					"CanBeEmpty", false
				>>,
				<<
					"IsDropDown", true
				>>,
				<<
					"IsLongFormat", false
				>>,
				<<
					"DateFormat", "dmy"
				>>,
				<<
					"Is2DigitYear", false
				>>
			>>;
		},
		CustomControl
		{
			left = 287; top = 40; right = 303; bottom = 56; IsVisible = true;
			IsDisabled = false; ID = "ID_SDATE_TO"; IsTabStop = true;
			IsGroupStart = false; ClassName = "GDatePicker";
			Attr =
			<<
				<<
					"CanBeEmpty", false
				>>,
				<<
					"IsDropDown", true
				>>,
				<<
					"IsLongFormat", false
				>>,
				<<
					"DateFormat", "dmy"
				>>,
				<<
					"Is2DigitYear", false
				>>
			>>;
		}
	>>
}
