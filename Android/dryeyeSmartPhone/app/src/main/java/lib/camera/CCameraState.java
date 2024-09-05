package lib.camera;

import android.hardware.camera2.CameraCaptureSession;
import android.util.Log;
import android.view.Surface;


import androidx.annotation.NonNull;

import lib.common.ActionCallback;
import lib.common.EventIdle;
import lib.common.MessageBox;

import static lib.common.CDialog.MB_CANCEL;

public class CCameraState extends CameraCaptureSession.StateCallback {
    private EventIdle wait;
    private ActionCallback action;

    public CCameraState(EventIdle _wait, ActionCallback actionCallback)
    {
        action = actionCallback;
        wait = _wait;
    }

    @Override
    public void onConfigured(@NonNull CameraCaptureSession session)
    {
        Log.e("CCameraState", "onConfigured");

        action.run(wait, session, "open");
    }

    @Override
    public void onConfigureFailed(@NonNull CameraCaptureSession session)
    {
        Log.e("CCameraState", "onConfigureFailed");
        action.run(wait, session, "error");
    }

    public void onReady(@NonNull CameraCaptureSession session)
    {
        Log.e("CCameraState", "onReady");
    }

    @Override
    public void onActive(@NonNull CameraCaptureSession session)
    {
        Log.e("CCameraState", "onActive");
    }

    @Override
    public void onClosed(@NonNull CameraCaptureSession session)
    {
        Log.e("CCameraState", "onClosed");
        action.run(wait, session, "close");
    }

    @Override
    public void onSurfacePrepared(@NonNull CameraCaptureSession session, @NonNull Surface surface)
    {
        Log.e("CCameraState", "onSurfacePrepared");
    }


}
