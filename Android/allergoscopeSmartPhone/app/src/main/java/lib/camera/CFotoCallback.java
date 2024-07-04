package lib.camera;

import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.os.Build;
import android.util.Log;


import androidx.annotation.NonNull;

import lib.common.ActionCallback;
import lib.common.EventIdle;
import lib.common.MessageBox;
import lib.utils.FileUtils;

import static lib.camera.CameraCalculatePreview.fPirke;
import static lib.common.CDialog.MB_CANCEL;

class CFotoCallback extends CameraCaptureSession.CaptureCallback {
    private EventIdle wait;
    private ActionCallback action;
    boolean fNotSupportFocuse;
    boolean fFotoAlways = true;
    public CFotoCallback(boolean _fNotSupportFocuse, EventIdle _wait, ActionCallback actionCallback)
    {
        fNotSupportFocuse = _fNotSupportFocuse;
        action = actionCallback;
        wait = _wait;
    }

    public void Disable()
    {
        action  = null;
    }
    @Override
    public void onCaptureCompleted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull TotalCaptureResult result)
    {
//        Log.e("CFotoCallback", "onCaptureCompleted");
        int state = 0;
        Integer stateAE = result.get(CaptureResult.CONTROL_AE_STATE);
        if (stateAE == null)
            stateAE = CaptureResult.CONTROL_AE_STATE_CONVERGED;
        Integer stateAF =  null;
        if (!fNotSupportFocuse)
            stateAF =  result.get(CaptureResult.CONTROL_AF_STATE);
        if (stateAF == null || stateAF == CaptureResult.CONTROL_AF_STATE_PASSIVE_FOCUSED || stateAF == CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED)
        {
            switch (stateAE)
            {
                case CaptureResult.CONTROL_AE_STATE_CONVERGED:
                    state = 2;
                    break;
              case CaptureResult.CONTROL_AE_STATE_FLASH_REQUIRED:
                  if (fFotoAlways)
                        state = 2;
                    break;

            }
        }
        if (fPirke)
        state = 2;
        if (action != null && state > 0)
        {
        action.run(wait, "open", result);
        }

    }

    @Override
    public void onCaptureFailed(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull CaptureFailure failure)
    {
        String error = "unknown";
        super.onCaptureFailed(session, request, failure);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
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
        }
            Log.e("CFotoCallback", error);

        FileUtils.AddToLog("ERROR onCaptureFailed :" + error);
    }

}