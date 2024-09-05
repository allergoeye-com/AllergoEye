/*
 * AClass.h
 *
 *  Created on: Jun 14, 2017
 *      Author: alex
 */

#ifndef MAINDLL_ACLASS_H_
#define MAINDLL_ACLASS_H_

#include "ArHashMap.h"
#include "AMember.h"
#include "Java/JVariable.h"
#include "Java/CreateClassUtil.h"

#pragma pack (push)
#pragma pack (4)

EXPORTNIX class AClass : public JSpec {
public:
    AClass();

    virtual ~AClass();

    bool Init(JPtr &cls, CreateClassUtil &util, bool fInitAll = false);

    bool IsValid()
    { return name != 0; }

    const char *Name()
    { return name; }

    AMember *GetMember(const char *pName, const char *pParam, JNIEnv *env = 0);

    ArHashMap<AMember *, TString> &GetMembers()
    { return members; }

private :
    void InitParentClass(CreateClassUtil &util, JNIEnv *env, bool fInitAll);

    void InitFields(JPtr &cls, CreateClassUtil &util);

    void InitFunc(JPtr &cls, CreateClassUtil &util);

    void InitConstructor(jobject cls, CreateClassUtil &util);

    ArHashMap<AMember *, TString> members;
    AClass *parent;

};
#pragma pack (pop)

#endif /* MAINDLL_ACLASS_H_ */
