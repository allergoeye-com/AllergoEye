package lib.common;

import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

public class CSemaphore extends Semaphore {

    //------------------------------------
    //
    //------------------------------------
    public CSemaphore(int permits)
    {
        super(permits, false);

    }
    protected CSemaphore(int permits, boolean fair)
    {
        super(permits, fair);

    }

    //------------------------------------
    //
    //------------------------------------
    public boolean Leave()
    {
        release();
        return true;
    }
    //------------------------------------
    //
    //------------------------------------
    private boolean _Enter()
    {

        boolean f = true;
        try {
            try {
                acquire();
            } catch (InterruptedException e) {
                f = false;
            }
        }catch (IllegalArgumentException e)
        {
            f = false;
        }
        return f;
    }
   public boolean Enter()
    {
        try {
            acquire();
        } catch (InterruptedException e) {
            ;
        }
        return true;
    }

    //------------------------------------
    //
    //------------------------------------
    public boolean TryEnter(int mili)
    {

        boolean f = false;
        try {
            f = tryAcquire(mili, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            f = false;
        }
        return f;

    }

    //------------------------------------
    //
    //------------------------------------
    public boolean TryEnter()
    {
        boolean f;
        try {
        f =  tryAcquire();
        }catch (IllegalArgumentException e)
        {
            f = false;
        }
        return f;

    }
    //------------------------------------
    //
    //------------------------------------
    public int SetCount(int n)
    {
        if (availablePermits() > n)
        {
            try {
                tryAcquire(availablePermits() - n);
            }catch(IllegalArgumentException e)
            {
                ;
            }catch(Exception e)
            {
            }

        }
        if(availablePermits() < n)
            try {
                release(n - availablePermits());
            }catch(IllegalArgumentException e)
            {
                ;
            }catch(Exception e)
            {
            }
        return availablePermits();
    }

    //------------------------------------
    //
    //------------------------------------
    public int Count()
    {
        return availablePermits();

    }


}
