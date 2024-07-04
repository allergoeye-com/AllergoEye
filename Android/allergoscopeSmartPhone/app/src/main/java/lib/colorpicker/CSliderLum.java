package lib.colorpicker;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.Region;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

//import androidx.renderscript.Allocation;

//import gr.wavenet.sns.ScriptC_hls_color;

import lib.utils.AGlobals;
import lib.utils.FileUtils;

public class CSliderLum extends CtrlWnd  {
    Rect draw;
    public int border = 10;
  //  static ScriptC_hls_color script;
    public static void Init()
    {
       // script  = new ScriptC_hls_color(AGlobals.rs);
    }
 //   private Allocation al = null;
    Point point = new Point(0, 0);
    boolean fLock = false;
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public CSliderLum(Context context) {
        super(context);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    public CSliderLum(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    public CSliderLum(Context context, AttributeSet attrs, int defStyleAttr) {
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
    //    if (al != null)
      //      al.destroy();
        bitmap = null;
       // al = null;
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    protected void DrawScene(Canvas canvas)
    {

        try {

            if (parent != null)
            {
                InitPlane(canvas.getWidth(), canvas.getHeight());
                int D = draw.right/2;
                ColorPicker.HLSLum(bitmap, parent.H(), parent.S(), D, draw.top, draw.right, draw.bottom);
               /* if (al != null)
                {
                   script.set_H(parent.H());
                    script.set_S(parent.S());
                    script.set_left(draw.left);
                    script.set_top(draw.top);
                    script.set_right(draw.right);
                    script.set_bottom(draw.bottom);
                    script.forEach_root(al);
                    al.copyTo(bitmap);
                }
                */



            }
        }
        catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }
        super.DrawScene(canvas);
        if (parent != null && bitmap != null)
        {
            int D = draw.right/2;
            parent.SetRadius(D);
            int x = D + (draw.right  - D)/2;
            float stp = (float)(draw.height())/100.0f;
            int y = (int)(draw.height() - parent.L() * stp);
            y += draw.top;
            canvas.clipRect(draw.left, draw.top, draw.right, draw.bottom, Region.Op.INTERSECT);
            canvas.drawCircle(x, y, D/2, strokeLine1);
            canvas.drawCircle(x, y, D/2 -1, strokeLine2);
            canvas.drawCircle(x, y, D/2 + 1, strokeLine1);
        }
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
             //   al.destroy();
                bitmap = null;
             //   al = null;

            }
        }
        if (bitmap == null)
        {
            draw = new Rect(0, border, w - border, h - border);
            Bitmap.Config b = Bitmap.Config.valueOf("ARGB_8888");
            bitmap = Bitmap.createBitmap(w, h, b);
        //    al = Allocation.createFromBitmap(AGlobals.rs, bitmap);
        }

    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (bitmap == null || parent  == null) return false;
        int action = event.getAction() & MotionEvent.ACTION_MASK;

        int x = (int)event.getX(0);
        int y = (int) event.getY(0);
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
            float stp = (float)(draw.height())/100.0f;

            if (y >= draw.bottom) y = draw.bottom - 1;
            if (y < draw.top) y = draw.top;
            y -= draw.top;

            parent.OnL((float)(draw.height() - y)/stp, action & MotionEvent.ACTION_MASK);
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
