package lib.common;

import android.graphics.Rect;

/**
 * Created by alex on 27.01.2018.
 */

public interface IControlOnClick {
    boolean OnClick();
    Rect ScreenRect();
    void SetClipRect(Rect _clipRect);
    boolean IsEnableEvent();

}
