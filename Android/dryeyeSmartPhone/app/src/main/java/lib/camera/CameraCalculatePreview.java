package lib.camera;

import android.app.Activity;
import android.content.res.Configuration;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraCharacteristics;
import android.util.Log;
import android.util.Size;
import android.util.TypedValue;
import android.view.Surface;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;


import lib.common.CF_Size;
import lib.utils.AGlobals;

public class CameraCalculatePreview {
    private CCamera camera;
    static public CF_Size mPreviewSize = null;
    static public Rect mCropRect = null;
    static public CF_Size largest = null;
    static public Rect cropDisplay = null;
    static public Rect focusRect = null;
    static public CF_Size displaySize = null;
    static public boolean fPirke = true;
    public static int iPirke = 0;
    public static float scaleScrren = 1.0f;


    //--------------------------------------
    //
    //--------------------------------------
    public CameraCalculatePreview(CCamera _camera)
    {
        camera = _camera;
    }
    //--------------------------------------
    //
    //--------------------------------------
    public CF_Size CalcRatio(int width, int height)
    {
        displaySize = new CF_Size(width, height);
        if (camera.IsValid())
        {
        try {
        int rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
         CameraCharacteristics ch = camera.Characteristics();
        int mSensorOrientation = ch.get(CameraCharacteristics.SENSOR_ORIENTATION);
        boolean swappedDimensions = false;
        switch (rotation)
        {
            case Surface.ROTATION_0:
            case Surface.ROTATION_180:
                if (mSensorOrientation == 90 || mSensorOrientation == 270)
                    swappedDimensions = true;
                break;
            case Surface.ROTATION_90:
            case Surface.ROTATION_270:
                if (mSensorOrientation == 0 || mSensorOrientation == 180)
                    swappedDimensions = true;
                break;
        }

            int rotatedPreviewWidth = width;
            int rotatedPreviewHeight = height;
            if (swappedDimensions)
            {
                rotatedPreviewWidth = height;
                rotatedPreviewHeight = width;
            }
            int orientation = AGlobals.currentActivity.getResources().getConfiguration().orientation;
            mPreviewSize = new CF_Size(chooseOptimalSize(camera.StreamConfigurationMap().getOutputSizes(SurfaceTexture.class),
                    rotatedPreviewWidth, rotatedPreviewHeight, largest));
            int h1 = rotatedPreviewHeight / 2;
            float xscale = fPirke ? (largest.m_cy/ mPreviewSize.m_cy): (largest.m_cx/ mPreviewSize.m_cx);
            float top = (mPreviewSize.m_cy - height)/2f;
            float left = (mPreviewSize.m_cx - width)/2f;
            float px = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM, 1,
                     AGlobals.GetDisplayMetrcs());
            int w1 = (int)((width - px * 60.0f)/2);
            scaleScrren = 1.0f;

            if (fPirke)
            {
	            Point displaySize = new Point();
	            AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getSize(displaySize);
                scaleScrren = displaySize.y/mPreviewSize.m_cy;
                cropDisplay = new Rect(w1,10,width - w1, height - 10);
                if (left < 0)
                {

                    cropDisplay.left = Math.max((int)(-left), w1);
                    cropDisplay.left = (cropDisplay.left/2) * 2;
                    cropDisplay.right = width - cropDisplay.left;
                    left = 0;

                }
                if (top < 0)
                {
                    top -= 2;
                    cropDisplay.top = Math.max((int)(-top), 10);
                    cropDisplay.top = (cropDisplay.top/2) *2;
                    cropDisplay.bottom = height - cropDisplay.top;


                    top = 0;

                }
            }
            else
            {
                cropDisplay = new Rect(10,h1/2,width - 10, height - h1/2);

                if (left < 0)
                {
                    left -= 2;
                    cropDisplay.left = Math.max((int)(-left), 10);
                    cropDisplay.left = (cropDisplay.left/2) * 2;
                    cropDisplay.right = width - cropDisplay.left;
                    left = 0;

                }
                if (top < 0)
                {
                    top -= 2;
                    cropDisplay.top = Math.max((int)(-top), h1/2);
                    cropDisplay.top = (cropDisplay.top/2) *2;
                    cropDisplay.bottom = height - cropDisplay.top;


                    top = 0;

                }
            }

           mCropRect = new Rect((int)(xscale * (cropDisplay.left + left)), (int)(xscale * (cropDisplay.top + top)),
                    (int)(xscale * (cropDisplay.left + left + cropDisplay.width())), (int)(xscale * (cropDisplay.top + top + cropDisplay.height())));
            w1 = cropDisplay.width();
            iPirke =  -cropDisplay.left;
            cropDisplay.left = 0;
            cropDisplay.right = w1;

            return mPreviewSize;
        }catch (Exception e)
        {
            e.printStackTrace();
        }
        }
        return null;

    }
    //-------------------------------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------------------------------
    public Matrix MakeTransform(int w, int h)
    {
        if (camera.IsValid())
        {
        try {
            int rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
            Matrix matrix = new Matrix();
            RectF viewRect = new RectF(0, 0, w, h);
            RectF bufferRect = new RectF(0, 0, mPreviewSize.getHeight(), mPreviewSize.getWidth());
            float centerX = viewRect.centerX();
            float centerY = viewRect.centerY();
            if (true) //Surface.ROTATION_90 == rotation || Surface.ROTATION_270 == rotation)
            {
                bufferRect.offset(centerX - bufferRect.centerX(), centerY - bufferRect.centerY());
                matrix.setRectToRect(viewRect, bufferRect, Matrix.ScaleToFit.FILL);
                //float sh = mPreviewSize.getWidth() * largest.m_cy/largest.m_cx;
                //float sw = mPreviewSize.getWidth() * largest.m_cy/largest.m_cx;
                float scale = scaleScrren; //((float)largest.m_cx/ mPreviewSize.m_cx); //1.0f; //Math.max((float) h / mPreviewSize.getHeight(), (float) w / mPreviewSize.getWidth());
     			matrix.postScale(scale, scale, centerX, centerY);
                 if (Surface.ROTATION_90 == rotation || Surface.ROTATION_270 == rotation)
                    matrix.postRotate(90 * (rotation - 2), centerX, centerY);
//                else
  //                  matrix.postRotate(90 * rotation, centerX, centerY);
                matrix.postTranslate(iPirke, 0);
            }
            else if (Surface.ROTATION_180 == rotation)
                matrix.postRotate(180, centerX, centerY);
            else
                matrix.postRotate(0, centerX, centerY);

                return matrix;
        }catch (Exception e)
        {
            e.printStackTrace();
        }
        }
        return null;

    }

    private Size chooseOptimalSize(Size[] choices, int width,
                                          int height, CF_Size aspectRatio) {
        if ((width & 1) == 1)
            ++width;
        if ((height & 1) == 1)
            ++height;
        List<Size> allEnough = new ArrayList<Size>();
        List<Size> big2Enough = new ArrayList<Size>();
        List<Size> bigEnough = new ArrayList<Size>();
        float ratio = aspectRatio.m_cy / aspectRatio.m_cx;
        for (Size option : choices)
        {
            allEnough.add(option);
            if (option.getHeight() == (int)(option.getWidth() * ratio) && option.getWidth() >= width && option.getHeight() >= height)
                bigEnough.add(option);
            else
            if (option.getWidth() >= width && option.getHeight() >= height)
                big2Enough.add(option);

        }
        if (bigEnough.size() > 0)
            return Collections.max(bigEnough, new CompareSizesByArea());
        if (big2Enough.size() > 0)
            return Collections.min(big2Enough, new CompareSizesByArea());
        return Collections.max(allEnough, new CompareSizesByArea());


    }
    //-------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    public static class CompareSizesByArea implements Comparator<Size> {

        @Override
        public int compare(Size lhs, Size rhs) {
            return Long.signum((long) lhs.getWidth() * (long)lhs.getHeight() -
                    (long) rhs.getWidth() * (long)rhs.getHeight());
        }

    }

}
