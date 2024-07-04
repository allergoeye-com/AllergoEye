package com.allergoscope.app.timer;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;

import lib.common.button.CMenuButton;
import lib.utils.AGlobals;

public class TimerButton extends CMenuButton {
    String gText = null;

    public TimerButton(Context context)
    {
        super(context);
    }

    public TimerButton(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public TimerButton(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
    }
    public void SetText(String s)
    {
        gText = s;
        this.invalidate();
    }
    @Override
    public void draw(Canvas canvas)
    {
        if (gText == null)
           super.draw(canvas);
        else
        {
            Resources resources = AGlobals.currentActivity.getResources();
            float scale = resources.getDisplayMetrics().density;
            Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
            // text color - #3D3D3D
            paint.setColor(Color.rgb(255, 255, 255));
            // text size in pixels
            paint.setTextSize((int) (14 * scale));
            // text shadow
            paint.setShadowLayer(1f, 0f, 1f, Color.WHITE);
            Paint paint2 = new Paint(Paint.ANTI_ALIAS_FLAG);
            // text color - #3D3D3D
            paint2.setColor(Color.rgb(255, 255, 255));
            // text size in pixels
            // text shadow
            paint2.setShadowLayer(1f, 0f, 1f, Color.WHITE);
            paint2.setStyle(Paint.Style.STROKE);

            // draw text to the Canvas center
            Rect bounds = new Rect();
            paint.getTextBounds(gText, 0, gText.length(), bounds);

            int x = (canvas.getWidth())/2;
            int y = (canvas.getHeight())/2;
            canvas.drawCircle(x, y, Math.min(x, y), paint2);
            x = (canvas.getWidth() - bounds.width())/2;
            y = (canvas.getHeight() + bounds.height())/2;
            canvas.drawText(gText, x, y, paint);
        }
    }
}
