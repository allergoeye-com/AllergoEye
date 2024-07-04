package lib.colorpicker;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;


import lib.common.AActivity;
import lib.utils.AGlobals;

public class CtrlWnd extends androidx.appcompat.widget.AppCompatImageView implements View.OnTouchListener {
    protected Bitmap bitmap;
    protected Paint strokeLine1;
    protected Paint strokeLine2;
    protected ColorPicker parent = null;

    protected int touchSlop;

    public CtrlWnd(Context context)
    {
        super(context);
        InitDraw();
    }

    public CtrlWnd(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        InitDraw();
    }

    public CtrlWnd(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        InitDraw();
    }

    protected void InitDraw()
    {
        bitmap = null;
        setOnTouchListener(this);
        touchSlop = ViewConfiguration.get(getContext()).getScaledTouchSlop();
        strokeLine1 = new Paint();
        strokeLine1.setAntiAlias(true);
        strokeLine1.setColor(Color.BLACK);
        strokeLine1.setStrokeWidth(1);
        strokeLine1.setStyle(Paint.Style.STROKE);
        strokeLine2 = new Paint();
        strokeLine2.setAntiAlias(true);
        strokeLine2.setColor(Color.WHITE);
        strokeLine2.setStrokeWidth(1);
        strokeLine2.setStyle(Paint.Style.STROKE);
        setBackgroundColor(0);
    }


    @Override
    public void draw(Canvas canvas)
    {
        super.draw(canvas);
        DrawScene(canvas);


    }

    public void Init(ColorPicker _parent)
    {
        parent = _parent;
    }

    protected void DrawScene(Canvas canvas)
    {
        if (bitmap != null)
        {
            canvas.drawBitmap(bitmap, 0, 0, null);
        }
    }

    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public void Invalidate()
    {
        invalidate();
    }

    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    @Override
    public void onDraw(Canvas canvas)
    {
        DrawScene(canvas);

    }

    public boolean SetCapture()
    {

        return (AGlobals.currentActivity instanceof AActivity) && ((AActivity) AGlobals.currentActivity).SetCapture(this);
    }

    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public boolean ReleaseCapture()
    {
        return (AGlobals.currentActivity instanceof AActivity) && ((AActivity) AGlobals.currentActivity).ReleaseCapture(this);
    }


    @Override
    public boolean onTouch(View v, MotionEvent event)
    {
        return false;
    }
}
