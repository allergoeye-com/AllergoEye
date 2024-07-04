package lib.common;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;

import androidx.annotation.Nullable;

import android.util.AttributeSet;

import lib.utils.AGlobals;

/**
 * Created by alex on 18.11.17.
 */

public class CheckRecycleImageView extends androidx.appcompat.widget.AppCompatImageView {
    public Rect rect = null;
    Bitmap mBitmap = null;
    public ActionCallback onFirstDraw = null;

    public CheckRecycleImageView(Context context)
    {
        super(context);
    }

    public CheckRecycleImageView(Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
    }

    public CheckRecycleImageView(Context context, @Nullable AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
    }

    public void Invalidate()
    {
        drawableStateChanged();
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas)
    {
        try
        {
            rect = canvas.getClipBounds();
            if (onFirstDraw != null)
            {
                onFirstDraw.run(rect);

            }
            Drawable dw = getDrawable();
            if (dw != null)
            {
                if (dw instanceof BitmapDrawable)
                {
                    Bitmap bitmap = ((BitmapDrawable) dw).getBitmap();
                    if (bitmap.isRecycled())
                        setImageBitmap(null);
                }
            }
        } catch (Exception e)
        {
            ;
        }
        super.onDraw(canvas);

    }

    @Override
    protected void onDetachedFromWindow()
    {
        super.onDetachedFromWindow();
        if (mBitmap != null)
        {
            mBitmap.recycle();
            mBitmap = null;
        }

    }

    /*   @Override
       protected void onDetachedFromWindow()  {
           Bitmap bitmap = ((BitmapDrawable)getDrawable()).getBitmap();
           if (bitmap != null && !bitmap.isRecycled() && !bitmap.equals(AGlobals.default_image))
               bitmap.recycle();
           setImageBitmap(null);
           super.onDetachedFromWindow();

       }

   */
 /*   @Override
    public void setImageBitmap(Bitmap bitmap)
    {
        if (mBitmap != null)
        {
            mBitmap.recycle();
            mBitmap = null;
        }
        Rect rc = new Rect();
        getDrawingRect(rc);
        float w = rc.width();
        float h = rc.height();
        if (bitmap != null && !bitmap.isRecycled() && w > 0f && h > 0f)
        {

            float aspect = (float)(float)bitmap.getWidth()/bitmap.getHeight();
            if (aspect > 1.0f)
                h = w/aspect;
            else
                w = h * aspect;
            mBitmap =  Bitmap.createScaledBitmap(bitmap, (int)(w), (int)(h), false);
            setImageBitmap(mBitmap);
            return;
        }
        super.setImageBitmap(bitmap);
    }
    */
    public void CleanUp()
    {
        try
        {
            Drawable dw = getDrawable();
            if (dw != null)
            {
                if (dw instanceof BitmapDrawable)
                {
                    Bitmap bitmap = ((BitmapDrawable) getDrawable()).getBitmap();
                    if (bitmap != null && !bitmap.isRecycled() && !bitmap.equals(AGlobals.default_image))
                        bitmap.recycle();
                }
            }
        } catch (Exception e)
        {
            ;
        }

        setImageBitmap(null);
    }
}
