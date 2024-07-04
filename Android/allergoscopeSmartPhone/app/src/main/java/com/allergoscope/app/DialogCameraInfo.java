package com.allergoscope.app;

import android.app.AlertDialog;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Point;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.util.Size;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.StringTokenizer;

import lib.camera.CameraCalculatePreview;
import lib.common.CDialog;
import lib.common.CEdit;
import lib.utils.AGlobals;


public class DialogCameraInfo extends CDialog {
    View view;

    public DialogCameraInfo()
    {
        super(AGlobals.currentActivity);
        view = LayoutInflater.from(getContext()).inflate(R.layout.test_camera, null);
        view.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        setView(view);
        AddButton(AlertDialog.BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);

    }

    @Override
    public void OnInitDialog()
    {

        CameraCharacteristics characteristics;

        int imageFormat = ImageFormat.JPEG;
        CameraManager mCameraManager = (CameraManager) AGlobals.contextApp.getSystemService(Context.CAMERA_SERVICE);
        if (mCameraManager != null)
            try
            {
                String[] list = mCameraManager.getCameraIdList();
                for (String id : list)
                {
                    characteristics = mCameraManager.getCameraCharacteristics(id);
                    Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);

                    if (facing == CameraCharacteristics.LENS_FACING_FRONT)
                    {
                        StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                        if (map == null)
                            continue;
                        CameraCalculatePreview.largest = null;
                        //new CF_Size( Collections.max(Arrays.asList(map.getOutputSizes(imageFormat)), new CameraCalculatePreview.CompareSizesByArea()));
                        Point displaySize = new Point();
                        AGlobals.currentActivity.getWindowManager().getDefaultDisplay().getSize(displaySize);

                        List<Size> sz = Arrays.asList(map.getOutputSizes(imageFormat));
                        Collections.sort(sz, new CameraCalculatePreview.CompareSizesByArea());

                        for (Size s : sz)
                        {
                            Trace("width =" + s.getWidth() + ", height = " + s.getHeight());

                        }


                    }

                }
            } catch (Exception e)
            {
            }


    }

    public void Trace(String msg)
    {
        if (msg == null)
            return;

        CEdit log = findViewById(R.id.inputText);
        String ss;
        if (!msg.contains("\r\n"))
            ss = msg + "\r\n";
        else
            ss = msg;

        log.append(ss);
        int scrollAmount = log.getLayout().getLineTop(log.getLineCount()) - log.getHeight();
        if (scrollAmount > 0)
            log.scrollTo(0, scrollAmount);
        else
            log.scrollTo(0, 0);
    }


}