// TimerAndView.cpp : implementation file
//

#include "stdafx.h"
#include "AllergoscopeApp.h"
#include "TimerAndView.h"
#include "afxdialogex.h"
#include "shlobj.h"
#include "NetDir.h"
#include "_ffile.h"

void Idle();
CEdit* pOnError = 0;
HWND GLOBAL_WND = 0;
void OutText(HWND hWnd, LPCTSTR b)
{
	CString* str = new CString();
	*str = b;
	if (GLOBAL_WND)
	PostMessage(GLOBAL_WND, USER_ON_ADD_TRACE, (WPARAM)hWnd, (LPARAM)str);
	/*

	if (!IsWindow(hWnd)) return;
	int len = GetWindowTextLength(hWnd);
	SendMessage(hWnd, EM_SETSEL, len, len);

	if ((int)(len + mstrlen(b) + 2) >= (int)SendMessage(hWnd, EM_GETLIMITTEXT, 0, 0) - 1)
	{
		SetWindowText(hWnd, 0);
		len = 0;
		len = 0;
	}
	SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)(LPCSTR)b);
	len = GetWindowTextLength(hWnd);
	SendMessage(hWnd, EM_SETSEL, len, len);
	SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)TEXT("\r\n"));
	Idle();
	*/
}

void AddToLog(wchar_t* t)
{
	WString str = t;
	if (pOnError  && IsWindow(pOnError->m_hWnd))
		OutText(pOnError->m_hWnd, str.GetPtr());
	else
		TRACE(str.GetPtr());
}

void AddToLog(const UString &t)
{
	WString str;
	str = t;
	AddToLog(str.GetPtr());
}
CriticalSection onerror;
int OnError(LPCTSTR str)

{
	int err = 0;
	try {
		onerror.Enter();
#ifndef WIN32
		err = errno;
		printf("%s\r\n", strerror(err));
		errno = 0;
		return err;
#else
		DWORD err = WSAGetLastError();
		WString pstr;
		if (str)
			pstr = str;
		if (err)
		{
			LPTSTR buff;
			int n = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&buff, 0, NULL);
			if (n)
			{
				if (pstr.StrLen())
					pstr += L" ";
				pstr += buff;
				LocalFree(buff);

			}
			WSASetLastError(0);
		}
		if (pstr.StrLen())
			AddToLog(pstr.GetPtr());
	onerror.Leave();
	}
	catch (...)
	{
		onerror.Leave();
	}
	return err;
#endif
}
int OnError()
{
	return OnError(0);
}

#define MAX_CONECTION 5

DWORD HostFunc(HANDLE h)
{
	SetThreadLocale(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN));


	HostData* hs = (HostData*)h;
	HWND hWnd = hs->hWnd;
	CEdit* pTrace = hs->pTraceError;
	CEdit* pTraceData = hs->pTraceData;
	WSAEVENT handle[2] = { hs->hExitEvent, WSACreateEvent() };
	SOCKET hAccept, hHost = hs->hSocket;
	WSANETWORKEVENTS NetworkEvents;
	CString str;


	if (WSAEventSelect(hHost, handle[1], FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{

		OnError(L"Error set ACCEPT Event");
		goto hostExit;

	}
	if (listen(hHost, MAX_CONECTION))
	{

		OnError();
		goto hostExit;
	}
	DWORD dwEvent;
	str = TEXT("Host Listen");

	OutText(pTraceData->m_hWnd, str);

	while (TRUE)
	{
		if ((dwEvent = WSAWaitForMultipleEvents(2, handle, FALSE, 1000, FALSE)) == WSA_WAIT_FAILED)
		{
			OnError();
			break;
		}

		if (dwEvent == WSA_WAIT_EVENT_0)
			break;

		if (WSAEnumNetworkEvents(hHost, handle[1], &NetworkEvents) == SOCKET_ERROR)
		{

			OnError();
			break;
		}

		if (NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			WSACloseEvent(handle[1]);
			break;
		}
		if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (NetworkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{

				OnError();
				continue;
			}

			if ((hAccept = accept(hHost, 0, 0)) == INVALID_SOCKET)
			{

				OnError(L"accept error");

			}
			else
				SendMessage(hWnd, USER_ON_ACCEPT, 0, (LPARAM)hAccept);

		}
	}
hostExit:
	closesocket(hHost);
	WSACloseEvent(handle[1]);
	return 0;

}
#if (0)
DWORD DirMonitor(HANDLE h)
{
	SetThreadLocale(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN));

	MonitorData* monitorData = (MonitorData*)h;
	FFileINI file("allergoscopeeditor.ini", true);

	CString dir = file.GetString("DIR", "DATA");
	HANDLE hDir = CreateFile(dir,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	if (hDir == INVALID_HANDLE_VALUE) return 0;
	OVERLAPPED o;
	o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE handle[2];
	handle[0] = monitorData->eventExit;
	handle[1] = o.hEvent;
	DWORD outSize = 1024 * 1024;

	MArray<BYTE> out(outSize);
	void *pBuf = out.GetPtr();
	DWORD dwRes;
	BOOL fOk;
	DWORD cbOffset;
	NetDir::TreeDir dr;

	while (1)
	{
		out.Set(0);
		fOk = ReadDirectoryChangesW(hDir,
			pBuf,
			outSize,
			TRUE,
			
			FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE,
			&dwRes,
			&o,
			NULL);
		if (!fOk) break;
		DWORD event = WaitForMultipleObjects(2, handle, FALSE, INFINITE);
		if (event == WAIT_OBJECT_0)
			break;
		if (event == WAIT_OBJECT_0 + 1)
		{
			GetOverlappedResult(hDir, &o, &dwRes, TRUE);
			PFILE_NOTIFY_INFORMATION fni = (PFILE_NOTIFY_INFORMATION)pBuf;
			BOOL fSend = FALSE;
			MVector<UString>* p = new  MVector<UString>();
			do
			{
				
				cbOffset = fni->NextEntryOffset;
				if (fni->Action == FILE_ACTION_ADDED || fni->Action == FILE_ACTION_MODIFIED)
				{
					DWORD dw;
					WString ph = dir;
					if (ph.Right(1) != L"\\" && ph.Right(1) != L"/")
						ph += "\\";
					ph += fni->FileName;
					UString uph = fni->FileName;
					TRACE(L"%s\r\n", fni->FileName);
					if ((dw = GetFileAttributesW(ph.GetPtr())) != 0xFFFFFFFF && (dw & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
					{
						p->Add() = uph;
//						dr.AddDir(uph);
						fSend = TRUE;
					}
					else
						if ((dw = GetFileAttributesW(ph.GetPtr())) != 0xFFFFFFFF && (dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
						{
							WString ext = ph.Right(4);
							if (ext == L".jpg" || ext == L".nam" || ext == L".ctr")
							{
								p->Add() = uph;
								fSend = true;
								//dr.AddFile(uph);
							}
						}

					
				}
				fni = (PFILE_NOTIFY_INFORMATION)((LPBYTE)fni + cbOffset);

			} while (cbOffset);

			if (fSend)
			{
				/*UString str;
				dr.MakeXML(str);
				WString w = str;
				TRACE(L"%s\r\n", w.GetPtr()); */
				PostMessage(monitorData->hWnd, USER_ON_ADD_BIN, 0, (LPARAM)p);
			}
			else
				delete p;
		}
	}
	return 0;
}
#else

void UpdateDB()
{
	FFileINI file("allergoscopeeditor.ini", true);

	CString FileName = file.GetString(L"PATH", L"PLUK");
	if (FileName.GetBuffer(0)[FileName.GetLength() - 1] != '\\')
		FileName += "\\";
	CString sz = FileName;
	sz += L"dbupdate.exe ";
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	if (CreateProcess(NULL, sz.GetBuffer(0),
		NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		while (WaitForSingleObject(pi.hProcess, 10000) == WAIT_TIMEOUT)
		{
			Sleep(100);
		}
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}
void SaveChanged(RBSet<WString> &set)
{
	if (!set.Len()) return;
	FFileINI file("allergoscopeeditor.ini", true);

	CString FileName = file.GetString("DIR", "DATA");
	if (FileName.GetBuffer(0)[FileName.GetLength() - 1] != '\\')
		FileName += L"\\";
	CString sz = FileName;
	file.Close();
	RBSet<WString>::iterator it(set);
	NetDir::TreeDir dir_changes;
	for (WString* p = it.begin(); p; p = it.next())
	{
	DWORD dw;
		WString ph = FileName;
		ph += *p;
		if ((dw = GetFileAttributesW(ph.GetPtr())) != 0xFFFFFFFF && (dw & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			dir_changes.AddDir(*p);
		}
		else
			if ((dw = GetFileAttributesW(ph.GetPtr())) != 0xFFFFFFFF && (dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				dir_changes.AddFile(*p);
			}

	}
	WString s;
	dir_changes.MakeXML(s);
	CString sz1 = sz;
	int n = 0;
	if (GetFileAttributes(sz) == 0xFFFFFFFF) return;
	do
	{
		CString s;
		s.Format(L"upd%d.lg", n);
		sz = sz1 + s;
		++n;
	} while (GetFileAttributes(sz) != 0xFFFFFFFF);
	
	sz1 += "___";
	HANDLE hFile = CreateFileW(sz1,
		GENERIC_WRITE | GENERIC_READ,
		0,
		0,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

	if (hFile == INVALID_HANDLE_VALUE) return;
	DWORD dw, size = sizeof(wchar_t) * s.StrLen();

	if (!WriteFile(hFile, s.GetPtr(), size, &dw, 0) || dw != (DWORD)size)
	{
		OnError((LPTSTR)0);
	}
	CloseHandle(hFile);
	MoveFile(sz1, sz);

}

#include "MakePolyline.h"

DWORD DirMonitor(HANDLE h)
{
	
	
	MonitorData* monitorData = (MonitorData*)h;
	h = monitorData->eventExit;
	FFileINI file("allergoscopeeditor.ini", true);
	TString tfolder = file.GetString("DIR", "DATA");
	if (tfolder.Right(1) == "\\")
		tfolder[tfolder.StrLen() - 1] = 0;
	WString folder = tfolder;
	WString path = folder;
	
	NetDir::TreeDir* pDir = new NetDir::TreeDir(path);
	int I = 0;
	while (true)
	{
		DWORD event = WaitForSingleObject(h, 3000);
		if (event == WAIT_OBJECT_0)
			break;
		(dynamic_cast<AllergoscopeApp*>(AfxGetApp()))->waitExe.Leave();
		FFileINI file("allergoscopeeditor.ini", true);

		TString tfolder = file.GetString("DIR", "DATA");
		if (tfolder.Right(1) == "\\")
			tfolder[tfolder.StrLen() - 1] = 0;
		
		WString dir = tfolder;
		if (folder != dir)
		{
			I = 0;
			folder = dir;
			path = folder;
			delete pDir;
			pDir = new NetDir::TreeDir(path);
		}
		else
		if (monitorData->hWnd != 0 && !monitorData->hWnd->client.GetLen())
		{
			NetDir::DirNode* root = pDir->Root();
			if (root)
			{
				MArray<NetDir::FileNode*> arr;
				root->CheckDir(folder, L"*.*", arr);
				if (arr.GetLen())
				{
					//MVector<WString>* p = new  MVector<WString>();
					RBSet<WString> data;
					for (int i = 0; i < arr.GetLen(); ++i)
					{
						WString s;
						s = folder;

						s = arr[i]->GetPath();
						if (s.StrLen() > 3 && s.Right(3) != "___" && s.Right(3) != ".lg")
						{
							s.ReplaceAll("/", "\\");
							//p->Add() = s.GetPtr();
							data.Add(s);
						}

					}
					I = 0;
					//UpdateDB();
					delete pDir;
					pDir = new NetDir::TreeDir(path);
					SaveChanged(data);
					//PostMessage(monitorData->hWnd, USER_ON_ADD_BIN, 0, (LPARAM)p);
				}
				else
					++I;

			}
	
		}
		(dynamic_cast<AllergoscopeApp*>(AfxGetApp()))->waitExe.Leave();
	}
	delete pDir;
	return 0;
}
#endif
IMPLEMENT_DYNAMIC(TimerAndView, CDialog)

TimerAndView::TimerAndView(CWnd* pParent /*=nullptr*/)
	
{
	Create(IDD_TimerAndView, pParent);
}

TimerAndView::~TimerAndView()
{
	//ClosePort();
}

void TimerAndView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OnHost, m_OnHost);
	DDX_Control(pDX, IDC_ONEROR, m_OnError);
	DDX_Control(pDX, IDC_EDITPATH, m_EditPath);
	DDX_Control(pDX, IDC_ONPORT, m_EditPort);
	pOnError = &m_OnError;
}


BEGIN_MESSAGE_MAP(TimerAndView, CDialog)
	ON_WM_TIMER()
	ON_MESSAGE(USER_ON_TRAY, &TimerAndView::OnUserOnTray)
	ON_MESSAGE(USER_ON_ACCEPT, &TimerAndView::OnUserOnAccept)
	ON_MESSAGE(USER_CLIENT_CLOSE, &TimerAndView::OnUserOnClientClose)
	ON_BN_CLICKED(IDC_ONPATH, &TimerAndView::OnBnClickedOnpath)
	ON_MESSAGE(USER_ON_WAIT_ROI, &TimerAndView::OnUserWaitRoi)
	ON_MESSAGE(USER_ON_ADD_BIN, &TimerAndView::OnUserAddBin)
	ON_MESSAGE(USER_ON_ADD_TRACE, &TimerAndView::OnUserTrace)
	ON_MESSAGE(USER_ON_RETRANSLATE, &TimerAndView::OnUserRetranslate)


	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_CLOSE, &TimerAndView::OnBnClickedClose)
END_MESSAGE_MAP()


// TimerAndView message handlers

LRESULT TimerAndView::OnUserRetranslate(WPARAM wParam, LPARAM lParam)
{
	static bool fWait = 0;
	static int count = 0;
	++count;
	if (fWait) return 0;
	MDNSReader::SendData res;
	WString changes;
	fWait = true;
	do {
		--count;
		if (reader->Receive(res))
		{


			UString num_connection;
			num_connection.Format("%d", MAX_CONECTION - client.GetLen());
 MVector<UString> sarr;
                        MVector<MArray<wchar_t> > s1;
			MDNSStruct::ReadMDNS(res.rsend.GetPtr(), res.rsend.GetLen(), sarr, s1);
                        UString ss = inet_ntoa(res.addr.sin_addr);
                        if (!names.Find(ss))
                        {
                                ss += " : ";
                                for (int i = 0, l = sarr.GetLen(); i < l; ++i)
                                {
                                    ss += sarr[i];
                                    ss += " ";
                                }
                                ss += "\r\n";
                                AddToLog(ss.GetPtr());
                                if (sarr.GetLen() > 2)
                                if (sarr[2].Find("allergoscope", 0) != -1)
                                names.Add(inet_ntoa(res.addr.sin_addr));
                        }
/*
			res.addr.sin_port = htons((USHORT)5353);
			//dns.Write(res.addr, msg, num_connection, changes);
			names.Add(inet_ntoa(res.addr.sin_addr));
*/
		}
		else
			break;
	} while (count > 0);

	fWait = false;
	return 0;
}

void TimerAndView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 100)
	{
		UString num_connection;
		WString changes, str;
		if (!dir_changes.IsEmpty())
		{
			if (dir_changes_tic_coint == 10)
			{
				dir_changes_tic_coint = 0;
				dir_changes.Clear();
			}
			else
			if (!client.GetLen())
			{
				char szPath[128] = "";
				gethostname(szPath, sizeof(szPath));
				dir_changes.MakeXML(str);
				
				WString nm = szPath;
				//changes.SetLen(nm.StrLen() + str.StrLen() + 2);
				changes = nm.GetPtr();
				changes += L":";
				changes += str.GetPtr();
				++dir_changes_tic_coint;
			}
		}
		num_connection.Format("%d", MAX_CONECTION - client.GetLen());
		RBSet<TString>::iterator it(names);
		for (TString* p = it.begin(); p; p = it.next())
		{
			sockaddr_in m_saHostGroup;
			memset(&m_saHostGroup, 0, sizeof(m_saHostGroup));
			InetPtonA(AF_INET, p->GetPtr(), &m_saHostGroup.sin_addr.s_addr);
			m_saHostGroup.sin_family = AF_INET;
			m_saHostGroup.sin_port = htons((USHORT)8281);

			dns.Write(m_saHostGroup, msg, num_connection, changes);
		}
	}

	CDialog::OnTimer(nIDEvent);
}


BOOL TimerAndView::OnInitDialog()
{
	CDialog::OnInitDialog();
	FFileINI file("allergoscopeeditor.ini", true);
	reader = new MDNSReader(m_hWnd, USER_ON_RETRANSLATE, "224.0.0.251", 5353);
	if (0)
	{
		NetDir::TreeDir Dir("C:\\aller");
		MArray<NetDir::FileNode*> files;
		MVector<WString> mask;
		mask.Add() = ".jpg";
		MVector<WString> snd;
		Dir.GetFileList(mask, files);
		for (int i = 0, l = files.GetLen(); i < l; ++i)
		{
			WString ph = "C:\\aller";
			ph += files[i]->GetPath();
			ph.Replace(L".jpg", L".bin");
			snd.Add() = ph;

		}
		MVector<Variable> var;
		MakePolyLine line(snd, var, FALSE);
		snd.Clear();

	}
	CString sz = file.GetString(L"PATH", L"PLUK");
	if (!sz.GetLength())
	{
		BROWSEINFO bi;
		ITEMIDLIST  __unaligned * lpi = 0;
		memset(&bi, 0, sizeof(BROWSEINFO));
		bi.hwndOwner = m_hWnd;
		bi.lpszTitle = L"Select Pluk directory";
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		lpi = SHBrowseForFolder(&bi);
		WString path(MAX_PATH);
		if (lpi &&  SHGetPathFromIDList(lpi, path.GetPtr()))
		{
			if (path.Right(1) != L"\\")
				path += L"\\";

			file.WriteString(L"PATH", L"PlUK", path.GetPtr());
		}

		if (lpi)
		{
			IMalloc* pMalloc;
			if (!FAILED(SHGetMalloc(&pMalloc)))
			{
				pMalloc->Free(lpi);
				pMalloc->Release();
			}
			OnBnClickedOnpath();
		}

	}
	CString FileName = file.GetString(L"PATH", L"PLUK");
	if (FileName.GetBuffer(0)[FileName.GetLength() - 1] != '\\')
		FileName += "\\";
	sz = FileName;
		sz += L"nn-test.exe";
	STARTUPINFO si = { sizeof(si) };
	memset(&pi, 0, sizeof(pi));
	CreateProcess(NULL, sz.GetBuffer(0),	NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);


	CString name = file.GetString("DIR", "DATA");

	m_EditPath.SetWindowText(name);
	m_EditPort.SetWindowText(TEXT("8182"));
	NetServerThread::rwLock = new RWLock();
	monitorData.hWnd = this;
	monitorData.eventExit = m_Dir.ExitEvent();
	m_Dir.Start((LPTHREAD_START_ROUTINE)DirMonitor, &monitorData);

	dns.Start();
	
	OpenPort();

	SetTimer(100, 500, 0);
	GLOBAL_WND = m_hWnd;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


afx_msg LRESULT TimerAndView::OnUserOnAccept(WPARAM wParam, LPARAM dwSock)
{
static int busy = 0;
	if (busy)
	{
		PostMessage(USER_ON_ACCEPT, wParam, dwSock);
		return 0;
	}
	busy = 1;

	SOCKADDR_IN Addr;
	UString adr_port;
	WString ws;
	int AddrLen = sizeof(Addr);
	NetServerThread* th;
	HostData tmpData;
	try {
		if (getpeername((SOCKET)dwSock, (sockaddr*)&Addr, &AddrLen) == 0)
		{
			adr_port.Format("%s:%d", inet_ntoa(Addr.sin_addr), ntohs(Addr.sin_port));
			ws = L"Connected ";
			ws += adr_port;
			OutText(m_OnHost.m_hWnd, ws.GetPtr());
			th = new NetServerThread();
			tmpData.adr_port = adr_port;
			tmpData.hWnd = m_hWnd;
			tmpData.hExitEvent = 0;
			tmpData.hSocket = (SOCKET)dwSock;
			tmpData.pTraceError = &m_OnError;
			tmpData.pTraceData = &m_OnHost;

			if (th->Connect(tmpData))
				client.Add() = th;
			else
			{
				OnError(L"Connect to thread error");
				delete th;
				closesocket((SOCKET)dwSock);
			}
		}
		else
		{
			OnError(L"getpeername error");

			closesocket((SOCKET)dwSock);
		}
	}
	catch (...)
	{
		OnError(L"OnUserOnAccept throw");

		busy = 0;
	}

	busy = 0;
	return 0;
}


void TimerAndView::OnBnClickedOnpath()
{
	CRect r1, r2, r3, r4;
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(m_hWnd, &wp);
	GetWindowRect(&r2);

	::GetWindowPlacement(m_EditPath.m_hWnd, &wp);
	r3 = wp.rcNormalPosition;
	m_EditPath.GetWindowRect(&r1);
	GetWindowRect(&r2);
	r4 = r1;
	ScreenToClient(&r4);
	m_Dir.Stop();
	BOOL ret = false;
	BROWSEINFO bi;
	ITEMIDLIST  __unaligned * lpi = 0;
	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.lpszTitle = L"Select Allergoscope data directory";
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | 0x0040;
	lpi = SHBrowseForFolder(&bi);
	WString path(MAX_PATH);
	if (lpi && SHGetPathFromIDList(lpi, path.GetPtr()))
	{
		ret = TRUE;
		if (path.Right(1) != L"\\")
			path += L"\\";
		FFileINI file("allergoscopeeditor.ini", true);

		file.WriteString("DIR", "DATA", path.GetPtr());
		m_EditPath.SetWindowText(path.GetPtr());
	}

	if (lpi)
	{
		IMalloc *pMalloc;
		if (!FAILED(SHGetMalloc(&pMalloc)))
		{
			pMalloc->Free(lpi);
			pMalloc->Release();
		}
	} 
	monitorData.hWnd = this;
	monitorData.eventExit = m_Dir.ExitEvent();
	m_Dir.Start((LPTHREAD_START_ROUTINE)DirMonitor, &monitorData);

	//OleUninitialize();

}
void TimerAndView::OpenPort()
{
	m_HostData.hSocket = INVALID_SOCKET;
	//m_tmpData.hClient = INVALID_SOCKET;
	CString ss;
	int port;
	m_EditPort.GetWindowText(ss);
	UString s;
	s = ss.GetBuffer(0);

	sscanf_s(s.GetPtr(), "%d", &port);
	if ((m_HostData.hSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0)) == INVALID_SOCKET)
	{
		OnError();
		closesocket(m_HostData.hSocket);
		m_HostData.hSocket = INVALID_SOCKET;
		return;
	} 
	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons((USHORT)port);
	if (bind(m_HostData.hSocket, (PSOCKADDR) &InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		OnError();
		closesocket(m_HostData.hSocket);
		m_HostData.hSocket = INVALID_SOCKET;
		return;
	}
	m_HostData.hWnd = m_hWnd;
	m_HostData.hExitEvent = (WSAEVENT)m_Host.ExitEvent();
	m_HostData.pTraceError = &m_OnError;
	m_HostData.pTraceData = &m_OnHost;
	if (!m_Host.Start((LPTHREAD_START_ROUTINE)HostFunc, &m_HostData))
	{
		OnError();
		closesocket(m_HostData.hSocket);
		m_HostData.hSocket = INVALID_SOCKET;

	}
	else
	{
		monitorData.hWnd = this;
		monitorData.eventExit = m_Dir.ExitEvent();
		m_Dir.Start((LPTHREAD_START_ROUTINE)DirMonitor, &monitorData);
	}
	
}
void TimerAndView::ClosePort()
{
	if (NetServerThread::rwLock)
	{
		pOnError = 0;
		m_Host.Stop();
		dns.End();
		delete NetServerThread::rwLock;
		NetServerThread::rwLock = 0;
	}


}
LRESULT TimerAndView::OnUserOnClientClose(WPARAM wParam, LPARAM pThread)
{
	for (int i = 0, l = client.GetLen(); i < l; ++i)
	{
		if ((LPARAM)client[i] == pThread)
		{

			WString tmp = "Disconnect ";
			tmp += client[i]->m_HostData.adr_port;
			OutText(m_OnHost.m_hWnd, tmp.GetPtr());
			delete client[i];
			client.Remove(i, 1);
			break;
		}
	} 
	return 0;
}
LRESULT TimerAndView::OnUserTrace(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;
	CString *str = (CString*)lParam;
	if (!::IsWindow(hWnd))
	{
		delete str;
		return 0;
	}
	int len = ::GetWindowTextLength(hWnd);
	::SendMessage(hWnd, EM_SETSEL, len, len);
	LPCTSTR b = *str;
	if ((int)(len + mstrlen(b) + 2) >= (int)::SendMessage(hWnd, EM_GETLIMITTEXT, 0, 0) - 1)
	{
		::SetWindowText(hWnd, 0);
		len = 0;
	}
	::SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)(LPCTSTR)b);
	len = ::GetWindowTextLength(hWnd);
	::SendMessage(hWnd, EM_SETSEL, len, len);
	::SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)TEXT("\r\n"));
	delete str;
	return 0;
}
LRESULT TimerAndView::OnUserAddBin(WPARAM wParam, LPARAM lParam)
{
	if (lParam)
	{
		MVector<WString>* p = (MVector<WString> *)(lParam);
		FFileINI file("allergoscopeeditor.ini", true);
		WString dir = file.GetString("DIR", "DATA");
		file.Close();
		if (dir.Right(1) != L"\\" && dir.Right(1) != "/")
			dir += "\\";
		DWORD dw;
		for (int i = 0, l = p->GetLen(); i < l; ++i)
		{
			WString ph = dir;
			WString ph1 = (*p)[i];
			ph += ph1;
			if ((dw = GetFileAttributesW(ph.GetPtr())) != 0xFFFFFFFF && (dw & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				dir_changes.AddDir((*p)[i]);
			}
			else
				if ((dw = GetFileAttributesW(ph.GetPtr())) != 0xFFFFFFFF && (dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
				{
					dir_changes.AddFile((*p)[i]);
				}

		}
		dir_changes_tic_coint = 0;



	}
	return 0;
}
LRESULT TimerAndView::OnUserWaitRoi(WPARAM wParam, LPARAM lParam)
{  
	LRESULT res = 0;
	if (lParam)
	{
		WString* data = (WString*)(lParam);
		msg.Add(*data);
		delete data;
	}
	else
		if (wParam)
		{
			WString* data = (WString*)(wParam);

			int i = msg.Find(*data);
			if (i != -1)
			{
				res = 1;
				msg.Remove(i, 1);
			}
			delete data;
		}

	return res;

	

}
LRESULT TimerAndView::OnUserOnTray(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN)
	{

		ShowWindow(SW_SHOWNORMAL);
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = m_hWnd;
		nid.uID = 1000;
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}
	/*else
	{
		if (lParam == WM_RBUTTONDOWN)
		{
			CMenu menu;
			menu.LoadMenu(IDR_MENU1);
			CMenu* pSubMenu = menu.GetSubMenu(0);
			CPoint pt;
			GetCursorPos(&pt);
			SetForegroundWindow();
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}
	*/
	return 0;
}


void TimerAndView::OnDestroy()
{
	CDialog::OnDestroy();
	m_Dir.Stop();
	msg.Clear();
	ClosePort();
	TerminateThread(pi.hThread, 0);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	::PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0, 0);

}
/*

void TimerAndView::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	
	CDialog::OnClose();
	ClosePort();
	::PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0, 0);
	
}
*/

void TimerAndView::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE)
	{
		NOTIFYICONDATA nid;

		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = m_hWnd;
		nid.uID = 1000;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.uCallbackMessage = USER_ON_TRAY;
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_SMALL);
		wcscpy_s(nid.szTip, L"Allergoscope");
		nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

		Shell_NotifyIcon(NIM_ADD, &nid);
		ShowWindow(SW_HIDE);

	}
	else
	CDialog::OnSysCommand(nID, lParam);
}
void TimerAndView::Minimize()
{
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	nid.uID = 1000;
	
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = USER_ON_TRAY;
	nid.hIcon = AfxGetApp()->LoadIcon(IDI_SMALL);
	wcscpy_s(nid.szTip, L"Allergoscope");
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	Shell_NotifyIcon(NIM_ADD, &nid);
	ShowWindow(SW_HIDE);

}

//--------------------------------------
//
//--------------------------------------
BOOL TimerAndView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch((short int)wParam)
	{
	 case IDOK:	
	 case IDCANCEL:
		 Minimize();
		return 0;

		
	}
	return CDialog::OnCommand(wParam, lParam);
}




void TimerAndView::OnBnClickedClose()
{
	m_Dir.Stop();
	CDialog::OnClose();
	ClosePort();
	TerminateThread(pi.hThread, 0);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	::PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0, 0);
}
