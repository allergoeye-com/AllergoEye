#include "stdafx.h"
#include "lzwcomp.h"

//-----------------------------------------------------
//
//-----------------------------------------------------
PackTable::PackTable(short TableSize, short MinSize)
{
    MaxTableLen = TableSize;
    LastInTable = FirstInTable = MinSize + 2;
    Table = new PackTableNode[MaxTableLen + 1];
    PTI = new PackTableIndex[MaxTableLen + 1];
    int i;
    for (i = 0; i < FirstInTable - 2; ++i)
    {
        Table[i].code = -1;
        Table[i].ch = (char) i;
    }

    for (i = 0; i <= MaxTableLen; ++i)
    {
        PTI[i].index0 = 0;
        PTI[i].index = 0;
    }
}

//-----------------------------------------------------
//
//-----------------------------------------------------
PackTable::~PackTable()
{
    delete[] Table;
    delete[] PTI;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
short PackTable::Add(short code, BYTE ch)
{
    if (LastInTable == MaxTableLen)
        return -1;
    Table[LastInTable].code = code;
    Table[LastInTable].ch = ch;

    if (!PTI[code].index0)
        PTI[code].index0 = LastInTable++;
    else
    {
        int i;
        for (i = PTI[code].index0; PTI[i].index;)
            i = PTI[i].index;
        PTI[i].index = LastInTable++;
    }
    return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
short PackTable::Find(short Code, BYTE ch)
{
    for (short i = PTI[Code].index0; i; i = PTI[i].index)
        if (Table[i].ch == ch)
            return i;
    return -1;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void PackTable::Reset()
{
    LastInTable = FirstInTable;

    for (short i = 0; i <= MaxTableLen; ++i)
    {
        PTI[i].index = 0;
        PTI[i].index0 = 0;
    }
}

//-----------------------------------------------------
//
//-----------------------------------------------------
PackLZW::PackLZW(BYTE *_Src, int _SrcLen, MArray<BYTE> &dst, short TableSize, short MinCodeLen) :
        LZWstreamOut(dst, MinCodeLen, _SrcLen), Table(TableSize, 1 << (MinCodeLen - 1))
{
    PackLZW::MinCodeLen = MinCodeLen;
    ResetTable = 1 << (MinCodeLen - 1);
    EndOfCode = ResetTable + 1;

    for (short i = 0; i < 16; ++i)
        Mask[i] = (1 << i) + 1;

    Prefix = -1;
    Src = _Src;
    SrcLen = _SrcLen;
    if (Src)
    {
        // ������ ��������� ������ ������ :
        // ����� �������������� ������			4 �����
        // ����� ������������� ������      		4 �����
        // ������ ������������ �������			2 �����
        // ����� (� �����) ������������ ����	1 ����
        PutData((BYTE *) &SrcLen, 4);
        PutData((BYTE *) &SrcLen, 4);
        PutData((BYTE *) &TableSize, 2);
        BYTE tmp = MinCodeLen;
        PutData(&tmp, 1);
        PutCode(ResetTable);
    } else
        valid = 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
LONG PackLZW::Pack()
{
    BYTE ch;
    short Code;
    LONG Index = 0;
    LONG OldIndex = GetIndex();

    if (!valid)
        return 0L;

    Table.Reset();

    if (Prefix < 0)
        Prefix = Src[Index++];

    while (Index < SrcLen)
    {
        ch = Src[Index++];
        if ((Code = Table.Find(Prefix, ch)) == -1)
        {
            PutCode(Prefix);
            if (Table.Add(Prefix, ch) == -1)
            {
                PutCode(ResetTable);
                Table.Reset();
                _CodeLen = MinCodeLen;
            }
            if (Table.GetLastIndex() == Mask[_CodeLen] && Table.FreeSpace())
                ++_CodeLen;
            Prefix = ch;
        } else
            Prefix = Code;
    }

    PutCode(Prefix);
    Prefix = -1;
    PutCode(EndOfCode, 1);
    CloseStream();
    return GetIndex() - OldIndex;
}
