#include "stdafx.h"

#ifndef _CONSOLE_PROG_

#ifdef _TRACE_X
#include "trace.h"
CriticalSection TraceXTerm::sec;
bool TraceXTerm::Start(const char *name_wnd)
{
    sec.Enter();
    int mm = time(0);
    bool fRet;
    for (int i = 0; !(fRet = New(name_wnd, mm)) && i < 3; ++i);
    sec.Leave();
    return fRet;
}
void TraceXTerm::Trace(const char *pFormat, ...)
{
    if (!hIn && !buff.GetLen()) return;
    va_list args;

    va_start(args, pFormat);
    vsprintf(buff.GetPtr(), pFormat, args);
    va_end(args);
    write(hIn, (void*)buff.GetPtr(), strlen(buff.GetPtr()));
}
char *TraceXTerm::Read()
{
        buff.Set(0);
        read(hOut, (void*)buff.GetPtr(), strlen(buff.GetPtr()));
        return buff.GetPtr();
}

void TraceXTerm::Stop()
{
    if (Pid)
    {
        if (hOut)
        {
            int f = fcntl(hOut, F_GETFL, 0);
            fcntl(hOut, F_SETFL, f  | O_NONBLOCK);
            close(hOut);
        }
        if (hIn)
        close(hIn);
        kill(Pid, SIGKILL);

         remove(name.GetPtr());
    }
    Pid = 0;
    hIn = hOut = 0;
}
bool TraceXTerm::New(const char *name_wnd, int &i)
{
    buff.SetLen(1024);
    Stop();
    int file;

    name.Format("/tmp/xterm_trace%d", i);
    while((file = open(name, O_RDONLY | O_NONBLOCK, 0)) > 0)
    {
        ++i;
        name.Format("/tmp/xterm_trace%d", i);
        close(file);
    }
    if (!mkfifo(name.GetPtr(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))
    {
        if ((hOut = open(name.GetPtr(), O_RDONLY | O_NONBLOCK, 0)) > 0)
        {
            int f = fcntl(hOut, F_GETFL, 0);
            f = fcntl(hOut, F_SETFL, f & ~O_NONBLOCK);
            TString s1 = "xterm";
            TString s2 = "-title";
            TString s3 = "Output ";
            s3 += name_wnd;
            TString s4 = "+hold";

            TString s5;
            s5.Format("-Sdo/%d", hOut);
        posix_spawnattr_t X;
            posix_spawn_file_actions_t Y;
            char *argv[5];
            argv[0] = s1.GetPtr();
            argv[1] = s2.GetPtr();
            argv[2] = s3.GetPtr();
            //argv[3] = s4.GetPtr();
            argv[3] = s5.GetPtr();
            argv[4] = 0;
            posix_spawnattr_init(&X);
            posix_spawn_file_actions_init(&Y);
            if (!posix_spawn(&Pid, "/usr/bin/xterm",&Y,&X, argv, environ))
            {
                if ((hIn = open(name.GetPtr(), O_WRONLY, 0)) < 0)
                {
                    TRACE("error open file 2 \n");
                    Stop();
                }
            }
            else
            {
                TRACE("error error spawn\n");
                Stop();
            }
        }
        else
            TRACE("error open file 1 \n");

    }
    else
        TRACE("error create pipe file 1 \n");
    return hIn != 0;
}
#endif
#endif
