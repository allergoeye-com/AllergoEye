package com.allergoscope.app.card;

import android.content.Context;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.TextView;

import com.allergoscope.app.MainActivity;
import com.allergoscope.app.R;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Locale;
import java.util.StringTokenizer;

import lib.common.CComboBox;
import lib.common.CDialog;
import lib.common.EditableSpinner;
import lib.utils.AGlobals;
import lib.utils.FileUtils;
import lib.utils.StringUtils;

public class DlgNewTest extends CDialog {
    View self;
    EditText test;
    public String result = null;
    static int t1 = 0;
    static int t2 = 0;
    static int t3 = 0;
    static int t4 = 0;
    boolean isDismiss = true;
    public DlgNewTest(Context context)
    {
        super(context);
        self = LayoutInflater.from(getContext()).inflate(R.layout.dlg_name_test, null);

        String str = getContext().getString(R.string.new_test); //AGlobals.ReadString("IDS_TEST");
        SetText(str);
        str = AGlobals.ReadIni("CURRENT", "RES", "lang.ini");
        String path = AGlobals.contextApp.getApplicationInfo().dataDir + "/";
        path += str + ".txt";

        setView(self);
        ReadStr(path);
/*        CComboBox spinner;
        String [] test1 = new String [] {
                "mite Farinae",
                "mite Pteronyssinus",
                "birch",
                "alder",
                "hazel",
                "ash-tree",
                "grass",
                "mugwort",
                "ragweed",
                "Altenaria Altenata",
                "Penicillium Notatum",
                "cat"
        };
        String [] test2 = new String [] {
                "start test",
                "control solution",
                "stock solution",
                "1-10",
                "1-100",
                "1-1 000",
                "1-10 000",
                "1-100 000" };
        String [] test3 = new String [] {
                "left",
                "right",
                "both sides"
        };
       String n =  AGlobals.ReadIni("TEST", "1", "test.ini");
       if (n != null)
           t1 = Integer.valueOf(n.trim()).intValue();
        n =  AGlobals.ReadIni("TEST", "2", "test.ini");
        if (n != null)
            t2 = Integer.valueOf(n.trim()).intValue();
        n =  AGlobals.ReadIni("TEST", "3", "test.ini");
        if (n != null)
            t3 = Integer.valueOf(n.trim()).intValue();

        spinner = (CComboBox)self.findViewById(R.id.test1);
        spinner.Init(test1, t1,  null);
        spinner = (CComboBox)self.findViewById(R.id.test2);

        spinner.Init(test2, t2, null);
       spinner = (CComboBox)self.findViewById(R.id.test3);
        spinner.Init(test3,  t3, null);
        */

        AddButton(BUTTON_NEGATIVE, getContext().getString(R.string.cancel), IDCANCEL);
        AddButton(BUTTON_POSITIVE, getContext().getString(R.string.ok), IDOK);
    }
    public void SetText(String str)
    {
        TextView txt = self.findViewById(R.id.textPlease);
        txt.setText(str);

    }
    public void OnInitDialog()
    {


    }

    @Override
    public void show()
    {
        if (isDismiss)
        {
            isDismiss = false;
            super.show();
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            WindowManager.LayoutParams attrs = getWindow().getAttributes();
            getWindow().setLayout((int) (metrics.widthPixels), attrs.height);
        }
    }
    protected void OnCommand(int id)
    {
        if (id == IDOK)
        {
            String test2 = "";
            ArrayList<ArrayList<String> > test = MainActivity.DefTest();
            //result = //test.getText().toString().trim();
            CComboBox spinner;
            spinner = (CComboBox) self.findViewById(R.id.test1);
            t1 = spinner.GetSel();
            test2 = test.get(0).get(t1) + "_";
            result = spinner.GetSelString() + "_";
            spinner = (CComboBox) self.findViewById(R.id.test2);
            t2 = spinner.GetSel();
            test2 +=  test.get(1).get(t2) + "_";
            result += spinner.GetSelString() + "_";
            spinner = (CComboBox) self.findViewById(R.id.test3);
            t3 = spinner.GetSel();
            test2 +=  test.get(2).get(t3) + "_";
            result += spinner.GetSelString() + "_";
            spinner = (CComboBox) self.findViewById(R.id.test4);
            t4 = spinner.GetSel();
            test2 += test.get(3).get(t4) ;
            result += spinner.GetSelString();
            result = StringUtils.ReplaceAll(result, ":", "-");
            test2 = StringUtils.ReplaceAll(test2, ":", "-");
            result = test2;

            AGlobals.SaveIni("TEST", "1", "" + t1, "test.ini");
            AGlobals.SaveIni("TEST", "2", "" + t2, "test.ini");
            AGlobals.SaveIni("TEST", "3", "" + t3, "test.ini");
            AGlobals.SaveIni("TEST", "4", "" + t4, "test.ini");

        }
        EndDialog(id);
    }

    class CSetText implements Runnable {
        EditableSpinner edit;
        String s;

        CSetText(EditableSpinner _edit, String _s)
        {
            edit = _edit;
            s = _s;
        }

        @Override
        public void run()
        {
            edit.SetText(s);
        }
    }


    public void ReadStr(String fileName)
    {
        String n = AGlobals.ReadIni("TEST", "1", "test.ini");
        if (n != null)
            t1 = Integer.valueOf(n.trim()).intValue();
        n = AGlobals.ReadIni("TEST", "2", "test.ini");
        if (n != null)
            t2 = Integer.valueOf(n.trim()).intValue();
        n = AGlobals.ReadIni("TEST", "3", "test.ini");
        if (n != null)
            t3 = Integer.valueOf(n.trim()).intValue();
        n = AGlobals.ReadIni("TEST", "4", "test.ini");
        if (n != null)
            t4 = Integer.valueOf(n.trim()).intValue();
        CComboBox spinner;
        int i = 1;
        String str = getContext().getString(R.string.test_choise);
        StringTokenizer st1 = new StringTokenizer(str, "/");
        while (st1.hasMoreTokens())
        {
            String mLine = st1.nextToken();

            ArrayList<String> data = new ArrayList<String>();
            StringTokenizer st = new StringTokenizer(mLine, ";");
            while (st.hasMoreTokens())
            {
                String s = st.nextToken();
                //  s = StringUtils.ReplaceAll(s, "-", ":");

                data.add(s);
            }
            switch (i)
            {
                case 1:
                    spinner = (CComboBox) self.findViewById(R.id.test1);
                    spinner.Init(data, t1, null);
                    break;
                case 2:
                    spinner = (CComboBox) self.findViewById(R.id.test2);
                    spinner.Init(data, t2, null);
                    break;
                case 3:
                    spinner = (CComboBox) self.findViewById(R.id.test3);
                    spinner.Init(data, t3, null);
                    break;
                case 4:
                    spinner = (CComboBox) self.findViewById(R.id.test4);
                    spinner.Init(data, t4, null);
                    break;
            }
            ++i;
        }

    }
/*

        try
        {
            File file = new File(fileName);
            FileReader fr = new FileReader(file);
            BufferedReader br = new BufferedReader(fr);
            String mLine;
            CComboBox spinner;
            for (int i = 0; br.ready() && i < 5; ++i)
                if ((mLine = br.readLine()) != null)
                {
                    ArrayList<String> data = new ArrayList<String>();
                    StringTokenizer st = new StringTokenizer(mLine, ";");
                    while (st.hasMoreTokens())
                    {
                        String s = st.nextToken();
                      //  s = StringUtils.ReplaceAll(s, "-", ":");

                        data.add(s);
                    }
                    switch (i)
                    {
                        case 1:
                            spinner = (CComboBox) self.findViewById(R.id.test1);
                            spinner.Init(data, t1, null);
                            break;
                        case 2:
                            spinner = (CComboBox) self.findViewById(R.id.test2);
                            spinner.Init(data, t2, null);
                            break;
                        case 3:
                            spinner = (CComboBox) self.findViewById(R.id.test3);
                            spinner.Init(data, t3, null);
                            break;
                        case 4:
                            spinner = (CComboBox) self.findViewById(R.id.test4);
                             spinner.Init(data, t4, null);
                            break;
                    }


                }
            br.close();
            fr.close();

        } catch (IOException e)
        {
            FileUtils.AddToLog(e);
        }

 */

}
