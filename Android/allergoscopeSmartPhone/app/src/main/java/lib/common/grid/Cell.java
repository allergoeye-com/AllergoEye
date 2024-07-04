package lib.common.grid;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;

import androidx.core.content.ContextCompat;
import androidx.core.graphics.drawable.DrawableCompat;

import android.text.InputType;
import android.util.TypedValue;
import android.view.ContextThemeWrapper;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.Space;
import android.widget.Switch;
import android.widget.TextView;


import com.allergoscope.app.R;

import java.util.ArrayList;

//import lib.colorpicker.ModalColorDialog;
import lib.common.ActionCallback;
import lib.common.CComboBox;
import lib.common.CEdit;
import lib.common.CListBox;
import lib.common.IControlOnClick;
import lib.common.ImageSlider;
import lib.common.button.CSpinButton;
import lib.common.button.CTextButton;
import lib.explorer.common.DirEntry;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

import static java.lang.String.format;

public class Cell implements View.OnFocusChangeListener {
    public final static int CELL_TEXT = 1;
    public final static int CELL_CHECKBOX = 2;
    public final static int CELL_SPIN = 3;
    public final static int CELL_COMBOBOX = 4;
    public final static int CELL_LISTBOX = 5;
    public final static int CELL_COLOR = 6;
    public final static int CELL_BUTTON = 8;
    public final static int CELL_SPACE = 9;
    public final static int CELL_IMAGE = 10;
    public final static int CELL_IMAGE_SLIDER = 11;
    public final static int CELL_RADIOBUTTON = 12;
    public final static int CELL_GRID = 13;
    LinearLayout.LayoutParams params = null;

    public final static int TEXT_EDIT = 1;
    public final static int TEXT_READONLY = 2;
    public final static int TEXT_INTEGER = 4;
    public final static int TEXT_DECIMAL = 8;
    public final static int BUTTON_TRANSPARENT = 16;
    public final static int TEXT_BOLD = 32;

    public final static int CELL_WIGHT = 64;
    public final static int TEXT_MOVING = 128;

    public boolean fIsFocused = false;
    public Integer iAligment = null;
    public int type;
    public int prop;
    public String[] text = null;
    public Bitmap[] bitmaps = null;
    public String[] bitmaps_path = null;
    public int extra = 0;
    public int value = 0;
    public View ctrl = null;
    public ActionCallback callback = null;
    public long c_object = 0;
    public int id = 0;
    public ArrayList<DirEntry> dirEntries = null;
    public Cell[][] param_grid = null;
    public Cell(int _type, Cell [][] _param_grid)
    {
        param_grid = _param_grid;
        type = _type;
        value = LinearLayout.VERTICAL;
    }
    public Cell(int _type, int orient, Cell [][] _param_grid)
    {
        param_grid = _param_grid;
        type = _type;
        value = orient;
    }
    public Grid GetGrid()
    {

        return ctrl != null && (ctrl instanceof Grid) ? (Grid)ctrl : null;
    }
    public Cell(ArrayList<DirEntry> dir, int index, ActionCallback c)
    {
        dirEntries = dir;
        value = index;
        callback = c;
        type = CELL_IMAGE_SLIDER;
        prop = 2;
    }

    public Cell(int _type, int _prop, String[] str, Bitmap[] bmp, String[] _bitmaps_path, int index, long _c_object, int _id)
    {
        prop = _prop;
        type = _type;
        text = str;
        bitmaps = bmp;
        bitmaps_path = _bitmaps_path;
        value = index;
        if (_id > 0 && _c_object != 0)
            SetDelegate(_c_object, _id);
        ctrl = null;
    }

    public View Control()
    {
        return ctrl;
    }

    //------------------------------------------
    //
    //------------------------------------------
    public Cell(int _type, int _prop, String[] str, int index)
    {
        prop = _prop;
        type = _type;
        text = str;
        value = index;
        ctrl = null;
    }

    public Cell(int _type, int _prop, String[] str, Bitmap[] bmp, int index)
    {
        prop = _prop;
        type = _type;
        text = str;
        bitmaps = bmp;
        value = index;
        ctrl = null;
    }

    //------------------------------------------
    //
    //------------------------------------------
    public Cell(int _type, int _prop, String str, int index)
    {
        prop = _prop;
        type = _type;
        text = new String[1];
        text[0] = str;
        value = index;
        ctrl = null;
    }

    //------------------------------------------
    //
    //------------------------------------------
    public Cell(int _type, Bitmap bmp)
    {
        prop = 0;
        type = _type;
        bitmaps = new Bitmap[1];
        bitmaps[0] = bmp;
        ctrl = null;
    }

    //------------------------------------------
    //
    //------------------------------------------
    public Cell(int _type, int _prop, String str)
    {
        value = 0;
        prop = _prop;
        type = _type;
        text = new String[1];
        text[0] = str;
        ctrl = null;
    }

    //------------------------------------------
    //
    //------------------------------------------
    public void SetDelegate(long _c_object, int _id)
    {
        c_object = _c_object;
        id = _id;
        if (id != 0 && c_object != 0)
            switch (type)
            {
                case CELL_RADIOBUTTON:
                case CELL_CHECKBOX:
                    callback = new ActionCallback(this, "OnCheck", boolean.class);
                    break;
                case CELL_COLOR:
                case CELL_BUTTON:
                    callback = new ActionCallback(this, "OnClick");
                    break;
                case CELL_COMBOBOX:
                case CELL_LISTBOX:
                    //               case CELL_STITCH_TYPE:
                    callback = new ActionCallback(this, "OnSelect", int.class);
            }
    }

    //------------------------------------------
    //
    //------------------------------------------
    public View Create(Context context, ViewGroup container)
    {
        try
        {
            switch (type)
            {
                case CELL_TEXT:
                    CreateText(context);
                    break;
                case CELL_CHECKBOX:
                    CreateCheckBox(context);
                    break;
                case CELL_RADIOBUTTON:
                    CreateRadioButton(context);
                    break;
                case CELL_SPIN:
                    CreateSpin(context, container);
                    break;
                case CELL_COMBOBOX:
                    CreateComboBox(context);
                    break;
                case CELL_COLOR:
                    CreateColor(context);
                    break;
                case CELL_LISTBOX:
                    CreateListBox(context);
                    break;
                case CELL_IMAGE_SLIDER:
                    CreateImageSlider(context);
                    break;
                case CELL_BUTTON:
                    CreateTextButton(context);
                    break;
                case CELL_SPACE:
                    CreateSpace(context);
                    break;
                case CELL_IMAGE:
                    CreateImageView(context);
                    break;
                case CELL_GRID:
                CreateGrid(context);
                    break;

            }
        } catch (Exception e)
        {
            ctrl = null;
        }

        return ctrl;
    }

    //------------------------------------------
    //
    //------------------------------------------
    public void Reset()
    {
        try
        {
            switch (type)
            {
                case CELL_COMBOBOX:
                    if (ctrl instanceof CComboBox)
                    {
                        CComboBox box = (CComboBox) ctrl;
                        box.setAdapter(null);
                    }

                    break;
                case CELL_LISTBOX:
                    if (ctrl instanceof CListBox)
                    {
                        CListBox box = (CListBox) ctrl;
                        box.setAdapter(null);
                    }
                    break;
                case CELL_IMAGE_SLIDER:
                {
                    if (ctrl instanceof ImageSlider)
                    {
                        ImageSlider box = (ImageSlider) ctrl;
                        box.Reset();
                    }
                }
                break;
                case CELL_GRID:
                    if (ctrl instanceof Grid)
                    {
                        Grid grid = (Grid)ctrl;
                        grid.Clear();
                    }
                    break;

            }


        } catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }

    }

    //------------------------------------------
    //
    //------------------------------------------
    public boolean Replace(Context context, Cell c)
    {
        prop = c.prop;
        type = c.type;
        text = c.text;
        bitmaps = c.bitmaps;
        value = c.value;
        if (c.id > 0 && c.c_object != 0)
            SetDelegate(c.c_object, c.id);
        try
        {
            switch (type)
            {
                case CELL_TEXT:
                    UpdateText(context);
                    break;
                case CELL_CHECKBOX:
                    UpdateCheckBox(context);
                    break;
                case CELL_RADIOBUTTON:
                    UpdateRadioButton(context);
                    break;
                case CELL_SPIN:
                    UpdateSpin(context);
                    break;
                case CELL_COMBOBOX:
                    UpdateComboBox(context);
                    break;
                case CELL_COLOR:
                    UpdateColor(context);
                    break;
                case CELL_LISTBOX:
                    UpdateListBox(context);
                    break;
                case CELL_IMAGE_SLIDER:
                    UpdateImageSlider(context);
                    break;
                case CELL_BUTTON:
                    UpdateTextButton(context);
                    break;


            }
        } catch (Exception e)
        {
            return false;
        }
        return true;

    }

    //------------------------------------------
    //
    //------------------------------------------
    private void UpdateText(Context context)
    {

        if ((prop & TEXT_EDIT) == TEXT_EDIT)
        {
            CEdit edit = (CEdit) ctrl;
            edit.setText(text[0]);
        } else
        {
            TextView edit = (TextView) ctrl;
            edit.setText(text[0]);
        }
    }

    //------------------------------------------
    //
    //------------------------------------------
    private void UpdateColor(Context context)
    {

        CTextButton txt = (CTextButton) ctrl;
        txt.setText(text[0]);
        txt.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
        txt.setBackgroundColor(value);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void UpdateComboBox(Context context)
    {
        CComboBox box = (CComboBox) ctrl;
        box.Init(text, value, callback);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void UpdateTextButton(Context context)
    {
        CTextButton box = (CTextButton) ctrl;
        box.Init(callback);
        box.setText(text[0]);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void UpdateImageSlider(Context context)
    {
        ImageSlider box = (ImageSlider) ctrl;
        box.Init(dirEntries, value, callback);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void UpdateListBox(Context context)
    {
        CListBox box = (CListBox) ctrl;
        box.Init(text, value, callback);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void UpdateCheckBox(Context context)
    {
        Switch c = (Switch) ctrl;
        c.setChecked((value & 1) == 1);
        c.setText(text[0]);
        c.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void UpdateRadioButton(Context context)
    {
        RadioButton c = (RadioButton)ctrl;
        c.setChecked((value & 1) == 1);
        c.setText(text[0]);
        c.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @SuppressLint("ResourceType")
    private void UpdateSpin(Context context)
    {
        CEdit edit = ctrl.findViewById(R.id.text);
        edit.setText(text[0]);

    }

    //Init(ActionCallback c)
    private void SetColor(int color)
    {

        CTextButton txt = (CTextButton) ctrl;
        AGlobals.SetTextColor(txt, color);
        ctrl = txt;

    }

    //------------------------------------------
    //
    //------------------------------------------
    public void OnChangeColor()
    {
        /*
        CTextButton txt = (CTextButton)ctrl;
        Drawable background = txt.getBackground();
        int c = ((ColorDrawable) background).getColor();
        ModalColorDialog dlg = new ModalColorDialog(c, SINGER_DIALOG_THEME);
        dlg.SetCurrent(c);
        if (dlg.DoModal() == IDOK)
        {
            SetColor(dlg.colorResult);
        }
*/
    }

    private void CreateSpace(Context context)
    {
        Space space = new Space(context);
        ViewGroup.LayoutParams params = null;
        ViewGroup.LayoutParams c = space.getLayoutParams();
        if (c != null)
        {
            params = c;
            params.height = value;
            params.width = ViewGroup.LayoutParams.MATCH_PARENT;
        } else
            params = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, value);
        space.setLayoutParams(params);
        ctrl = space;
    }

    private void CreateImageView(Context context)
    {
        ImageView view = new ImageView(context);
        ColorStateList csl = new ColorStateList(
                new int[][]{new int[]{android.R.attr.state_enabled, android.R.attr.state_selected},
                        new int[]{}}, new int[]{R.color.theme_dlg_text_color, R.color.theme_dlg_text_color});
        Drawable drawable = new BitmapDrawable(context.getResources(), bitmaps[0]);
        drawable = DrawableCompat.wrap(drawable);
        DrawableCompat.setTintList(drawable, csl);
        DrawableCompat.setTintMode(drawable, PorterDuff.Mode.SRC_IN);
        view.setImageDrawable(drawable);
        ctrl = view;
    }

    //------------------------------------------
    //
    //------------------------------------------
    private void CreateColor(Context context)
    {

        CTextButton txt = new CTextButton(context);
        ctrl = txt;
        txt.Init(new ActionCallback(this, "OnChangeColor"));
        txt.setTextAppearance(context, android.R.style.TextAppearance_Small);
        SetColor(value);

    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void CreateComboBox(Context context)
    {

        CComboBox box = new CComboBox(new ContextThemeWrapper(context, R.style.AppThemeSeek));
        box.SetColors(ContextCompat.getColor(context, R.color.theme_dlg_text_color), ContextCompat.getColor(context, R.color.theme_dlg_background));

        box.Init(text, value, callback);
        ctrl = box;
        box.setOnFocusChangeListener(this);
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void CreateTextButton(Context context)
    {
        CTextButton box = new CTextButton(context);
        box.Init(callback);
        box.setText(text[0]);
        box.setTextAppearance(context, android.R.style.TextAppearance_Medium);
        box.setAllCaps(false);
        box.setSingleLine(false);
        box.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
        value = 0;
        if ((prop & BUTTON_TRANSPARENT) == BUTTON_TRANSPARENT)
            box.setBackgroundColor(0);
       box.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v)
            {
                value = 1;
                if (callback != null)
                    callback.run();
            }
        });
        ctrl = box;
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void CreateRadioButton(Context context)
    {
        RadioButton c = new RadioButton(new ContextThemeWrapper(context, R.style.AppThemeSeek));
        c.setText(text[0]);
        c.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
        ctrl = c;

    }
    public void SetRadioButtonData()
    {
        if (type == CELL_RADIOBUTTON)
        {
            RadioButton c = (RadioButton)ctrl;
            c.setChecked((value & 1) == 1);
        }
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void CreateImageSlider(Context context)
    {
        ImageSlider box = (ImageSlider) LayoutInflater.from(context).inflate(R.layout.grid_cell_slider, null);
        box.Init(dirEntries, value, callback);

        ctrl = box;
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void CreateListBox(Context context)
    {
        CListBox box = new CListBox(context);
        box.Init(text, value, callback);
        ctrl = box;
    }

    //----------------------------------------------------
    //----------------------------------------------------
    //
    //----------------------------------------------------
    private void CreateCheckBox(Context context)
    {
        Switch c = new Switch(new ContextThemeWrapper(context, R.style.AppThemeSeek));
        c.setChecked((value & 1) == 1);
        c.setText(text[0]);
        c.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
        c.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                value = isChecked ? 1 : 0;
                if (callback != null)
                    callback.run(id, isChecked);
            }
        });


        ctrl = c;
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @SuppressLint("ResourceType")
    private void CreateSpin(Context context, ViewGroup container)
    {
        LayoutInflater inflater = LayoutInflater.from(context);
        ctrl = inflater.inflate(R.layout.ctrl_spin, container, false);
        CEdit edit = ctrl.findViewById(R.id.text);
        if ((prop & TEXT_DECIMAL) == TEXT_DECIMAL)
            edit.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL);
        else if ((prop & TEXT_INTEGER) == TEXT_INTEGER)
            edit.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_SIGNED);

        edit.setText(text[0]);
        edit.setTextAppearance(context, android.R.style.TextAppearance_Small);
        edit.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
        edit.clearFocus();
        edit.setOnFocusChangeListener(this);

        CSpinButton[] repeat = new CSpinButton[2];
        repeat[0] = (CSpinButton) ctrl.findViewById(R.id.plus);
        repeat[1] = (CSpinButton) ctrl.findViewById(R.id.minus);
        repeat[0].SetCallback(this, "OnSpinPlus");
        repeat[1].SetCallback(this, "OnSpinMinus");

    }
    private void CreateGrid(Context context)
    {

        Grid grid = new Grid(context);

        grid.setWillNotDraw(false);
        grid.setOrientation(value);
        grid.setBackgroundColor(ContextCompat.getColor(context, R.color.theme_dlg_background));
        LinearLayout.LayoutParams lparams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
        lparams.weight = 1;
        grid.setLayoutParams(lparams);
        if (this.param_grid != null)
        {
            for (Cell[] c:param_grid)
                grid.AddRow(c);
        }
        ctrl = grid;



    }
    public void requestLayout()
    {
        if (ctrl != null)
        {
           ctrl.requestLayout();
        }
    }

        //----------------------------------------------------
    //
    //----------------------------------------------------
    @SuppressLint("DefaultLocale")
    public void OnSpinPlus()
    {
        if (ctrl != null && type == CELL_SPIN)
        {
            CEdit c = ctrl.findViewById(R.id.text);
            String s = c.getText().toString();
            c.requestFocusFromTouch();
            if ((prop & TEXT_INTEGER) == TEXT_INTEGER)
            {

                int val = Integer.valueOf(s.trim());
                ++val;
                s = format("%d", val);
            } else
            {
                boolean bMetric = false; // CEOSDoc.GetSystemMetrics() ==  CEOSDoc.SYSTEM_METRIC;
                float val = AGlobals.StringToFloat(s.trim());
                if (bMetric)
                {
                    val += 0.1;
                    s = format("%.1f", val);
                } else
                {
                    val += 0.01;
                    s = format("%.2f", val);
                }
                int i = s.lastIndexOf(",");
                if (i != -1)
                    s = s.replace(',', '.');


            }
            c.setText(s);
        }
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    @SuppressLint("DefaultLocale")
    public void OnSpinMinus()
    {
        if (ctrl != null && type == CELL_SPIN)
        {
            CEdit c = ctrl.findViewById(R.id.text);
            c.requestFocusFromTouch();
            String s = c.getText().toString();
            if ((prop & TEXT_INTEGER) == TEXT_INTEGER)
            {

                int val = Integer.valueOf(s.trim());
                --val;
                s = format("%d", val);
            } else
            {
                boolean bMetric = false; //CEOSDoc.GetSystemMetrics() ==  CEOSDoc.SYSTEM_METRIC;
                float val = AGlobals.StringToFloat(s.trim());
                if (bMetric)
                {
                    val -= 0.1;
                    s = format("%.1f", val);
                } else
                {
                    val -= 0.01;
                    s = format("%.2f", val);
                }
                int i = s.lastIndexOf(",");
                if (i != -1)
                    s = s.replace(',', '.');


            }
            c.setText(s);
        }
    }

    //------------------------------------------
    //
    //------------------------------------------
    @SuppressLint("ResourceType")
    protected void CreateText(Context context)
    {

        if ((prop & TEXT_EDIT) == TEXT_EDIT)
        {
            CEdit edit = new CEdit(context);
            if ((prop & TEXT_DECIMAL) == TEXT_DECIMAL)
                edit.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL);
            else if ((prop & TEXT_INTEGER) == TEXT_INTEGER)
                edit.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_SIGNED);
            if (text != null)
                edit.setText(text[0]);
            if ((prop & TEXT_BOLD) == TEXT_BOLD)
                edit.setTextAppearance(context, android.R.style.TextAppearance_Medium);
            else
                edit.setTextAppearance(context, android.R.style.TextAppearance_Small);
            edit.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL);

            edit.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
            edit.setSingleLine(true);
            edit.clearFocus();
            ctrl = edit;
            // edit.setImeOptions(IME_ACTION_NEXT);
            //android:imeOptions="actionNext"
            if (iAligment != null)
                edit.setGravity(iAligment);
            edit.setOnFocusChangeListener(this);
        } else
        {
            TextView edit = new TextView(context);
            if ((prop & TEXT_DECIMAL) == TEXT_DECIMAL)
                edit.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL);
            else if ((prop & TEXT_INTEGER) == TEXT_INTEGER)
                edit.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_SIGNED);
            if (text != null)
                edit.setText(text[0]);
            edit.setTextColor(ContextCompat.getColor(context, R.color.theme_dlg_text_color));
            if ((prop & TEXT_BOLD) == TEXT_BOLD)
                edit.setTextAppearance(context, android.R.style.TextAppearance_Medium);
            else
                edit.setTextAppearance(context, android.R.style.TextAppearance_Small);
            edit.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL);

            ctrl = edit;
            if (iAligment != null)
                edit.setGravity(iAligment);

            edit.setOnFocusChangeListener(this);
            edit.setSingleLine(true);
        }

    }

    //------------------------------------------
    //
    //------------------------------------------
    public void SetClipRect(Rect b)
    {
        if (type == CELL_SPIN)
        {
            ((CSpinButton) ctrl.findViewById(R.id.plus)).SetClipRect(b);
            ((CSpinButton) ctrl.findViewById(R.id.minus)).SetClipRect(b);
            ((CEdit) ctrl.findViewById(R.id.text)).SetClipRect(b);

        } else if (ctrl instanceof IControlOnClick)
            ((IControlOnClick) ctrl).SetClipRect(b);

    }

    public void SetLayout(boolean b)
    {
        if (ctrl == null || type == CELL_SPACE) return;
        try
        {
            ViewGroup.LayoutParams c = ctrl.getLayoutParams();
            if (c != null)
                params = (LinearLayout.LayoutParams) c;
            else
                params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);

            params.height = LinearLayout.LayoutParams.WRAP_CONTENT;
            if (type == CELL_LISTBOX)
                params.height = 150;
            if (b)
            {
                params.width = 0;
                params.weight = 1;

            } else
            {
                params.width = LinearLayout.LayoutParams.MATCH_PARENT;
                params.weight = 0;
            }
            params.gravity = Gravity.CENTER_VERTICAL;
            Resources r = AGlobals.currentActivity.getResources();
            int px = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 5, r.getDisplayMetrics());
            params.setMargins(params.getMarginStart(), px, params.getMarginEnd(), px);

            ctrl.setLayoutParams(params);
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);

        }
    }
    public boolean SetLayout()
    {
        if (params != null)
            ctrl.setLayoutParams(params);

        return params != null;

    }

    public void SetLayout(int w, int h, int weight)
    {
        if (ctrl == null || type == CELL_SPACE) return;
        try
        {
            LinearLayout.LayoutParams params = null;
            ViewGroup.LayoutParams c = ctrl.getLayoutParams();
            if (c != null)
                params = (LinearLayout.LayoutParams) c;
            else
                params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            params.width = w;
            params.height = h; //LinearLayout.LayoutParams.WRAP_CONTENT;
            params.weight = weight;
            ctrl.setLayoutParams(params);
        } catch (Exception e)
        {
            FileUtils.AddToLog(e);

        }
    }

    //------------------------------------------
    //
    //------------------------------------------
    public void UpdateData()
    {
        if (ctrl == null) return;
        try
        {
            switch (type)
            {
                case CELL_BUTTON:
                {
                    CTextButton edit = (CTextButton) ctrl;
                    text[0] = edit.getText().toString();

                }
                break;
                case CELL_TEXT:
                    GetText();
                    break;
                case CELL_CHECKBOX:
                    GetCheckBox();
                    break;
                case CELL_RADIOBUTTON:
                    GetRadioButton();
                    break;
                case CELL_SPIN:
                    GetSpin();
                    break;
                case CELL_COMBOBOX:
                    GetComboBox();
                    break;
                case CELL_COLOR:
                    GetColor();
                    break;
                case CELL_LISTBOX:
                    GetListBox();
                    break;
            }
        } catch (Exception e)
        {
            ctrl = null;
        }
    }

    //------------------------------------------
    //
    //------------------------------------------
    private void GetColor()
    {
        TextView txt = (TextView) ctrl;
        Drawable background = txt.getBackground();
        if (background instanceof ColorDrawable)
            value = ((ColorDrawable) background).getColor();
        text[0] = txt.getText().toString();
    }

    //------------------------------------------
    //
    //------------------------------------------
    private void GetComboBox()
    {
        CComboBox box = (CComboBox) ctrl;
        value = box.GetSel();

    }

    //------------------------------------------
    //
    //------------------------------------------
    private void GetListBox()
    {
        CListBox box = (CListBox) ctrl;
        value = box.GetSel();

    }

    //------------------------------------------
    //
    //------------------------------------------
    private void GetText()
    {

        if ((prop & TEXT_EDIT) == TEXT_EDIT)
        {
            CEdit edit = (CEdit) ctrl;
            text[0] = edit.getText().toString();
        } else
        {
            TextView edit = (TextView) ctrl;
            text[0] = edit.getText().toString();
        }

    }

    //------------------------------------------
    //
    //------------------------------------------
    private void GetSpin()
    {

        CEdit edit = ctrl.findViewById(R.id.text);
        text[0] = edit.getText().toString();


    }

    //------------------------------------------
    //
    //------------------------------------------
    private void GetCheckBox()
    {
        Switch c = (Switch) ctrl;
        value = c.isChecked() ? 1 : 0;

    }
    //------------------------------------------
    //
    //------------------------------------------
    private void GetRadioButton()
    {
        RadioButton c = (RadioButton) ctrl;
        value = c.isChecked() ? 1 : 0;
    }
    //------------------------------------------
    //
    //------------------------------------------
    public void OnClick()
    {
    }

    //------------------------------------------
    //
    //------------------------------------------
    public void OnSelect(int _i)
    {
    }

    //------------------------------------------
    //
    //------------------------------------------
    public void OnCheck(boolean b)
    {
    }

    @Override
    public void onFocusChange(View v, boolean hasFocus)
    {
        fIsFocused = hasFocus;
    }
}
