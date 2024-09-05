#ifndef __AFX_TMP__
#define __AFX_TMP__

#include "mstring.h"
#include "mfile.h"
#include "mtree.h"

#include "except.h"

#define MAIN_INI "main.rc.ini"
#define SWP_NOMOVE 1
#define SWP_NOSIZE 2
#define SWP_NOZORDER 4
#define SWP_NOACTIVATE 8
#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6
#define _MAX_DRIVE _MAX_PATH
#define _MAX_FNAME _MAX_PATH
#define _MAX_EXT _MAX_PATH
typedef tagPTR CObject;
#ifndef ANDROID_NDK
#define ACCESS access
#else
EXPORTNIX int ACCESS(LPCSTR, int);
#endif

#define FILE_ATTRIBUTE_READONLY             0x00000001
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020
#pragma pack(push) //NADA
#pragma pack(4)
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

EXPORTNIX struct WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    char cFileName[MAX_PATH];
    char cAlternateFileName[14];

    WIN32_FIND_DATAA()
    { memset(this, 0, sizeof(WIN32_FIND_DATAA)); }

    WIN32_FIND_DATAA(LPCSTR filename)
    { Init(filename); }

    BOOL Init(LPCSTR filename);
};
typedef WIN32_FIND_DATAA *LPWIN32_FIND_DATAA;
struct WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    wchar_t cFileName[MAX_PATH];
    wchar_t cAlternateFileName[14];
};

typedef WIN32_FIND_DATAW *LPWIN32_FIND_DATAW;
#define WIN32_FIND_DATA WIN32_FIND_DATAA

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
#pragma pack(pop)
#define FILE_NOTIFY_CHANGE_FILE_NAME    IN_MODIFY
#define FILE_NOTIFY_CHANGE_DIR_NAME     IN_MODIFY
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   IN_ATTRIB
#define FILE_NOTIFY_CHANGE_SIZE         (IN_CREATE | IN_DELETE | IN_MODIFY)
#define FILE_NOTIFY_CHANGE_LAST_WRITE   IN_CLOSE_WRITE
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  IN_ACCESS
EXPORTNIX bool
GetDirList(LPCSTR path, LPCSTR mask, WIN32_FIND_DATAA *&dir, int &num_dir, WIN32_FIND_DATAA *&file,
           int &num_file);
EXPORTNIX bool
GetDirList(LPCSTR path, LPCSTR mask, WIN32_FIND_DATAW *&dir, int &num_dir, WIN32_FIND_DATAW *&file,
           int &num_file);
EXPORTNIX bool UpdatePathFromWIN32(LPCSTR path, bool fIsDir, TString &res);

EXPORTNIX BOOL FindClose(HANDLE hFindFile);
EXPORTNIX BOOL FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATAA ret);
EXPORTNIX HANDLE FindFirstFile(LPCSTR lpFileName, LPWIN32_FIND_DATAA ret);

EXPORTNIX void FileTimeToSystemTime(FILETIME *utcft, SYSTEMTIME *sys);
EXPORTNIX BOOL SystemTimeToFileTime(SYSTEMTIME *sys, FILETIME *utcft);
EXPORTNIX DWORD GetFileAttributes(TString path);
EXPORTNIX BOOL GetFileTime(TString path, FILETIME *ret);
EXPORTNIX BOOL GetFileTime(FILEHANDLE hFile, FILETIME *ret);

#define PROGRESS_CONTINUE   0
#define PROGRESS_CANCEL     1
#define PROGRESS_SKIP    2

typedef DWORD (WINAPI *MPROGRESS_ROUTINE)(LPCSTR filename, UINT_PTR size, UINT_PTR pos,
                                          LPVOID lpUserData);


#define DIR_ERROR_INVALID_NAME               -123
#define DIR_ERROR_ALREADY_EXISTS             -183
#define DIR_ERROR_NOERROR  1
#define DIR_ERROR  0
EXPORTNIX int CreateDirectoryAlways(LPCSTR _dir);
EXPORTNIX int CreateDirectoryRetErr(LPCSTR lpPathName, DWORD Attributes);

inline BOOL CreateDirectory(LPCSTR lpPathName, DWORD Attributes)
{
    int err = CreateDirectoryRetErr(lpPathName, Attributes);
    return err == DIR_ERROR_NOERROR || err == DIR_ERROR_ALREADY_EXISTS;
}

EXPORTNIX void MakeDirFromFile(const char *_file);

EXPORTNIX BOOL _CopyFile(const char *SrcName, const char *DstName, MPROGRESS_ROUTINE func = 0,
                         void *lpUserData = 0, bool fMove = false);

inline BOOL CopyFile(const char *a, const char *b, int)
{ return _CopyFile(a, b); }

inline BOOL MoveFile(const char *a, const char *b)
{ return _CopyFile(a, b, 0, 0, true); }

EXPORTNIX BOOL CopyFileTo(const char *SrcName, const char *DstName, MPROGRESS_ROUTINE func = 0,
                          void *lpUserData = 0, bool fMove = false);

inline BOOL MoveFileTo(const char *SrcName, const char *DstName, MPROGRESS_ROUTINE func = 0,
                       void *lpUserData = 0)
{
    return CopyFileTo(SrcName, DstName, func, lpUserData, true);

}

EXPORTNIX BOOL CalcDirInfo(LPCSTR _dir, size_t &size, size_t &num_files, size_t &num_dirs);
EXPORTNIX BOOL _RemoveDir(LPCSTR, MPROGRESS_ROUTINE func = 0, void *lpUserData = 0);
EXPORTNIX BOOL _ClearDir(LPCSTR, MPROGRESS_ROUTINE func = 0, void *lpUserData = 0);

inline BOOL DeleteFile(LPCSTR name)
{ return remove(name) != -1; }

inline BOOL RenameFile(LPCSTR oldname, LPCSTR newname)
{ return rename(oldname, newname) != -1; }

inline BOOL RenameDirectory(LPCSTR oldname, LPCSTR newname)
{ return rename(oldname, newname) != -1; }

inline BOOL RemoveDirectory(LPCSTR d)
{ return _RemoveDir(d); }

#define SetCurrentDirectory(name) chdir(name)
EXPORTNIX DWORD GetCurrentDirectory(int sz, LPSTR szCurDir);
EXPORTNIX int GetDriveType(char *buff);
EXPORTNIX int GetLogicalDriveStrings(MVector<TString> &path);
EXPORTNIX int GetLogicalDriveStrings(int len, char *buff);

EXPORTNIX BOOL GetTempFileName(const char *dir, const char *prfx, int lenbuff, char *res);
EXPORTNIX BOOL GetTempPath(int len, char *res);
EXPORTNIX void _splitpath(const char *str, char *_dr, char *_pa, char *_na, char *_ex);
EXPORTNIX void _makepath(char *all, const char *dr, const char *pa, const char *na, const char *ex);

EXPORTNIX void SetSystemTime(SYSTEMTIME *nt);
EXPORTNIX int CompareFileTime(FILETIME *lpFileTime1, FILETIME *lpFileTime2);
EXPORTNIX void GetSystemTime(SYSTEMTIME *);
EXPORTNIX bool GetWIN32_FIND_DATA(const char *file, WIN32_FIND_DATAA *ret);

inline DWORD_PTR GetCurrentTime()
{ return GetTickCount(); }

#pragma pack (push)
#pragma pack (4)
EXPORTNIX struct TaskInfo {
    TString UserName;
    TString PathExe;
    TString WorkDir;
    TString CmdLine;
    MVector<TString> env;
    pid_t pid; //%d      (1) The process ID.
    TString Name; // %s     (2) The filename of the executable, in
    //                   parentheses.  This is visible whether or not the
    //                 executable is swapped out.
    char state; // %c    (3) One character from the string "RSDZTW" where R
    //            is running, S is sleeping in an interruptible
    //          wait, D is waiting in uninterruptible disk sleep,
    //        Z is zombie, T is traced or stopped (on a signal),
    //          and W is paging.
    pid_t ppid; // %d     (4) The PID of the parent.
    pid_t pgrp; // %d     (5) The process group ID of the process.
    int session; // %d  (6) The session ID of the process.

    int tty_nr; // %d   (7) The controlling terminal of the process.  (The
    //              minor device number is contained in the
    //              combination of bits 31 to 20 and 7 to 0; the major
    //              device number is in bits 15 to 8.)

    int tpgid; //%d    (8) The ID of the foreground process group of the
    //            controlling terminal of the process.

    UINT flags; // %u (%lu before Linux 2.6.22)
    //            (9) The kernel flags word of the process.  For bit
    //          meanings, see the PF_* defines in the Linux kernel
    //        source file include/linux/sched.h.  Details depend
    //      on the kernel version.

    unsigned int minflt; // %lu  (10) The number of minor faults the process has
    //    made which have not required loading a memory page
    //   from disk.

    unsigned int cminflt; //%lu (11) The number of minor faults that the process's
    //   waited-for children have made.

    unsigned int majflt; //%lu;  (12) The number of major faults the process has
    // made which have required loading a memory page
    // from disk.

    unsigned int cmajflt; // %lu (13) The number of major faults that the process's
    //   waited-for children have made.

    unsigned int utime; // %lu   (14) Amount of time that this process has been
    //   scheduled in user mode, measured in clock ticks
    // (divide by sysconf(_SC_CLK_TCK)).  This includes
    // guest time, guest_time (time spent running a
    //virtual CPU, see below), so that applications that
    //are not aware of the guest time field do not lose
    //that time from their calculations.

    unsigned int stime; // %lu   (15) Amount of time that this process has been
    //    scheduled in kernel mode, measured in clock ticks
    //    (divide by sysconf(_SC_CLK_TCK)).

    int cutime; // %ld;  (16) Amount of time that this process's waited-for
    //         children have been scheduled in user mode,
    //         measured in clock ticks (divide by
    //         sysconf(_SC_CLK_TCK)).  (See also times(2).)  This
    //         includes guest time, cguest_time (time spent
    //         running a virtual CPU, see below).

    unsigned int cstime; // %ld  (17) Amount of time that this process's waited-for
    //    children have been scheduled in kernel mode,
    //    measured in clock ticks (divide by
    //    sysconf(_SC_CLK_TCK)).

    int priority; // %ld   (18) (Explanation for Linux 2.6) For processes
    //    running a real-time scheduling policy (policy
    //    below; see sched_setscheduler(2)), this is the
    //    negated scheduling priority, minus one; that is, a
    //    number in the range -2 to -100, corresponding to
    //    real-time priorities 1 to 99.  For processes
    //    running under a non-real-time scheduling policy,
    //    this is the raw nice value (setpriority(2)) as
    //    represented in the kernel.  The kernel stores nice
    //    values as numbers in the range 0 (high) to 39
    //    (low), corresponding to the user-visible nice
    //   range of -20 to 19.


    int nice; // %ld    (19) The nice value (see setpriority(2)), a value
    //               in the range 19 (low priority) to -20 (high
    //               priority).

    int num_threads; // %ld  (20) Number of threads in this process (since
    //        Linux 2.6).  Before kernel 2.6, this field was
    //        hard coded to 0 as a placeholder for an earlier
    //        removed field.

    int itrealvalue; // %ld (21) The time in jiffies before the next SIGALRM
    //        is sent to the process due to an interval timer.
    //        Since kernel 2.6.17, this field is no longer
    //        maintained, and is hard coded as 0.

    unsigned long long starttime; // %llu (was %lu before Linux 2.6)
    //     (22) The time the process started after system
    //     boot.  In kernels before Linux 2.6, this value was
    //     expressed in jiffies.  Since Linux 2.6, the value
    //     is expressed in clock ticks (divide by
    //     sysconf(_SC_CLK_TCK)).

    int vsize; // %lu   (23) Virtual memory size in bytes.

    int rss; //  %ld     (24) Resident Set Size: number of pages the
    //                process has in real memory.  This is just the
    //                pages which count toward text, data, or stack
    //                space.  This does not include pages which have not
    //                been demand-loaded in, or which are swapped out.

    unsigned int rsslim; // %lu  (25) Current soft limit in bytes on the rss of the
    //             process; see the description of RLIMIT_RSS in
    //            getrlimit(2).
    TaskInfo()
    {
        pid = 0;
        state = 0;
        ppid = pgrp = 0;
        session = tty_nr = tpgid = 0;
        flags = 0;
        minflt = cminflt = majflt = cmajflt = utime = stime = cstime = 0;
        starttime = 0;
        cutime = priority = nice = num_threads = itrealvalue = vsize = rss = 0;
        rsslim = 0;
    }

    BOOL IsValid()
    { return pid != 0; }

    BOOL Init(int _pid);

    void Print();
};
EXPORTNIX int GetTaskList(MVector<TaskInfo> &res);


EXPORTNIX class COleDateTime {
public:
    enum DateTimeStatus {
        valid = 0,
        invalid = 1,    // Invalid date (out of range, etc.)
        null = 2       // Literally has no value
    };
    enum DATE {
        DATE_MIN = -657434,
        DATE_MAX = 2958465
    };

    double m_dt;
    DateTimeStatus m_status;

    COleDateTime();

    static COleDateTime GetCurrentTime();

    COleDateTime(int nYear, int nMonth, int nDay, int nHour = 0, int nMin = 0, int nSec = 0,
                 int iMili = 0)
    {
        SetDate(nYear, nMonth, nDay, nHour, nMin, nSec);

    }

    COleDateTime(const struct tm &dt)
    {
        SetDate(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
    }

    COleDateTime(SYSTEMTIME &st)
    { Convert(st, &m_dt); }

    COleDateTime(FILETIME *time)
    { SetDate(time); }

    void SetDate(FILETIME *time);

    void GetDate(FILETIME *m);

    void GetDate(SYSTEMTIME &st)
    { Convert(m_dt, st); }

    TString Format()
    {
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        TString s;
        s.Format("%d/%d/%d %d:%d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
        return s;
    }

#ifndef _CONSOLE_PROG_
#ifdef __WXGTK__
    void GetDate( wxDateTime &dt)
    {
        struct tm t;
        GetDate(t);
        dt.Set(t);
    }
    void SetDate(wxDateTime &dt)
    {
        SYSTEMTIME st;
        st.wYear = dt.GetYear();
        st.wMonth = dt.GetMonth() + 1;
        st.wDayOfWeek = dt.GetWeekDay();
        st.wDay  = dt.GetDay();
        st.wHour  = dt.GetHour();
        st.wMinute  = dt.GetMinute();
        st.wSecond  = dt.GetSecond();
        st.wMilliseconds  = dt.GetMillisecond();
        Convert(st, &m_dt);
    }
#endif
#endif

    void GetDate(struct tm &dt)
    {
        memset(&dt, 0, sizeof(struct tm));
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        dt.tm_year = st.wYear - 1900;
        dt.tm_mon = st.wMonth - 1;
        dt.tm_mday = st.wDay;
        dt.tm_hour = st.wHour;
        dt.tm_min = st.wMinute;
        dt.tm_sec = st.wSecond;
    }

    void SetDate(const struct tm &dt)
    {
        SetDate(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
    }

    void SetDate(int nYear, int nMonth, int nDay, int nHour = 0, int nMin = 0, int nSec = 0)
    {
        SYSTEMTIME st;
        m_status = valid;
        bzero(&st, sizeof(SYSTEMTIME));
        st.wYear = (WORD) nYear;
        st.wMonth = (WORD) nMonth;
        st.wDay = (WORD) nDay;
        st.wHour = (WORD) nHour;
        st.wMinute = (WORD) nMin;
        st.wSecond = (WORD) nSec;
        Convert(st, &m_dt);

    }

    int GetYear()
    {
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        return st.wYear;
    };

    int GetDayOfWeek()
    {
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        return st.wDayOfWeek;
    };

    // Month of year (1 = January)
    int GetMonth()
    {
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        return st.wMonth;
    };

    int GetDay()
    {
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        return st.wDay;
    };

    int GetHour()
    {
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        return st.wHour;
    };

    int GetMinute()
    {
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        return st.wMinute;
    };

    int GetSecond()
    {
        SYSTEMTIME st;
        m_status = Convert(m_dt, st);
        return st.wSecond;
    };

    DateTimeStatus GetSysTime(SYSTEMTIME &st)
    { return m_status = Convert(m_dt, st); }

    DateTimeStatus GetAsSystemTime(SYSTEMTIME &st)
    { return GetSysTime(st); }

    static void Convert(SYSTEMTIME &st, double *pDateOut);

    static DateTimeStatus Convert(double dateIn, SYSTEMTIME &st);

protected:
    static int DateFromJulian(int dateIn);

    static int JulianFromDMY(USHORT year, USHORT month, USHORT day);

    static int JulianFromDate(int dateIn);

    static void DMYFromJulian(int jd, USHORT *year, USHORT *month, USHORT *day);

    static DateTimeStatus RollUdate(SYSTEMTIME &st);
};
#define CTime COleDateTime
EXPORTNIX struct CFileStatus : public stat {
    COleDateTime FileDateTime(const char *p = 0);

    int Size(const char *);


};

#define AfxThrowFileException(a) (throw a);

template<class _TYPE_>
class AFXArray : public MArray<_TYPE_> {
public:
    AFXArray()
    {}

    int GetSize() const
    { return MArray<_TYPE_>::Len; }

    int GetUpperBound() const
    { return MArray<_TYPE_>::Len - 1; }

    void SetSize(int nNewSize, int nGrowBy = -1)
    { MArray<_TYPE_>::SetLen(nNewSize); }

    void FreeExtra()
    {}

    void RemoveAll()
    { MArray<_TYPE_>::Clear(); }

    // Accessing elements
    _TYPE_ GetAt(int nIndex)
    {
        if (nIndex >= MArray<_TYPE_>::Len) throw 1;
        return MArray<_TYPE_>::ptr[nIndex];
    }

    void SetAt(int nIndex, _TYPE_ newElement)
    {
        if (nIndex >= MArray<_TYPE_>::Len) return;
        MArray<_TYPE_>::ptr[nIndex] = newElement;
    }

    _TYPE_ &ElementAt(int nIndex)
    { return MArray<_TYPE_>::ptr[nIndex]; }

    _TYPE_ *GetData() const
    { return MArray<_TYPE_>::ptr; }

    // Potentially growing the array
    void SetAtGrow(int nIndex, _TYPE_ newElement)
    {
        if (nIndex >= MArray<_TYPE_>::Len)
            MArray<_TYPE_>::SetLen(nIndex + 1);
        MArray<_TYPE_>::ptr[nIndex] = newElement;

    }

    int Add(_TYPE_ newElement)
    {
        MArray<_TYPE_>::Add() = newElement;
        return MArray<_TYPE_>::Len - 1;
    }

    int Append(const AFXArray &src)
    {
        MArray<_TYPE_>::operator+=(src);
        return GetSize();
    }

    void Copy(const AFXArray &src)
    { MArray<_TYPE_>::operator=(src); }

    // overloaded operator helpers
    _TYPE_ &operator[](int nIndex) const
    { return MArray<_TYPE_>::ptr[nIndex]; }

    // Operations that move elements around
    void InsertAt(int nIndex, _TYPE_ newElement, int nCount = 1)
    { MArray<_TYPE_>::Insert(nIndex, newElement); }

    void RemoveAt(int nIndex, int nCount = 1)
    { MArray<_TYPE_>::Remove(nIndex, nCount); }

    void InsertAt(int nStartIndex, AFXArray *pNewArray)
    { MArray<_TYPE_>::Insert(nStartIndex, pNewArray->ptr, pNewArray->Len); }

public:
    virtual  ~AFXArray()
    {}

private :


};

template<class _TYPE_, class TMP = int>
class AFXVector : public MVector<_TYPE_> {
public:
    AFXVector()
    {}

    AFXVector(const AFXVector &v) : MVector<_TYPE_>(v)
    {}

    ~AFXVector()
    {}

    int GetSize() const
    { return (int) MVector<_TYPE_>::Len; }

    int GetUpperBound()
    { return (int) MVector<_TYPE_>::GetLen() - 1; }

    void SetSize(int nNewSize, int nGrowBy = -1)
    { MVector<_TYPE_>::SetLen(nNewSize); }

    void FreeExtra()
    {}

    void RemoveAll()
    { MVector<_TYPE_>::Clear(); }

    // Accessing elements
    _TYPE_ GetAt(int nIndex)
    {
        if (nIndex >= MVector<_TYPE_>::Len) throw 1;
        return MVector<_TYPE_>::ptr[nIndex];
    }

    void SetAt(int nIndex, _TYPE_ newElement)
    {
        if (nIndex >= MVector<_TYPE_>::Len) return;
        MVector<_TYPE_>::ptr[nIndex] = newElement;
    }

    _TYPE_ &ElementAt(int nIndex)
    { return MVector<_TYPE_>::ptr[nIndex]; }

    _TYPE_ *GetData()
    { return MVector<_TYPE_>::ptr; }

    // Potentially growing the array
    void SetAtGrow(int nIndex, _TYPE_ newElement)
    {
        if (nIndex >= MVector<_TYPE_>::Len)
            MVector<_TYPE_>::SetLen(nIndex + 1);
        MVector<_TYPE_>::ptr[nIndex] = newElement;

    }

    int Add(_TYPE_ newElement)
    {
        MVector<_TYPE_>::Add() = newElement;
        return MVector<_TYPE_>::Len - 1;
    }

    int Append(const AFXVector &src)
    {
        MVector<_TYPE_>::operator+=(src);
        return GetSize();
    }

    void Copy(const AFXVector &src)
    { MVector<_TYPE_>::operator=(src); }

    // overloaded operator helpers
    _TYPE_ &operator[](int nIndex) const
    { return MVector<_TYPE_>::ptr[nIndex]; }

    // Operations that move elements around
    void InsertAt(int nIndex, _TYPE_ newElement, int nCount = 1)
    { MVector<_TYPE_>::Insert(nIndex, newElement); }

    void RemoveAt(int nIndex, int nCount = 1)
    { MVector<_TYPE_>::Remove(nIndex, nCount); }

    void InsertAt(int nStartIndex, AFXVector *pNewArray)
    { MVector<_TYPE_>::Insert(nStartIndex, pNewArray->ptr, pNewArray->Len); }


};


EXPORTNIX class _PEXPORT AFXFile : public MFILE {

public:
// Flag values
    enum OpenFlags {
        modeRead = F_READONLY,
        modeWrite = F_WRITEONLY,
        modeReadWrite = F_READWRITE,
        modeCreate = F_CREATE,
        shareDenyNone = 0
    };

    enum Attribute {
        normal = 0x00,
        readOnly = 0x01,
        hidden = 0x02,
        system = 0x04,
        volume = 0x08,
        directory = 0x10,
        archive = 0x20
    };

    enum SeekPosition {
        begin = 0x0, current = 0x1, end = 0x2
    };

    enum {
        hFileNull = -1
    };

// Constructors
    AFXFile()
    {}

    AFXFile(int hFile);

    AFXFile(LPCSTR lpszFileName, UINT nOpenFlags);

    virtual DWORD GetPosition()
    { return (DWORD) MFILE::GetSeek(); }

    virtual TString GetFileName() const
    { return TString(name); }

    virtual TString GetFileTitle() const
    { return title; }

    virtual TString GetFilePath() const
    {
        TString s(name);
        int i1 = s.ReverseFind(FD);
        int i2 = s.ReverseFind('\\');
        i1 = max(i1, i2);
        if (i1 > 0)
            s[i1 + 1] = 0;
        return s;
    }

    static bool GetStatus(const char *, CFileStatus &st);

    virtual void SetFilePath(LPCSTR lpszFileName);

// Operations
    virtual BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags, CFileException *p = 0);

    static void Rename(LPCSTR lpszOldName,
                       LPCSTR lpszNewName);

    static void Remove(LPCSTR lpszFileName);

    virtual DWORD SeekToEnd()
    { return (DWORD)MFILE::SetSeek(GetSize()); }

    virtual void SeekToBegin()
    { MFILE::SetSeek(0); }

    DWORD ReadHuge(void *lpBuffer, DWORD dwCount)
    { return Read(lpBuffer, dwCount); }

    BOOL WriteHuge(const void *lpBuffer, DWORD dwCount)
    { return Write(lpBuffer, dwCount); }

    virtual LONG Seek(LONG lOff, UINT nFrom);

    virtual void SetLength(DWORD dwNewLen)
    { SetSize(dwNewLen); }

    virtual DWORD GetLength()
    { return (DWORD) GetSize(); }

    virtual UINT Read(void *lpBuf, UINT nCount);

    virtual BOOL Write(const void *lpBuf, UINT nCount);

    virtual void LockRange(DWORD dwPos, DWORD dwCount)
    {};

    virtual void UnlockRange(DWORD dwPos, DWORD dwCount)
    {}


    virtual void Abort()
    { Close(); }

// Implementation
public:
    virtual ~AFXFile()
    { Close(); }

    TString title;
};

EXPORTNIX class _PEXPORT AFXMemFile : public AFXFile {

public:
// Constructors
    AFXMemFile()
    { pos = 0; }

    AFXMemFile(BYTE *p, int size)
    {
        if (p) buff.Copy((char *) p, size); else buff.SetLen(size);
        pos = 0;
    }

    void Attach(BYTE *p, int size, int fadd = 128)
    {
        buff.SetAddedRealSize(fadd);
        if (p) buff.Attach((char *) p, size, true);
        pos = 0;
    }

    virtual BYTE *Detach()
    {
        BYTE *p = (BYTE *) buff.GetPtr();
        buff.Detach();
        pos = 0;
        return p;
    }

    virtual DWORD GetPosition()
    { return pos; }

    virtual DWORD SeekToEnd()
    {
        pos = GetSize();
        return pos;
    }

    virtual void SeekToBegin()
    { pos = 0; }

    virtual off_t GetSize()
    { return buff.GetLen(); }

    virtual bool SetSize(off_t size)
    {
        buff.SetLen((int) size);
        return true;
    }

    virtual LONG Seek(LONG lOff, UINT nFrom)
    {
        switch (nFrom)
        {
            case begin:
                pos = lOff;
                break;
            case current:
                pos += lOff;
                break;
            case end:
                pos = buff.GetLen() + lOff;
                break;
        }

        if (pos >= buff.GetLen())
            buff.SetLen(pos + 1);
        return (LONG) pos;
    }

    virtual void SetLength(DWORD dwNewLen)
    { buff.SetLen(dwNewLen); }

    virtual DWORD GetLength()
    { return (DWORD) buff.GetLen(); }

    virtual UINT Read(void *lpBuff, UINT nCount)
    {
        if (pos + (int) nCount > buff.GetLen())
        {
            nCount = buff.GetLen() - pos;
        }
        if (nCount)
            memcpy(lpBuff, buff.GetPtr() + pos, nCount);
        pos += nCount;
        return nCount;
    }

    virtual BOOL Write(const void *lpBuff, UINT nCount)
    {
        if (pos + (int) nCount > buff.GetLen())
            buff.SetLen(pos + (int) nCount);
        memcpy(buff.GetPtr() + pos, lpBuff, nCount);
        pos += nCount;
        return TRUE;
    }

    virtual void Abort()
    { Close(); }

    virtual bool Close()
    {
        buff.SetLen(0);
        return true;
    }

// Implementation
public:
    virtual ~AFXMemFile()
    { Close(); }

protected :
    int pos;
public:
    MArray<char> buff;
};

EXPORTNIX class _PEXPORT AFXArchive {
public:
    enum Mode {
        store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4
    };

    AFXArchive(AFXFile *pFile, UINT nMode, int nBufSize = 4096, void *lpBuf = NULL);

    ~AFXArchive();

    BOOL IsLoading() const
    { return (m_nMode & load) != 0; }

    BOOL IsStoring() const
    { return (m_nMode & load) == 0; }

    AFXFile *GetFile() const
    { return m_pFile; }

    UINT Read(void *lpBuf, UINT nMax);

    UINT Write(const void *lpBuf, UINT nMax);

    void Flush();

    void Close();

    void Abort();

    void WriteString(LPCSTR lpsz);

    LPSTR ReadString(LPSTR lpsz, UINT nMax);

    BOOL ReadString(TString &rString);

public:
#ifndef __PLUK_LINUX__
    AFXArchive& operator<<(int i) { return operator<<((LONG)i); }
    AFXArchive& operator<<(unsigned int u) { return operator<<((LONG)u); }
#endif

    AFXArchive &operator<<(short w)
    { return operator<<((WORD) w); }

    AFXArchive &operator<<(char ch)
    { return operator<<((BYTE) ch); }

    AFXArchive &operator<<(BYTE by);

    AFXArchive &operator<<(WORD w);

    AFXArchive &operator<<(int l);

    AFXArchive &operator<<(DWORD dw);

    AFXArchive &operator<<(float f);

    AFXArchive &operator<<(double d);

    AFXArchive &operator<<(LPCSTR d);


#ifndef __PLUK_LINUX__
    AFXArchive& operator>>(int& i) { return operator>>((LONG&)i); }
    AFXArchive& operator>>(unsigned int &u) { return operator>>((LONG&)u); }
#endif

    AFXArchive &operator>>(short &w)
    { return operator>>((WORD &) w); }

    AFXArchive &operator>>(char &ch)
    { return operator>>((BYTE &) ch); }

    AFXArchive &operator>>(BYTE &by);

    AFXArchive &operator>>(WORD &w);

    AFXArchive &operator>>(DWORD &dw);

    AFXArchive &operator>>(int &l);

    AFXArchive &operator>>(float &f);

    AFXArchive &operator>>(double &d);

    AFXArchive &operator>>(TString &d);


public:
    int m_nMode;
    AFXFile *m_pFile;
    TString m_strFileName;

};

struct AFXPoint : public IPoint {

    AFXPoint()
    { x = y = 0; }

    AFXPoint(int initX, int initY)
    {
        x = initX;
        y = initY;
    }

    AFXPoint(POINT initPt)
    { *(POINT *) this = initPt; }

    AFXPoint(DWORD dwPoint)
    {
        x = (short) LOWORD(dwPoint);
        y = (short) HIWORD(dwPoint);
    }

    void Set(int _x, int _y)
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

    //operator POINT ()  const { return *this; }
    operator AFXPoint *()
    { return this; }

    BOOL operator==(POINT point) const
    { return (x == point.x && y == point.y); }

    BOOL operator!=(POINT point) const
    { return (x != point.x || y != point.y); }

    void operator+=(POINT point)
    {
        x += point.x;
        y += point.y;
    }

    void operator-=(POINT point)
    {
        x -= point.x;
        y -= point.y;
    }

    AFXPoint operator-() const
    { return AFXPoint(-x, -y); }

    AFXPoint operator+(POINT point) const
    { return AFXPoint(x + point.x, y + point.y); }
};

class AFXRect : public IRect {
public:
    AFXRect()
    { memset(this, 0, sizeof(RECT)); }

    AFXRect(int l, int t, int r, int b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    AFXRect(const RECT &rc)
    {
        left = rc.left;
        top = rc.top;
        right = rc.right;
        bottom = rc.bottom;
    }

    AFXRect(const POINT &lt, const POINT &rb)
    {
        left = lt.x;
        top = lt.y;
        right = rb.x;
        bottom = rb.y;
    }

    AFXRect &operator=(const RECT &rc)
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

    AFXRect(LPCRECT lpSrcRect)
    { memcpy(this, lpSrcRect, sizeof(RECT)); }

    AFXRect(const POINT &topLeft, const SIZE &sz)
    {
        left = topLeft.x;
        top = topLeft.y;
        right = left + sz.cx;
        bottom = top + sz.cy;
    }

    //operator RECT() const { return *(RECT *)this; }
    AFXPoint &TopLeft()
    { return *((AFXPoint *) this); }

    AFXPoint &BottomRight()
    { return *((AFXPoint *) this + 1); }

    AFXPoint CenterPoint() const
    { return AFXPoint((left + right) / 2, (top + bottom) / 2); }

    operator AFXRect *()
    { return this; }

    void SwapLeftRight()
    { SwapLeftRight(this); }

    void SwapLeftRight(LPRECT lpRect)
    {
        int temp = lpRect->left;
        lpRect->left = lpRect->right;
        lpRect->right = temp;
    }

    BOOL IsRectEmpty() const
    { return (left == 0 && right == 0 && top == 0 && bottom == 0); }

    BOOL IsRectNull() const
    { return (left == 0 && right == 0 && top == 0 && bottom == 0); }

    BOOL PtInRect(POINT p)
    { return p.x >= left && p.y >= top && p.x <= right && p.y <= bottom; };

    BOOL PtInRect(POINT *p)
    { return p->x >= left && p->y >= top && p->x <= right && p->y <= bottom; };

    AFXRect &SetRect(int l, int t, int r, int b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
        return *this;
    }

    AFXRect &SetRect(POINT topLeft, POINT bottomRight)
    {
        left = topLeft.x;
        top = topLeft.y;
        right = bottomRight.x;
        bottom = bottomRight.y;
        return *this;
    }

    void SetRectEmpty()
    { left = top = right = bottom = 0; }

    void CopyRect(LPCRECT lpSrcRect)
    { memcpy(this, lpSrcRect, sizeof(RECT)); }

    BOOL EqualRect(LPCRECT lpRect)
    {
        return lpRect->left == left && lpRect->right == right && lpRect->top == top &&
               lpRect->bottom == bottom;
    }

    void InflateRect(int x, int y)
    {
        left -= x;
        top -= y;
        right += x;
        bottom += y;
    }

    void InflateRect(LPCRECT lpRect)
    {
        left -= lpRect->left;
        top -= lpRect->top;
        right += lpRect->right;
        bottom += lpRect->bottom;
    }

    void InflateRect(int l, int t, int r, int b)
    {
        left -= l;
        top -= t;
        right += r;
        bottom += b;
    }

    void DeflateRect(int x, int y)
    {
        left += x;
        top += y;
        right -= x;
        bottom -= y;
    }

    void DeflateRect(LPCRECT lpRect)
    {
        left += lpRect->left;
        top += lpRect->top;
        right -= lpRect->right;
        bottom -= lpRect->bottom;
    }

    void DeflateRect(int l, int t, int r, int b)
    {
        left += l;
        top += t;
        right -= r;
        bottom -= b;
    }

#ifndef _CONSOLE_PROG_
#ifdef __WXGTK__
    AFXRect &operator = (const wxRect &rc)  { left = rc.x; top = rc.y; right = rc.width + rc.x; bottom = rc.height + rc.y; return *this; }  
#endif
#endif

    void OffsetRect(int x, int y)
    {
        left += x;
        top += y;
        right += x;
        bottom += y;
    }

    void OffsetRect(POINT point)
    {
        left += point.x;
        top += point.y;
        right += point.x;
        bottom += point.y;
    }

    BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2)
    {

        left = max(lpRect1->left, lpRect2->left);
        top = max(lpRect1->top, lpRect2->top);
        right = min(lpRect1->right, lpRect2->right);
        bottom = min(lpRect1->bottom, lpRect2->bottom);
        return left <= right && top <= bottom ? TRUE : FALSE;
    }

    void NormalizeRect()
    { RECT::Normalize(); }

    BOOL UnionRect(LPCRECT lpRect1, LPCRECT lpRect2)
    {
        SetRect(min(lpRect1->left, lpRect2->left), min(lpRect1->top, lpRect2->top),
                max(lpRect1->right, lpRect2->right), max(lpRect1->bottom, lpRect2->bottom));
        return !IsRectNull();

    }

    AFXRect MulDiv(int nMultiplier, int nDivisor) const
    {
        AFXRect r(
                ::MulDiv(left, nMultiplier, nDivisor),
                ::MulDiv(top, nMultiplier, nDivisor),
                ::MulDiv(right, nMultiplier, nDivisor),
                ::MulDiv(bottom, nMultiplier, nDivisor));
        return r;
    }

    AFXRect &operator|=(const AFXRect &r)
    {
        left = min(left, r.left);
        top = min(top, r.top),
                right = max(right, r.right);
        bottom = max(bottom, r.bottom);
        return *this;
    }

    AFXRect &operator&=(const AFXRect &r)
    {
        left = max(left, r.left);
        top = max(top, r.top);
        right = min(right, r.right);
        bottom = min(bottom, r.bottom);
        if (left > right || top > bottom)
            SetRect(0, 0, 0, 0);
        return *this;
    }

    bool Minus(const AFXRect &a, MArray<AFXRect> &ret)
    {
        int len = ret.GetLen();
        AFXRect newr;
        newr.left = max(a.left, left);
        newr.top = max(a.top, top);
        newr.right = min(a.right, right);
        newr.bottom = min(a.bottom, bottom);
        if (newr.left <= newr.right && newr.top <= newr.bottom)
        {
            if (newr.left > left)
                ret.Add().SetRect(left, top, newr.left, bottom);
            if (newr.right < right)
                ret.Add().SetRect(newr.right, top, right, bottom);
            if (newr.top > top)
                ret.Add().SetRect(newr.left, top, newr.right, newr.top);
            if (newr.bottom < bottom)
                ret.Add().SetRect(newr.left, newr.bottom, newr.right, bottom);
        }
        return ret.GetLen() == len;

    };

};

#ifndef POSITION
struct __POSITION {
};
typedef __POSITION *POSITION;
#endif

inline void InflateRect(RECT *rc, int x, int y)
{ ((AFXRect *) rc)->InflateRect(x, y); }

template<class _TYPE>
class AFXPtrList : public CObject {
protected:
    struct CNode : public __POSITION {
        CNode *pNext;
        CNode *pPrev;
        _TYPE data;

        CNode(CNode *a = 0)
        {
            pPrev = pNext = 0;
            data = 0;
            if (a)
            {
                pPrev = a;
                a->pNext = this;
            }
        }

        void Clear()
        {
            pPrev = pNext;
            data = 0;
        }

        void InsertBefore(CNode *a)
        {
            pPrev = pNext = 0;
            if (a)
            {
                pPrev = a->pPrev;
                a->pPrev = this;
                pNext = a;
            }
        }

        void InsertAfter(CNode *a)
        {
            pPrev = pNext = 0;
            if (a)
            {
                pPrev = a;
                pNext = a->pNext;
                a->pNext = this;
            }
        }

        ~CNode()
        {
            if (pPrev)
                pPrev->pNext = pNext;
            if (pNext)
                pNext->pPrev = pPrev;
        }

        void Reset()
        {
            if (pPrev)
                pPrev->pNext = pNext;
            if (pNext)
                pNext->pPrev = pPrev;
            pNext = pPrev = 0;
            data = 0;
        }
    };

public:
    AFXPtrList(int n = -1)
    {
        mAlloc._Max_Alloc_ = n;
        m_pNodeHead = m_pNodeTail = 0;
        m_nCount = 0;
    }

    virtual ~AFXPtrList()
    { mAlloc.Clear(); }

    int GetCount() const
    { return m_nCount; }

    int GetSize() const
    { return m_nCount; }

    BOOL IsEmpty()
    { return m_nCount == 0; }

    _TYPE GetHead()
    { return m_nCount ? m_pNodeHead->data : 0; }

    _TYPE GetTail()
    { return m_nCount ? m_pNodeTail->data : 0; }

    POSITION GetHeadPosition() const
    { return m_pNodeHead; }

    POSITION GetTailPosition() const
    { return m_pNodeTail; }

    _TYPE RemoveHead()
    {
        _TYPE ret = m_pNodeHead ? m_pNodeHead->data : 0;
        RemoveAt(m_pNodeHead);
        return ret;
    }

    _TYPE RemoveTail()
    {
        _TYPE ret = m_pNodeTail ? m_pNodeTail->data : 0;
        RemoveAt(m_pNodeTail);
        return ret;
    }

    POSITION AddHead(_TYPE newElement);

    POSITION AddTail(_TYPE newElement);

    void AddHead(AFXPtrList *pNewList);

    void AddTail(AFXPtrList *pNewList);

    void RemoveAll();

    _TYPE GetNext(POSITION &rPosition);

    _TYPE GetPrev(POSITION &rPosition);

    _TYPE GetAt(POSITION rPosition);

    void SetAt(POSITION rPosition, _TYPE newElement);

    void RemoveAt(POSITION rPosition);

    POSITION InsertBefore(POSITION rPosition, _TYPE newElement);

    POSITION InsertAfter(POSITION rPosition, _TYPE newElement);

    POSITION Find(_TYPE searchValue, POSITION startAfter = NULL);

    POSITION FindIndex(int nIndex);

protected:

    CNode *m_pNodeHead;
    CNode *m_pNodeTail;
    int m_nCount;
    Allocator<CNode> mAlloc;
};

template<class _TYPE>
POSITION AFXPtrList<_TYPE>::AddHead(_TYPE newElement)
{
    CNode *n = mAlloc.Alloc(false);
    n->InsertBefore(m_pNodeHead);
    n->data = newElement;
    m_pNodeHead = n;
    if (!m_pNodeTail) m_pNodeTail = n;
    ++m_nCount;
    return n;
}

template<class _TYPE>
POSITION AFXPtrList<_TYPE>::AddTail(_TYPE newElement)
{
    CNode *n = mAlloc.Alloc(false);
    n->InsertAfter(m_pNodeTail);
    n->data = newElement;
    m_pNodeTail = n;
    if (!m_pNodeHead)
        m_pNodeHead = n;
    ++m_nCount;
    return n;
}

template<class _TYPE>
void AFXPtrList<_TYPE>::AddHead(AFXPtrList<_TYPE> *pNewList)
{
    CNode *n = pNewList->m_pNodeTail;
    while (n)
    {
        AddHead(n->data);
        n = n->pPrev;
    }
}

template<class _TYPE>
void AFXPtrList<_TYPE>::AddTail(AFXPtrList<_TYPE> *pNewList)
{
    CNode *n = pNewList->m_pNodeHead;
    while (n)
    {
        AddTail(n->data);
        n = n->pNext;
    }
}

template<class _TYPE>
void AFXPtrList<_TYPE>::RemoveAll()
{
    m_pNodeHead = m_pNodeTail = 0;
    m_nCount = 0;
    mAlloc.Clear();
}

template<class _TYPE>
_TYPE AFXPtrList<_TYPE>::GetNext(POSITION &rPosition)
{
    CNode *p = (CNode *) rPosition;
    if (p)
    {
        rPosition = p->pNext;
        return rPosition ? p->pNext->data : 0;
    }
    return 0;
}

template<class _TYPE>
_TYPE AFXPtrList<_TYPE>::GetPrev(POSITION &rPosition)
{
    CNode *p = (CNode *) rPosition;
    if (p)
    {
        rPosition = p->pPrev;
        return rPosition ? p->pPrev->data : 0;
    }
    return 0;
}

template<class _TYPE>
_TYPE AFXPtrList<_TYPE>::GetAt(POSITION rPosition)
{
    CNode *p = (CNode *) rPosition;
    return p ? p->data : 0;
}

template<class _TYPE>
void AFXPtrList<_TYPE>::SetAt(POSITION rPosition, _TYPE newElement)
{
    CNode *p = (CNode *) rPosition;
    if (p)
    {
        p->data = newElement;
    }
}

template<class _TYPE>
void AFXPtrList<_TYPE>::RemoveAt(POSITION rPosition)
{
    if (rPosition && m_nCount)
    {
        CNode *p = (CNode *) rPosition;
        if (p == m_pNodeTail)
            m_pNodeTail = p->pPrev;
        if (p == m_pNodeHead)
            m_pNodeHead = p->pNext;
        p->Reset();
        mAlloc.Free(p);
        --m_nCount;

    }
}

template<class _TYPE>
POSITION AFXPtrList<_TYPE>::InsertBefore(POSITION rPosition, _TYPE newElement)
{
    CNode *p = (CNode *) rPosition;
    if (!p || p == m_pNodeHead)
        return AddHead(newElement);
    CNode *n = mAlloc.Alloc(false);
    n->InsertBefore(p);
    n->data = newElement;
    ++m_nCount;
    return n;
}

template<class _TYPE>
POSITION AFXPtrList<_TYPE>::InsertAfter(POSITION rPosition, _TYPE newElement)
{
    CNode *p = (CNode *) rPosition;
    if (!p || p == m_pNodeTail)
        return AddTail(newElement);
    CNode *n = mAlloc.Alloc(false);
    n->InsertAfter(p);
    n->data = newElement;
    ++m_nCount;
    return n;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
template<class _TYPE>
POSITION AFXPtrList<_TYPE>::Find(_TYPE searchValue, POSITION startAfter)
{
    CNode *n = startAfter ? ((CNode *) startAfter)->pNext : m_pNodeHead;
    while (n)
    {
        if (n->data == searchValue)
            break;
        n = n->pNext;
    }
    return n;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
template<class _TYPE>
POSITION AFXPtrList<_TYPE>::FindIndex(int nIndex)
{

    if (nIndex >= m_nCount) return 0;
    CNode *n = m_pNodeHead;
    while (nIndex--)
        n = n->pNext;
    return n;
}

typedef AFXPtrList<void *> CPtrList;
typedef AFXPtrList<CObject *> CObList;
typedef AFXVector<TString, LPCSTR> CStringArray;
typedef AFXArray<WORD> CWordArray;
typedef AFXArray<DWORD> CDWordArray;
typedef AFXArray<void *> CPtrArray;
typedef AFXArray<BYTE> CByteArray;
#define  CArray AFXVector
typedef TString CString;
typedef AFXFile CFile;
typedef AFXArchive CArchive;
typedef AFXMemFile CMemFile;
typedef AFXPoint CPoint;
typedef AFXRect CRect;

#pragma pack (pop)

#ifndef _CONSOLE_PROG_
#include "shoperation.h"
#endif
#endif
