package com.allergoscope.app.card;

import android.annotation.SuppressLint;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import android.graphics.Rect;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


import lib.OrientActivity;
import lib.common.ActionCallback;
import lib.common.CEdit;
import lib.common.IResultFragmentDlg;
import lib.common.button.CMenuButton;
import lib.common.button.CTextButton;
import lib.common.grid.Cell;
import lib.common.grid.Grid;
import lib.common.layout.CPanelDialog;
import lib.utils.AGlobals;

import android.os.Bundle;
import android.view.ViewTreeObserver;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.allergoscope.app.App;
import com.allergoscope.app.MenuActivity;
import com.allergoscope.app.R;
import lib.utils.TreeDir;

import static lib.common.grid.Cell.CELL_BUTTON;
import static lib.common.grid.Cell.CELL_TEXT;
import static lib.common.grid.Cell.TEXT_BOLD;
import static lib.common.grid.Cell.TEXT_EDIT;
import static lib.common.grid.Cell.TEXT_INTEGER;
import static lib.common.grid.Cell.TEXT_READONLY;

@SuppressLint("ValidFragment")
public class FragmentCard extends Fragment implements IResultFragmentDlg {
    View self;
    Grid grid;
    String[] info = null;
    Cell[] date = new Cell[2];
    Cell[] name = new Cell[2];
    Cell[] surname = new Cell[2];
    Cell[] id = new Cell[2];
    public String[] result_info = null;
    Runnable runnable = null;
    String path = null;
    ActionCallback onexit;
    CheckVisibility checkVisibility;
    float Y;
    CPanelDialog container;
    int heightKeyboard = 0;
    TreeDir.DirNodeVisible node = null;

    public FragmentCard(ActionCallback _onexit)
    {
        super();
        onexit = _onexit;
    }

    public FragmentCard(TreeDir.DirNodeVisible _node, ActionCallback _onexit)
    {
        super();
        onexit = _onexit;
        node = _node;
    }
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup _container, Bundle savedInstanceState)
    {
        try
        {
            container = ((CPanelDialog) _container);
            container.SetOrient("Top");

            container.Start();
            container.SetCallbacks(new ActionCallback(this, "OnInitDialog"), new ActionCallback(this, "OnDestroyDialog"));
            container.fAutoClosed = false;
            self = inflater.inflate(R.layout.card_fragment1, container, false);
            if (node != null)
            {
                info = new String[4];
                String path = App.defaultPath + node.dir.GetPath();
                String s = path + "/info.ini";
                info[0] = AGlobals.ReadIniW("CARD", "NAME", s);
                info[1] = AGlobals.ReadIniW("CARD", "FAMILY", s);
                info[2] = AGlobals.ReadIniW("CARD", "DATE", s);
                info[3] = AGlobals.ReadIniW("CARD", "ID", s);

            } else
                info = App.cardPath.ReadCardInfo();
            int H = self.getMeasuredHeight();
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            self.setLayoutParams(new LinearLayout.LayoutParams(metrics.widthPixels / 2, LinearLayout.LayoutParams.MATCH_PARENT));

            self.measure(0, 0);
            InitTextGrid();

            int w = grid.getMeasuredWidth();
            int h = self.getMeasuredHeight();
            grid.measure(0, 0);
            self.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT));
            self.measure(0, 0);


            w = self.getMeasuredWidth();
            h = self.getMeasuredHeight();
            Y = (metrics.heightPixels - h) / 2.f;
            container.setTranslationY(Y);
            container.setTranslationX((metrics.widthPixels - w) / 2.f);
            CTextButton mb = self.findViewById(R.id.idOk);
            if (onexit == null)
                mb.SetCallback(this, "OnCancel");
            else
                mb.SetCallback(this, "OnOk");
            mb = self.findViewById(R.id.idCancel);
            if (AGlobals.currentActivity instanceof MenuActivity)
                mb.SetCallback(this, "OnCancel");
                else
                mb.setVisibility(View.GONE);
        } catch (CException p)
        {
            p.printStackTrace();
        }

        return self;
    }

    public void OnOk()
    {

        try
        {
            result_info = new String[4];
        grid.UpdateData();
        if (name[1].text == null || name[1].text[0] == null || name[1].text[0].trim().length() == 0)
            return;
        result_info[0] = name[1].text[0].trim();
        if (surname[1].text == null || surname[1].text[0] == null || surname[1].text[0].trim().length() == 0)
            return;
        result_info[1] = surname[1].text[0].trim();
        if (date[1].text == null || date[1].text[0] == null || date[1].text[0].trim().length() == 0 || date[1].text[0].equals("//"))
            return;
        result_info[2] = date[1].text[0].trim();
            if (id[1].text == null || id[1].text[0] == null || id[1].text[0].trim().length() == 0)
                return;

            result_info[3] = id[1].text[0].trim();
            if (node == null)
            {
                App.cardPath.CreateNew(result_info);
                if (onexit != null)
                    runnable = new ActionCallback() {

                        @Override
                        public void run()
                        {

                            try
                            {
                                onexit.run(AGlobals.currentActivity, App.cardPath.Dir());
                            } catch (CException e)
                            {
                                e.printStackTrace();
                            }
                        }
                    };
            } else
                onexit.run(result_info, node);
        } catch (CException e)
        {
            e.printStackTrace();
        }

        ((OrientActivity) AGlobals.currentActivity).DialogHide();

    }

    public void OnCancel()
    {
        result_info = null;
        ((OrientActivity) AGlobals.currentActivity).DialogHide();
    }

    void InitTextGrid()
    {
        grid = self.findViewById(R.id.idGridCard);

        DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
        grid.iLayoutParamsWidth = metrics.widthPixels / 2;
        Cell[] header = new Cell[1];

        String str = getContext().getString(R.string.card_info); // AGlobals.ReadString("IDS_PATIENT_INFO");
        header[0] = new Cell(CELL_TEXT, TEXT_READONLY | TEXT_BOLD, str);
        header[0].iAligment = Gravity.LEFT;
        String space = "  ";
        str = getContext().getString(R.string.card_first_name); // AGlobals.ReadString("IDS_NAME");
        str += "   ";
        name[0] = new Cell(CELL_TEXT, TEXT_READONLY, space + str);
        name[0].iAligment = Gravity.LEFT;

        str = getContext().getString(R.string.card_last_name);  //AGlobals.ReadString("IDS_SSURNAME");
        str += "   ";

        surname[0] = new Cell(CELL_TEXT, TEXT_READONLY, space + str);
        surname[0].iAligment = Gravity.LEFT;
        str = getContext().getString(R.string.card_birthdate); // AGlobals.ReadString("IDS_DATE");
        str += "   ";
        date[0] = new Cell(CELL_TEXT, TEXT_READONLY, space + str);
        id[0] = new Cell(CELL_TEXT, TEXT_READONLY, "  ID  ");
        date[0].iAligment = Gravity.LEFT;
        id[0].iAligment = Gravity.LEFT;

        if (AGlobals.currentActivity instanceof MenuActivity)
        {
            name[1] = new Cell(CELL_TEXT, TEXT_EDIT, info == null || info[0] == null ? "" : info[0]);
            surname[1] = new Cell(CELL_TEXT, TEXT_EDIT, info == null || info[1] == null ? "" : info[1]);
            id[1] = new Cell(CELL_TEXT, TEXT_EDIT | TEXT_INTEGER, info == null || info[3] == null ? "" : info[3]);
            date[1] = new Cell(CELL_BUTTON, 0, info == null ? "//" : info[2]);
            date[1].callback = new ActionCallback(this, "OnClickDate");

        } else
        {
            name[1] = new Cell(CELL_TEXT, TEXT_READONLY, info == null || info[0] == null ? "" : info[0]);
            surname[1] = new Cell(CELL_TEXT, TEXT_READONLY, info == null || info[1] == null ? "" : info[1]);
            date[1] = new Cell(CELL_TEXT, TEXT_READONLY, info == null || info[2] == null ? "" : info[2]);
            id[1] = new Cell(CELL_TEXT, TEXT_READONLY, info == null || info[3] == null ? "" : info[3]);

        }
        name[1].iAligment = Gravity.LEFT;
        surname[1].iAligment = Gravity.LEFT;
        date[1].iAligment = Gravity.LEFT;
        id[1].iAligment = Gravity.LEFT;

        grid.AddRow(header).fDrawBorder = false;
        grid.AddRow(id).fDrawBorder = false;
        grid.AddRow(name).fDrawBorder = false;
        grid.AddRow(surname).fDrawBorder = false;
        grid.AddRow(date).fDrawBorder = false;
        if (AGlobals.currentActivity instanceof MenuActivity)
        {
            ((CEdit) name[1].ctrl).setImeOptions(EditorInfo.IME_ACTION_NEXT | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
            ((CEdit) surname[1].ctrl).setImeOptions(EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
            ((CEdit) id[1].ctrl).setImeOptions(EditorInfo.IME_ACTION_NEXT | EditorInfo.IME_FLAG_NO_EXTRACT_UI);

            ((CEdit) name[1].ctrl).setOnEditorActionListener(new TextView.OnEditorActionListener() {
                @Override
                public boolean onEditorAction(TextView v, int actionId, KeyEvent event)
                {
                    if (actionId == EditorInfo.IME_ACTION_NEXT)
                    {
                        AGlobals.rootHandler.Send(new Runnable() {
                            @Override
                            public void run()
                            {
                                OnKeyboardShow(heightKeyboard);

                            }
                        }, 100);
                    }
                    return false;
                }
            });
            ((CEdit) id[1].ctrl).setOnEditorActionListener(new TextView.OnEditorActionListener() {
                @Override
                public boolean onEditorAction(TextView v, int actionId, KeyEvent event)
                {
                    if (actionId == EditorInfo.IME_ACTION_NEXT)
                    {
                        AGlobals.rootHandler.Send(new Runnable() {
                            @Override
                            public void run()
                            {
                                OnKeyboardShow(heightKeyboard);

                            }
                        }, 100);
                    }
                    return false;
                }
            });


        }
        grid.measure(0, 0);

    }

    public void OnClickDate()
    {
        CDateDlg dlg = new CDateDlg(info == null ? "//" : info[2]);
        dlg.DoModal();
        ((CTextButton) date[1].ctrl).setText(dlg.date);
    }

    public void OnInitDialog()
    {
        grid.UpdateView();
        InitStandartKeyboardInput();
    }

    public void OnDestroyDialog()
    {
        ((OrientActivity) AGlobals.currentActivity).CloseDialog();

    }

    //---------------------------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------------------------
    public void OnKeyboardShow(int _heightKeyboard)
    {
        if (AGlobals.currentActivity == null)
        {
            AGlobals.ShowKeyboard(false);
            return;
        }
        View v = ((AppCompatActivity) AGlobals.currentActivity).getCurrentFocus();
        heightKeyboard = _heightKeyboard;
        if (v instanceof CEdit)
        {
            CEdit cEdit = (CEdit) v;
            Rect rc = cEdit.ScreenRect();
            //View decorView = AGlobals.currentActivity.getWindow().getDecorView();
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            Rect rcFrame = new Rect(0, 0, metrics.widthPixels, metrics.heightPixels - heightKeyboard);
            float y = container.getTranslationY();
            //decorView.getWindowVisibleDisplayFrame(rcFrame);
            int shift = (rc.bottom + rc.height()/2) - rcFrame.bottom;
            if (shift >= 0)
            {
           //     grid.SetVisible(0, View.GONE);

                container.setTranslationY(y - shift);
            } else
            {
                container.setTranslationY(Y);
           //     grid.SetVisible(0, View.VISIBLE);
            }
        }
    }

    @Override
    public Runnable GetResult()
    {
        return runnable;
    }

    //---------------------------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------------------------
    public void OnKeyboardHidden()
    {
        heightKeyboard = 0;
        grid.SetVisible(0, View.VISIBLE);
        container.setTranslationY(Y);
    }

    void InitStandartKeyboardInput()
    {
        AGlobals.currentActivity.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);
        View decorView = AGlobals.currentActivity.getWindow().getDecorView();
        checkVisibility = new CheckVisibility();
        decorView.getViewTreeObserver().addOnGlobalLayoutListener(checkVisibility);

    }

    ;

    //**********************************************************************
    class CheckVisibility implements ViewTreeObserver.OnGlobalLayoutListener {
        Rect rcFrame = new Rect();
        int lastDecorHeight;

        public CheckVisibility()
        {
            View decorView = container.getRootView();
            decorView.getWindowVisibleDisplayFrame(rcFrame);
            lastDecorHeight = rcFrame.height();
        }

        @Override
        public void onGlobalLayout()
        {
            {
                View decorView = container.getRootView();
                decorView.getWindowVisibleDisplayFrame(rcFrame);
                int decoreHeight = rcFrame.height();
                if (lastDecorHeight != 0)
                {

                    if (lastDecorHeight > decoreHeight)
                    {
                        DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();

                        int currentKeyboardHeight = metrics.heightPixels - (rcFrame.height() - rcFrame.top);
                        OnKeyboardShow(currentKeyboardHeight);

                    } else if (lastDecorHeight < decoreHeight)
                    {
                        OnKeyboardHidden();
                    }
                }
                lastDecorHeight = decoreHeight;
            }
        }
    }

}
