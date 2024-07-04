package lib.common.button;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;


import com.allergoscope.app.R;

import lib.common.AActivity;
import lib.common.IControlOnClick;
import lib.utils.AGlobals;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by alex on 25.01.2018.
 */

public class CSpinButton extends CMenuButton implements IControlOnClick {
    Object sync = new Object();
    boolean fRunSpin = false;
    protected Call callback_spin = new Call();
    int speed = 200;
    Rect clipRect = null;
    private boolean fSpinMode;

    public CSpinButton(Context context) {
        super(context);
        _InitAtribut(context, null);

    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public CSpinButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        _InitAtribut(context, attrs);

    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public CSpinButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        _InitAtribut(context, attrs);
    }
    public void SetSpinMode(boolean b)
    {
        fSpinMode = b;
    }
    //-------------------------------------------
    //
    //-------------------------------------------
    private void _InitAtribut(Context context, AttributeSet attrs)
    {
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.CSpinButton, 0, 0);
            try {
                fSpinMode = a.getBoolean(R.styleable.CSpinButton_is_spin_mode, true);


            }

            catch(Exception e)
            {
                ;
            }
            a.recycle();
        }
        else
        {
            fSpinMode = true;

        }

    }
    public void SetSpeed(int s)
    {
        speed = s;
    }
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    @Override
    public boolean OnClick()
    {

        if (clipRect != null && !clipRect.contains(ScreenRect())) return false;
        if (!fDisable)
        {
            if (!fSpinMode)
            {
                callback_spin.run();
            }
            else
            if (((AActivity)AGlobals.currentActivity).SetCapture(this))
            {
                fRunSpin = true;
                SetSingeColor(color_btn_down, push);
                Spin();
            }
            return true;
        }
        return false;
    }

    @Override
    public Rect ScreenRect() {
        return GetScreenRect();
    }

    @Override
    public void SetClipRect(Rect _clipRect) {
        clipRect = _clipRect;
    }

    @Override
    public boolean IsEnableEvent() {
        return super.IsEnable();
    }

    @Override
    public void Detach(ArrayList<Object> stack)
    {
        if (!stack.contains(this))
        {
            stack.add(this);
            ((AActivity)AGlobals.currentActivity).ReleaseCapture(this);
            callback.Detach(stack);
            callback_spin.Detach(stack);
            setOnClickListener(null);
        }
    }
    //---------------------------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------------------------
    @Override
    public void SetCallback(Object _object, String func)
    {
        setOnClickListener(null);
        callback_spin.SetCallback(_object, func);
    }


    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {

        if (AGlobals.currentActivity instanceof AActivity)
        {
            View v = ((AActivity)AGlobals.currentActivity).GetCapture();
            if (v != null && v.equals(this))
            {
                int action = event.getAction();
                if ((action & MotionEvent.ACTION_MASK) ==MotionEvent.ACTION_UP) //  || (action & MotionEvent.ACTION_MASK) ==MotionEvent.ACTION_CANCEL)
                {
                    synchronized(sync)
                    {
                        fRunSpin = false;
                        SetSingeColor(color_btn, pop);
                        ((AActivity)AGlobals.currentActivity).ReleaseCapture(this);
                        return super.onTouchEvent(event);
                    }
                }
                return true;
            }
        }
        return false;
    }
    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public boolean IsDown() { return fRunSpin; }
    @Override
    public void Enable(boolean b)
    {
        if (!b)
        {
            if (fRunSpin)
            {
                fRunSpin = false;
                SetSingeColor(color_btn, pop);
                ((AActivity)AGlobals.currentActivity).ReleaseCapture(this);
            }

        }
        super.Enable(b);
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    protected void Spin()
    {

        if (fRunSpin)
        {
            if (!IsEnable()) return;
            callback_spin.run();
            if (!IsEnable())
            {
                UpdateState();
                return;
            }
            (new Timer()).schedule(new TimerTask(){
                @Override
                public void run() {
                    synchronized(sync)
                    {
                        if (!fRunSpin || !IsEnable())
                            return;
                   }
                    AGlobals.currentActivity.runOnUiThread(new Runnable(){
                        @Override
                        public void run() {
                            Spin();
                        }});

                }}, speed);
        }

    }

}
