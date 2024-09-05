package lib.common;

import android.view.View;

public class OnLongClickCalback extends ActionCallback implements View.OnLongClickListener{

    public OnLongClickCalback(Object obj, String func)
    {
        SetCallback(obj, func, View.class);
    }

    @Override
    public boolean onLongClick(View v) {
        try {
            run(v);
        } catch (Exception e) {
            return false;
        }
        return true;
    }
}
