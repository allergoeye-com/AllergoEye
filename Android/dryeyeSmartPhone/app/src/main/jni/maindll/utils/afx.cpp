#include "stdafx.h"
#include "afx.h"
#include "base_error.h"
#include "_ffile.h"

void TaskInfo::Print()
{
    printf("\r\n*****************************************************\r\n");
    printf("Name %s\r\n", Name.GetPtr());
    printf("CmdLine %s\r\n", CmdLine.GetPtr());
    printf("User %s\r\n", UserName.GetPtr());
    printf("FullPath %s\r\n", PathExe.GetPtr());
    printf("Work Dir %s\r\n", WorkDir.GetPtr());

    //  for (int i = 0, n = env.GetLen(); i < n; ++i)
//	printf("%s\r\n", env[i].GetPtr());
    printf("State : %c\r\n", state);
    printf("pid %d\r\n", pid); // %d (4)
    printf("ppid %d\r\n", ppid); // %d (4)
    printf("pgrp %d\r\n", pgrp); // %d     (5)
    printf("session %d\r\n", session); // %d  (6)
    printf("tty_nr %d\r\n", tty_nr); // %d   (7)
    printf("tpgid %d\r\n", tpgid); //%d    (8)
    printf("flags %d\r\n", flags); // %u (9)
    printf("minflt %u\r\n", minflt); // %u  (10)
    printf("cminflt %u\r\n", cminflt); //%u (11)
    printf("majflt %u\r\n", majflt); // %u  (12)
    printf("cmajflt %u\r\n", cmajflt); // %u (13)
    printf("utime %u\r\n", utime); // %u   (14)
    printf("stime %u\r\n", stime); // %u   (15)
    printf("cutime %d\r\n", cutime); // %d;  (16)
    printf("cstime %d\r\n", cstime); // %d  (17)
    printf("priority %d\r\n", priority); // %d   (18)
    printf("nice %d\r\n", nice); // %d    (19)
    printf("num_threads %d\r\n", num_threads); // %d  (20)
    printf("itrealvalue %d\r\n", itrealvalue); // %d (21)
    printf("starttime %llu\r\n", (unsigned long long) starttime); // %llu (22)
    printf("vsize %u\r\n", vsize); // %u   (23)
    printf("rss %d\r\n", rss); //  %ld     (24)
    printf("rsslim %u\r\n", rsslim); // %u  (25)


    printf("/r/n*****************************************************\r\n");
}

BOOL TaskInfo::Init(int t)
{
    pid = 0;
    TString path;
    int len;
    TString buff;
    env.Clear();
    Name.Clear();
    UserName.Clear();
    PathExe.Clear();
    WorkDir.Clear();
    CmdLine.Clear();
    MVector<TString> env;
    len = 5000;
    path.Format("/proc/%d/cmdline", t);
    {
        MFILE file(path.GetPtr());
        file.SetOpenMode(O_RDONLY);
        if (!file.Open()) return 0;
        buff.SetLen(len + 1);
        buff.Set(0);
        file.Read(buff.GetPtr(), len); //return 0;

        char *bp = buff.GetPtr();
        while (*bp)
        {
            CmdLine += bp;
            bp += strlen(bp) + 1;
            if (*bp)
                CmdLine += " ";
        }
        file.Close();
    }
    {
        MFILE file(path.GetPtr());
        file.SetOpenMode(O_RDONLY);
        path.Format("/proc/%d/environ", t);
        if (!file.Open()) return 0;
        buff.SetLen(len + 1);
        buff.Set(0);
        file.Read(buff.GetPtr(), len); //return 0;
        file.Close();
    }
    char *p = buff.GetPtr();
    while (*p)
    {
        env.Add() = p;
        p += strlen(p) + 1;
    }


    WorkDir.SetLen(MAX_PATH + 1);
    WorkDir.Set(0);
    PathExe.SetLen(MAX_PATH + 1);
    PathExe.Set(0);
    path.Format("/proc/%d/exe", t);
    size_t sz = readlink(path.GetPtr(), PathExe.GetPtr(), PATH_MAX);
    if (!PathExe.StrLen())
        return 0;

    path.Format("/proc/%d/cwd", t);
    readlink(path.GetPtr(), WorkDir.GetPtr(), PATH_MAX);
    if (!WorkDir.StrLen())
        return 0;
    path.Format("/proc/%d/stat", t);
    {
        MFILE file(path.GetPtr());
        file.SetOpenMode(O_RDONLY);

        if (!file.Open()) return 0;
        buff.SetLen(len + 1);
        buff.Set(0);
        file.Read(buff.GetPtr(), len); // return 0;
        struct stat buf;

        fstat(file.File(), &buf);
        struct passwd *p;
        p = getpwuid(buf.st_uid);
        if (p)
            UserName = p->pw_name;

        file.Close();
    }
    MVector<TString> tock;
    buff.Tokenize("() ", tock);
    if (tock.GetLen() < 25) return 0;
    sscanf(tock[0].GetPtr(), "%d", &pid);
    if (pid != t) return 0;
    TString UserName;

    state = tock[2][0];
    Name = tock[1];
    sscanf(tock[3].GetPtr(), "%d", &ppid); // %d (4)
    sscanf(tock[4].GetPtr(), "%d", &pgrp); // %d     (5)
    sscanf(tock[5].GetPtr(), "%d", &session); // %d  (6)
    sscanf(tock[6].GetPtr(), "%d", &tty_nr); // %d   (7)
    sscanf(tock[7].GetPtr(), "%d", &tpgid); //%d    (8)
    sscanf(tock[8].GetPtr(), "%d", &flags); // %u (9)
    sscanf(tock[9].GetPtr(), "%u", &minflt); // %lu  (10)
    sscanf(tock[10].GetPtr(), "%u", &cminflt); //%lu (11)
    sscanf(tock[11].GetPtr(), "%u", &majflt); // %lu  (12)
    sscanf(tock[12].GetPtr(), "%u", &cmajflt); // %lu (13)
    sscanf(tock[13].GetPtr(), "%u", &utime); // %lu   (14)
    sscanf(tock[14].GetPtr(), "%u", &stime); // %lu   (15)
    sscanf(tock[15].GetPtr(), "%d", &cutime); // %ld;  (16)
    sscanf(tock[16].GetPtr(), "%d", &cstime); // %ld  (17)
    sscanf(tock[17].GetPtr(), "%d", &priority); // %ld   (18)
    sscanf(tock[18].GetPtr(), "%d", &nice); // %ld    (19)
    sscanf(tock[19].GetPtr(), "%d", &num_threads); // %ld  (20)
    sscanf(tock[20].GetPtr(), "%d", &itrealvalue); // %ld (21)
    sscanf(tock[21].GetPtr(), "%llu", &starttime); // %llu (22)
    sscanf(tock[22].GetPtr(), "%u", &vsize); // %lu   (23)
    sscanf(tock[23].GetPtr(), "%d", &rss); //  %ld     (24)
    sscanf(tock[24].GetPtr(), "%u", &rsslim); // %lu  (25)
    return 1;
}

EXPORTNIX int GetTaskList(MVector<TaskInfo> &res)
{
    DIR *dir_proc = opendir("/proc/");
    struct dirent *entity;
    if (!dir_proc)
    {
        StdError(0);
        return 0;
    }

    TaskInfo f;
    while ((entity = readdir(dir_proc)))
    {
        if (entity->d_type == DT_DIR)
        {
            BOOL next = false;
            char *n = entity->d_name;
            for (; *n; ++n)
                if (*n < '0' || *n > '9')
                {
                    next = true;
                    break;
                }
            if (!next)
            {
                int pid = 0;
                sscanf(entity->d_name, "%d", &pid);
                if (f.Init(pid))
                    res.Add(f);
            }
        }
    }
    closedir(dir_proc);
    return res.GetLen();

}


EXPORTNIX TString GetHomeDir()
{
#ifdef ANDROID_NDK
    return MGetApp()->sPath;
#else
    TString str;
        struct passwd *pw = getpwuid(getuid());

        str = pw->pw_dir;
        return str;
#endif
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX bool UpdatePathFromWIN32(LPCSTR _dir, bool fIsDir, TString &res)
{
    res = "";
    TString test = _dir;
    while (test.Find("//") != -1)
        test.Replace("//", "/");
    DWORD dw = GetFileAttributes(_dir);

    if (dw != 0xFFFFFFFF)
    {
        res = _dir;
        return fIsDir ? (dw & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY :
               (dw & FILE_ATTRIBUTE_ARCHIVE) == FILE_ATTRIBUTE_ARCHIVE;
    }
#ifdef ANDROID_NDK
    else
    {

        if (memcmp(_dir, "ASSETS", 6) == 0)
        {
            res = _dir;
            return true;
        }
    }
#endif
    TString str = _dir;
    MVector<TString> tock;
    str.Tokenize("\\/", tock);
    TString path;
    int N = tock.GetLen();
    BOOL fChange = false;
    for (int j, i = 0; i < N; ++i)
    {
        path += SFD;
        res += SFD;
        path += tock[i];
        dw = GetFileAttributes(path.GetPtr());
        if (fIsDir || i < N - 1)
            if ((dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
                return 0;
        if (dw == 0xFFFFFFFF)
        {
            WIN32_FIND_DATAA *dir = 0;
            WIN32_FIND_DATAA *file = 0;
            int num_dir = 0, num_file = 0;
            if (GetDirList(res, "*", dir, num_dir, file, num_file))
            {
                if (fIsDir || i < N - 1)
                {
                    for (j = 0; j < num_dir; ++j)
                    {
                        if (tock[i].CompareNoCase(dir[j].cFileName) == 0)
                        {
                            tock[i] = dir[j].cFileName;
                            ++fChange;
                            path = res;
                            path += dir[j].cFileName;
                            res += dir[j].cFileName;
                            break;
                        }
                    }
                    if (dir) HFREE(dir);
                    if (file) HFREE(file);
                    if (j == num_dir) return 0;
                } else
                {
                    for (j = 0; j < num_file; ++j)
                    {
                        if (tock[i].CompareNoCase(file[j].cFileName) == 0)
                        {
                            ++fChange;
                            tock[i] = file[j].cFileName;
                            path = res;
                            path += file[j].cFileName;
                            res += file[j].cFileName;
                            break;
                        }
                    }
                    if (j == num_file)
                        res += tock[i];
                    if (dir) HFREE(dir);
                    if (file) HFREE(file);
                    return fChange;
                }

            } else
                return 0;
        } else
            res += tock[i];
    }
    return 1;
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX FILE *FOpenFile(__const char *__restrict __filename,
                          __const char *__restrict __modes)
{
    TString res;
    if (!UpdatePathFromWIN32(__filename, false, res))
        return fopen(__filename, __modes);
    return fopen(res.GetPtr(), __modes);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX void _makepath(char *all, const char *dr, const char *pa, const char *na, const char *ex)
{
    TString s;
    s += dr;

    s += pa;
    if (s.StrLen())
        if (s.Right(1) != SFD)
            s += SFD;
    s += na;
    s += ex;
    if (s.StrLen())
    {
        while (s.Find("//") != -1)
            s.Replace("//", SFD);
        strcpy(all, s.GetPtr());
    } else
        all[0] = 0;
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX void _splitpath(const char *str, char *_dr, char *_pa, char *_na, char *_ex)
{
    TString t = str;
    if (_dr)
        _dr[0] = 0;
    if (_pa)
        _pa[0] = 0;
    if (_na)
        _na[0] = 0;
    if (_ex)
        _ex[0] = 0;
    int e1 = t.ReverseFind('/');

    int e = t.ReverseFind('.');
    if (e1 > e)
        e = -1;
    if (e == -1)
    {
        _ex[0] = 0;
        e = t.StrLen();
    } else if (_ex)
    {
        if (e < t.StrLen() - 1)
            strcpy(_ex, t.Mid(e, -1).GetPtr());
    }
    int n = t.ReverseFind(FD);
    if (_na)
    {
        if (n == -1 || e == n - 1)
        {
            if (n == -1)
            {
                if (e == -1)
                    e = t.StrLen();
                strcpy(_na, t.Left(e).GetPtr());
            } else
                _na[0] = 0;
        } else
        {
            if (e - n - 1 > 0)
                strcpy(_na, t.Mid(n + 1, e - n - 1).GetPtr());
            else
                _na[0] = 0;
        }
    }
    if (_pa && n > 1)
        strcpy(_pa, t.Mid(1, n - 1).GetPtr());
    if (_dr) strcpy(_dr, SFD);
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX void MakeCheckDir(const char *_dir)
{
    TString str = _dir;
    MVector<TString> tock;
    str.Tokenize(SFD, tock);
    TString path;
    DWORD dw;
    for (int i = 0; i < tock.GetLen(); ++i)
    {
        path += SFD;
        path += tock[i];
        if ((dw = GetFileAttributes(path.GetPtr())) == 0xFFFFFFFF ||
            (dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
            mkdir(path.GetPtr(), S_IRWXU);
    }

}
EXPORTNIX void MakeDirFromFile(const char *_file)
{
    TString str = _file;
    MVector<TString> tock;
    str.Tokenize(SFD, tock);
    TString path;
    DWORD dw;
    for (int i = 0, l = tock.GetLen() - 1; i < l; ++i)
    {
        path += SFD;
        path += tock[i];
        if ((dw = GetFileAttributes(path.GetPtr())) == 0xFFFFFFFF ||
            (dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
            mkdir(path.GetPtr(), S_IRWXU);
    }

}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX BOOL GetTempFileName(const char *_dir, const char *prfx, int len, char *res)
{
    if (_dir && *_dir)
        MakeCheckDir(_dir);
    TString t;
    TString dir(_dir);
    if (dir.Right(1) != SFD)
        dir += SFD;
    if (len == 0) len = MAX_PATH;
    if (len < dir.StrLen() + strlen(prfx) + 7) return 0;
    t.Format("%s%s.XXXXXX", dir.GetPtr(), prfx);
    strcpy(res, t.GetPtr());
    int fd = mkstemp(res);
    if (fd > 0)
    {
        close(fd);
        return true;

    }
/*
    t = tempnam(dir.GetPtr(), prfx);
    if (t.StrLen() && (len <= 0 || t.StrLen() < len))
    {
        while (t.Find("//") != -1)
            t.Replace("//", SFD);
        strcpy(res, t.GetPtr());
        return 1;
    }
    */
    return 0;
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX char *TmpNam(char *tmp);

EXPORTNIX BOOL GetTempPath(int len, char *res)
{
#ifndef ANDROID_NDK
    TString t = tempnam(0, "c");
#else
    TString t(MAX_PATH);
    TmpNam(t.GetPtr());
#endif
    int i = t.ReverseFind(FD);
    if (i == -1) return 0;
    t[i + 1] = 0;
    if (len <= 0 || t.StrLen() < len)
    {
        while (t.Find("//") != -1)
            t.Replace("//", SFD);
        strcpy(res, t.GetPtr());
        return 1;
    }
    return 0;
}

#define IsLeapYear(y) (((y % 4) == 0) && (((y % 100) != 0) || ((y % 400) == 0)))
#define SECSPERMIN         60
#define SECSPERDAY        86400
#define TICKSPERSEC       10000000
#define TICKSPERMSEC      10000
#define SECSPERHOUR        3600
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define DAYSPERQUADRICENTENNIUM (365 * 400 + 97)
#define DAYSPERNORMALQUADRENNIUM (365 * 4 + 1)
static const int MonthLengths[2][12] =
        {
                {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
        };
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX BOOL SystemTimeToFileTime(SYSTEMTIME *sys, FILETIME *utcft)
{
    int month, year, cleaps, day;

    /* FIXME: normalize the TIME_FIELDS structure here */
    /* No, native just returns 0 (error) if the fields are not */
    if ((short) sys->wMilliseconds < 0 || sys->wMilliseconds > 999 ||
        (short) sys->wSecond < 0 || sys->wSecond > 59 ||
        (short) sys->wMinute < 0 || sys->wMinute > 59 ||
        (short) sys->wHour < 0 || sys->wHour > 23 ||
        (short) sys->wMonth < 1 || sys->wMonth > 12 ||
        (short) sys->wDay < 1 ||
        (short) sys->wDay > MonthLengths
        [sys->wMonth == 2 || IsLeapYear(sys->wYear)]
        [sys->wMonth - 1] ||
        sys->wYear < 1601)
        return FALSE;

    /* now calculate a day count from the date
     * First start counting years from March. This way the leap days
     * are added at the end of the year, not somewhere in the middle.
     * Formula's become so much less complicate that way.
     * To convert: add 12 to the month numbers of Jan and Feb, and
     * take 1 from the year */
    if (sys->wMonth < 3)
    {
        month = sys->wMonth + 13;
        year = sys->wYear - 1;
    } else
    {
        month = sys->wMonth + 1;
        year = sys->wYear;
    }
    cleaps = (3 * (year / 100) + 3) / 4;   /* nr of "century leap years"*/
    day = (36525 * year) / 100 - cleaps + /* year * dayperyr, corrected */
          (1959 * month) / 64 +         /* months * daypermonth */
          sys->wDay -          /* day of the month */
          584817;                      /* zero that on 1601-01-01 */
    /* done */
    LARGE_INTEGER utc;
    utc.QuadPart = (((((LONGLONG) day * HOURSPERDAY +
                       sys->wHour) * MINSPERHOUR +
                      sys->wMinute) * SECSPERMIN +
                     sys->wSecond) * 1000 +
                    sys->wMilliseconds) * TICKSPERMSEC;

    utcft->dwLowDateTime = utc.u.LowPart;
    utcft->dwHighDateTime = utc.u.HighPart;
    return TRUE;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX void FileTimeToSystemTime(
        FILETIME *utcft,
        SYSTEMTIME *sys)
{

    LARGE_INTEGER utc;

    utc.u.LowPart = utcft->dwLowDateTime;
    utc.u.HighPart = utcft->dwHighDateTime;

    __int64 SecondsInDay;
    __int64 cleaps, years, yearday, months;
    __int64 Days;
    __int64 Time;

    /* Extract millisecond from time and convert time into seconds */
    sys->wMilliseconds =
            (WORD) ((utc.QuadPart % TICKSPERSEC) / TICKSPERMSEC);
    Time = utc.QuadPart / TICKSPERSEC;

    /* The native version of RtlTimeToTimeFields does not take leap seconds
     * into account */

    /* Split the time into days and seconds within the day */
    Days = Time / SECSPERDAY;
    SecondsInDay = Time % SECSPERDAY;

    /* compute time of day */
    sys->wHour = (WORD) (SecondsInDay / SECSPERHOUR);
    SecondsInDay = SecondsInDay % SECSPERHOUR;
    sys->wMinute = (WORD) (SecondsInDay / SECSPERMIN);
    sys->wSecond = (WORD) (SecondsInDay % SECSPERMIN);

    /* compute day of week */
//	short wWeekday = (short) ((EPOCHWEEKDAY + Days) % DAYSPERWEEK);

    /* compute year, month and day of month. */
    cleaps = (3 * ((4 * Days + 1227) / DAYSPERQUADRICENTENNIUM) + 3) / 4;
    Days += 28188 + cleaps;
    years = (20 * Days - 2442) / (5 * DAYSPERNORMALQUADRENNIUM);
    yearday = Days - (years * DAYSPERNORMALQUADRENNIUM) / 4;
    months = (64 * yearday) / 1959;
    /* the result is based on a year starting on March.
     * To convert take 12 from Januari and Februari and
     * increase the year by one. */
    if (months < 14)
    {
        sys->wMonth = (WORD) (months - 1);
        sys->wYear = (WORD) (years + 1524);
    } else
    {
        sys->wMonth = (WORD) (months - 13);
        sys->wYear = (WORD) (years + 1525);
    }
    /* calculation of day of month is based on the wonderful
     * sequence of INT( n * 30.6): it reproduces the
     * 31-30-31-30-31-31 month lengths exactly for small n's */
    sys->wDay = (WORD) (yearday - (1959 * months) / 64);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXFile::AFXFile(int _hFile)
{
    hFile = (FILEHANDLE) _hFile;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXFile::AFXFile(LPCSTR lpszFileName, UINT nOpenFlags) : MFILE(lpszFileName, 0, T_FILE)
{

    int mode = nOpenFlags & (~nOpenFlags);
    if (mode)
        MFILE::SetOpenMode(mode);
    int ret = MFILE::Open((nOpenFlags & modeCreate) == modeCreate);
    if (!ret)
        THROW1(CFileException, CFileException::badPath);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
BOOL AFXFile::Open(LPCSTR lpszFileName, UINT nOpenFlags, CFileException *p)
{
    if (lpszFileName && lpszFileName[0])
    {
        if (name) HFREE(name);
        name = (char *) HMALLOC(strlen(lpszFileName) + 1);
        strcpy(name, lpszFileName);
    }
    MFILE::SetOpenMode(nOpenFlags);
    int ret = MFILE::Open((nOpenFlags & modeCreate) == modeCreate);

    if (!ret)
    {
        if (p)
            ((CFileException *)p)->m_cause = CFileException::badPath;
        else
            THROW1(CFileException, CFileException::badPath);
    }
    return true;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
UINT AFXFile::Read(void *lpBuf, UINT nCount)
{
    if (hFile == INVALID_HANDLE_VALUE)
        THROW1(CFileException, CFileException::badPath);
    off_t off = MFILE::Read((off_t) -1, lpBuf, (off_t) nCount);
    if (!off)
    {
        THROW1(CFileException, CFileException::hardIO);

    }
    return (UINT) off;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
BOOL AFXFile::Write(const void *lpBuf, UINT nCount)
{
    if (hFile == INVALID_HANDLE_VALUE)
        THROW1(CFileException, CFileException::badPath);
//	off_t pos = MFILE::GetSeek();
    BOOL ret = MFILE::Write((off_t) -1, lpBuf, (off_t) nCount);
    if (!ret)
    {
        THROW1(CFileException, CFileException::hardIO);

    }
    return ret;

}

void AFXFile::SetFilePath(LPCSTR lpszFileName)
{
    Close();
    if (lpszFileName && lpszFileName[0])
    {
        if (name) HFREE(name);
        name = (char *) HMALLOC(strlen((char *) lpszFileName) + 1);
        strcpy(name, lpszFileName);
        TString s = name;
        int i1 = s.ReverseFind(FD);
        int i2 = s.ReverseFind('\\');
        i1 = max(i1, i2);
        i2 = s.ReverseFind('.');
        if (i2 == -1)
            i2 = s.StrLen();
        title = s.Mid(i1 + 1, i2 - i1 - 1);


    }
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
LONG AFXFile::Seek(LONG lOff, UINT nFrom)
{
    if (hFile == INVALID_HANDLE_VALUE)
        THROW1(CFileException, CFileException::badPath);
    switch (nFrom)
    {
        case begin:
            return MFILE::SetSeek(lOff);
        case current:
            return MFILE::SetSeek((LONG) MFILE::GetSeek() + lOff);
        case end:
            break;
        default:
        {
            THROW1(CFileException, CFileException::badSeek);

        }

    }
    return MFILE::SetSeek((LONG) MFILE::GetSize() + lOff);
}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void AFXFile::Rename(LPCSTR lpszOldName, LPCSTR lpszNewName)
{
#ifdef __PLUK_LINUX__
    rename(lpszOldName, lpszNewName);
#else
    ::MoveFile((LPTSTR)lpszOldName, (LPTSTR)lpszNewName);
#endif
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void AFXFile::Remove(LPCSTR lpszFileName)
{
#ifdef __PLUK_LINUX__
    unlink(lpszFileName);
#else
    ::DeleteFile(lpszFileName);
#endif
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator<<(BYTE by)
{
    Write(&by, 1);
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator<<(WORD w)
{
    Write(&w, sizeof(WORD));
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator<<(int l)
{
    Write(&l, sizeof(int));
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator<<(DWORD dw)
{
    Write(&dw, sizeof(DWORD));
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator<<(float f)
{
    Write(&f, sizeof(float));
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator<<(double d)
{
    Write(&d, sizeof(double));
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator>>(BYTE &by)
{
    if (Read(&by, sizeof(BYTE)) != sizeof(BYTE))
    {
        THROW1(CFileException, CArchiveException::badIndex);
    }
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator<<(LPCSTR str)
{
    WriteString(str);
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator>>(WORD &w)
{
    if (Read(&w, sizeof(WORD)) != sizeof(WORD))
    {
        THROW1(CFileException, CArchiveException::badIndex);
    }
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator>>(DWORD &dw)
{
    if (Read(&dw, sizeof(DWORD)) != sizeof(DWORD))
    {
        THROW1(CFileException, CArchiveException::badIndex);
    }
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator>>(float &f)
{
    if (Read(&f, sizeof(float)) != sizeof(float))
    {
        THROW1(CFileException, CArchiveException::badIndex);
    }
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator>>(double &d)
{
    if (Read(&d, sizeof(double)) != sizeof(double))
    {
        THROW1(CFileException, CArchiveException::badIndex);
    }
    return *this;
}

AFXArchive &AFXArchive::operator>>(TString &d)
{
    char ch;
    for (;;)
    {
        *this >> ch;
        d.Add(ch);
        if (ch == '\0')
            break;
    }
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive &AFXArchive::operator>>(int &l)
{
    if (Read(&l, sizeof(int)) != sizeof(int))
    {
        THROW1(CFileException, CArchiveException::badIndex);
    }
    return *this;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive::AFXArchive(AFXFile *pFile, UINT nMode, int nBufSize, void *lpBuf) :
        m_strFileName(pFile->GetFilePath())
{
    m_nMode = nMode;
    m_pFile = pFile;

}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
AFXArchive::~AFXArchive()
{
    if (m_pFile != NULL && !(m_nMode & bNoFlushOnDelete))
    {
        Close();
        m_pFile = NULL;
    }
    m_pFile = NULL;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void AFXArchive::Abort()
{

}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void AFXArchive::Close()
{
    if (m_pFile)
        m_pFile->Close();
    m_pFile = NULL;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
UINT AFXArchive::Read(void *lpBuf, UINT nMax)
{
    int fRet = 0;
    if (m_pFile)
        fRet = m_pFile->Read(lpBuf, nMax);
    return fRet;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
UINT AFXArchive::Write(const void *lpBuf, UINT nMax)
{
    return m_pFile ? m_pFile->Write(lpBuf, nMax) : 0;

}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void AFXArchive::Flush()
{
    if (m_pFile)
        m_pFile->Flush();
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void AFXArchive::WriteString(LPCSTR lpsz)
{
    int len = 0;
    if (lpsz)
        len = strlen(lpsz);
    if (len)
        Write(lpsz, strlen(lpsz));
    else
        *this << '\0';
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
LPSTR AFXArchive::ReadString(LPSTR lpsz, UINT nMax)
{
    int nStop = abs((int) nMax);

    BYTE ch;
    int nRead = 0;

    try
    {
        while (nRead < nStop)
        {
            *this >> ch;
            if (ch == '\0')
                break;
            if (ch == '\n' || ch == '\r')
            {
                if (ch == '\r')
                    *this >> ch;
                if ((int) nMax != nStop)
                    lpsz[nRead++] = ch;
                break;
            }
            lpsz[nRead++] = ch;
        }
    }
    catch (...)
    {
        return NULL;
    }
    lpsz[nRead] = '\0';
    return lpsz;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
BOOL AFXArchive::ReadString(TString &rString)
{

    const int nMaxSize = 128;
    LPSTR lpsz = (LPSTR) rString.GetBuffer(nMaxSize);
    LPSTR lpszResult;
    int nLen;
    for (;;)
    {
        lpszResult = ReadString(lpsz, (UINT) -nMaxSize);

        if (lpszResult == NULL ||
            (nLen = strlen(lpsz)) < nMaxSize ||
            lpsz[nLen - 1] == '\n')
        {
            break;
        }

        nLen = rString.GetLength();
        lpsz = (LPSTR) rString.GetBuffer(nMaxSize + nLen) + nLen;
    }
    lpsz = (LPSTR) rString.GetBuffer(0);
    nLen = rString.GetLength();
    if (nLen != 0 && lpsz[nLen - 1] == '\n')
        rString.GetBufferSetLength(nLen - 1);

    return lpszResult != NULL;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
int COleDateTime::DateFromJulian(int dateIn)
{
    int julianDays = dateIn;

    julianDays -= 1757585;  /* Convert to + days from 1 Jan 100 AD */
    julianDays += DATE_MIN; /* Convert to +/- days from 1 Jan 1899 AD */
    return julianDays;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
int COleDateTime::JulianFromDMY(USHORT year, USHORT month, USHORT day)
{
    int m12 = (month - 14) / 12;

    return ((1461 * (year + 4800 + m12)) / 4 + (367 * (month - 2 - 12 * m12)) / 12 -
            (3 * ((year + 4900 + m12) / 100)) / 4 + day - 32075);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
int COleDateTime::JulianFromDate(int dateIn)
{
    int julianDays = dateIn;

    julianDays -= DATE_MIN; /* Convert to + days from 1 Jan 100 AD */
    julianDays += 1757585;  /* Convert to + days from 23 Nov 4713 BC (Julian) */
    return julianDays;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void COleDateTime::Convert(SYSTEMTIME &st, double *pDateOut)
{
    double dateVal, dateSign;
    dateVal = DateFromJulian(JulianFromDMY(st.wYear, st.wMonth, st.wDay));
    dateSign = (dateVal < 0.0) ? -1.0 : 1.0;
    dateVal += st.wHour / 24.0 * dateSign;
    dateVal += st.wMinute / 1440.0 * dateSign;
    dateVal += st.wSecond / 86400.0 * dateSign;
    *pDateOut = dateVal;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void COleDateTime::DMYFromJulian(int jd, USHORT *year, USHORT *month, USHORT *day)
{
    int j, i, l, n;

    l = jd + 68569;
    n = l * 4 / 146097;
    l -= (n * 146097 + 3) / 4;
    i = (4000 * (l + 1)) / 1461001;
    l += 31 - (i * 1461) / 4;
    j = (l * 80) / 2447;
    *day = l - (j * 2447) / 80;
    l = j / 11;
    *month = (j + 2) - (12 * l);
    *year = 100 * (n - 49) + i + l;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
COleDateTime::DateTimeStatus COleDateTime::RollUdate(SYSTEMTIME &st)
{
    static const BYTE days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    short iYear, iMonth, iDay, iHour, iMinute, iSecond;

    /* interpret values signed */
    iYear = st.wYear;
    iMonth = st.wMonth;
    iDay = st.wDay;
    iHour = st.wHour;
    iMinute = st.wMinute;
    iSecond = st.wSecond;


    if (iYear > 9999 || iYear < -9999)
        return invalid; //error
    /* Years < 100 are treated as 1900 + year */
    if (iYear > 0 && iYear < 100)
        iYear += 1900;

    iMinute += iSecond / 60;
    iSecond = iSecond % 60;
    iHour += iMinute / 60;
    iMinute = iMinute % 60;
    iDay += iHour / 24;
    iHour = iHour % 24;
    iYear += iMonth / 12;
    iMonth = iMonth % 12;
    if (iMonth <= 0)
    {
        iMonth += 12;
        iYear--;
    }
    while (iDay > days[iMonth])
    {
        if (iMonth == 2 && IsLeapYear(iYear))
            iDay -= 29;
        else
            iDay -= days[iMonth];
        iMonth++;
        iYear += iMonth / 12;
        iMonth = iMonth % 12;
    }
    while (iDay <= 0)
    {
        iMonth--;
        if (iMonth <= 0)
        {
            iMonth += 12;
            iYear--;
        }
        if (iMonth == 2 && IsLeapYear(iYear))
            iDay += 29;
        else
            iDay += days[iMonth];
    }

    if (iSecond < 0)
    {
        iSecond += 60;
        iMinute--;
    }
    if (iMinute < 0)
    {
        iMinute += 60;
        iHour--;
    }
    if (iHour < 0)
    {
        iHour += 24;
        iDay--;
    }
    if (iYear <= 0) iYear += 2000;

    st.wYear = iYear;
    st.wMonth = iMonth;
    st.wDay = iDay;
    st.wHour = iHour;
    st.wMinute = iMinute;
    st.wSecond = iSecond;
    return valid;


}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void COleDateTime::SetDate(FILETIME *m)
{
    SYSTEMTIME tm;
    FileTimeToSystemTime(m, &tm);
    Convert(tm, &m_dt);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void COleDateTime::GetDate(FILETIME *m)
{
    SYSTEMTIME tm;
    Convert(m_dt, tm);
    SystemTimeToFileTime(&tm, m);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
COleDateTime::DateTimeStatus COleDateTime::Convert(double dateIn, SYSTEMTIME &st)
{
    /* Cumulative totals of days per month */
/*  static const USHORT cumulativeDays[] =
  {
	0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
 * */
    double datePart, timePart;
    int julianDays;

    if (dateIn <= (DATE_MIN - 1.0) || dateIn >= (DATE_MAX + 1.0))
        return invalid; //error

    datePart = dateIn < 0.0 ? ceil(dateIn) : floor(dateIn);
    /* Compensate for int truncation (always downwards) */
    timePart = dateIn - datePart + 0.00000000001;
    if (timePart >= 1.0)
        timePart -= 0.00000000001;

    /* Date */
    julianDays = JulianFromDate((int) dateIn);
    DMYFromJulian(julianDays, &st.wYear, &st.wMonth,
                  &st.wDay);

    datePart = (datePart + 1.5) / 7.0;
    st.wDayOfWeek = (WORD) ((datePart - floor(datePart)) * 7);
    if (st.wDayOfWeek == 0)
        st.wDayOfWeek = 5;
    else if (st.wDayOfWeek == 1)
        st.wDayOfWeek = 6;
    else
        st.wDayOfWeek -= 2;

    /* Time */
    timePart *= 24.0;
    st.wHour = (WORD) timePart;
    timePart -= st.wHour;
    timePart *= 60.0;
    st.wMinute = (WORD) timePart;
    timePart -= st.wMinute;
    timePart *= 60.0;
    st.wSecond = (WORD) timePart;
    timePart -= st.wSecond;
    st.wMilliseconds = 0;
    if (timePart > 0.5)
    {
        /* Round the milliseconds, adjusting the time/date forward if needed */
        if (st.wSecond < 59)
            st.wSecond++;
        else
        {
            st.wSecond = 0;
            if (st.wMinute < 59)
                st.wMinute++;
            else
            {
                st.wMinute = 0;
                if (st.wHour < 23)
                    st.wHour++;
                else
                {
                    st.wHour = 0;
                    /* Roll over a whole day */
                    if (++st.wDay > 28)
                        return RollUdate(st);
                }
            }
        }
    }
    return valid;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
COleDateTime::COleDateTime()
{
    time_t sec = time(0);
    struct tm *t = localtime(&sec);
    SetDate(*t);

}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX void SetSystemTime(SYSTEMTIME *nt)
{
    char dateTime[100];
    char command[50] = "date '+%Y-%m-%d %H:%M:%S' -s ";

    sprintf(dateTime, "%s '%d-%d-%d %d:%d:%d'", command, nt->wYear, nt->wMonth, nt->wDay, nt->wHour,
            nt->wMinute, nt->wSecond);
    system(dateTime);


/*
    time_t sec = time(0);
    struct tm *t1 =  localtime(&sec);
	struct tm t;
    memcpy(&t, t1, sizeof(struct tm));
    t.tm_year = nt->wYear - 1900;
    t.tm_mon = nt->wMonth - 1;
    t.tm_mday = nt->wDay;
    t.tm_hour = nt->wHour;
    t.tm_min = nt->wMinute;
    t.tm_sec = nt->wSecond;
    sec = mktime(&t);
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
*/
}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX void GetSystemTime(SYSTEMTIME *t)
{
    COleDateTime ct;
    ct.GetSysTime(*t);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
COleDateTime CFileStatus::FileDateTime(const char *file)
{
    if (file)
    {
        if (::stat(file, this) == -1)
            return COleDateTime();
    }

    struct tm *t = localtime((time_t *) &st_ctime);
    COleDateTime ct(*t);
    return ct;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
int CFileStatus::Size(const char *file)
{

    int ret = ::stat(file, this);
    return ret == -1 ? 0 : st_size;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
COleDateTime COleDateTime::GetCurrentTime()
{
    return COleDateTime();
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
bool AFXFile::GetStatus(const char *file, CFileStatus &st)
{
    return ::stat(file, &st) != -1;
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX int CompareFileTime(FILETIME *lpFileTime1, FILETIME *lpFileTime2)
{
    SYSTEMTIME tm1, tm2;
    FileTimeToSystemTime(lpFileTime1, &tm1);
    FileTimeToSystemTime(lpFileTime2, &tm2);
    if (tm1.wYear > tm2.wYear) return 1;
    else if (tm1.wYear < tm2.wYear) return -1;
    if (tm1.wMonth > tm2.wMonth) return 1;
    else if (tm1.wMonth < tm2.wMonth) return -1;
    if (tm1.wDay > tm2.wDay) return 1;
    else if (tm1.wDay < tm2.wDay) return -1;
    if (tm1.wHour > tm2.wHour) return 1;
    else if (tm1.wHour < tm2.wHour) return -1;
    if (tm1.wMinute > tm2.wMinute) return 1;
    else if (tm1.wMinute < tm2.wMinute) return -1;

    if (tm1.wSecond > tm2.wSecond) return 1;
    else if (tm1.wSecond < tm2.wSecond) return -1;
    return 0;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX bool GetWIN32_FIND_DATA(const char *file, WIN32_FIND_DATAA *ret)
{
    TString name = file;
    int n = name.ReverseFind(FD);
    int e = name.StrLen();
    if (n == -1 || e == n - 1)
        return 0;
    struct stat st;
    if (::stat(file, &st) == -1)
        return 0;
    if (S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode)) return 0;

    struct tm *t = localtime((time_t *) &st.st_ctime);
    COleDateTime ct(*t);
    ct.GetDate(&ret->ftCreationTime);
    t = localtime((time_t *) &st.st_mtime);
    ct.SetDate(*t);
    ct.GetDate(&ret->ftLastAccessTime);

    ret->ftLastWriteTime = ret->ftLastAccessTime;
    LARGE_INTEGER li;
    li.QuadPart = st.st_size;
    ret->nFileSizeHigh = li.HighPart;
    ret->nFileSizeLow = li.LowPart;
    strcpy(ret->cFileName, name.Mid(n + 1, e - n - 1).GetPtr());
    ret->cAlternateFileName[0] = 0;
    ret->dwFileAttributes = 0;
    if (S_ISDIR(st.st_mode))
        ret->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    else
        ret->dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
    if (!(st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)))
        ret->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
    return 1;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void WIN32_FIND_DATAA_TO_WIN32_FIND_DATAW(WIN32_FIND_DATAA &src, WIN32_FIND_DATAW &dst)
{
    dst.dwFileAttributes = src.dwFileAttributes;
    dst.ftCreationTime = src.ftCreationTime;
    dst.ftLastAccessTime = src.ftLastAccessTime;
    dst.ftLastWriteTime = src.ftLastWriteTime;
    dst.nFileSizeHigh = src.nFileSizeHigh;
    dst.nFileSizeLow = src.nFileSizeLow;
    __T2U(src.cFileName, dst.cFileName, strlen(src.cFileName) + 1);
    dst.cAlternateFileName[0] = 0;
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX bool
GetDirList(LPCSTR path, LPCSTR mask, WIN32_FIND_DATAW *&dir, int &num_dir, WIN32_FIND_DATAW *&file,
           int &num_file)
{
    WIN32_FIND_DATAA *_dir = 0;
    WIN32_FIND_DATAA *_file = 0;
    if (GetDirList(path, mask, _dir, num_dir, _file, num_file))
    {

        if (num_dir)
        {
            dir = (WIN32_FIND_DATAW *) HMALLOC(sizeof(WIN32_FIND_DATAW) * num_dir);
            for (int i = 0; i < num_dir; ++i)
                WIN32_FIND_DATAA_TO_WIN32_FIND_DATAW(_dir[i], dir[i]);
            HFREE(_dir);
        }
        if (num_file)
        {
            file = (WIN32_FIND_DATAW *) HMALLOC(sizeof(WIN32_FIND_DATAW) * num_file);
            for (int i = 0; i < num_file; ++i)
                WIN32_FIND_DATAA_TO_WIN32_FIND_DATAW(_file[i], file[i]);
            HFREE(_file);
        }
    }
    return num_dir + num_file > 0;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
BOOL WIN32_FIND_DATAA::Init(LPCSTR filename)
{
    struct stat st;
    if (stat(filename, &st) != -1)
    {
        TString d_name = filename;
        int i1 = d_name.ReverseFind(FD);
        if (i1 != -1)
            d_name = d_name.Tail(i1 + 1);

        struct tm *t = localtime((time_t *) &st.st_ctime);
        COleDateTime ct(*t);
        ct.GetDate(&ftCreationTime);
        t = localtime((time_t *) &st.st_mtime);
        ct.SetDate(*t);
        ct.GetDate(&ftLastAccessTime);
        ftLastWriteTime = ftLastAccessTime;
        LARGE_INTEGER li;
        li.QuadPart = st.st_size;
        nFileSizeHigh = li.HighPart;
        nFileSizeLow = li.LowPart;
        strcpy(cFileName, d_name.GetPtr());
        cAlternateFileName[0] = 0;
        dwFileAttributes = 0;
        if (S_ISDIR(st.st_mode))
            dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        else
            dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
        if (!(st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)))
            dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
        return 1;
    }
    return 0;
}

#ifdef ANDROID_NDK
EXPORTNIX int ACCESS(LPCSTR a, int)
{
    TString assets = a;
    int fn;
    if ((fn = assets.Find("ASSETS/")) != -1)
    {
        assets.Remove(0, fn);
        return MGetApp()->IsResourceFile(assets.GetPtr()) ? 0 : -1;
    }
    TString res;

    if (a && strlen(a) && a[0] == FD && UpdatePathFromWIN32(a, false, res))
        //strcpy((LPSTR)a, res.GetPtr());
        a = res.GetPtr();

    return GetFileAttributes(a) == 0xFFFFFFFF ? -1 : 0;

}
#endif
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX bool
GetDirList(LPCSTR path, LPCSTR mask, WIN32_FIND_DATAA *&dir, int &num_dir, WIN32_FIND_DATAA *&file,
           int &num_file)
{
    TString assets = path;
    MArray<WIN32_FIND_DATAA> vDir, vFile;
    int fn;
#ifdef ANDROID_NDK
    if ((fn = assets.Find("ASSETS/")) != -1)
    {
        MVector<TString> _dir;
        MVector<TString> _files;

        assets.Replace("ASSETS/", "");
        MGetApp()->ReadResourceDir(assets.GetPtr(), mask, _dir, _files);
        for (int i = 0, len = _dir.GetLen(); i < len; ++i)
        {
            WIN32_FIND_DATAA &ret = vDir.Add();
            memset(&ret, 0, sizeof(WIN32_FIND_DATAA));
            strcpy(ret.cFileName, _dir[i].GetPtr());
            ret.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        }
        for (int i = 0, len = _files.GetLen(); i < len; ++i)
        {
            WIN32_FIND_DATAA &ret = vDir.Add();
            memset(&ret, 0, sizeof(WIN32_FIND_DATAA));
            strcpy(ret.cFileName, _files[i].GetPtr());
            ret.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        }
        num_dir = vDir.GetLen();
        num_file = vFile.GetLen();
        file = vFile.GetPtr();
        dir = vDir.GetPtr();
        vFile.Detach();
        vDir.Detach();
        return true;
    }
#endif
    TString Path = path;
    if (Path[Path.StrLen() - 1] != FD)
        Path += SFD;
    else
        while (Path.StrLen() > 2 && Path[Path.StrLen() - 2] == FD)
            Path[Path.StrLen() - 1] = 0;
    TString msk = mask;
    if (msk == "*.*")
    {
        msk = "*";
        mask = msk.GetPtr();
    }


    num_dir = num_file = 0;
    DIR *ptr = opendir(Path.GetPtr());
    if (!ptr) return 0;
    TString dn;
    dirent *dp;
    while ((dp = readdir(ptr)) != NULL)
        if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
            if (!mask || !fnmatch(mask, dp->d_name, FNM_CASEFOLD))
            {
                dn = Path;
                dn += dp->d_name;
                //  if (!strcmp(dp->d_name, ".")) continue;
                struct stat st;
                if (stat(dn.GetPtr(), &st) != -1)
                {
                    //      if (S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode)) continue;
                    WIN32_FIND_DATAA *ret;
                    if (S_ISDIR(st.st_mode))
                    {
                        WIN32_FIND_DATAA &ret1 = vDir.Add();
                        ret = &ret1;
                    } else if (S_ISREG(st.st_mode))
                    {
                        WIN32_FIND_DATAA &ret1 = vFile.Add();
                        ret = &ret1;
                    } else
                        continue;
                    struct tm *t = localtime((time_t *) &st.st_ctime);
                    COleDateTime ct(*t);
                    ct.GetDate(&ret->ftCreationTime);
                    t = localtime((time_t *) &st.st_mtime);
                    ct.SetDate(*t);
                    ct.GetDate(&ret->ftLastAccessTime);
                    ret->ftLastWriteTime = ret->ftLastAccessTime;
                    LARGE_INTEGER li;
                    li.QuadPart = st.st_size;
                    ret->nFileSizeHigh = li.HighPart;
                    ret->nFileSizeLow = li.LowPart;
                    strcpy(ret->cFileName, dp->d_name);
                    ret->cAlternateFileName[0] = 0;
                    ret->dwFileAttributes = 0;
                    if (S_ISDIR(st.st_mode))
                        ret->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
                    else
                        ret->dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
                    if (!(st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)))
                        ret->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
                }
            }
    closedir(ptr);
    num_dir = vDir.GetLen();
    num_file = vFile.GetLen();
    file = vFile.GetPtr();
    dir = vDir.GetPtr();
    vFile.Detach();
    vDir.Detach();
    return 1;

}
extern CMemFile *spyfile;

int _filelength(LPCSTR hFile)
{
    if (hFile)
    {
        if (spyfile)
        {
            if (spyfile->GetFileName() == hFile)
                return spyfile->GetSize();
        }

        struct stat st;
        return ::stat(hFile, &st) != -1 ? st.st_size : EOF;
    }
    return EOF;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX DWORD GetFileAttributes(TString path)
{
    if (spyfile)
    {
        if (path == spyfile->GetFileName())
            return FILE_ATTRIBUTE_ARCHIVE;
    }
    DWORD ret = 0xFFFFFFFF;
    if (path.StrLen())
    {
        struct stat st;
        while (path.StrLen() > 1 && path.Right(1) == SFD)
            path[path.StrLen() - 1] = 0;

        if (stat(path.GetPtr(), &st) != -1)
        {
            if (S_ISDIR(st.st_mode))
                ret = FILE_ATTRIBUTE_DIRECTORY;
            else
                ret = FILE_ATTRIBUTE_ARCHIVE;
            if (!(st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)))
                ret |= FILE_ATTRIBUTE_READONLY;
        }
    }
    return ret;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX int GetDriveType(char *buff)
{
    FFile file;
    MVector<TString> path;
//        char b[1024];
    if (!strcmp(buff, SFD)) return DRIVE_FIXED;
    if (file.Open(_PATH_MOUNTED, "r") || file.Open("/proc/mounts", "r"))
    {
        struct mntent *res;

        while ((res = getmntent(file.hFile)) != NULL) //, (struct mntent *)b, b, 1024))
        {
            if (!strcmp(buff, res->mnt_dir))
            {

                struct stat st;
                if (stat(res->mnt_fsname, &st) != -1)
                {
                    if (S_ISBLK(st.st_mode))
                    {

#ifndef _ARM_EOS_
                        if (strstr(res->mnt_fsname, "sda")) return DRIVE_FIXED;
#else
                        if (strstr(res->mnt_fsname, "root")) return DRIVE_FIXED;

#endif
                        if (strstr(res->mnt_fsname, "cdrom") || strstr(res->mnt_fsname, "sr"))
                            return DRIVE_CDROM;
                        return DRIVE_REMOVABLE;
                    }

                }


            }
        }
        file.Close();
    }
    return DRIVE_UNKNOWN;
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX int GetLogicalDriveStrings(MVector<TString> &path)
{
    FFile file;
//            char b[1024];

    if (file.Open(_PATH_MOUNTED, "r") || file.Open("/proc/mounts", "r"))
    {
        struct mntent *res;

        while ((res = getmntent(file.hFile)) != NULL) //, (struct mntent *)b, b, 1024))
        {
            struct stat st;
            TString s = res->mnt_fsname;
            if (s == "/dev/root" || (stat(res->mnt_fsname, &st) != -1))
            {
                if (s == "/dev/root" || S_ISBLK(st.st_mode))
                {
                    path.Add() = res->mnt_dir;
                }

            }
        }

        file.Close();
    }
    return path.GetLen();
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX int GetLogicalDriveStrings(int len, char *buff)
{
    MVector<TString> path;
    int iRet = 0;
    if (GetLogicalDriveStrings(path))
    {
        int i, l = path.GetLen();
        for (i = 0; i < l; ++i)
            iRet += path[i].StrLen() + 1;

        ++iRet;
        if (iRet > 1)
        {
            if (len >= iRet)
            {
                memset(buff, 0, len);
                for (i = 0; i < l; ++i)
                {
                    strcpy(buff, path[i].GetPtr());
                    buff += strlen(buff) + 1;
                }
            }
        }
    }
    return iRet + 1;
}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX DWORD GetCurrentDirectory(int sz, LPSTR szCurDir)
{
    if (sz && szCurDir)
    {
        if (!getcwd(szCurDir, sz))
        {
            errno = 0;
            return 0;
        }
        return strlen(szCurDir);
    }
    TString _name;
    _name.SetLen(255);
    _name.Set(0);
    while (!getcwd(_name.GetPtr(), _name.GetLen() - 1))
    {
        _name.SetLen(_name.GetLen() * 2);
        _name.Set(0);
    }
    return _name.StrLen();
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX BOOL CalcDirInfo(LPCSTR _dir, size_t &size, size_t &num_files, size_t &num_dirs)
{
    struct stat st1;
    size = 0;
    num_files = 0;
    num_dirs = 0;

    int iSt1 = ::stat(_dir, &st1);
    if (iSt1 == -1) return 0;
    if (!S_ISDIR(st1.st_mode))
    {
        size = st1.st_size;
        num_files = 1;
        return TRUE;
    }
    WIN32_FIND_DATAA *dir = 0, *file = 0;
    int num_dir = 0, num_file = 0;
    TString path = _dir;
    if (path.Right(1) != SFD)
        path += SFD;
    GetDirList(path, "*", dir, num_dir, file, num_file);
    int i;
    size_t sz, N, nd;
    num_dirs = num_dir;
    for (i = 0; i < num_dir; ++i)
    {
        if (strcmp(dir[i].cFileName, ".") && strcmp(dir[i].cFileName, ".."))
            if (CalcDirInfo(path + dir[i].cFileName, sz, N, nd))
            {
                size += sz;
                num_files += N;
                num_dirs += nd;
            }
    }
    num_files += num_file;
    for (i = 0; i < num_file; ++i)
    {
        iSt1 = ::stat(path + file[i].cFileName, &st1);
        if (iSt1 != -1)
            size += st1.st_size;
    }
    if (file) HFREE(file);
    if (dir) HFREE(dir);
    return TRUE;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX BOOL _RemoveDir(LPCSTR _dir, MPROGRESS_ROUTINE func, void *lpData)
{
    struct stat st1;

    int iSt1 = ::stat(_dir, &st1);
    if (iSt1 == -1) return 0;
    if (!S_ISDIR(st1.st_mode))
        return DeleteFile(_dir);
    _ClearDir(_dir, func, lpData);
    return rmdir(_dir) != -1;
}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX BOOL _ClearDir(LPCSTR _dir, MPROGRESS_ROUTINE func, void *lpData)
{
    struct stat st1;

    int iSt1 = ::stat(_dir, &st1);
    if (iSt1 == -1) return 0;
    if (!S_ISDIR(st1.st_mode)) return 0;

    WIN32_FIND_DATAA *dir = 0, *file = 0;
    int num_dir = 0, num_file = 0;
    TString path = _dir;
    if (path.Right(1) != SFD)
        path += SFD;
    GetDirList(path, "*", dir, num_dir, file, num_file);
    int i;
    for (i = 0; i < num_file; ++i)
    {
        if (func)
            if (func(path + file[i].cFileName, 0, 0, lpData) == PROGRESS_CANCEL)
                return FALSE;

        DeleteFile(path + file[i].cFileName);
    }
    for (i = 0; i < num_dir; ++i)
        if (strcmp(dir[i].cFileName, ".") && strcmp(dir[i].cFileName, ".."))
            _RemoveDir(path + dir[i].cFileName);
    if (file) HFREE(file);
    if (dir) HFREE(dir);
    return true;
}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX BOOL
CopyFileTo(const char *SrcName, const char *DstName, MPROGRESS_ROUTINE func, void *lpData,
           bool fMove)
{
    struct stat st1, st2;
    TString src(SrcName), dst(DstName);


    int iSt1 = ::stat(SrcName, &st1);
    int iSt2 = ::stat(DstName, &st2);
    if (iSt1 == -1 || iSt2 == -1)
        return 0;
    if (!S_ISDIR(st2.st_mode) && !S_ISDIR(st1.st_mode))
        return _CopyFile(SrcName, DstName, func, lpData, fMove);
    if (dst.Right(1) != SFD)
        dst += SFD;
    int pos = src.ReverseFind(FD);
    if (S_ISDIR(st1.st_mode))
    {
        WIN32_FIND_DATAA *dir = 0, *file = 0;
        int num_dir = 0, num_file = 0;
        TString sdir = dst + src.Mid(pos);
        CreateDirectory(sdir.GetPtr(), 0);
        GetDirList(SrcName, "*", dir, num_dir, file, num_file);
        int i;
        if (src.Right(1) != SFD)
            src += SFD;
        if (sdir.Right(1) != SFD)
            sdir += SFD;
        for (i = 0; i < num_file; ++i)
            if (!_CopyFile(src + file[i].cFileName, sdir + file[i].cFileName, func, lpData, fMove))
            {
                if (file) HFREE(file);
                if (dir) HFREE(dir);
                return FALSE;
            }
        for (i = 0; i < num_dir; ++i)
            if (!CopyFileTo(src + dir[i].cFileName, sdir, func, lpData, fMove))
            {
                if (file) HFREE(file);
                if (dir) HFREE(dir);
                return FALSE;
            }
        if (fMove)
            _RemoveDir(src + dir[i].cFileName);
        if (file) HFREE(file);
        if (dir) HFREE(dir);
        return 1;
    }
    return _CopyFile(src, dst + src.Mid(pos + 1), func, lpData, fMove);

}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
EXPORTNIX BOOL
_CopyFile(const char *SrcName, const char *DstName, MPROGRESS_ROUTINE func, void *lpUserData,
          bool fMove)
{
    struct stat st;
    if (::stat(SrcName, &st) == -1 || S_ISDIR(st.st_mode))
        return 0;
    if (!strcmp(SrcName, DstName)) return TRUE;

    FILE *fr = FOpenFile(SrcName, "rb");
    if (!fr)
        return FALSE;

    FILE *fw = FOpenFile(DstName, "wb");
    if (!fw)
    {
        fclose(fr);
        return FALSE;
    }

    char _buff[256];
    DWORD Len = 1024 * 1024;
    char *buff = (char *) HMALLOC(Len);
    if (!buff)
    {
        buff = _buff;
        Len = 256L;
    }
    struct stat buf;
    fstat(fileno(fr), &buf);
    int size = buf.st_size;
    BOOL fError = 0;
//    BOOL fCancel = FALSE;
    for (DWORD len = min(Len, (DWORD) size); size > 0L; len = min(Len, (DWORD) size))
    {

        if (fread(buff, 1, (size_t) len, fr) != (size_t) len)
        {
            fError = TRUE;
            break;
        }
        if (fwrite(buff, 1, (size_t) len, fw) != (size_t) len)
        {
            fError = TRUE;
            break;
        }
        size -= len;
        if (func)
            if (func(SrcName, st.st_size, st.st_size - size, lpUserData) == PROGRESS_CANCEL)
                return 0;
    }
    if (func)
        if (func(SrcName, st.st_size, st.st_size, lpUserData) == PROGRESS_CANCEL)
            return 0;
    fclose(fr);
    fclose(fw);
    if (fMove)
        DeleteFile(SrcName);
    if (buff != _buff)
        HFREE(buff);
    return fError == 0;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------

struct _mDirHandle {
    enum TYPE_DIR {
        REAL, ASSETS
    };

    _mDirHandle(TYPE_DIR d)
    { type = d; }

    virtual ~_mDirHandle()
    {}

    TYPE_DIR type;
};

struct mRealDir : public _mDirHandle {
    DIR *ptr;
    char mask[MAX_PATH];
    char path[MAX_PATH];

    mRealDir() : _mDirHandle(REAL)
    {
        ptr = 0;
        memset(mask, 0, MAX_PATH);
        memset(path, 0, MAX_PATH);

    }

    virtual ~mRealDir()
    {}

};

#ifdef ANDROID_NDK

struct mAssetsDir : public _mDirHandle {
    MVector<TString> dir;
    MVector<TString> files;
    MArray<int> sizes;

    mAssetsDir() : _mDirHandle(ASSETS)
    {}

    bool Init(TString assets, TString mask)
    {
        int fn;
        if ((fn = assets.Find("ASSETS/")) == -1) return 0;

        assets.Replace("ASSETS/", "");
        MGetApp()->ReadResourceDir(assets.GetPtr(), mask.GetPtr(), dir, files);
        return dir.GetLen() + files.GetLen() > 0;
    }

    BOOL FindNextFile(LPWIN32_FIND_DATAA ret)
    {
        if (dir.GetLen())
        {
            memset(ret, 0, sizeof(WIN32_FIND_DATAA));
            strcpy(ret->cFileName, dir[0].GetPtr());
            ret->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
            dir.Remove(0, 1);
            return true;
        }
        if (files.GetLen())
        {
            memset(ret, 0, sizeof(WIN32_FIND_DATAA));
            strcpy(ret->cFileName, files[0].GetPtr());
            ret->dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
            files.Remove(0, 1);
            return true;
        }
        return 0;
    }
};

#endif
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
EXPORTNIX BOOL FindClose(HANDLE hFindFile)
{
    if (hFindFile == INVALID_HANDLE_VALUE) return FALSE;
    _mDirHandle *dir = (_mDirHandle *) hFindFile;
    delete dir;
    return TRUE;
}
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
EXPORTNIX BOOL FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATAA ret)
{
    if (hFindFile == INVALID_HANDLE_VALUE) return FALSE;
    _mDirHandle *_dir = (_mDirHandle *) hFindFile;
#ifdef ANDROID_NDK

    if (_dir->type == _mDirHandle::ASSETS)
    {
        return ((mAssetsDir *) _dir)->FindNextFile(ret);
    } else
#endif
    if (_dir->type != _mDirHandle::REAL)
        return 0;
    mRealDir *dir = (mRealDir *) hFindFile;

    TString dn;
    dirent *dp;
    while ((dp = readdir(dir->ptr)) != NULL)
        if (!dir->mask[0] || !fnmatch(dir->mask, dp->d_name, 0))
        {
            dn = dir->path;
            dn += dp->d_name;
            struct stat st;
            if (stat(dn.GetPtr(), &st) != -1)
            {

                if (!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode))
                    continue;
                struct tm *t = localtime((time_t *) &st.st_ctime);
                COleDateTime ct(*t);
                ct.GetDate(&ret->ftCreationTime);
                t = localtime((time_t *) &st.st_mtime);
                ct.SetDate(*t);
                ct.GetDate(&ret->ftLastAccessTime);
                ret->ftLastWriteTime = ret->ftLastAccessTime;
                LARGE_INTEGER li;
                li.QuadPart = st.st_size;
                ret->nFileSizeHigh = li.HighPart;
                ret->nFileSizeLow = li.LowPart;
                strcpy(ret->cFileName, dp->d_name);
                ret->cAlternateFileName[0] = 0;
                ret->dwFileAttributes = 0;
                if (S_ISDIR(st.st_mode))
                    ret->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
                else
                    ret->dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
                if (!(st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)))
                    ret->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
                return TRUE;
            }
        }
    return FALSE;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
EXPORTNIX HANDLE FindFirstFile(LPCSTR lpFileName, LPWIN32_FIND_DATAA ret)
{
    TString ph = lpFileName;
    int e = ph.ReverseFind(FD);
    if (e == -1) return INVALID_HANDLE_VALUE;
    TString Path, mask;
    Path = ph.Mid(0, e);
    if (e != ph.StrLen())
        mask = ph.Tail(e + 1);
    if (mask == "*.*")
        mask = "*";
#ifdef ANDROID_NDK

    mAssetsDir *test = new mAssetsDir();
    if (test->Init(Path, mask))
    {
        if (!FindNextFile((HANDLE) test, ret))
        {
            delete test;
            return INVALID_HANDLE_VALUE;
        }
        return (HANDLE) test;
    } else
        delete test;
#endif
    if (Path.Right(1) != SFD)
        Path += SFD;
    DIR *ptr = opendir(Path.GetPtr());
    if (!ptr) return INVALID_HANDLE_VALUE;
    mRealDir *dir = new mRealDir;
    strcpy(dir->path, Path.GetPtr());
    dir->ptr = ptr;
    if (mask.StrLen())
        strcpy(dir->mask, mask.GetPtr());
    if (!FindNextFile((HANDLE) dir, ret))
    {
        delete dir;
        return INVALID_HANDLE_VALUE;
    }
    return (HANDLE) dir;
}
EXPORTNIX int CreateDirectoryRetErr(LPCSTR a, DWORD b)
{
    int er = mkdir(a, b | S_IRWXU);
    if (er < 0)
    {
        er = errno;
        switch (er)
        {
            case EEXIST:
                return DIR_ERROR_ALREADY_EXISTS;
            case ENAMETOOLONG:
            case EPERM:
            case EBADF:
            case ENOTDIR:
                return DIR_ERROR_INVALID_NAME;


        }
        return DIR_ERROR;
    }
    return 1;
}
EXPORTNIX int CreateDirectoryAlways(LPCSTR _dir)
{
    TString str = _dir;
    MVector<TString> tock;
    str.Tokenize("\\/", tock);
    TString path = "/";
    for (int i = 0, l = tock.GetLen(); i < l; ++i)
    {
        path += tock[i];
        if (GetFileAttributes(path.GetPtr()) == 0xffffffff)
        {
            int ret;
            if ((ret = CreateDirectoryRetErr(path.GetPtr(), 0)) != 1) return ret;
        }
        path += "/";

    }
    return 1;
}
EXPORTNIX BOOL GetFileTime(TString path, FILETIME *ret)
{

    if (path.StrLen())
    {
        struct stat st;
        while (path.StrLen() > 1 && path.Right(1) == SFD)
            path[path.StrLen() - 1] = 0;

        if (stat(path.GetPtr(), &st) != -1)
        {

            struct tm *t = localtime((time_t *) &st.st_mtime);
            COleDateTime ct(*t);
            ct.GetDate(ret);
            return TRUE;

        }
    }
    return FALSE;
}
EXPORTNIX BOOL GetFileTime(FILEHANDLE hFile, FILETIME *ret)
{

    if (hFile != INVALID_HANDLE_VALUE)
    {
        struct stat st;
        if (fstat(hFile, &st) != -1)
        {

            struct tm *t = localtime((time_t *) &st.st_mtime);
            COleDateTime ct(*t);
            ct.GetDate(ret);
            return TRUE;

        }
    }
    return FALSE;
}
