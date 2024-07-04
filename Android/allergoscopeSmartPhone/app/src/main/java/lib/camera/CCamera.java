package lib.camera;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.MeteringRectangle;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Build;
import android.os.Handler;
import android.util.Log;
import android.util.Range;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.Surface;
import android.widget.Toast;


import com.allergoscope.app.App;
import com.allergoscope.app.MainActivity;

import java.io.Closeable;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Set;

import lib.common.ActionCallback;
import lib.common.CF_Size;
import lib.common.CHandler;
import lib.common.EventIdle;
import lib.common.grid.CCell;
import lib.utils.AGlobals;
import lib.utils.ASound;


import static android.hardware.camera2.CameraCharacteristics.STATISTICS_INFO_AVAILABLE_LENS_SHADING_MAP_MODES;
import static android.hardware.camera2.CameraMetadata.CONTROL_AF_TRIGGER_CANCEL;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_AUTO;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_CLOUDY_DAYLIGHT;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_DAYLIGHT;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_FLUORESCENT;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_INCANDESCENT;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_OFF;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_SHADE;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_TWILIGHT;
import static android.hardware.camera2.CameraMetadata.CONTROL_AWB_MODE_WARM_FLUORESCENT;
import static android.hardware.camera2.CameraMetadata.CONTROL_EXTENDED_SCENE_MODE_BOKEH_CONTINUOUS;
import static android.hardware.camera2.CameraMetadata.CONTROL_EXTENDED_SCENE_MODE_BOKEH_STILL_CAPTURE;
import static android.hardware.camera2.CameraMetadata.CONTROL_EXTENDED_SCENE_MODE_DISABLED;
import static android.hardware.camera2.CameraMetadata.CONTROL_MODE_AUTO;
import static android.hardware.camera2.CameraMetadata.CONTROL_MODE_OFF;
import static android.hardware.camera2.CameraMetadata.CONTROL_MODE_OFF_KEEP_STATE;
import static android.hardware.camera2.CameraMetadata.CONTROL_MODE_USE_SCENE_MODE;
import static android.hardware.camera2.CameraMetadata.DISTORTION_CORRECTION_MODE_FAST;
import static android.hardware.camera2.CameraMetadata.DISTORTION_CORRECTION_MODE_HIGH_QUALITY;
import static android.hardware.camera2.CameraMetadata.DISTORTION_CORRECTION_MODE_OFF;
import static android.hardware.camera2.CameraMetadata.HOT_PIXEL_MODE_FAST;
import static android.hardware.camera2.CameraMetadata.HOT_PIXEL_MODE_HIGH_QUALITY;
import static android.hardware.camera2.CameraMetadata.HOT_PIXEL_MODE_OFF;
import static android.hardware.camera2.CameraMetadata.INFO_SUPPORTED_HARDWARE_LEVEL_FULL;
import static android.hardware.camera2.CameraMetadata.LENS_FACING_BACK;
import static android.hardware.camera2.CameraMetadata.LENS_OPTICAL_STABILIZATION_MODE_OFF;
import static android.hardware.camera2.CameraMetadata.LENS_OPTICAL_STABILIZATION_MODE_ON;
import static android.hardware.camera2.CameraMetadata.NOISE_REDUCTION_MODE_FAST;
import static android.hardware.camera2.CameraMetadata.NOISE_REDUCTION_MODE_MINIMAL;
import static android.hardware.camera2.CameraMetadata.NOISE_REDUCTION_MODE_OFF;
import static android.hardware.camera2.CameraMetadata.NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG;
import static android.hardware.camera2.CameraMetadata.SHADING_MODE_FAST;
import static android.hardware.camera2.CameraMetadata.SHADING_MODE_HIGH_QUALITY;
import static android.hardware.camera2.CameraMetadata.SHADING_MODE_OFF;
import static android.hardware.camera2.CameraMetadata.STATISTICS_LENS_SHADING_MAP_MODE_OFF;
import static android.hardware.camera2.CameraMetadata.STATISTICS_LENS_SHADING_MAP_MODE_ON;
import static android.hardware.camera2.CameraMetadata.TONEMAP_MODE_PRESET_CURVE;
import static lib.camera.CameraCalculatePreview.cropDisplay;
import static lib.camera.CameraCalculatePreview.fPirke;
import static lib.camera.CameraCalculatePreview.focusRect;
import static lib.camera.CameraCalculatePreview.largest;
import static lib.camera.CameraCalculatePreview.mPreviewSize;
import static lib.camera.CameraCalculatePreview.scaleScrren;
import static lib.common.grid.CCell.CELL_RADIOBUTTON;
import static lib.common.grid.CCell.TEXT_READONLY;
import static lib.utils.FileUtils.AddToLog;

import androidx.annotation.NonNull;


public class CCamera implements Closeable {
    public static final int  CAMERA_IDLE = 1;
    public static final int  CAMERA_SCAN = 2;
    public static final int  CAMERA_SCAN_OK = 3;
    public static final int  CAMERA_FOCUSED = 4;
    public static final int  CAMERA_CANCELED = 5;
    float fLastZoom = 1.0f;
    int proba_new_foto = 0;
    public class MapModeKey {
        public CCell [] key;
        public String str;
        MapModeKey (CCell [] _key, String _str)
        {
            key = _key;
            str = _str;
        }
    }

    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();
    public static CCamera self = null;
    public static int LENS_FACING = CameraCharacteristics.LENS_FACING_FRONT; //.LENS_FACING_BACK;

    public static Rect clipRect = new Rect(0, 0, 0, 0);
    boolean fStartFoto = false;
    boolean fWaitFoto = false;

    static
    {
        ORIENTATIONS.append(Surface.ROTATION_0, 90);
        ORIENTATIONS.append(Surface.ROTATION_90, 0);
        ORIENTATIONS.append(Surface.ROTATION_180, 270);
        ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }
    ActionCallback callbackOnPreviewOk = null;
    ActionCallback callbackOnInitPreview = null;

    public ActionCallback onexit = null;
    public ActionCallback onErrorFoto = null;
    public CameraManager mCameraManager;
    public String cameraId;
    public CameraDevice device;

    public int imageFormat = ImageFormat.JPEG;
    public CaptureRequest.Builder previewBuilder;
    public CameraCaptureSession session;
    public EventIdle wait;
    private ImageReader mImageReader;
    public SurfaceTexture preview;
    private Boolean fAutoFlash;
    Rect lastCrop = null;
    CFotoCallback fotoCallback = null;
    CaptureRequest mPreviewRequest = null;
    boolean isMeteringAreaAFSupported = false;
    boolean fSizeRange;
    CPreviewCallback previewCallback = null;
     Handler handler;
   HashMap<Object, Object > cModes = new HashMap<Object, Object >();;
    CaptureRequest.Builder captureBuilder = null;


    CCamera()
    {
        preview = null;
        previewBuilder = null;
        session = null;
        wait = null;
        device = null;
        cameraId = null;
        mCameraManager = null;
        String or = MainActivity.ReadIni("CAMERA_ORIENT");
        if (or == null || or.length() <= 1)
        {
            MainActivity.WriteIni("CAMERA_ORIENT", "LENS_FACING_BACK");
        }

    }
    public void WriteOrient(int or)
    {
        if (or == LENS_FACING_BACK)
            MainActivity.WriteIni("CAMERA_ORIENT", "LENS_FACING_BACK");
        else
            MainActivity.WriteIni("CAMERA_ORIENT", "LENS_FACING_FRONT");

    }
    CameraCharacteristics Characteristics() throws CameraAccessException
    {
        mCameraManager = (CameraManager) AGlobals.contextApp.getSystemService(Context.CAMERA_SERVICE);

        return  mCameraManager.getCameraCharacteristics(cameraId);
    }
    CameraCharacteristics Characteristics(String id) throws CameraAccessException
    {



        return  mCameraManager.getCameraCharacteristics(id);
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public StreamConfigurationMap StreamConfigurationMap() throws CameraAccessException
    {
            CameraCharacteristics c = Characteristics();
            return c != null ? c.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP) : null;
    }

    public boolean Init(SurfaceTexture _preview)
    {
       // String or = MainActivity.ReadIni("CAMERA_ORIENT");
        LENS_FACING = /*or.equals("LENS_FACING_BACK") ? LENS_FACING_BACK : */LENS_FACING_BACK;
        CameraCharacteristics characteristics = null;
        largest = null;
        mCameraManager = (CameraManager) AGlobals.contextApp.getSystemService(Context.CAMERA_SERVICE);

        StreamConfigurationMap map1 = null;
        cModes = new HashMap<Object, Object >();
        if (mCameraManager != null)
            try
            {
                String[] list = mCameraManager.getCameraIdList();
                Size largest = null;
                String id_largest = null;

                for (String id : list)
                {
                    characteristics = mCameraManager.getCameraCharacteristics(id);
                    Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);

                    if (facing == LENS_FACING) //CameraCharacteristics.LENS_FACING_FRONT)
                    {
                        StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                        if (map == null)
                            continue;
                        map1 = map;
                        CameraCalculatePreview.largest = null;
                        //new CF_Size( Collections.max(Arrays.asList(map.getOutputSizes(imageFormat)), new CameraCalculatePreview.CompareSizesByArea()));
                        Point displaySize = new Point();
                        AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getSize(displaySize);

                        List<Size> sz = Arrays.asList(map.getOutputSizes(imageFormat));
                        Collections.sort(sz, new CameraCalculatePreview.CompareSizesByArea());

                        Size size = null;
                        for (Size s : sz)
                        {
                            if (!fPirke)
                            {
                                if (s.getWidth() > 1000 && s.getWidth() >= displaySize.x && s.getHeight() > 1000 && s.getHeight() >= displaySize.y)
                                {
                                    size = s;
                                    break;

                                }
                            }
                            else
                            {
                                if (s.getWidth() >= displaySize.x && s.getHeight() >= displaySize.y)
                                {
                                    if (size  == null || size.getHeight() < s.getHeight())
                                        size = s;

                                }
                            }

                        }
                     //   if (fPirke)
                  //          size = Collections.max(Arrays.asList(map.getOutputSizes(ImageFormat.JPEG)), new CameraCalculatePreview.CompareSizesByArea());


                        if (size == null)
                            size = Collections.max(sz, new CameraCalculatePreview.CompareSizesByArea());
                        if (largest == null)
                        {
                            largest = size;
                            id_largest = id;
                        }
                        else
                            if (size.getWidth() > largest.getWidth() && size.getHeight() > largest.getHeight())
                            {
                                largest = size;
                                id_largest = id;

                            }
                }
                }
           cameraId = id_largest;
           ReadIni();

                CameraCalculatePreview.largest = new CF_Size(largest.getWidth(), largest.getHeight());
            characteristics = mCameraManager.getCameraCharacteristics(cameraId);
            fAutoFlash = characteristics.get(CameraCharacteristics.FLASH_INFO_AVAILABLE) != null;
            Integer maxAFRegions = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AF);
            Integer maxAERegions = characteristics.get(CameraCharacteristics.CONTROL_MAX_REGIONS_AE);
                isMeteringAreaAFSupported = maxAFRegions != null && maxAFRegions >= 1;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
                {
                    int [] s = characteristics.get(CameraCharacteristics.DISTORTION_CORRECTION_AVAILABLE_MODES);
                    fSizeRange = s == null;
                }
                int leve = characteristics.get(CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL);
                if (leve == INFO_SUPPORTED_HARDWARE_LEVEL_FULL)
                if (cModes.size() == 0)
            {
                int [] modesmodes = characteristics.get(CameraCharacteristics.STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES);
                if (Arrays.binarySearch( modesmodes, CameraCharacteristics.STATISTICS_FACE_DETECT_MODE_SIMPLE) >= 0)
                {
                cModes.put(CameraCharacteristics.STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES, CameraCharacteristics.STATISTICS_FACE_DETECT_MODE_SIMPLE);
                }
                    int[] modes = characteristics.get(CameraCharacteristics.CONTROL_AVAILABLE_SCENE_MODES);
                    if (modes != null)
                    {
                             Arrays.sort(modes);
//                        if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_HDR) >= 0)
  //                          cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_HDR);
                    //    else
                   /*     if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_LANDSCAPE) >= 0)
                            cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_LANDSCAPE);
                        else
                             if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_CANDLELIGHT) >= 0)
                                 cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_CANDLELIGHT);
                             else
                             if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_NIGHT) >= 0)
                                 cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_NIGHT);
                             else
                             if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_THEATRE) >= 0)
                                 cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_THEATRE);
                             else
                             if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_ACTION) >= 0)
                                 cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_ACTION);
                             else
                     */
                        if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_NIGHT_PORTRAIT) >= 0)
                            cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_NIGHT_PORTRAIT);
                        else
                        if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_PORTRAIT) >= 0)
                            cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_PORTRAIT);
                        //   else
                       // if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_SCENE_MODE_FACE_PRIORITY) >= 0)
                         //   cModes.put(CaptureRequest.CONTROL_SCENE_MODE, CameraCharacteristics.CONTROL_SCENE_MODE_FACE_PRIORITY);


                    }
                    modes = characteristics.get(CameraCharacteristics.CONTROL_AWB_AVAILABLE_MODES);
                    if (modes != null)
                    {
                            Arrays.sort(modes);
                        if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_AWB_MODE_AUTO) >= 0)
                            cModes.put(CaptureRequest.CONTROL_AWB_MODE,CameraCharacteristics.CONTROL_AWB_MODE_AUTO);
                        else
                        if (Arrays.binarySearch(modes, CameraCharacteristics.CONTROL_AWB_MODE_OFF) >= 0)
                            cModes.put(CaptureRequest.CONTROL_AWB_MODE,CameraCharacteristics.CONTROL_AWB_MODE_OFF);

                    }
                    modes = characteristics.get(CameraCharacteristics.EDGE_AVAILABLE_EDGE_MODES);
                    if (modes != null)
                    {
                        Arrays.sort(modes);
                        if (Arrays.binarySearch(modes, CameraCharacteristics.EDGE_MODE_FAST) >= 0)
                            cModes.put(CaptureRequest.EDGE_MODE,CameraCharacteristics.EDGE_MODE_FAST);
                        else
                        if (Arrays.binarySearch(modes, CameraCharacteristics.EDGE_MODE_HIGH_QUALITY) >= 0)
                            cModes.put(CaptureRequest.EDGE_MODE,CameraCharacteristics.EDGE_MODE_HIGH_QUALITY);
                    }

                    modes =  characteristics.get(CameraCharacteristics.TONEMAP_AVAILABLE_TONE_MAP_MODES);
                    if (modes != null)
                    {
                        Arrays.sort(modes);
                        if (Arrays.binarySearch(modes, CameraCharacteristics.TONEMAP_MODE_HIGH_QUALITY) >= 0)
                            cModes.put(CaptureRequest.TONEMAP_MODE, CameraCharacteristics.TONEMAP_MODE_HIGH_QUALITY);
                        else
                        if (Arrays.binarySearch(modes, CameraCharacteristics.TONEMAP_MODE_FAST) >= 0)
                            cModes.put(CaptureRequest.TONEMAP_MODE, CameraCharacteristics.TONEMAP_MODE_FAST);
                    }

                    modes = characteristics.get(CameraCharacteristics.HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES);
                    if (modes != null)
                    {
                        Arrays.sort(modes);
                        if (Arrays.binarySearch(modes, HOT_PIXEL_MODE_FAST) >= 0)
                            cModes.put(CaptureRequest.HOT_PIXEL_MODE, HOT_PIXEL_MODE_FAST);
                        else
                        if (Arrays.binarySearch(modes, HOT_PIXEL_MODE_HIGH_QUALITY) >= 0)
                            cModes.put(CaptureRequest.HOT_PIXEL_MODE, HOT_PIXEL_MODE_HIGH_QUALITY);
                    }
                    modes = characteristics.get(CameraCharacteristics.NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES);
                    if (modes != null)
                    {
                        Arrays.sort(modes);
                        if (Arrays.binarySearch(modes, NOISE_REDUCTION_MODE_FAST) >= 0)
                            cModes.put(CaptureRequest.NOISE_REDUCTION_MODE, NOISE_REDUCTION_MODE_FAST);
                        else
                        if (Arrays.binarySearch(modes, CameraCharacteristics.NOISE_REDUCTION_MODE_HIGH_QUALITY) >= 0)
                            cModes.put(CaptureRequest.NOISE_REDUCTION_MODE, CameraCharacteristics.NOISE_REDUCTION_MODE_HIGH_QUALITY);
                    }
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
                    {
                        modes = characteristics.get(CameraCharacteristics.SHADING_AVAILABLE_MODES);
                        if (modes != null)
                        {
                            List<int[]> ls = Arrays.asList(modes);
                            if (Arrays.binarySearch(modes, SHADING_MODE_FAST) >= 0)
                                cModes.put(CaptureRequest.NOISE_REDUCTION_MODE, SHADING_MODE_FAST);
                            else
                            if (Arrays.binarySearch(modes, SHADING_MODE_HIGH_QUALITY) >= 0)
                                cModes.put(CaptureRequest.SHADING_MODE, SHADING_MODE_HIGH_QUALITY);
                        }
                    }
                    modes = characteristics.get(CameraCharacteristics.LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION);
                    if (modes != null)
                    {
                        Arrays.sort(modes);
                        if (Arrays.binarySearch(modes, LENS_OPTICAL_STABILIZATION_MODE_ON) >= 0)
                            cModes.put(CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE, LENS_OPTICAL_STABILIZATION_MODE_ON);
                    }



            }
            Range<Integer>[] ranges = characteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
            if (ranges != null && ranges.length > 0)
            {
              //  Arrays.sort(ranges);
                int n = ranges.length /2;
                Range<Integer> result = ranges[n];
                /*
                for (Range<Integer> range : ranges)
                {
                    int upper = range.getUpper();
                    if (upper >= 10)
                        if (result == null || upper > result.getUpper().intValue())
                            result = range;


                }

                 */
                if (result == null)
                    result = ranges[0];
                cModes.put(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, result);
            }


                preview = _preview;
            wait = new EventIdle("cameraWait");
            int[] afModes = characteristics.get(CameraCharacteristics.CONTROL_AF_AVAILABLE_MODES);

                previewCallback = new CPreviewCallback(afModes == null || afModes.length == 1, wait, new ActionCallback(this, "OnPreviewOk", int.class));
             handler = new CHandler(wait.GetLooper());
            return true;
            } catch (CameraAccessException | NullPointerException e)
            {
                mCameraManager = null;
            }
        mCameraManager = null;
        return false;
    }

    public boolean IsOpenedSession()
    {
        return mCameraManager != null && cameraId != null && device != null && session != null;
    }
/*
      android.hardware.camera2.params.Face[] faces = captureResult.get(CaptureResult.STATISTICS_FACES);
            if (faces != null) {
                Log.i(TAG, "faces : " + faces.length + " , mode : " + mode);
                for (android.hardware.camera2.params.Face face : faces) {
                    Rect faceBounds = face.getBounds();
                    // Once processed, the result is sent back to the View
                    presenterView.onFaceDetected(mapCameraFaceToCanvas(faceBounds, face.getLeftEyePosition(),
                            face.getRightEyePosition()));
                }

 */

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    @SuppressLint("MissingPermission")
    public boolean OpenCamera(ActionCallback _callbackOnPreviewOk)
    {
        self = this;
        callbackOnPreviewOk = _callbackOnPreviewOk;
        try
        {
            mImageReader = ImageReader.newInstance(largest.getWidth(), largest.getHeight(), imageFormat, 2);

            mCameraManager.openCamera(cameraId, new CStateCamera(wait,
                    new ActionCallback(this, "OnCameraAction", CameraDevice.class, String.class)), handler);


        } catch (CameraAccessException e)
        {
            AddToLog(e);
        }
        return device != null;

    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public float Scale(float zoom)
    {

        if (!fPirke && zoom > 1.15f)
            zoom = 1.15f;
        return SetCropRegion(previewBuilder, zoom);

    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------

    private float SetCropRegion(CaptureRequest.Builder builder, float zoom)
    {
        try
        {
            if (mCameraManager == null) return 1.0f;
        CameraCharacteristics c = null;
        try
        {
            c = mCameraManager.getCameraCharacteristics(cameraId);
        } catch (CameraAccessException e)
        {
            AddToLog(e);
            return 1.0f;
        }

        Rect rect;
        rect = c.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        if (fSizeRange)
        {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
            {
                Rect rc1 = c.get(CameraCharacteristics.SENSOR_INFO_PRE_CORRECTION_ACTIVE_ARRAY_SIZE);
                if (rc1 != null)
                    rect = rc1;
            }


        }
            float maxZoom, min_zoom = 1.0f;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R)
            {
                min_zoom = c.get(CameraCharacteristics.CONTROL_ZOOM_RATIO_RANGE).getLower();
                maxZoom = c.get(CameraCharacteristics.CONTROL_ZOOM_RATIO_RANGE).getUpper();
            }
            else
            {
                maxZoom = c.get(CameraCharacteristics.SCALER_AVAILABLE_MAX_DIGITAL_ZOOM)/1.2f;
            }
            if (zoom < min_zoom) zoom = min_zoom;
      //  if (zoom < 1.25f) zoom = 1.25f;
        if (maxZoom < zoom) zoom = maxZoom;
        fLastZoom = zoom;
        int centerX = rect.centerX();
        int centerY = rect.centerY();
        float width = (rect.width() / zoom);
        float left = (rect.width() - width) / 2.0f;
        if (left < 0) left = 0;
        float height = (rect.height() / zoom);

        float top = (rect.height() - height) / 2.0f;
        if (top < 0)
        {
            top = 0;
        }
            if (lastCrop == null)
            {
                lastCrop = new Rect((int) Math.floor(left), (int) Math.floor(top), (int) Math.floor(left + width), (int) Math.floor(top + height));
            }
            else
            {
                Rect crop = new Rect((int) Math.floor(left), (int) Math.floor(top), (int) Math.floor(left + width), (int) Math.floor(top + height));
            //    if (((float)crop.width()/(float)lastCrop.width()) == 1.0f)
              //      return zoom;
                lastCrop = crop;
            }
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                builder.set(CaptureRequest.CONTROL_ZOOM_RATIO, zoom);
            }
            else {
                builder.set(CaptureRequest.SCALER_CROP_REGION, lastCrop);

            }
            if(isMeteringAreaAFSupported)
            {
                MeteringRectangle [] focus = null;
                if (focusRect != null)
                {
                  /*  int y = (lastCrop.top + lastCrop.bottom)/2;
                    int x1 = (int)(lastCrop.left + lastCrop.width()/4.0f - 50.0f/zoom);
                    int x2 = (int)(x1 + 100.0f/zoom);
                    if (x1 < 0) x1 = 0;
                    if (x2 > width) x2 = (int)width;

                   */
                    float scalex =  (float) cropDisplay.width()/(float) lastCrop.width();
                    float scaley =  (float) cropDisplay.height()/(float) lastCrop.height();
                    ;
                    float x =  (focusRect.left - cropDisplay.left) *  scalex + lastCrop.left;
                    float y = lastCrop.top +  (focusRect.top - cropDisplay.top) * scaley;

                    focus = new MeteringRectangle [] {
                        new MeteringRectangle(
                                (int) x,
                                (int) y,
                                (int) (scalex * focusRect.width()),
                                (int)(scaley * focusRect.height()),
                                MeteringRectangle.METERING_WEIGHT_MAX - 1),
                        };
                }
                else
                {
                    focus = new MeteringRectangle [] {
                            new MeteringRectangle(
                                    lastCrop.left,
                                    lastCrop.top,
                                    lastCrop.width(),
                                    lastCrop.height(),

                                    MeteringRectangle.METERING_WEIGHT_MAX - 1),
                    };
                }

                if (focus != null)
                {
                    builder.set(CaptureRequest.CONTROL_AF_REGIONS, focus);
                    builder.set(CaptureRequest.CONTROL_AE_REGIONS, focus);
                }

            }
            builder.set(CaptureRequest.CONTROL_AE_MODE, CameraMetadata.CONTROL_AE_MODE_ON);
           // builder.set(CaptureRequest.STATISTICS_FACE_DETECT_MODE, CameraMetadata.STATISTICS_FACE_DETECT_MODE_SIMPLE);
            session.setRepeatingRequest(builder.build(), previewCallback, handler);
            previewBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
            IdleFocus();
        }
         catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return zoom;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    private void FlushOff(CaptureRequest.Builder builder)
    {
        if (fAutoFlash)
        {
            builder.set(CaptureRequest.FLASH_MODE, CaptureRequest.FLASH_MODE_OFF);
        }

    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    private void StartCaptureSesion()
    {
        if (mCameraManager == null) return;
        if (device != null)
        {
            try
            {
                fStartFoto = false;
                fWaitFoto = false;

                Surface surface = new Surface(preview);
                previewBuilder = device.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);

                previewBuilder.addTarget(surface);
                device.createCaptureSession(Arrays.asList(surface, mImageReader.getSurface()),
                        new CCameraState(wait, new ActionCallback(this, "InitPreview", CameraCaptureSession.class, String.class)), handler);
            } catch (CameraAccessException e)
            {
                AddToLog(e);
            }

        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void OnCameraAction(CameraDevice dev, String action)
    {

        if (action.equals("open"))
        {
            device = dev;
            StartCaptureSesion();
        }
        if (action.equals("close") || action.equals("error"))
        {

            mCameraManager = null;

            onexit.run((Bitmap)null);
        }

    }



    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void StartFoto(ActionCallback _callOnResult, ActionCallback _callOnResultError)
    {

        if (mCameraManager == null || fStartFoto) return;
        proba_new_foto = 0;

        onErrorFoto = _callOnResultError;
        onexit = _callOnResult;

        startFoto();
    }
    private void startFoto()
    {
        try
        {

            int[] afModes = Characteristics().get(CameraCharacteristics.CONTROL_AF_AVAILABLE_MODES);

            previewBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_START);
            fotoCallback = new CFotoCallback(afModes == null || afModes.length == 1, wait, new ActionCallback(this, "OnFoto", String.class, TotalCaptureResult.class));
            mPreviewRequest = previewBuilder.build();
            session.capture(mPreviewRequest, fotoCallback, handler);
            session.setRepeatingRequest(mPreviewRequest, fotoCallback, handler);
            fStartFoto = true;
            fWaitFoto = false;
        } catch (CameraAccessException e)
        {
            fStartFoto = false;
            fWaitFoto = false;
            AddToLog(e);
        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void OnPreview()
    {

    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void OnFoto(String action, TotalCaptureResult result)
    {

        if (action.equals("open") && fStartFoto && !fWaitFoto)
        {
            try
            {
                fWaitFoto = true;
                session.capture(mPreviewRequest, null, null);
                session.setRepeatingRequest(mPreviewRequest, null, null);
                fotoCallback.Disable();

                    captureBuilder = device.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
                    captureBuilder.addTarget(mImageReader.getSurface());
                    captureBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                    captureBuilder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, previewBuilder.get(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE));
                    captureBuilder.set(CaptureRequest.CONTROL_AF_REGIONS, previewBuilder.get(CaptureRequest.CONTROL_AF_REGIONS));
                    captureBuilder.set(CaptureRequest.CONTROL_AE_REGIONS, previewBuilder.get(CaptureRequest.CONTROL_AE_REGIONS));
                    captureBuilder.set(CaptureRequest.SCALER_CROP_REGION, previewBuilder.get(CaptureRequest.SCALER_CROP_REGION));
                    captureBuilder.set(CaptureRequest.LENS_FOCUS_DISTANCE, previewBuilder.get(CaptureRequest.LENS_FOCUS_DISTANCE));
                    Set<Object> set = cModes.keySet();
                    for (Object key: set)
                    {
                        try
                        {
                            CaptureRequest.Key<Object> ckey =  (CaptureRequest.Key<Object>) key;
                            captureBuilder.set(ckey, cModes.get(key));
                        }
                        catch (Exception e)
                        {
                            e.printStackTrace();
                        }
                    }

                    FlushOff(captureBuilder);

                    int rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
                    if (lastCrop != null)
                        captureBuilder.set(CaptureRequest.SCALER_CROP_REGION, lastCrop);
                    //captureBuilder.set(CaptureRequest.JPEG_ORIENTATION, getOrientation(rotation));
                    captureBuilder.set(CaptureRequest.JPEG_QUALITY, (byte) 100);
                //

                    mImageReader.setOnImageAvailableListener(new CSaveFotoListner(wait, new ActionCallback(this, "OnSavePicture", ImageReader.class)), handler);
                int[] afModes = Characteristics().get(CameraCharacteristics.CONTROL_AF_AVAILABLE_MODES);

                CaptureRequest bld =  captureBuilder.build();
                session.stopRepeating();
                CameraCaptureSession.CaptureCallback CaptureCallback = new CameraCaptureSession.CaptureCallback() {

                    @Override
                    public void onCaptureCompleted(@NonNull CameraCaptureSession session,
                                                   @NonNull CaptureRequest request, @NonNull TotalCaptureResult result) {
                        Toast.makeText(AGlobals.contextApp, "Saved", Toast.LENGTH_SHORT).show();
                    }
                    public void onCaptureFailed(@NonNull CameraCaptureSession session,
                                                @NonNull CaptureRequest request, @NonNull CaptureFailure failure) {
                        super.onCaptureFailed(session, request, failure);
                        String error = "";

                        switch (failure.getReason())
                        {
                            case CaptureFailure.REASON_ERROR:
                                error = "Capture failed: REASON_ERROR";
                                break;
                            case CaptureFailure.REASON_FLUSHED:
                                error = "Capture failed: REASON_FLUSHED";
                                break;
                            default:
                                error = "Capture failed: UNKNOWN";
                                break;
                        }
                        Toast.makeText(AGlobals.contextApp, error, Toast.LENGTH_SHORT).show();

                    }

                };
                session.capture(bld, CaptureCallback, null);

            } catch (CameraAccessException e)
            {
                AddToLog(e);
                fWaitFoto = false;
                fStartFoto = true;
				onexit.run((ActionCallback) null);
            }
        }

    }
    public void onFotoSave(String opt, TotalCaptureResult res) {
        Log.e("onFotoSave", opt);
        Log.e("onFotoSave", res.toString());
    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void OnSavePicture(ImageReader reader)
    {
        if (!fStartFoto) return;
        fStartFoto = false;

        if (onexit != null)
        {
            try
            {
                session.stopRepeating();
                Image image = reader.acquireNextImage();
                Rect rc = CameraCalculatePreview.mCropRect;
                if (fPirke)
                {
                    float scale = 1.f/scaleScrren; //((float)largest.m_cx/ mPreviewSize.m_cx); //1.0f; //Math.max((float) h / mPreviewSize.getHeight(), (float) w / mPreviewSize.getWidth());
                    float _w = scale *  CameraCalculatePreview.mCropRect.width();
                    float _h = scale *  CameraCalculatePreview.mCropRect.height();
                    float cx = CameraCalculatePreview.mCropRect.centerX();
                    float cy = CameraCalculatePreview.mCropRect.centerY();
                    rc.set((int)(cx - _w/2.0f),(int)(cy - _h/2.0f),(int)(cx + _w/2.0f),(int)(cy + _h/2.0f));
                }
                ByteBuffer buffer = image.getPlanes()[0].getBuffer();
                byte[] bytes = new byte[buffer.capacity()];
                buffer.get(bytes);
                image.close();
                Bitmap bmp = BitmapFactory.decodeByteArray(bytes, 0, bytes.length, null);
                Matrix m = GetOrientation();
                if (CameraCalculatePreview.fPirke)
                {
                   // m.postRotate(90f);
                }
                Bitmap bitmapImage = Bitmap.createBitmap(bmp, bmp.getWidth() <= (rc.width() + rc.left) ? 0 : rc.left, bmp.getHeight() <= (rc.height() + rc.top) ? 0 : rc.top,
                        Math.min(rc.width(), bmp.getWidth()),
                        Math.min(rc.height(), bmp.getHeight()), m, false);
                bmp.recycle();
                ASound.PlayAudio("camera_shutter_click");

                onexit.run(bitmapImage);

            } catch (Exception e)
            {
                AddToLog(e);
                if (proba_new_foto < 5)
                {
                     ++proba_new_foto;
                    ;
                    (new CHandler()).Send(new Runnable() {
                        @Override
                        public void run() {
                            startFoto();
                        }
                    }, 10);
                    return;
                }


                onexit.run((ActionCallback) null);
            }
  }


    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void UnlockFocus()
    {
        try
        {
            previewCallback.retOk = CCamera.CAMERA_FOCUSED;
            previewBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CONTROL_AF_TRIGGER_CANCEL);
            session.capture(previewBuilder.build(), previewCallback, handler);
            previewBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_IDLE);
    session.setRepeatingRequest(previewBuilder.build(), previewCallback, handler);

        } catch (CameraAccessException e)
        {
            AddToLog(e);
        }
    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void IdleFocus()
    {
        try
        {
            previewCallback.retOk = CCamera.CAMERA_SCAN_OK;
            previewBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_CANCEL);
            session.capture(previewBuilder.build(), previewCallback, handler);
            (new CHandler()).Send(new Runnable() {
                @Override
                public void run() {
                    previewBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_IDLE);
                    try {
                        session.setRepeatingRequest(previewBuilder.build(), previewCallback, handler);
                    } catch (CameraAccessException e) {
                        AddToLog(e);
                    }

                }
            });
        } catch (CameraAccessException e)
        {
            AddToLog(e);
        }
    }    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void LockFocus()
    {
        try
        {
        int res;
            if ((res = previewBuilder.get(CaptureRequest.CONTROL_AF_TRIGGER))  != CameraMetadata.CONTROL_AF_TRIGGER_START)
            {
            if (res != CONTROL_AF_TRIGGER_CANCEL)
            {
               previewCallback.retOk = CCamera.CAMERA_FOCUSED;
            }
                previewBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_CANCEL);
                session.capture(previewBuilder.build(), previewCallback, handler);
                (new CHandler()).Send(new Runnable() {
                    @Override
                    public void run() {
                        previewBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_START);
                        try {
                            session.setRepeatingRequest(previewBuilder.build(), previewCallback, handler);
                        } catch (CameraAccessException e) {
                            AddToLog(e);
                        }

                    }
                }, 1);

            }
        } catch (CameraAccessException e)
        {
            AddToLog(e);
        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    Range<Integer> GetLuminate()
    {
        CameraCharacteristics chars = null;
        try
        {
            chars = mCameraManager.getCameraCharacteristics(cameraId);
            Range<Integer>[] ranges = chars.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
            Range<Integer> result = null;
            for (Range<Integer> range : ranges)
            {
                int upper = range.getUpper();
                if (upper >= 10)
                    if (result == null || upper > result.getUpper().intValue())
                        result = range;


            }
            if (result == null)
                result = ranges[0];

            return result;
        } catch (CameraAccessException e)
        {
            AddToLog(e);
        }
        return null;
    }
    void InitModes()
    {
        if (cModes.size() == 0)
        {
            Integer mode = previewBuilder.get(CaptureRequest.STATISTICS_LENS_SHADING_MAP_MODE);
            if (mode != null && cModes.get(CaptureRequest.STATISTICS_LENS_SHADING_MAP_MODE) == null)
                cModes.put(CaptureRequest.STATISTICS_LENS_SHADING_MAP_MODE, mode);
            mode = previewBuilder.get(CaptureRequest.CONTROL_AWB_MODE);
            if (mode != null && cModes.get(CaptureRequest.CONTROL_AWB_MODE) == null)
                cModes.put(CaptureRequest.CONTROL_AWB_MODE, mode);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
            {
                mode = previewBuilder.get(CaptureRequest.DISTORTION_CORRECTION_MODE);
                if (mode != null && cModes.get(CaptureRequest.DISTORTION_CORRECTION_MODE) == null)
                    cModes.put(CaptureRequest.DISTORTION_CORRECTION_MODE, mode);
            }
            mode = previewBuilder.get(CaptureRequest.CONTROL_MODE);
            if (mode != null && cModes.get(CaptureRequest.CONTROL_MODE) == null)
                cModes.put(CaptureRequest.CONTROL_MODE, mode);
            mode = previewBuilder.get(CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE);
            if (mode != null && cModes.get(CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE) == null)
                cModes.put(CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE, mode);
            mode = previewBuilder.get(CaptureRequest.SHADING_MODE);
            if (mode != null && cModes.get(CaptureRequest.SHADING_MODE) == null)
                cModes.put(CaptureRequest.SHADING_MODE, mode);
            mode = previewBuilder.get(CaptureRequest.NOISE_REDUCTION_MODE);
            if (mode != null && cModes.get(CaptureRequest.NOISE_REDUCTION_MODE) == null)
                cModes.put(CaptureRequest.NOISE_REDUCTION_MODE, mode);
            mode = previewBuilder.get(CaptureRequest.HOT_PIXEL_MODE);
            if (mode != null && cModes.get(CaptureRequest.HOT_PIXEL_MODE) == null)
                cModes.put(CaptureRequest.HOT_PIXEL_MODE, mode);
            mode = previewBuilder.get(CaptureRequest.TONEMAP_MODE);
            if (mode != null && cModes.get(CaptureRequest.TONEMAP_MODE) == null)
                cModes.put(CaptureRequest.TONEMAP_MODE, mode);
            mode = previewBuilder.get(CaptureRequest.EDGE_MODE);
            if (mode != null && cModes.get(CaptureRequest.EDGE_MODE) == null)
                cModes.put(CaptureRequest.EDGE_MODE, mode);
            mode = previewBuilder.get(CaptureRequest.CONTROL_SCENE_MODE);
            if (mode != null && cModes.get(CaptureRequest.CONTROL_SCENE_MODE) == null)
                cModes.put(CaptureRequest.CONTROL_SCENE_MODE, mode);

        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public void InitPreview(CameraCaptureSession _session, String action)
    {
        if (_session != null)
        {
            if (action.equals("open"))
            {
                session = _session;
                try
                {
                    previewBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                    InitModes();
                    Set<Object> set = cModes.keySet();
                    for (Object key: set)
                    {
                        try
                        {
                            CaptureRequest.Key<Object> ckey =  (CaptureRequest.Key<Object>) key;
                            previewBuilder.set(ckey, cModes.get(key));
                        }
                        catch (Exception e)
                        {
                            e.printStackTrace();
                        }
                    }
                    previewBuilder.set(CaptureRequest.CONTROL_CAPTURE_INTENT, CaptureRequest.CONTROL_CAPTURE_INTENT_PREVIEW);
/*
                    if(isMeteringAreaAFSupported)
                    {
                        int y = (CameraCalculatePreview.mCropRect.top + CameraCalculatePreview.mCropRect.bottom)/2;
                        int x1 = CameraCalculatePreview.mCropRect.left + CameraCalculatePreview.mCropRect.width()/4;
                        int x2 = x1 + CameraCalculatePreview.mCropRect.width()/2;
                        MeteringRectangle [] focus = new MeteringRectangle [] {
                                new MeteringRectangle(
                                x1 - 50,
                                 y - 50,
                                 100,
                                 100,

                                 MeteringRectangle.METERING_WEIGHT_MAX - 1),
                                new MeteringRectangle(
                                        x2 - 50,
                                        y - 50,
                                        100,
                                        100,

                                        MeteringRectangle.METERING_WEIGHT_MAX - 1),
                        };
                        previewBuilder.set(CaptureRequest.CONTROL_AF_REGIONS, focus);
                        previewBuilder.set(CaptureRequest.CONTROL_AE_REGIONS, focus);

                    }

                 */
                    int rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
                    //session.setRepeatingRequest(previewBuilder.build(), previewCallback, handler);
                    Scale(MainActivity.ReadIniFloat("CAMERA_ZOOM"));

                } catch (Exception e)
                {
                    AddToLog(e);
                }

            }
            if (session != null && session.equals(_session) && action.equals("close") || action.equals("error"))
            {
                session = _session;
                final SurfaceTexture _preview = preview;
                try
                {
                    close();
                } catch (IOException e)
                {
                    e.printStackTrace();
                }
                Init(_preview);
                AGlobals.rootHandler.Send(new Runnable() {
                    @Override
                    public void run()
                    {
                        Init(_preview);
                        OpenCamera(callbackOnPreviewOk);

                    }
                }, 100);
            }
        }

    }

    public void OnPreviewOk(int state)
    {
        if (callbackOnPreviewOk != null)
            callbackOnPreviewOk.run(state);
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    public boolean IsValid()
    {
        return mCameraManager != null;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    @Override
    public void close() throws IOException
    {
        self = null;
        if (session != null)
            session.close();
        if (device != null)
            device.close();
        if (mImageReader != null)
            mImageReader.close();
        mImageReader = null;
        device = null;
        session = null;
        preview = null;
        previewBuilder = null;
        wait = null;
        cameraId = null;
        mCameraManager = null;

    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    private Matrix GetOrientation() throws CameraAccessException
    {
        int rotation = AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getRotation();
        CameraCharacteristics c = Characteristics();
        int sensorOrientation = c.get(CameraCharacteristics.SENSOR_ORIENTATION);
        int deviceOrientation =  0;
        boolean facingFront = c.get(CameraCharacteristics.LENS_FACING) == CameraCharacteristics.LENS_FACING_FRONT;
        switch(rotation)
        {
            case Surface.ROTATION_90:
            deviceOrientation = -90;
             if (facingFront) deviceOrientation = -deviceOrientation;
            break;
            case Surface.ROTATION_270:
             deviceOrientation = -270;
                if (facingFront) deviceOrientation = -deviceOrientation;
                break;
            case Surface.ROTATION_180:
              deviceOrientation = -180;
             break;
            default:
                deviceOrientation = -180;
        }
        int jpegOrientation = (sensorOrientation + deviceOrientation + 360) % 360;
        Matrix m = new Matrix();
        m.preScale(facingFront ? -1 : 1, 1);
        m.postRotate(jpegOrientation);


        return m;

    }
/*    //Sets whether the camera device uses optical image stabilization (OIS) when capturing images.
    public CCell[] GetOpticalMode() throws CameraAccessException
    //Quality of lens shading correction applied to the image data.
    public CCell[] GetShadingMode() throws CameraAccessException
    //Mode of operation for the noise reduction algorithm.
    public CCell[] GetNoiceReductionMode() throws CameraAccessException
    //List of hot pixel correction mode
    public CCell[] GetHotPixelMode() throws CameraAccessException

    //List of tonemapping modes
    public CCell[] GetToneModes() throws CameraAccessException

    //List of edge enhancement modes
    public CCell[] GetEdgeMode() throws CameraAccessException
    //Mode of operation for the lens distortion correction block.
    public CCell[] GetCorrectionModes() throws CameraAccessException
    //Overall mode of 3A (auto-exposure, auto-white-balance, auto-focus) control routines.
    public CCell[] GetControlModes() throws CameraAccessException
    //Scene mode
    public CCell[] GetParamSceneMode() throws CameraAccessException
        //Whether the camera device will output the lens shading map in output result metadata.
    public CCell[] GetShadingLensModes() throws CameraAccessException
 //Overall mode of 3A (auto-exposure, auto-white-balance, auto-focus) control routines.
    public CCell[] GetControlModes() throws CameraAccessException
    //Mode of operation for the lens distortion correction block.
    public CCell[] GetCorrectionModes() throws CameraAccessException


*/
    //Scene mode
    public CCell[] GetParamSceneMode() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes = characteristics.get(CameraCharacteristics.CONTROL_AVAILABLE_SCENE_MODES);
        if (modes == null) return null;
        CCell [] cell = new CCell[modes.length];
        int mode = previewBuilder.get(CaptureRequest.CONTROL_SCENE_MODE);
        String str = null;
        for (int i = 0; i < modes.length; ++i)
        {
            switch (modes[i])
            {
            case CameraCharacteristics.CONTROL_SCENE_MODE_DISABLED:
                str = "CONTROL_SCENE_MODE_DISABLED";
                break;

            case CameraCharacteristics.CONTROL_SCENE_MODE_FACE_PRIORITY :
                str = "CONTROL_SCENE_MODE_FACE_PRIORITY";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_ACTION:
                str = "CONTROL_SCENE_MODE_ACTION";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_PORTRAIT:
                str = "CONTROL_SCENE_MODE_PORTRAIT";
                break;
                case CameraCharacteristics.CONTROL_SCENE_MODE_LANDSCAPE:
                    str = "CONTROL_SCENE_MODE_LANDSCAPE";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_NIGHT:
                str = "CONTROL_SCENE_MODE_NIGHT";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_NIGHT_PORTRAIT:
                str = "CONTROL_SCENE_MODE_NIGHT_PORTRAIT";
                break;

            case CameraCharacteristics.CONTROL_SCENE_MODE_THEATRE:
                str = "CONTROL_SCENE_MODE_THEATRE";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_BEACH:
                str = "CONTROL_SCENE_MODE_BEACH";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_SNOW:
                str = "CONTROL_SCENE_MODE_SNOW";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_SUNSET:
                str = "CONTROL_SCENE_MODE_SUNSET";
                break;

            case CameraCharacteristics.CONTROL_SCENE_MODE_STEADYPHOTO:
                str = "CONTROL_SCENE_MODE_STEADYPHOTO";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_FIREWORKS:
                str = "CONTROL_SCENE_MODE_FIREWORKS";
                break;

            case CameraCharacteristics.CONTROL_SCENE_MODE_SPORTS:
                str = "CONTROL_SCENE_MODE_SPORTS";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_PARTY:
                str = "CONTROL_SCENE_MODE_PARTY";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_CANDLELIGHT:
                str = "CONTROL_SCENE_MODE_CANDLELIGHT";
                break;
            case CameraCharacteristics.CONTROL_SCENE_MODE_BARCODE:
                str = "CONTROL_SCENE_MODE_BARCODE";
                break;

            case CameraCharacteristics.CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO:
                str = "CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO";
                break;

                case CameraCharacteristics.CONTROL_SCENE_MODE_HDR:
                    str = "CONTROL_SCENE_MODE_HDR";
                break;
                default:
                    str = "" + modes[i];
            }
            cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
            cell[i].extra = modes[i];
            if (mode == modes[i])
                cell[i].value = 1;

        }
        return cell;
        }catch (Exception e)
        {
            return null;
        }

    }

    //List of edge enhancement modes
    public CCell[] GetEdgeMode() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes = characteristics.get(CameraCharacteristics.EDGE_AVAILABLE_EDGE_MODES);
            if (modes == null) return null;

            CCell [] cell = new CCell[modes.length];
    int mode = previewBuilder.get(CaptureRequest.EDGE_MODE);

        String str = null;
        for (int i = 0; i < modes.length; ++i)
        {
            switch (modes[i])
            {
                case CameraCharacteristics.EDGE_MODE_OFF:
                    str = "EDGE_MODE_OFF";
                    break;

                case CameraCharacteristics.EDGE_MODE_FAST:
                    str = "EDGE_MODE_FAST";
                break;
                case CameraCharacteristics.EDGE_MODE_HIGH_QUALITY:
                    str = "EDGE_MODE_HIGH_QUALITY";
                    break;
                case CameraCharacteristics.EDGE_MODE_ZERO_SHUTTER_LAG:
                    str = "EDGE_MODE_ZERO_SHUTTER_LAG";
                    break;
                default:
                    str = "" + modes[i];
            }
            cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
            cell[i].extra = modes[i];
            if (mode == modes[i])
               cell[i].value = 1;

        }
        return cell;
        }catch (Exception e)
        {
            return null;
        }
    }

    //List of tonemapping modes
    public CCell[] GetToneModes() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes = characteristics.get(CameraCharacteristics.TONEMAP_AVAILABLE_TONE_MAP_MODES);
            if (modes == null) return null;

            CCell [] cell = new CCell[modes.length];

        String str = null;
        int mode = previewBuilder.get(CaptureRequest.TONEMAP_MODE);

        for (int i = 0; i < modes.length; ++i)
        {
            switch (modes[i])
            {
                case CameraCharacteristics.TONEMAP_MODE_CONTRAST_CURVE:
                str = "TONEMAP_MODE_CONTRAST_CURVE";
                break;
                case CameraCharacteristics.TONEMAP_MODE_FAST:
                    str = "TONEMAP_MODE_FAST";
                break;
                case CameraCharacteristics.TONEMAP_MODE_HIGH_QUALITY:
                    str = "TONEMAP_MODE_HIGH_QUALITY";
                 break;
                case CameraCharacteristics.TONEMAP_MODE_GAMMA_VALUE:
                    str = "TONEMAP_MODE_GAMMA_VALUE";
                    break;
                case TONEMAP_MODE_PRESET_CURVE:
                    str = "TONEMAP_MODE_PRESET_CURVE";
                    break;
                default:
                    str = "" + modes[i];
            }
            cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
            cell[i].extra = modes[i];

            if (mode == modes[i])
                cell[i].value = 1;
        }
        return cell;
        }catch (Exception e)
        {
            return null;
        }

    }
    //List of hot pixel correction mode
    public CCell[] GetHotPixelMode() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes = characteristics.get(CameraCharacteristics.HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES);
            if (modes == null) return null;

            CCell [] cell = new CCell[modes.length];
        int mode = previewBuilder.get(CaptureRequest.HOT_PIXEL_MODE);

        String str = null;
        for (int i = 0; i < modes.length; ++i)
        {
            switch (modes[i])
            {
                case  HOT_PIXEL_MODE_OFF:
                    str = "HOT_PIXEL_MODE_OFF";
                    break;


                case HOT_PIXEL_MODE_FAST:
                    str = "HOT_PIXEL_MODE_FAST";
                    break;
                case HOT_PIXEL_MODE_HIGH_QUALITY:
                    str = "HOT_PIXEL_MODE_HIGH_QUALITY";
                    break;
                default:
                    str = "" + modes[i];
            }
            cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
            if (mode == modes[i])
                cell[i].value = 1;
            cell[i].extra = modes[i];

        }
        return cell;
        }catch (Exception e)
        {
            return null;
        }
    }

    //Mode of operation for the noise reduction algorithm.
    public CCell[] GetNoiceReductionMode() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes = characteristics.get(CameraCharacteristics.NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES);
            if (modes == null) return null;

            CCell [] cell = new CCell[modes.length];
        int mode = previewBuilder.get(CaptureRequest.NOISE_REDUCTION_MODE);
        String str = null;
        for (int i = 0; i < modes.length; ++i)
        {
            switch (modes[i])
            {
                case NOISE_REDUCTION_MODE_OFF:
                    str = "NOISE_REDUCTION_MODE_OFF";
                    break;
                case NOISE_REDUCTION_MODE_FAST:
                    str = "NOISE_REDUCTION_MODE_FAST";
                    break;
                case CameraCharacteristics.NOISE_REDUCTION_MODE_HIGH_QUALITY:
                    str = "NOISE_REDUCTION_MODE_HIGH_QUALITY";
                    break;

                case  NOISE_REDUCTION_MODE_MINIMAL:
                    str = "NOISE_REDUCTION_MODE_MINIMAL";
                    break;

                case NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG:
                    str = "NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG";
                    break;
                default:
                    str = "" + modes[i];
            }
            cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
            cell[i].extra = modes[i];
            if (mode == modes[i])
                cell[i].value = 1;

        }
        return cell;
        }catch (Exception e)
        {
            return null;
        }

    }
    //Quality of lens shading correction applied to the image data.
    public CCell[] GetShadingMode() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes;

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M)
        {
            modes = characteristics.get(CameraCharacteristics.SHADING_AVAILABLE_MODES);
            if (modes == null) return null;

            CCell [] cell = new CCell[modes.length];
            int mode = previewBuilder.get(CaptureRequest.SHADING_MODE);

            String str = null;
            for (int i = 0; i < modes.length; ++i)
            {
                switch (modes[i])
                {
                    case SHADING_MODE_OFF:
                        str = "SHADING_MODE_OFF";
                        break;
                    case SHADING_MODE_FAST:
                        str = "SHADING_MODE_FAST";
                        break;
                    case SHADING_MODE_HIGH_QUALITY:
                        str = "SHADING_MODE_HIGH_QUALITY";
                        break;
                    default:
                        str = "" + modes[i];
                }
                cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
                cell[i].extra = modes[i];
                if (mode == modes[i])
                    cell[i].value = 1;
            }
            return cell;
        }
        return null;
        }catch (Exception e)
        {
            return null;
        }
    }
    //Sets whether the camera device uses optical image stabilization (OIS) when capturing images.
    public CCell[] GetOpticalMode() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes ;
        modes = characteristics.get(CameraCharacteristics.LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION);

        if (modes == null) return null;

            CCell [] cell = new CCell[modes.length];
        int mode = previewBuilder.get(CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE);
        String str = null;
        for (int i = 0; i < modes.length; ++i)
        {
            switch (modes[i])
            {
                case LENS_OPTICAL_STABILIZATION_MODE_OFF:
                    str = "LENS_OPTICAL_STABILIZATION_MODE_OFF";
                    break;
                case LENS_OPTICAL_STABILIZATION_MODE_ON:
                    str = "LENS_OPTICAL_STABILIZATION_MODE_ON";
                    break;
                default:
                    str = "" + modes[i];
            }
            cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
            cell[i].extra = modes[i];
            if (mode == modes[i])
                cell[i].value = 1;
        }
        return cell;
        }catch (Exception e)
        {
            return null;
        }

    }
    public CCell[] GetDistortionCorrectionModes() throws CameraAccessException
    {

        //Overall mode of 3A (auto-exposure, auto-white-balance, auto-focus) control routines.
        try {
            CameraCharacteristics characteristics = Characteristics();
            int[] modes;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
            {
                modes = characteristics.get(CameraCharacteristics.DISTORTION_CORRECTION_AVAILABLE_MODES);
                if (modes == null) return null;
                CCell [] cell = new CCell[modes.length];

                int mode = 0;
                    mode = previewBuilder.get(CaptureRequest.DISTORTION_CORRECTION_MODE);
                String str = null;
                for (int i = 0; i < modes.length; ++i)
                {
                    switch (modes[i])
                    {
                        case DISTORTION_CORRECTION_MODE_OFF:
                            str = "DISTORTION_CORRECTION_MODE_OFF";
                            break;
                        case DISTORTION_CORRECTION_MODE_FAST:
                            str = "DISTORTION_CORRECTION_MODE_FAST";
                            break;
                        case DISTORTION_CORRECTION_MODE_HIGH_QUALITY:
                            str = "DISTORTION_CORRECTION_MODE_HIGH_QUALITY";
                            break;
                        default:
                            str = "" + modes[i];

                    }
                    cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
                    cell[i].extra = modes[i];
                    if (mode == modes[i])
                        cell[i].value = 1;

                }
                return cell;
            }
            return null;
        }catch (Exception e)
        {
            return null;
        }


    }

    //Whether extended scene mode is enabled for a particular capture request.
    public CCell[] GetControlExtendedModes() throws CameraAccessException
    {

        //Overall mode of 3A (auto-exposure, auto-white-balance, auto-focus) control routines.
        try {
            CameraCharacteristics characteristics = Characteristics();
            int[] modes;
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.R)
            {
                modes = characteristics.get(CameraCharacteristics.CONTROL_AVAILABLE_SCENE_MODES);
                if (modes == null) return null;
                CCell [] cell = new CCell[modes.length];

                int mode = 0;
                mode = previewBuilder.get(CaptureRequest.CONTROL_EXTENDED_SCENE_MODE);
                String str = null;
                for (int i = 0; i < modes.length; ++i)
                {
                    switch (modes[i])
                    {
                        case CONTROL_EXTENDED_SCENE_MODE_DISABLED:
                            str = "CONTROL_EXTENDED_SCENE_MODE_DISABLED";
                            break;
                        case CONTROL_EXTENDED_SCENE_MODE_BOKEH_STILL_CAPTURE:
                            str = "CONTROL_EXTENDED_SCENE_MODE_BOKEH_STILL_CAPTURE";
                            break;
                        case CONTROL_EXTENDED_SCENE_MODE_BOKEH_CONTINUOUS:
                            str = "CONTROL_EXTENDED_SCENE_MODE_BOKEH_CONTINUOUS";
                            break;
                        default:
                            str = "" + modes[i];

                    }
                    cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
                    cell[i].extra = modes[i];
                    if (mode == modes[i])
                        cell[i].value = 1;

                }
                return cell;
            }
            return null;
        }catch (Exception e)
        {
            return null;
        }

    }
    //Overall mode of 3A (auto-exposure, auto-white-balance, auto-focus) control routines.
    public CCell[] GetControlModes() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M)
        {
            modes = characteristics.get(CameraCharacteristics.CONTROL_AVAILABLE_MODES);
            if (modes == null) return null;
            CCell [] cell = new CCell[modes.length];

            int mode = previewBuilder.get(CaptureRequest.CONTROL_MODE);
            String str = null;
            for (int i = 0; i < modes.length; ++i)
            {
                switch (modes[i])
                {
                    case CONTROL_MODE_OFF:
                        str = "CONTROL_MODE_OFF";
                        break;
                    case CONTROL_MODE_AUTO:
                        str = "CONTROL_MODE_AUTO";
                        break;
                    case CONTROL_MODE_USE_SCENE_MODE:
                        str = "CONTROL_MODE_USE_SCENE_MODE";
                        break;
                    case CONTROL_MODE_OFF_KEEP_STATE:
                        str = "CONTROL_MODE_OFF_KEEP_STATE";
                        break;
                    default:
                        str = "" + modes[i];

                }
                cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
                cell[i].extra = modes[i];
                if (mode == modes[i])
                    cell[i].value = 1;

            }
            return cell;
        }
        return null;
        }catch (Exception e)
        {
            return null;
        }

    }
    //Mode of operation for the lens distortion correction block.
    public CCell[] GetCorrectionModes() throws CameraAccessException
    {

        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
        {
            modes = characteristics.get(CameraCharacteristics.DISTORTION_CORRECTION_AVAILABLE_MODES);

            if (modes == null) return null;
            int mode = previewBuilder.get(CaptureRequest.DISTORTION_CORRECTION_MODE);
            CCell [] cell = new CCell[modes.length];

            String str = null;
            for (int i = 0; i < modes.length; ++i)
            {
                switch (modes[i])
                {
                    case CameraCharacteristics.DISTORTION_CORRECTION_MODE_HIGH_QUALITY:
                        str = "DISTORTION_CORRECTION_MODE_HIGH_QUALITY";
                        break;
                    case DISTORTION_CORRECTION_MODE_OFF:
                        str = "DISTORTION_CORRECTION_MODE_OFF";
                        break;

                    case  DISTORTION_CORRECTION_MODE_FAST:
                        str = "DISTORTION_CORRECTION_MODE_FAST";
                        break;


                    default:
                        str = "" + modes[i];

                }
                cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
                cell[i].extra = modes[i];
                if (mode == modes[i])
                    cell[i].value = 1;

            }
            return cell;
        }
        }catch (Exception e)
        {
            return null;
        }
        return null;
    }
    public CCell[] GetAWBMode() throws CameraAccessException
    {
        try {
            CameraCharacteristics characteristics = Characteristics();
            int[] modes ;
            modes = characteristics.get(CameraCharacteristics.CONTROL_AWB_AVAILABLE_MODES);

            if (modes == null) return null;

            CCell [] cell = new CCell[modes.length];
            int mode = previewBuilder.get(CaptureRequest.CONTROL_AWB_MODE);
            String str = null;
            for (int i = 0; i < modes.length; ++i)
            {
                switch (modes[i])
                {

                    case CONTROL_AWB_MODE_OFF:
                        str = "CONTROL_AWB_MODE_OFF";
                        break;

                    case CONTROL_AWB_MODE_AUTO:
                    str = "CONTROL_AWB_MODE_AUTO";
                    break;
                     case CONTROL_AWB_MODE_INCANDESCENT:
                    str = "CONTROL_AWB_MODE_INCANDESCENT";
                    break;


                    case CONTROL_AWB_MODE_FLUORESCENT:
                        str = "CONTROL_AWB_MODE_FLUORESCENT";
                        break;
                    case CONTROL_AWB_MODE_WARM_FLUORESCENT:
                        str = "CONTROL_AWB_MODE_WARM_FLUORESCENT";
                        break;
                    case  CONTROL_AWB_MODE_DAYLIGHT:
                        str = "CONTROL_AWB_MODE_DAYLIGHT";
                        break;


                    case CONTROL_AWB_MODE_CLOUDY_DAYLIGHT:
                        str = "CONTROL_AWB_MODE_CLOUDY_DAYLIGHT";
                        break;
                    case CONTROL_AWB_MODE_TWILIGHT:
                        str = "CONTROL_AWB_MODE_TWILIGHT";
                        break;


                    case CONTROL_AWB_MODE_SHADE:
                        str = "CONTROL_AWB_MODE_SHADE";
                        break;


                    default:
                        str = "" + modes[i];
                }
                cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
                cell[i].extra = modes[i];
                if (mode == modes[i])
                    cell[i].value = 1;
            }
            return cell;
        }catch (Exception e)
        {
            return null;
        }

    }

    //Whether the camera device will output the lens shading map in output result metadata.
    public CCell[] GetShadingLensModes() throws CameraAccessException
    {
        try {
        CameraCharacteristics characteristics = Characteristics();
        int[] modes;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
             modes = characteristics.get(STATISTICS_INFO_AVAILABLE_LENS_SHADING_MAP_MODES);
            //STATISTICS_LENS_SHADING_MAP_MODE
            if (modes == null) return null;
            CCell [] cell = new CCell[modes.length];
            int mode = previewBuilder.get(CaptureRequest.STATISTICS_LENS_SHADING_MAP_MODE);
            String str = null;
            for (int i = 0; i < modes.length; ++i)
            {
                switch (modes[i])
                {

                    case STATISTICS_LENS_SHADING_MAP_MODE_OFF:
                        str = "STATISTICS_LENS_SHADING_MAP_MODE_OFF";
                        break;
                    case STATISTICS_LENS_SHADING_MAP_MODE_ON:
                        str = "STATISTICS_LENS_SHADING_MAP_MODE_ON";
                        break;

                    default:
                        str = "" + modes[i];

                }
                cell[i] = new CCell(CELL_RADIOBUTTON, TEXT_READONLY, " " + str + " ");
                cell[i].extra = modes[i];
                if (mode == modes[i])
                    cell[i].value = 1;
            }
            return cell;
        }
        return null;
        }catch (Exception e)
        {
            return null;
        }

    }
    public HashMap<Object, MapModeKey > GetModes() throws CameraAccessException
    {

        HashMap<Object, MapModeKey > map = new HashMap<Object, MapModeKey >();
        CCell[] c = GetOpticalMode();
        if (c != null)
            map.put(CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE, new MapModeKey(c, "LENS_OPTICAL_STABILIZATION_MODE"));
        c =  GetShadingMode();
        if (c != null)
            map.put(CaptureRequest.SHADING_MODE, new MapModeKey(c, "SHADING_MODE"));
        c = GetAWBMode();
        if (c != null)
            map.put(CaptureRequest.CONTROL_AWB_MODE, new MapModeKey(c, "CONTROL_AWB_MODE"));
        c =  GetNoiceReductionMode();
        if (c != null)
            map.put(CaptureRequest.NOISE_REDUCTION_MODE, new MapModeKey(c, "NOISE_REDUCTION_MODE"));
        c = GetHotPixelMode();
        if (c != null)
            map.put(CaptureRequest.HOT_PIXEL_MODE, new MapModeKey(c, "HOT_PIXEL_MODE"));
        c = GetToneModes();
        if (c != null)
            map.put(CaptureRequest.TONEMAP_MODE, new MapModeKey(c, "TONEMAP_MODE"));
        c = GetEdgeMode();
        if (c != null)
            map.put(CaptureRequest.EDGE_MODE, new MapModeKey(c, "EDGE_MODE"));
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
        {
            c = GetCorrectionModes();
            if (c != null)
            {
                map.put(CaptureRequest.DISTORTION_CORRECTION_MODE, new MapModeKey(c, "DISTORTION_CORRECTION_MODE"));
            }
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
            c = GetControlModes();
            if (c != null)
            {
                map.put(CaptureRequest.CONTROL_MODE, new MapModeKey(c, "CONTROL_MODE"));
            }
        }
        c = GetParamSceneMode();
        if (c != null)
            map.put(CaptureRequest.CONTROL_SCENE_MODE, new MapModeKey(c, "CONTROL_SCENE_MODE"));
        c = GetShadingLensModes();
        if (c != null)
            map.put(CaptureRequest.STATISTICS_LENS_SHADING_MAP_MODE, new MapModeKey(c, "STATISTICS_LENS_SHADING_MAP_MODE"));
        c = GetControlModes();
        if (c != null)
            map.put(CaptureRequest.CONTROL_MODE, new MapModeKey(c, "CONTROL_MODE"));
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
        {
            c = GetCorrectionModes();
            if (c != null)
            {
                map.put(CaptureRequest.DISTORTION_CORRECTION_MODE, new MapModeKey(c, "DISTORTION_CORRECTION_MODE"));

            }
        }



        return map;
    }
    public void SetModes(HashMap<Object, MapModeKey > data) throws CameraAccessException
    {

        Set<Object> set = data.keySet();
        int i = 0;
        for (Object key: set)
        {
            try
            {
                ++i;
                CCell [] cells = data.get(key).key;
                boolean b = false;
                CaptureRequest.Key<Object> ckey =  (CaptureRequest.Key<Object>) key;
                for (CCell c: cells)
                {
                  if (c.value > 0)
                  {
                      b = true;
                      AGlobals.SaveIni("CAMERA" + cameraId, ckey.getName(), "" + c.extra, App.defaultPath + "/progini.ini");
                      cModes.put(key, c.extra);
                      break;
                  }
                }
                if (!b)
                    AGlobals.SaveIni("CAMERA" + cameraId, ckey.getName(), "0", App.defaultPath + "/progini.ini");


            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }

        set = cModes.keySet();
        for (Object key: set)
        {
            try
            {
                CaptureRequest.Key<Object> ckey =  (CaptureRequest.Key<Object>) key;
                previewBuilder.set(ckey, cModes.get(key));
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }

        //previewBuilder.set(CaptureRequest.SCALER_CROP_REGION, lastCrop);
        //session.setRepeatingRequest(previewBuilder.build(), previewCallback, handler);
        Scale(fLastZoom);

    }
    void ReadIni()
    {
        CaptureRequest.Key<Integer>[] keys = new CaptureRequest.Key[]{CaptureRequest.CONTROL_AWB_MODE, CaptureRequest.CONTROL_SCENE_MODE, CaptureRequest.EDGE_MODE, CaptureRequest.TONEMAP_MODE, CaptureRequest.HOT_PIXEL_MODE, CaptureRequest.NOISE_REDUCTION_MODE, CaptureRequest.SHADING_MODE,
                CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE, CaptureRequest.SHADING_MODE, CaptureRequest.STATISTICS_LENS_SHADING_MAP_MODE, CaptureRequest.CONTROL_MODE,
                CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE, CaptureRequest.CONTROL_MODE};
        CaptureRequest.Key<Integer> key;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P)
        {
            key = CaptureRequest.DISTORTION_CORRECTION_MODE;
            String n = AGlobals.ReadIni("CAMERA"+ cameraId, key.getName(),  App.defaultPath + "/progini.ini");
            if (n != null)
                cModes.put(key, Integer.valueOf(n.trim()));

        }
        for (CaptureRequest.Key<Integer> k: keys)
        {
            String n = AGlobals.ReadIni("CAMERA"+ cameraId, k.getName(),  App.defaultPath + "/progini.ini");
            if (n != null)
                cModes.put(k, Integer.valueOf(n.trim()));

        }
        if (cModes.size() > 0)
        {
            cModes.put(CaptureRequest.CONTROL_AWB_MODE,CameraCharacteristics.CONTROL_AWB_MODE_AUTO);
        }
    }

}