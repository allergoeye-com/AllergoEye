package lib.explorer;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;


import com.dryeye.app.R;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;

import lib.common.ActionCallback;
import lib.common.CheckRecycleImageView;
import lib.common.OnClickCalback;
import lib.explorer.common.DirEntry;
import lib.explorer.common.ILoadingListener;
import lib.explorer.common.TaskDir;
import lib.utils.AGlobals;
import lib.utils.BitmapUtils;
import lib.utils.FileUtils;

public class AdapterExplorerView extends RecyclerView.Adapter<AdapterExplorerView.RViewHolder> {
    public ActionCallback onselect = null;
    TaskDir task = null;
    AGlobals.CasheFromExplore cache;
    ArrayList<DirEntry> result;

    int lastVisibleFirst = 0;
    int lastVisibleLast = 0;
    AdapterExplorerView self;
    GridLayoutManager grid;
    boolean firstTime = true;


    int[] iIndex = null;
    int[] invIndex = null;
    int numIndex = 0;
    int selectedPos = -1;
    Object syncLastVisible = new Object();
    Object sync = new Object();
    private int w = 0;
    private int h = 0;
    private final HashSet<Integer> colect = new HashSet<Integer>();
    int grid_view_items = R.layout.grid_view_items;

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    public AdapterExplorerView(GridLayoutManager _layoutManager, @NonNull AGlobals.CasheFromExplore _cache, TaskDir _task, ActionCallback onSelect)
    {
        self = this;
        onselect = onSelect;
        task = _task;
        firstTime = true;
        cache = _cache;
        grid = _layoutManager;
        Refresh();
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    public AdapterExplorerView(GridLayoutManager _layoutManager, @NonNull AGlobals.CasheFromExplore _cache, TaskDir _task, ActionCallback onSelect, int _grid_view_items)
    {
        self = this;
        onselect = onSelect;
        task = _task;
        firstTime = true;
        cache = _cache;
        grid = _layoutManager;
        grid_view_items = _grid_view_items;
        Refresh();
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    public void Refresh()
    {
        result = new ArrayList<DirEntry>();
        if (cache.result != null && cache.result.size() > 0)
            result.addAll(cache.result);
        numIndex = result.size();
        iIndex = null;
        invIndex = null;
        selectedPos = -1;
        CheckIndex();


    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    @NonNull
    @Override
    public RViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType)
    {
        View v = LayoutInflater.from(parent.getContext()).inflate(grid_view_items, parent, false);
        RViewHolder vh = new RViewHolder(v);
        v.invalidate();
        return vh;
    }

    public void RefreshBoundRect(Rect rect)
    {
        if (w == 0 || h == 0)
        {

            w = rect.width();
            h = rect.height();
            (new Handler()).post(new Runnable() {
                @Override
                public void run()
                {
                    notifyDataSetChanged();
                }
            });

        }
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    @Override
    public void onBindViewHolder(@NonNull RViewHolder holder, int position)
    {
        if (iIndex == null || task == null || position >= numIndex) return;
        if (iIndex.length != result.size() || numIndex >= iIndex.length)
            CheckIndex();
        if (position >= numIndex) return;
        if (w == 0 || h == 0)
        {
            holder.imgFile.onFirstDraw = new ActionCallback(this, "RefreshBoundRect", Rect.class);
            return;
        }
        SetVisibleRange(position);
        DirEntry o = result.get(iIndex[position]);
        int i = holder.getLayoutPosition();
        Rect rc = new Rect();
        holder.imgFile.Invalidate();
        holder.imgFile.getDrawingRect(rc); //.getMeasuredHeight();
        //  if (rc.width() > 0 && rc.height() > 0 && rc.height() <= h && rc.width() <= w)
        {
            //    w = rc.width();
            //   h = rc.height();
        }
        // else
        if (w == 0 || h == 0)
        {
            holder.imgFile.measure(0, 0);
            h = holder.imgFile.getMeasuredHeight();
            w = holder.imgFile.getMeasuredWidth();

        }


        if (o != null)
        {

            holder.title.setText(o.title.replaceAll("@", ":"));
            String s = o.info;
            if (s.length() > 0)
                holder.sizeImage.setText(s);
            else
            {
                s += o.img.width;
                s += "x";
                s += o.img.height;
            }
            holder.sizeImage.setText(s);
            if (!o.fIsDir && !colect.contains(iIndex[position]))
            {
                o.self = iIndex[position];
                synchronized (sync)
                {
                    colect.add(o.self);
                }
                if (Check(holder, o)) return;

                holder.imgFile.setImageBitmap(null);
                /*if (firstTime)
                {
                    holder.v.measure(0, 0);
                    h = holder.imgFile.getMeasuredHeight();
                    w = holder.imgFile.getMeasuredWidth();
                    firstTime = false;
                }*/
                View v = grid.findViewByPosition(invIndex[o.self]);
                if (v != null)
                {
                    ProgressBar progressBar = v.findViewById(R.id.progress);
                    progressBar.setProgress(0);
                    progressBar.setVisibility(View.GONE);
                }
                if (o.thumb_img == null)
                    task.Submit(new LoadThumbImage(o, w, h));
                else
                    Invalidate(holder, o);
            } else if (o.fIsDir)
            {
                holder.imgFile.setImageResource(R.drawable.directory_icon);
            } else if (o.thumb_img != null)
            {
                Invalidate(holder, o);
            } else
                holder.imgFile.setImageBitmap(null);


        }
        if (position == selectedPos || holder.itemView.isSelected())
        {
            holder.itemView.setSelected(position == selectedPos);
            if (position == selectedPos && !o.fIsDir)
                holder.sizeImage.setVisibility(View.VISIBLE);
            else
                holder.sizeImage.setVisibility(View.INVISIBLE);
        }
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    @Override
    public int getItemCount()
    {
        CheckIndex();

        return numIndex;
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    public DirEntry GetSelectedElement()
    {
        return selectedPos < 0 ? null : result.get(iIndex[selectedPos]);
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    @Override
    public void onDetachedFromRecyclerView(RecyclerView recyclerView)
    {

        self = null;
        if (onselect != null)
            onselect.Clear();
        onselect = null;
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    protected boolean Check(RViewHolder holder, DirEntry o)
    {
        try
        {
            if (o.thumb_img != null && o.thumb_img.contains(".png"))
            {
                try
                {
                    holder.imgFile.setImageBitmap(BitmapUtils.Load(o.thumb_img, 1));
                    return true;
                } catch (Exception e)
                {
                    FileUtils.AddToLog(e);

                }
            }
            Bitmap bitmap = null;
            bitmap = cache.cache.Get(o.self, o.MakeTrumbName(), iIndex[lastVisibleFirst], iIndex[lastVisibleLast]);

            if (bitmap == null || bitmap.isRecycled())
                return false;
            else
            {
                o.thumb_img = o.MakeTrumbName();
                holder.imgFile.setImageBitmap(bitmap);
                return true;
            }
        } catch (IOException e)
        {
            FileUtils.AddToLog(e);
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    protected boolean Invalidate(RViewHolder holder, DirEntry o)
    {
        try
        {
            Bitmap bitmap = null;
            if (o.thumb_img != null && o.thumb_img.contains(".png"))
            {
/*                float aspect = (float)(float)bitmaps[position].getWidth()/bitmaps[position].getHeight();
                if (aspect > 1.0f)
                    h = w/aspect;
                else
                    w = h * aspect;

                data[position] = Bitmap.createScaledBitmap(bitmaps[position], (int)(w), (int)(h), false);
*/
                try
                {
                    bitmap = BitmapUtils.Load(o.thumb_img, 1);

                } catch (Exception e)
                {
                    FileUtils.AddToLog(e);

                }
            }

            bitmap = cache.cache.Get(o.self, o.thumb_img, iIndex[lastVisibleFirst], iIndex[lastVisibleLast]);

            if (bitmap == null || bitmap.isRecycled())
                bitmap = null; //AGlobals.default_image;
            else
            {
                holder.imgFile.setImageBitmap(bitmap);
                return true;
            }
        } catch (IOException e)
        {
            FileUtils.AddToLog(e);
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    public Bitmap GetImage(DirEntry o)
    {
        try
        {
            return cache.cache.Get(o.self, o.thumb_img, iIndex[lastVisibleFirst], iIndex[lastVisibleLast]);


        } catch (IOException e)
        {
            FileUtils.AddToLog(e);
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }
        return null;
    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    public void Select(String word)
    {
        CheckIndex();
        if (word == null || word.length() == 0)
        {
            numIndex = result.size();
            for (int i = 0; i < numIndex; ++i)
                invIndex[i] = iIndex[i] = i;
        } else
        {
            numIndex = result.size();
            int n = 0;
            for (int i = 0; i < numIndex; ++i)
            {
                String line = result.get(i).title;
                invIndex[i] = i;
                if (line.toLowerCase().contains(word.toLowerCase()))
                {
                    iIndex[n] = i;
                    invIndex[i] = n;
                    ++n;
                }
            }

            numIndex = n;
        }
        selectedPos = -1;
        notifyDataSetChanged();
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    protected void CheckIndex()
    {
        int count = result.size();
        if (iIndex == null && count == 0)
        {
            numIndex = 0;
            selectedPos = -1;
        } else if (iIndex == null || iIndex.length != count)
        {
            if (count > 0)
            {
                iIndex = new int[count];
                invIndex = new int[count];
                for (int i = 0; i < count; ++i)
                    invIndex[i] = iIndex[i] = i;
            } else
            {
                iIndex = null;
                invIndex = null;

            }
            numIndex = count;
            selectedPos = -1;
        }

    }

    //--------------------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------------------
    public void SetVisibleRange(int position)
    {
        synchronized (syncLastVisible)
        {
            lastVisibleFirst = grid.findFirstVisibleItemPosition();
            lastVisibleLast = grid.findLastVisibleItemPosition();

            if (lastVisibleFirst < 0)
                lastVisibleFirst = 0;
            if (lastVisibleFirst > position)
                lastVisibleFirst = position;
            if (lastVisibleLast < position)
                lastVisibleLast = position;
        }
    }

    void setImageFile(Bitmap bmp)
    {
    }

    //*******************************************************************
    public class RViewHolder extends RecyclerView.ViewHolder {
        CheckRecycleImageView imgFile;
        ProgressBar progressBar;
        TextView title;
        TextView sizeImage;
        View v;

        //-----------------------------------------------------------
        //
        //-----------------------------------------------------------
        public RViewHolder(View itemView)
        {
            super(itemView);
            itemView.setOnClickListener(new OnClickCalback(this, "OnClick"));
            v = itemView;
            imgFile = itemView.findViewById(R.id.imageView);
            progressBar = (ProgressBar) itemView.findViewById(R.id.progress);
            sizeImage = (TextView) itemView.findViewById(R.id.sizeImage);
            title = (TextView) itemView.findViewById(R.id.textView);


        }

        //-----------------------------------------------------------
        //
        //-----------------------------------------------------------
        public void OnClick(View v)
        {
            int oldpos = selectedPos;
            int pos = getLayoutPosition();
            if (selectedPos == pos)
            {
                selectedPos = -2;
                v.setSelected(false);
                sizeImage.setVisibility(View.INVISIBLE);
                if (onselect != null)
                    onselect.run((DirEntry) null);
            } else
            {
                selectedPos = pos;
                v.setSelected(true);
                if (oldpos >= 0)
                    notifyItemChanged(oldpos);
                //      if (type_data == DATA_IMAGE)
                //        sizeImage.setVisibility(View.VISIBLE);
                notifyItemChanged(pos);
                if (onselect != null)
                    onselect.run(GetSelectedElement());

            }

        }

    }

    class LoadThumbImage extends ILoadingListener {
        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        public LoadThumbImage(DirEntry o, int w, int h)
        {
            super(o, w, h);
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        public void OnNotify(long cmd, long wparam, long lparam)
        {
            if (self == null) return;
            Bitmap b = (Bitmap) AGlobals.LongToObject(wparam);
            if (b != null)
                OnEnd(b);
            else
            {
                String uri = "drawable/error_photo";
                int imageResource = AGlobals.contextApp.getResources().getIdentifier(uri, null, AGlobals.contextApp.getPackageName());
                b = ((BitmapDrawable) ContextCompat.getDrawable(AGlobals.contextApp,imageResource)).getBitmap();
                //OnEnd(Bitmap.createBitmap(AGlobals.error_image));
                OnEnd(b);
            }
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public boolean IsVisiblePosition(int position)
        {
            int f, l;
            if (self == null) return false;
            position = invIndex[position];
            synchronized (syncLastVisible)
            {
                f = lastVisibleFirst;
                l = lastVisibleLast;
            }
            return position >= f && position <= l;
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public boolean OnStart()
        {
            if (self == null) return false;

            return cache.cache.MemPoolSize() < (lastVisibleLast - lastVisibleFirst);
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void OnEnd(byte[] _loadedImage, int scale)
        {
            if (self == null) return;

            boolean fOk = false;
            try
            {
                int first, last;
                synchronized (syncLastVisible)
                {
                    first = iIndex[lastVisibleFirst];
                    last = iIndex[lastVisibleLast];
                }
                if ((obj.thumb_img = cache.cache.Add(obj.MakeTrumbName(), obj.self, first, last, _loadedImage, scale)) != null)
                    fOk = true;
            } catch (IOException e)
            {

                FileUtils.AddToLog(e);


            } catch (Exception e)
            {
                FileUtils.AddToLog(e);
            }
            if (fOk)
                OnLoad();

        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void OnEnd(Bitmap _loadedImage)
        {
            if (self == null) return;

            boolean fOk = false;
            {
                if (_loadedImage == null)
                {
                    String uri = "drawable/error_photo";
                    int imageResource = AGlobals.contextApp.getResources().getIdentifier(uri, null, AGlobals.contextApp.getPackageName());
                    _loadedImage = ((BitmapDrawable)ContextCompat.getDrawable(AGlobals.contextApp,imageResource)).getBitmap();
                }
            }
            try
            {
                int first, last;
                synchronized (syncLastVisible)
                {
                    first = iIndex[lastVisibleFirst];
                    last = iIndex[lastVisibleLast];
                }
                if ((obj.thumb_img = cache.cache.Add(obj.MakeTrumbName(), obj.self, first, last, _loadedImage)) != null)
                    fOk = true;
            } catch (IOException e)
            {

                FileUtils.AddToLog(e);
            }
            if (fOk)
                OnLoad();
        }

        @Override
        public void OnEnd(String title, Bitmap _loadedImage)
        {
            boolean fOk = false;
            if (self == null) return;

            {
                if (_loadedImage == null)
                {
                    String uri = "drawable/error_photo";
                    int imageResource = AGlobals.contextApp.getResources().getIdentifier(uri, null, AGlobals.contextApp.getPackageName());
                    _loadedImage = ((BitmapDrawable)ContextCompat.getDrawable(AGlobals.contextApp,imageResource)).getBitmap();
                }
            }
            try
            {
                int first, last;
                synchronized (syncLastVisible)
                {
                    first = iIndex[lastVisibleFirst];
                    last = iIndex[lastVisibleLast];
                }
                obj.title = title;
                if ((obj.thumb_img = cache.cache.Add(obj.MakeTrumbName(), obj.self, first, last, _loadedImage)) != null)
                {
                    fOk = true;
                }
            } catch (IOException e)
            {

                FileUtils.AddToLog(e);
            }
            if (fOk)
                OnLoad();

        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void OnEnd(String path)
        {

        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        private void OnLoad()
        {

            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run()
                {

                    if (self == null) return;
                    int first, last;
                    View v = grid.findViewByPosition(invIndex[obj.self]);
                    if (v != null)
                    {
                        SetVisibleRange(lastVisibleFirst);
                        Bitmap loadedImage;
                        first = iIndex[lastVisibleFirst];
                        last = iIndex[lastVisibleLast];
                        try
                        {
                            if (obj.self < first)
                                first = obj.self;
                            if (obj.self > last)
                                last = obj.self;
                            loadedImage = cache.cache.Get(obj.self, obj.thumb_img, first, last);
                            if (loadedImage != null)
                            {
                                CheckRecycleImageView imgFile = v.findViewById(R.id.imageView);
                                imgFile.setImageBitmap(loadedImage);

                            }
                        } catch (IOException e)
                        {
                            FileUtils.AddToLog(e);

                        } catch (Exception e)
                        {
                            FileUtils.AddToLog(e);
                        }

                    } else
                        notifyItemChanged(invIndex[obj.self]);


                }
            });

        }

        //-------------------------------------------------------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------------------------------------------------------
        @Override
        public void OnCancel()
        {
            if (self == null) return;
            AGlobals.rootHandler.Send(new Runnable() {
                @Override
                public void run()
                {
                    colect.remove(invIndex[obj.self]);
                    notifyItemChanged(invIndex[obj.self]);
                }
            });
        }

        //-------------------------------------------------------------------------------------------------------------------------------
        //
        //-------------------------------------------------------------------------------------------------------------------------------
        @Override
        public void OnProgress(int _current, int _total)
        {
        }
    }

}
