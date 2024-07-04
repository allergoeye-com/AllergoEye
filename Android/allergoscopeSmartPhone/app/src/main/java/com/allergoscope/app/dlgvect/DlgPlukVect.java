package com.allergoscope.app.dlgvect;


import android.graphics.Typeface;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.TextView;

import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import com.allergoscope.app.R;
import com.allergoscope.app.card.CException;

import java.util.ArrayList;
import java.util.StringTokenizer;

import lib.OrientActivity;
import lib.common.ActionCallback;
import lib.common.CHandler;
import lib.common.IResultFragmentDlg;
import lib.common.button.CMenuButton;
import lib.common.grid.CCell;
import lib.common.grid.Cell;
import lib.common.grid.Grid;
import lib.common.layout.CPanelDialog;
import lib.utils.AGlobals;
import lib.utils.FileUtils;
import lib.utils.TreeDir;

public class DlgPlukVect  extends Fragment implements IResultFragmentDlg {
    View self;
    Grid grid;
    Runnable runnable = null;
    ActionCallback onexit;
    CPanelDialog container;
    String [] header;
    TreeDir.DirNodeVisible node = null;
    float dp, dpButton;
    Cell [][] radio = null;
    Cell [] columns = null;
    public DlgPlukVect(ActionCallback _onexit)
    {
        super();
        onexit = _onexit;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup _container, Bundle savedInstanceState)
    {
        try
        {
            container = ((CPanelDialog) _container);
            container.SetOrient("Top");

            container.SetCallbacks(new ActionCallback(this, "OnInitDialog"), new ActionCallback(this, "OnDestroyDialog"));
            container.fAutoClosed = false;
            self = inflater.inflate(R.layout.dlg_grid, container, false);

            ArrayList<String> vec = new ArrayList<String>();
            FileUtils.ReadFromAssets("vect_de.txt", vec);
            String str = vec.get(0);
            StringTokenizer st = new StringTokenizer(str, ",");
            int N = st.countTokens();
            if (N > 0)
            {
                header = new String[N];
                radio = new Cell[N][4];

                for (int i = 0; i < N; ++i)
                {
                    header[i] = st.nextToken();
                    for (int j = 0; j < 4; ++j)
                        radio[i][j] = new Cell(Cell.CELL_RADIOBUTTON, Cell.TEXT_READONLY, "" + j, j == 0 ? 1 : 0);
                }

            }
            DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
            self.setLayoutParams(new LinearLayout.LayoutParams(metrics.widthPixels, LinearLayout.LayoutParams.WRAP_CONTENT));

            self.measure(0, 0);

            InitGrid();

            int w = grid.getMeasuredWidth();
            int h = self.getMeasuredHeight();
          //  grid.measure(0, 0);
            //self.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT));
            //self.measure(0, 0);
            self.measure(0, 0);


            w = self.getMeasuredWidth();
            h = self.getMeasuredHeight();
            float Y = (metrics.heightPixels - h) / 2.f;
            container.setTranslationY(Y);
            container.setTranslationX(0.0f);
            self.measure(0, 0);
            CMenuButton mb = self.findViewById(R.id.idOk);
            if (onexit == null)
                mb.SetCallback(this, "OnCancel");
            else
                mb.SetCallback(this, "OnOk");
            mb = self.findViewById(R.id.idCancel);
            mb.SetCallback(this, "OnCancel");
            container.Start();

        } catch (Exception p)
        {
            FileUtils.AddToLog(p);
        }

        return self;
    }
    //-----------------------------------------------------------------
    //
    //-----------------------------------------------------------------
    void InitGrid()
    {

        DisplayMetrics metrics = AGlobals.GetDisplayMetrcs();
        grid = self.findViewById(R.id.idGrid);
        dp = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 10, metrics);
        dpButton = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 24, metrics);
        columns = new Cell[header.length];

        for (int i = 0; i < header.length; ++i)
        {

            Cell [] cells = new Cell[5];
                cells[0] = new CCell(Cell.CELL_TEXT, Cell.TEXT_READONLY, " " +  header[i] + " ");
                columns[i] = cells[0];
            for (int j = 1; j < 5; ++j)
                    cells[j] = radio[i][j - 1];
            grid.AddRow(cells);
        }
        UpdateView();

    }
    //---------------------------------------------------
    //
    //---------------------------------------------------
    public void OnGridLayout(boolean changed, int left, int top, int right, int bottom)
    {
        (new CHandler()).Send(new Runnable() {
            @Override
            public void run()
            {
                grid.CalbackOnLayout = null;
                UpdateView();
            }
        });
    }
    void UpdateView()
    {
        for (int i = 0; i < header.length; ++i)
        {
            TextView textView = (TextView)columns[i].ctrl;
            textView.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL);
            textView.setTypeface(Typeface.DEFAULT_BOLD);
            textView.setTextAppearance(getContext(), android.R.style.TextAppearance_Large);
            textView.setSingleLine(true);
            textView.requestLayout();
            for (int j = 0; j < 4; ++j)
            {
                RadioButton r = (RadioButton)radio[i][j].ctrl;
                r.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL);
                r.setTextAppearance(getContext(), android.R.style.TextAppearance_Medium);

            }
        }
        grid.requestLayout();
    }

    public void OnOk()
    {

        ((OrientActivity) AGlobals.currentActivity).DialogHide();

    }

    public void OnCancel()
    {
        ((OrientActivity) AGlobals.currentActivity).DialogHide();
    }


    public void OnInitDialog()
    {
        grid.UpdateView();
    }

    public void OnDestroyDialog()
    {
        ((OrientActivity) AGlobals.currentActivity).CloseDialog();

    }

    @Override
    public Runnable GetResult()
    {
        return runnable;
    }


}
