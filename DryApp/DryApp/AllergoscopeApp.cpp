// AllergoscopeApp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "cfgmgr32.h"
#include "Setupapi.h"
#include "WinIoCtl.h"
#include "AllergoscopeApp.h"
#include "TimerAndView.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#include "_ffile.h"
#include "mfile.h"

#include "MultiThread.h"
MultiThreadS* pMultiThreads = 0;


SERVICE_STATUS Status;
SERVICE_STATUS_HANDLE Handler = 0;
int CheckPoint = 1;
struct KillProcStatus {
	BOOL Status;
	HWND hWnd;
	DWORD ProcessID;
};

MService AllergoscopeApp::g_Service;
TCHAR MService::m_szServiceName[256];
bool MService::fIsServName = false;
SERVICE_STATUS_HANDLE MService::hHandler;
DWORD MService::dwThreadID;

void ReportStatus(DWORD CurState, DWORD Win32ExitCode, DWORD WaitHint);
void ReportStatus(DWORD Win32ExitCode, DWORD WaitHint);

AllergoscopeApp PApp;


void MService::SetServiceStatus(DWORD dw)
{
	AllergoscopeApp::g_Service.dwCurrentState = dw;
	::SetServiceStatus(AllergoscopeApp::g_Service.hHandler, &AllergoscopeApp::g_Service);
}

void WINAPI MService::ServiceMain(DWORD nArg, LPTSTR* Argv)
{

	// 	if (pPApp == 0)
	//		pPApp = new AllergoscopeApp;
	AfxWinInit(::GetModuleHandle(NULL), NULL, GetCommandLine(), 0);
	AllergoscopeApp::g_Service.dwThreadID = GetCurrentThreadId();
	AllergoscopeApp::g_Service.dwCurrentState = SERVICE_START_PENDING;
	if (AllergoscopeApp::g_Service.hHandler = RegisterServiceCtrlHandler(AllergoscopeApp::g_Service.m_szServiceName, AllergoscopeApp::g_Service.Handler))
	{
		SetServiceStatus(SERVICE_START_PENDING);
		AllergoscopeApp::g_Service.dwWin32ExitCode = S_OK;
		AllergoscopeApp::g_Service.dwCheckPoint = 0;
		AllergoscopeApp::g_Service.dwWaitHint = 0;

		SetServiceStatus(SERVICE_RUNNING);
		char pstrRegPath[512];
		HKEY key;
		DWORD dwKeyType, dwDataLen = 0;
		TString ss;
		ss = Argv[0];
		sprintf_s(pstrRegPath, 512, "SYSTEM\\CurrentControlSet\\Services\\%s", ss.GetPtr());
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, pstrRegPath, 0, KEY_READ, &key) == ERROR_SUCCESS)
			if (RegQueryValueExA(key, "DisplayName", NULL, &dwKeyType, NULL, &dwDataLen) == ERROR_SUCCESS && dwKeyType == REG_SZ)
				if (dwDataLen < 255 && RegQueryValueExA(key, "DisplayName", NULL, &dwKeyType, (unsigned char*)AllergoscopeApp::g_Service.m_szServiceName, &dwDataLen) == ERROR_SUCCESS)
				{
					AllergoscopeApp::g_Service.Start();
					MSG msg;
					BOOL bRet;
					while ((bRet = GetMessageA(&msg, NULL, 0, 0)) != 0)
					{
						if (bRet == -1)
						{
							// handle the error and possibly exit
						}
						else
						{
							TranslateMessage(&msg);
							DispatchMessageW(&msg);
						}
					}

					SetServiceStatus(SERVICE_STOPPED);
				}
	}
}

void WINAPI MService::Handler(DWORD dwOpcode)
{
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_STOP:
		//	AllergoscopeApp::g_Service.Close();
		SetServiceStatus(SERVICE_STOP_PENDING);
		PostThreadMessage(AllergoscopeApp::g_Service.dwThreadID, WM_QUIT, 0, 0);
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		ReportStatus(NOERROR, 0);
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	}
}


bool MService::Install()
{

	bool fRet = false;
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM)
	{
		printf("OpenSCManager OK\r\n");
		SC_HANDLE hService = ::OpenService(hSCM, AllergoscopeApp::g_Service.m_szServiceName, SERVICE_QUERY_CONFIG);
		if (!hService)
		{
			TCHAR szFilePath[_MAX_PATH];
			::GetModuleFileName(NULL, szFilePath, _MAX_PATH);
			if (hService = CreateService(hSCM,          // SCManager database
				AllergoscopeApp::g_Service.m_szServiceName,           // name of service
				AllergoscopeApp::g_Service.m_szServiceName,           // name to display
				SERVICE_ALL_ACCESS,    // desired access
				SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, // service type
				SERVICE_AUTO_START,  // start type
				SERVICE_ERROR_NORMAL,  // error control type
				szFilePath,            // service's binary
				NULL,                  // no load ordering group
				NULL,                  // no tag identifier
				0,                  // no dependencies
				NULL,                  // LocalSystem account
				NULL                   // no password
			))
			{
				printf("CreateService OK\r\n");
				fRet = true;

				printf("StartService %s\r\nwait strted...\r\n", StartService(hService, 0, 0) ? "OK" : "faild");
			}
			CloseServiceHandle(hService);
		}
		else
		{
			printf("OpenService OK");
			fRet = true;
		}
		CloseServiceHandle(hSCM);
	}
	return fRet;
}
bool MService::Uninstall()
{
	BOOL fRet = false;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM)
	{
		SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);
		if (hService)
		{
			SERVICE_STATUS status;
			::ControlService(hService, SERVICE_CONTROL_STOP, &status);
			fRet = DeleteService(hService);
			CloseServiceHandle(hService);
		}
		CloseServiceHandle(hSCM);
	}
	return fRet != 0;

}
bool MService::Start()
{

	SERVICE_TABLE_ENTRY st[] =
	{
		{ m_szServiceName, ServiceMain },
		{ NULL, NULL }
	};
	return StartServiceCtrlDispatcher(st) != 0;
}



//===========================================================
//
//===========================================================
AllergoscopeApp::AllergoscopeApp()
{

}
//===========================================================
//
//===========================================================
AllergoscopeApp::~AllergoscopeApp()
{


}
void ConvertJPG(const WString& folder);


DWORD TRACELastError()
{
	char* buff;
	DWORD SysErr = ::GetLastError();
	int n = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, ::GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPTSTR)&buff, 0, NULL);
	if (n)
	{
		TRACE("%s\r\n", buff);
		LocalFree(buff);
	}
	SetLastError(0);
	return SysErr;

}
void ConvertANSI2UTF8(const WString& folder, const WString& newfolder);
namespace VERSION_Info {
#pragma pack(push)
#pragma pack(2)
// https://learn.microsoft.com/en-us/windows/win32/menurc/version-information-structures
	struct VSRecord {
		WORD  wLength;
		WORD  wValueLength;
		WORD  wType;
		WCHAR szKey[1];
		//WORD  Padding;
	};
	inline BYTE* AlignRC(const BYTE* org, const void* key, int sz)
	{
		DWORD_PTR p = ((const BYTE*)key - org) + sz + 3;
		return (BYTE*)org + (p & ~3);
	}
struct String : public VSRecord
{
//	WORD  Value;
};
struct StringTable : public VSRecord
{
	//String Children[1];
};
struct StringFileInfo : public VSRecord
{
	//StringTable Children[1];
};
struct Var : public VSRecord 
{
	//DWORD Value;
};
struct VarFileInfo : public VSRecord
{
//	Var Children[1];
};
struct VERSIONINFO : public VSRecord
{
	VS_FIXEDFILEINFO Value;
	//WORD             Padding2;
	//WORD             Children[];
};

struct FixFileInfo : public VS_FIXEDFILEINFO {
	FixFileInfo(bool fIsDll = false) 
	{
		
		dwSignature = VS_FFI_SIGNATURE;
		dwStrucVersion = VS_FFI_STRUCVERSION;
		dwFileFlagsMask = VS_FFI_FILEFLAGSMASK;
		dwFileOS = VOS_NT_WINDOWS32;
		dwFileType = fIsDll ? VFT_DLL : VFT_APP;
		dwFileSubtype = VFT2_UNKNOWN;
		dwFileFlags = 0;
	}
	void SetProductVersion(WORD nVer[4])
	{
		dwProductVersionMS = MAKELONG(nVer[0], nVer[1]);
		dwProductVersionLS = MAKELONG(nVer[2], nVer[3]);

	}
	void SetFileVersion(WORD nVer[4])
	{
		dwFileVersionMS = MAKELONG(nVer[0], nVer[1]);
		dwFileVersionLS = MAKELONG(nVer[2], nVer[3]);

	}
};
struct CVSRecord {
	WORD  wLength;
	WORD  wValueLength;
	WORD  wType;
	WString szKey;
	
};

WString strTab[8] {
	L"CompanyName",
	L"FileDescription",
	L"FileVersion",
	L"InternalName",
	L"LegalCopyright",
	L"OriginalFilename",
	L"ProductName",
	L"ProductVersion"
};

struct CVERSIONINFO
{
	RBMap<WString, WString> stringFileInfo;
	RBMap<DWORD, WString> varFileInfo;
	FixFileInfo Value;
	
	void ToVerInt(LPCWSTR str, WORD nVer[4])
	{
		TString s;
		s = str;
		MVector<TString> vs = s.Tokenize(".,");
		if (!vs.GetLen())
		{
			vs.Add() = "1";
			vs.Add() = "0";
			vs.Add() = "0";
			vs.Add() = "1";
		}
		for (int i = vs.GetLen(); i < 4; ++i)
			vs.Add() = "0";
		for (int i = 0; i < 4; ++i)
		{
			nVer[i] = vs[i].ToInt();
		}

	}

	BOOL MakeBuffer(MArray<BYTE> &b)
	{
		b.SetLen(0x10000);
		BYTE* org = b.GetPtr();
		memset(b.GetPtr(), 0, 0x10000);
		VERSIONINFO* ver = (VERSIONINFO*)org;
		ver->wValueLength = sizeof(VS_FIXEDFILEINFO);
		ver->wType = 0;
		WString Key = L"VS_VERSION_INFO";
		wcscpy_s(ver->szKey, Key.StrLen() + 1, Key.GetPtr());
		VS_FIXEDFILEINFO* pVi = (VS_FIXEDFILEINFO*)AlignRC(org, ver->szKey, (wcslen(ver->szKey) + 1) << 1);
		memcpy(pVi, &Value, sizeof(VS_FIXEDFILEINFO));
		StringFileInfo* pStrFileInfo = (StringFileInfo *)AlignRC(org, pVi, ver->wValueLength);
		pStrFileInfo->wType = 1;
		Key = L"StringFileInfo";
		wcscpy_s(pStrFileInfo->szKey, Key.StrLen() + 1, Key.GetPtr());
		StringTable* pStringTable = (StringTable*)AlignRC(org, pStrFileInfo->szKey, +(wcslen(pStrFileInfo->szKey) + 1) << 1);
		pStringTable->wType = 1;
		DWORD lang = 0x04b00409;
		varFileInfo.Lookup(L"Translation", lang);
		Key.Format("%04x%04x", LOWORD(lang), HIWORD(lang));

		Key = L"040904B0";
		wcscpy_s(pStringTable->szKey, Key.StrLen() + 1, Key.GetPtr());
		String *pStringNext, *pString = (String*)AlignRC(org, pStringTable->szKey, (wcslen(pStringTable->szKey) + 1) << 1);
		int lenStrTable = 0;
		for (int i = 0; i < 8; ++i)
		{
			WString res;
			stringFileInfo.Lookup(strTab[i], res);
			int reslen = res.StrLen();
			if (reslen)
			{

				pString->wType = 1;
				pString->wValueLength = reslen + 1;
				wcscpy_s(pString->szKey, strTab[i].StrLen() + 1, strTab[i].GetPtr());
				LPWSTR value = (LPWSTR)AlignRC(org, pString->szKey, (wcslen(pString->szKey) + 1) << 1);
				wcscpy_s(value, reslen + 1, res.GetPtr());
				pStringNext = (String*)AlignRC(org, value, (res.StrLen() + 1) << 1);
				pString->wLength = (WORD)((DWORD_PTR)pStringNext - (DWORD_PTR)pString);
				lenStrTable += pString->wLength;
				pString = (String*)AlignRC(org, pString, pString->wLength);
			}
		}
		pStringTable->wLength = (WORD)((DWORD_PTR)pString - (DWORD_PTR)pStringTable);
		pStrFileInfo->wLength = (WORD)((DWORD_PTR)pString - (DWORD_PTR)pStrFileInfo);
		VarFileInfo* pVarInfo = (VarFileInfo*)pString;
		pVarInfo->wType = 1;
		Key = L"VarFileInfo";
		wcscpy_s(pVarInfo->szKey, Key.StrLen() + 1, Key.GetPtr());
		Var* pVar = (Var*)AlignRC(org, pVarInfo->szKey, (wcslen(pVarInfo->szKey) + 1) << 1);
		pVar->wType = 0;
		Key = L"Translation";
		wcscpy_s(pVar->szKey, Key.StrLen() + 1, Key.GetPtr());
		DWORD* pValue = (DWORD*)AlignRC(org, pVar->szKey, (wcslen(pVar->szKey) + 1) << 1);
		pVar->wValueLength = 4;
		*pValue = lang;
		++pValue;
		pVar->wLength = (WORD)((DWORD_PTR)pValue - (DWORD_PTR)pVar);
		pVarInfo->wLength = (WORD)((DWORD_PTR)pValue - (DWORD_PTR)pVarInfo);
		char *ptr = (char *)AlignRC(org, pVarInfo, pVarInfo->wLength);
		ver->wLength = (WORD)((DWORD_PTR)ptr - (DWORD_PTR)org);
		b.SetLen(ver->wLength);
		return 1;


	}
	CVERSIONINFO(LPCWSTR Name)
	{
		stringFileInfo[L"CompanyName"];
		stringFileInfo[L"FileDescription"];
		stringFileInfo[L"FileVersion"];
		stringFileInfo[L"InternalName"];
		stringFileInfo[L"LegalCopyright"];
		stringFileInfo[L"OriginalFilename"];
		stringFileInfo[L"ProductName"];
		stringFileInfo[L"ProductVersion"];
		WString str = Name;
		int p1 = str.ReverseFind(L'.');
		int p2 = str.ReverseFind(L'\\');
		WString exe = str.Right(str.StrLen() - p2 - 1);
		WString name = str.Mid(p2 + 1,  p1 - p2 - 1);
		if (p1 >= 0 && p2 >= 0)
		{
			stringFileInfo[L"FileVersion"] = L"1.0.0.1";
			stringFileInfo[L"ProductVersion"] = L"1.0.0.1";
			stringFileInfo[L"FileDescription"] = name; 
			stringFileInfo[L"InternalName"] = exe;
			stringFileInfo[L"LegalCopyright"] = "(c) 2023";
			stringFileInfo[L"OriginalFilename"] = exe;
			stringFileInfo[L"ProductName"] = name;
			WString sVerProduct = stringFileInfo[L"ProductVersion"];
			WORD nVerFile[4], nVerProd[4];
			ToVerInt(sVerProduct, nVerProd);
			Value.SetProductVersion(nVerProd);
			WString sVerFile = stringFileInfo[L"FileVersion"];
			ToVerInt(sVerFile, nVerFile);
			Value.SetFileVersion(nVerFile);


		}
		varFileInfo[L"Translation"] = 0x04b00409;


	}
	CVERSIONINFO(RBMap<WString, WString> &mapStringFileInfo, RBMap<DWORD, WString> &mapVarFileInfo)
	{

		WString sVerProduct = mapStringFileInfo[L"ProductVersion"];
		WORD nVerFile[4], nVerProd[4];
		ToVerInt(sVerProduct, nVerProd);
		Value.SetProductVersion(nVerProd);
		WString sVerFile = mapStringFileInfo[L"FileVersion"];
		ToVerInt(sVerFile, nVerFile);
		Value.SetFileVersion(nVerFile);
		stringFileInfo = mapStringFileInfo;
		varFileInfo = mapVarFileInfo;

	}
	CVERSIONINFO(MArray<BYTE>& buff)
	{
		BYTE* org = buff.GetPtr();
		VERSIONINFO* ver = (VERSIONINFO*)org;
		int n = sizeof(strTab);
		VS_FIXEDFILEINFO* pVi = (VS_FIXEDFILEINFO*)AlignRC(org, ver->szKey, (wcslen(ver->szKey) + 1) << 1);
		memcpy(&Value, pVi, sizeof(VS_FIXEDFILEINFO));
		VSRecord* rec = (VSRecord*)AlignRC(org, pVi, ver->wValueLength);
		VSRecord* end = (VSRecord*)((char*)ver + ver->wLength);
		while (rec < end)
		{

			if (!wcscmp(rec->szKey, L"StringFileInfo"))
			{
				StringFileInfo* pStrFileInfo = (StringFileInfo*)rec;
				StringTable* pStringTable = (StringTable*)AlignRC(org, pStrFileInfo->szKey, +(wcslen(pStrFileInfo->szKey) + 1) << 1);
			
				StringTable* pStringTableEnd = (StringTable*)((char*)pStrFileInfo + pStrFileInfo->wLength);
				//040904B0
				while (pStringTable < pStringTableEnd)
				{
					String* pString = (String*)AlignRC(org, pStringTable->szKey, (wcslen(pStringTable->szKey) + 1) << 1);
					String* pStringEnd = (String*)((char*)pStringTable + pStringTable->wLength);

					while (pString < pStringEnd)
					{
				
						LPWSTR m_strKey = pString->szKey;
						stringFileInfo[m_strKey] = (LPWSTR)AlignRC(org, pString->szKey, (wcslen(pString->szKey) + 1) << 1);
						pString = (String*)AlignRC(org, pString, pString->wLength);
					}
					pStringTable = (StringTable*)AlignRC(org, pStringTable, pStringTable->wLength);
				}
			}
			else
				if (!wcscmp(rec->szKey, L"VarFileInfo"))
				{
					VarFileInfo* pVarInfo = (VarFileInfo*)rec;
			
					Var* pVar = (Var*)AlignRC(org, pVarInfo->szKey, (wcslen(pVarInfo->szKey) + 1) << 1);
					Var* pVarEnd = (Var*)((char*)pVarInfo + pVarInfo->wLength);
					while (pVar < pVarEnd)
					{
						DWORD* pValue = (DWORD*)AlignRC(org, pVar->szKey, (wcslen(pVar->szKey) + 1) << 1);
						DWORD* pValueEnd = (DWORD*)((char*)pValue + pVar->wValueLength);
						if (pValue < pValueEnd)
							varFileInfo[pVar->szKey] = *pValue;

						pVar = (Var*)AlignRC(org, pVar, pVar->wLength);

					}

				}
				else
					assert(0);
			rec = (VSRecord*)AlignRC(org, rec, rec->wLength);
		}

	}
	
		
};


#pragma pack(pop)
void ReadFileInfo(MArray<BYTE>& buff)
{
	BYTE *org = buff.GetPtr();
	VERSIONINFO* ver = (VERSIONINFO *)org;
	TRACE(L"VERSIONINFO\r\n");
	TRACE(L"wLength =  %d\r\n", ver->wLength);
	TRACE(L"wValueLength =  %d\r\n", ver->wValueLength);
	TRACE(L"wType =  %d\r\n", ver->wType);
	TRACE(L"szKey =  %s\r\n", ver->szKey);

	
	VS_FIXEDFILEINFO* pVi = (VS_FIXEDFILEINFO*)AlignRC(org, ver->szKey, (wcslen(ver->szKey) + 1) << 1);
	BOOL b = pVi->dwSignature == 0xFEEF04BD;
	int nVer[4];
	nVer[0] = HIWORD(pVi->dwProductVersionMS);
	nVer[1] = LOWORD(pVi->dwProductVersionMS);
	nVer[2] = HIWORD(pVi->dwProductVersionLS);
	nVer[3] = LOWORD(pVi->dwProductVersionLS);
	TRACE(L"---------VS_FIXEDFILEINFO-------------------------\r\n");
	//VOS_NT_WINDOWS32
	TRACE(L"Product %d.%d.%d.%d\r\n", nVer[0], nVer[1], nVer[2], nVer[3]);
	nVer[0] = HIWORD(pVi->dwFileVersionMS);
	nVer[1] = LOWORD(pVi->dwFileVersionMS);
	nVer[2] = HIWORD(pVi->dwFileVersionLS);
	nVer[3] = LOWORD(pVi->dwFileVersionLS);
	TRACE(L"File %d.%d.%d.%d\r\n", nVer[0], nVer[1], nVer[2], nVer[3]);

	TRACE(L"----------------------------------\r\n");
	VSRecord* rec = (VSRecord*)AlignRC(org, pVi,  ver->wValueLength);
	VSRecord *end = (VSRecord*)((char*)ver + ver->wLength);
	while (rec < end)
	{
		
		if (!wcscmp(rec->szKey, L"StringFileInfo"))
		{
			StringFileInfo* pStrFileInfo = (StringFileInfo*)rec;
			TRACE(L"StringFileInfo\r\n");
			TRACE(L"wLength =  %d\r\n", pStrFileInfo->wLength);
			TRACE(L"wValueLength =  %d\r\n", pStrFileInfo->wValueLength);
			TRACE(L"wType =  %d\r\n", pStrFileInfo->wType);
		

			StringTable* pStringTable = (StringTable*)AlignRC(org, pStrFileInfo->szKey, + (wcslen(pStrFileInfo->szKey) + 1) << 1);
			TRACE(L"StringTable\r\n");
			TRACE(L"wLength =  %d\r\n", pStringTable->wLength);
			TRACE(L"wValueLength =  %d\r\n", pStringTable->wValueLength);
			TRACE(L"wType =  %d\r\n", pStringTable->wType);
			TRACE(L"szKey =  %s\r\n", pStringTable->szKey);

			StringTable* pStringTableEnd = (StringTable*)((char*)pStrFileInfo + pStrFileInfo->wLength);
			//040904B0
			while (pStringTable < pStringTableEnd)
			{
				String* pString = (String*)AlignRC(org, pStringTable->szKey,  (wcslen(pStringTable->szKey) + 1) << 1);
				String* pStringEnd = (String*)((char*)pStringTable + pStringTable->wLength);
				while (pString < pStringEnd)
				{
					TRACE(L"String\r\n");
					TRACE(L"wLength =  %d\r\n", pString->wLength);
					TRACE(L"wValueLength =  %d\r\n", pString->wValueLength);
					TRACE(L"wType =  %d\r\n", pString->wType);
					LPWSTR m_strKey = pString->szKey;
					LPWSTR lpwszValue = (LPWSTR)AlignRC(org, pString->szKey,  (wcslen(pString->szKey) + 1) << 1);
					int k = (wcslen(lpwszValue) + wcslen(m_strKey) + 2) * 2 + 6;
					TRACE(L"%s =  %s\r\n", m_strKey, lpwszValue);
					pString = (String*)AlignRC(org, pString, pString->wLength);
				}
				pStringTable = (StringTable*)AlignRC(org, pStringTable, pStringTable->wLength);
			}
		}
		else
			if (!wcscmp(rec->szKey, L"VarFileInfo"))
			{
				VarFileInfo* pVarInfo = (VarFileInfo*)rec;
				TRACE(L"VarFileInfo\r\n");
				TRACE(L"wLength =  %d\r\n", pVarInfo->wLength);
				TRACE(L"wValueLength =  %d\r\n", pVarInfo->wValueLength);
				TRACE(L"wType =  %d\r\n", pVarInfo->wType);


				Var* pVar = (Var*)AlignRC(org, pVarInfo->szKey, (wcslen(pVarInfo->szKey) + 1) << 1);
				TRACE(L"Var\r\n");
				TRACE(L"wLength =  %d\r\n", pVar->wLength);
				TRACE(L"wValueLength =  %d\r\n", pVar->wValueLength);
				TRACE(L"wType =  %d\r\n", pVar->wType);

				Var* pVarEnd = (Var*)((char *)pVarInfo + pVarInfo->wLength);
				while (pVar < pVarEnd)
				{
					DWORD* pValue = (DWORD*)AlignRC(org, pVar->szKey, (wcslen(pVar->szKey) + 1) << 1);
					DWORD* pValueEnd = (DWORD*)((char *)pValue + pVar->wValueLength);
					while (pValue < pValueEnd)
					{
						TRACE(L"%s %04x%04x\r\n", pVar->szKey, LOWORD(*pValue), HIWORD(*pValue));
						++pValue;
					}



					pVar = (Var*)AlignRC(org, pVar, pVar->wLength);

				}

			}
			else
				assert(0);
		rec = (VSRecord *)AlignRC(org, rec, rec->wLength);
	}



}

};
void TestUpdateResource()
{
	MArray<BYTE> buff;
	CFileDialog dlg(1, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT("All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		BOOL b = 0;
		VERSION_Info::CVERSIONINFO cb(dlg.GetPathName().GetString());
		cb.MakeBuffer(buff);
		VERSION_Info::ReadFileInfo(buff);
		int lg =MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);
		HGLOBAL hUpdateRes = BeginUpdateResource(dlg.GetPathName().GetString(), FALSE);
		if (hUpdateRes)
		{
			b = UpdateResource(hUpdateRes, RT_VERSION, MAKEINTRESOURCE(VS_VERSION_INFO),
				MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), buff.GetPtr(), buff.GetLen());

			b = EndUpdateResource(hUpdateRes, FALSE);
			FreeResource(hUpdateRes);
		}
		return;
		
		HINSTANCE hG = LoadLibrary(dlg.GetPathName().GetString());
		if (hG)
		{
			HRSRC   hResLoad = FindResource((HINSTANCE)hG, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
			if (hResLoad)
			{
				DWORD dwSize = SizeofResource((HINSTANCE)hG, hResLoad);
				HGLOBAL hResData = LoadResource((HINSTANCE)hG, hResLoad);
				BYTE* mpBuffer = (BYTE*)LockResource(hResData);
				MArray<BYTE> buf1;
				buf1.Copy(mpBuffer, dwSize);
				UnlockResource(hResData);
				VERSION_Info::ReadFileInfo(buf1);



			}
			else
			{
				VERSION_Info::CVERSIONINFO cb(dlg.GetPathName().GetString());
				cb.MakeBuffer(buff);
				VERSION_Info::ReadFileInfo(buff);
		

			}
			FreeLibrary(hG);
		


		}
	}
}
void ConvertUTF8toUTF16(const WString& folder, const WString& newfolder);

//===========================================================
//
//===========================================================
BOOL AllergoscopeApp::InitInstance()
{
	double db = 0.44;
	int k = db;
//	g_Service.Start();
	//SetThreadLocale(MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA));
	//ConvertANSI2UTF8(L"K:\\2\\1", L"K:\\2\\2");
	//ULONG idL = MAKELANGID(LANG_GREEK, SUBLANG_GREEK_GREECE);
	
	//SetThreadLocale(idL);
	SetThreadLocale(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN));
//	ConvertUTF8toUTF16(L"C:\\AllergoEye", L"C:\\alergo");

	CWinApp::InitInstance();
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(TEXT("Windows sockets initialization failed"));
		return FALSE;
	}
	//TestUpdateResource();
	/*
	HRSRC   hResLoad = FindResource(m_hInstance, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	DWORD dwSize = SizeofResource(m_hInstance, hResLoad);
	HGLOBAL hResData = LoadResource(m_hInstance, hResLoad);
	BYTE* mpBuffer = (BYTE*)LockResource(hResData);
	MArray<BYTE> buf1, buff;
	buf1.Copy(mpBuffer, dwSize);
	
	UnlockResource(hResData);
	VERSION_Info::ReadFileInfo(buf1);
	VERSION_Info::CVERSIONINFO cb(buf1);
	cb.MakeBuffer(buff);
	VERSION_Info::ReadFileInfo(buff);
*/
	TimerAndView dlg;
	m_pMainWnd = &dlg;
//	ConvertJPG(L"F:\\yura\\com.allergoscope.app");
	pMultiThreads = new MultiThreadS();
	pMultiThreads->Create();

	dlg.UpdateWindow();
	dlg.ShowWindow(SW_SHOWNORMAL);
	bool fShow = false;
	BOOL bRet;
	MSG msg;
	while ((bRet = GetMessageA(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			if (!fShow)
			{
				fShow = true;
				dlg.Minimize();
			}
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	
	ExitInstance();
	return TRUE;
}
//===========================================================
//
//===========================================================
int AllergoscopeApp::ExitInstance()
{
	if (pMultiThreads)
		delete pMultiThreads;
	pMultiThreads = 0;
	return 0;
}

void ReportStatus(DWORD CurState, DWORD Win32ExitCode, DWORD WaitHint)
{
	if (CurState == SERVICE_START_PENDING)
		AllergoscopeApp::g_Service.dwControlsAccepted = 0;
	else
		AllergoscopeApp::g_Service.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	AllergoscopeApp::g_Service.dwCurrentState = CurState;
	AllergoscopeApp::g_Service.dwWin32ExitCode = Win32ExitCode;
	AllergoscopeApp::g_Service.dwWaitHint = WaitHint;
	if (CurState == SERVICE_RUNNING || CurState == SERVICE_STOPPED)
		AllergoscopeApp::g_Service.dwCheckPoint = 0;
	else
		AllergoscopeApp::g_Service.dwCheckPoint = CheckPoint++;
	if (AllergoscopeApp::g_Service.hHandler)
		SetServiceStatus(AllergoscopeApp::g_Service.hHandler, &Status);
}
void ReportStatus(DWORD Win32ExitCode, DWORD WaitHint)
{
	ReportStatus(AllergoscopeApp::g_Service.dwCurrentState, Win32ExitCode, WaitHint);
}
// current instance

int APIENTRY wWinMain(__in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in_opt LPWSTR lpCmdLineW,
    __in int nShowCmd
    )
{
	HANDLE test = ::CreateMutex(NULL, FALSE, _T("006E16BD-E8EA-4389-86ED-342560657FBF"));

	if (::GetLastError() == ERROR_ALREADY_EXISTS)
		return 0;
	
	WString lpCmdLine = lpCmdLineW;
		int nRetCode = 0;
		/*MVector<WString> s = lpCmdLine.Tokenize(L"-/");

	for (int i = 0; i < s.GetLen(); ++i) 
	{
		if (lstrcmpi(s[i].GetPtr(), _T("install")) == 0)
			AllergoscopeApp::g_Service.Install();
		if (lstrcmpi(s[i].GetPtr(), TEXT("uninstall")) == 0)
			return AllergoscopeApp::g_Service.Uninstall();
		AllergoscopeApp::g_Service.Start();
	}
	*/

		PWSTR   ppszPath;    // variable to receive the path memory block pointer.
		HRESULT hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &ppszPath);

		hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &ppszPath);

		{
			TString strValue;
			FFileINI file("dryeye.ini", true);
			strValue = file.GetString("DIR", "DATA");
			if (!strValue.GetLength())
			{
		/*		TCHAR szPath[MAX_PATH];
				// Get path for each computer, non-user specific and non-roaming data.
				if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
				{
					strValue = szPath;
					if (strValue.Right(1) != L"\\")
						strValue += L"\\";
				}
				else
				{
					char* pValue;
					size_t len;
					errno_t err = _dupenv_s(&pValue, &len, "APPDATA");
					TString app = pValue;
					strValue = app.GetPtr();  //L"C:\\Allergoscope";
				}
				*/
				strValue ="C:\\AllergoEye\\";
				file.WriteString("DIR", "DATA", strValue);
				//FFileINI file2("dryeye.ini", true);
				//TString str = strValue;
				//if (str.Right(1) != "\\")
				//str += "\\";

				//file2.WriteString("DIR", "DATA", str.GetPtr());
				FFileINI fileT("AllergoscopeApp.txt", true);

				TString NN = fileT.GetString(L"NAME","NN");
				file.WriteString(L"NAME","NN", NN.GetPtr());
				TString GroupIP = fileT.GetString("MUTICAST", "ADDRESS", "224.0.0.251");
				UINT nGroupPort = fileT.GetInt("MUTICAST", "PORT", 5353);
				file.WriteString(L"MUTICAST", "ADDRESS", GroupIP.GetPtr());
				file.WriteInt(L"MUTICAST", "PORT", nGroupPort);


			//	file2.Close();


				if (GetFileAttributesA(strValue) == 0xffffffff)
					CreateDirectoryA(strValue, 0);

					TCHAR szPathName[_MAX_PATH];
					GetModuleFileName(0, szPathName, _MAX_PATH);
					LPTSTR pszFileName = _tcsrchr(szPathName, '\\');
					*pszFileName = 0;

					strValue = szPathName;
					file.WriteString(L"PATH","PLUK", strValue);


				
				file.Close();
			
			}
		

		}
		try {
			if (!AfxWinInit(::GetModuleHandle(NULL), NULL, lpCmdLine.GetPtr(), 0))
			{
				// TODO: change error code to suit your needs
				_tprintf(_T("Fatal Error: MFC initialization failed\n"));
				nRetCode = 1;
			}
			else
			{
				PApp.InitInstance();
			}
			if (test)
				::CloseHandle(test);
		}
		catch (...)
		{
			if (test)
				::CloseHandle(test);
		}
	return nRetCode;
}
#include "atlimage.h"

void ConvertJPG(const WString& folder)
{
	WString ppath = folder;
	WString  path;
	path = ppath.GetPtr();
	int ret = 0;
	int len = path.StrLen();
	if (path.Right(1) != L"\\")
		path += L"\\";
	ppath = path;
	path += L"*.*";

	WIN32_FIND_DATAW FindFileData;
	memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));

	HANDLE hFile = FindFirstFileW(path, &FindFileData);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dw = GetLastError();
		if (GetLastError() != ERROR_FILE_NOT_FOUND)
			return;
		return;
	}
	do
	{

		if (!wcscmp(FindFileData.cFileName, L"."))
			continue;
		if (!wcscmp(FindFileData.cFileName, L".."))
			continue;
		WString res = ppath;
		res += FindFileData.cFileName;
		WString name = FindFileData.cFileName;
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			ConvertJPG(res);
		else
		{
			if (name.Find(".jpg") != -1)
			{
				CImage image;
				image.Load(res.GetPtr());
				image.Save(res.GetPtr(), Gdiplus::ImageFormatJPEG);
			}
		}

		memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));
	} while (FindNextFileW(hFile, &FindFileData));
	FindClose(hFile);

}
void ConvertUTF8toUTF16(const WString& folder, const WString& newfolder)
{
	WString ppath = folder;
	WString npath = newfolder;
	WString  path;
	path = ppath.GetPtr();
	int ret = 0;
	int len = path.StrLen();
	if (path.Right(1) != L"\\")
		path += L"\\";
	if (npath.Right(1) != L"\\")
		npath += L"\\";

	ppath = path;
	path += L"*.*";

	WIN32_FIND_DATAW FindFileData;
	memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));

	HANDLE hFile = FindFirstFileW(path, &FindFileData);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dw = GetLastError();
		if (GetLastError() != ERROR_FILE_NOT_FOUND)
			return;
		return;
	}
	do
	{

		if (!wcscmp(FindFileData.cFileName, L"."))
			continue;
		if (!wcscmp(FindFileData.cFileName, L".."))
			continue;
		WString res = ppath;
		res += FindFileData.cFileName;
		WString name = FindFileData.cFileName;
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
				WString nres = npath;
				if (FindFileData.cFileName[0] != L'.')
				{
					nres += FindFileData.cFileName;
					CreateDirectory(nres.GetPtr(), 0);
					ConvertUTF8toUTF16(res, nres);
				}
		}
		else
			if (name.StrLen() > 4)
			{
				WString lname = name;
				lname.Lower();
				WString _right = lname.Right(4);
				if (_right == L".ini")
				{
					WString lbuff = ppath;
					lbuff += lname;
					UString buff1;
					UString buff;
					buff = lbuff;
					MFILE file(buff.GetPtr());
					if (file.Open(false))
					{
						buff1.Clear();
						buff1.SetLen(file.GetSize());
						file.Read(-1, buff1.GetPtr(), buff1.GetLen());
						buff1.Add() = 0;
						file.Close();
						WString wstr, str;
						str = buff1;

						wstr = npath;
						wstr += lname;

						buff = wstr;
						MFILE file1(buff.GetPtr());
						if (file1.Open(true))
						{

							file1.Write(-1, str.GetPtr(), str.StrLen() * 2);
							file1.Close();
						}


					}

				}
				else
				{
					WString wstr = npath;
					wstr += name;
					CopyFile(res.GetPtr(), wstr.GetPtr(), FALSE);
				}

			}

		memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));
	} while (FindNextFileW(hFile, &FindFileData));
	FindClose(hFile);

}

void ConvertANSI2UTF8(const WString& folder, const WString& newfolder)
{
	WString ppath = folder;
	WString npath = newfolder;
	WString  path;
	path = ppath.GetPtr();
	int ret = 0;
	int len = path.StrLen();
	if (path.Right(1) != L"\\")
		path += L"\\";
	if (npath.Right(1) != L"\\")
		npath += L"\\";

	ppath = path;
	path += L"*.*";

	WIN32_FIND_DATAW FindFileData;
	memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));

	HANDLE hFile = FindFirstFileW(path, &FindFileData);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dw = GetLastError();
		if (GetLastError() != ERROR_FILE_NOT_FOUND)
			return;
		return;
	}
	do
	{

		if (!wcscmp(FindFileData.cFileName, L"."))
			continue;
		if (!wcscmp(FindFileData.cFileName, L".."))
			continue;
		WString res = ppath;
		res += FindFileData.cFileName;
		WString name = FindFileData.cFileName;
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			if (name != L"h2" && name != "Linux" && name != L"utf"  && name != "PLUKPROG_" && name != L"old")
			{
				WString nres = npath;
				if (FindFileData.cFileName[0] != L'.')
				{
					nres += FindFileData.cFileName;
					CreateDirectory(nres.GetPtr(), 0);
					ConvertANSI2UTF8(res, nres);
				}
			}
		}
		else
			if (name.StrLen() > 4)
		{
			WString lname = name;
			lname.Lower();
			WString _right = lname.Right(4);
				if (_right == L".scw"  || _right ==L".pir"  || _right == L".plk" || _right ==L".cpp"  || lname.Right(2) ==L".h" || _right == L".inl")
			{
					if (_right == L".scw" || _right == L".pir" || _right == L".plk")
					{
						lname = name;
					}
				WString lbuff = ppath;
				lbuff += lname;
				TString buff;
				buff = lbuff;
				MFILE file(buff.GetPtr());
				if (file.Open(false))
				{
					buff.Clear();
					buff.SetLen(file.GetSize());
					file.Read(-1, buff.GetPtr(), buff.GetLen());
					buff.Add() = 0;
					file.Close();
					UString str;
					WString wstr;
					wstr = buff;
					str = wstr;

					wstr = npath;
					wstr += lname;

					buff = wstr;
					MFILE file1(buff.GetPtr());
					if (file1.Open(true))
					{

						file1.Write(-1, str.GetPtr(), str.StrLen());
						file1.Close();
					}


				}
			
			}
				else
				{
					WString wstr = npath;
					wstr += name;
					CopyFile(res.GetPtr(), wstr.GetPtr(), FALSE);
				}

		}

		memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));
	} while (FindNextFileW(hFile, &FindFileData));
	FindClose(hFile);

}