package lib.explorer.common;

import java.io.Closeable;
import java.io.IOException;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import lib.utils.FileUtils;

public abstract class TaskDir implements Closeable {
    protected CompletionService cs = null;

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public TaskDir(int sizePoolTread)
    {
        cs = new CompletionService(Executors.newFixedThreadPool(sizePoolTread));

    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public TaskDir()
    {

    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public void Release()
    {
        if (cs != null)
            cs.shutdown();
        cs = null;

    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public void OnResult(Integer i)
    {
    }

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    abstract public boolean Submit(ILoadingListener listner);

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    abstract public boolean Read(ILoadingListener listner);

    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    @Override
    public void close() throws IOException
    {
        Release();
    }

    //*****************************************************************************
    //
    //*****************************************************************************
    protected class CompletionService extends ExecutorCompletionService {

        private ExecutorService mExecutor = null;

        //--------------------------------------------------------
        //
        //--------------------------------------------------------
        public CompletionService(ExecutorService executor)
        {
            super(executor);
            mExecutor = executor;
        }

        //--------------------------------------------------------
        //
        //--------------------------------------------------------
        public void shutdown()
        {
            try
            {
                while (poll(1, TimeUnit.MILLISECONDS) != null) ;
                mExecutor.shutdown();
            } catch (InterruptedException e)
            {
                FileUtils.AddToLog(e);
            }
        }

        //--------------------------------------------------------
        //
        //--------------------------------------------------------
        public boolean isTerminated()
        {
            return mExecutor.isTerminated();
        }

    }
}
