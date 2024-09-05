#ifndef __JVariable__
#define __JVariable__

#include "JType.h"
#include "DList.h"
#include "JSpec.h"
#include "TStruct.h"

#pragma pack(push)
#pragma pack(4)

EXPORTNIX union JPtr {
    JPtr()
    { obj = 0; }

    JPtr(const JPtr &_obj);

    JPtr(const jclass _obj)
    { cls = _obj; }

    JPtr(const jobject _obj)
    { obj = _obj; }

    JPtr(const jstring a)
    { str = a; }

    JPtr(const jobjectArray a)
    { arr = a; }

    JPtr(const jbooleanArray a)
    { boolarr = a; }

    JPtr(const jbyteArray a)
    { bytearr = a; }

    JPtr(const jshortArray a)
    { shortarr = a; }

    JPtr(const jintArray a)
    { intarr = a; }

    JPtr(const jlongArray a)
    { int64arr = a; }

    JPtr(const jfloatArray a)
    { floatarr = a; }

    JPtr(const jdoubleArray a)
    { doublearr = a; }

    JPtr(const jthrowable a)
    { jthrow = a; }

    ~JPtr()
    { Clear(); }

    void Detach()
    { obj = 0; }

    void Clear();

    TString String() const;

    WString wString() const;
    MArray<jchar>  w2String() const;
    static bool W2String(const char *src, MArray<jchar> &dst);

    operator TString() const
    { return String(); }

    operator bool() const
    { return obj != 0; }

    operator jthrowable() const
    { return jthrow; }

    operator jclass() const
    { return cls; }

    operator jobject() const
    { return obj; }

    operator jstring() const
    { return str; }

    operator jobjectArray() const
    { return arr; }

    operator jbooleanArray() const
    { return boolarr; }

    operator jbyteArray() const
    { return bytearr; }

    operator jshortArray() const
    { return shortarr; }

    operator jintArray() const
    { return intarr; }

    operator jlongArray() const
    { return int64arr; }

    operator jfloatArray() const
    { return floatarr; }

    operator jdoubleArray() const
    { return doublearr; }

    operator jarray() const
    { return _arr; }

    JPtr &operator=(const JPtr &_obj);

    JPtr &operator=(const jclass _obj)
    {
        Clear();
        cls = _obj;
        return *this;
    }

    JPtr &operator=(const jobject _obj)
    {
        Clear();
        obj = _obj;
        return *this;
    }

    JPtr &operator=(const jstring a)
    {
        Clear();
        str = a;
        return *this;
    }

    JPtr &operator=(const jobjectArray a)
    {
        Clear();
        arr = a;
        return *this;
    }

    JPtr &operator=(const jbooleanArray a)
    {
        Clear();
        boolarr = a;
        return *this;
    }

    JPtr &operator=(const jbyteArray a)
    {
        Clear();
        bytearr = a;
        return *this;
    }

    JPtr &operator=(const jshortArray a)
    {
        Clear();
        shortarr = a;
        return *this;
    }

    JPtr &operator=(const jintArray a)
    {
        Clear();
        intarr = a;
        return *this;
    }

    JPtr &operator=(const jlongArray a)
    {
        Clear();
        int64arr = a;
        return *this;
    }

    JPtr &operator=(const jfloatArray a)
    {
        Clear();
        floatarr = a;
        return *this;
    }

    JPtr &operator=(const jdoubleArray a)
    {
        Clear();
        doublearr = a;
        return *this;
    }

    JPtr &operator=(const jthrowable a)
    {
        Clear();
        jthrow = a;
        return *this;
    }

    jthrowable jthrow;
    jclass cls;
    jobject obj;
    jstring str;
    jarray _arr;
    jobjectArray arr;
    jbooleanArray boolarr;
    jbyteArray bytearr;
    jshortArray shortarr;
    jintArray intarr;
    jlongArray int64arr;
    jfloatArray floatarr;
    jdoubleArray doublearr;
};
EXPORTNIX class JVariable : public JSpec {
public:
    JVariable()
    {
        obj = 0;
        fObjIsGlobal = false;
    }

    JVariable(double m)
    {
        fObjIsGlobal = false;
        type = TJDOUBLE;
        value.d = m;
    }

    JVariable(float m)
    {
        fObjIsGlobal = false;
        type = TJFLOAT;
        value.f = m;
    }

    JVariable(DWORD m)
    {
        fObjIsGlobal = false;
        type = TJINT;
        value.i = m;
    }

    JVariable(int m)
    {
        fObjIsGlobal = false;
        type = TJINT;
        value.i = m;
    }

    JVariable(USHORT m)
    {
        fObjIsGlobal = false;
        type = TJSHORT;
        value.s = m;
    }

    JVariable(short m)
    {
        fObjIsGlobal = false;
        type = TJSHORT;
        value.s = m;
    }

    JVariable(char m)
    {
        fObjIsGlobal = false;
        type = TJBYTE;
        value.b = m;
    }

    JVariable(BYTE m)
    {
        fObjIsGlobal = false;
        type = TJBYTE;
        value.b = m;
    }

    JVariable(bool m)
    {
        fObjIsGlobal = false;
        type = TJBOOL;
        value.z = m;
    }

    JVariable(INT64 m)
    {
        fObjIsGlobal = false;
        type = TJLONG;
        value.j = m;
    }

    JVariable(const char *m)
    {
        fObjIsGlobal = false;
        type = TJEMPTY;
        Set(m);
    }
    JVariable(const wchar_t *m)
    {
        fObjIsGlobal = false;
        type = TJEMPTY;
        Set(m);
    }

    JVariable(const JVariable &r);

    virtual ~JVariable()
    { Clear(); }

    JVariable &operator=(double m)
    {
        Clear();
        type = TJDOUBLE;
        value.d = m;
        return *this;
    }

    JVariable &operator=(float m)
    {
        Clear();
        type = TJFLOAT;
        value.f = m;
        return *this;
    }

    JVariable &operator=(DWORD m)
    {
        Clear();
        type = TJINT;
        value.i = m;
        return *this;
    }

    JVariable &operator=(int m)
    {
        Clear();
        type = TJINT;
        value.i = m;
        return *this;
    }

    JVariable &operator=(USHORT m)
    {
        Clear();
        type = TJSHORT;
        value.s = m;
        return *this;
    }

    JVariable &operator=(short m)
    {
        Clear();
        type = TJSHORT;
        value.s = m;
        return *this;
    }

    JVariable &operator=(char m)
    {
        Clear();
        type = TJBYTE;
        value.b = m;
        return *this;
    }

    JVariable &operator=(BYTE m)
    {
        Clear();
        type = TJBYTE;
        value.b = m;
        return *this;
    }

    JVariable &operator=(bool m)
    {
        Clear();
        type = TJBOOL;
        value.z = m;
        return *this;
    }

    JVariable &operator=(INT64 m)
    {
        Clear();
        type = TJLONG;
        value.j = m;
        return *this;
    }

    JVariable &operator=(const char *m)
    {
        Clear();
        Set(m);
        return *this;
    }
    JVariable &operator=(const wchar_t *m)
    {
        Clear();
        Set(m);
        return *this;
    }

    JVariable &operator=(jclass obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jobject obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jstring obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jarray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jobjectArray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jbooleanArray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jbyteArray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jshortArray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jintArray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jlongArray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jfloatArray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    JVariable &operator=(jdoubleArray obj)
    {
        Copy((jobject) obj);
        return *this;
    }

    BOOL AddTo(LPCSTR name, TStruct &p);

    operator double() const;

    operator float() const
    { return (float) operator double(); }

    operator DWORD() const
    { return (DWORD) operator INT64(); }

    operator INT64() const;

    operator int() const
    { return (int) operator INT64(); }

    operator short() const
    { return (short) operator INT64(); }

    operator USHORT() const
    { return (USHORT) operator INT64(); }

    operator char() const
    { return (char) operator INT64(); }

    operator BYTE() const
    { return (BYTE) operator INT64(); }

    operator bool() const
    { return (DWORD) operator INT64() != 0; }

    operator TString() const;

    bool Set(const char *p);
    bool Set(const wchar_t *p);

    bool Set(double *p, int l);

    bool Set(float *p, int l);

    bool Set(int *p, int l);

    bool Set(short *p, int l);

    bool Set(INT64 *p, int l);

    bool Set(char *p, int l);

    bool Set(BYTE *p, int l);

    bool Set(bool *p, int l);

    bool Set(jobject *p, int l);

    bool Set(jobject *p, int l, LPCSTR spec);

    bool Attach(jobject p, JSpec *t);

    bool AttachStringArray(jobjectArray p);

    bool AttachObjectArray(jobjectArray p, LPCSTR s);

    bool Attach(jobject p, const TString &spec);

    bool Attach(jobject p);

    bool Copy(jobject p, JSpec *t);

    bool Copy(jobject p, TString &spec);

    bool Copy(jobject p);

    bool MakeWString(jchar *n, int l);

    bool Set(MVector<AList::ListEntry> &mList);

    bool Set(MVector<TString> &mList);

    bool Get(MArray<double> &);

    bool Get(MArray<float> &);

    bool Get(MArray<int> &);

    bool Get(MArray<INT64> &);

    bool Get(MArray<char> &);

    bool Get(MArray<BYTE> &);

    bool Get(MArray<bool> &);

    bool Get(MArray<jobject> &);

    bool Get(MVector<TString> &mList);
    bool Get(TString &name);


    void Clear();

    void ToGlobal();

    BOOL IsNumber();

    int GetLen();

    void Detach()
    {
        obj = 0;
        type = TJEMPTY;
    }

    static bool GetSpec(MVector<JVariable> &var, TString &res);

    union {
        jvalue value;
        jstring str;
        jobject obj;
        jclass cls;
        jobjectArray arr;
        jbooleanArray boolarr;
        jbyteArray bytearr;
        jshortArray shortarr;
        jintArray intarr;
        jlongArray int64arr;
        jfloatArray floatarr;
        jdoubleArray doublearr;
    };
    bool fObjIsGlobal;
};


#pragma pack(pop)
#endif
