package com.dryeye.app.wifi;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;

import com.dryeye.app.R;

import lib.common.CDialog;

public class DlgOptions extends CDialog {
    View view;
    EditText test;
    public String result = null;

    public DlgOptions(Context context)
    {
        super(context);
        view = LayoutInflater.from(getContext()).inflate(R.layout.dlg_options, null);
        test = view.findViewById(R.id.idName);
        String name = SyncThread.LocalHost();
        if (name != null)
            test.setText(name);
        test = view.findViewById(R.id.idAddress);
        test.setText(SyncThread.MulticastAddress());
        test = view.findViewById(R.id.idPort);
        name = "";
        name += SyncThread.MulticastPort();
        test.setText(name);


        setView(view);

        AddButton(BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);
        Lock(true);
    }


    protected void OnCommand(int id)
    {
        if (id == IDOK)
        {
            test = view.findViewById(R.id.idName);
            SyncThread.LocalHost(test.getText().toString().trim());
            test = view.findViewById(R.id.idAddress);
            SyncThread.MulticastAddress(test.getText().toString().trim());
            test = view.findViewById(R.id.idPort);
            SyncThread.MulticastPort(test.getText().toString().trim());
            result = test.getText().toString().trim();
            if (result == null || result.length() < 1) return;
            Lock(false);
            EndDialog(id);
        }

    }

}
