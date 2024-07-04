package lib.common;

import android.content.Intent;
import android.graphics.Rect;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.OrientationEventListener;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;


import com.allergoscope.app.ParentFromMainAndViewActivity;

import lib.common.button.CMenuButton;
import lib.utils.AGlobals;

/**
 * Created by alex on 11.11.17.
 */

public abstract class AActivity extends ParentFromMainAndViewActivity implements TextView.OnEditorActionListener {
    protected View lockedCapture = null;
    protected boolean fIgnoreMouse = false;
    protected Intent returnIntent = null;
    protected ActionCallback onexit = null;


    public void LockMouse(boolean b)
    {
        // fIgnoreMouse = b;
    }

    public boolean IsLockedMouse()
    {
        return fIgnoreMouse;
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Intent intent = getIntent();


    }

    public void ShowHintMessage(String text)
    {
        Toast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT).show();
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    public boolean dispatchTouchEvent(MotionEvent ev)
    {
        ViewGroup group = (ViewGroup) findViewById(android.R.id.content);
        return dispatchTouchEvent(ev, group);
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public boolean fIsToush = false;

    public boolean dispatchTouchEvent(MotionEvent ev, ViewGroup group)
    {
        if (!DispatchTouchEvent(ev, group))
            return super.dispatchTouchEvent(ev);
        return true;
    }

    public boolean DispatchTouchEvent(MotionEvent ev, ViewGroup group)
    {
        int action = ev.getAction();
        boolean _fIsToush = fIsToush;
        if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_DOWN)
            fIsToush = true;
        if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_UP || (action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_CANCEL)
            fIsToush = false;

        boolean fRet = false;
        if (fIgnoreMouse)
            return true;

        if (lockedCapture != null)
        {
            //   int action = ev.getAction();
            //   if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_DOWN)
            //      lockedCapture = null;
            //else
            {

                try
                {
                    fRet = lockedCapture.onTouchEvent(ev);

                } catch (Exception e)
                {
                    fRet = false;
                }
            }
        } else
        {
            if (group != null)
            {
                IControlOnClick b = CheckControlOnClick(ev, group);
                if (b != null)
                {
                    try
                    {
                        if (fRet = b.OnClick())
                            group.onTouchEvent(ev);
                    } catch (Exception e)
                    {
                        fRet = false;
                    }

                }
            }

        }
        if (!fRet)
            fIsToush = _fIsToush;
        return fRet;
    }

    //----------------------------------------------------------------------
    //
    //----------------------------------------------------------------------
    public IControlOnClick CheckControlOnClick(MotionEvent ev, ViewGroup group)
    {

        int N = group.getChildCount();
        if (group.getVisibility() == View.VISIBLE)
            for (int i = 0; i < N; ++i)
            {

                try
                {

                    View vv = group.getChildAt(i);

                    if (vv instanceof ViewGroup)
                    {
                        IControlOnClick v1;
                        if ((v1 = CheckControlOnClick(ev, (ViewGroup) vv)) != null)
                            return v1;
                    }
                    if (vv.getVisibility() == View.VISIBLE && vv instanceof IControlOnClick)
                    {
                        boolean canCheck = true;
                        IControlOnClick v = (IControlOnClick) vv;
                        if (vv instanceof CMenuButton)
                        {
                            CMenuButton btn = (CMenuButton) vv;
                            canCheck = btn.IsEnable();
                        }

                        if (canCheck)
                        {
                            int action = ev.getAction();
                            if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_DOWN)
                            {
                                final int pointerIndex = ev.getActionIndex();
                                final float x;
                                final float y;
                                y = ev.getRawY();
                                x = ev.getRawX();

                                Rect rect = v.ScreenRect();
                                if (rect.left < rect.right && rect.top < rect.bottom && x >= rect.left && y >= rect.top && x <= rect.right && y <= rect.bottom)
                                    return v;
                            }
                        }

                    }
                } catch (Exception e)
                {
                    break;
                }
            }
        return null;
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public View GetCapture()
    {
        return lockedCapture;
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public boolean SetCapture(View v)
    {
        if (lockedCapture == null)
        {
            lockedCapture = v;
            return true;
        }
        return false;
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    public boolean ReleaseCapture(View v)
    {
        boolean ret = false;
        if ((lockedCapture != null && lockedCapture.equals(v)))
        {
            lockedCapture = null;
            ret = true;
        }
        return ret;
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    public void onBackPressed()
    {
        finish();
    }
    public void Finish() { super.finish(); }
    boolean fInFinish = false;

    @Override
    public void finish()
    {
        if (fInFinish) return;
        fInFinish = true;
        try
        {
            if (onexit != null)
                onexit.run();
        } catch (Exception e)
        {
            ;
        }
        onexit = null;
        super.finish();
        fInFinish = false;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    public boolean onEditorAction(TextView v, int actionId, KeyEvent event)
    {
        AGlobals.ShowKeyboard(false);

        return true;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    public void onPointerCaptureChanged(boolean hasCapture)
    {

    }
}
