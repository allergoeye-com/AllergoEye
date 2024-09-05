/*
 * AClass.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: alex
 */

#include "stdafx.h"
#include "minstance.h"
#include "JVariable.h"
#include "AClass.h"
#include "AMember.h"

//---------------------------------
//
//---------------------------------
AClass::AClass()
{
    parent = 0;
}

//---------------------------------
//
//---------------------------------
AClass::~AClass()
{
}

bool AClass::Init(JPtr &cls, CreateClassUtil &util, bool fInitAll)
{
    JNIEnv *env = util.env;
    JPtr str;
    str = env->CallObjectMethod(cls, util.clazz_getName);
    if (MInstance::ClearException(env, 0)) return 0;
    if (!(bool) str) return 0;
    TString _name = str.String();
    name = (char *) HMALLOCZ(_name.GetLen());
    strcpy(name, _name.GetPtr());
    type = TJOBJECT;
    InitConstructor(cls, util);

    InitFields(cls, util);
    InitFunc(cls, util);
    if (fInitAll)
        InitParentClass(util, env, fInitAll);
    return 1;
}

void AClass::InitParentClass(CreateClassUtil &util, JNIEnv *env, bool fInitAll)
{
    if (!parent)
    {
        TString parent_name;
        TString tmp = name;
        tmp.ReplaceAll(".", "/");
        JPtr obj = env->FindClass(tmp.GetPtr());
        if (MInstance::ClearException(env, 0)) return;
        JPtr str, b = env->CallObjectMethod(obj.obj, util.clazz_getSuperclass);
        if (MInstance::ClearException(env, 0)) return;
        str = env->CallObjectMethod(b.cls, util.clazz_getName);
        if (MInstance::ClearException(env, 0)) return;
        parent_name = str.String();
        if (parent_name != "java.lang.Object")
        {
            if (!MInstance::InitSpec(parent_name, fInitAll))
                parent_name = "java.lang.Object";
        }
        if (parent_name != "java.lang.Object")
        {
            tmp = parent_name;
            tmp.ReplaceAll(".", "/");
            obj = env->FindClass(tmp.GetPtr());
            if (MInstance::ClearException(env, 0) || !obj.obj) return;
            parent = new AClass();
            parent->Init(obj, util, fInitAll);
            if (parent->IsValid())
            {
                parent->type = TJOBJECT;
                MInstance::AddAClass(parent_name, parent);
            } else
            {
                delete parent;
                parent = 0;
            }

        }
    }
}

//---------------------------------
//
//---------------------------------
AMember *AClass::GetMember(const char *pName, const char *pParam, JNIEnv *env)
{
    AMember *func = 0;
    TString str;
    str.Format("%s/r", pName);
    if (pParam) str += pParam;

    if (!members.Lookup(str.GetPtr(), func))
    {
        if (!parent && strcmp(name, "java.lang.Object"))
        {
            TString parent_name;
            CreateClassUtil util(env ? env : MInstance::GetLocalJNI());
            env = util.env;
            TString tmp = name;
            tmp.ReplaceAll(".", "/");
            JPtr obj = env->FindClass(tmp.GetPtr());
            if (MInstance::ClearException(env, 0)) return 0;
            JPtr str, b = env->CallObjectMethod(obj.obj, util.clazz_getSuperclass);
            if (MInstance::ClearException(env, 0)) return 0;
            str = env->CallObjectMethod(b.cls, util.clazz_getName);
            if (MInstance::ClearException(env, 0)) return 0;
            parent_name = str.String();
            if (parent_name != "java.lang.Object")
            {
                if (!MInstance::InitSpec(parent_name))
                    parent_name = "java.lang.Object";
            }
            if (parent_name != "java.lang.Object")
            {
                tmp = parent_name;
                tmp.ReplaceAll(".", "/");
                obj = env->FindClass(tmp.GetPtr());
                if (MInstance::ClearException(env, 0)) return 0;
                parent = new AClass();
                parent->Init(obj, util);
                if (parent->IsValid())
                {
                    parent->type = TJOBJECT;
                    MInstance::AddAClass(parent_name, parent);
                } else
                {
                    delete parent;
                    parent = 0;
                }

                if (parent)
                    return parent->GetMember(pName, pParam, env);
            }
        } else if (parent)
            return parent->GetMember(pName, pParam, env);

    }
    return func;
}

//----------------------------------------------
//
//----------------------------------------------
void AClass::InitConstructor(jobject cls, CreateClassUtil &util)
{
    JNIEnv *env = util.env;
    JPtr arr;
    arr.obj = env->CallObjectMethod(cls, util.clazz_getConstructors);
    if (MInstance::ClearException(env, 0)) return;
    TString cls_name = MInstance::GetNameObject(util.env, arr.obj);
    int N = arr.obj ? env->GetArrayLength(arr) : 0;
    MInstance::ClearException(env, 0);
    if (N)
    {

        for (int i = 0; i < N; ++i)
        {
            TString decl_java;
            JPtr method = env->GetObjectArrayElement(arr, i);
            if (MInstance::ClearException(env, 0)) continue;
            JPtr str = env->CallObjectMethod(method, util.constructor_toString);
            if (MInstance::ClearException(env, 0)) continue;
            decl_java = str.String();
            if (decl_java.Find('@') != -1 || decl_java.Find(name) == -1)
                decl_java = "";
            if (!decl_java.GetLen())
            {
                continue;
            }

            int modifiers = env->CallIntMethod(method, util.constructor_getModifiers);
            if (MInstance::ClearException(env, 0)) continue;
            int fIsPublic = env->CallStaticBooleanMethod(util.Modifier, util.Modifier_isPublic,
                                                         modifiers);
            if (MInstance::ClearException(env, 0)) continue;
            int fIsStatic = env->CallStaticBooleanMethod(util.Modifier, util.Modifier_isStatic,
                                                         modifiers);
            if (MInstance::ClearException(env, 0)) continue;
            if (!fIsPublic || fIsStatic)
                continue;

            JPtr types = (jobjectArray) env->CallObjectMethod(method,
                                                              util.constructor_getParameterTypes);
            if (MInstance::ClearException(env, 0)) continue;

            int L = (bool) types ? env->GetArrayLength(types) : 0;
            if (1)
            {
                MVector<TString> prm(L);
                BOOL fFindParam = true;
                for (int j = 0; j < L; ++j)
                {
                    JPtr type = env->GetObjectArrayElement(types, j);
                    if (MInstance::ClearException(env, 0))
                    {
                        fFindParam = false;
                        break;
                    }
                    //TODO str  = env->CallObjectMethod(type, util.type_getTypeName);

                    /*JPtr type_cls = env->CallObjectMethod(type, util.object_getClass);
                    if (MInstance::ClearException(env, 0))
                    {
                        fFindParam = false;
                        break;
                    }*/
                    str = env->CallObjectMethod(type, util.clazz_getName);
                    if (MInstance::ClearException(env, 0))
                    {
                        fFindParam = false;
                        break;
                    }

                    prm[j] = str.String();
                }
                if (!fFindParam) continue;
                BOOL fError = 0;
                AMember *b = new AMember(name, 0, prm.GetPtr(), (int) prm.GetLen(), fError);
                if (!fError)
                {
                    b->fConstructor = 1;
                    TString params;
                    b->GetFuncSpec(params);
                    b->id = env->GetMethodID((jclass) cls, "<init>", params.GetPtr());
                    if (MInstance::ClearException(env, 0))
                        delete b;
                    else
                    {
                        TString metod_name = "<init>/r";
                        for (int k = 0; k < b->nParam; ++k)
                            b->params[k].FromFunc(metod_name);
                        members[metod_name] = b;
                    }
                } else
                    delete b;
            }
        }
    }

}

//----------------------------------------------
//
//----------------------------------------------
void AClass::InitFunc(JPtr &cls, CreateClassUtil &util)
{
    JNIEnv *env = util.env;
    JPtr arr = env->CallObjectMethod(cls, util.clazz_getDeclaredMethods);
    if (MInstance::ClearException(env, 0)) return;
    int N = (bool) arr ? env->GetArrayLength(arr) : 0;
    if (N)
    {
        TString decl_java;
        for (int i = 0; i < N; ++i)
        {
            JPtr method = env->GetObjectArrayElement(arr, i);
            if (MInstance::ClearException(env, 0)) continue;
            int modifiers = env->CallIntMethod(method, util.method_getModifiers);
            if (MInstance::ClearException(env, 0)) continue;
            int fIsPublic = env->CallStaticBooleanMethod(util.Modifier, util.Modifier_isPublic,
                                                         modifiers);
            if (MInstance::ClearException(env, 0)) continue;
            int fIsStatic = env->CallStaticBooleanMethod(util.Modifier, util.Modifier_isStatic,
                                                         modifiers);
            if (MInstance::ClearException(env, 0)) continue;
            if (!fIsPublic)
                continue;

            JPtr types = (jobjectArray) env->CallObjectMethod(method,
                                                              util.method_getParameterTypes);
            if (MInstance::ClearException(env, 0)) continue;
            int L = (bool) types ? env->GetArrayLength(types) : 0;
            MVector<TString> prm(L);
            JPtr str = (jstring) env->CallObjectMethod(method, util.method_getName);
            if (MInstance::ClearException(env, 0)) continue;
            TString metod_name = str.String();
            str = (jstring) env->CallObjectMethod(method, util.method_toString);
            if (MInstance::ClearException(env, 0)) continue;
            decl_java = str.String();
            //	if (decl_java.Find("abstract ") != -1)
            //		continue;
            BOOL fFindParam = true;
            for (int j = 0; j < L; ++j)
            {
                JPtr type = env->GetObjectArrayElement(types, j);
                if (MInstance::ClearException(env, 0))
                {
                    fFindParam = false;
                    break;
                }
                //TODO str  = env->CallObjectMethod(type, util.type_getTypeName);
                str = env->CallObjectMethod(type, util.clazz_getName);
                if (MInstance::ClearException(env, 0))
                {
                    fFindParam = false;
                    break;
                }
                prm[j] = str.String();
            }
            if (!fFindParam) continue;

            JPtr type = env->CallObjectMethod(method, util.method_getReturnType);
            if (MInstance::ClearException(env, 0)) continue;
            //TODO str  = env->CallObjectMethod(type, util.type_getTypeName);
            str = env->CallObjectMethod(type, util.clazz_getName);
            if (MInstance::ClearException(env, 0))
            {
                fFindParam = false;
                break;
            }

            if (MInstance::ClearException(env, 0)) continue;
            TString rt = str.String();
            BOOL fError = 0;
            AMember *b = new AMember(metod_name.GetPtr(), rt.GetPtr(), prm.GetPtr(),
                                     (int) prm.GetLen(), fError);
            if (!fError)
            {
                TString params;
                b->GetFuncSpec(params);
                if ((b->fStatic = fIsStatic))
                    b->id = env->GetStaticMethodID((jclass) cls, metod_name.GetPtr(),
                                                   params.GetPtr());
                else
                    b->id = env->GetMethodID((jclass) cls, metod_name.GetPtr(), params.GetPtr());
                if (MInstance::ClearException(env, 0))
                    delete b;
                else
                {

                    metod_name += "/r";
                    for (int i = 0; i < b->nParam; ++i)
                        b->params[i].FromFunc(metod_name);
                    members[metod_name] = b;

                }

            } else
                delete b;
        }
    }

}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void AClass::InitFields(JPtr &cls, CreateClassUtil &util)
{
    JNIEnv *env = util.env;
    JPtr arr = env->CallObjectMethod(cls, util.clazz_getDeclaredField);
    if (MInstance::ClearException(env, 0)) return;
    int N = (bool) arr ? env->GetArrayLength(arr) : 0;
    if (N)
    {
        for (int i = 0; i < N; ++i)
        {
            JPtr field = env->GetObjectArrayElement(arr, i);
            if (MInstance::ClearException(env, 0)) continue;
            int modifiers = env->CallIntMethod(field, util.field_getModifiers);
            if (MInstance::ClearException(env, 0)) continue;
            int fIsPublic = env->CallStaticBooleanMethod(util.Modifier, util.Modifier_isPublic,
                                                         modifiers);
            if (MInstance::ClearException(env, 0)) continue;
            int fIsStatic = env->CallStaticBooleanMethod(util.Modifier, util.Modifier_isStatic,
                                                         modifiers);
            if (MInstance::ClearException(env, 0)) continue;
            if (!fIsPublic)
                continue;
            JPtr type = env->CallObjectMethodA(field, util.field_getType, 0);
            if (MInstance::ClearException(env, 0)) continue;
            JPtr str = env->CallObjectMethod(type, util.clazz_getName);

            if (MInstance::ClearException(env, 0)) continue;
            TString rt = str.String();
            str = env->CallObjectMethod(field, util.field_getName);
            if (MInstance::ClearException(env, 0)) continue;
            TString field_name = str.String();
            BOOL fError = 0;
            str = env->CallObjectMethod(field, util.field_toString);
            if (MInstance::ClearException(env, 0)) continue;
            TString decl_java = str.String();
            AMember *b = new AMember(field_name.GetPtr(), rt.GetPtr(), 0, 0, fError);
            if (!fError)
            {
                TString params;
                b->fField = true;
                b->GetFuncSpec(params);
                if (!fIsStatic)
                    b->fid = env->GetFieldID((jclass) cls, field_name.GetPtr(), params.GetPtr());
                else
                    b->fid = env->GetStaticFieldID((jclass) cls, field_name.GetPtr(),
                                                   params.GetPtr());
                if (MInstance::ClearException(env, 0))
                    delete b;
                else
                {
                    jfieldID fid = b->fid;
                    b->fStatic = fIsStatic;
                    field_name += "/r";
                    members[field_name] = b;
                    b->ret.FromFunc(field_name);

                    AMember *b1 = new AMember(*b);
                    b1->fFieldIn = true;
                    members[field_name] = b1;
                }

            } else
                delete b;
        }
    }
}
