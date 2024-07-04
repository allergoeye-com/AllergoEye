package lib.common;

import android.app.Activity;
import android.content.Context;
import android.inputmethodservice.Keyboard;
import android.text.InputType;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.inputmethodservice.KeyboardView;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.allergoscope.app.R;

import lib.utils.AGlobals;

/**
 * Created by alex on 14.02.2018.
 */

public class CKeyboardView extends KeyboardView {

    //-------------------------------------------
    //
    //-------------------------------------------
    public CKeyboardView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        Init(context);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    public CKeyboardView(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        Init(context);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    public void Init(Context context)
    {
        setKeyboard(new Keyboard(context, R.xml.querty));
        if (!isInEditMode())
        {
            setPreviewEnabled(false);
            setOnKeyboardActionListener(new CKeyboardListner());
            AGlobals.currentActivity.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
        }
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    public void ShowFrom(View v)
    {
        setVisibility(View.VISIBLE);
        setEnabled(true);
        if (v != null)
            ((InputMethodManager) AGlobals.currentActivity.getSystemService(Activity.INPUT_METHOD_SERVICE)).hideSoftInputFromWindow(v.getWindowToken(), 0);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    public void Hide()
    {
        setVisibility(View.GONE);
        setEnabled(false);
    }

    //-------------------------------------------
    //
    //-------------------------------------------
    public void Register(CEdit edittext)
    {
//        edittext.setOnFocusChangeListener(new CFocusChangeListener());
        //      edittext.setOnClickListener(new CClickListener());
        edittext.setOnTouchListener(new CTouchListener());
        edittext.setInputType(edittext.getInputType() | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
    }

    //****************************************************************************
    class CTouchListener implements OnTouchListener {
        @Override
        public boolean onTouch(View v, MotionEvent event)
        {
            CEdit edittext = (CEdit) v;
            int inType = edittext.getInputType();
            edittext.setInputType(InputType.TYPE_NULL);
            edittext.onTouchEvent(event);
            edittext.setInputType(inType);
            return true;
        }
    }

    //***************************************************************************
    class CClickListener implements OnClickListener {
        @Override
        public void onClick(View v)
        {
            ShowFrom(v);
        }
    }

    //***************************************************************************
    class CFocusChangeListener implements OnFocusChangeListener {
        @Override
        public void onFocusChange(View v, boolean hasFocus)
        {
            if (hasFocus)
                ShowFrom(v);
            else
                Hide();
        }
    }

    //****************************************************************************
    class CKeyboardListner implements KeyboardView.OnKeyboardActionListener {
        @Override
        public void onPress(int primaryCode)
        {

        }

        @Override
        public void onRelease(int primaryCode)
        {

        }

        @Override
        public void onKey(int primaryCode, int[] keyCodes)
        {
            View focusCurrent = AGlobals.currentActivity.getWindow().getCurrentFocus();
            if (focusCurrent == null || !(focusCurrent instanceof CEdit)) return;
            ((CEdit) focusCurrent).OnCustomKeuboard_KeyDown(primaryCode, keyCodes);

        }

        @Override
        public void onText(CharSequence text)
        {

        }

        @Override
        public void swipeLeft()
        {

        }

        @Override
        public void swipeRight()
        {

        }

        @Override
        public void swipeDown()
        {

        }

        @Override
        public void swipeUp()
        {

        }
    }

}
