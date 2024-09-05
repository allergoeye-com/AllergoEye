// RC51.h: interface for the RC5 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RC51_H__58B6AE2C_1BE7_4A93_8A0B_F698A36B68CC__INCLUDED_)
#define AFX_RC51_H__58B6AE2C_1BE7_4A93_8A0B_F698A36B68CC__INCLUDED_

#include "buffer.h"

class RC5 {
public:
    RC5(BYTE Key[16]);

    virtual ~RC5();

    bool Encode(MArray<BYTE> &in, MArray<BYTE> &out);

    bool Decode(MArray<BYTE> &in, MArray<BYTE> &out);

private:
    BYTE m_pKey[16];
};

#endif // !defined(AFX_RC51_H__58B6AE2C_1BE7_4A93_8A0B_F698A36B68CC__INCLUDED_)
