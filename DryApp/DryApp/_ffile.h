#ifndef __FFile1232__
#include "mtree.h"
#include "mstring.h"
//#include "expdef.h"
#ifdef __PLUK_LINUX__
#define _fileno fileno
inline int _filelength(FILE *hFile)
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
inline int _filelength(int hFile)
{
	if (hFile)
	{
        struct stat st;
        return fstat(hFile, &st) != -1 ? st.st_size : EOF;
	}
	return EOF;
}
int _filelength(LPCSTR hFile);

#endif
#pragma pack(push)
#pragma pack(4)

EXPORTNIX class _PEXPORT FFile
{
public:
	FFile() { hFile = 0; m_FileName = m_Attr = 0;
#ifdef ANDROID_NDK
	fDel = false;
#endif
	}
	~FFile() { Close(); }
	bool Open(const char *filename, const char *attr);
	bool IsOpen() { return hFile != 0; }
	int GetSeek();
	int SetSeek(int offset);
	int GetSize();
	bool Close();
	bool Write(int offset, const void *s, int size);
	off_t Read(int offset, void *s, int size);
	bool SetSize(int size);
	bool Flush();
public:
	char *m_FileName;
	char *m_Attr;
	FILE *hFile;
#ifdef ANDROID_NDK
	bool fDel;
#endif
};
EXPORTNIX class _PEXPORT FFileINI : public FFile {
public:
	FFileINI(const char *fname, bool fAddPathDir = true);
	~FFileINI() { Close(); }
#ifdef WIN32
	bool IsOpen() { return IsValid(); }

#endif
	bool WriteString(const TString &Section, const TString &Entry, const TString &lpszValue);
	int GetInt(const TString &Section, const TString &Entry, int iDefault = 0);
	int GetIntHex(const TString &Section, const TString &Entry, int iDefault = 0);
	bool WriteIntHex(const TString &Section, const TString &Entry, int iValue);
	bool WriteInt(const TString &Section, const TString &Entry, int iValue);
	double GetDouble(const TString &Section, const TString &Entry, double fDefault = 0.0); 
	bool WriteDouble(const TString &Section, const TString &Entry, double fValue);
	void SectionNames(MVector<TString> &);
	void DeleteSection(const TString &Section);
	bool GetSectionEntry(const TString &Section, _RBMapSimple(TString, TString) &sec);
	TString GetString(TString Section, TString Entry, const char *lpszDefault = 0);
	BOOL IsValid();
protected:
#ifndef WIN32

	bool _Write(DWORD offset, const void *s, DWORD size);
	bool Parse(TString &data, MHashMap<_RBMapSimple(TString, TString), TString> &m_data, MVector<TString> &sections);
	void ReadIni(MHashMap<_RBMapSimple(TString, TString), TString> &m_data, MVector<TString> &sections);
	bool WriteIni(MHashMap<_RBMapSimple(TString, TString), TString> &m_data, MVector<TString> &sections);
#endif	
};
EXPORTNIX class _PEXPORT FFileINIW : public FFile {
public:
	FFileINIW(const char *fname, bool fAddPathDir = true);
	~FFileINIW() { Close(); }
	bool WriteString(WString Section, WString Entry, const wchar_t *lpszValue);
	int GetInt(WString Section, WString Entry, int iDefault = 0);
	int GetIntHex(WString Section, WString Entry, int iDefault = 0);
	bool WriteIntHex(WString Section, WString Entry, int iValue);
	bool WriteInt(WString Section, WString Entry, int iValue);
	double GetDouble(WString Section, WString Entry, double fDefault = 0.0); 
	bool WriteDouble(WString Section, WString Entry, double fValue);
	void SectionNames(MVector<WString> &);
	void DeleteSection(WString Section);
	bool GetSectionEntry(WString Section, _RBMapSimple(WString, WString) &sec);
	WString GeWString(WString Section, WString Entry, const wchar_t *lpszDefault = 0);
	WString GetString(WString Section, WString Entry, const wchar_t *lpszDefault = 0) { return GeWString(Section, Entry, lpszDefault); }
	BOOL IsValid();
protected:
#ifndef WIN32
	bool _Write(DWORD offset, const void *s, DWORD size);
	bool Parse(WString &data, MHashMap<_RBMapSimple(WString, WString), WString> &m_data, MVector<WString> &sections);
	void ReadIni(MHashMap<_RBMapSimple(WString, WString), WString> &m_data, MVector<WString> &sections);
	bool WriteIni(MHashMap<_RBMapSimple(WString, WString), WString> &m_data, MVector<WString> &sections);
#else
	WString w_FileName;
#endif
};
#pragma pack(pop)

#ifdef __PLUK_LINUX__
extern "C++" BOOL WritePrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName);
extern "C++" DWORD GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName);
extern "C++" int GetPrivateProfileIntA(LPCSTR lpAppName,LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName);
extern "C++" int GetPrivateProfileSectionA(LPCSTR lpAppName, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName);
extern "C++" int GetPrivateProfileSectionNamesA(LPSTR lpszReturnBuffer, DWORD nSize, LPCSTR lpFileName);

extern "C++" BOOL WriteProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString);
extern "C++" DWORD GetProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize);
extern "C++" int GetProfileIntA(LPCSTR lpAppName,LPCSTR lpKeyName, INT nDefault);
extern "C++" int GetProfileSectionA(LPCSTR lpAppName, LPSTR lpReturnedString, DWORD nSize);
extern "C++" int GetProfileSectionNamesA(LPSTR lpszReturnBuffer, DWORD nSize);

extern "C++" BOOL WritePrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString, LPCWSTR lpFileName);
extern "C++" DWORD GetPrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName);
extern "C++" int GetPrivateProfileIntW(LPCWSTR lpAppName,LPCWSTR lpKeyName, INT nDefault, LPCWSTR lpFileName);
extern "C++" int GetPrivateProfileSectionW(LPCWSTR lpAppName, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName);
extern "C++" int GetPrivateProfileSectionNamesW(LPWSTR lpszReturnBuffer, DWORD nSize, LPCWSTR lpFileName);
extern "C++" int GetProfileSectionNamesW(LPWSTR lpszReturnBuffer, DWORD nSize);
extern "C++" int GetProfileSectionW(LPCWSTR lpAppName, LPWSTR lpReturnedString, DWORD nSize);
extern "C++" int GetProfileIntW(LPCWSTR lpAppName,LPCWSTR lpKeyName, INT nDefault);
extern "C++" DWORD GetProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize);
extern "C++" BOOL WriteProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString);

#define GetPrivateProfileSectionNames GetPrivateProfileSectionNamesA
#define WritePrivateProfileString WritePrivateProfileStringA
#define GetPrivateProfileString GetPrivateProfileStringA
#define GetPrivateProfileInt GetPrivateProfileIntA
#define GetPrivateProfileSection GetPrivateProfileSectionA 

#endif

#endif

