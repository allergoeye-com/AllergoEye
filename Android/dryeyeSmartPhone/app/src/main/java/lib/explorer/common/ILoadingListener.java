package lib.explorer.common;


import android.graphics.Bitmap;
import android.graphics.Typeface;

public abstract class ILoadingListener {
    public DirEntry obj = null;
    public int width = 0;
    public int height = 0;

    public ILoadingListener(DirEntry o, int w, int h)
    {
        obj = o;
        width = w;
        height = h;
    }
    public String GetPath() { return obj.img.url; }
    public abstract boolean OnStart();
    public abstract void  OnEnd(byte [] _loadedImage, int scale);
    public abstract void  OnEnd(Bitmap _loadedImage);
    public abstract void  OnEnd(String title, Bitmap _loadedImage);
    public abstract void  OnEnd(String path);
    public abstract void OnCancel();
    public abstract void OnProgress(int _current, int _total); //вызывается для обнавленя прогресбара
    public abstract boolean IsVisiblePosition(int _position);
    public void OnEnd(String path, Typeface typeface) {}
}
