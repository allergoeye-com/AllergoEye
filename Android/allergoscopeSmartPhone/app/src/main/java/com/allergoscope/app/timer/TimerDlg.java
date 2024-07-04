package com.allergoscope.app.timer;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import androidx.core.content.ContextCompat;

import com.allergoscope.app.R;

import lib.common.ActionCallback;
import lib.common.CDialog;
import lib.common.CListBox;
import lib.utils.AGlobals;

public class TimerDlg extends CDialog {
    TextView test;
    CListBox box;
    public int result = 0;

    View view;
    public TimerDlg()
    {
        super(AGlobals.currentActivity);
        String minute =AGlobals.currentActivity.getString(R.string.minute);
        view = LayoutInflater.from(getContext()).inflate(R.layout.timer_select, null);


        RadioGroup gr = view.findViewById(R.id.idLangGroup);
        for (int i = 0; i < gr.getChildCount(); ++i)
        {
            RadioButton c = (RadioButton)gr.getChildAt(i);
            String s = " " + (i + 1) * 5;
            s += " ";
            s += minute;
            c.setText(s);
            if (i == 0)
            {
                c.setChecked(true);
                c.setTextColor(ContextCompat.getColor(AGlobals.currentActivity, R.color.theme_bar_color_down));
            }

        }

        setView(view);
        AddButton(BUTTON_NEGATIVE, getContext().getString(R.string.cancel), IDCANCEL);
        AddButton(BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);
        gr.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                OnClick(checkedId);
                for (int i = 0; i < gr.getChildCount(); ++i)
                {
                    RadioButton c = (RadioButton)gr.getChildAt(i);
                    if (checkedId != c.getId())
                        c.setTextColor(ContextCompat.getColor(AGlobals.currentActivity, R.color.theme_dlg_text_color));
                    else
                        c.setTextColor(ContextCompat.getColor(AGlobals.currentActivity, R.color.background_material_light));


                }


            }
        });

    }
    public void OnClick(int i) {
        switch (i) {
            case R.id.id5:
                result = 0;
                break;
            case R.id.id10:
                result = 1;
                break;
            case R.id.id15:
                result = 2;
                break;

        }
    }
}
