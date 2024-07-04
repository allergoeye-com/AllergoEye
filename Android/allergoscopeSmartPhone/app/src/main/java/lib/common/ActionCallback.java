package lib.common;
import android.os.Handler;
import android.os.Looper;
import android.os.Parcel;
import android.os.Parcelable;

import lib.utils.AGlobals;
import lib.utils.FileUtils;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;


/**
 * Created by alex on 23.01.2018.
 */

public class ActionCallback implements IActionCalbackClient, Runnable {
    protected Object parent = null;
    protected Method method = null;
    protected Class [] param = null;
    public String funcName = null;
    protected Object [] sendParams = null;
    protected final Object sync = new Object();
    static boolean CHECK_ACTION = true;

    //-----------------------------------
    //
    //-----------------------------------
    public ActionCallback()
    {

    }

    //-----------------------------------
    //
    //-----------------------------------
    public Object Parent() {
        return parent;
    }
    //-----------------------------------
    //
    //-----------------------------------
    public boolean IsEmty() {
        return parent == null;
    }

    //-----------------------------------
    //
    //-----------------------------------
    public void SetSendParams(Object ... _param) {
        sendParams = _param;
    }
    public ActionCallback(ActionCallback c)
    {
        parent = c.parent;
        method = c.method;
        if (c.param != null)
        {
            param = new Class[c.param.length];
            for (int i = 0; i < c.param.length; ++i)
                param[i] = c.param[i];
        }
        else
            c.param = null;
    }


    //-----------------------------------
    //
    //-----------------------------------
    public ActionCallback(Object _object, String func)
    {
        SetCallback(_object, func);
    }
    public ActionCallback(Object _object, String func, Class ...params)
    {
        SetCallback(_object, func, params);
    }

    //-----------------------------------
    //
    //-----------------------------------
    public boolean IsValid()
    {
        synchronized (sync)
        {
            return parent != null;
        }
    }
    public void Clear()
    {
        synchronized (sync)
        {
            parent = null;
            method = null;
            param = null;
        }
    }
    //-----------------------------------
    //
    //-----------------------------------
    public boolean SetCallback(Object _object, String func)
    {
        return SetCallback(_object, func, (Class[]) null);

    }

    //-----------------------------------
    //
    //-----------------------------------
    public boolean SetCallback(Object _object, String func, Class ...params)
    {


          synchronized (sync)
        {
            if (_object == null || func == null)
            {
                method = null;
                parent = null;
                funcName = null;

            }
            else
            {
                func = func.trim();

                param = params;
                Class<?>  cls = _object.getClass();
                try {
                    method = cls.getMethod(func, (Class<?>[])param);
                    if (method != null)
                        if (!method.isAccessible())
                            method.setAccessible(true);
                    if (CHECK_ACTION)
                    {
                        if (param != null)
                            for (int i = 0; i < param.length; ++i)
                                params[i] = CheckPrimitive(params[i]);
                    }
                    funcName = method.toString();


                }
                catch (NoSuchMethodException e)
                {
                    FileUtils.AddToLog(e);
                    method = null;
                    parent = null;
                    funcName = null;
                    return false;
                }
            }
            parent = method != null ? _object : null;
        }
        return parent != null;
    }
    private Class CheckPrimitive(Class cls)
    {

        if (cls.isPrimitive())
        {
            if (cls.equals(Integer.TYPE))
                cls = Integer.class;
            else
            if (cls.equals(Boolean.TYPE))
                cls = Boolean.class;
            else
            if (cls.equals(Long.TYPE))
                cls = Long.class;
            else
            if (cls.equals(Float.TYPE))
                cls = Float.class;
            else
            if (cls.equals(Double.TYPE))
                cls = Double.class;
            else
            if (cls.equals(Byte.TYPE))
                cls = Byte.class;
            else
            if (cls.equals(Short.TYPE))
                cls = Short.class;
            if (cls.equals(Character.TYPE))
                cls = Character.class;
            else
            if (cls.equals(Void.TYPE))
                cls = Void.class;

        }

        return cls;
    }
    //-----------------------------------
    //
    //-----------------------------------
    protected void OnException()
    {

    }
    //-----------------------------------
    //
    //-----------------------------------
    public void run(Looper looper, Object ... params)
    {
        if (looper != null)
        {
            sendParams = params;
            (new Handler(looper)).post(this);
        }
        else
            run(params);
    }
    //-----------------------------------
    //
    //-----------------------------------
    public void run(EventIdle looper, Object ... params)
    {
        if (looper != null)
        {
            sendParams = params;
            (new Handler(looper.GetLooper())).post(this);
        }
        else
            run(params);
    }
    //-----------------------------------
    //
    //-----------------------------------
    public void run(Looper looper)
    {
        (new Handler(looper)).post(this);
    }

    //-----------------------------------
    //
    //-----------------------------------
  
    public void run(Object ... _param)
    {
        _run(_param);
    }
    @SuppressWarnings("unchecked")
    private void _run(Object [] _param)
    {
        synchronized (sync)
        {
            if (parent != null && method != null)
            {
                if (AGlobals.currentActivity  instanceof AActivity)
                    ((AActivity)AGlobals.currentActivity).LockMouse(true);
                try
                {
                    try
                    {
                        if (CHECK_ACTION)
                        {
                            if (_param != null && param != null && _param.length == param.length)
                            {
                                for (int i = 0; i < param.length; ++i)
                                {
                                     if (_param[i] != null && !param[i].equals(_param[i].getClass())
                                     && !param[i].isAssignableFrom(_param[i].getClass()))
                                         return;
                                }

                            }
                            else
                            if (_param == null && (param != null) && !param[0].isPrimitive())
                            {
                                _param = new Object[1];
                                _param[0] = null;


                            }
                            else
                            if (_param != null || param != null)
                                return;
                        }
                        method.invoke(parent, _param);
                    }
                    catch (IllegalAccessException | InvocationTargetException e)
                    {
                        FileUtils.AddToLog(e);
                        OnException();
                    }
                }
                catch (Exception exception)
                {
                    FileUtils.AddToLog(exception);
                    OnException();
                }
                if (AGlobals.currentActivity  instanceof AActivity)
                    ((AActivity)AGlobals.currentActivity).LockMouse(false);
            }
        }
    }

    //-----------------------------------
    //
    //-----------------------------------
    @Override
    public void run()
    {
        if (param != null && sendParams == null)
            return;
        run(sendParams);
        sendParams = null;
    }
    //-----------------------------------
    //
    //-----------------------------------
    @Override
    public void Detach(ArrayList<Object> stack) {
        if (stack == null)
            stack = new ArrayList<Object>();

        if (!stack.contains(this))
        {
            synchronized (sync)
            {
                parent = null;
                method = null;
                param = null;
            }
        }

    }

    @Override
    public void Detach() {
        Detach(null);
    }

    //-----------------------------------
    //
    //-----------------------------------
    @Override
    public void Init() {

    }

}

