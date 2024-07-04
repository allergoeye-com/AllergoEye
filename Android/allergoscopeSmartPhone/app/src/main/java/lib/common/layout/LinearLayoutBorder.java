package lib.common.layout;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Build;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.core.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.widget.LinearLayout;


import com.allergoscope.app.R;

import lib.common.ActionCallback;


public class LinearLayoutBorder   extends LinearLayout {
    Paint paint = null;
    int color;
    public boolean draw_cell;
    public ActionCallback CalbackOnLayout = null;
    public LinearLayoutBorder(Context context) {
        super(context);
        InitAtribut(context, null, 0, 0);
    }

    public LinearLayoutBorder(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);
    }

    public LinearLayoutBorder(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public LinearLayoutBorder(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        InitAtribut(context, attrs, defStyleAttr, defStyleRes);
    }
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.LinearLayoutBorder, defA, defS);
            try {
                color = a.getColor(R.styleable.LinearLayoutBorder_color_border, ContextCompat.getColor(context, R.color.btn_color_up));
                draw_cell = a.getBoolean(R.styleable.LinearLayoutBorder_cell_draw, false);

            }

            catch(Exception e)
            {
                ;
            }
            a.recycle();
        }
        else
        {
            draw_cell = false;
            color = ContextCompat.getColor(context, R.color.btn_color_up);
        }
        Init(context);
    }
    protected void Init(Context context)
    {

        paint = new Paint();
        paint.setColor(color);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(1);
        paint.setAntiAlias(true);
        setWillNotDraw(false);

    }
    @Override
    @SuppressLint("DrawAllocation")
    protected void onDraw(Canvas canvas) {
        Rect rc = canvas.getClipBounds() ;
        if (draw_cell)
        {
           /* int[] l = new int[2];
            getLocationOnScreen(l);
            Rect screenRect = new Rect();
            screenRect.left = l[0] + rc.left;
            screenRect.top = lmn[1] + rc.top;
            screenRect.right = rc.right + l[0];
            screenRect.bottom = rc.bottom + l[1];
*/
            for (int i = 0, len = getChildCount(); i < len; ++i)
            {
                View v = (View)getChildAt(i);
                if (v.getBottom() < rc.top || v.getTop() > rc.bottom) continue;
                Rect outline = new Rect( v.getLeft(), v.getTop(),
                        v.getRight(),v.getBottom());
                canvas.drawRect(outline, paint) ;

            }
        }
        else
        {
            rc.inset(1,1);
            canvas.drawRect(rc, paint) ;

        }

    }
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
