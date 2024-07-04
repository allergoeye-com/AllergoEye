package lib.common;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;

import androidx.appcompat.widget.AppCompatEditText;
import android.text.Editable;
import android.text.Selection;
import android.text.TextWatcher;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputConnectionWrapper;
import android.widget.EditText;


import com.allergoscope.app.R;

import java.util.ArrayList;

/**
 * Created by alex on 24.01.2018.
 */

public class CEdit extends AppCompatEditText implements IActionCalbackClient, IControlOnClick {

    Paint borderPaint = null;
    Paint borderSelectPaint = null;
    int fgColor;
    int fgSelectColor;
    Rect clipRect = null;

    int borderWidth;
    int borderRadius;
    boolean fPaint;
    boolean show_ui;

    public ActionCallback onClick = null;
    public ActionCallback onChar = null;
    public ActionCallback onSize = null;

    CTextChanger callback = new CTextChanger();
    //------------------------------------------
    //
    //------------------------------------------
    public CEdit(Context context) {
        super(context);
        InitAtribut(context, null, 0, 0);
    }
    @Override
    public boolean performClick()
    {
        return super.performClick();
    }
    //------------------------------------------
    //
    //------------------------------------------
    public CEdit(Context context, AttributeSet attrs) {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);

    }

    //------------------------------------------
    //
    //------------------------------------------
    public CEdit(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, 0, 0);

    }
    //-------------------------------------------
    //
    //-------------------------------------------
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.CEdit, defA, defS);
            try {
                fPaint  =  a.getBoolean(R.styleable.CEdit_border_draw, false);
                fgColor =  a.getColor(R.styleable.CEdit_border_color, 0);
                fgSelectColor =  a.getColor(R.styleable.CEdit_border_focus_color, 0);
                borderWidth =  a.getInt(R.styleable.CEdit_border_width, 4);
                borderRadius =  a.getInt(R.styleable.CEdit_border_radius, 8);
                show_ui  =  a.getBoolean(R.styleable.CEdit_show_ui, true);
                InitBorder();

            }

            catch(Exception e)
            {
                ;
            }
            a.recycle();
        }
        else
        {
            fPaint  =  false;
            fgColor =  0;
            fgSelectColor =  0;
            borderWidth =  4;
            borderRadius =  8;
            show_ui  =  true;

        }
        setImeOptions(EditorInfo.IME_ACTION_DONE  | (show_ui ? 0 : EditorInfo.IME_FLAG_NO_EXTRACT_UI));

    }
    //-------------------------------------------
    //
    //-------------------------------------------
    boolean InitBorder()
    {

        if (borderPaint == null)
        {
            borderPaint = new Paint();
            borderPaint.setStyle(Paint.Style.STROKE);
            borderPaint.setAntiAlias(true);
            borderPaint.setColor(fgColor);
            borderPaint.setStrokeWidth(borderWidth);
        }
        if (borderSelectPaint == null)
        {
            borderSelectPaint = new Paint();
            borderSelectPaint.setStyle(Paint.Style.STROKE);
            borderSelectPaint.setAntiAlias(true);
            borderSelectPaint.setColor(fgSelectColor);
            borderSelectPaint.setStrokeWidth(borderWidth);
        }

        return false;
    }
    //------------------------------------------
    //
    //------------------------------------------
   public void SetCallback(Object _object, String func)
   {
        if (callback.SetCallback(_object, func))
        {
            addTextChangedListener(callback);
        //    setOnEditorActionListener(callback);
        }

    }
    //-------------------------------------------
    //
    //-------------------------------------------
    public void OnCustomKeuboard_KeyDown(int primaryCode, int[] keyCodes)
    {
        if (onChar != null)
        {
            onChar.run(primaryCode);
        }
        /*
        Editable editable = getText();
        editable.clear();
        if ( primaryCode != Keyboard.KEYCODE_DELETE)
        editable.insert(0, Character.toString((char) primaryCode).toUpperCase());
        */
    }
    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public void Detach(ArrayList<Object> stack) {
        if (stack == null)
            stack = new ArrayList<Object>();

        if (!stack.contains(this))
        {
            stack.add(this);
            addTextChangedListener(null);
            setOnEditorActionListener(null);
            callback.Detach(stack);
        }

    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public void Detach() {
        Detach(null);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @SuppressLint("DrawAllocation")
    @Override
    protected void onDraw(Canvas canvas) {

        super.onDraw(canvas);
        if (borderPaint != null)
        {

            RectF rc = new RectF();
            rc.set(canvas.getClipBounds());

            rc.inset(borderWidth,borderWidth);

            canvas.drawRoundRect(rc, borderRadius,borderRadius, !isFocused() ? borderPaint : borderSelectPaint);
        }

    }
    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public void Init() {
      InitBorder();
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public boolean OnClick()
    {
        if (clipRect != null && !clipRect.contains(ScreenRect())) return false;
       if (onClick != null)
       {
            onClick.run();
            return true;
       }
       return false;

    }

    @Override
    public void setSelection(int start, int stop)
    {
        super.setSelection(start, stop);
    }

    @Override
    public void selectAll()
    {
        super.selectAll();
    }

    @Override
    public void extendSelection(int index)
    {
        super.extendSelection(index);
    }

    //-------------------------------------------------------------
    //
    //-------------------------------------------------------------
    @Override
    public boolean dispatchTouchEvent(MotionEvent ev)
    {
        int action = ev.getAction();
        if (onClick != null)
        {
            if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_UP) // || (action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_UP)
            {
                boolean b = OnClick();
            }

        }

        return onClick != null ? true : super.dispatchTouchEvent(ev);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    protected void onSizeChanged(int l, int t, int r, int b)
    {
        super.onSizeChanged(l, t, r, b);
        if (onSize != null)
            onSize.run(l, t, r, b);
    }
    //-------------------------------------------
    //
    //-------------------------------------------
    @Override
    public Rect ScreenRect() {
        int[] l = new int[2];
        getLocationOnScreen(l);
        Rect rect = new Rect(l[0], l[1], l[0] +  getWidth(),
                l[1] +  getHeight());
        return rect;
    }

    @Override
    public void SetClipRect(Rect _clipRect)
    {
        clipRect = _clipRect;
    }

    @Override
    public boolean IsEnableEvent() {
        return true;
    }

    private class EditTextInputConnection extends InputConnectionWrapper {

        public EditTextInputConnection(InputConnection target, boolean mutable) {
            super(target, mutable);
        }

        @Override
        public boolean commitText(CharSequence text, int newCursorPosition) {
            // some code which takes the input and manipulates it and calls editText.getText().replace() afterwards
            return true;
        }

    }
    /*
    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        InputConnection con = super.onCreateInputConnection(outAttrs);
        EditTextInputConnection connectionWrapper = new EditTextInputConnection(con, true);
        return connectionWrapper;
    }
    */
    //************************************************************
    class CTextChanger  extends ActionCallback implements TextWatcher {
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
        }

        @Override
        public void afterTextChanged(Editable s) {
            int start = getSelectionStart();
            int  end = getSelectionEnd();


            run();
            try {
            if (s != null && s.length() > 0)
            setSelection(start, end);
            }
            catch (Exception e)
            {
                ;
            }
        }

    }

}
