/*
 * AObject.h
 *
 *  Created on: Jun 22, 2017
 *      Author: alex
 */

#ifndef MAINDLL_AOBJECT_H_
#define MAINDLL_AOBJECT_H_

#include "AClass.h"
#include "TStruct.h"
#include "JVariable.h"

#pragma pack (push)
#pragma pack (4)
EXPORTNIX class AObject {
public:
    enum _TYPE_ {
        TJDEFAULT,
        TJLOCAL,
        TJGLOBAL,
        TJSTATIC
    };

    AObject();

    virtual ~AObject();

    void Clear();

    bool IsValid()
    { return obj != 0; }

    BOOL Run(const char *name, JNIEnv *env = 0);

    BOOL Run(const char *name, JVariable *v, int n, JNIEnv *env = 0);

    BOOL Run(const char *name, JVariable &res, JNIEnv *env = 0);

    virtual BOOL Run(const char *name, JVariable *v, int n, JVariable &res, JNIEnv *env = 0);

    BOOL New(const char *name, JVariable *v, int n, _TYPE_ type = TJGLOBAL, JNIEnv *env = 0);

    BOOL Attach(jobject o, _TYPE_ type = TJLOCAL, JNIEnv *env = 0);
    BOOL Attach(jobject o, LPCSTR stype, _TYPE_ type = TJLOCAL, JNIEnv *env = 0);

    BOOL Attach(const char *name);

    jobject Detach();

    BOOL GetMembers(TStruct &s, JNIEnv *env = 0);

    const char *Name()
    { return spec ? spec->Name() : 0; }

    virtual bool OnNotify(int cmd, WPARAM wParam, LPARAM lParam)
    { return false; }

    _TYPE_ Type()
    { return fLocal; }

public:
    jobject obj;
    AClass *spec;
private:
    BOOL Invoke(AMember *m, JVariable *v, int n, JVariable &res, JNIEnv *env);

    BOOL InvokeField(AMember *m, JVariable *v, int n, JVariable &res, JNIEnv *env);

protected:
    _TYPE_ fLocal;

};
#pragma pack (pop)
typedef AObject *JOBJECT;
#endif /* MAINDLL_AOBJECT_H_ */
