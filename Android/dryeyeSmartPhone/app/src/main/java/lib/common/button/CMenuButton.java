package lib.common.button;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.TypedArray;
import android.graphics.PorterDuff;
import android.graphics.Rect;

import androidx.core.content.ContextCompat;
import androidx.appcompat.widget.AppCompatImageButton;

import android.graphics.drawable.Drawable;
import android.graphics.drawable.StateListDrawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.dryeye.app.R;

import lib.common.AActivity;
import lib.common.IActionCalbackClient;
import lib.common.ActionCallback;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

import java.util.ArrayList;

public class CMenuButton extends AppCompatImageButton implements IActionCalbackClient, View.OnTouchListener {
    int color_btn_down = 0xFF10A0FF;
    int color_btn = 0xFFB9B9B9;
    int color_btn_disable = 0xFF494949;
    int currentColor;
    boolean colors_png = false;
    boolean disable_color = false;
    protected boolean fSetCalback = false;
    int idResourceDown = 0;
    int idResourceUp = 0;
    boolean fDisable = false;
    protected Call callback = new Call();
    protected Drawable push = null;
    protected Drawable pop = null;
    protected Drawable disable = null;
    public boolean fRotated = false;

    StateListDrawable stateListDrawable = null;
    protected boolean fLongClick = false;

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public CMenuButton(Context context)
    {
        super(context);
        InitAtribut(context, null, 0, 0);
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public CMenuButton(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);


    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public CMenuButton(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public CMenuButton(Context context, AttributeSet attrs, int defStyleAttr, int def)
    {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, defStyleAttr, def);
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public Rect GetScreenRect()
    {
        int[] l = new int[2];

        getLocationOnScreen(l);
        int left = l[0]; // - AGlobals.touchSlop;
        int top = l[1]; // - AGlobals.touchSlop;
        int right = l[0] + getWidth(); //+ AGlobals.touchSlop;
        int bottom = l[1] + getHeight(); //+ AGlobals.touchSlop;
        if (this.getRotation() == 90)
        {
            right = l[0]; //+ AGlobals.touchSlop;
            left = l[0] -  getWidth(); // - AGlobals.touchSlop;

        }

        /*if (clipRect != null)
        {
            left = Math.max(left, clipRect.left);
            top = Math.max(top, clipRect.top);
            right = Math.min(right, clipRect.right);
            bottom = Math.min(bottom, clipRect.bottom);

        }
        if (bottom == l[1] +  getHeight())
            bottom += AGlobals.touchSlop;
        if (top == l[1])
            top -= AGlobals.touchSlop;

        if (right == l[0] +  getWidth())
            right += AGlobals.touchSlop;
        if (left == l[0])
            left -= AGlobals.touchSlop;
*/
        return new Rect(left, top, right, bottom);
    }

    public boolean PtIn(float x, float y)
    {
        Rect rect = GetScreenRect();
        return x >= rect.left && y >= rect.top && x <= rect.right && y <= rect.bottom;
    }

    public void SetImageResource(int up, int down)
    {
        if (idResourceDown == 0)
            setOnTouchListener(this);
        setImageResource(up);
        idResourceDown = down;
        idResourceUp = up;
    }
    @SuppressLint("RestrictedApi")
    public void SetPngColors(boolean n) { colors_png = n; if (n) setSupportImageTintList(null); else SetTwoColor(); ; }
    //-------------------------------------------
    //
    //-------------------------------------------
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {
        if (attrs != null)
        {
            TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.CMenuButton, defA, defS);
            try
            {
                fDisable = a.getBoolean(R.styleable.CMenuButton_disable, false);
                currentColor = color_btn = a.getColor(R.styleable.CMenuButton_color_up, ContextCompat.getColor(context, R.color.btn_color_up));
                color_btn_down = a.getColor(R.styleable.CMenuButton_color_down, ContextCompat.getColor(context, R.color.btn_color_down));
                color_btn_disable = a.getColor(R.styleable.CMenuButton_color_disable, ContextCompat.getColor(context, R.color.btn_color_disable));
                disable_color = a.getBoolean(R.styleable.CMenuButton_disable_colors, false);
                colors_png = a.getBoolean(R.styleable.CMenuButton_colors_png, false);
                fLongClick = a.getBoolean(R.styleable.CMenuButton_long_click, false);
                try {
                push = a.getDrawable(R.styleable.CMenuButton_icon_push);
                    pop = a.getDrawable(R.styleable.CMenuButton_icon_pop);
                    disable = a.getDrawable(R.styleable.CMenuButton_icon_disable);
                }
                catch(Exception e1)
                {
                    push = null;

                    pop = null;

                    disable = null;
                }


                if (push != null || pop!= null || disable!= null)
                {
                    stateListDrawable = new StateListDrawable();
                    if (push != null) {
                        stateListDrawable.addState(new int[]{android.R.attr.state_pressed, android.R.attr.state_enabled}, push);
                    }
                    if (pop != null) {
                        stateListDrawable.addState(new int[]{android.R.attr.state_focused, android.R.attr.state_enabled}, pop);
                    }
                    stateListDrawable.addState(new int[] {android.R.attr.state_enabled}, pop);
                    if (disable != null) {
                        stateListDrawable.addState(new int[]{-android.R.attr.state_enabled}, disable);
                    }

                }



            } catch (Exception e)
            {
                e.printStackTrace();
            }
            a.recycle();
        } else
        {
            fDisable = false;
            currentColor = color_btn = ContextCompat.getColor(context, R.color.btn_color_up);
            color_btn_down = ContextCompat.getColor(context, R.color.btn_color_down);
            color_btn_disable = ContextCompat.getColor(context, R.color.btn_color_disable);

        }
        _UpdateState(true);
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public boolean IsDown()
    {
        return false;
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public boolean GetCheck()
    {
        return false;
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void SetCheck(boolean b)
    {
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public boolean IsEnable()
    {
        return !fDisable;
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public int GetCurrentColor()
    {
        return currentColor;
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public View GetCapture()
    {
        return (
                (AActivity) AGlobals.currentActivity).GetCapture();
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void UpdateState()
    {
        _UpdateState(false);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    protected void _UpdateState(boolean fConstruct)
    {

        if (fDisable)
        {
            SetSingeColor(color_btn_disable, disable);
            this.setEnabled(false);
            this.setClickable(false);
        } else
        {
            this.setEnabled(true);
            this.setClickable(true);
            if (fConstruct)
            {
                SetTwoColor();
                SetListner(callback);
            } else
                UpdateColor();
        }

    }
   public  void SetLongClik(boolean b)
    {
        fLongClick = b;
        setOnLongClickListener(null);
        setOnClickListener(null);
    }

    void SetListner(Call c)
    {
        if (fLongClick)
            setOnLongClickListener(c);
        else
            setOnClickListener(c);

    }
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void SetColorUp(int iColor)
    {
        color_btn = iColor;
        UpdateState();
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void SetColorDown(int iColor)
    {
        color_btn_down = iColor;
        UpdateState();
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void SetColorDisable(int iColor)
    {
        color_btn_disable = iColor;
        UpdateState();

    }


    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    protected void UpdateColor()
    {
        SetTwoColor();
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    @SuppressLint("RestrictedApi")
    protected void SetTwoColor()
    {
/*
        BitmapDrawable draw = (BitmapDrawable)getDrawable();

        if (draw != null)
        {
            Bitmap bitmap = draw.getBitmap();
            if (bitmap != null)
            {
                Rect rect = new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight());
                measure(0, 0);
                int h1 = getMeasuredHeight();
                int w1 = getMeasuredWidth();

                Bitmap bmp = BitmapUtils.Create(w1, h1);
                Canvas canvas = new Canvas(bmp);
                //Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
                //paint.setColor(Color.argb(0, 0, 0, 0));
                canvas.drawARGB(0,0,0,0);
                float f =  ((float)bitmap.getWidth())/((float)bitmap.getHeight());
                int w = (int)(f * h1 + 0.5f);
                int x0 = (w1 - w)/2;
                Rect rect_dst = new Rect(x0, 0, x0 + w, h1);


                canvas.drawBitmap(bitmap, rect, rect_dst, null);

                setImageBitmap(null);
                setBackground(new BitmapDrawable(AGlobals.contextApp.getResources(), bmp));
            }
        }
        */
        if (colors_png) return;


        if (!disable_color)
        {
            if (stateListDrawable != null)
            {
                setImageDrawable(stateListDrawable);

            }
            else
            {
            setSupportImageTintList(new ColorStateList(
                    new int[][]{new int[]{android.R.attr.state_pressed},
                            new int[]{}}, new int[]{color_btn_down, color_btn}));

            setSupportImageTintMode(PorterDuff.Mode.SRC_IN);
            }
        }


        currentColor = color_btn_down;

    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    @SuppressLint("RestrictedApi")
    protected void SetSingeColor(int color, Drawable drawable)
    {
        if (colors_png) return;
        if (!disable_color)
        {
            if (drawable != null)
            {
                setImageDrawable(drawable);
            }
            else
            {
                setSupportImageTintList(new ColorStateList(
                        new int[][]{new int[]{android.R.attr.state_pressed},
                                new int[]{}}, new int[]{color, color}));
                setSupportImageTintMode(PorterDuff.Mode.SRC_IN);
                }
        }
        currentColor = color_btn_down;
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void SetCallback(Object _object, String func)
    {
        callback.SetCallback(_object, func);
        if (callback.IsValid())
        {
            fSetCalback = true;
            fDisable = true;
            Enable(true);
            fSetCalback = false;

        }
    }
    public void Init(Object _object, String func)
    {
        SetCallback(_object, func);
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void Enable(boolean b)
    {
        if (fDisable == !b) return;
        fDisable = !b;
        if (b)
            SetListner(callback);
        else
            SetListner(null);

        UpdateState();
        setEnabled(b);

    }

    @Override
    public void Detach(ArrayList<Object> stack)
    {
        if (stack == null)
            stack = new ArrayList<Object>();
        if (!stack.contains(this))
        {
            stack.add(this);
            callback.Detach(stack);
            SetListner(null);
        }

    }

    @Override
    public void Detach()
    {
        Detach(null);
    }


    @Override
    public void Init()
    {

    }

    @Override
    public boolean onTouch(View v, MotionEvent event)
    {
        switch (event.getAction())
        {
            case MotionEvent.ACTION_DOWN:
            {
                setImageResource(idResourceDown);

                v.invalidate();
                break;
            }
            case MotionEvent.ACTION_UP:
            {

                setImageResource(idResourceUp);
                v.invalidate();
                break;
            }
        }
        return false;

    }

    //********************************************************************************
    protected class Call extends ActionCallback implements OnClickListener, OnLongClickListener {


        @Override
        public void onClick(View v)
        {
            try
            {
                UpdateState();
                if (!fDisable)
                    run();
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
                UpdateState();
            }

        }

        @Override
        public boolean onLongClick(View v) {
            try
            {
                UpdateState();
                if (!fDisable)
                    run();
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
                UpdateState();
            }

            return false;
        }
    }
}
