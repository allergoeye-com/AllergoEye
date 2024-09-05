#include "stdafx.h"
#include "RWLock.h"
#include "NetServerThread.h"
#include "NetDir.h"
#include "TimerAndView.h"
#include "mfile.h"
#include "MakePolyline.h"
#include "_ffile.h"
#include "lzwcomp.h"
#include <Shellapi.h>

#include <ShlDisp.h>

RWLock *NetServerThread::rwLock = 0;
int OnError();
int OnError(LPCTSTR str);
BOOL __CreateDirectoryW(LPTSTR ph, LPSECURITY_ATTRIBUTES n)
{
	WString s = ph;
	s.ReplaceAll("L\\\\", L"\\");
	wchar_t tmp[MAX_PATH];
	wcscpy_s(tmp, MAX_PATH, s.GetPtr());


	return CreateDirectoryW(ph, n);
}
DWORD WINAPI NetServerThread::MessageLoop(void* h)
{
	SetThreadLocale(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN));

	NetServerThread *hs = (NetServerThread*)h;
	WSAEVENT handle[2] = { hs->eExit, hs->eOnRead };
	DWORD dwEvent;
	try {
		if (WSAEventSelect(hs->fd, handle[1], FD_READ | FD_CLOSE) == SOCKET_ERROR)
			goto __exit_th;
		hs->mutex.Leave();
		while (TRUE)
		{
			if ((dwEvent = WSAWaitForMultipleEvents(2, handle, FALSE, WSA_INFINITE, FALSE)) == WSA_WAIT_FAILED)
				goto __exit_th;
			if (dwEvent == WSA_WAIT_TIMEOUT)
			{
				; // hs->mutex.SetLeave(1);
			}

			if (dwEvent == WSA_WAIT_EVENT_0)
			{
				WSAResetEvent(handle[0]);
				goto __exit_th;
			}
			else
				if (dwEvent == WSA_WAIT_EVENT_0 + 1)
				{

					if (WSAEnumNetworkEvents(hs->fd, handle[1], &hs->NetworkEvents) == SOCKET_ERROR)
					{
						OnError();
						goto __exit_th;
					}
					if ((hs->NetworkEvents.lNetworkEvents & FD_CLOSE) == FD_CLOSE)
						goto __exit_th;

					if ((hs->NetworkEvents.lNetworkEvents & FD_READ) == FD_READ)
					{
						WSAResetEvent(handle[1]);
						hs->OnRead(hs->fd);
					}



				}
		}
	}
	catch (...)
	{
		;
	}

__exit_th:

	hs->mutex.Leave();
	hs->Close();
	PostMessage(hs->m_HostData.hWnd, USER_CLIENT_CLOSE, 0, (LPARAM)hs);
	
	return 0;
}
void AddToLog(const UString& t);
int CompareFileTime(SYSTEMTIME& tm1, SYSTEMTIME& tm2);

#ifdef WIN32 
	#include <sys\locking.h>
	#include <sys\stat.h>
	#include <sys\utime.h>
	#include <windows.h>
	#include <time.h>
	#include <process.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <utime.h>
	#define CHAR_T char
#endif
#include "lzwdecom.h"

void fSetAFileTime(LPCTSTR Name,  SYSTEMTIME& tm1)
{
	BOOL res = 0;
#ifdef WIN32
	struct _stat sta;
#endif
#ifdef __PLUK_LINUX__
	struct stat sta;
#endif
#ifdef WIN32
#ifdef UNICODE
	if (_wstat(Name, &sta) == 0)
	{
#else
	if (_stat(Name, &sta) == 0)
	{
#endif
#endif
#ifdef __PLUK_LINUX__
#ifdef UNICODE
		WCHAR Name[_MAX_PATH + 1];
		ConvertFileName(Name, RawName);
		if (wstat(Name, &sta) == 0)
		{
#else
		char Name[_MAX_PATH + 1];
		ConvertFileName(Name, RawName);
		if (stat(Name, &sta) == 0)
		{
#endif
#endif

#ifdef WIN32
			struct _utimbuf utimbu;
#endif
#ifdef __PLUK_LINUX__
			struct utimbuf utimbu;
#endif
			utimbu.modtime = sta.st_mtime;
			utimbu.actime = sta.st_atime;
			tm tm;
			tm.tm_hour = tm1.wHour;		// hour
			tm.tm_min = tm1.wMinute;			// min
			tm.tm_sec = tm1.wSecond;			// sec
			tm.tm_mday = tm1.wDay;		// month day
			tm.tm_yday = 0;		// year day
			tm.tm_wday = 0;		// week day
			tm.tm_mon = tm1.wMonth - 1;		// month
			tm.tm_year = tm1.wYear - 1900;	// year
			tm.tm_isdst = 0;
			utimbu.actime = mktime(&tm);
#ifdef WIN32
#ifdef UNICODE
			res = _wutime(Name, &utimbu) == 0;
#else
			res = _utime(Name, &utimbu) == 0;
#endif
#endif
#ifdef __PLUK_LINUX__
#ifdef UNICODE
			res = wutime(Name, &utimbu) == 0;
#else
			res = utime(Name, &utimbu) == 0;
#endif
#endif
		}
}
BOOL UnZip(WString &wZipFile, WString &wFolder)
{
	BSTR lpZipFile = wZipFile.AllocSysString();
	BSTR lpFolder = wFolder.AllocSysString();
	IShellDispatch* pISD;
	Folder* pZippedFile = 0L;
	Folder* pDestination = 0L;
	bool retval = 0;

	long FilesCount = 0;
	IDispatch* pItem = 0L;
	FolderItems* pFilesInside = 0L;
	WString s;
	VARIANT Options, OutFolder, InZipFile, Item;
	CoInitialize(NULL);
	if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pISD) == S_OK)
	{

		InZipFile.vt = VT_BSTR;
		InZipFile.bstrVal = lpZipFile;
		pISD->NameSpace(InZipFile, &pZippedFile);
		if (pZippedFile)
		{
			OutFolder.vt = VT_BSTR;
			OutFolder.bstrVal = lpFolder;
			pISD->NameSpace(OutFolder, &pDestination);
			if (pDestination)
			{
				pZippedFile->Items(&pFilesInside);
				if (pFilesInside)
				{
					pFilesInside->get_Count(&FilesCount);
					if (FilesCount)
					{
						pFilesInside->QueryInterface(IID_IDispatch, (void**)&pItem);

						Item.vt = VT_DISPATCH;
						Item.pdispVal = pItem;

						Options.vt = VT_I4;
						Options.lVal = 1024 | 512 | 16 | 4;
						retval = pDestination->CopyHere(Item, Options) == S_OK;
						pItem->Release();
					}
				}
			}
		}
		if (pFilesInside)
			pFilesInside->Release();
		if (pDestination)
			pDestination->Release();
		if (pZippedFile)
			pZippedFile->Release();
		if (pISD)
			pISD->Release();
		SysFreeString(lpZipFile);
		SysFreeString(lpFolder);
	}
	return retval;
}
void NetServerThread::OnRead(SOCKET fd)
{
	FFileINI file("dryeye.ini", true);

	WString pathT = file.GetString("DIR", "DATA");
	if (pathT.Right(1) == "\\")
		pathT[pathT.StrLen() - 1] = 0;

	MArray<char> buffer;
	if (!RWThread::OnRead(fd, buffer))
	{
		OnError(L"OnRead error");
		WriteResponce("ERROR", 0, 0);
	}
	else
	{
		FunCall * func = (FunCall*)buffer.GetPtr();
		Param* par1 = func->param;
		if (!strcmp(func->name, "WRITE"))
		{
			if (!strcmp(par1->param, "CALCALL"))
			{
				Param* par2 = (Param*)(par1->param + par1->len);
				WString un = (wchar_t*)par2->param;
				//UString un = nm;
				NetDir::Tree tree(un);
				NetDir::TreeDir dir(&tree);
				MArray<NetDir::FileNode*> list;
				dir.GetFileList(list);
				UString sRet = "OK";
				WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);
				MVector<WString> snd;
				for (int i = 0, l = list.GetLen(); i < l; ++i)
				{
					WString ph = pathT;
					ph += list[i]->GetPath();
					ph.Replace(L".roi", L".jpg");
					snd.Add() = ph;
				}
				rwLock->ReadEnter();
				MVector<Variable> var;
				MakePolyLine line(snd, var, TRUE);
				rwLock->ReadLeave();

			}
			else
			if (!strcmp(par1->param, "FGET"))
			{

				UString ss = m_HostData.adr_port + " FGET";
				AddToLog(ss.GetPtr());
				WString str = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>";
				Param* par2 = (Param*)(par1->param + par1->len);
				WString s((wchar_t*)par2->param);
				MVector<WString> m;
				s.Tokenize(L":", m);
				if (m.GetLen() == 2)
				{

					WString* wait = new WString();

					*wait = s;
					
					if (SendMessage(m_HostData.hWnd, USER_ON_WAIT_ROI, (LPARAM)wait, 0))
					{
						s = m[1];
						NetDir::Tree tree(s);
						NetDir::TreeDir dir(&tree);
						MVector<WString> mask;
						mask.Add() = L".jpg";
						mask.Add() = L".ini";
						rwLock->ReadEnter();
						dir.UpdateAddFile(mask, pathT);
						rwLock->ReadLeave();

						dir.MakeXML(str);
					}
					else
					{
						str += L"<file-area>";
						str += L"</file-area>";
					}
					WString ts = str;
					WriteResponce("WRITE", (BYTE*)ts.GetPtr(), (ts.StrLen() + 1) * 2);

				}


			}
			else
			if (!strcmp(par1->param, "FSYS"))
			{
				UString ss = m_HostData.adr_port + " Read info";
				AddToLog(ss.GetPtr());

				Param *par2 = (Param *)(BYTE *)(par1->param + par1->len);
				WString d = (wchar_t*)par2->param;
				NetDir::Tree tree(d);
				NetDir::TreeDir dir(&tree);
				rwLock->WriteEnter();
				dir.MkDirs(pathT);
				rwLock->WriteLeave();
				MArray<NetDir::FileNode*> list;
				dir.GetFileList(list);

				rwLock->ReadEnter();

				for (int i = 0, l = list.GetLen(); i < l; ++i)
				{
					WString ph = pathT;
					ph += list[i]->GetPath();
					if (GetFileAttributes(ph.GetPtr()) != 0xffffffff)
					{
						UString p;
						p = ph;
						MFILE file(p.GetPtr());
						if (file.Open(false))
						{
							FILETIME lpCreationTime, ftLastWriteTime, lpLastAccessTime;
							SYSTEMTIME st, st1;

							FILEHANDLE hFile = file.File();
							if (GetFileTime(hFile,
								&lpCreationTime,
								&lpLastAccessTime,
								&ftLastWriteTime))
							{
								FILETIME localTime;
								FileTimeToLocalFileTime(&ftLastWriteTime, &localTime);

								FileTimeToSystemTime(&localTime, &st1);

								MVector<WString> date;
								list[i]->date.Tokenize(L"- ", date);
								if (date.GetLen() == 6)
								{
									st.wYear = date[0].ToInt();
									st.wMonth = date[1].ToInt();
									st.wDay = date[2].ToInt();
									st.wHour = date[3].ToInt();
									st.wMinute = date[4].ToInt();
									st.wSecond = date[5].ToInt();
									if (CompareFileTime(st1, st) >= 0)
										dir.DeleteNode(list[i]);
								}
							}
							
						}
						else
						dir.DeleteNode(list[i]);

						
					}
				}
				rwLock->ReadLeave();

			
				WString str = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>";
				dir.MakeXML(str);
				
				WriteResponce("WRITE", (BYTE*)str.GetPtr(), (str.StrLen() + 1) * 2);


			}
			else
				if (!strcmp(par1->param, "RECALC"))
				{
					Param* par2 = (Param*)(par1->param + par1->len);
					WString un = (wchar_t*)par2->param;
					//UString un = nm;
					NetDir::Tree tree(un);
					NetDir::TreeDir dir(&tree);
					MArray<NetDir::FileNode*> list;
					dir.GetFileList(list);
					MVector<WString> snd;
					for (int i = 0, l = list.GetLen(); i < l; ++i)
					{
						WString ph = pathT;
						ph += list[i]->GetPath();
						ph.Replace(L".jpg", L".bin");
						snd.Add() = ph;
					}
					rwLock->ReadEnter();
					MVector<Variable> var;
					MakePolyLine line(snd, var, TRUE);
					rwLock->ReadLeave();
					
					UString sRet = "OK";
					WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);
				}
			else
			if (!strcmp(par1->param, "SENDTO"))
			{

				UString ss = m_HostData.adr_port + " Read SENDTO";
				AddToLog(ss.GetPtr());
				WString *wait = new WString();

				Param* par2 = (Param*)(par1->param + par1->len);
				UString name = (const char*)par2->param;
				*wait = name;
				Param* par3 = (Param*)(par2->param + par2->len);
				*wait += L":";
				WString ptr = (wchar_t*)par3->param;
				//ptr.Copy((char*)par3->param, CP_THREAD_ACP);
				*wait += ptr;
				UString sRet = "OK";
				
				WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);
				PostMessage(m_HostData.hWnd, USER_ON_WAIT_ROI, 0, (LPARAM)wait);

			}
			else
			if (!strcmp(par1->param, "MKDIR"))
			{
				UString ss = m_HostData.adr_port + " MKDIR ";
				AddToLog(ss.GetPtr());
				UString sRet = "ERROR";
				Param* par2 = (Param*)(par1->param + par1->len);
				WString s((wchar_t*)par2->param);
				NetDir::Tree tree(s);
				NetDir::TreeDir dir(&tree);
				rwLock->WriteEnter();
				dir.MkDirs(pathT);
				if (dir.root && dir.root->childs.GetLen() && (func->len - par1->len - (int)sizeof(FunCall) - 2 * (int)sizeof(Param)) > 0)
				{
					Param* par3 = (Param*)(par2->param + par2->len);
						
					WString ph;
					ph = pathT;
					ph += dir.root->childs[0]->name;
					MFILE file(ph.GetPtr());

					if (file.Open(true))
					{
						if (!file.Write(par3->param, par3->len))
							sRet = "ERROR";
						else
							sRet = "OK";
						file.Close();
					}
				}
				rwLock->WriteLeave();
				WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);


			}
			else
			
			if (!strcmp(par1->param, "FILE") || !strcmp(par1->param, "NEW"))
			{
				UString add_str = m_HostData.adr_port + " Write file ";

				UString sRet = "ERROR";
				Param* par2 = (Param*)(par1->param + par1->len);
				WString d = (wchar_t*)par2->param;
				WString s = d;
				WString ph = pathT + s;
				Param* par21 = (Param*)(par2->param + par2->len);
				WString _date;
                _date = (char *)par21->param;
				Param* par3 = (Param*)(par21->param + par21->len);
				//rwLock->ReadEnter();
				///if (GetFileAttributes(ph.GetPtr()) == 0xffffffff)
				{ 
					//rwLock->ReadLeave();
					rwLock->WriteEnter();
					if (GetFileAttributes(pathT.GetPtr()) == 0xffffffff)
						CreateDirectoryW(pathT.GetPtr(), 0);

					MVector<WString> token;
					s.Tokenize(L"/", token);
					WString path = pathT;
					DWORD dw;
					int l = token.GetLen() - 1;
					for (int i = 0; i < l; ++i)
					{
						path += L"\\";
						path += token[i];
						if ((dw = GetFileAttributesW(path.GetPtr())) == 0xFFFFFFFF || (dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
							CreateDirectoryW(path.GetPtr(), 0);
					}
					
					ph.ReplaceAll(L"/", L"\\");
					UString phs = ph;
					AddToLog((add_str + phs).GetPtr());

					MFILE file(phs.GetPtr());
					BOOL fr;
					if (file.Open(true))
					{
						MArray<BYTE> buff1;
						if (par3->len > 4 && !memcmp(par3->param, "PLZ2", 4))
						{
							UnPackLZW pack((BYTE*)par3->param + 4, par3->len - 4);
							pack.Unpack(buff1);
							if (file.Write(buff1.GetPtr(), buff1.GetLen()))
								sRet = "OK";


						}
						else
						{
							if (file.Write(par3->param, par3->len))
								sRet = "OK";
							FILETIME local_ft, ft;
							MVector<WString> date;
							_date.Tokenize(L"- ", date);
							SYSTEMTIME st;
							file.Close();
							if (date.GetLen() == 6)
							{
								st.wDayOfWeek = 0;
								st.wMilliseconds = 0;
								st.wYear = date[0].ToInt();
								st.wMonth = date[1].ToInt();
								st.wDay = date[2].ToInt();
								st.wHour = date[3].ToInt();
								st.wMinute = date[4].ToInt();
								st.wSecond = date[5].ToInt();

								WString p;
								p = phs;
								fr = SystemTimeToFileTime(&st, &local_ft);
								LocalFileTimeToFileTime(&local_ft, &ft);
								//fSetAFileTime(p.GetPtr(), st);
								MFILE file(phs.GetPtr());
								file.Open(false);

								fr = SetFileTime(file.File(), &ft, &ft, &ft);
								file.Close();
							}
						}
					}
					rwLock->WriteLeave();
					WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);
					
					

				}
				/*else
				{
			//		rwLock->ReadLeave();
					sRet = "OK";
					WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);

				}*/


			}
			else
			{
				UString sRet = "ERROR";
				WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);

			}
		}
		else
			if (!strcmp(func->name, "READ"))
			{

				if (!strcmp(par1->param, "ROI"))
				{

					UString sRet = "NOT";
					Param* par2 = (Param*)(par1->param + par1->len);
					UString ss = m_HostData.adr_port + " GET ROI ";

					ss += (const wchar_t *)par2->param;

					WString s = (wchar_t*)par2->param;
					WString ph = pathT + s;
					Variable var;
					rwLock->ReadEnter();
					MakePolyLine line(ph.GetPtr(), var);
					rwLock->ReadLeave();
					if (var.Dim())
					{
						MArray<BYTE> b;
						var.Serialize(b);
						AddToLog(ss.GetPtr());

						WriteResponce("WRITE", (BYTE*)b.GetPtr(), b.GetLen());

					}
					else
						WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);

				}
				else
				if (!strcmp(par1->param, "DIR"))
				{

					UString sRet = "NOT";
					WString ph = pathT;
					UString ss;
					if (func->num_param == 2)
					{
						Param* par2 = (Param*)(par1->param + par1->len);
						ss = m_HostData.adr_port + " GET DIR ";
						ss += (const wchar_t*)par2->param;
						UString s = (wchar_t*)par2->param;
						ph += (s.StrLen() > 1 ? s : L"");
					}
					else
						ss = m_HostData.adr_port + " GET DIR ";
					AddToLog(ss.GetPtr());

					rwLock->ReadEnter();
					NetDir::TreeDir dir(ph);

					rwLock->ReadLeave();
					WString str = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>";
					str.SetAddedRealSize(0x100000);
					dir.MakeXML(str);
					/*MArray<BYTE> dst;
					PackLZW pack((BYTE*)str.GetPtr(), str.StrLen(), dst);
					pack.Pack();
					UString plz = "PLZ2";
					dst.Insert(0, (BYTE*)plz.GetPtr(), 4);
					*/
					WriteResponce("WRITE",(BYTE *) str.GetPtr(), (str.StrLen() + 1)*2);

				}
			
				else
				if (!strcmp(par1->param, "FILE"))
				{
					UString sRet = "ERROR";
					Param* par2 = (Param*)(par1->param + par1->len);
					UString ss = m_HostData.adr_port + " GET FILE ";
					ss += (const wchar_t*)par2->param;
					WString s = (wchar_t*)par2->param;
					MVector< WString > vec;
					s.Tokenize(L";", vec);
					WString ph = pathT + s;
					MVector< MArray<BYTE> > buff;
					rwLock->ReadEnter();
					MArray<BYTE> b;

					if (GetFileAttributes(ph.GetPtr()) != 0xffffffff)
					{
						UString ts;
						ts = ph;
						AddToLog((ss + ts).GetPtr() );
						MFILE file(ts.GetPtr());
						if (file.Open(false))
						{
							b.SetLen(file.GetSize());

							file.Read(0, b.GetPtr(), file.GetSize());
							file.Close();
							rwLock->ReadLeave();
							
							WriteResponce("WRITE", (BYTE*)b.GetPtr(), b.GetLen());
							return;

						}
					}
					rwLock->ReadLeave();
					int i = sRet.StrLen();
					WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), i + 1);

				}
				else
				if (!strcmp(par1->param, "FILES"))
				{
					UString sRet = "ERROR";
					Param* par2 = (Param*)(par1->param + par1->len);
					UString ss = m_HostData.adr_port + " GET FILE ";
					ss += (const wchar_t *)par2->param;
					
					WString s = (const wchar_t*)par2->param;
					MVector< WString > vec;
					s.Tokenize(L";", vec);
					WString ph = pathT + s;
					MVector< MArray<BYTE> > buff;
					rwLock->ReadEnter();

					for (int i = 0, l = vec.GetLen(); i < l; ++i)
					{
						WString ph = pathT + vec[i];
						if (GetFileAttributes(ph.GetPtr()) != 0xffffffff)
						{
							UString ts;
							ts = ph;
							AddToLog((ss + ts).GetPtr());
							MFILE file(ts.GetPtr());
							if (file.Open(false))
							{
								MArray<BYTE>& b = buff.Add();
								b.SetLen(file.GetSize());

								file.Read(0, b.GetPtr(), file.GetSize());
								file.Close();
							}
							else
								buff.Add();
						}
						else
							buff.Add();
					}
					rwLock->ReadLeave();
					if (buff.GetLen() == vec.GetLen())
					{
						int i = sRet.StrLen();
						WriteResponce("WRITE", buff);
					}
					else
					{
						int i = sRet.StrLen();
						WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), i + 1);
					}

				}
				else
				{
					UString sRet = "ERROR";
					WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);

				}



			}
			else
			{
				UString sRet = "ERROR";
				WriteResponce("WRITE", (BYTE*)sRet.GetPtr(), sRet.StrLen() + 1);

			}

	}
	

}
bool NetServerThread::WriteResponce(LPCSTR cmd, MVector < MArray<BYTE> > & param)
{
	int size = 0;
	for (int i = 0, l = param.GetLen(); i < l; ++i)
		size += param[i].GetLen();

	int len = sizeof(FunCall) + (param.GetLen() - 1) * (sizeof(Param) - 1) - 1 + size;
	MArray<char> buff(len);
	FunCall* call = (FunCall*)buff.GetPtr();
	if (call)
	{
		call->Init(cmd);
		call->len = len;
		call->num_param = param.GetLen();
		Param* _param = call->param;
		for (int i = 0, l = param.GetLen(); i < l; ++i)
		{
			_param->len = param[i].GetLen();
			memmove(_param->param, param[i].GetPtr(), param[i].GetLen());
			_param = (Param *)(_param->param + _param->len);

		}
		if (write(call, len))
			return 1;
	}
	return 0;

}

bool NetServerThread::WriteResponce(LPCSTR cmd, BYTE* param, int bufLen)
{
	int len = param ? sizeof(FunCall) +  bufLen - 1 : sizeof(FunCall);
	MArray<char> buff(len);
	FunCall* call = (FunCall*)buff.GetPtr();
	if (call)
	{
		call->Init(cmd);
		if (param)
		{
			call->len = len;
			call->num_param = 1;
			Param* _param = call->param;
			_param->len = bufLen;
			memmove(_param->param, param, bufLen);
		}
		if (write(call, len))
			return 1;
	}
	return 0;
}
BOOL NetServerThread::Close()
{
	return RWThread::Close();
}

bool NetServerThread::Connect(HostData &client)
{
	m_HostData = client;
	fd = client.hSocket;
	eExit = thread.ExitEvent();
	RecvOverlapped.hEvent = WSACreateEvent();
	SendOverlapped.hEvent = WSACreateEvent();
	eOnRead = WSACreateEvent();
	int Size = 1024 * 32;
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&Size, sizeof(Size));

	mutex.Enter();
	return thread.Start((LPTHREAD_START_ROUTINE)MessageLoop, this);
	
}


