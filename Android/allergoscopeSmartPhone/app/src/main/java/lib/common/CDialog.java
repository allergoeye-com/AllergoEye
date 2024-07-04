package lib.common;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Point;
import android.graphics.Rect;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

import com.allergoscope.app.R;

import lib.utils.AGlobals;

import java.util.HashMap;

/**
 * Created by alex on 12.02.2018.
 */

public class CDialog extends AlertDialog {
    final static public int IDOK = 1;
    final static public int IDCANCEL = 2;
    final static public int IDABORT = 3;
    final static public int IDRETRY = 4;
    final static public int IDIGNORE = 5;
    final static public int IDYES = 6;
    final static public int IDNO = 7;
    final static public int IDCLOSE = 8;
    final static public int IDSYSTEMERROR = 9;

    public final static int MB_YESNO = 5;
    public final static int MB_OK = 3;
    public final static int MB_OKCANCEL = 4;
    public final static int MB_CANCEL = 100;
    public final static int MB_UNKNOWN = 200;
    public final static int MB_MINIMIZE = 300;


    protected int Result;
    protected boolean isDismiss = true;
    protected Point dimensionPixel = null;
    protected boolean fLock = false;
    protected EventIdle idle = null;


    public HashMap<Integer, ActionCallback> commands;

    public CDialog()
    {
        super(AGlobals.currentActivity);
    }


    //-------------------------------------------------
    //
    //---------------------------------------------------
    public CDialog(Context context)
    {
        this(context, THEME_HOLO_DARK);
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    public CDialog(Context context, HashMap<Integer, ActionCallback> cmd)
    {
        this(context, cmd, THEME_HOLO_DARK);
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    public CDialog(Context context, int theme)
    {
        this(context, null, theme);
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    public CDialog(Context context, HashMap<Integer, ActionCallback> cmd, int theme)
    {
        super(context, theme);
//        new ContextThemeWrapper(context, R.style.AppThemeSeek)
        commands = cmd == null ? new HashMap<Integer, ActionCallback>() : cmd;
        Result = IDCANCEL;

        this.setOnShowListener(new CShowListener());

    }

    public void SetDimensions(int x, int y)
    {
        dimensionPixel = new Point(x, y);
    }

    public void SetDimensionsProcent(float x, float y)
    {
        Rect displayRectangle = new Rect();
        Window window = AGlobals.currentActivity.getWindow();

        window.getDecorView().getWindowVisibleDisplayFrame(displayRectangle);
        SetDimensions((int) (displayRectangle.width() * x), (int) (displayRectangle.height() * y));
    }

    @Override
    public void show()
    {
        if (isDismiss)
        {
            isDismiss = false;
            super.show();
            if (dimensionPixel != null)
            {
                WindowManager.LayoutParams attrs = getWindow().getAttributes();
                getWindow().setLayout(dimensionPixel.x != 0 ? dimensionPixel.x : attrs.width , dimensionPixel.y != 0 ? dimensionPixel.y: attrs.height);
            }
        }
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    public void OnInitDialog()
    {
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    protected void OnCommand(int id)
    {
        Result = id;
        ActionCallback cmd = commands.get(id);
        if (cmd != null)
            cmd.run(id);
        else if (id == IDOK)
            OnOk();
        else if (id == IDCANCEL)
            OnCancel();
        else
            dismiss();
    }

    public void OnOk()
    {
        Result = IDOK;
        dismiss();

    }

    public void OnCancel()
    {
        Result = IDCANCEL;
        dismiss();
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    public void AddButton(int whichButton, CharSequence text, int ID)
    {
        setButton(whichButton, text, new COnClick(ID));
    }

    public void EnableButton(int whichButton, boolean f)
    {
        Button b = getButton(whichButton);
        b.setEnabled(f);
    }
    //-------------------------------------------------
    //
    //---------------------------------------------------
    public void AddButton(int whichButton, int ID)
    {
        String str = null;
        switch (ID)
        {
            case IDNO:
                    str = AGlobals.currentActivity.getString(R.string.no);
                break;
            case IDYES:
                    str = AGlobals.currentActivity.getString(R.string.yes);
                break;
            case IDOK:
        str =AGlobals.currentActivity.getString(R.string.ok);
                break;
            case IDCANCEL:
                 str = AGlobals.currentActivity.getString(R.string.cancel);
                break;
            default:
                str = " ";
        }
        setButton(whichButton, str, new COnClick(ID));
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    protected void InitRCButton(View v, int idButton, String func)
    {

        v.findViewById(idButton).setOnClickListener(new OnClickCalback(this, func));
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    public void ShowDialog()
    {
        Show();
    }

    public void Show()
    {

        show();
    }


    public void Hide()
    {
        if (!isDismiss)
        {
            isDismiss = true;
            super.dismiss();
            ActionCallback cmd = commands.get(MB_MINIMIZE);
            if (cmd != null)
                cmd.run();
        }
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    public int DoModal()
    {
        if (idle != null) return -1;
        idle = new EventIdle(this.toString());
        Show();
        idle.Loop();
        idle = null;
        return Result;

    }

    //-----------------------------------------------
    //
    //-----------------------------------------------
    @Override
    public void dismiss()
    {
        if (!fLock)
        {
            if (!isDismiss)
            {
                isDismiss = true;
                super.dismiss();
            }
        }
        if (idle != null && idle.IsInLoop())
            idle.RunAndExit();
    }

    public void Lock(boolean b)
    {
        fLock = b;
    }

    public void EndDialog(int res)
    {
        Result = res;
        dismiss();
    }

    //*******************************************************************
    class CShowListener implements OnShowListener {
        @Override
        public void onShow(DialogInterface dialog)
        {
            OnInitDialog();
        }
    }

    //*******************************************************************
    class COnClick implements OnClickListener {
        int id;

        public COnClick(int _id)
        {
            id = _id;
        }

        @Override
        public void onClick(DialogInterface dialog, int which)
        {
            OnCommand(id);
        }
    }


}
