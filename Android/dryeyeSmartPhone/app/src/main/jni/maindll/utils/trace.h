/*
 * trace.h
 *
 *  Created on: Sep 12, 2010
 *      Author: rink
 */

#ifndef TRACE_H_
#define TRACE_H_
#ifndef _CONSOLE_PROG_

#ifdef __PLUK_LINUX__
#include <sys/poll.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <spawn.h>
#include "mtree.h"
#include "critical.h"

extern "C" class TraceXTerm {
public:
    TraceXTerm()
    {
        hIn = hOut = 0;
        Pid = 0;
        buff.SetLen(1024);
    }
    virtual ~TraceXTerm() { Stop(); }
    bool Start(const char *name_wnd);
    void Trace(const char *pFormat, ...);
        char *Read();
    void Stop();
    static CriticalSection sec;
private:
    bool New(const char *name_wnd, int &i);
    int hIn, hOut;
    pid_t Pid;
    TString name;
    MArray<char> buff;
};
#endif
#endif /* TRACE_H_ */
#endif
