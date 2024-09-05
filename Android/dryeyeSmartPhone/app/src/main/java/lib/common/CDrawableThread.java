package lib.common;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.view.SurfaceHolder;

import lib.utils.AGlobals;
import lib.utils.FileUtils;

import java.io.IOException;

import static com.dryeye.app.App.WM_DESTROY;


/**
 * Created by alex on 05.12.17.
 */

public class CDrawableThread extends CObject {
    protected Thread thread = null;
    protected long threadID = 0;
    protected SurfaceHolder holder = null;
    protected Bitmap bitmap = null;
    Object sync = new Object();
    public Rect drawablRect;

    public CDrawableThread(long _cObject)
    {
        super(_cObject);
        drawablRect = null;
    }

    //---------------------------------------------------------------
    //
    //---------------------------------------------------------------
    protected void Run()
    {
        try
        {
            threadID = AGlobals.AttachThread(ID);
            if (threadID != 0)
            {
                while (AGlobals.OnThreadLoop(threadID))
                {
                    if (!holder.getSurface().isValid())
                        continue;
                    Canvas canvas = null;
                    try
                    {
                        canvas = holder.lockCanvas();
                        Rect rc = drawablRect == null ? holder.getSurfaceFrame() : drawablRect;
                        synchronized (holder)
                        {

                            canvas.drawBitmap(bitmap, rc.left, rc.top, null);
                        }
                    } finally
                    {
                        if (canvas != null)
                            holder.unlockCanvasAndPost(canvas);
                    }

                }
                synchronized (sync)
                {
                    AGlobals.DetachThread(threadID);
                    threadID = 0L;
                }
            }
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }

    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public void start(final SurfaceHolder _holder, Bitmap bmp)
    {
        holder = _holder;
        bitmap = bmp;
        if (thread != null)
        {
            stop();
        }
        thread = new Thread() {

            @Override
            public void run()
            {
                Run();
            }
        };
        thread.start();
        while (threadID == 0L)
        {
            try
            {
                Thread.sleep(10);
            } catch (InterruptedException e)
            {
                FileUtils.AddToLog(e);
            }
        }
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public void stop()
    {
        if (threadID != 0L)
        {
            synchronized (sync)
            {
                if (threadID != 0L)
                    AGlobals.PostCommand2Thread(threadID, WM_DESTROY, 0, 0);
            }

            while (threadID != 0L)
            {
                try
                {
                    Thread.sleep(10);
                } catch (InterruptedException e)
                {
                    FileUtils.AddToLog(e);
                }
            }
        }
        thread = null;
    }

    @Override
    public void close() throws IOException
    {

        stop();
        super.close();
    }


}
