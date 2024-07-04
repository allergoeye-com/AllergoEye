package com.allergoscope.app.card;

import android.app.AlertDialog;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RadioButton;
import android.widget.TextView;

import com.allergoscope.app.R;

import lib.common.CDialog;
import lib.utils.AGlobals;

import static java.lang.String.format;

public class DlgDeleteMerge extends CDialog {
    View view;
    public int pos;

    public DlgDeleteMerge()
    {
        super(AGlobals.currentActivity);
        view = LayoutInflater.from(getContext()).inflate(R.layout.select_delete, null);
        view.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        pos = 0;
        RadioButton btn = view.findViewById(R.id.ID_DELETE);
        String str = AGlobals.ReadString("IDS_DELETE");
        if (str != null)
            btn.setText(str);
        str = AGlobals.ReadString("IDS_OR");
        TextView or = view.findViewById(R.id.ID_OR);
        if (str != null)
            or.setText(str);
        btn = view.findViewById(R.id.ID_MERGE);
        str = AGlobals.ReadString("IDS_MERGE");
        if (str != null)
            btn.setText(str);


        setView(view);
        AddButton(AlertDialog.BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);
        AddButton(AlertDialog.BUTTON_NEGATIVE, getContext().getString(R.string.cancel), IDCANCEL);


    }

    @Override
    public void OnOk()
    {
        RadioButton btn = view.findViewById(R.id.ID_MERGE);
        pos = btn.isChecked() ? 0 : 1;
        Result = IDOK;
        dismiss();

    }

}
