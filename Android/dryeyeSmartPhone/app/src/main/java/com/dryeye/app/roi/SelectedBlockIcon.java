package com.dryeye.app.roi;

import android.graphics.Bitmap;

public class SelectedBlockIcon {
    public Bitmap[] bmp;
    public boolean[] fSel;
    public String[] names;
    public int[] colors;

    public SelectedBlockIcon(Bitmap[] bi, boolean[] _fSel, String[] _names, int[] _colors)
    {
        colors = _colors;
        names = _names;
        fSel = _fSel;
        bmp = bi;

    }

    public int Count()
    {
        return names.length;
    }
}