#ifndef __VARIABLE__
#define __VARIABLE__
enum TYPE_VAR {VAR_EMPTY, VAR_WSTRING, VAR_STRING, VAR_DOUBLE, VAR_SHORT, VAR_USHORT, VAR_LONG, VAR_ULONG, VAR_INT, VAR_UINT, VAR_CHAR, VAR_UCHAR, VAR_VAR, VAR_VECTOR, VAR_BUFFER, VAR_GUID, VAR_ALIGNMENT};
#include "mstring.h"
#include "mtree.h"	 
#include "mfile.h"

#pragma pack(push)
#pragma pack(2)
class  Variable {
public:
	Variable() { m_Type = VAR_EMPTY; m_Data.db = 0.0; };
	Variable(const Variable &a){ m_Type = VAR_EMPTY; *this = a; }
	Variable(const MArray<BYTE> &a) { m_Type = VAR_EMPTY; *this = a; }
	~Variable() { Clear(); };
	Variable(const char *a){m_Type = VAR_EMPTY; *this = a; }
	Variable(double a){m_Type = VAR_EMPTY; *this = a; }
	Variable(short  a){m_Type = VAR_EMPTY; *this = a; }
	Variable(unsigned short a){m_Type = VAR_EMPTY; *this = a; }
	Variable(int a){m_Type = VAR_EMPTY; *this = a; }
#ifdef WIN32
	Variable(DWORD a){m_Type = VAR_EMPTY; *this = a; }
#endif
//	Variable(int a){m_Type = VAR_EMPTY; *this = a; }
#ifdef WIN32
	Variable(UINT a){m_Type = VAR_EMPTY; *this = a; }
#endif
	Variable(char a){m_Type = VAR_EMPTY; *this = a; }
	Variable(BYTE a){m_Type = VAR_EMPTY; *this = a; }
	TYPE_VAR Type() { return (TYPE_VAR)m_Type; };
	void Clear();
	int SizeOf();
	int SizeOfVariant();


	Variable &operator [] (int i);
//	Variable &operator [] (int i) { return operator [] ((int)i); }


	int Dim(int n = -1);
	void DeleteInVector(int pos, int num);

	Variable &operator = (const Variable &var);
	Variable &operator = (const MArray<BYTE> &var);
	Variable &operator = (const GUID &guid);
	Variable &operator = (const char *str);
	Variable &operator = (double);
	Variable &operator = (short);
	Variable &operator = (unsigned short);
	Variable &operator = (int);
	Variable &operator = (DWORD);
	//Variable &operator = (int);
#ifdef WIN32
	Variable &operator = (UINT);
#endif
	Variable &operator = (char);
	Variable &operator = (BYTE);
	Variable &operator = (const wchar_t *);

	BOOL operator == (const Variable &var);
	BOOL operator == (const MArray<BYTE> &var) { return m_Type == VAR_BUFFER ? *m_Data.buff == var : false; };
	BOOL operator == (const GUID &guid){ return m_Type == VAR_GUID ? !memcmp(&guid, m_Data.guid, sizeof(GUID)) : false; };
	BOOL operator == (const char *str) { return m_Type == VAR_STRING ? *m_Data.str == str : false; };
	BOOL operator == (double a){return a == operator double(); }
	BOOL operator == (short a) { return a == operator short(); }
	BOOL operator == (unsigned short a) { return a == USHORT(); }
	BOOL operator == (int a) { return a == operator int(); }
	BOOL operator == (DWORD a) { return a == operator DWORD(); }
//	BOOL operator == (int a) { return a == operator int(); }
#ifdef WIN32
	BOOL operator == (UINT a) { return a == operator UINT(); }
#endif
	BOOL operator == (char a) { return a == operator char(); }
	BOOL operator == (BYTE a) { return a == operator BYTE(); }
	BOOL operator == (const wchar_t *str) { return m_Type == VAR_WSTRING ? *m_Data.wstr == str : false; };

	BOOL operator != (const Variable &a)  { return !(*this == a); }
	BOOL operator != (const MArray<BYTE> &a) { return !(*this == a); }
	BOOL operator != (const GUID &a)  { return !(*this == a); }
	BOOL operator != (const char *str) { return !(*this == str); };
	BOOL operator != (double a) { return !(*this == a); }
	BOOL operator != (short a) { return !(*this == a); }
	BOOL operator != (unsigned short a) { return !(*this == a); }
	BOOL operator != (int a) { return !(*this == a); }
	BOOL operator != (DWORD a) { return !(*this == a); }
//	BOOL operator != (int a) { return !(*this == a); }
#ifdef WIN32
	BOOL operator != (UINT a) { return !(*this == a); }
#endif
	BOOL operator != (char a) { return !(*this == a); }
	BOOL operator != (BYTE a) { return !(*this == a); }
	BOOL operator != (const wchar_t *str) { return !(*this == str); };

	Variable &operator << (const Variable &var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (TString &var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (const MArray<BYTE> &var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (const GUID &var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (const char *var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (double var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (short var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (unsigned short var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (int var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (DWORD var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
//	Variable &operator << (int var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
#ifdef WIN32
	Variable &operator << (UINT var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
#endif
	Variable &operator << (char var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	Variable &operator << (BYTE var) { int d = Dim(); Dim(d + 1); operator [] (d) = var; return *this; }
	DWORD &RefDWORD() {
		if (m_Type != VAR_ULONG)
			SetDWORD();
		return m_Data.ulg;	
	}
	operator TString ();
	operator WString ();
	operator GUID ();
	operator const char *();
	operator const wchar_t * ();
#ifdef _VS_2005_
	operator wchar_t () {  return (wchar_t)operator int(); }
#endif
	operator double ();
	operator DWORD ();
	operator int ();
//	operator int ()  { return (int)operator int(); }
#ifdef WIN32
	operator UINT ()  { return (UINT)operator DWORD(); }
#endif
	operator short ()  { return (short)operator int(); }
	operator USHORT ()  { return (USHORT)operator DWORD(); }
	operator char ()  { return (char)operator int(); }
	operator BYTE ()  { return (char)operator DWORD(); }
	operator bool () { return operator DWORD() != 0; }
	operator MVector<Variable> *() { return m_Type == VAR_VECTOR ? m_Data.vect : 0; }
//	operator Variable ();
	operator MArray<BYTE> ();
	void Serialize(MArray<BYTE> &b, UINT *alignBuffer = 0);
	void UnSerialize(MArray<BYTE> &b, BOOL fNoUnserializeBuffer = 0, UINT *algn = 0);
	void UnSerialize(MFILE * hFile, BOOL fNoUnserializeBuffer = 0, UINT *algn = 0, int offset = 0);

	bool IsNumber();
	bool IsString() { return m_Type == VAR_WSTRING || m_Type == VAR_STRING; }
	bool IsVector() { return m_Type == VAR_VECTOR; }
	bool IsBuffer() { return m_Type == VAR_BUFFER; }
	TString GetHexString();
	MArray<BYTE> *GetArray() { return m_Type == VAR_BUFFER ? m_Data.buff : 0; }
	MArray<BYTE> *InitArray() { 
		Clear();
		m_Data.buff = new MArray<BYTE>();
		m_Data.buff->SetAddedRealSize(sizeof(DWORD));
		m_Type = VAR_BUFFER;
		return m_Data.buff;
	}

	void Put (BYTE *b, int len)
	{
		Clear();
		m_Data.buff = new MArray<BYTE>();
		m_Data.buff->SetAddedRealSize(sizeof(DWORD));

		m_Data.buff->Copy(b, len);
		m_Type = VAR_BUFFER;
	}
protected :
	void UnSerialize(BYTE *b, int blen, int &pos, UINT &alignBuffer, BOOL fNoUnserializeBuffer);
	void Serialize1(MArray<BYTE> &b, UINT &alignBuffer);
	void SizeOf(int &size);
	void SizeOfVariant(int &size);
	void UnSerialize(MFILE * hFile, int blen, int &pos, UINT &algnBuffer, BOOL fNoUnserializeBuffer);
	double File2Double(MFILE * hFile, int offset);
	BYTE File2Byte(MFILE * hFile, int offset);
	USHORT File2UShort(MFILE * hFile, int offset);
	int File2Long(MFILE * hFile, int offset);
	DWORD ReadNext(MFILE * hFile, int offset, const void *s, DWORD size);
	void SetDWORD();



protected :
	union {
		TString *str;
		WString *wstr;
		MVector<Variable> *vect;
		MArray<BYTE> *buff;
		double db;
		short sh;
		unsigned short ush;
		int lg;
		DWORD ulg;
		int in;
		UINT uin;
		char ch;
		BYTE uch;
		GUID *guid;

	} m_Data;
	int m_Type;

};
#pragma pack(pop)
inline Variable &Variable::operator [] (int i) 
{ 
	if (m_Type == VAR_VECTOR && i < m_Data.vect->GetLen())
			return (*m_Data.vect)[i];
	return *this;
}
inline int Variable::File2Long(MFILE * hFile, int offset)
{
	int n;
	ReadNext(hFile, offset, &n, sizeof(int));
	return n;
} 
inline USHORT Variable::File2UShort(MFILE * hFile, int offset)
{
	USHORT n;
	ReadNext(hFile, offset, &n, sizeof(USHORT));
	return n;
} 
inline BYTE Variable::File2Byte(MFILE * hFile, int offset)
{
	BYTE n;
	ReadNext(hFile, offset, &n, sizeof(BYTE));
	return n;
} 
inline double Variable::File2Double(MFILE * hFile, int offset)
{
	double n;
	ReadNext(hFile, offset, &n, sizeof(double));
	return n;
} 

#endif
