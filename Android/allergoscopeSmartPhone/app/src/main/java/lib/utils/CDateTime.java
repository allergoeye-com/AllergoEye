package lib.utils;

import android.annotation.SuppressLint;

import java.io.File;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Comparator;
import java.util.Date;
import java.util.StringTokenizer;

/**
 * Created by alex on 07.11.17.
 */
public class CDateTime {
    public int wYear;
    public int wMonth;
    public int wDay;
    public int wHour;
    public int wMinute;
    public int wSecond;
    public String date;
    boolean fValid = false;
    @SuppressLint("SimpleDateFormat")
    static public SimpleDateFormat formatDate = new SimpleDateFormat("yyyy-MM-dd HH-mm-ss");;
    static public SimpleDateFormat formatView = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss");;
    static public Comparator<CDateTime> cmp = new Comparator<CDateTime>() {
        public int compare(CDateTime tm1, CDateTime tm2)
        {
            return Compare(tm1, tm2);
        }};
    //----------------------------------------------------
    //
    //----------------------------------------------------
    static public int Compare(CDateTime tm1, CDateTime tm2)
    {
        if (tm1.fValid && tm2.fValid)
        {
            if (tm1.wYear > tm2.wYear) return 1;
            else if (tm1.wYear < tm2.wYear) return -1;
            if (tm1.wMonth > tm2.wMonth) return 1;
            else if (tm1.wMonth < tm2.wMonth) return -1;
            if (tm1.wDay > tm2.wDay) return 1;
            else if (tm1.wDay < tm2.wDay) return -1;
            if (tm1.wHour > tm2.wHour) return 1;
            else if (tm1.wHour < tm2.wHour) return -1;
            if (tm1.wMinute > tm2.wMinute) return 1;
            else if (tm1.wMinute < tm1.wMinute) return -1;

            if (tm1.wSecond > tm2.wSecond) return 1;
            else if (tm1.wSecond < tm2.wSecond) return -1;
            return 0;
        }
        return -1;
    };
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public CDateTime(File f)
    {
        Long last_modi = f.lastModified();
        InitTime(formatDate.format(last_modi));

    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public CDateTime(String f)
    {
        InitTime(f);
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public CDateTime(Long f)
    {
        InitTime(formatDate.format(f));
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    boolean IsValid() { return fValid; }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public Long GetFileTime()
    {
        if (!fValid) return 0L;
        try
        {
            return formatDate.parse(date).getTime();
        } catch (ParseException e)
        {
            return 0L;
        }

    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    static public String ConvertDate(String s)
    {
        StringTokenizer st = new StringTokenizer(s, "- ");
        String sYear, sMonth;
        if (st.hasMoreTokens())
        {
            sYear = st.nextToken();
            if (st.hasMoreTokens())
            {
                sMonth = st.nextToken();
                if (st.hasMoreTokens())
                    return st.nextToken() + "/" + sMonth + "/" + sYear;
            }
        }
        return null;
    }
    Date GetDate()
    {
        return new Date(wYear, wMonth, wDay, wHour, wMinute, wSecond);
    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    static public String ConvertTime(String s)
    {
        StringTokenizer st = new StringTokenizer(s, "- ");
        String sYear, sMonth, sDay, sHour, sMinute, sSecond;
        if (st.hasMoreTokens())
        {
            sYear = st.nextToken();
            if (st.hasMoreTokens())
            {
                sMonth = st.nextToken();
                if (st.hasMoreTokens())
                {
                    sDay = st.nextToken();
                    if (st.hasMoreTokens())
                    {
                        sHour = st.nextToken();
                        if (st.hasMoreTokens())
                        {
                            sMinute  = st.nextToken();
                            if (st.hasMoreTokens())
                            {
                                sSecond  = st.nextToken();
                                return sDay + "/" + sMonth + "/" + sYear + " " + sHour + ":" + sMinute + ":" + sSecond;

                            }
                        }
                    }
                }
            }
        }
        return null;
    }

    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void InitDate(String s)
    {
        try {
            date = s;
            StringTokenizer st = new StringTokenizer(s, "- ");
            if (st.hasMoreTokens())
            {
                wYear = Integer.parseInt(st.nextToken());
                if (st.hasMoreTokens())
                {
                    wMonth = Integer.parseInt(st.nextToken());
                    if (st.hasMoreTokens())
                    {
                        wDay = Integer.parseInt(st.nextToken());
                        fValid = true;
                    }
                }
            }
        }
        catch(Exception e)
        {
            fValid = false;
        }

    }

    public void InitTime(String s)
    {
        try {
            date = s;
            StringTokenizer st = new StringTokenizer(s, "- ");
            if (st.hasMoreTokens())
            {
                wYear = Integer.parseInt(st.nextToken());
                if (st.hasMoreTokens())
                {
                    wMonth = Integer.parseInt(st.nextToken());
                    if (st.hasMoreTokens())
                    {
                        wDay = Integer.parseInt(st.nextToken());
                        if (st.hasMoreTokens())
                        {
                            wHour = Integer.parseInt(st.nextToken());
                            if (st.hasMoreTokens())
                            {
                                wMinute  = Integer.parseInt(st.nextToken());
                                if (st.hasMoreTokens())
                                {
                                    wSecond  = Integer.parseInt(st.nextToken());
                                    fValid = true;

                                }
                            }
                        }
                    }
                }
            }
        }
        catch(Exception e)
        {
            fValid = false;
        }

    }
    //----------------------------------------------------
    //
    //----------------------------------------------------
    public void Init(String s)
    {
        try {
            date = s;
            StringTokenizer st = new StringTokenizer(s, "- ");
            if (st.hasMoreTokens())
            {
                wYear = Integer.parseInt(st.nextToken());
                if (st.hasMoreTokens())
                {
                    wMonth = Integer.parseInt(st.nextToken());
                    if (st.hasMoreTokens())
                    {
                        wDay = Integer.parseInt(st.nextToken());
                        if (st.hasMoreTokens())
                        {
                            wHour = Integer.parseInt(st.nextToken());
                            if (st.hasMoreTokens())
                            {
                                wMinute  = Integer.parseInt(st.nextToken());
                                if (st.hasMoreTokens())
                                {
                                    wSecond  = Integer.parseInt(st.nextToken());
                                    fValid = true;

                                }
                            }
                        }
                    }
                }
            }
        }
        catch(Exception e)
        {
            fValid = false;
        }

    }
}
