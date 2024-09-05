#include "stdafx.h"
#include "JVariable.h"

void JPtr::Clear()
{
    if (obj)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        env->DeleteLocalRef(obj);
        MInstance::ClearException(env);

    }
    obj = 0;
}

JPtr::JPtr(const JPtr &_obj)
{
    obj = 0;
    if (_obj.obj)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        obj = env->NewLocalRef(_obj.obj);
    }
}

JPtr &JPtr::operator=(const JPtr &_obj)
{
    Clear();
    if (_obj.obj)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        obj = env->NewLocalRef(_obj.obj);
    }
    return *this;
}

TString JPtr::String() const
{
    TString name;
    if (obj)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        const char *jnamestr = env->GetStringUTFChars(str, NULL);
        jsize len = env->GetStringUTFLength(str);
        const char *c = jnamestr;
        while (len--)
            name.Add() = *c++;
        name.Add() = 0;
        env->ReleaseStringUTFChars(str, jnamestr);
    }
    return name;

}

WString JPtr::wString() const
{
    WString name;
    if (obj)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        jsize len = env->GetStringLength(str);

        const jchar *jnamestr = env->GetStringChars(str, NULL);
        const jchar *c = jnamestr;

        while (len--)
        {
            name.Add() = (wchar_t) *c;
            ++c;
        }
        name.Add() = 0;
        env->ReleaseStringChars(str, jnamestr);
    }

    return name;

}
MArray<jchar>  JPtr::w2String() const
{
    MArray<jchar> name;
    if (obj)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        jsize len = env->GetStringLength(str);
        const jchar *jnamestr = env->GetStringChars(str, NULL);
        name.Copy(jnamestr, len);
        name.Add() = 0;
        env->ReleaseStringChars(str, jnamestr);
    }
    return name;
}
bool JPtr::W2String(const char *src, MArray<jchar> &name)
{
    JNIEnv *env = MInstance::GetLocalJNI();
    jstring str = env->NewStringUTF(src);
    if (!MInstance::ClearException(env))
    {
        if (str)
        {
            JNIEnv *env = MInstance::GetLocalJNI();
            jsize len = env->GetStringLength(str);
            const jchar *jnamestr = env->GetStringChars(str, NULL);
            name.Copy(jnamestr, len);
            name.Add() = 0;
            env->ReleaseStringChars(str, jnamestr);
            env->DeleteLocalRef(str);
            return true;
        }
    }
    return 0;
}

bool JVariable::MakeWString(jchar *b, int l)
{
    Clear();
    if (b)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        str = env->NewString(b, l);
        if (MInstance::ClearException(env))
            return 0;
    }
    TString t = "Ljava.lang.String;";
    JSpec::Init2(t);
    return type == TJSTRING;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(const char *p)
{
    Clear();
    if (p)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        str = env->NewStringUTF(p);
        if (MInstance::ClearException(env))
            return 0;
    }
    TString t = "Ljava.lang.String;";
    JSpec::Init2(t);
    return type == TJSTRING;
}
bool JVariable::Set(const wchar_t *p)
{
    if (p)
    {
        int i = wcslen(p);
        JNIEnv *env = MInstance::GetLocalJNI();
        MArray<jchar> wstr(i);
        for (int j = 0; j < i; ++j)
            wstr[j] = (jchar)p[j];

        str = env->NewString(wstr.GetPtr(), i);
        if (MInstance::ClearException(env))
            return 0;
    }
    TString t = "Ljava.lang.String;";
    JSpec::Init2(t);
    return type == TJSTRING;

}
void JVariable::ToGlobal()
{
    JNIEnv *env = MInstance::GetLocalJNI();
    if (nVect || type == TJSTRING || type == TJOBJECT)
    {
        if (obj)
        {
            jobject tmp = obj;
            obj = env->NewGlobalRef(tmp);
            if (!fObjIsGlobal)
                env->DeleteLocalRef(tmp);
            else
                env->DeleteGlobalRef(tmp);
            fObjIsGlobal = true;
        }
    }

}

//--------------------------------------
//
//--------------------------------------
void JVariable::Clear()
{
    JNIEnv *env = MInstance::GetLocalJNI();
    if (nVect)
    {
        if (obj)
        {
            if (!fObjIsGlobal)
                env->DeleteLocalRef(obj);
            else
                env->DeleteGlobalRef(obj);
        }
    } else
    {
        switch (type)
        {
            case TJEMPTY:
            case TJINT:
            case TJFLOAT:
            case TJSHORT:
            case TJBYTE:
            case TJCHAR:
            case TJBOOL:
            case TJDOUBLE:
            case TJLONG:
                break;
            case TJSTRING:
            case TJOBJECT:
                if (obj)
                {
                    if (!fObjIsGlobal)
                        env->DeleteLocalRef(obj);
                    else
                        env->DeleteGlobalRef(obj);
                }
                break;
            default:
                break;

        }
    }
    obj = 0;
    fObjIsGlobal = false;
    JSpec::Clear();
}

BOOL JVariable::AddTo(LPCSTR name, TStruct &p)
{
    if (nVect) return 0;

//	JNIEnv *env = MInstance::GetLocalJNI();
    switch (type)
    {
        case TJDOUBLE:
            p.put(name, value.d);
            break;
        case TJFLOAT:
            p.put(name, value.f);
            break;
        case TJINT:
            p.put(name, value.i);
            break;
        case TJSHORT:
            p.put(name, value.s);
            break;
        case TJBYTE:
            p.put(name, value.b);
            break;
        case TJCHAR:
            p.put(name, value.c);
            break;
        case TJBOOL:
            p.put(name, value.z);
            break;
        case TJLONG:
            p.put(name, value.j);
            break;
        case TJSTRING:
        {
            TString s = *this;
            p.put(name, s.GetPtr());
            break;
        }
        default:
            return 0;
    }
    return TRUE;
}

JVariable::JVariable(const JVariable &r)
{
    nVect = 0;
    type = TJEMPTY;
    nVect = r.nVect;
    switch (r.type)
    {
        case TJINT:
        case TJFLOAT:
        case TJSHORT:
        case TJBYTE:
        case TJCHAR:
        case TJBOOL:
        case TJDOUBLE:
        case TJLONG:
            if (!nVect)
                memcpy(&value.d, &r.value.d, sizeof(jdouble));
            break;
        case TJSTRING:
        case TJOBJECT:
            if (r.obj)
                Copy(r.obj);
            break;
        default:
            break;
    }

}

//--------------------------------------
//
//--------------------------------------
JVariable::operator double() const
{
    switch (type)
    {
        case TJDOUBLE:
            return value.d;
        case TJFLOAT:
            return (double) value.f;
        case TJINT:
            return (double) value.i;
        case TJSHORT:
            return (double) value.s;
        case TJBYTE:
            return (double) value.b;
        case TJCHAR:
            return (double) value.c;
        case TJBOOL:
            return (double) value.z;
        case TJLONG:
            return (double) value.j;
        default:
            break;
    }
    return 0.0;
}

//--------------------------------------
//
//--------------------------------------
JVariable::operator INT64() const
{
    switch (type)
    {
        case TJDOUBLE:
            return (INT64) value.d;
        case TJFLOAT:
            return (INT64) value.f;
        case TJINT:
            return (INT64) value.i;
        case TJSHORT:
            return (INT64) value.s;
        case TJBYTE:
            return (INT64) value.b;
        case TJCHAR:
            return (INT64) value.c;
        case TJBOOL:
            return (INT64) value.z;
        case TJLONG:
            return (INT64) value.j;
        default:
            break;
    }
    return 0;

}

//--------------------------------------
//
//--------------------------------------
JVariable::operator TString() const
{
    TString name;
    if (type == TJSTRING && str)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        const char *jnamestr = env->GetStringUTFChars(str, NULL);
        name = jnamestr;
        env->ReleaseStringUTFChars(str, jnamestr);
    }
    return name;
}
bool JVariable::Get(TString &name)
{
    name.SetLen(0);
    if (type == TJSTRING && str)
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        const char *jnamestr = env->GetStringUTFChars(str, NULL);
        name = jnamestr;
        env->ReleaseStringUTFChars(str, jnamestr);
    }
    return name.StrLen();
}

//--------------------------------------
//
//--------------------------------------
BOOL JVariable::IsNumber()
{
    switch (type)
    {
        case TJINT:
        case TJFLOAT:
        case TJSHORT:
        case TJBYTE:
        case TJCHAR:
        case TJBOOL:
        case TJDOUBLE:
        case TJLONG:
            return true;
        default:
            break;
    }
    return false;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(double *p, int l)
{
    Clear();
    JNIEnv *env = MInstance::GetLocalJNI();
    if (l)
    {
        if (!p) return 0;
        doublearr = env->NewDoubleArray(l);
        if (MInstance::ClearException(env))
            return 0;
        if (doublearr)
            env->SetDoubleArrayRegion(doublearr, 0, l, p);
        else
            return 0;
    }
    type = TJDOUBLE;
    nVect = 1;
    return type != TJEMPTY;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(float *p, int l)
{
    Clear();
    JNIEnv *env = MInstance::GetLocalJNI();
    if (l)
    {
        if (!p) return 0;

        floatarr = env->NewFloatArray(l);
        if (MInstance::ClearException(env))
            return 0;
        if (floatarr)
            env->SetFloatArrayRegion(floatarr, 0, l, p);
        else
            return 0;
    }
    type = TJFLOAT;
    nVect = 1;
    return type != TJEMPTY;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(int *p, int l)
{
    Clear();
    if (l)
    {
        if (!p) return 0;
        JNIEnv *env = MInstance::GetLocalJNI();
        intarr = env->NewIntArray(l);
        if (MInstance::ClearException(env))
            return 0;
        if (intarr)
            env->SetIntArrayRegion(intarr, 0, l, (const int *) p);
        else
            return 0;
    }
    type = TJINT;
    nVect = 1;
    return type != TJEMPTY;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(short *p, int l)
{
    Clear();
    if (l)
    {
        if (!p) return 0;
        JNIEnv *env = MInstance::GetLocalJNI();
        shortarr = env->NewShortArray(l);
        if (MInstance::ClearException(env))
            return 0;
        if (intarr)
            env->SetShortArrayRegion(shortarr, 0, l, p);
        else
            return 0;
    }
    type = TJSHORT;
    nVect = 1;
    return type != TJEMPTY;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(INT64 *p, int l)
{
    Clear();
    if (l)
    {
        if (!p) return 0;
        JNIEnv *env = MInstance::GetLocalJNI();
        int64arr = env->NewLongArray(l);
        if (MInstance::ClearException(env))
            return 0;
        if (int64arr)
            env->SetLongArrayRegion(int64arr, 0, l, p);
        else
            return 0;
    }
    type = TJLONG;
    nVect = 1;
    return type != TJEMPTY;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(char *p, int l)
{
    Clear();
    if (l)
    {
        if (!p) return 0;
        JNIEnv *env = MInstance::GetLocalJNI();
        bytearr = env->NewByteArray(l);

        if (MInstance::ClearException(env))
            return 0;
        if (bytearr)
            env->SetByteArrayRegion(bytearr, 0, l, (jbyte *) p);
        else
            return 0;
    }
    type = TJBYTE;
    nVect = 1;

    return type != TJEMPTY;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(BYTE *p, int l)
{
    return Set((char *) p, l);
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(bool *p, int l)
{
    Clear();
    if (l)
    {
        if (!p) return 0;
        JNIEnv *env = MInstance::GetLocalJNI();
        boolarr = env->NewBooleanArray(l);
        if (MInstance::ClearException(env))
            return 0;
        MArray<jboolean> b(l);
        for (int i = 0; i < l; ++i)
            b[i] = p[i];
        if (boolarr)
            env->SetBooleanArrayRegion(boolarr, 0, l, b.GetPtr());

    }
    type = TJBOOL;
    nVect = 1;
    return type != TJEMPTY;

}

bool JVariable::Set(jobject *p, int l, LPCSTR spec)
{
    Clear();
    if (l)
    {
        if (!p) return 0;
        JNIEnv *env = MInstance::GetLocalJNI();
        JPtr clazz = MInstance::FindClass(spec);
        if (MInstance::ClearException(env))
            return 0;
        arr = env->NewObjectArray(l, clazz.cls, 0);
        if (MInstance::ClearException(env))
            return 0;
        if (arr)
        {
            int i;
            for (i = 0; i < l; ++i)
            {
                env->SetObjectArrayElement(arr, i, p[i]);
                if (MInstance::ClearException(env))
                    break;
            }
            if (l != i) return false;

        }
    }
    type = TJOBJECT;
    nVect = 1;
    name = (char *) HMALLOCZ(strlen(spec) + 1);
    strcpy(name, spec);

    return type != TJEMPTY;

}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Set(jobject *p, int l)
{
    return Set(p, l, "java/lang/Object");
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MArray<double> &v)
{
    if (type != TJDOUBLE) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    if (arr)
    {
        int i;
        int l = env->GetArrayLength(intarr);
        MInstance::ClearException(env);
        v.SetLen(l);
        if (l)
        {
            double *b = env->GetDoubleArrayElements(doublearr, 0);
            v.SetLen(l);
            double *_v = v.GetPtr();
            for (i = 0; i < l; ++i)
                _v[i] = b[i];
            env->ReleaseDoubleArrayElements(doublearr, b, 0);
        }
    }
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MArray<float> &v)
{
    if (type != TJFLOAT) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    if (arr)
    {
        int i;
        int l = env->GetArrayLength(floatarr);
        MInstance::ClearException(env);
        v.SetLen(l);
        if (l)
        {

            float *b = (float *) env->GetFloatArrayElements(floatarr, 0);
            v.SetLen(l);
            float *_v = v.GetPtr();
            for (i = 0; i < l; ++i)
                _v[i] = b[i];
            env->ReleaseFloatArrayElements(floatarr, b, 0);
        }
    }
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MArray<int> &v)
{
    if (type != TJINT) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    if (arr)
    {
        int i;
        int l = env->GetArrayLength(intarr);
        MInstance::ClearException(env);
        v.SetLen(l);
        if (l)
        {
            int *b = (int *) env->GetIntArrayElements(intarr, 0);
            v.SetLen(l);
            int *_v = v.GetPtr();
            for (i = 0; i < l; ++i)
                _v[i] = b[i];
            env->ReleaseIntArrayElements(intarr, (jint *) b, 0);
        }
    }
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MArray<INT64> &v)
{
    if (type != TJLONG) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    if (arr)
    {
        int i;
        int l = env->GetArrayLength(int64arr);
        MInstance::ClearException(env);
        v.SetLen(l);
        if (l)
        {
            INT64 *b = env->GetLongArrayElements(int64arr, 0);
            v.SetLen(l);
            INT64 *_v = v.GetPtr();
            for (i = 0; i < l; ++i)
                _v[i] = b[i];
            env->ReleaseLongArrayElements(int64arr, b, 0);
        }
    }
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MArray<char> &v)
{
    if (type != TJBYTE) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    if (arr)
    {
        int i;
        int l = env->GetArrayLength(bytearr);
        MInstance::ClearException(env);
        v.SetLen(l);
        if (l)
        {
            char *b = (char *) env->GetByteArrayElements(bytearr, 0);
            char *_v = v.GetPtr();
            for (i = 0; i < l; ++i)
                _v[i] = b[i];
            env->ReleaseByteArrayElements(bytearr, (jbyte *) b, 0);
        }
    }
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MArray<BYTE> &v)
{
    if (type != TJBYTE) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    if (arr)
    {
        int i;
        int l = env->GetArrayLength(bytearr);
        MInstance::ClearException(env);
        v.SetLen(l);
        if (l)
        {
            BYTE *b = (BYTE *) env->GetByteArrayElements(bytearr, 0);
            BYTE *_v = v.GetPtr();
            for (i = 0; i < l; ++i)
                _v[i] = b[i];
            env->ReleaseByteArrayElements(bytearr, (jbyte *) b, 0);
        }
    }
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MArray<bool> &v)
{
    if (type != TJBOOL) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    if (arr)
    {
        int i;
        int l = env->GetArrayLength(boolarr);
        MInstance::ClearException(env);
        v.SetLen(l);
        if (l)
        {
            jboolean *b = env->GetBooleanArrayElements(boolarr, 0);
            bool *_v = v.GetPtr();
            for (i = 0; i < l; ++i)
                _v[i] = b[i] != 0;
            env->ReleaseBooleanArrayElements(boolarr, b, 0);
        }
    }
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MArray<jobject> &v)
{
    if (type != TJOBJECT) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    if (arr)
    {
        int i;
        int l = env->GetArrayLength(arr);
        MInstance::ClearException(env);
        v.SetLen(l);
        if (l)
        {
            for (i = 0; i < l; ++i)
                v[i] = env->GetObjectArrayElement(arr, i);
        }
        return true;
    }
    return false;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Get(MVector<TString> &mList)
{
    mList.Clear();
    if (type != TJSTRING) return 0;
    if (arr)
    {
        int i;
        JNIEnv *env = MInstance::GetLocalJNI();
        int l = env->GetArrayLength(arr);
        MInstance::ClearException(env);

        if (l)
        {
            for (i = 0; i < l; ++i)
            {
                JPtr p = env->GetObjectArrayElement(arr, i);
                mList.Add(p.String());
            }
        }
        return true;
    }
    return false;


}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Copy(jobject p, JSpec *t)
{
    Clear();
    JNIEnv *env = MInstance::GetLocalJNI();
    obj = env->NewLocalRef(p);
    JSpec::Copy(*t);
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Copy(jobject p)
{
    Clear();
    JNIEnv *env = MInstance::GetLocalJNI();
    TString spec = MInstance::GetNameObject(env, p);
    return Copy(p, spec);
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Copy(jobject p, TString &spec)
{
    Clear();
    if (JSpec::Init2(spec))
    {
        JNIEnv *env = MInstance::GetLocalJNI();
        obj = env->NewLocalRef(p);
        return obj != 0;
    }
    return 0;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Attach(jobject p, JSpec *t)
{
    Clear();
    obj = p;
    if (!t)
        return Attach(p);
    JSpec::Copy(*t);
    return 1;
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Attach(jobject p)
{
    Clear();
    JNIEnv *env = MInstance::GetLocalJNI();
    TString spec = MInstance::GetNameObject(env, p);
    return Attach(p, spec);
}

//--------------------------------------
//
//--------------------------------------
bool JVariable::Attach(jobject p, const TString &_spec)
{
    Clear();
    TString spec(_spec);
    if (JSpec::Init2(spec))
    {
        obj = p;
        return 1;
    }
    return 0;
}

bool JVariable::GetSpec(MVector<JVariable> &var, TString &res)
{
    for (int i = 0, l = var.GetLen(); i < l; ++i)
        var[i].FromFunc(res);
    return true;
}


//--------------------------------------
//
//--------------------------------------
int JVariable::GetLen()
{
    if (type == TJEMPTY || !nVect) return 0;
    JNIEnv *env = MInstance::GetLocalJNI();
    int l = env->GetArrayLength(arr);
    MInstance::ClearException(env);
    return l;
}

bool JVariable::Set(MVector<AList::ListEntry> &mList)
{
    Clear();
    JNIEnv *env = MInstance::GetLocalJNI();
    int l = mList.GetLen();
    JPtr tp = env->FindClass("java/lang/String");
    if (MInstance::ClearException(env)) return 0;
    arr = env->NewObjectArray(l, tp.cls, 0);
    if (MInstance::ClearException(env)) return 0;
    for (int i = 0; i < l; ++i)
    {
        JPtr s1 = env->NewStringUTF(mList[i].text.GetPtr());
        env->SetObjectArrayElement(arr, i, s1);
        if (MInstance::ClearException(env)) return 0;
    }
    type = TJSTRING;
    nVect = 1;
    name = (char *) HMALLOCZ(17);
    strcpy(name, "java.lang.String");

    return true;

}

bool JVariable::AttachStringArray(jobjectArray p)
{
    Clear();
    arr = p;
    type = TJSTRING;
    nVect = 1;
    name = (char *) HMALLOCZ(17);
    strcpy(name, "java.lang.String");
    return true;

}

bool JVariable::AttachObjectArray(jobjectArray p, LPCSTR _name)
{
    Clear();
    arr = p;
    type = TJOBJECT;
    nVect = 1;
    name = (char *) HMALLOCZ(strlen(_name) + 1);
    strcpy(name, _name);
    return true;

}

bool JVariable::Set(MVector<TString> &mList)
{
    Clear();

    JNIEnv *env = MInstance::GetLocalJNI();
    int l = mList.GetLen();
    if (l)
    {
        JPtr tp = env->FindClass("java/lang/String");
        if (MInstance::ClearException(env)) return 0;
        arr = env->NewObjectArray(l, tp.cls, 0);
        if (MInstance::ClearException(env)) return 0;
        for (int i = 0; i < l; ++i)
        {
            JPtr s1;
            if (mList[i].StrLen())
                s1 = env->NewStringUTF(mList[i].GetPtr());
            env->SetObjectArrayElement(arr, i, s1);
            if (MInstance::ClearException(env)) return 0;
        }
    }
    type = TJSTRING;
    nVect = 1;
    name = (char *) HMALLOCZ(17);
    strcpy(name, "java.lang.String");
    return true;
}
