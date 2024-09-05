package lib.common;

import android.os.Handler;
import android.os.Looper;

import lib.utils.AGlobals;
import lib.utils.FileUtils;

import java.io.Closeable;


/**
 * Created by alex on 09.02.2018.
 */

public class EventIdle implements Closeable {

    private String name_loop;
    public final static int IDLE = 1;
    public final static int RUN = 2;
    public final static int EXIT = 3;
    public final static int RUN_AND_EXIT = 4;


    protected CriticalCounter fRun = new CriticalCounter(6);
    protected WaitThread thread;
    protected boolean fInIdle = false;
    protected CSemaphore enter = null;
    protected CSemaphore leave = null;
    protected Looper mloop = null;
    protected EventIdle self;
    protected boolean fErrorExit = false;


    protected int _timeout = 10;
    ActionCallback prevRun = null;

    public class ExitIdleException extends RuntimeException {
        public ExitIdleException()
        {
            super(name_loop);
            looper = self;
        }

        public ExitIdleException(String name)
        {
            super(name);
        }

        public EventIdle looper;
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public Looper GetLooper()
    {
        return mloop;
    }

    public Looper SettLooper(Looper _looper)
    {
        Looper ret = mloop;
        mloop = _looper;
        return ret;

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void Run(Runnable c)
    {
        if (mloop != null)
        {
            CHandler h = new CHandler(mloop);
            h.Send(c);
        }
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void Run(Runnable c, int _timeout)
    {
        if (mloop != null)
        {
            CHandler h = new CHandler(mloop);
            h.Send(c, _timeout);
        }
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public EventIdle(String _name_loop, ActionCallback _prevRun)
    {
        Init(_name_loop, null, _prevRun);

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public EventIdle(String _name_loop)
    {
        Init(_name_loop, null, null);

    }

    public EventIdle(String _name_loop, Looper _loop)
    {
        Init(_name_loop, _loop, null);

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    @Override
    public void close()
    {
        if (fRun.Count() == RUN)
        {
            fRun.SetCount(EXIT);
            enter.Leave();
        }
    }

    public void RunAndExit()
    {
        if (fRun.Count() == RUN)
        {
            fRun.SetCount(RUN_AND_EXIT);
            enter.Leave();
        }
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void Exit()
    {
        close();

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void Init(String _name_loop, Looper _loop, ActionCallback _prevRun)
    {
        self = this;
        prevRun = _prevRun;
        name_loop = "EventIdle_" + _name_loop;
        IntSemaphore();
        CreateThread();
        fRun.SetCount(IDLE);
        mloop = _loop == null ? Looper.myLooper() : _loop;
        if (mloop == null)
            mloop = Looper.getMainLooper();

        enter.Enter();
        thread.start();
        while (fRun.Count() != RUN)
            try
            {
                Thread.sleep(100);
            } catch (InterruptedException e)
            {
                FileUtils.AddToLog(e);
            }

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    protected void IntSemaphore()
    {
        enter = new CriticalSection(true);
        leave = new CriticalSection(true);

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    protected void CreateThread()
    {
        thread = new WaitThread();

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    protected boolean Run()
    {
        while (true)
        {
            try
            {
                if (prevRun != null)
                    prevRun.run();
                mloop.loop();
            } catch (ExitIdleException e1) // перезапуск пустыми ссобщениями
            {
/*                if (CEOSDoc.cuurentView != null)
                {
                    AGlobals.rootHandler.Send(new Runnable() {
                        public void run() {

                        }
                    });
                    if (e1.getMessage() == name_loop)
                        break;
                }*/

                String exep = e1.getMessage();
                if (exep.equals(name_loop))
                    break;
                else
                {
                    AGlobals.rootHandler.Send(new Runnable() {
                        public void run()
                        {

                        }
                    });
                    //((ExitIdleException)e1).looper.Throw();
                }
            } catch (RuntimeException e)
            {
                FileUtils.AddToLog(e);
            }
            return false;
        }
        return true;
    }

    public boolean Idle()
    {
        return Idle(50);
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public boolean Idle(int timeout)
    {

        if (fInIdle || leave.Count() == 0)
            return false;
        _timeout = timeout;
        fInIdle = true;

        leave.Enter();
        enter.Leave();
        boolean fRet = Run();
        fInIdle = false;
        return fRet;
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public boolean Loop()
    {

        if (fInIdle)
            return false;
        fInIdle = true;

        boolean fRet = Run();
        fInIdle = false;
        return fRet;
    }

    public boolean IsInLoop()
    {
        return fInIdle;
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void ExitLoop()
    {
        if (!fInIdle || leave.Count() == 0)
            return;
        leave.Enter();
        enter.Leave();

        try
        {
            Thread.sleep(10);
        } catch (InterruptedException e)
        {
            e.printStackTrace();
        }
    }

    //************************************************
    protected class WaitThread extends Thread {
        @Override
        public void run()
        {
            fRun.SetCount(RUN);
            while (fRun.Count() == RUN)
            {
                enter.Enter();
                enter.Leave();
                if (fRun.Count() == EXIT)
                    break;
                Handler h = new Handler(mloop);
                if (fRun.Count() == RUN_AND_EXIT)
                {
                    h.post(new Runnable() {
                        @Override
                        public void run()
                        {
                            throw new ExitIdleException();
                        }
                    });
                    break;
                }
                if (_timeout <= 2)
                    h.post(new ExitHandle());
                else
                    h.postDelayed(new ExitHandle(), _timeout);
                leave.Enter();
                /*
                int i = 0;
                while (true)
                {
                    for (i = 0; i < 4; ++i)
                        if (leave.TryEnter(500))
                            break;
                    if (i == 4)
                        h.post(new ExitHandle());
                    else
                        break;

                }
                */
                leave.Leave();

            }
            fRun.SetCount(EXIT);
        }
    }

    void Throw()
    {
        throw new ExitIdleException();
    }

    //************************************************
    public class ExitHandle implements Runnable {
        @Override
        public void run()
        {
            if (fRun.Count() == RUN)
                enter.Enter();
            leave.Leave();
            if (fInIdle)
                throw new ExitIdleException();

        }
    }

}
