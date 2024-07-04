package lib.common;

import java.io.IOException;
import java.util.HashMap;

/**
 * Created by alex on 25.11.17.
 */

public class CThread extends CObject {
    protected long IDThread = 0;
    protected HashMap<Integer, Object> data = new HashMap<Integer, Object>();
    protected Object sync = new Object();
    public INotify parent = null;

    public CThread(long _cObject) {
        super(_cObject);
    }

    //----------------------------------------------
    // добавить задачу в MSGThread (jni/maindll/utils/MSGThread.h)
    //----------------------------------------------
    public  boolean Submit(int id, Object obj)
    {

        synchronized(sync)
        {
            data.put(id, obj);
        }
        return _Submit(ID, id, obj);

    }

    //----------------------------------------------
    // вызов из c++
    //----------------------------------------------
    public void OnResult(int id, Object _data)
    {
        try {
            Object object;
            synchronized(sync)
            {
                object = data.get(id);
            }
                if (object != null && parent != null)
                {
                    parent.OnNotify(object, _data);
                }
            synchronized(sync)
            {
                data.remove(id);
            }
        }catch(Exception e)
        {
            return;
        }
    }
    //----------------------------------------------
    // вызов из c++
    //----------------------------------------------
    public void OnResult(int id, Object [] _data)
    {
        try {
            Object object;
            synchronized(sync)
            {
                object = data.get(id);
            }
            if (object != null && parent != null)
                parent.OnNotify(object, _data);
            synchronized(sync)
            {
                data.remove(id);
            }
        }catch(Exception e)
        {
            return;
        }
    }

    public boolean Start()
    {
        return _Start(ID);
    }
    //----------------------------------------------
    //
    //----------------------------------------------
    @Override
    public void close() throws IOException {
        parent = null;
        _Stop(ID);
        super.close();
    }

//**************************************************************
    native protected boolean _Start(long idObject);
    native protected boolean _Submit(long idObject, int id, Object obj);
    native protected boolean _Stop(long idObject);

}
