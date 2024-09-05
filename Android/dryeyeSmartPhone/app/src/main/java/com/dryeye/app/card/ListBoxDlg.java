package com.dryeye.app.card;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;

import com.dryeye.app.App;
import com.dryeye.app.R;
import lib.utils.TreeDir;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;

import lib.common.ActionCallback;
import lib.common.CDialog;
import lib.common.CListBox;
import lib.utils.AGlobals;

import static com.dryeye.app.card.CurrentPath.LEVEL_FIO;

public class ListBoxDlg extends CDialog {
    View view;
    private TextView test;
    public String[] data;
    public String result = null;
    public ArrayList<TreeDir.DirNode> list;
    CListBox box;
    int index = -1;
    public int ResultIndex = -1;
    boolean fInserted = false;
    static public int IDCREATE = 1000;

    public ListBoxDlg(Context context)
    {
        super(context);
        view = LayoutInflater.from(getContext()).inflate(R.layout.dlg_next_step, null);
        test = view.findViewById(R.id.idText);
        box = view.findViewById(R.id.idList);
        String level = null;

        try
        {
            list = new ArrayList<TreeDir.DirNode>();
            App.cardPath.CurrentDirList(list);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
            Calendar c = Calendar.getInstance();
            String sname = sdf.format(c.getTime());


            for (int i = 0, l = list.size(); i < l; ++i)
            {
                if (App.cardPath.Level() == LEVEL_FIO)
                    if (list.get(i).name.equals(sname))
                        index = i;
            }
            fInserted = index != -1;

            data = new String[list.size()];
            for (int i = 0, l = list.size(); i < l; ++i)
                data[i] = list.get(i).name;

            if (App.cardPath.Level() == LEVEL_FIO)
            {
                String str = getContext().getString(R.string.select_date);
                test.setText(str);
                if (index == -1)
                {
                    str = getContext().getString(R.string.today);
                    level = str;
                }
            } else
            {
                String str =  getContext().getString(R.string.select_test);
                test.setText(str);
                str = getContext().getString(R.string.select_new_test);
                level = str;
            }
            box.Init(data, index, new ActionCallback(this, "OnClick", int.class));

        } catch (CException e)
        {
            e.printStackTrace();
        }

        setView(view);
        AddButton(BUTTON_NEGATIVE, getContext().getString(R.string.cancel), IDCANCEL);
        if (index == -1)
            AddButton(BUTTON_NEUTRAL, level, 1000);
        AddButton(BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);
    }

    public void OnInitDialog()
    {
        EnableButton(BUTTON_POSITIVE, index != -1);
    }

    //-------------------------------------------------
    //
    //---------------------------------------------------
    @Override
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

    public void OnClick(int i)
    {
        EnableButton(BUTTON_POSITIVE, true);
        index = i;

    }
}
