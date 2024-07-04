package lib.explorer;

import android.graphics.Bitmap;

import java.io.File;
import java.util.concurrent.Callable;

import lib.explorer.common.ILoadingListener;
import lib.explorer.common.TaskDir;
import lib.utils.AGlobals;
import lib.utils.BitmapUtils;
import lib.utils.FileUtils;

public class TaskReadDir extends TaskDir {
    //--------------------------------------------------------
    //
    //--------------------------------------------------------
    public TaskReadDir()
    {
        super(2);

    }

    @SuppressWarnings("unchecked")
    @Override
    public boolean Submit(ILoadingListener listner)
    {
        if (cs != null)
            cs.submit(new ImageTask(listner));


        return cs != null;
    }

    @Override
    public boolean Read(ILoadingListener listner)
    {
        listner.OnStart();
        try
        {
            Bitmap b = BitmapUtils.Load(listner.obj.img.url, 1);
            String path = AGlobals.cachePath + File.separator + listner.obj.title;
            path += ".png";
            Bitmap nb = BitmapUtils.ResizeBitmap(b, listner.width, listner.height);
            boolean fRet = BitmapUtils.Save(path, nb);
            b.recycle();
            nb.recycle();
            if (fRet)
                listner.OnEnd(path);
            else
                return false;
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
            return false;
        }
        return true;
    }

    //*****************************************************************************
    //
    //*****************************************************************************
    private class ImageTask extends FileLocal implements Callable<Integer> {
        private ILoadingListener listner = null;

        //--------------------------------------------------------
        //
        //--------------------------------------------------------
        public ImageTask(ILoadingListener _listner)
        {
            listner = _listner;
        }

        //--------------------------------------------------------
        //
        //--------------------------------------------------------
        @Override
        public Integer call() throws Exception
        {
            return execute(listner);
        }

    }
}
