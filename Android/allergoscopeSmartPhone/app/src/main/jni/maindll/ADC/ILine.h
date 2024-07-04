#ifndef __ILine__
#define __ILine__
#define R2_COPYALPHA 12345678
#include "AColor.h"
#pragma pack(push)
#pragma pack(8)
EXPORTNIX class EXPMDC  IPixel {
public :
	IPixel() { _Rop = 0; }
	typedef void (IPixel::*Operation)(DWORD *, DWORD);
	void Rop(DWORD *ptr, DWORD pen) { (this->*_Rop)(ptr, pen); }
	void SetRop(int rop);
	Operation _Rop;
//private :
		void opCOPYALPHA(DWORD *ptr, DWORD pen) { *ptr = (*ptr & 0x00FFFFFF) | (pen & 0xFF000000); } 
		void opBLACK(DWORD *ptr, DWORD pen); // 	0
		void opCOPYPEN(DWORD *ptr, DWORD pen); //P
		void opMASKNOTPEN(DWORD *ptr, DWORD pen); // 	DPna
		void opMASKPEN(DWORD *ptr, DWORD pen); // 	DPa
		void opMASKPENNOT(DWORD *ptr, DWORD pen); // 	PDna
		void opMERGENOTPEN(DWORD *ptr, DWORD pen); // 	DPno
		void opMERGEPEN(DWORD *ptr, DWORD pen); // 	DPo
		void opMERGEPENNOT(DWORD *ptr, DWORD pen); // 	PDno
		void opNOP(DWORD *ptr, DWORD pen); // 	D
		void opNOT(DWORD *ptr, DWORD pen); // 	Dn
		void opNOTCOPYPEN(DWORD *ptr, DWORD pen); // 	Pn
		void opNOTMASKPEN(DWORD *ptr, DWORD pen); // 	DPan
		void opNOTMERGEPEN(DWORD *ptr, DWORD pen); // 	DPon
		void opNOTXORPEN(DWORD *ptr, DWORD pen); // 	DPxn
		void opWHITE(DWORD *ptr, DWORD pen); // 	1
		void opXORPEN(DWORD *ptr, DWORD pen); // 	DPx

};
EXPORTNIX class EXPMDC  ILine : public IPixel {
public :
		enum TYPE_LINE { VERT, HORZ, NHORZ, NVERT };
		ILine() {memset (this, 0, sizeof (ILine)); }
		void Init(int x1, int y1, int x2, int y2);
		ILine(ILine &src);
		BOOL Next() { return (this->*_Next)(); }

		void Reset() { 	cX = sX; cY = sY; Ex = Ex0; Ey = Ey0;}
		void Draw(DWORD*, int, int, DWORD);
		void Draw(DWORD *Buffer, const ImageInfo &info, DWORD color);
		void Draw(DWORD *Buffer, const ImageInfo &info, DWORD color, int op);
		void Draw(DWORD *Buffer, const ImageInfo &info, int width, DWORD color, DWORD bkcolor, int bkmode,  int op, int attrib);
		void Draw(DWORD *Buffer, const ImageInfo &info, int width, DWORD color);
		void DrawRestrict(ColorRef *Buffer, const ImageInfo &info, BYTE color_alfa, BYTE bg_alfa);
public:
	int cX, cY;
private :
		BOOL NextForX();
		BOOL NextForY();
		void DrawY(DWORD *Buffer, const ImageInfo &info, int width, DWORD color);
		void DrawX(DWORD *Buffer, const ImageInfo &info, int width, DWORD color);
		void DrawDashY(DWORD *Buffer, const ImageInfo &info, int width, DWORD color, int step1, DWORD bkcolor, int bkmode, int step2);
		void DrawDashX(DWORD *Buffer, const ImageInfo &info, int width, DWORD color, int step1, DWORD bkcolor, int bkmode, int step2);
		void DrawDash(DWORD *Buffer, const ImageInfo &info, int width, DWORD color, int step1, DWORD bkcolor, int bkmode, int step2);

//		void DrawPix(DWORD *Buffer, int _x, int _y, const ImageInfo &info, int width, int w2p1, DWORD color);
	//	void _DrawPix(DWORD *Buffer, int __x, int _y, const ImageInfo &info, int width, int k, int w2p1, DWORD color);
		typedef BOOL (ILine::*NextPoint)();
		NextPoint _Next;
		int sX, sY;
		int XEnd, YEnd;
		int Dx, Dy;
		int Dx2, Dy2;
		int dX, dY;
		int Ex, Ey;
		int Ex0, Ey0;
		int type;
		bool fIsPixel;
};
#pragma pack(pop)

inline void IPixel::opBLACK(DWORD *ptr, DWORD pen) // 	0
{
		*ptr &= (0xFF000000 & pen);
}
inline void IPixel::opCOPYPEN(DWORD *ptr, DWORD pen) //P
{
	*ptr = pen;
}
inline void IPixel::opMASKNOTPEN(DWORD *ptr, DWORD pen) // 	 перо & ~устройство DPna
{
	*ptr = ((pen &0x00FFFFFF) & ((~ptr[0]) & 0x00FFFFFF)) | (0xFF000000 & pen);
}
inline void IPixel::opMASKPEN(DWORD *ptr, DWORD pen) // 	DPa
{
	*ptr = (((pen &0x00FFFFFF) & ptr[0]) & 0x00FFFFFF) | (0xFF000000 & pen);

}
inline void IPixel::opMASKPENNOT(DWORD *ptr, DWORD pen) // 	PDna
{
	*ptr = (((~pen) &0x00FFFFFF) & (ptr[0] & 0x00FFFFFF)) | (0xFF000000 & pen);
}
inline void IPixel::opMERGENOTPEN(DWORD *ptr, DWORD pen) // 	DPno
{
	*ptr = ((pen &0x00FFFFFF) | ((~ptr[0]) & 0x00FFFFFF)) | (0xFF000000 & pen);
}
inline void IPixel::opMERGEPEN(DWORD *ptr, DWORD pen) // 	DPo
{
	*ptr = ((pen &0x00FFFFFF) | (ptr[0] & 0x00FFFFFF)) | (0xFF000000 & pen);
}
inline void IPixel::opMERGEPENNOT(DWORD *ptr, DWORD pen) // 	PDno
{
	*ptr = (((~pen) &0x00FFFFFF) | (ptr[0] & 0x00FFFFFF)) | (0xFF000000 & pen);
}
inline void IPixel::opNOP(DWORD *ptr, DWORD pen) // 	D
{
	*ptr = (*ptr & 0x00FFFFFF) | (0xFF000000 & pen);
}
inline void IPixel::opNOT(DWORD *ptr, DWORD pen) // 	Dn
{
	*ptr = ((~ptr[0]) & 0x00FFFFFF) | (0xFF000000 & pen);

}
inline void IPixel::opNOTCOPYPEN(DWORD *ptr, DWORD pen) // 	Pn
{
	*ptr = ((~pen) & 0x00FFFFFF) | (0xFF000000 & pen);
}
inline void IPixel::opNOTMASKPEN(DWORD *ptr, DWORD pen) // 	DPan
{
	*ptr = (~(pen & ptr[0]) & 0x00FFFFFF) | (0xFF000000 & pen);
}
inline void IPixel::opNOTMERGEPEN(DWORD *ptr, DWORD pen) // 	DPon
{
	*ptr = (~(pen | ptr[0]) & 0x00FFFFFF) | (0xFF000000 & pen);
}
inline void IPixel::opNOTXORPEN(DWORD *ptr, DWORD pen) // 	DPxn
{
	*ptr = (~(pen ^ ptr[0]) & 0x00FFFFFF) | (0xFF000000 & pen);
}
inline void IPixel::opWHITE(DWORD *ptr, DWORD pen) // 	1
{
	*ptr = 0x00FFFFFF | (0xFF000000 & pen);
}
inline void IPixel::opXORPEN(DWORD *ptr, DWORD pen) // 	DPx
{
	*ptr = ((pen ^ ptr[0]) & 0x00FFFFFF) | (0xFF000000 & pen);

}

inline BOOL ILine::NextForX()
{
	cX += dX;
	Ey += Dy2;
	if (Ey >= 0)
	{
		Ey -= Dx2;
		cY += dY;
	}
	return cX != XEnd;
}
inline BOOL ILine::NextForY()
{
	cY += dY;
	Ex += Dx2;
	if (Ex >= 0)
	{
		Ex -= Dy2;
		cX += dX;
	}
	return cY != YEnd;
}
/*
inline void ILine::DrawPix(DWORD *Buffer, int _x, int _y, const ImageInfo &info, int width, int w2p1, DWORD color)
{
	int __x;
	for (int k = - width; k <= width; ++k)
	{
		__x = _x + k;
		if (__x >= 0 && __x < X)
		_DrawPix(Buffer, _x + k, _y, X, Y, width, k, w2p1, color);
	}
}
inline void ILine::_DrawPix(DWORD *Buffer, int __x, int _y, const ImageInfo &info, int width, int k, int w2p1, DWORD color)
{
	int k2 = k * k;
	int __y, d2;
	for (int l = -width; l <= width; ++l)
	{
		 __y = _y + l;
		if (__y >= 0 && __y < Y)
		{
			d2 = k2 + l * l;
			if ((d2 + 1) <= w2p1)
				Buffer[__x + __y * X] = color;
		}
	}
}
inline void ILine::Draw(DWORD *Buffer, const ImageInfo &info, int width, DWORD color)
{
	if (width == 1)
		Draw(Buffer, X, Y, width, color);
	else
	{
		Reset();

		int w2p1 = width * width + 1;
		do
			DrawPix(Buffer, cX, cY, X, Y, width, w2p1, color);
		while ((this->*_Next)());
		DrawPix(Buffer, cX, cY, X, Y, width, w2p1, color);

	}
}
*/



#endif
