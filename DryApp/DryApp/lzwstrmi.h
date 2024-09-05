#ifndef __LZW_STREAM_IN__
#define __LZW_STREAM_IN__

class LZWstreamIn {
public:
		LZWstreamIn(BYTE  *Src, LONG len);
		LZWstreamIn();
		void Init(BYTE  *Src, LONG len);
		void ResetIndex() { Index = 0; }
		void ResetStream() {Index = 0; BlockIndex = 0; BitShift = 0; }
		void InitStream(LONG offset);
		LONG GetIndex() { return Index; }
		short GetCode();
protected :
		bool GetNextBlock();
		bool CheckCode(WORD NextShift);
		short GetCode(WORD NextShift);
		bool CheckBlockSize0();
		bool CheckBlockSize1();
		bool CheckBlockSize2();


private :
		BYTE Block[4];
		BYTE *pBlock;
		WORD BlockIndex;
		WORD prevBlockIndex;
protected :
		LONG Index;
		WORD BlockSize;
		WORD BitShift;
		WORD Mask[16];
		BYTE  *Src;
		LONG SrcLen;
		WORD _CodeLen;
		short LastInTable;

};
inline bool LZWstreamIn::GetNextBlock()
{
	BlockSize = Src[Index++];
	if (Index + BlockSize <= SrcLen)
	{
		pBlock  = Src + Index; 
	//	memcpy(Block + BlockIndex, Src + Index, BlockSize);
		Index += BlockSize;
		BlockSize += BlockIndex;
		BlockIndex = 0;
		return 1;
	}
	return 0;
}
inline void LZWstreamIn::InitStream(LONG offset)
{
	Index = offset;
	BlockSize = Src[Index++];
	pBlock  = Src + Index; 
	Index += BlockSize;
}

inline bool LZWstreamIn::CheckBlockSize0()
{
	BlockIndex = prevBlockIndex = 0;
	return  GetNextBlock();
}
inline bool LZWstreamIn::CheckBlockSize1()
{
	if (prevBlockIndex && BlockIndex < prevBlockIndex)
		*Block = *(Block + BlockIndex);
	else
		*Block = *(pBlock + BlockIndex - prevBlockIndex);
	BlockIndex = prevBlockIndex = 1;
	if (GetNextBlock())
	{
		*(Block + 1) = *pBlock;
		return 1;
	}
	return 0;
}
inline bool LZWstreamIn::CheckBlockSize2()
{
	if (prevBlockIndex && BlockIndex < prevBlockIndex)
		*Block = *(Block + BlockIndex);
	else
		*Block = *(pBlock + BlockIndex - prevBlockIndex);
	++BlockIndex;
	if (prevBlockIndex && BlockIndex < prevBlockIndex)
		*(Block + 1) = *(Block + BlockIndex);
	else
		*(Block + 1) = *(pBlock + BlockIndex - prevBlockIndex);
	BlockIndex = prevBlockIndex = 2;

	if (GetNextBlock())
	{
		*(Block + 2) = *pBlock;
		*(Block + 3) = *(pBlock + 1);
		return 1;
	}
	return 0;
}
inline bool LZWstreamIn::CheckCode(WORD NextShift)
{
	switch(BlockSize)
	{
	case 0:
		return CheckBlockSize0();
	case 1:
		return CheckBlockSize1();
	case 2:
	if (NextShift >= 16)
		return CheckBlockSize2();
	}

	return 1;
}
inline short LZWstreamIn::GetCode(WORD NextShift)
{
	WORD out;
	BYTE *in;
	short add; 
	if (prevBlockIndex && BlockIndex < prevBlockIndex)
		in = Block + BlockIndex;
	else
		in = pBlock + BlockIndex - prevBlockIndex;

	if (NextShift >= 16)
	{
		out = (WORD)(((DWORD)(*in | ((USHORT)(in[1]) << 8) | ((DWORD)(in[2]) << 16) | ((DWORD)(in[3]) << 24))) >> BitShift) & *(Mask + _CodeLen - 1);
		add = 2;
	}
	else
	{
		out = (((WORD)(((WORD)(in[1]) << 8) & 0xff00) | in[0]) >> BitShift) & *(Mask + _CodeLen - 1);
		add = 1;
	}
	BitShift = NextShift & 7;
	BlockIndex += add;
	BlockSize -= add;
	return out;
}
//#ifndef _DEBUG
inline short LZWstreamIn::GetCode()
{
	WORD NextShift = BitShift + _CodeLen;
	return CheckCode(NextShift) ? GetCode(NextShift) : -1;
}
//#endif

#endif
