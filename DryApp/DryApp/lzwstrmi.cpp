#include "stdafx.h"
#include "lzwstrmi.h"

//-----------------------------------------------------
//
//-----------------------------------------------------
LZWstreamIn::LZWstreamIn(BYTE  *src, LONG len)
{
WORD j = 0xffff;
	for (short i = 15; i >= 0; --i, j >>= 1)
		Mask[i] = j;
	Init(Src, len);
}
LZWstreamIn::LZWstreamIn()
{
	Src = 0;
	SrcLen = 0;
	_CodeLen = 9;
	Index = 0L;
	BitShift = 0;
	BlockIndex = 0;
	BlockSize = 0;
	prevBlockIndex = 0;
	WORD j = 0xffff;
	for (short i = 15; i >= 0; --i, j >>= 1)
		Mask[i] = j;

}

void LZWstreamIn::Init(BYTE  *src, LONG len)
{
	Src = src;
	SrcLen = len;
	_CodeLen = 9;
	Index = 0L;
	BitShift = 0;
	BlockIndex = 0;
	BlockSize = 0;
	prevBlockIndex = 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
#if (0) //def _DEBUG
short LZWstreamIn::GetCode()
{
	WORD out;
	WORD NextShift = BitShift + _CodeLen;
	BYTE *in;

	if (!BlockSize)
	{
	
		BlockIndex = prevBlockIndex = 0;
		if (!GetNextBlock())
			return -1;
	}
	else
	if (BlockSize == 1)
	{
		if (prevBlockIndex && BlockIndex < prevBlockIndex)
			*Block = *(Block + BlockIndex);
		else
			*Block = *(pBlock + BlockIndex - prevBlockIndex);
		BlockIndex = prevBlockIndex = 1;
		if (!GetNextBlock())
			return -1;
		*(Block + 1) = *pBlock;
		*(Block + 2) = *(pBlock + 1);
		*(Block + 3) = *(pBlock + 2);
	}
	else
	if (BlockSize ==  2)
	{
		if (NextShift >= 16)
		{
			if (prevBlockIndex && BlockIndex < prevBlockIndex)
				*Block = *(Block + BlockIndex);
			else
				*Block = pBlock[BlockIndex - prevBlockIndex];
			++BlockIndex;
			if (prevBlockIndex && BlockIndex < prevBlockIndex)
				*(Block + 1) = *(Block + BlockIndex);
			else
				*(Block + 1) = *(pBlock + BlockIndex - prevBlockIndex);
			BlockIndex = prevBlockIndex = 2;

			if (!GetNextBlock())
				return -1;
			*(Block + 2) = *pBlock;
			*(Block + 3) = *(pBlock + 1);
		}
	}
	if (prevBlockIndex && BlockIndex < prevBlockIndex)
		in = Block + BlockIndex;
	else
		in = pBlock + BlockIndex - prevBlockIndex;

	if (NextShift >= 16)
	{
		out = (WORD)(((DWORD)(*in | ((USHORT)(in[1]) << 8) | ((DWORD)(in[2]) << 16) | ((DWORD)(in[3]) << 24))) >> BitShift) & *(Mask + _CodeLen - 1);
		BitShift = NextShift & 7;
		BlockIndex += 2;
		BlockSize -= 2;
	}
	else
	{
		 out = (((WORD)(((WORD)(in[1]) << 8) & 0xff00) | *in) >> BitShift) & *(Mask + _CodeLen - 1);
		BitShift = NextShift & 7;
		++BlockIndex;
		--BlockSize;
	}
	return out;
}
#endif