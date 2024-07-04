package lib.common.layout;

import android.content.Context;
import android.content.res.TypedArray;

import androidx.annotation.Nullable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewParent;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.allergoscope.app.R;

import lib.common.IActionCalbackClient;
import lib.common.ActionCallback;
import lib.common.button.CMenuButton;

import java.util.ArrayList;

/**
 * Created by alex on 26.01.2018.
 */

public class LayoutButton extends LinearLayout  implements IActionCalbackClient {
    CMenuButton btn = null;
    int color_down =0xFF10A0FF;
    int color_up =0xFFB9B9B9;
    boolean fCheck = false;
    boolean disable_change_color = false;
    ActionCallback callback = new ActionCallback();
    public LayoutButton(Context context) {
        super(context);
        InitAtribut(context, null, 0, 0);
    }

    public LayoutButton(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);

    }

    public LayoutButton(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, 0, defStyleAttr);
    }


    //-------------------------------------------
    //
    //-------------------------------------------
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.LayoutButton, defA, defS);
            try {
                color_down = a.getColor(R.styleable.LayoutButton_childs_color_down, 0xFF10A0FF);
                color_up = a.getColor(R.styleable.LayoutButton_childs_color_up, 0xFFB9B9B9);
                disable_change_color = a.getBoolean(R.styleable.LayoutButton_disable_change_color, false);
           }

            catch(Exception e)
            {
                ;
            }
            a.recycle();
        }
        else
        {
            color_down = 0xFF10A0FF;
            color_up = 0xFFB9B9B9;
            disable_change_color = false;

        }

        for (int i = 0, l = getChildCount(); i < l; ++i)
        {

            View v = (View)getChildAt(i);
            if (v instanceof CMenuButton)
            {
                btn = (CMenuButton)v;

                btn.setOnClickListener(null);
                break;
            }

        }
/*        Drawable drw = btn.getBackground();
        //
        int color = Color.TRANSPARENT;
        Drawable background = btn.getBackground();
        if (background instanceof ColorDrawable)
            color = ((ColorDrawable) background).getColor();
*/
     /*  for (int i = 0, l = getChildCount(); i < l; ++i)
        {
            View v = (View)getChildAt(i);
            v.setBackgroundColor(color_up);
        }
        */
     //   if (btn != null)
       //     btn.SetCallback(this, "OnChangeColor");
        setOnClickListener(new COnClickButton());



    }
    public CMenuButton Button() {return btn; }
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        //if (btn == null)
        {
            for (int i = 0, l = getChildCount(); i < l; ++i)
            {

                View v = (View)getChildAt(i);
                if (v instanceof CMenuButton)
                {
                    btn = (CMenuButton)v;

                    btn.setClickable(false);
                    break;
                }

            }
        }
        super.onLayout(changed, left, top, right, bottom);
    }
        public void SetText(String str)
    {
        for (int i = 0, l = getChildCount(); i < l; ++i)
        {
            View v = (View)getChildAt(i);
            if (v instanceof TextView)
            {
                ((TextView)v).setText(str);
                break;
            }


        }

    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void OnChangeColor()
    {
      //  if (btn == null) return;
        if (!disable_change_color)
        {
            int color = btn.IsDown() ? color_down : 0;

            for (int i = 0, l = getChildCount(); i < l; ++i)
            {
                View v = (View)getChildAt(i);
                v.setBackgroundColor(color);
            }
        }
        if (callback != null)
            callback.run();
    }
    public void Enable(boolean b)
    {
        if (btn == null)
            Init();
        if (btn != null)
            btn.Enable(b);

    }
    public void SetImage(int id)
    {
        if (btn != null)
            btn.setImageResource(id);

    }

    public boolean IsEnable()
    {
            return btn != null && btn.IsEnable();

    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        ViewParent parent = getParent();
        if (!(parent instanceof CPanelDialog))
        {
            parent = parent.getParent();
            if (!(parent instanceof CPanelDialog))
                parent = null;

        }

        int action = event.getAction();
        switch (action & MotionEvent.ACTION_MASK)
        {
            case MotionEvent.ACTION_MOVE:
                if (parent != null)
                    return ((AnimatedLayout)parent).onTouchEvent(event);
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_UP:
                if (btn != null && btn.isClickable())
                {

                    btn.onTouchEvent(event);

                    if (parent != null)
                        ((AnimatedLayout)parent).onTouchEvent(event);

                }

        }
        return super.onTouchEvent(event);

    }
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void SetCallback(ActionCallback _callback)
    {
        callback = _callback;
    }

    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public void SetCallback(Object _object, String func)
    {
        callback.SetCallback(_object, func);
    }
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    public boolean GetCheck()
    {
        return btn != null && btn.GetCheck();
    }

    public void SetCheck(boolean b)
    {
        fCheck = b;
        if (btn != null)
            btn.SetCheck(b);
        OnChangeColor();

    }


    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    @Override
    public void Detach(ArrayList<Object> stack)
    {
        if (stack == null)
            stack = new ArrayList<Object>();

        if (!stack.contains(this))
        {
            stack.add(this);
            callback.Detach(stack);
            if (btn != null)
                btn.Detach(stack);
        }
    }

    @Override
    public void Detach() {
        Detach(null);
    }

    @Override
    public void Init()
    {
        if (btn == null)
        for (int i = 0, l = getChildCount(); i < l; ++i)
        {

            View v = (View)getChildAt(i);
            if (v instanceof CMenuButton)
            {
                btn = (CMenuButton)v;
             //   btn.SetCheck(fCheck);
               // OnChangeColor();
                break;
            }

        }
        if (btn != null && btn.isClickable())
            btn.SetCallback(this, "OnChangeColor");
    }
    class COnClickButton implements View.OnClickListener {
        @Override
        public void onClick(View v)
        {
            if (btn == null || !btn.isClickable())
            {
                Init();
                OnChangeColor();
            }

        }
    }


}
