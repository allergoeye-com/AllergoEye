//
// Created by alex on 06.12.17.
//

#ifndef FUTURA_JBITMAP_H
#define FUTURA_JBITMAP_H

#ifndef WIN32

#include "Java/AObject.h"

#else

class AObject {
public:
    AObject() {}
    virtual ~AObject() {}
    void Clear() {}
    enum { TJSTATIC = 102, TJDEFAULT = 101, TJLOCAL = 100, TJGLOBAL = 101 };

};
typedef AObject * JOBJECT;

#endif
#pragma pack(push)
#pragma pack(4)
EXPORTNIX class JBitmap : protected AObject {
public:
    enum TYPE {
        TJEMPTY = -1, TJMEMORY = TJSTATIC + 1
    };

    JBitmap()
    {
        type = TJEMPTY;
        fAccesCount = 0;
        type = TJEMPTY;
        m_data = 0;
    }

    virtual ~JBitmap()
    { Clear(); }

    JBitmap(int x, int y, int _type = TJLOCAL)
    {
        type = TJEMPTY, fAccesCount = 0;
        Create(x, y, _type);
    }

#ifndef WIN32

    BOOL Create(jobject bmp, int _type = TJLOCAL);

    bool Load(LPCSTR path);

    bool Save(LPCSTR path, LPCSTR type = "PNG");

    static bool Save(MemImage &img, LPCSTR path, LPCSTR type = "PNG");

    bool IsValid()
    { return AObject::IsValid(); }
#else
    bool IsValid()
    {
            return data.GetLen() > 0;
    }

#endif

    bool GetData(MArray<DWORD> &data, ImageInfo &info);

    BOOL Create(DWORD *bmp, int x, int y, int _type = TJLOCAL)
    {
        Clear();
        return _Create(bmp, x, y, _type);
    }

    BOOL Create(int x, int y, int _type = TJLOCAL);

    BOOL Create(JBitmap &bmp);

    BOOL Create(ABmp &bmp);

    BOOL Create(ABmp *bmp)
    { return bmp ? Create(*bmp) : 0; }

    int Type() const
    { return type; }

    void ResetAccess();

    DWORD *SetAccess();

    BOOL ReleaseAccess();

    BOOL GetInfo(ImageInfo &info);

    void Clear(bool fRecycle = true);

#ifndef WIN32

    BOOL Attach(jobject bmp, int type = TJDEFAULT);

    jobject Detach();

    jobject Get_jobject()
    {
        ResetAccess();
        return obj;
    }

    virtual BOOL Run(const char *name, JVariable *v, int n, JVariable &res, JNIEnv *env = 0);

#endif

    BOOL Resize(int x, int y);

    int Width() const
    { return type == TJEMPTY ? 0 : width; }

    int Height() const
    { return type == TJEMPTY ? 0 : height; }

private:
    BOOL _Create(DWORD *m_data, int x, int y, int tp);

    BOOL _Create(int x, int y, int _type);

private:
    int width;
    int height;
    int type;
    int fAccesCount;
    MArray<DWORD> data;
    DWORD *m_data;
};
#pragma pack(pop)
typedef JBitmap *JBITMAP;
#endif //FUTURA_JBITMAP_H
