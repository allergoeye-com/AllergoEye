/*
 * EmbGDISurf.cpp
 *
 *  Created on: Sep 7, 2014
 *      Author: alex
 */
#include "stdafx.h"
#include "EmbGDISurf.h"
#include "AClip.h"
#include "ILine.h"

void EmbGDISurf::XPolyLine3D(IPoint *pt, int len, COLORREF c, int d)
{

    if (!ptr || !pt) return;
    IRect rc(0, 0, info.height, info.width);
    pen->color = 0xFF000000 | c;
    if (len < 2)
    {
        SetPixel(pt->x + curOrg.x, pt->y + curOrg.x, pen->color);
        return;
    }
    if ((rc & clip).IsEmpty())
        return;
    MArray<IPoint> apt;
    apt.Copy(pt, len);
    pt = apt.GetPtr();
    IRect rc1(pt[0].x + curOrg.x, pt[0].y + curOrg.y, pt[0].x + curOrg.x, pt[0].y + curOrg.y);
    for (int i = 0; i < len; ++i)
    {
        pt[i].x += curOrg.x;
        pt[i].y += curOrg.y;
        rc1.Update(pt[i].x, pt[i].y);

    }
    if ((rc1 & clip) == rc1)
    {
        //_PolyLine(pt, len);
        _XPolyLine3D(pt, len, c, d);
        return;
    }
    DWClipLine<IPoint, int> clipL;
    clipL.Init(clip, 1);
//	--clipL.right;
//	--clipL.bottom;

    int n, num = 0;
    IPoint dst[3], tmp[2];

    BYTE fContinue = 0, prev_b = 0, next_b = 0, cur_b = (((BYTE) ((pt->x < clipL.left) << 3) & 8) |
                                                         ((BYTE) ((pt->y < clipL.top) << 2) & 4) |
                                                         ((BYTE) ((pt->x > clipL.right) << 1) & 2) |
                                                         ((BYTE) ((pt->y > clipL.bottom)) & 1));
    bool fFirst = 1;

    while (len--)
    {
        if (len)
        {
            next_b = ((BYTE) (((pt + 1)->x < clipL.left) << 3) & 8) |
                     ((BYTE) (((pt + 1)->y < clipL.top) << 2) & 4) |
                     ((BYTE) (((pt + 1)->x > clipL.right) << 1) & 2) |
                     ((BYTE) (((pt + 1)->y > clipL.bottom)) & 1);
            if (prev_b == cur_b)
                fContinue = cur_b & next_b;
        }
        if (!fContinue)
        {
            if (!prev_b && !cur_b && !next_b)
            {
                *(dst + num) = *pt;
                if (!(num && (dst + num)->x == (dst + num - 1)->x &&
                      (dst + num)->y == (dst + num - 1)->y))
                    ++num;
            } else
            {
                if (!fFirst)
                {

                    if (!prev_b && !cur_b)
                    {
                        *(dst + num) = *pt;
                        if (!(num && (dst + num)->x == (dst + num - 1)->x &&
                              (dst + num)->y == (dst + num - 1)->y))
                            ++num;

                    } else
                    {
                        if (!(prev_b & cur_b))
                        {
                            if (num == 1)
                                num = 0;
                            tmp[0] = *(pt - 1);
                            tmp[1] = *pt;
                            if (clipL.ClipBarsk(tmp[0], tmp[1]))
                            {
                                *(dst + num) = *tmp;
                                ++num;
                                *(dst + num) = *(tmp + 1);
                                ++num;

                            }

                        }
                    }
                    if (num > 1)
                    {
                        //_PolyLine(dst, num);
                        _XPolyLine3D(dst, num, c, d);
                        n = num;
                        if ((num = (!cur_b && len)))
                        {
                            dst[0] = dst[n - 1];
                        }
                    }

                }
            }
        }
        if (num > 1)
        {
            _XPolyLine3D(dst, num, c, d);
            //_PolyLine(dst, num);
            dst[0] = dst[num - 1];
            num = 1;
        }
        fFirst = 0;
        prev_b = cur_b;
        cur_b = next_b;
        fContinue = 0;
        next_b = 64;
        ++pt;
    }


}

void EmbGDISurf::XLine3D(int x1, int y1, int x2, int y2, COLORREF c, int d)
{

    int x01 = x1 - xOff;
    int x02 = x2 - xOff;

    if ((x01 < 0 && x02 < 0) || (x01 > info.width && x02 > info.width))
        return;

    int y01 = y1 - yOff;
    int y02 = y2 - yOff;

    if ((y01 < 0 && y02 < 0) || (y01 > info.height && y02 > info.height))
        return;

    int muli = 10000;

    int l = d2i(hypotf(x2 - x1, y2 - y1));
    if (l <= 0)
        return;

    int drawd = d;
    int nhorz = l;//d2i(hypot(x2-x1,y2-y1)/2.0);//12;
    if (MAXHORZ > 0 && nhorz > MAXHORZ)
        nhorz = MAXHORZ;
    if (nhorz < 2)
        nhorz = 2;
///	int p40 = 8000; //0.8*muli
    double p40 = 0.8;
    int cet = iCalcCet(x1, y1, x2, y2, l, nhorz);
    int devx = x2 - x1;
    int devy = y2 - y1;
    for (int dd = drawd; dd > 0; --dd)
    {
        int ox = x1, oy = y1;
        for (int i = 1; i <= nhorz; i++)
        {
            int xx = x1 + (devx) * i / nhorz;
            int yy = y1 + (devy) * i / nhorz;

            int ll;
            int cetm = max(cet, nhorz - cet);
            int ii;
            if (i <= cet) ii = i; else ii = cet - (i - 1 - cet);
            ll = muli - muli * abs(cet - ii) / cetm;

            ll = maxe + (maxc - maxe) * ll / muli;

            double ci = drawd > 1 ? p40 + (1.0 - p40) * (dd - drawd) / (1.0 - drawd) : 1.0;
            COLORREF cc = CalcColor(c, ci * ll);

            XLine(ox, oy, xx, yy, cc, dd);
            ox = xx;
            oy = yy;

        }
    }

}


void EmbGDISurf::XLine(int x1, int y1, int x2, int y2, COLORREF c, int d)
{
///	d=3;
    x1 -= xOff;
    y1 -= yOff;
    x2 -= xOff;
    y2 -= yOff;
    if (d <= 1)
    {
        ILine bline;
        bline.Init(x1, y1, x2, y2);
        bline.Draw((DWORD *) _scrb, info.width, info.height, c | 0xFF000000);
    } else
    {
#if 1    //0=>4point polys 1=>6 point polys
        int x[6], y[6], dx = x2 - x1, dy = y2 - y1;

        int xx1[4], yy1[4], xx2[4], yy2[4];

        calcxy(x1, y1, xx1, yy1, d, dx, dy);
        calcxy(x2, y2, xx2, yy2, d, dx, dy);

#define SP(i, p, j) x[i]=xx##p[j];y[i]=yy##p[j];
        if (dx == 0)
        {
            if (dy > 0)
            {
                SP(0, 1, 1);
                SP(1, 1, 2);
                SP(2, 2, 3);
                SP(3, 2, 0);
            } else if (dy < 0)
            {
                SP(0, 1, 0);
                SP(1, 2, 1);
                SP(2, 2, 2);
                SP(3, 1, 3);
            } else
            {
                zerozero:
                SP(0, 1, 0);
                SP(1, 1, 1);
                SP(2, 1, 2);
                SP(3, 1, 3);
            }
            polypix(_scrb, 4, x, y, c);
            return;
        } else if (dy == 0)
        {
            if (dx > 0)
            {
                SP(0, 1, 0);
                SP(1, 1, 1);
                SP(2, 2, 2);
                SP(3, 2, 3);
            } else if (dx < 0)
            {
                SP(0, 2, 0);
                SP(1, 2, 1);
                SP(2, 1, 2);
                SP(3, 1, 3);
            } else
            {
                goto zerozero;
            }
            polypix(_scrb, 4, x, y, c);
            return;
        } else if (dx > 0 && dy < 0)
        {
            SP(0, 1, 0);
            SP(1, 1, 1);
            SP(2, 2, 1);
            SP(3, 2, 2);
            SP(4, 2, 3);
            SP(5, 1, 3);
        } else if (dx > 0 && dy > 0)
        {
            SP(0, 1, 1);
            SP(1, 1, 2);
            SP(2, 2, 2);
            SP(3, 2, 3);
            SP(4, 2, 0);
            SP(5, 1, 0);
        } else if (dx < 0 && dy < 0)
        {
            SP(0, 2, 1);
            SP(1, 2, 2);
            SP(2, 1, 2);
            SP(3, 1, 3);
            SP(4, 1, 0);
            SP(5, 2, 0);
        } else if (dx < 0 && dy > 0)
        {
            SP(0, 2, 0);
            SP(1, 2, 1);
            SP(2, 1, 1);
            SP(3, 1, 2);
            SP(4, 1, 3);
            SP(5, 2, 3);
        }
#undef SP
        polypix(_scrb, 6, x, y, c);
        return;
#else
        int xi[4], yi[4];
        float x[4], y[4], r=d/2.0f, dx=float(x2-x1), dy=float(y2-y1);
        float R = float(sqrt(dx*dx+dy*dy));

        float f=1000.0f;
        R*=f;
        r*=f;

        x[0]=0.0f; y[0]=r;
        x[1]=0.0f; y[1]=-r;
        x[2]=R;    y[2]=-r;
        x[3]=R;    y[3]=r;

        float a = (float)-atan2(dy,dx), sn=(float)sin(a), cs=(float)cos(a);

        for(int i=0; i<4; i++){
            xi[i] = x1+f2i((+ x[i]*cs + y[i]*sn)/f);
            yi[i] = y1+f2i((- x[i]*sn + y[i]*cs)/f);
        }

        polypix(_scrb,4,xi,yi,c);
#endif
    }
}

void EmbGDISurf::polypix(DWORD *dc, int n, int *x, int *y, COLORREF color)
{
    int i, minx, maxx, maxy, miny, *yp, *xp;
    yp = y;
    xp = x;
    maxy = miny = *yp++;
    maxx = minx = *xp++;
    for (i = 1; i < n; i++)
    {
        if (*yp > maxy)maxy = *yp;
        if (*yp < miny)miny = *yp;
        if (*xp > maxx)maxx = *xp;
        if (*xp < minx)minx = *xp;
        xp++;
        yp++;
    }
    if (maxy - miny <= maxx - minx)
    {
        if (maxy >= info.height)maxy = info.height - 1;
        if (miny < 0) miny = 0;
        for (int iy = miny; iy <= maxy; iy++)
        {
            minx = info.width;
            maxx = -1;
            int iv = n - 1;
            for (int nv = 0; nv < n; nv++)
            {
                if (max(y[iv], y[nv]) >= iy &&
                    min(y[iv], y[nv]) <= iy &&
                    y[iv] != y[nv])
                {
#if 1
#define RMUmul  (iy-y[iv])
#define RMUdiv  (y[nv]-y[iv])
                    int ixi = x[iv] + ((x[nv] - x[iv]) * RMUmul + RMUdiv / 2) / RMUdiv;
#undef RMUmul
#undef RMUdiv
#else
                    float RMU = float(iy-y[iv])/float(y[nv]-y[iv]);
                    int   ixi = int(x[iv]+RMU*(x[nv]-x[iv]));
#endif
                    if (ixi < minx) minx = ixi;
                    if (ixi > maxx) maxx = ixi;
                }
                iv = nv;
            }
            if (maxx >= info.width)maxx = info.width - 1;
            if (minx < 0)minx = 0;
            if (minx <= maxx)
            {
                if (dc != 0)
                {
                    DWORD *p = minx ? plot(dc,minx,iy,color) : dc + iy * info.width + minx;
//                    DWORD *p = plot(dc,minx,iy,color);
                    plot(p, color, maxx - minx);
                }


            }
        }
    } else
    {
        if (maxx >= info.width)maxx = info.width - 1;
        if (minx < 0) minx = 0;
        for (int ix = minx; ix <= maxx; ix++)
        {
            miny = info.height;
            maxy = -1;
            int ih = n - 1;
            for (int nh = 0; nh < n; nh++)
            {
                if (max(x[ih], x[nh]) >= ix &&
                    min(x[ih], x[nh]) <= ix &&
                    x[ih] != x[nh])
                {
#if 1
#define RMUmul (ix-x[ih])
#define RMUdiv (x[nh]-x[ih])
                    int iyi = y[ih] + ((y[nh] - y[ih]) * RMUmul + RMUdiv / 2) / RMUdiv;
#undef RMUmul
#undef RMUdiv
#else
                    float RMU = float(ix-x[ih])/float(x[nh]-x[ih]);
                    int   iyi = int(y[ih]+RMU*(y[nh]-y[ih]));
#endif
                    if (iyi < miny) miny = iyi;
                    if (iyi > maxy) maxy = iyi;
                }
                ih = nh;
            }
            if (maxy >= info.height)maxy = info.height - 1;
            if (miny < 0)miny = 0;
            if (miny <= maxy)
            {
                DWORD *p = miny ? plot(dc,ix,miny,color) : dc + miny * info.width + ix;
                for(int ppp=miny+1;ppp<=maxy;ppp++)
                {
                    plot(p,color);
                    p -= info.width;
                }
//                DWORD *p = dc + miny * info.width + ix; //plot(dc,ix,miny,color);
  //              for (int ppp = miny + 1; ppp <= maxy; ppp++)
    //            {
      //              for (int ppp = miny; ppp <= maxy; ++ppp)
        //            {
          //              plot(p, color);
            //            p += info.width;
              //      }
                //}

            }
        }
    }

#if (0)
    int i, minx, maxx, maxy, miny, *yp, *xp;
    yp=y; xp=x;
    maxy = miny = *yp++;
    maxx = minx = *xp++;
    for(i=1; i<n; i++){
        if(*yp>maxy)maxy=*yp;
        if(*yp<miny)miny=*yp;
        if(*xp>maxx)maxx=*xp;
        if(*xp<minx)minx=*xp;
        xp++;
        yp++;
    }
    if(maxy-miny <= maxx-minx){
        if(maxy>=info.height)maxy=info.height-1;
        if(miny<0)    miny=0;
        for(int iy=miny; iy<=maxy; iy++){
            minx = info.width;
            maxx = -1;
            int iv   = n-1;
            for(int nv=0; nv<n; nv++){
                if( max(y[iv],y[nv])>=iy &&
                    min(y[iv],y[nv])<=iy &&
                    y[iv] != y[nv] ){
#if 1
#define RMUmul  (iy-y[iv])
#define RMUdiv  (y[nv]-y[iv])
                    int ixi	= x[iv]+((x[nv]-x[iv])*RMUmul+RMUdiv/2)/RMUdiv;
#undef RMUmul
#undef RMUdiv
#else
                    float RMU = float(iy-y[iv])/float(y[nv]-y[iv]);
                    int   ixi = int(x[iv]+RMU*(x[nv]-x[iv]));
#endif
                    if(ixi<minx) minx=ixi;
                    if(ixi>maxx) maxx=ixi;
                }
                iv=nv;
            }
            if(maxx>=info.width)maxx=info.width-1;
            if(minx<0)minx=0;
            if(minx<=maxx)
                    plot(dc +  iy*info.width + minx,color,maxx-minx + 1);

        }
    }else{
        if(maxx>=info.width)maxx=info.width-1;
        if(minx<0)    minx=0;
        for(int ix=minx; ix<=maxx; ix++){
            miny = info.height;
            maxy = -1;
            int ih   = n-1;
            for(int nh=0; nh<n; nh++){
                if( max(x[ih],x[nh])>=ix &&
                    min(x[ih],x[nh])<=ix &&
                    x[ih] != x[nh] ){
#if 1
#define RMUmul (ix-x[ih])
#define RMUdiv (x[nh]-x[ih])
                    int   iyi = y[ih]+((y[nh]-y[ih])*RMUmul+RMUdiv/2)/RMUdiv;
#undef RMUmul
#undef RMUdiv
#else
                    float RMU = float(ix-x[ih])/float(x[nh]-x[ih]);
                    int   iyi = int(y[ih]+RMU*(y[nh]-y[ih]));
#endif
                    if(iyi<miny) miny=iyi;
                    if(iyi>maxy) maxy=iyi;
                }
                ih=nh;
            }
            if(maxy>=info.height)maxy=info.height-1;
            if(miny<0)miny=0;
            if(miny<=maxy){
                DWORD *p = dc +  miny*info.width + ix; //plot(dc,ix,miny,color);
                for(int ppp=miny;ppp<=maxy;++ppp)
                {
                    plot(p,color);
                    p += info.width;
                }

            }
        }
    }
#endif
}

int EmbGDISurf::SetLight(int red, int green, int blue, double intens)
{
    double y0 = (((intens - 100.0) * 255.0) / 100.0 + (intens >= 100 ? 0.5 : -0.5));
    if (intens >= 100.0)
    {
        double yy = 255.0 - y0;
        red = int(y0 + (128 + yy * red) / 255);
        green = int(y0 + (128 + yy * green) / 255.0);
        blue = int(y0 + (128 + yy * blue) / 255.0);
    } else
    {
        double yy = 255.0 + y0;    //y0<0 here
        red = int((128.0 + yy * red) / 255.0);
        green = int((128.0 + yy * green) / 255.0);
        blue = int((128.0 + yy * blue) / 255.0);
    }

    return RGB(red, green, blue);
}
