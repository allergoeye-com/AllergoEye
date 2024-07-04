#include "stdafx.h"

#include "_ffile.h"


#ifndef __PLUK_LINUX__
#include <io.h>
#include <stdio.h>

#endif
#ifdef ANDROID_NDK

extern bool CreateTmpFile(TString &assets);

#endif

bool FFile::Open(const char *filename, const char *attr)
{
    hFile = 0;
#ifdef ANDROID_NDK
    TString assets = filename;
    if (CreateTmpFile(assets))
    {
        filename = assets.GetPtr();
        fDel = true;
        hFile = fopen(filename, attr);
    }
#endif
    if (hFile || (hFile = FOpenFile(filename, attr)))
    {
        m_FileName = (char *) HMALLOC(strlen(filename) + 1);
        m_Attr = (char *) HMALLOC(strlen(attr) + 1);
        strcpy(m_Attr, attr);
        strcpy(m_FileName, filename);

    }
    return hFile != 0;
}

//-------------------------------------
//
//-------------------------------------
int FFile::GetSeek()
{
    return hFile ? ftell(hFile) : EOF;
}

//-------------------------------------
//
//-------------------------------------
int FFile::SetSeek(int offset)
{
    return hFile ? (!fseek(hFile, offset, SEEK_SET) ? ftell(hFile) : -1) : EOF;

}

//-------------------------------------
//
//-------------------------------------
int FFile::GetSize()
{
    if (hFile)
    {
        int pos = ftell(hFile);
        fseek(hFile, 0, SEEK_END);
        int n = ftell(hFile);
        fseek(hFile, pos, SEEK_SET);
        return n;
    }
    return EOF;
}

//-------------------------------------
//
//-------------------------------------
bool FFile::Close()
{
    int fc = 0;
    if (hFile)
        fc = fclose(hFile);
#ifdef ANDROID_NDK
    if (fDel)
        DeleteFile(m_FileName);
#endif
    hFile = 0;
    if (m_FileName)
        HFREE(m_FileName);
    if (m_Attr)
        HFREE(m_Attr);
    m_FileName = m_Attr = 0;
    return fc == 0;
}

//-------------------------------------
//
//-------------------------------------
bool FFile::Write(int offset, const void *s, int size)
{
    if (hFile && s && size)
    {

        if (offset != -1)
        {
            if (offset != ftell(hFile))
                SetSeek(offset);
        } else
            offset = ftell(hFile);
        if (fwrite(s, 1, (size_t) size, hFile) > 0)
            return true;
    }
    return false;
}

//-------------------------------------
//
//-------------------------------------
off_t FFile::Read(int offset, void *s, int size)
{
    if (hFile)
    {
        if (offset != -1)
        {
            int n = ftell(hFile);
            if (offset != n)
                SetSeek(offset);
        } else
            offset = ftell(hFile);
        if (fread(s, 1, size, hFile) > 0)
            return size;
    }
    return 0;
}

//-------------------------------------
//
//-------------------------------------
bool FFile::SetSize(int size)
{
    if (hFile)
    {
        int offset = ftell(hFile);
#ifdef __PLUK_LINUX__
        ftruncate(fileno(hFile), size);
#else
        _chsize(_fileno(hFile), size);
#endif
        if (offset < size)
            fseek(hFile, offset, SEEK_SET);
        else
            fseek(hFile, 0, SEEK_END);

        return true;
    }
    return 0;
}

//-------------------------------------
//
//-------------------------------------
bool FFile::Flush()
{
    return hFile ? fflush(hFile) != EOF : false;
}

//-------------------------------------
//
//-------------------------------------
FFileINI::FFileINI(const char *fname, bool fAddPathDir)
{

    TString fn;
    if (fAddPathDir)
    {
#ifndef __PLUK_LINUX__
        TCHAR szPathName[_MAX_PATH];

        GetModuleFileName(0, szPathName, _MAX_PATH);
        LPTSTR pszFileName = _tcsrchr(szPathName, FD) + 1;
        *pszFileName = 0;
        fn = szPathName;
#else
#if (0)
        fn.SetLen(255);
        fn.Set(0);
        while(!getcwd(fn.GetPtr(), fn.GetLen() - 1))
        {
            fn.SetLen(fn.GetLen() * 2);
            fn.Set(0);
        }
#else
        fn = MGetApp()->sPath;
#endif
#endif
        if (fn[fn.StrLen() - 1] != FD)
            fn += SFD;
    }
    fn += fname;
    if (!Open(fn.GetPtr(), "rb+"))
        Open(fn.GetPtr(), "wb+");
}

//-------------------------------------
//
//-------------------------------------
bool FFileINI::_Write(DWORD offset, const void *s, DWORD size)
{
    if (!IsOpen()) return 0;

    SetSize(size);
    if (FFile::Write(0, s, size))
    {
        FFile::Flush();
        return true;
    }
    return false;
}

//-------------------------------------
//
//-------------------------------------
void FFileINI::SectionNames(MVector<TString> &res)
{
    MHashMap<_RBMapSimple(TString, TString), TString> m_data;
    MVector<TString> sections;
    ReadIni(m_data, sections);

    for (int i = 0, len = sections.GetLen(); i < len; ++i)
        res.Add() = sections[i];
}

//-------------------------------------
//
//-------------------------------------
bool FFileINI::Parse(TString &data, MHashMap<_RBMapSimple(TString, TString), TString> &m_data,
                     MVector<TString> &sections)
{
    sections.Clear();
    m_data.Clear();
    int j, len1, i, len;
    char *p = data.GetPtr();
    /*for (i = 0, len = data.GetLen() - 1; i < len; ++i)
    {
        if (p[i] != '\r')
            if (p[i] != '\n')
                if (!isprint(p[i]))
                {
                    data.Remove(i, 1);
                    p = data.GetPtr();
                    --len;
                    --i;
                }
    }
*/
    MVector<TString> v;
    data.Tokenize("\r\n", v);
    i = 0;
    len = v.GetLen();
    while (i < len && v[i].Find("[") == -1)
        ++i;
    if (i > 0)
        v.Remove(0, i);
    i = 0;
    len = v.GetLen();
    while (i < len)
    {
        v[i].Replace("[", "");
        v[i].Replace("]", "");
        v[i].ClearSpace();
        v[i].Upper();
        if (!m_data.Lookup(v[i]))
            sections.Add(v[i]);
        _RBMapSimple(TString, TString) &sec = m_data[v[i]];

        ++i;
        while (i < len)
        {
            if (v[i].Find("[") != -1 && v[i].Find("]") != -1)
                break;

            /*	MVector<TString> v1;
                v[i].Tokenize("=", v1);
                ++i;
                len1 = v1.GetLen() > 2 ? 2 : v1.GetLen();
                for (j = 0; j < len1; ++j)
                    v1[j].ClearSpace();
                v1[0].Upper();
                if (len1 == 2)
                    sec[v1[0]] = v1[1];
                else
                    if (len1 == 1)
                        sec[v1[0]] = "";
            */
            int k = v[i].Find('=');
            if (k != -1)
            {
                int l = v[i].StrLen();
                TString s2, s1 = v[i].Left(k);
                s1.ClearSpace();
                if (l > k + 1)
                {
                    s2 = v[i].Right(l - k - 1);
                    s2.ClearSpace();
                }
                s1.Upper();
                sec[s1] = s2;
            }
            ++i;


        }
    }
    return m_data.Len() != 0;
}

//-------------------------------------
//
//-------------------------------------
TString FFileINI::GetString(TString Section, TString Entry, const char *lpszDefault)
{
    if (IsOpen())
    {
        MHashMap<_RBMapSimple(TString, TString), TString> m_data;
        MVector<TString> sections;
        ReadIni(m_data, sections);
        Section.Upper();
        Section.ClearSpace();
        Entry.Upper();
        Entry.ClearSpace();
        _RBMapSimple(TString, TString) *mp;
        if (m_data.Lookup(Section, &mp))
        {
            if (mp->Lookup(Entry))
                return (*mp)[Entry];
        }
    }
    TString tmp(lpszDefault);
    return tmp;

}

//-------------------------------------
//
//-------------------------------------
BOOL FFileINI::IsValid()
{
    if (IsOpen())
    {
        int size = GetSize();
        TString data;
        data.SetLen(size);
        Read(0, data.GetPtr(), size);
        return size == data.StrLen();
    }
    return 0;
}

//-------------------------------------
//
//-------------------------------------
void FFileINI::ReadIni(MHashMap<_RBMapSimple(TString, TString), TString> &m_data,
                       MVector<TString> &sections)
{
    if (IsOpen())
    {
        DWORD size = GetSize();
        TString data;
        data.SetLen(size);
        Read(0, data.GetPtr(), size);
        Parse(data, m_data, sections);
    }
}

//-------------------------------------
//
//-------------------------------------
bool FFileINI::WriteIni(MHashMap<_RBMapSimple(TString, TString), TString> &m_data,
                        MVector<TString> &sections)
{
    if (IsOpen())
    {
        TString data;
        for (int i = 0, len = sections.GetLen(); i < len; ++i)
        {
            data += "[";
            data += sections[i];
            data += "]\r\n";

            _RBMapSimple(TString, TString)::iterator it_e(m_data[sections[i]]);
            for (RBData<TString, TString> *e = it_e.begin(); e; e = it_e.next())
            {
                data += e->key;
                data += "=";
                data += e->value;
                data += "\r\n";

            }
            data += "\r\n";
        }
        return _Write(0, data.GetPtr(), data.StrLen());
    }
    return false;
}

//-------------------------------------
//
//-------------------------------------
void FFileINI::DeleteSection(TString Section)
{
    if (!IsOpen() || !Section.StrLen())
        return;
    MHashMap<_RBMapSimple(TString, TString), TString> m_data;
    MVector<TString> sections;
    ReadIni(m_data, sections);
    Section.Upper();
    Section.ClearSpace();
    m_data.Delete(Section);
    int n = sections.Find(Section);
    if (n != -1)
    {
        sections.Remove(n, 1);
    }
    WriteIni(m_data, sections);
}

//-------------------------------------
//
//-------------------------------------
bool FFileINI::WriteString(TString Section, TString Entry, const char *lpszValue)
{
    if (!IsOpen() || !Section.StrLen() || !Entry.StrLen())
        return 0;
    MHashMap<_RBMapSimple(TString, TString), TString> m_data;
    MVector<TString> sections;
    ReadIni(m_data, sections);

    Section.Upper();
    Section.ClearSpace();
    Entry.Upper();
    Entry.ClearSpace();
    if (!m_data.Lookup(Section))
        sections.Add() = Section;

    _RBMapSimple(TString, TString) &mp = m_data[Section];
    mp[Entry] = lpszValue;
    return WriteIni(m_data, sections);


}

//-------------------------------------
//
//-------------------------------------
int FFileINI::GetInt(TString Section, TString Entry, int iDefault)
{
    if (!IsOpen())
    {
        return iDefault;
    }

    TString res = GetString(Section, Entry);
    if (!res.StrLen())
        return iDefault;
    return atoi(res.GetPtr());

}

//-------------------------------------
//
//-------------------------------------
int FFileINI::GetIntHex(TString Section, TString Entry, int iDefault)
{
    if (!IsOpen())
    {
        return iDefault;
    }

    TString res = GetString(Section, Entry);
    if (!res.StrLen())
        return iDefault;
    int dw;
    sscanf(res.GetPtr(), "%x", &dw);
    return dw;
}

//-------------------------------------
//
//-------------------------------------
bool FFileINI::WriteInt(TString Section, TString Entry, int iValue)
{
    if (!IsOpen())
    {
        return 0;
    }
    TString res;
    res.Format("%d", iValue);
    return WriteString(Section, Entry, res);
}

//-------------------------------------
//
//-------------------------------------
bool FFileINI::WriteIntHex(TString Section, TString Entry, int iValue)
{
    if (!IsOpen())
    {
        return 0;
    }
    TString res;
    res.Format("%x", iValue);
    return WriteString(Section, Entry, res);
}

//-------------------------------------
//
//-------------------------------------
bool FFileINI::GetSectionEntry(TString Section, _RBMapSimple(TString, TString) &sc)
{
    MHashMap<_RBMapSimple(TString, TString), TString> m_data;
    MVector<TString> sections;
    ReadIni(m_data, sections);
    Section.Upper();
    Section.ClearSpace();
    sc.Clear();
    if (m_data.Lookup(Section))
        sc = m_data[Section];
    return sc.m_iNum != 0;
}

//-------------------------------------
//
//-------------------------------------
double FFileINI::GetDouble(TString Section, TString Entry, double fDefault)
{
    if (!IsOpen())
    {
        return fDefault;
    }
    TString res = GetString(Section, Entry);
    if (!res.StrLen())
        return fDefault;
    return atof(res.GetPtr());
}

//-------------------------------------
//
//-------------------------------------
bool FFileINI::WriteDouble(TString Section, TString Entry, double fValue)
{
    if (!IsOpen()) return 0;
    TString res;
    res.Format("%lf", fValue);

    return WriteString(Section, Entry, res);

}

//-------------------------------------
//
//-------------------------------------
FFileINIW::FFileINIW(const char *fname, bool fAddPathDir)
{

    TString fn;
    if (fAddPathDir)
    {
#ifndef __PLUK_LINUX__
        TCHAR szPathName[_MAX_PATH];

        GetModuleFileName(0, szPathName, _MAX_PATH);
        LPTSTR pszFileName = _tcsrchr(szPathName, FD) + 1;
        *pszFileName = 0;
        fn = szPathName;
#else
#if (0)
        fn.SetLen(255);
        fn.Set(0);
        while(!getcwd(fn.GetPtr(), fn.GetLen() - 1))
        {
            fn.SetLen(fn.GetLen() * 2);
            fn.Set(0);
        }
#else
        fn = MGetApp()->sPath;
#endif
        if (fn[fn.StrLen() - 1] != FD)
            fn += SFD;

#endif
    }
    fn += fname;
    if (!Open(fn.GetPtr(), "rb+"))
        Open(fn.GetPtr(), "wb+");

}

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::_Write(DWORD offset, const void *s, DWORD size)
{
    if (!IsOpen()) return 0;

    SetSize(size);
    if (FFile::Write(0, s, size))
    {
        FFile::Flush();
        return true;
    }
    return false;
}

//-------------------------------------
//
//-------------------------------------
void FFileINIW::SectionNames(MVector<WString> &res)
{
    MHashMap<_RBMapSimple(WString, WString), WString> m_data;
    MVector<WString> sections;
    ReadIni(m_data, sections);

    for (int i = 0, len = sections.GetLen(); i < len; ++i)
        res.Add() = sections[i];
}

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::Parse(WString &data, MHashMap<_RBMapSimple(WString, WString), WString> &m_data,
                      MVector<WString> &sections)
{
    sections.Clear();
    m_data.Clear();
    // int n = sizeof(wchar_t);
    int j, i, len;
    wchar_t *p = data.GetPtr();
    for (i = 0, len = data.GetLen() - 1; i < len; ++i)
    {
        if (p[i] != L'\r')
            if (p[i] != L'\n')
            {
                if (!iswprint(p[i]))
                {
                    data.Remove(i, 1);
                    p = data.GetPtr();
                    --len;
                    --i;
                }
            }
    }

    MVector<WString> v;
    data.Tokenize(L"\r\n", v);
    i = 0;
    len = v.GetLen();
    while (i < len && v[i].Find(L"[") == -1)
        ++i;
    if (i > 0)
        v.Remove(0, i);
    i = 0;
    len = v.GetLen();
    while (i < len)
    {
        v[i].TrimLeft(L' ');
        v[i].TrimRight(L' ');
        int i1 = v[i].Find(L"[");
        int i2 = v[i].Find(L"]");
        if (i1 == -1 || i2 == -1)
        {
            ++i;
            continue;
        } else
            v[i] = v[i].Mid(i1 + 1, i2 - i1 - 1);
        v[i].Upper();
        if (!m_data.Lookup(v[i]))
            sections.Add(v[i]);
        _RBMapSimple(WString, WString) &sec = m_data[v[i]];

        ++i;
        if (i < len)
        {
            v[i].TrimLeft(L' ');
            v[i].TrimRight(L' ');
        }
        while (i < len) // && v[i][0] != L'[' && v[i].Find(L"]") != -1)
        {
            WString v1, v2;
            //MVector<WString> v1;
            //v[i].Tokenize(L"=", v1);

            if ((j = v[i].Find(L'=')) == -1)
                break;
            v1 = v[i].Left(j);
            v2 = v[i].Tail(j + 1);
            //len1 = v1.GetLen() > 2 ? 2 : v1.GetLen();
            //for (j = 0; j < len1; ++j)
            //	v1[j].ClearSpace();
            v1.Upper();


            sec[v1] = v2;
            ++i;
            if (i < len)
            {
                v[i].TrimLeft(L' ');
                v[i].TrimRight(L' ');

                int i0 = v[i].Find(L"=");
                int i1 = v[i].Find(L"[");
                int i2 = v[i].Find(L"]");
                if (i2 != -1 && i1 != -1)
                {
                    if (i0 == -1)
                        break;
                }
            }

        }
    }
    return m_data.Len() != 0;
}

//-------------------------------------
//
//-------------------------------------
WString FFileINIW::GeWString(WString Section, WString Entry, const wchar_t *lpszDefault)
{
    if (IsOpen())
    {
        MHashMap<_RBMapSimple(WString, WString), WString> m_data;
        MVector<WString> sections;
        ReadIni(m_data, sections);
        Section.Upper();
        Section.ClearSpace();
        Entry.Upper();
        Entry.ClearSpace();
        _RBMapSimple(WString, WString) *mp;
        if (m_data.Lookup(Section, &mp))
        {
            if (mp->Lookup(Entry))
                return (*mp)[Entry];
        }
    }
    WString tmp(lpszDefault);
    return tmp;

}

//-------------------------------------
//
//-------------------------------------
BOOL FFileINIW::IsValid()
{
    if (IsOpen())
    {
        DWORD size = GetSize();
        MHashMap<_RBMapSimple(WString, WString), WString> m_data;
        if (size)
        {
            MVector<WString> sections;
            ReadIni(m_data, sections);
            return !size || sections.GetLen();
        }
    }
    return 0;
}

//-------------------------------------
//
//-------------------------------------
void FFileINIW::ReadIni(MHashMap<_RBMapSimple(WString, WString), WString> &m_data,
                        MVector<WString> &sections)
{
    if (IsOpen())
    {
        DWORD size = GetSize();
        WString data;
        data.SetLen(size / sizeof(short));
        MArray<USHORT> tmp(size / sizeof(short));
        Read(0, tmp.GetPtr(), size);
        for (int i = 0, l = size / sizeof(short); i < l; ++i)
            data[i] = tmp[i];
        Parse(data, m_data, sections);
    }
}

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::WriteIni(MHashMap<_RBMapSimple(WString, WString), WString> &m_data,
                         MVector<WString> &sections)
{
    if (IsOpen())
    {
        WString data;
        for (int i = 0, len = sections.GetLen(); i < len; ++i)
        {
            data += L"[";
            data += sections[i];
            data += L"]\r\n";

            _RBMapSimple(WString, WString)::iterator it_e(m_data[sections[i]]);
            for (RBData<WString, WString> *e = it_e.begin(); e; e = it_e.next())
            {
                data += e->key;
                data += L"=";
                data += e->value;
                data += L"\r\n";

            }
            data += L"\r\n";
        }
        MArray<USHORT> tmp(data.StrLen());
        for (int i = 0, l = data.StrLen(); i < l; ++i)
            tmp[i] = (USHORT) data[i];

        return _Write(0, tmp.GetPtr(), data.StrLen() * sizeof(USHORT));
    }
    return false;
}

//-------------------------------------
//
//-------------------------------------
void FFileINIW::DeleteSection(WString Section)
{
    if (!IsOpen() || !Section.StrLen())
        return;
    MHashMap<_RBMapSimple(WString, WString), WString> m_data;
    MVector<WString> sections;
    ReadIni(m_data, sections);
    Section.Upper();
    Section.ClearSpace();
    m_data.Delete(Section);
    int n = sections.Find(Section);
    if (n != -1)
    {
        sections.Remove(n, 1);
    }
    WriteIni(m_data, sections);
}

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::WriteString(WString Section, WString Entry, const wchar_t *lpszValue)
{
    if (!IsOpen() || !Section.StrLen() || !Entry.StrLen())
        return 0;
    MHashMap<_RBMapSimple(WString, WString), WString> m_data;
    MVector<WString> sections;
    ReadIni(m_data, sections);

    Section.Upper();
    Section.ClearSpace();
    Entry.Upper();
    Entry.ClearSpace();
    if (!m_data.Lookup(Section))
        sections.Add() = Section;

    _RBMapSimple(WString, WString) &mp = m_data[Section];
    mp[Entry] = lpszValue;
    return WriteIni(m_data, sections);


}

//-------------------------------------
//
//-------------------------------------
int FFileINIW::GetInt(WString Section, WString Entry, int iDefault)
{
    if (!IsOpen())
    {
        return iDefault;
    }

    TString res;
    res = GeWString(Section, Entry);
    if (!res.StrLen())
        return iDefault;
    return atoi(res.GetPtr());

}

//-------------------------------------
//
//-------------------------------------
int FFileINIW::GetIntHex(WString Section, WString Entry, int iDefault)
{
    if (!IsOpen())
    {
        return iDefault;
    }

    TString res;
    res = GeWString(Section, Entry);
    if (!res.StrLen())
        return iDefault;
    int dw;
    sscanf(res.GetPtr(), "%x", &dw);
    return dw;
}

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::WriteInt(WString Section, WString Entry, int iValue)
{
    if (!IsOpen())
    {
        return 0;
    }
    WString res;
    res.Format("%d", iValue);
    return WriteString(Section, Entry, res);
}

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::WriteIntHex(WString Section, WString Entry, int iValue)
{
    if (!IsOpen())
    {
        return 0;
    }
    WString res;
    res.Format("%x", iValue);
    return WriteString(Section, Entry, res);
}

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::GetSectionEntry(WString Section, _RBMapSimple(WString, WString) &sc)
{
    MHashMap<_RBMapSimple(WString, WString), WString> m_data;
    MVector<WString> sections;
    ReadIni(m_data, sections);
    Section.Upper();
    Section.ClearSpace();
    sc.Clear();
    if (m_data.Lookup(Section))
        sc = m_data[Section];
    return sc.m_iNum != 0;
}

//-------------------------------------
//
//-------------------------------------
double FFileINIW::GetDouble(WString Section, WString Entry, double fDefault)
{
    if (!IsOpen())
    {
        return fDefault;
    }
    TString res;
    res = GeWString(Section, Entry);
    if (!res.StrLen())
        return fDefault;
    return atof(res.GetPtr());
}

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::WriteDouble(WString Section, WString Entry, double fValue)
{
    if (!IsOpen()) return 0;
    WString res;
    res.Format("%lf", fValue);

    return WriteString(Section, Entry, res);

}

#ifdef __PLUK_LINUX__
//-------------------------------------
//
//-------------------------------------
extern "C++" DWORD GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault,
                                            LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
    FFileINI file(lpFileName, FALSE);
    if (!lpKeyName || !*lpKeyName)
    {
        _RBMapSimple(TString, TString) sc;
        file.GetSectionEntry(lpAppName, sc);
        _RBMapSimple(TString, TString)::iterator it(sc);
        MArray<char> buff;
        for (RBData<TString, TString> *d = it.begin(); d; d = it.next())
        {
            buff.Append(d->key.GetPtr(), d->key.StrLen());
            buff.Add() = 0;
        }
        buff.Add() = 0;
        if (lpReturnedString)
        {
            if (nSize < buff.GetLen())
            {
                if (nSize - 1 > 0)
                    buff[nSize - 1] = 0;
                if (nSize - 2 > 0)
                    buff[nSize - 2] = 0;
            } else
                nSize = buff.GetLen();
            memcpy(lpReturnedString, buff.GetPtr(), nSize);

        }
        return buff.GetLen();

    }

    TString res = file.GetString(lpAppName, lpKeyName);
    if (res.StrLen() && res.StrLen() < nSize)
        strcpy(lpReturnedString, res.GetPtr());
    else
    {
        if (lpDefault)
            strcpy(lpReturnedString, lpDefault);
        return 0;
    }
    return res.StrLen();

}
//-------------------------------------
//
//-------------------------------------
extern "C++" int
GetPrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName)
{
    FFileINI file(lpFileName, FALSE);
    return file.GetInt(lpAppName, lpKeyName, nDefault);

}

//-------------------------------------
//
//-------------------------------------
extern "C++" BOOL
WritePrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName)
{
    FFileINI file(lpFileName, FALSE);
    return file.WriteString(lpAppName, lpKeyName, lpString);
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int
GetPrivateProfileSectionA(LPCSTR lpAppName, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
    FFileINI file(lpFileName, FALSE);
    _RBMapSimple(TString, TString) sc;
    file.GetSectionEntry(lpAppName, sc);
    _RBMapSimple(TString, TString)::iterator it(sc);
    MArray<char> buff;
    for (RBData<TString, TString> *d = it.begin(); d; d = it.next())
    {
        buff.Append(d->key.GetPtr(), d->key.StrLen());
        buff.Add() = '=';
        buff.Append(d->value.GetPtr(), d->value.StrLen());
        buff.Add() = 0;
    }
    buff.Add() = 0;
    if (lpReturnedString)
    {
        if (nSize < buff.GetLen())
        {
            if (nSize - 1 > 0)
                buff[nSize - 1] = 0;
            if (nSize - 2 > 0)
                buff[nSize - 2] = 0;
        } else
            nSize = buff.GetLen();
        memcpy(lpReturnedString, buff.GetPtr(), nSize);

    }
    return buff.GetLen();
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int
GetPrivateProfileSectionNamesA(LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
    FFileINI file(lpFileName, FALSE);
    MVector<TString> names;
    file.SectionNames(names);
    MArray<char> buff;
    for (int i = 0, l = names.GetLen(); i < l; ++i)
    {
        buff.Append(names[i].GetPtr(), names[i].StrLen());
        buff.Add() = 0;
    }
    buff.Add() = 0;
    if (lpReturnedString)
    {
        if (nSize < buff.GetLen())
        {
            if (nSize - 1 > 0)
                buff[nSize - 1] = 0;
            if (nSize - 2 > 0)
                buff[nSize - 2] = 0;
        } else
            nSize = buff.GetLen();
        memcpy(lpReturnedString, buff.GetPtr(), nSize);

    }
    return buff.GetLen();

}
//-------------------------------------
//
//-------------------------------------
EXPORTNIX TString DefaultIniNameA()
{
    TString defIniName = AfxGetApp()->ModuleName;
    int i1 = defIniName.ReverseFind(FD);

    if (i1 != -1)
        defIniName = defIniName.Tail(i1 + 1);

    defIniName.Replace(".exe", "");
    defIniName += ".ini";
    return defIniName;
}

//-------------------------------------
//
//-------------------------------------
WString DefaultIniNameW()
{
    WString defIniName;
    defIniName = DefaultIniNameA();
    return defIniName;
}

//-------------------------------------
//
//-------------------------------------
extern "C++" BOOL WriteProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString)
{
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, DefaultIniNameA());
}
//-------------------------------------
//
//-------------------------------------
extern "C++" DWORD
GetProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString,
                  DWORD nSize)
{
    return GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize,
                                    DefaultIniNameA());
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault)
{
    return GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, DefaultIniNameA());

}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileSectionA(LPCSTR lpAppName, LPSTR lpReturnedString, DWORD nSize)
{
    return GetPrivateProfileSectionA(lpAppName, lpReturnedString, nSize, DefaultIniNameA());

}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileSectionNamesA(LPSTR lpszReturnBuffer, DWORD nSize)
{
    return GetPrivateProfileSectionNamesA(lpszReturnBuffer, nSize, DefaultIniNameA());
}

//-------------------------------------
//
//-------------------------------------
extern "C++" DWORD GetPrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault,
                                            LPWSTR lpReturnedString, DWORD nSize,
                                            LPCWSTR lpFileName)
{
    TString s = lpFileName;
    FFileINIW file(s.GetPtr(), FALSE);
    WString res = file.GeWString(lpAppName, lpKeyName);
    if (res.StrLen() && res.StrLen() < nSize)
        wcscpy(lpReturnedString, res.GetPtr());
    else
    {
        if (lpDefault)
            wcscpy(lpReturnedString, lpDefault);
        return 0;
    }
    return res.StrLen();

}
//-------------------------------------
//
//-------------------------------------
extern "C++" int
GetPrivateProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName)
{
    TString s = lpFileName;
    FFileINIW file(s.GetPtr(), FALSE);
    return file.GetInt(lpAppName, lpKeyName, nDefault);

}

//-------------------------------------
//
//-------------------------------------
extern "C++" BOOL WritePrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString,
                                             LPCWSTR lpFileName)
{
    TString s = lpFileName;
    FFileINIW file(s.GetPtr(), FALSE);
    return file.WriteString(lpAppName, lpKeyName, lpString);
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetPrivateProfileSectionW(LPCWSTR lpAppName, LPWSTR lpReturnedString, DWORD nSize,
                                           LPCWSTR lpFileName)
{
    TString s = lpFileName;
    FFileINIW file(s.GetPtr(), FALSE);
    _RBMapSimple(WString, WString) sc;
    file.GetSectionEntry(lpAppName, sc);
    _RBMapSimple(WString, WString)::iterator it(sc);
    MArray<wchar_t> buff;
    for (RBData<WString, WString> *d = it.begin(); d; d = it.next())
    {
        buff.Append(d->key.GetPtr(), d->key.StrLen());
        buff.Add() = '=';
        buff.Append(d->value.GetPtr(), d->value.StrLen());
        buff.Add() = 0;
    }
    buff.Add() = 0;
    if (lpReturnedString)
    {
        if (nSize < buff.GetLen())
        {
            if (nSize - 1 > 0)
                buff[nSize - 1] = 0;
            if (nSize - 2 > 0)
                buff[nSize - 2] = 0;
        } else
            nSize = buff.GetLen();
        memcpy(lpReturnedString, buff.GetPtr(), nSize * sizeof(wchar_t));

    }
    return buff.GetLen();
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int
GetPrivateProfileSectionNamesW(LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName)
{
    TString s = lpFileName;
    FFileINIW file(s.GetPtr(), FALSE);
    MVector<WString> names;
    file.SectionNames(names);
    MArray<wchar_t> buff;
    for (int i = 0, l = names.GetLen(); i < l; ++i)
    {
        buff.Append(names[i].GetPtr(), names[i].StrLen());
        buff.Add() = 0;
    }
    buff.Add() = 0;
    if (lpReturnedString)
    {
        if (nSize < buff.GetLen())
        {
            if (nSize - 1 > 0)
                buff[nSize - 1] = 0;
            if (nSize - 2 > 0)
                buff[nSize - 2] = 0;
        } else
            nSize = buff.GetLen();
        memcpy(lpReturnedString, buff.GetPtr(), nSize * sizeof(wchar_t));

    }
    return buff.GetLen();

}
//-------------------------------------
//
//-------------------------------------
extern "C++" BOOL WriteProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString)
{

    return WritePrivateProfileStringW(lpAppName, lpKeyName, lpString, DefaultIniNameW());
}
//-------------------------------------
//
//-------------------------------------
extern "C++" DWORD
GetProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString,
                  DWORD nSize)
{
    return GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize,
                                    DefaultIniNameW());
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault)
{
    return GetPrivateProfileIntW(lpAppName, lpKeyName, nDefault, DefaultIniNameW());

}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileSectionW(LPCWSTR lpAppName, LPWSTR lpReturnedString, DWORD nSize)
{
    return GetPrivateProfileSectionW(lpAppName, lpReturnedString, nSize, DefaultIniNameW());

}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileSectionNamesW(LPWSTR lpszReturnBuffer, DWORD nSize)
{
    return GetPrivateProfileSectionNamesW(lpszReturnBuffer, nSize, DefaultIniNameW());

}

#endif
