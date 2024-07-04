#ifndef _MINSTANCE__
#define _MINSTANCE__
#include "mtree.h"
#ifdef ANDROID_NDK
#include "AWnd.h"
#include "ADC.h"
#endif
#include "critical.h"
#include "msgthread.h"
#include "uxmessage.h"
#include "uxsignal.h"
#include "ArHashMap.h"
#include "Java/JVariable.h"
#include "Java/CreateClassUtil.h"
#include "Java/AClass.h"

//******************************************************
//******************************************************
#ifndef _CONSOLE_PROG_
#include "wx_inc.h"
#endif

#ifndef _CONSOLE_PROG_
class  MCursor;
class  MIcon;
class  MBmp;
struct DLGTEMPLATE;
EXPORTNIX class _PEXPORT MInstance :public wxApp, public MSGThread {
    public :
	MInstance():wxApp(), MSGThread(0) {}
	~MInstance() {}
	virtual bool OnInit();
	virtual int OnExit();
    virtual int FilterEvent(wxEvent& event);
    void _OnIdle(wxIdleEvent &event);
  	void _OnTimer(wxTimerEvent & ev);
        virtual LRESULT OnMessage(TMSG &msg);
    virtual BOOL InitInstance() { return 1; }
    virtual int ExitInstance() { return 0; }
    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual bool ProcessIdle();

    virtual void WakeUpIdle();
    virtual BOOL PreTranslateMessage(GMSG* pMsg) { return FALSE; }
    DLGTEMPLATE *LoadDialog(UINT id);
 private :
    MArray<char> buff;
    MHashMap<DLGTEMPLATE *, int> dlgs;
#else
#ifdef ANDROID_NDK
struct ABitmapCreator;
#endif
#pragma pack(push)
#pragma pack(8)

struct InitURLLoader {
	JPtr urlLoader;
	jmethodID loadClass;
	JNIEnv *env;
	BOOL fValid;
	InitURLLoader(MVector<TString> &jar, JNIEnv *_env);
	~InitURLLoader();
};

EXPORTNIX class _PEXPORT MInstance : public MSGThread {
public:
	MInstance(char **argv=0, int argc=0);
	virtual ~MInstance();
#endif
public:
    char *GetName() { return ModuleName; }

#ifndef _CONSOLE_PROG_
    void OnIdle(int) {GuiIdle(); }
#else
    void OnIdle(int i) { Sleep(i); }
#endif
    struct ResString {
    	ArHashMap<int, DWORD> index;
    	MArray<char> str;
    	ResString() {}
    	~ResString() {}

    };
    ResString resStrigs;

	HINSTANCE m_hInstance;
    char *Environ;
    char *sPath;
    char *ModuleName;
    char *m_pszAppName;
    MPThread main_thread;
	TString resFileName;
    CriticalSection interlock;
#ifdef ANDROID_NDK
public:
	MHashMap<MArray<BYTE>, int> dlgs;

	virtual BOOL InitInstance();
    BOOL ReadResourceFile(const char *filename, CMemFile &res);
    BOOL ReadResourceDir(LPCSTR path, LPCSTR mask, MVector<TString> &dir,   MVector<TString> &file);
    BOOL ExtractResourceFile(LPCSTR src, LPCSTR dst);
    BOOL ExtractResourceDir(LPCSTR src, LPCSTR dst);

    BOOL ResourceFile2TempFile(LPCSTR path_src, CMemFile &res);
    BOOL IsResourceFile(const char *filename);
    virtual ABmp *LoadBitmap(int, LPCSTR ini = 0);
    virtual ABmp *_LoadBitmap(int, ABmp *res, LPCSTR ini = 0);

    virtual MString LoadString(int, LPCSTR ini = 0);
    virtual ABmp *LoadBitmap(LPCSTR, LPCSTR ini = 0);
    virtual ABmp *_LoadBitmap(LPCSTR, ABmp *res, LPCSTR ini = 0);

    virtual MString LoadString(LPCSTR, LPCSTR ini = 0);

public:
	virtual void AddToLog(LPCSTR t) {}
	static JNIEnv* GetLocalJNI();
	static BOOL InitJNI(JNIEnv *);
	static BOOL InitSpec(const char *sp, bool fInitAll = false);
	static jclass FindClass(const char *sp, BOOL fJar = false);
	static TString GetNameObject(JNIEnv *pJniEnv, jobject cls);
	static TString GetNameJClass(JNIEnv *pJniEnv, jclass cls);
	static int GetChildren(JNIEnv *pJniEnv, jobject wnd, RBMap<TString, jobject> &childs);
	static jmethodID GetMethodID(JNIEnv *pJniEnv, jclass  clazz, const char* name, const char* sig);
	static jfieldID GetFieldID(JNIEnv *pJniEnv, jclass  clazz, const char* name, const char* sig);
	static InitURLLoader *loader;
//	static CreateClassUtil *utils;
	static BOOL ClearException(JNIEnv *, BOOL fTrace = true);
	static void DetachThreadJNI();

	jobject CreateAndroidBitmap(int w, int h, AndroidBitmapInfo *info = 0);
	static void ReleaseJNI();

	static bool LookupAClass(LPCSTR name);
	static void AddAClass(LPCSTR name, AClass *cls);
	static void DeleteAClass(LPCSTR name);
	static AClass *GetAClass(LPCSTR name);
protected:
	void SetWorkDir(JNIEnv *pJniEnv,  jclass  , jobject obj);
	void SetDisplayMetrics(JNIEnv *pJniEnv,  jobject obj);
	static void ThrowTrace(JNIEnv *env, TString &msg, jthrowable except);
	static CriticalSection *clazz_section;
	static MHashMap<AClass *, TString> *clazz;
public:
	struct METRICS{
		int widthPixels;
		int heightPixels;
		float density;
		int densityDpi;
		float xdpi;
		float ydpi;
		float scaledDensity;

	};
protected :
	METRICS metrics;
public:
	virtual METRICS *GetMetrics();
	jobject aaMng;
	TString srl;
	static int iTouchSlop;
	static float fTouchSlop;
private:
	static BOOL InitSpec(const char *sp, MHashSet<TString> *addTypes, bool fInitAll = false);
	static jobject CreateJObject(const char *sp);

    static ABitmapCreator *bmp_instance;
    static JavaVM *m_JavaVM;
    static jclass throwable;
    static jmethodID throwable_getCause;
    static jmethodID throwable_getStackTrace;
    static jmethodID throwable_toString;
    static jclass stacktrace;
    static jmethodID stacktrace_toString;

public:
#endif
#ifdef __PLUK_LINUX__
        pid_t hParent;
#ifndef ANDROID_NDK
        UXMessage ProcMsg;
#endif
        HINSTANCE hResHandle;
#ifndef _CONSOLE_PROG_
    void GuiIdle();
    virtual MCursor *LoadCursor(int, LPCSTR ini = 0);
    virtual MIcon *LoadIcon(int, LPCSTR ini = 0);
    virtual MBmp *LoadBitmap(int, LPCSTR ini = 0);
    virtual MString LoadString(int, LPCSTR ini = 0);
    virtual MCursor *LoadCursor(LPCSTR, LPCSTR ini = 0);
    virtual MIcon *LoadIcon(LPCSTR, LPCSTR ini = 0);
    virtual MBmp *LoadBitmap(LPCSTR, LPCSTR ini = 0);
    virtual MString LoadString(LPCSTR, LPCSTR ini = 0);
    virtual MBmp *_LoadBitmap(int, MBmp *res, LPCSTR ini = 0);
    virtual MBmp *_LoadBitmap(LPCSTR, MBmp *res, LPCSTR ini = 0);

    ProxyWindowHandler *m_pMainWnd;

#else
#ifndef ANDROID_NDK
    MSGThread *m_pMainWnd;
#else
    GHWND m_pMainWnd;
#endif
  
#endif
#endif
      BOOL MakeDoor(LPCSTR pName); 
#if !defined(_CONSOLE_PROG_) || defined(ANDROID_NDK)

      bool LoadMSResource(LPCSTR path);
#endif
    typedef void (*_tMain)(char **, int);
    static _tMain MainApp;
    static int iCheckThreadEvents;
    static MInstance *m_Instance;
     static int fBlocking;
   char *m_lpCmdLine;

   static RBHashMap <TString, void *> *dlls;
#ifndef ANDROID_NDK
   CriticalSection cIdle;
#endif
    private:
    TString defIniName;
    MPThread private_th;
    public:
 #ifndef _CONSOLE_PROG_
    DECLARE_DYNAMIC_CLASS(MInstance)
    DECLARE_NO_COPY_CLASS(MInstance)
#endif 
#ifdef ANDROID_NDK
	friend class JSpec;
//	friend class AObject;
	friend class AClass;
	friend struct AMember;
#endif
};
#pragma pack(pop)

inline MInstance *MGetApp() { return MInstance::m_Instance; }
inline pthread_t AfxGetMainThreadId() {return MGetApp()->main_thread.hThread; }

typedef MInstance CWinApp;
#ifdef __PLUK_LINUX__
#if defined(_CONSOLE_PROG_) && !defined(ANDROID_NDK)
#define AfxGetApp MGetApp
inline MSGThread *AfxGetMainWnd() {return MGetApp()->m_pMainWnd; }
inline MSGThread *AfxGetMainThread() {return MGetApp()->m_pMainWnd; }
#else
inline MSGThread *AfxGetMainThread() {return (MSGThread *)MInstance::m_Instance; }
#ifndef ANDROID_NDK
inline void Idle() {MGetApp()->GuiIdle(); }
inline ProxyWindowHandler *AfxGetMainWnd() {return MGetApp()->m_pMainWnd; }
#else
inline GHWND AfxGetMainWnd() {return MGetApp()->m_pMainWnd; }
#endif
#define AfxGetApp MGetApp
#ifdef ANDROID_NDK
inline const string LOADSTRING(int a) { TString g = MGetApp()->LoadString(a); return g.GetPtr() ? g.GetPtr() : ""; }
#endif
EXPORTNIX void AfxSetResourceHandle (HINSTANCE h);
EXPORTNIX HINSTANCE AfxGetResourceHandle (LPCSTR dllname = 0);
#ifndef _CONSOLE_PROG_
#define ABmp MBmp
#endif
#if !defined(_CONSOLE_PROG_) || defined(ANDROID_NDK)
inline ABmp *LoadBitmapA(HINSTANCE hInstance, LPCSTR lpBitmapName)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	AfxSetResourceHandle(hInstance);
	ABmp *bmp = MGetApp()->LoadBitmap(lpBitmapName);
	AfxSetResourceHandle(hInst);
	return bmp;
}
#endif

#ifndef _CONSOLE_PROG_
inline MIcon *LoadIconA(HINSTANCE hInstance, LPCSTR lpBitmapName)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	AfxSetResourceHandle(hInstance);
	MIcon *bmp = MGetApp()->LoadIcon(lpBitmapName);
	AfxSetResourceHandle(hInst);
	return bmp;
}
inline MCursor *LoadCursorA(HINSTANCE hInstance, LPCSTR lpBitmapName)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	AfxSetResourceHandle(hInstance);
	MCursor *bmp = MGetApp()->LoadCursor(lpBitmapName);
	AfxSetResourceHandle(hInst);
	return bmp;
}
#endif


#endif
#define AfxGetInstanceHandle AfxGetResourceHandle
EXPORTNIX int GetModuleFileName(HINSTANCE h, LPSTR, int);

#ifndef GUID
#pragma pack(push) //NADA
#pragma pack(2)

struct tagGUID {
    UINT Data1;
    USHORT Data2;
    USHORT Data3;
    BYTE Data4[8];
};
#pragma pack(pop)
typedef tagGUID GUID;
#endif
extern "C" void CoCreateGuid(GUID *);
extern "C" BOOL IsEqualGUID(GUID &g1, GUID &g2);
inline void InterlockedDecrement(int *p) { if (MInstance::m_Instance) { MInstance::m_Instance->interlock.Enter(); --*p; MGetApp()->interlock.Leave(); } }
inline void InterlockedIncrement(int *p) { if (MInstance::m_Instance) { MInstance::m_Instance->interlock.Enter(); ++*p; MGetApp()->interlock.Leave(); } }
#endif
#endif
