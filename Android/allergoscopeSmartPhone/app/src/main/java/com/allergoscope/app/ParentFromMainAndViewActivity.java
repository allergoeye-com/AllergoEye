package com.allergoscope.app;


import android.os.Bundle;

import java.util.HashMap;

import lib.OrientActivity;
import lib.common.ActionCallback;
import lib.utils.AGlobals;

public class ParentFromMainAndViewActivity extends OrientActivity {
    public HashMap<String, ActionCallback> menuMainMenu;
    public HashMap<String, ActionCallback> menuViewMenu;

    protected void onCreate(Bundle savedInstanceState)
    {

        SetParent(AGlobals.currentActivity);
        super.onCreate(savedInstanceState);

    }


}