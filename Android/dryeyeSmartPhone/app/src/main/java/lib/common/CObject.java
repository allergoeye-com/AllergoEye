package lib.common;

import lib.utils.AGlobals;
import lib.utils.GlobalRef;

import java.io.Closeable;
import java.io.IOException;

/**
 * Created by alex on 01.12.17.
 */

public class CObject implements Closeable {
    public long ID = 0L;
    public long cObject = 0L;
    public CObject(long _cObject)
    {
        cObject = _cObject;
    }
    public static CObject CreateInstance(Class<?> child)
    {
        GlobalRef o = null;
        try {
            o = CreateGlobalInstance(child);
            if (o != null)
                ((CObject)o.obj).OnCreate(o.ID);
            else
                return null;
        }
        catch(Exception e)
        {
            return null;
        }
        return (CObject)o.obj;

    }
    //----------------------------------------------
    public long GetID() { return ID; }
    public void OnCreate(long id)
    {
        ID = id;
    }
    //---------------------------
    //вызов из деструктора c++
    //---------------------------
    public void OnDestroy()
    {
        ID = 0L;
        cObject = 0L;
    }
    @Override
    public void close() throws IOException {
        if (ID != 0L)
            AGlobals.ReleaseGlobalInstance(ID);
        ID = 0L;
    }
    static protected native GlobalRef CreateGlobalInstance(Class<?> cls);

}
