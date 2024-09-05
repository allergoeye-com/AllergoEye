
#include "stdafx.h"
#include "App.h"

#pragma pack(push)
#pragma pack(0)

#include "com_dryeye_app_CView.h"

#pragma pack(pop)

/*
 * Class:     com_dryeye_app_CView
 * Method:    ScalePt
 * Signature: (DFF)Z
 */
extern "C" JNIEXPORT jboolean JNICALL Java_com_dryeye_app_CView_ScalePt
        (JNIEnv *, jobject, jdouble scale, jfloat x, jfloat y)
{
    WndImageView *view = theApp->Dlg;
    if (!view) return 0;
    CRect r;
    view->GetClientRect(r);
    Transform2F<CPoint, RECT> trans = view->trans;
    CPoint c((int) x, (int) y);

    trans.Scale((float) scale, r, &c);
    if (trans.form.m11ScaleX < 0.1 || trans.form.m11ScaleX > 10) return 0;
    view->sec.Enter();
    view->trans = trans;
    view->sec.Leave();
    return 1;
}
/*
 * Class:     com_dryeye_app_CView
 * Method:    Offset
 * Signature: (FF)Z
 */
extern "C" JNIEXPORT jboolean JNICALL Java_com_dryeye_app_CView_Offset
        (JNIEnv *, jobject, jfloat dx, jfloat dy)
{
    WndImageView *view = theApp->Dlg;
    if (!view) return 0;

    view->sec.Enter();
    view->trans.Offset(dx, dy);
    view->sec.Leave();
    return 1;
}

/*
 * Class:     com_dryeye_app_CView
 * Method:    OnMouseDown
 * Signature: (FF)V
 */
extern "C" JNIEXPORT void JNICALL Java_com_dryeye_app_CView_OnMouseDown
        (JNIEnv *, jobject, jfloat, jfloat)
{
}

/*
 * Class:     com_dryeye_app_CView
 * Method:    OnMouseUp
 * Signature: (FF)V
 */
extern "C" JNIEXPORT void JNICALL Java_com_dryeye_app_CView_OnMouseUp
        (JNIEnv *, jobject, jfloat, jfloat)
{
}


/*
 * Class:     com_dryeye_app_CView
 * Method:    OnOpenDocument
 * Signature: (Landroid/graphics/Bitmap;)Z
 */
extern "C" JNIEXPORT jboolean JNICALL Java_com_dryeye_app_CView_OnOpenDocument
        (JNIEnv *env, jobject, jobject bmp)
{
    WndImageView *view = theApp->Dlg;
    if (!view) return 0;
    view->OnLoad(bmp);
    return 1;
}

/*
 * Class:     com_dryeye_app_CView
 * Method:    OnSetSize
 * Signature: (II)Landroid/graphics/Bitmap;
 */
extern "C" JNIEXPORT jobject JNICALL Java_com_dryeye_app_CView_OnSetSize
        (JNIEnv *, jobject, jint w, jint h)
{
    IRect rc;
    theApp->Dlg->GetClientRect(&rc);
  //  if (w != rc.Width() || h != rc.Height())
        theApp->Dlg->SetSize(w, h);
    JBITMAP bmp = theApp->Dlg->GetSurface();
    return bmp ? bmp->Get_jobject() : 0;
}

/*
 * Class:     com_dryeye_app_CView
 * Method:    OnPaint
 * Signature: ()V
 */
extern "C" JNIEXPORT void JNICALL Java_com_dryeye_app_CView_OnPaint
        (JNIEnv *, jobject)
{
    WndImageView *view = theApp->Dlg;
    if (!view) return;
    view->OnPaint(0);


}

/*
 * Class:     com_dryeye_app_CView
 * Method:    OnOpenRoi
 * Signature: (Ljava/lang/String;Z)Z
 */
extern "C" JNIEXPORT jboolean JNICALL Java_com_dryeye_app_CView_OnOpenRoi
        (JNIEnv *, jobject, jstring pathname, jboolean fView)
{
    WndImageView *view = theApp->Dlg;
    if (!view) return false;

    JPtr tmp;
    tmp = pathname;
    TString spathname = tmp.String();
    view->OnRoi(spathname, fView);
    return true;
}
/*
 * Class:     com_dryeye_app_CView
 * Method:    GetRoi
 * Signature: (II)Lcom//app/roi/SelectedBlockIcon;
 */
extern "C" JNIEXPORT jobject JNICALL Java_com_dryeye_app_CView_GetRoi
        (JNIEnv *env, jobject, jint xsize, jint size)
{
    WndImageView *view = theApp->Dlg;
    return view ? view->GetRoi(env, xsize, size) : 0;
}
/*
 * Class:     com_dryeye_app_CView
 * Method:    UpdateRoi
 * Signature: (Lcom//app/roi/SelectedBlockIcon;)Z
 */
extern "C" JNIEXPORT jboolean JNICALL Java_com_dryeye_app_CView_UpdateRoi
        (JNIEnv *env, jobject, jobject obj)
{
    WndImageView *view = theApp->Dlg;
    return (jboolean) (view && view->UpdateRoi(env, obj));
}

/*
 * Class:     com_dryeye_app_CView
 * Method:    UpdateRoiIcon
 * Signature: (Landroid/graphics/Bitmap;Ljava/lang/String;I)Z
 */
extern "C" JNIEXPORT jboolean JNICALL Java_com_dryeye_app_CView_UpdateRoiIcon
        (JNIEnv *, jobject, jobject _bmp, jstring st, jint color)
{
    jboolean ret = 0;
    WndImageView *view = theApp->Dlg;
    if (view)
    {
        ColorRef clr;
        clr.color = (COLORREF) color;
        BYTE b = clr.b;
        clr.b = clr.r;
        clr.r = b;
        JBitmap bmp;
        bmp.Attach(_bmp, AObject::TJLOCAL);
        JPtr nm = st;
        TString str = nm.String();

        ret = (jboolean) view->UpdateRoiIcon(bmp, str.GetPtr(), clr);
        bmp.Clear(false);

    }
    return ret;
}

