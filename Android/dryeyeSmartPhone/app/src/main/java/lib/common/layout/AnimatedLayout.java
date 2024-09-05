package lib.common.layout;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.app.Activity;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.os.Build;
import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.core.view.ViewCompat;

import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.DecelerateInterpolator;
import android.widget.FrameLayout;

import com.dryeye.app.R;

import lib.common.ActionCallback;

import lib.utils.AGlobals;
import lib.utils.FileUtils;

import java.util.ArrayList;

/**
 * Created by alex on 17.01.2018.
 */

public class AnimatedLayout extends FrameLayout {

    protected enum Direct {
        Left,
        Top,
        Right,
        Bottom;
    }

    boolean fLock = false;
    protected boolean fDraging = true;
    protected Direct orient = Direct.Left;
    protected boolean orientVH;
    protected boolean willNotDraw = true;
    protected View panel = null;
    protected boolean isVisible = true;
    protected float startPos;
    protected int hand_size = 20;
    protected boolean VERT = true;
    protected boolean HORZ = false;
    protected boolean LeftToRight = true;
    protected int curentVisibilety = View.VISIBLE;
    protected Rect screen = new Rect(0, 0, 0, 0);
    protected DecelerateInterpolator sDecelerator = new DecelerateInterpolator();
    protected VelocityTracker velocityTracker = null;
    protected ObjectAnimator animator = null;
    protected Object sync = new Object();
    protected Integer idToDrag;
    protected ActionCallback onKey = null;
    protected ArrayList<Runnable> onhide;
    protected ArrayList<Runnable> onshow;

    public void SetFlagIsVisible(boolean b)
    {
        SetState(b);
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public AnimatedLayout(@NonNull Context context)
    {
        super(context);

        InitAtribut(context, null, 0, 0);
    }

    //-----------------------------------------------------
//
//-----------------------------------------------------
    public AnimatedLayout(@NonNull Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
        InitAtribut(context, attrs, 0, 0);
    }

    //-----------------------------------------------------
//
//-----------------------------------------------------
    public AnimatedLayout(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        InitAtribut(context, attrs,  defStyleAttr, 0);
//        LayoutParams params = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        //      setLayoutParams(params);
    }

    @Override
    public void setLayoutParams(ViewGroup.LayoutParams params)
    {
        super.setLayoutParams(params);
    }

    //-----------------------------------------------------
//
//-----------------------------------------------------
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public AnimatedLayout(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes)
    {
        super(context, attrs, defStyleAttr, defStyleRes);
        InitAtribut(context, attrs, defStyleAttr, defStyleRes);
    }

    public boolean SetOrient(String dir)
    {
        try
        {
            orient = dir == null ? Direct.Bottom : Direct.valueOf(dir);
            SetOrient();
        } catch (Exception e)
        {
            return false;
        }
        return true;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void EnableDraging(boolean b)
    {
        fDraging = b;
    }

    public boolean IsDragingEnable()
    {

        return fDraging;

    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void Lock(boolean b)
    {
        fLock = b;
    }

    public boolean IsLock()
    {
        return fLock;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void SetOrient()
    {

        switch (orient)
        {
            case Top:
                LeftToRight = true;
                orientVH = VERT;
                break;
            case Bottom:
                LeftToRight = false;
                orientVH = VERT;
                break;
            case Left:
                LeftToRight = true;
                orientVH = HORZ;
                break;
            case Right:
                orientVH = HORZ;
                LeftToRight = false;
                break;
        }
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void InitAtribut(Context context, AttributeSet attr, int defA, int defS)
    {
        if (attr != null)
        {
            TypedArray a = context.getTheme().obtainStyledAttributes(attr, R.styleable.AnimatedLayout, defA, defS);
            try
            {
                hand_size = (int) a.getDimensionPixelSize(R.styleable.AnimatedLayout_size_hand, 0);
                String dir = a.getString(R.styleable.AnimatedLayout_drag_orient);
                orient = dir == null ? Direct.Bottom : Direct.valueOf(dir);


                idToDrag = a.getResourceId(R.styleable.AnimatedLayout_idToDrag, 0);
                isVisible = a.getBoolean(R.styleable.AnimatedLayout_isVisible, true);


            } catch (Exception e)
            {
                ;
            }
            a.recycle();
        } else
            hand_size = 0;
        onhide = new ArrayList<Runnable>();
        onshow = new ArrayList<Runnable>();

        SetOrient();
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public Rect GetScreenRect()
    {

        Rect rc = screen;
        if (panel != null)
        {
            int[] l = new int[2];
            panel.getLocationOnScreen(l);
            int height = panel.getMeasuredHeight();
            int width = panel.getMeasuredWidth();
            rc.left = l[0];
            rc.top = l[1];
            if (!isVisible)
            {
                switch (orient)
                {
                    case Top:
                        rc.right = rc.left + width;
                        rc.bottom = rc.top + hand_size;
                        break;
                    case Left:
                    case Right:
                        rc.right = rc.left + hand_size;
                        rc.bottom = rc.top + height;
                        break;

                }

            } else
                rc = new Rect(l[0], l[1], l[0] + width, l[1] + height);
        }

        return rc;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void Hide()
    {
        if (fLock) return;

        Expand(false, 1.8f);
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void Show()
    {
        if (!isVisible)
            Expand(true, 1.8f);
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void OnStartHide()
    {
    }

    public void OnStartShow()
    {
    }

    protected Rect GetRectVisible()
    {

        Rect rc = new Rect(0, 0, screen.width(), screen.height());
        if (panel != null)
        {
            int[] l = new int[2];
            getLocationOnScreen(l);
            int[] l1 = new int[2];
            panel.getLocationOnScreen(l1);

            l[0] = l1[0] - l[0];
            l[1] = l1[1] - l[1];
            int height = panel.getMeasuredHeight();
            int width = panel.getMeasuredWidth();
            rc.left = l[0];
            rc.top = l[1];
            rc.right = rc.left + width;
            rc.bottom = rc.top + height;
            if (!isVisible)
            {
                switch (orient)
                {
                    case Top:
                        rc.bottom += hand_size;
                        break;

                    case Bottom:
                        rc.top -= hand_size;
                        break;
                    case Left:
                        rc.right += hand_size;
                        break;

                    case Right:
                        rc.left -= height;
                        break;

                }

            }

        }
        return rc;
    }

    public void SetCallbakOnKeyRun(ActionCallback c)
    {
        onKey = c;
    }

    @Override
    public boolean dispatchKeyEventPreIme(KeyEvent event)
    {

        if (event != null)
            if (onKey != null)
            {
                onKey.run(event);
                return true;
            }
        return false;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        super.onLayout(changed, left, top, right, bottom);
        screen.left = (int) (left + getTranslationX());
        screen.right = (int) (right + getTranslationX());
        screen.top = (int) (top + getTranslationY());
        screen.bottom = (int) (bottom + getTranslationY());
        if (getChildCount() < 1)
            return;

        if (panel == null && idToDrag != 0)
            panel = ((Activity) this.getContext()).findViewById(idToDrag);


        if (panel == null)
            panel = getChildAt(0);
        left = 0;
        top = 0;
        if (panel != null)
        {
            if (!isVisible)
            {

                int height = panel.getMeasuredHeight();
                int width = panel.getMeasuredWidth();

                switch (orient)
                {
                    case Top:
                        panel.layout(left, top - height + hand_size, right, hand_size);
                        break;
                    case Bottom:
                        panel.layout(left, bottom - hand_size, right, bottom + height - hand_size);
                        break;
                    case Left:
                        panel.layout(left - width + hand_size, top, hand_size, bottom);
                        break;
                    case Right:
                        panel.layout(width - hand_size, top, 2 * width - hand_size, bottom);
                        break;

                }
                OnHide();

            } else
            {
                panel.layout(left, top, right, bottom);
                OnShow();
            }
        }

    }


    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void StartDrag(MotionEvent event)
    {
        startPos = orientVH == VERT ? event.getY() : event.getX();
        if (velocityTracker == null)
            velocityTracker = VelocityTracker.obtain();
        if (velocityTracker != null)
        {
            panel.setVisibility(View.VISIBLE);
            velocityTracker.addMovement(event);
            setWillNotDraw(willNotDraw);
        }
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void Dragging(MotionEvent event)
    {
        if (panel != null && velocityTracker != null)
        {
            velocityTracker.addMovement(event);

            float offset = (orientVH ? event.getY() : event.getX()) - startPos;
            if (!LeftToRight)
                offset *= -1.0f;

            float size = orientVH == VERT ? panel.getMeasuredHeight() : panel.getMeasuredWidth();

            if (isVisible)
            {
                if (offset > 0)
                    offset = 0;
                if (Math.abs(offset) >= size - hand_size)
                    offset = -size + hand_size;
            } else
            {
                if (offset < 0)
                    offset = 0;
                if (offset >= size - hand_size)
                    offset = size - hand_size;
            }
            float dist = LeftToRight ? offset : -offset;
            if (orientVH == VERT)
                panel.setTranslationY(dist);
            else
                panel.setTranslationX(dist);

            if (!willNotDraw)
                ViewCompat.postInvalidateOnAnimation(this);
        }
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void EndDrag(MotionEvent event)
    {
        if (velocityTracker != null)
        {
            velocityTracker.addMovement(event);
            velocityTracker.computeCurrentVelocity(1);
            float velocity = orientVH == VERT ? velocityTracker.getYVelocity() : velocityTracker.getXVelocity();
            velocityTracker.recycle();
            velocityTracker = null;
            Expanding(velocity);
        }
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    public boolean onInterceptTouchEvent(MotionEvent event)
    {
        return false;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        synchronized (sync)
        {

            if (fLock)
                return super.onTouchEvent(event);
            if (panel == null || animator != null) return true;

            if (hand_size != 0 && velocityTracker == null)
            {
                Rect rect = GetRectVisible();
                final int pointerIndex = event.getActionIndex();
                final float x = event.getX(pointerIndex) - panel.getTranslationX();
                final float y = event.getY(pointerIndex) - panel.getTranslationY();
                if (x < (rect.left - AGlobals.touchSlop) || y < (rect.top - AGlobals.touchSlop) || x > (rect.right + AGlobals.touchSlop) || y > (rect.bottom + AGlobals.touchSlop))
                    return false;
            }
            if (!fDraging)
            {
                Rect rect = GetRectVisible();
                final int pointerIndex = event.getActionIndex();
                final float x = event.getX(pointerIndex) - panel.getTranslationX();
                final float y = event.getY(pointerIndex) - panel.getTranslationY();
                if (x < rect.left || y < rect.top || x > rect.right || y > rect.bottom)
                    return false;
                return true;
            }
            int action = event.getAction();
            switch (action & MotionEvent.ACTION_MASK)
            {
                case MotionEvent.ACTION_DOWN:
                    StartDrag(event);
                    break;
                case MotionEvent.ACTION_MOVE:
                    Dragging(event);
                    break;
                case MotionEvent.ACTION_CANCEL:
                case MotionEvent.ACTION_UP:
                    EndDrag(event);
                    break;
                default:
                    ;
            }
        }
        return true;
    }
    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void SetScreenVisible(boolean fLayOut, boolean fChilds)
    {
        int v = fLayOut ? View.VISIBLE : View.GONE;
        isVisible = fChilds;

        curentVisibilety = v;
        setVisibility(curentVisibilety);

    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public boolean Expand(boolean fOpen, float speed)
    {
        synchronized (sync)
        {

            if (panel == null || animator != null)
            {
                return false;
            }
            if (!fOpen) OnStartHide();
            else OnStartShow();
            boolean closed;
            float dist;
            long duration;
            float size_panel = orientVH == VERT ? panel.getMeasuredHeight() : panel.getMeasuredWidth();
            float trans = orientVH == VERT ? panel.getTranslationY() : panel.getTranslationX();
            closed = !fOpen;
            if (LeftToRight)
            {
                if (!isVisible)
                    dist = closed ? trans : (size_panel - hand_size - trans);

                else
                    dist = closed ? -(size_panel - hand_size + trans) : -trans;
            } else
            {
                if (!isVisible)
                    dist = closed ? 0 : -trans + hand_size - size_panel;
                else
                    dist = closed ? size_panel - hand_size - trans : 0;
            }
            duration = Math.abs(Math.round(dist / speed));
            int[] l = new int[2];
            getLocationOnScreen(l);
            int off = orientVH == VERT ? l[1] : l[0];
            if (trans + dist != trans)
            {
                animator = ObjectAnimator.ofFloat(panel, orientVH == VERT ? View.TRANSLATION_Y : View.TRANSLATION_X, trans, trans + dist);
                AnimatorSet set = new AnimatorSet();
                set.playTogether(animator);
                set.setDuration(duration);
                set.setInterpolator(sDecelerator);
                set.addListener(new AnimatedListener(closed));
                set.start();
            }
            return true;
        }
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void Expanding(float velocity)
    {
        if (panel == null) return;
        final boolean draging = Math.abs(velocity) < 0.5;
        boolean closed;
        float dist;
        long duration;
        float trans = orientVH == VERT ? panel.getTranslationY() : panel.getTranslationX();
        float size = orientVH == VERT ? panel.getMeasuredHeight() : panel.getMeasuredWidth();

        if (!draging)
        {
            closed = velocity < 0;
            if (!closed) OnStartHide();
            else OnStartShow();

            if (LeftToRight)
            {
                if (!isVisible)
                    dist = closed ? trans : (size - hand_size - trans);

                else
                    dist = closed ? -(size - hand_size + trans) : -trans;
            } else
            {
                closed = !closed;
                if (!isVisible)
                    dist = closed ? 0 : -trans + hand_size - size;
                else
                    dist = closed ? size - hand_size - trans : 0;
            }
            duration = Math.abs(Math.round(dist / velocity));
        } else
        {
            if (LeftToRight)
            {
                closed = Math.abs(trans) >= (size - hand_size) / 2;
                if (!isVisible)
                    dist = closed ? size - hand_size - trans : -trans;
                else
                    dist = closed ? -(size - hand_size + trans) : -trans;
                if (!isVisible)
                    closed = !closed;
            } else
            {
                closed = Math.abs(trans) <= (size - hand_size) / 2;
                if (!isVisible)
                    dist = closed ? hand_size - trans : -trans + hand_size - size;
                else
                    dist = closed ? -trans : size - hand_size - trans;
                if (isVisible)
                    closed = !closed;

            }
            duration = Math.round(500 * (double) Math.abs((double) trans)
                    / (double) (size - hand_size));
        }
        ObjectAnimator animator;
        animator = ObjectAnimator.ofFloat(panel, orientVH == VERT ? View.TRANSLATION_Y : View.TRANSLATION_X, trans, trans + dist);
        AnimatorSet set = new AnimatorSet();
        set.playTogether(animator);
        set.setDuration(duration);
        set.setInterpolator(sDecelerator);
        set.addListener(new AnimatedListener(closed));
        set.start();
    }

    public boolean IsVisible()
    {
        return isVisible;
    }

    @Override
    protected void finalize() throws Throwable
    {
        FileUtils.AddToLog("AnimatedLayout->FINALIZE");
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void AddOnHideAction(Runnable r)
    {
        onhide.add(r);

    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void AddOnShowAction(Runnable r)
    {
        onshow.add(r);
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void OnHide()
    {

        for (Runnable r : onhide)
            AGlobals.rootHandler.Send(r, 500);
        onhide.clear();
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void OnShow()
    {
        Handler h = new Handler();
        for (Runnable r : onshow)
            AGlobals.rootHandler.Send(r);
        onshow.clear();
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    protected void SetState(boolean opened)
    {

        isVisible = opened;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    public void draw(Canvas canvas)
    {
        super.draw(canvas);

        if (!willNotDraw)
            ViewCompat.postInvalidateOnAnimation(AnimatedLayout.this);

    }

    //***************************************************************************
//
//***************************************************************************
    class AnimatedListener implements Animator.AnimatorListener {

        int oldLayerType;
        boolean fOpen;

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        public AnimatedListener(boolean opened)
        {
            super();
            fOpen = opened;
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void onAnimationStart(Animator animation)
        {
            oldLayerType = panel.getLayerType();
            panel.setLayerType(View.LAYER_TYPE_HARDWARE, null);
            panel.setVisibility(View.VISIBLE);

            if (!willNotDraw)
                ViewCompat.postInvalidateOnAnimation(AnimatedLayout.this);
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void onAnimationRepeat(Animator animation)
        {
        }

        @Override
        public void onAnimationEnd(Animator animation)
        {
            synchronized (sync)
            {
                SetState(!fOpen);
                if (panel != null)
                {
                    panel.setTranslationY(0);
                    panel.setTranslationX(0);
                    panel.setLayerType(oldLayerType, null);

                    requestLayout();

                    if (!willNotDraw)
                        ViewCompat.postInvalidateOnAnimation(AnimatedLayout.this);

                }
                animator = null;
            }
        }

        //-----------------------------------------------------
        //
        //-----------------------------------------------------
        @Override
        public void onAnimationCancel(Animator animation)
        {
            if (!willNotDraw)
                ViewCompat.postInvalidateOnAnimation(AnimatedLayout.this);

        }

    }
}
