/*
 * mpprocess.cpp
 *
 *  Created on: Mar 19, 2012
 *      Author: alex
 */
#include "stdafx.h"

#include "mpprocess.h"
#include "base_error.h"

//-------------------------------------
//
//-------------------------------------
int MPProcess::Wait()
{
#ifdef __PLUK_LINUX__
	if (!Pid) return 0;
	int status = 0;
	pid_t pid = waitpid(Pid, &status, 0);
	errno = 0;
	return pid == 0 ? status : 0;
#else
	if (Info.hProcess)
		WaitForSingleObject(Info.hProcess, INFINITE);
	Close();
	return true;
#endif	
}


//-------------------------------------
//
//-------------------------------------
int MPProcess::IsRun()
{
#ifdef __PLUK_LINUX__
	return Pid ? !kill(Pid, 0) : 0;
#else
	DWORD dwRetCode = 0;
	return Info.hThread && GetExitCodeThread(Info.hThread, &dwRetCode) && dwRetCode == STILL_ACTIVE;
#endif
}

//-------------------------------------
//
//-------------------------------------
void MPProcess::Close()
{
	if (IsRun())
	{
#ifdef __PLUK_LINUX__
		if (fKill)
			StdError(kill(Pid, SIGTERM));
	}
	Pid = 0;
#else
		if (fKill)
			TerminateProcess(Info.hProcess, 1);
	}
	if (fCloseHandle)
	if (Info.hThread)
	{
		CloseHandle(Info.hThread);
		CloseHandle(Info.hProcess);
	}	
	memset(&Info, 0, sizeof(PROCESS_INFORMATION));
#endif
	if (ExeName)
		HFREE(ExeName);
	ExeName = 0;
}
void TestChld(int, siginfo_t *sig, void*)
{
    MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);
    if (startup)
    {
        TMSG msg; 
        msg.message = WM_DESTROY;
        msg.iwnd = sig->si_pid;
        memcpy(&msg.sig, sig, sizeof(siginfo_t));
        
        startup->PushFifo(msg);

#ifndef _CONSOLE_PROG_

        MGetApp()->GuiIdle();
#endif

    }   
}
//-------------------------------------
//
//-------------------------------------
BOOL MPProcess::Open(const char *_name, const char *_Environ, bool fCheck, bool fHide)
{
		Close();
	

#ifdef __PLUK_LINUX__
		MVector<TString> wenv;
		if (_Environ)
		{
			const char *env = _Environ;
			while(*env)
			{
				wenv.Add() = env;
				env += strlen(env) + 1;
			}
		}
		else
		{
			for (int i = 0; environ[i]; ++i)
				wenv.Add() = environ[i];
		}
		MVector<TString> Params;
        TString _en = _name;
		_en.Tokenize(" ", Params);
		ExeName = Params[0].GetPtr();
                _en = ExeName;
                Params[0].Detach();
		Params.Remove(0, 1);
		MVector<TString>  tok;
		_en.Tokenize("\\/", tok);
		_en.Tokenize("\\/", tok);
		int l = tok.GetLen();
		int i = 0;
		TString s1 = tok[l - 1];
		char **argv = (char **)HMALLOC(sizeof(char *) *(Params.GetLen() + 2));
		for (i = 0; i < Params.GetLen(); ++i)
			argv[i + 1] = Params[i].GetPtr();
		argv[0] = tok[l - 1].GetPtr();
		argv[i + 1] = 0;
TaskInfo info;
info.Init(getpid());
    TString _sPath = ExeName;
    i = _sPath.ReverseFind(FD);
    if (i != -1)
    {
        i = chdir(_sPath.GetPtr());
     }
    else
        chdir(AfxGetApp()->sPath);
        

    int res = 0;
#ifndef ANDROID_NDK

		posix_spawnattr_t X;
		posix_spawn_file_actions_t Y;
		posix_spawnattr_init(&X);
       StdError(posix_spawnattr_setflags(&X, POSIX_SPAWN_RESETIDS | POSIX_SPAWN_SETPGROUP | POSIX_SPAWN_SETSIGDEF));
     UXSignal set;
    set.SetHandler((void (*)(int, siginfo_t*, void*))TestChld, SIGCHLD);

       posix_spawnattr_setsigmask(&X, &set); 
		posix_spawn_file_actions_init(&Y);
		res = posix_spawn(&Pid, ExeName,&Y,&X, argv, environ);
       
        posix_spawnattr_destroy(&X);
        posix_spawn_file_actions_destroy(&Y);
#endif

        i = chdir(info.WorkDir.GetPtr());

        if (!res) 
        {
           	while(!IsRun()) Sleep(10);
            return Pid;
        }
        StdError(res);
		
#else
        ExeName = (char *)HMALLOC(strlen(_name)  + 1);
        strcpy(ExeName, _name);
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
/*
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = hChildStdOut;
	si.hStdInput  = hChildStdIn;
	si.hStdError  = hChildStdErr;
*/
	
	si.wShowWindow = fHide ? SW_HIDE : SW_SHOW;
	memset(&Info, 0, sizeof(PROCESS_INFORMATION));
	if (CreateProcess(NULL, ExeName, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, (void *)_Environ, NULL, &si, &Info)) 
	{
		if (!fCheck) return 1;
		DWORD dw = 0;
		if (GetExitCodeProcess(Info.hProcess, &dw))
		{
			if (dw == STILL_ACTIVE)
				return 1;
		}
	}
	DWORD dw1 = GetLastError();
	memset(&Info, 0, sizeof(PROCESS_INFORMATION));
#endif
	if (ExeName)
		HFREE(ExeName);
	ExeName = 0;
	return 0;
}

//-------------------------------------
//
//-------------------------------------
BOOL MPProcess::Open(const char *_name, const char *args, const char *_Environ, bool fCheck, bool fHide)
{
		Close();
		ExeName = (char *)HMALLOC(strlen(_name)  + 1);
		strcpy(ExeName, _name);

	
#ifdef __PLUK_LINUX__
		MVector<TString> wenv;
		if (_Environ)
		{
			const char *env = _Environ;
			while(*env)
			{
				wenv.Add() = env;
				env += strlen(env) + 1;
			}
		}
		else
		{
			for (int i = 0; environ[i]; ++i)
				wenv.Add() = environ[i];
		}
		MArray<char *> anv;
		int lv;
		if ((lv = wenv.GetLen()))
		{
			anv.SetLen(wenv.GetLen() + 1);
			for (int i = 0; i < lv; ++i)
				anv[i] = wenv[i].GetPtr();  				
			anv[lv] = 0;
		}
                TString en = ExeName;
                MVector<TString>  tok;
		en.Tokenize("\\/", tok);
		int l = tok.GetLen();
		int i = 0;
		TString s1 = tok[l - 1];
		TString param;
		if (args) param = args;
		MVector<TString> Params;
		param.Tokenize(" ", Params);
		char **argv = (char **)HMALLOC(sizeof(char *) *(Params.GetLen() + 2));
		for (i = 0; i < Params.GetLen(); ++i)
			argv[i + 1] = Params[i].GetPtr();
		argv[0] = tok[l - 1].GetPtr();
		argv[i + 1] = 0;
		int res = 0;
#ifndef ANDROID_NDK
        posix_spawnattr_t X;
		posix_spawn_file_actions_t Y;
		posix_spawnattr_init(&X);
        posix_spawnattr_setflags(&X, POSIX_SPAWN_RESETIDS | POSIX_SPAWN_SETPGROUP);
		posix_spawn_file_actions_init(&Y);
		res = posix_spawn(&Pid, ExeName,&Y,&X, argv, anv.GetPtr());
         posix_spawnattr_destroy(&X);
        posix_spawn_file_actions_destroy(&Y);
#endif
        if (!res) 
        {
           	while(!IsRun()) Sleep(10);
            return Pid;
        }
        StdError(res);
#else
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.dwXCountChars = 256;
	si.dwYCountChars = 25;
/*
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = hChildStdOut;
	si.hStdInput  = hChildStdIn;
	si.hStdError  = hChildStdErr;
*/
	
	si.wShowWindow = fHide ? SW_HIDE : SW_SHOW;

	memset(&Info, 0, sizeof(PROCESS_INFORMATION));
	TString f = ExeName;
	f += " ";
	f += args;
	if (CreateProcess(NULL, f.GetPtr(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &Info)) 
	{
		if (!fCheck) return 1;
		DWORD dw;
		if (GetExitCodeProcess(Info.hProcess, &dw))
		{
			if (dw == STILL_ACTIVE)
				return 1;
		}
	}
	DWORD dw1 = GetLastError();
        memset(&Info, 0, sizeof(PROCESS_INFORMATION));
#endif
	if (ExeName != 0)
		HFREE(ExeName);
	ExeName = 0;
	return 0;
}
/*
//-----------------------------------------------------------
//
//----------------------------------------------------------
BOOL MPProcess::Open(const char *_name, HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr, bool fHide)
{
	Close();
	ExeName = (char *)HMALLOC(strlen(_name)  + 1);
	strcpy(ExeName, _name);
	ExeName = 0;
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = hChildStdOut;
	si.hStdInput  = hChildStdIn;
	si.hStdError  = hChildStdErr;
	
	si.wShowWindow = fHide ? SW_HIDE : SW_SHOW;

	memset(&Info, 0, sizeof(PROCESS_INFORMATION));

	if (CreateProcess(NULL, ExeName, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &Info)) 
	{
		DWORD dw = 0;
		if (GetExitCodeProcess(Info.hProcess, &dw))
		{
			if (dw == STILL_ACTIVE)
				return 1;
		}
	}
	DWORD dw1 = GetLastError();
	memset(&Info, 0, sizeof(PROCESS_INFORMATION));
	if (ExeName)
		HFREE(ExeName);
	ExeName = 0;
	return 0;
}
*/
