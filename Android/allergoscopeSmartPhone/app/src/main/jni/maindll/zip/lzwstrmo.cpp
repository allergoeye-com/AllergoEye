#include "stdafx.h"
#include "lzwstrmo.h"



//-----------------------------------------------------
//
//-----------------------------------------------------
LZWstreamOut::LZWstreamOut(MArray<BYTE> &dst, short CodeLen, int BufferLen)
{
	Dst = &dst;
	Dst->SetLen(BufferLen);
	Stream = Dst->GetPtr();
	if (Stream) 
		StreamLen = BufferLen;
	else
		StreamLen = 0;
	_CodeLen = CodeLen;
	Index = 0;
	BitShift = 0;
	BlockIndex = 0;
	valid = 1;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
LZWstreamOut::LZWstreamOut(MArray<BYTE> &dst, short CodeLen)
{
	Dst = &dst;
	Dst->SetLen(32768L);
	Stream = Dst->GetPtr();
	StreamLen = Dst->GetLen();
	_CodeLen = CodeLen;
	Index = 0;
	BitShift = 0;
	BlockIndex = 0;
	valid = 1;
}
//======================================================
//
//======================================================
BOOL LZWstreamOut::PutData(BYTE  *str, LONG len)
{
	if (Index + len >= StreamLen)
	{
		Dst->SetLen(StreamLen + len + 32768L);
		Stream = Dst->GetPtr();
		StreamLen = Dst->GetLen();
	}
	memcpy(Stream + Index, str, len);
	Index += len;
	return 1;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
void LZWstreamOut::CloseStream()
{
	*(DWORD *)Dst->GetPtr() = StreamLen;
	*(DWORD *)(Dst->GetPtr() + sizeof(DWORD)) = Index;
	Dst->SetLen(Index);

}
void LZWstreamOut::CreateCode(DWORD &lch, BOOL brk)
{
	BYTE mask = 0xff >> (8 - BitShift);
	lch <<= BitShift;
	*(Block + BlockIndex) &= mask;
	*(Block + BlockIndex) |= (BYTE)(lch & 0xff);
	short NextShift = BitShift + _CodeLen;
	if (NextShift > 7)
		*(Block + BlockIndex + 1) = (BYTE)((lch >> 8) & 0xff);
	if (NextShift > 15)
		*(Block + BlockIndex + 2) = (BYTE)((lch >> 16) & 0xff);
	BlockIndex += NextShift >> 3;
	BitShift = NextShift & 7;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL LZWstreamOut::PutCode(short code, BOOL brk)
{
	DWORD lch = code;
	CreateCode(lch, brk);
	if (BlockIndex > 250 || brk != 0)
	{
		if (Index + BlockIndex + 4 >= StreamLen)
		{
			Dst->SetLen(StreamLen + 32768L);
			Stream = (BYTE  *)Dst->GetPtr();
			StreamLen = Dst->GetLen();
		}
		if (brk)
		{
			*(Stream + Index) = (BYTE)(BlockIndex + 1);
			++Index;
			for (WORD i = 0; i <= BlockIndex; ++i, ++Index)
			   *(Stream + Index) = *(Block + i);
			BlockIndex = 0;
			BitShift = 0;
		}
		else
		{
			*(Stream + Index) = (BYTE)BlockIndex;
			++Index;
			int i = 0;
			for (; i < BlockIndex; ++i, ++Index)
				*(Stream + Index) = *(Block + i);
			BlockIndex = 0;
			*Block = *(Block + i);
		}
		return 1;
	}
	return 0;
}
