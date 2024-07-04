#ifndef __LZW_PACK__
#define __LZW_PACK__
#include "lzwstrmo.h"

struct PackTableNode {
		short code;
		BYTE ch;
	};
struct PackTableIndex {
		short index0;
		short index;
	};

class PackTable {
		PackTableNode *Table;
		PackTableIndex *PTI;

		short LastInTable;
		short MaxTableLen;
		short FirstInTable;

public :
		PackTable(short size, short minsize);
		~PackTable();
		short GetLastIndex() const { return LastInTable; }

		short Find(short code, BYTE ch);
		short Add(short code, BYTE ch);
		void Reset();
		short FreeSpace() const { return MaxTableLen - LastInTable; }
	};

class PackLZW : public LZWstreamOut {
	public:
		PackLZW(BYTE  *Src, int len, MArray<BYTE> &dst, short TableSize = 8192, short MinCode = 9);
		~PackLZW() {}
		LONG Pack();
protected:
		PackTable Table;
		short ResetTable;
		short EndOfCode;
		short MinCodeLen;
		short Mask[16];
		short Prefix;

		BYTE  *Src;
		LONG SrcLen;
	};

#endif
