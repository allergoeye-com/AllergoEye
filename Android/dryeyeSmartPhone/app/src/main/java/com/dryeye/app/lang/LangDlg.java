package com.dryeye.app.lang;

import android.view.LayoutInflater;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import androidx.core.content.ContextCompat;

import com.dryeye.app.R;

import lib.common.ActionCallback;
import lib.common.CDialog;
import lib.utils.AGlobals;

public class LangDlg extends CDialog {
    View view;
    public String Result;
    public LangDlg()
    {
        super(AGlobals.currentActivity);
        view = LayoutInflater.from(getContext()).inflate(R.layout.lang_select, null);
        Result = AGlobals.ReadLang();
        if (Result.equals("DE"))
        {
            RadioButton c = view.findViewById(R.id.idDe);
            c.setChecked(true);
            c.setTextColor(ContextCompat.getColor(AGlobals.currentActivity, R.color.theme_bar_color_down));

        }
        else
        if (Result.equals("FR"))
        {
            RadioButton c = view.findViewById(R.id.idFr);
            c.setChecked(true);
            c.setTextColor(ContextCompat.getColor(AGlobals.currentActivity, R.color.theme_bar_color_down));
        }
        else
        if (Result.equals("ES"))
        {
            RadioButton c = view.findViewById(R.id.idEs);
            c.setChecked(true);
            c.setTextColor(ContextCompat.getColor(AGlobals.currentActivity, R.color.theme_bar_color_down));
        }
        else
        if (Result.equals("PL"))
        {
            RadioButton c = view.findViewById(R.id.idPol);
            c.setChecked(true);
            c.setTextColor(ContextCompat.getColor(AGlobals.currentActivity, R.color.theme_bar_color_down));


        }
        else
        {
            RadioButton c = view.findViewById(R.id.idEn);
            c.setChecked(true);
            c.setTextColor(ContextCompat.getColor(AGlobals.currentActivity, R.color.theme_bar_color_down));


        }
        RadioGroup gr = view.findViewById(R.id.idLangGroup);
        gr.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                RadioGroup gr = view.findViewById(R.id.idLangGroup);
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

        setView(view);
        AddButton(BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);

    }
    @Override
    public void OnOk()
    {
        RadioButton c = view.findViewById(R.id.idEn);
        if (c.isChecked())
            Result = "EN";
        else
        {
            c = view.findViewById(R.id.idDe);
            if (c.isChecked())
                Result = "DE";
            else
            {
                c = view.findViewById(R.id.idEs);
                if (c.isChecked())
                      Result = "ES";
                else
                {
                    c = view.findViewById(R.id.idPol);
                    if (c.isChecked())
                        Result = "PL";
                     else
                        Result = "FR";
                }
            }
        }

        super.OnOk();
    }

}
