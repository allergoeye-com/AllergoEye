package lib.common;

import lib.utils.AGlobals;
import lib.utils.FileUtils;

import java.util.Timer;
import java.util.TimerTask;


public class CTimer extends ActionCallback {


    final public static int NONE = 0;
    final public static int RUN = 1;

    private CriticalCounter timer_run;
    private Timer timer;
    protected int ms;
    private CriticalCounter count_timer;
    private Object sync;
    CHandler mHandler;
    CTimerTask current;

    //-----------------------------------
    //
    //-----------------------------------
    public CTimer(Object _object, String func)
    {
        super(_object, func);
        current = null;
        count_timer = new CriticalCounter(10);
        timer = new Timer();
        timer_run = new CriticalCounter(2);
        timer_run.SetCount(0);
        sync = new Object();
        mHandler = AGlobals.rootHandler;


    }

    //-----------------------------------
    //
    //-----------------------------------
    public CTimer(Object _object, String func, CHandler h)
    {
        super(_object, func);
        current = null;
        count_timer = new CriticalCounter(10);
        timer = new Timer();
        timer_run = new CriticalCounter(2);
        timer_run.SetCount(0);

        sync = new Object();
        mHandler = h;
    }

    //-----------------------------------
    //
    //-----------------------------------
    public int Status()
    {
        return timer_run.Count();
    }

    //-----------------------------------
    //
    //-----------------------------------
    public boolean Run(int _ms)
    {
        synchronized (sync)
        {
            ms = _ms;
            if (timer_run.Count() == RUN)
                Stop();
            if (timer_run.Count() != RUN)
            {
                timer_run.SetCount(RUN);
                count_timer.Clear();
                _SetTimer();
            }
        }
        return true;
    }

    public boolean Run()
    {
        synchronized (sync)
        {
            if (timer_run.Count() == RUN)
                Stop();
            if (timer_run.Count() != RUN)
            {
                timer_run.SetCount(RUN);
                count_timer.Clear();
                _SetTimer();
            }
        }
        return true;
    }

    //-----------------------------------
    //
    //-----------------------------------
    public boolean Stop()
    {
        synchronized (sync)
        {
            if (!IsValid())
            {
                Clear();
                return false;
            }
            timer_run.SetCount(NONE);

            timer.purge();
            if (current != null)
                current.Reset();
            count_timer.Clear();
        }
        return true;
    }

    @Override
    protected void OnException()
    {
        count_timer.Clear();
    }

    private void _SetTimer()
    {

        timer_run.SetCount(RUN);

        timer.schedule(current = new CTimerTask(), ms);
    }

    @Override
    public void run()
    {
        try
        {
            if (timer_run.Count() == RUN)
                super.run();
            onrun();
        } catch (RuntimeException e)
        {
            FileUtils.AddToLog(e);
        }

    }

    protected void onrun()
    {
        count_timer.Decrement();
        if (timer_run.Count() == RUN)
            _SetTimer();
    }

    private void OnTimer()
    {
        if (timer_run.Count() == RUN)
        {
            if (count_timer.Count() < 1)
            {
                count_timer.Increment();
                onTimer();
            }
        } else
            timer_run.SetCount(NONE);

    }

    protected void onTimer()
    {
        mHandler.Send(this);
    }

    class CTimerTask extends TimerTask {
        CriticalCounter running;

        void Reset()
        {
            running.SetCount(0);

        }

        public CTimerTask()
        {
            super();
            running = new CriticalCounter(1);
            running.SetCount(1);
        }

        @Override
        public void run()
        {
            if (running.Count() > 0)
                OnTimer();

        }
    }
}
