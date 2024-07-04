package lib.common;

import android.os.Handler;
import android.os.Looper;

import java.io.Closeable;
import java.io.IOException;
import java.util.concurrent.Semaphore;

public abstract class CStdThread extends Thread implements Closeable {
    protected CHandler client;
    protected Semaphore sec;
    protected Looper looper;

    //-------------------------------------
    //
    //-------------------------------------
    public CStdThread()
    {
        sec = new Semaphore(1, false);
        looper = null;
        client = new CHandler(Looper.getMainLooper());
    }
    public Looper GetLooper()
    {
        return looper;
    }
    //-------------------------------------
    //
    //-------------------------------------
    @Override
    public void close() throws IOException {

        if (looper != null)
        {
            new Handler(looper).post(new Runnable() {
                @Override
                public void run() {
                    if (sec != null)
                        throw new  RuntimeException("Exit");
                }
            });


            try {
                sec.acquire();
                sec.release();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        looper = null;



    }
    //-------------------------------------
    //
    //-------------------------------------
    @Override
    public void run()
    {
        synchronized (this) {
            Looper.prepare();
            looper = Looper.myLooper();
            notifyAll();

        }
        sec.release();
        while (true)
        {
            try {
                looper.loop();
            }
            catch (RuntimeException e1)
            {

                if (e1.getMessage().equals("Exit"))
                    break;
                else
                {

                }


            }
        }
        OnClose();
        sec.release();
        looper = null;

    }
    public void Run()
    {
        try {
            sec.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        this.start();
        try {
            sec.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        CHandler h = new CHandler(looper);
        h.Send(new Runnable() {
            @Override
            public void run() {
                OnRun();

            }
        }, 10);

    }
    protected abstract void OnRun();
    protected abstract void OnClose();
}