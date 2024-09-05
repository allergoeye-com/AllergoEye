#ifndef __LZW_UNPACK__
#define __LZW_UNPACK__

#include "lzwstrmi.h"
#include "mfile.h"


enum LZW_READ {
    LZW_ENDOFCODE = -2, LZW_READERROR = -1, LZW_READOK = 0
};
struct UnPackTable {
    short code;
    BYTE ch;
};

class UnPackLZW : public LZWstreamIn, public MArray<char> {
public:
    UnPackLZW();

    UnPackLZW(MArray<BYTE> &Src);

    UnPackLZW(BYTE *Src, int len);

    void Init(BYTE *Src, int len);

    ~UnPackLZW();

    int GetStatus()
    { return status; }

    LONG Unpack(MArray<char> &Dst);

    LONG Unpack(MArray<BYTE> &Dst);

    LONG Unpack(BYTE *dst, int len);

    LONG Unpack(MFILE &file);

    int status;
protected :
    short Reset();

    short EnterFirstCode();

protected :
    BYTE *Dst;
    LONG DstLen;
    MArray<UnPackTable> table;
    UnPackTable *Table;
    //	short LastInTable;
    short MaxTableLen;
    short ResetTable;
    short EndOfCode;
    short FirstInTable;
    short MinCodeLen;
    short Mask[16];

    short Final;
};


#endif
