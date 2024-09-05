package lib.common.button;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;

import androidx.annotation.Nullable;

import android.util.AttributeSet;

import com.dryeye.app.R;


/**
 * Created by alex on 22.01.2018.
 */

public class CMenuTogleButton extends CMenuButton {
    protected boolean isTogleColor = true;
    protected boolean isDown = false;
    private Drawable drawable = null;

    public CMenuTogleButton(Context context)
    {
        super(context);
        InitAtribut(context, null, 0, 0);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    public CMenuTogleButton(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    public CMenuTogleButton(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.CMenuTogleButton, defA, defS);
            try
            {
                isDown = a.getBoolean(R.styleable.CMenuTogleButton_icon_pushed, false);
                isTogleColor = a.getBoolean(R.styleable.CMenuTogleButton_eneble_togle_color, false);

            } catch (Exception e)
            {
                ;
            }
            a.recycle();
        } else
        {
            push = null;
            pop = null;
            isTogleColor = true;
            isDown = false;

        }
        updateState();
    }

    @Override
    public void setImageDrawable(@Nullable Drawable _drawable)
    {
        if (drawable == null || !drawable.equals(_drawable))
        {
            drawable = _drawable;
            super.setImageDrawable(drawable);
        }
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public boolean IsDown()
    {
        return isDown;
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    protected void SetState()
    {
        //if (!fSetCalback)
        isDown = !isDown;
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public void UpdateState()
    {
        SetState();
        updateState();
        if (isDown)
        {
            if (push != null)
                setImageDrawable(push);
        } else
        {
            if (pop != null)
                setImageDrawable(pop);

        }
    }

    private void updateState()
    {
        super.UpdateState();
        if (isDown)
        {
            if (push != null)
                setImageDrawable(push);
        } else
        {
            if (pop != null)
                setImageDrawable(pop);

        }
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    protected void UpdateColor()
    {
        if (!isTogleColor)
            SetTwoColor();
        else
        {
            if (isDown)
                SetSingeColor(color_btn_down, push);
            else
                SetSingeColor(color_btn, pop);
        }

    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public void SetCheck(boolean b)
    {

        isDown = !b;
        UpdateState();


    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public boolean GetCheck()
    {
        return isDown;

    }

}
