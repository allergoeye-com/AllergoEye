/*
 * mpprocess.h
 *
 *  Created on: Mar 20, 2012
 *      Author: alex
 */

#ifndef MPPROCESS_H_
#define MPPROCESS_H_
#pragma pack(push) //NADA
#pragma pack(4)
EXPORTNIX class _PEXPORT MPProcess {
public:
    MPProcess()
    {
#ifdef __PLUK_LINUX__
        Pid = 0;
#else
        memset(&Info, 0, sizeof(PROCESS_INFORMATION));
#endif
        fKill = TRUE;
        ExeName = 0;
        fCloseHandle = TRUE;
    }

    virtual ~MPProcess()
    {}

    BOOL Open(const char *_name, const char *args, const char *Environ, bool fCheck, bool fHide);

    BOOL Open(const char *_name, const char *Environ, bool fCheck, bool fHide);
//	BOOL Open(const char *_name, HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr, bool fHide = false);

    void Close();

    int IsRun();

    int Status();

    int Wait();

    bool Resume()
    {
        if (IsRun())
        {
#ifdef __PLUK_LINUX__
            kill(Pid, SIGCONT);
#else
            ::ResumeThread(Info.hThread);
#endif
            return true;
        }
        return false;
    }

    bool Suspend()
    {
        if (IsRun())
        {
#ifdef __PLUK_LINUX__
            kill(Pid, SIGSTOP);
#else
            ::SuspendThread(Info.hThread);
#endif
            return true;
        }
        return false;
    }

#ifdef __PLUK_LINUX__

    int Fork();

#endif
    char *ExeName;
    BOOL fKill;
    BOOL fCloseHandle;
#ifdef __PLUK_LINUX__
    pid_t Pid;
#else
    PROCESS_INFORMATION Info;
#endif
private:
    bool New();
};
#pragma pack(pop) //NADA

#endif /* PROCESS_H_ */
