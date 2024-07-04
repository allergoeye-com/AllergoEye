package lib.common;

import android.os.Looper;

import lib.utils.AGlobals;


import java.util.HashMap;
import java.util.Timer;
import java.util.TimerTask;

import static android.content.DialogInterface.BUTTON_NEGATIVE;
import static android.content.DialogInterface.BUTTON_POSITIVE;
import static lib.common.CDialog.IDCANCEL;
import static lib.common.CDialog.IDNO;
import static lib.common.CDialog.IDOK;
import static lib.common.CDialog.IDYES;
import static lib.common.CDialog.MB_CANCEL;
import static lib.common.CDialog.MB_OK;
import static lib.common.CDialog.MB_OKCANCEL;
import static lib.common.CDialog.MB_YESNO;

import com.allergoscope.app.R;

/**
 * Created by alex on 23.02.2018.
 */

public class MessageBox {
    private CDialog dlg;
    String  lpText;
    String  lpCaption;
    int uType;
    CTimer timer = null;
    public HashMap<Integer, ActionCallback> commands;
    //---------------------------------------
    //
    //---------------------------------------
    public MessageBox(String  _lpText,
               String  _lpCaption,
               int _uType)
    {
        lpText = _lpText;
        lpCaption = _lpCaption;
        uType = _uType;
        dlg = null;
        commands = new HashMap<Integer, ActionCallback>();
        if (Thread.currentThread() == Looper.getMainLooper().getThread())
            Init();
        else
        {

            final CriticalSection enter = new CriticalSection();
            enter.Enter();
            AGlobals.rootHandler.Send(new Runnable(){
                @Override
                public void run() {
                    try {
                        Init();
                    }catch(Exception e)
                    {
                        e.printStackTrace();
                        dlg = null;
                    }
                    enter.Leave();
                }
            });
            enter.Enter();

        }

    }
    public void SetAutocloseCloseMiliseconds(int mili)
    {
        Timer ti = new Timer();
        ti.schedule(new TimerTask() {
            @Override
            public void run() {
                AGlobals.rootHandler.Send(new Runnable() {
                    @Override
                    public void run() {
                        EndDialog(IDOK);
                    }
                });
            }
        }, mili);

    }
    //---------------------------------------
    //
    //---------------------------------------
    private void Init()
    {

        dlg = new CDialog(AGlobals.currentActivity, commands);
        if (lpCaption != null && lpCaption.length() > 0)
        dlg.setTitle(lpCaption);

        if (lpText != null && lpText.length() > 0)
            dlg.setMessage(lpText);

        if (uType == MB_YESNO)
        {
            dlg.AddButton(BUTTON_NEGATIVE, AGlobals.currentActivity.getString(R.string.no), IDNO);
            dlg.AddButton(BUTTON_POSITIVE, AGlobals.currentActivity.getString(R.string.yes), IDYES);
        }
        else
        if (uType == MB_OKCANCEL)
        {
            dlg.AddButton(BUTTON_POSITIVE, AGlobals.currentActivity.getString(R.string.ok), IDOK);
            dlg.AddButton(BUTTON_NEGATIVE, AGlobals.currentActivity.getString(R.string.cancel), IDCANCEL);
        }
        else
        if (uType == MB_OK)
            dlg.AddButton(BUTTON_POSITIVE, AGlobals.currentActivity.getString(R.string.ok), IDOK);
        else
        if (uType == MB_CANCEL)
            dlg.AddButton(BUTTON_NEGATIVE, AGlobals.currentActivity.getString(R.string.cancel), IDCANCEL);



    }
    public void Lock(final boolean b)
    {
        if (dlg == null) return;
        if (!Thread.currentThread().equals(Looper.getMainLooper().getThread()))
        {
            final CriticalSection enter = new CriticalSection();
            enter.Enter();
            AGlobals.rootHandler.Send(new Runnable(){
                @Override
                public void run() {
                    try {
                        dlg.Lock(b);
                    }catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                    enter.Leave();
                }
            });
            enter.Enter();

        }
        else
            dlg.Lock(b);
    }
    //---------------------------------------
    //
    //---------------------------------------
    public void ShowDialog()
    {
        if (dlg == null) return;
        if (!Thread.currentThread().equals(Looper.getMainLooper().getThread()))
        {
            final CriticalSection enter = new CriticalSection();
            enter.Enter();
            AGlobals.rootHandler.Send(new Runnable(){
                @Override
                public void run() {
                    try {
                        dlg.ShowDialog();
                    }catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                    enter.Leave();
                }
            });
            enter.Enter();

        }
        else
            dlg.ShowDialog();
    }
    //---------------------------------------
    //
    //---------------------------------------
    public void EndDialog(final int ID)
    {
        if (dlg == null) return;
        if (!Thread.currentThread().equals(Looper.getMainLooper().getThread()))
        {
            final CriticalSection enter = new CriticalSection();
            enter.Enter();
            AGlobals.rootHandler.Send(new Runnable(){
                @Override
                public void run() {
                    try {
                        dlg.EndDialog(ID);
                    }catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                    enter.Leave();
                }
            });
            enter.Enter();

        }
        else
            dlg.EndDialog(ID);

    }
    //---------------------------------------
    //
    //---------------------------------------
    public int DoModal()
    {
        if (dlg == null) return IDCANCEL;
        if (Thread.currentThread() == Looper.getMainLooper().getThread())
            return dlg.DoModal();
        final CriticalSection enter = new CriticalSection();
        final int[] result = new int[1];
        result[0] = IDCANCEL;
        enter.Enter();
        AGlobals.rootHandler.Send(new Runnable(){
            @Override
            public void run() {
                try {
                result[0] = dlg.DoModal();
                }catch(Exception e)
                {
                    e.printStackTrace();
                }
                enter.Leave();
            }
        });
        enter.Enter();
        return result[0];
    }
    //---------------------------------------
    //
    //---------------------------------------
    public void SetText(final CharSequence text)
    {
        if (dlg == null) return;
        if (!Thread.currentThread().equals(Looper.getMainLooper().getThread()))
        {
            final CriticalSection enter = new CriticalSection();
            enter.Enter();
            AGlobals.rootHandler.Send(new Runnable(){
                @Override
                public void run() {
                    try {
                        dlg.setMessage(text);
                    }catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                    enter.Leave();
                }
            });
            enter.Enter();

        }
        else
            dlg.setMessage(text);
    }
    //---------------------------------------
    //
    //---------------------------------------
    public void Hide()
    {
        if (dlg == null) return;
        if (!Thread.currentThread().equals(Looper.getMainLooper().getThread()))
        {
            final CriticalSection enter = new CriticalSection();
            enter.Enter();
            AGlobals.rootHandler.Send(new Runnable(){
                @Override
                public void run() {
                    try {
                        dlg.Hide();
                    }catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                    enter.Leave();
                }
            });
            enter.Enter();

        }
        else
            dlg.Hide();
    }
    //---------------------------------------
    //
    //---------------------------------------
    public void Show()
    {
        if (dlg == null) return;
        if (!Thread.currentThread().equals(Looper.getMainLooper().getThread()))
        {
            final CriticalSection enter = new CriticalSection();
            enter.Enter();
            AGlobals.rootHandler.Send(new Runnable(){
                @Override
                public void run() {
                    try {
                        dlg.Show();
                    }catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                    enter.Leave();
                }
            });
            enter.Enter();

        }
        else
            dlg.Show();
    }


}
