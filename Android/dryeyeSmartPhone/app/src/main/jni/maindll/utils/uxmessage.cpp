#include "stdafx.h"
#include "uxmessage.h"
#include "mpthread.h"
#ifndef ANDROID_NDK

#ifdef __PLUK_LINUX__
//-------------------------------------------
//
//-------------------------------------------
void UXMessage::MsgFunc(int signum, siginfo_t *info, void *context)
{
    char buff[1024];
    int l;
    if (signum == SIG_UXMESSAGE)
    {
         if (info->si_value.sival_ptr && ((info->si_value.sival_int >> 16) & 0xFFFF))
         {
            UXMessage *self = (UXMessage *)info->si_value.sival_ptr;
         
            if (l = self->Receive(buff, 1023, getpid()))
                    self->handler(buff,  l, info->si_pid);
            MPThread::Startup *startup = (MPThread::Startup *)TlsGetValue(MPThread::dwStartupPtr);

            self->SetNoify(startup->self->ProcSignal);
         }
    }
}
//-----------------------------------------------------
//
//-----------------------------------------------------
bool UXMessage::SetAttrib(int _iMsgNum, int _iSize)
{
        if (msgId != INVALID_MQ)
        {
            mq_attr attr, old;
            memset(&attr, 0, sizeof(mq_attr));
            StdError(mq_getattr (msgId, &attr));
            attr.mq_maxmsg = _iMsgNum;
            attr.mq_msgsize = _iSize;

            StdError(mq_setattr (msgId,
		       &attr,
		       &old));
            StdError(mq_getattr (msgId, &attr));
            iMsgNum = attr.mq_maxmsg;
            iSize = attr.mq_msgsize;
            return 1;
        }
        return false;
    
}
//-------------------------------------------
//
//-------------------------------------------
bool UXMessage::Create(const char *_name, UXMessage::Func _handler)
{
    Close();
    if (name) HFREE(name);
    name = (char *)HMALLOC(strlen(_name) + 1);
    strcpy(name, _name);
     if ((msgId = mq_open(name, O_RDWR | O_NONBLOCK, S_IRWXU, 0)) != INVALID_MQ)
     {
         fCreate = true;
         Close();
     }
    msgId = mq_open(name, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU, 0);
      if (msgId != INVALID_MQ)
        {
            mq_attr attr;
            memset(&attr, 0, sizeof(mq_attr));
            StdError(mq_getattr (msgId, &attr));
            iMsgNum = attr.mq_maxmsg;
            iSize = attr.mq_msgsize;
            handler = _handler;

        }
        else
        {
                StdError(-1);
                mq_unlink(name);
        }
        return msgId != INVALID_MQ;
}

//-------------------------------------------
//
//-------------------------------------------
bool UXMessage::Open(const char *_name, UXMessage::Func _handler) 
{
    Close();
    if (name) HFREE(name);
    name = (char *)HMALLOC(strlen(_name) + 1); 
    strcpy(name, _name);	
        if ((msgId = mq_open(name, O_RDWR | O_NONBLOCK, S_IRWXU, 0)) != INVALID_MQ)
        {
            mq_attr attr;
            memset(&attr, 0, sizeof(mq_attr));
            StdError(mq_getattr (msgId, &attr));
            iMsgNum = attr.mq_maxmsg;
            iSize = attr.mq_msgsize;
            handler = _handler;

        }
        else
                StdError(-1);
        return msgId != INVALID_MQ;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
void UXMessage::SetNoify(UXSignal &mSignal)
{
    if (msgId != INVALID_MQ) 
    {
        mSignal.Add(SIGUSR1);
        sigevent sig;
        memset(&sig, 0, sizeof(sig)); 
        sig.sigev_notify = SIGEV_SIGNAL;
        sig.sigev_signo  = SIG_UXMESSAGE;
        sig.sigev_value.sival_ptr = (void *)this;
        mSignal.SetHandler(MsgFunc, SIGUSR1);
        StdError(mq_notify(msgId, &sig));
    }
}
//-------------------------------------------
//
//-------------------------------------------
void UXMessage::Close()
{
        if (msgId != INVALID_MQ) 
        {
        	StdError(mq_close( msgId));
            if (fCreate)
        	StdError(mq_unlink(name));
        }
        msgId = INVALID_MQ;
}
//-------------------------------------------
//
//-------------------------------------------
int UXMessage::Send(void* buf, int size, pid_t type)
{
    mq_attr attr;
   MArray<BYTE> b;
    if (msgId != INVALID_MQ && !StdError(mq_getattr(msgId,&attr)))
    {
		if (size != attr.mq_msgsize)
		{
			b.SetLen(attr.mq_msgsize);
			memcpy(b.GetPtr(), buf, size);
			buf = b.GetPtr();
		}
		return msgId != INVALID_MQ ? StdError(mq_send(msgId,(char*)buf, attr.mq_msgsize, type)) == 0 : 0;
	}
   return 0;
}
//-------------------------------------------
//
//-------------------------------------------
int UXMessage::Receive( void* buf, int size, pid_t type) 
{
        mq_attr attr;
        int len = 0;
        if (msgId != INVALID_MQ && !StdError(mq_getattr(msgId,&attr)))
        {
                if (attr.mq_curmsgs)
                { // num messages
                    MArray<char> b(attr.mq_msgsize);

						unsigned rtype;
                        len = StdError(mq_receive(msgId,b.GetPtr(), attr.mq_msgsize, &rtype));
                        if (len > 0)
                        {
							if (type && rtype && type != rtype)
							{

								StdError(mq_send(msgId, b.GetPtr(), attr.mq_msgsize, rtype ));
									len = 0;

							}
							else
							memcpy(buf, b.GetPtr(), min((int)size, (int)attr.mq_msgsize));
						}
						else
						{
							len = 0;
						}
}
        }
        return len;
}
#endif
#endif
