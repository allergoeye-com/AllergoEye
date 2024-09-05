#include "stdafx.h"
#include "CreateClassUtil.h"
#include "minstance.h"
#include "JVariable.h"

//----------------------------------------------
//
//----------------------------------------------
CreateClassUtil::CreateClassUtil(JNIEnv *_env)
{
    memset(this, 0, sizeof(CreateClassUtil));
    env = _env;

    JPtr cls = env->FindClass("java/lang/Class");
    if (MInstance::ClearException(env, 0)) return;
    clazz_getConstructors = env->GetMethodID(cls.cls, "getConstructors",
                                             "()[Ljava/lang/reflect/Constructor;");
    if (MInstance::ClearException(env, 0)) return;
    clazz_getDeclaredMethods = env->GetMethodID(cls.cls, "getDeclaredMethods",
                                                "()[Ljava/lang/reflect/Method;");
    if (MInstance::ClearException(env, 0)) return;
    clazz_getDeclaredField = env->GetMethodID(cls.cls, "getDeclaredFields",
                                              "()[Ljava/lang/reflect/Field;");
    if (MInstance::ClearException(env, 0)) return;
    clazz_getName = env->GetMethodID(cls.cls, "getName", "()Ljava/lang/String;");
    if (MInstance::ClearException(env, 0)) return;
    clazz_toString = env->GetMethodID(cls.cls, "toString", "()Ljava/lang/String;");
    if (MInstance::ClearException(env, 0)) return;

    clazz_NewInstance = env->GetMethodID(cls.cls, "newInstance", "()Ljava/lang/Object;");
    if (MInstance::ClearException(env, 0)) return;
    clazz_getSuperclass = env->GetMethodID(cls.cls, "getSuperclass", "()Ljava/lang/Class;");
    if (MInstance::ClearException(env, 0)) return;

    JPtr Constructor = env->FindClass("java/lang/reflect/Constructor");
    if (MInstance::ClearException(env, 0)) return;
    constructor_getParameterTypes = env->GetMethodID(Constructor.cls, "getParameterTypes",
                                                     "()[Ljava/lang/Class;");
    if (MInstance::ClearException(env, 0)) return;
    constructor_getModifiers = env->GetMethodID(Constructor.cls, "getModifiers", "()I");
    if (MInstance::ClearException(env, 0)) return;
    constructor_getName = env->GetMethodID(Constructor.cls, "getName", "()Ljava/lang/String;");
    if (MInstance::ClearException(env, 0)) return;
    constructor_toString = env->GetMethodID(Constructor.cls, "toString", "()Ljava/lang/String;");
    if (MInstance::ClearException(env, 0)) return;

    JPtr field = env->FindClass("java/lang/reflect/Field");
    if (MInstance::ClearException(env, 0)) return;
    field_getModifiers = env->GetMethodID(field.cls, "getModifiers", "()I");
    if (MInstance::ClearException(env, 0)) return;
    field_getName = env->GetMethodID(field.cls, "getName", "()Ljava/lang/String;");
    field_getType = env->GetMethodID(field.cls, "getType", "()Ljava/lang/Class;");
    if (MInstance::ClearException(env, 0)) return;
    field_toString = env->GetMethodID(field.cls, "toString", "()Ljava/lang/String;");
    if (MInstance::ClearException(env, 0)) return;


    JPtr method = env->FindClass("java/lang/reflect/Method");
    if (MInstance::ClearException(env, 0)) return;
    method_getModifiers = env->GetMethodID(method.cls, "getModifiers", "()I");
    if (MInstance::ClearException(env, 0)) return;
    method_getName = env->GetMethodID(method.cls, "getName", "()Ljava/lang/String;");
    if (MInstance::ClearException(env, 0)) return;
    method_toString = env->GetMethodID(method.cls, "toString", "()Ljava/lang/String;");
    if (MInstance::ClearException(env, 0)) return;
    method_getParameterTypes = env->GetMethodID(method.cls, "getParameterTypes",
                                                "()[Ljava/lang/Class;");
    if (MInstance::ClearException(env, 0)) return;
    method_getReturnType = env->GetMethodID(method.cls, "getReturnType", "()Ljava/lang/Class;");
    if (MInstance::ClearException(env, 0)) return;

    Modifier = env->FindClass("java/lang/reflect/Modifier");
    if (MInstance::ClearException(env, 0)) return;
    Modifier_isPublic = env->GetStaticMethodID(Modifier.cls, "isPublic", "(I)Z");
    Modifier_isStatic = env->GetStaticMethodID(Modifier.cls, "isStatic", "(I)Z");
    if (MInstance::ClearException(env, 0)) return;

    //Type = env->FindClass("java/lang/reflect/Type");
    //if (MInstance::ClearException(env, 0)) return;

    //type_getTypeName  = env->GetMethodID(Type, "getTypeName", "()Ljava/lang/String;");
    //if (MInstance::ClearException(env, 0)) return;
    fValid = true;

}

//----------------------------------------------
//
//----------------------------------------------
CreateClassUtil::~CreateClassUtil()
{

}



