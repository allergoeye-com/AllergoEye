package lib.common.layout;

import android.content.Context;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ScrollView;

import lib.common.ActionCallback;

public class CScrollView extends ScrollView implements View.OnLayoutChangeListener {
    Rect rc = new Rect(0, 0, 0, 0);
    public Rect screenRect = new Rect(0, 0, 0, 0);
    public ActionCallback onChangeLayout = null;
    public CScrollView(Context context) {
        super(context);
        this.addOnLayoutChangeListener(this);
    }

    public CScrollView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.addOnLayoutChangeListener(this);
    }

    public CScrollView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        this.addOnLayoutChangeListener(this);
    }
    @Override
    protected void onScrollChanged(int l, int t, int oldl, int oldt) {

        super.onScrollChanged(l, t, oldl, oldt);
    }
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        int[] l = new int[2];
        getLocationOnScreen(l);
        screenRect.left = left + l[0];
        screenRect.top = top + l[1];
        screenRect.right = right + l[0];
        screenRect.bottom = bottom + l[1];

        super.onLayout(changed, left, top, right, bottom);
       /* if (onChangeLayout != null)
            onChangeLayout.run(screenRect);
            */

    }

    @Override
    public void onLayoutChange(View v, int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight, int oldBottom) {
       /* int[] l = new int[2];
        v.getLocationOnScreen(l);
        screenRect.left = left + l[0];
        screenRect.top = top + l[1];
        screenRect.right = right + l[0];
        screenRect.bottom = bottom + l[1];
*/
        if (onChangeLayout != null)
            onChangeLayout.run(screenRect);
    }
    /*@Override
    public boolean dispatchTouchEvent(MotionEvent ev)
    {

        int pointerIndex = ev.getActionIndex();
        float x = ev.getX(pointerIndex);
        float y = ev.getY(pointerIndex);
        if (screenRect == null)
            return super.dispatchTouchEvent(ev);
        if (x >= (screenRect.left - AGlobals.touchSlop/2) && y >= (screenRect.top -  AGlobals.touchSlop/2) &&
                x <= (screenRect.right + AGlobals.touchSlop/2) && y <= (screenRect.bottom + AGlobals.touchSlop/2))
            return super.dispatchTouchEvent(ev);
        return false;
    }
    */
}