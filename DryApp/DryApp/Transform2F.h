#ifndef __TRANSFORM2F__
#define __TRANSFORM2F__
#include "mform2f.h"

#pragma pack(push)
#pragma pack(8)

template<class T, class RC = RECT, class _TYPE = int>
struct Transform2F {


	Transform2F(const RC &_rDisp, const RC &_rDes, bool prop = true) { Init(_rDisp, _rDes, prop); };
	Transform2F() { };
	Transform2F(const Transform2F &a) { operator = (a); }
	Transform2F &operator = (const Transform2F &a);
	void Init(const RC &rDisp, const RC &rDes, bool prop = true);
	void InitInvert();
	void LPtoDP(T &p, T &res);
	void DPtoLP(_TYPE &x, _TYPE &y);
	void DPtoLP(T *ptr, int len);
	void DPtoLP(T &p);
	void LPtoDP(T *ptr, int len);
	void LPtoDP(_TYPE &x, _TYPE &y);
	void LPtoDP(T &p);
	void Rotate(int iAngle, const RC &rDisp, T *center);
	void Scale(float fScale, const RC &rDisp, T *center);
	void Offset(_TYPE x, _TYPE y);
	
	MFORM2F form;
	MFORM2F invert;
};
#pragma pack(pop)

template<class T, class RC, class _TYPE  >
void Transform2F<T, RC, _TYPE  >::Offset(_TYPE x, _TYPE y)
{
	form.m13Dx += (float)x;
	form.m23Dy += (float)y;
	InitInvert();
}
template<class T, class RC, class _TYPE  >
void Transform2F<T, RC, _TYPE  >::Scale(float fScale, const RC &rDisp, T *center)
{
	T cb;
	Vector2F cm;
	if (!center)
	{
		cb.x = rDisp.CenterPoint().x;
		cb.y = rDisp.CenterPoint().y;
	}
	else
	cb = *center;

	MFORM2F shift, scale, rotate;
	form.Decompose (shift, scale, rotate, cm);
	Transform2F<T, RC> tmp;
	scale.m11ScaleX *= fScale;
	scale.m22ScaleY *= fScale;
	tmp.form = rotate;
	tmp.form *= scale;
	_TYPE x = (_TYPE)cb.x;
	_TYPE y = (_TYPE)cb.y;
	DPtoLP(x, y);
	tmp.LPtoDP(x, y);
	form = tmp.form;
	form.m13Dx = cb.x - x;
	form.m23Dy = cb.y - y;
	InitInvert();

}
template<class T, class RC, class _TYPE  >
void Transform2F<T, RC, _TYPE  >::Rotate(int iAngle, const RC &rDisp, T *center)
{
	T cb;
	Vector2F cm;
	if (!center)
	{
		cb.x = rDisp.CenterPoint().x;
		cb.y = rDisp.CenterPoint().y;
	}
	else
	cb = *center;
	MFORM2F shift, scale, rotate;
	form.Decompose (shift, scale, rotate, cm);
	Transform2F<T, RC> tmp;
	MFORM2F r;
	r.Init();
	r.Rotate(iAngle);
	rotate *= r;
	tmp.form = rotate;
	tmp.form *= scale;
	_TYPE x = (_TYPE)cb.x;
	_TYPE y = (_TYPE)cb.y;
	DPtoLP(x, y);
	tmp.LPtoDP(x, y);
	form = tmp.form;
	form.m13Dx = cb.x - x;
	form.m23Dy = cb.y - y;
	InitInvert();
}

template<class T, class RC, class _TYPE  >
Transform2F<T, RC, _TYPE  > &Transform2F<T, RC, _TYPE  >::operator = (const Transform2F<T, RC, _TYPE  > &a)
{
	form = a.form;
	invert = a.invert;
	return *this;
}


//---------------------------------------------------
//
//---------------------------------------------------
template<class T, class RC, class _TYPE  >
void Transform2F<T, RC, _TYPE  >::Init(const RC &rDisp, const RC &rDes, bool prop)
{
	form.Init();
	form.m11ScaleX = (float)fabs((float)rDisp.Width()/(float)rDes.Width());
	form.m22ScaleY = (float)fabs((float)rDisp.Height()/(float)rDes.Height());
	if (prop)
		form.m22ScaleY = form.m11ScaleX = form.m22ScaleY > form.m11ScaleX ? form.m11ScaleX : form.m22ScaleY;
	float dx = (float)(rDes.left + rDes.right)/2.0f * form.m11ScaleX;
	float dy = (float)(rDes.bottom + rDes.top)/2.0f * form.m22ScaleY;
	form.m13Dx = (float)(rDisp.left + rDisp.right)/2.0f - dx;
	form.m23Dy = (float)(rDisp.top + rDisp.bottom)/2.0f - dy;
	InitInvert();
}


//---------------------------------------------------
//
//---------------------------------------------------
template<class T, class RC, class _TYPE  >
inline void Transform2F<T, RC, _TYPE  >::LPtoDP(T &p)
{
	LPtoDP(&p, 1);
}

//---------------------------------------------------
//
//---------------------------------------------------
template<class T, class RC, class _TYPE  >
inline void Transform2F<T, RC, _TYPE  >::LPtoDP(_TYPE &x, _TYPE &y)
{
	float _x = (float)x; 
	x = (_TYPE)(form.m11ScaleX * _x + form.m12ShiftX * (float)y + form.m13Dx );
	y = (_TYPE)(form.m21ShiftY * _x + form.m22ScaleY * (float)y + form.m23Dy );
}

//---------------------------------------------------
//
//---------------------------------------------------
template<class T, class RC, class _TYPE  >
inline void Transform2F<T, RC, _TYPE  >::LPtoDP(T *ptr, int len)
{
	while(len--)
	{
		float _x = (float)ptr->x; 
		ptr->x = (_TYPE)(form.m11ScaleX * (float)ptr->x + form.m12ShiftX * (float)ptr->y + form.m13Dx);
		ptr->y = (_TYPE)(form.m22ScaleY * (float)ptr->y + form.m21ShiftY * _x + form.m23Dy);
		++ptr;
	}
}

//---------------------------------------------------
//
//---------------------------------------------------
template<class T, class RC, class _TYPE  >
inline void Transform2F<T, RC, _TYPE  >::DPtoLP(T &p)
{
	DPtoLP(&p, 1);
}
template<class T, class RC, class _TYPE  >
void Transform2F<T, RC, _TYPE  >::InitInvert()
{
	Vector2F offset;
	MFORM2F shift, scale, rotate;
	invert.Init();
	form.Decompose (shift, scale, rotate, offset);
	if (scale.m11ScaleX)
		scale.m11ScaleX = 1.0f/scale.m11ScaleX;

	if (scale.m22ScaleY)
		scale.m22ScaleY = 1.0f/scale.m22ScaleY;
	
	rotate.m12ShiftX = -rotate.m12ShiftX;
	rotate.m21ShiftY = -rotate.m21ShiftY;
	invert.Move(-offset.x, -offset.y);
	invert *= scale;
	invert *= rotate;
} 
//---------------------------------------------------
//
//---------------------------------------------------
template<class T, class RC, class _TYPE  >
inline void Transform2F<T, RC, _TYPE  >::DPtoLP(T *ptr, int len)
{
	float _x, _y;
	while(len--)
	{
   		_x = (float)ptr->x;
		_y = (float)ptr->y;
		_x += invert.m13Dx;
		_y += invert.m23Dy;
		ptr->x = (_TYPE)(invert.m11ScaleX * _x + invert.m12ShiftX * _y  );
		ptr->y = (_TYPE)(invert.m22ScaleY * _y + invert.m21ShiftY * _x );
		++ptr;
	}

}
//---------------------------------------------------
//
//---------------------------------------------------
template<class T, class RC, class _TYPE  >
inline void Transform2F<T, RC, _TYPE  >::DPtoLP(_TYPE &x, _TYPE &y)
{
   	float _x = (float)x;
	float _y = (float)y;
	_x += invert.m13Dx;
	_y += invert.m23Dy;

	x = (_TYPE)(invert.m11ScaleX * _x + invert.m12ShiftX * _y);
	y = (_TYPE)(invert.m22ScaleY * _y + invert.m21ShiftY * _x);
}
template<class T, class RC, class _TYPE  >
inline void Transform2F<T, RC, _TYPE  >::LPtoDP(T &p, T &res)
{
	res.x = (_TYPE)(form.m11ScaleX * p.x + form.m12ShiftX * p.y + form.m13Dx );
	res.y = (_TYPE)(form.m22ScaleY * p.y + form.m21ShiftY * p.x + form.m23Dy );
}
#endif
