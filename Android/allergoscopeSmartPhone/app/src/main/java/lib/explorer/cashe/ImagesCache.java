package lib.explorer.cashe;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.Point;

import java.io.Closeable;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import lib.utils.AGlobals;
import lib.utils.BitmapUtils;
import lib.utils.FileUtils;

public class ImagesCache implements Closeable {
    public String path = null;
    private HashMap<Integer, String> location = null;
    private MemCache mem = null;
    Object sync = new Object();

    /*************************************************************************************
     * MemCache
     *************************************************************************************/
    class MemCache {

        Object sync = new Object();
        ThreadRecycle thread = null;
        int count = 0;

        //***********Entry******************************************************************
        class Entry {
            public int key;
            public Bitmap value;

            Entry(int _key, Bitmap val)
            {
                key = _key;
                value = val;
            }

        }

        //**********Entry end*******************************************************************
        ArrayList<Entry> list = new ArrayList<Entry>();
        int size_dop = 10;


        /*************************************************************************************
         * ThreadRecycle
         *************************************************************************************/
        class ThreadRecycle implements Closeable {
            protected CompletionService cs = null;

            //  protected ThreadRun thread = null;
            //--------------------------------------------------------
            //
            //--------------------------------------------------------
            public ThreadRecycle(int sizePoolTread)
            {
                cs = new CompletionService(Executors.newFixedThreadPool(2));//Executors.newCachedThreadPool());
                //    thread = new ThreadRun(cs);
                //  thread.start();

            }

            void ClearResult()
            {
                try
                {
                    while (cs.poll(1, TimeUnit.MILLISECONDS) != null) ;
                } catch (InterruptedException e)
                {
                    FileUtils.AddToLog(e);
                }
            }

            //--------------------------------------------------------
            //
            //--------------------------------------------------------
            public void Release()
            {
                if (cs != null)
                {
                    try
                    {
                        while (cs.poll(1, TimeUnit.MILLISECONDS) != null) ;
                    } catch (InterruptedException e)
                    {
                        FileUtils.AddToLog(e);
                    }

                    cs.shutdown();
                }
                cs = null;

            }

            //--------------------------------------------------------
            //
            //--------------------------------------------------------
            @SuppressWarnings("unchecked")
            public void Delete(Bitmap bmp)
            {
                try
                {
                    cs.poll(1, TimeUnit.MILLISECONDS);
                } catch (InterruptedException e)
                {
                    FileUtils.AddToLog(e);
                }

                if (bmp != null)
                {
                    if (!bmp.isRecycled())
                    {
                        cs.submit(new ImageTask(bmp));
                        synchronized (sync)
                        {
                            ++count;
                        }
                    }
                }

            }

            //--------------------------------------------------------
            //
            //--------------------------------------------------------
            public int PoolSize()
            {

                synchronized (sync)
                {
                    return count;
                }

            }

            //--------------------------------------------------------
            //
            //--------------------------------------------------------
            @SuppressWarnings("unchecked")
            public void Save(String ph, Bitmap bmp)
            {
                if (bmp != null)
                {
                    cs.submit(new ImageTask(ph, bmp));
                    synchronized (sync)
                    {
                        ++count;
                    }
                }

            }

            //--------------------------------------------------------
            //
            //--------------------------------------------------------
            @SuppressWarnings("unchecked")
            public void Save(String ph, byte[] bytes, int scale)
            {
                if (bytes != null)
                {
                    cs.submit(new ImageTask(ph, bytes, scale));
                    synchronized (sync)
                    {
                        ++count;
                    }
                }

            }

            //--------------------------------------------------------
            //
            //--------------------------------------------------------
            @Override
            public void close() throws IOException
            {
                Release();
            }

            //*****************************************************************************
            //          ImageTask
            //*****************************************************************************
            private class ImageTask implements Callable<Boolean> {
                Bitmap bmp = null;
                String filename = null;
                byte[] bytes = null;
                int scale = 1;

                //--------------------------------------------------------
                //
                //--------------------------------------------------------
                public ImageTask(Bitmap _bmp)
                {
                    bmp = _bmp;
                }

                //--------------------------------------------
                //
                //--------------------------------------------
                public ImageTask(String ph, Bitmap _bmp)
                {
                    bmp = _bmp;
                    filename = ph;
                }

                //--------------------------------------------
                //
                //--------------------------------------------
                public ImageTask(String ph, byte[] _bytes, int _scale)
                {
                    bytes = _bytes;
                    filename = ph;
                    scale = _scale;
                }

                //--------------------------------------------------------
                //
                //--------------------------------------------------------
                @Override
                public Boolean call()
                {
                    synchronized (sync)
                    {
                        --count;
                    }
                    Boolean fOk = false;
                    try
                    {
                        if (filename != null)
                        {
                            Thread.sleep(1);
                            if (bytes != null)
                                bmp = BitmapUtils.Load(bytes, scale);
                            if (bmp != null)
                                BitmapUtils.Save(filename, bmp);
                        }
                        fOk = true;
                    } catch (InterruptedException e)
                    {
                        FileUtils.AddToLog(e);
                    } catch (Exception e)
                    {
                        FileUtils.AddToLog(e);
                    }
                    if (bmp != null)
                        bmp.recycle();

                    return fOk;
                }

            }
            //********************ImageTask end*********************************************************

            //*****************************************************************************
            // CompletionService
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
                    mExecutor.shutdown();
                }

                //--------------------------------------------------------
                //
                //--------------------------------------------------------
                public boolean isTerminated()
                {
                    return mExecutor.isTerminated();
                }
            }
            //*******************CompletionService end**********************************************************
        }
        //*************** ThreadRecycle end***********************************************************************

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        MemCache()
        {

            thread = new ThreadRecycle(Runtime.getRuntime().availableProcessors());
        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        void close()
        {
            try
            {
                thread.close();
            } catch (IOException e)
            {
                FileUtils.AddToLog(e);
            }
            while (!list.isEmpty())
            {
                if (list.get(0).value != null && !list.get(0).value.isRecycled())
                    list.get(0).value.recycle();
                list.get(0).value = null;
                list.remove(0);
            }

        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        private int dopFirst(int first, int last)
        {
            int range = last - first;
            if (range > size_dop)
                size_dop = range * 2;
            if (size_dop > 50)
                size_dop = range;
            int n = range + size_dop;
            return first - n < 0 ? 0 : first - n;

        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        private int dopLast(int first, int last)
        {
            int range = last - first;
            if (range > size_dop)
                size_dop = range;
            return last + range + size_dop;

        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        public void clear()
        {
            while (!list.isEmpty())
            {
                thread.Delete(list.get(0).value);
                list.get(0).value = null;
                list.remove(0);
            }
        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        private Point Update(int first, int last)
        {
            while (!list.isEmpty() && list.get(0).key < first)
            {
                thread.Delete(list.get(0).value);
                list.remove(0);
            }
            int count = list.size();
            for (int i = count - 1; i >= 0; --i)
                if (list.get(i).key > last)
                {
                    thread.Delete(list.get(i).value);
                    list.remove(i);
                } else
                    break;
            if (list.isEmpty())
                return null;
            return new Point(list.get(0).key, list.get(list.size() - 1).key);
        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        public void Save(String p, Bitmap b)
        {
            thread.Save(p, b);
        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        public void Save(String p, byte[] b, int scale)
        {
            thread.Save(p, b, scale);
        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        public int PoolSize()
        {
            return thread.PoolSize();
        }

        //--------------------------------------------------------------
        //
        //-------------------------------------------------------------
        public Entry Get(Integer pos, int _first, int _last)
        {

            if (list.isEmpty())
            {
                Entry e = new Entry(pos, null);
                list.add(e);
                return e;
            }
            int first = dopFirst(_first, _last);
            int last = dopLast(_first, _last);
            if (pos < first)
                first = pos;
            if (pos > last)
                last = pos;

            Point range = new Point(list.get(0).key, list.get(list.size() - 1).key);
            if (pos >= range.x && pos <= range.y)
            {
                Entry e = list.get(pos - range.x);
                if (e.value != null && e.value.isRecycled())
                    e.value = null;
                if ((first - last) * 3 < range.y - range.x)
                    Update(first, last);
                return e;
            }

            range = Update(first, last);
            if (range == null)
            {
                Entry e = new Entry(pos, null);
                list.add(e);
                return e;
            }
            if (pos > range.y)
            {
                for (int i = range.y + 1; i < pos; ++i)
                    list.add(new Entry(i, null));
                Entry e = new Entry(pos, null);
                list.add(e);
                return e;
            }
            if (pos < range.x)
            {
                for (int i = range.x - 1; i > pos; --i)
                    list.add(0, new Entry(i, null));
                Entry e = new Entry(pos, null);
                list.add(0, e);
                return e;
            }
            Entry e = list.get(pos - range.x);
            if (e.value != null && e.value.isRecycled())
                e.value = null;
            return e;
        }

    }

    //*********************MemCache end*********************************
    //----------------------------------------------
    //
    //----------------------------------------------
    @SuppressLint("UseSparseArrays")
    public boolean Init(String name)
    {
        if (AGlobals.cachePath != null)
        {
            path = AGlobals.cachePath + File.separator + name;
            mem = new MemCache();
            location = new HashMap<Integer, String>();

            File dir = new File(path);
            try
            {
                if (!dir.exists())
//                    FileUtils.DeleteDir(dir);
                    //              else
                    dir.mkdirs();
                return true;
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }

        }
        return false;

    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public boolean Restart()
    {
        if (path != null)
        {
            if (mem != null)
            {
                try {

                    mem.close();
                    location.clear();
                    FileUtils.DeleteDir(path);
                } catch (Exception e) {
                    FileUtils.AddToLog(e);
                }

            }
            mem = new MemCache();
            location = new HashMap<Integer, String>();

            File dir = new File(path);
            try
            {
                if (dir.exists())
                    FileUtils.DeleteDir(dir);
                else
                    dir.mkdirs();
                return true;
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }

        }
        return false;

    }

    //----------------------------------------------
    //
    //----------------------------------------------
    @Override
    public void close() throws IOException
    {
        if (path != null)
        {
            try
            {

                mem.close();
                location.clear();
                if (!path.contains("local"))
                    FileUtils.DeleteDir(path);
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }
            path = null;
            mem = null;
        }


    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public void ClearMemory()
    {
        try
        {
            if (mem != null)
            {
                location.clear();
                mem.clear();
                mem = new MemCache();
                location = new HashMap<Integer, String>();

                File dir = new File(path);
                if (!dir.exists())
                    dir.mkdirs();
            }
        } catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public Bitmap Get(Integer key, String hashKey, Integer range_x, Integer range_y) throws Exception
    {
        if (path != null)
        {
            synchronized (sync)
            {
                MemCache.Entry e = mem.Get(key, range_x, range_y);

                if (e.value == null && hashKey != null)
                {
                    if (hashKey != null)
                    {
                        String filename = path + File.separator + hashKey + ".png";
                        File f = new File(filename);
                        if (f.exists())
                        {
                            e.value = BitmapUtils.Load(filename, 1);
                            location.put(key, hashKey);
                        }
                    }
                }
                return e.value;

            }
        }
        return null;
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public String Add(Integer key, Integer range_x, Integer range_y, Bitmap bitmap) throws IOException
    {

        if (path != null)
        {
            synchronized (sync)
            {
                String hashKey = null;

                if (path != null && bitmap != null)
                {
                    hashKey = AGlobals.CreateHashKey(key.toString());
                    location.put(key, hashKey);
                    String filename = path + File.separator + hashKey + ".png";
                    if (key >= range_x && key <= range_y)
                    {
                        if (BitmapUtils.Save(filename, bitmap))
                        {
                            MemCache.Entry e = mem.Get(key, range_x, range_y);
                            e.value = bitmap;
                        }
                    } else
                    {
                        mem.Save(filename, bitmap);
                        //bitmap.recycle();
                        //bitmap = null;
                    }


                }
                return hashKey;
            }
        }
        return null;
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public String Add(String name_key, Integer key, Integer range_x, Integer range_y, Bitmap bitmap) throws IOException
    {

        if (path != null)
        {
            synchronized (sync)
            {
                String hashKey = null;

                if (path != null && bitmap != null)
                {
                    hashKey = name_key;
                    location.put(key, hashKey);
                    String filename = path + File.separator + hashKey + ".png";
                    if (key >= range_x && key <= range_y)
                    {
                        if (BitmapUtils.Save(filename, bitmap))
                        {
                            MemCache.Entry e = mem.Get(key, range_x, range_y);
                            e.value = bitmap;
                        }
                    } else
                    {
                        mem.Save(filename, bitmap);
                        //bitmap.recycle();
                        //bitmap = null;
                    }


                }
                return hashKey;
            }
        }
        return null;
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public String Add(String name_key, Integer key, Integer range_x, Integer range_y, byte[] bytes, int scale) throws Exception
    {

        if (key >= range_x && key <= range_y)
            return Add(name_key, key, range_x, range_y, BitmapUtils.Load(bytes, scale));

        synchronized (sync)
        {
            String hashKey = null;

            if (path != null && bytes != null)
            {
                hashKey = name_key; //AGlobals.CreateHashKey(key.toString());
                location.put(key, hashKey);
                String filename = path + File.separator + hashKey + ".png";
                mem.Save(filename, bytes, scale);
                return hashKey;
            }
        }
        return null;
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public int MemPoolSize()
    {
        return mem.PoolSize();
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    public long SizeCache()
    {
        long size = 0;
        if (path != null)
        {
            try
            {

                size = FileUtils.GetDirSize(path);
            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }
        }
        return size;

    }


}
