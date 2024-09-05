#include "stdafx.h"
#include "lzwdecom.h"
//-----------------------------------------------------
//
//-----------------------------------------------------
UnPackLZW::UnPackLZW(BYTE *src, int _len)
{
	for (int i = 0; i < 16; ++i)
		Mask[i] = 1 << i;

	Init(src, _len);
}
UnPackLZW::UnPackLZW()
{
	for (int i = 0; i < 16; ++i)
		Mask[i] = 1 << i;
	status = LZW_ENDOFCODE;
	Dst = 0;
	DstLen = 0;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
void UnPackLZW::Init(BYTE *src, int _len)
{
	LZWstreamIn::Init(src, _len);
	status = LZW_READOK;
	DstLen = *(LONG *)src;
	MaxTableLen = *(short *)(src + 8);
	MinCodeLen = *(src + 10);
	InitStream(11);
	_CodeLen = MinCodeLen;
	ResetTable = 1 << (MinCodeLen - 1);
	EndOfCode = ResetTable + 1;
	LastInTable = FirstInTable = EndOfCode + 1;
	table.SetLen(MaxTableLen + 1);
	Table = table.GetPtr();
	for (int i = 0; i < ResetTable; ++i)
	{
		Table[i].code = -1;
		Table[i].ch = (char)i;
	}
	Dst = 0;
	
}

//-----------------------------------------------------
//
//-----------------------------------------------------
UnPackLZW::UnPackLZW(MArray<BYTE> &src)
{
	for (int i = 0; i < 16; ++i)
		Mask[i] = 1 << i;
	Init(src.GetPtr(), src.GetLen());
}
//-----------------------------------------------------
//
//-----------------------------------------------------
UnPackLZW::~UnPackLZW()
{
	
}
LONG UnPackLZW::Unpack(MFILE &file)
{
	DWORD dw = file.GetSeek();

	file.SetSize(dw + DstLen);
	MArray<char> b(0x22000);	
	LONG Index = 0;
	short Code;
	short OldCode;
	short InCode;
	short ch;
	char *_stack, stack[1024];
	_stack = stack;
	char StackPos = 0;
	char *_Dst = b.GetPtr();
	if (status != LZW_READOK)
		return 0L;
	switch (OldCode = Code = EnterFirstCode())
	{
	case -1 :
		return -1L;
	case - 2 :
		return Index;
	default :
		*_Dst++ = (char)Code;
		++Index;
		Final = Code;
	}

	while (Index < DstLen)
	{
		
		if ((Code = GetCode()) > LastInTable || Code == -1)
		{
			status = LZW_READERROR; 
			return -1;
		}
	/*	if (Code == EndOfCode)
		{
			status = LZW_ENDOFCODE;
			return Index;
		}
	*/	if (Code == ResetTable)
		{
			switch (OldCode = Code = Reset())
			{
			case -1 :
				return -1;
			case -2 :
				return Index;
			default :
				*_Dst++ = (char)Code;
				++Index;
				Final = Code;
			}
		}
		else
		{
			InCode = Code;
			do
			{
				if (Code == LastInTable)
				{
					*_stack++ = (char)Final;
					Code = OldCode;
				}
				ch = (Table + Code)->ch;
				*_stack++ = (char)ch;
			}
			while ((Code = (Table + Code)->code) >= 0);

			Final = ch;
			Index += _stack - stack;
			while (_stack != stack)
				*_Dst++ = *(--_stack);

			(Table + LastInTable)->code = OldCode;
			Table[LastInTable++].ch = (BYTE)ch;

			if (LastInTable == *(Mask + _CodeLen) && LastInTable != MaxTableLen)
				++_CodeLen;
			OldCode = InCode;
		}
		if (_Dst - b.GetPtr() >= 0x20000)
		{
			file.Write(b.GetPtr(), _Dst - b.GetPtr());
			_Dst = b.GetPtr(); 
		}
	}
	if (_Dst - b.GetPtr() > 0)
		file.Write(b.GetPtr(), _Dst - b.GetPtr());

	return Index;
	 
}
//-----------------------------------------------------
//
//-----------------------------------------------------
LONG UnPackLZW::Unpack(BYTE *dst, int len)
{
	LONG Index = 0;
	short Code;
	short OldCode;
	short InCode;
	short ch;
	char *_stack, stack[1024];
	_stack = stack;
	char StackPos = 0;
	if (DstLen > len) return -1;
	char *_Dst = (char *)dst;
	if (status != LZW_READOK)
		return 0L;
	switch (OldCode = Code = EnterFirstCode())
	{
	case -1 :
		return -1L;
	case - 2 :
		return Index;
	default :
		*_Dst++ = (char)Code;
		++Index;
		Final = Code;
	}

	while (Index < DstLen)
	{
		
		if ((Code = GetCode()) > LastInTable || Code == -1)
		{
			status = LZW_READERROR; 
			return -1;
		}
	/*	if (Code == EndOfCode)
		{
			status = LZW_ENDOFCODE;
			return Index;
		}
	*/	if (Code == ResetTable)
		{
			switch (OldCode = Code = Reset())
			{
			case -1 :
				return -1;
			case -2 :
				return Index;
			default :
				*_Dst++ = (char)Code;
				++Index;
				Final = Code;
			}
		}
		else
		{
			InCode = Code;
			do
			{
				if (Code == LastInTable)
				{
					*_stack++ = (char)Final;
					Code = OldCode;
				}
				ch = (Table + Code)->ch;
				*_stack++ = (char)ch;
			}
			while ((Code = (Table + Code)->code) >= 0);

			Final = ch;
			Index += _stack - stack;
			while (_stack != stack)
				*_Dst++ = *(--_stack);

			(Table + LastInTable)->code = OldCode;
			Table[LastInTable++].ch = (BYTE)ch;

			if (LastInTable == *(Mask + _CodeLen) && LastInTable != MaxTableLen)
				++_CodeLen;
			OldCode = InCode;
		}
	}
	return Index;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
LONG UnPackLZW::Unpack(MArray<BYTE> &dst)
{
	dst.SetLen(DstLen);
	return Unpack(dst.GetPtr(), DstLen);
}
LONG UnPackLZW::Unpack(MArray<char> &dst)
{
	dst.SetLen(DstLen);
	return Unpack((BYTE *)dst.GetPtr(), DstLen);
}

short UnPackLZW::EnterFirstCode()
{
short Code;

	if ((Code = GetCode()) == ResetTable)
	{
		LastInTable = FirstInTable;
		_CodeLen = MinCodeLen;
		while ((Code = GetCode()) == ResetTable)
			;
		if (Code == -1 || Code > LastInTable)
		{
			status = LZW_READERROR;
			return -1;
		}
		if (Code == EndOfCode)
		{
			status = LZW_ENDOFCODE;
			return -2;
		}
	}
	return Code;
}
short UnPackLZW::Reset()
{
short Code;
	LastInTable = FirstInTable;
	_CodeLen = MinCodeLen;
	while ((Code = GetCode()) == ResetTable);
	if (Code == -1 || Code > LastInTable)
	{
		status = LZW_READERROR;
		return -1;
	}
	if (Code == EndOfCode)
	{
		status = LZW_ENDOFCODE;
		return -2;
	}
	return Code;
}
