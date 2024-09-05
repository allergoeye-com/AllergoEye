#include "lib_colorpicker_ColorPicker.h"
#include "lib_utils_AGlobals.h"
#include "lib_common_CObject.h"
#include "lib_common_CWnd.h"
#include "com_dryeye_app_MenuActivity.h"
#include "stdafx.h"
#include "App.h"
#include "MultiThread.h"
#include "resource.h"


extern MultiThreadS *pMultiThreads;

CApp *theApp = 0;
EXPORTNIX void WinMain(char **, int)
{
    theApp = new CApp();
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{

    MInstance::MainApp = WinMain;
    WinMain(0, 0);
    MInstance::InitSpec(STR_GLOBALREF);
    MInstance::InitSpec(STR_CONTEXT);
    MInstance::InitSpec(STR_CVIEW);
    MInstance::InitSpec(STR_ADDSECTLIST);
    MInstance::InitSpec(STR_AGLOBALS);
    return JNI_VERSION_1_6;
}


CApp::CApp() : MInstance(0, 0)
{
    MInstance::m_Instance = this;

}


CApp::~CApp()
{
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
bool LoadImage(TString &ph, JNIEnv*env, AObject &options, AObject &factory, MArray<DWORD> &data, ImageInfo &info)
{
    bool fRet = 0;
    JVariable param[2], res;
    param[0] = ph.GetPtr();
    param[1].Attach(options.obj, "android.graphics.BitmapFactory$Options");
    factory.Run("decodeFile", param, 2, res, env);
    param[1].Detach();
    if (res.obj)
    {
        JBitmap bmp;
        bmp.Attach(res.obj, AObject::TJSTATIC);
        DWORD *pdata = (DWORD *)bmp.SetAccess();
        if (pdata)
        {
            bmp.GetInfo(info);
            data.Copy(pdata, info.width * info.height);
            fRet = 1;

        }
        bmp.ReleaseAccess();
        bmp.Detach();
    }
    return fRet;
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
bool MakeJava_BitmapFactoryAndBitmapFactoryOptions(JNIEnv*env, AObject &options, AObject &factory)
{
    options.New("android.graphics.BitmapFactory$Options", 0, 0, AObject::TJLOCAL, env);
    JVariable param;
    JVariable ce;
    AObject conf;
    conf.New("android.graphics.Bitmap$Config", 0, 0, AObject::TJSTATIC,  env);
    conf.Run("ARGB_8888",0, 0, ce, env);
    param.Attach(ce.obj, &ce);
    options.Run("inPreferredConfig", &param, 1, env);
    param.Detach();
    factory.New("android.graphics.BitmapFactory", 0, 0, AObject::TJDEFAULT, env);
    return options.obj != 0 && factory.obj != 0;
}
bool SaveImage(TString type, TString &ph, JNIEnv*env, MArray<DWORD> &data, ImageInfo &info)
{
    try {
        JBitmap bmp;
        bmp.Create(data.GetPtr(), info.width, info.height, AObject::TJLOCAL);
        JVariable var[3], ve, r;
        AObject format, stream;
        format.New("android.graphics.Bitmap$CompressFormat", 0, 0, AObject::TJDEFAULT,  env);
        format.Run(type.GetPtr(),0, 0, ve, env);
        var[0] = ph.GetPtr();
        var[1] = (bool)true;
        stream.New("java.io.FileOutputStream", var, 2, AObject::TJDEFAULT, env);
        var[0].Attach(ve.obj, &ve);
        var[1] = 0;
        var[2].Attach(stream.obj, "java.io.OutputStream");
        bmp.Run("compress", var, 3, r);
        var[2].Detach();
        var[0].Detach();
        stream.Run("flash", env);
        stream.Run("close", env);
        return r.value.b == 1;
    }
    catch(...)
    {
        ;
    }
    return false;

}
#if (0)
bool LoadPng()
{

    JNIEnv*env = MInstance::GetLocalJNI();
    AObject options, factory;
    MakeJava_BitmapFactoryAndBitmapFactoryOptions(env, options, factory);

    WIN32_FIND_DATAA *dir = 0, *file = 0;
    int num_dir = 0, num_file = 0;
    TString path ="/storage/emulated/0/test";
    GetDirList(path.GetPtr(), "*", dir, num_dir, file, num_file);
    MArray<DWORD> data;
    ImageInfo info;
    TString ph, f;
    path += "/";
    f = path + "1";
    _ClearDir(f.GetPtr());
    for (int k = 0; k < num_file; ++k)
    {
        MArray<DWORD> data2;
        f = file[k].cFileName;
        ph = path;
        ph += f;
        if (LoadImage(ph, env, options, factory, data2, info))
        {
            f.Lower();
            ph = path;
            ph += "1/";
            f.Replace("-", "_");
            ph += f;
            if (data.GetLen() < data2.GetLen())
                data.SetLen(data2.GetLen());
            if (data.GetLen() > data2.GetLen())
                data2.SetLen(data.GetLen());
            BYTE *bi = (BYTE *)data.GetPtr();
            BYTE *bi2 = (BYTE *)data2.GetPtr();
            data = data2;
            for (int j = 0; j < data.GetLen(); ++j)
            {
                BYTE b = max(bi2[j * 4 + 1], bi2[j * 4]);
                bi[j * 4 + 1] = b;
                bi[j * 4] = b;
            }
            SaveImage("PNG", ph, env, data, info);
        }

    }
    return true;
}
#endif
BOOL CApp::InitInstance()
{
    MInstance::InitInstance();
//---
    TString m_path = sPath;
    m_path += "/lang.ini";
    if (GetFileAttributes(m_path.GetPtr()) == 0xffffffff)
    {
        CMemFile mf;
        MGetApp()->ReadResourceFile("lang.ini", mf);
        MFILE f(m_path.GetPtr());
        if (f.Open(true))
        {
            f.Write(mf.buff.GetPtr(), mf.buff.GetLen());
            f.Close();
        }
    }
    m_path = sPath;
    m_path += "/en.txt";
   // if (GetFileAttributes(m_path.GetPtr()) == 0xffffffff)
    {
        CMemFile mf;
        MGetApp()->ReadResourceFile("en.txt", mf);
        MFILE f(m_path.GetPtr());
        if (f.Open(true))
        {
            f.Write(mf.buff.GetPtr(), mf.buff.GetLen());
            f.Close();
        }
    }
    m_path = sPath;
    m_path += "/de.txt";
  //  if (GetFileAttributes(m_path.GetPtr()) == 0xffffffff)
    {
        CMemFile mf;
        MGetApp()->ReadResourceFile("de.txt", mf);
        MFILE f(m_path.GetPtr());
        if (f.Open(true))
        {
            f.Write(mf.buff.GetPtr(), mf.buff.GetLen());
            f.Close();
        }
    }

    FFileINI ini("lang.ini", true);
    TString res = "ASSETS/rc_";
    TString ext = ini.GetString("CURRENT", "RES");
    ext.MakeLower();
    if (!ext.StrLen())
    {
        ini.WriteString("CURRENT", "RES", "de");
        res = "ASSETS/rc_de.res";
    } else
    {
        res += ext;
        res += ".res";
    }


  //  LoadMSResource(res.GetPtr());
    pMultiThreads = new MultiThreadS();
    pMultiThreads->Create();
    Dlg = new WndImageView();
    InitMsg();
    return true;
}

BOOL CApp::ExitInstance()
{
    if (pMultiThreads) delete pMultiThreads;
    return true;
}

//---------------------------------------------------------
//
//---------------------------------------------------------
jobject CApp::CreateJavaObject(const TString &name)
{

    AObject *ret = NULL;
    if (name.StrLen())
    {

        ret = new AObject();
        JVariable var;
        var = (jlong) ret;
        ret->New(name, &var, 1, AObject::TJGLOBAL);
        if (ret->obj)
        {
            ret->Run("OnTest");
            globals.Add(ret->obj, ret);
        } else
        {
            delete ret;
            ret = NULL;
        }


    }
    return ret ? ret->obj : 0;
}

//---------------------------------------------------------
//
//---------------------------------------------------------
void CApp::ReleaseInstance(jobject obj)
{
    AObject *val = 0;
    if (globals.Lookup(obj, val))
    {
        TString test = val->Name();
        if (val != NULL)
            val->Clear();
        globals.Delete(obj);

    }
}
//---------------------------------------------------------
//
//---------------------------------------------------------
jobject CApp::CreateJavaWindow(JNIEnv *env, jclass cls, jobject context, int w, int h)
{
    TString name = GetNameJClass(env, cls);
    AObject *ret = NULL;
    InitMsg();
    JVariable var[3];
    if (w == 0 && h == 0)
    {
        w = GetMetrics()->widthPixels;
        h = GetMetrics()->heightPixels;
    }
    AWnd *wnd = 0;
    ret = new AObject();
    if (name == STR_CVIEW)
    {
        delete Dlg;
        Dlg = new WndImageView();
        wnd = Dlg;
    }
    if (wnd)
    {
        CRect rc, rc2(0, 0, w, h);
        wnd->GetClientRect(&rc);
        // if (rc != rc2)
        {
            wnd->SetDefaultTypeSurface(AObject::TJGLOBAL);
            wnd->SetSize(w, h);
            JBITMAP bmp = wnd->GetSurface();
            if (bmp)
            {
                jobject jbmp = bmp->Get_jobject();
                if (jbmp)
                {
                    var[0].Attach(context, STR_CONTEXT);
                    var[1] = (jlong) ret;
                    var[2].Attach(jbmp);

                    ret->New(name, var, 3, AObject::TJGLOBAL);
                    var[0].Detach();
                    var[2].Detach();
                    if (ret->obj)
                    {
                        ret->Run("OnTest", env);
                        globals.Add(ret->obj, ret);
                        wnd->SetView(ret);
                        if (name == STR_CVIEW)
                        {
                            Dlg->OnPaint(0);
                        }

                    }
                }
            }
        }
    }
    if (ret && !ret->obj)
    {
        delete ret;
        ret = 0;
    }
    return ret ? ret->obj : 0;
}

void CApp::InitAndroidIdle(jobject action)
{
    JVariable var[2];
    var[0] = "android_EOSApp";
    var[1].Attach(action);
    androidIdle.New(STR_APP_IDLE, var, 2, AObject::TJGLOBAL);
    var[1].Detach();
}
CApp::METRICS *CApp::GetMetrics()
{
    if (!AGlobals.IsValid())
    {
        AGlobals.New(STR_AGLOBALS, 0, 0, AObject::TJGLOBAL);
    }
    if (AGlobals.IsValid())
    {
        JVariable res;
        AGlobals.Run("GetDisplayMetrcs", res);
     if (res.obj)
        SetDisplayMetrics(GetLocalJNI(), res.obj);

    }
    return MInstance::GetMetrics();



}
//---------------------------------------------------------
//
//---------------------------------------------------------
void CApp::RemoveAndroidIdle()
{
    if (androidIdle.IsValid())
        androidIdle.Run("close");

    androidIdle.Clear();

}

//---------------------------------------------------------
//
//---------------------------------------------------------
void CApp::RunAndroidIdle(int iMilisecond)
{
    if (iMilisecond > 0)
        if (androidIdle.IsValid())
        {
            JVariable param;
            param = iMilisecond;
            androidIdle.Run("Idle", &param, 1);
        }

}
/*------------------------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    ReleaseGlobalInstance
 * Signature: (J)V
 -----------------------------------------------------------------------*/
extern "C" JNIEXPORT void JNICALL Java_lib_utils_AGlobals_ReleaseGlobalInstance
        (JNIEnv *, jclass, jlong obj)
{
    if (obj)
        theApp->ReleaseInstance((jobject) obj);
}

/*----------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    AttachThread
 * Signature: (J)J
 -------------------------------------------------------*/
extern "C" JNIEXPORT jlong JNICALL Java_lib_utils_AGlobals_AttachThread
        (JNIEnv *env, jclass, jlong param)
{
    MPThread *th = new MPThread();
    if (th->Attach(false))
    {
        AObject *obj = theApp->Lookup((jobject) param);
        if (obj)
        {
            MSGThread *msgThread = dynamic_cast<MSGThread *>(obj);
            if (msgThread)
            {
                msgThread->InitMsg();
                return msgThread->m_hWnd;
            }
            return th->startup->MessageProc->m_hWnd;
        }
    }
    return 0;

}

/*---------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    DetachThread
 * Signature: (J)Z
 -------------------------------------------------------*/
extern "C" JNIEXPORT jboolean JNICALL Java_lib_utils_AGlobals_DetachThread
        (JNIEnv *, jclass, jlong _th)
{
    if (_th)
    {
        MSGThread *th = MSGThread::FromHandle((THWND) _th);

        if (th)
        {
            MPThread *t = th->Self();
            if (t)
            {

                th->SendMessage(WM_DESTROY, (WPARAM) t, 0);
                t->Detach();
            }
            return 1;
        }
    }
    return 0;
}


/*-----------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    LongToObject
 * Signature: (J)Ljava/lang/Object;
 -------------------------------------------*/
extern "C" JNIEXPORT jobject JNICALL Java_lib_utils_AGlobals_LongToObject
        (JNIEnv *, jclass, jlong obj)
{

    return (jobject) obj;
}
/*----------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    PostCommand2Thread
 * Signature: (JJJJ)Z
 ----------------------------------*/
extern "C" JNIEXPORT jboolean JNICALL Java_lib_utils_AGlobals_PostCommand2Thread
        (JNIEnv *, jclass, jlong _th, jlong cmd, jlong wParam, jlong lParam)
{

    if (_th)
    {
        MSGThread *th = MSGThread::FromHandle((THWND) _th);

        if (th)
        {
            th->PostMessage((MSGTYPE) cmd, (WPARAM) wParam, (LPARAM) lParam);
            return true;
        }

    }
    return false;
}

/*------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    SendCommand2Thread
 * Signature: (JJJJ)Z
 --------------------------------------------------*/
extern "C" JNIEXPORT jboolean JNICALL Java_lib_utils_AGlobals_SendCommand2Thread
        (JNIEnv *, jclass, jlong _th, jlong cmd, jlong wParam, jlong lParam)
{

    if (_th)
    {
        MSGThread *th = MSGThread::FromHandle((THWND) _th);

        if (th)
        {
            th->PostMessage((MSGTYPE) cmd, (WPARAM) wParam, (LPARAM) lParam);
            return true;
        }

    }
    return false;
}

/*-----------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    CallCommand
 * Signature: (JJJJ)Z
 --------------------------------------------------*/
extern "C" JNIEXPORT jboolean JNICALL Java_lib_utils_AGlobals_CallCommand
        (JNIEnv *, jclass, jlong obj, jlong cmd, jlong wParam, jlong lParam)
{
    AObject *b = theApp->Lookup((jobject) obj);
    if (b)
        return b->OnNotify(cmd, wParam, lParam);
    return 0;
}

/*-------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    AddGlobalIdle
 * Signature: (Llib/common/ActionCallback;)V
 --------------------------------------------------*/
extern "C" JNIEXPORT void JNICALL Java_lib_utils_AGlobals_AddGlobalIdle
        (JNIEnv *, jclass, jobject action)
{
    theApp->InitAndroidIdle(action);
}

/*-------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    ReleaseGlobalIdle
 * Signature: ()V
 ----------------------------------------*/
extern "C" JNIEXPORT void JNICALL Java_lib_utils_AGlobals_ReleaseGlobalIdle
        (JNIEnv *, jclass)
{
    theApp->RemoveAndroidIdle();

}

/*---------------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    SaveIni
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 ------------------------------------------------------------*/
extern "C" JNIEXPORT void JNICALL Java_lib_utils_AGlobals_SaveIni
        (JNIEnv *, jclass, jstring s, jstring n, jstring d, jstring jfile)
{
    JPtr sec = s;
    JPtr name = n;
    JPtr data = d;
    TString sini;
    sini = theApp->sPath;
    JPtr f = jfile;
    TString nm = f.String();
    if (nm[0] != '/')
    {
        sini += "/";
        sini += nm;
    } else
        sini = nm;

    FFileINI ini(sini.GetPtr(), FALSE);
    ini.WriteString(sec.String(), name.String(), data.String());

}

/*---------------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    SaveIni
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 ------------------------------------------------------------*/
extern "C" JNIEXPORT void JNICALL Java_lib_utils_AGlobals_SaveIniW
        (JNIEnv *, jclass, jstring s, jstring n, jstring d, jstring jfile)
{
    JPtr sec = s;
    JPtr name = n;
    JPtr data = d;
    TString sini;
    sini = theApp->sPath;
    JPtr f = jfile;
    TString nm = f.String();
    if (nm[0] != '/')
    {
        sini += "/";
        sini += nm;
    } else
        sini = nm;

    FFileINIW ini(sini.GetPtr(), FALSE);
    ini.WriteString(sec.wString(), name.wString(), data.wString());

}

/*-------------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    ReadIni
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 ---------------------------------------------------------*/
extern "C" JNIEXPORT jstring JNICALL Java_lib_utils_AGlobals_ReadIniW
        (JNIEnv *, jclass, jstring s, jstring n, jstring jfile)
{
    JPtr sec = s;
    JPtr name = n;
    TString sini;
    sini = theApp->sPath;
    if (jfile != 0)
    {
        JPtr f = jfile;
        TString nm = f.String();
        if (nm[0] != '/')
        {
            sini += "/";
            sini += nm;
        } else
            sini = nm;
    }
    FFileINIW ini(sini.GetPtr(), FALSE);
    WString res = ini.GetString(sec.wString(), name.wString());
    JVariable var;
    jstring str = 0;
    if (res.StrLen())
    {
        var = res.GetPtr();
        str = var.str;
        var.Detach();
    }
    return str;

}
/*-------------------------------------------------------
 * Class:     lib_utils_AGlobals
 * Method:    ReadIni
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 ---------------------------------------------------------*/
extern "C" JNIEXPORT jstring JNICALL Java_lib_utils_AGlobals_ReadIni
        (JNIEnv *, jclass, jstring s, jstring n, jstring jfile)
{
    JPtr sec = s;
    JPtr name = n;
    TString sini;
    sini = theApp->sPath;
    if (jfile != 0)
    {
        JPtr f = jfile;
        TString nm = f.String();
        if (nm[0] != '/')
        {
            sini += "/";
            sini += nm;
        } else
            sini = nm;
    }
    FFileINI ini(sini.GetPtr(), FALSE);
    TString res = ini.GetString(sec.String(), name.String());
    JVariable var;
    jstring str = 0;
    if (res.StrLen())
    {
        var = res.GetPtr();
        str = var.str;
        var.Detach();
    }
    return str;

}
/*--------------------------------------------------------------
 * Class:     lib_common_CObject
 * Method:    CreateGlobalInstance
 * Signature: (Ljava/lang/Class;)Llib/utils/GlobalRef;
 ------------------------------------------------------------------*/
extern "C" JNIEXPORT jobject JNICALL Java_lib_common_CObject_CreateGlobalInstance
        (JNIEnv *env, jclass, jclass cls)
{
    jobject ob = theApp->CreateJavaObject(env, cls);
    AObject b;
    b.New(STR_GLOBALREF, 0, 0, AObject::TJLOCAL);
    JVariable param;
    param.Attach(ob, "java.lang.Object");
    b.Run("obj", &param, 1, env);
    param.Detach();
    param = (jlong) ob;
    b.Run("ID", &param, 1, env);
    ob = b.obj;
    b.Detach();
    return ob;


}
/*-------------------------------------------------------------------------
 * Class:     lib_common_CWnd
 * Method:    CreateGlobalInstance
 * Signature: (Ljava/lang/Class;Landroid/content/Context;)Llib/utils/GlobalRef;
 ---------------------------------------------------------------------------*/
extern "C" JNIEXPORT jobject JNICALL Java_lib_common_CWnd_CreateGlobalInstance
        (JNIEnv *env, jclass, jclass cls, jobject context)
{
    jobject ob = theApp->CreateJavaWindow(env, cls, context);
    if (ob)
    {
        AObject b;
        b.New(STR_GLOBALREF, 0, 0, AObject::TJLOCAL);
        JVariable param;
        param.Attach(ob, "java.lang.Object");
        b.Run("obj", &param, 1, env);
        param.Detach();
        param = (jlong) ob;
        b.Run("ID", &param, 1, env);
        ob = b.obj;
        b.Detach();
        return ob;
    }
    return 0;

}
extern "C" JNIEXPORT void JNICALL Java_com_dryeye_app_MenuActivity_InitInstance
        (JNIEnv *, jobject)
{
    theApp->InitInstance();

}
extern "C" JNIEXPORT jobject JNICALL Java_lib_common_CWnd__1CreateGlobalInstance
        (JNIEnv *env, jclass, jclass cls, jobject context, jint x, jint y)
{
    jobject ob = theApp->CreateJavaWindow(env, cls, context, x, y);
    if (ob)
    {
        AObject b;
        b.New(STR_GLOBALREF, 0, 0, AObject::TJLOCAL);
        JVariable param;
        param.Attach(ob, "java.lang.Object");
        b.Run("obj", &param, 1, env);
        param.Detach();
        param = (jlong) ob;
        b.Run("ID", &param, 1, env);
        ob = b.obj;
        b.Detach();
        return ob;
    }
    return 0;

}

extern "C" JNIEXPORT jboolean JNICALL Java_lib_utils_AGlobals_OnThreadLoop
        (JNIEnv *, jclass, jlong _th)
{

    return 0;
}

int GetAndroidID(TString &tstr)
{
    if (tstr == "IDS_NEW_PATIENT")
        return IDS_NEW_PATIENT;
    if (tstr == "IDS_PATIENTS")
        return IDS_PATIENTS;
    if (tstr == "IDS_STYNC")
        return IDS_STYNC;
    if (tstr == "IDS_EXIT")
        return IDS_EXIT;
    if (tstr == "IDS_HOSTNAME")
        return IDS_HOSTNAME;
    if (tstr == "IDS_NAME")
        return IDS_NAME;
    if (tstr == "IDS_SSURNAME")
        return IDS_SSURNAME;
    if (tstr == "IDS_DATE")
        return IDS_DATE;
    if (tstr == "IDS_DELETE_IMAGE")
        return IDS_DELETE_IMAGE;

    if (tstr == "IDS_DELETE")
        return IDS_DELETE;
    if (tstr == "IDS_PATIENT")
        return IDS_PATIENT;
    if (tstr == "IDS_PHOTO")
        return IDS_PHOTO;
    if (tstr == "IDS_TEST")
        return IDS_TEST;
    if (tstr == "IDS_SEARCH")
        return IDS_SEARCH;
    if (tstr == "IDS_CAMERA")
        return IDS_CAMERA;
    if (tstr == "IDS_SAVE")
        return IDS_SAVE;
    if (tstr == "IDS_SELECT_DATE")
        return IDS_SELECT_DATE;
    if (tstr == "IDS_TODAY")
        return IDS_TODAY;
    if (tstr == "IDS_SELECT_TEST")
        return IDS_SELECT_TEST;

    if (tstr == "IDS_NEW_TEST")
        return IDS_NEW_TEST;
    if (tstr == "IDS_PATIENT_INFO")
        return IDS_PATIENT_INFO;
    if (tstr == "IDS_OR")
        return IDS_OR;
    if (tstr == "IDS_RENAME")
        return IDS_RENAME;
    if (tstr == "IDS_MERGE")
        return IDS_MERGE;

    return -1;
}


/*
 * Class:     lib_utils_AGlobals
 * Method:    ReadString
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
extern "C" JNIEXPORT jstring JNICALL Java_lib_utils_AGlobals_ReadString
        (JNIEnv *, jclass, jstring id)
{
    int i;
    jstring str = 0;
    JPtr p = id;
    TString tstr = p.String();

    if ((i = GetAndroidID(tstr)) != -1)
    {
        TString res = LOADSTRING(i);
        if (res.StrLen())
        {
            JVariable var;
            var = res.GetPtr();
            str = var.str;
            var.Detach();
        }
    }
    return str;
}

/*
 * Class:     lib_utils_AGlobals
 * Method:    ReadLang
 * Signature: ()Ljava/lang/String;
 */
extern "C" JNIEXPORT jstring JNICALL Java_lib_utils_AGlobals_ReadLang
        (JNIEnv *, jclass)
{
    FFileINI ini("lang.ini", true);
    TString ext = ini.GetString("CURRENT", "RES");
    ext.MakeUpper();
    JVariable var;
    var = ext.GetPtr();
    jstring res = var.str;
    var.Detach();
    return res;
}

/*
 * Class:     lib_utils_AGlobals
 * Method:    WriteLang
 * Signature: (Ljava/lang/String;)V
 */
extern "C" JNIEXPORT void JNICALL Java_lib_utils_AGlobals_WriteLang
        (JNIEnv *, jclass, jstring lang)
{

    FFileINI ini("lang.ini", true);
    TString res = "ASSETS/rc_";
    JPtr p;
    p = lang;
    TString ext = p.String();
    ext.MakeLower();
    ini.WriteString("CURRENT", "RES", ext.GetPtr());
    res += ext;
    res += ".res";
    theApp->LoadMSResource(res.GetPtr());
}
static void HSL(float H, float S, float L, float rgb[3])
{
    L /= 100.0f;
    S /= 100.0f;
    float Q = L < 0.5f ? L + S * L : L + S - L*S;
    float P = 2.0 * L  - Q;
    float Hk = H/360.f;

    rgb[0] = Hk + 1.f/3.f;
    rgb[1] = Hk;
    rgb[2] = Hk - 1.f/3.f;
    for (int i = 0; i < 3; ++i)
    {
        if (rgb[i] < 0.0f) rgb[i] += 1.f;
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
}
struct HSLParam {
    float H;
    float S;
    float X;
    float ScaleX;
    float ScaleY;
    ColorRef *in;
    ImageInfo info;
    IRect rc;
    int yStart;
    int yEnd;
    HSLParam(float _H, float _S, ColorRef *_in, ImageInfo &_info, IRect &_rc, int _yStart, int _yEnd)
    {
        H = _H;
        S = _S;
        in = _in;
        info = _info;
        rc = _rc;
        yStart = _yStart;
        yEnd = _yEnd;
    }
    HSLParam(float _X, float _ScaleX, float _ScaleY, ColorRef *_in, ImageInfo &_info, IRect &_rc, int _yStart, int _yEnd)
    {
        X = _X;
        ScaleX = _ScaleX;
        ScaleY = _ScaleY;
        in = _in;
        info = _info;
        rc = _rc;
        yStart = _yStart;
        yEnd = _yEnd;
    }

};
void HSLColor(void *param)
{
    HSLParam *p = (HSLParam *)param;
    float stp = (float)(p->rc.bottom - p->rc.top)/100.0f;
    float rgb[3];
    p->in += p->info.width * p->yStart;
    for (int y = p->yStart; y < p->yEnd; ++y)
    {
        HSL(p->H, p->S, (p->rc.bottom - y + p->rc.top )/stp, rgb);
        int R = (int)(rgb[0] * 255.f);
        int G = (int)(rgb[1] * 255.f);
        int B = (int)(rgb[2] * 255.f);
        if (R > 255) R = 255;
        if (G > 255) G = 255;
        if (B > 255) B = 255;
        for (int x = p->rc.left; x < p->rc.right; ++x)
        {
            (p->in + x)->color  = RGBA((int)(BYTE)R, (int)(BYTE)G, (int)(BYTE)B, 255);
        }
        p->in += p->info.width;
    }
}
extern MultiThreadS *pMultiThreads;

void HLSPlane(void *param)
{
    HSLParam *p = (HSLParam *) param;

    p->in += p->info.width * p->yStart;
    float rgb[3];
    for (int y = p->yStart; y < p->yEnd; ++y)
    {
        for (int x = p->rc.left; x < p->rc.right; ++x)
        {
            float H = (x - p->rc.left) * p->ScaleX;
            float S = (p->rc.bottom - y) * p->ScaleY;
            HSL(H, S, p->X, rgb);
            int R = (int)(rgb[0] * 255.f);
            int G = (int)(rgb[1] * 255.f);
            int B = (int)(rgb[2] * 255.f);
            if (R > 255) R = 255;
            if (G > 255) G = 255;
            if (B > 255) B = 255;
            (p->in + x)->color  = RGBA((int)(BYTE)R, (int)(BYTE)G, (int)(BYTE)B, 255);
        }
        p->in += p->info.width;
    }
}

extern "C" JNIEXPORT void JNICALL Java_lib_colorpicker_ColorPicker_HLSPlane
        (JNIEnv *, jclass, jobject bmp, jfloat X, jfloat ScaleX, jfloat ScaleY, jint left, jint top, jint right, jint botom)
{
    JBitmap jbm;
    ImageInfo info;
    IRect rc(left, top, right, botom);
    jbm.Attach(bmp, AObject::TJSTATIC);
    jbm.GetInfo(info);
    ColorRef *in = (ColorRef *)jbm.SetAccess();
    if (in)
    {
        memset(in, 0, info.height * info.width * sizeof(DWORD));
        HSLParam param(X, ScaleX, ScaleY, in, info, rc, top, botom);
        HLSPlane(&param);
    }
    jbm.ReleaseAccess();

    jbm.Detach();


}


extern "C" JNIEXPORT void JNICALL Java_lib_colorpicker_ColorPicker_HLSLum
        (JNIEnv *, jclass, jobject bmp, jfloat H, jfloat S, jint left, jint top, jint right, jint botom)
{
    JBitmap jbm;
    ImageInfo info;
    IRect rc(left, top, right, botom);
    jbm.Attach(bmp, AObject::TJSTATIC);
    jbm.GetInfo(info);
    ColorRef *in = (ColorRef *)jbm.SetAccess();
    if (in)
    {
        memset(in, 0, info.height * info.width * sizeof(DWORD));
        HSLParam param(H, S, in, info, rc, top, botom);
        HSLColor(&param);
    }
    jbm.ReleaseAccess();
    //   jbm.Save("/storage/emulated/0/test/1.pmg");
    jbm.Detach();


}