package lib.camera;


import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;

import androidx.annotation.NonNull;

import android.os.Build;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.TextureView;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.Toast;

import com.dryeye.app.MainActivity;
import com.dryeye.app.camera.FragmentCamera;

import java.io.IOException;


import lib.common.ActionCallback;
import lib.common.CF_Size;
import lib.common.CHandler;
import lib.common.ScaleListener;
import lib.utils.AGlobals;
import lib.utils.BitmapUtils;

import static android.hardware.camera2.CameraMetadata.LENS_FACING_BACK;
import static android.hardware.camera2.CameraMetadata.LENS_FACING_FRONT;
import static lib.camera.CameraCalculatePreview.cropDisplay;
import static lib.camera.CameraCalculatePreview.focusRect;

public class CCameraView extends TextureView implements TextureView.SurfaceTextureListener, View.OnTouchListener {
    public CCamera camera = null;
    Activity activity;
    CameraCalculatePreview calc;
    CF_Size mRatio;
    boolean  isLongClick = false;
    private ScaleListener.SCALE scale = new ScaleListener.SCALE();
    private Float lastScaleFactor = null;
    private ScaleGestureDetector mScaleDetector;
    TextureView self;
    public View fromUpdate = null;
    int rotation;
    public ActionCallback callbackOnScale = null;
    public ActionCallback callbackOnPreviewOk = null;
    public FragmentCamera wnd;
    Bitmap frame = null;
    //-------------------------------------
    //
    //-------------------------------------
    public CCameraView(Context context)
    {

        super(context);
        focusRect = null;
        Init(context);

    }
    //-------------------------------------
    //
    //-------------------------------------
    public CCameraView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Init(context);

    }
    //-------------------------------------
    //
    //-------------------------------------
    public CCamera Camera () { return camera; }
    //-------------------------------------
    //
    //-------------------------------------
    public CCameraView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        Init(context);

    }
    //-------------------------------------
    //
    //-------------------------------------
    public CCameraView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        Init(context);

    }
    private void Init(Context context)
    {
        self = this;
        lastScaleFactor = MainActivity.ReadIniFloat("CAMERA_ZOOM");
        if (lastScaleFactor == null)
        {
            lastScaleFactor = 1.0f;
            MainActivity.WriteIni("CAMERA_ZOOM", lastScaleFactor);
        }
        activity = (Activity) context;
        camera = new CCamera();
        calc = new CameraCalculatePreview(camera);
        setSurfaceTextureListener(this);

        mScaleDetector = new ScaleGestureDetector(getContext(), new ScaleListener(scale));
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
            mScaleDetector.setStylusScaleEnabled(false);
        }
        setDrawingCacheEnabled(true);

        setOpaque(false);

    }
    public Bitmap GetCurrent()
    {
        /*
        Bitmap bitmap = null;
        buildDrawingCache();
        Bitmap b = getDrawingCache();
        if (b != null)
        {
            bitmap = Bitmap.createBitmap(b);
            destroyDrawingCache();
        }
        else
            setDrawingCacheEnabled(true);

        return bitmap;

         */
        return frame;
    }
    //-------------------------------------
    //
    //-------------------------------------
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);
        if (mRatio != null)
        {
              setMeasuredDimension((int)(mRatio.m_cx), (int)(mRatio.m_cy));
              if (rotation != AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation())
              {
                  mRatio = calc.CalcRatio(width, height);
                  setTranslationX(-(int)((mRatio.m_cx - width)/2f));
                  setTranslationY(-(int)((mRatio.m_cy - height)/2f));
                   rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
                  setTransform(calc.MakeTransform(width, height));
                  if (frame != null)
                      frame.recycle();
                  frame = null;
              }
            if (callbackOnScale != null)
                callbackOnScale.run(lastScaleFactor);

        }

    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void ChangeOrient()
    {
        lastScaleFactor = MainActivity.ReadIniFloat("CAMERA_ZOOM");
    final float scaleLast = lastScaleFactor;
        Scale(lastScaleFactor + 0.1f);
        setOnTouchListener(null);
        SurfaceTexture surface = camera.preview;
        try {
            camera.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
       camera.WriteOrient(camera.LENS_FACING == LENS_FACING_FRONT ? LENS_FACING_BACK : LENS_FACING_FRONT);
        if (camera.Init(surface))
        {
            int width = calc.displaySize.getWidth();
            int height = calc.displaySize.getHeight();
            mRatio = calc.CalcRatio(width, height);
            surface.setDefaultBufferSize((int)mRatio.m_cx, (int)mRatio.m_cy);
            setTranslationX(-(int)((mRatio.m_cx - width)/2f));
            setTranslationY(-(int)((mRatio.m_cy - height)/2f));
            rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
            setTransform(calc.MakeTransform(width, height));

            camera.OpenCamera(callbackOnPreviewOk);
            scale.start();
            setOnTouchListener(this);
            (new CHandler()).Send(new Runnable() {
                @Override
                public void run()
                {
                    lastScaleFactor = scaleLast;
                    Scale(scaleLast);
                    requestLayout();

                }
            }, 100);
        }

    }
    public void Restart()
    {

        setOnTouchListener(null);
        SurfaceTexture surface = getSurfaceTexture();
        try {
            camera.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
        camera = new CCamera();
        calc = new CameraCalculatePreview(camera);
        if (camera.Init(surface))
        {
            lastScaleFactor = null;

            int width = calc.displaySize.getWidth();
            int height = calc.displaySize.getHeight();
            mRatio = calc.CalcRatio(width, height);
            surface.setDefaultBufferSize((int)mRatio.m_cx, (int)mRatio.m_cy);
            setTranslationX(-(int)((mRatio.m_cx - width)/2f));
            setTranslationY(-(int)((mRatio.m_cy - height)/2f));
            rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
            setTransform(calc.MakeTransform(width, height));
            camera.OpenCamera(callbackOnPreviewOk);
            scale.start();

            setOnTouchListener(this);

            (new CHandler()).Send(new Runnable() {
                                      @Override
                                      public void run()
                                      {
                                          requestLayout();

                                      }
                                  }, 1000);

        }

    }

    //-------------------------------------
    //
    //-------------------------------------
    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height)
    {
        try {
            if (camera.Init(surface))
            {

                mRatio = calc.CalcRatio(width, height);
                surface.setDefaultBufferSize((int)mRatio.m_cx, (int)mRatio.m_cy);
                setTranslationX(-(int)((mRatio.m_cx - width)/2f));
                setTranslationY(-(int)((mRatio.m_cy - height)/2f));
                rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
                setTransform(calc.MakeTransform(width, height));
                AGlobals.rootHandler.Send(new Runnable() {
                                              @Override
                                              public void run() {
                                                  self.requestLayout();
                                                  camera.OpenCamera(callbackOnPreviewOk);

                                              }
                                          });
                setOnTouchListener(this);
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
    //-------------------------------------
    //
    //-------------------------------------
    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height)
    {
        try{
            setTransform(calc.MakeTransform(width, height));

        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

    }

    //-------------------------------------
    //
    //-------------------------------------
    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface)
    {
        try {
            camera.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    //-------------------------------------
    //
    //-------------------------------------
    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture mSurface) {

      if (fromUpdate != null)
          fromUpdate.invalidate();
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void Scale(float f)
    {
        lastScaleFactor = camera.Scale(f);
        MainActivity.WriteIni("CAMERA_ZOOM", lastScaleFactor);
        if (callbackOnScale != null)
            callbackOnScale.run(lastScaleFactor);
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public float GetScale()
    {
        return lastScaleFactor;
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    boolean fTest = false;
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        final Handler handler = new Handler();
        final float x = event.getX(0);
        final float y = event.getY(0);

        Runnable mLongPressed = new Runnable() {
            public void run() {
            if (fTest)
            {
                focusRect = new Rect();
                focusRect.left = (int) (x - 100.0f);
                focusRect.top = (int) (y - 100.0f);
                focusRect.right = focusRect.left + 200;
                focusRect.bottom = focusRect.top + 200;
                callbackOnScale.run(lastScaleFactor);
            }
            }
        };
        if (camera.IsOpenedSession())
        {
     try {
         if (event.getPointerCount() > 1)
         {
             focusRect = null;
             fTest = false;
             handler.removeCallbacks(mLongPressed);


             mScaleDetector.onTouchEvent(event);
                int action = event.getAction();
                if ((action & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_MOVE)
                {
                    if (scale.IsScale())
                    {
                        Scale(lastScaleFactor * scale.GetScale());
                    }
                }
         }
         else
         {



             if (CameraCalculatePreview.cropDisplay.contains((int) event.getX(0),  (int) event.getY(0)))
             {
                wnd.onFoto();
             }
             if(event.getAction() == MotionEvent.ACTION_DOWN)
             {
                 fTest = true;
                 handler.postDelayed(mLongPressed, ViewConfiguration.getLongPressTimeout());

             }
             else
             if(event.getAction() == MotionEvent.ACTION_UP)
                 fTest = false;
             (new CHandler()).Send(new Runnable() {
                 @Override
                 public void run() {
                     camera.IdleFocus();
                 }
             });
         }
     }
     catch(Exception e)
     {
         e.printStackTrace();
     }
        }
        return true;

    }
    @Override
    public boolean performLongClick() {
        isLongClick = true;
        return super.performLongClick();
    }


}
