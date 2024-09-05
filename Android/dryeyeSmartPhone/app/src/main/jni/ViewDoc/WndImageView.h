//
// Created by alex on 25.06.2019.
//

#ifndef ALLERGOSCOPE_WNDIMAGEVIEW_H
#define ALLERGOSCOPE_WNDIMAGEVIEW_H


class WndImageView : public AWnd {
public:
    WndImageView();

    virtual ~WndImageView();

    virtual void OnSize(UINT nType, int cx, int cy);

    void OnPaint(int);

    void OnLoad(jobject _bmp);

    void OnRoi(TString &filename, bool fView);

    jobject GetRoi(JNIEnv *env, int width, int height);

    BOOL UpdateRoi(JNIEnv *env, jobject obj);

    BOOL UpdateRoiIcon(JBitmap &_bmp, LPCSTR name, ColorRef &ref);

    void Clear();

    JBitmap bmp;
    MemImage img;
    CRect rcOrig;
    bool fFirst;
    CriticalSection sec;
    int startAngle;
    Transform2F<CPoint, RECT> trans;
    RBMap<MVector<MArray<Vector2F> >, TString> roi;
    MHashMap<int, TString> enable_roi;
    MHashMap<int, TString> colors;
};


#endif //ALLERGOSCOPE_WNDIMAGEVIEW_H
