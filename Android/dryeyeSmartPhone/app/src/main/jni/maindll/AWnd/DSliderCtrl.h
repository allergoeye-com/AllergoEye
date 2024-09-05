/*
 * DSliderCtrl.h
 *
 *  Created on: Dec 18, 2014
 *      Author: alex
 */

#ifndef DSLIDERCTRL_H_
#define DSLIDERCTRL_H_
#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC ASliderCtrl : public ADispatcher {
public:
    typedef void (*callSetPos)(ADispatcher *, int);

    typedef void (*callGetPos)(ADispatcher *, int *);

    typedef void (*callSetRange)(ADispatcher *, int);

    ASliderCtrl();

    virtual ~ASliderCtrl();

    void SetRange(int _iMin, int _iMax, BOOL)
    {
        if (funcSetRange)
            funcSetRange(this, _iMax - _iMin);
        iMin = _iMin;
        iMax = _iMax;
        iPos = iMin;
    }

    void SetPos(int i)
    {
        if (funcSetPos) funcSetPos(this, i - iMin);
        iPos = i;
    }

    int GetPos()
    {
        if (funcGetPos)
        {
            funcGetPos(this, &iPos);
            iPos += iMin;
        }
        return iPos;
    }

    virtual void UpdateWindow()
    { SetPos(iPos); }

    callSetPos funcSetPos;
    callGetPos funcGetPos;
    callSetRange funcSetRange;

    int iMin;
    int iMax;
    int iPos;
};
EXPORTNIX class EXPMDC FloatSliderCtrl : public ADispatcher {
public:
    FloatSliderCtrl();

    virtual ~FloatSliderCtrl()
    {}

    void SetRange(float _iMin, float _iMax, BOOL _fIsLn)
    {
        fIsLn = _fIsLn;
        if (fIsLn)
            step = 100.0f / (float) (log(_iMax) - log(_iMin));
        else
            step = 100.0f / (_iMax - _iMin);

        if (funcSetRange)
        {
            if (fIsLn)
                funcSetRange(this, int(step * (log(_iMax) - log(_iMin))));
            else
                funcSetRange(this, int(step * (_iMax - _iMin)));
        }
        iMin = _iMin;
        iMax = _iMax;
        iPos = iMin;
    }

    void SetPos(float i)
    {
        if (funcSetPos)
        {
            funcSetPos(this, ConvertToPos(i));

        }
        iPos = i;
    }

    float GetPos(int *pos)
    {
        int i = 0;
        if (funcGetPos)
        {
            funcGetPos(this, &i);
            iPos = ConvertPos(i);
            if (pos) *pos = i;
        }

        return iPos;
    }

    int ConvertToPos(float i)
    {
        if (fIsLn)
            return int(step * (log(i) - log(iMin)));
        return int(step * (i - iMin));

    }

    float ConvertPos(int src)
    {
        float dst;
        if (fIsLn)
            dst = (float) exp(src / step + log(iMin));
        else
            dst = src / step + iMin;
        return dst;
    }

    float GetPos()
    { return GetPos(0); }

    ASliderCtrl::callSetPos funcSetPos;
    ASliderCtrl::callGetPos funcGetPos;
    ASliderCtrl::callSetRange funcSetRange;

    float iMin;
    float iMax;
    float iPos;
    float step;
    BOOL fIsLn;

};

EXPORTNIX class EXPMDC ACastomSliderCtrl : public AWnd {
public:
    typedef void (*callDrawProgress)(ACastomSliderCtrl *);

    ACastomSliderCtrl();

    virtual ~ACastomSliderCtrl();

    void SetRange(int _iMin, int _iMax, BOOL f)
    {
        if (iMin != _iMin || iMax != _iMax || iPos != iMin)
        {
            iMin = _iMin;
            iMax = _iMax;
            iPos = iMin;
            if (f) OnPaint();
        }
    }

    void SetPos(int i)
    {
        iPos = i;
        OnPaint();
    }

    int GetPos()
    { return iPos; }

    virtual void OnPaint()
    { if (call) call(this); }

    callDrawProgress call;
    int iMin;
    int iMax;
    int iPos;
};
#pragma pack(pop)

#endif /* DSLIDERCTRL_H_ */
