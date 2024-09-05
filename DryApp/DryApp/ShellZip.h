// ShellZip.h: interface for the CShellZip class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ShellZip__
#define __ShellZip__

 #include <tlhelp32.h>
#include <Shellapi.h>
#include <shlwapi.h>
#include "shlobj.h"


#include "netdir.h"
class CShellZip
{
public:
	CShellZip();
	virtual ~CShellZip();
	BOOL Extract(CString wZipFile, CString wFolder);
	BOOL Dir(NetDir::TreeDir& dir);
	BOOL Dir(CString wZipFile, NetDir::TreeDir& dir);
	BOOL Open(CString wZipFile);
	void Close();
	BOOL Create(CString strDest);
	BOOL Add(CString strSrc, CString cZipFile);


protected:
	int  Dir(UString sZipFile, Folder* folder, NetDir::TreeDir& dir);
	FolderItem* Find(Folder* folder, MVector<WString>& ph, int iItem);
	FolderItem* Find(MVector<WString>& ph);
	BOOL ListThreads(RBSet<DWORD>& set, RBSet<DWORD>* exclude);
	Folder* pZippedFile;
	IShellDispatch* pShell;
};

#endif
