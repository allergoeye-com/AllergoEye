package com.allergoscope.app.card;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.DatePicker;

import com.allergoscope.app.R;

import java.util.StringTokenizer;

import lib.common.CDialog;
import lib.common.CEdit;
import lib.utils.AGlobals;

import static java.lang.String.*;

@SuppressLint("DefaultLocale")
public class CDateDlg extends CDialog {
    View view;
    public String date;
    DatePicker pic;

    public CDateDlg(String _date)
    {
        super(AGlobals.currentActivity);
        view = LayoutInflater.from(getContext()).inflate(R.layout.date, null);
        view.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        pic = view.findViewById(R.id.datePicker);
        date = _date;
        StringTokenizer st = new StringTokenizer(date, "/");
        int N = st.countTokens();
        if (N == 3)
        {
            String s = st.nextToken();
            int d = Integer.valueOf(s);
            s = st.nextToken();
            int m = Integer.valueOf(s);
            s = st.nextToken();
            int y = Integer.valueOf(s);
            pic.init(y, m - 1, d, null);
        }
        setView(view);
        AddButton(AlertDialog.BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);
        AddButton(AlertDialog.BUTTON_NEGATIVE, getContext().getString(R.string.cancel), IDCANCEL);


    }

    @Override
    public void OnOk()
    {
        date = format("%d/%d/%d", pic.getDayOfMonth(), pic.getMonth() + 1, pic.getYear());

        Result = IDOK;
        dismiss();

    }

}
