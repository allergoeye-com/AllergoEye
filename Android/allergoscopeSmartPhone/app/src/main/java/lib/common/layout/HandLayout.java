package lib.common.layout;

import android.app.Activity;
import android.content.Context;
import android.content.res.TypedArray;
import android.os.Build;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.allergoscope.app.R;

import lib.common.AActivity;
import lib.common.button.CMenuButton;
import lib.utils.AGlobals;

public class HandLayout  extends AnimatedLayout {
    protected boolean fFirstTime = false;
    protected CMenuButton hand = null;
    protected HandLayout self;
    Integer id = null;
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public HandLayout(@NonNull Context context) {
        super(context);
        InitAtribut2(context, null, 0, 0);
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public HandLayout(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        InitAtribut2(context, attrs, 0, 0);

    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public HandLayout(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        InitAtribut2(context, attrs, defStyleAttr, 0);
    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public HandLayout(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        InitAtribut2(context, attrs, defStyleAttr, defStyleRes);
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        super.onLayout(changed, left, top, right, bottom);
        if (hand == null && id != null && id != 0)
        {
            hand = ((Activity)this.getContext()).findViewById(id);
            if (hand != null)
            {
                hand.SetCallback(this, "On_Hand");
            }
        }
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    protected void InitAtribut2(Context context, AttributeSet attrs, int defA, int defS)
    {
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.HandLayout, defA, defS);
            try {

                id =  a.getResourceId(R.styleable.HandLayout_idToHandButton, 0);
            }
            catch(Exception e)
            {
                ;
            }
            a.recycle();
        }
        self = this;
    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public void Init()
    {


    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public void On_Hand()
    {
        if (((AActivity)AGlobals.currentActivity).fIsToush)
        {
            ((AActivity)AGlobals.currentActivity).SetCapture(this);
            fFirstTime = true;
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run() {
                    if (fFirstTime)
                    {
                        ((AActivity)AGlobals.currentActivity).ReleaseCapture(self);
                        fFirstTime = false;
                    }
                }
            }, 1000);
        }
    }
    protected void OnToushUp()
    {

    }
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (AGlobals.currentActivity == null || !(AGlobals.currentActivity instanceof AActivity)) return false;

        View v = ((AActivity)AGlobals.currentActivity).GetCapture();
        if (v != null && v.equals(this))
        {
            int action = event.getAction();
            if (fFirstTime)
            {
                StartDrag(event);
                fFirstTime = false;
            }
            else
            if ((action & MotionEvent.ACTION_MASK) ==MotionEvent.ACTION_DOWN)
            {
                ((AActivity) AGlobals.currentActivity).ReleaseCapture(this);
                return false;
            }
            super.onTouchEvent(event);
            if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_UP || (action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_CANCEL)
            {
                ((AActivity) AGlobals.currentActivity).ReleaseCapture(this);
                hand.onTouchEvent(event);
                if (panel != null)
                    panel.requestLayout();
                OnToushUp();

            }

            return true;
        }
        return isVisible && super.onTouchEvent(event);

    }
}

