//
// Created by alex on 06.12.17.
//
#include "stdafx.h"

#ifndef WIN32

#include <android/bitmap.h>

#endif

#include "AWnd.h"
#include "ADC.h"
#include "JBitmap.h"

#ifndef WIN32
EXPORTNIX bool CreateTmpFile(TString &assets);

bool JBitmap::Load(LPCSTR _path)
{
    LPCSTR path = _path;
    Clear();
    TString ph;
    ph = path;
    CreateTmpFile(ph);
    path = ph.GetPtr();

    bool fRet = 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    JVariable param[2], res;
    JVariable ce;
    AObject options, factory, conf;
    options.New("android.graphics.BitmapFactory$Options", 0, 0, AObject::TJLOCAL, env);
    conf.New("android.graphics.Bitmap$Config", 0, 0, AObject::TJLOCAL, env);
    conf.Run("ARGB_8888", 0, 0, ce, env);
    param[0].Attach(ce.obj, &ce);
    options.Run("inPreferredConfig", param, 1, env);
    param[0].Detach();
    factory.New("android.graphics.BitmapFactory", 0, 0, AObject::TJLOCAL, env);
    param[0] = path;
    param[1].Attach(options.obj, "android.graphics.BitmapFactory$Options");
    factory.Run("decodeFile", param, 2, res, env);
    param[1].Detach();
    if (res.obj)
    {
        AndroidBitmapInfo info;

        if (AndroidBitmap_getInfo(env, res.obj, &info) >= 0)
        {

            width = info.width;
            height = info.height;
            obj = res.obj;
            res.Detach();
            type = fLocal = TJLOCAL;
            spec = MInstance::GetAClass("android.graphics.Bitmap");
            fRet = 1;
        }
    }
    if (ph != _path)
    {
        DeleteFile(ph.GetPtr());
    }
    return fRet;
}

bool JBitmap::Save(LPCSTR path, LPCSTR typeSave)
{
    JNIEnv *env = MInstance::GetLocalJNI();

    JPtr jptr;

    if (type == TJMEMORY)
    {
        if (!m_data) return 0;
        AndroidBitmapInfo info1;
        jptr = MGetApp()->CreateAndroidBitmap(width, height, &info1);
        obj = jptr.obj;
        spec = MInstance::GetAClass("android.graphics.Bitmap");
        DWORD *dw = 0;
        if (AndroidBitmap_lockPixels(env, obj, (void **) &dw) < 0)
        {
            memcpy(dw, m_data, width * height * sizeof(DWORD));
            AndroidBitmap_unlockPixels(env, obj);
        }
    } else if (!IsValid()) return 0;

    JVariable var[3], ve, r = 0;
    AObject format, stream;
    format.New("android.graphics.Bitmap$CompressFormat", 0, 0, AObject::TJSTATIC, env);
    if (format.Run(typeSave, 0, 0, ve, env))
    {
        var[0] = path;
        var[1] = (bool) true;
        stream.New("java.io.FileOutputStream", var, 2, AObject::TJLOCAL, env);
        var[0].Attach(ve.obj, &ve);
        var[1] = 0;
        var[2].Attach(stream.obj, "java.io.OutputStream");
        Run("compress", var, 3, r);
        var[2].Detach();
        var[0].Detach();
        stream.Run("flash", env);
        stream.Run("close", env);
    }
    if (type == TJMEMORY)
    {
        obj = 0;
        spec = 0;
    }
    return r.value.b == 1;

}

//--------------------------------------------------
//
//--------------------------------------------------
bool JBitmap::Save(MemImage &img, LPCSTR path, LPCSTR type)
{
    JBitmap bmp;
    if (type && img.SetAccess())
    {
        bmp.Create(img.GetPtr(), img.info.width, img.info.height, AObject::TJLOCAL);
        img.ReleaseAccess();
        return bmp.Save(path, type);
    }
    return FALSE;
}

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::Create(jobject bmp, int tp)
{
    Clear();

    DWORD *m_data = 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    AndroidBitmapInfo info;

    if (AndroidBitmap_lockPixels(env, bmp, (void **) &m_data) < 0 ||
        AndroidBitmap_getInfo(env, bmp, &info) < 0)
    {
        MInstance::ClearException(env);
        return 0;
    }
    BOOL fRet = _Create(m_data, info.width, info.height, tp);
    MInstance::ClearException(env);
    AndroidBitmap_unlockPixels(env, bmp);
    return fRet;
}

#endif

//--------------------------------------------------
//
//--------------------------------------------------
bool JBitmap::GetData(MArray<DWORD> &data, ImageInfo &info)
{
    bool fRet = 0;
    DWORD *pdata = (DWORD *) SetAccess();
    if (pdata)
    {
        GetInfo(info);
        data.Copy(pdata, info.width * info.height);
        fRet = 1;

    }
    ReleaseAccess();
    return fRet;
}

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::Create(JBitmap &bmp)
{
    Clear();
#ifndef WIN32
    if (bmp.type == TJMEMORY)
#endif
    {
        data = bmp.data;
        width = bmp.width;
        height = bmp.height;
        type = TJMEMORY;
        return true;
    }
#ifndef WIN32
    return Create(bmp.obj, bmp.type);
#endif

}

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::Create(ABmp &bmp)
{
    Clear();
    ABITMAP b;
    b = (ABITMAP) bmp;
    if (!b) return 0;
    jobject jbmp = 0;
#ifndef WIN32
    JNIEnv *env = MInstance::GetLocalJNI();
    AndroidBitmapInfo info1;
    jbmp = MGetApp()->CreateAndroidBitmap(bmp.GetWidth(), bmp.GetHeight(), &info1);
    char *_data = 0;
    if (AndroidBitmap_lockPixels(env, jbmp, (void **) &_data) < 0)
        _data = 0;
    else
    {
        ImageInfo info;
        DWORD *pBmp = b->SetAccess();

        memcpy(_data, pBmp, info1.width * info1.height * sizeof(DWORD));
        b->ReleaseAccess();
        AndroidBitmap_unlockPixels(env, jbmp);
        width = info1.width;
        height = info1.height;
        obj = jbmp;
        type = fLocal = TJLOCAL;
        spec = MInstance::GetAClass("android.graphics.Bitmap");
        return true;
    }
#endif
    return false;
}

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::_Create(int x, int y, int _type)
{

    width = x;
    height = y;
    type = _type;
#ifndef WIN32
    if (type != TJMEMORY)
    {
        AndroidBitmapInfo info1;
        jobject o = MGetApp()->CreateAndroidBitmap(width, height, &info1);
        if (!o)
            return 0;
        AObject::Attach(o, (_TYPE_) _type);
        JNIEnv *env = MInstance::GetLocalJNI();
        if (_type == TJLOCAL || _type == TJGLOBAL)
            env->DeleteLocalRef(o);

    } else
#endif
        data.SetLen(width * height);

    return 1;

}

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::_Create(DWORD *m_data, int x, int y, int tp)
{

    if (_Create(x, y, tp))
    {
        DWORD *dw = SetAccess();
        memcpy(dw, m_data, width * height * sizeof(DWORD));
        ReleaseAccess();
        return 1;
    }
    return 0;
}

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::Create(int x, int y, int tp)
{
    Clear();
    return _Create(x, y, tp);

}

//--------------------------------------------------
//
//--------------------------------------------------
DWORD *JBitmap::SetAccess()
{
    if (type == TJEMPTY) return NULL;
    if (fAccesCount == 0)
    {
        m_data = 0;
#ifndef WIN32
        switch (type)
        {
            case TJMEMORY:
                m_data = data.GetPtr();
                break;
            case TJSTATIC:
            case TJGLOBAL:
            case TJLOCAL:
            {
                JNIEnv *env = MInstance::GetLocalJNI();

                if (AndroidBitmap_lockPixels(env, obj, (void **) &m_data) < 0)
                {
                    MInstance::ClearException(env);
                    return 0;
                }
                break;
            }
            default:;
        }
#else
        m_data = data.GetPtr();
#endif

    }
    ++fAccesCount;
    return m_data;
}

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::ReleaseAccess()
{
    if (type != TJEMPTY)
    {
        if (fAccesCount)
        {
            --fAccesCount;
            if (!fAccesCount)
            {
#ifndef WIN32
                if (type != TJMEMORY)
                {
                    JNIEnv *env = MInstance::GetLocalJNI();
                    AndroidBitmap_unlockPixels(env, obj);
                }
#endif
                m_data = 0;
            }
            return TRUE;
        }
    }
    return FALSE;

}

//--------------------------------------------------
//
//--------------------------------------------------
void JBitmap::Clear(bool fRecycle)
{
    ResetAccess();
#ifndef WIN32
    if (fRecycle && obj)
        AObject::Run("recycle");
        AObject::Clear();
#endif
    data.Clear();
    fAccesCount = 0;
    type = TJEMPTY;
    width = height = 0;
    m_data = 0;
}

#ifndef WIN32

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::Run(const char *name, JVariable *v, int n, JVariable &res, JNIEnv *env)
{
    if (obj)
        return AObject::Run(name, v, n, res, env);
    return 0;
}

#endif

//--------------------------------------------------
//
//--------------------------------------------------
BOOL JBitmap::GetInfo(ImageInfo &info)
{
    info.Set(width, height, width);
    return true;
}

//--------------------------------------------------
//
//--------------------------------------------------
void JBitmap::ResetAccess()
{
    while (ReleaseAccess());

}
//--------------------------------------------------
//
//--------------------------------------------------
#ifndef WIN32

BOOL JBitmap::Attach(jobject bmp, int _type)
{
    Clear();
    JNIEnv *env = MInstance::GetLocalJNI();
    AndroidBitmapInfo info;

    if (AndroidBitmap_getInfo(env, bmp, &info) < 0)
    {
        MInstance::ClearException(env);
        return 0;
    }
    width = info.width;
    height = info.height;

    if (AObject::Attach(bmp, (_TYPE_) _type))
        type = _type;
    else
        type = TJEMPTY;

    return type != TJEMPTY;

}

//--------------------------------------------------
//
//--------------------------------------------------
jobject JBitmap::Detach()
{
    if (type == TJEMPTY || type == TJMEMORY) return 0;
    ResetAccess();
    jobject _obj = obj;
    AObject::Detach();
    return _obj;
}

#endif

//
//--------------------------------------------------
BOOL JBitmap::Resize(int cx, int cy)
{
    if (type == TJEMPTY)
        return false;
    if (width == cx && height == cy) return true;
    DWORD *dw = 0;
#ifndef WIN32
    if (type == TJMEMORY)
#endif
        dw = SetAccess();
    ResetAccess();
    int _type = type;
#ifndef WIN32
    jobject bmp = obj;
    if (obj)
        Clear();
#endif
#ifndef WIN32
    if (type == TJMEMORY)
#endif
        data.Detach();
    ImageInfo _info(width, height, width);
    fAccesCount = 0;
#ifndef WIN32
    obj = 0;
#endif
    type = TJEMPTY;
    BOOL fRet;
    if ((fRet = Create(cx, cy, _type)))
    {
        MemImage img;
        MemImage new_img;
        ImageInfo info(width, height, width);
        new_img.Init(SetAccess(), info);
        img.Init(dw, _info);
        new_img.Show(img, 0, 0, SRCCOPY);
        ReleaseAccess();
    }
#ifndef WIN32
    if (type == TJMEMORY)
#endif
        HFREE(dw);

#ifndef WIN32
    else if (0)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        if (bmp)
            switch (_type)
            {
                case TJLOCAL:
                case TJDEFAULT:
                    env->DeleteLocalRef(bmp);
                    break;
                case TJGLOBAL:
                    env->DeleteGlobalRef(bmp);
                    break;
                default:
                    break;
            }
    }
#endif
    return fRet;

}

