package lib.common.layout;

import android.content.Context;
import android.graphics.Rect;
import android.util.AttributeSet;

import lib.common.IControlOnClick;
import lib.common.button.CMenuButton;
import lib.utils.AGlobals;

public class HandLayoutButton extends CMenuButton implements IControlOnClick {
    Rect clipRect = null;

    public HandLayoutButton(Context context)
    {
        super(context);
    }

    public HandLayoutButton(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public HandLayoutButton(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
    }

    @Override
    public boolean OnClick()
    {
        if (clipRect != null && !clipRect.contains(ScreenRect())) return false;
        callback.run();
        return true;
    }

    @Override
    public void SetClipRect(Rect _clipRect)
    {
        clipRect = _clipRect;
    }

    @Override
    public boolean IsEnableEvent() {
        return super.IsEnable();
    }

    @Override
    public Rect GetScreenRect()
    {
        int[] l = new int[2];
        getLocationOnScreen(l);
        Rect rect = new Rect((int) (l[0] - AGlobals.touchSlop * 2.5), l[1] - AGlobals.touchSlop * 2, (int) (l[0] + getWidth() + AGlobals.touchSlop * 2.5),
                l[1] + getHeight() + AGlobals.touchSlop * 2);
        return rect;
    }

    @Override
    public Rect ScreenRect()
    {
        return GetScreenRect();
    }


}
