package lib.explorer;

import android.graphics.Point;
import android.os.AsyncTask;
import android.widget.ProgressBar;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;

import lib.explorer.common.DirEntry;
import lib.explorer.common.IQuerySelectedPage;
import lib.utils.BitmapUtils;
import lib.utils.CDateTime;
import lib.utils.FileUtils;

public class DirLocal extends AsyncTask<Void, Integer, String> {
    private ArrayList<DirEntry> temp;
    private ArrayList<DirEntry> dirEntries;
    private static Object sinc = new Object();
    private String query;
    private IQuerySelectedPage context = null;
    private ProgressBar progress = null;
    private String [] mask;
    private File dir;
    private ArrayList<String> path;
    boolean fAddDir = true;
    //---------------------------------------------------------------
    //
    //---------------------------------------------------------------

    public DirLocal(IQuerySelectedPage _context, File _dirs, String [] _mask, ProgressBar _progress, ArrayList<DirEntry> result, boolean _fAddDir)
    {
        dirEntries = new ArrayList<DirEntry>();
        fAddDir = _fAddDir;
        dir = _dirs;
        mask = _mask;
        context = _context;
        temp = result;
        progress = _progress;
        path = new ArrayList<String>();
    }
    //---------------------------------------------------------------
    //
    //---------------------------------------------------------------

    public DirLocal(IQuerySelectedPage _context, File _dirs, String [] _mask, ProgressBar _progress, ArrayList<DirEntry> result)
    {
        dirEntries = new ArrayList<DirEntry>();
        dir = _dirs;
        mask = _mask;
        context = _context;
        temp = result;
        progress = _progress;
        path = new ArrayList<String>();
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    @Override
    protected String doInBackground(Void... voids) {
        try
        {
            int len;
            if (fAddDir)
            {
                FileUtils.GetDirList(dir, path);
                len = path.size();
                for (int i = 0; i < len; ++i)
                {
                    DirEntry entry = new DirEntry();
                    String str = path.get(i);
                    int first_i = str.lastIndexOf("/");
                    entry.img.url = str;
                    entry.title = str.substring(first_i +1, str.length());
                    if (progress != null)
                        publishProgress(Math.round(((float)i / ((float)len) * 50)));
                    entry.fIsDir = true;

                    dirEntries.add(entry);
                }
            }
            publishProgress(0);
            path.clear();
            FileUtils.GetFileList(dir, mask, false, path);
            publishProgress(50);
            len = path.size();
            for (int i = 0; i < len; ++i)
            {
                DirEntry entry = new DirEntry();
                String str = path.get(i);
                if (str.contains(".jpg"))
                {
                    String filename = str.replace(".jpg", ".res");
                    ArrayList<String> t = new ArrayList<String> ();
                    FileUtils.ReadText(filename, t);
                    if (t.size() > 0)
                        entry.info = t.get(0);
                }
                int last_i = str.lastIndexOf(".");
                int first_i = str.lastIndexOf("/");
                entry.img.url = str;
                entry.title = CDateTime.ConvertTime(str.substring(first_i +1, last_i));

                Point dm = BitmapUtils.Dimensions(str);
                entry.img.width = dm.x;
                entry.img.height = dm.y;

                entry.thumb.url = str;
                entry.thumb.width = dm.x;
                entry.thumb.height = dm.y;
                if (progress != null)
                    publishProgress(Math.round(50 + ((float)i / ((float)len) * 50)));

                temp.add(entry);
            }
            publishProgress(100);
            publishProgress(0);
            return "OK";

        }
        catch (Exception e) {
            FileUtils.AddToLog(e);
        }

        return "FALSE";
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    protected void onProgressUpdate(Integer... i) {
        if (progress != null) progress.setProgress(i[0]);
    }

    //----------------------------------------------
    //
    //----------------------------------------------
    @Override
    protected void onPostExecute(String result) {
        Collections.sort(dirEntries, DirEntry.cmp);

        Collections.sort(temp, DirEntry.cmp);
        for (int i = 0, len = dirEntries.size(); i < len; ++i)
            temp.add(i, dirEntries.get(i));
        try {
            context.Show();
        } catch (NoSuchMethodException e) {
            FileUtils.AddToLog(e);
        } catch (Exception e)
        {
            e.printStackTrace();
        }

    }
}

