#ifdef ANDROID_NDK
#pragma pack(push)
#pragma pack(0)

#include <jni.h>
#include <android/bitmap.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#pragma pack(pop)
#endif

#include "stdafx.h"
#include "minstance.h"
#include "uxmessage.h"
#include "_ffile.h"

#ifdef NOPLUK

#include "variable.h"

#endif
#ifdef ANDROID_NDK

#include "AObject.h"

#endif

#include <dlfcn.h>
#include "MultiThread.h"
#include "AClass.h"

#define RT_MENU        4
#define RT_DIALOG   5
#define RT_STRING    6
#define RT_BITMAP   2
#pragma pack(push)
#pragma pack(4)
struct MenuHeader {
    WORD wVersion;           // Currently zero
    WORD cbHeaderSize;       // Also zero
};
struct NormalMenuItem {
    WORD fItemFlags;
    WORD id;
    LPCWSTR lpszMenu;
};

struct PopupMenuItem {
    WORD fItemFlags;
    LPCWSTR lpszMenu;
};
#define MFR_END 0x80
#define MFR_POPUP 0x0010

#pragma pack(pop)
#define USE_MENU_STRING

MultiThreadS *pMultiThreads = 0;
MultiThreadS *pMultiThreadsDraw = 0;

#ifdef ANDROID_NDK
JavaVM *MInstance::m_JavaVM = 0;
ABitmapCreator *MInstance::bmp_instance = 0;

jclass MInstance::throwable = 0;
jmethodID MInstance::throwable_getCause = 0;
jmethodID MInstance::throwable_getStackTrace = 0;
jmethodID MInstance::throwable_toString = 0;
jclass MInstance::stacktrace = 0;
jmethodID MInstance::stacktrace_toString = 0;
MHashMap<AClass *, TString> *MInstance::clazz = 0;
InitURLLoader *MInstance::loader = 0;
CriticalSection *MInstance::clazz_section = 0;
int MInstance::iTouchSlop = 8;
float MInstance::fTouchSlop = 8;

//CreateClassUtil *MInstance::utils = 0;
//--------------------------------------
//
//--------------------------------------
struct CreateJarUtil {
    JPtr fileInputStream;
    JPtr jarInputStream;
    JPtr jarNextJarEntry;

    JPtr fInputStram;
    JPtr jInputStream;
    jmethodID getNextJarEntry;
    jmethodID getName;

    CreateJarUtil(const char *jar, JNIEnv *env);

    ~CreateJarUtil();

    JNIEnv *env;
    BOOL fValid;


};

//--------------------------------------
//
//--------------------------------------
CreateJarUtil::CreateJarUtil(const char *jar, JNIEnv *_env)
{
    memset(this, 0, sizeof(CreateJarUtil));
    env = _env;
    fileInputStream = env->FindClass("java/io/FileInputStream");
    if (MInstance::ClearException(env)) return;
    jmethodID constructor = env->GetMethodID(fileInputStream, "<init>", "(Ljava/lang/String;)V");
    if (MInstance::ClearException(env)) return;
    jstring name = env->NewStringUTF(jar);
    fInputStram = env->NewObject(fileInputStream, constructor, name);
    if (MInstance::ClearException(env)) return;
    jarInputStream = env->FindClass("java/util/jar/JarInputStream");
    if (MInstance::ClearException(env)) return;
    constructor = env->GetMethodID(jarInputStream, "<init>", "(Ljava/io/InputStream;)V");
    if (MInstance::ClearException(env)) return;
    jInputStream = env->NewObject((jclass) jarInputStream, constructor, (jobject) fInputStram);
    if (MInstance::ClearException(env)) return;
    getNextJarEntry = env->GetMethodID(jarInputStream, "getNextJarEntry",
                                       "()Ljava/util/jar/JarEntry;");
    if (MInstance::ClearException(env)) return;
    jarNextJarEntry = env->FindClass("java/util/jar/JarEntry");
    if (MInstance::ClearException(env)) return;
    getName = env->GetMethodID(jarNextJarEntry, "getName", "()Ljava/lang/String;");
    if (MInstance::ClearException(env)) return;
    fValid = true;

}

//--------------------------------------
//
//--------------------------------------
CreateJarUtil::~CreateJarUtil()
{
}

InitURLLoader::~InitURLLoader()
{
}

//--------------------------------------
//
//--------------------------------------
InitURLLoader::InitURLLoader(MVector<TString> &jar, JNIEnv *_env)
{
    fValid = 0;
    memset(this, 0, sizeof(InitURLLoader));
    env = _env;

    JPtr url;
    JPtr url_loader;

    url = env->FindClass("java/net/URL");
    if (MInstance::ClearException(env)) return;
    url_loader = env->FindClass("java/net/URLClassLoader");
    loadClass = env->GetMethodID(url_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    if (MInstance::ClearException(env)) return;

    JPtr arr = env->NewObjectArray((jsize) jar.GetLen(), url, 0);
    for (jsize i = 0, l = (jsize) jar.GetLen(); i < l; ++i)
    {
        TString urlPath = "file:///";
        urlPath += jar[i];
        JPtr name = env->NewStringUTF(urlPath.GetPtr());
        jmethodID curl = env->GetMethodID(url, "<init>", "(Ljava/lang/String;)V");
        if (MInstance::ClearException(env)) return;

        JPtr url_obj = env->NewObject(url, curl, (jobject) name);
        if (MInstance::ClearException(env)) return;
        env->SetObjectArrayElement(arr, i, url_obj);
    }

    JPtr thread = env->FindClass("java/lang/Thread");
    if (MInstance::ClearException(env)) return;
    jmethodID thread_currentThread = env->GetStaticMethodID(thread, "currentThread",
                                                            "()Ljava/lang/Thread;");
    if (MInstance::ClearException(env)) return;
    JPtr currentThread = env->CallStaticObjectMethod(thread, thread_currentThread);
    if (MInstance::ClearException(env)) return;
    jmethodID getContextClassLoader = env->GetMethodID(thread, "getContextClassLoader",
                                                       "()Ljava/lang/ClassLoader;");
    if (MInstance::ClearException(env)) return;
    jmethodID setContextClassLoader = env->GetMethodID(thread, "setContextClassLoader",
                                                       "(Ljava/lang/ClassLoader;)V");

    JPtr classLoaderCLS = env->FindClass("java/lang/ClassLoader");
    JPtr classLoader = env->CallObjectMethod(currentThread, getContextClassLoader);

    jmethodID url_init = env->GetMethodID(url_loader, "<init>",
                                          "([Ljava/net/URL;Ljava/lang/ClassLoader;)V");
    if (MInstance::ClearException(env)) return;
    urlLoader = env->NewObject((jclass) url_loader, url_init, (jobjectArray) arr,
                               (jobject) classLoader);
    if (MInstance::ClearException(env)) return;
    env->CallVoidMethod(currentThread, setContextClassLoader, (jobject) urlLoader);
    if (MInstance::ClearException(env)) return;

    fValid = 1;
}

#endif

struct ResHeader {
    DWORD DataSize;           // size of data without header
    DWORD HeaderSize;         // Length of the additional header
    Variable Type;     // type identifier, id or string
    Variable Name;     // name identifier, id or string
    DWORD DataVersion;        // predefined resource data version
    WORD MemoryFlags;        // state of the resource
    WORD LanguageId;         // UNICODE support for NLS
    DWORD Version;            // Version of the resource data
    DWORD Characteristics;    // Characteristics of the data
    ResHeader()
    {
        MemoryFlags = LanguageId = 0;
        DataVersion = 0;
        DataSize = HeaderSize = 0;
        Characteristics = Version = 0;
    }

    ~ResHeader()
    {}

    BOOL IsValid()
    { return DataSize > 0 && HeaderSize > 0; }

    BOOL Read(MArray<char> &buff, int offset);
};

BOOL ResHeader::Read(MArray<char> &buff, int offset)
{
    WORD lo;
    BYTE *p = (BYTE *) buff.GetPtr();
    memcpy(&DataSize, p + offset, sizeof(DWORD));
    offset += sizeof(DWORD);
    memcpy(&HeaderSize, p + offset, sizeof(DWORD));
    offset += sizeof(DWORD);
    memcpy(&lo, p + offset, sizeof(WORD));
    if (lo != 0xFFFF)
    {
        WString res;
        char tmp[] = {0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00};
        if (!memcmp(p + offset, tmp, sizeof(tmp)))
        {
            offset += 8;
            Type = 0;
        } else
        {
            offset += sizeof(WORD);
            while (lo)
            {
                memcpy(&lo, p + offset, sizeof(WORD));
                res.Add() = (wchar_t) lo;
                offset += sizeof(WORD);
            }
            res.Add() = 0;
            Type = res.GetPtr();
        }


    } else
    {
        offset += sizeof(WORD);
        memcpy(&lo, p + offset, sizeof(WORD));
        Type = lo;
        offset += sizeof(WORD);
    }
    memcpy(&lo, p + offset, sizeof(WORD));
    offset += sizeof(WORD);
    if (lo != 0xFFFF)
    {
        WString res;
        while (lo)
        {
            memcpy(&lo, p + offset, sizeof(WORD));
            res.Add() = (wchar_t) lo;
            offset += sizeof(WORD);
        }
        res.Add() = 0;
        Name = res.GetPtr();


    } else
    {
        memcpy(&lo, p + offset, sizeof(WORD));
        Name = lo;
        offset += sizeof(WORD);
    }
    memcpy(&DataVersion, p + offset, sizeof(DWORD));
    offset += sizeof(DWORD);


    memcpy(&MemoryFlags, p + offset, sizeof(WORD));
    offset += sizeof(WORD);

    memcpy(&LanguageId, p + offset, sizeof(WORD));
    offset += sizeof(WORD);
    memcpy(&Version, p + offset, sizeof(DWORD));
    offset += sizeof(DWORD);
    memcpy(&Characteristics, p + offset, sizeof(DWORD));
    offset += sizeof(DWORD);
    if (offset > buff.GetLen())
    {
        DataSize = HeaderSize = 0;
    }
    return IsValid();

}

#ifndef _CONSOLE_PROG_
#include "mdc.h"
#ifdef __PLUK_LINUX__
#endif

IMPLEMENT_DYNAMIC_CLASS(MInstance, wxApp)
//IMPLEMENT_APP(MInstance)


#endif

static char TmpDir[MAX_PATH] = {0};
EXPORTNIX char *TmpNam2(char *tmp, const char *ext);
EXPORTNIX char *TmpNam(char *tmp)
{
    return TmpNam2(tmp, "XXXXXX");

}
EXPORTNIX char *TmpNam2(char *tmp, const char *ext)
{
    TString t;
    t.Format("%s/XXXXXX.%s", TmpDir, ext);
    strcpy(tmp, t.GetPtr());
    int fd = mkstemp(tmp);
    if (fd > 0)
    {
        close(fd);
        return tmp;

    }

    return mktemp(tmp);
}

Semaphore *test_sem;
int bIdleEvent = 0;

MInstance *MInstance::m_Instance = 0;

EXPORTNIX void WinMain(char **a, int b)
{
#ifndef _CONSOLE_PROG_
    MInstance::m_Instance = new MInstance();
#else
    MInstance::m_Instance = new MInstance(a, b);
#endif
}

MInstance::_tMain MInstance::MainApp = WinMain;
int MInstance::iCheckThreadEvents;
RBHashMap<TString, void *> *MInstance::dlls = 0;
EpollThread *ePollGLB = 0;
MPThread *eMSGProc = 0;
int MInstance::fBlocking = 0;
//Display *MInstance::__display;
//int MInstance::__screen;

#ifndef WIN32

#include "uuid.h"

#ifdef ANDROID_NDK

struct ABitmapCreator {
    JVariable param[3];
    AObject bitmap;
    jclass jbitmap_class;
    jmethodID idCreateBmp;
    jclass bcfg_class;
    jobject rgbObj;
    JVariable res;
    CriticalSection sec;

    ABitmapCreator(JNIEnv *env)
    {
        bitmap.New("android.graphics.Bitmap", 0, 0, AObject::TJGLOBAL, env);
        AObject const_v;
        const_v.New("android/graphics/Bitmap$Config", 0, 0, AObject::TJDEFAULT, env);
        JVariable prm;
        prm = "ARGB_8888";
        const_v.Run("valueOf", &prm, 1, param[2]);
        param[2].ToGlobal();


        JPtr tmpClass = env->FindClass("android/graphics/Bitmap");
        jbitmap_class = (jclass) env->NewGlobalRef(tmpClass);
        idCreateBmp = env->GetStaticMethodID(jbitmap_class, "createBitmap",
                                             "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        tmpClass = env->FindClass("android/graphics/Bitmap$Config");
        bcfg_class = (jclass) env->NewGlobalRef(tmpClass);
        JPtr type = env->NewStringUTF("ARGB_8888");
        jmethodID idValueOf = env->GetStaticMethodID(bcfg_class, "valueOf",
                                                     "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
        JPtr rgbObj1 = (jobject) env->CallStaticObjectMethod(bcfg_class, idValueOf, (jobject) type);
        rgbObj = env->NewGlobalRef(rgbObj1);

    }

    jobject Create(int w, int h, AndroidBitmapInfo *info)
    {

        JNIEnv *env = MInstance::GetLocalJNI();
/*
		param[0] = w;
		param[1] = h;
		bitmap.Run("createBitmap", param, 3, res, env);
		jobject bmp = res.obj;
		res.Detach();
		*/
        sec.Enter();
        jobject bmp = env->CallStaticObjectMethod(jbitmap_class, idCreateBmp, w, h, rgbObj);
        if (bmp && info)
            AndroidBitmap_getInfo(env, bmp, info);
        sec.Leave();

        return bmp;

    }
};

EXPORTNIX jlong getNativeHeapAllocatedSize()
{
    JNIEnv *env = MInstance::GetLocalJNI();
    jclass clazz = env->FindClass("android/os/Debug");
    MInstance::ClearException(env);
    jlong ret = -1;

    if (clazz)
    {
        jmethodID id = env->GetStaticMethodID(clazz, "getNativeHeapAllocatedSize", "()J");
        MInstance::ClearException(env);
        if (id)
        {
            ret = env->CallStaticLongMethod(clazz, id);
            MInstance::ClearException(env);
        }
    }
    return ret;
}
EXPORTNIX jlong getNativeHeapFreeSize()
{
    JNIEnv *env = MInstance::GetLocalJNI();
    jclass clazz = env->FindClass("android/os/Debug");
    MInstance::ClearException(env);
    jlong ret = -1;
    if (clazz)
    {
        jmethodID id = env->GetStaticMethodID(clazz, "getNativeHeapFreeSize", "()J");
        MInstance::ClearException(env);
        if (id)
        {
            ret = env->CallStaticLongMethod(clazz, id);
            MInstance::ClearException(env);
        }
    }
    return ret;
}
EXPORTNIX jlong getNativeHeapSize()
{
    JNIEnv *env = MInstance::GetLocalJNI();
    jclass clazz = env->FindClass("android/os/Debug");
    MInstance::ClearException(env);
    jlong ret = -1;
    if (clazz)
    {
        jmethodID id = env->GetStaticMethodID(clazz, "getNativeHeapSize", "()J");
        MInstance::ClearException(env);
        if (id)
        {
            ret = env->CallStaticLongMethod(clazz, id);
            MInstance::ClearException(env);
        }
    }
    return ret;
}

//---------------------------------------------
//
//---------------------------------------------
jclass MInstance::FindClass(const char *sp, BOOL fJar)
{
    JNIEnv *env = GetLocalJNI();
    jclass load = 0;
    if (!fJar)
    {
        TString tmp = sp;
        tmp.ReplaceAll(".", "/");
        load = env->FindClass(tmp.GetPtr());
        ClearException(env, 0);
    }
    if (!load && loader)
    {
        JPtr na = env->NewStringUTF(sp);
        jobject cl = env->CallObjectMethod((jclass) loader->urlLoader, loader->loadClass, na.str);
        if (!ClearException(env, 0))
        {

            load = (jclass) cl;
        }
    }
    return load;
}
MInstance::METRICS *MInstance::GetMetrics()
{
    return &metrics;
}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::InitJNI(JNIEnv *m_JavaVMEnv)
{
    BOOL f = m_JavaVMEnv->GetJavaVM(&m_JavaVM) != 0;
//	if (f)
    {
        MInstance::clazz = new MHashMap<AClass *, TString>();
        clazz_section = new CriticalSection();
        JPtr throwable1 = m_JavaVMEnv->FindClass("java/lang/Throwable");
        throwable = (jclass) m_JavaVMEnv->NewGlobalRef(throwable1);
        throwable_getCause = m_JavaVMEnv->GetMethodID(throwable, "getCause",
                                                      "()Ljava/lang/Throwable;");
        throwable_getStackTrace = m_JavaVMEnv->GetMethodID(throwable, "getStackTrace",
                                                           "()[Ljava/lang/StackTraceElement;");
        throwable_toString = m_JavaVMEnv->GetMethodID(throwable, "toString",
                                                      "()Ljava/lang/String;");
        JPtr stacktrace1 = m_JavaVMEnv->FindClass("java/lang/StackTraceElement");
        stacktrace = (jclass) m_JavaVMEnv->NewGlobalRef(stacktrace1);
        stacktrace_toString = m_JavaVMEnv->GetMethodID(stacktrace, "toString",
                                                       "()Ljava/lang/String;");
        bmp_instance = new ABitmapCreator(m_JavaVMEnv);
    }
    return 1;

}

void MInstance::ThrowTrace(JNIEnv *env, TString &msg, jthrowable except)
{
    jobjectArray arr = (jobjectArray) env->CallObjectMethod(except, throwable_getStackTrace);
    if (arr)
    {
        int len = env->GetArrayLength(arr);
        jstring jstr = (jstring) env->CallObjectMethod(except, throwable_toString);
        const char *jnamestr = env->GetStringUTFChars(jstr, NULL);
        TString str = jnamestr;
        env->ReleaseStringUTFChars(jstr, jnamestr);
        if (msg.StrLen())
        {
            msg += "Cause : ";
            msg += str;
        } else
        {
            msg = str;
            if (!len)
                msg += "\r\n";
        }
        for (int i = 0; i < len; i++)
        {
            jobject frame = env->GetObjectArrayElement(arr, i);
            jstr = (jstring) env->CallObjectMethod(frame, stacktrace_toString);
            const char *jnamestr = env->GetStringUTFChars(jstr, NULL);
            str = jnamestr;
            env->ReleaseStringUTFChars(jstr, jnamestr);
            msg += "\r\n    ";
            msg += str;
            env->DeleteLocalRef(frame);
        }

        jthrowable cause = (jthrowable) env->CallObjectMethod(except, throwable_getCause);
        if (cause)
        {
            ThrowTrace(env, msg, cause);
            env->DeleteLocalRef(cause);
        }
        env->DeleteLocalRef(arr);

    }
}

//--------------------------------------
//
//--------------------------------------
BOOL MInstance::ClearException(JNIEnv *env, BOOL fTrace)
{
    jthrowable except = 0;
    if (env->ExceptionCheck() || (except = env->ExceptionOccurred()))
    {
        if (!except)
            except = env->ExceptionOccurred();
        env->ExceptionDescribe();
        env->ExceptionClear();
        if (fTrace)
        {
            TString msg;
            ThrowTrace(env, msg, except);
            msg += "\r\n";
            TRACE(msg.GetPtr());
        }
        if (except)
            env->DeleteLocalRef(except);
        return true;
    }
    return false;
}

jobject MInstance::CreateAndroidBitmap(int w, int h, AndroidBitmapInfo *info)
{
    jobject b = bmp_instance->Create(w, h, info);
    return b;
}

void MInstance::ReleaseJNI()
{
    if (loader)
        delete loader;
    loader = 0;
    MHashMap<AClass *, TString>::iterator it(*clazz);
    for (RBData<AClass *, TString> *b = it.begin(); b; b = it.next())
        delete b->value;
    (*clazz).Clear();
    if (m_JavaVM)
    {
        JNIEnv *env = GetLocalJNI();
        env->DeleteLocalRef(throwable);
        env->DeleteLocalRef(stacktrace);
        m_JavaVM->DetachCurrentThread();
        //m_JavaVM->DestroyJavaVM();
    }
    m_JavaVM = 0;
}

//--------------------------------------
//
//--------------------------------------
BOOL MInstance::InitSpec(const char *sp, MHashSet<TString> *addTypes, bool fInitAll)
{

    BOOL fRet = 0;
    if (!m_JavaVM) return 0;
    TString tmp, tsp = sp;
    if (tsp == "java.lang.Object") return 1;
    MHashSet<TString> _addTypes;
    if (LookupAClass(tsp)) return 1;

    JNIEnv *env = GetLocalJNI();
    CreateClassUtil utils(env);

    tmp = sp;
    tmp.ReplaceAll(".", "/");
    AClass *cc = new AClass();
    JPtr load = env->FindClass(tmp.GetPtr());
    if (ClearException(env, 0))
    {
        if (loader)
        {
            JPtr na = env->NewStringUTF(sp);
            JPtr cl = env->CallObjectMethod((jclass) loader->urlLoader, loader->loadClass,
                                            (jobject) na);
            if (!ClearException(env, 0))
                cc->Init(cl, utils, fInitAll);
        }

    } else
    {
        cc->Init(load, utils);
    }
    if ((fRet = cc->IsValid()))
    {
        cc->type = TJOBJECT;
        AddAClass(tsp, cc);
    } else
    {
        clazz->Delete(tsp);
        delete cc;
    }

    return fRet;
}
//---------------------------------------------
//
//---------------------------------------------
/*
JClass *MInstance::GetJClass(JNIEnv *env, const char *name)
{
		JClass *spec = 0;
		if (!MInstance::clazz->Lookup(name, spec))
		{
			InitSpec(name);
			MInstance::clazz->Lookup(name, spec);
		}
		return spec;

}
*/
//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::InitSpec(const char *sp, bool fInitAll)
{
    if (LookupAClass(sp))
        //if ((*clazz).Lookup(sp))
    {
        return true;
    }

    BOOL fRet = 0;
    JNIEnv *env = GetLocalJNI();
    CreateClassUtil utils(env);


    TString tmp = sp;
    tmp.ReplaceAll(".", "/");
    AClass *cc = new AClass();
    JPtr load = env->FindClass(tmp.GetPtr());
    if (!ClearException(env, 0))
    {
        cc->Init(load, utils, fInitAll);
    }
    if ((fRet = cc->IsValid()))
    {
        cc->type = TJOBJECT;
        AddAClass(sp, cc);
        fRet = 1;
    } else
    {
        clazz->Delete(sp);
        delete cc;
    }

    return fRet;

}

jobject MInstance::CreateJObject(const char *sp)
{
    if (!m_JavaVM) /* || !loader) */ return 0;
    TString tmp, tsp = sp;
    if (!LookupAClass(tsp))
    {
        InitSpec(sp, 0, false);
        if (!LookupAClass(sp)) return 0;
    }
    AClass *cc = MInstance::GetAClass(tsp);

    JNIEnv *env = GetLocalJNI();
    jobject tst = 0;
    tmp = sp;
    tmp.ReplaceAll(".", "/");
    JPtr load = env->FindClass(tmp.GetPtr());
    if (!ClearException(env))
    {
        jmethodID init = env->GetMethodID(load, "<init>", "()V");
        if (!MInstance::ClearException(env))
            tst = env->NewObject(load, init);
    }
    return tst;
}

void MInstance::DetachThreadJNI()
{
    m_JavaVM->DetachCurrentThread();
}

//---------------------------------------------
//
//---------------------------------------------
JNIEnv *MInstance::GetLocalJNI()
{

    JNIEnv *env = 0;
    if (m_JavaVM)
    {
        int status = m_JavaVM->GetEnv((void **) &env, JNI_VERSION_1_6);
        if (status == JNI_EDETACHED)
        {
            status = m_JavaVM->AttachCurrentThread(&env, 0);
            if (status != JNI_OK)
                return 0;
        }
    }
    return env;
}
//---------------------------------------------
//
//---------------------------------------------
/*
jobject  MInstance::GetRootViwFromActivity (JNIEnv *pJniEnv, jobject activity)
{
	jobject root = 0;
	if (activity)
	{
		JPtr cl1, cl = pJniEnv->GetObjectClass(activity);
		if (cl)
		{
			jmethodID id =  GetMethodID(pJniEnv, cl, "getWindow", "()Landroid/view/Window;");

			if (id)
			{

				JPtr wnd = pJniEnv->CallObjectMethod(activity, id);
				if (wnd)
				{
					cl1 = pJniEnv->GetObjectClass(wnd);
					if (cl1)
					{
						id =  GetMethodID(pJniEnv, cl1, "getDecorView", "()Landroid/view/View;");
						if (id)
						{
							wnd = pJniEnv->CallObjectMethod(wnd, id);
							if (wnd)
							{
								cl1 = pJniEnv->GetObjectClass(wnd);
								if (cl1)
								{
									id =  GetMethodID(pJniEnv, cl1, "getRootView", "()Landroid/view/View;");
									if (id)
										root = pJniEnv->CallObjectMethod(wnd, id);

								}
							}
						}
					}
				}
			}
		}
	}
	return root;
}
*/
//*****************************************************************
//---------------------------------------------
//
//---------------------------------------------
bool MInstance::LookupAClass(LPCSTR name)
{
    bool fRet;
    clazz_section->Enter();
    if (!strchr(name, '.'))
    {
        TString str = name;
        str.ReplaceAll("/", ".");
        fRet = clazz->Lookup(str.GetPtr());
    } else
        fRet = clazz->Lookup(name);
    clazz_section->Leave();
    return fRet;

}

//---------------------------------------------
//
//---------------------------------------------
void MInstance::AddAClass(LPCSTR name, AClass *cls)
{
    clazz_section->Enter();
    if (!strchr(name, '.'))
    {
        TString str = name;
        str.ReplaceAll("/", ".");
        (*clazz)[str.GetPtr()] = cls;
    } else
        (*clazz)[name] = cls;


    clazz_section->Leave();

}

//---------------------------------------------
//
//---------------------------------------------
void MInstance::DeleteAClass(LPCSTR name)
{
    clazz_section->Enter();
    if (!strchr(name, '.'))
    {
        TString str = name;
        str.ReplaceAll("/", ".");
        clazz->Delete(str.GetPtr());
    } else
        clazz->Delete(name);
    clazz_section->Leave();

}

//---------------------------------------------
//
//---------------------------------------------
AClass *MInstance::GetAClass(LPCSTR decl_class)
{
    AClass *spec;

    clazz_section->Enter();
    if (decl_class)
    {
        if (!strchr(decl_class, '.'))
        {
            TString str = decl_class;
            str.ReplaceAll("/", ".");
            spec = (*clazz)[str.GetPtr()];
        } else
            spec = (*clazz)[decl_class];
    }
    clazz_section->Leave();
    return spec;

}

//********************************************************************
//---------------------------------------------
//
//---------------------------------------------
TString MInstance::GetNameJClass(JNIEnv *pJniEnv, jclass cls)
{
    TString ret;
    JPtr cl1 = pJniEnv->GetObjectClass(cls);
    if (!ClearException(pJniEnv))
    {

        jmethodID id = GetMethodID(pJniEnv, cl1, "getName", "()Ljava/lang/String;");
        if (!ClearException(pJniEnv))
        {
            JPtr t = pJniEnv->CallObjectMethod(cls, id);
            if (!ClearException(pJniEnv))
                ret = t.String();
        }
    }
    return ret;

}

//---------------------------------------------
//
//---------------------------------------------
TString MInstance::GetNameObject(JNIEnv *env, jobject _cls)
{
    if (!_cls) return "";
    jmethodID id;
    TString ret;
    if (_cls)
    {
        jmethodID id, id2;
        JPtr t, cls, cl2, cl1 = env->GetObjectClass(_cls);
        if (cl1.obj)
        {

            id = env->GetMethodID(cl1.cls, "getClass", "()Ljava/lang/Class;");
            if (!ClearException(env) && id)
            {

                cls = env->CallObjectMethod(_cls, id);
                if (!ClearException(env) && cls.obj)
                {

                    cl2 = env->GetObjectClass(cls.obj);
                    if (!ClearException(env) && cl2.obj)
                    {

                        id2 = env->GetMethodID(cl2.cls, "getName", "()Ljava/lang/String;");
                        if (!ClearException(env) && id2)
                        {
                            t = env->CallObjectMethod(cls.obj, id2);
                            if (!ClearException(env) && t.obj)
                                ret = t.String();
                        }
                    }


                }
            }
        }
    }
    return ret;
/*	JPtr cl1 = pJniEnv->GetObjectClass(_cls);
	if (!MInstance::ClearException(pJniEnv))
	if ((bool)cl1)
	{
		id =  GetMethodID(pJniEnv, cl1, "getClass", "()Ljava/lang/Class;");
		if (!MInstance::ClearException(pJniEnv))
		{
			JPtr cls = pJniEnv->CallObjectMethod(_cls, id);
			if (!MInstance::ClearException(pJniEnv))
				return GetNameJClass(pJniEnv, cls.cls);
		}
	}
 return "";
*/

}

//---------------------------------------------
//
//---------------------------------------------
jmethodID MInstance::GetMethodID(JNIEnv *pJniEnv, jclass clazz, const char *name, const char *sig)
{
    jmethodID id = pJniEnv->GetMethodID(clazz, name, sig);

    if (MInstance::ClearException(pJniEnv))
        return 0;
    return id;

}

jfieldID MInstance::GetFieldID(JNIEnv *pJniEnv, jclass clazz, const char *name, const char *sig)
{
    jfieldID id = pJniEnv->GetFieldID(clazz, name, sig);
    if (MInstance::ClearException(pJniEnv))
        return 0;
    return id;
}

//---------------------------------------------
//
//---------------------------------------------
int MInstance::GetChildren(JNIEnv *pJniEnv, jobject wnd, RBMap<TString, jobject> &childs)
{
    JPtr cl1 = pJniEnv->GetObjectClass(wnd);
    if ((bool) cl1)
    {
        TString ch; //, root = GetNameClass(pJniEnv, wnd);

        jmethodID id;
        id = GetMethodID(pJniEnv, cl1, "getChildCount", "()I");
        MInstance::ClearException(pJniEnv);
        if (id)
        {
            jint N = pJniEnv->CallIntMethod(wnd, id);
            MInstance::ClearException(pJniEnv);
            if (N)
            {
                id = GetMethodID(pJniEnv, cl1, "getChildAt", "(I)Landroid/view/View;");
                MInstance::ClearException(pJniEnv);
                if (id)
                    for (int i = 0; i < N; ++i)
                    {
                        JPtr child = pJniEnv->CallObjectMethod(wnd, id, i);
                        MInstance::ClearException(pJniEnv);
                        if ((bool) child)
                        {
                            ch = GetNameObject(pJniEnv, child);
                            childs[child] = ch;
//							GetChildren(pJniEnv, child, childs);
                        }

                    }
            }


        }
    }
    return childs.m_iNum;
}

//---------------------------------------------
//
//---------------------------------------------
void MInstance::SetWorkDir(JNIEnv *pJniEnv, jclass, jobject obj)
{

//	TString str;
    if (obj)
    {
        JPtr clazz = pJniEnv->GetObjectClass(obj);
        MInstance::ClearException(pJniEnv);
        if ((bool) clazz)
        {
            /*	jfieldID jfield = pJniEnv->GetFieldID(clazz, "packageName", "Ljava/lang/String;");
                MInstance::ClearException(pJniEnv);
                if(jfield)
                {
                    JPtr t = pJniEnv->GetObjectField(obj, jfield);
                    MInstance::ClearException(pJniEnv);
                    if(t)
                    {

                        str = t.String();

                    }
                }*/
            jfieldID jfield = pJniEnv->GetFieldID(clazz.cls, "dataDir", "Ljava/lang/String;");
            MInstance::ClearException(pJniEnv);
            if (jfield)
            {
                JPtr t = pJniEnv->GetObjectField(obj, jfield);
                MInstance::ClearException(pJniEnv);
                {
                    if ((bool) t)
                    {
                        TString path;
                        path = t.String();
                        HFREE(sPath);
                        sPath = (char *) HMALLOC(path.StrLen() + 1);
                        strcpy(sPath, path.GetPtr());
                        path += "/";
                        path += m_pszAppName;
                        HFREE(ModuleName);
                        ModuleName = (char *) HMALLOC(path.StrLen() + 1);
                        strcpy(ModuleName, path.GetPtr());
                        m_pszAppName = strrchr(ModuleName, FD);
                        ++m_pszAppName;

                    }
                }
            }


        }
    }
}

//---------------------------------------------
//
//---------------------------------------------
void MInstance::SetDisplayMetrics(JNIEnv *pJniEnv, jobject obj)
{

    JPtr clazz = pJniEnv->GetObjectClass(obj);
    MInstance::ClearException(pJniEnv);
    if ((bool) clazz)
    {
        jfieldID id = GetFieldID(pJniEnv, clazz, "widthPixels", "I");
        MInstance::ClearException(pJniEnv);
        if (id)
            metrics.widthPixels = pJniEnv->GetIntField(obj, id);
        if ((id = GetFieldID(pJniEnv, clazz, "heightPixels", "I")))
            metrics.heightPixels = pJniEnv->GetIntField(obj, id);
        else
            MInstance::ClearException(pJniEnv);
        if ((id = GetFieldID(pJniEnv, clazz, "density", "F")))
            metrics.density = pJniEnv->GetFloatField(obj, id);
        else
            MInstance::ClearException(pJniEnv);
        if ((id = GetFieldID(pJniEnv, clazz, "densityDpi", "I")))
            metrics.densityDpi = pJniEnv->GetIntField(obj, id);
        else
            MInstance::ClearException(pJniEnv);
        if ((id = GetFieldID(pJniEnv, clazz, "scaledDensity", "F")))
            metrics.scaledDensity = pJniEnv->GetFloatField(obj, id);
        else
            MInstance::ClearException(pJniEnv);
        if ((id = GetFieldID(pJniEnv, clazz, "xdpi", "F")))
            metrics.xdpi = pJniEnv->GetFloatField(obj, id);
        else
            MInstance::ClearException(pJniEnv);
        if ((id = GetFieldID(pJniEnv, clazz, "ydpi", "F")))
            metrics.ydpi = pJniEnv->GetFloatField(obj, id);
        else
            MInstance::ClearException(pJniEnv);
    }
}

//---------------------------------------------
//
//---------------------------------------------
BOOL FixPath(jobject aaMng, TString &filename)
{
    if (filename.Find("ASSETS/") != -1)
        filename.Replace("ASSETS/", "");

    MVector<TString> tock;
    filename.Tokenize("\\/", tock);
    int N = tock.GetLen();
    TString path = "";
    int fChange = 0;
    AObject obj;
    obj.Attach(aaMng, AObject::TJSTATIC);
    if (obj.obj)
        for (int j, i = 0; i < N; ++i)
        {
            JVariable param, res;
            param = path.StrLen() ? path.GetPtr() : "";
            obj.Run("list", &param, 1, res);
            MVector<TString> arr;
            res.Get(arr);
            bool fFind = false;
            for (int j = 0, l = arr.GetLen(); !fFind && j < l; ++j)
            {
                if (arr[j].StrLen() && tock[i].CompareNoCase(arr[j]) == 0)
                {
                    tock[i] = arr[j];
                    fFind = true;
                    ++fChange;
                }

            }
            /*
            JPtr jpath = jni->NewStringUTF(path.GetPtr() != 0 ? path.GetPtr() : "" );
           MInstance::ClearException(jni);
           {
               bool fFind = false;
               JPtr jpathlist = (jobjectArray)jni->CallObjectMethod(aaMng, list_id, (jstring)jpath);
               MInstance::ClearException(jni);
               if((bool)jpathlist)
               {
                   for (jsize j = 0, l = jni->GetArrayLength(jpathlist); !fFind && j < l; ++j)
                   {
                       JPtr elem = (jstring)jni->GetObjectArrayElement(jpathlist, j);
                       MInstance::ClearException(jni);

                       if((bool)elem)
                       {
                           TString s1 = elem.String();
                           if (s1.StrLen() && tock[i].CompareNoCase(s1) == 0)
                           {
                               tock[i] = s1;
                               fFind = true;
                               ++fChange;
                           }
                       }
                   }
               }
             */
            if (fFind)
            {
                if (path.StrLen())
                    path += SFD;
                path += tock[i];
            } else
            {
                path.Clear();
                break;
            }
        }


    if (path.StrLen())
    {
        filename = path;
        return true;
    }
    return false;

}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::ResourceFile2TempFile(const char *_filename, CMemFile &res)
{
    if (!_filename) return 0;
    JNIEnv *jni = GetLocalJNI();
    AAssetManager *mng = AAssetManager_fromJava(jni, this->aaMng);
    MInstance::ClearException(jni);;
    TString filename;
    filename = _filename;
    if (filename.Find("ASSETS/") != -1)
        filename.Replace("ASSETS/", "");

    AAsset *file = AAssetManager_open(mng, filename, AASSET_MODE_UNKNOWN);
    MInstance::ClearException(jni, 0);
    if (!file) return 0;
    BOOL fRes = false;
    off_t size = AAsset_getLength(file);
    MInstance::ClearException(jni, 0);
    if ((int) size > 0)
    {
        res.buff.SetLen(size);
        off_t iRead = AAsset_read(file, res.buff.GetPtr(), size);
        MInstance::ClearException(jni, 0);
        fRes = iRead == size;
        res.SetSeek(0);

    }
    AAsset_close(file);
    MInstance::ClearException(jni);
    return fRes;
}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::IsResourceFile(const char *filename)
{
    if (!filename) return 0;
    JNIEnv *jni = GetLocalJNI();
    AAssetManager *mng = AAssetManager_fromJava(jni, this->aaMng);
    MInstance::ClearException(jni, 0);
    if (!mng) return 0;
    AAsset *file = AAssetManager_open(mng, filename, AASSET_MODE_UNKNOWN);
    MInstance::ClearException(jni, 0);
    if (!file)
    {

        TString path = filename;
        if (path.Find("ASSETS/") != -1)
            path.Replace("ASSETS/", "");

        if (FixPath(aaMng, path))
        {
            file = AAssetManager_open(mng, path.GetPtr(), AASSET_MODE_UNKNOWN);
            MInstance::ClearException(jni);
        }
        MInstance::ClearException(jni);
        if (!file) return 0;

    }
    int size = (int) AAsset_getLength(file);
    AAsset_close(file);
    MInstance::ClearException(jni);
    return size;
}

//---------------------------------------------
//
//---------------------------------------------
EXPORTNIX bool CreateTmpFile(TString &assets)
{
    int fn;
    if ((fn = assets.Find("ASSETS/")) != -1)
    {

        assets.Replace("ASSETS/", "");
        CMemFile f;
        if (MGetApp()->ReadResourceFile(assets, f))
        {
            char _name[MAX_PATH * 2] = {0};
            char *p = TmpNam(_name);

            MFILE file(p);
            if (file.Open(true))
            {
                file.Write(f.buff.GetPtr(), f.buff.GetLen());
                file.Close();
                assets = _name;
                return true;
            }
        }

    }
    return false;
}

//---------------------------------------------
//
//---------------------------------------------
BOOL ExtractResourceFile(LPCSTR _assets, LPCSTR dst, int fCheck)
{
    bool c = false;
    TString assets = _assets;
    CMemFile f;
    if (!fCheck || fCheck == 1)
        c = MGetApp()->ResourceFile2TempFile(assets, f);
    if (c)
    {


        MFILE file(dst);
        if (file.Open(true))
        {
            file.Write(f.buff.GetPtr(), f.buff.GetLen());
            file.Close();
            return true;
        }
    } else if (!fCheck || fCheck == 2)
    {
        TString mdir = dst;
        int pos = assets.ReverseFind(FD);
        if (pos == -1)
        {
            pos = 0;
            if (mdir.Right(1) != "/")
                mdir += "/";
        }
        TString sdir = mdir + assets.Mid(pos);
        CreateDirectory(sdir.GetPtr(), 0);
        MVector<TString> _dir;
        MVector<TString> _files;
        MGetApp()->ReadResourceDir(assets.GetPtr(), "", _dir, _files);

        for (int i = 0, len = _dir.GetLen(); i < len; ++i)
        {
            TString b = assets;
            if (b.Right(1) != "/")
                b += "/";
            b += _dir[i];
            ExtractResourceFile(b, sdir.GetPtr(), 2);
        }
        for (int i = 0, len = _files.GetLen(); i < len; ++i)
        {
            TString b = assets;
            if (b.Right(1) != "/")
                b += "/";
            TString a = sdir;
            if (a.Right(1) != "/")
                a += "/";
            a += _files[i];
            b += _files[i];
            ExtractResourceFile(b.GetPtr(), a.GetPtr(), 1);
        }
    }
    return true;
}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::ExtractResourceDir(LPCSTR src, LPCSTR dst)
{
    int ret = 0;
    int fn;
    TString assets = src;
    if ((fn = assets.Find("ASSETS/")) != -1)
    {
        if (fn)
            assets.Remove(0, fn);
        assets.Replace("ASSETS/", "");
        JNIEnv *jni = GetLocalJNI();
        MDeq<TString> stack;
        stack.PushBack(assets);
        AAssetManager *mng = AAssetManager_fromJava(jni, this->aaMng);
        MInstance::ClearException(jni);
        if (!mng) return 0;
        AObject object;
        object.Attach(aaMng, AObject::TJSTATIC);
        MArray<BYTE> buff;
        while (!stack.IsEmpty())
        {
            MVector<TString> fn;
            TString path = stack.PopFront();
            TString mdir = dst;
            mdir += "/";
            mdir += path;
            CreateDirectory(mdir.GetPtr(), 0);
            AAssetDir *ptr = AAssetManager_openDir(mng, path.GetPtr());
            MInstance::ClearException(jni, 0);
            if (!ptr) return 0;
            TString dp = AAssetDir_getNextFileName(ptr);
            MInstance::ClearException(jni, 0);
            while (dp.StrLen())
            {

                fn.Add() = dp;
                TString pp = path;
                pp += "/";
                pp += dp;
                AAsset *file = AAssetManager_open(mng, pp.GetPtr(), AASSET_MODE_UNKNOWN);
                MInstance::ClearException(jni);
                if (!file) return 0;
                off_t size = AAsset_getLength(file);
                MInstance::ClearException(jni);
                if ((int) size > 0)
                {
                    buff.SetLen(size);
                    off_t iRead = AAsset_read(file, buff.GetPtr(), size);
                    MInstance::ClearException(jni);
                    if (iRead == size)
                    {
                        TString p = dst;
                        p += "/";
                        p += pp;
                        MFILE _file(p);
                        if (_file.Open(true))
                        {
                            _file.Write(buff.GetPtr(), buff.GetLen());
                            _file.Close();
                            ++ret;
                        }
                    }
                }
                AAsset_close(file);
                MInstance::ClearException(jni);
                dp = AAssetDir_getNextFileName(ptr);
                MInstance::ClearException(jni);

            }
            AAssetDir_close(ptr);
            JVariable jvar, jres;
            jvar = path.GetPtr();
            object.Run("list", &jvar, 1, jres);
            MVector<TString> arr;
            jres.Get(arr);
            for (int i = 0, l = arr.GetLen(); i < l; ++i)
            {
                TString &p = arr[i];
                MInstance::ClearException(jni);
                if (p.StrLen() && fn.Find(p) == -1)
                {
                    TString pp = path;
                    pp += "/";
                    pp += p;
                    stack.PushBack(pp);
                }
            }
        }
    }
    return ret;
}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::ExtractResourceFile(LPCSTR src, LPCSTR dst)
{
    int fn;
    TString assets = src;
    if ((fn = assets.Find("ASSETS/")) != -1)
    {
        if (fn)
            assets.Remove(0, fn);
        assets.Replace("ASSETS/", "");
        return ::ExtractResourceFile(assets, dst, 0);
    }
    return false;
}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::ReadResourceFile(LPCSTR path_src, CMemFile &res)
{
    TString assets = path_src;
    int fn;
    if ((fn = assets.Find("ASSETS/")) != -1)
    {
        if (fn)
            assets.Remove(0, fn);
        assets.Replace("ASSETS/", "");
    }
    path_src = assets.GetPtr();
    if (!ResourceFile2TempFile(path_src, res))
    {
        TString path = path_src;
        if (FixPath(aaMng, path))
            ResourceFile2TempFile(path.GetPtr(), res);

    }
    return res.buff.GetLen() != 0;

}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::ReadResourceDir(LPCSTR path, LPCSTR mask, MVector<TString> &dir, MVector<TString> &file)
{
    JNIEnv *jni = GetLocalJNI();
    TString msk = mask;
    TString assets = path;
    if ((assets.Find("ASSETS/")) != -1)
    {
        assets.Replace("ASSETS/", "");
        path = assets.GetPtr();
    }
    if (msk == "*.*" || !msk.StrLen())
        msk = "*";
    if (msk == "*")
    {
        AObject object;
        object.Attach(aaMng, AObject::TJSTATIC);
        JVariable param, jarr;
        param = path;
        object.Run("list", &param, 1, jarr);
        jarr.Get(dir);

        AAssetManager *mng = AAssetManager_fromJava(jni, this->aaMng);
        MInstance::ClearException(jni);
        if (!mng) return 0;
        AAssetDir *ptr = AAssetManager_openDir(mng, path);
        MInstance::ClearException(jni);
        if (!ptr) return 0;
        const char *dp = AAssetDir_getNextFileName(ptr);
        MInstance::ClearException(jni);
        int i;
        TString tp, msk2;
        if (dp)
            tp = dp;
        bool fTest = dir.GetLen() > 0;
        while (tp.StrLen())
        {
            if (fTest)
                if ((i = dir.Find(tp)) != -1)
                {
                    if (msk == "*" || !fnmatch(msk.GetPtr(), tp.GetPtr(), 0))
                        file.Add() = dp;
                    dir.Remove(i, 1);
                }
            dp = AAssetDir_getNextFileName(ptr);
            MInstance::ClearException(jni);
            if (dp)
                tp = dp;
            else
                tp.Clear();
        }
        AAssetDir_close(ptr);
        MInstance::ClearException(jni);

    } else
    {
        AAssetManager *mng = AAssetManager_fromJava(jni, this->aaMng);
        MInstance::ClearException(jni);
        if (!mng) return 0;
        AAssetDir *ptr = AAssetManager_openDir(mng, path);
        MInstance::ClearException(jni);
        if (!ptr) return 0;
        const char *dp = AAssetDir_getNextFileName(ptr);
        MInstance::ClearException(jni);
        TString tp = dp, msk2;
        msk2 = msk;
        msk.Upper();
        msk2.Lower();
        while (tp.StrLen())
        {
            if (!fnmatch(msk.GetPtr(), tp.GetPtr(), 0) || !fnmatch(msk2.GetPtr(), tp.GetPtr(), 0))
                file.Add() = dp;
            dp = AAssetDir_getNextFileName(ptr);
            MInstance::ClearException(jni);
            if (dp)
                tp = dp;
            else
                tp.Clear();
        }
        AAssetDir_close(ptr);
        MInstance::ClearException(jni);
    }

    return 1;
}

#if (0)

extern "C" JNIEXPORT void JNICALL Java_gr_compucon_futura_MyApp_InitInstance
  (JNIEnv *_pJniEnv,  jobject self, jobject obj, jobject metrics, jobject amng)
{

    MInstance::MainApp(0, 0);
    MInstance::m_Instance->InitJNI(_pJniEnv);
    JNIEnv *pJniEnv =MInstance::m_Instance->GetLocalJNI();
    if (metrics)
    MInstance::m_Instance->SetDisplayMetrics(_pJniEnv,  metrics);
    MInstance::m_Instance->SetWorkDir(pJniEnv,  0, obj);
    MInstance::m_Instance->aaMng = pJniEnv->NewGlobalRef(amng);
    JPtr clazz = pJniEnv->GetObjectClass(self);

    jmethodID id = pJniEnv->GetMethodID(clazz, "getApplicationContext",	"()Landroid/content/Context;");
    MInstance::ClearException(pJniEnv);
    if (id)
    {
        JPtr context = pJniEnv->CallObjectMethod(self, id);
        if (context)
        {
            clazz = pJniEnv->GetObjectClass(context);
            MInstance::ClearException(pJniEnv);

            jmethodID getCacheDir = pJniEnv->GetMethodID(clazz, "getCacheDir", "()Ljava/io/File;");
            MInstance::ClearException(pJniEnv);
            JPtr file = pJniEnv->CallObjectMethod(context, getCacheDir);
            clazz = pJniEnv->GetObjectClass(file);
            MInstance::ClearException(pJniEnv);
            jmethodID getAbsolutePath = pJniEnv->GetMethodID(clazz, "getAbsolutePath", "()Ljava/lang/String;");
            MInstance::ClearException(pJniEnv);
            JPtr jpath = (jstring)pJniEnv->CallObjectMethod(file, getAbsolutePath);
            MInstance::ClearException(pJniEnv);
            TString b = jpath.String();
            strcpy(TmpDir, b.GetPtr());
            MInstance::ClearException(pJniEnv);
        }
    }




}
extern "C" JNIEXPORT void JNICALL Java_gr_compucon_futura_MyApp_OnInitInstance(JNIEnv *, jobject)
{
    TString m_path = MGetApp()->sPath;
    TString  m_temp_path = m_path + TEXT("/cache");
    if (GetFileAttributes(m_temp_path.GetPtr()) == 0xffffffff)
        CreateDirectory(m_temp_path.GetPtr(), 0);

    MInstance::m_Instance->InitInstance();
}
#endif

extern void InitErrorHandles();

//---------------------------------------------
//
//---------------------------------------------

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{

    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;
    InitErrorHandles();

    MInstance::InitJNI(env);
    return JNI_VERSION_1_6;
}

#endif

//-------------------------------------
//
//-------------------------------------
EXPORTNIX HINSTANCE AfxGetResourceHandle(LPCSTR dllname/* = 0*/)
{
    if (dllname)
    {
        RBHashMap<TString, void *>::iterator it(*MInstance::dlls);
        for (RBData<TString, void *> *p = it.begin(); p; p = it.next())
            if (p->value == dllname)
                return p->key;
    }
    return MGetApp()->hResHandle;
}
//-------------------------------------
//
//-------------------------------------
EXPORTNIX void AfxSetResourceHandle(HINSTANCE h)
{
    if (MInstance::dlls && MInstance::dlls->Lookup(h))
        MGetApp()->hResHandle = h;
}

//---------------------------------------------
//
//---------------------------------------------

EXPORTNIX int GetModuleFileName(HINSTANCE h, LPSTR buff, int len)
{
#ifndef ANDROID_NDK
    link_map  *info = 0;
#endif

    if (!h || (__pid_t) (size_t) h == getpid())
    {
        MInstance *inst = MInstance::m_Instance;
        if (len < strlen(inst->ModuleName) + 1) return 0;
        strcpy(buff, inst->ModuleName);
    } else
#ifndef ANDROID_NDK
        if (dlinfo(h, RTLD_DI_ORIGIN , (void *)path) > -1)
        {

            if (dlinfo(h, RTLD_DI_LINKMAP, (void *)&info) > -1)
            {
                strcat(path, "/");
                strcat(path, info->l_name);
                if (len < strlen(path) + 1) return 0;
                strcpy(buff, path);
            }
        }
#else
    {
        TString path;
        if (MInstance::dlls->Lookup(h, path))
        {
            if (len < path.StrLen() + 1) return 0;
            strcpy(buff, path.GetPtr());

        }
    }
#endif
    return 1;
}
//-------------------------------------
//
//-------------------------------------
extern "C" int RegisterDll(void *s, const char *name)
{
    if (!MInstance::dlls)
    {
        /*   if (0)
           {
       umask(0);
           setsid();
           setpgrp();

           freopen( "/dev/null", "r", stdin);
           freopen( "/dev/null", "w", stdout);
           freopen( "/dev/null", "w", stderr);
           }
          */
        MInstance::dlls = new RBHashMap<TString, void *>;
    }
    TString nm = name;
    int i = nm.ReverseFind(FD);
    if (i == -1)
        (*MInstance::dlls)[s] = name;
    else
        (*MInstance::dlls)[s] = nm.Tail(i + 1);

    // fprintf(stderr, "register 0x%lx, %s\r\n", (unsigned int)s,  name);
    return 1;
}
//-------------------------------------
//
//-------------------------------------
extern "C" int UnRegisterDll(void *s, const char *name)
{
    if (MInstance::dlls)
    {
        MInstance::dlls->Delete(s);
        if (!MInstance::dlls->Len())
        {
            delete MInstance::dlls;
            MInstance::dlls = 0;
        }
    }
    //fprintf(stderr, "unregister  0x%x, %s\r\n", (unsigned int)s,  name);
    return 1;
}
#if (0)//NOPLUK
//-------------------------------------
//
//-------------------------------------
__attribute__((constructor))
static void MInstanceLoadDLL(void) 
{

#ifndef ANDROID_NDK

    Dl_info dl_info;
    dladdr((const void *)MInstanceLoadDLL, &dl_info);
    void *dl = dlopen(dl_info.dli_fname, RTLD_LAZY);
    if (dl) dlclose(dl);
    RegisterDll(dl, dl_info.dli_fname);
    fprintf(stderr, "module %s loaded\n", dl_info.dli_fname);
#else
   Dl_info dl_info;
   ++done;
    dladdr((const void *)MInstanceLoadDLL, &dl_info);

//    MInstance::m_Instance = new MInstance();
 //   TRACE("=======>>>>standart module %s loaded\n", dl_info.dli_fname);
#endif
}

//-------------------------------------
//
//-------------------------------------
__attribute__((destructor))
static void MInstanceFreeDLL(void) 
{

#ifndef ANDROID_NDK
    Dl_info dl_info;
    dladdr((const void *)MInstanceFreeDLL, &dl_info);
    void *dl = dlopen(dl_info.dli_fname, RTLD_LAZY);
    if (dl) dlclose(dl);
    UnRegisterDll(dl, dl_info.dli_fname);
    fprintf(stderr, "module %s free\n", dl_info.dli_fname);
#else
    Dl_info dl_info;
    dladdr((const void *)MInstanceFreeDLL, &dl_info);
    //delete MInstance::m_Instance;
//    void *dl = dlopen(dl_info.dli_fname, RTLD_LAZY);
  //  if (dl) dlclose(dl);
    //UnRegisterDll(dl, dl_info.dli_fname);
    //fprintf(stderr, "module %s free\n", dl_info.dli_fname);

#endif
}
#endif
//-------------------------------------
//
//-------------------------------------
extern "C" void CoCreateGuid(GUID *g)
{
        uuid_generate((UCHAR *)g);

}
extern "C" BOOL IsEqualGUID(GUID &g1, GUID &g2)
{
            
           return uuid_compare((UCHAR *)&g1, (UCHAR *)&g2) == 0;
}
#endif
#ifdef __PLUK_LINUX__
//-------------------------------------
//
//-------------------------------------
void *PollGLBexit_func(void * arg)
{
    if (ePollGLB)
    delete ePollGLB;
    ePollGLB = 0;
    if (eMSGProc)
    {
        eMSGProc->Stop();
        delete eMSGProc;
        eMSGProc = 0;
    }
    
    TRACE("cleanup func.\n");
	return 0;
}
ProcCon *con;
void *MSGfunc(void *arg)
{
    con = new ProcCon;
    con->Register((LPSTR)arg);
    if (arg) HFREE(arg);
    while (1)
    {
        TMSG msg;
                if (GetMessageA(&msg, TRUE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                }
    }
    return 0;


}
//---------------------------------------------
//
//---------------------------------------------

void *MSGClenfunc(void *arg)
{
   con->Free();
   delete con;
   return 0;
}

//-------------------------------------
//
//-------------------------------------
void *PollGLBfunc(void *arg)
{
    ePollGLB = new EpollThread;
#ifdef _CONSOLE_PROG_
    Sleep(100);
#endif
    ePollGLB->Run();
    return 0;
}
#endif
int tfd = 0;

extern void InitErrorHandles();

//MInstance *mInstance = 0;

#ifndef _CONSOLE_PROG_
wxAssertHandler_t  defAssertHandle;

_EXPORT void PAssertHandler(const wxString& file, int line, const wxString& func, const wxString& cond, const wxString& msg)
{
   ASSERT(0);
  // defAssertHandle(file, line, func, cond, msg);
 
}

int MInstance::OnRun()
{
    return wxApp::OnRun();
}
void MInstance::OnLaunched()
{
    wxApp::OnLaunched();
    Init();
}
//-------------------------------------
//
//-------------------------------------
bool MInstance::OnInit()
{
	bool ret = wxApp::OnInit();

        defAssertHandle =  wxSetAssertHandler(PAssertHandler);
        return 1;

}
int MInstance::Init()
{

#else
//-------------------------------------
//
//-------------------------------------
MInstance::MInstance(char **argv, int argc) : MSGThread(0)
{
#endif
	
#ifdef ANDROID_NDK
    aaMng = 0;
#endif
#ifdef __PLUK_LINUX__
    m_pMainWnd = 0;
    pthread_key_create(&MPThread::dwStartupPtr, 0);
    pthread_key_create(&MPThread::dwMessageWaitBusy, 0);
    pthread_key_create(&MPThread::dwWaitForMultipleObjects, 0);
#ifndef _CONSOLE_PROG_
   pthread_key_create(&MDC::dwDCStockObjects, 0);
   if (!m_Instance)
        m_Instance = this;
   wxRichTextBuffer::AddHandler(new wxRichTextPlainTextHandler);
#endif
 //__display = XOpenDisplay(NULL);
//__screen = DefaultScreen(__display);
 
#else
	MPThread::dwStartupPtr = TlsAlloc();
	MPThread::dwMessageWaitBusy  = TlsAlloc();
    MPThread::dwWaitForMultipleObjects  = TlsAlloc();
#ifndef _CONSOLE_PROG_
		MDC::dwDCStockObjects = TlsAlloc();
#endif
#endif
	main_thread.Attach();
	TString _ModuleName, _sPath;
#ifdef __PLUK_LINUX__
	TString ph;
	_ModuleName.SetLen(PATH_MAX + 1); 
	ph.Format("/proc/%u/exe", getpid());
	//TRACE("%s\r\n", ph.GetPtr());

	readlink(ph.GetPtr(), _ModuleName.GetPtr(), PATH_MAX); 
//	TRACE("moj path ->>>>%s\r\n", _ModuleName.GetPtr());
    m_hInstance = (HINSTANCE)(size_t)getpid();
	hResHandle = (HINSTANCE)m_hInstance;
	hParent = getppid();
   extern char **environ;
    int i, l = 0;
	for (i = 0; environ[i]; ++i)
  		l += strlen(environ[i]) + 1;
	++l;
	Environ = (char *)HMALLOC(l);
	memset(Environ, 0, l);
	char *env = Environ;
	for (i = 0; environ[i]; ++i)
	{
		strcpy(env, environ[i]);
		env += strlen(environ[i]) + 1;
	}
    _sPath = _ModuleName;
    i = _sPath.ReverseFind(FD);
    if (i == -1)
    {
        _sPath.SetLen(255);
        _sPath.Set(0);
        while (!getcwd(_sPath.GetPtr(), _sPath.GetLen()))
        {
            _sPath.SetLen(_sPath.GetLen() * 2);
            _sPath.Set(0);
        }
    } else
    {
        _sPath[i] = 0;
        i = chdir(_sPath.GetPtr());
     }
#else
    TCHAR *env = GetEnvironmentStrings();
    size_t l = 0;
    while (*env)
    {
        l += strlen(env) + 1;
        env += strlen(env) + 1;
    }
    Environ = (char *)HMALLOC(l + 1);
    memcpy(Environ, env, l + 1);
    _ModuleName.SetLen(MAX_PATH + 1);
    ::GetModuleFileName(0, _ModuleName.GetPtr(), MAX_PATH);
    m_hInstance = GetModuleHandle(0);
    _sPath = _ModuleName;
    int i = _sPath.ReverseFind(FD);
    if (i == -1)
        _sPath = "";
    else
        _sPath[i + 1] = 0;
#endif
    ModuleName = (char *) HMALLOC(_ModuleName.StrLen() + 1);
    strcpy(ModuleName, _ModuleName.GetPtr());
    if (_sPath.StrLen())
    {
        sPath = (char *) HMALLOC(_sPath.StrLen() + 1);
        strcpy(sPath, _sPath.GetPtr());
    } else
        sPath = 0;
    defIniName = ModuleName;

    m_pszAppName = strrchr(ModuleName, FD);
    if (m_pszAppName)
        ++m_pszAppName;
    else
        m_pszAppName = ModuleName;
    int i1 = defIniName.ReverseFind(FD);

    if (i1 != -1)
        defIniName = defIniName.Tail(i1 + 1);

    RegisterDll((HINSTANCE) m_hInstance, defIniName);
    defIniName.Replace(".exe", "");
    defIniName += ".rc.ini";
    TString t, CmdLine;

#ifndef _CONSOLE_PROG_
    for (int _i = 1; _i < this->argc; ++_i)
#else

    for (int _i = 1; _i < argc; ++_i)
#endif
    {
        t = argv[_i];
        CmdLine += t;
        CmdLine += " ";

    }

    m_lpCmdLine = (char *) HMALLOC(CmdLine.StrLen() + 1);
    if (!CmdLine.StrLen())
        m_lpCmdLine[0] = 0;
    else
        strcpy(m_lpCmdLine, CmdLine.GetPtr());

    /*
         TRACE("%d\r\n", ftok(fn.GetPtr(), 64560));
     TRACE("%d\r\n", ftok(fn.GetPtr(), 10));
     TRACE("%d\r\n", ftok(fn.GetPtr(), 10));= 0;
     TRACE("%d\r\n", ftok(fn.GetPtr(), 10));
     TRACE("%d\r\n", ftok(fn.GetPtr(), 10));
     TRACE("%d\r\n", ftok(fn.GetPtr(), 10));
         */
#ifdef __PLUK_LINUX__
#if(1) //ndef ANDROID_NDK
    private_th.Start(PollGLBfunc, 0, FALSE, PollGLBexit_func, 0);
     InitMsg();
#endif
#endif
	InitMsg();
	InitErrorHandles();
#ifndef _CONSOLE_PROG_
    MDC::MakeStockObjects();
 //       MDC::FreeStockObjects();

        Connect(wxEVT_IDLE, wxIdleEventHandler(MInstance::_OnIdle));
        Connect( wxEVT_TIMER, wxTimerEventHandler(MInstance::_OnTimer));
        
        mapTimer = new wxTimer(this, 123456789);
            mapTimer->Start(TIMEOUT_WX, wxTIMER_ONE_SHOT);
            return InitInstance();;
            return 1;
#endif
}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::InitInstance()
{
    JNIEnv *env = MInstance::GetLocalJNI();
    AObject obj, metrics;
    obj.Attach("android.app.AppGlobals");
    JVariable jname, japp, context, windowManager, display, info, resource, param, file, tmp;
    if (!obj.Run("getInitialPackage", 0, 0, jname))
        return 0;
    TString name = jname;
    if (!obj.Run("getInitialApplication", 0, 0, japp))
        return 0;
    if (!japp.obj)
        return JNI_ERR;


    obj.Attach(japp.obj);
    if (!obj.Run("getApplicationContext", 0, 0, context))
        return 0;
    if (!obj.Run("getResources", 0, 0, resource))
        return 0;

    obj.Attach(context.obj);

    if (!obj.Run("getApplicationInfo", 0, 0, info))
        return 0;
    param = "window"; //	public static final String WINDOW_SERVICE = "window";
    if (!obj.Run("getSystemService", &param, 1, windowManager))
        return 0;
    obj.Run("getCacheDir", 0, 0, file);
    obj.Attach(file.obj);

    if (!obj.Run("getAbsolutePath", 0, 0, param))
        return 0;
    TString b = param;
    strcpy(TmpDir, b.GetPtr());

    obj.Attach(windowManager.obj);
    if (!obj.Run("getDefaultDisplay", 0, 0, display))
        return 0;
    obj.Attach(display.obj);
    if (!metrics.New("android.util.DisplayMetrics", 0, 0, AObject::TJLOCAL))
        return 0;
    JVariable v;
    v.Attach(metrics.obj);
    obj.Run("getMetrics", &v, 1, param);
    v.Detach();
    MInstance::m_Instance->SetDisplayMetrics(env, metrics.obj);
    MInstance::m_Instance->SetWorkDir(env, 0, info.obj);
    obj.Attach(resource.obj);
    obj.Run("getAssets", 0, 0, param);
    MInstance::m_Instance->aaMng = env->NewGlobalRef(param.obj);
    obj.Attach("android.view.ViewConfiguration");

    if (obj.Run("get", &context, 1, param) && param.obj)
    {
        obj.Attach(param.obj);
        obj.Run("getScaledTouchSlop", param);
        iTouchSlop = param.value.i * 2;
    }


    return TRUE;
}

//---------------------------------------------
//
//---------------------------------------------
BOOL MInstance::MakeDoor(LPCSTR pName)
{
    if (eMSGProc)
    {
        eMSGProc->Stop();
        delete eMSGProc;
    }

    eMSGProc = new MPThread;
    char *param = 0;
    if (pName)
    {
        param  = (char *)HMALLOC(strlen(pName) + 1); 
        strcpy(param, pName); 
    }
    return eMSGProc->Start(MSGfunc, param, FALSE, MSGClenfunc, 0);
    
}
#if !defined(_CONSOLE_PROG_) || defined(ANDROID_NDK)

//---------------------------------------------
//
//---------------------------------------------
bool MInstance::LoadMSResource(LPCSTR name)
{
    resFileName = name;
#ifdef ANDROID_NDK

    CMemFile f;
    int fA = 0;
    if (ReadResourceFile(resFileName.GetPtr(), f))
    {
        int offset = 0;
        dlgs.Clear();
        resStrigs.index.Clear();
        resStrigs.str.Clear();

        while (offset < f.buff.GetLen())
        {

            ResHeader test;
            test.Read(f.buff, offset);
            DWORD k, t = test.HeaderSize + test.DataSize;
            k = (t / 4 + (t % 4 ? 1 : 0)) * 4;
            if (test.IsValid() && (int) test.Type == RT_DIALOG)
            {
                dlgs[(int) test.Name].Copy((BYTE *) f.buff.GetPtr() + offset + test.HeaderSize,
                                           test.DataSize);
                fA += test.DataSize;
            }
            offset += k;
        }
    }
    return true;
#else
    MFILE file(name);
    if (file.Open(false))
    {

        int n = file.GetSize();
        buff.SetLen(n);
        file.Read(buff.GetPtr(), n);
        file.Close();

        int offset = 0;
        while (offset < buff.GetLen())
        {
                ResHeader test;
            test.Read(buff, offset);
            DWORD k, t = test.HeaderSize + test.DataSize;
            k = (t /4 + (t % 4 ? 1 : 0))*4;
            if (test.IsValid() && (int)test.Type == RT_DIALOG)
            {
                if (1) //*(WORD *)(buff.GetPtr() + offset + test.HeaderSize + 2) != 0xFFFF)
                {
                    LPDLGTEMPLATE dlg = (LPDLGTEMPLATE)(buff.GetPtr() + offset + test.HeaderSize);
                    dlgs[(int)test.Name] = dlg;

                }
            }



            offset += k;

        }
        return true;


    }
#endif
    return false;
}

#endif
#ifndef _CONSOLE_PROG_
 DLGTEMPLATE *MInstance::LoadDialog(UINT id)
 {
     DLGTEMPLATE *res = 0;
     if (!dlgs.Lookup(id, res)) return 0;
     return res;

 }
extern BOOL IsWindow2(GHWND hWnd);
//-------------------------------------
//
//-------------------------------------
LRESULT MInstance::OnMessage(TMSG &msg)
{
 //  this->ProcessPendingEvents();
    if (msg.sig.si_signo == SIG_SENDMESSAGE_WND || msg.sig.si_signo == SIG_POSTMESSAGE_WND)
    {
       if (!fBlocking &&  IsWindow2(msg.ghWnd))
       {


           MWnd *ptr = MWnd::FromHandle(msg.ghWnd);
           if (ptr)
               return ptr->MessageProc(msg);
        }
       else
       {
           if (msg.fIsSend)
           ReplyMessage(0);
           return true;
       }
   }
    return MSGThread::OnMessage(msg);
}
void MInstance::_OnTimer(wxTimerEvent & ev)
{
    ev.Skip();
    if (0) //!fOnInitInstance)
    {
        fOnInitInstance = true; 
        InitInstance();
    
        
    }
    //ePollGLB->SendMessage(WM_USER + MSG_SENDTEST, 0, 0, 0);
    if (!bIdleEvent)
    {
        wxIdleEvent event;
        event.SetEventObject(this);
        AddPendingEvent(event);
    }
  //  MPThread::Idle();
       
    
}
bool MInstance::ProcessIdle()
{
    return !MInstance::fBlocking && wxApp::ProcessIdle();
}
int GetNumMessages();

//-------------------------------------
//
//-------------------------------------
void MInstance::_OnIdle(wxIdleEvent& event)
{
  //if (mapTimer->IsRunning())
    //mapTimer->Stop();
      event.Skip();
     
     TMSG msg;
      if (!bIdleEvent) 
      {
       	
            bIdleEvent = TRUE;
            int N = GetNumMessages();
            for (int i = 0; i < N && !fBlocking && PeekMessage(&msg, TRUE); ++i)

             //   while (!fBlocking && PeekMessage(&msg, TRUE))
            {
               #ifdef __PLUK_DLL__
                       Processor *_Pluk = GetOwner();
                   if (_Pluk)
                   {
                       if (_Pluk->GetEndFlagStatus())
                               break;
                       _Pluk->SetRequestToInterrupt();
                   }
               #endif

                           TranslateMessage(&msg);
                          DispatchMessage(&msg);

               #ifdef __PLUK_DLL__
                       _Pluk = GetOwner();
                   if (_Pluk)
                   {
                       if (_Pluk->GetEndFlagStatus())
                               break;
                       _Pluk->SetRequestToInterrupt();
                   }
               #endif

           }
        bIdleEvent = FALSE;
      }
    
  

     mapTimer->Start(500, wxTIMER_ONE_SHOT );
  }

//-------------------------------------
//
//-------------------------------------
int MInstance::OnExit()
{
    ExitInstance();

    MDC::FreeStockObjects();

        defAssertHandle =  wxSetAssertHandler(PAssertHandler);


#else

//---------------------------------------------
//
//---------------------------------------------
MInstance::~MInstance()
{
#endif

#ifdef __PLUK_LINUX__
    defIniName = ModuleName;
    int i1 = defIniName.ReverseFind(FD);
    if (i1 != -1)
        defIniName = defIniName.Tail(i1 + 1);

    private_th.Stop();
#ifndef ANDROID_NDK
    ProcMsg.Close();
#endif
#endif
    if (sPath) HFREE(sPath);
    if (ModuleName) HFREE(ModuleName);
    if (m_lpCmdLine) HFREE(m_lpCmdLine);
    defIniName.Clear();
    TlsFree(MPThread::dwStartupPtr);
    TlsFree(MPThread::dwMessageWaitBusy);
    TlsFree(MPThread::dwWaitForMultipleObjects);
#ifndef _CONSOLE_PROG_
    TlsFree(MDC::dwDCStockObjects);

    UnRegisterDll((HINSTANCE)m_hInstance, defIniName);
    return wxApp::OnExit();;
#endif

}

#ifndef _CONSOLE_PROG_

//-------------------------------------
//
//-------------------------------------
bool MInstance::Yield(bool f)
{
    bool fRet = 0;
static int i = 0;
    if (!i)
    {
        ++i;
       fRet = wxApp::Yield(f);
       i = 0;
    }
return fRet;
}
//-------------------------------------
//
//-------------------------------------
void MInstance::WakeUpIdle()
{
    wxApp::WakeUpIdle();
}
//-------------------------------------
//
//-------------------------------------
;
void MInstance::GuiIdle() 
{ 
#if (1)
    
    if (GetCurrentThreadId() == main_thread.hThread)
    {
        static int b = false;
        if (bIdleEvent) return;
        if (b)
        {
         //   this->ProcessPendingEvents ();
            return;
        }
        b = true;
    
     //   wxYield();
       //  wxIdleEvent event;
       // event.SetEventObject(this);
       // AddPendingEvent(event);
        wxWakeUpIdle(); 
         wxYield();
        wxWakeUpIdle(); 
          //     wxYield();
        b = false;
    }
   
    #else

 
    if (GetCurrentThreadId() == main_thread.hThread)
    {
        static int b = false;
        if (bIdleEvent) return;
        if (b)
        {
         //   this->ProcessPendingEvents ();
            return;
        }
        b = true;
        
        wxYield();
        
     //   wxWakeUpIdle();
    //    this->ProcessPendingEvents ();
        b = false;
        //DoIdle();

        return;
    }
    else
    {
 /*      if (cIdle.Check() || !cIdle.TryEnter())
        ++iCheckThreadEvents; 
  */    
       //  if (cIdle.TryEnter())
         {
 
       // wxWakeUpIdle(); 
        //cIdle.Leave();
         }
    //    cIdle.Enter();
     //   ++iCheckThreadEvents; 
     //   cIdle.Leave();
        
    }
    
    
   #endif     
}

//-------------------------------------
//
//-------------------------------------
MCursor *MInstance::LoadCursor(int i, LPCSTR ini)
{
    TString ind;
    ind.Format("%d", i);
    return LoadCursor(ind.GetPtr(), ini);

}
//-------------------------------------
//
//-------------------------------------
MIcon *MInstance::LoadIcon(int i, LPCSTR ini)
{
    TString ind;
    ind.Format("%d", i);
    return LoadIcon(ind.GetPtr(), ini);
}

int MInstance::FilterEvent(wxEvent& event)
{
    //    if (((LRESULT)TlsGetValue(MPThread::dwWaitForMultipleObjects))) 
   //         return 0;

    return  -1;
}

//-------------------------------------
//
//-------------------------------------
MCursor *MInstance::LoadCursor(LPCSTR _str, LPCSTR ini)
{
     TString str;
    MCursor *ico = 0;
    TString name = ini;
    if (!ini)
    {
        name = (*dlls)[hResHandle];
        int i = name.ReverseFind('.');
        if (i != -1)
            name[i] = 0;
        name += ".rc.ini";
    }
    FFileINIW wini(name.GetPtr(), TRUE);
    if (!wini.IsValid())
    {
        wini.Close();
        FFileINI tini(name.GetPtr(), TRUE);
        str = tini.GetString("RC.CURSOR", _str);
    }
    else
    {
        WString ind;
        ind = _str;
        str = wini.GetString("RC.CURSOR", ind);
    }
    if (!str.GetLen())
    {
        TRACE("not load cursor %s > %s\n", _str, name.GetPtr());
        return 0;
    }

    ico = new MCursor();
    TString ph(sPath);
    ph += SFD;
    ph += str;
    str = ph;
    ico->Load(str.GetPtr());
    if (!ico->IsHandle())
    {
        TRACE("not load cursor %s > %s\n", _str, name.GetPtr());
       delete ico;
        ico = 0;
    }
    return ico;


}
//-------------------------------------
//
//-------------------------------------
MIcon *MInstance::LoadIcon(LPCSTR _str, LPCSTR ini)
{
   TString str;
    MIcon *ico = 0;
    TString name = ini;
    if (!ini)
    {
        name = (*dlls)[hResHandle];
        int i = name.ReverseFind('.');
        if (i != -1)
            name[i] = 0;
        name += ".rc.ini";
    }
    FFileINIW wini(name.GetPtr(), TRUE);
    if (!wini.IsValid())
    {
        wini.Close();
        FFileINI tini(name.GetPtr(), TRUE);
        str = tini.GetString("RC.ICON", _str);
    }
    else
    {
        WString ind;
        ind = _str;
        str = wini.GetString("RC.ICON", ind);
    }
    if (!str.GetLen())
    {
        TRACE("not load icon %s > %s\n", _str, name.GetPtr());
        return 0;
    }
    ico = new MIcon();
    TString ph(sPath);
    ph += SFD;
    ph += str;
    str = ph;
    ico->Load(str.GetPtr());
    if (!ico->IsHandle())
    {
        TRACE("not load icon %s > %s\n", _str, name.GetPtr());
        delete ico;
        ico = 0;
    }
    return ico;

}


#endif
#if !defined(_CONSOLE_PROG_) || defined(ANDROID_NDK)

//-------------------------------------
//
//-------------------------------------
ABmp *MInstance::_LoadBitmap(int i, ABmp *res, LPCSTR ini)
{
#ifdef ANDROID_NDK
    CMemFile f;
    ABmp *bb;
    if (ReadResourceFile(resFileName.GetPtr(), f))
    {
        int offset = 0;
        while (offset < f.buff.GetLen())
        {

            ResHeader test;
            test.Read(f.buff, offset);
            DWORD k, t = test.HeaderSize + test.DataSize;
            k = (t / 4 + (t % 4 ? 1 : 0)) * 4;
            if (test.IsValid() && (int) test.Type == (int) RT_BITMAP && test.Name.IsNumber() &&
                i == (int) test.Name)
            {
                bb = res ? res : new ABmp;
                bb->LoadDib((char *) (f.buff.GetPtr() + offset + test.HeaderSize));
                return bb;
            }
            offset += k;
        }
    }
#endif
    TString ind;
    ind.Format("%d", i);
    return _LoadBitmap(ind.GetPtr(), res, ini);

}
//---------------------------------------------
//
//---------------------------------------------

MBmp *MInstance::LoadBitmap(int i, LPCSTR ini)
{
    return _LoadBitmap(i, 0, ini);

}

//-------------------------------------
//
//-------------------------------------
MString MInstance::LoadString(int i, LPCSTR ini)
{
#ifdef ANDROID_NDK
    CMemFile f;
    //USHORT index = (((USHORT)i) >> 4) + 1;
//	USHORT offI = i & 0xf;
    int HI, LO;
    TString res;

    if (!resStrigs.str.GetLen() && ReadResourceFile(resFileName.GetPtr(), f))
    {
        int offset = 0;

        while (offset < f.buff.GetLen())
        {
            ResHeader test;
            test.Read(f.buff, offset);
            DWORD k, t = test.HeaderSize + test.DataSize;
            k = (t / 4 + (t % 4 ? 1 : 0)) * 4;
            if ((int) test.Type == (int) RT_STRING) // && (USHORT)test.Name == index)
            {
                int index = (((USHORT) test.Name - 1) << 4);
                BYTE *ptr = (BYTE *) f.buff.GetPtr() + offset + test.HeaderSize;
                int kk = 0;
                while (test.DataSize)
                {

                    short nn = *(SHORT *) ptr;
                    ptr += 2;
                    test.DataSize -= 2;
                    if (nn)
                    {
                        int iID = index | kk;
                        WString ss(nn);
                        USHORT *p = (USHORT *) ptr;
                        for (int j = 0; j < nn; ++j)
                            ss[j] = (wchar_t) p[j];
                        ss.Add() = 0;
                        TString res;
                        res = ss;
                        resStrigs.index[iID] = resStrigs.str.GetLen();
                        resStrigs.str.Append(res.GetPtr(), res.StrLen() + 1);

                        /*	if (kk == offI)
                            {
                                WString ss(nn);
                                USHORT *p = (USHORT *)ptr;
                                for (int i = 0; i < nn; ++i)
                                    ss[i] = (wchar_t)p[i];
                                ss.Add() = 0;
                                TString res;
                                res = ss;
                                return res;
                            }
                            */
                        ptr += nn * 2;
                        test.DataSize -= nn * 2;

                    }
                    ++kk;
                    if ((int) test.DataSize < 0)
                        break;
                }

            } else
#ifdef USE_MENU_STRING
            if ((int) test.Type == (int) RT_MENU)
            {
                HI = (int) test.Name;
                USHORT *pw = (USHORT *) (f.buff.GetPtr() + offset + test.HeaderSize);
                USHORT *pw_end = pw + test.DataSize / sizeof(USHORT);
                pw += 2;
                BOOL fFirst = true;
                USHORT fItemFlags;
                int N = 0;
                while (pw < pw_end)
                    do
                    {
                        WString ss;
                        LO = 0;
                        fItemFlags = *pw++;
                        if (!(fItemFlags & MFR_POPUP))
                            LO = *pw++;
                        if ((fItemFlags & MFR_POPUP) || LO)
                        {
                            USHORT *p = (USHORT *) pw;
                            for (int j = 0; *pw; ++j)
                                ss.Add() = (wchar_t) *pw++;
                            ss.Add() = 0;
                        }
                        ++pw;
                        if (ss.StrLen())
                        {
                            int iID = 0;
                            TString res;
                            res = ss;
                            if (!LO)
                                LO = N++;
                            iID = MAKELONG(LO, HI);
                            if (iID)
                            {
                                resStrigs.index[iID] = resStrigs.str.GetLen();
                                resStrigs.str.Append(res.GetPtr(), res.StrLen() + 1);
                            }
                        }

                    } while ((fItemFlags & MFR_END) == MFR_END);

            }
#endif

            offset += k;
        }
    }
    if (resStrigs.str.GetLen())
    {
        int *index = 0;
        if (resStrigs.index.Lookup(i, &index))
        {
            return resStrigs.str.GetPtr() + *index;
        }
    }
#endif
    TString ind;
    ind.Format("%d", i);
    return LoadString(ind.GetPtr(), ini);

}

//-------------------------------------
//
//-------------------------------------
MBmp *MInstance::_LoadBitmap(LPCSTR _str, MBmp *res, LPCSTR ini)
{
    ABmp *ico = 0;
#ifdef ANDROID_NDK
    CMemFile f;
    if (ReadResourceFile(resFileName.GetPtr(), f))
    {
        int offset = 0;
        while (offset < f.buff.GetLen())
        {

            ResHeader test;
            test.Read(f.buff, offset);
            DWORD k, t = test.HeaderSize + test.DataSize;
            k = (t / 4 + (t % 4 ? 1 : 0)) * 4;
            if (test.IsValid() && (int) test.Type == (int) RT_BITMAP && test.Name.IsString())
            {
                TString ts = test.Name;
                if (ts == _str)
                {
                    ico = res ? res : new ABmp;
                    ico->LoadDib((char *) (f.buff.GetPtr() + offset + test.HeaderSize));
                    return ico;
                }
            }
            offset += k;
        }
    }
#endif

    TString str;
    TString name = ini;
    if (!ini)
    {
        name = (*dlls)[hResHandle];
        int i = name.ReverseFind('.');
        if (i != -1)
            name[i] = 0;
        name += ".rc.ini";
    }
    FFileINIW wini(name.GetPtr(), TRUE);
    if (!wini.IsValid())
    {
        wini.Close();
        FFileINI tini(name.GetPtr(), TRUE);
        str = tini.GetString("RC.BITMAP", _str);
    } else
    {
        WString ind;
        ind = _str;
        str = wini.GetString("RC.BITMAP", ind);
    }
    if (!str.GetLen())
    {
        TRACE("not load cursor %s > %s\n", _str, name.GetPtr());
        return 0;
    }
    ico = res ? res : new MBmp();
    TString ph(sPath);
    ph += SFD;
    ph += str;
    str = ph;
    ico->Load(str.GetPtr());
    if (!ico->IsHandle())
    {
        TRACE("not load cursor %s > %s\n", _str, name.GetPtr());
        if (!res)
            delete ico;
        ico = 0;
    }
    return ico;

}


//-------------------------------------
//
//-------------------------------------
MBmp *MInstance::LoadBitmap(LPCSTR _str, LPCSTR ini)
{

    return _LoadBitmap(_str, 0, ini);
}

//-------------------------------------
//
//-------------------------------------
MString MInstance::LoadString(LPCSTR str, LPCSTR ini)
{
    TString name = ini;
    MString res;
    if (!ini)
    {
        name = (*dlls)[hResHandle];
        int i = name.ReverseFind('.');
        if (i != -1)
            name[i] = 0;
        name += ".rc.ini";
    }
    FFileINIW wini(name.GetPtr(), TRUE);
    if (!wini.IsValid())
    {
        wini.Close();
        FFileINI tini(name.GetPtr(), TRUE);
        res = tini.GetString("RC.STRING", str);
    } else
    {
        WString ind;
        ind = str;
        res = wini.GetString(L"RC.STRING", ind);
    }
    return res;

}
EXPORTNIX int AfxMessageBox(LPCSTR lpszText, UINT nType, UINT nIDHelp)
{
	return IDYES;
}
#endif
