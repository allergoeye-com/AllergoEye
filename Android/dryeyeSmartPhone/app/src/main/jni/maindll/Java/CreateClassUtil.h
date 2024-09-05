#ifndef __CreateClassUtil__
#define __CreateClassUtil__

#include "JVariable.h"

#pragma pack (push)
#pragma pack (4)

EXPORTNIX struct CreateClassUtil {
    jmethodID clazz_getConstructors;
    jmethodID clazz_getDeclaredMethods;
    jmethodID clazz_getName;
    jmethodID clazz_getSuperclass;
    jmethodID clazz_NewInstance;
    jmethodID clazz_getDeclaredField;
    jmethodID clazz_toString;


    jmethodID constructor_getParameterTypes;
    jmethodID constructor_getModifiers;
    jmethodID constructor_getName;
    jmethodID constructor_toString;

    jmethodID method_getModifiers;
    jmethodID method_getParameterTypes;
    jmethodID method_getReturnType;
    jmethodID method_getName;
    jmethodID method_toString;

    jmethodID field_getModifiers;
    jmethodID field_getName;
    jmethodID field_getType;
    jmethodID field_toString;
    JPtr Modifier;
    jmethodID Modifier_isPublic;
    jmethodID Modifier_isStatic;

    JNIEnv *env;
    BOOL fValid;

    CreateClassUtil(JNIEnv *env);

    ~CreateClassUtil();
};
#pragma pack (pop)

#endif

