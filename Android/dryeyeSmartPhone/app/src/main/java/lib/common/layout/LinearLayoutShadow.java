package lib.common.layout;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;

import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;

import android.util.AttributeSet;
import android.widget.LinearLayout;

import com.dryeye.app.R;


public class LinearLayoutShadow extends LinearLayout {
    Paint paint = null;
    int color;

    public LinearLayoutShadow(Context context)
    {
        super(context);
        InitAtribut(context, null, 0, 0);
    }

    public LinearLayoutShadow(Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);
    }

    public LinearLayoutShadow(Context context, @Nullable AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, defStyleAttr, 0);
    }

    protected void Init()
    {


        paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setShadowLayer(12, 0, 0, color);
        //paint.setColor(1);
        paint.setStyle(Paint.Style.STROKE);
        setLayerType(LAYER_TYPE_SOFTWARE, paint);
        setWillNotDraw(false);


    }

    //-------------------------------------------
    //
    //-------------------------------------------
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {
        if (attrs != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.LinearLayoutShadow, defA, defS);
            try
            {
                color = a.getColor(R.styleable.LinearLayoutShadow_shadow_color, ContextCompat.getColor(context, R.color.btn_color_up));
            } catch (Exception e)
            {
                ;
            }
            a.recycle();
        } else
        {
            color = ContextCompat.getColor(context, R.color.btn_color_up);
        }
        Init();
    }

    @Override
    @SuppressLint("DrawAllocation")
    protected void onDraw(Canvas canvas)
    {
        Rect rc = canvas.getClipBounds();
//        rc.inset(1,1);
        canvas.drawRect(rc, paint);

    }


}
