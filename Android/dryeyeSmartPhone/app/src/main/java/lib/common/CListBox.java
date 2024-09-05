package lib.common;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;

import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;


import com.dryeye.app.R;

import java.util.ArrayList;

import lib.utils.AGlobals;

public class CListBox extends ListView {
    ListBoxAdapter adapter;
    ActionCallback action;
    int index;

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public CListBox(Context context)
    {
        super(context);
        Init(context);
    }

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public CListBox(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        Init(context);
    }


    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    public CListBox(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        Init(context);

    }


    //------------------------------------------------------
    //
    //------------------------------------------------------
    private void Init(Context context)
    {
        action = null;
        adapter = new ListBoxAdapter(context);
        index = -1;
        setAdapter(adapter);
        this.setOnItemClickListener(adapter);
        setDividerHeight(2);
        setClickable(true);
        setChoiceMode(ListView.CHOICE_MODE_SINGLE);
        setScrollContainer(true);
        setVerticalScrollBarEnabled(true);
        //  setFastScrollAlwaysVisible(true);
        setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                v.getParent().requestDisallowInterceptTouchEvent(true);
                return false;
            }
        });
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Init(String[] data, int _index, ActionCallback callback)
    {
        index = _index;
        action = callback;
        adapter.Init(data);
        setSelection(index);

    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Replace(String[] data, int _index)
    {
        index = _index;
        adapter.Init(data);
        setSelection(index);

    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Replace(ArrayList<String> data, int _index)
    {
        String[] d = new String[data.size()];
        for (int i = 0, l = data.size(); i < l; ++i)
            d[i] = data.get(i);
        Replace(d, _index);

    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Init(ArrayList<String> data, int _index, ActionCallback callback)
    {
        String[] d = new String[data.size()];
        for (int i = 0, l = data.size(); i < l; ++i)
            d[i] = data.get(i);
        Init(d, _index, callback);
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Init(String[] data, int _index)
    {
        index = _index;
        adapter.Init(data);
        setSelection(index);
        adapter.notifyDataSetInvalidated();
    }
    public void Init(ActionCallback callback)
    {
        action = callback;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public int GetSel()
    {
        return index;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void SetSel(int _index)
    {

        index = _index;
        if (index < 0)
            clearChoices();
        else
            setSelection(index);
        adapter.notifyDataSetInvalidated();
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public String GetSelString()
    {
        return index >= 0 && adapter != null && adapter.data != null && adapter.data.length > index ? adapter.data[index] : null;
    }

    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    class ListBoxAdapter extends ArrayAdapter<String> implements OnItemClickListener {
        String[] data;
        Context context;

        public ListBoxAdapter(@NonNull Context _context)
        {
            super(_context, R.layout.list_box);
            context = _context;
            data = null;
        }

        public void Init(String[] _data)
        {
            data = _data;
            this.clear();
            this.addAll(data);
            notifyDataSetChanged();

        }

        //---------------------------------------------------------
        //
        //---------------------------------------------------------
        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            if (convertView == null)
            {
                LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView = inflater.inflate(R.layout.list_box, null);
            }
            TextView text = (TextView) convertView.findViewById(R.id.textView);
            ImageView image = (ImageView) convertView.findViewById(R.id.imageView);
            Drawable b = null;
            if (index == position)
            {
                String uri = "drawable/full_circle";
                int imageResource = context.getResources().getIdentifier(uri, null, context.getPackageName());
                b = ContextCompat.getDrawable(AGlobals.contextApp,imageResource);

                convertView.setBackgroundColor(ContextCompat.getColor(getContext(), R.color.middle_grey));
            } else {
                String uri = "drawable/empty_circle";
                int imageResource = context.getResources().getIdentifier(uri, null, context.getPackageName());
                b = ContextCompat.getDrawable(AGlobals.contextApp,imageResource);

                convertView.setBackgroundColor(0x00000000);
            }
            text.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
            image.setImageDrawable(b);
            text.setText(data[position]);

            return convertView;
        }

        //---------------------------------------------------------
        //
        //---------------------------------------------------------
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            if (index == position) return;
            index = position;
            if (action != null)
                action.run(position);
            this.notifyDataSetInvalidated();
        }
    }
}
