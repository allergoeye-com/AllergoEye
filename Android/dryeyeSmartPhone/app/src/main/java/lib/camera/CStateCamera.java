package lib.camera;

import android.hardware.camera2.CameraDevice;
import android.util.Log;

import androidx.annotation.NonNull;

import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.common.EventIdle;
import lib.common.MessageBox;
import lib.utils.FileUtils;

import static lib.common.CDialog.MB_CANCEL;

public class CStateCamera extends CameraDevice.StateCallback {
    private EventIdle wait;
    private ActionCallback actionCallback;
    public CameraDevice device;
    boolean fClosed = false;
    public String state = "close";
    String error = null;

    public CStateCamera(EventIdle _wait, ActionCallback _actionCallback)
    {
        device = null;
        actionCallback = _actionCallback;
        wait = _wait;
    }

    @Override
    public void onOpened(@NonNull CameraDevice camera)
    {
        device = camera;
        state = "open";
        Log.e("CStateCamera", "onOpened");

        actionCallback.run(wait, device, state);

    }

    @Override
    public void onDisconnected(@NonNull CameraDevice camera)
    {
        Log.e("CStateCamera", "onDisconnected");
        if (fClosed) return;
        if (camera.equals(device))
        {
            state = "close";
            fClosed = true;
            actionCallback.run(wait, device, state);
            device = null;
        }
    }

    @Override
    public void onError(@NonNull CameraDevice camera, int iError)
    {
        if (camera.equals(device))
        {
            switch (iError)
            {
                case CameraDevice.StateCallback.ERROR_CAMERA_DEVICE:
                    error = "Camera device has encountered a fatal error.";
                    break;
                case CameraDevice.StateCallback.ERROR_CAMERA_DISABLED:
                    error = "Camera device could not be opened due to a device policy.";
                    break;
                case CameraDevice.StateCallback.ERROR_CAMERA_IN_USE:
                    error = "Camera device is in use already.";
                    break;
                case CameraDevice.StateCallback.ERROR_CAMERA_SERVICE:
                    error = "Camera service has encountered a fatal error.";
                    break;
                case CameraDevice.StateCallback.ERROR_MAX_CAMERAS_IN_USE:
                    error = "Camera device could not be opened because there are too many other open camera devices.";
                    break;
            }
            state = "error";
            if (!fClosed)
            {
                fClosed = true;
                actionCallback.run(wait, device, state);
            }
            FileUtils.AddToLog("ERROR StateCamera :" + error);
            Log.e("CStateCamera", "ERROR StateCamera :" + error);

        }

    }
}
