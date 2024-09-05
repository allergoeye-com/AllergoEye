package lib.camera;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.Region;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;


public class ViewRect extends View {

    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public ViewRect(Context context) {
        super(context);
        Init();

    }
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public ViewRect(Context context, AttributeSet attrs) {
        super(context, attrs);
        Init();
    }

    public ViewRect(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        Init();
    }


    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    public void Init() {
      /*  setOnLongClickListener(new View.OnLongClickListener()
        {
            @Override
            public boolean onLongClick(View v) {
                Toast.makeText(getContext(), "Long pressed", Toast.LENGTH_SHORT).show();
                return false;
            }
        });
*/
    }

    @Override
    protected void onDraw(Canvas canvas) {

        Rect rc  = canvas.getClipBounds();
        Paint  paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setStyle(Paint.Style.STROKE);
        paint.setColor(Color.GREEN);
        Paint  paint2 = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint2.setStyle(Paint.Style.STROKE);
        paint2.setColor(Color.RED);

        if (CameraCalculatePreview.cropDisplay != null)
        {
        canvas.drawRect(CameraCalculatePreview.cropDisplay,paint);
        }
        /*if (CameraCalculatePreview.focusRect != null)
        {
            canvas.drawRect(CameraCalculatePreview.focusRect,paint2);
        }
*/
        paint.setStyle(Paint.Style.FILL_AND_STROKE);
        paint.setColor(Color.BLACK);
        if (CameraCalculatePreview.cropDisplay != null)
        {
        canvas.clipRect(CameraCalculatePreview.cropDisplay, Region.Op.DIFFERENCE);
        }


        canvas.drawRect(rc,paint);



    }

}
