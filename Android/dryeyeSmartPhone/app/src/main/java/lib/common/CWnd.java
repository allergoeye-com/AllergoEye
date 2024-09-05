package lib.common;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import lib.camera.CameraCalculatePreview;
import lib.utils.AGlobals;
import lib.utils.GlobalRef;

import java.io.Closeable;
import java.io.IOException;


/**
 * Created by alex on 05.12.17.
 */

public class CWnd extends SurfaceView  implements SurfaceHolder.Callback,Closeable {
    protected SurfaceHolder holder = null;
    public long ID = 0L;
    public long cObject = 0L;
    protected boolean fThreadDisable = true;
    public Bitmap bitmap = null;
    public Bitmap bitmap_tools = null;
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public CWnd(Context context, long _cObject, Bitmap surface) {
        super(context);
        bitmap = surface;
        cObject = _cObject;

    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public CWnd(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public CWnd(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }


    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public void Init() {
        setWillNotDraw(!fThreadDisable);
        setWillNotCacheDrawing(true);
        holder = getHolder();
        holder.addCallback(this);

    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public void OnTest() {
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public void Reset() {
        holder.removeCallback(this);
        holder.addCallback(this);

    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public int GetWidth()
    {
        Rect rc = null;
        if (holder != null)
            rc = holder.getSurfaceFrame();
        return rc != null ? rc.width() : 0;
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public int GetHeight()
    {
        Rect rc = null;
        if (holder != null)
            rc = holder.getSurfaceFrame();
        return rc != null ? rc.height() : 0;
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public void Invalidate()
    {
        OnDraw();


    }
    protected void OnDraw()
    {
        if(holder == null || bitmap == null || holder.getSurface() == null || !holder.getSurface().isValid())
            return;
        if (bitmap.isRecycled())
        {
            bitmap = null;
            return;
        }

            Canvas canvas = null;
        try{
            canvas = holder.lockCanvas();
            Rect rc = canvas.getClipBounds();

            synchronized (holder)
            {
        /*    try
                {
            Canvas c = new Canvas(bitmap);
                Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
                paint.setStyle(Paint.Style.FILL);
                paint.setColor(Color.GREEN);
                c.drawRect(rc,paint);
                }
                catch(Exception e)
                {
                    e.printStackTrace();
                }
          */          canvas.drawBitmap(bitmap, 0, 0, null);
                if (bitmap_tools != null)
                    canvas.drawBitmap(bitmap_tools, 0, 0, null);
            }
        }
        finally
        {
            if(canvas != null)
                holder.unlockCanvasAndPost(canvas);
        }
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    @Override
    public void onDraw(Canvas canvas)
    {
        if (bitmap != null)
        {
            if (bitmap.isRecycled())
            {
                bitmap = null;
                return;
            }
            canvas.drawBitmap(bitmap, 0, 0, null);
            if (bitmap_tools != null)
                canvas.drawBitmap(bitmap_tools, 0, 0, null);
        }

    }

    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    @Override
    public void invalidate ()
    {
        Invalidate();
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public boolean SetCapture()
    {

        return (AGlobals.currentActivity instanceof AActivity) && ((AActivity)AGlobals.currentActivity).SetCapture(this);
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public boolean ReleaseCapture()
    {
        return (AGlobals.currentActivity instanceof AActivity) && ((AActivity)AGlobals.currentActivity).ReleaseCapture(this);
    }

    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Invalidate();
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
                               int height) {
        Invalidate();
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    @Override
    public void close() throws IOException {
        if (ID != 0L)
            AGlobals.ReleaseGlobalInstance(ID);
        ID = 0L;
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public static CWnd CreateInstance(Class<?> child, Context cx)
    {
        GlobalRef o = null;
        try {
            o = CreateGlobalInstance(child, cx);
            if (o != null)
                ((CWnd)o.obj).OnCreate(o.ID);
            return (CWnd)o.obj;
        }
        catch(Exception e)
        {

        }
        return null;
    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public static CWnd CreateInstance(Class<?> child, Context cx, int x, int y)
    {
        GlobalRef o = null;
        try {
            o = _CreateGlobalInstance(child, cx, x, y);
            if (o != null)
                ((CWnd)o.obj).OnCreate(o.ID);
        }
        catch(Exception e)
        {
            return null;
        }
        return o == null ? null :  (CWnd)o.obj;
    }

    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    protected void OnCreate(long id)
    {
        ID = id;
    }

    static protected native GlobalRef CreateGlobalInstance(Class<?> cls, Context cx);
    static protected native GlobalRef _CreateGlobalInstance(Class<?> cls, Context cx, int x, int y);

}
