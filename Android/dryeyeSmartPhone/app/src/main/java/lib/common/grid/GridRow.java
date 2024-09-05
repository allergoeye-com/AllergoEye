package lib.common.grid;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;

import androidx.core.content.ContextCompat;

import android.util.Log;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.RadioGroup;

import com.dryeye.app.R;

import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.utils.AGlobals;
import lib.utils.FileUtils;

import static lib.common.grid.Cell.CELL_SPACE;

public class GridRow  extends RadioGroup {
    public Cell[] cells = null;
    Paint paint = null;
    ActionCallback calback = null;
    int param_calback;
    public boolean fDrawBorder = true;
    public ActionCallback CalbackOnLayout = null;
    public Rect rectScreen = null;
    //--------------------------------------------------
    //
    //--------------------------------------------------
    public GridRow(int iOrient, int iLayoutParamsWidth, int iLayoutParamsHeight, Context context,  Paint _paint, Cell... params)
    {
        super(context);
        initGridRow(iOrient, iLayoutParamsWidth, iLayoutParamsHeight, context,  _paint, params);
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public GridRow(int iLayoutParamsWidth, int iLayoutParamsHeight, Context context,  Paint _paint, Cell... params)
    {
        super(context);
        initGridRow(LinearLayout.HORIZONTAL, iLayoutParamsWidth, iLayoutParamsHeight, context,  _paint, params);
    }
    //--------------------------------------------------
    //
    //--------------------------------------------------
    private void initGridRow(int iOrient, int iLayoutParamsWidth, int iLayoutParamsHeight, Context context,  Paint _paint, Cell... params)
    {
        paint = _paint;
        setWillNotDraw(false);
        setOrientation(iOrient);
        setBackgroundColor(ContextCompat.getColor(context, R.color.theme_dlg_background));
        LinearLayout.LayoutParams lparams = new LinearLayout.LayoutParams(iLayoutParamsWidth, iLayoutParamsHeight);
        lparams.weight = 1;
        setLayoutParams(lparams);

        cells = params;
        if ( cells != null)
        {
            int n = params.length;
            for (int i = 0; i < params.length; ++i)
            {

                View v = params[i].Create(getContext(), this);
                if (v != null)
                {
                    params[i].SetLayout(n > 1 && iOrient  == LinearLayout.HORIZONTAL);
                    addView(v);

                }
            }
            for (int i = 0; i < params.length; ++i)
                params[i].SetRadioButtonData();
        }
    }

    public void SetCalback(ActionCallback calback, int param)
    {
        calback = calback;
        param_calback = param;
    }

    public void UpdateData()
    {
        try {
            if ( cells != null)
                for (int i = 0; i < cells.length; ++i)
                    cells[i].UpdateData();
        }
        catch (Exception e)
        {
            FileUtils.AddToLog(e);
        }

    }
    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void Reset()
    {
        if ( cells != null)
            for (int i = 0; i < cells.length; ++i)
                cells[i].Reset();

    }
    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void Clear()
    {

        if (cells != null)
        for (int i = 0; i < cells.length; ++i)
            removeView(cells[i].ctrl);
        cells = null;

    }
    public void Replace(Cell... params)
    {
        if (params == null)
        {
            Reset();
            Clear();
            return;
        }
        boolean fReplace = false;
        if (cells != null && params.length == cells.length)
        {
            fReplace = true;
            for (int i = 0; i < params.length; ++i)
                if (params[i].type !=  cells[i].type || params[i].prop !=  cells[i].prop)
                {
                    fReplace = false;
                    break;
                }
        }

        if (!fReplace)
        {
            Clear();
            cells = params;
            if ( params != null)
            {
                int n = params.length;
                for (int i = 0; i < params.length; ++i)
                {
                    View v = params[i].Create(getContext(), this);
                    if (v != null)
                    {
                        params[i].SetLayout(n > 1);

                        addView(v);
                    }
                }
            }
        }
        else
        {
            fReplace = false;
            int n = params.length;
            for (int i = 0; i < params.length; ++i)
            {
                try {
                fReplace = !cells[i].Replace(AGlobals.currentActivity, params[i]);
                if (fReplace)
                    break;
                }catch (Exception e)
                {
                    fReplace = true;
                }

            }
            if (fReplace)
            {
                Reset();
                Clear();
                Replace(params);
            }


        }
    }
    public void SetClipRect(Rect rc)
    {
        if (cells != null)
            for (int i = 0; i < cells.length; ++i)
                cells[i].SetClipRect(rc);

    }
    public Cell GetFocused()
    {
        if (cells != null)
            for (int i = 0; i < cells.length; ++i)
                if (cells[i].fIsFocused)
                    return cells[i];
        return null;
    }

    @Override
    @SuppressLint("DrawAllocation")
    protected void onDraw(Canvas canvas) {
        Rect r = canvas.getClipBounds() ;
        if (fDrawBorder && cells != null)
        {
            for (int i = 0; i < cells.length; ++i)
            {
                if (cells[i].ctrl != null)
                {
                    Rect outline = new Rect( cells[i].ctrl.getLeft(), 0,
                            cells[i].ctrl.getRight(),r.bottom);
                    if (cells[i].type != CELL_SPACE)
                    canvas.drawRect(outline, paint) ;
                }
            }
        }
        else
            super.onDraw(canvas);

    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        int action = event.getAction();
        if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_DOWN)
        {
            ;
        }
        else
        if (((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_CANCEL) ||
                ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_UP))
        {
            if (calback != null)
                (new CHandler()).Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        calback.run(param_calback);
                    }
                });

        }

        return calback != null;
    }
    @Override
    public void requestLayout()
    {

        super.requestLayout();
    }
    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        super.onLayout(changed, left, top, right, bottom);
        rectScreen = new Rect(left, top, right, bottom);

        if (CalbackOnLayout != null)
            CalbackOnLayout.run(changed, left, top, right, bottom);
    }
}