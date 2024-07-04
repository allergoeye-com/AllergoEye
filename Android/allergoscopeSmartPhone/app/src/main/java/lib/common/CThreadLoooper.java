package lib.common;

import android.os.Looper;
import android.os.Process;

import lib.utils.FileUtils;

import java.io.Closeable;
import java.io.IOException;
import java.util.concurrent.Semaphore;

public class CThreadLoooper implements Runnable, Closeable {

    protected Thread self;
    protected EventIdle mLooper;
    protected Semaphore sec;

    //-----------------------------------------
    //
    //-----------------------------------------
    public boolean IsRun()
    {
        return self != null && sec.availablePermits() == 0;
    }

    public CThreadLoooper()
    {
        self = null;
        mLooper = null;
        sec = new Semaphore(1, false);

    }

    //-----------------------------------------
    //
    //-----------------------------------------
    @Override
    public void run()
    {
        Looper loop;
        Looper.prepare();
        synchronized (this)
        {
            loop = Looper.myLooper();
            notifyAll();
            mLooper = new EventIdle(this.toString(), loop);

        }
        sec.release();
        Process.setThreadPriority(Process.THREAD_PRIORITY_DEFAULT);
        mLooper.Loop();
        mLooper.close();
        mLooper = null;
        self = null;
        sec.release();

    }
    //-----------------------------------------
    //
    //-----------------------------------------
    private void Start() {
        self = new Thread(this);
        self.start();
    }
    //-----------------------------------
    //
    //-----------------------------------
    public boolean IsValid()
    {

        return self != null;
    }
    //-----------------------------------------
    //
    //-----------------------------------------
    public boolean Run()
    {
        if (IsRun()) return false;

        try {

            sec.acquire();
            Start();
            sec.acquire();


        } catch (InterruptedException e) {
            FileUtils.AddToLog(e);
            return false;
        }
        return true;
    }
    //-----------------------------------
    //
    //-----------------------------------
    @Override
    public void close() throws IOException {
        if (IsRun())
            if (mLooper != null)
            mLooper.Exit();
    }

}
