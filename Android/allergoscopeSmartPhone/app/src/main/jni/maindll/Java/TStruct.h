//
// Created by alex on 26.11.17.
//

#ifndef FUTURA_TSTRUCT_H
#define FUTURA_TSTRUCT_H

#include "std.h"
#include "buffer.h"
#include "mtree.h"

#pragma pack(push)
#pragma pack(4)

class TStruct {

public:
    enum TYPE {
        MEMPTY, MCHAR, MBYTE, MSHORT, MUSHORT, MINT, MUINT, MINT64, MDOUBLE, MFLOAT, MSTRING
    };

    union RET {
        char mchar;
        BYTE mbyte;
        USHORT mushort;
        short mshort;
        int mint;
        UINT muint;
        INT64 dint;
        float mfloat;
        double mdouble;
        LPCSTR str;
    };

    TStruct()
    { pos = 0; }

    void Attach(BYTE *p, int size, int fadd = 128)
    {
        buff.SetAddedRealSize(fadd);
        if (p) buff.Attach((char *) p, size, true);
        pos = 0;
    }

    BYTE *Detach(int &len)
    {
        BYTE *p = (BYTE *) buff.GetPtr();
        len = buff.GetLen();
        buff.Detach();
        pos = 0;
        return p;
    }

    bool put(LPCSTR name, char ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MCHAR);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, BYTE ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MBYTE);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, USHORT ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MUSHORT);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, short ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MSHORT);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, int ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MINT);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, UINT ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MUINT);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, float ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MFLOAT);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, double ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MDOUBLE);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, INT64 ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MINT64);
        Write(&ch, sizeof(ch));
        return ret;
    }

    bool put(LPCSTR name, LPCSTR ch)
    {
        bool ret = mp.Lookup(name);
        mp[name] = pos;
        WriteType(MSTRING);
        WriteString(ch);
        return ret;
    }

    void ToEnd()
    { pos = buff.GetLen(); }

    int size()
    { return mp.m_iNum; }

    RET get(LPCSTR name)
    {
        RET r;
        bool ret = mp.Lookup(name);
        if (ret)
        {
            TYPE tp = MEMPTY;
            pos = mp[name];
            Read(&tp, 1);
            switch (tp)
            {

                case MINT64:
                    Read(&r.dint, sizeof(r.dint));
                case MCHAR:
                    Read(&r.mchar, sizeof(r.mchar));
                    break;
                case MBYTE:
                    Read(&r.mbyte, sizeof(r.mbyte));
                    break;
                case MSHORT:
                    Read(&r.mshort, sizeof(r.mshort));
                    break;
                case MUSHORT:
                    Read(&r.mushort, sizeof(r.mushort));
                    break;
                case MINT:
                    Read(&r.mint, sizeof(r.mint));
                    break;
                case MUINT:
                    Read(&r.muint, sizeof(r.muint));
                    break;
                case MDOUBLE:
                    Read(&r.mdouble, sizeof(r.mdouble));
                    break;
                case MFLOAT:
                    Read(&r.mfloat, sizeof(r.mfloat));
                    break;
                case MSTRING:
                    r.str = ReadString();
                    break;
                default:
                    r.dint = 0;
            }

        }
        return r;
    }

protected :
    void WriteString(LPCSTR lpsz)
    {
        int len = 0;
        if (lpsz)
            len = strlen(lpsz);
        if (len)
            Write(lpsz, strlen(lpsz));
        char c = '\0';
        Write(&c, 1);
    }

    LPCSTR ReadString()
    {
        LPCSTR ret = (LPCSTR) buff.GetPtr() + pos;
        int count = buff.GetLen() - pos;
        int N = 0;
        for (; ret[N] != 0 && count; --count, ++N);
        pos += N + 1;
        return ret;

    }

    UINT Read(void *lpBuff, int nCount)
    {
        if (pos + (int) nCount > buff.GetLen())
        {
            nCount = buff.GetLen() - pos;
        }
        if (nCount)
            memcpy(lpBuff, buff.GetPtr() + pos, nCount);
        pos += nCount;
        return nCount;
    }

    void WriteType(int tp)
    {
        Write(&tp, 1);
    }

    BOOL Write(const void *lpBuff, int nCount)
    {
        if (pos + (int) nCount > buff.GetLen())
            buff.SetLen(pos + (int) nCount);
        memcpy(buff.GetPtr() + pos, lpBuff, nCount);
        pos += nCount;
        return TRUE;
    }

protected :
    int pos;
    MArray<char> buff;
    MHashMap<int, TString> mp;
};

#pragma pack(pop)

#endif //FUTURA_TSTRUCT_H
