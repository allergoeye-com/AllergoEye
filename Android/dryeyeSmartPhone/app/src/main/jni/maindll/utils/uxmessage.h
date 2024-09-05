#ifndef _UXMessage__H_
#define _UXMessage__H_
#ifdef __PLUK_LINUX__
#ifndef ANDROID_NDK
#ifndef INVALID_MQ
#define INVALID_MQ (mqd_t)-1
#endif
#include "mpthread.h"
#include "uxsignal.h"

EXPORTNIX class _PEXPORT UXMessage 
{
public:
        typedef void(*Func)(char *, int, pid_t __pid);
public:

    UXMessage() { msgId = INVALID_MQ; iMsgNum =  iSize = 0; fCreate = false; }
    bool SetAttrib(int _iMsgNum, int _iSize);

    bool Create(const char *_name, Func handler);
    bool Open(const char *_name, Func handler);
    ~UXMessage() { Close(); };
    bool IsOk() { return msgId != INVALID_MQ; };
    void Close();
    int Send(void* buf, int size, pid_t type);
    int Receive( void* buf, int size, pid_t type = getpid());
        static void MsgFunc(int signum, siginfo_t *info, void *context);
        int GetMessageSize() const { return iSize; }
        void SetNoify(UXSignal &mSignal);
 
protected:
    mqd_t msgId;
    int iMsgNum;
    int iSize;
    char *name;
    bool fCreate;
    public:
    Func handler;
};
#endif
#endif
#endif
