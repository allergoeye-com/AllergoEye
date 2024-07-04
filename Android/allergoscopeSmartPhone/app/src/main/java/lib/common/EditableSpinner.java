package lib.common;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.Rect;
import android.text.Editable;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.TextView;


import com.allergoscope.app.R;
import lib.utils.AGlobals;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by alex on 18.02.2018.
 */

public class EditableSpinner extends androidx.appcompat.widget.AppCompatSpinner implements IActionCalbackClient {
    private int idLayout;
    private int idEdit;
    private int idLabel;
    boolean IsEdit = false;
    boolean inCalback = false;
    private ListAdapter adapter;
    private ActionCallback callback = null;
    int selected;

    EditableSpinner self;
    String label = null;
    boolean isShow = false;
    public CEdit nextFocus = null;
    public Integer color = null;

    //-----------------------------------
    //
    //-----------------------------------
    public EditableSpinner(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);
    }

    //-----------------------------------
    //
    //-----------------------------------
    public EditableSpinner(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, 0, 0);
    }

    //-----------------------------------
    //
    //-----------------------------------
    public EditableSpinner(Context context, AttributeSet attrs, int defStyleAttr, int mode)
    {
        super(context, attrs, defStyleAttr, mode);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }

    //-----------------------------------
    //
    //-----------------------------------
    public EditableSpinner(Context context, AttributeSet attrs, int defStyleAttr, int mode, Resources.Theme popupTheme)
    {
        super(context, attrs, defStyleAttr, mode, popupTheme);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }

    //-----------------------------------
    //
    //-----------------------------------
    @Override
    public void Detach(ArrayList<Object> stack)
    {
        if (stack == null)
            stack = new ArrayList<Object>();
        if (!stack.contains(this))
        {
            stack.add(this);
            callback.Detach(stack);
            setOnClickListener(null);
        }

    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {

        self = this;

        TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.EditableSpinner, defA, defS);
        try
        {

            idLayout = a.getResourceId(R.styleable.EditableSpinner_idLayout, 0);
            idEdit = a.getResourceId(R.styleable.EditableSpinner_idEdit, 0);
            idLabel = a.getResourceId(R.styleable.EditableSpinner_idText, 0);


        } catch (Exception e)
        {
        }
        a.recycle();
        Init();
    }

    //-----------------------------------
    //
    //-----------------------------------
    @Override
    public void Detach()
    {
        Detach(null);
    }

    //-----------------------------------
    //
    //-----------------------------------
    public void OnClk()
    {
        if (adapter != null && adapter.list != null)
        {
            CEdit v = adapter.GetTextViewSelected();
            v.requestFocusFromTouch();
            IsEdit = true;
            AGlobals.ShowKeyboard(v);
        }
    }

    //-----------------------------------
    //
    //-----------------------------------
    @Override
    public void Init()
    {
        label = null;
        if (idLayout == 0) return;
        adapter = new ListAdapter();
        setAdapter(adapter);

        setOnItemSelectedListener(adapter);
        setFocusable(true);
        setFocusableInTouchMode(true);

    }

    public Editable GetSelString()
    {
        CEdit v = adapter.GetTextViewSelected();
        return v != null ? v.getText() : null;

    }

    public int GetSel()
    {

        return selected;

    }


    //-----------------------------------
    //
    //-----------------------------------
    @Override
    public boolean onTouchEvent(MotionEvent ev)
    {
        if (IsEdit)
        {
            CEdit v = adapter.GetTextViewSelected();
            if (v == null)
                return super.onTouchEvent(ev);

            final int pointerIndex = ev.getActionIndex();
            final float x;
            x = ev.getX();
            final float y = ev.getY();
            Rect rect = v.ScreenRect();
            if (x >= rect.left && y >= rect.top && x <= rect.right && y <= rect.bottom)
            {
                return false;
            }
            IsEdit = false;
            isShow = false;

        }
        if (!isShow)
        {
            isShow = true;
            performClick();

        }
        return super.onTouchEvent(ev);

    }

    //-----------------------------------
    //
    //-----------------------------------
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

    }

    //-----------------------------------
    //
    //-----------------------------------
    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b)
    {
        if (!IsEdit)
            super.onLayout(changed, l, t, r, b);
    }

    //-----------------------------------
    //
    //-----------------------------------
    public void InitData(String[] name, String _label)
    {

        if (idLayout == 0) return;
        label = _label;
        if (adapter != null)
            adapter.Init(name);
    }

    //-----------------------------------
    //
    //-----------------------------------
    public void Hide()
    {
        if (idLayout == 0) return;
        onDetachedFromWindow();

    }

    //-----------------------------------
    //
    //-----------------------------------
    public void updateString(String r)
    {

        View view = getSelectedView();
        if (view != null)
        {
            CEdit textView = (CEdit) view.findViewById(idEdit);
            textView.setText(r);
        }

    }

    public void SetText(String s)
    {
        if (inCalback) return;
        CEdit v = adapter.GetTextViewSelected();
        if (v != null)
        {
            String old = v.getText().toString();
            if (!old.equals(s))
                v.setText(s);
        } else
            adapter.tempSelect = s;

    }

    //-----------------------------------
    //
    //-----------------------------------
    public boolean SetCallback(Object _object, String func)
    {
        if (idLayout == 0) return false;

        callback = new ActionCallback(_object, func, Integer.class, String.class);
        return true;
    }

    @Override
    public void setSelection(int position)
    {
        selected = position;
        super.setSelection(selected);
    }
    //***********************************************************************************
    class OnCloseKeyboard implements android.widget.TextView.OnEditorActionListener {
        @Override
        public boolean onEditorAction(TextView v, int actionId, KeyEvent event)
        {
            IsEdit = false;
            AGlobals.ShowKeyboard(false);
            if (nextFocus != null)
                nextFocus.requestLayout();
            else
                v.clearFocus();
            return true;
        }
    }

    //****************************************************************************************
    public class ListAdapter extends BaseAdapter implements AdapterView.OnItemSelectedListener {
        ArrayList<String> list;
        String tempSelect = null;

        public ListAdapter()
        {
            list = null;
            selected = -1;

        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        public void Init(String[] _list)
        {
            if (_list != null)
            {
                list = new ArrayList<String>();
                list.addAll(Arrays.asList(_list));
            } else
                list = null;
            selected = -1;
            this.notifyDataSetChanged();

        }

        public CEdit GetTextViewSelected()
        {
            View view = getSelectedView();
            return view == null ? null : (CEdit) view.findViewById(idEdit);
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public int getCount()
        {
            return list == null ? 0 : list.size();
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public Object getItem(int position)
        {
            return list == null ? 0 : list.get(position);
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public long getItemId(int position)
        {
            return list == null ? -1 : position;
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            if (list == null || position >= list.size()) return null;
            if (convertView == null)
            {
                LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView = inflater.inflate(idLayout, null);
            }

            if (label != null)
            {
                TextView v = (TextView) convertView.findViewById(idLabel);
                v.setText(label);
            }
            CEdit textView = (CEdit) convertView.findViewById(idEdit);
            textView.setText(list.get(position));
            textView.setOnClickListener(null);
            return convertView;
        }


        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public View getDropDownView(int position, View _convertView, ViewGroup parent)
        {
            View convertView;
            if (_convertView == null)
                convertView = getView(position, _convertView, parent);
            else
                convertView = _convertView;
            CEdit textView = (CEdit) convertView.findViewById(idEdit);
            textView.setText(list.get(position));
            textView.setFocusable(false);
            textView.setCursorVisible(false);
            textView.setClickable(true);
            textView.setSingleLine(false);
            if (color != null)
            {
                AGlobals.SetBackground(textView, color);
                if (label != null)
                {
                    TextView v = (TextView) convertView.findViewById(idLabel);
                    AGlobals.SetBackground(v, color);
                }
            }
            textView.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v)
                {
                    if (isShow)
                    {
                        isShow = false;
                        CEdit t = (CEdit) v;
                        String s = t.getText().toString();
                        int position = list.indexOf(s);
                        setSelection(position);
                        Hide();
                    }

                }
            });
            return convertView;


        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void onItemSelected(AdapterView<?> parent, View _view, int position, long id)
        {
            if (_view == null || list == null) return;
            CEdit textView = (CEdit) _view.findViewById(idEdit);
            textView.setFocusable(true);
            textView.setFocusableInTouchMode(true);
            textView.setCursorVisible(true);
            textView.setClickable(true);
            textView.setSingleLine(true);
            if (tempSelect != null)
            {
                textView.setText(tempSelect);
                tempSelect = null;
            }
            textView.setOnEditorActionListener(new OnCloseKeyboard());
            textView.SetCallback(this, "OnChangeText");
            textView.onClick = new ActionCallback(self, "OnClk");
            isShow = false;
            selected = position;
            if (nextFocus != null)
                nextFocus.requestLayout();
            else
                self.clearFocus();

            OnChangeText();
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        public void OnChangeText()
        {
            if (list == null) return;
            inCalback = true;
            View view = getSelectedView();

            CEdit textView = (CEdit) view.findViewById(idEdit);
            if (callback != null)
            {
                Integer position = (int) getSelectedItemId();
                callback.run(position, textView.getText().toString());
            }
            inCalback = false;

        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void onNothingSelected(AdapterView<?> parent)
        {

        }


    }

}
