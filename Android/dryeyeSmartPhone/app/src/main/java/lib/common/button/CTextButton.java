package lib.common.button;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;

import lib.common.ActionCallback;
import lib.utils.FileUtils;

public class CTextButton extends androidx.appcompat.widget.AppCompatButton {
    ActionCallback callback = null;

    public CTextButton(Context context)
    {
        super(context);
    }

    public CTextButton(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public CTextButton(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
    }

    public void Init(ActionCallback c)
    {
        callback = c;
        if (c == null)
            setOnClickListener(null);
        else
            setOnClickListener(new ClickListener());

    }
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void SetCallback(Object _object, String func)
    {
        callback = new ActionCallback(_object, func);
        setOnClickListener(new ClickListener());
    }

    //********************************************************************************
    protected class ClickListener implements OnClickListener {


        @Override
        public void onClick(View v)
        {
            try
            {
                callback.run();
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }

        }

    }
}
