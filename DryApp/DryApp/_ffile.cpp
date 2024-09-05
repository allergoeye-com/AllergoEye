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
	TString assets= filename;
	if (CreateTmpFile(assets))
	{
		filename = assets.GetPtr();
		fDel = true;
		hFile = fopen(filename, attr);
	}
#endif
	if (hFile || (hFile = fopen(filename, attr)))
	{
		m_FileName = (char *)HMALLOC(strlen(filename) + 1);
		m_Attr = (char *)HMALLOC(strlen(attr) + 1);
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
	return hFile ? (!fseek(hFile, offset, SEEK_SET) ? ftell(hFile) : -1)  : EOF;

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
		}
		else
		offset = ftell(hFile);
		if (fwrite(s, 1, (size_t)size, hFile) > 0)
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
		}
		else
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
		LPTSTR pszFileName = _tcsrchr(szPathName, L'\\') + 1;
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
		if (fn[fn.StrLen() - 1] != '\\')
			fn += "\\";
	}
	fn += fname;
#ifdef WIN32
	m_FileName = (char*)HMALLOC(fn.StrLen() + 1);
	strcpy(m_FileName, fn.GetPtr());
#endif

	if (!Open(fn.GetPtr(), "rb+"))
		Open(fn.GetPtr(), "wb+");
#ifdef WIN32
	if (hFile)
		fclose(hFile);
	hFile = 0;

#endif
}
//-------------------------------------
//
//-------------------------------------
void FFileINI::SectionNames(MVector<TString> &res)
{
#ifdef WIN32
	if (m_FileName)
	{
		MArray<char> val;
		int sz;
		int size = 128;
		val.SetLen(size);
		sz = GetPrivateProfileSectionNamesA(val.GetPtr(), size, m_FileName);
		if (sz > size - 2)
		{
			val.SetLen(sz + 4);
			sz = GetPrivateProfileSectionNamesA(val.GetPtr(), sz + 4, m_FileName);
		}
		if (sz)
		{
			char* p = val.GetPtr();
			while (p)
			{
				res.Add() = p;
				p += strlen(p) + 1;

			}

		}
	}
#else

	MHashMap<_RBMapSimple(TString, TString), TString> m_data;
	MVector<TString> sections;
	ReadIni(m_data, sections);

	for (int i = 0, len = sections.GetLen(); i < len; ++i)
		res.Add() = sections[i];
#endif
}

//-------------------------------------
//
//-------------------------------------
TString FFileINI::GetString(TString Section, TString Entry, const char *lpszDefault)
{

#ifdef WIN32

	if (m_FileName)
	{
		MArray<char> val;
		int sz;
		int size = 128;
		val.SetLen(size);


		sz = GetPrivateProfileStringA(Section.GetPtr(), Entry.GetPtr(), 0, val.GetPtr(), size - 1, m_FileName);
		if (sz > size - 1)
		{
			val.SetLen(sz + 2);
			sz = GetPrivateProfileStringA(Section.GetPtr(), Entry.GetPtr(), 0, val.GetPtr(), sz + 2, m_FileName);
		}
		return val.GetPtr();
	}
#else
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
#endif
	TString tmp(lpszDefault);
	return tmp;


}
//-------------------------------------
//
//-------------------------------------
BOOL FFileINI::IsValid()
{
#ifdef WIN32
	return m_FileName && GetFileAttributesA(m_FileName) != 0xffffffff;
#else
	if (IsOpen()) 
	{
		int size = GetSize();
		TString data;
		data.SetLen(size);
		Read(0, data.GetPtr(), size);
		return size == data.StrLen();
    }
    return 0;
#endif
}
#ifndef WIN32
//-------------------------------------
//
//-------------------------------------
bool FFileINI::_Write(DWORD offset, const void* s, DWORD size)
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
bool FFileINI::Parse(TString& data, MHashMap<_RBMapSimple(TString, TString), TString>& m_data, MVector<TString>& sections)
{
	sections.Clear();
	m_data.Clear();
	int i, len;
	char* p = data.GetPtr();
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
		_RBMapSimple(TString, TString)& sec = m_data[v[i]];

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
void FFileINI::ReadIni(MHashMap<_RBMapSimple(TString, TString), TString> &m_data, MVector<TString> &sections)
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
bool FFileINI::WriteIni(MHashMap<_RBMapSimple(TString, TString), TString> &m_data, MVector<TString> &sections)
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
#endif
//-------------------------------------
//
//-------------------------------------
void FFileINI::DeleteSection(const TString& Section)
{
#ifdef WIN32
	if (m_FileName)
	WritePrivateProfileStringA(Section.GetPtr(), 0, 0 , m_FileName);
#else
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
#endif
}
//-------------------------------------
//
//-------------------------------------

bool FFileINI::WriteString(const TString &Section, const TString& Entry, const TString& lpszValue)
{
#ifdef WIN32
	return m_FileName && WritePrivateProfileStringA(Section.GetPtr(), Entry.GetPtr(), lpszValue, m_FileName);
#else

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
#endif

	 
}
//-------------------------------------
//
//-------------------------------------
int FFileINI::GetInt(const TString& Section, const TString& Entry, int iDefault)
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
int FFileINI::GetIntHex(const TString &Section, const TString& Entry, int iDefault)
{
	TString res;

	if (!IsOpen()) 
	{
		return iDefault;
	}

	res = GetString(Section, Entry);
	if (!res.StrLen())
		return iDefault;
	int dw;
	sscanf(res.GetPtr(), "%x", &dw);
	return dw;
} 

//-------------------------------------
//
//-------------------------------------
bool FFileINI::WriteInt(const TString &Section, const TString &Entry, int iValue)
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
bool FFileINI::WriteIntHex(const TString &Section, const TString& Entry, int iValue)
{
	TString val;
	val.Format("%x", iValue);
	if (!IsOpen()) 
	{
		return 0;
	}
	return WriteString(Section, Entry, val);

}
//-------------------------------------
//
//-------------------------------------
bool FFileINI::GetSectionEntry(const TString& section, _RBMapSimple(TString, TString)& sc)
{
#ifdef WIN32
	if (m_FileName)
	{
		MArray<char> val;
		int sz;
		int size = 128;
		val.SetLen(size);
		sz = GetPrivateProfileSectionA(section.GetPtr(), val.GetPtr(), size, m_FileName);
		if (sz > size - 2)
		{
			val.SetLen(sz + 4);
			sz = GetPrivateProfileSectionA(section.GetPtr(), val.GetPtr(),sz + 2, m_FileName);
		}
		if (sz)
		{
			char* p = val.GetPtr();
			while (p)
			{
				TString t = p;
				MVector<TString>  v;
				t.Tokenize("=", v);
				switch (v.GetLen())
				{
				case 1:
					sc[v[0]] = "";
					break;
				case 2:
					sc[v[0]] = v[1];
					break;
				}
				p += strlen(p) + 1;

			}

		}
	}
#else
	MHashMap<_RBMapSimple(TString, TString), TString> m_data;
	MVector<TString> sections;
	ReadIni(m_data, sections);
	Section.Upper();
	Section.ClearSpace();
	sc.Clear();
	if (m_data.Lookup(Section))
		sc = m_data[Section];
#endif
	return sc.m_iNum != 0; 

}

//-------------------------------------
//
//-------------------------------------
double FFileINI::GetDouble(const TString& Section, const TString& Entry, double fDefault)
{
	TString res;
	if (!IsOpen()) 
	{
		return fDefault;
	}
	res = GetString(Section, Entry);
	if (!res.StrLen())
		return fDefault;
	return atof(res.GetPtr());
} 
//-------------------------------------
//
//-------------------------------------
bool FFileINI::WriteDouble(const TString& Section, const TString &Entry, double fValue)
{
	TString res;
	res.Format("%lf", fValue);
	if (!IsOpen()) return 0;

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
		LPTSTR pszFileName = _tcsrchr(szPathName, '\\') + 1;
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

#ifdef WIN32
	m_FileName = (char*)HMALLOC(fn.StrLen() + 1);
	strcpy(m_FileName, fn.GetPtr());
	w_FileName = fn.GetPtr();

#endif

	if (!Open(fn.GetPtr(), "rb+"))
		Open(fn.GetPtr(), "wb+");
#ifdef WIN32
	if (hFile)
		fclose(hFile);
	hFile = 0;
#endif


}
#ifndef WIN32

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
#endif
//-------------------------------------
//
//-------------------------------------
void FFileINIW::SectionNames(MVector<WString> &res)
{
#ifdef WIN32
	if (m_FileName)
	{
		MArray<wchar_t> val;
		int sz;
		int size = 128;
		val.SetLen(size);
		sz = GetPrivateProfileSectionNamesW(val.GetPtr(), size, w_FileName.GetPtr());
		if (sz > size - 2)
		{
			val.SetLen(sz + 4);
			sz = GetPrivateProfileSectionNamesW(val.GetPtr(), sz + 4, w_FileName.GetPtr());
		}
		if (sz)
		{
			wchar_t* p = val.GetPtr();
			while (p)
			{
				res.Add() = p;
				p += wcslen(p) + 1;

			}

		}
	}
#else

	MHashMap<_RBMapSimple(WString, WString), WString> m_data;
	MVector<WString> sections;
	ReadIni(m_data, sections);

	for (int i = 0, len = sections.GetLen(); i < len; ++i)
		res.Add() = sections[i];
#endif
}
#ifndef WIN32

//-------------------------------------
//
//-------------------------------------
bool FFileINIW::Parse(WString &data, MHashMap<_RBMapSimple(WString, WString), WString> &m_data, MVector<WString> &sections)
{
	sections.Clear();
	m_data.Clear();
       // int n = sizeof(wchar_t);
	int len = data.GetLen() - 1;
	if (len <= 0)
		return false;
	int j, i  = 0;
	wchar_t *p = data.GetPtr();
	if (p[i] != L'[' || !iswalpha(p[i]))
		++i;
        TString test;
	for (; i < len; ++i)
	{
		if (p[i] != L'\r')
			if (p[i] != L'\n')
             {
               test = data.Mid(i, 1);
               if (!iswprint(p[i]) || !isprint(test[0]))
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
	while(i < len && v[i].Find(L"[") == -1)
		++i;
	if (i > 0)
                v.Remove(0, i);
	i = 0;
	len = v.GetLen();
	while(i < len)
	{
		v[i].TrimLeft(L' ');
                v[i].TrimRight(L' ');
                int i1 = v[i].Find(L"[");
                int i2 = v[i].Find(L"]");
                if (i1 == -1 || i2 == -1) 
                {
                    ++i;
                    continue;
                }
                else
                v[i] = v[i].Mid(i1 + 1, i2 - i1 - 1);
		v[i].Upper();
		if (!m_data.Lookup(v[i]))
			sections.Add(v[i]);
		_RBMapSimple(WString, WString)  &sec = m_data[v[i]];
		
		++i;
		if (i < len)
                {
                    v[i].TrimLeft(L' ');
                    v[i].TrimRight(L' ');
                }    
                while(i < len) // && v[i][0] != L'[' && v[i].Find(L"]") != -1)
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
                            if (i2 != -1  && i1 != -1)
                            {
                                if (i0 == -1)
                                    break;
                            }
                        }
                        
		}
	}
	return m_data.Len() != 0;	 
}
#endif
//-------------------------------------
//
//-------------------------------------
WString FFileINIW::GeWString(WString Section, WString Entry, const wchar_t *lpszDefault)
{
#ifdef WIN32
	if (m_FileName)
	{
		MArray<wchar_t> val;
		int sz;
		int size = 128;
		val.SetLen(size);


		sz = GetPrivateProfileStringW(Section.GetPtr(), Entry.GetPtr(), 0, val.GetPtr(), size - 1, w_FileName.GetPtr());
		if (sz > size - 1)
		{
			val.SetLen(sz + 2);
			sz = GetPrivateProfileStringW(Section.GetPtr(), Entry.GetPtr(), 0, val.GetPtr(), sz + 2, w_FileName.GetPtr());
		}
		return val.GetPtr();
	}
#else
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
#endif
	WString tmp(lpszDefault);
	return tmp;

}
//-------------------------------------
//
//-------------------------------------
BOOL FFileINIW::IsValid()
{
#ifdef WIN32
	return m_FileName && GetFileAttributesA(m_FileName) != 0xffffffff;
#else

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
#endif
}
#ifndef WIN32

//-------------------------------------
//
//-------------------------------------
void FFileINIW::ReadIni(MHashMap<_RBMapSimple(WString, WString), WString> &m_data, MVector<WString> &sections)
{
	if (IsOpen()) 
	{
		DWORD size = GetSize();
		WString data;
		data.SetLen(size/sizeof(short));
		MArray<USHORT> tmp(size/sizeof(short)); 
                Read(0, tmp.GetPtr(), size);
                for (int i = 0, l = size/sizeof(short); i < l; ++i)
                    data[i] = tmp[i];
		Parse(data, m_data, sections);
	}
 }
//-------------------------------------
//
//-------------------------------------
bool FFileINIW::WriteIni(MHashMap<_RBMapSimple(WString, WString), WString> &m_data, MVector<WString> &sections)
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
                    tmp[i] = (USHORT)data[i]; 

		return _Write(0, tmp.GetPtr(), data.StrLen() * sizeof(USHORT)); 	
	}
	return false;
}
#endif
//-------------------------------------
//
//-------------------------------------
void FFileINIW::DeleteSection(WString Section)
{
#ifdef WIN32
	if (m_FileName)
		WritePrivateProfileStringW(Section.GetPtr(), 0, 0, w_FileName.GetPtr());
#else

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
#endif
}
//-------------------------------------
//
//-------------------------------------
bool FFileINIW::WriteString(WString Section, WString Entry, const wchar_t *lpszValue)
{	

	if (!IsOpen() || !Section.StrLen() || !Entry.StrLen()) 
		return 0;
#ifdef WIN32
	return m_FileName && WritePrivateProfileStringW(Section.GetPtr(), Entry.GetPtr(), lpszValue, w_FileName.GetPtr());
#else

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
#endif
	 
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
#ifdef WIN32
	if (m_FileName)
	{
		MArray<wchar_t> val;
		int sz;
		int size = 128;
		val.SetLen(size);
		sz = GetPrivateProfileSectionW(Section.GetPtr(), val.GetPtr(), size, w_FileName.GetPtr());
		if (sz > size - 2)
		{
			val.SetLen(sz + 4);
			sz = GetPrivateProfileSectionW(Section.GetPtr(), val.GetPtr(), sz + 2, w_FileName.GetPtr());
		}
		if (sz)
		{
			wchar_t* p = val.GetPtr();
			while (p)
			{
				WString t = p;
				MVector<WString>  v;
				t.Tokenize(L"=", v);
				switch (v.GetLen())
				{
				case 1:
					sc[v[0]] = "";
					break;
				case 2:
					sc[v[0]] = v[1];
					break;
				}
				p += wcslen(p) + 1;

			}

		}
	}
#else

	MHashMap<_RBMapSimple(WString, WString), WString> m_data;
	MVector<WString> sections;
	ReadIni(m_data, sections);
	Section.Upper();
	Section.ClearSpace();
	sc.Clear();
	if (m_data.Lookup(Section))
		sc = m_data[Section];
#endif
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
extern "C++" DWORD GetPrivateProfileStringA(LPCSTR lpSection, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
    FFileINI file(lpFileName, FALSE);
    if (!lpKeyName || !*lpKeyName)
    {
        _RBMapSimple(TString, TString) sc;
	file.GetSectionEntry(lpSection, sc);
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
		if(nSize < buff.GetLen())
		{
			if (nSize - 1 > 0)
				buff[nSize - 1] = 0;
			if (nSize - 2 > 0)
				buff[nSize - 2] = 0;
		}
		else
			nSize = buff.GetLen();
		memcpy(lpReturnedString, buff.GetPtr(), nSize);
		
	}
	return buff.GetLen();

    }
        
	TString res = file.GetString(lpSection, lpKeyName);
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
extern "C++" int GetPrivateProfileIntA(LPCSTR lpSection,LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName)
{
	FFileINI file(lpFileName, FALSE);
	return file.GetInt(lpSection, lpKeyName, nDefault);

}

//-------------------------------------
//
//-------------------------------------
extern "C++" BOOL WritePrivateProfileStringA(LPCSTR lpSection, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName)
{
	FFileINI file(lpFileName, FALSE);
	return file.WriteString(lpSection, lpKeyName, lpString);
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetPrivateProfileSectionA(LPCSTR lpSection, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
	FFileINI file(lpFileName, FALSE);
	_RBMapSimple(TString, TString) sc;
	file.GetSectionEntry(lpSection, sc);
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
		if(nSize < buff.GetLen())
		{
			if (nSize - 1 > 0)
				buff[nSize - 1] = 0;
			if (nSize - 2 > 0)
				buff[nSize - 2] = 0;
		}
		else
			nSize = buff.GetLen();
		memcpy(lpReturnedString, buff.GetPtr(), nSize);
		
	}
	return buff.GetLen();
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetPrivateProfileSectionNamesA(LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
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
		if(nSize < buff.GetLen())
		{
			if (nSize - 1 > 0)
				buff[nSize - 1] = 0;
			if (nSize - 2 > 0)
				buff[nSize - 2] = 0;
		}
		else
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
    TString defIniName =  AfxGetApp()->ModuleName;
    int i1 =  defIniName.ReverseFind(FD);
    
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
    defIniName =  DefaultIniNameA();
    return defIniName;
}

//-------------------------------------
//
//-------------------------------------
extern "C++" BOOL WriteProfileStringA(LPCSTR lpSection, LPCSTR lpKeyName, LPCSTR lpString)
{
    return WritePrivateProfileStringA(lpSection, lpKeyName, lpString, DefaultIniNameA());
}
//-------------------------------------
//
//-------------------------------------
extern "C++" DWORD GetProfileStringA(LPCSTR lpSection, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize)
{
    return GetPrivateProfileStringA(lpSection, lpKeyName, lpDefault, lpReturnedString, nSize, DefaultIniNameA());
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileIntA(LPCSTR lpSection,LPCSTR lpKeyName, INT nDefault)
{
    return GetPrivateProfileIntA(lpSection, lpKeyName, nDefault, DefaultIniNameA());
    
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileSectionA(LPCSTR lpSection, LPSTR lpReturnedString, DWORD nSize)
{
    return GetPrivateProfileSectionA(lpSection, lpReturnedString, nSize, DefaultIniNameA());
    
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
extern "C++" DWORD GetPrivateProfileStringW(LPCWSTR lpSection, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName)
{
	TString s = lpFileName;
	FFileINIW file(s.GetPtr(), FALSE);
	WString res = file.GeWString(lpSection, lpKeyName);
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
extern "C++" int GetPrivateProfileIntW(LPCWSTR lpSection,LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName)
{
	TString s = lpFileName;
	FFileINIW file(s.GetPtr(), FALSE);
	return file.GetInt(lpSection, lpKeyName, nDefault);

}

//-------------------------------------
//
//-------------------------------------
extern "C++" BOOL WritePrivateProfileStringW(LPCWSTR lpSection, LPCWSTR lpKeyName, LPCWSTR lpString, LPCWSTR lpFileName)
{
	TString s = lpFileName;
	FFileINIW file(s.GetPtr(), FALSE);
	return file.WriteString(lpSection, lpKeyName, lpString);
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetPrivateProfileSectionW(LPCWSTR lpSection, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName)
{
	TString s = lpFileName;
	FFileINIW file(s.GetPtr(), FALSE);
	_RBMapSimple(WString, WString) sc;
	file.GetSectionEntry(lpSection, sc);
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
		if(nSize < buff.GetLen())
		{
			if (nSize - 1 > 0)
				buff[nSize - 1] = 0;
			if (nSize - 2 > 0)
				buff[nSize - 2] = 0;
		}
		else
			nSize = buff.GetLen();
		memcpy(lpReturnedString, buff.GetPtr(), nSize * sizeof(wchar_t));
		
	}
	return buff.GetLen();
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetPrivateProfileSectionNamesW(LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName)
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
		if(nSize < buff.GetLen())
		{
			if (nSize - 1 > 0)
				buff[nSize - 1] = 0;
			if (nSize - 2 > 0)
				buff[nSize - 2] = 0;
		}
		else
			nSize = buff.GetLen();
		memcpy(lpReturnedString, buff.GetPtr(), nSize * sizeof(wchar_t));
		
	}
	return buff.GetLen();

}
//-------------------------------------
//
//-------------------------------------
extern "C++" BOOL WriteProfileStringW(LPCWSTR lpSection, LPCWSTR lpKeyName, LPCWSTR lpString)
{

    return WritePrivateProfileStringW(lpSection, lpKeyName, lpString, DefaultIniNameW());
}
//-------------------------------------
//
//-------------------------------------
extern "C++" DWORD GetProfileStringW(LPCWSTR lpSection, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize)
{
    return GetPrivateProfileStringW(lpSection, lpKeyName, lpDefault, lpReturnedString, nSize, DefaultIniNameW());
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileIntW(LPCWSTR lpSection,LPCWSTR lpKeyName, INT nDefault)
{
    return GetPrivateProfileIntW(lpSection, lpKeyName, nDefault, DefaultIniNameW());
    
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileSectionW(LPCWSTR lpSection, LPWSTR lpReturnedString, DWORD nSize)
{
    return GetPrivateProfileSectionW(lpSection, lpReturnedString, nSize, DefaultIniNameW());
    
}
//-------------------------------------
//
//-------------------------------------
extern "C++" int GetProfileSectionNamesW(LPWSTR lpszReturnBuffer, DWORD nSize)
{
    return GetPrivateProfileSectionNamesW(lpszReturnBuffer, nSize, DefaultIniNameW());
    
}

#endif
