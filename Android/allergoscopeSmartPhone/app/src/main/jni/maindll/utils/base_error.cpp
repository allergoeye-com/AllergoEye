/*
 * base_error.cpp
 *
 *  Created on: Mar 24, 2012
 *      Author: alex
 */
#include "stdafx.h"
#include "base_error.h"
#include "setjmp.h"
#ifndef ANDROID_NDK
#include "execinfo.h"
#else
/*
#include <iostream>
#include <vector>
#include <algorithm>
#undef _Unwind_Ptr

#include <unwind.h>
typedef struct {
    size_t count;
    void** addrs;
} stack_crawl_state_t;

static _Unwind_Reason_Code trace_function(_Unwind_Context *context, void *arg)
{
    stack_crawl_state_t* state = (stack_crawl_state_t*)arg;
    if (state->count) {
        void* ip = (void*)_Unwind_GetIP(context);

        if (ip) {
            state->addrs[0] = ip;
            state->addrs++;
            state->count--;
            return _URC_NO_REASON;
        }
    }

    return _URC_END_OF_STACK;
}
int backtrace(void** addrs, size_t size)
{
	stack_crawl_state_t state;
	state.count = size;
	state.addrs = addrs;
	_Unwind_Backtrace(trace_function, (void*)&state);
	return size - state.count;
}
*/
extern int backtrace(void** addrs, size_t size);


//char **backtrace_symbols (void *__const *__array, int __size) { return 0; }
#endif
#include <cxxabi.h>
#ifdef ANDROID_NDK
#include <stdlib.h>


DINT MultiByteToWideChar(int ignore, int ignore1, const char *src, DINT srclength, WCHAR *dst, DINT dstlength)
{
	JNIEnv*env;
	if (MGetApp() && (env = MInstance::GetLocalJNI()))
	{

		DINT l = 0;

		JPtr jstr = env->NewStringUTF(src);

	    jsize len = env->GetStringLength(jstr);
	    l = len;
            const jchar *raw = env->GetStringChars(jstr, 0);
	    const jchar *temp = raw;
	    while (len--)
	        *dst++ = *(temp++);
	    env->ReleaseStringChars(jstr, raw);
	return l;
	}
    if (srclength <= 0) srclength = strlen(src) + 1;
    DINT ln = 0, cp = min(srclength, dstlength);
    DINT l = 0;
    while (cp)
    {

        ln = mbstowcs (dst, src, cp);
        if (ln < 0) return 0;
        if (!dst[ln - 1] && src[ln - 1])
        	--ln;

        dst += ln;
        src += ln;
        cp -= ln;
        l += ln;
    }
    return l;


}
DINT WideCharToMultiByte(const WCHAR *src, MArray<char> &res)
{
    JNIEnv*env;
    if (MGetApp() && (env = MInstance::GetLocalJNI()))
    {
        DINT i, l = wcslen(src);
        MArray<jchar> bb(l + 1);
        jchar *b = bb.GetPtr();
        for (int i = 0; i < l; ++i)
            b[i] = (jchar)src[i];
        b[l] = 0;
        JPtr jstr = env->NewString(b, l);
        const char *raw = env->GetStringUTFChars(jstr, NULL);
        int len = env->GetStringUTFLength(jstr);
//		if (dstlength < len) return 0;
        l = len;
        res.SetLen(len + 1);
        strcpy(res.GetPtr(), raw);
        env->ReleaseStringUTFChars(jstr, raw);
        return l;
    }
    int srclength = wcslen(src)  + 1;

    res.SetLen(srclength + 1);
    char *dst = res.GetPtr();
    DINT ln = 0, cp = srclength;
    DINT l = 0;
    while (cp)
    {

        ln = wcstombs (dst, src, cp);
        if (ln < 0) return 0;
        if (!dst[ln - 1] && src[ln - 1])
            --ln;
        if (!ln)
            break;
        dst += ln;
        src += ln;
        cp -= ln;
        l += ln;
    }
    return l;

}

DINT WideCharToMultiByte(int ignore, int ignore1, const WCHAR *src, DINT srclength, char *dst, DINT dstlength, const char *ignore2, int *ignore3)
{
	JNIEnv*env;
	if (MGetApp() && (env = MInstance::GetLocalJNI()))
	{
		DINT i, l = wcslen(src);
        MArray<jchar> bb(l + 1);
		jchar *b = bb.GetPtr();
		for (int i = 0; i < l; ++i)
			b[i] = (jchar)src[i];
        b[l] = 0;
		JPtr jstr = env->NewString(b, l);
		const char *raw = env->GetStringUTFChars(jstr, NULL);
		int len = env->GetStringUTFLength(jstr);
//		if (dstlength < len) return 0;
		l = len;
		const char *temp = raw;
		while (len--)
			*dst++ = *(temp++);
        *dst = 0;
		env->ReleaseStringUTFChars(jstr, raw);
		return l;
	}
	if (srclength <= 0) srclength = wcslen(src)  + 1;

	DINT ln = 0, cp = min(srclength, dstlength);
	DINT l = 0;
    while (cp)
    {

        ln = wcstombs (dst, src, cp);
        if (ln < 0) return 0;
        if (!dst[ln - 1] && src[ln - 1])
        	--ln;
        if (!ln)
            break;
        dst += ln;
        src += ln;
        cp -= ln;
        l += ln;
    }
    return l;


}
#endif
static BOOL fIsTestPtr = FALSE;
static struct sigaction prevsig;
static sigset_t oldset;
static jmp_buf mJump;
static CriticalSection crit;


void DebugOutput(LPSTR err)
{
#ifdef __PLUKDLL__ 
    Processor *Pluk = GetOwner();
    if (Pluk)
        Pluk->ErrorOutput(err);
    else
    fprintf(stderr, "%s", err);
        
#else
#ifdef ANDROID_NDK
    TRACE("%s", err);
    //AfxGetApp()->AddToLog(err);

#else
    fprintf(stderr, "%s", err);
#endif
#endif
}
void DebugOutput(const char *pFormat, ...)
{
    char tmp[1024];
    if (!pFormat || !pFormat[0]) return;
	va_list args;
	va_start(args, pFormat);
	vsprintf(tmp, pFormat, args);
	va_end(args);
	int n;
	if ((n = (int)strlen(tmp) ))
    {
		tmp[n] = '\n';
 		tmp[n + 1] = 0;
        DebugOutput(tmp);
    }
}

void DataOutput(LPSTR err)
{
    
    int f = -1;
    if (AfxGetMainThreadId() == GetCurrentThreadId())
    {    
        f = AfxGetApp()->fBlocking;
        AfxGetApp()->fBlocking = 1;
    }    
#ifdef __PLUKDLL__ 
    Processor *Pluk = GetOwner();
    if (Pluk)
        Pluk->ErrorOutput(err);
    else
    fprintf(stderr, "%s", err);
        
#else
#ifdef ANDROID_NDK
    TRACE("%s", err);
#else
    fprintf(stderr, "%s", err);
#endif
#endif
    if (f != -1)
    AfxGetApp()->fBlocking = f;
}

//-------------------------------------
//
//-------------------------------------

 void TestHandler(int, siginfo_t *sig, void*) 
 {
    fIsTestPtr = FALSE;
    longjmp(mJump, 1);

}
 
//-------------------------------------
//
//-------------------------------------

EXPORTNIX BOOL IsBadReadPtr(void* lp, UINT cb) 
{
    return 0;
#ifndef NDEBUG
    crit.Enter();
    BOOL fRet = FALSE;
    fIsTestPtr = TRUE;
    if (setjmp(mJump)) 
    {

        goto _exit_2;
    }
    if (!fIsTestPtr)
    {
    	goto _exit_2;
    }
    struct sigaction act;
    memset(&act, 0, sizeof (act));

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGSEGV);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = TestHandler;
    if (StdError(sigaction(SIGSEGV, &act, &prevsig)) != 0)
    {
        goto _exit_2;
    }
    StdError(sigprocmask(SIG_UNBLOCK, &act.sa_mask, &oldset));

    for (int b, i = 0; fIsTestPtr && i < cb; ++i)
        b = ((BYTE*) lp)[i];
    fRet = fIsTestPtr == 0;
	sigaction(SIGSEGV, &prevsig, 0);
    sigprocmask(SIG_BLOCK, &oldset, 0);
_exit_1:
_exit_2:
    fIsTestPtr = FALSE;
    crit.Leave();

    return fRet;
#else
    return false;
#endif

}
void PrintStack();
EXPORTNIX void PrintStackToBuffer(TString &b, int N);

//-------------------------------------
//
//-------------------------------------
static void *stack[100];
static char n[1024];
static char addr2line_cmd[512];
#ifdef ANDROID_NDK
//#include <asm-generic/mutex-xchg.h>
#endif
void DampHandler(int s, siginfo_t *sig, void*) 
 {
	int i = 0;


    switch(s)
    {
    case SIGABRT:
        DebugOutput("signal %d(SIGABRT)\r\n", s);

        break;
    case SIGSTOP:
        DebugOutput("signal %d(SIGSTOP)\r\n", s);
        break;

    case SIGQUIT:
        DebugOutput("signal %d(SIG GUIT)\r\n", s);
         break;
        case SIGSEGV:
            DebugOutput("signal %d(Segmentation violation)\r\n", s);
            throw std::runtime_error("Segmentation violation");
            return;
          break;
#ifdef  SIGSTKFLT
        case  SIGSTKFLT:
            DebugOutput("signal %d(Stack fault)\r\n", s);
            break;
#endif
        case SIGILL:
            DebugOutput("signal %d(Illegal instruction)\r\n", s);
            break;
        case SIGFPE:
            DebugOutput("signal %d(Floating-point exception)\r\n", s);
            break;
        case SIGPIPE:
            DebugOutput("signal %d(Broken pipe)\r\n", s);
            break;
        case SIGINT:
            DebugOutput("signal %d(Interrupt)\r\n", s);
            break;
        case SIGTERM:
            DebugOutput("signal %d(Termination)\r\n", s);
            break;
        case SIGTRAP:
            DebugOutput("signal %d(Trace trap)\r\n", s);
            break;
    }
  int N = 100;
   
	
    
    size_t size;
    FILE *in; 
    size = backtrace(stack, N);
    Dl_info dlinfo;
    DebugOutput("%s N=%d\r\n", "DUMP**************************", size);
  
  //  char **str = backtrace_symbols(stack, size);
    for (size_t i = 0; i < size; ++i) 
    {
        char *name = 0;
        if (dladdr(stack[i], &dlinfo)) 
        {
            int err = -1;
            size_t s = 1023;
            name = __cxxabiv1::__cxa_demangle(dlinfo.dli_sname, n, &s, &err);

            if (!err && name && strlen(name))
            {
             DebugOutput("%s\r\n", name);

            }
//            else
  //          DebugOutput("%s\r\n", str[i]);
        } 
    //    else
#ifndef ANDROID_NDK
           
      //   DebugOutput("%s\r\n", str[i]);
        bzero(addr2line_cmd, 512);
        sprintf(addr2line_cmd, "addr2line -e %s %p\n", dlinfo.dli_fname,
         (void *)(( ( char * ) stack[i] ) - ( char * ) dlinfo.dli_fbase ) );
         if(in = popen(addr2line_cmd, "r"))
           {
               fgets(addr2line_cmd, 512, in); 
               pclose(in); 
             DebugOutput(addr2line_cmd);
           }    

        system(addr2line_cmd);
#endif
    }
    DebugOutput("%s", "**************************DUMP\r\n");
   if (s != SIGTRAP && s != SIGINT)
	   exit(0);
}

//-------------------------------------
//
//-------------------------------------
static stack_t m_stack;
void SetErrorHandle(int h)
{
#if (1) //ndef NDEBUG
   struct sigaction act;
    memset(&act, 0, sizeof (act));

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, h);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = DampHandler;
    if (StdError(sigaction(h, &act, 0)) != 0) 
    {
        return;
    }
    StdError(sigprocmask(SIG_UNBLOCK, &act.sa_mask, 0));
#endif
}
void newHandler()
{

	DebugOutput("OUT OF MEMORY");
    PrintStack();
}

void on_terminate()
{
    DebugOutput("std::terminate");
    PrintStack();

    std::exception_ptr eptr = std::current_exception();
    if (eptr)
    {
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (const std::exception &e)
        {
        }
    }
    abort();
}

//-------------------------------------
//
//-------------------------------------
void InitErrorHandles()
{
	memset(&m_stack, 0, sizeof(m_stack));
	/* Reserver the system default stack size. We don't need that much by the way. */
	m_stack.ss_size = SIGSTKSZ;
	m_stack.ss_sp = malloc(m_stack.ss_size);
	m_stack.ss_flags = 0;
	/* Install alternate stack size. Be sure the memory region is valid until you revert it. */
	if (m_stack.ss_sp != NULL && sigaltstack(&m_stack, NULL) == 0)
	{
		TRACE("Set STACK");
	}

#ifdef ANDROID_NDK
	std::set_new_handler(newHandler);
//	return;
#endif

	SetErrorHandle( SIGABRT);
    SetErrorHandle( SIGSEGV);
#ifdef  SIGSTKFLT
    SetErrorHandle( SIGSTKFLT);
#endif
    SetErrorHandle(SIGILL);
    SetErrorHandle(SIGFPE );
    SetErrorHandle(SIGPIPE);
    SetErrorHandle(SIGINT);
    SetErrorHandle(SIGTERM);
    SetErrorHandle(SIGTRAP);
    //SetErrorHandle(SIGQUIT);
    //SetErrorHandle(SIGSTOP);

    std::set_terminate(on_terminate);

    
}
//extern char nativeLibraryDir[1024];

//-------------------------------------
//
//-------------------------------------
EXPORTNIX void PrintStackToBuffer(TString &b, int N) 
{
    crit.Enter();
    
    TString s;
  FILE *in; 
    TRACE("STACK**************************\r\n");
    size_t size;
    void **stack = (void **)malloc(N * sizeof(void *));

    size = backtrace(stack, N);
    Dl_info dlinfo;
    TString tmp;  
//    TString path = nativeLibraryDir;
  //  char **str = backtrace_symbols(stack, size);
    for (size_t i = 1; i < size; ++i) 
    {
        char *name = 0;
        tmp = "";
        if (dladdr(stack[i], &dlinfo)) 
        {
            int err = -1;
            char *n = (char *)alloca(1024);
            size_t s = 1023;
            name = __cxxabiv1::__cxa_demangle(dlinfo.dli_sname, n, &s, &err);

            if (!err) 
            {
                tmp = name;

             //   free(name);
            } 
           // else
             //   tmp = str[i];
        } 
        //else
          // 
            //tmp = str[i];
#ifndef ANDROID_NDK

        if (tmp.StrLen())
        {
            tmp += "\n";
            b += tmp;
            DataOutput(tmp.GetPtr());
           char addr2line_cmd[MAX_PATH];
           bzero(addr2line_cmd, MAX_PATH);
           tmp = path;
           tmp += "/";
           tmp += dlinfo.dli_fname;
			DWORD dw = GetFileAttributes(tmp);
			if (dw == 0xFFFFFFFF)
		           tmp = dlinfo.dli_fname;
           sprintf(addr2line_cmd, "arm-linux-androideabi-addr2line -e %s %p\n", tmp.GetPtr(),
            (void *)(( ( char * ) stack[i] ) - ( char * ) dlinfo.dli_fbase ) );
			TRACE("addr2line_cmd = %s\r\n", addr2line_cmd);
           tmp.Clear();
           tmp.SetLen(512);
            if(in = popen(addr2line_cmd, "r"))
              {
                  fgets(tmp.GetPtr(), 512, in); 
                  pclose(in); 
                DataOutput(tmp.GetPtr());
                  b += tmp;
              }    
        }
#else
        tmp += "\n";
        DataOutput(tmp.GetPtr());

#endif

    }
    free(stack);
    b += "**************************STACK\r\n";
    crit.Leave();
}

void PrintStack() 
{
    TString s;
 
    PrintStackToBuffer(s, 20);
  
  //  DebugOutput(s.GetPtr());
    

}

//-------------------------------------
//
//-------------------------------------

_FEXPORT32 void __ASSERT(bool f, const char *pFile, int line)
{
    if (f == 0)
    {
        TRACE(">>>>>>>ASSERT %s : %d = \r\n", pFile, line);
		PrintStack();
		TRACE("ASSERT<<<<<<<<<<<<<<<<<<<<<<<<<<<\r\n");

	 //  raise(SIGTRAP);

    }
}
//-------------------------------------
//
//-------------------------------------

void OnEndChildProcess(int status, pid_t pid) 
{
    TString err, err1; 
    err.Format("pid=%d ", pid);
    if (WIFEXITED(status))
        err1.Format("terminate status = %d\r\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        err1.Format("kill signal = %d%s\r\n",WTERMSIG(status), WCOREDUMP(status) ? " (kill kernel)" : "");
    err += err1;
    DebugOutput(err.GetPtr());
}
//-------------------------------------
//
//-------------------------------------
//-------------------------------------

_FEXPORT32 int __BoolWarning(int res, const char *pstr, const char *pFile, int line) 
{
    if (res == 1) return res;
    __ASSERT(res, pFile, line);
    
 
#ifdef _DEBUG_

    TString err;
    err = "***********************************\r\n";
    if (pstr)
        err.Format("Warning %s -> %s(%d)\r\n", pstr, pFile, line);
    else
        err.Format("Warning -> %s(%d)\r\n", pFile, line);
    PrintStackToBuffer(err, 100);
    err += "***********************************\r\n";
    DebugOutput(err.GetPtr());
#endif
    return res;
}
//-------------------------------------

_FEXPORT32 int __CheckError(int res, const char *pstr, const char *pFile, int line) 
{
    if (res >= 0) return res;
    int err = errno;
    if (!err) return res;
    err = abs(err);
    errno = 0;
#if (1) //def _DEBUG_
    TString str;
    str.Format("%s(%d) -> ", pFile, line);
    if (pstr)
        str += pstr;
#ifdef __PLUK_LINUX__
    str += strerror(err);
#else
    char *buff;
    int n = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
            (char *) &buff, 0, NULL);
    if (n) {
        str += buff;
        LocalFree(buff);
    } else {
        TString te;
        te.Format(" %d", err);
        str += te;
    }
    SetLastError(0);

#endif
    str += "\r\n";
    TString out;
    out.Format("System error : %s\r\n", str.GetPtr());
    DebugOutput(out.GetPtr());
     PrintStack(); 
#endif
     return -1;
}
