#ifndef ___STD_H_
#define ___STD_H_


#define NOPLUK


#pragma pack(push)
#pragma pack(0)
#ifndef _CONSOLE_PROG_
#include "wx/wx.h"
#endif
#ifdef _EOS_LINUX_
#define __PLUK_LINUX__ _EOS_LINUX_
#endif
#ifdef ANDROID_NDK

#include <jni.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma pack(pop)

#ifdef __PLUK_LINUX__
#pragma pack(push)
#pragma pack(0)
#ifdef NOPLUK

#include <iostream>

#endif
#ifndef ANDROID_NDK
#include <bits/c++config.h>
#endif

#include <ctype.h>
#include <wchar.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
#include <mntent.h>
#include <signal.h>
//#include <sys/time.h>
#ifndef ANDROID_NDK
#include <mqueue.h>
#include <sys/shm.h>
#include <spawn.h>
#include <sys/timerfd.h>

#else

#include <linux/shm.h>

#if defined(__ARM_ARCH_ISA_A64) || defined(__x86_64__)
#define DINT    int64_t
#define DUINT    uint64_t
#define _VER_PLK_64
#else
#define DINT	int32_t
#define DUINT	uint32_t
#endif

#endif

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <errno.h>


#include <pthread.h>
#include <cstdlib>
#include <new>
#include <sys/inotify.h>
#include <sys/times.h>

#include <pthread.h>
#include <semaphore.h>
#include <pwd.h>
#include <math.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <link.h>

#pragma pack(pop)

#include  "uxsignal.h"

#ifndef TIMERID
#define  TIMERID timer_t
#endif

#ifndef CALLBACK
#define CALLBACK
#endif
#define CP_ACP 1

#ifndef AFX_CDECL
#define AFX_CDECL
#endif
#ifndef TEXT
#define TEXT(a) (a)
#endif
#pragma pack(push) //NADA
#pragma pack(4)

typedef struct _EXCEPTION_POINTERS {
    int i;
} EXCEPTION_POINTERS,
        *PEXCEPTION_POINTERS;
#pragma pack(pop)

#ifdef NOPLUK
using namespace std;
#else

template<typename T> inline const T &min(const T &a, const T &b)
{
    return b < a ? b : a;
}
template<typename T> inline const T &max(const T &a, const T &b)
{
    return b > a ? b : a;
}
#endif


#define _stricmp strcasecmp
#ifndef FLOAT
typedef float FLOAT;
#endif
#ifndef LONG
typedef int32_t LONG;
#endif
#ifndef LPLONG
typedef int32_t *LPLONG;
#endif
#ifndef PSTR
typedef char *PSTR;
#endif
#ifndef FLOAT
typedef float FLOAT;
#endif

#ifndef int32
typedef int32_t int32;
#endif
#ifndef _int32
typedef int32_t _int32;
#endif
#ifndef THWND
typedef uint64_t THWND;
#ifdef _CONSOLE_PROG_
#define HWND THWND
#endif


#endif
#ifndef VOID
#define VOID void
#endif
#ifndef int64
typedef int64_t int64;
#endif
#ifndef __int32
typedef int __int32;
#endif
#ifndef __int64
typedef int64_t __int64;
#endif
#ifndef DWORD64
typedef uint64_t DWORD64;
#endif
#ifndef UINT64
typedef uint64_t UINT64;
#endif

#ifndef INT64
typedef int64_t INT64;
#endif
#ifndef LONGLONG
#define LONGLONG INT64
#endif
#ifndef INT32
typedef int32_t INT32;
#endif
#ifndef QWORD
typedef uint64_t QWORD;
#endif

#ifndef WINAPI
#define WINAPI
#endif
#ifndef __cdecl
#define __cdecl
#endif
#ifndef BOOL
typedef int32 BOOL;
#endif
#ifndef SHORT
typedef short SHORT;
#endif

#ifndef INT
typedef int32 INT;
#endif
#ifndef LPINT
typedef int32 *LPINT;
#endif
#ifndef ULONG
typedef uint32_t ULONG;
#endif

#ifndef DWORD
typedef uint32_t DWORD;
#endif
#ifndef PDWORD
typedef uint32_t *PDWORD;
#endif

#ifndef UINT
typedef uint32_t UINT;
#endif
#ifndef LPVOID
typedef void *LPVOID;
#endif
#ifndef WCHAR
typedef wchar_t WCHAR;
#endif
#ifndef IN
#define IN
#define OUT
#endif
#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef PBYTE
typedef unsigned char *PBYTE;
#endif

#ifndef UCHAR
typedef unsigned char UCHAR;
#endif
#ifndef CHAR
typedef char CHAR;
#endif
#ifndef PCHAR
typedef char *PCHAR;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif
#ifndef SIZE_T
typedef size_t SIZE_T;
#endif
#ifndef LPCSTR
typedef const char *LPCSTR;
#endif
#ifndef LPSTR
typedef char *LPSTR;
#endif

#ifdef UNICODE
#ifndef _EOS_LINUX_
#ifndef LPCTSTR
typedef const wchar_t * LPCTSTR;
typedef wchar_t * LPTSTR;
#ifndef TCHAR
typedef wchar_t TCHAR;
#endif
#endif
#else
#ifndef TCHAR
typedef char TCHAR;
#endif

#ifndef LPCTSTR
typedef const char * LPCTSTR;
typedef char * LPTSTR;
#endif

#endif
#else
#ifndef TCHAR
typedef char TCHAR;
#endif

#ifndef LPCTSTR
typedef const char *LPCTSTR;
typedef char *LPTSTR;
#endif

#endif
#ifndef LPBYTE
typedef unsigned char *LPBYTE;
#endif

#ifndef LPSTR
typedef char *LPSTR;
#endif
#ifndef LPCWSTR
typedef const wchar_t *LPCWSTR;
#endif
#ifndef WORD
typedef unsigned short WORD;
#endif
#ifndef _hypot
#define _hypot hypot
#endif
#ifndef DWORD_PTR
typedef size_t DWORD_PTR;
typedef size_t ULONG_PTR;
typedef size_t UINT_PTR;

#if defined(__ARM_ARCH_ISA_A64) || defined(__x86_64__)
typedef int64_t INT_PTR;
#else
typedef int INT_PTR;
#endif
#ifndef LRESULT
typedef DWORD_PTR LRESULT;
#endif

#endif
#ifndef HRESULT
#define HRESULT LRESULT
#endif

#ifndef WPARAM
typedef DWORD_PTR WPARAM;
#endif
#ifndef LPARAM
typedef DWORD_PTR LPARAM;
#endif

#ifndef HANDLE
typedef DWORD_PTR HANDLE;
#endif
#ifndef HGLOBAL
typedef void *HGLOBAL;
#endif

#ifndef HMODULE
typedef void *HMODULE;
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef WINAPI
#define WINAPI
#endif
#ifndef LPWSTR
typedef wchar_t *LPWSTR;
#endif
#ifndef HINSTANCE
typedef void *HINSTANCE;
#endif
#ifndef LANGID
typedef UINT LANGID;
#endif
#pragma pack(push) //NADA
#pragma pack(4)
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };

    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;
typedef LARGE_INTEGER *PLARGE_INTEGER;
#ifndef MAX_PATH
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#define MAX_PATH PATH_MAX
#endif
#pragma pack(pop)


#ifndef LoadLibraryA

typedef int (*FARPROC)(void);

#define HINSTANCE_ERROR 0

inline void *LoadLibraryA(const char *a)
{
    void *p = dlopen(a, RTLD_LAZY);
    if (p)
    {
        typedef int (*mFunc)(HINSTANCE);

        mFunc a = (mFunc) dlsym(p, "baselib");
        if (a) a((HINSTANCE) p);
    }
    return p;
}

#define GetModuleHandleA(a) (a ? LoadLibraryA(a) : (void *)getpid())
#define GetModuleHandle GetModuleHandleA
#define LoadLibrary LoadLibraryA
#define FreeLibrary dlclose

inline FARPROC GetProcAddress(void *a, const char *b)
{
    switch ((size_t) b)
    {
        case 2:
            return (FARPROC) dlsym(a, "CHECK");
        case 3:
        {
            FARPROC fc;
            fc = (FARPROC) dlsym(a, "REGMETHOD");
            if (!fc)
                fc = (FARPROC) dlsym(a, "REGMETHODS");
            return fc;
        }
        case 4:
            return (FARPROC) dlsym(a, "GETERRORS");
        case 5:
            return (FARPROC) dlsym(a, "INITDLL");
    }
    return (FARPROC) dlsym(a, b);
}

#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))
#define MAKEWPARAM(l, h)      ((WPARAM)(DWORD)MAKELONG(l, h))
#define MAKELPARAM(l, h)      ((LPARAM)(DWORD)MAKELONG(l, h))
#define MAKELRESULT(l, h)     ((LRESULT)(DWORD)MAKELONG(l, h))

#define INVALID_HANDLE_VALUE -1
#ifndef HDRAWDIB
typedef HANDLE HDRAWDIB;
#endif
#ifndef HGDIOBJ
typedef HGLOBAL HGDIOBJ;
#endif

//#define GetTickCount() (clock()/1000)
inline DWORD GetTickCount()
{
    DWORD cs = sysconf(_SC_CLK_TCK);
    time_t t = times(0) * 1000;
    DWORD res = (DWORD) (t / cs);
    return res;

}

//#ifndef WRONG_PARAMETERS
//#define FILE_OPEN_ERROR 3
//#define OUT_OF_MEMORY 2

//#define WRONG_PARAMETERS 1
//#define PlukError
//#endif
#ifndef NOPLUK
#ifndef _FEXPORT32
#define _FEXPORT32
#endif
#endif

inline int MulDiv(int a, int b, int c)
{ return int((double) (a * b) / (double) c + 0.5); }

#pragma pack(push) //NADA
#pragma pack(4)
typedef struct _FIXED {
#ifndef _MAC
    WORD fract;
    short value;
#else
    short   value;
    WORD    fract;
#endif
} FIXED;

#define _tcscpy strcpy
#define _tcsstr strstr
#define _tcslen strlen
#define _tcscmp strcmp
#define _sntprintf snprintf
#define tcsupr _toupper
#define tcsupr _toupper
#define _tcscat strcat
#define lstrcmp strcmp

inline char *_tcsupr(char *p)
{

    for (int i = 0, len = strlen(p); i < len; ++i)
#ifndef ANDROID_NDK
        p[i] =_toupper(p[i]);
#else
            p[i] = toupper(p[i]);
#endif
    return p;
}

#define ZeroMemory(a, b) memset(a, 0, b)
#ifndef VERIFY
#define VERIFY(a) (a)
#endif
#ifndef lstrcpyn
#define lstrcpyn strncpy
#define lstrcpy strcpy
#define lstrcat strcat

#endif
/*
#define //export "C"
#define _PEXPORT
#define _PEXPORT*/
#else
#ifndef _FEXPORT32
#define _FEXPORT32
#endif

#include <new.h>
#include <windows.h>
#include <wchar.h>
#include <mbstring.h>
#include <stdio.h>
#include <tchar.h>
#include <io.h>

#define bzero(a, b) memset(a, 0, b);
typedef HANDLE pid_t;
#endif

struct DPoint {
    double x, y;

    void Set(double _x, double _y)
    {
        x = _x;
        y = _y;
    }
};

#ifndef FILEHANDLE
#ifdef __PLUK_LINUX__
typedef int FILEHANDLE;
#else
typedef HANDLE FILEHANDLE;
#endif
#endif
#ifndef MAPHANDLE
#ifdef __PLUK_LINUX__
typedef void *MAPHANDLE;

#include <alloca.h>

#ifndef _alloca
#define _alloca alloca
#endif

#else
#ifndef TIMERID
#define  TIMERID int
#endif

typedef HANDLE MAPHANDLE;
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <limits.h>

#ifndef _MAX_PATH
#define _MAX_PATH   MAX_PATH
#endif

#ifdef __PLUK_LINUX__

#ifndef _getcwd
#define _getcwd getcwd
#endif
#ifndef _access
#define  _access access
#endif
#ifndef _chdir
#define  _chdir chdir
#endif
#ifndef _chmod
#define  _chmod chmod
#endif
#ifndef _getpid
#define _getpid getpid
#endif
#endif
#ifdef __PLUK_LINUX__
#ifndef SW_HIDE
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_RESTORE         7
#define SW_SHOWMINNOACTIVE SW_MINIMIZE
#endif
#ifndef FAR
#define FAR
#endif
#ifndef CONST
#define CONST const
#endif
#ifndef BASED_CODE
#define BASED_CODE
#endif
#define WM_GETDLGCODE                   0x0087
#define WM_SETFONT                      0x0030
#define WM_GETFONT                      0x0031

#define WM_ACTIVATE                     0x0006

#define WM_CTLCOLORMSGBOX               0x0132
#define WM_CTLCOLOREDIT                 0x0133
#define WM_CTLCOLORLISTBOX              0x0134
#define WM_CTLCOLORBTN                  0x0135
#define WM_CTLCOLORDLG                  0x0136
#define WM_CTLCOLORSCROLLBAR            0x0137
#define WM_CTLCOLORSTATIC               0x0138

#define WM_SETTEXT                      0x000C
#define WM_GETTEXT                      0x000D
#define WM_GETTEXTLENGTH                0x000E
#define WM_NCHITTEST                    0x0084
#define WM_CTLCOLOR     0x0019

#define WM_NOTIFY                       0x004E

#define WM_CLOSE 0x0010
#define WM_COMMAND 0x111
#define WM_USER 0x0400
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002
#define WM_MOVE                         0x0003
#define WM_SIZE                         0x0005
#define WM_QUIT                         0x0012
#define WM_ERASEBKGND                   0x0014
#define WM_NCDESTROY 0x0082
#define WM_PAINT                        0x000F
#define WM_NCPAINT                      0x0085
#define WM_TIMER                        0x0113
#define WM_KILLFOCUS                    0x0008
#define WM_SETFOCUS                     0x0007
#define WM_MOUSEMOVE                    0x0200
#define WM_MOUSEWHEEL                   0x020A
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_LBUTTONDBLCLK                0x0203
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_RBUTTONDBLCLK                0x0206
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
#define WM_MBUTTONDBLCLK                0x0209

#define WM_SIZING                       0x0214

#define WM_CUT                          0x0300
#define WM_COPY                         0x0301
#define WM_PASTE                        0x0302
#define WM_KEYFIRST                     0x0100
#define WM_KEYLAST                      0x0109
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_CHAR                         0x0102
#define WM_SYSKEYDOWN                   0x0104
#define WM_SYSKEYUP                     0x0105
#define WM_SYSCHAR                      0x0106
#define WM_INITDIALOG                   0x0110
#define WM_SETCURSOR                    0x0020
#define WM_HSCROLL                      0x0114
#define WM_VSCROLL                      0x0115
#define WM_SYSCOLORCHANGE               0x0015
#define WM_MOUSEHOVER                   0x02A1
#define WM_MOUSELEAVE                   0x02A3

#define CN_COMMAND              0               // void ()
#define CN_UPDATE_COMMAND_UI    ((INT)(-1))    // void (CCmdUI*)

typedef int SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define WSAGETASYNCERROR(lParam)            HIWORD(lParam)
#define WSAGETSELECTEVENT(lParam)           LOWORD(lParam)
#define MAXGETHOSTSTRUCT        1024

#define FD_READ_BIT      0
#define FD_READ          (1 << FD_READ_BIT)

#define FD_WRITE_BIT     1
#define FD_WRITE         (1 << FD_WRITE_BIT)

#define FD_OOB_BIT       2
#define FD_OOB           (1 << FD_OOB_BIT)

#define FD_ACCEPT_BIT    3
#define FD_ACCEPT        (1 << FD_ACCEPT_BIT)

#define FD_CONNECT_BIT   4
#define FD_CONNECT       (1 << FD_CONNECT_BIT)

#define FD_CLOSE_BIT     5
#define FD_CLOSE         (1 << FD_CLOSE_BIT)

#define FD_QOS_BIT       6
#define FD_QOS           (1 << FD_QOS_BIT)


#ifndef _PATH_MOUNTED
#define	_PATH_MOUNTED	"/etc/mtab"
#endif

#else

#include <new.h>
#include <windows.h>
#include <wchar.h>
#include <mbstring.h>
#include <stdio.h>
#include <tchar.h>
#include <io.h>
#define bzero(a, b) memset(a, 0, b);
typedef HANDLE pid_t;
#endif
#ifndef FILEHANDLE
#ifdef __PLUK_LINUX__
typedef int FILEHANDLE;
#else
typedef HANDLE FILEHANDLE;
#endif
#endif
#ifndef MAPHANDLE
#ifdef __PLUK_LINUX__
typedef void *MAPHANDLE;
#else
#ifndef TIMERID
#define  TIMERID int
#endif
typedef HANDLE MAPHANDLE;
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
//#pragma pack(2) 
#ifndef NOPLUK
#include "expdef.h"
#include "palloc.h"
#else
#ifndef _HUGE_
#define _HUGE_
#endif

#ifndef _FEXPORT
#define _FEXPORT
#endif

#ifndef _FEXPORT32
#ifdef __PLUK_LINUX__
#define _FEXPORT32 __attribute__((visibility("default")))

#else
#define _FEXPORT32 __declspec(dllexport)
#endif
#endif

#ifdef __PLUKDLL__
#ifndef _PEXPORT
#ifdef __PLUK_LINUX__
#define _PEXPORT __attribute__((visibility("default")))
#else
#define _PEXPORT __declspec(dllexport)
#endif

#endif
#ifndef _FPEXPORT32
#ifdef __PLUK_LINUX__
#define _FEXPORT32_
#define _FPEXPORT32 __attribute__((visibility("default")))

#else
#define _FPEXPORT32	__declspec(dllexport)
#define_FPEXPORT32_ __declspec(dllexport)
#endif

#endif
#else
#ifndef _PEXPORT
#ifdef __PLUK_LINUX__
#ifndef ANDROID_NDK
#define _PEXPORT __attribute__((visibility("default")))
#else
#define _PEXPORT
#endif

#else
#define _PEXPORT __declspec(dllimport)
#endif
#endif
#ifndef _FPEXPORT32
#ifdef __PLUK_LINUX__
#define _FPEXPORT32 extern "C++" \

#else
#define _FPEXPORT32 __declspec(dllexport)
#endif
#endif
#endif

#ifndef _FPEXPORT
#define _FPEXPORT
#endif
#ifndef _SETDS
#define _SETDS
#endif

#ifndef _RESETDS
#define _RESETDS
#endif
#ifndef EXPORTNIX
#ifdef __PLUK_LINUX__
#define EXPORTNIX extern "C++" \

#define EXPORTNC extern "C" \

#define EXPMDC __attribute__((visibility("default")))

#else
#define EXPORTNC __declspec(dllexport)
#define EXPORTNIX
#endif
#endif
#ifndef EXPORTNC
#define EXPORTNC EXPORTNIX
#endif


#endif

#include "buffer.h"

struct iPOINT {
    int32 x, y;
};

struct POINT : public iPOINT {
    POINT()
    { x = y = 0; }

    POINT(int initX, int initY)
    {
        x = initX;
        y = initY;
    }

    POINT(DWORD dwPoint)
    {
        x = (short) LOWORD(dwPoint);
        y = (short) HIWORD(dwPoint);
    }

    void Set(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    void SetPoint(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    void Offset(int xOffset, int yOffset)
    {
        x += xOffset;
        y += yOffset;
    }

    void Offset(POINT point)
    {
        x += point.x;
        y += point.y;
    }

    BOOL operator==(const POINT &point) const
    { return (x == point.x && y == point.y); }

    BOOL operator!=(const POINT &point) const
    { return (x != point.x || y != point.y); }

    void operator+=(const POINT &point)
    {
        x += point.x;
        y += point.y;
    }

    void operator-=(const POINT &point)
    {
        x -= point.x;
        y -= point.y;
    }

    void operator+=(int i)
    {
        x += i;
        y += i;
    }

    void operator-=(int i)
    {
        x -= i;
        y -= i;
    }

    POINT &operator/=(double d)
    {
        x = int(x / d + 0.5);
        y = int(y / d + 0.5);
        return *this;
    }

    POINT &operator*=(double d)
    {
        x = int(x / d + 0.5);
        y = int(y / d + 0.5);
        return *this;
    }

    POINT operator*(double d)
    {
        POINT v(*this);
        v.x = int(v.x * d + 0.5);
        v.y = int(v.y * d + 0.5);
        return *this;
    }

    POINT operator/(double d)
    {
        POINT v(*this);
        v.x = int(v.x / d + 0.5);
        v.y = int(v.y / d + 0.5);
        return *this;
    }

    int &operator[](int i)
    { return !i ? x : y; }

    POINT operator-() const
    { return POINT(-x, -y); }

    POINT operator+(const POINT &point) const
    { return POINT(x + point.x, y + point.y); }

    POINT operator-(const POINT &point) const
    { return POINT(x - point.x, y - point.y); }

    POINT operator+(int i) const
    { return POINT(x + i, y + 1); }

    POINT operator-(int i) const
    { return POINT(x - i, y - 1); }

    bool operator<(const POINT &b) const
    {
        if (x != b.x) return x < b.x;
        if (y != b.y) return y < b.y;
        return false;
    }

    bool operator>(const POINT &b) const
    {
        if (x != b.x) return x > b.x;
        if (y != b.y) return y > b.y;
        return false;
    }

};

typedef POINT *LPPOINT;

struct POINTL {
    int32 x, y;
};

struct SIZE {
    int32 cx, cy;

    SIZE(int x, int y)
    {
        cx = x;
        cy = y;
    }

    SIZE(const SIZE &sz)
    {
        cx = sz.cx;
        cy = sz.cy;
    }

    SIZE()
    { cx = cy = 0; }

    SIZE &operator+=(const SIZE &sz)
    {
        cx += sz.cx;
        cy += sz.cy;
        return *this;
    }

    SIZE &operator-=(const SIZE &sz)
    {
        cx -= sz.cx;
        cy -= sz.cy;
        return *this;
    }
};

#define CSize SIZE

struct RECT {
    int32 left;
    int32 top;
    int32 right;
    int32 bottom;

    RECT()
    { left = top = right = bottom = 0; }

    RECT(int32 l, int32 t, int32 r, int32 b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    RECT(const RECT *lpSrcRect)
    { memcpy(this, lpSrcRect, sizeof(RECT)); }

    RECT(POINT topLeft, POINT bottomRight)
    {
        left = topLeft.x;
        top = topLeft.y;
        right = bottomRight.x;
        bottom = bottomRight.y;
    }

    RECT(POINT topLeft, SIZE sz)
    {
        left = topLeft.x;
        top = topLeft.y;
        right = left + sz.cx;
        bottom = top + sz.cy;
    }

    RECT &operator=(const RECT &rc)
    {
        left = rc.left;
        top = rc.top;
        right = rc.right;
        bottom = rc.bottom;
        return *this;
    }

    bool operator==(const RECT &r)
    { return r.left == left && r.right == right && r.top == top && r.bottom == bottom; }

    bool operator!=(const RECT &r)
    { return !(operator==(r)); }

    RECT &Set(int32 l, int32 t, int32 r, int32 b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
        return *this;
    }

    void Set(const POINT &lt, const POINT &rb)
    {
        left = lt.x;
        top = lt.y;
        right = rb.x;
        bottom = rb.y;
    }

    RECT &SetRect(int32 l, int32 t, int32 r, int32 b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
        return *this;
    }

    int Width() const
    { return right - left; }

    int Height() const
    { return bottom - top; }

    void Update()
    {
        int l = min(left, right), t = min(top, bottom), r = max(left, right), b = max(top, bottom);
        Set(l, t, r, b);
    }

    void SetWidth(int n)
    {
        right = left + n;
        Update();
    }

    void SetHeight(int n)
    {
        top = bottom + n;
        Update();
    }

    void Move(int x, int y)
    {
        right += x - left;
        bottom += y - top;
        top = y;
        left = x;
    }

    BOOL IsEmpty() const
    { return (left == 0 && right == 0 && top == 0 && bottom == 0); }

    BOOL IsRectEmpty() const
    { return (left == 0 && right == 0 && top == 0 && bottom == 0); }

    BOOL IsRectNull() const
    { return (left == 0 && right == 0 && top == 0 && bottom == 0); }

    void SetRectEmpty()
    { left = top = right = bottom = 0; }

    void Clear()
    { left = right = top = bottom = 0; }

#ifndef _CONSOLE_PROG_
#ifdef __WXGTK__

    operator wxRect () const { return wxRect(left, top, right - left, bottom - top); }
    RECT &operator = (const wxRect &rc)  { left = rc.x; top = rc.y; right = rc.width + rc.x; bottom = rc.height + rc.y; return *this; }
#endif
#endif
public:
    POINT CenterPoint() const
    { return POINT((left + right) / 2, (top + bottom) / 2); }

    operator RECT *()
    { return this; }

    BOOL PtIn(int x, int y)
    { return x >= left && y >= top && x <= right && y <= bottom; };

    BOOL PtInRegion(int x, int y)
    { return x >= left && y >= top && x < right && y < bottom; };

    void InflateRect(int x, int y)
    {
        left -= x;
        top -= y;
        right += x;
        bottom += y;
    }

    void OffsetRect(int x, int y)
    {
        left += x;
        top += y;
        right += x;
        bottom += y;
    }

    void Normalize()
    {
        int nTemp;
        if (left > right)
        {
            nTemp = left;
            left = right;
            right = nTemp;
        }
        if (top > bottom)
        {
            nTemp = top;
            top = bottom;
            bottom = nTemp;
        }
    }

    RECT &operator|=(const RECT &r)
    {
        left = min(left, r.left);
        top = min(top, r.top),
                right = max(right, r.right);
        bottom = max(bottom, r.bottom);
        return *this;
    }

    RECT &operator&=(const RECT &r)
    {
        left = max(left, r.left);
        top = max(top, r.top);
        right = min(right, r.right);
        bottom = min(bottom, r.bottom);
        if (left >= right || top >= bottom)
            Clear();
        return *this;
    }

    void Update(int x, int y)
    {
        left = min(x, left);
        top = min(y, top);
        right = max(x, right);
        bottom = max(y, bottom);
    }

#ifdef ANDROID_NDK

    bool Minus(const RECT &a, MArray<RECT> &ret)
    {
        int len = ret.GetLen();
        RECT newr;
        newr.left = max(a.left, left);
        newr.top = max(a.top, top);
        newr.right = min(a.right, right);
        newr.bottom = min(a.bottom, bottom);
        if (newr.left <= newr.right && newr.top <= newr.bottom)
        {
            if (newr.left > left)
                ret.Add().Set(left, top, newr.left, bottom);
            if (newr.right < right)
                ret.Add().Set(newr.right, top, right, bottom);
            if (newr.top > top)
                ret.Add().Set(newr.left, top, newr.right, newr.top);
            if (newr.bottom < bottom)
                ret.Add().Set(newr.left, newr.bottom, newr.right, bottom);
        }
        return ret.GetLen() == len;

    }

#endif

};

inline void CopyRect(RECT *a, RECT *b)
{ *a = *b; }

#pragma pack(pop)

typedef const RECT *LPCRECT;
typedef RECT *LPRECT;
#ifdef NOPLUK
typedef RECT IRect;
#endif
typedef POINT IPoint;


inline RECT operator|(const RECT &a, const RECT &r)
{
    return RECT(min(a.left, r.left), min(a.top, r.top), max(a.right, r.right),
                max(a.bottom, r.bottom));
}

inline RECT operator&(const RECT &a, const RECT &r)
{
    RECT newr;
    newr.left = max(a.left, r.left);
    newr.top = max(a.top, r.top);
    newr.right = min(a.right, r.right);
    newr.bottom = min(a.bottom, r.bottom);
    return newr.left <= newr.right && newr.top <= newr.bottom ? newr : newr.Set(0, 0, 0, 0);
}

inline void SetRect(RECT *r, int32 left, int32 top, int32 right, int32 bottom)
{
    r->left = left;
    r->top = top;
    r->right = right;
    r->bottom = bottom;
}

inline BOOL PtInRect(RECT *rc, POINT *p)
{

    return rc && p ? p->x >= rc->left && p->y >= rc->top && p->x <= rc->right && p->y <= rc->bottom
                   : 0;

}

inline BOOL PtInRect(RECT *rc, const POINT &p)
{

    return rc ? p.x >= rc->left && p.y >= rc->top && p.x <= rc->right && p.y <= rc->bottom : 0;

}

inline const char *itoa(int num, char *buf, size_t len)
{
    if (!buf)
        buf = (char *) alloca(len * 2);
    if (snprintf(buf, len, "%d", num) == -1)
        return ""; /* or whatever */

    return buf;
}

#ifndef WIN32
EXPORTNIX struct EXPMDC tagPTR {
    tagPTR()
    {}

    virtual ~tagPTR()
    {}
};

#include "afx.h"

#endif
#ifdef ANDROID_NDK

/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#define VK_BACK           0x08
#define VK_TAB            0x09

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define VK_KANA           0x15
#define VK_HANGEUL        0x15  /* old name - should be here for compatibility */
#define VK_HANGUL         0x15
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19

#define VK_ESCAPE         0x1B

#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F
#define VK_SLEEP          0x5F

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87
#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91

#endif
#endif
