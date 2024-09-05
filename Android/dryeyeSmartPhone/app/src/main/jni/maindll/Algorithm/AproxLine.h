#ifndef __AproxLine__
#define __AproxLine__

template<class T, class _TYPE_>
struct AproxLine {

    AproxLine()
    {}

    void Exec(MArray<T> &in, MArray<T> &out, int step);

    void ExecBez(MArray<T> &in, MArray<T> &out, int step);

    _TYPE_ Corner1(_TYPE_ x1, _TYPE_ x2, _TYPE_ x3, _TYPE_ x4)
    { return (-5.0f * x1 + 18.0f * x2 - 9.0f * x3 + 2.0f * x4) / 6.0f; }

    _TYPE_ Corner2(_TYPE_ x1, _TYPE_ x2, _TYPE_ x3, _TYPE_ x4)
    { return (2.0f * x1 - 9.0f * x2 + 18.0f * x3 - 5.0f * x4) / 6.0f; }

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
        PathBezier((_TYPE_) a.x, (_TYPE_) a.y, Corner1(a.x, b.x, c.x, d.x),
                   Corner1(a.y, b.y, c.y, d.y), Corner2(a.x, b.x, c.x, d.x),
                   Corner2(a.y, b.y, c.y, d.y), (_TYPE_) d.x, (_TYPE_) d.y, res);
        res.Add() = d;
    }

    void PathBezier(_TYPE_ x1, _TYPE_ y1, _TYPE_ x2, _TYPE_ y2, _TYPE_ x3, _TYPE_ y3, _TYPE_ x4, _TYPE_ y4,  MArray<T> &pt);
    void PathBezier(_TYPE_ x1, _TYPE_ y1, _TYPE_ x2, _TYPE_ y2, _TYPE_ x3, _TYPE_ y3, MArray<T> & pt);
   

};

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
template<class T, class _TYPE_>
void AproxLine<T, _TYPE_>::Exec(MArray<T> &in, MArray<T> &out, int step)
{

    int i, count = in.GetLen() - 1;

    while (step && !(count / step >= 3))
        --step;
    if (!step)
    {
        out += in;
        return;
    }
    int ext = count - (count / (step * 3) * step * 3);
    i = 0;
    if (ext)
    {
        int n = ext / 3;
        if (n == 0)
        {
            MArray<T> p = in.Left(ext);
            Exec(p, out, step);
        }
        else
        {
            out.Add(in[0]);
            ToBezier(in[0], in[n], in[n * 2], in[ext - 1], out);
        }
        i = ext;
    }
    else
        out.Add(in[0]);
    int s3 = step * 3;
    int s2 = step * 2;
    count -= s3;
    for (; i < count; i += s3)
        ToBezier(in[i], in[i + step], in[i + s2], in[i + s3], out);
    count += s3;
    ToBezier(in[i], in[i + step], in[i + s2], in[count], out);
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
template<class T, class _TYPE_>
void AproxLine<T, _TYPE_>::ExecBez(MArray<T> &in, MArray<T> &out, int step)
{

    int i, count = in.GetLen() - 1;

    while (step && !(count / step >= 3))
        --step;
    if (!step)
    {
        out += in;
        return;
    }
    int ext = count - (count / (step * 3) * step * 3);
    i = 0;
    if (ext)
    {
        MArray<T> p = in.Left(ext);
        ExecBez(p, out, step);
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
template<class T, class _TYPE_>
void AproxLine<T, _TYPE_>::PathBezier(_TYPE_ x1, _TYPE_ y1, _TYPE_ x2, _TYPE_ y2, _TYPE_ x3, _TYPE_ y3,
                                      _TYPE_ x4, _TYPE_ y4, MArray<T> &pt)
{
    _TYPE_ A = y4 - y1;
    _TYPE_ B = x1 - x4;
    _TYPE_ C = -B * y1 - x1 * A;
    _TYPE_ AB = A * A + B * B;
    if (AB <= 1.0e-6)
    {
        T &p = pt.Add();
        p.x = (_TYPE_) x4;
        p.y = (_TYPE_) y4;
        return;
    }
    _TYPE_ f1 = A * x2 + B * y2 + C;
    if ((f1 * f1) < AB)
    {
        f1 = A * x3 + B * y3 + C;
        if ((f1 * f1) < AB)
        {
            T &p = pt.Add();

            p.x = (_TYPE_) x4;
            p.y = (_TYPE_) y4;
            return;
        }
    }
    _TYPE_ x12 = x1 + x2;
    _TYPE_ y12 = y1 + y2;
    _TYPE_ x23 = x2 + x3;
    _TYPE_ y23 = y2 + y3;
    _TYPE_ x34 = x3 + x4;
    _TYPE_ y34 = y3 + y4;

    _TYPE_ x1223 = x12 + x23;
    _TYPE_ y1223 = y12 + y23;
    _TYPE_ x2334 = x23 + x34;
    _TYPE_ y2334 = y23 + y34;

    _TYPE_ x = x1223 + x2334;
    _TYPE_ y = y1223 + y2334;

    PathBezier(x1, y1, x12 / 2.0f, y12 / 2.0f, x1223 / 4.0f, y1223 / 4.0f, x / 8.0f, y / 8.0f, pt);
    PathBezier(x / 8.0f, y / 8.0f, x2334 / 4.0f, y2334 / 4.0f, x34 / 2.0f, y34 / 2.0f, x4, y4, pt);
}
template<class T, class _TYPE_>
void AproxLine<T, _TYPE_>::PathBezier(_TYPE_ x1, _TYPE_ y1, _TYPE_ x2, _TYPE_ y2, _TYPE_ x3, _TYPE_ y3, MArray<T>& pt)
{
    _TYPE_  A = y3 - y1;
    _TYPE_  B = x1 - x3;
    _TYPE_  C = y1 * (x3 - x1) - x1 * (y3 - y1);
    _TYPE_  AB = A * A + B * B;
    if (AB <= 1.0e-10)
    {
        T &p = pt.Add();
        p.x = (_TYPE_)x3;
        p.y = (_TYPE_)y3;
        return;
    }
    if ((A * x2 + B * y2 + C) * (A * x2 + B * y2 + C) < AB)
    {
        T &p = pt.Add();
        p.x = (_TYPE_)x3;
        p.y = (_TYPE_)y3;
        return;
    }
    _TYPE_  x12 = x1 + x2;
    _TYPE_  y12 = y1 + y2;
    _TYPE_  x23 = x2 + x3;
    _TYPE_  y23 = y2 + y3;
    _TYPE_  x1223 = x12 + x23;
    _TYPE_  y1223 = y12 + y23;
    PathBezier(x1, y1, x12 / 2, y12 / 2, x1223 / 4, y1223 / 4, pt);
    PathBezier(x1223 / 4, y1223 / 4, x23 / 2, y23 / 2, x3, y3, pt);

}

#endif
