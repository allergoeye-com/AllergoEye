
#ifndef EOSAPP_H_
#define EOSAPP_H_

#include "minstance.h"
#include "Java/AObject.h"
#include "WndImageView.h"

#define STR_GLOBALREF "lib.utils.GlobalRef"
#define STR_APP_IDLE "lib.common.EventIdle"
#define STR_CVIEW "com.dryeye.app.CView"
#define STR_CONTEXT "android.content.Context"
#define STR_ADDSECTLIST "com.dryeye.app.roi.SelectedBlockIcon"
#define STR_AGLOBALS "lib.utils.AGlobals"


class CApp : public MInstance {
public:
    CApp();

    virtual ~CApp();

    virtual BOOL InitInstance();

    virtual BOOL ExitInstance();

    virtual void HandleMessages()
    {}

    MHashMap<AObject *, jobject> globals;

    jobject CreateJavaObject(JNIEnv *env, jclass cls)
    {
        return CreateJavaObject(GetNameJClass(env, cls));

    }

    void ReleaseInstance(jobject obj);

    jobject CreateJavaObject(const TString &name);

    AObject *Lookup(jobject o)
    { return globals.Lookup(o) ? globals[o] : 0; }

    void RunAndroidIdle(int iMilisecond);

    void RemoveAndroidIdle();

    void InitAndroidIdle(jobject action);

    WndImageView *Dlg;

    jobject CreateJavaWindow(JNIEnv *env, jclass cls, jobject context, int w = 0, int h = 0);
    virtual METRICS *GetMetrics();;


private:
    AObject androidIdle;
    AObject AGlobals;


};

extern CApp *theApp;


#endif /* EOSAPP_H_ */
