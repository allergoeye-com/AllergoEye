package lib.camera;

import android.hardware.HardwareBuffer;
import android.media.ImageReader;
import android.util.Log;

import lib.common.ActionCallback;
import lib.common.EventIdle;

public class CSaveFotoListner implements ImageReader.OnImageAvailableListener {
    private EventIdle wait;
    private ActionCallback action;

    public CSaveFotoListner(EventIdle _wait, ActionCallback actionCallback)
    {
        action = actionCallback;
        wait = _wait;
    }

    @Override
    public void onImageAvailable(ImageReader reader)
    {
        Log.e("CSaveFotoListner", "onImageAvailable");

        action.run(wait, reader);
    }
}
