package lib.common.grid;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.core.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.widget.LinearLayout;

import com.dryeye.app.R;

import lib.common.ActionCallback;
import lib.common.layout.LinearLayoutBorder;

import lib.utils.AGlobals;
import lib.utils.FileUtils;

import java.util.ArrayList;

public class Grid  extends LinearLayoutBorder {

    Paint paint = null;
    ArrayList<GridRow> data = null;
    Rect clipRect = null;
    public int iLayoutParamsWidth = LayoutParams.MATCH_PARENT;
    public int iLayoutParamsHeight = LayoutParams.WRAP_CONTENT;
    public ActionCallback CalbackOnLayout = null;
    //--------------------------------------------------
    //
    //--------------------------------------------------
    public Grid(Context context) {
        super(context);
        Init();
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public Grid(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        Init();

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public Grid(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        Init();

    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public Grid(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        Init();
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void Init()
    {
        data = new ArrayList<GridRow>();
        paint = new Paint();
        paint.setColor(ContextCompat.getColor(getContext(), R.color.theme_dlg_text_color));
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(2);
    }
    public void SetClipRect(Rect rc)
    {
        clipRect = rc;
        UpdateClipRect();
    }
    public void UpdateClipRect()
    {
        for (int i = 0, l = data.size(); i < l; ++i)
            data.get(i).SetClipRect(clipRect);
    }

    public Cell GetFocused()
    {
        for (int i = 0, l = data.size(); i < l; ++i)
        {
            Cell c = data.get(i).GetFocused();
            if (c != null)
                return c;
        }
        return null;
    }
    //--------------------------------------------------
    //
    //--------------------------------------------------
    public GridRow AddRow(Cell... params)
    {
        GridRow row = new GridRow(getOrientation() == LinearLayout.HORIZONTAL ? LinearLayout.VERTICAL : LinearLayout.HORIZONTAL, iLayoutParamsWidth, iLayoutParamsHeight, AGlobals.currentActivity, paint, params);
        data.add(row);
        addView(row);
        UpdateClipRect();
        return row;

    }
    public GridRow ReplaceRow(int index, Cell... params)
    {
        GridRow row = null;
        if (data.size() > index)
        {
            row = data.get(index);
            row.Replace(params);
        }
        else
            row = AddRow(params);
        UpdateClipRect();
        return row;
    }
    public void SetRowColor(int index, int color)
    {
        if (data.size() > index)
        {
            GridRow row = data.get(index);
            row.setBackgroundColor(color);
        }
    }
    public GridRow GetRowColor(int index)
    {
        return data.size() > index ? data.get(index) : null;
    }

    public void SetVisible(int index, int v)
    {
        if (data.size() > index)
        {
            GridRow row = data.get(index);
            row.setVisibility(v);
        }
    }
    public void SetRowCalback(int index, ActionCallback callback)
    {
        if (data.size() > index)
        {
            GridRow row = data.get(index);
            row.SetCalback(callback, index);
        }
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void UpdateView()
    {
        requestLayout();
    }
    public int GetNumRow() { return data.size(); }
    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void UpdateData()
    {
        for (int i = 0, l = data.size(); i < l; ++i)
            data.get(i).UpdateData();
    }
    public int CountRow()
    {
        return data != null ? data.size() : 0;
    }
    //--------------------------------------------------
    //
    //--------------------------------------------------
    public GridRow GetGridRow(int i)
    {
        if (data != null && i < data.size())
        {
            data.get(i).UpdateData();
            return data.get(i);
        }
        return null;
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public Cell[] GetRow(int i)
    {
        if (data != null && i < data.size())
        {
            data.get(i).UpdateData();
            return data.get(i).cells;
        }
        return null;
    }
    public Cell GetCell(int i, int j)
    {
        if (data != null && i < data.size() && j < data.get(i).cells.length)
        {
            data.get(i).UpdateData();
            return data.get(i).cells[j];
        }
        return null;
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public int Size()
    {
        if (data != null)
            return data.size();
        return 0;

    }


    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void Clear()
    {
        if (data != null)
        {
            try {
                for (int i = 0, l = data.size(); i < l; ++i)
                {
                    data.get(i).Reset();
                    removeView(data.get(i));
                }
                data.clear();

            }
            catch(Exception e)
            {
                FileUtils.AddToLog(e);
            }
            data.clear();
        }
    }

    //--------------------------------------------------
    //
    //--------------------------------------------------
    public void ClearFrom(int i)
    {
        try {
/*            while(getChildCount() > i + 1)
            {
                View v = getChildAt(i);
                if (v instanceof ViewGroup)
                    RemoveGroup((ViewGroup)v);
                removeView(v);
            }
            */
            if (i < data.size())
                for (int j = i, l = data.size(); j < l; ++j)
                {
                    data.get(j).Reset();
                    data.get(j).Clear();
                    //removeView(data.get(j));
                }
            //while (data.size() > i)
              //  data.remove(i);

        }
        catch(Exception e)
        {
            FileUtils.AddToLog(e);
        }
    }
 /*   @Override
    protected void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);
    }
*/
     //-----------------------------------------------------
     //
     //-----------------------------------------------------
     @Override
     protected void onLayout(boolean changed, int left, int top, int right, int bottom)
     {
         super.onLayout(changed, left, top, right, bottom);
         if (CalbackOnLayout != null)
             CalbackOnLayout.run(changed, left, top, right, bottom);
     }
}
