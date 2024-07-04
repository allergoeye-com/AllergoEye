package lib.common;

import java.util.concurrent.TimeUnit;

/**
 * Created by alex on 09.02.2018.
 */

public class CriticalSection extends CSemaphore {
    long threadID = 0L;
    boolean fIgnoreThisThread;

    public CriticalSection()
    {
        this(false);
    }

    public CriticalSection(boolean fIgnoreThisThread1)
    {
        super(1, true);
        fIgnoreThisThread = fIgnoreThisThread1;
    }

    @Override
    public boolean Enter()
    {
        if (fIgnoreThisThread && threadID != 0L && Thread.currentThread().getId() == threadID)
            return true;
        boolean fOk = false;
        while (true)
        {
            try
            {
                acquire();
                threadID = Thread.currentThread().getId();
                break;
            } catch (InterruptedException e)
            {
                ;

            }
        }
        return true;
    }

    public long GetThreadID()
    {
        return threadID;
    }

    @Override
    public boolean TryEnter()
    {
        if (fIgnoreThisThread && threadID != 0L && Thread.currentThread().getId() == threadID)
            return true;

        boolean f = tryAcquire();
        if (f)
            threadID = Thread.currentThread().getId();
        return f;

    }

    @Override
    public boolean TryEnter(int mili)
    {

        if (fIgnoreThisThread && threadID != 0L && Thread.currentThread().getId() == threadID)
            return true;
        boolean f = false;
        try
        {
            f = tryAcquire(mili, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e)
        {
            f = false;
        }
        if (f)
            threadID = Thread.currentThread().getId();
        return f;

    }

    @Override
    public boolean Leave()
    {
        if (fIgnoreThisThread && Thread.currentThread().getId() != threadID) return false;
        threadID = 0L;
        release();
        return true;
    }

}
