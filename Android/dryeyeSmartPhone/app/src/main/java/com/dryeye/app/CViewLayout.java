package com.dryeye.app;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;

import androidx.annotation.Nullable;

import lib.common.ActionCallback;

public class CViewLayout extends LinearLayout {
    public ActionCallback CalbackOnLayout = null;
    public CViewLayout(Context context)
    {
        super(context);
    }

    public CViewLayout(Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
    }

    public CViewLayout(Context context, @Nullable AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
    }

    public CViewLayout(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes)
    {
        super(context, attrs, defStyleAttr, defStyleRes);
    }
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        super.onLayout(changed, left, top, right, bottom);
        if (CalbackOnLayout != null)
            CalbackOnLayout.run(changed, left, top, right, bottom);
    }

}
