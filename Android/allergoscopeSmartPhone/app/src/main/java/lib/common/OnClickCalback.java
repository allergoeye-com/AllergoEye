package lib.common;

import android.view.View;

import lib.utils.FileUtils;

public class OnClickCalback extends ActionCallback implements View.OnClickListener {

    public OnClickCalback(Object obj, String func)
    {
        SetCallback(obj, func, View.class);
    }

    @Override
    public void onClick(View v)
    {
        try
        {
            run(v);
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }

    }
}
