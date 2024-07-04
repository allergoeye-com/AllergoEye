#ifndef __LZW_STREAM_OUT__
#define __LZW_STREAM_OUT__

class LZWstreamOut {
	public:
		LZWstreamOut(MArray<BYTE> &dst, short CodeLen, int BufferLen);
		LZWstreamOut(MArray<BYTE> &dst, short CodeLen);

		BYTE *GetStream() const { return Stream; }
		void ResetIndex() { Index = 0; }
		void ResetStream() { Index = 0; BlockIndex = 0; BitShift = 0; }
		LONG GetIndex() const { return Index; }

		BOOL PutCode(short code, BOOL brk = 0);
		BOOL PutData(BYTE  *str, LONG len);
		void CloseStream();
protected :
		void CreateCode(DWORD &lch, BOOL brk);
protected :
		MArray<BYTE> *Dst;
		BYTE  *Stream;
		LONG StreamLen;
		BYTE Block[260];
		LONG Index;
		WORD BlockIndex;
		WORD BitShift;
		WORD _CodeLen;
		BOOL valid;
};
#endif
