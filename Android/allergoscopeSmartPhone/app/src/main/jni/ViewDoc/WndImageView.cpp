//
// Created by alex on 25.06.2019.
//


#include "stdafx.h"
#include "WndImageView.h"
#include "App.h"
#include "variable.h"

//------------------------------------------
//
//------------------------------------------
WndImageView::WndImageView()
{
    startAngle = 0;
    fFirst = TRUE;
}

//------------------------------------------
//
//------------------------------------------
WndImageView::~WndImageView()
{
}

//------------------------------------------
//
//------------------------------------------
void WndImageView::OnSize(UINT nType, int cx, int cy)
{

    CRect rc1;
    CRect rc2;
    GetClientRect(&rc2);
    if (!img.GetPtr())
        rc1 = rc2;
    else
    {
        rcOrig.Set(0, 0, bmp.Width(), bmp.Height());
        rc1 = rcOrig;
    }
    if (fFirst)
    {
        fFirst = FALSE;
        sec.Enter();
        trans.Init(rc2, rc1);
        sec.Leave();

    }
    AWnd::OnSize(nType, cx, cy);

}

void WndImageView::Clear()
{
    ADC *dc;
    if ((dc = GetDC()))
    {
        MemImage &src = img;
        int w = src.info.width;
        int h = src.info.height;
        if (w && h)
        {
            GDC screen = dc->m_hDC;
            info = screen->info;
            CRect rect(0, 0, info.width, info.height);
            CRect rcDes(0, 0, w, h);
            CRect rc1 = rect;
            screen->SetClipBox(rect);
            screen->SetBgColor(0);
            screen->Clear();

        }
        ReleaseDC(dc);
    }
}

//------------------------------------------
//
//------------------------------------------
void WndImageView::OnPaint(int fWait)
{
    ADC *dc;
    if (GetView() && (dc = GetDC()))
    {
        if (!fWait)
        {
            MemImage &src = img;
            int w = src.info.width;
            int h = src.info.height;
            if (w && h)
            {
                GDC screen = dc->m_hDC;
                info = screen->info;
                CRect rect(0, 0, info.width, info.height);
                CRect rcDes(0, 0, w, h);
                CRect rc1 = rect;
                screen->SetClipBox(rect);
                screen->SetBgColor(0);
                screen->Clear();
                sec.Enter();
                Transform2F<CPoint, RECT> _trans = trans;
                sec.Leave();
                if (img.GetPtr())
                {
                    CRect rc = rc1;
                    _trans.DPtoLP((CPoint *) &rc, 2);
                    rc = rc & rcDes;
                    rect = rc;
                    if (!rect.IsEmpty())
                    {
                        _trans.LPtoDP((CPoint *) &rect, 2);
                        screen->Stretch(img, rc, rect);

                    }
                    if (roi.Len())
                    {
                        RBMap<MVector<MArray<Vector2F> >, TString>::iterator it(roi);
                        for (RBData<MVector<MArray<Vector2F> >, TString> *b = it.begin(); b; b = it.next())
                        {
                            if (enable_roi[b->key.GetPtr()])
                            {
                                FGColor fg(PS_SOLID, 2, colors[b->key.GetPtr()]);
                                GPEN pen = screen->SelectObject(&fg);
                                MVector<MArray<Vector2F> > &buf = b->value;
                                for (int j = 0, l = buf.GetLen(); j < l; ++j)
                                {
                                    MArray<Vector2F> &data = buf[j];
                                    int len = data.GetLen();
                                    Vector2F *ptr = data.GetPtr();
                                    MArray<IPoint> res(len);
                                    IPoint *pres = res.GetPtr();

                                    while (len--)
                                    {

                                        float _x = (float) ptr->x;
                                        pres->x = (int) (trans.form.m11ScaleX * ptr->x +
                                                         trans.form.m12ShiftX * ptr->y +
                                                         trans.form.m13Dx);
                                        pres->y = (int) (trans.form.m22ScaleY * ptr->y +
                                                         trans.form.m21ShiftY * _x +
                                                         trans.form.m23Dy);
                                        ++ptr;
                                        ++pres;

                                    }
                                    screen->PolyLine(res.GetPtr(), res.GetLen());

                                }
                                screen->SelectObject(pen);

                            }
                        }
                        screen->SelectObject(pen);
                    }
                }
            }
        } else
        {
            CRect rc;
            dc->GetClipBox(&rc);
            GDC gdc = *dc;
            IPoint pt = rc.CenterPoint();
            int r1 = rc.Height() / 8;
            Vector2D c(pt), nul(pt.x, pt.y - r1);
            double angle = startAngle * PI / 180.0;
            double SinA = -sin(angle);
            double CosA = cos(angle);
            double x = nul.x - c.x;
            double y = nul.y - c.y;
            nul.x = ((x * CosA + y * SinA) + c.x);
            nul.y = ((y * CosA - x * SinA) + c.y);
            nul -= c;
            Circle2D c1(c, r1);
            Circle2D c2(c, r1 - r1 / 8);
            int X = pt.x + r1;
            double g;
            DWORD *dw = gdc->GetPtr();
            ColorRef cl;
            cl.a = 255;
            for (int y = pt.y - r1, Y = pt.y + r1; y <= Y; ++y)
                for (int x = pt.x - r1; x <= X; ++x)
                {
                    Vector2D d(x, y), p;
                    if (c1.PtInCircle2D(d))
                        if (!c2.PtInCircle2D(d))
                        {
                            p = d - c;
                            g = nul.PolarAngle() - p.PolarAngle();
                            if (g < 0) g += 360.0f;
                            cl.r = (BYTE) (g * 255 / 360);
                            if (cl.r < 32) cl.r = 32;
                            cl.b = cl.g = cl.r;
                            dw[x + y * gdc->info.width] = cl.color;
                        }
                }
            startAngle += 5;
            if (startAngle > 360)
                startAngle = 0;
            ReleaseDC(dc);
        }
    }
}

//------------------------------------------
//
//------------------------------------------
void WndImageView::OnLoad(jobject _bmp)
{
    img.Detach();
    roi.Clear();
    enable_roi.Clear();
    if (_bmp)
    {
        bmp.Create(_bmp, AObject::TJGLOBAL);
        DWORD *ptr = bmp.SetAccess();
        if (ptr)
        {
            ImageInfo info(bmp.Width(), bmp.Height(), bmp.Width());
            img.Init(ptr, info);
        }
        CRect rc1;
        GetClientRect(&rc1);
        fFirst = true;
        OnSize(0, rc1.Width(), rc1.Height());
    } else
    {
        bmp.Clear();
        startAngle = 0;
        fFirst = TRUE;
    }
}

//------------------------------------------
//
//------------------------------------------
void WndImageView::OnRoi(TString &filename, bool fView)
{
    Variable var;
    MFILE file(filename.GetPtr());
    if (file.Open(false))
    {
        MArray<BYTE> data;
        int l = file.GetSize();
        data.SetLen(l);
        file.Read(0, data.GetPtr(), l);
        var.UnSerialize(data);
        FFileINI ini("roi.ini", true);
        for (int i = 0, l = var.Dim(); i < l; ++i)
        {
            Variable &v = var[i];
            int j = v.Dim();
            TString str = v[0];

            for (int k = 1; k < j; ++k)
            {
                MArray<BYTE> *pt = v[k].GetArray();
                roi[str.GetPtr()].Add().Copy((Vector2F *) pt->GetPtr(),
                                             pt->GetLen() / sizeof(Vector2F));
                enable_roi[str.GetPtr()] = fView;

            }
            TString str2 = str;
            str.Upper();
            str.ReplaceAll(" ", "_");
            colors[str2.GetPtr()] = (COLORREF) ini.GetInt("COLORS", str.GetPtr(), RGB(0, 255, 0));

        }
    }
}

//------------------------------------------
//
//------------------------------------------
BOOL WndImageView::UpdateRoi(JNIEnv *env, jobject obj)
{
    JVariable ret;
    MArray<bool> sel;
    MVector<TString> name;
    MArray<int> clr;
    AObject o;
    int l = roi.Len();
    if (o.Attach(obj, AObject::TJLOCAL, env))
    {

        o.Run("fSel", 0, 0, ret, env);
        ret.Get(sel);
        ret.Clear();
        o.Run("names", 0, 0, ret, env);
        ret.Get(name);
        ret.Clear();
        o.Run("colors", 0, 0, ret, env);
        ret.Get(clr);
        ret.Clear();
        o.Detach();
        if (sel.GetLen() != l || name.GetLen() != l || clr.GetLen() != l) return 0;
        FFileINI ini("roi.ini", true);
        for (int i = 0; i < l; ++i)
        {
            LPCSTR nm = name[i].GetPtr();
            if (!nm) return 0;
            enable_roi[nm] = sel[i];
            ColorRef c;
            c.color = clr[i];
            colors[nm] = RGB(c.b, c.g, c.r);
            TString str = nm;
            str.Upper();
            str.ReplaceAll(" ", "_");
            ini.WriteInt("COLORS", str.GetPtr(), (int) RGB(c.b, c.g, c.r));
        }
        return 1;

    }
    return 0;
}

//------------------------------------------
//
//------------------------------------------
BOOL WndImageView::UpdateRoiIcon(JBitmap &_bmp, LPCSTR name, ColorRef &clr)
{
    int width = _bmp.Width();
    int height = _bmp.Height();
    CRect rc(0, 0, width, height);
    rc.Set(2, 5, width - 5, height - 2);

    MemImage _img;

    Transform2F<CPoint, RECT> trans;
    ImageInfo info(width, height, width);
    DWORD *DW = (DWORD *) _bmp.SetAccess();
    if (DW)
    {
        _img.Init(DW, info);
        _img.Clear(0x00000000);
        FGColor fg(PS_SOLID, 2, clr.color);
        GPEN pen = _img.SelectObject(&fg);
        MVector<MArray<Vector2F> > &buf = roi[name];
        CRect rc_src(INT16_MAX, INT16_MAX, 0, 0);
        int j = 0, l = buf.GetLen();
        for (; j < l; ++j)
        {

            MArray<Vector2F> &data = buf[j];
            int len = data.GetLen();
            Vector2F *ptr = data.GetPtr();
            while (len--)
            {

                rc_src.left = min((int) ptr->x, rc_src.left);
                rc_src.right = max((int) ptr->x, rc_src.right);
                rc_src.top = min((int) ptr->y, rc_src.top);
                rc_src.bottom = max((int) ptr->y, rc_src.bottom);
                ++ptr;

            }

        }
        CRect src(-10, -10, rc_src.Width() + 10, rc_src.Height() + 10);
        trans.Init(rc, rc_src, true);
        CRect rc_dst = rc_src;
        trans.LPtoDP((CPoint *) &rc_dst, 2);

        _img.Stretch(img, rc_src, rc_dst);
        j = 0;
        for (; j < l; ++j)
        {

            MArray<Vector2F> &data = buf[j];
            int len = data.GetLen();
            Vector2F *ptr = data.GetPtr();
            MArray<IPoint> res(len);
            IPoint *pres = res.GetPtr();

            while (len--)
            {


                float _x = (float) ptr->x;
                pres->x = (int) (trans.form.m11ScaleX * ptr->x + trans.form.m12ShiftX * ptr->y +
                                 trans.form.m13Dx);
                pres->y = (int) (trans.form.m22ScaleY * ptr->y + trans.form.m21ShiftY * _x +
                                 trans.form.m23Dy);
                ++ptr;
                ++pres;

            }
            _img.PolyLine(res.GetPtr(), res.GetLen());

        }

        _img.SelectObject(pen);
        _img.Detach();
        _bmp.ReleaseAccess();
        return true;
    }
    return false;
}

//------------------------------------------
//
//------------------------------------------
jobject WndImageView::GetRoi(JNIEnv *env, int width, int height)
{
    jobject result = 0;
    int l;
    if ((l = roi.Len()))
    {

        width /= roi.Len();
        AObject obj;
        JPtr jsel = env->NewBooleanArray(l);
        if (MInstance::ClearException(env)) return 0;
        jboolean *sel = env->GetBooleanArrayElements(jsel.boolarr, 0);
        JPtr clazz = MInstance::FindClass("android/graphics/Bitmap");
        JPtr jbmp = env->NewObjectArray(l, clazz.cls, 0);
        if (MInstance::ClearException(env)) return 0;
        JPtr tp = env->FindClass("java/lang/String");
        if (MInstance::ClearException(env)) return 0;
        JPtr jnames = env->NewObjectArray(l, tp.cls, 0);
        if (MInstance::ClearException(env)) return 0;
        JPtr jcolors = env->NewIntArray(l);
        if (MInstance::ClearException(env)) return 0;
        jint *color = env->GetIntArrayElements(jcolors.intarr, 0);
        int i = 0;
        CRect rc(0, 0, width, height);
        rc.Set(2, 5, width - 5, height - 2);
        RBMap<TString, int> ind;
        {
            RBMap<MVector<MArray<Vector2F> >, TString>::iterator it(roi);
            for (RBData<MVector<MArray<Vector2F> >, TString> *b = it.begin(); b; b = it.next())
            {
                int X = INT16_MAX;
                MVector<MArray<Vector2F> > &buf = b->value;
                for (int j = 0, l = buf.GetLen(); j < l; ++j)
                {
                    MArray<Vector2F> &data = buf[j];
                    int len = data.GetLen();
                    Vector2F *ptr = data.GetPtr();
                    while (len--)
                    {

                        X = min((int) ptr->x, X);
                        ++ptr;
                    }
                }
                ind[X] = b->key;
            }
        }
        RBMap<TString, int>::iterator it(ind);
        for (RBData<TString, int> *b = it.begin(); b; b = it.next(), ++i)
        {
            LPCSTR name = b->value.GetPtr();
            sel[i] = (jboolean) enable_roi[name];
            ColorRef clr;
            clr.color = colors[name] | 0xFF000000;
            color[i] = (int) (RGB(clr.b, clr.g, clr.r) | 0xFF000000);
            JPtr s = env->NewStringUTF(name);
            env->SetObjectArrayElement(jnames, i, s);
            JBitmap _bmp;
            _bmp.Create(width, height, AObject::TJLOCAL);
            if (!UpdateRoiIcon(_bmp, name, clr)) return 0;
            env->SetObjectArrayElement(jbmp, i, _bmp.Get_jobject());
            _bmp.Clear(false);
        }
/*
        RBMap<MVector<MArray<Vector2F> >, StringUtils >::iterator it(roi);
        for (RBData<MVector<MArray<Vector2F> >, StringUtils > *b = it.begin(); b; b = it.next(), ++i)
        {
            LPCSTR name = b->key.GetPtr();
            sel[i] = (jboolean)enable_roi[name];
            ColorRef clr;
            clr.color = colors[b->key.GetPtr()] | 0xFF000000;
            color[i] = (int)(RGB(clr.b, clr.g, clr.r) | 0xFF000000);

            JPtr s = env->NewStringUTF(name);
            env->SetObjectArrayElement(jnames, i, s);

            JBitmap _bmp;
           _bmp.Create(width, height, AObject::TJLOCAL);
            if (!UpdateRoiIcon(_bmp, name, clr)) return 0;
            env->SetObjectArrayElement(jbmp, i, _bmp.Get_jobject());
            _bmp.Clear(false);

        }
        */
        env->ReleaseBooleanArrayElements(jsel.boolarr, sel, 0);
        env->ReleaseIntArrayElements(jcolors.intarr, color, 0);
        AObject o;
        JVariable variable[7];
        variable[0].Attach(jbmp.obj);
        variable[1].Attach(jsel.obj);
        variable[2].Attach(jnames.obj);
        variable[3].Attach(jcolors.obj);
        o.New(STR_ADDSECTLIST, variable, 4, AObject::TJLOCAL);
        variable[0].Detach();
        variable[1].Detach();
        variable[2].Detach();
        variable[3].Detach();
        result = o.obj;
        o.Detach();

    }
    return result;
}

