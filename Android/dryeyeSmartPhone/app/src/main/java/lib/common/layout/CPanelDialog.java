package lib.common.layout;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import lib.common.IActionCalbackClient;
import lib.common.ActionCallback;
import lib.utils.FileUtils;

import java.util.ArrayList;

/**
 * Created by alex on 23.01.2018.
 */

public class CPanelDialog extends AnimatedLayout implements IActionCalbackClient {
    ActionCallback start = null;
    ActionCallback end = null;
    ActionCallback hide = null;
    boolean fStartHide = false;
    public boolean fDetachOld = true;
    protected boolean fStartExpand = false;
    public boolean fAutoClosed = true;
    boolean fDisableRequestLayout = false;

    public void DisableRequestLayout(boolean f)
    {
        fDisableRequestLayout = f;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public CPanelDialog(@NonNull Context context)
    {
        super(context);
        hand_size = 0;

    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public CPanelDialog(@NonNull Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
        hand_size = 0;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public CPanelDialog(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        hand_size = 0;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void SetSizeHand(int hand)
    {
        hand_size = hand;
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom)
    {
        super.onLayout(changed, left, top, right, bottom);
        if (fDisableRequestLayout) return;
        if (fStartExpand)
        {
            isVisible = false;
            fStartExpand = false;
            Init();
            if (!fStartHide)
                Expand(true, 1.5f);
            else
            {
                fStartHide = false;
                if (start != null)
                {
                    start.run();
                    start = null;
                }
            }
            fStartHide = false;
        } else if (getChildCount() == 1 && animator == null)
        {
            if (!isVisible)
            {

                if (end != null)
                {
                    if (fDetachOld)
                    {
                        while (getChildCount() > 0)
                        {
                            panel = getChildAt(0);
                            DetachChilds(panel);
                            removeView(panel);
                        }
                        panel = null;
                    }
                    SetScreenVisible(false, false);
                    hand_size = 0;
                    end.run();
                    end = null;
                }
                fStartExpand = false;
                fStartHide = false;

            } else
            {
                if (start != null)
                {
                    start.run();
                    start = null;
                }
            }

        }

    }

    //----------------------------------------------------------------------
    //
    //----------------------------------------------------------------------
    protected void DetachChilds(View test)
    {
        if (test instanceof IActionCalbackClient)
        {
            ArrayList<Object> stack = new ArrayList<Object>();
            stack.add(this);
            ((IActionCalbackClient) test).Detach(stack);
            return;
        } else if (test instanceof ViewGroup)
        {

            ViewGroup group = (ViewGroup) test;
            int N = group.getChildCount();
            for (int i = 0; i < N; ++i)
            {
                try
                {

                    View vv = group.getChildAt(i);
                    DetachChilds(vv);
                } catch (Exception e)
                {
                    FileUtils.AddToLog(e);
                }
            }
        }

    }

    @Override
    public void Detach(ArrayList<Object> stack)
    {
        if (stack == null)
            stack = new ArrayList<Object>();
        if (onKey != null)
            onKey = null;
        if (!stack.contains(this))
        {
            stack.add(this);
            if (start != null)
            {
                start.Detach(stack);
                start = null;
            }
            if (panel != null)
                panel = null;
            while (getChildCount() > 0)
            {
                View v = getChildAt(0);
                DetachChilds(v);
                removeView(v);

            }
            if (end != null)
            {
                end.run();
                end.Detach(stack);
                end = null;
            }
        }
    }

    @Override
    public void Detach()
    {
        Detach(null);
    }


    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void SetCallbacks(ActionCallback _start, ActionCallback _end)
    {
        start = _start;
        end = _end;

    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void SetCallbacks(ActionCallback _start, ActionCallback _end, ActionCallback _hide)
    {
        start = _start;
        end = _end;
        hide = _hide;

    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    public void Start(boolean _fStartShow)
    {
        if (fStartExpand) return;
        fDisableRequestLayout = false;
        fStartExpand = true;
        fStartHide = !_fStartShow;
        SetScreenVisible(true, false);

    }

    public void Start()
    {
        fAutoClosed = true;
        fDraging = true;
        Start(true);
    }


    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    public void OnStartHide()
    {
        if (hide != null)
            hide.run();
    }

    //-----------------------------------------------------
    //
    //-----------------------------------------------------
    @Override
    protected void SetState(boolean opened)
    {
        super.SetState(opened);
        //   this.setLeft(100);
    }

    //---------------------------------------------------------------------
    //
    //----------------------------------------------------------------------
    protected void Init(View test)
    {
        if (test == null) return;
        if (test instanceof IActionCalbackClient)
        {
            ((IActionCalbackClient) test).Init();
            return;
        } else if (test instanceof ViewGroup)
        {

            ViewGroup group = (ViewGroup) test;
            int N = group.getChildCount();
            for (int i = 0; i < N; ++i)
            {
                try
                {

                    View vv = group.getChildAt(i);
                    Init(vv);
                } catch (Exception e)
                {
                    FileUtils.AddToLog(e);
                }
            }
        }

    }

    @Override
    public void Init()
    {
        for (int i = 0, l = getChildCount(); i < l; ++i)
            Init(getChildAt(i));

    }

    public void Reset()
    {
        while (getChildCount() > 0)
        {
            panel = getChildAt(0);
            DetachChilds(panel);
            removeView(panel);
        }
        panel = null;
        SetScreenVisible(false, false);
        hand_size = 0;
        end = null;
        fStartExpand = false;
        fStartHide = false;
    }


}