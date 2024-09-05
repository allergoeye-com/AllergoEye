package lib.common;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.webkit.WebView;

public class WebViewScroll extends WebView {
    public ActionCallback callback = null;
    public WebViewScroll(Context context)
    {
        super(context);
    }

    public WebViewScroll(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public WebViewScroll(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
    }

    public WebViewScroll(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes)
    {
        super(context, attrs, defStyleAttr, defStyleRes);
    }
    @Override
    protected void onScrollChanged(int l, int t, int oldl, int oldt)
    {
        super.onScrollChanged(l, t, oldl, oldt);
        int h = (computeVerticalScrollRange() - getHeight());
        int w = (computeHorizontalScrollRange() - getWidth());

        if (callback != null)
            callback.run(l, w, t, h);


    }
}
