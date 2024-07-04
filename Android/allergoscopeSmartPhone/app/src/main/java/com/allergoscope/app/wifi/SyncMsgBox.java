package com.allergoscope.app.wifi;

import lib.common.MessageBox;

public class SyncMsgBox extends MessageBox {
    public static boolean onFinish = true;
    public SyncMsgBox(String _lpText, String _lpCaption, int _uType)
    {
        super(_lpText, _lpCaption, _uType);
    }
    @Override
    public int DoModal()
    {
        onFinish = false;
        int re = super.DoModal();
        onFinish = true;
        return re;
    }
}
