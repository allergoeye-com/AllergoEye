package lib.common;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.SeekBar;


public class CSlider extends androidx.appcompat.widget.AppCompatSeekBar {
    protected float iMin = 0;
    protected float iMax = 100;
    protected float iPos = 0;
    protected float step = 1;
    protected boolean fIsLn = false;
    ActionCallback onchaned = null;
    final public int ONSTART = 1;
    final public int ONEND = 2;
    final public int ONPROGRESS = 3;

    public CSlider(Context context)
    {
        super(context);
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public CSlider(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public float GetMax()
    {
        return iMax;
    }

    public float GetMin()
    {
        return iMin;
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public CSlider(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public void SetCalback(Object parent, String func)//parameter START, END, RUN, boolean isUser
    {
        if (parent == null || func == null)
        {
            setOnSeekBarChangeListener(null);
            onchaned = null;
        } else
        {
            onchaned = new ActionCallback(parent, func, int.class, boolean.class);
            setOnSeekBarChangeListener(new OnProgressChange());
        }
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public void SetCalback(ActionCallback c)
    {
        onchaned = c;
        if (c == null)
            setOnSeekBarChangeListener(null);
        else
            setOnSeekBarChangeListener(new OnProgressChange());
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public void SetRange(float _iMin, float _iMax, boolean _fIsLn)
    {
        fIsLn = _fIsLn;
        if (fIsLn)
        {
            if (_iMin == 0.0f)
                _iMin = 0.0001f;
            if (_iMax == 0.0f)
                _iMax = 0.0001f;
            step = (float) (100.0 / (Math.log(_iMax) - Math.log(_iMin)));
        } else
            step = 100.0f / (_iMax - _iMin);

        if (fIsLn)
            setMax((int) (step * (Math.log(_iMax) - Math.log(_iMin))));
        else
            setMax((int) (step * (_iMax - _iMin)));
        iMin = _iMin;
        iMax = _iMax;
        iPos = iMin;
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public void SetRange(int _iMin, int _iMax)
    {
        SetRange((float) _iMin, (float) _iMax, false);
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public void SetRange(float _iMin, float _iMax)
    {
        SetRange(_iMin, _iMax, false);
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public float Min()
    {
        return iMin;
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public float Max()
    {
        return iMax;
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public void SetPos(float i)
    {
        setProgress(ConvertToPos(i));
        iPos = i;
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public void SetPos(int i)
    {
        SetPos((float) i);
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    public float GetPos()
    {
        int i = getProgress();
        iPos = ConvertPos(i);
        return iPos;
    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    protected int ConvertToPos(float i)
    {
        if (fIsLn)
            return (int) (step * (Math.log(i) - Math.log(iMin)));
        return (int) (step * (i - iMin));

    }

    //----------------------------------------------------------------
    //
    //----------------------------------------------------------------
    protected float ConvertPos(int src)
    {
        float dst;
        if (fIsLn)
            dst = (float) Math.exp(src / step + Math.log(iMin));
        else
            dst = src / step + iMin;
        return dst;
    }

    class OnProgressChange implements SeekBar.OnSeekBarChangeListener {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
        {
            if (onchaned != null) onchaned.run(ONPROGRESS, fromUser);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar)
        {
            if (onchaned != null) onchaned.run(ONEND, true);

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar)
        {
            if (onchaned != null) onchaned.run(ONSTART, true);

        }
    }

}
