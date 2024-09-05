#ifndef __UXSignal_H
#define    __UXSignal_H

#ifdef __PLUK_LINUX__
#define SIG_POSTMESSAGE (SIGRTMAX - 1)
#define SIG_SENDMESSAGE (SIGRTMAX - 2)
#define SIG_POSTMESSAGE_WND (SIGRTMAX - 3)
#define SIG_SENDMESSAGE_WND (SIGRTMAX - 4)

#define SIG_RUN_MY (SIGRTMAX - 5)
#define SIG_SOCK_SEND (SIGRTMAX - 6)

#define SIG_REAL_TIME SIGUSR2

#define SIG_UXMESSAGE SIGUSR1
#pragma pack(push) //NADA
#pragma pack(4)
#ifndef ANDROID_NDK
class UXSignal: public sigset_t {
#else
struct _sigset_t_ {
    sigset_t self;
};

class UXSignal : public _sigset_t_ {
public:
    operator sigset_t() const
    { return self; }

#endif
public:
    typedef void(*Func)(int signum, siginfo_t *info, void *context);

    UXSignal(bool fEmpty = true);

    void Add(int sig);

    void BlockThreads();

    void BlockMainThread();

    bool SetHandler(Func handler, int sig);

    void UnBlockMainThread();

    void UnBlockThreads();

    void Remove(int signum);

    bool Wait(siginfo_t &);

protected:
    sigset_t oldSet;
};

#pragma pack(pop) //NADA

#endif
#endif
