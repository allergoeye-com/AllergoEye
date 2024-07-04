package lib.common;

import android.graphics.RectF;
import android.util.Size;

public class CF_Size {
    public float m_cx = 0.0f;
    public float m_cy = 0.0f;

    public CF_Size()
    {
    }

    public CF_Size(Size sz)
    {
        m_cx = sz.getWidth();
        m_cy = sz.getHeight();

    }

    public CF_Size(float x, float y)
    {
        m_cx = x;
        m_cy = y;

    }

    public CF_Size(CF_Size c)
    {
        m_cx = c.m_cx;
        m_cy = c.m_cy;

    }

    public int getWidth()
    {
        return (int) (m_cx + 0.5f);
    }

    public int getHeight()
    {
        return (int) (m_cy + 0.5f);

    }

    public void CX(float c)
    {
        m_cx = c;
    }

    public void CY(float c)
    {
        m_cy = c;
    }

    public void Set(CF_Size c)
    {
        m_cx = c.m_cx;
        m_cy = c.m_cy;

    }

    public void Set(RectF c)
    {
        m_cx = c.width();
        m_cy = c.height();

    }

    public void Set(float x, float y)
    {
        m_cx = x;
        m_cy = y;

    }

    void Inflate(float i)
    {
        m_cx -= i;
        m_cy -= i;

    }

    void Inflate(float x, float y)
    {
        m_cx -= x;
        m_cy -= y;

    }

}
