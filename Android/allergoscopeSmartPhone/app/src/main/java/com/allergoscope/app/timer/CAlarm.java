package com.allergoscope.app.timer;

import android.view.View;
import android.os.SystemClock;
import android.view.ViewGroup;

import androidx.appcompat.app.AppCompatActivity;

import com.allergoscope.app.R;
import java.text.SimpleDateFormat;
import lib.common.CTimer;
import lib.common.MessageBox;
import lib.utils.AGlobals;
import lib.utils.ASound;

import static lib.common.CDialog.IDOK;
import static lib.common.CDialog.IDYES;
import static lib.common.CDialog.MB_OKCANCEL;
import static lib.common.CDialog.MB_YESNO;

public class CAlarm{
    long time = 0;
    long wait = 0;
    long elapsedTime = 0;
    CTimer timer = null;

    public CAlarm()
    {
        super();
   }
    public void StartTimer(long _wait)
    {
        wait = _wait;
        time = SystemClock.elapsedRealtime();
        timer = new CTimer(this, "OnTimeButton");
        timer.Run(1000);

    }
    public void OnTimer()
    {
        if (timer == null)
        {
            TimerDlg dlg = new TimerDlg();
            if (dlg.DoModal() == IDOK)
            {
                StartTimer(((dlg.result + 1) * 60000) * 5);
            }
        }
        else
        {
            long currentTime = SystemClock.elapsedRealtime();
            elapsedTime = (currentTime - time);
            if (timer != null && wait <= elapsedTime)
                StopTimer();
            else
            {
                MessageBox b = new MessageBox(AGlobals.currentActivity.getString(R.string.stop_timer), null, MB_YESNO);
                if (b.DoModal() == IDYES)
                    StopTimer();
            }
        }
    }

    public void StopTimer()
    {
        if (timer != null)
            timer.Stop();
        timer = null;
        stopTimer();

    }

    public void OnTimeButton()
    {
        long currentTime = SystemClock.elapsedRealtime();
        elapsedTime = (currentTime - time);
        if (timer != null && wait <= elapsedTime)
        {
            //stopTimer();
            ASound.PlayAudio("censor_beep");
            //return;
        }
        if (AGlobals.currentActivity != null)
        {
            ViewGroup group = (ViewGroup)AGlobals.currentActivity.findViewById(android.R.id.content);
            SimpleDateFormat dateformat = new SimpleDateFormat("mm:ss");
            String s = dateformat.format(elapsedTime);

            setTime(group, s);
        }
    }
    void setTime(ViewGroup group, String str)
    {
        if (group == null) return;
        int N = group.getChildCount();
        for (int i = 0; i < N; ++i)
        {

            try
            {

                View v2 = group.getChildAt(i);
                if (v2 != null)
                {
                    View v = v2.findViewById(R.id.idTimer);
                    if (v != null && (v instanceof TimerButton))
                    {
                        TimerButton tv = (TimerButton)v;
                        tv.SetText(str);
                    }
                }

                if (v2 instanceof ViewGroup)
                {
                    setTime((ViewGroup)v2, str);
                }
            }catch (Exception e)
            {
                ;
            }


        }
    }

    void stopTimer()
    {
        if (AGlobals.currentActivity != null)
        {
            ViewGroup group = (ViewGroup)AGlobals.currentActivity.findViewById(android.R.id.content);
            setTime(group, null);
        }
    }
    public boolean IsRun()
    {
        return timer != null;
    }



}
