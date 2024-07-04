package lib.common;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;

import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.viewpager.widget.PagerAdapter;
import androidx.viewpager.widget.ViewPager;

import android.graphics.drawable.StateListDrawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;


import com.allergoscope.app.R;

import java.io.IOException;
import java.util.ArrayList;

import lib.common.button.CMenuButton;
import lib.common.layout.LinearLayoutBorder;
import lib.explorer.TaskReadDir;
import lib.explorer.common.DirEntry;
import lib.explorer.common.ILoadingListener;
import lib.explorer.common.TaskDir;
import lib.utils.AGlobals;
import lib.utils.BitmapUtils;
import lib.utils.FileUtils;

public class ImageSlider  extends LinearLayoutBorder {
    ArrayList<DirEntry> dirEntries = null;
    CListBox cListBox = null;
    String [] types = null;
    Bitmap[] bitmaps = null;
    boolean [] fChanged = null;
    int index;
    ViewPager viewPager = null;
    CMenuButton button[]  = null;
    boolean isSlide;
    ActionCallback callback = null;
    SlideAdapter adapter = null;
    public ActionCallback  OnTypeChanged; //(int i, boolean fReplce);
    AGlobals.CasheFromExplore cache;
    TaskDir task;
    Bitmap [] data = null;
    boolean fReplace = false;
    boolean fViewTitle = true;
    boolean fViewList = true;

    //---------------------------------------------
    //
    //---------------------------------------------
    public ImageSlider(Context context) {
        super(context);
        InitAtribut(context, null, 0, 0);
    }

    //---------------------------------------------
    //
    //---------------------------------------------
    public ImageSlider(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);

    }

    //---------------------------------------------
    //
    //---------------------------------------------
    public ImageSlider(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }
    //-------------------------------------------
    //
    //-------------------------------------------
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.ImageSlider, defA, defS);
            try
            {
                fViewTitle = a.getBoolean(R.styleable.ImageSlider_view_title, true);
                fViewList = a.getBoolean(R.styleable.ImageSlider_view_list, true);


            } catch (Exception e)
            {
                ;
            }
            a.recycle();

        }
    }

    public void Init(ArrayList<DirEntry> _dirEntries, int _index, ActionCallback _callback)
    {
        fReplace = false;
        dirEntries = _dirEntries;
        cache = AGlobals.CreateCache("thromb_local");
        cache.cache.ClearMemory();
        cache.result.clear();

        task = new TaskReadDir();
        callback = _callback;
        bitmaps = new Bitmap[dirEntries.size()];
        fChanged = new boolean[bitmaps.length];
        types = new String[dirEntries.size()];
        for (int i = 0; i < bitmaps.length; ++i)
        {
            fChanged[i] = true;
            bitmaps[i] = null;
            types[i] = dirEntries.get(i).title;
            dirEntries.get(i).self = i;


        }
        index =_index;
        isSlide =  false;
        cListBox = findViewById(R.id.idListView);
        if (!fViewList && cListBox != null)
        {
            cListBox.setVisibility(GONE);
            cListBox = null;
        }
        button  = new CMenuButton[2];
        button[0] = findViewById(R.id.idPrev);
        button[1] = findViewById(R.id.idNext);
        button[0].setVisibility(View.GONE);
        button[1].setVisibility(View.GONE);
        button[0].SetCallback(this, "OnPrevSlide");
        button[1].SetCallback(this, "OnNextSlide");
        viewPager =  findViewById(R.id.idViewPager);
        if (cListBox != null)
        cListBox.Init(types, index);
        adapter = new SlideAdapter();
        viewPager.setAdapter(adapter);
        adapter.notifyDataSetChanged();
        viewPager.addOnPageChangeListener(new COnChangeSlide());
        viewPager.setCurrentItem(index);
        if (cListBox != null)
            cListBox.Init(new ActionCallback(this, "OnListChanged", int.class));
        CheckButtons();
        if (cListBox == null)
            requestLayout();

    }
    public String Result()
    {
        return dirEntries.size() > 0 ? dirEntries.get(index).img.url : null;
    }
    public void Replace(int i, Bitmap bmp)
    {
        if (bitmaps != null && i < bitmaps.length)
        {
            if (bitmaps[i] != null)
                bitmaps[i].recycle();
            bitmaps[i] = bmp;
            fChanged[i] = true;
            adapter.notifyDataSetChanged();

        }


    }
    public void Replace(int i, String bmp)
    {
        if (bitmaps != null && i < bitmaps.length)
        {
            if (bitmaps[i] != null)
                bitmaps[i].recycle();
            bitmaps[i] = BitmapUtils.LoadAssets(bmp);
            fChanged[i] = true;
            adapter.notifyDataSetChanged();

        }


    }

    public void Reset()
    {
        OnDestroySlideAdapter();

    }

    //---------------------------------------------
    //
    //---------------------------------------------
    public void OnListChanged(int _index)
    {
        isSlide = true;
        index = _index;
        boolean fDataChanged = bitmaps[index] == null;
        _OnTypeChanged(index, bitmaps[index] == null );

        viewPager.setCurrentItem(_index, true);
        CheckButtons();

    }
    //---------------------------------------------
    //
    //---------------------------------------------
    public void OnChangeSlide(int _index)
    {

        if (index == _index)
        {
            isSlide = false;

        }
        if (isSlide) return;
        if (index != _index)
        {
            index = _index;

            _OnTypeChanged(index, bitmaps[index] == null );

        }
        if (callback != null) callback.run(index);

        if (cListBox != null)
            if (cListBox.GetSel() != index)
            {
                cListBox.SetSel(index);
                CheckButtons();
            }
    }
    void CheckButtons()
    {
        if (index > 0)
            button[0].setVisibility(View.VISIBLE);
        else
            button[0].setVisibility(View.GONE);

        if (index < bitmaps.length - 1)
            button[1].setVisibility(View.VISIBLE);
        else
            button[1].setVisibility(View.GONE);


    }
    void OnDestroySlideAdapter()
    {
        viewPager.setAdapter(null);
        for (int j = 0; j < bitmaps.length; ++j)
        {
            if (bitmaps[j] != null && !bitmaps[j].isRecycled())
                bitmaps[j].recycle();
            if (data[j] != null && !data[j].isRecycled())
                data[j].recycle();
                data[j] = null;
                bitmaps[j] = null;
        }
        try {
            cache.cache.ClearMemory();
            cache.result.clear();
            cache.cache.close();
            cache = null;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    //---------------------------------------------
    //
    //---------------------------------------------
    public void OnPrevSlide()
    {
        if (index >  0)
        {
            --index;
            boolean fDataChanged = bitmaps[index] == null;

            _OnTypeChanged(index, bitmaps[index] == null );

            isSlide = true;
            viewPager.setCurrentItem(index, true);

        }
    }
    //---------------------------------------------
    //
    //---------------------------------------------
    public void OnNextSlide()
    {
        if (index < bitmaps.length - 1)
        {
            ++index;
            isSlide = true;
            boolean fDataChanged = bitmaps[index] == null;
            _OnTypeChanged(index, bitmaps[index] == null );
            viewPager.setCurrentItem(index, true);

        }
    }
    //------------------------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------------------
    class COnChangeSlide implements ViewPager.OnPageChangeListener {
        //---------------------------------------------
        //
        //---------------------------------------------
        @Override
        public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels)
        {
            OnChangeSlide(position);
        }

        //---------------------------------------------
        //
        //---------------------------------------------
        @Override
        public void onPageSelected(int position) {

            //  OnChangeSlide(position);
        }

        @Override
        public void onPageScrollStateChanged(int state) {

        }
    };
    //---------------------------------------------
    //
    //---------------------------------------------
    class SlideAdapter  extends PagerAdapter {

        boolean fFirstTime = true;


        @Override
        public int getCount() {

            return bitmaps == null ? 0 : bitmaps.length;
        }
        //------------------------------------------------------------------
        //
        //------------------------------------------------------------------
        @Override
        public boolean isViewFromObject(View view, Object object) {
            return view.equals(object);
        }
        //------------------------------------------------------------------
        //
        //------------------------------------------------------------------
        @Override
        public Object instantiateItem(ViewGroup container, int position) {
            if (types == null || types.length <= position) return null;
            if (data == null || data.length != bitmaps.length)
            {
                if (data != null && data.length > 0)
                    for (int i = 0; i < data.length; ++i)
                        if (data[i] != null)
                            data[i].recycle();

                data = new Bitmap[bitmaps.length];
                for (int i = 0; i < data.length; ++i)
                    data[i] = null;
            }
            View itemView = LayoutInflater.from(container.getContext()).inflate(R.layout.grid_item_slider, container, false);
            ImageView imgView = (ImageView) itemView.findViewById(R.id.imageView);
            TextView txtView = (TextView) itemView.findViewById(R.id.textView);

            itemView.measure(0, 0);
            if (!fViewTitle)
            {
                txtView.setVisibility(GONE);
            }
            else
            {
                if (dirEntries.get(position).info.length() > 0)
                    txtView.setText(dirEntries.get(position).info);
            }
            float h = imgView.getMeasuredHeight();
            float w = imgView.getMeasuredWidth();
            if (dirEntries.get(position).any != null && dirEntries.get(position).any instanceof Bitmap)
            {
                bitmaps[position] = (Bitmap)dirEntries.get(position).any;
                fChanged[position] = false;
                fFirstTime = false;
                if (imgView.getMeasuredHeight() == 0 && imgView.getMeasuredWidth() == 0)
                {
                    w = bitmaps[position].getWidth();
                    h = bitmaps[position].getHeight();
                }
            }

            if (fFirstTime)
            {
                final float _h = h;
                final float _w = w;

                fFirstTime =false;
                postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        for (int j = 0; j < dirEntries.size(); ++j)
                            if (bitmaps[j] == null)
                                if (dirEntries.get(j).any != null && dirEntries.get(j).any instanceof Bitmap)
                                {
                                    bitmaps[j] = (Bitmap)dirEntries.get(j).any;
                                    fChanged[j] = true;
                                }
                                else
                                task.Submit(new LoadThumbImage(dirEntries.get(j), (int)_w, (int)_h));

                    }
                }, 500);

            }
         /*   if (data[position] == null)
            {
                float aspect = (float)bitmaps[position].getHeight()/(float)bitmaps[position].getWidth();
                h = w * aspect;
                data[position] = Bitmap.createScaledBitmap(bitmaps[position], (int)(w  + 0.5f), (int)(h + 0.5f), false);


            }
*/
            if (fChanged[position])
            {
                imgView.setImageBitmap(null);
                if (bitmaps[position] != null)
                    fChanged[position] = false;
            }
            if (data[position] == null && bitmaps[position] != null)
            {
                float aspect = (float)(float)bitmaps[position].getWidth()/bitmaps[position].getHeight();
                if (aspect > 1.0f)
                    h = w/aspect;
                else
                    w = h * aspect;
                data[position] = BitmapUtils.ResizeBitmap(bitmaps[position], (int)(w), (int)(h));
                fChanged[position] = false;


            }

            if (data[position] != null && data[position].isRecycled())
            {
                data[position] = null;
                fChanged[position] = true;
            }

            imgView.setImageBitmap(data[position]);
            container.addView(itemView);
            if (bitmaps[index] != null && data[index] == null)
            {
                final int N = index;
                AGlobals.rootHandler.Send(new Runnable() {
                    @Override
                    public void run() {

                        if (bitmaps[N] != null && data[N] == null)
                        {
                            fChanged[N] = true;
                            adapter.notifyDataSetChanged();
                        }

                    }
                }, 100);

            }
//            if (index == position)
  //          if (bitmaps[index] != null && data[index] != null)
    //            txtView.setText(types[position]);


            return itemView;
        }

        //------------------------------------------------------------------
        //
        //------------------------------------------------------------------
        @Override
        public void destroyItem(ViewGroup container, int position, Object object) {
            container.removeView((LinearLayout) object);
            if (data[position] != null )
            {
                data[position].recycle();
                data[position] = null;
            }

        }
        @Override
        public int getItemPosition(Object object){
            for (int i = 0; i < bitmaps.length; ++i)
                if (fChanged[i])
                    return PagerAdapter.POSITION_NONE;
            return super.getItemPosition(object);
        }
    }
    boolean  ttt = false;
    private void _OnTypeChanged(int i, boolean fReplce)
    {
        /*
        if (ttt) return;
        ttt = true;
        try {
            OnTypeChanged.run(i, fReplce);
        }
        catch (Exception e)
        {
        }
        ttt = false;
        */

    }
    class LoadThumbImage extends ILoadingListener {

        public LoadThumbImage(DirEntry o, int w, int h) {
            super(o, w, h);
        }

        @Override
        public boolean OnStart() {
            return false;
        }

        @Override
        public void OnEnd(byte[] _loadedImage, int scale) {

            try {

                if ((obj.thumb_img = cache.cache.Add(obj.MakeTrumbName(), obj.self, 0, dirEntries.size(), _loadedImage, scale)) != null)
                {
                    final Bitmap bmp;
                    final int N = obj.self;

                            bmp = cache.cache.Get(obj.self, obj.thumb_img, 0, dirEntries.size());

                    AGlobals.rootHandler.Send(new Runnable() {
                                                  @Override
                                                  public void run() {
                                                      bitmaps[N] = bmp;
                                                      fChanged[N] = true;
                                                      adapter.notifyDataSetChanged();

                                                  }
                                              });
                }
            } catch (IOException e) {

                FileUtils.AddToLog(e);


            }
            catch (Exception e) {
                FileUtils.AddToLog(e);
            }

        }

        @Override
        public void OnEnd(Bitmap _loadedImage) {

        }

        @Override
        public void OnEnd(String title, Bitmap _loadedImage) {

        }

        @Override
        public void OnEnd(String path) {

        }

        @Override
        public void OnCancel() {

        }

        @Override
        public void OnProgress(int _current, int _total) {

        }

        @Override
        public boolean IsVisiblePosition(int _position) {
            return true;
        }
    }


}

