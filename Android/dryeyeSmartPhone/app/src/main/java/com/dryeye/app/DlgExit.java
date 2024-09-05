package com.dryeye.app;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import lib.common.CDialog;
import lib.common.button.CMenuButton;

public class DlgExit extends CDialog {
    View view;
    public DlgExit(Context context) {
        super(context, THEME_HOLO_LIGHT);
        view  = LayoutInflater.from(getContext()).inflate(R.layout.dialog_view_exit, null);
        view.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        CMenuButton button = view.findViewById(R.id.idCancel);
        button.SetCallback(this, "OnCancel");
        button = view.findViewById(R.id.idExit);
        button.SetCallback(this, "OnExit");
        button = view.findViewById(R.id.idSaveAndExit);
        button.SetCallback(this, "OnSaveAndExit");
        setView(view);
    }
    public void OnCancel()
    {
        EndDialog(IDCANCEL);
    }
    public void OnExit()
    {
        EndDialog(IDCLOSE);
    }
    public void OnSaveAndExit()
    {
        EndDialog(IDOK);
    }
}
