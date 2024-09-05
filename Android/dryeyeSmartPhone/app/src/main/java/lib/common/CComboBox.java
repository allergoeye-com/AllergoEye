package lib.common;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;

import androidx.core.content.ContextCompat;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.dryeye.app.R;

import lib.utils.AGlobals;

import java.util.ArrayList;

public class CComboBox extends androidx.appcompat.widget.AppCompatSpinner {
    protected ComboBoxAdapter adapter;
    protected int index;
    protected int text_color = 0;
    protected int bg_color = 0;
    protected int text_size = 0;
    public void SetColors(int fg_text, int bg_text)
    {
        text_color = fg_text;
        bg_color = bg_text;

    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public CComboBox(Context context) {
        super(context);
        InitAtribut(context, null, 0, 0);
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public CComboBox(Context context, int mode) {
        super(context, mode);
        InitAtribut(context, null, 0, 0);
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public CComboBox(Context context, AttributeSet attrs) {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public CComboBox(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public CComboBox(Context context, AttributeSet attrs, int defStyleAttr, int mode) {
        super(context, attrs, defStyleAttr, mode);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public CComboBox(Context context, AttributeSet attrs, int defStyleAttr, int mode, Resources.Theme popupTheme) {
        super(context, attrs, defStyleAttr, mode, popupTheme);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }
    @SuppressLint("ResourceType")
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int desS)
    {

        text_color = 0;
        bg_color = 0;
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.CComboBox, defA, desS);
            try {
                text_color = a.getColor(R.styleable.CComboBox_text_color, ContextCompat.getColor(context, R.color.theme_dlg_text_color));
                bg_color = a.getColor(R.styleable.CComboBox_bg_color, ContextCompat.getColor(context, R.color.theme_dlg_background));
                text_size = a.getResourceId(R.styleable.CComboBox_text_size, android.R.style.TextAppearance_Small);



            }

            catch(Exception e)
            {
                ;
            }
            a.recycle();
        }
        adapter = new ComboBoxAdapter(context);
        index = -1;
        setAdapter(adapter);

        this.setOnItemSelectedListener(adapter);
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Init(Drawable [] bmp, String [] data, int _index, ActionCallback callback)
    {
        index = _index;
        adapter.Init(bmp, data, callback);
        setSelection(index);
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Init(String [] data, int _index, ActionCallback callback)
    {
        index = _index;
        adapter.Init(data, callback);
        setSelection(index);
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Init(ArrayList<String> data, int _index, ActionCallback callback)
    {
        String [] d = new String[data.size()];
        for (int i = 0, l = data.size(); i < l; ++i)
            d[i] = data.get(i);
        Init(d, _index, callback);
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    public void Init(String [] data, int [] colors, int _index, ActionCallback callback)
    {
        index = _index;
        adapter.Init(data, colors, callback);
        setSelection(index);
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
        setSelection(index);
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public String GetSelString()
    {
        return index >= 0 && adapter != null && adapter.data != null && adapter.data.length > index ?adapter.data[index] : null;
    }
    //------------------------------------------------------
    //
    //------------------------------------------------------
    public int GetSelColor()
    {
        return index >= 0 && adapter != null && adapter.colors != null && adapter.colors.length > index ?adapter.colors[index] : -1;
    }

    //------------------------------------------------------
    //
    //------------------------------------------------------
    private class ComboBoxAdapter extends BaseAdapter implements AdapterView.OnItemSelectedListener {
        ActionCallback action;
        String [] data;
        int [] colors;
        Context context;
        Drawable [] bmp = null;
        public ComboBoxAdapter(Context _context)
        {
            context = _context;
            data = null;
            action = null;

        }

        public void Init(Drawable [] _bmp, String [] _data, ActionCallback _action)
        {
            bmp = _bmp;
            colors = null;
            data = _data;
            action = _action;
            notifyDataSetChanged();

        }
        public void Init(String [] _data, ActionCallback _action)
        {
            colors = null;
            data = _data;
            action = _action;
            notifyDataSetChanged();

        }
        public void Init(String [] _data, int [] _colors, ActionCallback _action)
        {

            colors = _colors;
            data = _data;
            action = _action;
            notifyDataSetChanged();

        }
        public void Init(Drawable [] _bmp, String [] _data, int [] _colors, ActionCallback _action)
        {
            bmp = _bmp;
             colors = _colors;
            data = _data;
            action = _action;
            notifyDataSetChanged();

        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public int getCount() {
            return data != null ? data.length : 0;
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public Object getItem(int position) {
            return data != null && data.length > position ?  data[position] : 0;
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public long getItemId(int position) {
            return data != null && data.length > position ? position : -1;
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (data == null) return null;
            if(convertView == null){
                LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView = inflater.inflate(R.layout.combobox_cell, null);
            }
            convertView.setWillNotDraw(true);
            if (bmp != null)
            {
                ImageView img = (ImageView) convertView.findViewById(R.id.imageView);
                img.setImageDrawable(bmp[position]);
            }
            TextView text = (TextView) convertView.findViewById(R.id.textView);
            if (text_size != 0 && AGlobals.currentActivity != null && text != null)
                text.setTextAppearance(AGlobals.currentActivity, text_size);
                if (text != null && colors != null)
                    AGlobals.SetTextColor(text, colors[position]);
            else
            {
                if (bg_color != 0)
                    convertView.setBackgroundColor(bg_color);

                if (text_color != 0)
                    text.setTextColor(text_color);
                else
                {
                    int c = 0;
                    Drawable background = text.getBackground();
                    if (background == null)
                        if (parent != null)
                           background = parent.getBackground();
                    if (background != null && background instanceof ColorDrawable)
                        c = ((ColorDrawable)background).getColor();
                    if (c != 0)
                    {
                        text.setBackgroundColor(c);
                        AGlobals.SetTextColor(text, c);
                    }
                }
            }
            text.setText(data[position]);

            return convertView;
        }
        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public View getDropDownView(int position, View convertView, ViewGroup parent) {
            if (data == null) return null;
            if(convertView == null){
                LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView = inflater.inflate(R.layout.combobox_cell, null);
            }
            convertView.setWillNotDraw(false);
            TextView text = (TextView) convertView.findViewById(R.id.textView);
            if (text_size != 0)
                text.setTextAppearance(AGlobals.currentActivity, text_size);
            if (bmp != null)
            {
                ImageView img = (ImageView) convertView.findViewById(R.id.imageView);
                img.setImageDrawable(bmp[position]);
            }

            if (colors != null)
                AGlobals.SetTextColor(text, colors[position]);
            else
            {
                if (bg_color != 0)
                    convertView.setBackgroundColor(bg_color);
                if (text_color != 0)
                    text.setTextColor(text_color);
                else
                {
                    int c = 0;
                    Drawable background = text.getBackground();
                    if (background == null)
                        if (parent != null)
                            background = parent.getBackground();
                    if (background != null && background instanceof ColorDrawable)
                        c = ((ColorDrawable)background).getColor();
                    if (c != 0)
                    {
                        text.setBackgroundColor(c);
                        AGlobals.SetTextColor(text, c);
                    }

                }
            }

            text.setText(data[position]);

            return convertView;
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void onItemSelected(AdapterView<?> parent, View _view, int position, long id) {
            if (index == position) return;
            index = position;
            if (action != null)
                action.run(position);
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void onNothingSelected(AdapterView<?> parent) {

        }
    }

}
