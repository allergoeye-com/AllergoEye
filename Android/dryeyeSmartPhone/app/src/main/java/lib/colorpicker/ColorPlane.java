package lib.colorpicker;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.Region;
import android.util.AttributeSet;


import lib.utils.AGlobals;
import lib.utils.FileUtils;

import android.view.MotionEvent;
import android.view.View;

//import androidx.renderscript.Allocation;

//import gr.wavenet.sns.ScriptC_hls;


public class ColorPlane extends CtrlWnd {
    boolean fLock = false;
    Rect draw = null;
    int border = 10;

    //static ScriptC_hls script = null;
    public static void Init()
    {
        //script = new ScriptC_hls(AGlobals.rs);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    public ColorPlane(Context context) {
        super(context);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    public ColorPlane(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    public ColorPlane(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        if (bitmap != null)
            bitmap.recycle();
        bitmap = null;
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void InitPlane(int w, int h)
    {
        if (parent == null) return;
        if (bitmap != null)
        {
            if (w != bitmap.getWidth() || h != bitmap.getHeight())
            {
                bitmap.recycle();
                bitmap = null;

            }
        }
        if (bitmap == null)
        {
            draw = new Rect(border, border, w, h - border);
            Bitmap.Config b = Bitmap.Config.valueOf("ARGB_8888");
            bitmap = Bitmap.createBitmap(w, h, b);
//            Allocation al = Allocation.createFromBitmap(AGlobals.rs, bitmap);
            w = draw.width();
            h = draw.height();
            ColorPicker.HLSPlane(bitmap, 50.0f, 360.f/w, 100.f/h, draw.left, draw.top, draw.right - border, draw.bottom);
/*
            Allocation al = Allocation.createFromBitmap(AGlobals.rs, bitmap);
            script.set_left(draw.left);
            script.set_top(draw.top);
            script.set_right(draw.right);
            script.set_bottom(draw.bottom);
            script.set_X(50.0f);
            script.set_ScaleX(360.f/w);
            script.set_ScaleY(100.f/h);
            script.forEach_root(al);
            al.copyTo(bitmap);
            al.destroy();
*/
        }


    }
    public void SetRadius(int D)
    {
        if (touchSlop != D)
        {
            touchSlop = D;
            invalidate();
        }
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    protected void DrawScene(Canvas canvas)
    {
        try {
            InitPlane(canvas.getWidth(), canvas.getHeight());

        }
        catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }
        super.DrawScene(canvas);
        if (parent != null && bitmap != null)
        {
            float ScaleX = 360.f/(float)draw.width();
            float ScaleY = 100.f/(float)draw.height();
            int x = draw.left + (int)(parent.H()/ScaleX + 0.5f);
            int y = (int)(draw.bottom - parent.S()/ScaleY + 0.5f);
            canvas.clipRect(draw.left, draw.top, draw.right, draw.bottom, Region.Op.INTERSECT);
            canvas.drawCircle(x, y, touchSlop/2, strokeLine1);
            canvas.drawCircle(x, y, touchSlop/2 -1, strokeLine2);
            canvas.drawCircle(x, y, touchSlop/2 + 1, strokeLine1);
        }

    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (bitmap == null || parent  == null) return false;

        int action = event.getAction() & MotionEvent.ACTION_MASK;
        int x = (int) event.getX();
        int  y = (int) event.getY();
        boolean fEnd = false;
        switch (action)
        {
            case MotionEvent.ACTION_DOWN:
            {
                SetCapture();
                if (x >= 0 && x <  bitmap.getWidth() && y >= 0 && y <  bitmap.getHeight())
                    fLock = true;

            }
            break;
            case MotionEvent.ACTION_UP:
            {
                fEnd = true;
            }
            break;
            case MotionEvent.ACTION_POINTER_DOWN:
            {

            }
            break;
            case MotionEvent.ACTION_CANCEL:
            {
                fEnd = true;

            }
            case MotionEvent.ACTION_POINTER_UP:
            {
                break;
            }
            case MotionEvent.ACTION_MOVE:
            {

                break;
            }
        }
        if (fLock)
        {
            if (y >= draw.bottom) y = draw.bottom - 1;
            if (x >= draw.right) x = draw.right - 1;
            if (y < draw.top) y = draw.top;
            if (x < draw.left) x = draw.left;
            x -=  draw.left;
            y -= draw.top;
            float ScaleX = 360.f/(float)draw.width();
            float ScaleY = 100.f/(float)draw.height();
            float H = x * ScaleX;
            float S = (draw.height() - y) * ScaleY;
            parent.OnHS(H, S, action);
            if (fEnd)
            {
                fLock = false;
                parent.OnEvent(action);
                ReleaseCapture();
                return true;
            }
        }
        return fLock;

    }
}
