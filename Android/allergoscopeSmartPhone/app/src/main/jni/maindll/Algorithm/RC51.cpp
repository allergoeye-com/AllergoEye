// RC51.cpp: implementation of the RC5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RC51.h"

#define Pw        0xC526A3B9UL
#define Qw        0x2E156A8DUL

class InRC_5 {
public :
    InRC_5(BYTE Key[16]);

    void Encode(DWORD &A, DWORD &B);

    void Decode(DWORD &A, DWORD &B);

private:
    DWORD dwRR(DWORD Value, BYTE Shift);

    DWORD dwRL(DWORD Value, BYTE Shift);

private:
    BYTE m_pKey[16];
    DWORD m_pInerKey[4];
    DWORD m_pExtendKey[34];
};

//===============================================================
//
//===============================================================
InRC_5::InRC_5(BYTE Key[16])
{
    memset(this, 0, sizeof(InRC_5));
    memcpy(m_pKey, Key, 16);
    memcpy(m_pInerKey, m_pKey, 16);
    m_pExtendKey[0] = Pw;
    int i;
    for (i = 1; i < 34; ++i)
        m_pExtendKey[i] = m_pExtendKey[i - 1] + Qw;
    int lim = 3 * 34;
    i = 0;
    DWORD A = 0;
    DWORD B = 0;
    for (int k = 0, j = 0; k < lim; ++k)
    {
        A = m_pExtendKey[i] = dwRL(m_pExtendKey[i] + A + B, 3);
        B = m_pInerKey[j] = dwRL(m_pInerKey[j] + A + B, (BYTE) ((A + B) % 32));
        i = ++i % 34;
        j = ++j % 4;
    }
}

//===============================================================
//
//===============================================================
DWORD InRC_5::dwRR(DWORD Value, BYTE Shift)
{
    for (BYTE i = 0; i < Shift; ++i)
    {
        DWORD flg = Value & 1;
        Value >>= 1;
        if (flg)
            Value |= 0x80000000;
    }
    return Value;
}

//===============================================================
//
//===============================================================
DWORD InRC_5::dwRL(DWORD Value, BYTE Shift)
{
    for (BYTE i = 0; i < Shift; ++i)
    {
        DWORD flg = Value & 0x80000000;
        Value <<= 1;
        if (flg)
            Value |= 1;
    }
    return Value;
}

//===============================================================
//
//===============================================================
void InRC_5::Encode(DWORD &A, DWORD &B)
{
    A += m_pExtendKey[0];
    B += m_pExtendKey[1];
    for (int i = 1; i <= 16; ++i)
    {
        A = dwRL(A ^ B, (BYTE) (B % 32)) + m_pExtendKey[2 * i];
        B = dwRL(A ^ B, (BYTE) (A % 32)) + m_pExtendKey[2 * i + 1];
    }
}

//===============================================================
//
//===============================================================
void InRC_5::Decode(DWORD &A, DWORD &B)
{
    for (int i = 16; i >= 1; --i)
    {
        B = dwRR(B - m_pExtendKey[2 * i + 1], (BYTE) (A % 32)) ^ A;
        A = dwRR(A - m_pExtendKey[2 * i], (BYTE) (B % 32)) ^ B;
    }
    B -= m_pExtendKey[1];
    A -= m_pExtendKey[0];
}

RC5::RC5(BYTE Key[16])
{
    memcpy(m_pKey, Key, 16);
}

RC5::~RC5()
{

}

bool RC5::Encode(MArray<BYTE> &in, MArray<BYTE> &out)
{
    if (!in.GetLen())
        return 0;
    InRC_5 rc5(m_pKey);
    BYTE *cbuff;
    out = in;
    DWORD len = out.GetLen();
//	out.Insert(0, (BYTE *)&len, sizeof (DWORD));
//	len += 4;
//	out.Insert(0, (BYTE *)cRC5, sizeof (DWORD));
    cbuff = out.GetPtr();

    if (len % 8)
    {
        len = (len / 8L + 1L) * 8L;
        out.SetLen(len);
        cbuff = out.GetPtr();
    }
//	len = len / 4 + 1;
    len = len / 4;
    if (!cbuff)
        return false;
    DWORD *dbuff = (DWORD *) cbuff;
//	for (DWORD i = 1; i < len; i += 2)
    for (DWORD i = 0; i < len; i += 2)
        rc5.Encode(dbuff[i], dbuff[i + 1]);
    return true;
}

bool RC5::Decode(MArray<BYTE> &in, MArray<BYTE> &out)
{

    InRC_5 rc5(m_pKey);
    BYTE *cbuff = in.GetPtr();
    if (!cbuff)
        return false;
//	int offset = 1;
    int offset = 0;
//	if (memcmp(cbuff, cRC5, 4))
//		return false;
    out = in;
    int len = out.GetLen() - offset * 4;
    if (len % 8)
    {
        len = (len / 8L + 1L) * 8L;
        out.SetLen(len + offset * 4);
    }
    len /= 4;
    len += offset;
    DWORD *buff = (DWORD *) out.GetPtr();
    DWORD dlen = (DWORD) len;
//	for (DWORD i = 1; i < dlen; i += 2)
    for (DWORD i = 0; i < dlen; i += 2)
        rc5.Decode(buff[i], buff[i + 1]);
    //len = *(DWORD *)(out.GetPtr() + 4);
    //if (len > in.GetLen() - 8)
    //	return FALSE;
    //out.Remove(0, 8);
    //out.SetLen(len);
    return true;
}
