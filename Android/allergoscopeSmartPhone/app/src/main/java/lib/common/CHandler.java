package lib.common;

import android.os.Handler;
import android.os.Looper;

public class CHandler extends Handler {
    Object sync = new Object();

    public CHandler()
    {
        super(Looper.myLooper());
    }

    public CHandler(Looper loop)
    {
        super(loop);
    }

    public void Send(Runnable r)
    {
        synchronized (sync)
        {
            post(r);
        }
    }

    public void Send(Runnable r, int timeout)
    {
        synchronized (sync)
        {
            postDelayed(r, timeout);
        }
    }


}
