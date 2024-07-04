#ifndef __AproxLine__
#define __AproxLine__
template <class T, class _TYPE_ >
struct AproxLine {

	AproxLine () {}
	void Exec (MArray<T> &in, MArray<T> &out, int step);
	void ExecBez (MArray<T> &in, MArray<T> &out, int step);

	float Corner1(float x1, float x2, float x3, float x4) { return (-5.0f * x1 + 18.0f * x2 - 9.0f * x3 + 2.0f * x4)/6.0f; }
	float Corner2(float x1, float x2, float x3, float x4) { return ( 2.0f * x1 - 9.0f * x2 + 18.0f * x3 - 5.0f * x4)/6.0f; }
	void ToBezier(T &a, T &b, T &c, T &d)
	{

		T b0(Corner1(a.x, b.x, c.x, d.x), Corner1(a.y, b.y, c.y, d.y));
		T c0(Corner2(a.x, b.x, c.x, d.x), Corner2(a.y, b.y, c.y, d.y));
		b = b0;
		c = c0;
	}
	void ToBezier(T *p)
	{

		T b0(Corner1(p[0].x, p[1].x, p[2].x, p[3].x), Corner1(p[0].y, p[1].y, p[2].y, p[3].y));
		T c0(Corner2(p[0].x, p[1].x, p[2].x, p[3].x), Corner2(p[0].y, p[1].y, p[2].y, p[3].y));
		p[1] = b0;
		p[2] = c0;
	}

	void ToBezier(T &a, T &b, T &c, T &d, MArray<T> &res)
	{
		res.Add() = a;
		PathBezier((float)a.x, (float)a.y, Corner1(a.x, b.x, c.x, d.x), Corner1(a.y, b.y, c.y, d.y), Corner2(a.x, b.x, c.x, d.x), Corner2(a.y, b.y, c.y, d.y), (float)d.x, (float)d.y, res);
		res.Add() = d;
	}
	void PathBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, MArray<T> &pt);

};
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
template <class T, class _TYPE_ >
void AproxLine<T, _TYPE_>::Exec (MArray<T> &in, MArray<T> &out, int step)
{

	int i, count = in.GetLen() - 1;

	while (step && !(count/step >= 3))
		--step;
	if (!step)
	{
		out += in;
		return;
	}
	int ext = count - (count/(step * 3) * step * 3);
	i = 0;
	if (ext)
	{
		MArray<T> p = in.Left(ext);
		Exec (p, out, step);
		i = ext;
	}

	for (; i < count; i += step * 3)
		ToBezier(in[i], in[i + step], in[i + step * 2], in[i + step * 3], out);
}
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
template <class T, class _TYPE_ >
void AproxLine<T, _TYPE_>::ExecBez (MArray<T> &in, MArray<T> &out, int step)
{

	int i, count = in.GetLen() - 1;

	while (step && !(count/step >= 3))
		--step;
	if (!step)
	{
		out += in;
		return;
	}
	int ext = count - (count/(step * 3) * step * 3);
	i = 0;
	if (ext)
	{
		MArray<T> p = in.Left(ext);
		ExecBez (p, out, step);
		i = ext;
	}
	
	for (; i < count; i += step * 3)
	{
		int pos = out.GetLen();
		if (!pos)
			out.Add(in[i]);
		else
			--pos;
		out.Add(in[i + step]);
		out.Add(in[i + step * 2]);
		out.Add(in[i + step * 3]);
		ToBezier(out.GetPtr() + pos);
	}
}


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
template <class T, class _TYPE_ >
void AproxLine<T, _TYPE_>::PathBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, MArray<T> &pt)
{
	float A = y4 - y1;
	float B = x1 - x4;
	float C = -B * y1 - x1 * A;
	float AB  = A * A + B * B;
	if (AB <= 1.0e-6)
	{
		T &p = pt.Add();
		p.x = (_TYPE_)x4;
		p.y = (_TYPE_)y4;
		return;
	}
	float f1 = A * x2 + B * y2 + C;
	if ((f1 * f1) < AB)
	{
		f1 = A * x3 + B * y3 + C;
		if ((f1 * f1) < AB)
		{
			T &p = pt.Add();

			p.x = (_TYPE_)x4;
			p.y = (_TYPE_)y4;
			return;
		}
	}
	float x12 = x1 + x2;
	float y12 = y1 + y2;
	float x23 = x2 + x3;
	float y23 = y2 + y3;
	float x34 = x3 + x4;
	float y34 = y3 + y4;

	float x1223 = x12 + x23;
	float y1223 = y12 + y23;
	float x2334 = x23 + x34;
	float y2334 = y23 + y34;

	float x = x1223 + x2334;
	float y = y1223 + y2334;

	PathBezier(x1, y1, x12/2.0f, y12/2.0f, x1223/4.0f, y1223/4.0f, x/8.0f, y/8.0f, pt);
	PathBezier(x/8.0f, y/8.0f, x2334/4.0f, y2334/4.0f, x34/2.0f, y34/2.0f, x4, y4, pt);
}
#endif
