package lib.camera;

import android.graphics.Rect;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.util.Log;


import androidx.annotation.NonNull;

import lib.common.ActionCallback;
import lib.common.EventIdle;
import lib.common.MessageBox;

import static lib.common.CDialog.MB_CANCEL;

class CPreviewCallback extends CameraCaptureSession.CaptureCallback {
    private EventIdle wait;
    private ActionCallback action;
    boolean fNotSupportFocuse;
    public int retOk = CCamera.CAMERA_FOCUSED;
    public int retScan = CCamera.CAMERA_SCAN;
    public CPreviewCallback(boolean _fNotSupportFocuse, EventIdle _wait, ActionCallback actionCallback)
    {
        action = actionCallback;
        wait = _wait;
        fNotSupportFocuse = _fNotSupportFocuse;
    }


    @Override
    public void onCaptureCompleted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull TotalCaptureResult result)
    {
        Integer mode = result.get(CaptureResult.STATISTICS_FACE_DETECT_MODE);
        if (mode != null) {
            android.hardware.camera2.params.Face[] faces = result.get(CaptureResult.STATISTICS_FACES);
            if (faces != null)
            {
                for (android.hardware.camera2.params.Face face : faces) {
                    Rect faceBounds = face.getBounds();
                }
            }
        }
        int state = 0;
        Integer stateAE = result.get(CaptureResult.CONTROL_AE_STATE);

        Integer stateAF =  result.get(CaptureResult.CONTROL_AF_STATE);
        if (stateAF == null)
        {
            if (stateAE == null)
                stateAF = CaptureResult.CONTROL_AF_STATE_PASSIVE_FOCUSED;


        }

        switch (stateAF)
        {
            case CaptureResult.CONTROL_AF_STATE_ACTIVE_SCAN:
            case CaptureResult.CONTROL_AF_STATE_PASSIVE_SCAN:
                state = retScan;
                    break;
            case CaptureResult.CONTROL_AF_STATE_PASSIVE_FOCUSED:
                state = retOk;
                break;

            case CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED:
                state = stateAE == CaptureResult.CONTROL_AE_STATE_CONVERGED ?   retOk : retScan;
        }
        action.run(wait, state);

    }

    @Override
    public void onCaptureFailed(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull CaptureFailure failure)
    {
        super.onCaptureFailed(session, request, failure);
        String error;

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
        Log.e("CPreviewCallback", error);
//        MessageBox box = new MessageBox(error, "PreviewCallback", MB_CANCEL);
//        box.Show();
    }

}