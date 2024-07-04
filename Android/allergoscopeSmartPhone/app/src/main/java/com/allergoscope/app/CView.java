package com.allergoscope.app;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.PointF;
import android.os.Build;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;

import com.allergoscope.app.roi.SelectedBlockIcon;

import java.util.HashMap;

import lib.common.CWnd;
import lib.common.ScaleListener;

public class CView   extends CWnd implements View.OnTouchListener {
    private boolean fCapture = false;
    private int iClick = 0;
    private ScaleListener.SCALE scale = new ScaleListener.SCALE();
    private HashMap<Integer, CurPoint > history;
    private ScaleGestureDetector mScaleDetector;
    boolean fPointInSelect = false;
    boolean fClickedCornerPoint = false;
    public Point startPoint = new Point(0, 0);
    public Point curPoint = new Point(0, 0);

    public CView(Context context, long _cObject, Bitmap surface) {
        super(context, _cObject, surface);

    }

    public CView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public CView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }
    //------------------------------
    //
    //------------------------------
    @Override
    public void Init()
    {
        history = new HashMap<Integer, CurPoint >();
        this.setOnTouchListener(this);
        mScaleDetector = new ScaleGestureDetector(getContext(), new ScaleListener(scale));
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            mScaleDetector.setStylusScaleEnabled(false);
        }
        super.Init();

    }
    //------------------------------
    //
    //------------------------------
    public void  Invalidate()
    {
        OnPaint();
        OnDraw();


    }

    //------------------------------
    //
    //------------------------------
    @Override
    public boolean onTouch(View v, MotionEvent event)
    {
        int action = event.getAction();
        if (!fCapture && (action & MotionEvent.ACTION_MASK) != MotionEvent.ACTION_DOWN)
            return false;
        mScaleDetector.onTouchEvent(event);

        switch (action & MotionEvent.ACTION_MASK)
        {
            case MotionEvent.ACTION_DOWN:
            {

                fCapture = SetCapture();
                int id = event.getPointerId(0);
                iClick = id;
                fClickedCornerPoint = false;
                startPoint.x = (int) event.getX(0);
                startPoint.y = (int) event.getY(0);
                curPoint.x = (int) event.getX(0);
                curPoint.y = (int) event.getY(0);

                history.put(id, new CurPoint(event.getX(0), event.getY(0)));
                OnMouseDown(event.getX(0), event.getY(0));
                Invalidate();

            }
            break;
            case MotionEvent.ACTION_UP:
            {

                int id = event.getPointerId(0);
                history.get(id).Push(id, event.getX(0), event.getY(0));
                history.get(id).Clear();
                history.remove(id);
                fCapture = false;
                ReleaseCapture();

                if (iClick != -1)
                {
                    if (fClickedCornerPoint)
                        OnMouseUp(event.getX(0), event.getY(0));
                }
                iClick = -1;
                Invalidate();
                break;
            }

            case MotionEvent.ACTION_POINTER_DOWN:
            {
                int index = event.getActionIndex();
                int id = event.getPointerId(index);
                if (iClick != -1 && fClickedCornerPoint)
                    OnMouseUp(event.getX(0), event.getY(0));


                history.put(id, new CurPoint(event.getX(index), event.getY(index)));
                iClick = -1;

            }
            break;
            case MotionEvent.ACTION_CANCEL:
            {
                ReleaseCapture();
                history.clear();;
                fCapture = false;
                return true;

            }
            case MotionEvent.ACTION_POINTER_UP:
            {

                int index = event.getActionIndex();
                int id = event.getPointerId(index);
                history.get(id).Push(id, event.getX(index), event.getY(index));
                history.get(id).Clear();
                history.remove(id);
                break;
            }
            case MotionEvent.ACTION_MOVE:
            {
                int N = event.getPointerCount();
                if (N > history.size())
                {
                    ReleaseCapture();
                    history.clear();
                    fCapture = false;
                    return true;

                }

                CurPoint pt;

                for (int i = 0; i < N; ++i)
                {

                    int id = event.getPointerId(i);
                    pt = history.get(id);
                    if (pt != null)
                    {
                        pt.Push(id, event.getX(i), event.getY(i));
                        if (N > 1 && scale.IsScale())
                            pt.Apply();
                    }
                }
                if (scale.IsScale())
                {
                    ScalePt(scale.GetScale(), scale.focus.x, scale.focus.y);
                    for (int i = 0; i < N; ++i)
                    {

                        int id = event.getPointerId(i);
                        pt = history.get(id);
                        if (pt != null)
                            pt.Apply();

                    }
                    Invalidate();
                }
                else
                if (N == 1)
                {
                    curPoint.x = (int) event.getX(0);
                    curPoint.y = (int) event.getY(0);
                    pt = history.get(event.getPointerId(0));
                    if (Math.abs(pt.dx()) > 5.0f || Math.abs(pt.dy()) > 5.0f)
                    {
                        Offset(pt.dx(), pt.dy());
                        Invalidate();
                        pt.Apply();
                    }

                }

                break;
            }
        }
        return true;

    }
    private boolean SGN(float a) { return a >= 0.0f; }
    public void setSize(int w, int h)
    {

        //if (bitmap == null || bitmap.getWidth() != w || bitmap.getHeight() != h)
        {
            //bitmap = OnSetSize(w, h);
            setWillNotDraw(fThreadDisable);


        }
    }
    //************************CurPoint********************************************
    class CurPoint {
        PointF fFirst;
        PointF fLast;
        //------------------------------
        //
        //------------------------------
        CurPoint(float x, float y)
        {
            fFirst = new PointF(x, y);
            fLast = new PointF(x, y);

        }
        //------------------------------
        //
        //------------------------------
        void Clear()
        {
            fFirst = null;
            fLast = null;

        }
        //------------------------------
        //
        //------------------------------
        boolean Check(float x, float y)
        {
            return  (Math.abs(fLast.x - x) > 1.0f || Math.abs(fLast.y - y) > 1.0);
        }
        //------------------------------
        //
        //------------------------------
        float dx() { return fLast.x - fFirst.x; }
        //------------------------------
        //
        //------------------------------
        float dy() { return fLast.y - fFirst.y; }
        //------------------------------
        //
        //------------------------------
        void Push(int id, float x, float y)
        {
            fLast.x = x;
            fLast.y = y;

        }
        //------------------------------
        //
        //------------------------------
        void Apply()
        {
            fFirst.x = fLast.x;
            fFirst.y = fLast.y;
        }
    }
    private native boolean ScalePt(double scale, float x, float y);
    private native boolean Offset(float x, float y);
    private native void OnMouseDown(float x, float y);
    private native void OnMouseUp(float x, float y);
    public native boolean OnOpenDocument(Bitmap bmp);
    public native boolean OnOpenRoi(String filename, boolean fView);
    public native SelectedBlockIcon GetRoi(int xsize, int ysize);
    public native boolean UpdateRoi(SelectedBlockIcon roi);
    public native boolean UpdateRoiIcon(Bitmap roi, String name, int color);
    private native Bitmap OnSetSize(int w, int h);



    private native void OnPaint();


}

