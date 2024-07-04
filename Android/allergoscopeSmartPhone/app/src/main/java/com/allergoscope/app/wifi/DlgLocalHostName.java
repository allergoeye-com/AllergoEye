package com.allergoscope.app.wifi;

import android.content.Context;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.TextView;

import com.allergoscope.app.R;

import lib.common.CDialog;
import lib.utils.AGlobals;

public class DlgLocalHostName extends CDialog {
    View view;
    EditText test;
    public String result = null;

    public DlgLocalHostName(Context context)
    {
        super(context);
        view = LayoutInflater.from(getContext()).inflate(R.layout.dlg_namehost, null);
        test = view.findViewById(R.id.idName);

        setView(view);

        AddButton(BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);
        Lock(true);
    }

    public DlgLocalHostName(String name, Context context)
    {
        super(context);
        view = LayoutInflater.from(getContext()).inflate(R.layout.dlg_namehost, null);
        test = view.findViewById(R.id.idName);
        if (name != null)
            test.setText(name);
        setView(view);

        AddButton(BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);
        Lock(true);
    }
    @Override
    public void show()
    {
        super.show();
     /*   if (isDismiss)
        {
            isDismiss = false;
            super.show();
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            WindowManager.LayoutParams attrs = getWindow().getAttributes();
            getWindow().setLayout((int) (metrics.widthPixels), attrs.height);
        }

      */
    }
    @Override
    protected void OnCommand(int id)
    {
        if (id == IDOK)
        {
            result = test.getText().toString().trim();
            if (result == null || result.length() < 1) return;
            Lock(false);
            EndDialog(id);
        }

    }

}
