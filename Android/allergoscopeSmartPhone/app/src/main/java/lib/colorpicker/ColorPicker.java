package lib.colorpicker;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.LinearLayout;

import androidx.annotation.Nullable;

import lib.common.ActionCallback;
import lib.common.layout.LinearLayoutBorder;

public class ColorPicker extends LinearLayoutBorder {
    ColorPlane plane= null;
    CSliderLum lum = null;
    float _H = 180, _S = 50, _L = 50;
    boolean fSetSize = false;
    public ActionCallback oncolorChanged = null;
    public ActionCallback oncolorChanging = null;

    public ColorPicker(Context context) {
        super(context);
        InitAtribut(context, null, 0, 0);
    }

    public ColorPicker(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);
    }
    public ColorPicker(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs, defStyleAttr, 0);

    }
    public void SetRadius(int D)
    {
        plane.SetRadius(D);
    }
    //-------------------------------------------
    //
    //-------------------------------------------
    private void InitAtribut(Context context, AttributeSet attrs, int defA, int defS)
    {
        this.setOrientation(LinearLayout.HORIZONTAL);

    }

    public void InitRow()
    {
        if (plane != null && lum != null) return;

        for (int i = 0, l = getChildCount(); i < l; ++i)
        {

            View v = (View)getChildAt(i);
            if (v instanceof ColorPlane)
                plane = (ColorPlane)v;
            else
            if (v instanceof CSliderLum)
                lum = (CSliderLum)v;

        }

        if (plane == null || lum == null)
        {
            if (plane == null)
            addView(plane = new ColorPlane(getContext()));
            if (lum == null)
                addView(lum = new CSliderLum(getContext()));
        }
        fSetSize = true;
        lum.Init(this);
        plane.Init(this);

    }
    public int GetColor() { return HSL2RGB(_H, _S, _L);}
    public void SetColor(int color)
    {
        HSL(Color.red(color), Color.green(color), Color.blue(color));
    }
    int BOUND(int x, int min,int max)
    {
        return x < min ? min : (x > max ? max : x);
    }
    void HSL(int R, int G, int B)
    {

        float MAX = Math.max(R, Math.max(G, B));
        float MIN = Math.min(R, Math.min(G, B));
        float S, H = 0.0f;
        float L = (MAX + MIN)/2.f;
        L /= 255.f;
        if (MAX != MIN)
        {
            if (MAX == R && G >= B)
                H = 60.f * (float)(G - B)/(float)(MAX - MIN);
            else
            if (MAX == R && G < B)
                H = 60.f * (float)(G - B)/(float)(MAX - MIN) + 360.f;
            else
            if (MAX == G)
                H = 60.f * (float)(B - R)/(float)(MAX - MIN) + 120.f;
            else
            if (MAX == B)
                H = 60.f * (float)(R - G)/(float)(MAX - MIN) + 240.f;
            MAX /= 255.f;
            MIN /= 255.f;

            if (L > 0 && L <= 0.5)
                S = (MAX - MIN) /(2 * L);
            else
                S = (MAX - MIN) /(2.f - 2.f * L);
            S *= 100;
        }
        else
            H = S = 0.f;
        L *= 100.f;
        if (H != _H && L != _L && S != _S)
        {
            _H = H;
            _L = L;
            _S = S;
            if (lum != null && plane != null)
            {
                lum.Invalidate();
                plane.Invalidate();
            }
        }
    }

    int HSL2RGB(float H, float S, float L)
    {

        S /= 100.f;
        float L1 = L/100.f;
        float Q = L1 < 0.5f ? L1 + S * L1 : L1 + S - L1*S;
        float P = 2.f * L1  - Q;
        float Hk = H/360.f;
        float [] rgb = new float[3];
        rgb[0] = Hk + 1.f/3.f;
        rgb[1] = Hk;
        rgb[2] = Hk - 1.f/3.f;
        for (int i = 0; i < 3; ++i)
        {
            if (rgb[i] < 0) rgb[i] += 1.f;
            if (rgb[i] > 1.f) rgb[i] -= 1.f;
            if (rgb[i] < 1.f/6.f)
                rgb[i] = P + ((Q - P) * 6.f * rgb[i]);
            else
            if (rgb[i] >= 1.f/6.f && rgb[i] < 0.5f)
                rgb[i] = Q;
            else
            if (rgb[i] < 2.f/3.f && rgb[i] >= 0.5f)
                rgb[i] = P + ((Q - P)* (2.f/3.f - rgb[i]) * 6.f);
            else
                rgb[i] = P;
        }
        return Color.argb(255, BOUND((int)(rgb[0] * 255.f + 0.5f),0,255),
                BOUND((int)(rgb[1]  * 255.f + 0.5f),0,255),
                BOUND((int)(rgb[2]  * 255.f + 0.5f),0,255));

    }

    public float H()
    {
        return _H;
    }
    public float S()
    {
        return _S;
    }
    public float L()
    {
        return _L;
    }
    public void OnHS(float h, float s, int motionEvent)
    {
        if (_H != h || _S != s)
        {
            _H = h;
            _S = s;
            lum.Invalidate();
            plane.Invalidate();
            OnEvent(motionEvent);
        }
        else
        if (motionEvent != MotionEvent.ACTION_MOVE && motionEvent != MotionEvent.ACTION_DOWN)
            OnEvent(motionEvent);
    }
    public void OnEvent(int motionEvent)
    {
        if (motionEvent == MotionEvent.ACTION_MOVE || motionEvent == MotionEvent.ACTION_DOWN)
        {
            if (oncolorChanging != null)
                oncolorChanging.run(HSL2RGB(_H, _S, _L));


        }
        else
        if (oncolorChanged != null)
            oncolorChanged.run(HSL2RGB(_H, _S, _L));

    }
    public void OnL(float l, int motionEvent)
    {
        if (_L != l)
        {
            _L = l;
            lum.Invalidate();
            OnEvent(motionEvent);
        }
        else
        if (motionEvent != MotionEvent.ACTION_MOVE && motionEvent != MotionEvent.ACTION_DOWN)
            OnEvent(motionEvent);
    }
    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @SuppressLint("DrawAllocation")
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        InitRow();
        if (plane != null && lum != null)
        {

          /*  int touchSlop = ViewConfiguration.get(getContext()).getScaledTouchSlop();
            int w = right - left;
            int w2 = touchSlop * 2 + 2;
            plane.setLayoutParams(new  LayoutParams(w - w2,  LayoutParams.MATCH_PARENT ));
            lum.setLayoutParams(new  LayoutParams(w2,  LayoutParams.MATCH_PARENT));
            lum.InitPlane(w2, bottom - top);
            plane.InitPlane(w - w2, bottom - top);*/
        }
        super.onLayout(changed, left, top, right, bottom);

    }
    public static native void HLSPlane(Bitmap b, float X, float ScaleX, float ScaleY, int left, int top, int right, int bottom);
    public static native void HLSLum(Bitmap b, float H, float S, int left, int top, int right, int bottom);
}
