package lib.explorer;

import android.graphics.Bitmap;

import java.io.IOException;

import lib.explorer.common.DirEntry;
import lib.explorer.common.ILoadingListener;
import lib.utils.BitmapUtils;
import lib.utils.FileUtils;

public class FileLocal {

    public Integer execute(ILoadingListener listner) throws IOException
    {
        DirEntry.Entry entry1 = listner.obj.GetTrue();
        int N = -1;
        //   synchronized(entry1)
        try
        {
            N = listner.obj.self;
            if (listner.IsVisiblePosition(listner.obj.self)) // || listner.OnStart()) //true)
            {
                //listner.OnStart();
                DirEntry.Entry entry = entry1;
                DirEntry.Entry entry2 = listner.obj.GetFalse();
                boolean fIs2 = false;
                if (entry.url == null)
                {
                    fIs2 = true;
                    if (entry2.url == null) return null;
                    entry = entry2;
                }
                float f = ((float) (entry.height)) / listner.height;
                float f1 = ((float) (entry.width)) / listner.width;
                f = Math.max(f + .7f, f1 + .7f);

                int inSampleSize = (int) (f < 1.0 ? 1.0 : f);
                Bitmap bmp = null;
                byte[] bytes = FileUtils.Load(entry.url);
                int w, h;
                if (bytes == null)
                {
                    bmp = BitmapUtils.Load(entry.url, inSampleSize);
                    h = bmp.getHeight();
                    w = bmp.getWidth();

                } else
                {
                    h = (int) (entry.height / f);
                    w = (int) (entry.height / f);
                }
                if (fIs2)
                {
                    entry1.height = h;
                    entry1.width = w;
                    entry1.url = entry.url;
                } else
                {
                    entry.height = h;
                    entry.width = w;
                }
                if (bytes != null)
                    listner.OnEnd(bytes, inSampleSize);
                else if (bmp != null)
                    listner.OnEnd(bmp);
                if (listner.obj.thumb_img == null)
                    listner.OnCancel();
            } else
            {
                listner.OnCancel();
            }

        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
            listner.OnCancel();
        }

        return N;
    }
}
