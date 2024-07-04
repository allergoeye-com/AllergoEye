package lib.common;

import java.util.concurrent.Semaphore;

/**
 * Created by alex on 04.03.2018.
 */

public class CriticalCounter extends Semaphore {

    public CriticalCounter(int permits)
    {
        super(permits, false);
        SetCount(0);
    }

    public void Clear()
    {
        try
        {
            if (availablePermits() > 0)
                tryAcquire(availablePermits());
        } catch (IllegalArgumentException e)
        {
            ;
        } catch (Exception e)
        {
        }
    }

    public int Increment()
    {

        release();
        return availablePermits();

    }

    public int SetCount(int n)
    {
        if (availablePermits() > n)
        {
            try
            {
                tryAcquire(availablePermits() - n);
            } catch (IllegalArgumentException e)
            {
                ;
            } catch (Exception e)
            {
            }

        }
        if (availablePermits() < n)
            try
            {
                release(n - availablePermits());
            } catch (IllegalArgumentException e)
            {
                ;
            } catch (Exception e)
            {
            }
        return availablePermits();
    }

    public int Count()
    {
        return availablePermits();

    }

    public int Decrement()
    {
        int i = 0;
        while (availablePermits() > 0 && !tryAcquire() && i < 1000)
        {
            try
            {
                Thread.sleep(1);
            } catch (InterruptedException e)
            {
                ;
            }
            ++i;
        }
        return availablePermits();

    }


}
